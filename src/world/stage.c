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

#include "stage.h"

static void logic(void);
static void draw(void);
static void drawBackground(void);
static void drawHud(void);
static void resetCloneData(void);
static void resetStage(void);
static void nextStage(int num);
static void doControls(void);
static void doTimeLimit(void);
static void initTips(cJSON *root);
static void doTips(void);
static void drawTips(void);
static void doGame(void);
static void doMenu(void);
static void drawGame(void);
static void drawMenu(void);
static void resume(void);
static void restart(void);
static void options(void);
static void quit(void);

static cJSON *stageJSON;
static int cloneWarning;
static int showTips;
static int tipIndex;
static int numTips;
static int show;
static AtlasImage *backgroundTile;
static AtlasImage *tipsPrompt;
static Widget *resumeWidget;
static Widget *restartWidget;
static Widget *optionsWidget;
static Widget *quitWidget;

void initStage(void)
{
	app.delegate.logic = logic;
	app.delegate.draw = draw;
	
	memset(&stage, 0, sizeof(Stage));
	
	stage.entityTail = &stage.entityHead;
	stage.particleTail = &stage.particleHead;
	stage.cloneDataTail = &stage.cloneDataHead;
	
	resumeWidget = getWidget("resume", "stage");
	resumeWidget->action = resume;
	
	restartWidget = getWidget("restart", "stage");
	restartWidget->action = restart;
	
	optionsWidget = getWidget("options", "stage");
	optionsWidget->action = options;
	
	quitWidget = getWidget("quit", "stage");
	quitWidget->action = quit;
	
	backgroundTile = getAtlasImage("gfx/tilesets/brick/0.png", 1);
	
	tipsPrompt = getAtlasImage("gfx/main/tips.png", 1);
	
	initWipe(WT_WIPE_IN);
	
	playSound(SND_WIPE, CH_PLAYER);
}

void loadStage(int randomTiles)
{
	cJSON *root;
	char *json;
	char filename[MAX_FILENAME_LENGTH];
	
	srand(256 * stage.num);
	
	sprintf(filename, "data/stages/%03d.json", stage.num);
	
	json = readFile(filename);
		
	root = cJSON_Parse(json);
	
	stage.cloneLimit = cJSON_GetObjectItem(root, "cloneLimit")->valueint;
	stage.timeLimit = cJSON_GetObjectItem(root, "timeLimit")->valueint;
	
	stage.time = (stage.timeLimit * FPS);
	
	show = SHOW_GAME;
	
	cloneWarning = 0;
	
	initMap(root);
	
	initEntities(root);
	
	initTips(root);
	
	if (randomTiles)
	{
		randomizeTiles();
	}
	
	free(json);
	
	stageJSON = root;
}

static void logic(void)
{
	if (doWipe())
	{
		switch (show)
		{
			case SHOW_MENU:
				doMenu();
				break;
				
			default:
				doGame();
				break;
		}
	}
	
	doCamera();
}

static void doGame(void)
{
	if (!showTips)
	{
		doControls();
		
		doEntities();
		
		doParticles();
		
		stage.frame++;
		
		if (stage.status == SS_COMPLETE)
		{
			stage.nextStageTimer--;
			
			if (stage.nextStageTimer == 0)
			{
				initWipe(WT_WIPE_OUT);
				
				playSound(SND_WIPE, CH_PLAYER);
			}
			else if (stage.nextStageTimer < 0)
			{
				nextStage(stage.num + 1);
			}
		}
		
		if (stage.reset)
		{
			resetStage();
			
			initWipe(WT_FADE_IN);
		}
		
		if (stage.status == SS_INCOMPLETE && stage.time > 0)
		{
			doTimeLimit();
		}
		
		cloneWarning = MAX(cloneWarning - 1, 0);
	}
	else
	{
		doTips();
	}
}

static void doMenu(void)
{
	doWidgets("stage");
	
	if (app.keyboard[SDL_SCANCODE_ESCAPE])
	{
		app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
		
		show = SHOW_GAME;
	}
}

static void doTimeLimit(void)
{
	int then;
	
	then = stage.time;
	
	stage.time--;
	
	/* 10 seconds remaining */
	if (stage.time <= (FPS * 11))
	{
		if (then / FPS != stage.time / FPS)
		{
			playSound(SND_CLOCK, CH_CLOCK);
			
			if (stage.time / FPS == 0)
			{
				playSound(SND_EXPIRED, CH_CLOCK);
				
				stage.status = SS_FAILED;
			}
		}
	}
}

static void doControls(void)
{
	if (stage.status == SS_INCOMPLETE)
	{
		if (app.keyboard[SDL_SCANCODE_SPACE])
		{
			app.keyboard[SDL_SCANCODE_SPACE] = 0;
			
			if (stage.clones < stage.cloneLimit)
			{
				initClone();
				
				stage.clones++;
				
				stage.reset = 1;
				
				playSound(SND_CLONE, -1);
			}
			else if (stage.cloneLimit > 0)
			{
				cloneWarning = FPS * 1.5;
				
				playSound(SND_NEGATIVE, CH_PLAYER);
			}
		}
		else if (app.keyboard[SDL_SCANCODE_F1])
		{
			app.keyboard[SDL_SCANCODE_F1] = 0;
			
			showTips = 1;
			
			tipIndex = 0;
			
			playSound(SND_TIP, CH_PLAYER);
		}
	}
	
	if (stage.status != SS_COMPLETE && app.keyboard[SDL_SCANCODE_BACKSPACE])
	{
		app.keyboard[SDL_SCANCODE_BACKSPACE] = 0;
		
		nextStage(stage.num);
	}
	
	if (app.keyboard[SDL_SCANCODE_ESCAPE])
	{
		app.keyboard[SDL_SCANCODE_ESCAPE] = 0;
		
		show = SHOW_MENU;
		
		showWidgets("stage", 1);
	
		calculateWidgetFrame("stage");
		
		app.selectedWidget = resumeWidget;
		
		playSound(SND_TIP, CH_WIDGET);
	}
}

static void doTips(void)
{
	if (app.keyboard[SDL_SCANCODE_RETURN])
	{
		app.keyboard[SDL_SCANCODE_RETURN] = 0;
		
		showTips = ++tipIndex < numTips;
		
		playSound(SND_TIP, CH_PLAYER);
	}
}

static void resetStage(void)
{
	stage.reset = 0;
	
	stage.keys = stage.totalKeys = 0;
	
	stage.items = stage.totalItems = 0;
	
	stage.coins = stage.totalCoins = 0;
	
	srand(256 * stage.num);
	
	resetCloneData();
	
	resetEntities();
	
	initEntities(stageJSON);
	
	resetClones();
}

static void draw(void)
{
	switch (show)
	{
		case SHOW_MENU:
			drawMenu();
			break;
		
		default:
			drawGame();
			break;
	}
	
	drawWipe();
}

static void drawGame()
{
	drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 64, 64, 64, 64);
	
	drawBackground();
	
	drawEntities(1);
	
	drawMap();
	
	drawEntities(0);
	
	drawParticles();
	
	drawHud();
	
	if (showTips)
	{
		drawTips();
	}
}

static void drawMenu()
{
	drawGame();
	
	drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0, 96);
	
	drawWidgetFrame();
	
	drawWidgets("stage");
}

static void drawBackground(void)
{
	int x, y, n;
	
	for (x = 0 ; x < MAP_RENDER_WIDTH ; x++)
	{
		for (y = 0; y < MAP_RENDER_HEIGHT ; y++)
		{
			n = ((x ^ y) / 3) % 4;
			
			if (n == 3)
			{
				blitAtlasImage(backgroundTile, x * TILE_SIZE, y * TILE_SIZE, 0, SDL_FLIP_NONE);
			}
		}
	}
}

static void drawTips(void)
{
	SDL_Rect r;
	
	r.w = 500;
	r.h = 300;
	r.x = (SCREEN_WIDTH - r.w) / 2;
	r.y = (SCREEN_HEIGHT - r.h) / 2;
	
	drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 0, 64);
	
	drawRect(r.x, r.y, r.w, r.h, 0, 0, 0, 192);
	drawOutlineRect(r.x, r.y, r.w, r.h, 192, 192, 192, 255);
	
	app.text.wrap = r.w - 25;
	
	drawText(r.x + 10, r.y, 32, TEXT_LEFT, app.colors.white, stage.tips[tipIndex]);
	
	drawText(r.x + 10, r.y + r.h - 32, 32, TEXT_LEFT, app.colors.white, "Press [Return] to continue");
	
	drawText(r.x + r.w - 10, r.y + r.h - 32, 32, TEXT_RIGHT, app.colors.white, "Tip %d / %d", tipIndex + 1, numTips);
}

static void drawHud(void)
{
	int m, s;
	
	drawRect(0, 0, SCREEN_WIDTH, 30, 0, 0, 0, 192);
	
	drawText(10, 0, 32, TEXT_LEFT, app.colors.white, "Stage: %03d", stage.num);
	
	if (numTips > 0)
	{
		blitAtlasImage(tipsPrompt, 135, 16, 1, SDL_FLIP_NONE);
	}
	
	if (cloneWarning > 0 && cloneWarning % 20 < 10)
	{
		drawText(256, 0, 32, TEXT_LEFT, app.colors.red, "Clones: %d / %d", stage.clones, stage.cloneLimit);
	}
	else
	{
		drawText(256, 0, 32, TEXT_LEFT, stage.cloneLimit > 0 ? app.colors.white : app.colors.darkGrey, "Clones: %d / %d", stage.clones, stage.cloneLimit);
	}
	
	drawText(512, 0, 32, TEXT_CENTER, stage.totalKeys > 0 ? app.colors.white : app.colors.darkGrey, "Keys: %d", stage.keys);
	
	drawText(768, 0, 32, TEXT_CENTER, stage.totalCoins > 0 ? app.colors.white : app.colors.darkGrey, "Coins: %d / %d", stage.coins, stage.totalCoins);
	
	drawText(1024, 0, 32, TEXT_CENTER, stage.totalItems > 0 ? app.colors.white : app.colors.darkGrey, "Items: %d / %d", stage.items, stage.totalItems);
	
	s = (stage.time / 60) % 60;
	m = stage.time / 3600;
	
	if (m > 0 || s > 10 || stage.time % 30 < 15)
	{
		drawText(SCREEN_WIDTH - 10, 0, 32, TEXT_RIGHT, app.colors.white, "Time: %02d:%02d", m, s);
	}
	else
	{
		drawText(SCREEN_WIDTH - 10, 0, 32, TEXT_RIGHT, app.colors.red, "Time: %02d:%02d", m, s);
	}
	
	if (stage.status == SS_FAILED)
	{
		drawRect(0, SCREEN_HEIGHT - 30, SCREEN_WIDTH, 30, 0, 0, 0, 192);
		
		drawText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 32, 32, TEXT_CENTER, app.colors.white, "Press backspace to retry");
	}
}

static void initTips(cJSON *root)
{
	cJSON *tip;
	
	memset(stage.tips, '\0', MAX_TIPS * MAX_DESCRIPTION_LENGTH);
	
	numTips = 0;
	
	tipIndex = 0;
	
	for (tip = cJSON_GetObjectItem(root, "tips")->child ; tip != NULL ; tip = tip->next)
	{
		STRNCPY(stage.tips[numTips], tip->valuestring, MAX_DESCRIPTION_LENGTH);
		
		numTips++;
	}
	
	showTips = numTips > 0 && app.config.tips;
}

static void resetCloneData(void)
{
	stage.frame = 0;
	
	stage.cloneDataTail = &stage.cloneDataHead;
}

static void destroyCloneData(void)
{
	CloneData *cd;
	
	while (stage.cloneDataHead.next)
	{
		cd = stage.cloneDataHead.next;
		stage.cloneDataHead.next = cd->next;
		free(cd);
	}
}

static void nextStage(int num)
{
	int sameStage;
	
	sameStage = stage.num == num;
	
	destroyEntities();
	
	destroyParticles();
	
	destroyCloneData();
	
	cJSON_Delete(stageJSON);
	
	initStage();
	
	stage.num = num;
	
	loadStage(1);
	
	if (sameStage)
	{
		showTips = 0;
	}
	else if (rand() % 4 == 0)
	{
		loadRandomStageMusic();
	}
}

static void resume(void)
{
	show = SHOW_GAME;
}

static void restart(void)
{
	show = SHOW_GAME;
	
	nextStage(stage.num);
}

static void returnFromOptions(void)
{
	showWidgets("stage", 1);
	
	calculateWidgetFrame("stage");
	
	app.selectedWidget = optionsWidget;
	
	app.delegate.logic = logic;
	app.delegate.draw = draw;
}

static void options(void)
{
	showWidgets("stage", 0);
	
	initOptions(returnFromOptions);
}

static void quit(void)
{
	initTitle();
}
