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

#include "title.h"

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

void initTitle(void)
{
	waterTexture = getAtlasImage("gfx/main/water.png", 1);
	closetTexture = getAtlasImage("gfx/main/closet.png", 1);
	
	startWidget = getWidget("start", "title");
	startWidget->action = start;
	
	optionsWidget = getWidget("options", "title");
	optionsWidget->action = options;
	optionsWidget->disabled = 1;
	
	statsWidget = getWidget("stats", "title");
	statsWidget->action = stats;
	statsWidget->disabled = 1;
	
	storyWidget = getWidget("story", "title");
	storyWidget->action = story;
	storyWidget->disabled = 1;
	
	creditsWidget = getWidget("credits", "title");
	creditsWidget->action = credits;
	creditsWidget->disabled = 1;
	
	quitWidget = getWidget("quit", "title");
	quitWidget->action = quit;
	
	app.delegate.logic = logic;
	app.delegate.draw = draw;
	
	app.selectedWidget = startWidget;
}

static void logic(void)
{
	doWidgets("title");
}

static void draw(void)
{
	blitAtlasImage(waterTexture, (SCREEN_WIDTH / 2) - (waterTexture->rect.w / 2) - 25, 200, 1, SDL_FLIP_NONE);
	blitAtlasImage(closetTexture, (SCREEN_WIDTH / 2) + (closetTexture->rect.w / 2) + 25, 200, 1, SDL_FLIP_NONE);
	
	drawWidgets("title");
}

static void start(void)
{
	initStage();
	
	stage.num = 1;
	
	loadStage(1);
	
	loadRandomStageMusic();
}

static void options(void)
{
}

static void stats(void)
{
}

static void story(void)
{
}

static void credits(void)
{
}

static void quit(void)
{
}
