//  $Id: AMSR_CTCCluster.h,v 1.2 2001/01/22 17:32:38 choutko Exp $

#ifndef AMSR_CTCCluster_H
#define AMSR_CTCCluster_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Cerenkov Threshold Counter Cluster                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include <TObject.h>
#endif
#include "AMSR_3DCluster.h"

class AMSR_CTCCluster : public AMSR_3DCluster {

private:
   Int_t          m_Status;        // Status word
   Int_t          m_Plane;         // Plane number (1...4 : up ... down)
   Float_t        m_RawSignal;     // Raw signal
   Float_t        m_Signal;        // Corrected signal
   Float_t        m_ErrSignal;     // Error of the above
// Float_t	  m_Position[3];   // coordinate in cm
// Float_t        m_ErrPosition[3];// error of coordinates

   Int_t          m_NTracks;       // number of tracks in m_Tracks;
   TObjArray *    m_Tracks;        // Tracks that pass this cluster


  public:
                  AMSR_CTCCluster() {;}
                  AMSR_CTCCluster(Int_t status, Int_t plane, Int_t rawsignal,
				Float_t signal, Float_t errsignal,
				Float_t * coo, Float_t * ercoo,
				Int_t ntracks, TObjArray * tracks);
   virtual       ~AMSR_CTCCluster() {;}
   virtual char  *GetObjectInfo(Int_t px, Int_t py);
   virtual void   Paint(Option_t *option="");
   virtual void   Clear(Option_t *option="");

//      Getters
   Int_t          GetStatus()     {return m_Status;}
   Int_t          GetPlane()      {return m_Plane;}
   Float_t        GetRawSignal()  {return m_RawSignal;}
   Float_t        GetSignal()     {return m_Signal;}
   Float_t        GetErrSignal()  {return m_ErrSignal;}
   TObjArray *    GetTracks()     {return m_Tracks;}

   virtual Bool_t Is3D() {return kTRUE;}
//   virtual void   Sizeof3D() const;

//      Setters

//	Friends
//   friend AMSR_CTCClusterReader;

   ClassDef(AMSR_CTCCluster, 1)   //AMS Time of Flight Cluster
};


#endif


