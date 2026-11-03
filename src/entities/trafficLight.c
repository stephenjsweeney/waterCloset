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
#include "trafficLight.h"
#include "../json/cJSON.h"
#include "../world/entities.h"
#include "../system/atlas.h"
#include "../entities/clone.h"
#include "../system/sound.h"
#include "../system/util.h"

extern Entity *self;
extern Stage stage;

static void tick(void);
static void toggle(void);
static void touch(Entity *other);
static void load(cJSON *root);
static void save(cJSON *root);

static AtlasImage *goTexture;
static AtlasImage *stopTexture;

void initTrafficLight(Entity *e)
{
	TrafficLight *t;

	goTexture = getAtlasImage("gfx/entities/trafficLightGo.png", 1);
	stopTexture = getAtlasImage("gfx/entities/trafficLightStop.png", 1);

	t = malloc(sizeof(TrafficLight));
	if (!t)
	{
		return;
	}
	memset(t, 0, sizeof(TrafficLight));

	e->typeName = "trafficLight";
	e->type = ET_SWITCH;
	e->data = t;
	e->atlasImage = stopTexture;
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->tick = tick;
	e->touch = touch;
	e->flags = EF_NO_ENT_CLIP+EF_STATIC;

	e->load = load;
	e->save = save;

	e->light.a = 48;
	e->light.foreground = 1;
}

static void tick(void)
{
	TrafficLight *t;

	t = (TrafficLight*)self->data;

	self->light.r = self->light.g = self->light.b = 0;

	if (t->on)
	{
		self->light.g = 255;
		self->light.y = 0;
	}
	else
	{
		self->light.r = 255;
		self->light.y = -16;
	}
}

static void touch(Entity *other)
{
	Walter *w;

	if (other != NULL)
	{
		if (other->type == ET_PLAYER || other->type == ET_CLONE)
		{
			w = (Walter*)other->data;

			if (w->action && (other->type == ET_PLAYER || (other->type == ET_CLONE && isValidCloneFrame(w))))
			{
				w->action = 0;

				toggle();
			}
		}
	}
}

static void toggle(void)
{
	TrafficLight *t;

	t = (TrafficLight*)self->data;

	t->on = !t->on;

	activeEntities(t->targetName, t->on);

	if (t->on)
	{
		self->atlasImage = goTexture;
	}
	else
	{
		self->atlasImage = stopTexture;
	}

	playPositionalSound(SND_TRAFFIC_LIGHT, CH_SWITCH, self->x, self->y, stage.player ? stage.player->x : self->x, stage.player ? stage.player->y : self->y);
}

static void load(cJSON *root)
{
	TrafficLight *t;

	t = (TrafficLight*)self->data;

	t->on = getJSONIntVal(root, "on", 0);
	STRNCPY(t->targetName, getJSONStringVal(root, "targetName", ""), MAX_NAME_LENGTH);

	if (t->on)
	{
		self->atlasImage = goTexture;
		self->light.g = 255;
		self->light.y = 0;
	}
	else
	{
		self->light.r = 255;
		self->light.y = -16;
		self->atlasImage = stopTexture;
	}
}

static void save(cJSON *root)
{
	TrafficLight *t;

	t = (TrafficLight*)self->data;

	cJSON_AddStringToObject(root, "targetName", t->targetName);
	cJSON_AddNumberToObject(root, "on", t->on);
}

