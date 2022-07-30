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
#include "pressurePlate.h"
#include "../json/cJSON.h"
#include "../world/entities.h"
#include "../system/atlas.h"
#include "../system/sound.h"

extern Entity *self;
extern Stage stage;

static void tick(void);
static void load(cJSON *root);
static void save(cJSON *root);
static void touch(Entity *other);

static AtlasImage *idleTexture;
static AtlasImage *activeTexture;

void initPressurePlate(Entity *e)
{
	PressurePlate *p;

	idleTexture = getAtlasImage("gfx/entities/pressurePlateIdle.png", 1);
	activeTexture = getAtlasImage("gfx/entities/pressurePlateActive.png", 1);

	p = malloc(sizeof(PressurePlate));
	memset(p, 0, sizeof(PressurePlate));

	e->typeName = "pressurePlate";
	e->type = ET_STRUCTURE;
	e->data = p;
	e->tick = tick;
	e->touch = touch;
	e->atlasImage = idleTexture;
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->flags = EF_SOLID+EF_WEIGHTLESS+EF_STATIC;

	e->load = load;
	e->save = save;

	e->light.r = 128;
	e->light.g = 192;
	e->light.b = 255;
	e->light.foreground = 1;
}

static void tick(void)
{
	PressurePlate *p;
	int prevWeight;

	p = (PressurePlate*)self->data;

	prevWeight = p->weight;

	p->weight = MAX(p->weight - 1, 0);

	if (p->weight == 0)
	{
		if (prevWeight > 0)
		{
			activeEntities(p->targetName, 0);
		}

		self->atlasImage = idleTexture;

		self->light.a = 0;
	}
	else
	{
		self->atlasImage = activeTexture;

		self->light.a = 192;
	}
}

static void touch(Entity *other)
{
	PressurePlate *p;

	if (other != NULL && other->type != ET_BULLET)
	{
		p = (PressurePlate*)self->data;

		if (p->weight == 0)
		{
			activeEntities(p->targetName, 1);

			playPositionalSound(SND_PRESSURE_PLATE, CH_SWITCH, self->x, self->y, stage.player->x, stage.player->y);
		}

		p->weight = 2;
	}
}

static void load(cJSON *root)
{
	PressurePlate *p;

	p = (PressurePlate*)self->data;

	STRNCPY(p->targetName, cJSON_GetObjectItem(root, "targetName")->valuestring, MAX_NAME_LENGTH);
}

static void save(cJSON *root)
{
	PressurePlate *p;

	p = (PressurePlate*)self->data;

	cJSON_AddStringToObject(root, "targetName", p->targetName);
}

