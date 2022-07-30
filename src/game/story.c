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
#include "story.h"
#include "../world/map.h"
#include "../system/widgets.h"
#include "../system/text.h"
#include "../system/io.h"
#include "../world/entities.h"
#include "../system/draw.h"

extern App app;

static void logic(void);
static void draw(void);
static void back(void);

static void (*returnFromStory)(void);
static char *text;

void initStory(void (*done)(void))
{
	text = readFile(getFileLocation("data/misc/story.txt"));

	returnFromStory = done;

	app.selectedWidget = getWidget("back", "story");
	app.selectedWidget->action = back;

	calculateWidgetFrame("story");

	showWidgets("story", 1);

	app.delegate.logic = logic;
	app.delegate.draw = draw;
}

static void logic(void)
{
	doWidgets("story");
}

static void draw(void)
{
	app.dev.drawing = 0;

	drawEntities(1);

	drawMap();

	drawEntities(0);

	drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0, 192);

	app.text.wrap = SCREEN_WIDTH - 200;

	drawText(SCREEN_WIDTH / 2, 25, 64, TEXT_CENTER, app.colors.white, "The Story!");

	drawText(100, 100, 40, TEXT_LEFT, app.colors.white, text);

	drawWidgetFrame();

	drawWidgets("story");
}

static void back(void)
{
	returnFromStory();

	free(text);
}

