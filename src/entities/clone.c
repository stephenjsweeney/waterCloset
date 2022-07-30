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
#include "clone.h"
#include "../system/atlas.h"
#include "../world/particles.h"
#include "../entities/player.h"
#include "../system/sound.h"
#include "../world/entityFactory.h"

extern Entity *self;
extern Game game;
extern Stage stage;

static AtlasImage *normalTexture;
static AtlasImage *shieldTexture;
static AtlasImage *plungerTexture;
static AtlasImage *waterPistolTexture;

static void tick(void);
static void die(void);

void initClone(void)
{
	Entity *e;
	Walter *c;

	c = malloc(sizeof(Walter));
	memset(c, 0, sizeof(Walter));

	c->dataHead = stage.cloneDataHead.next;

	stage.cloneDataHead.next = NULL;

	e = spawnEntity();

	e->typeName = "clone";
	e->type = ET_CLONE;
	e->atlasImage = getAtlasImage("gfx/entities/clone.png", 1);
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->flags = EF_PUSH+EF_PUSHABLE+EF_SLOW_PUSH;
	e->data = c;
	e->tick = tick;
	e->die = die;

	normalTexture = e->atlasImage;

	shieldTexture = getAtlasImage("gfx/entities/cloneShield.png", 1);

	plungerTexture = getAtlasImage("gfx/entities/clonePlunger.png", 1);

	waterPistolTexture = getAtlasImage("gfx/entities/clonePistol.png", 1);

	game.stats[STAT_CLONES]++;
}

static void tick(void)
{
	Walter *c;

	c = (Walter*)self->data;

	self->dx = 0;

	switch (c->equipment)
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

	if (c->advanceData)
	{
		c->advanceData = 0;

		if (c->pData == NULL)
		{
			c->pData = c->dataHead;
		}
		else
		{
			c->pData = c->pData->next;
		}
	}

	if (isValidCloneFrame(c))
	{
		memcpy(&c->data, c->pData, sizeof(CloneData));

		self->dx = c->data.dx;
		self->dy = c->data.dy;

		if (c->data.dx < 0)
		{
			self->facing = FACING_LEFT;
		}

		if (c->data.dx > 0)
		{
			self->facing = FACING_RIGHT;
		}

		if (c->data.dy < 0 && self->isOnGround)
		{
			self->riding = NULL;

			playPositionalSound(SND_JUMP, CH_CLONE, self->x, self->y, stage.player->x, stage.player->y);
		}

		c->action = c->data.action;

		if (c->action)
		{
			if (c->equipment == EQ_WATER_PISTOL)
			{
				/* done in player.c */
				fireWaterPistol();

				playPositionalSound(SND_SQUIRT, CH_SHOOT, self->x, self->y, stage.player->x, stage.player->y);
			}
		}

		c->advanceData = 1;
	}
}

int isValidCloneFrame(Walter *c)
{
	return c->pData != NULL && c->pData->frame == stage.frame;
}

static void die(void)
{
	addDeathParticles(self->x, self->y);

	playPositionalSound(SND_DEATH, CH_CLONE, self->x, self->y, stage.player->x, stage.player->y);

	game.stats[STAT_CLONE_DEATHS]++;
}

