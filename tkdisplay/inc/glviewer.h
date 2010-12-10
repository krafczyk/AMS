// $Id: glviewer.h,v 1.3 2010/12/10 21:38:01 shaino Exp $
//
// GLViewer : a class to manage GLCamera and GLLight
//            imported from ROOT:TGLViewer and arranged by SH
//
#ifndef GLVIEWER_H
#define GLVIEWER_H

class GLCamera;
class GLLight;

class GLViewer {

protected:
  double bSize;
  double bX;
  double bY;
  double bZ;
  int    bAx;

  double vpX;
  double vpY;
  double vpW;
  double vpH;

  GLLight  *glLight;
  GLCamera *glCamera;

public:
  GLViewer(int x, int y, int width, int height, int ssize = 250);
 ~GLViewer();

  GLCamera *getCamera() const { return glCamera; }
  GLLight  *getLight()  const { return glLight; }

  void setBSize(double size) { bSize = size; }
  void setBCen (double x, double y, double z, int ax = 3)
    { bX = x; bY = y; bZ = z; bAx = ax; }

  void initGL    ();
  void preDraw   ();
  void preRender ();
  void postRender();
  void postDraw  ();

  void pickObject (int x, int y, int width, int height);
  void setViewport(int x, int y, int width, int height);
};

#endif
