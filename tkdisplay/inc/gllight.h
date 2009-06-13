// $Id: gllight.h,v 1.1 2009/06/13 21:40:47 shaino Exp $
//
// GLLight : a class to manage OpenGL lights
//           imported from ROOT:TGLLightSet and arranged by SH
//
#ifndef GLLIGHT_H
#define GLLIGHT_H

class GLCamera;

class GLLight {

public:
  enum ELight { kLightFront    = 0x0001,
		kLightTop      = 0x0002,
		kLightBottom   = 0x0004,
		kLightLeft     = 0x0008,
		kLightRight    = 0x0010,
		kLightMask     = 0x001f,
		kLightSpecular = 0x0100 };
protected:
  int  lightState;
  bool useSpecular;

public:
  GLLight();

  void toggleLight(ELight light);
  void setLight   (ELight light, bool on);
  unsigned int getLightState() { return lightState; }

  bool getUseSpecular() const { return useSpecular; }
  void setUseSpecular(bool s) { useSpecular = s; }

  void setupLights(double x, double y, double z, double r);
};

#endif
