// Sprite.h
// Sprite class Ver 1.0
// C. Markham Jan 2015

#include <freeglut.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

class Sprite
{
    public:  GLuint Texture_Id; // Texture id in GPU associated with sprite
    public:  int frame_no;      // Frame numbered top left to bottom right 0,1,2...
	private: int frame_x;    
	private: int frame_y;
    public:  int frame_size_x;  // Width of sprite in separate images
    public:  int frame_size_y;  // Height of sprite, Note: (1,1)=Single image
    public:  int sprite_width;  // Sprites display width in pixels
    public:  int sprite_height; // Sprites display width in pixels
    public:  float sprite_x_position;  // x,y, Position of sprite in pixels
    public:  float sprite_y_position;  // Note: float to allow physics...
    public:  float sprite_x_velocity;  // x=x+vx*dt
    public:  float sprite_y_velocity;  // y=y+vy*dt
    public:  bool direction;

    public: static void spriteModeOn(int,int);   // Configure OpenGL window
    public: static void text(int,int,char*);     // Print text on screen
    public: void load(char *,int,int,int,int,int,int,bool); // load sprite
    public: void Sprite::drawSprite();           // Enter sprite into graphics pipeline
    public: void Sprite::advance_frame(int,int); // Advance frame_no between two bounds
    public: void Sprite::advance_projectile();   // Advance movement of sprite 
	public: bool Sprite::collision(Sprite);      // Detect collision between sprites

    private: GLuint glmloadTextureBMP(char *);   // load texture method (prototype)

    public: Sprite()         // Class constructor, value in object/sprite at start
	{
		frame_no=0;          // Index to first frame
		frame_size_x=1;      // Number of images (horizontal) in bitmap
		frame_size_y=1;      // Number of images (vertical) in bitmap
		sprite_width=64;     // Display size of sprite (horizontal pixels)
		sprite_height=64;    // Display size of sprite (vertical in pixels)
		sprite_x_position=0; // Horizontal position of sprite on screen
		sprite_y_position=0; // Vertical position of sprite on screen
		sprite_x_velocity=0; // Horizontal velocity of sprite on screen
		sprite_y_velocity=0; // Vertical velocity of sprite on screen
		direction=true;      // true=normal, false=flip_horizontal
	}  
};

// Tun on and onfigure OpenGL window to suit work with sprites
void Sprite::spriteModeOn(int winx, int winy)
{
	    static int window_width;
        static int window_height;
		static int call_count=0;

		if(call_count==0)  // If first visit to the function configure the application window etc
		{
			window_width=winx;
			window_height=winy;

			// Create a Window called "FreeGlut One" at 100,100 on main screen, sized 512x512 pixels
			glutInitWindowSize(winx,winy);
			glutInitWindowPosition(10,10);
		
			char *myargv [1];  // A bit of a fudge to get Init out of the main program
			int myargc=1;
			myargv [0]=_strdup ("OpenGL_Game");
			glutInit(&myargc, myargv);
        
			// glutInit(&argc, argv);
			glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
			glutCreateWindow("Sprite based game");
			// 2D program use an Orthographic projection to keep everything independent of Z
	    
			//Set blending 
			glDisable( GL_LIGHTING );
			glEnable( GL_BLEND ); 
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
			glDisable( GL_DEPTH_TEST );
		}

		glLoadIdentity(); // Only need to chnage the projection matrix following a resize.....
		glMatrixMode(GL_PROJECTION);
		// Maps vertex co-ords 1:1 to the screen co-ords (unusual but keeps idea of pixels)
		gluOrtho2D(0.0, float(window_width), 0.0, float(window_height));  
		call_count++;
};

// Function to add text to a graphics display 
void Sprite::text(int x, int y, char *string)
{
  glDisable(GL_TEXTURE_2D);
  glColor4f( 1.0f, 0.0f, 0.0f, 1.0f );  // Red
  glRasterPos2f(x, y);
  int len, i;
  len = (int)strlen(string);
  for (i = 0; i < len; i++) 
  {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]); // Other fonts possible
  }
  glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );  // Switch back to white
}

// Load a sprite into memory and intialise key parameters.
// file_name, No. images across, No. Images down, horizontal position on screen, vertical position on screen, 
// horizontal size on screen, vertical size on screen, direction sprite is pointing   
void Sprite::load(char * fname, int sx, int sy,int xpos, int ypos,int ssx, int ssy,bool d)
{
	frame_no=0;
	sprite_x_position=xpos;
	sprite_y_position=ypos;
	frame_size_x=sx;
	frame_size_y=sy;
	sprite_width=ssx;  
	sprite_height=ssy; 
	direction=d;
	Texture_Id=glmloadTextureBMP(fname);
};

// Put sprite into graphics pipleine
void Sprite::drawSprite()
{						
	    frame_y=frame_no/frame_size_x;  // Convert frame number to frame position in the texture
		frame_x=frame_no%frame_size_x;

		float offset_x=(float)frame_x/(float)frame_size_x;
		float size_x=1.0f/(float)frame_size_x;
		float offset_y=(float)frame_y/(float)frame_size_y;
		float size_y=1.0f/(float)frame_size_y;
		float _near=0.0005f;

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,Texture_Id);

		if(direction)
		{
			glBegin(GL_QUADS);
			glNormal3f(0, 1, 0);
			glTexCoord2f(size_x+offset_x-_near, offset_y+_near);
			glVertex3f(sprite_x_position+sprite_width, sprite_y_position,1.0f);
			glTexCoord2f(size_x+offset_x-_near, offset_y+size_y-_near);
			glVertex3f(sprite_x_position+sprite_width, sprite_y_position+sprite_height,offset_y+size_y);
			glTexCoord2f(offset_x+_near,offset_y+size_y-_near);
			glVertex3f(sprite_x_position, sprite_y_position+sprite_height,1.0f);
			glTexCoord2f(offset_x+_near, offset_y+_near);
			glVertex3f(sprite_x_position, sprite_y_position,1.0f);
			glEnd();
		}
		else
		{
			glBegin(GL_QUADS);
			glNormal3f(0, 1, 0);
			glTexCoord2f(offset_x+_near, offset_y+_near);
			glVertex3f(sprite_x_position+sprite_width, sprite_y_position,1.0f);
			glTexCoord2f(offset_x+_near, offset_y+size_y-_near);
			glVertex3f(sprite_x_position+sprite_width, sprite_y_position+sprite_height,offset_y+size_y);

			glTexCoord2f(size_x+offset_x-_near,offset_y+size_y-_near);
			glVertex3f(sprite_x_position, sprite_y_position+sprite_height,1.0f);

			glTexCoord2f(size_x+offset_x-_near, offset_y+_near);
			glVertex3f(sprite_x_position, sprite_y_position,1.0f);
			glEnd();
		}
}

// Advance sprite one frame between bounds [start,stop]
void Sprite::advance_frame(int start,int stop)
{
	frame_no++;
	if(frame_no>stop) frame_no=start;
}

// Euler's method for modelling projectiles in free fall
void Sprite::advance_projectile()
{
	float dt=0.1;    // time step
	float k1=-0.15;  // air resistance
	float g=0.25;    // acceleration due to gravity

	sprite_x_position+=sprite_x_velocity*dt;
    sprite_x_velocity+=((k1*sprite_x_velocity)*dt);      // Air drag k1

	sprite_y_position+=sprite_y_velocity*dt;
    sprite_y_velocity+=((g+(k1*sprite_y_position))*dt);  // Gravity and drag
}


// Detect colision between bounding boxes of two sprites
bool Sprite::collision(Sprite other_sprite)
{
	bool collision=true;

	if
	(
	  ( (other_sprite.sprite_x_position) > (sprite_x_position+sprite_width) ) ||
	  ( (other_sprite.sprite_x_position+other_sprite.sprite_width) < sprite_x_position ) ||
	  ( (other_sprite.sprite_y_position) > sprite_y_position+sprite_height  ) ||
	  ( (other_sprite.sprite_y_position+other_sprite.sprite_height)  < sprite_y_position )
	)
	{
		collision =false;
	}

 return collision;
}

// Could use Simple OpenGL Image Library (SOIL) instead
// Homebrew function to load texture (no error trapping).
// File must be 24bit Windows bitmap.
// Strongly recommend image size is evenly divisible by 4 in each direction e.g. 128x256 
// Code below was based on the above ...
// http://www.inversereality.org/tutorials/graphics%20programming/bitmaploading.html
GLuint Sprite::glmloadTextureBMP(char * fname)
{
	wchar_t* wString=new wchar_t[256];
    MultiByteToWideChar(CP_ACP, 0, fname, -1, wString, 256);
    
	HANDLE hBitMap = LoadImage(0,wString, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    BITMAP bitmap;
    GetObject(hBitMap,sizeof(BITMAP),&bitmap);

  //  int size = bitmap.bmHeight*bitmap.bmWidth*((bitmap.bmBitsPixel/8)+1);  	// Original
	 int size = bitmap.bmHeight*bitmap.bmWidth*4;  	// 4=RGBA

	// int size = (bitmap.bmHeight*bitmap.bmWidth*(bitmap.bmBitsPixel/8))+1;  	// New

    BYTE *lpBits = new BYTE[ size ];
    int PaddedWidth = (bitmap.bmWidth  + 3) & (~3); // Round width up to next multiple of 4
    GetBitmapBits((HBITMAP)hBitMap,size,lpBits ); 

	BYTE r,g,b,a;
	int templ,tempr;
    for(int j=0;j<size;j+=4)   
	{
			if((lpBits[j+2]==255)&&(lpBits[j+1]==0)&&(lpBits[j]==255))  // Magenta RGB=(255,0,255) is made transparent
			{
			  lpBits[j+0]=0; // Red
              lpBits[j+1]=0; // Green
              lpBits[j+2]=0; // Blue
			  lpBits[j+3]=0; // Alpha
			}
			else
			{
			  r=lpBits[j+0]; // Red
              g=lpBits[j+1]; // Green
              b=lpBits[j+2]; // Blue
			  a=lpBits[j+3]; // Alpha
			  a=255;         // 100% of the sprite (0% of the background)
			  lpBits[j+0]=b; // Red
              lpBits[j+1]=g; // Green
              lpBits[j+2]=r; // Blue
			  lpBits[j+3]=a; // Alpha
		}
	}

	// Flip texture vertical (inefficient but only done on initalisation)
	BYTE rgb;  
	for(int j=0;j<bitmap.bmHeight/2;j++)   
	{
		for(int i=0;i<PaddedWidth;i++)
		{
			for(int k=0;k<4;k++)
			{
             templ=4*(i+(j*PaddedWidth));						// Address of pixel at top
			 tempr=4*(i+((bitmap.bmHeight-j-1)*PaddedWidth));	// Address of pixel at bottom

			 rgb=lpBits[tempr+k];								// Swap Red
             lpBits[tempr+k]=lpBits[templ+k];
             lpBits[templ+k]=rgb;
			}
		}
	}

	GLuint textureID;
    glGenTextures(1, &textureID);
    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);
	// Turn on and configure texture mapping, texture copied into OpenGL/Graphics card (include the Alpha=0 transparent, A=255 opaque)
	glTexImage2D(GL_TEXTURE_2D,0, GL_RGBA,bitmap.bmWidth,bitmap.bmHeight,0,GL_RGBA,GL_UNSIGNED_BYTE, (GLvoid *) lpBits);
	// Magnification filter (texel larger than the pixel)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// Minification filter (texel smaller than the pixel) // GL_
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	// Use the texture loaded into the graphics card (rather than a colour)
	glEnable(GL_TEXTURE_2D);
	// Free system memory used to store texture (it is now been transfered to the graphics card)
    delete []lpBits;

	return textureID;
}

