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
#include "wipe.h"
#include "../system/draw.h"

extern App app;

static int doFadeIn(void);
static int doHorizontalWipe(void);
static void drawFadeIn(void);
static void drawWipeIn(void);
static void drawWipeOut(void);

void initWipe(int type)
{
	app.wipe.type = type;

	switch (app.wipe.type)
	{
		case WIPE_FADE:
			app.wipe.value = 255;
			break;

		case WIPE_IN:
		case WIPE_OUT:
			app.wipe.value = 0;
			break;

		default:
			break;
	}
}

int doWipe(void)
{
	switch (app.wipe.type)
	{
		case WIPE_FADE:
			return doFadeIn();

		case WIPE_IN:
		case WIPE_OUT:
			return doHorizontalWipe();

		default:
			break;
	}

	return 1;
}

static int doFadeIn(void)
{
	app.wipe.value -= 255 / 20;

	app.wipe.value = MAX(0, app.wipe.value);

	return app.wipe.value == 0;
}

static int doHorizontalWipe(void)
{
	app.wipe.value += SCREEN_WIDTH / 30;

	app.wipe.value = MIN(SCREEN_WIDTH, app.wipe.value);

	return app.wipe.value == SCREEN_WIDTH;
}

void drawWipe(void)
{
	switch (app.wipe.type)
	{
		case WIPE_FADE:
			drawFadeIn();
			break;

		case WIPE_IN:
			drawWipeIn();
			break;

		case WIPE_OUT:
			drawWipeOut();
			break;

		default:
			break;
	}
}

static void drawFadeIn(void)
{
	drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0, app.wipe.value);
}

static void drawWipeIn(void)
{
	drawRect(app.wipe.value, 0, SCREEN_WIDTH - app.wipe.value, SCREEN_HEIGHT, 0, 0, 0, 255);
}

static void drawWipeOut(void)
{
	drawRect(0, 0, app.wipe.value, SCREEN_HEIGHT, 0, 0, 0, 255);
}

