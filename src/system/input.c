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
#include "input.h"

extern App app;

void doKeyUp(SDL_KeyboardEvent *event)
{
	if (event->repeat == 0 && event->keysym.scancode < MAX_KEYBOARD_KEYS)
	{
		app.keyboard[event->keysym.scancode] = 0;
	}
}

void doKeyDown(SDL_KeyboardEvent *event)
{
	if (event->repeat == 0 && event->keysym.scancode < MAX_KEYBOARD_KEYS)
	{
		app.keyboard[event->keysym.scancode] = 1;

		app.lastKeyPressed = event->keysym.scancode;
	}
}

static void doMouseDown(SDL_MouseButtonEvent *event)
{
	if (event->button >= 0 && event->button < MAX_MOUSE_BUTTONS)
	{
		app.mouse.buttons[event->button] = 1;
	}
}

static void doMouseUp(SDL_MouseButtonEvent *event)
{
	if (event->button >= 0 && event->button < MAX_MOUSE_BUTTONS)
	{
		app.mouse.buttons[event->button] = 0;
	}
}

/*
 * Note: the following assumes that SDL_BUTTON_X1 and SDL_BUTTON_X2 are 4 and 5, respectively. They usually are.
 */
static void doMouseWheel(SDL_MouseWheelEvent *event)
{
	if (event->y == -1)
	{
		app.mouse.buttons[SDL_BUTTON_X1] = 1;
	}

	if (event->y == 1)
	{
		app.mouse.buttons[SDL_BUTTON_X2] = 1;
	}
}

static void doButtonDown(SDL_JoyButtonEvent *event)
{
	if (event->state == SDL_PRESSED)
	{
		app.joypadButton[event->button] = 1;

		app.lastButtonPressed = event->button;
	}
}

static void doButtonUp(SDL_JoyButtonEvent *event)
{
	if (event->state == SDL_RELEASED)
	{
		app.joypadButton[event->button] = 0;
	}
}

static void doJoyAxis(SDL_JoyAxisEvent *event)
{
	if (event->axis < JOYPAD_AXIS_MAX)
	{
		app.joypadAxis[event->axis] = event->value;
	}
}

void doInput(void)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_MOUSEWHEEL:
				doMouseWheel(&event.wheel);
				break;

			case SDL_MOUSEBUTTONDOWN:
				doMouseDown(&event.button);
				break;

			case SDL_MOUSEBUTTONUP:
				doMouseUp(&event.button);
				break;

			case SDL_KEYDOWN:
				doKeyDown(&event.key);
				break;

			case SDL_KEYUP:
				doKeyUp(&event.key);
				break;

			case SDL_JOYBUTTONDOWN:
				doButtonDown(&event.jbutton);
				break;

			case SDL_JOYBUTTONUP:
				doButtonUp(&event.jbutton);
				break;

			case SDL_JOYAXISMOTION:
				doJoyAxis(&event.jaxis);
				break;

			case SDL_QUIT:
				exit(0);
				break;

			default:
				break;
		}
	}

	SDL_GetMouseState(&app.mouse.x, &app.mouse.y);
}

