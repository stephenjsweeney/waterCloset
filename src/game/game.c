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

#include "game.h"

static void loadConfigFile(const char *filename);

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
		loadConfigFile("data/config/config.json");
	}
}

static void loadConfigFile(const char *filename)
{
	cJSON *root, *controls;
	char *json;
	
	json = readFile(filename);
		
	root = cJSON_Parse(json);
	
	app.config.soundVolume = cJSON_GetObjectItem(root, "soundVolume")->valueint;
	app.config.musicVolume = cJSON_GetObjectItem(root, "musicVolume")->valueint;
	app.config.winWidth = cJSON_GetObjectItem(root, "winWidth")->valueint;
	app.config.winHeight = cJSON_GetObjectItem(root, "winHeight")->valueint;
	app.config.fullscreen = cJSON_GetObjectItem(root, "fullscreen")->valueint;
	app.config.tips = cJSON_GetObjectItem(root, "tips")->valueint;
	
	controls = cJSON_GetObjectItem(root, "keyControls");
	
	app.config.keyControls[CONTROL_LEFT] = cJSON_GetObjectItem(controls, "left")->valueint;
	app.config.keyControls[CONTROL_RIGHT] = cJSON_GetObjectItem(controls, "right")->valueint;
	app.config.keyControls[CONTROL_JUMP] = cJSON_GetObjectItem(controls, "jump")->valueint;
	app.config.keyControls[CONTROL_USE] = cJSON_GetObjectItem(controls, "use")->valueint;
	app.config.keyControls[CONTROL_CLONE] = cJSON_GetObjectItem(controls, "clone")->valueint;
	app.config.keyControls[CONTROL_RESTART] = cJSON_GetObjectItem(controls, "restart")->valueint;
	app.config.keyControls[CONTROL_PAUSE] = cJSON_GetObjectItem(controls, "pause")->valueint;
	
	controls = cJSON_GetObjectItem(root, "joypadControls");
	
	app.config.joypadControls[CONTROL_LEFT] = cJSON_GetObjectItem(controls, "left")->valueint;
	app.config.joypadControls[CONTROL_RIGHT] = cJSON_GetObjectItem(controls, "right")->valueint;
	app.config.joypadControls[CONTROL_JUMP] = cJSON_GetObjectItem(controls, "jump")->valueint;
	app.config.joypadControls[CONTROL_USE] = cJSON_GetObjectItem(controls, "use")->valueint;
	app.config.joypadControls[CONTROL_CLONE] = cJSON_GetObjectItem(controls, "clone")->valueint;
	app.config.joypadControls[CONTROL_RESTART] = cJSON_GetObjectItem(controls, "restart")->valueint;
	app.config.joypadControls[CONTROL_PAUSE] = cJSON_GetObjectItem(controls, "pause")->valueint;
	
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
