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
#include "vomitToilet.h"
#include "../json/cJSON.h"
#include "../system/atlas.h"

extern Entity *self;

static void tick(void);

static AtlasImage *vomitFrames[2];

void initVomitToilet(Entity *e)
{
	Toilet *t;

	t = malloc(sizeof(Toilet));
	memset(t, 0, sizeof(Toilet));

	vomitFrames[0] = getAtlasImage("gfx/entities/vomitToilet1.png", 1);
	vomitFrames[1] = getAtlasImage("gfx/entities/vomitToilet2.png", 1);

	e->typeName = "vomitToilet";
	e->facing = 1;
	e->type = ET_VOMIT_TOILET;
	e->data = t;
	e->atlasImage = vomitFrames[0];
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->flags = EF_NO_ENT_CLIP+EF_STATIC;
	e->tick = tick;
}

static void tick(void)
{
	Toilet *t;

	t = (Toilet*)self->data;

	if (--t->animTimer <= 0)
	{
		if (++t->frameNum >= 2)
		{
			t->frameNum = 0;
		}

		t->animTimer = FPS / 2;
	}

	self->atlasImage = vomitFrames[t->frameNum];
}

