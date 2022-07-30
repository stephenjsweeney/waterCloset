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
#include "door.h"
#include "../json/cJSON.h"
#include "../system/atlas.h"
#include "../system/sound.h"

extern Entity *self;
extern Stage stage;

static void tick(void);
static void activate(int active);
static void touch(Entity *other);
static void load(cJSON *root);
static void save(cJSON *root);

void initDoor(Entity *e)
{
	Door *d;

	d = malloc(sizeof(Door));
	memset(d, 0, sizeof(Door));

	d->sx = e->x;
	d->sy = e->y;
	d->ex = e->x;
	d->ey = e->y;

	e->typeName = "door";
	e->type = ET_STRUCTURE;
	e->data = d;
	e->tick = tick;
	e->activate = activate;
	e->touch = touch;
	e->atlasImage = getAtlasImage("gfx/entities/door.png", 1);
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->flags = EF_SOLID+EF_WEIGHTLESS+EF_PUSH+EF_NO_WORLD_CLIP;
	e->background = 1;

	/* when opened */
	d->ey = e->y - (e->h - 4);

	e->load = load;
	e->save = save;
}

static void tick(void)
{
	Door *d;

	d = (Door*)self->data;

	self->dy = 0;
	self->flags |= EF_STATIC;

	if (d->open)
	{
		if (self->y > d->ey)
		{
			self->dy = -4;

			self->y = MAX(self->y, d->ey);

			self->flags &= ~EF_STATIC;
		}
	}
	else
	{
		if (self->y < d->sy)
		{
			self->dy = 4;

			self->y = MIN(self->y, d->sy);

			self->flags &= ~EF_STATIC;
		}
	}
}

static void activate(int active)
{
	Door *d;

	d = (Door*)self->data;

	d->open = !d->open;

	playPositionalSound(SND_DOOR, CH_STRUCTURE, self->x, self->y, stage.player->x, stage.player->y);
}

static void touch(Entity *other)
{
	Door *d;

	if (other != NULL && (other->type == ET_PLAYER || other->type == ET_CLONE) && stage.keys > 0)
	{
		d = (Door*)self->data;

		if (!d->open)
		{
			stage.keys--;
			d->open = 1;
			self->flags |= EF_NO_WORLD_CLIP;

			playPositionalSound(SND_DOOR, CH_STRUCTURE, self->x, self->y, stage.player->x, stage.player->y);
		}
	}
}

static void load(cJSON *root)
{
	Door *d;

	d = (Door*)self->data;

	d->open = cJSON_GetObjectItem(root, "open")->valueint;
}

static void save(cJSON *root)
{
	Door *d;

	d = (Door*)self->data;

	cJSON_AddNumberToObject(root, "open", d->open);
}

