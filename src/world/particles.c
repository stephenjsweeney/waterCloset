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
#include "particles.h"
#include "../system/atlas.h"
#include "../system/draw.h"

extern Stage stage;

static Particle *spawnParticle(void);

static AtlasImage *basicTexture;

void initParticles(void)
{
	basicTexture = getAtlasImage("gfx/particles/basic.png", 1);
}

void doParticles(void)
{
	Particle *p, *prev;

	prev = &stage.particleHead;

	for (p = stage.particleHead.next ; p != NULL ; p = p->next)
	{
		p->x += p->dx;
		p->y += p->dy;

		if (!p->weightless)
		{
			p->dy += 0.25;
		}

		if (--p->life <= 0)
		{
			if (p == stage.particleTail)
			{
				stage.particleTail = prev;
			}

			prev->next = p->next;
			free(p);
			p = prev;
		}

		prev = p;
	}
}

void drawParticles(void)
{
	Particle *p;

	for (p = stage.particleHead.next ; p != NULL ; p = p->next)
	{
		SDL_SetTextureColorMod(p->atlasImage->texture, p->color.r, p->color.g, p->color.b);

		blitAtlasImage(p->atlasImage, p->x - stage.camera.x, p->y - stage.camera.y, 1, SDL_FLIP_NONE);
	}

	/* restore colour */
	SDL_SetTextureColorMod(basicTexture->texture, 255, 255, 255);
}

void addCoinParticles(int x, int y)
{
	Particle *p;
	int i;

	for (i = 0 ; i < 12 ; i++)
	{
		p = spawnParticle();

		p->x = x;
		p->y = y;

		p->dx = 100 - (rand() % 200);
		p->dx /= 100;

		p->dy = 100 - (rand() % 200);
		p->dy /= 100;

		p->atlasImage = basicTexture;

		p->life = 15 + rand() % 45;
		p->weightless = 1;

		p->color.r = 255;
		p->color.g = 255;
		p->color.b = rand() % 255;
	}
}

void addPowerupParticles(int x, int y)
{
	Particle *p;
	int i;

	for (i = 0 ; i < 25 ; i++)
	{
		p = spawnParticle();

		p->x = x;
		p->y = y;

		p->dx = 200 - (rand() % 400);
		p->dx /= 100;

		p->dy = 200 - (rand() % 400);
		p->dy /= 100;

		p->atlasImage = basicTexture;

		p->life = 15 + rand() % 15;
		p->weightless = 1;

		p->color.r = 64 + rand() % 64;
		p->color.g = 128 + rand() % 128;
		p->color.b = 255;
	}
}

void addToiletSplashParticles(int x, int y)
{
	Particle *p;
	int i;

	for (i = 0 ; i < 20 ; i++)
	{
		p = spawnParticle();

		p->x = x;
		p->y = y;

		p->dx = 150 - (rand() % 300);
		p->dx /= 100;

		p->dy = -(200 + rand() % 400);
		p->dy /= 100;

		p->atlasImage = basicTexture;

		p->life = 15 + rand() % 30;

		p->color.b = 255;
		p->color.r = p->color.g = 128 + rand() % 128;
	}
}

void addDeathParticles(int x, int y)
{
	Particle *p;
	int i;

	for (i = 0 ; i < 100 ; i++)
	{
		p = spawnParticle();

		p->x = x;
		p->y = y;

		p->dx = 200 - (rand() % 400);
		p->dx /= 100;

		p->dy = -(200 + rand() % 600);
		p->dy /= 100;

		p->atlasImage = basicTexture;

		p->life = 15 + rand() % 45;

		p->color.r = 255;
		p->color.g = p->color.b = 128 + rand() % 128;
	}
}

void addWaterBurstParticles(int x, int y)
{
	Particle *p;
	int i;

	for (i = 0 ; i < 12 ; i++)
	{
		p = spawnParticle();

		p->x = x;
		p->y = y;

		p->dx = 200 - (rand() % 400);
		p->dx /= 100;

		p->dy = 200 - (rand() % 400);
		p->dy /= 100;

		p->atlasImage = basicTexture;

		p->life = 15 + rand() % 15;

		p->color.b = 255;
		p->color.r = p->color.g = 128 + rand() % 128;
	}
}

void addSlimeBurstParticles(int x, int y)
{
	Particle *p;
	int i;

	for (i = 0 ; i < 12 ; i++)
	{
		p = spawnParticle();

		p->x = x;
		p->y = y;

		p->dx = 200 - (rand() % 400);
		p->dx /= 100;

		p->dy = 200 - (rand() % 400);
		p->dy /= 100;

		p->atlasImage = basicTexture;

		p->life = 15 + rand() % 15;

		p->color.g = 255;
		p->color.r = p->color.b = rand() % 255;
	}
}

static Particle *spawnParticle(void)
{
	Particle *p;

	p = malloc(sizeof(Particle));
	memset(p, 0, sizeof(Particle));
	stage.particleTail->next = p;
	stage.particleTail = p;

	return p;
}

void destroyParticles(void)
{
	Particle *p;

	while (stage.particleHead.next)
	{
		p = stage.particleHead.next;
		stage.particleHead.next = p->next;
		free(p);
	}
}

