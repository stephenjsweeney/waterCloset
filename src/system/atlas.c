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
#include "atlas.h"
#include "../json/cJSON.h"
#include "../system/util.h"
#include "../system/textures.h"
#include "../system/io.h"

static void loadAtlasData(void);

static AtlasImage atlases[NUM_ATLAS_BUCKETS];
static SDL_Texture *atlasTexture;

void initAtlas(void)
{
	memset(&atlases, 0, sizeof(AtlasImage) * NUM_ATLAS_BUCKETS);

	loadAtlasData();
}

AtlasImage *getAtlasImage(char *filename, int required)
{
	AtlasImage *a;
	unsigned long i;

	if (filename == NULL)
	{
		return NULL;
	}

	i = hashcode(filename) % NUM_ATLAS_BUCKETS;

	for (a = atlases[i].next ; a != NULL ; a = a->next)
	{
		if (strcmp(a->filename, filename) == 0)
		{
			return a;
		}
	}

	if (required)
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, "No such atlas image '%s'", filename);
		exit(1);
	}

	return NULL;
}

static void loadAtlasData(void)
{
	AtlasImage *atlas;
	cJSON *root, *node;
	char *text;
	unsigned long i;

	atlasTexture = loadTexture(getFileLocation("gfx/atlas/atlas.png"));

	text = readFile(getFileLocation("data/atlas/atlas.json"));

	if (!text)
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, "Failed to read atlas.json");
		exit(1);
	}

	root = cJSON_Parse(text);

	if (!root)
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, "Failed to parse atlas.json");
		free(text);
		exit(1);
	}

	for (node = root->child ; node != NULL ; node = node->next)
	{
		atlas = malloc(sizeof(AtlasImage));
		if (!atlas)
		{
			continue;
		}
		memset(atlas, 0, sizeof(AtlasImage));

		STRNCPY(atlas->filename, getJSONStringVal(node, "filename", ""), MAX_DESCRIPTION_LENGTH);
		atlas->rect.x = getJSONIntVal(node, "x", 0);
		atlas->rect.y = getJSONIntVal(node, "y", 0);
		atlas->rect.w = getJSONIntVal(node, "w", 0);
		atlas->rect.h = getJSONIntVal(node, "h", 0);
		atlas->texture = atlasTexture;

		i = hashcode(atlas->filename) % NUM_ATLAS_BUCKETS;

		/* O(1) prepend into bucket instead of O(N) traversal */
		atlas->next = atlases[i].next;
		atlases[i].next = atlas;
	}

	cJSON_Delete(root);

	free(text);
}


void destroyAtlas(void)
{
	AtlasImage *a, *next;
	int i;

	for (i = 0 ; i < NUM_ATLAS_BUCKETS ; i++)
	{
		a = atlases[i].next;

		while (a != NULL)
		{
			next = a->next;
			free(a);
			a = next;
		}

		atlases[i].next = NULL;
	}

	atlasTexture = NULL;
}
