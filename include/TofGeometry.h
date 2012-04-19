// $Id: TofGeometry.h,v 1.2 2012/04/19 16:11:45 oliva Exp $

#ifndef __TofGeometry__
#define __TofGeometry__

#include "point.h"

#include <string>
#include <vector>
#include <cmath>

//! ToF geometry 
/*!  
 *  ToF geometry informations. No alignment applied.
 */
class TofGeometry {

 public: 

  /** @name ToF Geometry functions from A. Contin.
    */
  /**@{*/
  //! Is X the transverse coordinate, i.e. is Y the longitudinal coordinate? (il = 0, ..., 3)
  static bool     IsTransverseX(int il);
  //! Get the average Z of a layer (il = 0, ..., 3)
  static float    GetZLayer(int il);
  //! Get the layer global thickness (from the bottom of lowest counter to top of highest counter) (il = 0, ..., 3)
  static float    GetLayerThickness(int il);
  //! Get the Z position of a counter (il = 0, ..., 3; ib = 0, ..., nbars(layer)-1)
  static float    GetZPaddle(int il, int ib);
  //! Get number of bars per layer (il = 0, ..., 3)
  static int      GetNBars(int il);
  //! Asking if a point is on a counter 
  static bool     HitCounter(float x, float y, int il, int ic, float delta = 0);
  //! Path length inside counter
  static float    LengthInsideCounter(int il, int ib, float x, float y, AMSDir dir);
  //! Determine the layer/bar index (ilayer*10 + ibar) for a given point (-1: not found, -n: n counters found (should be impossible)) 
  static int      GetTofPaddleIndex(AMSPoint point, float delta);
  //! Drawing TOF
  static void     DrawTof(int il);
  /**@}*/


  /** @name Trying to have more generic ToF Geometry functions
    */
  /**@{*/
  //! Get the paddle corner by index (ix = 0/1, iy = 0/1, iz = 0/1)  
  static AMSPoint GetPaddleCorner(int il, int ib, int ix, int iy, int iz);
  //! Get the paddle surface selected corned by index 
  static AMSPoint GetPaddleSurfaceCorner(int il, int ib, int isurface, int icorner);
  //! Test if a point is over a plane determined by three points 
  static bool     IsInsidePlane(AMSPoint x[3], AMSPoint point);
  //! Test if a point is inside this 4-points planar polygon
  static bool     IsInsideRectangle(AMSPoint x[4], AMSPoint point);
  //! Test if a point is inside a paddle surface
  static bool     IsInsideSurface(int il, int ib, int isurface, AMSPoint point);   
  //! Given a point and a direction calculate intersection point with a plane given by three points
  static AMSPoint GetIntersection(AMSPoint x[3], AMSPoint point, AMSDir dir); 
  //! Calculate the pathlength inside a counter
  static float    PathLengthInAPaddle(int il, int ib, AMSPoint point, AMSDir dir);
  /**@}*/

  ClassDef(TofGeometry,1); 
};

#endif
