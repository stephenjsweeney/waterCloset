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

#include "sound.h"

static void loadSounds(void);
static void channelDone(int c);

static Mix_Chunk *sounds[SND_MAX];
static Mix_Music *music;
static char *musicFilenames[] = {
	"music/contemplation.ogg", "music/puzzle-1-a.mp3", "music/puzzle-1-b.mp3"
};
static int lastRandomMusic;
static int channelVolumes[CH_MAX];

void initSounds(void)
{
	int i;
	
	memset(sounds, 0, sizeof(Mix_Chunk*) * SND_MAX);
	
	music = NULL;
	
	lastRandomMusic = -1;
	
	loadSounds();
	
	for (i = 0 ; i < CH_MAX ; i++)
	{
		channelVolumes[i] = 0;
	}
	
	Mix_ChannelFinished(channelDone);
}

void loadMusic(char *filename)
{
	if (music != NULL)
	{
		Mix_HaltMusic();
		Mix_FreeMusic(music);
		music = NULL;
	}

	music = Mix_LoadMUS(filename);
}

void playMusic(int loop)
{
	Mix_PlayMusic(music, (loop) ? -1 : 0);
}

void playSound(int id, int channel)
{
	Mix_PlayChannel(channel, sounds[id], 0);
}

void playPositionalSound(int id, int channel, int srcX, int srcY, int destX, int destY)
{
	float distance, bearing, vol;

	distance = getDistance(destX, destY, srcX, srcY);

	if (distance <= SCREEN_WIDTH)
	{
		vol = 255;
		vol /= SCREEN_WIDTH;
		vol *= distance;
		
		if (vol >= channelVolumes[channel])
		{
			Mix_PlayChannel(channel, sounds[id], 0);

			if (distance >= SCREEN_WIDTH / 8)
			{
				bearing = 360 - getAngle(srcX, srcY, destX, destY);
				Mix_SetPosition(channel, (Sint16)bearing, (Uint8)vol);
			}
			else
			{
				Mix_SetDistance(channel, vol);
			}
		}
	}
}

void pauseSound(void)
{
	Mix_Pause(-1);
}

void resumeSound(void)
{
	Mix_Resume(-1);
}

static void channelDone(int c)
{
	channelVolumes[c] = 0;
}

static void loadSounds(void)
{
	sounds[SND_JUMP] = Mix_LoadWAV("sound/331381__qubodup__public-domain-jump-sound.ogg");
	sounds[SND_COIN] = Mix_LoadWAV("sound/135936__bradwesson__collectcoin.ogg");
	sounds[SND_FLUSH] = Mix_LoadWAV("sound/108413__kyle1katarn__toilet.ogg");
	sounds[SND_PLUNGER] = Mix_LoadWAV("sound/plunger.ogg");
	sounds[SND_KEY] = Mix_LoadWAV("sound/mortice_key_drop_on_concrete_floor.ogg");
	sounds[SND_DEATH] = Mix_LoadWAV("sound/death.ogg");
	sounds[SND_CLONE] = Mix_LoadWAV("sound/clone.ogg");
	sounds[SND_NUDGE] = Mix_LoadWAV("sound/nudge.ogg");
	sounds[SND_TELEPORT] = Mix_LoadWAV("sound/teleport.ogg");
	sounds[SND_WIPE] = Mix_LoadWAV("sound/wipe.ogg");
	sounds[SND_SPIT] = Mix_LoadWAV("sound/434479__dersuperanton__splatter.ogg");
	sounds[SND_SPIT_HIT] = Mix_LoadWAV("sound/446115__justinvoke__wet-splat.ogg");
	sounds[SND_MANHOLE_COVER] = Mix_LoadWAV("sound/429167__aropson__heavy-clang-1.ogg");
	sounds[SND_CLOCK] = Mix_LoadWAV("sound/clock.ogg");
	sounds[SND_EXPIRED] = Mix_LoadWAV("sound/expired.ogg");
	sounds[SND_NEGATIVE] = Mix_LoadWAV("sound/negative.ogg");
	sounds[SND_FANFARE] = Mix_LoadWAV("sound/449069__ricniclas__fanfare.ogg");
	sounds[SND_DOOR] = Mix_LoadWAV("sound/426770__cmilan__drawer-close.ogg");
	sounds[SND_TRAFFIC_LIGHT] = Mix_LoadWAV("sound/264446__kickhat__open-button-1.ogg");
	sounds[SND_FAIL] = Mix_LoadWAV("sound/fail.ogg");
	sounds[SND_ITEM] = Mix_LoadWAV("sound/item.ogg");
	sounds[SND_TIP] = Mix_LoadWAV("sound/tip.ogg");
	sounds[SND_PLUNGE] = Mix_LoadWAV("sound/plunge.ogg");
	sounds[SND_PRESSURE_PLATE] = Mix_LoadWAV("sound/245242__noirenex__beepping.ogg");
	sounds[SND_SPLASH] = Mix_LoadWAV("sound/398032__swordofkings128__splash.ogg");
	sounds[SND_DRIP] = Mix_LoadWAV("sound/25879__acclivity__drip1.ogg");
	sounds[SND_SQUIRT] = Mix_LoadWAV("sound/258047__jagadamba__water-spraying-from-a-bottle-02.mp3");
	sounds[SND_INFLATE] = Mix_LoadWAV("sound/110043__sandyrb__fart-005.ogg");
	sounds[SND_DEFLATE] = Mix_LoadWAV("sound/110051__sandyrb__fart-013.ogg");
}

void loadRandomStageMusic(void)
{
	int r;
	
	r = rand() % (sizeof(musicFilenames) / sizeof(char*));
	
	if (r != lastRandomMusic)
	{
		lastRandomMusic = r;
		
		loadMusic(musicFilenames[r]);
		
		playMusic(1);
	}
}
