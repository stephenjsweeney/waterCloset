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

#include "spikes.h"

static void touch(Entity *other);
static void save(cJSON *root);

void initSpikes(Entity *e)
{
	e->type = ET_TRAP;
	e->texture = loadTexture("gfx/entities/spikes.png");
	SDL_QueryTexture(e->texture, NULL, NULL, &e->w, &e->h);
	e->touch = touch;
	
	e->save = save;
}

static void touch(Entity *other)
{
	/* must hit to base of the spikes - looks better */
	if (other != NULL && (other->type == ET_PLAYER || other->type == ET_CLONE))
	{
		if (other->y + other->h >= self->y + self->h)
		{
			other->health = 0;
		}
	}
}

static void save(cJSON *root)
{
	cJSON_AddStringToObject(root, "type", "spikes");
}
