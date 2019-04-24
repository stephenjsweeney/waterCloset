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

typedef struct Texture Texture;
typedef struct Entity Entity;
typedef struct InitFunc InitFunc;
typedef struct Particle Particle;
typedef struct CloneData CloneData;
typedef struct cJSON cJSON;
typedef struct StageMeta StageMeta;
typedef struct AtlasImage AtlasImage;
typedef struct Lookup Lookup;
typedef struct Widget Widget;

struct Texture {
	char name[MAX_NAME_LENGTH];
	SDL_Texture *texture;
	Texture *next;
};

struct Lookup {
	char name[MAX_NAME_LENGTH];
	long value;
	Lookup *next;
};

struct InitFunc {
	char id[MAX_NAME_LENGTH];
	void (*init)(Entity *e);
	InitFunc *next;
};

struct AtlasImage {
	char filename[MAX_DESCRIPTION_LENGTH];
	SDL_Texture *texture;
	SDL_Rect rect;
	AtlasImage *next;
};

struct Entity {
	unsigned long id;
	unsigned int type;
	char *typeName;
	char name[MAX_NAME_LENGTH];
	float x;
	float y;
	int w;
	int h;
	int facing;
	float dx;
	float dy;
	int health;
	int isOnGround;
	int background;
	void (*data);
	AtlasImage *atlasImage;
	void (*init)(void);
	void (*tick)(void);
	void (*touch)(Entity *other);
	void (*activate)(int active);
	void (*die)(void);
	void (*load)(cJSON *root);
	void (*save)(cJSON *root);
	long flags;
	Entity *riding;
	Entity *next;
};

typedef struct {
	int animTimer;
	int requiresPlunger;
	int frameNum;
} Toilet;

typedef struct {
	float sx;
	float sy;
	float ex;
	float ey;
	int speed;
	int pause;
	int pauseTimer;
	int enabled;
	float dx;
	float dy;
} Platform;

typedef struct {
	float sx;
	float sy;
	float ex;
	float ey;
	int open;
} Door;

typedef struct {
	float bobValue;
	float sparkleValue;
} Item;

typedef struct {
	int interval;
	int reload;
	int enabled;
} Spitter;

typedef struct {
	int on;
	char targetName[MAX_NAME_LENGTH];
} TrafficLight;

typedef struct {
	int weight;
	char targetName[MAX_NAME_LENGTH];
} PressurePlate;

struct CloneData {
	int frame;
	float dx;
	float dy;
	int action;
	CloneData *next;
};

typedef struct {
	int action;
	int equipment;
	int advanceData;
	CloneData *dataHead, *pData;
	CloneData data;
} Walter;

struct Particle {
	float x;
	float y;
	float dx;
	float dy;
	int life;
	AtlasImage *atlasImage;
	SDL_Color color;
	int weightless;
	Particle *next;
};

typedef struct {
	int num;
	int map[MAP_WIDTH][MAP_HEIGHT];
	AtlasImage *tiles[MAX_TILES];
	StageMeta *meta;
	Entity entityHead, *entityTail;
	Entity *player;
	Particle particleHead, *particleTail;
	unsigned int clones, cloneLimit;
	unsigned int time, timeLimit;
	int keys, totalKeys;
	int coins, totalCoins;
	int items, totalItems;
	int frame;
	int reset;
	int status;
	int nextStageTimer;
	char tips[MAX_TIPS][MAX_DESCRIPTION_LENGTH];
	CloneData cloneDataHead, *cloneDataTail;
	struct {
		int x;
		int y;
		int minX;
		int maxX;
	} camera;
} Stage;

struct StageMeta {
	int stageNum;
	int coins, coinsFound;
	int items, itemsFound;
	StageMeta *next;
};

typedef struct {
	int numStages;
	int stagesComplete;
	StageMeta stageMetaHead, *currentStageMeta;
	int numItemTextures;
	unsigned int stats[STAT_MAX];
} Game;

struct Widget {
	char name[MAX_NAME_LENGTH];
	char groupName[MAX_NAME_LENGTH];
	char text[MAX_NAME_LENGTH];
	int type;
	int x;
	int y;
	int w;
	int h;
	int value;
	int disabled;
	int visible;
	int numOptions;
	char **options;
	void (*action)(void);
	Widget *prev;
	Widget *next;
};

typedef struct {
	char saveDir[MAX_FILENAME_LENGTH];
	SDL_Renderer *renderer;
	SDL_Window *window;
	SDL_Texture *backBuffer;
	int keyboard[MAX_KEYBOARD_KEYS];
	int joypadButton[SDL_CONTROLLER_BUTTON_MAX];
	int joypadAxis[JOYPAD_AXIS_MAX];
	Texture texturesHead, *texturesTail;
	Widget widgetsHead, *widgetsTail, *selectedWidget;
	SDL_Joystick *joypad;
	int awaitingWidgetInput;
	int lastKeyPressed;
	int lastButtonPressed;
	struct {
		void (*logic)(void);
		void (*draw)(void);
	} delegate;
	struct {
		SDL_Color red;
		SDL_Color orange;
		SDL_Color yellow;
		SDL_Color green;
		SDL_Color blue;
		SDL_Color cyan;
		SDL_Color purple;
		SDL_Color white;
		SDL_Color black;
		SDL_Color lightGrey;
		SDL_Color darkGrey;
	} colors;
	struct Mouse {
		int x;
		int y;
		int buttons[MAX_MOUSE_BUTTONS];
	} mouse;
	struct Config {
		int winWidth;
		int winHeight;
		int soundVolume;
		int musicVolume;
		int fullscreen;
		int tips;
		int keyControls[CONTROL_MAX];
		int joypadControls[CONTROL_MAX];
	} config;
	struct {
		unsigned int wrap;
	} text;
	struct {
		int type;
		int value;
	} wipe;
} App;
