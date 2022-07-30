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
#include "meta.h"
#include "../json/cJSON.h"
#include "../system/io.h"

extern Game game;

static void countCoinsItems(void);

void initStageMetaData(void)
{
	countCoinsItems();
}

StageMeta *getStageMeta(int n)
{
	StageMeta *s;

	for (s = game.stageMetaHead.next ; s != NULL ; s = s->next)
	{
		if (s->stageNum == n)
		{
			return s;
		}
	}

	if (n != 0 && n != 999)
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, "No meta data for stage %d", n);
		exit(1);
	}

	return NULL;
}

static void countCoinsItems(void)
{
	char filename[MAX_FILENAME_LENGTH], *json, *type;
	int n, exists;
	cJSON *root, *node;
	StageMeta *s, *tail;

	tail = &game.stageMetaHead;

	n = 1;

	do
	{
		sprintf(filename, "data/stages/%03d.json", n);

		exists = fileExists(filename);

		if (!exists)
		{
			sprintf(filename, DATA_DIR"/data/stages/%03d.json", n);

			exists = fileExists(filename);
		}

		if (exists)
		{
			json = readFile(filename);

			root = cJSON_Parse(json);

			s = malloc(sizeof(StageMeta));
			memset(s, 0, sizeof(StageMeta));
			tail->next = s;
			tail = s;

			s->stageNum = n;

			for (node = cJSON_GetObjectItem(root, "entities")->child ; node != NULL ; node = node->next)
			{
				type = cJSON_GetObjectItem(node, "type")->valuestring;

				if (strcmp(type, "coin") == 0)
				{
					s->coins++;
				}
				else if (strcmp(type, "item") == 0)
				{
					s->items++;
				}
			}

			free(json);

			cJSON_Delete(root);

			n++;

			game.numStages++;
		}

	} while (exists);
}

