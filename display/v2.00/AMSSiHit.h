//  $Id: AMSSiHit.h,v 1.3 2001/01/22 17:32:40 choutko Exp $

#ifndef AMSSiHit_H
#define AMSSiHit_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Silicon Hit                                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include <TObject.h>
#endif
#include "AMS3DCluster.h"

class AMSSiHit : public AMS3DCluster {

private:
   Int_t          m_Status;         // Status word
   Int_t          m_Plane;          // Layer number (1...6 : up ... down)
   Int_t          m_X;              // X track cluster
   Int_t          m_Y;              // Y track cluster
// Float_t	  m_Position[3];    // Hit position in global coordinate system
   Float_t	  m_ErrPosition[3]; // Error of m_Position[]
   Float_t	  m_Amplitude;	    // Amplitude
   Float_t	  m_AsymAmpl;	    // Asymmetry of amplitudes in X vs Y side
   TObjArray *    m_Tracks;         // Tracks that pass this hit


  public:
                  AMSSiHit() {;}
                  AMSSiHit(Int_t status, Int_t plane, Int_t px, Int_t py,
				Float_t * hit, Float_t * errhit, 
				Float_t ampl, Float_t asym,
				TObjArray * tracks);
   virtual       ~AMSSiHit() {;}
   virtual char  *GetObjectInfo(Int_t px, Int_t py);
   virtual void   Paint(Option_t *option="");
   virtual void   Clear(Option_t *option="");

//      Getters
   Int_t          GetStatus()    {return m_Status;}
   Int_t          GetPlane()     {return m_Plane;}
   Int_t          GetX()         {return m_X;}
   Int_t          GetY()         {return m_Y;}
   Float_t        GetAmplitude() {return m_Amplitude;}
   Float_t        GetAsymAmpl()  {return m_AsymAmpl;}
   TObjArray *    GetTracks()    {return m_Tracks;}

   virtual Bool_t Is3D() {return kTRUE;}
//   virtual void   Sizeof3D() const;

//      Setters
//   void           AddTrack(AMSTrack * track);

//	Friends
//   friend AMSSiHitMaker;

   ClassDef(AMSSiHit, 1)   //AMS Time of Flight Cluster
};

#endif


