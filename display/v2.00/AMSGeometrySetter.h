//  $Id: AMSGeometrySetter.h,v 1.2 2001/01/22 17:32:37 choutko Exp $
#ifndef AMSGeometrySetter_H
#define AMSGeometrySetter_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSGeometrySetter                                                    //
//                                                                      //
// Utility class to set AMS geometry display options.                   //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TGeometry_H
#include <TGeometry.h>
#endif
#include "TSwitch.h"

//
// AMS Geometry:
//
// Top node: 
// AMSG1
// 	TOF / TOF_HONEYCOMB, TOF_PMT, TOF_COUNTER, TOF_SCINTILATTOR
//	MAGNET / MAGN1, ALT12, ALT23, ALT34, ALT45
//	ANTI / ANTI_SUPP_TUBE, ANTI_WRAPPER
//	CTC / CTC_SUPPORT, CTC_AMP, 
//	      CTC_UPPER / CTC_UPPER_{WALL,CELL,GAP,PTF,AEROGEL,PMT}
//	      CTC_LOWER / CTC_LOWER_{WALL,CELL,GAP,PTF,AEROGEL,PMT} 
//	      CTC_EXTRA / CTC_EXTRA_{WALL,CELL,GAP,PTF,AEROGEL,PMT}
//			
//	TRACKER / STK11, STK22, STK33, STK44, STK55, STK66
//		   |
//		   --->  STK_L1_LADDER, STK_L1_FOAM, STK_L1_HONEYCOMB,
//			 STK_L1_ELECTRONICS
//	SHUTTLE / SHUTTLE_SHU, SHUTTLE_PSH, SHUTTLE_IRON
//
//
// The first design is to have AMSGeometry inherit from TGeometry.
// This doesn't work because there is no copy constructor of TGeometry.
//
// So I chose to let AMSGeometry have a pointer to a TGeometry that
// is read from a root file.


enum EVisibility {
  kDrawImmediateSonsOnly = -4,
  kDrawLeavesOnly        = -3,
  kDrawSelfOnly          = -2,
  kDrawNone              = -1,
  kDrawNoNode            = 0,
  kDrawNode              = 1,	// default
  kDrawSonsOnly          = 2,
  kDrawAll               = 3 
};


struct AMSGeoToggle {  
  char * name;  EVisibility vis; TSwitch * sw;  // TNode * node;
};

static const Int_t nToggle=6;

class AMSGeometrySetter {

protected:
   static AMSGeoToggle m_Toggle[];
   TGeometry   *m_Geometry;	// pointer to the geometry

public:
                AMSGeometrySetter();
                AMSGeometrySetter(TGeometry * geo);
               ~AMSGeometrySetter() {}

   void		TurnOn(char * name);
   void		TurnOnWithSons(char * name);
   void		TurnOff(char * name);
   void		TurnOffWithSons(char * name);

   void         CheckVisibility(char * name);
   void         SetGeo();

   ClassDef(AMSGeometrySetter, 0)   //Utility class to display AMS geometry
};

#endif
