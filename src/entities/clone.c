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

#include "clone.h"

static AtlasImage *normalTexture;
static AtlasImage *shieldTexture;
static AtlasImage *plungerTexture;

int isValidCloneFrame(Clone *c);
static void tick(void);
static void die(void);

void initClone(void)
{
	Entity *e;
	Clone *c;
	
	c = malloc(sizeof(Clone));
	memset(c, 0, sizeof(Clone));
	
	c->dataHead = stage.cloneDataHead.next;
	
	e = spawnEntity();
	
	e->typeName = "clone";
	e->type = ET_CLONE;
	e->atlasImage = getAtlasImage("gfx/entities/clone.png", 1);
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->data = c;
	e->tick = tick;
	e->die = die;
	
	normalTexture = e->atlasImage;
	
	shieldTexture = getAtlasImage("gfx/entities/cloneShield.png", 1);
	
	plungerTexture = getAtlasImage("gfx/entities/clonePlunger.png", 1);
}

static void tick(void)
{
	Clone *c;
	
	c = (Clone*)self->data;
	
	self->dx = 0;
	
	self->atlasImage = normalTexture;
	
	if (self->flags & EF_SHIELDED)
	{
		self->atlasImage = shieldTexture;
	}
	else if (self->flags & EF_PLUNGING)
	{
		self->atlasImage = plungerTexture;
	}
	
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
		
		c->advanceData = 1;
	}
}

int isValidCloneFrame(Clone *c)
{
	return c->pData != NULL && c->pData->frame == stage.frame;
}

static void die(void)
{
	addDeathParticles(self->x, self->y);
	
	playPositionalSound(SND_DEATH, CH_CLONE, self->x, self->y, stage.player->x, stage.player->y);
}
