//  $Id: AMSTrMCCluster.h,v 1.2 2001/01/22 17:32:40 choutko Exp $

#ifndef AMSTrMCCluster_H
#define AMSTrMCCluster_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Cerenkov Threshold Counter Cluster                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include <TObject.h>
#endif
#include "AMS3DCluster.h"

class AMSTrMCCluster : public AMS3DCluster {

private:
   Int_t          m_Part;        // Geant Particle ID
   Float_t        m_Signal;        // Edep in MeV

   Int_t          m_NTracks;       // number of tracks in m_Tracks;
   TObjArray *    m_Tracks;        // Tracks that pass this cluster


  public:
                  AMSTrMCCluster() {;}
                  AMSTrMCCluster(Int_t part, 
				Float_t signal, 
				Float_t * coo, Float_t * ercoo,
				Int_t ntracks, TObjArray * tracks);
   virtual       ~AMSTrMCCluster() {;}
   virtual char  *GetObjectInfo(Int_t px, Int_t py);
   virtual void   Paint(Option_t *option="");
   virtual void   Clear(Option_t *option="");

//      Getters
   Int_t          GetStatus()     {return m_Part;}
   Float_t        GetSignal()     {return m_Signal;}
   TObjArray *    GetTracks()     {return m_Tracks;}

   virtual Bool_t Is3D() {return kTRUE;}
//   virtual void   Sizeof3D() const;

//      Setters

//	Friends
//   friend AMSTrMCClusterReader;

   ClassDef(AMSTrMCCluster, 1)   //AMS Time of Flight Cluster
};


#endif


