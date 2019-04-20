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

#include "../common.h"
#include "../json/cJSON.h"

#include <time.h>

#define SHOW_GAME  0
#define SHOW_MENU  1

extern void blitAtlasImage(AtlasImage *atlasImage, int x, int y, int center, SDL_RendererFlip flip);
extern void destroyEntities(void);
extern void destroyParticles(void);
extern void doCamera(void);
extern void doEntities(void);
extern void doParticles(void);
extern int doWipe(void);
extern void drawEntities(int background);
extern void drawMap(void);
extern void drawOutlineRect(int x, int y, int w, int h, int r, int g, int b, int a);
extern void drawParticles(void);
extern void drawRect(int x, int y, int w, int h, int r, int g, int b, int a);
extern void drawText(int x, int y, int size, int align, SDL_Color color, const char *format, ...);
extern void drawWipe(void);
extern AtlasImage *getAtlasImage(char *filename, int required);
extern void initClone(void);
extern void initEntities(cJSON *root);
extern void initMap(cJSON *root);
extern void initWipe(int type);
extern void loadRandomStageMusic(void);
extern void playSound(int snd, int ch);
extern void randomizeTiles(void);
extern char *readFile(const char *filename);
extern void resetClones(void);
extern void resetEntities(void);
extern void doWidgets(const char *groupName);
extern void drawWidgets(const char *groupName);
extern void drawWidgetFrame(void);
extern void calculateWidgetFrame(const char *groupName);
extern void showWidgets(const char *groupName, int visible);
extern void initTitle(void);
extern void initOptions(void (*done)(void));
extern Widget *getWidget(const char *name, const char *groupName);
extern int isAcceptControl(void);
extern int isControl(int type);
extern void clearControl(int type);
extern void clearAcceptControls(void);

extern App app;
extern Stage stage;
