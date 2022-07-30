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
#include "widgets.h"
#include "../json/cJSON.h"
#include "../system/lookup.h"
#include "../system/controls.h"
#include "../system/sound.h"
#include "../system/io.h"
#include "../system/text.h"
#include "../system/draw.h"

extern App app;

static void loadAllWidgets(void);
static void loadWidgets(const char *filename);
static void findNextWidget(const char *groupName, int dir);
static void changeWidgetValue(int dir);
static SDL_Color getWidgetColor(Widget *w);
static void setupOptions(Widget *w, cJSON *root);

static SDL_Rect frame;

void initWidgets(void)
{
	loadAllWidgets();
}

void doWidgets(const char *groupName)
{
	int id;

	if (!app.awaitingWidgetInput)
	{
		if (app.keyboard[SDL_SCANCODE_UP] || isControl(CONTROL_UP))
		{
			clearControl(CONTROL_UP);

			app.keyboard[SDL_SCANCODE_UP] = 0;

			playSound(SND_TIP, CH_WIDGET);

			findNextWidget(groupName, -1);
		}

		if (app.keyboard[SDL_SCANCODE_DOWN] || isControl(CONTROL_DOWN))
		{
			clearControl(CONTROL_DOWN);

			app.keyboard[SDL_SCANCODE_DOWN] = 0;

			playSound(SND_TIP, CH_WIDGET);

			findNextWidget(groupName, 1);
		}

		if ((app.keyboard[SDL_SCANCODE_LEFT] || isControl(CONTROL_LEFT)) && app.selectedWidget->type == WT_SELECT)
		{
			clearControl(CONTROL_LEFT);

			app.keyboard[SDL_SCANCODE_LEFT] = 0;

			changeWidgetValue(-1);
		}

		if ((app.keyboard[SDL_SCANCODE_RIGHT] || isControl(CONTROL_RIGHT)) && app.selectedWidget->type == WT_SELECT)
		{
			clearControl(CONTROL_RIGHT);

			app.keyboard[SDL_SCANCODE_RIGHT] = 0;

			changeWidgetValue(1);
		}

		if (isAcceptControl())
		{
			clearAcceptControls();

			if (!app.selectedWidget->disabled)
			{
				playSound(SND_TIP, CH_WIDGET);

				app.selectedWidget->action();
			}
			else
			{
				playSound(SND_NEGATIVE, CH_WIDGET);
			}
		}
	}
	else
	{
		if (app.lastKeyPressed != -1)
		{
			app.awaitingWidgetInput = 0;

			id = lookup(app.selectedWidget->name);

			app.config.keyControls[id] = app.lastKeyPressed;

			updateControlWidget(app.selectedWidget, id);

			app.keyboard[app.lastKeyPressed] = 0;
		}

		if (app.lastButtonPressed != -1)
		{
			app.awaitingWidgetInput = 0;

			id = lookup(app.selectedWidget->name);

			app.config.joypadControls[id] = app.lastButtonPressed;

			updateControlWidget(app.selectedWidget, id);

			app.joypadButton[app.lastButtonPressed] = 0;
		}
	}
}

static void changeWidgetValue(int dir)
{
	app.selectedWidget->value = MAX(MIN(app.selectedWidget->value + dir, app.selectedWidget->numOptions - 1), 0);

	app.selectedWidget->action();

	playSound(SND_NUDGE, CH_WIDGET);
}

static void doInputWidget(void)
{
	app.awaitingWidgetInput = 1;

	app.lastKeyPressed = app.lastButtonPressed = -1;
}

static void findNextWidget(const char *groupName, int dir)
{
	int found;

	found = 0;

	do
	{
		if (dir == -1)
		{
			app.selectedWidget = app.selectedWidget->prev;

			if (app.selectedWidget == NULL)
			{
				app.selectedWidget = app.widgetsTail;
			}
		}
		else
		{
			app.selectedWidget = app.selectedWidget->next;

			if (app.selectedWidget == NULL)
			{
				app.selectedWidget = app.widgetsHead.next;
			}
		}

		found = app.selectedWidget->visible && strcmp(app.selectedWidget->groupName, groupName) == 0;
	}
	while (!found);
}

void drawWidgets(const char *groupName)
{
	Widget *w;
	SDL_Color c;
	char controlText[MAX_NAME_LENGTH];

	for (w = app.widgetsHead.next ; w != NULL ; w = w->next)
	{
		if (w->visible && strcmp(w->groupName, groupName) == 0)
		{
			c = getWidgetColor(w);

			switch (w->type)
			{
				case WT_SELECT:
					drawText(w->x, w->y, 64, TEXT_LEFT, c, w->text);
					drawText(SCREEN_WIDTH - w->x, w->y, 64, TEXT_RIGHT, c, w->options[w->value]);
					break;

				case WT_INPUT:
					drawText(w->x, w->y, 64, TEXT_LEFT, c, w->text);
					if (app.awaitingWidgetInput && w == app.selectedWidget)
					{
						drawText(SCREEN_WIDTH - w->x, w->y, 64, TEXT_RIGHT, c, "...");
					}
					else
					{
						if (strcmp(w->options[1], "") != 0)
						{
							sprintf(controlText, "%s or %s", w->options[0], w->options[1]);
						}
						else
						{
							sprintf(controlText, "%s", w->options[0]);
						}
						drawText(SCREEN_WIDTH - w->x, w->y, 64, TEXT_RIGHT, c, controlText);
					}
					break;

				default:
					drawText(w->x, w->y, 64, TEXT_LEFT, c, w->text);
					break;
			}

			if (w == app.selectedWidget)
			{
				drawRect(w->x - 40, w->y + 18, 24, 24, 0, 255, 0, 192);

				if (SDL_GetTicks() % 1000 < 500)
				{
					drawRect(w->x - 40, w->y + 18, 24, 24, 0, 255, 0, 255);
				}
			}
		}
	}
}

void drawWidgetFrame(void)
{
	drawRect(frame.x, frame.y, frame.w, frame.h, 0, 0, 0, 192);

	drawOutlineRect(frame.x, frame.y, frame.w, frame.h, 255, 255, 255, 255);
}

static SDL_Color getWidgetColor(Widget *w)
{
	if (w->disabled)
	{
		return app.colors.darkGrey;
	}
	else if (w == app.selectedWidget)
	{
		return app.colors.green;
	}

	return app.colors.white;
}

void calculateWidgetFrame(const char *groupName)
{
	Widget *w;

	frame.x = SCREEN_WIDTH;
	frame.y = SCREEN_HEIGHT;
	frame.w = 0;
	frame.h = 0;

	for (w = app.widgetsHead.next ; w != NULL ; w = w->next)
	{
		if (strcmp(w->groupName, groupName) == 0)
		{
			frame.x = MIN(w->x - 50, frame.x);
			frame.y = MIN(w->y - 25, frame.y);
			frame.w = MAX(w->w + 100, frame.w);
			frame.h = MAX(w->y + w->h + 25, frame.h);
		}
	}

	frame.h -= frame.y;
}

void showWidgets(const char *groupName, int visible)
{
	Widget *w;

	for (w = app.widgetsHead.next ; w != NULL ; w = w->next)
	{
		if (strcmp(w->groupName, groupName) == 0)
		{
			w->visible = visible;
		}
	}
}

Widget *getWidget(const char *name, const char *groupName)
{
	Widget *w;

	for (w = app.widgetsHead.next ; w != NULL ; w = w->next)
	{
		if (strcmp(w->name, name) == 0 && strcmp(w->groupName, groupName) == 0)
		{
			return w;
		}
	}

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, "No such widget name='%s', groupName='%s'", name, groupName);
	exit(1);

	return NULL;
}

void updateControlWidget(Widget *w, int c)
{
	sprintf(w->options[0], "%s", SDL_GetScancodeName(app.config.keyControls[c]));

	if (app.config.joypadControls[c] != -1)
	{
		sprintf(w->options[1], "Btn %d", app.config.joypadControls[c]);
	}
	else
	{
		strcpy(w->options[1], "");
	}
}

static void loadAllWidgets(void)
{
	char **filenames, filename[MAX_FILENAME_LENGTH];
	int count, i;

	filenames = getFileList("data/widgets", &count);

	app.widgetsTail = &app.widgetsHead;

	for (i = 0 ; i < count ; i++)
	{
		sprintf(filename, "data/widgets/%s", filenames[i]);

		loadWidgets(filename);

		free(filenames[i]);
	}

	free(filenames);
}

static void loadWidgets(const char *filename)
{
	Widget *w;
	cJSON *root, *node;
	char *text;

	text = readFile(getFileLocation(filename));

	root = cJSON_Parse(text);

	for (node = root->child ; node != NULL ; node = node->next)
	{
		w = malloc(sizeof(Widget));
		memset(w, 0, sizeof(Widget));
		w->prev = app.widgetsTail;
		app.widgetsTail->next = w;
		app.widgetsTail = w;

		w->type = lookup(cJSON_GetObjectItem(node, "type")->valuestring);
		STRNCPY(w->name, cJSON_GetObjectItem(node, "name")->valuestring, MAX_NAME_LENGTH);
		STRNCPY(w->groupName, cJSON_GetObjectItem(node, "groupName")->valuestring, MAX_NAME_LENGTH);
		w->x = cJSON_GetObjectItem(node, "x")->valueint;
		w->y = cJSON_GetObjectItem(node, "y")->valueint;
		STRNCPY(w->text, cJSON_GetObjectItem(node, "text")->valuestring, MAX_NAME_LENGTH);

		switch (w->type)
		{
			case WT_BUTTON:
				calcTextDimensions(w->text, 64, &w->w, &w->h);
				break;

			case WT_SELECT:
				calcTextDimensions(w->text, 64, &w->w, &w->h);
				w->w = SCREEN_WIDTH - (w->x * 2);
				setupOptions(w, cJSON_GetObjectItem(node, "options"));
				break;

			case WT_INPUT:
				calcTextDimensions(w->text, 64, &w->w, &w->h);
				w->w = SCREEN_WIDTH - (w->x * 2);
				w->options = malloc(sizeof(char *) * 2);
				w->options[0] = malloc(MAX_NAME_LENGTH);
				w->options[1] = malloc(MAX_NAME_LENGTH);
				w->action = doInputWidget;
				break;
		}

		if (w->x == -1)
		{
			w->x = (SCREEN_WIDTH - w->w) / 2;
		}
	}

	cJSON_Delete(root);

	free(text);
}

static void setupOptions(Widget *w, cJSON *root)
{
	cJSON *node;
	int i, l;

	w->numOptions = cJSON_GetArraySize(root);

	w->options = malloc(sizeof(char *) * w->numOptions);

	i = 0;

	for (node = root->child ; node != NULL ; node = node->next)
	{
		l = strlen(node->valuestring) + 1;

		w->options[i] = malloc(l);

		STRNCPY(w->options[i], node->valuestring, l);

		i++;
	}
}

