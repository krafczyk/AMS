//  $Id$
#ifndef AMSR_Axis_H
#define AMSR_Axis_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Axis class                                                       //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TPolyLine3D
#include <TPolyLine3D.h>
#endif



class AMSR_Axis : public TPolyLine3D {

private:

   char          *m_Title;	//title of the axis (usually "x", "y", "z")

public:
                  AMSR_Axis() {;}
                  AMSR_Axis(Float_t end[3], const char * title);
                  AMSR_Axis(Float_t x, Float_t y, Float_t z, const char * title);
   virtual       ~AMSR_Axis() { delete m_Title;}
   virtual char  *GetObjectInfo(Int_t px, Int_t py) const;
   virtual void   Paint(Option_t *option="");

};

#endif
