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
	AtlasImage *atlas, *a;
	cJSON *root, *node;
	char *text;
	unsigned long i;

	atlasTexture = loadTexture(getFileLocation("gfx/atlas/atlas.png"));

	text = readFile(getFileLocation("data/atlas/atlas.json"));

	root = cJSON_Parse(text);

	for (node = root->child ; node != NULL ; node = node->next)
	{
		atlas = malloc(sizeof(AtlasImage));
		memset(atlas, 0, sizeof(AtlasImage));

		STRNCPY(atlas->filename, cJSON_GetObjectItem(node, "filename")->valuestring, MAX_DESCRIPTION_LENGTH);
		atlas->rect.x = cJSON_GetObjectItem(node, "x")->valueint;
		atlas->rect.y = cJSON_GetObjectItem(node, "y")->valueint;
		atlas->rect.w = cJSON_GetObjectItem(node, "w")->valueint;
		atlas->rect.h = cJSON_GetObjectItem(node, "h")->valueint;
		atlas->texture = atlasTexture;

		i = hashcode(atlas->filename) % NUM_ATLAS_BUCKETS;

		a = &atlases[i];

		/* horrible bit to look for the tail */
		while (a->next)
		{
			a = a->next;
		}

		a->next = atlas;
	}

	cJSON_Delete(root);

	free(text);
}

