//*CMZ :  1.01/07 26/06/97  10.09.08  by  Valery Fine(fine@vxcern.cern.ch)
//*-- Author :    Nenad Buncic   17/08/95

//*KEEP,CopyRight,T=C.
/************************************************************************
 * Copyright(c) 1995-1997, The ROOT System, All rights reserved.
 * Authors: Rene Brun, Nenad Buncic, Valery Fine, Fons Rademakers.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for non-commercial purposes is hereby granted without fee,
 * provided that the above copyright notice appears in all copies and
 * that both the copyright notice and this permission notice appear in
 * the supporting documentation. The authors make no claims about the
 * suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 ************************************************************************/
//*KEND.

#include <fstream.h>
#include <iostream.h>

//*KEEP,TROOT.
#include "TROOT.h"
//*KEEP,THelix.
#include "THelix.h"
//*KEEP,TVirtualPad.
#include "TVirtualPad.h"
//*KEEP,TPostScript.
#include "TPostScript.h"
//*KEEP,TGXW.
#include "TGXW.h"
//*KEEP,TPoint.
#include "TPoint.h"
//*KEEP,TGLKernelABC,T=C++.
#include "TGLKernelABC.h"
//*KEEP,TView.
#include "TView.h"
//*KEEP,TPadView3D,T=C++.
#include "TPadView3D.h"
//*KEND.

ClassImp(THelix)

//______________________________________________________________________________
// Helix is, hmmm, well, a helix.  It has XXX different constructors.
//
//   First one, without any parameters THelix(), we call 'default
// constructor' and it's used in a case that just an initialisation is
// needed (i.e. pointer declaration).
//
//       Example:
//                 THelix *pl1 = new THelix;
//
//
//   Second one is 'normal constructor' with 10 parameters,
//
//       Example:
//                 THelix pl1(x0,y0,z0,vx0,vy0,vz0,w,hx,hy,hz);
//
// Neglecting hx,hy,hz will initializes a helix with its axis in Z direction.
//
//
//   Third one is similar to the second one, except the initial position,
// initial velocity and helix axis are all given in array of 3 double
// precision numbers.
//
//       Example:
//                 THelix pl1(xyz0, v0, w, axis);
//
//


Int_t THelix::minNSeg=5;


//______________________________________________________________________________
 void  THelix::SetHelix(Double_t x,  Double_t y,  Double_t z,
			Double_t vx, Double_t vy, Double_t vz,
       			Double_t w, 
			Double_t r1, Double_t r2, EHelixRangeType rType,
			Double_t hx, Double_t hy, Double_t hz )
{
//****----*-*-*-*-*Set all helix parameters*-*-*-*-*-*-*-*-*-*-*
//*-*              ========================
//*-*

   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   // Define the helix frame by setting the helix axis and rotation matrix
   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   Double_t axis[3];	axis[0] = hx;	axis[1] = hy;	axis[2] = hz;
   SetAxis(axis);

   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   // Calculate initial position and velocity in helix frame
   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   fW    = w;
   Double_t * M = fRotMat->GetMatrix();
   Double_t vx0, vy0, vz0;
   vx0 = vx * M[0] + vy * M[1] + vz * M[2];
   vy0 = vx * M[3] + vy * M[4] + vz * M[5];
   vz0 = vx * M[6] + vy * M[7] + vz * M[8];
   fVt   = TMath::Sqrt(vx0*vx0 + vy0*vy0);
   fPhi0 = TMath::ATan2(vy0,vx0);
   fVz   = vz0;
   fX0   =  x * M[0] +  y * M[1] +  z * M[2];
   fY0   =  x * M[3] +  y * M[4] +  z * M[5];
   fZ0   =  x * M[6] +  y * M[7] +  z * M[8];
   if (fW != 0) {
     fX0 += fVt / fW * TMath::Sin(fPhi0);  
     fY0 -= fVt / fW * TMath::Cos(fPhi0);
   }

   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   // Then calculate the range in t and set the polyline representation
   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   if (rType != kUnchanged) {
     fRange[0] = 0.0;	fRange[1] = TMath::Pi();   // initialize to half round
     SetRange(r1,r2,rType);
   }

   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   // a test
   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   Double_t x0 = fX0 - fVt/fW * TMath::Sin(fPhi0);
   Double_t y0 = fY0 + fVt/fW * TMath::Cos(fPhi0);
   Double_t z0 = fZ0;


   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   // Done
   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   return;
}



//______________________________________________________________________________
 THelix::THelix()
{
//*-*-*-*-*-*-*-*-*-*-*-*-*Helix default constructor*-*-*-*-*-*-*-*-*-*-*
//*-*                      =========================

   fX0 = fY0 = fZ0 = fVt = fPhi0 = fVz = fAxis[0] = fAxis[1] = 0.0;
   fAxis[2]  = 1.0;
   fW        = 1.5E7;	// roughly the cyclon frequency of proton in AMS
   fRange[0] = 0.0;
   fRange[1] = 1.0;
   fRotMat   = 0;
}


//______________________________________________________________________________
 THelix::THelix(Double_t x,  Double_t y,  Double_t z,
                Double_t vx, Double_t vy, Double_t vz,
                Double_t w, 
                Double_t r1, Double_t r2, EHelixRangeType rType,
                Double_t hx, Double_t hy, Double_t hz, Option_t *option)
	: TPolyLine3D()
{
//*-*-*-*-*-*-*-*-*-*-*-*-*Helix normal constructor*-*-*-*-*-*-*-*-*-*-*-*
//*-*                      ========================
//*-*

   SetHelix(x,y,z, vx,vy,vz, w, r1, r2, rType, hx,hy,hz);
   fOption = option;
}


//______________________________________________________________________________
 THelix::THelix(Double_t * p, Double_t * v, Double_t w, 
		Double_t * range, EHelixRangeType rType, Double_t * axis,
		Option_t *option)
	: TPolyLine3D()
{
//*-*-*-*-*-*-*-*-*-*-*-*-*Helix normal constructor*-*-*-*-*-*-*-*-*-*-*-*
//*-*                      ========================
//*-*

   Double_t r[2]; 
   if ( range ) { 
     r[0] = range[0];	r[1] = range[1];
   } else {
     r[0] = 0.0;	r[1] = 1.0;
   }

   if ( axis ) {			// specify axis
     SetHelix(p[0],p[1],p[2], v[0], v[1], v[2], w, r[0], r[1], rType, 
		     axis[0], axis[1], axis[2]);
   } else {				// default axis
     SetHelix(p[0],p[1],p[2], v[0], v[1], v[2], w, r[0], r[1], rType);
   }

   fOption = option;
}

/*
//______________________________________________________________________________
 THelix::THelix(const THelix &h) : TPolyLine3D()
{
//*-*-*-*-*-*-*-*-*-*-*-*-*Helix copy constructor*-*-*-*-*-*-*-*-*-*-*-*
//*-*                      ======================
//*-*

   fX0   = h.fX0;
   fY0   = h.fY0;
   fZ0   = h.fZ0;
   fVt   = h.fVt;
   fPhi0 = h.fPhi0;
   fVz   = h.fVz;
   fW    = h.fW;
   for (Int_t i=0; i<3; i++) fAxis[i] = h.fAxis[i];
   fRotMat = new TRotMatrix(*(h.fRotMat));
   fRange[0] = h.fRange[0];
   fRange[1] = h.fRange[1];

   fOption = h.fOption;
}
*/

//______________________________________________________________________________
 THelix::~THelix()
{
//*-*-*-*-*-*-*-*-*-*-*-*-*Helix default destructor*-*-*-*-*-*-*-*-*-*-*-*
//*-*                      ========================

   if (fRotMat) delete fRotMat;
}


//______________________________________________________________________________
 THelix::THelix(const THelix &helix)
{
   ((THelix&)helix).Copy(*this);
}


//______________________________________________________________________________
 void THelix::Copy(TObject &obj)
{
//*-*-*-*-*-*-*-*-*-*-*-*-*Copy this helix to obj*-*-*-*-*-*-*-*-*-*-*-*
//*-*                      ======================

   TObject::Copy(obj);
   TAttLine::Copy(((THelix&)obj));

   ((THelix&)obj).fX0        = fX0;
   ((THelix&)obj).fY0        = fY0;
   ((THelix&)obj).fZ0        = fZ0;
   ((THelix&)obj).fVt        = fVt;
   ((THelix&)obj).fPhi0      = fPhi0;
   ((THelix&)obj).fVz        = fVz;
   ((THelix&)obj).fW         = fW;
   for (Int_t i=0; i<3; i++)    
     ((THelix&)obj).fAxis[i] = fAxis[i];

   if (((THelix&)obj).fRotMat)
      delete ((THelix&)obj).fRotMat;
   ((THelix&)obj).fRotMat    = new TRotMatrix(*fRotMat);

   ((THelix&)obj).fRange[0]  = fRange[0];
   ((THelix&)obj).fRange[1]  = fRange[1];

   ((THelix&)obj).fOption    = fOption;

   //
   // Set range and make the graphic representation
   //
   ((THelix&)obj).SetRange(fRange[0], fRange[1], kHelixT);
}


/*

//______________________________________________________________________________
 Int_t THelix::DistancetoPrimitive(Int_t px, Int_t py)
{
//*-*-*-*-*-*-*-*Compute distance from point px,py to a helix*-*-*-*-*-*-*
//*-*            ============================================
//*-*
//*-*  Compute the closest distance of approach from point px,py to each segment
//*-*  of the polyline.
//*-*  Returns when the distance found is below DistanceMaximum.
//*-*  The distance is computed in pixels units.
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

   const Int_t inaxis = 7;
   Int_t dist = 9999;

   Float_t x = gPad->AbsPixeltoX(px);
   Float_t y = gPad->AbsPixeltoY(py);

   Int_t puxmin = gPad->XtoAbsPixel(gPad->GetUxmin());
   Int_t puymin = gPad->YtoAbsPixel(gPad->GetUymin());
   Int_t puxmax = gPad->XtoAbsPixel(gPad->GetUxmax());
   Int_t puymax = gPad->YtoAbsPixel(gPad->GetUymax());

//*-*- return if point is not in the user area
   if (px < puxmin - inaxis) return dist;
   if (py > puymin + inaxis) return dist;
   if (px > puxmax + inaxis) return dist;
   if (py < puymax - inaxis) return dist;

   TView *view = gPad->GetTView();
   if (!view) return dist;

   Int_t i, dsegment;
   Float_t x1,y1,x2,y2, xndc[3];
   for (i=0;i<fN-1;i++) {
      view->WCtoNDC(&fP[3*i], xndc);
      x1 = xndc[0];
      y1 = xndc[1];
      view->WCtoNDC(&fP[3*i+3], xndc);
      x2 = xndc[0];
      y2 = xndc[1];
      dsegment = DistancetoLine(px,py,x1,y1,x2,y2);
      if (dsegment < dist) dist = dsegment;
   }
   return dist;
}
*/


//______________________________________________________________________________
 void THelix::Draw(Option_t *option)
{
//*-*-*-*-*-*-*-*Draw this helix with its current attributes*-*-*-*-*-*-*
//*-*            ===========================================

   AppendPad(option);

}

/*
//______________________________________________________________________________
 void THelix::DrawPolyLine(Int_t n, Float_t *p, Option_t *option)
{
//*-*-*-*-*-*-*-*-*Draw this 3-D polyline with new coordinates*-*-*-*-*-*-*-*-*-*
//*-*              ============================================

   THelix *newpolyline = new THelix();
   newpolyline->fN =n;
   newpolyline->fP = new Float_t[3*fN];
   for (Int_t i=0; i<3*fN;i++) { newpolyline->fP[i] = p[i];}
   TAttLine::Copy(*newpolyline);
   newpolyline->fOption = fOption;
   newpolyline->SetBit(kCanDelete);
   newpolyline->AppendPad(option);
}

//______________________________________________________________________________
 void THelix::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
//*-*-*-*-*-*-*-*-*-*Execute action corresponding to one event*-*-*-*-*-*-*-*-*-*
//*-*                =========================================


        if (gPad->GetTView())
                gPad->GetTView()->ExecuteRotateView(event, px, py);

}


//______________________________________________________________________________
 void THelix::ls(Option_t *option)
{
//*-*-*-*-*-*-*-*-*-*List this 3-D polyline with its attributes*-*-*-*-*-*-*
//*-*                ==========================================

   IndentLevel();
   cout <<"PolyLine3D  N=" <<fN<<" Option="<<option<<endl;

}

/* don't need
//______________________________________________________________________________
 void THelix::Paint(Option_t *option)
{
//*-*-*-*-*-*-*-*-*Paint this 3-D polyline with its current attributes*-*-*-*-*
//*-*              ===================================================
//*-*
//*-* Option could be 'x3d', and it means that output
//*-* will be performed by X3D package.
//*-*
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

    //*-* Check whether there is some 3D view class for this TPad
    TPadView3D *view3D = gPad->GetView3D();
    if (view3D) view3D->PaintPolyLine(this,option);

    //*-* Check if option is 'x3d'.      NOTE: This is a simple checking
    //                                         but since there is no other
    //                                         options yet, this works fine.
    if ((*option != 'x') && (*option != 'X')) {
        PaintPolyLine(fN, fP, option);
    }
    else {
        X3DBuffer *buff = new X3DBuffer;
        if (buff) {
            buff->numPoints = fN;
            buff->numSegs   = fN-1;
            buff->numPolys  = 0;        //NOTE: Because of different structure, our
            buff->polys     = NULL;     //      THelix can't use polygons
            buff->points    = fP;
        }

        Int_t c = ((GetLineColor() % 8) - 1) * 4;     // Basic colors: 0, 1, ... 8
        if (c < 0) c = 0;

    //*-* Allocate memory for segments *-*
        buff->segs = new Int_t[buff->numSegs*3];
        if (buff->segs) {
            for (Int_t i = 0; i < buff->numSegs; i++) {
                buff->segs[3*i  ] = c;
                buff->segs[3*i+1] = i;
                buff->segs[3*i+2] = i+1;
            }
        }


        if (buff && buff->points && buff->segs) //If everything seems to be OK ...
            FillX3DBuffer(buff);
        else {                            // ... something very bad was happened
            gSize3D.numPoints -= buff->numPoints;
            gSize3D.numSegs   -= buff->numSegs;
            gSize3D.numPolys  -= buff->numPolys;
        }

        if (buff->segs)     delete [] buff->segs;
        if (buff->polys)    delete [] buff->polys;
        if (buff)           delete    buff;
    }
}
*/

/*
//______________________________________________________________________________
 void THelix::PaintPolyLine(Int_t n, Float_t *p, Option_t *option)
{
//*-*-*-*-*-*-*-*-*Draw this 3-D polyline with new coordinates*-*-*-*-*-*-*-*-*-*
//*-*              ===========================================

   if (n < 1) return;

   TView *view = gPad->GetTView();
   if(!view) return;

   TAttLine::Modify();  //Change line attributes only if necessary

//*-*- special case if n=2
   Float_t xndc[3];
   Float_t xx[2],yy[2];
   if (n == 2) {
      view->WCtoNDC(p, xndc);
      xx[0] = xndc[0];
      yy[0] = xndc[1];
      view->WCtoNDC(p+3, xndc);
      xx[1] = xndc[0];
      yy[1] = xndc[1];
      gPad->PaintLine(xx[0],yy[0],xx[1],yy[1]);
      if (gCurrentPS) gCurrentPS->DrawPS(2, xx, yy);
      return;
   }

//*-*- Create temporary array to store array in pixel coordinates
   TPoint *pxy = new TPoint[n];
   Float_t *x  = new Float_t[n];
   Float_t *y  = new Float_t[n];
   Float_t *ptr = p;

//*-*- convert points from world to pixel coordinate


   for (Int_t i = 0; i < n; i++) {
      view->WCtoNDC(ptr, xndc);
      x[i] = xndc[0];
      y[i] = xndc[1];
      pxy[i].fX = gPad->XtoPixel(x[i]);
      pxy[i].fY = gPad->YtoPixel(y[i]);
      ptr += 3;
   }

//*-*- invoke the graphics subsystem
   if (!gPad->IsBatch()) gGXW->DrawPolyLine(n, pxy);

   delete [] pxy;

   if (gCurrentPS) {
      gCurrentPS->DrawPS(n, x, y);
   }
   delete [] x;
   delete [] y;

}
*/

//______________________________________________________________________________
 void THelix::Print(Option_t *option)
{
//*-*-*-*-*-*-*-*-*-*Dump this helix with its attributes*-*-*-*-*-*-*-*-*
//*-*                ===================================

   cout <<"    THelix Printing N=" <<fN<<" Option="<<option<<endl;

}

//______________________________________________________________________________
 void THelix::SavePrimitive(ofstream &out, Option_t *option)
{
    // Save primitive as a C++ statement(s) on output stream out

   char quote = '"';
   out<<"   "<<endl;
   if (gROOT->GetCount(kPolyLine3D)) {
       out<<"   ";
   } else {
       out<<"   THelix *";
       gROOT->SetCount(kPolyLine3D,1);
   }
   out<<"helix = new THelix("<<fX0<<","<<fY0<<","<<fZ0<<","
      <<fVt*TMath::Cos(fPhi0)<<","<<fVt*TMath::Sin(fPhi0)<<","<<fVz<<","
      <<fW<<","<<fRange[0]<<","<<fRange[1]<<","<<kHelixT<<","
      <<fAxis[0]<<","<<fAxis[1]<<","<<fAxis[2]<<","
      <<quote<<fOption<<quote<<");"<<endl;

   SaveLineAttributes(out,"helix",1,1,1);

   out<<"   helix->Draw();"<<endl;
}

//______________________________________________________________________________
 void THelix::SetAxis(Double_t * axis)
{
   // set a new axis for the helix.  This will make a new rotation matrix
   //

   Double_t len = TMath::Sqrt(axis[0]*axis[0] + axis[1]*axis[1] + axis[2]*axis[2]);
   if (len <= 0) { 
     Error("SetAxis()", "Impossible! axis length %lf <= 0!", len);
     return; 
   }

   fAxis[0] = axis[0]/len;
   fAxis[1] = axis[1]/len;
   fAxis[2] = axis[2]/len;

   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   // Construct the rotational matrix from the axis
   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   SetRotMatrix();

   Double_t * M = fRotMat->GetMatrix();
// printf(" matrix:  %lf   %lf   %lf\n", M[0], M[1], M[2]);
// printf("          %lf   %lf   %lf\n", M[3], M[4], M[5]);
// printf("          %lf   %lf   %lf\n", M[6], M[7], M[8]);
}

//______________________________________________________________________________
 void THelix::SetAxis(Double_t x, Double_t y, Double_t z)
{
   Double_t axis[3];	axis[0] = x;	axis[1] = y;	axis[2] = z;
   SetAxis(axis);
}


//______________________________________________________________________________
 void THelix::SetRange(Double_t * range, EHelixRangeType rType)
{
   // set a new range for the helix.  This will remake the polyline.
   //

   Double_t a[2];
   Double_t halfpi = TMath::Pi()/2.0;
   Int_t i;
   Double_t vx = fVt * TMath::Cos(fPhi0);
   Double_t vy = fVt * TMath::Sin(fPhi0);
   Double_t phase;

   if ( fW != 0 && fVz != 0 ) {		// general case
     switch ( rType ) {
       case kHelixT :
		fRange[0] = range[0];  fRange[1] = range[1];  break;

       case kHelixX :  
		for (i=0; i<2; i++ ) {
		  a[i] = fW / fVt * (range[i] - fX0);
		  if ( a[i] < -1 || a[i] > 1 ) {
		    Error("SetRange()", 
		       "range out of bound (%lf:%lf): %lf.  Default used: %lf", 
		        fX0-fVt/fW, fX0+fVt/fW, range[i], fRange[i]);
		    return;
		  }
		  phase = FindClosestPhase(fPhi0+halfpi, a[i]);
		  fRange[i] = ( fPhi0 + halfpi - phase ) / fW;
		}
		break;

       case kHelixY :  
		for (i=0; i<2; i++ ) {
		  a[i] = fW / fVt * (range[i] - fY0);
		  if ( a[i] < -1 || a[i] > 1 ) {
		    Error("SetRange()", 
		       "range out of bound (%lf:%lf): %lf.  Default used: %lf", 
		        fY0-fVt/fW, fY0+fVt/fW, range[i], fRange[i]);
		    return;
		  }
		  phase = FindClosestPhase(fPhi0, a[i]);
		  fRange[i] = ( fPhi0 - phase ) / fW;
		}
		break;

       case kHelixZ :
		if ( fVz != 0 ) {
		  for (i=0; i<2; i++ ) {  
		    fRange[i] = (range[i] - fZ0) / fVz; 
		  }
		} else {		// fVz = 0, z = constant = fZ0
		  Error("SetRange()", 
			"Vz = 0 and attempts to set range along helix axis!");
		  return;
		}
		break;

       case kLabX :	
       case kLabY :
       case kLabZ :
		printf("setting range in lab axes is not implemented yet\n");
		break;
       default:
		Error("SetRange()","unknown range type %d", rType);
		break;
     }
   }

   else if ( fW == 0 ) {		// straight line: x = x0 + vx * t
     switch ( rType ) {
       case kHelixT :
		fRange[0] = range[0];  fRange[1] = range[1];  break;
       case kHelixX :
		if ( vx != 0 ) {
		  fRange[0] = (range[0] - fX0) / vx;
		  fRange[1] = (range[1] - fX0) / vx;
		} else {
		  Error("SetRange()", 
			"Vx = 0 and attempts to set range on helix x axis!");
		  return;
		}
		break;
       case kHelixY :
		if ( vy != 0 ) {
		  fRange[0] = (range[0] - fY0) / vy;
		  fRange[1] = (range[1] - fY0) / vy;
		} else {
		  Error("SetRange()", 
			"Vy = 0 and attempts to set range on helix y axis!");
		  return;
		}
		break;
       case kHelixZ :
		if ( fVz != 0 ) {
		  fRange[0] = (range[0] - fZ0) / fVz;
		  fRange[1] = (range[1] - fZ0) / fVz;
		} else {
		  Error("SetRange()", 
			"Vz = 0 and attempts to set range on helix z axis!");
		  return;
		}
		break;
       case kLabX   :
       case kLabY   :
       case kLabZ   :
		printf("setting range in lab axes is not implemented yet\n");
		break;
       default      :
		Error("SetRange()","unknown range type %d", rType);
		break;
     }
   }

   else if ( fVz == 0 ) {		// a circle, not fully implemented yet
     switch ( rType ) {
       case kHelixT :
		fRange[0] = range[0];  fRange[1] = range[1];  break;
       case kHelixX :
		if ( vx != 0 ) {
		  fRange[0] = (range[0] - fX0) / vx;
		  fRange[1] = (range[1] - fX0) / vx;
		} else {
		  Error("SetRange()", 
			"Vx = 0 and attempts to set range on helix x axis!");
		  return;
		}
		break;
       case kHelixY :
		if ( vy != 0 ) {
		  fRange[0] = (range[0] - fY0) / vy;
		  fRange[1] = (range[1] - fY0) / vy;
		} else {
		  Error("SetRange()", 
			"Vy = 0 and attempts to set range on helix y axis!");
		  return;
		}
		break;
       case kHelixZ :
		  Error("SetRange()", 
			"Vz = 0 and attempts to set range on helix z axis!");
		  return;
       case kLabX   :
       case kLabY   :
       case kLabZ   :
		printf("setting range in lab axes is not implemented yet\n");
		break;
       default      :
		Error("SetRange()","unknown range type %d", rType);
		break;
     }
   }

   if ( fRange[0] > fRange[1] ) {
     Double_t temp = fRange[1];	  fRange[1] = fRange[0];  fRange[0] = temp;
   }

   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   // Set the polylines in global coordinates
   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   Double_t degrad  = TMath::Pi() / 180.0;
   Double_t segment = 5.0 * degrad;		// 5 degree segments
// Double_t dt      = segment / fW;		// parameter span on each segm.
   Double_t dt      = segment / TMath::Abs(fW);	// parameter span on each segm.

   Int_t    nSeg    = (fRange[1]-fRange[0]) / dt + 1;
   if (nSeg < THelix::minNSeg) {
     nSeg = THelix::minNSeg;
     dt = (fRange[1]-fRange[0])/nSeg;
   }

   Double_t * xl    = new Double_t[nSeg+1];	// polyline in local coordinates
   Double_t * yl    = new Double_t[nSeg+1];
   Double_t * zl    = new Double_t[nSeg+1];

   //printf("use %d points in %lf <= t <= %lf\n", nSeg+1, fRange[0], fRange[1]);

   for (i=0; i<=nSeg; i++) {			// calculate xl[], yl[], zl[];
     Double_t t, phase;
     if (i==nSeg) t = fRange[1];		// the last point
     else	  t = fRange[0] + dt * i;
     phase = -fW * t + fPhi0;
     xl[i] = fX0 - fVt/fW * TMath::Sin(phase);
     yl[i] = fY0 + fVt/fW * TMath::Cos(phase);
     zl[i] = fZ0 + fVz * t;
   }

   Float_t  * xg = new Float_t[3*(nSeg+1)];	// global coordinates
// Float_t  * yg = new Float_t[nSeg+1];		// must be Float_t to call
// Float_t  * zg = new Float_t[nSeg+1];		// TPolyLine3D::SetPoint()
   Double_t * M = fRotMat->GetMatrix();
   for (i=0; i<=nSeg; i++) {			// M^{-1} = transpose of M
     xg[3*i]    =  xl[i] * M[0]  +  yl[i] * M[3]  +  zl[i] * M[6] ;
     xg[3*i+1]  =  xl[i] * M[1]  +  yl[i] * M[4]  +  zl[i] * M[7] ;
     xg[3*i+2]  =  xl[i] * M[2]  +  yl[i] * M[5]  +  zl[i] * M[8] ;
   }

   TPolyLine3D::SetPolyLine(nSeg+1, xg);

   delete[] xl;  delete[] yl;    delete[] zl;
   delete[] xg;

}

//______________________________________________________________________________
 void THelix::SetRange(Double_t r1, Double_t r2, EHelixRangeType rType)
{
   Double_t range[2];
   range[0] = r1;	range[1] = r2;
   SetRange(range, rType);
}

//______________________________________________________________________________
 void THelix::Sizeof3D() const
{
//*-*-*-*-*-*Return total X3D size of this shape with its attributes*-*-*-*-*-*-*
//*-*        =======================================================

    gSize3D.numPoints += fN;
    gSize3D.numSegs   += (fN-1);
    gSize3D.numPolys  += 0;

}


//_______________________________________________________________________
 void THelix::Streamer(TBuffer &b)
{
//*-*-*-*-*-*-*-*-*Stream a class object*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*              =========================================
   if (b.IsReading()) {
      Version_t v = b.ReadVersion();
      TObject::Streamer(b);
      TAttLine::Streamer(b);
      b >> fN;
      if (fN) {
         fP = new Float_t[3*fN];
         b.ReadFastArray(fP,3*fN);
      }
      fOption.Streamer(b);
   } else {
      b.WriteVersion(THelix::IsA());
      TObject::Streamer(b);
      TAttLine::Streamer(b);
      b << fN;
      if (fN) b.WriteFastArray(fP, 3*fN);
      fOption.Streamer(b);
   }
}





//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                                                                          //
//                  Protected     Member     Functions                      //
//                                                                          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


//______________________________________________________________________________
void THelix::SetRotMatrix()
{
   // set the rotational matrix according to the helix axis
   //

   //
   // Calculate all 6 angles.
   // Note that TRotMatrix::TRotMatrix() expects angles in degrees.
   //
   Double_t raddeg = 180.0 / TMath::Pi();
   Double_t halfpi = TMath::Pi()/2.0 * raddeg;
   				 // (theta3,phi3) is the helix axis
   Double_t theta3 = TMath::ACos(fAxis[2]) * raddeg;
   Double_t phi3   = TMath::ATan2(fAxis[1], fAxis[0]) * raddeg;
                                 //  (theta1,phi1) is the x-axis in helix frame
   Double_t theta1 = theta3 + halfpi;
   Double_t phi1   = phi3;
                                 //  (theta2,phi2) is the y-axis in helix frame
   Double_t theta2 = halfpi;
   Double_t phi2   = phi1 + halfpi;

   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   // Delete the old rotation matrix
   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   if (fRotMat) delete fRotMat;

   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   // Make a new rotation matrix
   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   fRotMat = new TRotMatrix("HelixRotMat", "Master frame -> Helix frame",
			    theta1, phi1,  theta2, phi2,  theta3, phi3 );
   return;
}





//______________________________________________________________________________
 Double_t  THelix::FindClosestPhase(Double_t phi0,  Double_t cosine)
{
   // Finds the closest phase to phi0 that gives cos(phase) = cosine
   //

   const Double_t pi    = TMath::Pi();
   const Double_t twopi = TMath::Pi() * 2.0;
   Double_t phi1 = TMath::ACos(cosine);
   Double_t phi2 = - phi1;

   while ( phi1 - phi0 >  pi )   phi1 -= twopi;
   while ( phi1 - phi0 < -pi )   phi1 += twopi;

   while ( phi2 - phi0 >  pi )   phi2 -= twopi;
   while ( phi2 - phi0 < -pi )   phi2 += twopi;
   //
   // Now phi1, phi2 and phi0 are within the same 2pi range
   // and cos(phi1) = cos(phi2) = cosine
   //
   if ( TMath::Abs(phi1-phi0) < TMath::Abs(phi2-phi0) )  return phi1;
   else                                                  return phi2;
}
