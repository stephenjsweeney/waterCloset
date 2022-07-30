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

#define PI 3.14159265358979323846
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define CAROLINE(a,b) (((a)<(b))?(a):(b))
#define STRNCPY(dest, src, n) strncpy(dest, src, n); dest[n - 1] = '\0'

#define SCREEN_WIDTH   1280
#define SCREEN_HEIGHT  720

#define CONFIG_FILENAME    "config.json"
#define SAVE_FILENAME      "game.json"

#define FPS   60

#define MAX_TILES    255

#define TILE_SIZE    48

#define MAP_WIDTH    108
#define MAP_HEIGHT   15

#define MAP_RENDER_WIDTH    27
#define MAP_RENDER_HEIGHT   15

#define PLAYER_MOVE_SPEED 6

#define MAX_TIPS    12

#define MAX_QT_CANDIDATES   128

#define MAX_NAME_LENGTH           32
#define MAX_DESCRIPTION_LENGTH    256
#define MAX_LINE_LENGTH           1024
#define MAX_FILENAME_LENGTH       256
#define MAX_PATH_LENGTH           4096

#define MAX_KEYBOARD_KEYS   350
#define MAX_MOUSE_BUTTONS   6

#define NUM_ATLAS_BUCKETS 32

#define EF_NONE            0
#define EF_WEIGHTLESS      (2 << 0)
#define EF_SOLID           (2 << 1)
#define EF_PUSH            (2 << 2)
#define EF_NO_WORLD_CLIP   (2 << 3)
#define EF_NO_MAP_BOUNDS   (2 << 4)
#define EF_PUSHABLE        (2 << 5)
#define EF_SLOW_PUSH       (2 << 6)
#define EF_NO_ENT_CLIP     (2 << 7)
#define EF_INVISIBLE       (2 << 8)
#define EF_STATIC          (2 << 9)

enum
{
	ET_PLAYER,
	ET_CLONE,
	ET_TOILET,
	ET_ITEM,
	ET_STRUCTURE,
	ET_BULLET,
	ET_TRAP,
	ET_SWITCH,
	ET_VOMIT_TOILET,
	ET_DECORATION
};

enum
{
	EQ_NONE,
	EQ_MANHOLE_COVER,
	EQ_PLUNGER,
	EQ_WATER_PISTOL
};

enum
{
	WIPE_FADE,
	WIPE_IN,
	WIPE_OUT
};

enum
{
	WT_BUTTON,
	WT_SELECT,
	WT_INPUT
};

enum
{
	TEXT_LEFT,
	TEXT_CENTER,
	TEXT_RIGHT
};

enum
{
	FACING_LEFT,
	FACING_RIGHT
};

enum
{
	SND_JUMP,
	SND_COIN,
	SND_FLUSH,
	SND_PLUNGER,
	SND_KEY,
	SND_DEATH,
	SND_CLONE,
	SND_NUDGE,
	SND_TELEPORT,
	SND_WIPE,
	SND_SPIT,
	SND_SPIT_HIT,
	SND_MANHOLE_COVER,
	SND_CLOCK,
	SND_EXPIRED,
	SND_NEGATIVE,
	SND_FANFARE,
	SND_DOOR,
	SND_TRAFFIC_LIGHT,
	SND_ITEM,
	SND_FAIL,
	SND_TIP,
	SND_PLUNGE,
	SND_PRESSURE_PLATE,
	SND_SPLASH,
	SND_DRIP,
	SND_SQUIRT,
	SND_INFLATE,
	SND_DEFLATE,
	SND_MAX
};

enum
{
	CH_PLAYER,
	CH_CLONE,
	CH_COIN,
	CH_ITEM,
	CH_SPIT,
	CH_SHOOT,
	CH_HIT,
	CH_CLOCK,
	CH_SWITCH,
	CH_STRUCTURE,
	CH_WIDGET,
	CH_MAX
};

enum
{
	SS_INCOMPLETE,
	SS_COMPLETE,
	SS_FAILED,
	SS_GAME_COMPLETE
};

enum
{
	JOYPAD_AXIS_X,
	JOYPAD_AXIS_Y,
	JOYPAD_AXIS_MAX
};

enum
{
	CONTROL_LEFT,
	CONTROL_RIGHT,
	CONTROL_UP,
	CONTROL_DOWN,
	CONTROL_JUMP,
	CONTROL_USE,
	CONTROL_CLONE,
	CONTROL_RESTART,
	CONTROL_PAUSE,
	CONTROL_MAX
};

enum
{
	STAT_PERCENT_COMPLETE,
	STAT_STAGES_STARTED,
	STAT_STAGES_COMPLETED,
	STAT_FAILS,
	STAT_DEATHS,
	STAT_CLONES,
	STAT_CLONE_DEATHS,
	STAT_KEYS,
	STAT_PLUNGERS,
	STAT_MANHOLE_COVERS,
	STAT_WATER_PISTOLS,
	STAT_ITEMS,
	STAT_COINS,
	STAT_JUMPS,
	STAT_MOVED,
	STAT_FALLEN,
	STAT_SHOTS_FIRED,
	STAT_TIME,
	STAT_MAX
};
