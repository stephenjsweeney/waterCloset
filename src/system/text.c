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
#include "text.h"
#include <SDL2/SDL_ttf.h>
#include "../system/textures.h"
#include "../system/io.h"

#define FONT_SIZE            32
#define FONT_TEXTURE_SIZE    512
#define MAX_WORD_LENGTH      128

extern App app;

static void initFont(const char *filename);
static void drawWord(char *word, int *x, int *y, int startX);
static void drawTextLines(int x, int y, int size, int align);
static void drawTextLine(int x, int y, int size, int align, const char *line);
static int toHex(char c);

static char drawTextBuffer[MAX_LINE_LENGTH];
static float scale;
static int ignoreColors;
static SDL_Color prevColor;
static SDL_Rect glyphs[128];
static SDL_Texture *fontTexture;

void initFonts(void)
{
	initFont(getFileLocation("fonts/EnterCommand.ttf"));
}

static void initFont(const char *filename)
{
	TTF_Font *font;
	SDL_Surface *surface, *text;
	SDL_Rect dest;
	int i;
	char c[2];
	SDL_Rect *g;
	SDL_Color white = {255, 255, 255, 255};

	memset(&glyphs, 0, sizeof(SDL_Rect) * 128);

	font = TTF_OpenFont(filename, FONT_SIZE);

	surface = SDL_CreateRGBSurface(0, FONT_TEXTURE_SIZE, FONT_TEXTURE_SIZE, 32, 0, 0, 0, 0xff);

	SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGBA(surface->format, 0, 0, 0, 0));

	dest.x = dest.y = 0;

	for (i = ' ' ; i <= 'z' ; i++)
	{
		memset(c, 0, 2);

		sprintf(c, "%c", i);

		text = TTF_RenderUTF8_Blended(font, c, white);

		TTF_SizeText(font, c, &dest.w, &dest.h);

		if (dest.x + dest.w >= FONT_TEXTURE_SIZE)
		{
			dest.x = 0;

			dest.y += dest.h + 1;

			if (dest.y + dest.h >= FONT_TEXTURE_SIZE)
			{
				SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_CRITICAL, "Out of glyph space in %dx%d font atlas texture map.", FONT_TEXTURE_SIZE, FONT_TEXTURE_SIZE);
				exit(1);
			}
		}

		SDL_BlitSurface(text, NULL, surface, &dest);

		g = &glyphs[i];

		g->x = dest.x;
		g->y = dest.y;
		g->w = dest.w;
		g->h = dest.h;

		SDL_FreeSurface(text);

		dest.x += dest.w;
	}

	TTF_CloseFont(font);

	fontTexture = toTexture(surface, 1);
}

void drawText(int x, int y, int size, int align, SDL_Color color, const char *format, ...)
{
	va_list args;

	memset(&drawTextBuffer, '\0', sizeof(drawTextBuffer));

	va_start(args, format);
	vsprintf(drawTextBuffer, format, args);
	va_end(args);

	if (app.text.wrap == 0)
	{
		ignoreColors = 1;

		SDL_SetTextureColorMod(fontTexture, 0, 0, 0);
		SDL_SetTextureAlphaMod(fontTexture, 255);

		drawTextLine(x + 2, y + 2, size, align, drawTextBuffer);
		drawTextLine(x + 1, y + 1, size, align, drawTextBuffer);

		SDL_SetTextureColorMod(fontTexture, color.r, color.g, color.b);
		SDL_SetTextureAlphaMod(fontTexture, color.a);

		ignoreColors = 0;

		drawTextLine(x, y, size, align, drawTextBuffer);
	}
	else
	{
		ignoreColors = 1;

		SDL_SetTextureColorMod(fontTexture, 0, 0, 0);
		SDL_SetTextureAlphaMod(fontTexture, 255);

		drawTextLines(x + 2, y + 2, size, align);
		drawTextLines(x + 1, y + 1, size, align);

		SDL_SetTextureColorMod(fontTexture, color.r, color.g, color.b);
		SDL_SetTextureAlphaMod(fontTexture, color.a);

		ignoreColors = 0;

		drawTextLines(x, y, size, align);
	}
}

static void drawTextLines(int x, int y, int size, int align)
{
	char line[MAX_LINE_LENGTH], token[MAX_WORD_LENGTH];
	int i, n, w, h, currentWidth, len;

	memset(&line, '\0', sizeof(line));
	memset(&token, '\0', sizeof(token));

	len = strlen(drawTextBuffer);

	n = currentWidth = 0;

	for (i = 0 ; i < len ; i++)
	{
		token[n++] = drawTextBuffer[i];

		if (drawTextBuffer[i] == ' ' || i == len - 1)
		{
			calcTextDimensions(token, size, &w, &h);

			if (currentWidth + w > app.text.wrap)
			{
				drawTextLine(x, y, size, align, line);

				currentWidth = 0;

				y += h * 1.2;

				memset(&line, '\0', sizeof(line));
			}

			strcat(line, token);

			n = 0;

			memset(&token, '\0', sizeof(token));

			currentWidth += w;
		}
	}

	drawTextLine(x, y, size, align, line);
}

static void drawTextLine(int x, int y, int size, int align, const char *line)
{
	int i, startX, n, w, h;
	char word[MAX_WORD_LENGTH];

	scale = size / (FONT_SIZE * 1.0f);

	startX = x;

	memset(word, 0, MAX_WORD_LENGTH);

	n = 0;

	calcTextDimensions(line, size, &w, &h);

	if (align == TEXT_RIGHT)
	{
		x -= w;
	}
	else if (align == TEXT_CENTER)
	{
		x -= (w / 2);
	}

	for (i = 0 ; i < strlen(line) ; i++)
	{
		word[n++] = line[i];

		if (line[i] == ' ')
		{
			drawWord(word, &x, &y, startX);

			memset(word, 0, MAX_WORD_LENGTH);

			n = 0;
		}
	}

	drawWord(word, &x, &y, startX);
}

static void drawWord(char *word, int *x, int *y, int startX)
{
	int i;
	int character;
	SDL_Rect dest, *g;
	SDL_Color c;

	i = 0;

	if (word[0] == '#')
	{
		if (!ignoreColors)
		{
			if (word[1] == '!')
			{
				SDL_SetTextureColorMod(fontTexture, prevColor.r, prevColor.g, prevColor.b);

				return;
			}

			c.r = toHex(word[1]);
			c.g = toHex(word[2]);
			c.b = toHex(word[3]);

			SDL_GetTextureColorMod(fontTexture, &prevColor.r, &prevColor.g, &prevColor.b);

			SDL_SetTextureColorMod(fontTexture, c.r, c.g, c.b);
		}

		return;
	}

	character = word[i++];

	while (character)
	{
		g = &glyphs[character];

		dest.x = *x;
		dest.y = *y;
		dest.w = g->w * scale;
		dest.h = g->h * scale;

		SDL_RenderCopy(app.renderer, fontTexture, g, &dest);

		*x += g->w * scale;

		character = word[i++];
	}
}

static int toHex(char c)
{
	int result;

	if (c >= '0' && c <= '9')
	{
		result = c - '0';
	}
	else if (c >= 'a' && c <= 'f')
	{
		result = 11 + (c - 'a');
	}
	else
	{
		result = 0;
	}

	return MIN(result * 16, 255);
}

void calcTextDimensions(const char *text, int size, int *w, int *h)
{
	float scale;
	int i, character;
	SDL_Rect *g;

	scale = size / (FONT_SIZE * 1.0f);

	*w = 0;
	*h = 0;

	i = 0;

	character = text[i++];

	while (character)
	{
		g = &glyphs[character];

		*w += g->w * scale;
		*h = MAX(g->h * scale, *h);

		character = text[i++];
	}
}

