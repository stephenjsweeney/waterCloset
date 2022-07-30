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
#include "lookup.h"

static Lookup head;
static Lookup *tail;

static void addLookup(const char *name, unsigned long value);

void initLookups(void)
{
	memset(&head, 0, sizeof(Lookup));
	tail = &head;

	addLookup("WT_BUTTON", WT_BUTTON);
	addLookup("WT_SELECT", WT_SELECT);
	addLookup("WT_INPUT", WT_INPUT);

	addLookup("STAT_PERCENT_COMPLETE", STAT_PERCENT_COMPLETE);
	addLookup("STAT_STAGES_STARTED", STAT_STAGES_STARTED);
	addLookup("STAT_STAGES_COMPLETED", STAT_STAGES_COMPLETED);
	addLookup("STAT_FAILS", STAT_FAILS);
	addLookup("STAT_DEATHS", STAT_DEATHS);
	addLookup("STAT_CLONES", STAT_CLONES);
	addLookup("STAT_CLONE_DEATHS", STAT_CLONE_DEATHS);
	addLookup("STAT_KEYS", STAT_KEYS);
	addLookup("STAT_PLUNGERS", STAT_PLUNGERS);
	addLookup("STAT_MANHOLE_COVERS", STAT_MANHOLE_COVERS);
	addLookup("STAT_WATER_PISTOLS", STAT_WATER_PISTOLS);
	addLookup("STAT_ITEMS", STAT_ITEMS);
	addLookup("STAT_COINS", STAT_COINS);
	addLookup("STAT_JUMPS", STAT_JUMPS);
	addLookup("STAT_MOVED", STAT_MOVED);
	addLookup("STAT_SHOTS_FIRED", STAT_SHOTS_FIRED);
	addLookup("STAT_FALLEN", STAT_FALLEN);
	addLookup("STAT_TIME", STAT_TIME);

	addLookup("left", CONTROL_LEFT);
	addLookup("right", CONTROL_RIGHT);
	addLookup("jump", CONTROL_JUMP);
	addLookup("use", CONTROL_USE);
	addLookup("clone", CONTROL_CLONE);
	addLookup("restart", CONTROL_RESTART);
	addLookup("pause", CONTROL_PAUSE);
}

static void addLookup(const char *name, unsigned long value)
{
	Lookup *lookup = malloc(sizeof(Lookup));
	memset(lookup, 0, sizeof(Lookup));

	STRNCPY(lookup->name, name, MAX_NAME_LENGTH);
	lookup->value = value;

	tail->next = lookup;
	tail = lookup;
}

char *getLookupName(char *prefix, long num)
{
	Lookup *l;

	for (l = head.next ; l != NULL ; l = l->next)
	{
		if (l->value == num && strncmp(prefix, l->name, strlen(prefix)) == 0)
		{
			return l->name;
		}
	}

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "No such lookup value %ld, prefix=%s", num, prefix);

	exit(1);

	return "";
}

unsigned long lookup(const char *name)
{
	Lookup *l;

	for (l = head.next ; l != NULL ; l = l->next)
	{
		if (strcmp(l->name, name) == 0)
		{
			return l->value;
		}
	}

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "No such lookup value '%s'", name);
	exit(1);

	return 0;
}

