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
#include "game.h"
#include "../json/cJSON.h"
#include "../system/lookup.h"
#include "../game/meta.h"
#include "../system/util.h"
#include "../system/io.h"

extern App app;
extern Game game;

static void loadConfigFile(const char *filename);

void loadGame(void)
{
	char filename[MAX_PATH_LENGTH], *json;
	cJSON *root, *node, *child;
	StageMeta *s;
	int n;

	sprintf(filename, "%s/%s", app.saveDir, SAVE_FILENAME);

	if (fileExists(filename))
	{
		json = readFile(filename);
		if (json != NULL)
		{
			root = cJSON_Parse(json);
			if (root != NULL)
			{
				game.stagesComplete = getJSONIntVal(root, "stagesComplete", 0);

				node = cJSON_GetObjectItem(root, "meta");
				if (node != NULL)
				{
					for (child = node->child ; child != NULL ; child = child->next)
					{
						n = getJSONIntVal(child, "stageNum", 0);
						s = getStageMeta(n);
						if (s != NULL)
						{
							s->coinsFound = getJSONIntVal(child, "coinsFound", 0);
							s->itemsFound = getJSONIntVal(child, "itemsFound", 0);
						}
					}
				}

				node = cJSON_GetObjectItem(root, "stats");
				if (node != NULL)
				{
					for (child = node->child ; child != NULL ; child = child->next)
					{
						cJSON *k = cJSON_GetObjectItem(child, "key");
						cJSON *v = cJSON_GetObjectItem(child, "value");
						if (k && k->valuestring && v)
						{
							n = lookup(k->valuestring);
							if (n >= 0 && n < STAT_MAX)
							{
								game.stats[n] = v->valueint;
							}
						}
					}
				}

				cJSON_Delete(root);
			}

			free(json);
		}
	}
}

void saveGame(void)
{
	char filename[MAX_PATH_LENGTH], *out;
	cJSON *root, *node, *metaJSON, *statsJSON;
	StageMeta *s;
	int i;

	root = cJSON_CreateObject();

	cJSON_AddNumberToObject(root, "stagesComplete", game.stagesComplete);

	metaJSON = cJSON_CreateArray();

	for (s = game.stageMetaHead.next ; s != NULL ; s = s->next)
	{
		node = cJSON_CreateObject();

		cJSON_AddNumberToObject(node, "stageNum", s->stageNum);
		cJSON_AddNumberToObject(node, "coinsFound", s->coinsFound);
		cJSON_AddNumberToObject(node, "itemsFound", s->itemsFound);

		cJSON_AddItemToArray(metaJSON, node);
	}

	cJSON_AddItemToObject(root, "meta", metaJSON);

	statsJSON = cJSON_CreateArray();

	for (i = 0 ; i < STAT_MAX ; i++)
	{
		node = cJSON_CreateObject();

		cJSON_AddStringToObject(node, "key", getLookupName("STAT_", i));
		cJSON_AddNumberToObject(node, "value", game.stats[i]);

		cJSON_AddItemToArray(statsJSON, node);
	}

	cJSON_AddItemToObject(root, "stats", statsJSON);

	sprintf(filename, "%s/%s", app.saveDir, SAVE_FILENAME);

	out = cJSON_Print(root);

	writeFile(filename, out);

	cJSON_Delete(root);

	free(out);
}

void loadConfig(void)
{
	char filename[MAX_PATH_LENGTH];

	sprintf(filename, "%s/%s", app.saveDir, CONFIG_FILENAME);

	if (fileExists(filename))
	{
		loadConfigFile(filename);
	}
	else
	{
		loadConfigFile(getFileLocation("data/config/config.json"));
	}
}

static void loadConfigFile(const char *filename)
{
	cJSON *root, *controls;
	char *json;

	json = readFile(filename);
	if (!json)
	{
		return;
	}

	root = cJSON_Parse(json);
	if (!root)
	{
		free(json);
		return;
	}

	app.config.soundVolume = getJSONIntVal(root, "soundVolume", 100);
	app.config.musicVolume = getJSONIntVal(root, "musicVolume", 100);
	app.config.winWidth = getJSONIntVal(root, "winWidth", SCREEN_WIDTH);
	app.config.winHeight = getJSONIntVal(root, "winHeight", SCREEN_HEIGHT);
	app.config.fullscreen = getJSONIntVal(root, "fullscreen", 0);
	app.config.tips = getJSONIntVal(root, "tips", 1);
	app.config.deadzone = getJSONIntVal(root, "deadzone", 64) * 256;

	controls = cJSON_GetObjectItem(root, "keyControls");
	if (controls != NULL)
	{
		app.config.keyControls[CONTROL_LEFT] = getJSONIntVal(controls, "left", SDL_SCANCODE_LEFT);
		app.config.keyControls[CONTROL_RIGHT] = getJSONIntVal(controls, "right", SDL_SCANCODE_RIGHT);
		app.config.keyControls[CONTROL_JUMP] = getJSONIntVal(controls, "jump", SDL_SCANCODE_Z);
		app.config.keyControls[CONTROL_USE] = getJSONIntVal(controls, "use", SDL_SCANCODE_X);
		app.config.keyControls[CONTROL_CLONE] = getJSONIntVal(controls, "clone", SDL_SCANCODE_C);
		app.config.keyControls[CONTROL_RESTART] = getJSONIntVal(controls, "restart", SDL_SCANCODE_R);
		app.config.keyControls[CONTROL_PAUSE] = getJSONIntVal(controls, "pause", SDL_SCANCODE_P);
	}

	controls = cJSON_GetObjectItem(root, "joypadControls");
	if (controls != NULL)
	{
		app.config.joypadControls[CONTROL_LEFT] = getJSONIntVal(controls, "left", 0);
		app.config.joypadControls[CONTROL_RIGHT] = getJSONIntVal(controls, "right", 0);
		app.config.joypadControls[CONTROL_JUMP] = getJSONIntVal(controls, "jump", 0);
		app.config.joypadControls[CONTROL_USE] = getJSONIntVal(controls, "use", 1);
		app.config.joypadControls[CONTROL_CLONE] = getJSONIntVal(controls, "clone", 2);
		app.config.joypadControls[CONTROL_RESTART] = getJSONIntVal(controls, "restart", 3);
		app.config.joypadControls[CONTROL_PAUSE] = getJSONIntVal(controls, "pause", 7);
	}

	cJSON_Delete(root);

	free(json);
}

void saveConfig(void)
{
	char filename[MAX_PATH_LENGTH], *out;
	cJSON *root, *controlsJSON;

	root = cJSON_CreateObject();

	cJSON_AddNumberToObject(root, "soundVolume", app.config.soundVolume);
	cJSON_AddNumberToObject(root, "musicVolume", app.config.musicVolume);
	cJSON_AddNumberToObject(root, "winWidth", app.config.winWidth);
	cJSON_AddNumberToObject(root, "winHeight", app.config.winHeight);
	cJSON_AddNumberToObject(root, "fullscreen", app.config.fullscreen);
	cJSON_AddNumberToObject(root, "tips", app.config.tips);
	cJSON_AddNumberToObject(root, "deadzone", app.config.deadzone / 256);

	controlsJSON = cJSON_CreateObject();

	cJSON_AddNumberToObject(controlsJSON, "left", app.config.keyControls[CONTROL_LEFT]);
	cJSON_AddNumberToObject(controlsJSON, "right", app.config.keyControls[CONTROL_RIGHT]);
	cJSON_AddNumberToObject(controlsJSON, "jump", app.config.keyControls[CONTROL_JUMP]);
	cJSON_AddNumberToObject(controlsJSON, "use", app.config.keyControls[CONTROL_USE]);
	cJSON_AddNumberToObject(controlsJSON, "clone", app.config.keyControls[CONTROL_CLONE]);
	cJSON_AddNumberToObject(controlsJSON, "restart", app.config.keyControls[CONTROL_RESTART]);
	cJSON_AddNumberToObject(controlsJSON, "pause", app.config.keyControls[CONTROL_PAUSE]);
	cJSON_AddItemToObject(root, "keyControls", controlsJSON);

	controlsJSON = cJSON_CreateObject();

	cJSON_AddNumberToObject(controlsJSON, "left", app.config.joypadControls[CONTROL_LEFT]);
	cJSON_AddNumberToObject(controlsJSON, "right", app.config.joypadControls[CONTROL_RIGHT]);
	cJSON_AddNumberToObject(controlsJSON, "jump", app.config.joypadControls[CONTROL_JUMP]);
	cJSON_AddNumberToObject(controlsJSON, "use", app.config.joypadControls[CONTROL_USE]);
	cJSON_AddNumberToObject(controlsJSON, "clone", app.config.joypadControls[CONTROL_CLONE]);
	cJSON_AddNumberToObject(controlsJSON, "restart", app.config.joypadControls[CONTROL_RESTART]);
	cJSON_AddNumberToObject(controlsJSON, "pause", app.config.joypadControls[CONTROL_PAUSE]);
	cJSON_AddItemToObject(root, "joypadControls", controlsJSON);

	sprintf(filename, "%s/%s", app.saveDir, CONFIG_FILENAME);

	out = cJSON_Print(root);

	writeFile(filename, out);

	cJSON_Delete(root);

	free(out);
}

