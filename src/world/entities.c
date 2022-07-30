/*
Copyright (C) 2019,2022 Parallel Realities

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
#include "entities.h"
#include "../json/cJSON.h"
#include "../world/quadtree.h"
#include "../system/draw.h"
#include "../system/util.h"
#include "../world/map.h"
#include "../system/atlas.h"
#include "../world/entityFactory.h"

extern App app;
extern Entity *self;
extern Stage stage;

static void move(Entity *e);
static int push(Entity *e, float dx, float dy);
static void moveToWorld(Entity *e, float dx, float dy);
static void moveToEntities(Entity *e, float dx, float dy);
static void loadEnts(cJSON *root);
static int canPush(Entity *e, Entity *other);
static void drawEntityLight(Entity *e);

static Entity deadListHead, *deadListTail;
static AtlasImage *sparkleTexture;

void initEntities(cJSON *root)
{
	memset(&deadListHead, 0, sizeof(Entity));
	deadListTail = &deadListHead;

	loadEnts(cJSON_GetObjectItem(root, "entities"));

	sparkleTexture = getAtlasImage("gfx/particles/light.png", 1);
}

void doEntities(void)
{
	Entity *e, *prev;

	prev = &stage.entityHead;

	app.dev.collisions = app.dev.ents = 0;

	for (e = stage.entityHead.next ; e != NULL ; e = e->next)
	{
		removeFromQuadtree(e, &stage.quadtree);

		app.dev.ents++;

		self = e;

		if (e->tick)
		{
			e->tick();
		}

		if (!(e->flags & EF_STATIC))
		{
			move(e);
		}

		if (e->health > 0)
		{
			addToQuadtree(e, &stage.quadtree);
		}
		else
		{
			if (e->die)
			{
				e->die();
			}

			if (e == stage.entityTail)
			{
				stage.entityTail = prev;
			}

			prev->next = e->next;

			/* add to dead list */
			deadListTail->next = e;
			deadListTail = e;
			deadListTail->next = NULL;

			e = prev;
		}

		prev = e;
	}

	for (e = stage.entityHead.next ; e != NULL ; e = e->next)
	{
		removeFromQuadtree(e, &stage.quadtree);

		if (e->riding != NULL)
		{
			push(e, e->riding->dx, 0);
		}

		if (!(e->flags & (EF_NO_WORLD_CLIP|EF_NO_MAP_BOUNDS)))
		{
			e->x = MIN(MAX(e->x, stage.camera.minX), stage.camera.maxX - (e->w + 16));
			e->y = MIN(MAX(e->y, 0), MAP_HEIGHT * TILE_SIZE);
		}

		addToQuadtree(e, &stage.quadtree);
	}
}

static void move(Entity *e)
{
	if (!(e->flags & EF_WEIGHTLESS))
	{
		e->dy += 1.5;
		e->dy = MAX(MIN(e->dy, 18), -999);
	}

	if (e->riding != NULL && e->riding->dy > 0)
	{
		e->dy = e->riding->dy + 1;
	}

	e->riding = NULL;

	e->isOnGround = 0;

	push(e, e->dx, 0);

	push(e, 0, e->dy);
}

static int push(Entity *e, float dx, float dy)
{
	float ex, ey;

	ex = e->x + dx;
	ey = e->y + dy;

	e->x += dx;
	e->y += dy;

	moveToEntities(e, dx, dy);

	if (!(e->flags & EF_NO_WORLD_CLIP))
	{
		moveToWorld(e, dx, dy);
	}

	return e->x == ex && e->y == ey;
}

static void moveToWorld(Entity *e, float dx, float dy)
{
	int mx, my, hit, adj;

	hit = 0;

	if (dx != 0)
	{
		mx = dx > 0 ? (e->x + e->w) : e->x;
		mx /= TILE_SIZE;

		my = (e->y / TILE_SIZE);

		hit = 0;

		if (!isInsideMap(mx, my) || stage.map[mx][my] != 0)
		{
			hit = 1;
		}

		my = (e->y + e->h - 1) / TILE_SIZE;

		if (!isInsideMap(mx, my) || stage.map[mx][my] != 0)
		{
			hit = 1;
		}

		if (hit)
		{
			adj = dx > 0 ? -e->w : TILE_SIZE;

			e->x = (mx * TILE_SIZE) + adj;

			e->dx = 0;
		}
	}

	if (dy != 0)
	{
		my = dy > 0 ? (e->y + e->h) : e->y;
		my /= TILE_SIZE;

		mx = e->x / TILE_SIZE;

		hit = 0;

		if (!isInsideMap(mx, my) || stage.map[mx][my] != 0)
		{
			hit = 1;
		}

		mx = (e->x + e->w - 1) / TILE_SIZE;

		if (!isInsideMap(mx, my) || stage.map[mx][my] != 0)
		{
			hit = 1;
		}

		if (hit)
		{
			adj = dy > 0 ? -e->h : TILE_SIZE;

			e->y = (my * TILE_SIZE) + adj;

			e->dy = 0;

			e->isOnGround = dy > 0;
		}
	}

	if (hit && e->touch)
	{
		e->touch(NULL);
	}
}

static void moveToEntities(Entity *e, float dx, float dy)
{
	Entity *other, *oldSelf, *candidates[MAX_QT_CANDIDATES];
	int adj, i;
	float pushPower;

	getAllEntsWithin(e->x, e->y, e->w, e->h, candidates, e);

	for (i = 0, other = candidates[0] ; i < MAX_QT_CANDIDATES && other != NULL ; other = candidates[++i])
	{
		app.dev.collisions++;

		if (collision(e->x, e->y, e->w, e->h, other->x, other->y, other->w, other->h))
		{
			if (!(e->flags & EF_NO_ENT_CLIP) && !(other->flags & EF_NO_ENT_CLIP))
			{
				if (canPush(e, other))
				{
					removeFromQuadtree(other, &stage.quadtree);

					pushPower = e->flags & EF_SLOW_PUSH ? 0.5f : 1.0f;

					oldSelf = self;

					self = other;

					if (dx != 0)
					{
						if (!push(other, e->dx * pushPower, 0))
						{
							e->x = other->x;

							if (e->dx > 0)
							{
								e->x -= e->w;
							}
							else
							{
								e->x += other->w;
							}
						}
					}

					if (dy != 0)
					{
						if (!push(other, 0, e->dy * pushPower))
						{
							e->y = other->y;

							if (e->dy > 0)
							{
								e->y -= e->h;
							}
							else
							{
								e->y += other->h;
							}
						}
					}

					self = oldSelf;

					addToQuadtree(other, &stage.quadtree);
				}

				if (other->flags & EF_SOLID)
				{
					if (dy != 0)
					{
						adj = dy > 0 ? -e->h : other->h;

						e->y = other->y + adj;

						e->dy = 0;

						if (dy > 0)
						{
							e->isOnGround = 1;

							if (!(e->flags & EF_WEIGHTLESS))
							{
								e->riding = other;
							}
						}
					}

					if (dx != 0)
					{
						adj = dx > 0 ? -e->w : other->w;

						e->x = other->x + adj;

						e->dx = 0;
					}
				}
			}

			if (e->touch)
			{
				e->touch(other);
			}

			if (other->flags & EF_STATIC && other->touch)
			{
				oldSelf = self;

				self = other;

				other->touch(e);

				self = oldSelf;
			}
		}
	}
}

static int canPush(Entity *e, Entity *other)
{
	if (e->flags & EF_SOLID || other->flags & EF_SOLID)
	{
		if (e->flags & EF_PUSH && other->flags & EF_PUSHABLE)
		{
			return 1;
		}
	}

	return 0;
}

void dropToFloor(void)
{
	Entity *e;
	int onGround;

	onGround = 0;

	for (e = stage.entityHead.next ; e != NULL ; e = e->next)
	{
		addToQuadtree(e, &stage.quadtree);
	}

	while (!onGround)
	{
		onGround = 1;

		for (e = stage.entityHead.next ; e != NULL ; e = e->next)
		{
			self = e;

			if ((!(e->flags & EF_WEIGHTLESS)) && !e->isOnGround)
			{
				removeFromQuadtree(e, &stage.quadtree);

				push(e, 0, 8);

				addToQuadtree(e, &stage.quadtree);

				onGround = 0;
			}
		}
	}
}

void drawEntities(int background)
{
	Entity *e, *candidates[MAX_QT_CANDIDATES];
	int i;

	getAllEntsWithin(stage.camera.x, stage.camera.y, SCREEN_WIDTH, SCREEN_HEIGHT, candidates, NULL);

	for (i = 0, e = candidates[0] ; i < MAX_QT_CANDIDATES && e != NULL ; e = candidates[++i])
	{
		if (e->background == background && !(e->flags & EF_INVISIBLE))
		{
			app.dev.drawing++;

			if (e->light.a > 0 && !e->light.foreground)
			{
				drawEntityLight(e);
			}

			blitAtlasImage(e->atlasImage, e->x - stage.camera.x, e->y - stage.camera.y, 0, e->facing == FACING_LEFT ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);

			if (e->light.a > 0 && e->light.foreground)
			{
				drawEntityLight(e);
			}
		}
	}
}

static void drawEntityLight(Entity *e)
{
	int x, y;

	if (e->light.a > 0)
	{
		x =  e->x + (e->w / 2) + e->light.x - stage.camera.x;
		y =  e->y + (e->h / 2) + e->light.y - stage.camera.y;

		SDL_SetTextureColorMod(sparkleTexture->texture, e->light.r, e->light.g, e->light.b);
		SDL_SetTextureAlphaMod(sparkleTexture->texture, e->light.a);

		blitAtlasImage(sparkleTexture, x, y, 1, SDL_FLIP_NONE);

		SDL_SetTextureColorMod(sparkleTexture->texture, 255, 255, 255);
		SDL_SetTextureAlphaMod(sparkleTexture->texture, 255);
	}
}

void activeEntities(char *targetName, int active)
{
	Entity *e, *oldSelf;

	oldSelf = self;

	for (e = stage.entityHead.next ; e != NULL ; e = e->next)
	{
		if (e->activate && strcmp(e->name, targetName) == 0)
		{
			self = e;

			e->activate(active);
		}
	}

	self = oldSelf;
}

void resetEntities(void)
{
	Entity *e, *prev;

	/* append deadlist to main list before reset */
	if (deadListHead.next)
	{
		stage.entityTail->next = deadListHead.next;
		stage.entityTail = deadListTail;
		deadListHead.next = NULL;
	}

	prev = &stage.entityHead;

	for (e = stage.entityHead.next ; e != NULL ; e = e->next)
	{
		if (e->type != ET_CLONE)
		{
			if (e->health > 0)
			{
				removeFromQuadtree(e, &stage.quadtree);
			}

			if (e == stage.entityTail)
			{
				stage.entityTail = prev;
			}

			prev->next = e->next;
			free(e->data);
			free(e);
			e = prev;
		}

		prev = e;
	}
}

void resetClones(void)
{
	Entity *e;
	Walter *c;

	for (e = stage.entityHead.next ; e != NULL ; e = e->next)
	{
		if (e->type == ET_CLONE)
		{
			removeFromQuadtree(e, &stage.quadtree);

			e->x = stage.player->x;
			e->y = stage.player->y;
			e->health = 1;

			c = (Walter*)e->data;
			c->equipment = EQ_NONE;
			c->pData = NULL;
			c->advanceData = 1;

			addToQuadtree(e, &stage.quadtree);
		}
	}
}

void destroyEntities(void)
{
	Entity *e;
	Walter *c;
	CloneData *cd;

	while (stage.entityHead.next)
	{
		e = stage.entityHead.next;
		stage.entityHead.next = e->next;

		if (e->type == ET_CLONE)
		{
			c = (Walter*)e->data;

			while (c->dataHead)
			{
				cd = c->dataHead;
				c->dataHead = cd->next;
				free(cd);
			}
		}

		free(e->data);
		free(e);
	}

	while (deadListHead.next)
	{
		e = deadListHead.next;
		deadListHead.next = e->next;
		free(e->data);
		free(e);
	}
}

static void loadEnts(cJSON *root)
{
	cJSON *node;

	for (node = root->child ; node != NULL ; node = node->next)
	{
		initEntity(node);
	}
}

