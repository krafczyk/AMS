// $Id: glviewer.cpp,v 1.3 2010/12/10 21:38:01 shaino Exp $
#include "glviewer.h"
#include "glcamera.h"
#include "gllight.h"

#include <GL/gl.h>
#include <GL/glu.h>

GLViewer::GLViewer(int x, int y, int width, int height, int ssize) 
{
  bSize = 150;
  bX = bY = bZ = 0;
  bAx = 3;

  glLight  = new GLLight;
  glCamera = new GLCamera(GLVector3(-1.0, 0.0, 0.0), 
			  GLVector3(0.0, 0.0, 1.0), ssize);

  setViewport(x, y, width, height);
}

GLViewer::~GLViewer()
{
  delete glCamera;
}

void GLViewer::initGL()
{
  // GL initialisation
  glEnable (GL_LIGHTING);
  glEnable (GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable (GL_COLOR_MATERIAL);
  glEnable (GL_BLEND);
  glEnable (GL_MULTISAMPLE);

  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClearDepth(1.0);

  glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
  glMaterialf(GL_BACK, GL_SHININESS, 0.0);

  glPolygonMode(GL_FRONT, GL_FILL);
  glBlendFunc  (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

  float lma[4] = {0.5f, 0.5f, 1.f, 1.f};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lma);
  glLightModeli (GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

  glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
  glHint(GL_LINE_SMOOTH_HINT,  GL_NICEST);
}

void GLViewer::preDraw()
{
  GLfloat crgb[3] = { 0, 0.05, 0 };
  glClearColor(crgb[0], crgb[1], crgb[2], 0.8);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLViewer::preRender()
{
  glCamera->Apply();

  GLVector3 ev = glCamera->EyePoint()-glCamera->FrustumCenter();
  if (bAx == 1) glLight->setupLights(bX-ev.Mag()*0.6, bY, bZ, bSize);
  if (bAx == 2) glLight->setupLights(bX, bY-ev.Mag()*0.6, bZ, bSize);
  if (bAx == 3) glLight->setupLights(bX, bY, bZ-ev.Mag()*0.6, bSize);
}

void GLViewer::pickObject(int x, int y, int width, int height)
{
  glCamera->Apply(x, y, width, height);
}

void GLViewer::postRender()
{
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}

void GLViewer::postDraw()
{
}

void GLViewer::setViewport(int x, int y, int width, int height)
{
  if (vpX == x && vpY == y && vpW == width && vpH == height) return;

  vpX = x;
  vpY = y;
  vpW = width;
  vpH = height;
  glCamera->SetViewport(vpX, vpY, vpW, vpH);
}
