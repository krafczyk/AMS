#ifndef ROOT_TMarkerS3DBox
#define ROOT_TMarkerS3DBox

// Copyright (C) 1995 CodeCERN. All rights reserved.

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// TMarkerS3DBox                                                           //
//                                                                        //
// Marker3DBox is a special 3-D marker designed for event display.        //
// It has the following parameters:                                       //
//    fDx;              half length in X                                  //
//    fDy;              half length in Y                                  //
//    fDz;              half length in Z                                  //
//    fTranslation[3];  the coordinates of the center of the box          //
//    fDirCos[3];       the direction cosinus defining the orientation    //
//    fRefObject;       A reference to an object                          //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
//*KEEP,TObject.
#include "TObject.h"
//*KEND.
#endif
#ifndef ROOT_TAttLine
//*KEEP,TAttLine.
#include "TAttLine.h"
//*KEND.
#endif
#ifndef ROOT_TAttFill
//*KEEP,TAttFill.
#include "TAttFill.h"
//*KEND.
#endif
#ifndef ROOT_X3DBuffer
//*KEEP,X3DBuffer,T=C.
#include "X3DBuffer.h"
//*KEND.
#endif

class TMarkerS3DBox : public TObject, public TAttLine, public TAttFill {

protected:

    Float_t  fX;               // X coordinate of center of box
    Float_t  fY;               // Y coordinate of center of box
    Float_t  fZ;               // Z coordinate of center of box
    Float_t  fDx;              // half length in x
    Float_t  fDy;              // half length in y
    Float_t  fDz;              // half length in z

    Float_t  fTheta;           // Angle of box z axis with respect to main Z axis
    Float_t  fPhi;             // Angle of box x axis with respect to main Xaxis
    TObject *fRefObject;       // Pointer to an object

protected:
    virtual Int_t   DistancetoPrimitive(Int_t px, Int_t py);
    virtual void    PaintGLPoints(Float_t *buff);

public:
    TMarkerS3DBox();
    TMarkerS3DBox(Float_t x, Float_t y, Float_t z,
                 Float_t dx, Float_t dy, Float_t dz,
                 Float_t theta, Float_t phi);
    virtual        ~TMarkerS3DBox();

    TObject        *GetRefObject() {return fRefObject;}
    virtual void    GetDirection(Float_t &theta, Float_t &phi) {theta = fTheta; phi = fPhi;}
    virtual void    GetPosition(Float_t &x, Float_t &y, Float_t &z) {x=fX; y=fY, z=fZ;}
    virtual void    GetSize(Float_t &dx, Float_t &dy, Float_t &dz) {dx=fDx; dy=fDy; dz=fDz;}
    virtual Bool_t  Is3D() {return kTRUE;}

    virtual void    Paint(Option_t *option);
    virtual void    PaintShape(X3DBuffer *buff);
    virtual void    SavePrimitive(ofstream &out, Option_t *option);
    virtual void    SetPoints(Float_t *buff);
    virtual void    SetDirection(Float_t theta, Float_t phi);
    virtual void    SetPosition(Float_t x, Float_t y, Float_t z);
    virtual void    SetSize(Float_t dx, Float_t dy, Float_t dz);
    virtual void    SetRefObject(TObject *obj=0) {fRefObject = obj;}

    virtual void    Sizeof3D() const;

    ClassDef(TMarkerS3DBox,1)  //a special 3-D marker designed for event display
};

#endif
