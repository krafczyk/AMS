// $Id: glviewer.cpp,v 1.1 2009/06/13 21:40:47 shaino Exp $
#include <QtOpenGL>

#include "glviewer.h"
#include "glcamera.h"
#include "gllight.h"

GLViewer::GLViewer(int x, int y, int width, int height) 
{
  bSize = 150;
  bX = bY = bZ = 0;

  glLight  = new GLLight;
  glCamera = new GLCamera(GLVector3(-1.0, 0.0, 0.0), 
			  GLVector3(0.0, 0.0, 1.0));

  setViewport(x, y, width, height);
}

GLViewer::~GLViewer()
{
  delete glCamera;
}

void GLViewer::initGL()
{
  // GL initialisation
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);

//glEnable(GL_CULL_FACE);
//glCullFace(GL_BACK);

  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClearDepth(1.0);
  glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);
  glMaterialf(GL_BACK, GL_SHININESS, 0.0);
  glPolygonMode(GL_FRONT, GL_FILL);
  glDisable(GL_BLEND);

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
  glLight->setupLights(bX, bY, bZ-ev.Mag()*0.6, bSize);
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
