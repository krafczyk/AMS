//  $Id: AMSToFCluster.h,v 1.2 2001/01/22 17:32:40 choutko Exp $

#ifndef AMSToFCluster_H
#define AMSToFCluster_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Time of Flight cluster                                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include <TObject.h>
#endif
#include "AMS3DCluster.h"

class AMSToFCluster : public AMS3DCluster {

private:
   Int_t          m_Status;        // Status word
   Int_t          m_Plane;         // Plane number (1...4 : up ... down)
   Int_t          m_Bar;           // Bar number (spec?)
   Float_t        m_Energy;        // Energy deposition in MeV
   Float_t        m_Time;          // time in seconds
   Float_t        m_ErTime;        // error of m_Time
// Float_t	  m_Position[3];   // coordinate in cm (global?)
// Float_t        m_ErrPosition[3];// error of coordinates

   Int_t          m_Ncells;        // Number of cells used for reconstructing 
				   // this cluster
   Int_t          m_Nparticles;    // Number of particles passing thru
   Int_t          m_NTracks;       // number of tracks in m_Tracks;
   TObjArray *    m_Tracks;        // Tracks that pass this cluster

//   Int_t          m_KFcode;        //Cluster KF-code

//   Int_t          m_UseFlag;       //Flag for cluster usage


  public:
                  AMSToFCluster() {;}
                  AMSToFCluster(Int_t status, Int_t plane, Int_t bar,
				Float_t energy, Float_t time, Float_t ertime,
				Float_t * coo, Float_t * ercoo,
				Int_t ncells, Int_t npart,
				Int_t ntracks, TObjArray * tracks);
   virtual       ~AMSToFCluster() {;}
   virtual char  *GetObjectInfo(Int_t px, Int_t py);
   virtual void   Paint(Option_t *option="");
   virtual void   Clear(Option_t *option="");

//      Getters
   Int_t          GetStatus()  {return m_Status;}
   Int_t          GetPlane()   {return m_Plane;}
   Int_t          GetBar()     {return m_Bar;}
   Float_t        GetEnergy()  {return m_Energy;}
   Float_t        GetTime()    {return m_Time;}
   Int_t          GetNcells()     {return m_Ncells;}
   Int_t          GetNparticles() {return m_Nparticles;}
   Int_t          GetNTracks() {return m_NTracks;}
   TObjArray *    GetTracks()  {return m_Tracks;}

   virtual Bool_t Is3D() {return kTRUE;}
//   virtual void   Sizeof3D() const;

//      Setters
   void           SetEnergy(Float_t energy) {m_Energy = energy;}
   void           SetTime(Float_t time)     {m_Time = time;}
//   void           AddCell(AMSToFCell * cell);
//   void           AddParticle(AMSParticle * particle);
//   void           AddTrack(AMSTrack * track);

//	Friends
//   friend AMSToFClusterMaker;

   ClassDef(AMSToFCluster, 1)   //AMS Time of Flight Cluster
};

#endif


