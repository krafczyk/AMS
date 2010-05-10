// $Id: gllight.h,v 1.2 2010/05/10 21:55:46 shaino Exp $
//
// GLLight : a class to manage OpenGL lights
//           imported from ROOT:TGLLightSet and arranged by SH
//
#ifndef GLLIGHT_H
#define GLLIGHT_H

class GLCamera;

namespace gllight {
  enum ELight { Light_FR = 0x0001, Light_TP = 0x0002,
		Light_BT = 0x0004, Light_LF = 0x0008,
		Light_RT = 0x0010, Light_MS = 0x001f, 
		Light_SP = 0x0100 };
};

using namespace gllight;

class GLLight {

public:
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
