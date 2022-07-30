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
#include "sound.h"
#include <SDL2/SDL_mixer.h>
#include "../system/util.h"
#include "../system/io.h"

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

	music = Mix_LoadMUS(getFileLocation(filename));
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

static Mix_Chunk *loadSound(char *filename)
{
	return Mix_LoadWAV(getFileLocation(filename));
}

static void loadSounds(void)
{
	sounds[SND_JUMP] = loadSound("sound/331381__qubodup__public-domain-jump-sound.ogg");
	sounds[SND_COIN] = loadSound("sound/135936__bradwesson__collectcoin.ogg");
	sounds[SND_FLUSH] = loadSound("sound/108413__kyle1katarn__toilet.ogg");
	sounds[SND_PLUNGER] = loadSound("sound/plunger.ogg");
	sounds[SND_KEY] = loadSound("sound/mortice_key_drop_on_concrete_floor.ogg");
	sounds[SND_DEATH] = loadSound("sound/death.ogg");
	sounds[SND_CLONE] = loadSound("sound/clone.ogg");
	sounds[SND_NUDGE] = loadSound("sound/nudge.ogg");
	sounds[SND_TELEPORT] = loadSound("sound/teleport.ogg");
	sounds[SND_WIPE] = loadSound("sound/wipe.ogg");
	sounds[SND_SPIT] = loadSound("sound/434479__dersuperanton__splatter.ogg");
	sounds[SND_SPIT_HIT] = loadSound("sound/446115__justinvoke__wet-splat.ogg");
	sounds[SND_MANHOLE_COVER] = loadSound("sound/429167__aropson__heavy-clang-1.ogg");
	sounds[SND_CLOCK] = loadSound("sound/clock.ogg");
	sounds[SND_EXPIRED] = loadSound("sound/expired.ogg");
	sounds[SND_NEGATIVE] = loadSound("sound/negative.ogg");
	sounds[SND_FANFARE] = loadSound("sound/449069__ricniclas__fanfare.ogg");
	sounds[SND_DOOR] = loadSound("sound/426770__cmilan__drawer-close.ogg");
	sounds[SND_TRAFFIC_LIGHT] = loadSound("sound/264446__kickhat__open-button-1.ogg");
	sounds[SND_FAIL] = loadSound("sound/fail.ogg");
	sounds[SND_ITEM] = loadSound("sound/item.ogg");
	sounds[SND_TIP] = loadSound("sound/tip.ogg");
	sounds[SND_PLUNGE] = loadSound("sound/plunge.ogg");
	sounds[SND_PRESSURE_PLATE] = loadSound("sound/245242__noirenex__beepping.ogg");
	sounds[SND_SPLASH] = loadSound("sound/398032__swordofkings128__splash.ogg");
	sounds[SND_DRIP] = loadSound("sound/25879__acclivity__drip1.ogg");
	sounds[SND_SQUIRT] = loadSound("sound/258047__jagadamba__water-spraying-from-a-bottle-02.mp3");
	sounds[SND_INFLATE] = loadSound("sound/110043__sandyrb__fart-005.ogg");
	sounds[SND_DEFLATE] = loadSound("sound/110051__sandyrb__fart-013.ogg");
}

void loadRandomStageMusic(int forceRandom)
{
	int r;

	if (forceRandom)
	{
		lastRandomMusic = -1;
	}

	r = rand() % (sizeof(musicFilenames) / sizeof(char*));

	if (r != lastRandomMusic)
	{
		lastRandomMusic = r;

		loadMusic(musicFilenames[r]);

		playMusic(1);
	}
}

void destroySounds(void)
{
	int i;

	for (i = 0 ; i < SND_MAX ; i++)
	{
		if (sounds[i])
		{
			Mix_FreeChunk(sounds[i]);
		}
	}

	if (music != NULL)
	{
		Mix_FreeMusic(music);
	}
}

