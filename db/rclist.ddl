#ifndef list_h
#define list_h

// class List of events
// May,    1996. ak. First try with Objectivity  
//                   V106 corrections
// July 10,1996. ak. add AMSBeta
//                       AMSCharge, AMSParticle
// Sep    ,1996. ak. Geometry, Materials, TMedia;
//                   ooVString Setup
//                   remove fRunNumber function
//                   modify number of parameters of AddTrTrack
// Oct    ,1996      add ooVstring listName
// Feb  12, 1997    mceventD, TimeDependendVariables (TDV)
//                  no ID, no map
// Mar  25, 1997    AMSsetupDB
//
// last edit May 9, 1997, ak.
//

#include <typedefs.h>

#include <list_ref.h>
#include <list.h>


class AMSeventD;

class AMSEventList : public EventList {

 private:

  integer   _nBetas;            // number of Betas
  integer   _nCharges;          // number of Charges
  integer   _nClusters[2];      // number of X/Y Clusters
  integer   _nCTCClusters;      // number of CTC clusters
  integer   _nHits[6];          // number of hits;
  integer   _nParticles;        // number of Particles
  integer   _nTOFClusters[4];   // number of TOF Clusters
  integer   _nTracks;           // number of tracks

 public:

// Constructor
  AMSEventList();
  AMSEventList(char* listName);
  AMSEventList(char* listName, char* setup);
 ~AMSEventList() {};

// Set/Get Methods
   integer getNTracks()                  {return _nTracks;}
   void    incNTracks()                  {_nTracks++;}
   integer getNHits(integer n)           { if(n<6) {
                                            return _nHits[n];
                                           } else {
                                            return -1;
                                           }
                                          }    
   void    incNHits(integer n)           {if(n<6) _nHits[n]++;}
   integer getNClusters(integer n)       { if(n<2) {
                                            return _nClusters[n];
                                           } else {
                                            return -1;}
                                          }
   void incNClusters(integer n)          { if(n<2) _nClusters[n]++;}

   integer getNTOFCl(integer n)           { if(n<4) {
                                            return _nTOFClusters[n];
                                           } else {
                                            return -1;
                                           }
                                          }    
   void    incNTOFCl(integer n)           {if(n<4) _nTOFClusters[n]++;}
   integer getNCTCClusters()             {return _nCTCClusters;}
   void    incNCTCClusters()             {_nCTCClusters++;}
   integer getNBetas()                   {return _nBetas;}
   void    incNBetas()                   {_nBetas++;}
   integer getNCharges()                 {return _nCharges;}
   void    incNCharges()                 {_nCharges++;}
   integer getNParticles()               {return _nParticles;}
   void    incNParticles()               {_nParticles++;}

 
//
void     resetRCCounters();
void     SetContainersNames();

// Add Objects
ooStatus  AddBeta(ooHandle(AMSeventD)&  eventH);
ooStatus  AddCharge(ooHandle(AMSeventD)&  eventH);
ooStatus  AddCTCCluster(ooHandle(AMSeventD)&  eventH);
ooStatus  AddParticle(ooHandle(AMSeventD)&  eventH);
ooStatus  AddTOFCluster(const integer N, ooHandle(AMSeventD)&  eventH);
ooStatus  AddTrCluster(const integer N, ooHandle(AMSeventD)& eventH);
ooStatus  AddTrRecHit(const integer N, ooHandle(AMSeventD)&  eventH);
ooStatus  AddTrTrack(ooHandle(AMSeventD)&  eventH);
ooStatus  AddParticleS(ooHandle(AMSeventD)&  eventH);

ooStatus  LinkHitClusterD(const integer N, ooHandle(AMSeventD)& eventH);
ooStatus  LinkTrackHitD(ooHandle(AMSeventD)& eventH);

};

#endif
