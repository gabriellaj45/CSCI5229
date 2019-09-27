/*
 *  Lorenz Attractor
 *
 *  Display Lorenz Attractor in 3D.
 *
 *  Key bindings:
 *  s, b, r Increase the s, b, r parameter of the Lorenz Attractor
 *  d, n, t Decrease the s, b, r parameter of the Lorenz Attractor
 *  +/-    Increase/decrease size of Lorenz Attractor plotted
 *  arrows Change view angle
 *  0      Reset view angle
 *  ESC || q   Exit
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

//  Globals
int th = 0;       // Azimuth of view angle
int ph = 0;       // Elevation of view angle
double dim = 2;   // Dimension of orthogonal box
double lorenzSize = 0.02; // dilation size of Lorenz Attractor
int numSteps = 50000; // steps for Euler intergration
double shrinkX = 0; // new reduced x coordinate for Lorenz Atrractor
double shrinkY = 0; // new reduced y coordinate for Lorenz Atrractor
double shrinkZ = 0; // new reduced z coordinate for Lorenz Atrractor

/*  Lorenz Parameters  */
double s  = 10;
double b  = 2.6666;
double r  = 28;

double lorenzPoints[50000][3]; // data structure to store Lorenz Attractor points

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  // Maximum length of text string
void Print(const char* format , ...)
{
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

void computeLorenz(){
   int i;
   /*  Coordinates  */
   double doubleX = 1;
   double doubleY = 1;
   double doubleZ = 1;
   /*  Time step  */
   double dt = 0.001;

   /*
   *  Integrate 50,000 steps (50 time units with dt = 0.001)
   *  Explicit Euler integration
   */
   for (i = 0; i < numSteps; i++) {
      double dx = s * (doubleY - doubleX);
      double dy = doubleX * (r - doubleZ) - doubleY;
      double dz = doubleX * doubleY - b * doubleZ;
      doubleX += dt * dx;
      doubleY += dt * dy;
      doubleZ += dt * dz;

      // shrink the size of the attractor
      shrinkX = doubleX * lorenzSize;
      shrinkY = doubleY * lorenzSize;
      shrinkZ = doubleZ * lorenzSize;

      // store Lorenz points in a data structure
      lorenzPoints[i][0] = shrinkX;
      lorenzPoints[i][1] = shrinkY;
      lorenzPoints[i][2] = shrinkZ;
   }
}

/*
 *  Display the scene
 */
void display()
{
   // Generate points for Lorenz Attractor
   computeLorenz();
   // Clear the image
   glClear(GL_COLOR_BUFFER_BIT);
   // Reset previous transforms
   glLoadIdentity();
   // Set view angle
   glRotated(ph,1,0,0);
   glRotated(th,0,1,0);
   
   // Draw Lorenz Attractor
   glColor3f(1,1,0);
   glPointSize(3);
   glBegin(GL_LINE_STRIP);
   for(int i = 0; i < numSteps; i++){
      glColor3dv(lorenzPoints[i]);
      glVertex3dv(lorenzPoints[i]);
   }
   glEnd();

   // Draw axes in white
   glColor3f(1,1,1);
   glBegin(GL_LINES);
   glVertex3d(0,0,0);
   glVertex3d(1,0,0);
   glVertex3d(0,0,0);
   glVertex3d(0,1,0);
   glVertex3d(0,0,0);
   glVertex3d(0,0,1);
   glEnd();

   // Label axes
   glRasterPos3d(1,0,0);
   Print("X");
   glRasterPos3d(0,1,0);
   Print("Y");
   glRasterPos3d(0,0,1);
   Print("Z");

   // Display parameters
   glWindowPos2i(5,5);
   Print("Rx=%d Ry=%d s=%.2lf b=%lf r=%.2lf",th, ph, s, b, r);

   // Flush and swap
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   // Exit on ESC or q key
   if (ch == 27 || ch == 'q')
      exit(0);
   // Reset view angle
   else if (ch == '0')
      th = ph = 0;
   // Increase size of Lorenz attractor by 0.005
   else if (ch == '+')
      lorenzSize += 0.005;
   // Decrease size of Lorenz attractor by 0.005
   else if (ch == '-')
      lorenzSize -= 0.005;
   // change the s, b, or r parameter for the Lorenz attractor
   else if (ch == 'r')
      r += 1;
   else if (ch == 'b')
      b += 0.33333;
   else if (ch == 's')
      s += 1;
   else if (ch == 't')
      r -= 1;
   else if (ch == 'n')
      b -= 0.33333;
   else if (ch == 'd')
      s -= 1;
   else if (ch == 'v') {
      s = 10;
      b  = 2.6666;
      r  = 28;
   }
   // Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   // Right arrow key - increase azimuth by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   // Left arrow key - decrease azimuth by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   // Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   // Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   
   // Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;

   // Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   // Ratio of the width to the height of the window
   double w2h = (height>0) ? (double)width/height : 1;
   // Set the viewport to the entire window
   glViewport(0,0, width,height);
   // Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   // Undo previous transformations
   glLoadIdentity();
   // Orthogonal projection box adjusted for the
   // aspect ratio of the window
   glOrtho(-dim*w2h,+dim*w2h, -dim,+dim, -dim,+dim);
   // Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   // Undo previous transformations
   glLoadIdentity();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   // Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   // Request double buffered, true color window 
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
   // Request 500 x 500 pixel window
   glutInitWindowSize(500,500);
   // Create the window
   glutCreateWindow("Gabriella Johnson");
   
   // Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);
   // Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   // Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   // Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);
   // Pass control to GLUT so it can interact with the user
   glutMainLoop();
   // Return code
   return 0;
}
