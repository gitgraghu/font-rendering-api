#include "TextureFont.h"
#include "Glyph.h"
#include "tinyxml.h"
#include "glpng.h"

int nextPowerOf2(int n)			
{
	int i = 1;
	while (i < n)
	{i*= 2;}
	return i;
}

TextureFont::TextureFont(void)
{
}

TextureFont::~TextureFont(void)
{
}
TextureFont::TextureFont(char *fontlocation,int pointsize)
{
 LoadFont(fontlocation,pointsize);
}

void TextureFont::InitTexture()
{
	TexMemory = new GLubyte[TexWidth*TexHeight*2];
	memset(TexMemory,0,TexWidth*TexHeight*2);

	glGenTextures(1,&TexID);
	glBindTexture(GL_TEXTURE_2D,TexID);
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D,0,1,TexHeight,TexWidth,0,GL_LUMINANCE_ALPHA,GL_UNSIGNED_BYTE,TexMemory);
}

GLubyte *TextureFont::PrepareGlyph(FT_Bitmap bitmap,int width,int height)
{
		int val=0,k=0;
		GLubyte *glyphdata;
		glyphdata = new GLubyte[width*height*2];

        for(int r=0;r<height;r++)
	   {for(int c=0;c<width;c++)
	   {	val=(c>=bitmap.width||r>=bitmap.rows)?0:bitmap.buffer[c+bitmap.width*r]; 			
			glyphdata[k*2]=val;
			glyphdata[k*2+1]=val;
			k++;
		}}
		
		return glyphdata;
}

int TextureFont::LoadFont(char *fontlocation,int pointsize)
{
	if(!Face.LoadFace(fontlocation,pointsize)){return FALSE;}  //Load Face from fontfile with reqd pointsize	
	pt=pointsize; TexWidth= pt*24; TexHeight= pt*24;
	int penX=0,penY=pt*2;
	InitTexture();						//Initialize Texture Memory and Texture Parameters
 
	for(int i=32;i<127;i++)               //Run through all valid characters
	{ 	
		FT_Bitmap bitmap = Face.LoadGlyphBitmap(i);    //Load Bitmap of Character with charcode 'i'

		int width = bitmap.width;width = nextPowerOf2(width);  //Get Width of bitmap glyph
		int height = bitmap.rows;height = nextPowerOf2(height); //Get Height of bitmap glyph
	
		if(penX+Face.getAdvance() >=(TexWidth-1))  //Check boundary conditions
		{penY=penY+Face.getAscent()+10;penX=0;}		//Go to next line
										
		if(penY+height>=TexHeight-1)	//Check boundary conditions
		return TRUE;					//Max no of characters filled so end

		GLubyte *glyphdata = PrepareGlyph(bitmap,width,height);
		glTexSubImage2D(GL_TEXTURE_2D,0,penX  ,penY - Face.getHoriBearing(),width,height,GL_LUMINANCE_ALPHA,GL_UNSIGNED_BYTE,glyphdata);
		delete[] glyphdata;
			
		TexGlyphs[i] = new Glyph(penX ,penY - Face.getHoriBearing(),bitmap.width,bitmap.rows,Face.getHoriBearing(),Face.getAdvance());
  					
		penX=penX+Face.getAdvance()+4;
	             }

	return TRUE;
}

int TextureFont::LoadBMFont(char *fontlocation)
{
	 TiXmlDocument XMLdoc(fontlocation);		
     bool loadOkay = XMLdoc.LoadFile();			//Load the XML doc of the font
	 if (loadOkay)
	 {
		TiXmlElement *pRoot, *pInfo, *pFile, *pChars, *pChar;
		pRoot = XMLdoc.FirstChildElement("font");
		if (pRoot)
		{ 
             pInfo = pRoot->FirstChildElement("info");
			 pt=atoi(pInfo->Attribute("size"));			//Point Size
			 pInfo = pInfo->NextSiblingElement("common");
			  
			 TexWidth=atoi(pInfo->Attribute("scaleW"));	//Texture Width
			 TexHeight=atoi(pInfo->Attribute("scaleH"));//Texture Height	
			 InitTexture();								//Initialize Texture

			 pInfo=pInfo->NextSiblingElement("pages");
			 pFile=pInfo->FirstChildElement("page");

			 const char* img;
			 img = pFile->Attribute("file");	//get the img file name

			pngInfo info;
			glBindTexture(GL_TEXTURE_2D,TexID);
         if (!pngLoad(img, PNG_NOMIPMAP, PNG_SOLID, &info)) {  //Load png file(the charmap) of the font as a texture           
					return FALSE;
           }

			 pChars = pRoot->FirstChildElement("chars");
			 pChar = pChars->FirstChildElement("char");		
			int ch,XOff,YOff,Width,Height,HoriBearing,Advance;
			ch=0;
		while(pChar)//Parse through all the characters in the charmap
		{   
			//ch		=atoi(pChar->Attribute("id"));
			XOff	=atoi(pChar->Attribute("x"));
			YOff	=atoi(pChar->Attribute("y"));
			Width	=atoi(pChar->Attribute("width"));
			Height	=atoi(pChar->Attribute("height"));
			HoriBearing=-atoi(pChar->Attribute("yoffset"));
			Advance	=atoi(pChar->Attribute("xadvance"));
			TexGlyphs[ch++] = new Glyph(XOff,YOff,Width,Height,HoriBearing,Advance);

			pChar = pChar->NextSiblingElement("char");				
		}
	   }
	 }
  return TRUE;
}
void TextureFont::RenderString(float x,float y,char *string)
{
    char *c;
	glPushMatrix();
		glTranslatef(x,y,0.0f);
		for(c=string;*c;c++)
		{DrawGlyph(*c);}	
	glPopMatrix();
}


void TextureFont::RenderString(float x, float y, float width, float height, char *string)
{
 float penx=0.0f,peny=0.0f;
 char *c;int ch;float div = 512.0;
 glPushMatrix();
    glTranslatef(x,y,0.0f);
     glPushMatrix();int k=1;
	for(c=string;*c;c++)
	{   ch=*c;
		penx+=(float)(TexGlyphs[ch]->Advance/div);
		if(penx>width)
		{
		 glPopMatrix();
         glTranslatef(0.0,-pt/div,0.0f);
		 glPushMatrix();
		 penx=(float)(TexGlyphs[ch]->Advance/div);peny+=0.05;
		 
		 if(peny>height)break;
		 
		 }
		
		DrawGlyph(*c);
	}
	glPopMatrix();
 glPopMatrix();
}
void TextureFont::DrawGlyph(int ch)
{
		float RealXOff = (float)TexGlyphs[ch]->XOff/(float)TexWidth;
		float RealYOff = (float)TexGlyphs[ch]->YOff/(float)TexHeight;
		float RealHeight = (float)(TexGlyphs[ch]->Height)/(float)TexHeight;
		float RealWidth = (float)(TexGlyphs[ch]->Width)/(float)TexWidth;

		float div = 512.0;

		glBindTexture(GL_TEXTURE_2D,TexID);
		glBegin(GL_QUADS);

			glTexCoord2f(RealXOff,RealYOff + RealHeight);
			glVertex2f(0,-(float)(TexGlyphs[ch]->Height - TexGlyphs[ch]->HoriBearing)/div);

			glTexCoord2f(RealXOff,RealYOff);
			glVertex2f(0,(float)TexGlyphs[ch]->HoriBearing/div);

			glTexCoord2f(RealXOff + RealWidth,RealYOff);
			glVertex2f((float)(TexGlyphs[ch]->Width/div),(float) TexGlyphs[ch]->HoriBearing/div);

			glTexCoord2f(RealXOff + RealWidth,RealYOff + RealHeight);
			glVertex2f((float)(TexGlyphs[ch]->Width/div),-(float)(TexGlyphs[ch]->Height - TexGlyphs[ch]->HoriBearing)/div);		

	    glEnd();
			glTranslatef((float)(TexGlyphs[ch]->Advance/div),0,0);

}