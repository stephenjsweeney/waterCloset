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

#include "spitter.h"

static void tick(void);
static void activate(int active);
static void fireBullet(void);
static void load(cJSON *root);
static void save(cJSON *root);

static SDL_Texture *bulletTexture;

void initSpitter(Entity *e)
{
	Spitter *s;
	
	s = malloc(sizeof(Spitter));
	memset(s, 0, sizeof(Spitter));
	
	e->type = ET_TRAP;
	e->data = s;
	e->texture = loadTexture("gfx/entities/spitter.png");
	SDL_QueryTexture(e->texture, NULL, NULL, &e->w, &e->h);
	e->flags = EF_WEIGHTLESS;
	e->tick = tick;
	e->activate = activate;
	
	bulletTexture = loadTexture("gfx/entities/spitterBullet.png");
	
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
		
		playSound(SND_SPIT, CH_SPIT);
		
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
	
	cJSON_AddStringToObject(root, "type", "spitter");
	cJSON_AddStringToObject(root, "facing", self->facing == 0 ? "left" : "right");
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
			/* hit in the back */
			if (self->facing == other->facing)
			{
				other->health = 0;
			}
			else if (!(other->flags & EF_SHIELDED))
			{
				other->health = 0;
			}
			
			self->health = 0;
			
			playSound(SND_SPIT_HIT, CH_HIT);
		}
		else if (other->flags & EF_SOLID)
		{
			self->health = 0;
			
			playSound(SND_SPIT_HIT, CH_HIT);
		}
	}
	else
	{
		self->health = 0;
		
		playSound(SND_SPIT_HIT, CH_HIT);
	}
}

static void bulletDie(void)
{
	addBulletBurstParticles(self->x, self->y);
}

static void fireBullet(void)
{
	Entity *e;
	
	e = spawnEntity();
	
	e->type = ET_BULLET;
	e->x = self->x;
	e->y = self->y;
	e->facing = self->facing;
	e->dx = self->facing ? 8 : -8;
	e->flags = EF_WEIGHTLESS;
	e->texture = bulletTexture;
	SDL_QueryTexture(e->texture, NULL, NULL, &e->w, &e->h);
	e->touch = bulletTouch;
	e->die = bulletDie;
	
	/* center horizontally */
	e->y += (self->w / 2) - (e->h / 2);
	
	if (e->facing)
	{
		e->x += self->w;
	}
}
