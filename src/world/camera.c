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
#include "camera.h"

extern Stage stage;

void doCamera(void)
{
	stage.camera.x = (int) stage.player->x + (stage.player->w / 2);
	stage.camera.y = (int) stage.player->y + (stage.player->h / 2);

	stage.camera.x -= (SCREEN_WIDTH / 2);
	stage.camera.y -= (SCREEN_HEIGHT / 2);

	stage.camera.x = MIN(MAX(stage.camera.x, stage.camera.minX), stage.camera.maxX - SCREEN_WIDTH + (TILE_SIZE - 64));
	stage.camera.y = MIN(MAX(stage.camera.y, 0), (MAP_HEIGHT * TILE_SIZE) - SCREEN_HEIGHT);
}

