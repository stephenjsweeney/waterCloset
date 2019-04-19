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

#include "options.h"

static void logic(void);
static void draw(void);
static void sound(void);
static void music(void);
static void windowSize(void);
static void fullscreen(void);
static void tips(void);
static void controls(void);
static void back(void);
static void setWindowSizeWidgetValue(void);

static Widget *soundWidget;
static Widget *musicWidget;
static Widget *windowSizeWidget;
static Widget *fullscreenWidget;
static Widget *tipsWidget;
static Widget *controlsWidget;
static Widget *backWidget;
static void (*oldDraw)(void);
static void (*returnFromOptions)(void);

void initOptions(void (*done)(void))
{
	soundWidget = getWidget("soundVolume", "options");
	soundWidget->action = sound;
	soundWidget->value = app.config.soundVolume / 12.8;
	
	musicWidget = getWidget("musicVolume", "options");
	musicWidget->action = music;
	musicWidget->value = app.config.musicVolume / 12.8;
	
	windowSizeWidget = getWidget("windowSize", "options");
	windowSizeWidget->action = windowSize;
	setWindowSizeWidgetValue();
	
	fullscreenWidget = getWidget("fullscreen", "options");
	fullscreenWidget->action = fullscreen;
	fullscreenWidget->value = app.config.fullscreen;
	
	tipsWidget = getWidget("tips", "options");
	tipsWidget->action = tips;
	tipsWidget->value = app.config.tips;
	
	controlsWidget = getWidget("controls", "options");
	controlsWidget->action = controls;
	controlsWidget->disabled = 1;
	
	backWidget = getWidget("back", "options");
	backWidget->action = back;
	
	app.selectedWidget = soundWidget;
	
	showWidgets("options", 1);
	
	calculateWidgetFrame("options");
	
	oldDraw = app.delegate.draw;
	
	returnFromOptions = done;
	
	app.delegate.logic = logic;
	app.delegate.draw = draw;
}

static void logic(void)
{
	doWidgets("options");
}

static void draw(void)
{
	oldDraw();
	
	drawText(SCREEN_WIDTH / 2, 25, 96, TEXT_CENTER, app.colors.white, "OPTIONS");
	
	drawWidgetFrame();
	
	drawWidgets("options");
}

static void setWindowSizeWidgetValue(void)
{
	int i;
	char resolution[MAX_NAME_LENGTH];
	
	sprintf(resolution, "%d x %d", app.config.winWidth, app.config.winHeight);
	
	for (i = 0 ; i < windowSizeWidget->numOptions ; i++)
	{
		if (strcmp(windowSizeWidget->options[i], resolution) == 0)
		{
			windowSizeWidget->value = i;
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
}

static void back(void)
{
	saveConfig();
	
	returnFromOptions();
}
