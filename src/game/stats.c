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
#include "stats.h"
#include "../system/widgets.h"
#include "../system/text.h"
#include "../system/atlas.h"
#include "../system/draw.h"
#include "../system/controls.h"

#define NUM_VISIBLE_STATS    7

extern App app;
extern Game game;

static void logic(void);
static void draw(void);
static void back(void);
static void initStatNames(void);
static void drawArrows(void);
static void drawStats(void);
static void calculatePercentComplete(void);

static void (*oldDraw)(void);
static void (*returnFromStats)(void);
char *statNames[STAT_MAX];
static int start, end;
static AtlasImage *arrow;

void initStats(void (*done)(void))
{
	initStatNames();

	calculatePercentComplete();

	arrow = getAtlasImage("gfx/main/arrow.png", 1);

	app.selectedWidget = getWidget("back", "stats");
	app.selectedWidget->action = back;

	calculateWidgetFrame("stats");

	showWidgets("stats", 1);

	oldDraw = app.delegate.draw;

	returnFromStats = done;

	start = 0;

	app.delegate.logic = logic;
	app.delegate.draw = draw;
}

static void logic(void)
{
	if (app.keyboard[SDL_SCANCODE_UP] || isControl(CONTROL_UP))
	{
		start = MAX(start - 1, 0);
	}

	if (app.keyboard[SDL_SCANCODE_DOWN] || isControl(CONTROL_DOWN))
	{
		start = MIN(start + 1, STAT_TIME - 1);
	}

	if (app.keyboard[SDL_SCANCODE_ESCAPE])
	{
		app.keyboard[SDL_SCANCODE_ESCAPE] = 0;

		back();
	}

	end = start + NUM_VISIBLE_STATS;

	doWidgets("stats");
}

static void draw(void)
{
	drawStats();

	drawArrows();

	drawWidgetFrame();

	drawWidgets("stats");
}

static void drawArrows(void)
{
	SDL_SetTextureColorMod(arrow->texture, 64, 64, 64);

	if (start > 0)
	{
		SDL_SetTextureColorMod(arrow->texture, 255, 255, 255);
	}

	blitAtlasImage(arrow, (SCREEN_WIDTH / 2) - 25, 490, 1, SDL_FLIP_NONE);

	SDL_SetTextureColorMod(arrow->texture, 64, 64, 64);

	if (start < STAT_TIME - 1)
	{
		SDL_SetTextureColorMod(arrow->texture, 255, 255, 255);
	}

	blitAtlasImage(arrow, (SCREEN_WIDTH / 2) + 25, 490, 1, SDL_FLIP_VERTICAL);

	SDL_SetTextureColorMod(arrow->texture, 255, 255, 255);
}

static void drawStats(void)
{
	int i, y, h, m, s;
	SDL_Rect r;

	r.w = 700;
	r.h = 450;
	r.x = (SCREEN_WIDTH - r.w) / 2;
	r.y = 115;

	oldDraw();

	drawText(SCREEN_WIDTH / 2, 25, 96, TEXT_CENTER, app.colors.white, "STATS");

	drawRect(r.x, r.y, r.w, r.h, 0, 0, 0, 255);
	drawOutlineRect(r.x, r.y, r.w, r.h, 255, 255, 255, 255);

	y = 125;

	for (i = start ; i < end ; i++)
	{
		if (i < STAT_MAX - 1)
		{
			drawText(r.x + 25, y, 48, TEXT_LEFT, app.colors.white, statNames[i]);

			switch (i)
			{
				case STAT_MOVED:
				case STAT_FALLEN:
					drawText(r.x + r.w - 25, y, 48, TEXT_RIGHT, app.colors.white, "%dm", game.stats[i] / 24);
					break;

				case STAT_PERCENT_COMPLETE:
					drawText(r.x + r.w - 25, y, 48, TEXT_RIGHT, app.colors.white, "%d%%", game.stats[i]);
					break;

				default:
					drawText(r.x + r.w - 25, y, 48, TEXT_RIGHT, app.colors.white, "%d", game.stats[i]);
					break;
			}

			y += 48;
		}
	}

	h = (game.stats[STAT_TIME] / 3600);
	m = (game.stats[STAT_TIME] / 60) % 60;
	s = game.stats[STAT_TIME] % 60;

	drawText(r.x + 25, r.y + r.h - 50, 48, TEXT_LEFT, app.colors.white, statNames[STAT_TIME]);
	drawText(r.x + r.w - 25, r.y + r.h - 50, 48, TEXT_RIGHT, app.colors.white, "%02d:%02d:%02d", h, m, s);
}

static void back(void)
{
	returnFromStats();
}

static void calculatePercentComplete(void)
{
	StageMeta *s;
	float current, total;

	current = total = 0;

	current += game.stagesComplete;

	for (s = game.stageMetaHead.next ; s != NULL ; s = s->next)
	{
		/* count number of stages */
		total++;

		total += s->coins;
		current += s->coinsFound;

		total += s->items;
		current += s->itemsFound;
	}

	current /= total;

	current *= 100;

	game.stats[STAT_PERCENT_COMPLETE] = ceil(current);
}

static void initStatNames(void)
{
	statNames[STAT_PERCENT_COMPLETE] = "Percent complete";
	statNames[STAT_STAGES_STARTED] = "Stages attempted";
	statNames[STAT_STAGES_COMPLETED] = "Stages completed";
	statNames[STAT_FAILS] = "Stages failed";
	statNames[STAT_DEATHS] = "Times killed";
	statNames[STAT_CLONES] = "Clones created";
	statNames[STAT_CLONE_DEATHS] = "Clones killed";
	statNames[STAT_KEYS] = "Keys collected";
	statNames[STAT_PLUNGERS] = "Plungers picked up";
	statNames[STAT_MANHOLE_COVERS] = "Manhole covers picked up";
	statNames[STAT_WATER_PISTOLS] = "Water pistols picked up";
	statNames[STAT_ITEMS] = "Items collected";
	statNames[STAT_COINS] = "Coins collected";
	statNames[STAT_JUMPS] = "Times jumped";
	statNames[STAT_MOVED] = "Total distance moved";
	statNames[STAT_FALLEN] = "Total distance fallen";
	statNames[STAT_SHOTS_FIRED] = "Water shots fired";
	statNames[STAT_TIME] = "Time played";
}

