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
// Oct  01,1996      add ooVstring listName
//
// last edit Oct 18, 1996, ak.
//

#include <ooMap.h>
#include <typedefs.h>

class AMSEventD;
class AMSgvolumeD;
class AMSNode;

class AMSEventList : public ooContObj {

 private:

//  uinteger  fRunNumber;         // Run Number (if any)
  integer   _listType;          // 0 == simulation
  integer   _nEvents;           // number of events
  integer   _nEventsP;          // number of events
  integer   _nHits[6];          // number of hits;
  integer   _nClusters[2];      // number of X/Y Clusters
  integer   _nTracks;           // number of tracks
  integer   _nMCClusters;       // number of MCClusters
  integer   _nTOFMCClusters;    // number of TOF MCClusters
  integer   _nAntiMCClusters;   // number of Anti MCClusters
  integer   _nTOFClusters[4];   // number of TOF Clusters
  integer   _nCTCClusters;      // number of CTC clusters
  integer   _nCTCMCClusters;    // number of TOF MCClusters
  integer   _nMCCTCClusters;    // number of MC CTC clusters
  integer   _nBetas;            // number of Betas
  integer   _nCharges;          // number of Charges
  integer   _nParticles;        // number of Particles
  integer   _nmcEventg;

  ooVString _listName;          // name of list
  ooVString _Setup;             // prefix will be used to find containers with
                                // Geometry, Materials and TrackingMedia
  ooVString _mapName;           // name of map
  ooVString _mceventgCont;      // name of tof mc clusters container
  ooVString _trlayerCont[6];    // name of track rechits container
  ooVString _trclusterCont[2];  // name of tof mc clusters container
  ooVString _trmcclusterCont;   // name of tof mc clusters container
  ooVString _tofmcclusterCont;  // name of tof mc clusters container
  ooVString _sclayerCont[4];    // name of tof container
  ooVString _ctcclusterCont;    // name of tof mc clusters container
  ooVString _ctcmcclusterCont;  // name of ctc mc clusters container
  ooVString _trtrackCont;       // name of track container
  ooVString _betaCont;          // name of beta container
  ooVString _chargeCont;        // name of charge container
  ooVString _particleCont;      // name of particle container
  ooVString _antimcclusterCont; // name of anti mc clusters container

 public:

// Constructor
  AMSEventList();
  AMSEventList(char* listname, char* setup);
//  AMSEventList(uinteger runNumber);

// Add Methods
   ooStatus AddGeometry();
   ooStatus AddMaterial();
   ooStatus AddTMedia();
   ooStatus Addamsdbc();
   ooStatus AddEvent (integer run, uinteger event, char* id, integer eventW, 
                      ooHandle(ooMap) mapH);
   ooStatus 
         AddTrCluster(char* id, const integer N, ooHandle(AMSEventD)& eventH);
  ooStatus  AddTrMCCluster(char* id, ooHandle(AMSEventD)&  eventH);
  ooStatus  AddTOFMCCluster(char* id, ooHandle(AMSEventD)&  eventH);
   ooStatus 
         AddTrRecHit(char* id, const integer N, ooHandle(AMSEventD)&  eventH);
   ooStatus  AddTrTrack(char* id, ooHandle(AMSEventD)&  eventH);
   ooStatus  AddmcEventg(char* id, ooHandle(AMSEventD)&  eventH);
   ooStatus  AddCTCCluster(char* id, ooHandle(AMSEventD)&  eventH);
   ooStatus  AddCTCMCCluster(char* id, ooHandle(AMSEventD)&  eventH);
   ooStatus  AddAntiMCCluster(char* id, ooHandle(AMSEventD)&  eventH);
   ooStatus 
       AddTOFCluster(char* id, const integer N, ooHandle(AMSEventD)&  eventH);
   ooStatus AddBeta(char* id, ooHandle(AMSEventD)&  eventH);
   ooStatus AddCharge(char* id, ooHandle(AMSEventD)&  eventH);
   ooStatus AddParticle(char* id, ooHandle(AMSEventD)&  eventH);
//Copy Methods
   ooStatus CopyGeometry(ooMode mode);
   ooStatus CopyMaterial(ooMode mode);
   ooStatus CopyTMedia(ooMode mode);
   ooStatus CopyEvent(char* id, ooHandle(AMSEventD)&  eventH, ooMode mode);
   ooStatus CopyMCEvent(char* id, ooHandle(AMSEventD)& eventH, ooMode mode);
   ooStatus CopyMCeventg(char* id, ooHandle(AMSEventD)& eventH, ooMode mode);
   ooStatus CopymcEventg(char* id, ooHandle(AMSEventD)& eventH, ooMode mode);
   ooStatus CopyTrRecHit(char* id, ooHandle(AMSEventD)&  eventH, ooMode mode);
   ooStatus CopyTrCluster(char* id, ooHandle(AMSEventD)&  eventH, ooMode mode);
   ooStatus CopyTrTrack(char* id, ooHandle(AMSEventD)&  eventH, ooMode mode);
   ooStatus 
         CopyTrMCCluster(char* id, ooHandle(AMSEventD)&  eventH, ooMode mode);
   ooStatus 
         CopyTOFMCCluster(char* id, ooHandle(AMSEventD)&  eventH, ooMode mode);
   ooStatus CopyTOFCluster(char* id, ooHandle(AMSEventD)& eventH, ooMode mode);
   ooStatus 
       CopyAntiMCCluster(char* id, ooHandle(AMSEventD)&  eventH, ooMode mode);
   ooStatus 
         CopyCTCMCCluster(char* id, ooHandle(AMSEventD)&  eventH, ooMode mode);
   ooStatus CopyCTCCluster(char* id, ooHandle(AMSEventD)& eventH, ooMode mode);
   ooStatus CopyBeta(char* id, ooHandle(AMSEventD)&  eventH, ooMode mode);
   ooStatus CopyCharge(char* id, ooHandle(AMSEventD)&  eventH, ooMode mode);
   ooStatus CopyParticle(char* id, ooHandle(AMSEventD)&  eventH, ooMode mode);

// Delete Methods
   ooStatus DeleteEvent(ooHandle(AMSEventD)&  eventH);

// Set/Get Methods
//   const   uinteger& RunNumber()         {return fRunNumber;}
           integer   ListType()          {return _listType;}
   void    setListType(integer listType) {_listType = listType;}
   integer getNEvents()                  {return _nEvents;}
   void    incNEvents()                  {_nEvents++;}
   void    decNEvents()                  {if (_nEvents > 0) _nEvents--;}
   integer getNEventsP()                  {return _nEventsP;}
   void    incNEventsP()                  {_nEventsP++;}
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
   integer getNMCCTCClusters()           {return _nMCCTCClusters;}
   void    incNMCCTCClusters()           {_nMCCTCClusters++;}
   integer getNBetas()                   {return _nBetas;}
   void    incNBetas()                   {_nBetas++;}
   integer getNCharges()                 {return _nCharges;}
   void    incNCharges()                 {_nCharges++;}
   integer getNParticles()               {return _nParticles;}
   void    incNParticles()               {_nParticles++;}
   integer getNMCClusters()              {return _nMCClusters;}
   void    incNMCClusters()              {_nMCClusters++;}
   integer getNTOFMCClusters()           {return _nTOFMCClusters;}
   void    incNTOFMCClusters()           {_nTOFMCClusters++;}
   integer getNCTCMCClusters()           {return _nCTCMCClusters;}
   void    incNCTCMCClusters()           {_nCTCMCClusters++;}
   integer getNAntiMCClusters()          {return _nAntiMCClusters;}
   void    incNAntiMCClusters()          {_nAntiMCClusters++;}
   integer getNmcEventg()                {return _nmcEventg;}
   void    incNmcEventg()                {_nmcEventg++;}

   ooStatus GetKeepingList(ooHandle(AMSEventList)& mylistH);

   void setsetup(char* setup);
   void setlistname(char* listname);
   void setmapname(char* mapname);
   void setlisttype(integer type) { _listType = type;}

// Find Methods
   ooStatus FindMap(char* mapName, ooMode mode, ooHandle(ooMap)& mapH);
   ooStatus FindEvent(char* id, ooMode mode, ooHandle(AMSEventD)& eventH);
   ooStatus FindEvent(char* id, ooMode mode, ooHandle(AMSEventD)& eventH,
                      ooHandle(ooMap)& mapH);
   ooStatus FindEventByN(integer eventN, ooMode mode, 
                         ooHandle(AMSEventD)& eventH);
   ooStatus CheckContainer(char* name, ooMode mode, 
                           ooHandle(ooContObj)& container, const int flag);
   ooStatus CheckAllContainers(ooMode mode);
   ooStatus LinkHitClusterD
                     (char* id, const integer N, ooHandle(AMSEventD)& eventH);
   ooStatus LinkHitClusterM(char* id, ooHandle(AMSEventD)& eventH);
   ooStatus LinkTrackHitD(char* id, ooHandle(AMSEventD)& eventH);
   ooStatus LinkTrackHitM(char* id, ooHandle(AMSEventD)& eventH);
   
   void CopyByPos(ooHandle(AMSgvolumeD)& ptr, ooMode mode);
   void CopyByPtr(AMSNode *ptr);

   ooStatus PrintMap(ooMode mode);

};

#endif


