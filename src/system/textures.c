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
#include "textures.h"
#include <SDL2/SDL_image.h>

extern App app;

static SDL_Texture *getTexture(const char *name)
{
	Texture *t;

	for (t = app.texturesHead.next ; t != NULL ; t = t->next)
	{
		if (strcmp(t->name, name) == 0)
		{
			return t->texture;
		}
	}

	return NULL;
}

static void addTextureToCache(const char *name, SDL_Texture *sdlTexture)
{
	Texture *texture;

	texture = malloc(sizeof(Texture));
	memset(texture, 0, sizeof(Texture));
	app.texturesTail->next = texture;
	app.texturesTail = texture;

	STRNCPY(texture->name, name, MAX_NAME_LENGTH);
	texture->texture = sdlTexture;
}

SDL_Texture *toTexture(SDL_Surface *surface, int destroySurface)
{
	SDL_Texture *texture;

	texture = SDL_CreateTextureFromSurface(app.renderer, surface);

	if (destroySurface)
	{
		SDL_FreeSurface(surface);
	}

	return texture;
}

SDL_Texture *loadTexture(const char *filename)
{
	SDL_Texture *texture;

	texture = getTexture(filename);

	if (texture == NULL)
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s ...", filename);
		texture = IMG_LoadTexture(app.renderer, filename);

		addTextureToCache(filename, texture);
	}

	return texture;
}

void destroyTextures(void)
{
	Texture *t;

	while (app.texturesHead.next)
	{
		t = app.texturesHead.next;
		app.texturesHead.next = t->next;
		SDL_DestroyTexture(t->texture);
		free(t);
	}
}

