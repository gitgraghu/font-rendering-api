#pragma once
#include "font.h"
#include "FontFace.h"
#include "Glyph.h"

class TextureFont :
	public Font
{
	FontFace Face;
	int pt;
	int TexWidth;
	int TexHeight;	
	GLubyte *TexMemory;
	Glyph *TexGlyphs[256];
public:
	GLuint TexID;
	TextureFont(void);
	TextureFont(char *fontlocation,int pointsize);
	~TextureFont(void);
	int LoadFont(char* fontlocation,int pointsize);
	int LoadBMFont(char* fontlocation); 
	void InitTexture();
	void RenderString(float x, float y, char * string);
	void RenderString(float x, float y, float width, float height, char *string);
	void DrawGlyph(int ch);
	GLubyte *PrepareGlyph(FT_Bitmap bitmap,int width,int height);
};
