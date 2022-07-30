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
#include "coin.h"
#include "../system/atlas.h"
#include "../world/particles.h"
#include "../system/sound.h"

extern Entity *self;
extern Game game;
extern Stage stage;

static void tick(void);
static void touch(Entity *other);
static void die(void);

void initCoin(Entity *e)
{
	Collectable *c;

	c = malloc(sizeof(Collectable));
	memset(c, 0, sizeof(Collectable));

	c->bobValue = rand() % 10;

	e->typeName = "coin";
	e->type = ET_ITEM;
	e->data = c;
	e->atlasImage = getAtlasImage("gfx/entities/coin.png", 1);
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->flags = EF_WEIGHTLESS+EF_NO_ENT_CLIP+EF_STATIC;
	e->tick = tick;
	e->touch = touch;
	e->die = die;

	e->light.r = 255;
	e->light.g = 255;
	e->light.a = 64;

	stage.totalCoins++;
}

static void tick(void)
{
	Collectable *c;

	c = (Collectable*)self->data;

	c->bobValue += 0.1;

	self->y += sin(c->bobValue) * 0.25;
}

static void touch(Entity *other)
{
	if (self->health > 0 && other != NULL && (other->type == ET_PLAYER || other->type == ET_CLONE))
	{
		self->health = 0;

		playPositionalSound(SND_COIN, CH_COIN, self->x, self->y, stage.player->x, stage.player->y);

		stage.coins++;

		if (stage.items == stage.totalItems && stage.coins == stage.totalCoins)
		{
			playPositionalSound(SND_FANFARE, CH_COIN, self->x, self->y, stage.player->x, stage.player->y);
		}

		game.stats[STAT_COINS]++;
	}
}

static void die(void)
{
	addCoinParticles(self->x + self->w / 2, self->y + self->h / 2);
}

