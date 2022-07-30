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
#include "waterButton.h"
#include "../json/cJSON.h"
#include "../world/entities.h"
#include "../system/atlas.h"
#include "../system/sound.h"

#define WATER_LEVEL_MAX    6

extern Entity *self;
extern Stage stage;

static void tick(void);
static void load(cJSON *root);
static void save(cJSON *root);
static void touch(Entity *other);

static AtlasImage *textures[WATER_LEVEL_MAX];

void initWaterButton(Entity *e)
{
	WaterButton *w;
	int i;
	char filename[MAX_NAME_LENGTH];

	for (i = 0 ; i < WATER_LEVEL_MAX ; i++)
	{
		sprintf(filename, "gfx/entities/waterButton%d.png", i + 1);

		textures[i] = getAtlasImage(filename, 1);
	}

	w = malloc(sizeof(WaterButton));
	memset(w, 0, sizeof(WaterButton));

	e->typeName = "waterButton";
	e->type = ET_STRUCTURE;
	e->data = w;
	e->tick = tick;
	e->touch = touch;
	e->atlasImage = textures[0];
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->flags = EF_SOLID+EF_WEIGHTLESS+EF_STATIC;

	e->load = load;
	e->save = save;
}

static void tick(void)
{
	WaterButton *w;
	int oldValue;

	w = (WaterButton*)self->data;

	self->atlasImage = textures[w->waterLevel];

	if (w->emptyRate > 0 && --w->emptyTimer <= 0)
	{
		oldValue = w->waterLevel;

		w->waterLevel = MAX(w->waterLevel - 1, 0);

		w->emptyTimer = w->emptyRate;

		if (w->inflated && oldValue != w->waterLevel)
		{
			playPositionalSound(SND_DEFLATE, CH_SWITCH, self->x, self->y, stage.player->x, stage.player->y);

			if (w->waterLevel == 0)
			{
				w->inflated = 0;

				activeEntities(w->targetName, 0);
			}
		}
	}
}

static void touch(Entity *other)
{
	WaterButton *w;
	int oldValue;

	if (other != NULL && other->type == ET_BULLET)
	{
		other->health = 0;

		w = (WaterButton*)self->data;

		oldValue = w->waterLevel;

		w->emptyTimer = w->emptyRate;

		w->waterLevel = MIN(w->waterLevel + 1, WATER_LEVEL_MAX - 1);

		if (!w->inflated && oldValue != w->waterLevel)
		{
			playPositionalSound(SND_INFLATE, CH_SWITCH, self->x, self->y, stage.player->x, stage.player->y);

			if (w->waterLevel == WATER_LEVEL_MAX - 1)
			{
				w->inflated = 1;

				activeEntities(w->targetName, 1);
			}
		}
	}
}

static void load(cJSON *root)
{
	WaterButton *w;

	w = (WaterButton*)self->data;

	w->emptyRate = cJSON_GetObjectItem(root, "emptyRate")->valueint;
	self->facing = strcmp(cJSON_GetObjectItem(root, "facing")->valuestring, "left") == 0 ? 0 : 1;
	STRNCPY(w->targetName, cJSON_GetObjectItem(root, "targetName")->valuestring, MAX_NAME_LENGTH);
}

static void save(cJSON *root)
{
	WaterButton *w;

	w = (WaterButton*)self->data;

	cJSON_AddNumberToObject(root, "emptyRate", w->emptyRate);
	cJSON_AddStringToObject(root, "facing", self->facing == 0 ? "left" : "right");
	cJSON_AddStringToObject(root, "targetName", w->targetName);
}

