#ifndef AMSAxis_H
#define AMSAxis_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Axis class                                                       //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TPolyLine3D
#include <TPolyLine3D.h>
#endif



class AMSAxis : public TPolyLine3D {

private:

   char          *m_Title;	//title of the axis (usually "x", "y", "z")

public:
                  AMSAxis() {;}
                  AMSAxis(Float_t end[3], char * title);
                  AMSAxis(Float_t x, Float_t y, Float_t z, char * title);
   virtual       ~AMSAxis() { delete m_Title;}
   virtual char  *GetObjectInfo(Int_t px, Int_t py);
   virtual void   Paint(Option_t *option="");

   ClassDef(AMSAxis, 0)   //AMS axis class
};

#endif
