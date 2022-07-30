/*
Copyright (C) 2018,2022 Parallel Realities

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "../common.h"
#include "quadtree.h"
#include "../system/util.h"

#define QT_CELL_SIZE           128
#define QT_INITIAL_CAPACITY    8

extern Stage stage;

static int getIndex(Quadtree *root, int x, int y, int w, int h);
static void removeEntity(Entity *e, Quadtree *root);
static int entityComparator(const void *a, const void *b);
static void getAllEntsWithinNode(int x, int y, int w, int h, Entity **candidates, Entity *ignore, Quadtree *root);
static void destroyQuadtreeNode(Quadtree *root);
static void resizeQTEntCapacity(Quadtree *root);

static int cIndex;
static int cCapacity;
static int totalDepth;
static int numCells;

void initQuadtree(Quadtree *root)
{
	Quadtree *node;
	int i, w, h;

	/* entire map */
	if (root->depth == 0)
	{
		root->w = MAP_WIDTH * TILE_SIZE;
		root->h = MAP_HEIGHT * TILE_SIZE;
		root->capacity = QT_INITIAL_CAPACITY;
		root->ents = malloc(sizeof(Entity*) * root->capacity);
		memset(root->ents, 0, sizeof(Entity*) * root->capacity);

		totalDepth = 0;
		numCells = 0;

		cIndex = 0;
		cCapacity = QT_INITIAL_CAPACITY;
	}

	w = root->w / 2;
	h = root->h / 2;

	if (w > QT_CELL_SIZE || h > QT_CELL_SIZE)
	{
		for (i = 0 ; i < 4 ; i++)
		{
			node = malloc(sizeof(Quadtree));
			memset(node, 0, sizeof(Quadtree));
			root->node[i] = node;

			node->depth = root->depth + 1;
			node->capacity = QT_INITIAL_CAPACITY;
			node->ents = malloc(sizeof(Entity*) * node->capacity);
			memset(node->ents, 0, sizeof(Entity*) * node->capacity);

			totalDepth = MAX(node->depth, totalDepth);
			numCells++;

			switch (i)
			{
				case 0:
					node->x = root->x;
					node->y = root->y;
					node->w = w;
					node->h = h;
					break;

				case 1:
					node->x = root->x + w;
					node->y = root->y;
					node->w = w;
					node->h = h;
					break;

				case 2:
					node->x = root->x;
					node->y = root->y + h;
					node->w = w;
					node->h = h;
					break;

				default:
					node->x = root->x + w;
					node->y = root->y + h;
					node->w = w;
					node->h = h;
					break;
			}

			initQuadtree(node);
		}
	}

	if (root->depth == 0)
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "Quadtree: [totalDepth = %d, numCells = %d, memory = %ldkb]\n", totalDepth, numCells, (long)((sizeof(Quadtree) * numCells) / 1024));
	}
}

void addToQuadtree(Entity *e, Quadtree *root)
{
	int index;

	root->addedTo = 1;

	if (root->node[0])
	{
		index = getIndex(root, e->x, e->y, e->w, e->h);

		if (index != -1)
		{
			addToQuadtree(e, root->node[index]);
			return;
		}
	}

	if (root->numEnts == root->capacity)
	{
		resizeQTEntCapacity(root);
	}

	root->ents[root->numEnts++] = e;
}

static void resizeQTEntCapacity(Quadtree *root)
{
	int n;

	n = root->capacity + QT_INITIAL_CAPACITY;

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "Resizing QT node: %d -> %d", root->capacity, n);

	root->ents = resize(root->ents, sizeof(Entity*) * root->capacity, sizeof(Entity*) * n);
	root->capacity = n;
}

static int getIndex(Quadtree *root, int x, int y, int w, int h)
{
	int index, verticalMidpoint, horizontalMidpoint, topQuadrant, bottomQuadrant;

	index = -1;

	verticalMidpoint = root->x + (root->w / 2);
	horizontalMidpoint = root->y + (root->h / 2);
	topQuadrant = (y < horizontalMidpoint && y + h < horizontalMidpoint);
	bottomQuadrant = (y > horizontalMidpoint);

	if (x < verticalMidpoint && x + w < verticalMidpoint)
	{
		if (topQuadrant)
		{
			index = 0;
		}
		else if (bottomQuadrant)
		{
			index = 2;
		}
	}
	else if (x > verticalMidpoint)
	{
		if (topQuadrant)
		{
			index = 1;
		}
		else if (bottomQuadrant)
		{
			index = 3;
		}
	}

	return index;
}

void removeFromQuadtree(Entity *e, Quadtree *root)
{
	int index;

	if (root->addedTo)
	{
		if (root->node[0])
		{
			index = getIndex(root, e->x, e->y, e->w, e->h);

			if (index != -1)
			{
				removeFromQuadtree(e, root->node[index]);
				return;
			}
		}

		removeEntity(e, root);

		if (root->numEnts == 0)
		{
			root->addedTo = 0;

			if (root->node[0])
			{
				root->addedTo = root->node[0]->addedTo || root->node[1]->addedTo || root->node[2]->addedTo || root->node[3]->addedTo;
			}
		}
	}
}

static void removeEntity(Entity *e, Quadtree *root)
{
	int i, n;

	n = root->numEnts;

	for (i = 0 ; i < root->capacity ; i++)
	{
		if (root->ents[i] == e)
		{
			root->ents[i] = NULL;
			root->numEnts--;
		}
	}

	qsort(root->ents, n, sizeof(Entity*), entityComparator);
}

Entity **getAllEntsWithin(int x, int y, int w, int h, Entity **candidates, Entity *ignore)
{
	cIndex = 0;

	memset(candidates, 0, sizeof(Entity*) * MAX_QT_CANDIDATES);

	getAllEntsWithinNode(x, y, w, h, candidates, ignore, &stage.quadtree);

	return candidates;
}

static void getAllEntsWithinNode(int x, int y, int w, int h, Entity **candidates, Entity *ignore, Quadtree *root)
{
	int index, i;

	if (root->addedTo)
	{
		if (root->node[0])
		{
			index = getIndex(root, x, y, w, h);

			if (index != -1)
			{
				getAllEntsWithinNode(x, y, w, h, candidates, ignore, root->node[index]);
			}
			else
			{
				for (i = 0 ; i < 4 ; i++)
				{
					getAllEntsWithinNode(x, y, w, h, candidates, ignore, root->node[i]);
				}
			}
		}

		for (i = 0 ; i < root->numEnts ; i++)
		{
			if (cIndex < MAX_QT_CANDIDATES)
			{
				if (root->ents[i] != ignore)
				{
					candidates[cIndex++] = root->ents[i];
				}
			}
			else
			{
				SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, "Out of quadtree candidate space (%d)", MAX_QT_CANDIDATES);
				exit(1);
			}
		}
	}
}

void destroyQuadtree(void)
{
	destroyQuadtreeNode(&stage.quadtree);
}

static void destroyQuadtreeNode(Quadtree *root)
{
	int i;

	free(root->ents);

	root->ents = NULL;

	if (root->node[0])
	{
		for (i = 0 ; i < 4 ; i++)
		{
			destroyQuadtreeNode(root->node[i]);

			free(root->node[i]);

			root->node[i] = NULL;
		}
	}
}

static int entityComparator(const void *a, const void *b)
{
	Entity *e1 = *((Entity**)a);
	Entity *e2 = *((Entity**)b);

	if (!e1)
    {
        return 1;
    }
    else if (!e2)
	{
		return -1;
	}
    else
	{
		return 0;
	}
}

