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
#include "decoration.h"
#include "../json/cJSON.h"
#include "../system/atlas.h"

extern Entity *self;

static void load(cJSON *root);
static void save(cJSON *root);

void initDecoration(Entity *e)
{
	Decoration *d;

	d = malloc(sizeof(Decoration));
	memset(d, 0, sizeof(Decoration));

	STRNCPY(d->textureFilename, "gfx/decoration/cabinet.png", MAX_NAME_LENGTH);

	e->typeName = "decoration";
	e->type = ET_DECORATION;
	e->data = d;
	e->facing = 1;
	e->atlasImage = getAtlasImage(d->textureFilename, 1);
	e->w = e->atlasImage->rect.w;
	e->h = e->atlasImage->rect.h;
	e->flags = EF_WEIGHTLESS+EF_NO_ENT_CLIP+EF_STATIC;

	e->load = load;
	e->save = save;
}

static void load(cJSON *root)
{
	Decoration *d;

	d = (Decoration*)self->data;

	STRNCPY(d->textureFilename, cJSON_GetObjectItem(root, "textureFilename")->valuestring, MAX_NAME_LENGTH);

	self->atlasImage = getAtlasImage(d->textureFilename, 1);
	self->w = self->atlasImage->rect.w;
	self->h = self->atlasImage->rect.h;
}

static void save(cJSON *root)
{
	Decoration *d;

	d = (Decoration*)self->data;

	cJSON_AddStringToObject(root, "textureFilename", d->textureFilename);
}

