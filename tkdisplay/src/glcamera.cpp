// $Id: glcamera.cpp,v 1.2 2010/05/10 21:55:46 shaino Exp $
#include "glcamera.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include <cmath>

double GLCamera::gFOVMin     = 0.01;
double GLCamera::gFOVDefault = 30;
double GLCamera::gFOVMax     = 120.0;

unsigned int GLCamera::gFOVDeltaSens   = 500;
unsigned int GLCamera::gDollyDeltaSens = 500;

GLCamera::GLCamera(const GLVector3 & hAxis, const GLVector3 & vAxis,
		   double ssize)
  : fieldOfView(gFOVDefault), nearClip(0), farClip(0),
    dollyDefault(1.0), dollyDistance(1.0), vAxisMinAngle(0.01f),
    vpX(0), vpY(0), vpW(100), vpH(100), sceneSize(ssize)
{
   for (unsigned int i = 0; i < kPlanesPerFrustum; i++ )
      fFrustumPlanes[i].Set(1.0, 0.0, 0.0, 0.0);

   GLVertex3 origin;
   fCamBase.Set(origin, vAxis, hAxis);

  Setup();
  fCamTrans.MoveLF(1, dollyDefault);
}

void GLCamera::Reset()
{
  fieldOfView = gFOVDefault;

  fCamTrans.SetIdentity();
  fCamTrans.MoveLF(1, dollyDefault);
}

bool GLCamera::Zoom(int delta, bool mod1, bool mod2)
{
  if (AdjustAndClampVal(fieldOfView, gFOVMin, gFOVMax, delta, 
			gFOVDeltaSens, mod1, mod2)) return true;
  return false;
}

bool GLCamera::Truck(int xDelta, int yDelta, bool mod1, bool mod2)
{
  double lenMidClip = 0.5*(farClip+nearClip)
    *std::tan(0.5*fieldOfView*M_PI/180);

  double xstep = xDelta*lenMidClip/vpH;
  double ystep = yDelta*lenMidClip/vpH;

  xstep = AdjustDelta(xstep, 1.0, mod1, mod2);
  ystep = AdjustDelta(ystep, 1.0, mod1, mod2);

  fCamTrans.MoveLF(2, -xstep);
  fCamTrans.MoveLF(3, -ystep);

  return true;
}

bool GLCamera::Rotate(int xDelta, int yDelta, bool mod1, bool mod2)
{
  double vRotate = AdjustDelta(xDelta, 2*M_PI/vpW, mod1, mod2);
  double hRotate = AdjustDelta(yDelta,   M_PI/vpH, mod1, mod2);

  return RotateRad(hRotate, vRotate);
}

bool GLCamera::RotateRad(double hRotate, double vRotate)
{
  // Rotate camera around center.

  if (hRotate != 0.0) {
    GLVector3 fwd  = fCamTrans.GetBaseVec(1);
    GLVector3 lft  = fCamTrans.GetBaseVec(2);
    GLVector3 up   = fCamTrans.GetBaseVec(3);
    GLVector3 pos  = fCamTrans.GetTranslation();

    GLVector3 deltaT = pos-(pos*lft)*lft;
    double     deltaF = deltaT*fwd;
    double     deltaU = deltaT*up;

    // up vector lock
    GLVector3 zdir = fCamBase.GetBaseVec(3);
    fCamBase.RotateIP(fwd);
    double theta = std::acos(fwd*zdir);
    if(theta+hRotate < vAxisMinAngle)
      hRotate = vAxisMinAngle-theta;
    else if(theta+hRotate > M_PI-vAxisMinAngle)
      hRotate = M_PI-vAxisMinAngle-theta;

    fCamTrans.MoveLF(1, -deltaF);
    fCamTrans.MoveLF(3, -deltaU);
    fCamTrans.RotateLF(3, 1, hRotate);
    fCamTrans.MoveLF(3,  deltaU);
    fCamTrans.MoveLF(1,  deltaF);
  }
  if (vRotate != 0.0)
    fCamTrans.RotatePF(1, 2, -vRotate);

  return true;
}

bool GLCamera::Dolly(int delta, bool mod1, bool mod2)
{
  double step = AdjustDelta(delta, dollyDistance, mod1, mod2);
  if (step == 0) return false;

  fCamTrans.MoveLF(1, -step);
  return true;
}

void GLCamera::Apply(int pickx, int picky, int pickw, int pickh)
{
  glViewport(vpX, vpY, vpW, vpH);

  if(vpW == 0 || vpH == 0) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    return;
  }

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(fieldOfView, vpW/vpH, 1.0, 1000.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  GLMatrix  mx     = fCamBase*fCamTrans;
  GLVector3 pos    = mx.GetTranslation();
  GLVector3 fwd    = mx.GetBaseVec(1);
  GLVector3 center = pos-fwd;
  GLVector3 up     = fCamBase.GetBaseVec(3);

  gluLookAt(pos[0],    pos[1],    pos[2],
	    center[0], center[1], center[2],
	    up[0],     up[1],     up[2]);
  UpdateCache();

  GLPlane clipPlane(EyeDirection(), EyePoint());
  for (int i = 0; i < 8; i++) {
    double sx = (i&1) ? -1 : 1;
    double sy = (i&2) ? -1 : 1;
    double sz = (i&4) ? -1 : 1;
    double cdist = clipPlane.DistanceTo(GLVertex3(sceneSize*sx, 
						   sceneSize*sy, 
						   sceneSize*sz));
    if (i == 0) farClip = nearClip = cdist;
    if (cdist < nearClip) nearClip = cdist;
    if (cdist >  farClip) farClip  = cdist;
  }
  nearClip *= 0.49;
  farClip  *= 2.01;
  if (farClip < 2.0) farClip = 2.0;
  if (nearClip < farClip/1000.0) nearClip = farClip/1000.0;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  if (pickw > 0 && pickh > 0) {
    int vport[4] = { vpX, vpY, vpW, vpH };
    gluPickMatrix(pickx, vpH-picky, pickw, pickh, vport);
    gluPerspective(fieldOfView, vpW/vpH, nearClip, farClip);
  }
  else
    gluPerspective(fieldOfView, vpW/vpH, nearClip, farClip);

  glMatrixMode(GL_MODELVIEW);

  UpdateCache();
}

void GLCamera::Configure(double fov, double dolly, double center[3],
                                     double hRotate, double vRotate)
{
  fieldOfView = fov;

  if      (fieldOfView > 170.0) fieldOfView = 170.0;
  else if (fieldOfView <   0.1) fieldOfView =   0.1;

  SetCenterVec(center[0], center[1], center[2]);
  fCamTrans.MoveLF(1, dolly);
  RotateRad(hRotate, vRotate);
}

void GLCamera::Setup(bool reset)
{
  SetCenterVec(0, 0, 0);

  double size = sceneSize*2*std::sqrt(2);
  double fov  = std::min(gFOVDefault, gFOVDefault*vpW/vpH);

  dollyDefault  = size/(2.0*std::tan(fov*M_PI/360));
  dollyDistance = 0.002*dollyDefault;

  if (reset) Reset();
}

void GLCamera::UpdateCache()
{
  GLMatrix projm;
  glGetDoublev(GL_PROJECTION_MATRIX, projm.Arr());

  GLMatrix modvm;
  glGetDoublev(GL_MODELVIEW_MATRIX,  modvm.Arr());

  GLMatrix clipm = projm;
  clipm *= modvm;

  fFrustumPlanes[kRight] .Set(clipm[ 3]-clipm[ 0], clipm[ 7]-clipm[ 4],
			      clipm[11]-clipm[ 8], clipm[15]-clipm[12]);
  fFrustumPlanes[kLeft]  .Set(clipm[ 3]+clipm[ 0], clipm[ 7]+clipm[ 4],
			      clipm[11]+clipm[ 8], clipm[15]+clipm[12]);
  fFrustumPlanes[kBottom].Set(clipm[ 3]+clipm[ 1], clipm[ 7]+clipm[ 5],
			      clipm[11]+clipm[ 9], clipm[15]+clipm[13]);
  fFrustumPlanes[kTop]   .Set(clipm[ 3]-clipm[ 1], clipm[ 7]-clipm[ 5],
			      clipm[11]-clipm[ 9], clipm[15]-clipm[13]);
  fFrustumPlanes[kFar]   .Set(clipm[ 3]-clipm[ 2], clipm[ 7]-clipm[ 6],
			      clipm[11]-clipm[10], clipm[15]-clipm[14]);
  fFrustumPlanes[kNear]  .Set(clipm[ 3]+clipm[ 2], clipm[ 7]+clipm[ 6],
			      clipm[11]+clipm[10], clipm[15]+clipm[14]);
}

GLVertex3 GLCamera::EyePoint()
{
  return Intersection(fFrustumPlanes[kRight], 
		      fFrustumPlanes[kLeft],
		      fFrustumPlanes[kTop]).second;
}

GLVector3 GLCamera::EyeDirection()
{
  return fFrustumPlanes[kNear].Norm();
}


GLVertex3 GLCamera::FrustumCenter()
{
  std::pair<bool, GLVertex3> nearBottomLeft 
    = Intersection(fFrustumPlanes[kNear],
		   fFrustumPlanes[kBottom],
		   fFrustumPlanes[kLeft]);
  std::pair<bool, GLVertex3> farTopRight
    = Intersection(fFrustumPlanes[kFar],
		   fFrustumPlanes[kTop],
		   fFrustumPlanes[kRight]);

  if (!nearBottomLeft.first || !farTopRight.first)
    return GLVertex3(0.0, 0.0, 0.0);
  
  return nearBottomLeft.second+(farTopRight.second-nearBottomLeft.second)/2.0;
}


bool GLCamera::AdjustAndClampVal(double & val, double min, double max,
				 int screenShift, int screenShiftRange,
				 bool mod1, bool mod2)
{
  if (screenShift == 0) {
    return false;
  }

  // Calculate a sensitivity based on passed modifiers
  double sens = val*static_cast<double>(screenShift);

  if (mod1) {
    sens *= 0.1;
    if (mod2) sens *= 0.1;
  }
  else if (mod2) sens *= 10.0;

  double oldVal = val;
  double shift  = sens/static_cast<double>(screenShiftRange);
  val -= shift;

  if      (val < min) val = min;
  else if (val > max) val = max;

  return val != oldVal;
}

double GLCamera::AdjustDelta(double screenShift, double deltaFactor,
			     bool mod1, bool mod2)
{
  if (screenShift == 0) return 0;

  double sens = 1.0;

  if (mod1) {
    sens *= 0.1;
    if (mod2) sens *= 0.1;
  }
  else if (mod2) sens *= 10.0;

  return sens*deltaFactor*screenShift;
}

void GLCamera::SetCenterVec(double x, double y, double z)
{
  // Set camera center vector.

  GLMatrix bt = fCamBase*fCamTrans;
  fCamBase.SetBaseVec(4, GLVector3(x, y, z));

  GLMatrix binv = fCamBase; binv.Invert();
  fCamTrans = binv*bt;
}


