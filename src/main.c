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

#include "common.h"
#include "main.h"
#include "system/draw.h"
#include "system/input.h"
#include "game/title.h"
#include "game/game.h"
#include "system/sound.h"
#include "system/init.h"
#include "world/stage.h"
#include "game/ending.h"

App app;
Entity *player;
Entity *self;
Game game;
Stage stage;

static void handleCommandLine(int argc, char *argv[]);
static void capFrameRate(long *then, float *remainder);

int main(int argc, char *argv[])
{
	long then, nextSecond;
	float remainder;
	int frames;

	memset(&app, 0, sizeof(App));
	app.texturesTail = &app.texturesHead;

	initSDL();

	atexit(cleanup);

	initGame();

	handleCommandLine(argc, argv);

	then = SDL_GetTicks();

	frames = remainder = 0;

	nextSecond = SDL_GetTicks() + 1000;

	while (1)
	{
		prepareScene();

		doInput();

		app.delegate.logic();

		app.delegate.draw();

		presentScene();

		frames++;

		capFrameRate(&then, &remainder);

		if (SDL_GetTicks() > nextSecond)
		{
			game.stats[STAT_TIME]++;

			app.dev.fps = frames;

			nextSecond = SDL_GetTicks() + 1000;

			frames = 0;
		}
	}

	return 0;
}

static void handleCommandLine(int argc, char *argv[])
{
	int i;

	for (i = 1 ; i < argc ; i++)
	{
		if (strcmp(argv[i], "-stage") == 0)
		{
			initStage();

			stage.num = atoi(argv[i + 1]);

			loadStage(1);

			loadRandomStageMusic(1);
		}
		else if (strcmp(argv[i], "-ending") == 0)
		{
			memset(&stage, 0, sizeof(Stage));
			stage.entityTail = &stage.entityHead;

			initEnding();

			stage.num = -1;
		}

		if (strcmp(argv[i], "-debug") == 0)
		{
			app.dev.debug = 1;

			SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG);
		}
	}

	if (stage.num == 0)
	{
		loadGame();

		initTitle();
	}
}

static void capFrameRate(long *then, float *remainder)
{
	long wait, frameTime;

	wait = 16 + *remainder;

	*remainder -= (int)*remainder;

	frameTime = SDL_GetTicks() - *then;

	wait -= frameTime;

	if (wait < 1)
	{
		wait = 1;
	}

	SDL_Delay(wait);

	*remainder += 0.667;

	*then = SDL_GetTicks();
}

