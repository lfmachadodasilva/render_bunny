// VScene test
// Visualiza��o 3D
// W. Celes - celes@inf.puc-rio.br
// PUC-Rio, Jan 2008


#include "manipulator.h"
#include "mesh.h"

#ifdef WIN32
#include <Windows.h>    /* includes only in MSWindows not in UNIX */
#include <gl/gl.h>     /* OpenGL functions*/
#include <gl/glu.h>    /* OpenGL utilitary functions*/
#include <gl/glut.h>    /* OpenGL utilitary functions*/
#else
//#include <GL/gl.h>     /* OpenGL functions*/
//#include <GL/glu.h>    /* OpenGL utilitary functions*/
#include <GL/glut.h>    /* OpenGL utilitary functions*/
#endif

#include <stdlib.h>

static VMesh* bunny;

static void Init ()
{
  bunny = new VMesh("../bunny.msh");
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
}
static void Redraw ()
{
  static bool first = true;
  if (first) {
    Init();
    first = false;
  }
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  int vp[4];
  glGetIntegerv(GL_VIEWPORT,vp);
  float xmin, xmax, ymin, ymax, zmin, zmax;
  bunny->GetBBox(&xmin, &xmax, &ymin, &ymax, &zmin, &zmax);
  float xcenter = (xmin+xmax) / 2;
  float ycenter = (ymin+ymax) / 2;
  float zcenter = (zmin+zmax) / 2;
  float zdist = 2*(zmax-zmin);


  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(50.0f, (float)vp[2]/vp[3], 0.1f, 100.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  VManipulator* manip = VManipulator::Instance();
  manip->SetZCenter(zdist);
  manip->Load();

  gluLookAt(xcenter, ycenter, zcenter-zdist,
            xcenter, ycenter, zcenter,
            0.0f, 1.0f, 0.0f
           );

  float pos[4] = {0.0f, 10.0f, 0.0f, 1.0f};
  glLightfv(GL_LIGHT0, GL_POSITION, pos);

  float white[4] = {1.0f, 1.0f, 1.0f, 1.0f};
  float red[4] = {1.0f, 0.1f, 0.1f, 1.0f};
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0f);
  bunny->Draw();

  glFlush();

  glutSwapBuffers();
}

static void Keyboard (unsigned char key, int x, int y)
{
  switch (key) {
  case 's': bunny->SetSmooth(true); break;
  case 'S': bunny->SetSmooth(false); break;
  case 'w': bunny->SetWeightedNormals(true); break;
  case 'W': bunny->SetWeightedNormals(false); break;
  case 'q': exit(0);
  }
  Redraw();
}

int main (int argc, char *argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
  glutInitWindowSize(800,800);
  glutCreateWindow("Vis3d - Bunny");
  glutDisplayFunc(Redraw);
  glutKeyboardFunc(Keyboard);
  glutMainLoop();
  return 0;
}
