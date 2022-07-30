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
#include "options.h"
#include <SDL2/SDL_mixer.h>
#include "../system/text.h"
#include "../system/lookup.h"
#include "../game/game.h"
#include "../system/widgets.h"

#define SHOW_CONTROLS    1
#define SHOW_NORMAL      0

extern App app;

static void logic(void);
static void draw(void);
static void sound(void);
static void music(void);
static void windowSize(void);
static void fullscreen(void);
static void tips(void);
static void controls(void);
static void back(void);
static void setWindowSizeWidgetValue(Widget *w);

static Widget *leftWidget;
static Widget *rightWidget;
static Widget *jumpWidget;
static Widget *useWidget;
static Widget *cloneWidget;
static Widget *restartWidget;
static Widget *pauseWidget;
static void (*oldDraw)(void);
static void (*returnFromOptions)(void);
static int show;

void initOptions(void (*done)(void))
{
	Widget *w;

	w = getWidget("soundVolume", "options");
	w->action = sound;
	w->value = app.config.soundVolume / 12.8;

	w = getWidget("musicVolume", "options");
	w->action = music;
	w->value = app.config.musicVolume / 12.8;

	w = getWidget("windowSize", "options");
	w->action = windowSize;
	setWindowSizeWidgetValue(w);

	w = getWidget("fullscreen", "options");
	w->action = fullscreen;
	w->value = app.config.fullscreen;

	w = getWidget("tips", "options");
	w->action = tips;
	w->value = app.config.tips;

	w = getWidget("controls", "options");
	w->action = controls;

	getWidget("back", "options")->action = back;
	getWidget("back", "controls")->action = back;

	leftWidget = getWidget("left", "controls");
	rightWidget = getWidget("right", "controls");
	jumpWidget = getWidget("jump", "controls");
	useWidget = getWidget("use", "controls");
	cloneWidget = getWidget("clone", "controls");
	restartWidget = getWidget("restart", "controls");
	pauseWidget = getWidget("pause", "controls");

	app.selectedWidget = getWidget("soundVolume", "options");

	show = SHOW_NORMAL;

	showWidgets("options", 1);

	calculateWidgetFrame("options");

	oldDraw = app.delegate.draw;

	returnFromOptions = done;

	app.delegate.logic = logic;
	app.delegate.draw = draw;
}

static void logic(void)
{
	int id;

	switch (show)
	{
		case SHOW_CONTROLS:
			doWidgets("controls");

			if (app.selectedWidget->type == WT_INPUT && app.keyboard[SDL_SCANCODE_BACKSPACE])
			{
				app.keyboard[SDL_SCANCODE_BACKSPACE] = 0;

				id = lookup(app.selectedWidget->name);

				app.config.keyControls[id] = 0;
				app.config.joypadControls[id] = -1;

				updateControlWidget(app.selectedWidget, id);

				app.keyboard[app.lastKeyPressed] = 0;
				app.joypadButton[app.lastButtonPressed] = 0;
			}

			break;

		default:
			doWidgets("options");
			break;
	}

	if (app.keyboard[SDL_SCANCODE_ESCAPE])
	{
		app.keyboard[SDL_SCANCODE_ESCAPE] = 0;

		back();
	}
}

static void draw(void)
{
	oldDraw();

	switch (show)
	{
		case SHOW_CONTROLS:
			drawText(SCREEN_WIDTH / 2, 25, 96, TEXT_CENTER, app.colors.white, "CONTROLS");
			drawWidgetFrame();
			drawWidgets("controls");
			if (!app.awaitingWidgetInput)
			{
				drawText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 75, 32, TEXT_CENTER, app.colors.yellow, "[Return] Change control :: [Backspace] Clear control");
			}
			else
			{
				drawText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 75, 32, TEXT_CENTER, app.colors.yellow, "Press key or joypad button to use");
			}
			break;

		default:
			drawText(SCREEN_WIDTH / 2, 25, 96, TEXT_CENTER, app.colors.white, "OPTIONS");
			drawWidgetFrame();
			drawWidgets("options");
			drawText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 75, 32, TEXT_CENTER, app.colors.yellow, "Restart the game to apply resolution and fullscreen options.");
			break;
	}
}

static void setWindowSizeWidgetValue(Widget *w)
{
	int i;
	char resolution[MAX_NAME_LENGTH];

	sprintf(resolution, "%d x %d", app.config.winWidth, app.config.winHeight);

	for (i = 0 ; i < w->numOptions ; i++)
	{
		if (strcmp(w->options[i], resolution) == 0)
		{
			w->value = i;
			return;
		}
	}
}

static void sound(void)
{
	int val;

	val = atoi(app.selectedWidget->options[app.selectedWidget->value]);

	app.config.soundVolume = val;

	Mix_Volume(-1, app.config.soundVolume * 1.28);
}

static void music(void)
{
	int val;

	val = atoi(app.selectedWidget->options[app.selectedWidget->value]);

	app.config.musicVolume = val;

	Mix_VolumeMusic(app.config.musicVolume * 1.28);
}

static void windowSize(void)
{
	char *val;

	val = app.selectedWidget->options[app.selectedWidget->value];

	sscanf(val, "%d x %d", &app.config.winWidth, &app.config.winHeight);
}

static void fullscreen(void)
{
	app.config.fullscreen = app.selectedWidget->value;
}

static void tips(void)
{
	app.config.tips = app.selectedWidget->value;
}

static void controls(void)
{
	updateControlWidget(leftWidget, CONTROL_LEFT);
	updateControlWidget(rightWidget, CONTROL_RIGHT);
	updateControlWidget(jumpWidget, CONTROL_JUMP);
	updateControlWidget(useWidget, CONTROL_USE);
	updateControlWidget(cloneWidget, CONTROL_CLONE);
	updateControlWidget(restartWidget, CONTROL_RESTART);
	updateControlWidget(pauseWidget, CONTROL_PAUSE);

	showWidgets("controls", 1);

	calculateWidgetFrame("controls");

	show = SHOW_CONTROLS;

	app.selectedWidget = leftWidget;
}

static void back(void)
{
	if (show == SHOW_NORMAL)
	{
		saveConfig();

		returnFromOptions();
	}
	else
	{
		show = SHOW_NORMAL;

		showWidgets("controls", 0);

		showWidgets("options", 1);

		calculateWidgetFrame("options");

		app.selectedWidget = getWidget("soundVolume", "options");
	}
}

