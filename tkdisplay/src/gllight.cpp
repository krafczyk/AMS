// $Id: gllight.cpp,v 1.1 2009/06/13 21:40:47 shaino Exp $
#include "gllight.h"

#include <GL/gl.h>
#include <GL/glu.h>

GLLight::GLLight()
{
  lightState  = kLightMask;
  useSpecular = true;
}

void GLLight::toggleLight(ELight light)
{
  if (light == kLightSpecular) useSpecular = !useSpecular;
  else if (light < kLightMask) lightState ^= light;
}

void GLLight::setLight(ELight light, bool on)
{
  if (light == kLightSpecular) useSpecular = on;

  else if (light < kLightMask) {
   if (on) lightState |=  light;
   else    lightState &= ~light;
  }
}

void GLLight::setupLights(double x, double y, double z, double r)
{
  glPushMatrix();
  glLoadIdentity();

  float pos0[4] = { x,   y,   z+r, 1 };
  float pos1[4] = { x,   y+r, z,   1 };
  float pos2[4] = { x,   y-r, z,   1 };
  float pos3[4] = { x-r, y,   z,   1 };
  float pos4[4] = { x+r, y,   z,   1 };

  const float cfront [4] = { 0.4, 0.4, 0.4, 1.0 };
  const float cside  [4] = { 0.7, 0.7, 0.7, 1.0 };
  const float spwhite[4] = { 0.8, 0.8, 0.8, 1.0 };
  const float spnull [4] = { 0.0, 0.0, 0.0, 1.0 };

  glLightfv(GL_LIGHT0, GL_POSITION, pos0);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, cfront);
  glLightfv(GL_LIGHT0, GL_SPECULAR, useSpecular ? spwhite : spnull);

  glLightfv(GL_LIGHT1, GL_POSITION, pos1);
  glLightfv(GL_LIGHT1, GL_DIFFUSE,  cside);
  glLightfv(GL_LIGHT2, GL_POSITION, pos2);
  glLightfv(GL_LIGHT2, GL_DIFFUSE,  cside);
  glLightfv(GL_LIGHT3, GL_POSITION, pos3);
  glLightfv(GL_LIGHT3, GL_DIFFUSE,  cside);
  glLightfv(GL_LIGHT4, GL_POSITION, pos4);
  glLightfv(GL_LIGHT4, GL_DIFFUSE,  cside);

  for (unsigned i = 0; (1<<i) < kLightMask; i++) {
    if ((1<<i) & lightState) glEnable (GLenum(GL_LIGHT0+i));
    else                     glDisable(GLenum(GL_LIGHT0+i));
  }

  glPopMatrix();
}
