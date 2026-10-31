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
#include "platform.h"
#include "../json/cJSON.h"
#include "../system/atlas.h"
#include "../system/util.h"

extern Entity *self;

static void tick(void);
static void activate(int active);
static void load(cJSON *root);
static void save(cJSON *root);

void initPlatform(Entity *e)
{
	Platform *p;

	p = malloc(sizeof(Platform));
	memset(p, 0, sizeof(Platform));

	/* defaults */
	p->sx = e->x;
	p->sy = e->y;
	p->ex = e->x;
	p->ey = e->y - 48;
	p->pause = FPS;
	p->speed = 2;

	e->typeName = "platform";
	e->type = ET_STRUCTURE;
	e->data = p;
	e->tick = tick;
	e->activate = activate;
	e->atlasImage = getAtlasImage("gfx/entities/platform.png", 1);
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->flags = EF_SOLID+EF_WEIGHTLESS+EF_PUSH;

	e->load = load;
	e->save = save;
}

static void tick(void)
{
	Platform *p;

	p = (Platform*)self->data;

	self->dx = p->dx;
	self->dy = p->dy;

	if (!p->enabled)
	{
		self->dx = self->dy = 0;
	}

	if (p->enabled)
	{
		if (fabs(self->x - p->sx) < p->speed && fabs(self->y - p->sy) < p->speed)
		{
			p->dx = p->dy = self->dx = self->dy = 0;

			self->flags |= EF_STATIC;

			if (--p->pauseTimer <= 0)
			{
				calcSlope(p->ex, p->ey, self->x, self->y, &self->dx, &self->dy);

				self->dx *= p->speed;
				self->dy *= p->speed;

				p->dx = self->dx;
				p->dy = self->dy;

				p->pauseTimer = p->pause;

				self->flags &= ~EF_STATIC;
			}
		}

		if (fabs(self->x - p->ex) < p->speed && fabs(self->y - p->ey) < p->speed)
		{
			p->dx = p->dy = self->dx = self->dy = 0;

			self->flags |= EF_STATIC;

			if (--p->pauseTimer <= 0)
			{
				calcSlope(p->sx, p->sy, self->x, self->y, &self->dx, &self->dy);

				self->dx *= p->speed;
				self->dy *= p->speed;

				p->dx = self->dx;
				p->dy = self->dy;

				p->pauseTimer = p->pause;

				self->flags &= ~EF_STATIC;
			}
		}
	}
}

static void activate(int active)
{
	Platform *p;

	p = (Platform*)self->data;

	p->enabled = !p->enabled;
}

static void load(cJSON *root)
{
	Platform *p;

	p = (Platform*)self->data;

	p->sx = cJSON_GetObjectItem(root, "sx")->valueint;
	p->sy = cJSON_GetObjectItem(root, "sy")->valueint;
	p->ex = cJSON_GetObjectItem(root, "ex")->valueint;
	p->ey = cJSON_GetObjectItem(root, "ey")->valueint;
	p->pause = cJSON_GetObjectItem(root, "pause")->valueint;
	p->speed = cJSON_GetObjectItem(root, "speed")->valueint;
	p->enabled = cJSON_GetObjectItem(root, "enabled")->valueint;

	self->x = p->sx;
	self->y = p->sy;

	p->pauseTimer = p->pause;
}

static void save(cJSON *root)
{
	Platform *p;

	p = (Platform*)self->data;

	cJSON_AddNumberToObject(root, "sx", p->sx);
	cJSON_AddNumberToObject(root, "sy", p->sy);
	cJSON_AddNumberToObject(root, "ex", p->ex);
	cJSON_AddNumberToObject(root, "ey", p->ey);
	cJSON_AddNumberToObject(root, "pause", p->pause);
	cJSON_AddNumberToObject(root, "speed", p->speed);
	cJSON_AddNumberToObject(root, "enabled", p->enabled);
}

