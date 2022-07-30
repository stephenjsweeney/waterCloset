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

void destroySounds(void);
void loadRandomStageMusic(int forceRandom);
void resumeSound(void);
void pauseSound(void);
void playPositionalSound(int id, int channel, int srcX, int srcY, int destX, int destY);
void playSound(int id, int channel);
void playMusic(int loop);
void loadMusic(char *filename);
void initSounds(void);
