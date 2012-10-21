#pragma once
#include "font.h"
#include "FontFace.h"
#include "Glyph.h"
#include "glext.h"

class TextureFont :
	public Font
{
	FontFace Face;
	int pt;
	int TexWidth;
	int TexHeight;	
	GLubyte *TexMemory;
	Glyph *TexGlyphs[255];
	GLuint TexID;
	GLuint vbo[128];
	void InitTexture();
	void InitVBOExtensions();
	GLubyte *PrepareGlyph(FT_Bitmap bitmap,int width,int height);
	void CreateVBO(int ch);
	void DrawVBO(int ch);
public:
	TextureFont(void);
	TextureFont(char *fontlocation,int pointsize);
	~TextureFont(void);
	int LoadFont(char* fontlocation,int pointsize);
	void RenderString(float x, float y, char * string);
};
