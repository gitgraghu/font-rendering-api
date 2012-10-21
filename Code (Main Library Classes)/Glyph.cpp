#include "Glyph.h"

Glyph::Glyph(int xoff, int yoff, int w, int h, int bearingY, int xadv)
{
  XOff = xoff;
  YOff = yoff;
  Width = w;
  Height = h;
  HoriBearing = bearingY;
  Advance = xadv;
}

Glyph::~Glyph(void)
{
}

int Glyph::getXOffset()
{
 return XOff;
}
int Glyph::getYOffset()
{
 return YOff;
}
int Glyph::getWidth()
{
 return Width;
}
int Glyph::getHeight()
{
 return Height;
}
int Glyph::getHoriBearing()
{
 return HoriBearing;
}
int Glyph::getAdvance()
{
 return Advance;
}


