#include "TextureFont.h"
#include "Glyph.h"

typedef struct
{
	GLfloat location[3];
	GLfloat tex[2];

}vertex;

#define BUFFER_OFFSET(i) ((char*)NULL + (i))
#define GL_ARRAY_BUFFER_ARB 0x8892
#define GL_STATIC_DRAW_ARB 0x88E4
typedef void (APIENTRY * PFNGLBINDBUFFERARBPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRY * PFNGLDELETEBUFFERSARBPROC) (GLsizei n, const GLuint *buffers);
typedef void (APIENTRY * PFNGLGENBUFFERSARBPROC) (GLsizei n, GLuint *buffers);
typedef void (APIENTRY * PFNGLBUFFERDATAARBPROC) (GLenum target, int size, const GLvoid *data, GLenum usage);

// VBO Extension Function Pointers
PFNGLGENBUFFERSARBPROC glGenBuffersARB = NULL;					// VBO Name Generation Procedure
PFNGLBINDBUFFERARBPROC glBindBufferARB = NULL;					// VBO Bind Procedure
PFNGLBUFFERDATAARBPROC glBufferDataARB = NULL;					// VBO Data Loading Procedure
PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB = NULL;			// VBO Deletion Procedure

int nextPowerOf2(int n)
{
 int i = 1;
while (i < n)
 {i *= 2;}
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

void TextureFont::InitVBOExtensions()
{
 glGenBuffersARB = (PFNGLGENBUFFERSARBPROC) wglGetProcAddress("glGenBuffersARB");
 glBindBufferARB = (PFNGLBINDBUFFERARBPROC) wglGetProcAddress("glBindBufferARB");
 glBufferDataARB = (PFNGLBUFFERDATAARBPROC) wglGetProcAddress("glBufferDataARB");
 glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC) wglGetProcAddress("glDeleteBuffersARB");
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

	//InitVBOExtensions();
	//for(int i=32;i<127;i++)
	//CreateVBO(i);
	

	return TRUE;
}


void TextureFont::RenderString(float x,float y,char *string)
{
    char *c;
	glBindTexture(GL_TEXTURE_2D,TexID);
	glPushMatrix();
		glTranslatef(x,y,0.0f);
		for(c=string;*c;c++)
		{DrawVBO(*c);}
	glPopMatrix();	
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
}
void TextureFont::CreateVBO(int ch)
{
		float RealXOff = (float)TexGlyphs[ch]->getXOffset()/(float)TexWidth;
		float RealYOff = (float)TexGlyphs[ch]->getYOffset()/(float)TexHeight;
		float RealHeight = (float)(TexGlyphs[ch]->getHeight())/(float)TexHeight;
		float RealWidth = (float)(TexGlyphs[ch]->getWidth())/(float)TexWidth;

		float div = 512.0;

	vertex verts[4];
	verts[0].location[0]=0.0;	verts[0].location[1]=-(float)(TexGlyphs[ch]->getHeight() - TexGlyphs[ch]->getHoriBearing())/div;	verts[0].location[2]=0.0;
	verts[0].tex[0]=RealXOff;	verts[0].tex[1]=RealYOff + RealHeight;

	verts[1].location[0]=0.0;	verts[1].location[1]=(float)TexGlyphs[ch]->getHoriBearing()/div;	verts[1].location[2]=0.0;
	verts[1].tex[0]=RealXOff;	verts[1].tex[1]=RealYOff;

	verts[2].location[0]=(float)(TexGlyphs[ch]->getWidth()/div);	verts[2].location[1]=(float) TexGlyphs[ch]->getHoriBearing()/div;	verts[2].location[2]=0.0;
	verts[2].tex[0]=RealXOff + RealWidth;verts[2].tex[1]=RealYOff;

	verts[3].location[0]=(float)(TexGlyphs[ch]->getWidth()/div);verts[3].location[1]=-(float)(TexGlyphs[ch]->getHeight() - TexGlyphs[ch]->getHoriBearing())/div;	verts[3].location[2]=0.0;
	verts[3].tex[0]= RealXOff + RealWidth;verts[3].tex[1]= RealYOff + RealHeight;


    glGenBuffersARB(1,&vbo[ch]);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo[ch]);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertex)*4, verts, GL_STATIC_DRAW_ARB);
	glVertexPointer(3, GL_FLOAT, sizeof(vertex), BUFFER_OFFSET(0));
	glTexCoordPointer(2, GL_FLOAT, sizeof(vertex), BUFFER_OFFSET(12));

}
void TextureFont::DrawVBO(int ch)
{
glBindBufferARB(GL_ARRAY_BUFFER_ARB,vbo[ch]);

glEnableClientState(GL_TEXTURE_COORD_ARRAY);
glEnableClientState(GL_VERTEX_ARRAY);

glVertexPointer(3, GL_FLOAT, sizeof(vertex), BUFFER_OFFSET(0));
glTexCoordPointer(2, GL_FLOAT, sizeof(vertex), BUFFER_OFFSET(12));

glDrawArrays(GL_QUADS,0,4);


glDisableClientState(GL_TEXTURE_COORD_ARRAY);
glDisableClientState(GL_VERTEX_ARRAY);

glTranslatef((float)(TexGlyphs[ch]->getAdvance()/512.0),0,0);

}