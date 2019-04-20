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

#include "player.h"

static AtlasImage *normalTexture;
static AtlasImage *shieldTexture;
static AtlasImage *plungerTexture;

static void recordCloneData(void);
static void tick(void);
static void die(void);
static void load(cJSON *root);
static void save(cJSON *root);

void initPlayer(Entity *e)
{
	Player *p;
	
	stage.player = e;
	
	p = malloc(sizeof(Player));
	memset(p, 0, sizeof(Player));
	
	e->typeName = "player";
	e->data = p;
	e->type = ET_PLAYER;
	e->atlasImage = getAtlasImage("gfx/entities/guy.png", 1);
	e->tick = tick;
	e->die = die;
	
	e->load = load;
	e->save = save;
	
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	
	normalTexture = e->atlasImage;
	
	shieldTexture = getAtlasImage("gfx/entities/guyShield.png", 1);
	
	plungerTexture = getAtlasImage("gfx/entities/guyPlunger.png", 1);
}

static void tick(void)
{
	Player *p;
	
	p = (Player*)self->data;
	
	self->dx = 0;
	p->action = 0;
	
	self->atlasImage = normalTexture;
	
	if (self->flags & EF_SHIELDED)
	{
		self->atlasImage = shieldTexture;
	}
	else if (self->flags & EF_PLUNGING)
	{
		self->atlasImage = plungerTexture;
	}
	
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
		
		if (isControl(CONTROL_JUMP) && self->isOnGround && (!(self->flags & EF_SHIELDED)))
		{
			self->riding = NULL;
			
			self->dy = -20;
			
			playSound(SND_JUMP, CH_PLAYER);
		}
		
		if (isControl(CONTROL_USE))
		{
			clearControl(CONTROL_USE);
			
			p->action = 1;
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
	Player *p;
	
	p = (Player*)self->data;
	
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
	}
}

static void load(cJSON *root)
{
	self->facing = strcmp(cJSON_GetObjectItem(root, "facing")->valuestring, "left") == 0 ? 0 : 1;
}

static void save(cJSON *root)
{
	cJSON_AddStringToObject(root, "facing", self->facing == 0 ? "left" : "right");
}
