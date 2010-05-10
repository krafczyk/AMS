// $Id: glcamera.h,v 1.2 2010/05/10 21:55:46 shaino Exp $
//
// GLCamera : a class to manage the view point of OpenGL display
//            imported from ROOT:TGLPerspectiveCamera and arranged by SH
//
#ifndef GLCAMERA_H
#define GLCAMERA_H

#include "glutils.h"

class GLCamera {

public:
   enum EFrustumPlane { kNear   = 0, kLeft = 1,
			kRight  = 2, kTop  = 3,
			kBottom = 4, kFar  = 5,
			kPlanesPerFrustum = 6 };

public:
  GLCamera(const GLVector3 &hAxis, const GLVector3 &vAxis,
	   double ssize = 100);

  void Reset();

  bool Zoom  (int delta,              bool mod1, bool mod2);
  bool Truck (int xDelta, int yDelta, bool mod1, bool mod2);
  bool Dolly (int delta,              bool mod1, bool mod2);
  bool Rotate(int xDelta, int yDelta, bool mod1, bool mod2);

  bool RotateRad(double hRotate, double vRotate);

  void Apply(int pickx = 0, int picky = 0, int pickw = 0, int pickh = 0);

  void Configure(double fov, double dolly, double center[3],
		 double hRotate, double vRotate);

  void SetViewport(double x, double y, double w, double h)
    { vpX = x; vpY = y; vpW = w; vpH = h; }
  
  GLVertex3 EyePoint();
  GLVector3 EyeDirection();
  GLVertex3 FrustumCenter();

protected:
  static double gFOVMin, gFOVDefault, gFOVMax;
  static unsigned int gFOVDeltaSens;
  static unsigned int gDollyDeltaSens;

  double fieldOfView;
  double nearClip;
  double farClip;

  double dollyDefault;
  double dollyDistance;
  double vAxisMinAngle;

  double vpX;
  double vpY;
  double vpW;
  double vpH;

  double sceneSize;

  GLMatrix fCamBase;
  GLMatrix fCamTrans;

  GLPlane fFrustumPlanes[kPlanesPerFrustum];

protected:
  void Setup(bool reset = true);

  void UpdateCache();

  bool AdjustAndClampVal(double & val, double min, double max,
			 int screenShift, int screenShiftRange,
			 bool mod1, bool mod2);
  double AdjustDelta(double screenShift, double deltaFactor,
		     bool mod1, bool mod2);

  void SetCenterVec(double x, double y, double z);
};

#endif
