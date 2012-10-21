#pragma once

class Font
{
public:
	Font(void);
	~Font(void);
	virtual int LoadFont(char *fontlocation,int pointsize)=0;
};
