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

#include "widgets.h"

static void loadAllWidgets(void);
static void loadWidgets(const char *filename);
static void findNextWidget(const char *groupName, int dir);
static void changeWidgetValue(int dir);

void initWidgets(void)
{
	loadAllWidgets();
}

void doWidgets(const char *groupName)
{
	if (app.keyboard[SDL_SCANCODE_UP])
	{
		findNextWidget(groupName, -1);
	}
	
	if (app.keyboard[SDL_SCANCODE_DOWN])
	{
		findNextWidget(groupName, 1);
	}
	
	if (app.keyboard[SDL_SCANCODE_LEFT])
	{
		changeWidgetValue(-1);
	}
	
	if (app.keyboard[SDL_SCANCODE_RIGHT])
	{
		changeWidgetValue(1);
	}
	
	if (app.keyboard[SDL_SCANCODE_SPACE] || app.keyboard[SDL_SCANCODE_RETURN])
	{
		app.selectedWidget->action();
	}
}

static void changeWidgetValue(int dir)
{
	app.selectedWidget->value = MAX(MIN(app.selectedWidget->value + dir, app.selectedWidget->maxValue), app.selectedWidget->minValue);
	
	app.selectedWidget->action();
}

static void findNextWidget(const char *groupName, int dir)
{
	int found;
	
	found = 0;
	
	do
	{
		if (dir == 1)
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
		
		found = strcmp(app.selectedWidget->groupName, groupName) == 0;
	}
	while (!found);
}

void drawWidgets(const char *groupName)
{
	Widget *w;
	
	for (w = app.widgetsHead.next ; w != NULL ; w = w->next)
	{
		if (strcmp(w->groupName, groupName) == 0)
		{
			switch (w->type)
			{
			}
		}
	}
}

Widget *getWidget(const char *name, const char *groupName)
{
	Widget *w;
	
	for (w = app.widgetsHead.next ; w != NULL ; w = w->next)
	{
		if (strcmp(w->groupName, groupName) == 0)
		{
			return w;
		}
	}
	
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, "No such widget name='%s', groupName='%s'", name, groupName);
	exit(1);
	
	return NULL;
}

static void loadAllWidgets(void)
{
	char **filenames, filename[MAX_FILENAME_LENGTH];
	int count, i;

	filenames = getFileList("data/widgets", &count);

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
	
	text = readFile(filename);

	root = cJSON_Parse(text);
	
	app.widgetsTail = &app.widgetsHead;
	
	for (node = root->child ; node != NULL ; node = node->next)
	{
		w = malloc(sizeof(Widget));
		memset(w, 0, sizeof(Widget));
		app.widgetsTail->next = w;
		app.widgetsTail = w;
		
		STRNCPY(w->name, cJSON_GetObjectItem(node, "name")->valuestring, MAX_NAME_LENGTH);
		STRNCPY(w->groupName, cJSON_GetObjectItem(node, "groupName")->valuestring, MAX_NAME_LENGTH);
		w->type = lookup(cJSON_GetObjectItem(node, "type")->valuestring);
		w->x = cJSON_GetObjectItem(node, "x")->valueint;
		w->y = cJSON_GetObjectItem(node, "y")->valueint;
		w->w = cJSON_GetObjectItem(node, "x")->valueint;
		w->h = cJSON_GetObjectItem(node, "y")->valueint;
		STRNCPY(w->text, cJSON_GetObjectItem(node, "text")->valuestring, MAX_NAME_LENGTH);
	}
	
	cJSON_Delete(root);
	
	free(text);
}
