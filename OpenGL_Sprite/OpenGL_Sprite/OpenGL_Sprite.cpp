// GLUT/Open_GL Single buffer example using some basic primitives in 2D
#include "stdafx.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define _USE_MATH_DEFINES // Required to access M_PI
#include <math.h>         // Required to access sin() and cos()  

#include <freeglut.h>
GLuint glmLoadTextureBMP(char *);
// Global variables
int sprite_x_position=320;
int frame=1; 
static void idle()
{
	//sprite_x_position+=2;
//if(sprite_x_position>600) sprite_x_position=0;
 glutPostRedisplay(); // Request redraw... 
} 
 static void qwerty_keys(unsigned char key, int x, int y)
{
 switch (key)
 {
 case 'a': sprite_x_position-=5;
 frame++; if (frame>10) frame=1;
 break;

 case 'd': sprite_x_position+=5;
 frame++; if (frame>10) frame=1;
 break;
 case 'q': glutLeaveMainLoop () ; break; // (Stop!) 
 }
 glutPostRedisplay(); // Send message to redraw screen
} 
static void display(void)
{
 glClearColor(1.0f, 1.0f, 1.0f, 1.0f) ;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);     // Enable use of texture uv mapping
    glDisable(GL_DEPTH_TEST);    // Depth testing not required (2D only 1 sprite)
	glDisable(GL_LIGHTING);   // Do not include lighting (yet) 
	glEnable(GL_BLEND);       // Enable Alpha blending of textures
// Screen pixel=(Exisiting screen pixel*(1-Alpha)) + (New pixel * Alpha)
// Transparent pixels RGBA = (0,0,0,0) Image pixels = (R,G,B,1) 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	                                                     


glBegin(GL_QUADS);
glTexCoord2f(frame/10.0,0);
glVertex2i(sprite_x_position,200);
glTexCoord2f((frame+1)/10.0,0);
glVertex2i(sprite_x_position+150,200);
glTexCoord2f((frame+1)/10.0,1);
glVertex2i(sprite_x_position+150,350);
glTexCoord2f(frame/10.0,1);
glVertex2i(sprite_x_position,350);
glEnd(); 


glutSwapBuffers();  

}

int _tmain(int argc, char** argv)      // Entry point of program
{
	glutInit(&argc, argv);         // Start glut
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);   // Create a single buffer with data in RGB format
	glutInitWindowSize(640,480);   // Open a window at (10,10) with size (640x480) called 
	glutInitWindowPosition(10,10); 
	glutCreateWindow("Sprite based game");
	glLoadIdentity(); 
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0.0, 640.0, 0.0, 480.0);  // Map OpenGL to Screen crds 1:1, ignore Z, 2D (X,Y)
	glmLoadTextureBMP("Spritesheet.bmp");
	glutKeyboardFunc(qwerty_keys); 
		  glutIdleFunc(idle);
		  glutDisplayFunc(display);
	glutMainLoop();			    // Start Glut main loop, exit via break
	return 0;
}
GLuint glmLoadTextureBMP(char * fname)  
{
    wchar_t* wString=new wchar_t[256]; // Convert char[] string to LPSTR/wchar
    MultiByteToWideChar(CP_ACP, 0, fname, -1, wString, 256);
    HANDLE hBitMap = LoadImage(0,wString, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);  
    
    BITMAP bitmap;
    GetObject(hBitMap,sizeof(BITMAP),&bitmap);
    int size = bitmap.bmHeight*bitmap.bmWidth*((bitmap.bmBitsPixel/8)+1);  	
    BYTE *lpBits = new BYTE[ size ];
    int PaddedWidth = (bitmap.bmWidth  + 3) & (~3); // Round width up to next multiple of 4
    GetBitmapBits((HBITMAP)hBitMap,size,lpBits ); 
    BYTE r,g,b,a;
    int templ,tempr;
    
    for(int j=0;j<size;j+=4) // Magenta RGBA=(255,0,255) tunrs transparent
    {
	if((lpBits[j+2]==255)&&(lpBits[j+1]==0)&&(lpBits[j]==255))  
	{
	 /*Red*/lpBits[j+0]=0; /*Green*/lpBits[j+1]=0; /*Blue*/lpBits[j+2]=0; /*Alpha*/lpBits[j+3]=0; 
	}
	else
	{
        // Reverse BGRA to RGBA and force A=255 (Alpha 100%)
	/*Red*/r=lpBits[j+0]; /*Green*/g=lpBits[j+1]; /*Blue*/b=lpBits[j+2]; /*Alpha*/a=lpBits[j+3]; 
	a=255; // 100% of the sprite (0% of the background)
	/*Red*/lpBits[j+0]=b; /*Green*/lpBits[j+1]=g; /*Blue*/lpBits[j+2]=r; /*Alpha*/lpBits[j+3]=a;
	}
    }

    BYTE rgb;  // Flip texture vertical (inefficient but only done on initalisation)
    for(int j=0;j<bitmap.bmHeight/2;j++)   
    {
	for(int i=0;i<PaddedWidth;i++)
	{
	  for(int k=0;k<4;k++)
	  {
             templ=4*(i+(j*PaddedWidth));    // Address of pixel at top
	     tempr=4*(i+((bitmap.bmHeight-j-1)*PaddedWidth));	// Address of pixel at bottom
	     rgb=lpBits[tempr+k];								
             lpBits[tempr+k]=lpBits[templ+k];
             lpBits[templ+k]=rgb;
	  }
	}
    }
    GLuint textureID;
    glGenTextures(1, &textureID);  // Create 1 texture
    // "Bind" the newly created texture address with an ID
    glBindTexture(GL_TEXTURE_2D, textureID);
    // Turn on and configure texture mapping, texture copied into OpenGL/Graphics card 
    glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA,bitmap.bmWidth,bitmap.bmHeight,0,GL_RGBA,GL_UNSIGNED_BYTE, (GLvoid *) lpBits);
    // Magnification filter (texel larger than the pixel)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // Minification filter (texel smaller than the pixel) _
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // Free system memory used to store texture (it is now been transfered to the graphics card)
    delete []lpBits;
    return textureID;
}

