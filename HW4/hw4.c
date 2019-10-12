/*
 *  Projections
 *
 *  Create a scene to be viewed in both orthogonal and perspective projections from multiple eye positions directed by user input.
 * 
 *  Key bindings:
 *  a/d/e      Look left/right/forward again in the scene
 *  w/s        Move forwards/backward into/from the scene
 *  m          Toggle between view modes: perspective, orthogonal, and first person perspective
 *  +/-        Changes field of view for perspective
 *  x          Toggle axes
 *  arrows     Change view angle
 *  [  ]       Zoom in and out: decrease/increase dim
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
int axes=0;       //  Display axes
int mode=0;       //  Projection mode
int fov=58;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=2.6;   //  Size of world
double MX = 0;
double MY = 0;
double MZ = 0;
double EX = 0;
double EY = 0;
double EZ = 5.2;

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
   glColor3f(1, 1, 1);
   glVertex3d(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));
}

/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void cube(double x,double y,double z,double dx,double dy,double dz){
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glScaled(dx,dy,dz);
   //  Cube
   glBegin(GL_QUADS);
   //  Front
   glColor3f(1,0,0);
   glVertex3f(-1,-1, 1);
   glVertex3f(+1,-1, 1);
   glVertex3f(+1,+1, 1);
   glVertex3f(-1,+1, 1);
   //  Back
   glColor3f(0,0,1);
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   //  Right
   glColor3f(1,1,0);
   glVertex3f(+1,-1,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,+1,+1);
   //  Left
   glColor3f(0,1,0);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,-1,+1);
   glVertex3f(-1,+1,+1);
   glVertex3f(-1,+1,-1);
   //  Top
   glColor3f(0,1,1);
   glVertex3f(-1,+1,+1);
   glVertex3f(+1,+1,+1);
   glVertex3f(+1,+1,-1);
   glVertex3f(-1,+1,-1);
   //  Bottom
   glColor3f(1,0,1);
   glVertex3f(-1,-1,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,-1,+1);
   glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   //  Undo transformations
   glPopMatrix();
}

static void tetrahedron(double x,double y,double z,double dx,double dy,double dz){
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glScaled(dx,dy,dz);  // Move left and into the screen
      glBegin(GL_TRIANGLES);           // Begin drawing the pyramid with 4 triangles
      // Front
      glColor3f(1.0f, 1.0f, 1.0f);    
      glVertex3f( .0f, 1.0f, 0.0f);
      glColor3f(1.0f, 1.0f, 1.0f);     
      glVertex3f(-1.0f, -1.0f, 1.0f);
      glColor3f(1.0f, 1.0f, 1.0f);     
      glVertex3f(1.0f, -1.0f, 1.0f);
 
      // Right
      glColor3f(1.0f, 1.0f, 1.0f);     
      glVertex3f(0.0f, 1.0f, 0.0f);
      glColor3f(1.0f, 1.0f, 1.0f);     
      glVertex3f(1.0f, -1.0f, 1.0f);
      glColor3f(1.0f, 1.0f, 1.0f);    
      glVertex3f(1.0f, -1.0f, -1.0f);
 
      // Back
      glColor3f(1.0f, 1.0f, 1.0f);    
      glVertex3f(0.0f, 1.0f, 0.0f);
      glColor3f(1.0f, 1.0f, 1.0f);    
      glVertex3f(1.0f, -1.0f, -1.0f);
      glColor3f(1.0f, 1.0f, 1.0f);     
      glVertex3f(-1.0f, -1.0f, -1.0f);
 
      // Left
      glColor3f(1.0f, 1.0f, 1.0f);       
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
 * Draws a cylinder; adapted code from https://gist.github.com/nikAizuddin/5ea402e9073f1ef76ba6
 * */
void cylinder(double doubleX, double doubleY, double doubleZ, double radius, double height){
    double x              = 0.0;
    double y              = 0.0;
    double angle          = 0.0;
    GLfloat angle_stepsize = 0.1;

    glPushMatrix();
    //  Offset
    glTranslated(doubleX, doubleY, doubleZ);
    glRotated(-90,1,0,0);
    
    /** Draw the tube */
    glColor3f(0.0f, 1.0f, 0.0f); 
    glBegin(GL_QUAD_STRIP);
    angle = 0.0;
    while( angle < 2*PI ) {
        x = radius * cos(angle);
        y = radius * sin(angle);
        glVertex3f(x, y , height);
        glVertex3f(x, y , 0.0);
        angle = angle + angle_stepsize;
    }
    glVertex3f(radius, 0.0, height);
    glVertex3f(radius, 0.0, 0.0);
    glEnd();

    /** Draw the circle on top of cylinder */
    glColor3f(0.0f, 1.0f, 0.0f); 
    glBegin(GL_POLYGON);
    angle = 0.0;
    while( angle < 2*PI ) {
        x = radius * cos(angle);
        y = radius * sin(angle);
        glVertex3f(x, y , height);
        angle = angle + angle_stepsize;
    }
    glVertex3f(radius, 0.0, height);
    glEnd();
    glPopMatrix();
}

/**
 * Draws a torus cut in half along the y axis; adapted code from https://www.opengl.org/archives/resources/code/samples/redbook/torus.c
 * */
static void halfTorus(double doubleX, double doubleY, double doubleZ, int numc, int numt, double r) {
    glPushMatrix();
    //  Offset
    glTranslated(doubleX, doubleY, doubleZ);
    glScaled(r,r,r);
   int i, j, k;
   double s, t, x, y, z, twopi;

   twopi = 2 * PI;
   for (i = 0; i < numc; i++) {
      glBegin(GL_QUAD_STRIP);
      for (j = 0; j <= numt / 2; j++) {
         for (k = 1; k >= 0; k--) {
            s = (i + k) % numc + 0.5;
            t = j % numt;

            x = (1+.2*cos(s*twopi/numc))*cos(t*twopi/numt);
            y = (1+.4*cos(s*twopi/numc))*sin(t*twopi/numt);
            z = .5 * sin(s * twopi / numc);
            glVertex3f(x, y, z);
         }
      }
      glEnd();
   }
   glPopMatrix();
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
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
   if (mode == 1){
      double Ex = -2*dim*Sin(th)*Cos(ph);
      double Ey = +2*dim        *Sin(ph);
      double Ez = +2*dim*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez , 0,0,0 ,0,Cos(ph),0);
   }
   //  Orthogonal - set world orientation
   else if (mode == 0) {
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
   }
   // First Person Perspective
   else if (mode == 2) {
      MX = -2*dim*Sin(th)*Cos(ph);
      MY = -2*dim        *Sin(ph);
      MZ = -2*dim*Cos(th)*Cos(ph);

      gluLookAt(EX, EY, EZ, EX + MX, EY + MY, EZ + MZ, 0,1,0);
   }
 
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
   halfTorus(-.15, 0, .2, 8, 26, .25);
   
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
   //  Display parameters
   glWindowPos2i(5,5);
   if(mode == 0)
      Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s",th,ph,dim,fov,"Orthogonal");
   if(mode == 1)
      Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s",th,ph,dim,fov,"Perspective");
   if(mode == 2)
      Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s",th,ph,dim,fov,"First Person");

   //  Render the scene
   glFlush();
   //  Make the rendered scene visible
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0') {
      th = ph = 0;
      dim = 2.6;
      MX = 0;
      MY = 0;
      MZ = 0;
      EX = 0;
      EY = 0;
      EZ = 2*dim;
   }
   //  Toggle axes
   else if (ch == 'x' || ch == 'X')
      axes = 1-axes;
   //  Switch display mode
   else if (ch == 'm' || ch == 'M')
      mode = (mode+1)%3;
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
   // Go forwards into the scene
   else if (ch == 'w' || ch == 'W') {
		EX += MX * .1;
      EZ += MZ * .1;
   }
   // Go backwards from the scene
   else if (ch == 's' || ch == 'S') {
		EX -= MX * .1;
      EZ -= MZ * .1;
   }
   // Look at the x axis
   else if (ch == 'd' || ch == 'D') {
		th = -90;
      ph = 0;
   }
   // Look at the y axis
   else if (ch == 'a' || ch == 'A') {
		th = 90;
      ph = 0;
   }
   // Look forward again
   else if (ch == 'e' || ch == 'E') {
		th = 0;
      ph = 0;
   }


   //  Reproject
   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
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
int main(int argc,char* argv[])
{
   //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitWindowSize(600,600);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   //  Create the window
   glutCreateWindow("Gabriella Johnson: Chicago Skyline");
   //  Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);
   //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}
