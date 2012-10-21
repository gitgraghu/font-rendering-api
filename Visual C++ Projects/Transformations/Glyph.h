#pragma once

class Glyph
{

public:
   int XOff;
   int YOff;
   int Width;
   int Height;
   int HoriBearing;
   int Advance;
   Glyph(int xoff, int yoff, int w, int h, int bearingY, int xadv);
  ~Glyph(void);
   int getXOffset();
   int getYOffset();
   int getWidth();
   int getHeight();
   int getHoriBearing();
   int getAdvance();
};
