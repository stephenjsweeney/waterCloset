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
#include "spitter.h"
#include "../json/cJSON.h"
#include "../system/atlas.h"
#include "../world/particles.h"
#include "../system/sound.h"
#include "../world/entityFactory.h"

extern Entity *self;
extern Stage stage;

static void tick(void);
static void activate(int active);
static void fireBullet(void);
static void load(cJSON *root);
static void save(cJSON *root);

static AtlasImage *bulletTexture;

void initSpitter(Entity *e)
{
	Spitter *s;

	s = malloc(sizeof(Spitter));
	memset(s, 0, sizeof(Spitter));

	e->typeName = "spitter";
	e->type = ET_TRAP;
	e->data = s;
	e->atlasImage = getAtlasImage("gfx/entities/spitter.png", 1);
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->flags = EF_WEIGHTLESS+EF_NO_ENT_CLIP+EF_STATIC;
	e->tick = tick;
	e->activate = activate;

	bulletTexture = getAtlasImage("gfx/entities/spitterBullet.png", 1);

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

		playPositionalSound(SND_SPIT, CH_SPIT, self->x, self->y, stage.player->x, stage.player->y);

		s->reload = s->interval;
	}
}

static void activate(int active)
{
	Spitter *s;

	s = (Spitter*)self->data;

	s->enabled = !s->enabled;
}

static void load(cJSON *root)
{
	Spitter *s;

	s = (Spitter*)self->data;

	self->facing = strcmp(cJSON_GetObjectItem(root, "facing")->valuestring, "left") == 0 ? 0 : 1;
	s->interval = cJSON_GetObjectItem(root, "interval")->valueint;
	s->enabled = cJSON_GetObjectItem(root, "enabled")->valueint;
}

static void save(cJSON *root)
{
	Spitter *s;

	s = (Spitter*)self->data;

	cJSON_AddStringToObject(root, "facing", self->facing == 0 ? "left" : "right");
	cJSON_AddNumberToObject(root, "interval", s->interval);
	cJSON_AddNumberToObject(root, "enabled", s->enabled);
}

/* === Bullet */

static void bulletTouch(Entity *other)
{
	Walter *w;

	if (other != NULL)
	{
		if (other->type == ET_PLAYER || other->type == ET_CLONE)
		{
			w = (Walter*)other->data;

			/* hit in the back */
			if (self->facing == other->facing)
			{
				other->health = 0;
			}
			else if (w->equipment != EQ_MANHOLE_COVER)
			{
				other->health = 0;
			}

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
	addSlimeBurstParticles(self->x, self->y);
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
	e->dx = self->facing ? 8 : -8;
	e->flags = EF_WEIGHTLESS+EF_NO_MAP_BOUNDS;
	e->atlasImage = bulletTexture;
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->touch = bulletTouch;
	e->die = bulletDie;

	/* center horizontally */
	e->y += (self->w / 2) - (e->h / 2);

	if (e->facing)
	{
		e->x += self->w;
	}

	e->light.g = 255;
	e->light.a = 48;
}

