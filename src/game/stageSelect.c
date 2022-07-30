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
#include "stageSelect.h"
#include "../system/draw.h"
#include "../system/controls.h"
#include "../system/widgets.h"
#include "../system/sound.h"
#include "../world/stage.h"
#include "../system/text.h"
#include "../system/atlas.h"

#define NUM_VISIBLE_STAGES    7

extern App app;
extern Game game;
extern Stage stage;

static void logic(void);
static void draw(void);
static void back(void);
static void drawArrows(void);
static void drawStages(void);

static void (*oldDraw)(void);
static void (*returnFromStageSelect)(void);
static int start;
static AtlasImage *arrow;
static AtlasImage *tick;
static AtlasImage *noTick;
static int scrollTimer;

void initStageSelect(void (*done)(void))
{
	arrow = getAtlasImage("gfx/main/arrow.png", 1);
	tick = getAtlasImage("gfx/main/tick.png", 1);
	noTick = getAtlasImage("gfx/main/noTick.png", 1);

	showWidgets("stageSelect", 1);

	oldDraw = app.delegate.draw;

	start = 1;

	scrollTimer = 0;

	returnFromStageSelect = done;

	app.delegate.logic = logic;
	app.delegate.draw = draw;
}

static void logic(void)
{
	if (--scrollTimer <= 0)
	{
		if (app.keyboard[SDL_SCANCODE_UP] || isControl(CONTROL_UP))
		{
			playSound(SND_TIP, CH_WIDGET);

			start = MAX(start - 1, 1);

			scrollTimer = 6;
		}

		if (app.keyboard[SDL_SCANCODE_DOWN] || isControl(CONTROL_DOWN))
		{
			playSound(SND_TIP, CH_WIDGET);

			start = MIN(start + 1, game.numStages);

			scrollTimer = 6;
		}
	}

	if (app.keyboard[SDL_SCANCODE_ESCAPE])
	{
		app.keyboard[SDL_SCANCODE_ESCAPE] = 0;

		back();
	}

	if (isAcceptControl())
	{
		clearAcceptControls();

		if (start <= game.stagesComplete + 1)
		{
			destroyStage();

			initStage();

			stage.num = start;

			loadStage(1);

			loadRandomStageMusic(1);
		}
		else
		{
			playSound(SND_NEGATIVE, CH_WIDGET);
		}
	}
}

static void draw(void)
{
	drawStages();

	drawArrows();
}

static void drawArrows(void)
{
	SDL_SetTextureColorMod(arrow->texture, 64, 64, 64);

	if (start > 0)
	{
		SDL_SetTextureColorMod(arrow->texture, 255, 255, 255);
	}

	blitAtlasImage(arrow, (SCREEN_WIDTH / 2) - 25, 560, 1, SDL_FLIP_NONE);

	SDL_SetTextureColorMod(arrow->texture, 64, 64, 64);

	if (start < game.numStages)
	{
		SDL_SetTextureColorMod(arrow->texture, 255, 255, 255);
	}

	blitAtlasImage(arrow, (SCREEN_WIDTH / 2) + 25, 560, 1, SDL_FLIP_VERTICAL);

	SDL_SetTextureColorMod(arrow->texture, 255, 255, 255);
}

static void drawStages(void)
{
	int y, from, end;
	SDL_Rect r;
	StageMeta *s;
	SDL_Color c;

	r.w = 600;
	r.h = 550;
	r.x = (SCREEN_WIDTH - r.w) / 2;
	r.y = 115;

	y = r.y + 16;

	oldDraw();

	drawText(SCREEN_WIDTH / 2, 25, 96, TEXT_CENTER, app.colors.white, "SELECT STAGE");

	drawRect(r.x, r.y, r.w, r.h, 0, 0, 0, 255);
	drawOutlineRect(r.x, r.y, r.w, r.h, 255, 255, 255, 255);

	drawText(r.x + 85, y, 48, TEXT_LEFT, app.colors.white, "Stage");
	drawText(r.x + (r.w / 2) - 48, y, 48, TEXT_LEFT, app.colors.white, "Coins");
	drawText(r.x + r.w - 64, y, 48, TEXT_RIGHT, app.colors.white, "Items");

	y += 56;

	from = MAX(start - 3, 1);
	end = from + NUM_VISIBLE_STAGES;

	for (s = game.stageMetaHead.next ; s != NULL ; s = s->next)
	{
		if (s->stageNum >= from && s->stageNum < end)
		{
			c = app.colors.white;

			if (s->stageNum > game.stagesComplete + 1)
			{
				c = app.colors.darkGrey;
			}
			else if (s->stageNum == start)
			{
				c = app.colors.green;
			}

			drawText(r.x + 85, y, 48, TEXT_LEFT, c, "%03d", s->stageNum);
			drawText(r.x + (r.w / 2) - 48, y, 48, TEXT_LEFT, c, "%02d / %02d", s->coinsFound, s->coins);
			drawText(r.x + r.w - 64, y, 48, TEXT_RIGHT, c, "%d / %d", s->itemsFound, s->items);

			if (s->coins)
			{
				if (s->coinsFound == s->coins)
				{
					blitAtlasImage(tick, r.x + 385, y + 13, 0, SDL_FLIP_NONE);
				}
				else
				{
					blitAtlasImage(noTick, r.x + 385, y + 13, 0, SDL_FLIP_NONE);
				}
			}

			if (s->items)
			{
				if (s->itemsFound == s->items)
				{
					blitAtlasImage(tick, r.x + 550, y + 13, 0, SDL_FLIP_NONE);
				}
				else
				{
					blitAtlasImage(noTick, r.x + 550, y + 13, 0, SDL_FLIP_NONE);
				}
			}

			if (s->stageNum == start)
			{
				drawRect(r.x + 25, y + 12, 20, 20, 0, 255, 0, 192);

				if (SDL_GetTicks() % 1000 < 500)
				{
					drawRect(r.x + 25, y + 12, 20, 20, 0, 255, 0, 255);
				}
			}

			y += 48;
		}
	}

	drawText(r.x + 25, r.y + r.h - 40, 32, TEXT_LEFT, app.colors.white, "[Escape] Back");

	if (start <= game.stagesComplete + 1)
	{
		drawText(r.x + r.w - 25, r.y + r.h - 40, 32, TEXT_RIGHT, app.colors.white, "[Return] Start");
	}
	else
	{
		drawText(r.x + r.w - 25, r.y + r.h - 40, 32, TEXT_RIGHT, app.colors.lightGrey, "Stage is locked");
	}
}

static void back(void)
{
	returnFromStageSelect();
}

