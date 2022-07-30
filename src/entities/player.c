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
#include "player.h"
#include "../json/cJSON.h"
#include "../system/atlas.h"
#include "../world/particles.h"
#include "../system/sound.h"
#include "../world/entityFactory.h"
#include "../system/controls.h"

extern Entity *self;
extern Game game;
extern Stage stage;

static void recordCloneData(void);
static void tick(void);
static void die(void);
static void load(cJSON *root);
static void save(cJSON *root);

static AtlasImage *normalTexture;
static AtlasImage *shieldTexture;
static AtlasImage *plungerTexture;
static AtlasImage *waterPistolTexture;
static AtlasImage *bulletTexture;
static float px;
static float py;

void initPlayer(Entity *e)
{
	Walter *p;

	stage.player = e;

	p = malloc(sizeof(Walter));
	memset(p, 0, sizeof(Walter));

	e->typeName = "player";
	e->data = p;
	e->type = ET_PLAYER;
	e->atlasImage = getAtlasImage("gfx/entities/guy.png", 1);
	e->flags = EF_PUSH+EF_PUSHABLE+EF_SLOW_PUSH;
	e->tick = tick;
	e->die = die;
	e->load = load;
	e->save = save;

	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;

	normalTexture = e->atlasImage;

	shieldTexture = getAtlasImage("gfx/entities/guyShield.png", 1);

	plungerTexture = getAtlasImage("gfx/entities/guyPlunger.png", 1);

	waterPistolTexture = getAtlasImage("gfx/entities/guyPistol.png", 1);

	bulletTexture = getAtlasImage("gfx/entities/waterBullet.png", 1);

	px = e->x;
	py = e->y;
}

static void tick(void)
{
	Walter *p;

	p = (Walter*)self->data;

	if (px != self->x)
	{
		game.stats[STAT_MOVED] += fabs(px - self->x);
	}

	if (abs(self->y > py))
	{
		game.stats[STAT_FALLEN] += self->y - py;
	}

	px = self->x;
	py = self->y;

	self->dx = 0;
	p->action = 0;

	switch (p->equipment)
	{
		case EQ_MANHOLE_COVER:
			self->atlasImage = shieldTexture;
			break;

		case EQ_PLUNGER:
			self->atlasImage = plungerTexture;
			break;

		case EQ_WATER_PISTOL:
			self->atlasImage = waterPistolTexture;
			break;

		default:
			self->atlasImage = normalTexture;
			break;
	}

	self->w = self->atlasImage->rect.w;
	self->h = self->atlasImage->rect.h;

	if (self->health > 0)
	{
		if (isControl(CONTROL_LEFT))
		{
			self->dx = -PLAYER_MOVE_SPEED;

			self->facing = FACING_LEFT;
		}

		if (isControl(CONTROL_RIGHT))
		{
			self->dx = PLAYER_MOVE_SPEED;

			self->facing = FACING_RIGHT;
		}

		if (isControl(CONTROL_JUMP) && self->isOnGround && p->equipment != EQ_MANHOLE_COVER)
		{
			self->riding = NULL;

			self->dy = -20;

			playSound(SND_JUMP, CH_PLAYER);

			game.stats[STAT_JUMPS]++;
		}

		if (isControl(CONTROL_USE))
		{
			clearControl(CONTROL_USE);

			p->action = 1;

			if (p->equipment == EQ_WATER_PISTOL)
			{
				game.stats[STAT_SHOTS_FIRED]++;

				fireWaterPistol();

				playPositionalSound(SND_SQUIRT, CH_SHOOT, self->x, self->y, stage.player->x, stage.player->y);
			}
		}

		if (self->dx != 0 || self->dy < 0 || p->action)
		{
			recordCloneData();
		}
	}
}

static void recordCloneData(void)
{
	CloneData *c;
	Walter *p;

	p = (Walter*)self->data;

	c = malloc(sizeof(CloneData));
	memset(c, 0, sizeof(CloneData));
	stage.cloneDataTail->next = c;
	stage.cloneDataTail = c;

	c->frame = stage.frame;
	c->dx = self->dx;
	c->dy = self->dy;
	c->action = p->action;
}

static void die(void)
{
	addDeathParticles(self->x, self->y);

	playSound(SND_DEATH, CH_PLAYER);

	if (stage.clones == stage.cloneLimit)
	{
		stage.status = SS_FAILED;

		playSound(SND_FAIL, CH_CLOCK);

		game.stats[STAT_FAILS]++;
	}

	game.stats[STAT_DEATHS]++;
}

static void load(cJSON *root)
{
	self->facing = strcmp(cJSON_GetObjectItem(root, "facing")->valuestring, "left") == 0 ? 0 : 1;
}

static void save(cJSON *root)
{
	cJSON_AddStringToObject(root, "facing", self->facing == 0 ? "left" : "right");
}

/* === Water pistol bullets === */

static void bulletTouch(Entity *other)
{
	if (other != NULL)
	{
		if (other->type == ET_BULLET)
		{
			other->health = self->health = 0;

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
	addWaterBurstParticles(self->x, self->y);
}

void fireWaterPistol(void)
{
	Entity *e;

	e = spawnEntity();

	e->type = ET_BULLET;
	e->typeName = "bullet";
	e->x = self->x;
	e->y = self->y;
	e->facing = self->facing;
	e->dx = self->facing ? 12 : -12;
	e->flags = EF_WEIGHTLESS+EF_NO_MAP_BOUNDS;
	e->atlasImage = bulletTexture;
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->touch = bulletTouch;
	e->die = bulletDie;

	e->y += (e->h / 2);

	if (e->facing)
	{
		e->x += self->w;
	}
}

