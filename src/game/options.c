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
static void back(void);

static Widget *soundWidget;
static Widget *musicWidget;
static Widget *windowSizeWidget;
static Widget *fullscreenWidget;
static Widget *tipsWidget;
static Widget *backWidget;
static void (*oldDraw)(void);
static void (*returnFromOptions)(void);

void initOptions(void (*done)(void))
{
	soundWidget = getWidget("soundVolume", "options");
	soundWidget->action = sound;
	
	musicWidget = getWidget("musicVolume", "options");
	musicWidget->action = music;
	
	windowSizeWidget = getWidget("windowSize", "options");
	windowSizeWidget->action = windowSize;
	
	fullscreenWidget = getWidget("fullscreen", "options");
	fullscreenWidget->action = fullscreen;
	
	tipsWidget = getWidget("tips", "options");
	tipsWidget->action = tips;
	
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
	
	drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0, 96);
	
	drawText(SCREEN_WIDTH / 2, 75, 96, TEXT_CENTER, app.colors.white, "OPTIONS");
	
	drawWidgetFrame();
	
	drawWidgets("options");
}

static void sound(void)
{
	int val;
	
	val = atoi(app.selectedWidget->options[app.selectedWidget->value]);
	
	app.config.soundVolume = val;
}

static void music(void)
{
	int val;
	
	val = atoi(app.selectedWidget->options[app.selectedWidget->value]);
	
	app.config.musicVolume = val;
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
	app.config.fullscreen = app.selectedWidget->value;
}

static void back(void)
{
	returnFromOptions();
}
