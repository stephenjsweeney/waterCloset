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

static AtlasImage *shieldTexture;

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
	
	shieldTexture = getAtlasImage("gfx/entities/guyShield.png", 1);
}

static void tick(void)
{
	Player *p;
	
	p = (Player*)self->data;
	
	self->dx = 0;
	p->action = 0;
	
	if (self->flags & EF_SHIELDED)
	{
		self->atlasImage = shieldTexture;
	}
	
	if (self->health > 0)
	{
		if (app.keyboard[SDL_SCANCODE_A])
		{
			self->dx = -PLAYER_MOVE_SPEED;
			
			self->facing = FACING_LEFT;
		}
		
		if (app.keyboard[SDL_SCANCODE_D])
		{
			self->dx = PLAYER_MOVE_SPEED;
			
			self->facing = FACING_RIGHT;
		}
		
		if (app.keyboard[SDL_SCANCODE_I] && self->isOnGround && (!(self->flags & EF_SHIELDED)))
		{
			self->riding = NULL;
			
			self->dy = -20;
			
			playSound(SND_JUMP, CH_PLAYER);
		}
		
		if (app.keyboard[SDL_SCANCODE_J])
		{
			app.keyboard[SDL_SCANCODE_J] = 0;
			
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
	cJSON_AddStringToObject(root, "type", self->typeName);
	cJSON_AddStringToObject(root, "facing", self->facing == 0 ? "left" : "right");
}
