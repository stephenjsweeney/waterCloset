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
#include "init.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>
#include "../system/lookup.h"
#include "../system/draw.h"
#include "../system/widgets.h"
#include "../game/game.h"
#include "../system/sound.h"
#include "../game/meta.h"
#include "../world/particles.h"
#include "../system/textures.h"
#include "../system/text.h"
#include "../system/atlas.h"
#include "../plat/win32/win32Init.h"
#include "../world/entityFactory.h"

extern App app;

static void showLoadingStep(float step, float maxSteps);

void initSDL(void)
{
	int rendererFlags, windowFlags;

	rendererFlags = SDL_RENDERER_ACCELERATED;

	windowFlags = 0;

	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_JOYSTICK) < 0)
	{
		printf("Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
    {
		printf("Couldn't initialize SDL Mixer\n");
		exit(1);
	}

	createSaveFolder();

	loadConfig();

	if (app.config.fullscreen)
	{
		windowFlags |= SDL_WINDOW_FULLSCREEN;
	}

	Mix_Volume(-1, app.config.soundVolume * 1.28);
	Mix_VolumeMusic(app.config.musicVolume * 1.28);

	Mix_AllocateChannels(CH_MAX);

	app.window = SDL_CreateWindow("Water Closet", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, app.config.winWidth, app.config.winHeight, windowFlags);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

	app.renderer = SDL_CreateRenderer(app.window, -1, rendererFlags);

	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

	if (TTF_Init() < 0)
	{
		printf("Couldn't initialize SDL TTF: %s\n", SDL_GetError());
		exit(1);
	}

	SDL_ShowCursor(0);
}

static void initJoypad(void)
{
	int i, n;

	n = SDL_NumJoysticks();

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "%d joysticks available", n);

	for (i = 0 ; i < n ; i++)
	{
		app.joypad = SDL_JoystickOpen(i);

		if (app.joypad)
		{
			SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Joystick [name='%s', Axes=%d, Buttons=%d]", SDL_JoystickNameForIndex(i), SDL_JoystickNumAxes(app.joypad), SDL_JoystickNumButtons(app.joypad));
			return;
		}
	}
}

void initGame(void)
{
	int i, numInitFuns;
	void (*initFuncs[]) (void) = {
		initLookups,
		initAtlas,
		initFonts,
		initSounds,
		initJoypad,
		initWidgets,
		initEntityFactory,
		initParticles,
		initStageMetaData
	};

	srand(time(NULL));

	numInitFuns = sizeof(initFuncs) / sizeof(void*);

	initGraphics();

	for (i = 0 ; i < numInitFuns ; i++)
	{
		showLoadingStep(i + 1, numInitFuns);

		initFuncs[i]();
	}
}

static void showLoadingStep(float step, float maxSteps)
{
	SDL_Rect r;
	float percent;

	prepareScene();

	r.w = 600;
	r.h = 6;
	r.x = (SCREEN_WIDTH - r.w) / 2;
	r.y = (SCREEN_HEIGHT - r.h) / 2;

	percent = step;
	percent /= maxSteps;

	drawRect(r.x, r.y, r.w, r.h, 64, 96, 128, 255);
	drawRect(r.x, r.y, r.w * percent, r.h, 128, 192, 255, 255);

	presentScene();

	SDL_Delay(1);
}

void cleanup(void)
{
	if (app.joypad != NULL)
	{
		SDL_JoystickClose(app.joypad);
	}

	destroyTextures();

	destroySounds();

	SDL_DestroyRenderer(app.renderer);

	SDL_DestroyWindow(app.window);

	TTF_Quit();

	SDL_Quit();
}

