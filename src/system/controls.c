/*
Copyright (C) 2015-2017 Parallel Realities

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

#include "controls.h"

int isControl(int type)
{
	int key, btn;
	
	key = app.config.keyControls[type];
	btn = app.config.joypadControls[type];
	
	if (type == CONTROL_LEFT && app.joypadAxis[JOYPAD_AXIS_X] <= -AXIS_MAX / 2)
	{
		return 1;
	}
	
	if (type == CONTROL_RIGHT && app.joypadAxis[JOYPAD_AXIS_X] >= AXIS_MAX / 2)
	{
		return 1;
	}
	
	if (type == CONTROL_UP && app.joypadAxis[JOYPAD_AXIS_Y] <= -AXIS_MAX / 2)
	{
		return 1;
	}
	
	if (type == CONTROL_DOWN && app.joypadAxis[JOYPAD_AXIS_Y] >= AXIS_MAX / 2)
	{
		return 1;
	}
	
	return ((key != 0 && app.keyboard[key]) || (btn != -1 && app.joypadButton[btn]));
}

int isAcceptControl(void)
{
	return (app.keyboard[SDL_SCANCODE_SPACE] ||app.keyboard[SDL_SCANCODE_RETURN] || isControl(CONTROL_USE));
}

void updateControlKey(const char *name)
{
	app.config.keyControls[lookup(name)] = app.lastKeyPressed;
}

void updateControlButton(const char *name)
{
	app.config.joypadControls[lookup(name)] = app.lastButtonPressed;
}

void clearControl(int type)
{
	int key;
	int btn;
	
	key = app.config.keyControls[type];
	btn = app.config.joypadControls[type];
	
	if (key != 0)
	{
		app.keyboard[key] = 0;
	}
	
	if (btn != 0)
	{
		app.joypadButton[btn] = 0;
	}
	
	if (type == CONTROL_LEFT || type == CONTROL_RIGHT)
	{
		app.joypadAxis[JOYPAD_AXIS_X] = 0;
	}
	
	if (type == CONTROL_UP || type == CONTROL_DOWN)
	{
		app.joypadAxis[JOYPAD_AXIS_Y] = 0;
	}
}

void clearAcceptControls(void)
{
	clearControl(CONTROL_USE);
	
	app.keyboard[SDL_SCANCODE_SPACE] = app.keyboard[SDL_SCANCODE_RETURN] = 0;
}

void clearControls(void)
{
	memset(&app.keyboard, 0, sizeof(int) * MAX_KEYBOARD_KEYS);
	memset(&app.joypadButton, 0, sizeof(int) * SDL_CONTROLLER_BUTTON_MAX);
}
