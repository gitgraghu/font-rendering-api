#include "FontFace.h"

FontFace::FontFace()
{
}

FontFace::~FontFace(void)
{
	if(face){FT_Done_Face(face);face = NULL;}
	if(library){FT_Done_FreeType(library);library=NULL;}
}
int FontFace::LoadFace(char *fontlocation,int pointsize)
{   
	if (FT_Init_FreeType(&library)){return FALSE;}
	if (FT_New_Face(library,fontlocation,0,&face)){FT_Done_FreeType(library);return FALSE;}
	if (FT_Set_Char_Size(face,pointsize << 6,pointsize << 6, 96, 96)){FT_Done_FreeType(library);FT_Done_Face(face);return FALSE;}	
	return TRUE;
}
int FontFace::getAscent()
{
	return face->size->metrics.ascender >> 6;
}
int FontFace::getDescent()
{
	return face->size->metrics.descender >> 6;
}
int FontFace::getLineGap()
{
	return (face->size->metrics.height >> 6) - getAscent() - getDescent();
}
int FontFace::getHoriBearing()
{
    return face->glyph->metrics.horiBearingY>>6;
}
int FontFace::getAdvance()
{
	return face->glyph->advance.x>>6;
}
FT_Bitmap FontFace::LoadGlyphBitmap(int charcode)
{
	FT_Bitmap bitmap;
	FT_Glyph glyph;
	FT_BitmapGlyph bitmap_glyph;

	if (FT_Load_Glyph(face,FT_Get_Char_Index(face,charcode),FT_LOAD_MONOCHROME)){return bitmap;}
	if (FT_Get_Glyph(face->glyph,&glyph)){return bitmap;}
	FT_Glyph_To_Bitmap(&glyph,ft_render_mode_normal,0,1);
	bitmap_glyph=(FT_BitmapGlyph)glyph;  
	bitmap=bitmap_glyph->bitmap;

	return bitmap;
}