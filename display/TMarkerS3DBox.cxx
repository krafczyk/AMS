//  $Id: TMarkerS3DBox.cxx,v 1.2 2001/01/22 17:32:33 choutko Exp $
//*CMZ :  2.00/00 17/12/97  15.57.06  by  Rene Brun
//*CMZ :  1.03/09 07/12/97  16.28.37  by  Fons Rademakers
//*-- Author :    "Valery fine"   31/10/97

//*KEEP,CopyRight,T=C.
/*************************************************************************
 * Copyright(c) 1995-1998, The ROOT System, All rights reserved.         *
 * Authors: Rene Brun, Nenad Buncic, Valery Fine, Fons Rademakers.       *
 *                                                                       *
 * Permission to use, copy, modify and distribute this software and its  *
 * documentation for non-commercial purposes is hereby granted without   *
 * fee, provided that the above copyright notice appears in all copies   *
 * and that both the copyright notice and this permission notice appear  *
 * in the supporting documentation. The authors make no claims about the *
 * suitability of this software for any purpose.                         *
 * It is provided "as is" without express or implied warranty.           *
 *************************************************************************/
//*KEND.

#include <fstream.h>
#include <iostream.h>

//*KEEP,TROOT.
#include "TROOT.h"
//*KEEP,TView.
#include "TView.h"
//*KEEP,TMarkerS3DBox,T=C++.
#include "TMarkerS3DBox.h"
//*KEEP,TVirtualPad.
#include "TVirtualPad.h"
//*KEEP,TGLKernelABC,T=C++.
#include "TGLKernelABC.h"
//*KEEP,TPadView3D,T=C++.
#include "TPadView3D.h"
//*KEND.

const Int_t kMarker3DBox = 91;

ClassImp(TMarkerS3DBox)

//______________________________________________________________________________
// Marker3DBox is a special 3-D marker designed for event display.
// It has the following parameters:
//    fX;               X coordinate of the center of the box
//    fY;               Y coordinate of the center of the box
//    fZ;               Z coordinate of the center of the box
//    fDx;              half length in X
//    fDy;              half length in Y
//    fDz;              half length in Z
//    fTheta;           Angle of box z axis with respect to main Z axis
//    fPhi;             Angle of box x axis with respect to main Xaxis
//    fRefObject;       A reference to an object
//


//______________________________________________________________________________
 TMarkerS3DBox::TMarkerS3DBox()
{
//*-*-*-*-*-*-*-*-*-*-*-*-*Marker3DBox  default constructor*-*-*-*-*-*-*-*-*-*
//*-*                      ================================

   fRefObject = 0;
}

//______________________________________________________________________________
 TMarkerS3DBox::TMarkerS3DBox( Float_t x, Float_t y, Float_t z,
                            Float_t dx, Float_t dy, Float_t dz,
                            Float_t theta, Float_t phi)
              :TAttLine(1,1,1), TAttFill(1,1)
{
//*-*-*-*-*-*-*-*-*-*-*-*-*Marker3DBox normal constructor*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                      =============================

    fDx = dx;
    fDy = dy;
    fDz = dz;

    fX  = x;
    fY  = y;
    fZ  = z;

    fTheta = theta;
    fPhi   = phi;
    fRefObject = 0;
}

//______________________________________________________________________________
 TMarkerS3DBox::~TMarkerS3DBox()
{
//*-*-*-*-*-*-*-*-*-*-*-*-*Marker3DBox shape default destructor*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                      =============================

}

//______________________________________________________________________________
 Int_t TMarkerS3DBox::DistancetoPrimitive(Int_t px, Int_t py)
{
//*-*-*-*-*-*-*-*Compute distance from point px,py to a Marker3DBox*-*-*-*-*
//*-*            ==================================================
//*-*
//*-*  Compute the closest distance of approach from point px,py to each corner
//*-*  point of the Marker3DBox.
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

   const Int_t numPoints = 8;
   Int_t dist = 9999;
   Float_t points[3*numPoints];

   TView *view = gPad->GetView();
   if (!view) return dist;
   const Int_t seg1[12] = {0,1,2,3,4,5,6,7,0,1,2,3};
   const Int_t seg2[12] = {1,2,3,0,5,6,7,4,4,5,6,7};

   SetPoints(points);

   Int_t i, i1, i2, dsegment;
   Float_t x1,y1,x2,y2, xndc[3];
   for (i = 0; i < 12; i++) {
      i1 = 3*seg1[i];
      view->WCtoNDC(&points[i1], xndc);
      x1 = xndc[0];
      y1 = xndc[1];

      i2 = 3*seg2[i];
      view->WCtoNDC(&points[i2], xndc);
      x2 = xndc[0];
      y2 = xndc[1];
      dsegment = DistancetoLine(px,py,x1,y1,x2,y2);
      if (dsegment < dist) dist = dsegment;
   }
   if (dist < 5) {
      gPad->SetCursor(kCross);
      if (fRefObject) {gPad->SetSelected(fRefObject); return 0;}
   }
   return dist;
}

//______________________________________________________________________________
 void TMarkerS3DBox::Paint(Option_t *option)
{
//*-*-*-*-*-*-*-*Paint this 3-D box marker with its current attributes*-*-*-*
//*-*            =====================================================

   const Int_t numpoints = 8;

//*-* Allocate memory for points *-*

   Float_t *points = new Float_t[3*numpoints];
   if (!points) return;

   SetPoints(points);

#ifdef NEVER
   Double_t rotate[16] = {fDirCos[0],      0,        0,     fTranslation[0],
                               0    ,  fDirCos[1],  0,      fTranslation[1],
                               0    ,      0,   fDirCos[2], fTranslation[2],
                               0    ,      0,       0,           1
                          };
   if (gPad->GetView3D()) {
     gGLKernel->UpdateMatrix(0,rotate);
     gGLKernel->SetLineAttr(GetLineColor(),GetLineWidth());
     PaintGLPoints(points);
   }
#endif

   Int_t c = ((GetLineColor() % 8) - 1) * 4;  // Basic colors: 0, 1, ... 7
   if (c < 0) c = 0;

//*-* Allocate memory for segments *-*

    X3DBuffer *buff = new X3DBuffer;
    if (buff) {
        buff->numPoints = 8;
        buff->numSegs   = 12;
        buff->numPolys  = 6;
    }

//*-* Allocate memory for points *-*

    buff->points = points;
    buff->segs = new Int_t[buff->numSegs*3];
    if (buff->segs) {
        buff->segs[ 0] = c;    buff->segs[ 1] = 0;    buff->segs[ 2] = 1;
        buff->segs[ 3] = c+1;  buff->segs[ 4] = 1;    buff->segs[ 5] = 2;
        buff->segs[ 6] = c+1;  buff->segs[ 7] = 2;    buff->segs[ 8] = 3;
        buff->segs[ 9] = c;    buff->segs[10] = 3;    buff->segs[11] = 0;
        buff->segs[12] = c+2;  buff->segs[13] = 4;    buff->segs[14] = 5;
        buff->segs[15] = c+2;  buff->segs[16] = 5;    buff->segs[17] = 6;
        buff->segs[18] = c+3;  buff->segs[19] = 6;    buff->segs[20] = 7;
        buff->segs[21] = c+3;  buff->segs[22] = 7;    buff->segs[23] = 4;
        buff->segs[24] = c;    buff->segs[25] = 0;    buff->segs[26] = 4;
        buff->segs[27] = c+2;  buff->segs[28] = 1;    buff->segs[29] = 5;
        buff->segs[30] = c+1;  buff->segs[31] = 2;    buff->segs[32] = 6;
        buff->segs[33] = c+3;  buff->segs[34] = 3;    buff->segs[35] = 7;
    }

//*-* Allocate memory for polygons *-*

    buff->polys = new Int_t[buff->numPolys*6];
    if (buff->polys) {
        buff->polys[ 0] = c;   buff->polys[ 1] = 4;  buff->polys[ 2] = 0;
        buff->polys[ 3] = 9;   buff->polys[ 4] = 4;  buff->polys[ 5] = 8;
        buff->polys[ 6] = c+1; buff->polys[ 7] = 4;  buff->polys[ 8] = 1;
        buff->polys[ 9] = 10;  buff->polys[10] = 5;  buff->polys[11] = 9;
        buff->polys[12] = c;   buff->polys[13] = 4;  buff->polys[14] = 2;
        buff->polys[15] = 11;  buff->polys[16] = 6;  buff->polys[17] = 10;
        buff->polys[18] = c+1; buff->polys[19] = 4;  buff->polys[20] = 3;
        buff->polys[21] = 8;   buff->polys[22] = 7;  buff->polys[23] = 11;
        buff->polys[24] = c+2; buff->polys[25] = 4;  buff->polys[26] = 0;
        buff->polys[27] = 3;   buff->polys[28] = 2;  buff->polys[29] = 1;
        buff->polys[30] = c+3; buff->polys[31] = 4;  buff->polys[32] = 4;
        buff->polys[33] = 5;   buff->polys[34] = 6;  buff->polys[35] = 7;
    }

    //*-* Paint in the pad
    PaintShape(buff);

    if (strstr(option, "x3d")) {
        if(buff && buff->points && buff->segs)
            FillX3DBuffer(buff);
        else {
            gSize3D.numPoints -= buff->numPoints;
            gSize3D.numSegs   -= buff->numSegs;
            gSize3D.numPolys  -= buff->numPolys;
        }
    }

    delete [] points;
    if (buff->segs)     delete [] buff->segs;
    if (buff->polys)    delete [] buff->polys;
    if (buff)           delete    buff;

}

//______________________________________________________________________________
 void TMarkerS3DBox::PaintGLPoints(Float_t *vertex)
{
//*-*-*-*-*-*-*-*-*-*-*Direct copy of TBRIK::PaintGLPoints*-*-*-*-*-*-*-*-*-*
//*-*                  ===================================
    gGLKernel->PaintBrik(vertex);
}

//______________________________________________________________________________
 void TMarkerS3DBox::PaintShape(X3DBuffer *buff)
{
//*-*-*-*-*Paint 3-D marker in current pad with its current attributes*-*-*-*-*
//*-*      ==========================================================

    //*-* Paint in the pad

    Float_t points[6];
    int i0;

    TAttLine::Modify();  //Change line attributes only if necessary
    TAttFill::Modify();  //Change fill area attributes only if necessary

    for (Int_t i = 0; i < buff->numSegs; i++) {
        i0 = 3*buff->segs[3*i+1];
        points[0] = buff->points[i0++];
        points[1] = buff->points[i0++];
        points[2] = buff->points[i0];

        i0 = 3*buff->segs[3*i+2];
        points[3] = buff->points[i0++];
        points[4] = buff->points[i0++];
        points[5] = buff->points[i0];

        gPad->PaintLine3D(&points[0], &points[3]);
    }
}

//______________________________________________________________________________
 void TMarkerS3DBox::SavePrimitive(ofstream &out, Option_t *)
{
    // Save primitive as a C++ statement(s) on output stream out

   out<<"   "<<endl;
   if (gROOT->GetCount(kMarker3DBox)) {
       out<<"   ";
   } else {
       out<<"   TMarkerS3DBox *";
       gROOT->SetCount(kMarker3DBox,1);
   }
   out<<"marker3DBox = new TMarkerS3DBox("<<fX<<","
                                         <<fY<<","
                                         <<fZ<<","
                                         <<fDx<<","
                                         <<fDy<<","
                                         <<fDz<<","
                                         <<fTheta<<","
                                         <<fPhi<<");"<<endl;

   SaveLineAttributes(out,"marker3DBox",1,1,1);
   SaveFillAttributes(out,"marker3DBox",1,0);

   out<<"   marker3DBox->Draw();"<<endl;
}

//______________________________________________________________________________
 void TMarkerS3DBox::SetDirection(Float_t theta, Float_t phi)
{
  fTheta = theta;
  fPhi   = phi;
}

//______________________________________________________________________________
 void TMarkerS3DBox::SetSize(Float_t dx, Float_t dy, Float_t dz)
{
  fDx = dx;
  fDy = dy;
  fDz = dz;
}

//______________________________________________________________________________
 void TMarkerS3DBox::SetPosition(Float_t x, Float_t y, Float_t z)
{
  fX  = x;
  fY  = y;
  fZ  = z;
}

//______________________________________________________________________________
 void TMarkerS3DBox::SetPoints(Float_t *buff)
{
//*-*-*-*-*-*-*-*-*-*-*Direct copy of TBRIK::SetPoints *-*-*-*-*-*-*-*-*-*-*
//*-*                  ===============================

    if (buff) {
        buff[ 0] = -fDx;    buff[ 1] = -fDy;    buff[ 2] = -fDz;
        buff[ 3] =  fDx;    buff[ 4] = -fDy;    buff[ 5] = -fDz;
        buff[ 6] =  fDx;    buff[ 7] =  fDy;    buff[ 8] = -fDz;
        buff[ 9] = -fDx;    buff[10] =  fDy;    buff[11] = -fDz;
        buff[12] = -fDx;    buff[13] = -fDy;    buff[14] =  fDz;
        buff[15] =  fDx;    buff[16] = -fDy;    buff[17] =  fDz;
        buff[18] =  fDx;    buff[19] =  fDy;    buff[20] =  fDz;
        buff[21] = -fDx;    buff[22] =  fDy;    buff[23] =  fDz;
    }

   Float_t x, y, z;
   const Double_t kPI = TMath::Pi();
   Float_t theta  = fTheta*kPI/180;
   Float_t phi    = fPhi*kPI/180;
   Double_t sinth = TMath::Sin(theta);
   Double_t costh = TMath::Cos(theta);
   Double_t sinfi = TMath::Sin(phi);
   Double_t cosfi = TMath::Cos(phi);

   //
   // Matrix to convert from fruit frame to master frame
   //
   Float_t M[9];
   M[0] =  costh * cosfi;       M[1] = -sinfi;          M[2] = sinth*cosfi;
   M[3] =  costh * sinfi;       M[4] =  cosfi;          M[5] = sinth*sinfi;
   M[6] = -sinth;               M[7] =  0;              M[8] = costh;

   for (Int_t i = 0; i < 8; i++) {
      x = buff[3*i];
      y = buff[3*i+1];
      z = buff[3*i+2];

      buff[3*i]   = fX + M[0] * x + M[1] * y + M[2] * z;
      buff[3*i+1] = fY + M[3] * x + M[4] * y + M[5] * z;
      buff[3*i+2] = fZ + M[6] * x + M[7] * y + M[8] * z;

// printf("i=%d, x=%f, y=%f, z=%f, xp=%f, yp=%f, zp=%f\n",
//       i,x,y,z,buff[3*i],buff[3*i+1],buff[3*i+2]);
   }
}

//______________________________________________________________________________
 void TMarkerS3DBox::Sizeof3D() const
{
//*-*-*-*-*-*-*Return total X3D size of this marker*-*-*-*-*-*-*-*
//*-*          ====================================

    gSize3D.numPoints += 8;
    gSize3D.numSegs   += 12;
    gSize3D.numPolys  += 6;
}

