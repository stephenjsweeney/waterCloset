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
#include "map.h"
#include "../json/cJSON.h"
#include "../system/atlas.h"
#include "../system/draw.h"

extern Stage stage;

static void loadTiles(void);
static void loadMap(cJSON *root);

void initMap(cJSON *root)
{
	memset(&stage.map, 0, sizeof(int) * MAP_WIDTH * MAP_HEIGHT);

	loadTiles();

	loadMap(root);
}

void drawMap(void)
{
	int x, y, n, x1, x2, y1, y2, mx, my;

	x1 = (stage.camera.x % TILE_SIZE) * -1;
	x2 = x1 + MAP_RENDER_WIDTH * TILE_SIZE + (x1 == 0 ? 0 : TILE_SIZE);

	y1 = (stage.camera.y % TILE_SIZE) * -1;
	y2 = y1 + MAP_RENDER_HEIGHT * TILE_SIZE + (y1 == 0 ? 0 : TILE_SIZE);

	mx = stage.camera.x / TILE_SIZE;
	my = stage.camera.y / TILE_SIZE;

	for (y = y1 ; y < y2 ; y += TILE_SIZE)
	{
		for (x = x1 ; x < x2 ; x += TILE_SIZE)
		{
			if (isInsideMap(mx, my))
			{
				n = stage.map[mx][my];

				if (n > 0)
				{
					blitAtlasImage(stage.tiles[n], x, y, 0, SDL_FLIP_NONE);
				}
			}

			mx++;
		}

		mx = stage.camera.x / TILE_SIZE;

		my++;
	}
}

static void loadTiles(void)
{
	int i;
	char filename[MAX_FILENAME_LENGTH];

	for (i = 1 ; i <= MAX_TILES ; i++)
	{
		sprintf(filename, "gfx/tilesets/brick/%d.png", i);

		stage.tiles[i] = getAtlasImage(filename, 0);
	}
}

static void loadMap(cJSON *root)
{
	char *data, *p;
	int x, y;

	data = cJSON_GetObjectItem(root, "map")->valuestring;

	if (data)
	{
		p = data;

		for (y = 0 ; y < MAP_HEIGHT ; y++)
		{
			for (x = 0 ; x < MAP_WIDTH ; x++)
			{
				stage.map[x][y] = atoi(p);

				do {p++;} while (*p != ' ');
			}
		}
	}

	stage.camera.minX = MAP_WIDTH;
	stage.camera.maxX = 0;

	for (y = 0 ; y < MAP_HEIGHT ; y++)
	{
		for (x = 0 ; x < MAP_WIDTH ; x++)
		{
			if (stage.map[x][y] != 0)
			{
				stage.camera.maxX = MAX(stage.camera.maxX, x + 1);
				stage.camera.minX = MIN(stage.camera.minX, x);
			}
		}
	}

	stage.camera.minX *= TILE_SIZE;
	stage.camera.maxX *= TILE_SIZE;
}

void randomizeTiles(void)
{
	int x, y;

	for (y = 0 ; y < MAP_HEIGHT ; y++)
	{
		for (x = 0 ; x < MAP_WIDTH ; x++)
		{
			if (stage.map[x][y] == 1)
			{
				stage.map[x][y] += rand() % 4;
			}
		}
	}
}

int isInsideMap(int x, int y)
{
	return x >= 0 && y >= 0 && x < MAP_WIDTH && y < MAP_HEIGHT;
}

