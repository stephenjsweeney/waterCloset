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
#include "title.h"
#include "../system/atlas.h"
#include "../game/credits.h"
#include "../game/options.h"
#include "../game/stats.h"
#include "../system/sound.h"
#include "../world/map.h"
#include "../system/widgets.h"
#include "../game/game.h"
#include "../game/stageSelect.h"
#include "../system/wipe.h"
#include "../world/stage.h"
#include "../system/text.h"
#include "../world/entities.h"
#include "../system/draw.h"
#include "../game/story.h"

extern App app;
extern Stage stage;

static void logic(void);
static void draw(void);
static void start(void);
static void options(void);
static void stats(void);
static void story(void);
static void credits(void);
static void quit(void);

static AtlasImage *waterTexture;
static AtlasImage *closetTexture;
static Widget *startWidget;
static Widget *optionsWidget;
static Widget *statsWidget;
static Widget *storyWidget;
static Widget *creditsWidget;
static Widget *quitWidget;
static Widget *previousWidget;

void initTitle(void)
{
	waterTexture = getAtlasImage("gfx/main/water.png", 1);
	closetTexture = getAtlasImage("gfx/main/closet.png", 1);

	startWidget = getWidget("start", "title");
	startWidget->action = start;

	optionsWidget = getWidget("options", "title");
	optionsWidget->action = options;

	statsWidget = getWidget("stats", "title");
	statsWidget->action = stats;

	storyWidget = getWidget("story", "title");
	storyWidget->action = story;

	creditsWidget = getWidget("credits", "title");
	creditsWidget->action = credits;

	quitWidget = getWidget("quit", "title");
	quitWidget->action = quit;

	showWidgets("title", 1);

	calculateWidgetFrame("title");

	app.selectedWidget = startWidget;

	memset(&stage, 0, sizeof(Stage));
	stage.entityTail = &stage.entityHead;

	loadStage(0);

	stage.player->atlasImage = getAtlasImage("gfx/entities/guyPlunger.png", 1);

	stage.player->tick = NULL;

	previousWidget = NULL;

	saveGame();

	randomizeTiles();

	initWipe(WIPE_FADE);

	loadMusic("music/Meadow Thoughts.ogg");

	playMusic(1);

	app.delegate.logic = logic;
	app.delegate.draw = draw;
}

static void logic(void)
{
	doWipe();

	doEntities();

	stage.camera.x = stage.camera.y = 0;

	doWidgets("title");
}

static void draw(void)
{
	app.dev.drawing = 0;

	drawEntities(1);

	drawMap();

	drawEntities(0);

	drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0, 96);

	if (previousWidget == NULL)
	{
		blitAtlasImage(waterTexture, (SCREEN_WIDTH / 2) - (waterTexture->rect.w / 2) - 25, 150, 1, SDL_FLIP_NONE);
		blitAtlasImage(closetTexture, (SCREEN_WIDTH / 2) + (closetTexture->rect.w / 2) + 25, 150, 1, SDL_FLIP_NONE);

		drawWidgetFrame();
	}

	drawWidgets("title");

	if (previousWidget != creditsWidget)
	{
		drawText(10, SCREEN_HEIGHT - 35, 32, TEXT_LEFT, app.colors.white, "Copyright Parallel Realities, 2019-2020");
		drawText(SCREEN_WIDTH - 10, SCREEN_HEIGHT - 35, 32, TEXT_RIGHT, app.colors.white, "Version %.1f.%d", VERSION, REVISION);
	}

	drawWipe();
}

static void returnFrom(void)
{
	showWidgets("title", 1);

	calculateWidgetFrame("title");

	app.selectedWidget = previousWidget;

	previousWidget = NULL;

	app.delegate.logic = logic;
	app.delegate.draw = draw;
}

static void start(void)
{
	showWidgets("title", 0);

	initStageSelect(returnFrom);

	previousWidget = startWidget;
}

static void options(void)
{
	showWidgets("title", 0);

	initOptions(returnFrom);

	previousWidget = optionsWidget;
}

static void stats(void)
{
	showWidgets("title", 0);

	initStats(returnFrom);

	previousWidget = statsWidget;
}

static void story(void)
{
	showWidgets("title", 0);

	initStory(returnFrom);

	previousWidget = storyWidget;
}

static void credits(void)
{
	showWidgets("title", 0);

	initCredits(returnFrom);

	previousWidget = creditsWidget;
}

static void quit(void)
{
	exit(0);
}

