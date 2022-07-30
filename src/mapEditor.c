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

#include "common.h"
#include "mapEditor.h"
#include "json/cJSON.h"
#include "world/quadtree.h"
#include "system/atlas.h"
#include "system/util.h"
#include "system/input.h"
#include "world/map.h"
#include "system/init.h"
#include "world/stage.h"
#include "system/text.h"
#include "system/io.h"
#include "world/entities.h"
#include "system/draw.h"
#include "world/entityFactory.h"

enum
{
	MODE_TILE,
	MODE_ENT,
	MODE_PICK
};

App app;
Entity *player;
Entity *self;
Game game;
Stage stage;

static void capFrameRate(long *then, float *remainder);

static int tile;
static int cameraTimer;
static Entity **entities;
static int numEnts;
static int entIndex;
static Entity *entity;
static Entity *selectedEntity;
static int mode;

static void saveMap(cJSON *root)
{
	int x, y;
	unsigned long l;
	FILE *fp;
	char *buff;

	fp = open_memstream(&buff, &l);

	for (y = 0 ; y < MAP_HEIGHT ; y++)
	{
		for (x = 0 ; x < MAP_WIDTH ; x++)
		{
			fprintf(fp, "%d ", stage.map[x][y]);
		}
	}

	fclose(fp);

	cJSON_AddStringToObject(root, "map", buff);

	free(buff);
}

static void saveEntities(cJSON *root)
{
	Entity *e;
	cJSON *entityJSON, *entitiesJSON;

	entitiesJSON = cJSON_CreateArray();

	for (e = stage.entityHead.next ; e != NULL ; e = e->next)
	{
		self = e;

		entityJSON = cJSON_CreateObject();

		cJSON_AddStringToObject(entityJSON, "type", e->typeName);
		cJSON_AddNumberToObject(entityJSON, "x", e->x);
		cJSON_AddNumberToObject(entityJSON, "y", e->y);

		if (strlen(e->name) > 0)
		{
			cJSON_AddStringToObject(entityJSON, "name", e->name);
		}

		if (e->save)
		{
			e->save(entityJSON);
		}

		cJSON_AddItemToArray(entitiesJSON, entityJSON);
	}

	cJSON_AddItemToObject(root, "entities", entitiesJSON);
}

static void saveTips(cJSON *root)
{
	cJSON *tipsJSON;
	int i;

	tipsJSON = cJSON_CreateArray();

	for (i = 0 ; i < MAX_TIPS ; i++)
	{
		if (strlen(stage.tips[i]) > 0)
		{
			cJSON_AddItemToArray(tipsJSON, cJSON_CreateString(stage.tips[i]));
		}
	}

	cJSON_AddItemToObject(root, "tips", tipsJSON);
}

static void saveStage(void)
{
	char filename[MAX_FILENAME_LENGTH], *out;
	cJSON *root;

	sprintf(filename, "data/stages/%03d.json", stage.num);

	printf("Saving %s ...\n", filename);

	root = cJSON_CreateObject();

	cJSON_AddNumberToObject(root, "cloneLimit", stage.cloneLimit);
	cJSON_AddNumberToObject(root, "timeLimit", stage.timeLimit);

	saveEntities(root);

	saveTips(root);

	saveMap(root);

	out = cJSON_Print(root);

	writeFile(filename, out);

	cJSON_Delete(root);

	free(out);

	printf("Saved %s\n", filename);
}

static void createEntity(void)
{
	Entity *e;
	int x, y;

	x = (app.mouse.x / 8) * 8;
	y = (app.mouse.y / 8) * 8;

	x += stage.camera.x;
	y += stage.camera.y;

	e = spawnEditorEntity(entity->typeName, x, y);

	addToQuadtree(e, &stage.quadtree);
}

static void deleteEntity(void)
{
	Entity *e, *prev;

	prev = &stage.entityHead;

	for (e = stage.entityHead.next ; e != NULL ; e = e->next)
	{
		if (collision(app.mouse.x + stage.camera.x, app.mouse.y + stage.camera.y, 1, 1, e->x, e->y, e->w, e->h))
		{
			if (e == stage.entityTail)
			{
				stage.entityTail = prev;
			}

			prev->next = e->next;

			removeFromQuadtree(e, &stage.quadtree);

			/* loaded, so safe to delete */
			if (e->id != -1)
			{
				free(e->data);
			}

			free(e);

			e = prev;
		}

		prev = e;
	}
}

static void cycleTile(int dir)
{
	int ok;

	do
	{
		tile += dir;

		if (tile < 0)
		{
			tile = MAX_TILES - 1;
		}
		else if (tile >= MAX_TILES)
		{
			tile = 0;
		}

		ok = stage.tiles[tile] != NULL;
	}
	while (!ok);
}

static void cycleEnt(int dir)
{
	entIndex += dir;

	if (entIndex < 0)
	{
		entIndex = numEnts - 1;
	}
	else if (entIndex >= numEnts)
	{
		entIndex = 0;
	}

	entity = entities[entIndex];
}

static void toggleSelectEntity(void)
{
	Entity *e;
	Platform *p;

	if (selectedEntity == NULL)
	{
		for (e = stage.entityHead.next ; e != NULL ; e = e->next)
		{
			if (collision(app.mouse.x + stage.camera.x, app.mouse.y + stage.camera.y, 1, 1, e->x, e->y, e->w, e->h))
			{
				selectedEntity = e;
				return;
			}
		}
	}
	else
	{
		removeFromQuadtree(selectedEntity, &stage.quadtree);

		selectedEntity->x = ((app.mouse.x / 8) * 8) + stage.camera.x;
		selectedEntity->y = ((app.mouse.y / 8) * 8) + stage.camera.y;

		addToQuadtree(selectedEntity, &stage.quadtree);

		if (strcmp(selectedEntity->typeName, "platform") == 0)
		{
			p = (Platform*)selectedEntity->data;

			p->sx = selectedEntity->x;
			p->sy = selectedEntity->y;
		}

		selectedEntity = NULL;
	}
}

static void flipSelectedEntity(void)
{
	Entity *e;

	if (selectedEntity != NULL)
	{
		selectedEntity->facing = !selectedEntity->facing;
	}
	else
	{
		for (e = stage.entityHead.next ; e != NULL ; e = e->next)
		{
			if (collision(app.mouse.x + stage.camera.x, app.mouse.y + stage.camera.y, 1, 1, e->x, e->y, e->w, e->h))
			{
				e->facing = !e->facing;
				return;
			}
		}
	}
}

static void logic(void)
{
	int x, y;

	if (app.mouse.buttons[SDL_BUTTON_LEFT])
	{
		switch (mode)
		{
			case MODE_TILE:
				x = (app.mouse.x + stage.camera.x) / TILE_SIZE;
				y = (app.mouse.y + stage.camera.y) / TILE_SIZE;
				stage.map[x][y] = tile;
				break;

			case MODE_ENT:
				app.mouse.buttons[SDL_BUTTON_LEFT] = 0;
				createEntity();
				break;

			case MODE_PICK:
				app.mouse.buttons[SDL_BUTTON_LEFT] = 0;
				toggleSelectEntity();
				break;
		}
	}

	if (app.mouse.buttons[SDL_BUTTON_RIGHT])
	{
		switch (mode)
		{
			case MODE_TILE:
				x = (app.mouse.x + stage.camera.x) / TILE_SIZE;
				y = (app.mouse.y + stage.camera.y) / TILE_SIZE;
				stage.map[x][y] = 0;
				break;

			case MODE_ENT:
				deleteEntity();
				break;

			case MODE_PICK:
				app.mouse.buttons[SDL_BUTTON_RIGHT] = 0;
				flipSelectedEntity();
				break;
		}
	}

	if (app.mouse.buttons[SDL_BUTTON_X1])
	{
		app.mouse.buttons[SDL_BUTTON_X1] = 0;

		if (mode == MODE_TILE)
		{
			cycleTile(1);
		}
		else if (mode == MODE_ENT)
		{
			cycleEnt(1);
		}
	}

	if (app.mouse.buttons[SDL_BUTTON_X2])
	{
		app.mouse.buttons[SDL_BUTTON_X2] = 0;

		if (mode == MODE_TILE)
		{
			cycleTile(-1);
		}
		else if (mode == MODE_ENT)
		{
			cycleEnt(-1);
		}
	}

	if (app.keyboard[SDL_SCANCODE_SPACE])
	{
		app.keyboard[SDL_SCANCODE_SPACE] = 0;

		saveStage();
	}

	if (app.keyboard[SDL_SCANCODE_1])
	{
		app.keyboard[SDL_SCANCODE_1] = 0;

		mode = MODE_TILE;
	}

	if (app.keyboard[SDL_SCANCODE_2])
	{
		app.keyboard[SDL_SCANCODE_2] = 0;

		mode = MODE_ENT;
	}

	if (app.keyboard[SDL_SCANCODE_3])
	{
		app.keyboard[SDL_SCANCODE_3] = 0;

		mode = MODE_PICK;
	}

	if (--cameraTimer <= 0)
	{
		cameraTimer = 3;

		if (app.keyboard[SDL_SCANCODE_W])
		{
			stage.camera.y -= TILE_SIZE;
		}

		if (app.keyboard[SDL_SCANCODE_S])
		{
			stage.camera.y += TILE_SIZE;
		}

		if (app.keyboard[SDL_SCANCODE_A])
		{
			stage.camera.x -= TILE_SIZE;
		}

		if (app.keyboard[SDL_SCANCODE_D])
		{
			stage.camera.x += TILE_SIZE;
		}

		/* use 64, so things don't look wonky on the right-hand side */
		stage.camera.x = MIN(MAX(stage.camera.x, 0), (MAP_WIDTH * TILE_SIZE) - SCREEN_WIDTH + (TILE_SIZE - 64));
		stage.camera.y = MIN(MAX(stage.camera.y, 0), (MAP_HEIGHT * TILE_SIZE) - SCREEN_HEIGHT);
	}
}

static void drawCurrentTile(void)
{
	int x, y;
	SDL_Rect r;

	x = (app.mouse.x / TILE_SIZE) * TILE_SIZE;
	y = (app.mouse.y / TILE_SIZE) * TILE_SIZE;

	blitAtlasImage(stage.tiles[tile], x, y, 0, SDL_FLIP_NONE);

	r.x = x;
	r.y = y;
	r.w = TILE_SIZE;
	r.h = TILE_SIZE;

	SDL_SetRenderDrawColor(app.renderer, 255, 255, 0, 255);
	SDL_RenderDrawRect(app.renderer, &r);
}

static void drawCurrentEnt(void)
{
	int x, y;

	x = (app.mouse.x / 8) * 8;
	y = (app.mouse.y / 8) * 8;

	blitAtlasImage(entity->atlasImage, x, y, 0, SDL_FLIP_NONE);
}

static void drawSelectedEnt(void)
{
	int x, y;

	if (selectedEntity != NULL)
	{
		x = (app.mouse.x / 8) * 8;
		y = (app.mouse.y / 8) * 8;

		removeFromQuadtree(selectedEntity, &stage.quadtree);

		selectedEntity->x = x + stage.camera.x;
		selectedEntity->y = y + stage.camera.y;

		addToQuadtree(selectedEntity, &stage.quadtree);
	}
}

static void drawInfo(void)
{
	Entity *e;
	int x, y;

	x = ((app.mouse.x + stage.camera.x) / TILE_SIZE) * TILE_SIZE;
	y = ((app.mouse.y + stage.camera.y) / TILE_SIZE) * TILE_SIZE;

	drawRect(0, 0, SCREEN_WIDTH, 30, 0, 0, 0, 192);

	drawText(10, 0, 32, TEXT_LEFT, app.colors.white, "Stage: %d", stage.num);

	drawText(310, 0, 32, TEXT_LEFT, app.colors.white, "Pos: %d,%d", x, y);

	if (mode == MODE_PICK)
	{
		for (e = stage.entityHead.next ; e != NULL ; e = e->next)
		{
			if (collision(app.mouse.x + stage.camera.x, app.mouse.y + stage.camera.y, 1, 1, e->x, e->y, e->w, e->h))
			{
				drawText(e->x + (e->w / 2) - stage.camera.x, e->y - 32 - stage.camera.y, 32, TEXT_CENTER, app.colors.white, "%d,%d", (int)e->x, (int)e->y);
			}
		}
	}
}

static void draw(void)
{
	drawMap();

	drawEntities(0);
	drawEntities(1);

	switch (mode)
	{
		case MODE_TILE:
			drawCurrentTile();
			break;

		case MODE_ENT:
			drawCurrentEnt();
			break;

		case MODE_PICK:
			drawSelectedEnt();
			break;
	}

	drawInfo();
}

static void tryLoadStage(void)
{
	Entity *e;
	char filename[MAX_FILENAME_LENGTH];

	sprintf(filename, "data/stages/%03d.json", stage.num);

	stage.timeLimit = 3600;

	if (fileExists(filename))
	{
		loadStage(0);

		for (e = stage.entityHead.next ; e != NULL ; e = e->next)
		{
			addToQuadtree(e, &stage.quadtree);
		}
	}
}

/* defaults */
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

static void centreOnPlayer(void)
{
	Entity *e;

	for (e = stage.entityHead.next ; e != NULL ; e = e->next)
	{
		if (e->type == ET_PLAYER)
		{
			stage.camera.x = e->x;
			stage.camera.y = e->y;

			stage.camera.x -= SCREEN_WIDTH / 2;
			stage.camera.y -= SCREEN_HEIGHT / 2;
		}

		e->flags &= ~EF_INVISIBLE;
	}
}

static void handleCommandLine(int argc, char *argv[])
{
	int i;

	for (i = 1 ; i < argc ; i++)
	{
		if (strcmp(argv[i], "-stage") == 0)
		{
			stage.num = atoi(argv[i + 1]);
		}
	}
}

int main(int argc, char *argv[])
{
	long then;
	float remainder;

	memset(&app, 0, sizeof(App));
	app.texturesTail = &app.texturesHead;

	tile = 1;
	cameraTimer = 0;
	mode = MODE_TILE;
	entIndex = 0;
	selectedEntity = NULL;

	initSDL();

	atexit(cleanup);

	SDL_ShowCursor(1);

	initGame();

	memset(&stage, 0, sizeof(Stage));
	stage.entityTail = &stage.entityHead;

	handleCommandLine(argc, argv);

	entities = initAllEnts(&numEnts);
	entity = entities[0];

	loadTiles();

	tryLoadStage();

	centreOnPlayer();

	then = SDL_GetTicks();

	remainder = 0;

	while (1)
	{
		prepareScene();

		doInput();

		logic();

		draw();

		presentScene();

		capFrameRate(&then, &remainder);
	}

	return 0;
}

static void capFrameRate(long *then, float *remainder)
{
	long wait, frameTime;

	wait = 16 + *remainder;

	*remainder -= (int)*remainder;

	frameTime = SDL_GetTicks() - *then;

	wait -= frameTime;

	if (wait < 1)
	{
		wait = 1;
	}

	SDL_Delay(wait);

	*remainder += 0.667;

	*then = SDL_GetTicks();
}

