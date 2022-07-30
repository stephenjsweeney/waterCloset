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
#include "slimeDrip.h"
#include "../json/cJSON.h"
#include "../system/atlas.h"
#include "../world/particles.h"
#include "../system/sound.h"
#include "../world/entityFactory.h"

extern Entity *self;
extern Stage stage;

static void tick(void);
static void fireBullet(void);
static void load(cJSON *root);
static void save(cJSON *root);

void initSlimeDrip(Entity *e)
{
	Spitter *s;

	s = malloc(sizeof(Spitter));
	memset(s, 0, sizeof(Spitter));

	e->typeName = "slimeDrip";
	e->type = ET_TRAP;
	e->data = s;
	e->atlasImage = getAtlasImage("gfx/entities/drip.png", 1);
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->flags = EF_WEIGHTLESS+EF_NO_ENT_CLIP+EF_INVISIBLE+EF_STATIC;
	e->tick = tick;

	e->load = load;
	e->save = save;
}

static void tick(void)
{
	Spitter *s;

	s = (Spitter*)self->data;

	if (s->enabled && --s->reload <= 0)
	{
		fireBullet();

		s->reload = s->interval;
	}
}

static void load(cJSON *root)
{
	Spitter *s;

	s = (Spitter*)self->data;

	s->interval = cJSON_GetObjectItem(root, "interval")->valueint;
	s->enabled = cJSON_GetObjectItem(root, "enabled")->valueint;
}

static void save(cJSON *root)
{
	Spitter *s;

	s = (Spitter*)self->data;

	cJSON_AddNumberToObject(root, "interval", s->interval);
	cJSON_AddNumberToObject(root, "enabled", s->enabled);
}

/* === Bullet */

static void bulletTouch(Entity *other)
{
	if (other != NULL)
	{
		if (other->type == ET_PLAYER || other->type == ET_CLONE)
		{
			other->health = 0;

			self->health = 0;

			playPositionalSound(SND_SPIT_HIT, CH_HIT, self->x, self->y, stage.player->x, stage.player->y);
		}
		else if (other->flags & EF_SOLID)
		{
			self->health = 0;

			playPositionalSound(SND_SPIT_HIT, CH_HIT, self->x, self->y, stage.player->x, stage.player->y);
		}
	}
	else
	{
		self->health = 0;

		playPositionalSound(SND_SPIT_HIT, CH_HIT, self->x, self->y, stage.player->x, stage.player->y);
	}
}

static void bulletDie(void)
{
	addSlimeBurstParticles(self->x, self->y + self->h / 2);
}

static void bulletTick(void)
{
	if (self->health > 1)
	{
		self->dy = 0.5f;

		if (--self->health == 1)
		{
			self->touch = bulletTouch;

			self->flags &= ~EF_WEIGHTLESS;
			self->flags &= ~EF_NO_WORLD_CLIP;
			self->flags &= ~EF_NO_ENT_CLIP;

			playPositionalSound(SND_DRIP, CH_SPIT, self->x, self->y, stage.player->x, stage.player->y);
		}
	}
}

static void fireBullet(void)
{
	Entity *e;

	e = spawnEntity();

	e->type = ET_BULLET;
	e->typeName = "bullet";
	e->x = self->x;
	e->y = self->y;
	e->facing = self->facing;
	e->atlasImage = self->atlasImage;
	e->w = self->w;
	e->h = self->h;
	e->background = 1;
	e->flags = EF_WEIGHTLESS+EF_NO_WORLD_CLIP+EF_NO_ENT_CLIP;
	e->health = FPS;
	e->tick = bulletTick;
	e->die = bulletDie;

	e->y -= e->h * 2;

	e->light.g = 255;
	e->light.a = 48;
}

