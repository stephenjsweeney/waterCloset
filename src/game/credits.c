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
#include "credits.h"
#include "../world/entities.h"
#include "../system/text.h"
#include "../system/io.h"

extern App app;

static void loadCredits(void);
static void logic(void);
static void draw(void);
static void destroyCredits(void);

static int timeout;
static Credit creditsHead;
static void (*oldDraw)(void);
static int scrollCredits;
static void (*returnFromCredits)(void);

void initCredits(void (*done)(void))
{
	memset(&creditsHead, 0, sizeof(Credit));

	loadCredits();

	scrollCredits = 1;

	timeout = FPS * 2;

	oldDraw = app.delegate.draw;

	returnFromCredits = done;

	app.delegate.logic = logic;
	app.delegate.draw = draw;
}

static void logic(void)
{
	Credit *c;

	if (scrollCredits)
	{
		for (c = creditsHead.next ; c != NULL ; c = c->next)
		{
			c->y--;

			if (!c->next && c->y <= SCREEN_HEIGHT - 100)
			{
				scrollCredits = 0;
			}
		}
	}

	if (!scrollCredits)
	{
		timeout--;
	}

	doEntities();

	if (timeout <= 0 || app.keyboard[SDL_SCANCODE_ESCAPE])
	{
		app.keyboard[SDL_SCANCODE_ESCAPE] = 0;

		destroyCredits();

		returnFromCredits();
	}
}

static void draw(void)
{
	Credit *c;

	oldDraw();

	for (c = creditsHead.next ; c != NULL ; c = c->next)
	{
		if (c->y > -48 && c->y < SCREEN_HEIGHT)
		{
			drawText(SCREEN_WIDTH / 2, c->y, 32 * c->size, TEXT_CENTER, app.colors.white, c->text);
		}
	}
}

static void loadCredits(void)
{
	char *text, *p, line[MAX_LINE_LENGTH];
	int y, i;
	Credit *c, *tail;

	y = SCREEN_HEIGHT;

	text = readFile(getFileLocation("data/misc/credits.txt"));

	p = text;

	tail = &creditsHead;

	while (p)
	{
		memset(line, '\0', MAX_LINE_LENGTH);

		i = 0;

		while (*p != '\n')
		{
			line[i++] = *p;

			p++;
		}

		/* chomp end of line */
		p++;

		if (i > 1)
		{
			c = malloc(sizeof(Credit));
			memset(c, 0, sizeof(Credit));
			tail->next = c;
			tail = c;

			c->text = malloc(i + 1);
			memset(c->text, '\0', i + 1);

			sscanf(line, "%d %[^\n]", &c->size, c->text);

			c->y = y;

			y += 32 * c->size;
		}
		else
		{
			y += 48;
		}

		if (*p == '\0')
		{
			p = NULL;
		}
	}

	free(text);
}

static void destroyCredits(void)
{
	Credit *c;

	while (creditsHead.next)
	{
		c = creditsHead.next;
		creditsHead.next = c->next;
		free(c->text);
		free(c);
	}
}

