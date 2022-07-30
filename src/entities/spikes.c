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
#include "spikes.h"
#include "../system/atlas.h"

extern Entity *self;

static void touch(Entity *other);

void initSpikes(Entity *e)
{
	e->typeName = "spikes";
	e->type = ET_TRAP;
	e->atlasImage = getAtlasImage("gfx/entities/spikes.png", 1);
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->touch = touch;
	e->flags = EF_NO_ENT_CLIP+EF_STATIC;
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

