//  $Id: AMSAntiCluster.h,v 1.3 2001/01/22 17:32:31 choutko Exp $

#ifndef AMSAntiCluster_H
#define AMSAntiCluster_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Cerenkov Threshold Counter Cluster                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include <TObject.h>
#endif
#include "AMS3DCluster.h"

class AMSAntiCluster : public AMS3DCluster {

private:
   Int_t          m_Status;        // Status word
   Int_t          m_Sector;         // Plane number (1...4 : up ... down)
   Float_t        m_Signal;        // Edep in MeV
// Float_t	  m_Position[3];   // coordinate in cm
// Float_t        m_ErrPosition[3];// error of coordinates

   Int_t          m_NTracks;       // number of tracks in m_Tracks;
   TObjArray *    m_Tracks;        // Tracks that pass this cluster


  public:
                  AMSAntiCluster() {;}
                  AMSAntiCluster(Int_t status, Int_t sector, 
				Float_t signal, 
				Float_t * coo, Float_t * ercoo,
				Int_t ntracks, TObjArray * tracks);
   virtual       ~AMSAntiCluster() {;}
   virtual char  *GetObjectInfo(Int_t px, Int_t py);
   virtual void   Paint(Option_t *option="");
   virtual void   Clear(Option_t *option="");

//      Getters
   Int_t          GetStatus()     {return m_Status;}
   Int_t          GetSector()      {return m_Sector;}
   Float_t        GetSignal()     {return m_Signal;}
   TObjArray *    GetTracks()     {return m_Tracks;}

   virtual Bool_t Is3D() {return kTRUE;}
//   virtual void   Sizeof3D() const;

//      Setters

//	Friends
//   friend AMSAntiClusterReader;

   ClassDef(AMSAntiCluster, 1)   //AMS Time of Flight Cluster
};


#endif


