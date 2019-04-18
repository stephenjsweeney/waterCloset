/*
Copyright (C) 2019 Parallel Realities

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

#include "coin.h"

static void tick(void);
static void touch(Entity *other);
static void die(void);
static void save(cJSON *root);

void initCoin(Entity *e)
{
	Coin *c;
	
	c = malloc(sizeof(Coin));
	memset(c, 0, sizeof(Coin));
	
	e->typeName = "coin";
	e->type = ET_ITEM;
	e->data = c;
	e->atlasImage = getAtlasImage("gfx/entities/coin.png", 1);
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->flags = EF_WEIGHTLESS;
	e->tick = tick;
	e->touch = touch;
	e->die = die;
	
	e->save = save;
	
	stage.totalCoins++;
}

static void tick(void)
{
	Coin *c;
	
	c = (Coin*)self->data;
	
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
	}
}

static void die(void)
{
	addCoinParticles(self->x + self->w / 2, self->y + self->h / 2);
}

static void save(cJSON *root)
{
	cJSON_AddStringToObject(root, "type", self->typeName);
}
