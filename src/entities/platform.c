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

#include "platform.h"

static void tick(void);
static void load(cJSON *root);
static void save(cJSON *root);

void initPlatform(Entity *e)
{
	Platform *p;
	
	p = malloc(sizeof(Platform));
	memset(p, 0, sizeof(Platform));
	
	e->type = ET_STRUCTURE;
	e->data = p;
	e->tick = tick;
	e->texture = loadTexture("gfx/platform.png");
	SDL_QueryTexture(e->texture, NULL, NULL, &e->w, &e->h);
	e->flags = EF_SOLID+EF_WEIGHTLESS+EF_PUSH;
	
	e->load = load;
	e->save = save;
}

static void tick(void)
{
	Platform *p;
	
	p = (Platform*)self->data;
	
	if (abs(self->x - p->sx) < PLATFORM_SPEED && abs(self->y - p->sy) < PLATFORM_SPEED)
	{
		calcSlope(p->ex, p->ey, self->x, self->y, &self->dx, &self->dy);
		
		self->dx *= PLATFORM_SPEED;
		self->dy *= PLATFORM_SPEED;
	}
	
	if (abs(self->x - p->ex) < PLATFORM_SPEED && abs(self->y - p->ey) < PLATFORM_SPEED)
	{
		calcSlope(p->sx, p->sy, self->x, self->y, &self->dx, &self->dy);
		
		self->dx *= PLATFORM_SPEED;
		self->dy *= PLATFORM_SPEED;
	}
}

static void load(cJSON *root)
{
	Platform *p;
	
	p = (Platform*)self->data;
	
	p->sx = cJSON_GetObjectItem(root, "sx")->valueint;
	p->sy = cJSON_GetObjectItem(root, "sy")->valueint;
	p->ex = cJSON_GetObjectItem(root, "ex")->valueint;
	p->ey = cJSON_GetObjectItem(root, "ey")->valueint;
	
	self->x = p->sx;
	self->y = p->sy;
}

static void save(cJSON *root)
{
	cJSON_AddStringToObject(root, "type", "platform");
}
