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

#define NUM_VISIBLE_STAGES  7

extern void blitAtlasImage(AtlasImage *atlasImage, int x, int y, int center, SDL_RendererFlip flip);
extern void clearAcceptControls(void);
extern void destroyStage(void);
extern void drawOutlineRect(int x, int y, int w, int h, int r, int g, int b, int a);
extern void drawRect(int x, int y, int w, int h, int r, int g, int b, int a);
extern void drawText(int x, int y, int size, int align, SDL_Color color, const char *format, ...);
extern AtlasImage *getAtlasImage(char *filename, int required);
extern void initStage(void);
extern int isAcceptControl(void);
extern int isControl(int type);
extern void loadRandomStageMusic(void);
extern void loadStage(int randomTiles);
extern void playSound(int snd, int ch);
extern void showWidgets(const char *groupName, int visible);

extern App app;
extern Game game;
extern Stage stage;
