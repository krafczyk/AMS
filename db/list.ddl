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
// last edit Mar 25, 1997, ak.
//

#include <typedefs.h>

class AMSEventD;
class AMSmceventD;
class AMSgvolumeD;
class AMSNode;

class AMSEventList : public ooContObj {

 private:

  integer   _listType;          // 0 == simulation
  integer   _eventType;         // AMSFFKEY.Write
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
  integer   _nBetas;            // number of Betas
  integer   _nCharges;          // number of Charges
  integer   _nParticles;        // number of Particles
  integer   _nmcEventg;

  ooVString _listName;          // name of list
  ooVString _Setup;             // prefix will be used to find containers with
                                // Geometry, Materials and TrackingMedia
  
 public:

// Constructor
  AMSEventList();
  AMSEventList(char* listname, char* setup);

// Add Methods
   ooStatus  AddGeometry(ooHandle(ooDBObj)& dbH);
   ooStatus  AddMaterial(ooHandle(ooDBObj)& dbH);
   ooStatus  AddTMedia(ooHandle(ooDBObj)& dbH);
   ooStatus  Addamsdbc(ooHandle(ooDBObj)& dbH);
   ooStatus  AddTDV(ooHandle(ooDBObj)& dbH);
   ooStatus  AddEvent (integer run, uinteger event, integer eventW);
   ooStatus  AddMCEvent (integer run, uinteger event, integer eventW); 
   ooStatus  AddDummyMCEvent (integer run, uinteger event, integer eventW);
   ooStatus  AddTrCluster(const integer N, ooHandle(AMSEventD)& eventH);
   ooStatus  AddTrMCCluster(ooHandle(AMSEventD)&  eventH);
   ooStatus  AddTOFMCCluster(ooHandle(AMSEventD)&  eventH);
   ooStatus  AddTrRecHit(const integer N, ooHandle(AMSEventD)&  eventH);
   ooStatus  AddTrTrack(ooHandle(AMSEventD)&  eventH);
   ooStatus  AddmcEventg(ooHandle(AMSEventD)&  eventH);
   ooStatus  AddCTCCluster(ooHandle(AMSEventD)&  eventH);
   ooStatus  AddCTCMCCluster(ooHandle(AMSEventD)&  eventH);
   ooStatus  AddAntiMCCluster(ooHandle(AMSEventD)&  eventH);
   ooStatus  AddTOFCluster(const integer N, ooHandle(AMSEventD)&  eventH);
   ooStatus  AddBeta(ooHandle(AMSEventD)&  eventH);
   ooStatus  AddCharge(ooHandle(AMSEventD)&  eventH);
   ooStatus  AddParticle(ooHandle(AMSEventD)&  eventH);

//Copy Methods
   ooStatus  CopyGeometry(ooMode mode, ooHandle(ooDBObj)& dbH);
   ooStatus  CopyMaterial(ooMode mode, ooHandle(ooDBObj)& dbH);
   ooStatus  CopyTMedia(ooMode mode, ooHandle(ooDBObj)& dbH);
   ooStatus  CopyTDV(time_t time, ooMode mode, ooHandle(ooDBObj)& dbH);
   ooStatus  CopyEventHeader(ooHandle(AMSEventD)&  eventH, ooMode mode);
   ooStatus  CopyEvent(ooHandle(AMSEventD)&  eventH, ooMode mode);
   ooStatus  CopyMCEvent(ooHandle(AMSEventD)& eventH, ooMode mode);
   ooStatus  CopyMCEventD(ooHandle(AMSmceventD)& eventH, ooMode mode);
   ooStatus  CopyMCeventg(ooHandle(AMSEventD)& eventH, ooMode mode);
   ooStatus  CopymcEventg(ooHandle(AMSEventD)& eventH, ooMode mode);
   ooStatus  CopyTrRecHit(ooHandle(AMSEventD)&  eventH, ooMode mode);
   ooStatus  CopyTrCluster(ooHandle(AMSEventD)&  eventH, ooMode mode);
   ooStatus  CopyTrTrack(ooHandle(AMSEventD)&  eventH, ooMode mode);
   ooStatus  CopyTrMCCluster(ooHandle(AMSEventD)&  eventH, ooMode mode);
   ooStatus  CopyTOFMCCluster(ooHandle(AMSEventD)&  eventH, ooMode mode);
   ooStatus  CopyTOFCluster(ooHandle(AMSEventD)& eventH, ooMode mode);
   ooStatus  CopyAntiMCCluster(ooHandle(AMSEventD)&  eventH, ooMode mode);
   ooStatus  CopyCTCMCCluster(ooHandle(AMSEventD)&  eventH, ooMode mode);
   ooStatus  CopyCTCCluster(ooHandle(AMSEventD)& eventH, ooMode mode);
   ooStatus  CopyBeta(ooHandle(AMSEventD)&  eventH, ooMode mode);
   ooStatus  CopyCharge(ooHandle(AMSEventD)&  eventH, ooMode mode);
   ooStatus  CopyParticle(ooHandle(AMSEventD)&  eventH, ooMode mode);

   ooStatus  LinkHitClusterD(const integer N, ooHandle(AMSEventD)& eventH);
   ooStatus  LinkTrackHitD(ooHandle(AMSEventD)& eventH);

   void CopyByPos(ooHandle(AMSgvolumeD)& ptr, ooMode mode);
   void CopyByPtr(AMSNode *ptr);

// Delete Methods


// Set/Get Methods
           integer   ListType()          {return _listType;}
           integer   EventType()         {return _eventType;}
   const   char*     ListName()          {return _listName;}
   const   char*     getsetup()          {return _Setup;}
   void    setListType(integer listType) {_listType = listType;}
   void    setEventType(integer eventType) {_eventType = eventType;}
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

   void setsetup(char* setup);
   void setlistname(char* listname);
   void SetContainersNames();
   void setlisttype(integer type) { _listType = type;}

// Find Methods
   ooStatus FindEvent(integer runNumber, uinteger eventNumber, 
                      ooMode mode, ooHandle(AMSEventD)& eventH);
   ooStatus FindEvent(integer runNumber, uinteger eventNumber, 
                      ooMode mode, ooHandle(AMSmceventD)& eventH);
   ooStatus 
       CheckContainer(char* name, ooMode mode, ooHandle(ooContObj)& container);
   ooStatus 
       CheckContainer(char* name, ooMode mode, 
                      ooHandle(ooContObj)& container, ooHandle(ooDBObj)& dbH);
   ooStatus   CheckAllContainers(ooMode mode);
   ooBoolean  CheckListSstring(char* sstring);

// Print Methods
   ooStatus PrintListStatistics(char* printMode);
   ooStatus PrintListStatistics_mc(char* printMode);

// Delete Methods
   ooStatus DeleteEventList();
   ooStatus DeleteAllContainers();

};

#endif


