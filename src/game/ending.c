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
#include "ending.h"
#include "../game/credits.h"
#include "../game/title.h"
#include "../world/map.h"
#include "../system/wipe.h"
#include "../world/stage.h"
#include "../system/text.h"
#include "../world/entities.h"
#include "../system/atlas.h"

extern App app;
extern Stage stage;

static void logic(void);
static void draw(void);
static void focusOnVomit(void);
static void drawDarkness(void);
static void returnFromCredits(void);

static AtlasImage *darknessTexture;
static int timeout;

void initEnding(void)
{
	stage.entityTail = &stage.entityHead;
	stage.particleTail = &stage.particleHead;
	stage.cloneDataTail = &stage.cloneDataHead;

	stage.num = 0;

	loadStage(0);

	/* prevent player control */
	stage.player->tick = NULL;

	initWipe(WIPE_FADE);

	darknessTexture = getAtlasImage("gfx/particles/darkness.png", 1);

	timeout = FPS * 5;

	app.delegate.logic = logic;
	app.delegate.draw = draw;
}

static void logic(void)
{
	/* don't let time run out */
	stage.time = FPS * 60 * 60;

	doWipe();

	doEntities();

	if (--timeout <= 0)
	{
		initCredits(returnFromCredits);
	}
}

static void draw(void)
{
	app.dev.drawing = 0;

	focusOnVomit();

	drawEntities(1);

	drawMap();

	drawDarkness();

	drawEntities(0);

	if (timeout > 0)
	{
		drawText(SCREEN_WIDTH / 2, 50, 32, TEXT_CENTER, app.colors.white, "Well, that answers the question of whether Walter was hallucinating.");
		drawText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 64, 32, TEXT_CENTER, app.colors.white, "Thanks for playing.");
	}

	drawWipe();
}

static void drawDarkness(void)
{
	SDL_Rect dest;

	dest.w = 550;
	dest.h = 350;
	dest.x = (SCREEN_WIDTH - dest.w) / 2;
	dest.y = (SCREEN_HEIGHT - dest.h) / 2;

	dest.y += 150;

	SDL_RenderCopyEx(app.renderer, darknessTexture->texture, &darknessTexture->rect, &dest, 0, NULL, SDL_FLIP_NONE);
}

static void focusOnVomit(void)
{
	Entity *e;

	for (e = stage.entityHead.next ; e != NULL ; e = e->next)
	{
		if (e->type == ET_VOMIT_TOILET)
		{
			stage.camera.x = (int) e->x + (e->w / 2);
			stage.camera.y = (int) e->y + (e->h / 2);

			stage.camera.x -= (SCREEN_WIDTH / 2);
			stage.camera.y -= (SCREEN_HEIGHT / 2);

			stage.camera.y -= 200;

			return;
		}
	}
}

static void returnFromCredits(void)
{
	destroyStage();

	initTitle();
}

