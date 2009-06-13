// $Id: glviewer.h,v 1.1 2009/06/13 21:40:47 shaino Exp $
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

  double vpX;
  double vpY;
  double vpW;
  double vpH;

  GLLight  *glLight;
  GLCamera *glCamera;

public:
  GLViewer(int x, int y, int width, int height);
 ~GLViewer();

  GLCamera *getCamera() const { return glCamera; }
  GLLight  *getLight()  const { return glLight; }

  void setBSize(double size) { bSize = size; }
  void setBCen (double x, double y, double z) { bX = x; bY = y; bZ = z; }

  void initGL    ();
  void preDraw   ();
  void preRender ();
  void postRender();
  void postDraw  ();

  void pickObject (int x, int y, int width, int height);
  void setViewport(int x, int y, int width, int height);
};

#endif
