/*
 *  HW5: Lightning
 *
 *  Incorporate lighting to create a a scene consisting of solid objects that can be viewed in three dimensions under user control
 * 
 *  Key bindings:
 *  l          Toggles lighting
 *  a/A        Decrease/increase ambient light
 *  d/D        Decrease/increase diffuse light
 *  s/S        Decrease/increase specular light
 *  e/E        Decrease/increase emitted light
 *  n/N        Decrease/increase shininess
 *  F1         Toggle smooth/flat shading
 *  F2         Toggle local viewer mode
 *  F3         Toggle light distance (1/5)
 *  F8         Change ball increment
 *  F9         Invert bottom normal
 *  m          Toggles light movement
 *  ,.         Lower/rise light
 *  p          Toggles ortogonal/perspective projection
 *  +/-        Change field of view of perspective
 *  x          Toggle axes
 *  arrows     Change view angle
 *  []         Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int axes=1;       //  Display axes
int mode=0;       //  Projection mode
int fov=58;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=3.0;   //  Size of world
int light=1;      //  Lighting
int move=1;       //  Move light
// Light values
int one       =   1;  // Unit value
int distance  =   3;  // Light distance
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   1;  // Elevation of light

//  Macro for sin & cos in degrees
#define Cos(th) cos(3.1415926/180*(th))
#define Sin(th) sin(3.1415926/180*(th))
#define PI 3.1415927

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  //  Maximum length of text string
void Print(const char* format , ...){
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

/*
 *  Set projection
 */
static void Project()
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   if (mode)
      gluPerspective(fov,asp,dim/8,8*dim);
   //  Orthogonal projection
   else
      glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

/*
 *  Draw vertex in polar coordinates
 */
static void Vertex(double th,double ph){
   double x = Sin(th)*Cos(ph);
   double y = Cos(th)*Cos(ph);
   double z =         Sin(ph);
   //  For a sphere at the origin, the position
   //  and normal vectors are the same
   glNormal3d(x,y,z);
   glVertex3d(x,y,z);
}

/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void cube(double x,double y,double z,double dx,double dy,double dz){
      //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Cube
   glBegin(GL_QUADS);
   //  Front
   glColor3f(1,0,0);
   glNormal3f( 0, 0, 1);
   glVertex3f(-1,-1, 1);
   glVertex3f(+1,-1, 1);
   glVertex3f(+1,+1, 1);
   glVertex3f(-1,+1, 1);
   //  Back
   glColor3f(0,0,1);
   glNormal3f( 0, 0,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   //  Right
   glColor3f(1,1,0);
   glNormal3f(+1, 0, 0);
   glVertex3f(+1,-1,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,+1,+1);
   //  Left
   glColor3f(0,1,0);
   glNormal3f(-1, 0, 0);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,-1,+1);
   glVertex3f(-1,+1,+1);
   glVertex3f(-1,+1,-1);
   //  Top
   glColor3f(0,1,1);
   glNormal3f( 0,+1, 0);
   glVertex3f(-1,+1,+1);
   glVertex3f(+1,+1,+1);
   glVertex3f(+1,+1,-1);
   glVertex3f(-1,+1,-1);
   //  Bottom
   glColor3f(1,0,1);
   glNormal3f( 0,-one, 0);
   glVertex3f(-1,-1,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,-1,+1);
   glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   //  Undo transofrmations
   glPopMatrix();
}

/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius (r)
 */
static void ball(double x,double y,double z,double r)
{
   int th,ph;
   float yellow[] = {1.0,1.0,0.0,1.0};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  White ball
   glColor3f(1,1,1);
   glMaterialf(GL_FRONT,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
   //  Bands of latitude
   for (ph=-90;ph<90;ph+=inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=2*inc)
      {
         Vertex(th,ph);
         Vertex(th,ph+inc);
      }
      glEnd();
   }
   //  Undo transofrmations
   glPopMatrix();
}

static void tetrahedron(double x,double y,double z,double dx,double dy,double dz){
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glScaled(dx,dy,dz);  // Move left and into the screen
   glBegin(GL_TRIANGLES); // Begin drawing the pyramid with 4 triangles

   // Front
   glColor3f(1.0f, 1.0f, 1.0f); 
   glNormal3f(0, 0.5, 1);   
   glVertex3f(0.0f, 1.0f, 0.0f);
   glColor3f(1.0f, 1.0f, 1.0f);     
   glVertex3f(-1.0f, -1.0f, 1.0f);
   glColor3f(1.0f, 1.0f, 1.0f);     
   glVertex3f(1.0f, -1.0f, 1.0f);

   // Right
   glColor3f(1.0f, 1.0f, 1.0f);   
   glNormal3f(1, 0.5, 0);  
   glVertex3f(0.0f, 1.0f, 0.0f);
   glColor3f(1.0f, 1.0f, 1.0f);     
   glVertex3f(1.0f, -1.0f, 1.0f);
   glColor3f(1.0f, 1.0f, 1.0f);    
   glVertex3f(1.0f, -1.0f, -1.0f);

   // Back
   glColor3f(1.0f, 1.0f, 1.0f); 
   glNormal3f(0, 0.5, -1);   
   glVertex3f(0.0f, 1.0f, 0.0f);
   glColor3f(1.0f, 1.0f, 1.0f);    
   glVertex3f(1.0f, -1.0f, -1.0f);
   glColor3f(1.0f, 1.0f, 1.0f);     
   glVertex3f(-1.0f, -1.0f, -1.0f);

   // Left
   glColor3f(1.0f, 1.0f, 1.0f); 
   glNormal3f(-1, 0.5, 0);       
   glVertex3f( 0.0f, 1.0f, 0.0f);
   glColor3f(1.0f, 1.0f, 1.0f);      
   glVertex3f(-1.0f,-1.0f,-1.0f);
   glColor3f(1.0f, 1.0f, 1.0f);     
   glVertex3f(-1.0f,-1.0f, 1.0f);
   glEnd();   
   //  Undo transformations
   glPopMatrix();

}

static void sphere(double x,double y,double z,double r) {
   const int d=5;
   int th,ph;

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,r);

   //  Latitude bands
   for (ph=-90;ph<90;ph+=d)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=d)
      {
         Vertex(th,ph);
         Vertex(th,ph+d);
      }
      glEnd();
   }

   //  Undo transformations
   glPopMatrix();
}

/**
 * Draws a cylinder
 * */
void cylinder(double doubleX, double doubleY, double doubleZ, double radius, double height){
   const int d=5;
  //  Save transformation
  glPushMatrix();
  //  Offset and scale
  glTranslated(doubleX, doubleY, doubleZ);
  glScalef(radius, height, radius);

  glBegin(GL_TRIANGLE_FAN);
  glVertex3f(0, 0, 0);
  for (int th=0; th<=360; th+=d) {
    glColor3f(0.0f, 1.0f, 0.0f); 
    glNormal3f(0, 1, 0);
    glVertex3f(Sin(th), 0, Cos(th));
  }
  glEnd();

  for (int th=0; th<=360; th+=d) {
    glBegin(GL_QUADS);
    glColor3f(0.0f, 1.0f, 0.0f);
    
    glNormal3f(Cos(th), 0, Sin(th));
    glVertex3f(Sin(th), 0, Cos(th));
    glVertex3f(Sin(th), 1, Cos(th));
    glVertex3f(Sin(th+d), 1, Cos(th+d));
    glVertex3f(Sin(th+d), 0, Cos(th+d));
    glEnd();
  }

  glBegin(GL_TRIANGLE_FAN);
  glVertex3f(0, 1, 0);
  for (int th=0; th<=360; th+=d) {
    glColor3f(0.0f, 1.0f, 0.0f);
    glNormal3f(0, -1, 0);
    glVertex3f(Sin(th), 1, Cos(th));
  }
  glEnd();
  //  Undo transformations
  glPopMatrix();
}

/**
 * Draws a torus cut in half along the y axis; adapted code from https://www.opengl.org/archives/resources/code/samples/redbook/torus.c
 * */
static void halfTorus(double doubleX, double doubleY, double doubleZ, int outerRadius, int innerRadius, double r) {
   glPushMatrix();
   glTranslated(doubleX, doubleY, doubleZ);
   glScaled(r,r,r);
   int slices = 48;
   int rings= 72;
   double centerRadius = (innerRadius + outerRadius) / 2;
   double tubeRadius = outerRadius - centerRadius;
   for (int i = 0; i < slices/2; i++) {
      double innerSlices = 1.0/slices * i;
      double outerSlices = 1.0/slices * (i+1);
      double innerCos = cos(2*PI*innerSlices);
      double innerSin = sin(2*PI*innerSlices);
      double outerCos = cos(2*PI*outerSlices);
      double outerSin = sin(2*PI*outerSlices);
      glBegin(GL_QUAD_STRIP);
      for (int j = 0; j <= rings; j++) {
         double t = 1.0/rings * j;
         double newCos = cos(2*PI*t - PI);
         double newSin = sin(2*PI*t - PI);
         
         double innerX = innerCos*(centerRadius + tubeRadius*newCos);
         double innerY = innerSin*(centerRadius + tubeRadius*newCos);
         double innerZ = newSin*tubeRadius;
         glNormal3d(-innerCos*newCos,innerSin*newCos,-newSin);
         glVertex3d(innerX,innerY,innerZ);

         double outerX = outerCos*(centerRadius + tubeRadius*newCos);
         double outerY = outerSin*(centerRadius + tubeRadius*newCos);
         double outerZ = newSin*tubeRadius;
         glNormal3d(-outerCos*newCos,outerSin*newCos,-newSin);
         glVertex3d(outerX,outerY,outerZ);
      }
			glEnd();
		}
   glPopMatrix();
}

void drawSkyline() {
   //  Chicago Skyline
   cube(-2.25, .15, .5, 0.15, 0.15, 0.15);
   sphere(-2.25, .25, .5, 0.15);
   cube(-2.25, .35, -.2, 0.2, 0.35, 0.2);
   cube(-1.65, .3, -1, 0.35, 0.3, 0.2);
   cylinder(-1.95, 1.2, -.25, .01, 0.15);
   cylinder(-1.9, 1.2, -.2, .02, 0.25);
   cylinder(-1.6, 1.2, -.2, .02, 0.25);
   cube(-1.75, .6, -.2, 0.22, 0.6, 0.2);
   cube(-1.25, .9, -1, 0.15, 0.9, 0.2);
   cylinder(-1.35, 1.8, -1, .02, 0.4);
   cylinder(-1.15, 1.8, -1, .02, 0.4);
   cube(-1, .7, -1, 0.15, 0.7, 0.2);
   cube(-0.7, .3, -1, 0.25, 0.3, 0.2);
   cube(-0.65, .65, -1, 0.13, 0.05, 0.1);
   cube(-0.65, .65, -1, 0.08, 0.3, 0.1);
   cube(-0.65, .95, -1, 0.05, 0.15, 0.1);
   cylinder(-0.68, 1.1, -1, .01, 0.1);

   // draw The Bean (Cloud Gate)
   halfTorus(-.15, 0, .2, 1, 4, .1);
   
   // draw the other half of the silolettue
   cube(0.3, .3, -.2, 0.25, 0.3, 0.2);
   tetrahedron(0.15, .8, -.2, .1, .2, .2);
   tetrahedron(0.45, .8, -.2, .1, .2, .2);
   cube(0.7, .5, -.2, 0.1, 0.5, 0.2);
   tetrahedron(0.7, 1.1, -.2, .1, .12, .12);
   cylinder(0.7, 1.2, -.2, .01, 0.15);
   cube(1, .7, -.2, 0.08, 0.7, 0.2);
   cube(0.95, .3, -.75, 0.1, 0.3, 0.1);
   cube(0.95, .1, -.75, 0.3, 0.1, 0.1);
   cube(1.1, .4, -.75, 0.15, 0.4, 0.1);
   cube(1.1, .75, -.2, 0.08, 0.75, 0.2);
   cylinder(1.1, 1.5, -.2, .01, 0.15);
   cube(1.45, .6, -.2, 0.15, 0.6, 0.2);
   cube(1.8, .6, -.2, 0.1, 0.6, 0.2);
   cylinder(1.75, 1.1, -.2, .01, 0.4);
   cylinder(1.85, 1.1, -.2, .01, 0.4);
   cube(1.65, .1, .5, 0.6, 0.1, 0.2);
   tetrahedron(1.85, .5, .5, .1, .1, .12);
   cylinder(1.73, .2, .65, .03, 0.4);
   cube(1.85, .67, .5, 0.05, 0.01, 0.02);
   cube(1.85, .65, .5, 0.01, 0.08, 0.02);
   cube(1.85, .3, .5, 0.1, 0.1, 0.1);
   tetrahedron(2.05, .78, .5, .1, .2, .12);
   cube(2.05, .3, .5, 0.1, 0.3, 0.1);
   cylinder(2.25, 0, .7, .03, 0.4);
   cube(2.05, 1.02, .5, 0.05, 0.01, 0.02);
   cube(2.05, 1, .5, 0.01, 0.08, 0.02);
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display() {
   const double len=1.5;  //  Length of axes
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   // Enable face culling in OpenGL
   glEnable(GL_CULL_FACE);
   //  Undo previous transformations
   glLoadIdentity();

   //  Perspective - set eye position
   if (mode) {
      double Ex = -2*dim*Sin(th)*Cos(ph);
      double Ey = +2*dim        *Sin(ph);
      double Ez = +2*dim*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
   }
   //  Orthogonal - set world orientation
   else {
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
   }

   //  Light switch
   if (light){
      //  Translate intensity to color vectors
      float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
      float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
      float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
      //  Light position
      float Position[]  = {distance*Cos(zh),ylight,distance*Sin(zh),1.0};
      //  Draw light position as ball (still no lighting here)
      glColor3f(1,1,1);
      ball(Position[0],Position[1],Position[2], 0.1);
      //  OpenGL should normalize normal vectors
      glEnable(GL_NORMALIZE);
      //  Enable lighting
      glEnable(GL_LIGHTING);
      //  Location of viewer for specular calculations
      glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
      //  glColor sets ambient and diffuse color materials
      glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);
      //  Enable light 0
      glEnable(GL_LIGHT0);
      //  Set ambient, diffuse, specular components and position of light 0
      glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
      glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
      glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
      glLightfv(GL_LIGHT0,GL_POSITION,Position);
   }
   else
      glDisable(GL_LIGHTING);

   drawSkyline();

   glDisable(GL_LIGHTING);
   //  White
   glColor3f(1,1,1);
   //  Draw axes
   if (axes)
   {
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }
    glWindowPos2i(5,5);
   Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s Light=%s",
     th,ph,dim,fov,mode?"Perpective":"Orthogonal",light?"On":"Off");
   if (light)
   {
      glWindowPos2i(5,45);
      Print("Model=%s LocalViewer=%s Distance=%d Elevation=%.1f",smooth?"Smooth":"Flat",local?"On":"Off",distance,ylight);
      glWindowPos2i(5,25);
      Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%.0f",ambient,diffuse,specular,emission,shiny);
   }
   //  Render the scene
   glFlush();
   //  Make the rendered scene visible
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y) {
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th -= 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th += 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  Smooth color model
   else if (key == GLUT_KEY_F1)
      smooth = 1-smooth;
   //  Local Viewer
   else if (key == GLUT_KEY_F2)
      local = 1-local;
   else if (key == GLUT_KEY_F3)
      distance = (distance==1) ? 5 : 1;
   //  Toggle ball increment
   else if (key == GLUT_KEY_F8)
      inc = (inc==10)?3:10;
   //  Flip sign
   else if (key == GLUT_KEY_F9)
      one = -one;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void idle() {
   //  Elapsed time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t,360.0);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y) {
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0') {
      th = ph = 0;
      dim = 2.6;
   }
   //  Toggle axes
   else if (ch == 'x' || ch == 'X')
      axes = 1-axes;
   //  Switch projection mode
   else if (ch == 'p' || ch == 'P')
      mode = 1-mode;
   //  Change field of view angle
   else if (ch == '-' && ch>1)
      fov--;
   else if (ch == '+' && ch<179)
      fov++;
   //  Right Bracket - increase dim
   else if (ch == ']')
      dim += 0.1;
   //  Left Bracket - decrease dim
   else if (ch == '[')
      dim -= 0.1;
   //  Toggle lighting
   else if (ch == 'l' || ch == 'L')
      light = 1-light;
   //  Toggle light movement
   else if (ch == 'm' || ch == 'M')
      move = 1-move;
   //  Move light
   else if (ch == '<')
      zh += 1;
   else if (ch == '>')
      zh -= 1;
   //  Change field of view angle
   else if (ch == '-' && ch>1)
      fov--;
   else if (ch == '+' && ch<179)
      fov++;
   //  Light elevation
   else if (ch==',')
      ylight -= 0.1;
   else if (ch=='.')
      ylight += 0.1;
   //  Ambient level
   else if (ch=='a' && ambient>0)
      ambient -= 5;
   else if (ch=='A' && ambient<100)
      ambient += 5;
   //  Diffuse level
   else if (ch=='d' && diffuse>0)
      diffuse -= 5;
   else if (ch=='D' && diffuse<100)
      diffuse += 5;
   //  Specular level
   else if (ch=='s' && specular>0)
      specular -= 5;
   else if (ch=='S' && specular<100)
      specular += 5;
   //  Emission level
   else if (ch=='e' && emission>0)
      emission -= 5;
   else if (ch=='E' && emission<100)
      emission += 5;
   //  Shininess level
   else if (ch=='n' && shininess>-1)
      shininess -= 1;
   else if (ch=='N' && shininess<7)
      shininess += 1;
   //  Translate shininess power to value (-1 => 0)
   shiny = shininess<0 ? 0 : pow(2.0,shininess);

   //  Reproject
   Project();
   //  Animate if requested
   glutIdleFunc(move?idle:NULL);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}



/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height) {
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[]) {
   //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitWindowSize(600,600);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   //  Create the window
   glutCreateWindow("Gabriella Johnson: Lighting");
   //  Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);
   //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);
   glutIdleFunc(idle);
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}
