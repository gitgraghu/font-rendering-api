#pragma once

#include "ft2build.h"
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>

class FontFace
{
public:
	FT_Library library;
	FT_Face face;
	FontFace();
   ~FontFace(void);
	int LoadFace(char* fontlocation,int pointsize);
	int getAscent();
	int getDescent();
	int getLineGap();
	int getHoriBearing();
	int getAdvance();
	FT_Bitmap LoadGlyphBitmap(int charcode);
};
