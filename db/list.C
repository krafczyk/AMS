// method source file for the object AMSEventList
// First try with Objectivity May 02, 1996
//
// May     1996. ak. Add AMSTrIdGeom to AMSTrCluster
//                   New scheme of Clusters <-> Hits
//                   Set bidirectional event <-> cluster, event <-> hit assoc.
// July    1996. ak. V109, AMSBeta, AMSCharge, AMSParticle
//                   Add TOFMCCluster
// Aug     1996. ak. modify AddEvent/CheckContainer
//                   modify LinkHitTrCluster iteration algo.
//                   scan over eventH, not container
//                   V1.24
//                   Add Geometry (AMSgvolumeD object)
// Sep     1996. ak. Copy Geometry. Add Materials
//                   Add TMedia (I expect that there is only one mother and 
//                   many daughters for Materials and Tmedia);
//                   Geometry, Materials and TMedia are stored in the container
//                   with name Object_'Setup'
//                   V1.25, AMSparticleD and AMSTrRecHitD are modified
//                   fRunNumber class member is removed
//                   npat is global variable and defined in commons.h
//                   use npatb for betas., AddBeta, CopyBeta, AddTrTrack are
//                   modified.
//                   Add class CTC to AMSParticleD
//                   Add class CTC MC cluster
// Oct    1996. ak. remove database 'general' event map, use map per list 
//                   Initialize subroutine is removed
//                   add mceventg class
//                   CopyEvent, CopyMCEvent, Copymceventg are modified
//                   Get run number from data cards
//                   friend class implementation.
//                   overwrite event in AddEvent if it is exist
//                   AddEvent is modified (Find is moved to the upper level)
//                   new function CopyEventHeader.
//                   keep number of hits, clusters, tracks, etc per event
// Nov    1996. ak.  modifications of Addamsdbc
//                   AMScommonsD is modified
//                   CmpTrMedia and CmpMaterials
// Feb  14,1997. ak. AMSmceventD, CopyGeometry is modified, 
//                   move timeid -> timeid_sub
// Mar  07,1997. ak. use unidirectional assoc. between track and trrechits,
//                   cluster and trrechit
//                   replace head() function by headC
//                   pSen for AMSTrRecHitD
//
// !!!               still AMSTrMCCluster,0 AMSTOFMCCluster,0 in gethead
//                   CopyGeometry is modified (inrm <-> nrm)
//
// last edit Mar 25, 1997, ak.
//
//
#include <iostream.h>
#include <string.h>

#include <ooIndex.h>

#include <db_comm.h>

#include <particleD_ref.h>
#include <chargeD_ref.h>
#include <tbeta_ref.h>
#include <tcluster_ref.h>
#include <tmccluster_ref.h>
#include <mceventgD_ref.h>
#include <mceventD_ref.h>
#include <mctofclusterD_ref.h>
#include <mcanticlusterD_ref.h>
#include <ctcrecD_ref.h>
#include <mcctcclusterD_ref.h>
#include <trrechit_ref.h>
#include <ttrack_ref.h>
#include <tofrecD_ref.h>
#include <gvolumeD_ref.h>
#include <gmatD_ref.h>
#include <gtmedD_ref.h>
#include <amsdbcD_ref.h>
#include <ctcdbcD_ref.h>
#include <tofdbcD_ref.h>
#include <commonsD_ref.h>
#include <eventD_ref.h>
#include <list_ref.h>
#include <list.h>
#include <eventD.h>
#include <tcluster.h>
#include <tmccluster.h>
#include <trrechit.h>
#include <tofrecD.h>
#include <ctcrecD.h>
#include <ttrack.h>
#include <tbeta.h>
#include <chargeD.h>
#include <particleD.h>
#include <mctofclusterD.h>
#include <mcanticlusterD.h>
#include <mcctcclusterD.h>
#include <gvolumeD.h>
#include <gmatD.h>
#include <gtmedD.h>
#include <amsdbcD.h>
#include <ctcdbcD.h>
#include <tofdbcD.h>
#include <commonsD.h>
#include <mceventgD.h>
#include <mceventD.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
#include <trrec.h>
#include <tofrec.h>
#include <mccluster.h>
#include <beta.h>
#include <charge.h>
#include <particle.h>
#include <amsgobj.h>
#include <gmat.h>
#include <amsdbc.h>
#include <ctcdbc.h>
#include <gsen.h>
// debugging and testing values

integer  event_size;
integer  NN;
integer  NOBJ;


ooHandle(AMSgvolumeD)* geometryHT;
integer*               geometryP;
AMSgvolume**           ptrsGV;

// Containers
ooHandle(ooContObj)  contH;                   // container
ooHandle(ooContObj)  contTrackH;              // tracks 
ooHandle(ooContObj)  contBetaH;               // betas  
ooHandle(ooContObj)  contChargeH;             // charges  
ooHandle(ooContObj)  contClusterH[2];         // tracker cluster 
ooHandle(ooContObj)  contMCClusterH;          // MC cluster 
ooHandle(ooContObj)  contCTCClusterH;         // CTC cluster 
ooHandle(ooContObj)  contCTCMCClusterH;       // CTC MC cluster 
ooHandle(ooContObj)  contTrLayerH[6];         // tracker layers
ooHandle(ooContObj)  contLayerSH[4];          // TOF
ooHandle(ooContObj)  contParticleH;           // Particle
ooHandle(ooContObj)  contTOFMCClusterH;       // TOF MC cluster 
ooHandle(ooContObj)  contAntiMCClusterH;      // Anti MC cluster 
ooHandle(ooContObj)  contGeometryH;           // geometry 
ooHandle(ooContObj)  contgmatH;               // material
ooHandle(ooContObj)  contgtmedH;              // tracking media
ooHandle(ooContObj)  contmceventgH;           // MC Event
ooHandle(ooContObj)  conttdvH;                // Time dependent var

// ooObj classes
ooHandle(AMSTOFMCClusterD) TOFMCClusterH;
ooHandle(AMSCTCMCClusterD) CTCMCClusterH;
ooHandle(AMSTrMCClusterD)  MCClusterH;
ooHandle(AMSAntiMCClusterD) AntiMCClusterH;
ooHandle(AMSTrClusterD)    clusterH;
ooHandle(AMSEventD)        eventH;
ooHandle(AMSBetaD)         betaH;
ooHandle(AMSChargeD)       chargeH;
ooHandle(AMSTrTrackD)      trackH;
ooHandle(AMSParticleD)     particleH;
ooHandle(AMSgvolumeD)      geometryH;              
ooHandle(AMSgmatD)         gmatH;                      
ooHandle(AMSgtmedD)        gtmedH;                      
ooHandle(AMSmceventgD)     mceventgH;
ooHandle(AMSDBcD)          amsdbcH;
ooHandle(CTCDBcD)          ctcdbcH;
ooHandle(TOFDBcD)          tofdbcH;
ooHandle(AMScommonsD)      commonsH;

// Iterators
ooItr(AMSTrRecHitD)    trRecHitItr;             // hit iterator
ooItr(AMSTrClusterD)   trClusterItr;            // cluster iterator
ooItr(AMSTrTrackD)     trTrackItr;              // track iterator
ooItr(AMSTrMCClusterD) trMCClusterItr;          // MC cluster
ooItr(AMSTOFMCClusterD) TOFMCClusterItr;        // TOF MC cluster
ooItr(AMSAntiMCClusterD) AntiMCClusterItr;        // Anti MC cluster
ooItr(AMSCTCMCClusterD) CTCMCClusterItr;        // CTC MC cluster
ooItr(AMSCTCClusterD)  ctcClusterItr;           // ctc cluster
ooItr(AMSTOFClusterD)  tofClusterItr;           // tof cluster
ooItr(AMSBetaD)        betaItr;                 // beta
ooItr(AMSChargeD)      chargeItr;               // charge
ooItr(AMSParticleD)    particleItr;             // particle
ooItr(AMSgvolumeD)     geometryItr;             // geometry
ooItr(AMSgmatD)        gmatItr;                 // material
ooItr(AMSgtmedD)       gtmedItr;                // tmedia
ooItr(AMSmceventgD)    mceventgItr;             // mc event
ooItr(AMSDBcD)         amsdbcItr;               // amsdbc
ooItr(TOFDBcD)         tofdbcItr;               // tofdbc
ooItr(AMScommonsD)     commonsItr;              // commons
ooItr(CTCDBcD)         ctcdbcItr;               // ctcdbc

// lists

// container  names
  char       nameTrL[6][9] = {
        "TrLayer1","TrLayer2","TrLayer3","TrLayer4","TrLayer5","TrLayer6"};
  char       nameC[2][11] = {"TrClusterX","TrClusterY"};
  char       nameS[4][9] = {"ScLayer1","ScLayer2","ScLayer3","ScLayer4"};

  static char* antimcclusterCont;
  static char* betaCont;
  static char* chargeCont;
  static char* ctcclusterCont;
  static char* ctcmcclusterCont;
  static char* mceventgCont;
  static char* particleCont;
  static char* sclayerCont[4];
  static char* tofmcclusterCont;
  static char* trclusterCont[2];
  static char* trlayerCont[6];
  static char* trtrackCont;
  static char* trmcclusterCont;


AMSEventList::AMSEventList (char* listname, char* setup)
{

  for (integer i=0; i<6; i++) _nHits[i]     = 0;
  for (i=0; i<2; i++) _nClusters[i] = 0;
  _nEvents   = 0;
  for (i=0; i<4; i++) _nTOFClusters[i] = 0;
  _nTracks   = 0;
  _nBetas    = 0;
  _nCharges  = 0;
  _nParticles= 0;
  _nMCClusters = 0;
  _nTOFMCClusters = 0;

  if (setup) _Setup = setup;
  if (listname) _listName = listname;
  SetContainersNames();
}

void AMSEventList::SetContainersNames()
{
  char*   name;
  integer i;

//mceventg
  if (!mceventgCont) {
    mceventgCont =  new char[strlen(_listName)+10];
    strcpy(mceventgCont,"mceventg_");
    strcat(mceventgCont,_listName);
  }

//AntiMCCluster
   if (!antimcclusterCont) {
    antimcclusterCont =  new char[strlen(_listName)+15];
    strcpy(antimcclusterCont,"AntiMCCluster_");
    strcat(antimcclusterCont,_listName);
   }

//CTCMCCluster
   if (!ctcmcclusterCont) {
    ctcmcclusterCont =  new char[strlen(_listName)+14];
    strcpy(ctcmcclusterCont,"CTCMCCluster_");
    strcat(ctcmcclusterCont,_listName);
   }

//TOFMCCluster
   if (!tofmcclusterCont) {
    tofmcclusterCont =  new char[strlen(_listName)+14];
    strcpy(tofmcclusterCont,"TOFMCCluster_");
    strcat(tofmcclusterCont,_listName);
   }

//trMCCluster
   if (!trmcclusterCont) {
    trmcclusterCont =  new char[strlen(_listName)+13];
    strcpy(trmcclusterCont,"TrMCCluster_");
    strcat(trmcclusterCont,_listName);
   }

//TrLayer
   char       nameTrL[6][10] = {
     "TrLayer1_","TrLayer2_","TrLayer3_","TrLayer4_","TrLayer5_","TrLayer6_"};
   for (i=0; i<6; i++) {
     if (!trlayerCont[i]) {
      trlayerCont[i] =  new char[strlen(_listName)+10];
      strcpy(trlayerCont[i],nameTrL[i]);
      strcat(trlayerCont[i],_listName);
     }
   }

//TrCluster
   if (!trclusterCont[0]) {
    trclusterCont[0] =  new char[strlen(_listName)+12];
    strcpy(trclusterCont[0],"TrClusterX_");
    strcat(trclusterCont[0],_listName);
   }
   if (!trclusterCont[1]) {
    trclusterCont[1] =  new char[strlen(_listName)+12];
    strcpy(trclusterCont[1],"TrClusterY_");
    strcat(trclusterCont[1],_listName);
   }

//ScLayer
   char  nameSc[4][10] = {"ScLayer1_","ScLayer2_","ScLayer3_","ScLayer4_"};
   for (i=0; i<4; i++) {
    if (!sclayerCont[i]) {
     sclayerCont[i] =  new char[strlen(_listName)+10];
     strcpy(sclayerCont[i],nameSc[i]);
     strcat(sclayerCont[i],_listName);
    }
   }

//CTCCluster
   if (!ctcclusterCont) {
    ctcclusterCont =  new char[strlen(_listName)+12];
    strcpy(ctcclusterCont,"CTCCluster_");
    strcat(ctcclusterCont,_listName);
   }

//TrTracks
   if (!trtrackCont) {
    trtrackCont =  new char[strlen(_listName)+10];
    strcpy(trtrackCont,"TrTracks_");
    strcat(trtrackCont,_listName);
   }

//Beta
   if (!betaCont) {
    betaCont =  new char[strlen(_listName)+5];
    strcpy(betaCont,"Beta_");
    strcat(betaCont,_listName);
   }

//Charge
   if (!chargeCont) {
    chargeCont =  new char[strlen(_listName)+8];
    strcpy(chargeCont,"Charge_");
    strcat(chargeCont,_listName);
   }

//Particle
   if (!particleCont) {
    particleCont =  new char[strlen(_listName)+12];
    strcpy(particleCont,"Particle_");
    strcat(particleCont,_listName);
   }
} 

void AMSEventList::setsetup (char* setup)
{
  if (setup) _Setup = setup;
}

void AMSEventList::setlistname (char* listname)
{
  if (listname) _listName = listname;
}

ooStatus AMSEventList::FindEvent(integer runNumber, uinteger eventNumber, 
                                 ooMode mode, ooHandle(AMSEventD)& eventH)
{
     ooStatus rstatus = oocError;
     ooItr(AMSEventD)   eventItr;

     ooHandle(AMSEventList) listH = ooThis();

     ooLookupKey               lookupKey(ooTypeN(AMSEventD),2);

     ooEqualLookupField        
        runELookupField(ooTypeN(AMSEventD), "_runNumber", &runNumber);
     ooGreaterThanLookupField  
        runGLookupField(ooTypeN(AMSEventD), "_runNumber", &runNumber);

     ooEqualLookupField        
      eventELookupField(ooTypeN(AMSEventD), "_eventNumber", &eventNumber);
     ooGreaterThanLookupField  
      eventGLookupField(ooTypeN(AMSEventD), "_eventNumber", &eventNumber);

     if (runNumber > 0) { 
      lookupKey.addField(runELookupField); 
     } else {
      lookupKey.addField(runGLookupField); 
     }

     if (eventNumber > 0) { 
       lookupKey.addField(eventELookupField); 
     } else { 
       lookupKey.addField(eventGLookupField); 
     }

     ooStatus status = eventItr.scan(listH,lookupKey,oocRead);
     if (status == oocSuccess) {
       while (eventItr.next()) {
        eventH = eventItr;
        if (dbg_prtout) cout 
             << "AMSEventList::FindEvent: found the event with run # "
             << runNumber<<" and event # "<<eventNumber<<endl;
        rstatus = oocSuccess;
        return rstatus;
       }
     } else {
       cout<<"AMSEventList::FindEvent -E- scan init failed"<<endl;
     }
     return rstatus;
}

ooStatus      AMSEventList::AddEvent(integer runNumber, uinteger eventNumber, 
                                      integer eventW)
  // runNumber    - run number
  // eventNumber  - event number
  // eventW       - match to AMSFFKEY.Write datacard
  //
{
  	 ooStatus	       rstatus = oocSuccess;	// Return status
         ooHandle(AMSEventD)   eventH;
         ooHandle(AMSmceventD) mceventH;
         char                err_mess[80];
         integer             i;
         integer             run;
         integer             runtype;
         integer             eventn;
         time_t              time;

        integer              nevts;
        integer              nevtsP;

        ooHandle(AMSEventList) listH = ooThis();
        if (listH == NULL) {
         rstatus = oocError;
         strcpy(err_mess, "listH pointer is NULL");
         goto error;
        }

        if ((eventW/DBWriteDumm)%2 == 1) {
          rstatus = AddDummyMCEvent(runNumber, eventNumber, eventW);
          strcpy(err_mess, "Cannot add mcevent ");
          goto error;
        }

        if ((eventW/DBWriteMCEv)%2 == 1) {
          rstatus = FindEvent(runNumber, eventNumber,oocUpdate, mceventH);
           if (rstatus == oocSuccess) {
             cout<<"AddEvent -W- Event "<<eventNumber<<", run "<<runNumber
                 <<" exists. Will be overwritten"<<endl;
             ooDelete(mceventH);
             listH -> decNEvents();
           }
          rstatus = AddMCEvent(runNumber, eventNumber, eventW);
          strcpy(err_mess, "Cannot add mcevent ");
          goto error;
        }

        // get pointers to all containers
        rstatus = CheckAllContainers(oocUpdate);
        if(rstatus != oocSuccess) return rstatus;

        // check event
        rstatus = FindEvent(runNumber, eventNumber,oocUpdate, eventH);
        if (rstatus == oocSuccess) {
         cout<<"AddEvent -W- Event "<<eventNumber<<", run "<<runNumber
             <<" exists. Will be overwritten"<<endl;
         ooDelete(eventH);
         listH -> decNEvents();
         }

        // Add Event Header

        nevts   = listH -> getNEvents();
        nevtsP  = listH -> getNEventsP();

        if (dbg_prtout == 1) cout <<"found events in "<<nevts<<endl;

           //Get Event from AMSEvent (not AMSeventD)
           if(AMSEvent::gethead() == NULL) {
             cerr<<"LMS::AddEvent -E- gethead = NULL"<<endl;
             return oocError;
           }
           runtype = AMSEvent::gethead() -> getruntype();
           time    = AMSEvent::gethead() -> gettime();
           eventH = new(listH) 
                     AMSEventD(runNumber, eventNumber,  runtype, time);
           listH -> incNEvents();
           listH -> incNEventsP();
           nevtsP++;
           eventH -> setPosition(nevtsP);
           // add event ID to the event map

       // AddmcEventg 
        if ( (eventW/DBWriteMCEg)%2 == 1) {
         rstatus = AddmcEventg(eventH);
         if (rstatus != oocSuccess) {
          strcpy(err_mess, "Cannot add mceventg to the  container");
           goto error;}
        }
        
        // add all MC banks
        if ( (eventW/DBWriteMC)%2 == 1) {
         //AddTrMCCluster
         rstatus = AddTrMCCluster(eventH);
         if (rstatus != oocSuccess) {
          strcpy(err_mess, "Cannot add TrMCCluster to the  container");
          goto error;}

         //AddTOFMCCluster         
         rstatus = AddTOFMCCluster(eventH);
         if (rstatus != oocSuccess) {
          strcpy(err_mess, "Cannot add TOFMCCluster to the  container");
          goto error;}
          
         //AddAntiMCCluster         
         rstatus = AddAntiMCCluster(eventH);
         if (rstatus != oocSuccess) {
          strcpy(err_mess, "Cannot add AntiMCCluster to the  container");
          goto error;}

         //AddCTCMCCluster         
         rstatus = AddCTCMCCluster(eventH);
         if (rstatus != oocSuccess) {
          strcpy(err_mess, "Cannot add CTCMCCluster to the  container");
          goto error;}
        }
        
        // add all banks
        if ( (eventW/DBWriteRecE)%2 == 1) {
        //AddTrRecHits
        for (i=0; i<6; i++) {
         rstatus = AddTrRecHit(i, eventH);
         if (rstatus != oocSuccess) {
          strcpy(err_mess, "Cannot add TrRecHit to the  container");
           goto error;}
         }

        //AddTrCluster
         for (i=0; i<2; i++) {
           rstatus = AddTrCluster(i, eventH);
           if (rstatus != oocSuccess) {
            strcpy(err_mess, "Cannot add TrCluster to the  container");
            goto error; }
         }

         //AddTrTrack
         rstatus = AddTrTrack(eventH);
         if (rstatus != oocSuccess) {
          strcpy(err_mess, "Cannot add TrTrack to the  container");
          goto error;}

         //LinkHitCluster
         for (i=0; i<6; i++) {
          rstatus = LinkHitClusterD(i, eventH);
          if (rstatus != oocSuccess) {
           strcpy(err_mess, "Cannot link Hit to Cluster");
           goto error;}
         }

         //LinkTrackHit
         rstatus = LinkTrackHitD(eventH);
         if (rstatus != oocSuccess) {
          strcpy(err_mess, "Cannot link Track to hit");
          goto error;}

          //AddTOFCluster
          for (i=0; i<4; i++) {
           rstatus = AddTOFCluster(i, eventH);
           if (rstatus != oocSuccess) {
            strcpy(err_mess, "Cannot add TOF Cluster to the container");
            goto error;}
           }

          //AddCTCCluster
           rstatus = AddCTCCluster(eventH);
           if (rstatus != oocSuccess) {
            strcpy(err_mess, "Cannot add CTC Cluster to the container");
            goto error;}

          //AddBeta
          rstatus = AddBeta(eventH);
          if (rstatus != oocSuccess) {
           strcpy(err_mess, "Cannot add Beta to the container");
           goto error;}

          //AddCharge
          rstatus = AddCharge(eventH);
          if (rstatus != oocSuccess) {
           strcpy(err_mess, "Cannot add Charge to the  container");
           goto error;}

        }
        if ( (eventW/DBWritePart)%2 == 1) {
          //AddParticle
          rstatus = AddParticle(eventH);
          if (rstatus != oocSuccess) {
           strcpy(err_mess, "Cannot add Particle to the  container");
           goto error;}
        }
error:
        if (rstatus != oocSuccess) cout <<"AddEvent-E- "<<err_mess<<endl;
        return rstatus;
}


ooStatus      AMSEventList::AddTrCluster(
              const integer N, ooHandle(AMSEventD) & eventH)

{
	ooStatus	        rstatus = oocSuccess;	// Return status
        ooHandle(AMSEventList)  listH = ooThis();

        if (eventH == NULL) {
          cerr <<"Error - eventH  == NULL "<<endl;
         return oocError;
        }

        if (contClusterH[N] == NULL) {
         rstatus= CheckContainer(trclusterCont[N], oocUpdate, contClusterH[N]);
         if (rstatus != oocSuccess) return rstatus;
        }

        // get number of clusters so far
        integer tclusters  = listH -> getNClusters(N);
        if (dbg_prtout == 1)
         cout <<"found clusters in "<<nameC[N]<<" "<<tclusters<<endl;

        // get first cluster
        AMSTrCluster* p = (AMSTrCluster*)AMSEvent::gethead() -> 
                                                   getheadC("AMSTrCluster",N);
        if (p == NULL && dbg_prtout == 1) {
         cout <<"AMSEventList::AddTrCluster-I- AMSTrCluster* p == NULL"<<endl;
         return oocSuccess;
        }

        if (dbg_prtout) cout <<"AddTrCluster -I- nelements "
                   <<AMSEvent::gethead()->getC("AMSTrCluster",N)->getnelem()
                   <<" for AMSTrCluster Cont "<<N<<endl;
        while ( p != NULL) {
         clusterH = new(contClusterH[N]) AMSTrClusterD(p);
         number*    pValues;
         number     pvalues[100];
         integer    tnelem = p -> getnelem();
         if ( tnelem > 0 ) {
          if (tnelem < 100) {
            p -> getValues(pvalues);
            clusterH -> setValues(pvalues);
          } else {
           pValues = new number[tnelem];
           p -> getValues(pValues);
           clusterH -> setValues(pValues);
           delete []pValues;
          }           
         }
         //cout<<"associate event with cluster "<<tclusters+1<<endl;
         rstatus = eventH -> add_pCluster(clusterH);
         if (rstatus != oocSuccess) {
          cout << "AMSEventList:AddTrCluster: Error - cannot set the "
               << "Cluster to Event 'pCluster' association." << endl;
          return rstatus;
         }
        tclusters++;
        eventH -> incTrClusters();
        p -> setContPos(tclusters);
        clusterH -> setPosition(tclusters);
        listH -> incNClusters(N);
        p = p -> next();
       }
        return rstatus;
}

ooStatus      AMSEventList::
                AddTrRecHit(const integer N, ooHandle(AMSEventD)& eventH) 
{
	ooStatus	       rstatus = oocSuccess;	// Return status
        ooHandle(AMSTrRecHitD) layerH;
        ooHandle(AMSEventList) listH = ooThis();

        if (eventH == NULL) {
          cerr <<"Error - eventH  == NULL "<<endl;
         return oocError;
        }

        if (contTrLayerH[N] == NULL) {
         rstatus = CheckContainer(trlayerCont[N], oocUpdate, contTrLayerH[N]);
         if (rstatus != oocSuccess) return rstatus;
        }

        for (integer j =0; j<2; j++) {
         if(contClusterH[j] == NULL) {
          rstatus = CheckContainer(trclusterCont[j],oocUpdate,contClusterH[j]);
          if (rstatus != oocSuccess) return rstatus;
         }
        }

        integer   hits  = listH -> getNHits(N);
        if (dbg_prtout == 1) cout <<"found hits for "<<nameTrL[N]
                                  <<" "<<hits<<endl;
        //AMSTrRecHit* p = AMSTrRecHit::gethead(N);
        AMSTrRecHit* p = (AMSTrRecHit*)AMSEvent::gethead() -> 
                                                   getheadC("AMSTrRecHit",N);
        if (p == NULL && dbg_prtout == 1) {
          cout <<"AMSEventList::AddTrRecHit-I-  "<<nameTrL[N]
               <<" AMSTrRecHit* p == NULL"<<endl;
           return oocSuccess;
        }
        while ( p != NULL) {
         layerH = new(contTrLayerH[N]) AMSTrRecHitD(p);
         AMSgSen* pSen = p -> getpsen();
         integer gid  = pSen -> getid();
         char*   name = pSen -> getname();
         if (dbg_prtout) cout<<"AddTrRecHit -I- psensor -> id "<<gid
                             <<", psensor -> name "<<name<<endl;
         layerH -> setgid(gid);
         layerH -> setname(name);
         //---rstatus = layerH -> set_pEventL(eventH);
         rstatus = eventH -> add_pTrRecHitS(layerH);
         if (rstatus != oocSuccess) {
          cerr << "AMSEventList:AddTrrecHit: Error - cannot set the "
               << "TrRecHit to Event 'pEventL' association." << endl;
          return rstatus;
         }
         hits++;
         eventH -> incTrHits();
         p -> setContPos(hits);
         layerH -> setPosition(hits);
         listH  -> incNHits(N);
         if (dbg_prtout) cout<<"AddTrRecHit -I- add hit pos "<<p->getpos()
                             <<" ContPos "<<p -> getContPos()
                             <<", Layer "<< p -> getLayer()<<endl;;
         p = p -> next();
        }
        return rstatus;
}

ooStatus   AMSEventList::CopyTrRecHit(ooHandle(AMSEventD)& eventH, ooMode mode)
{
	ooStatus	       rstatus = oocSuccess;	// Return status
        integer                ihits = 0;
        ooHandle(AMSEventList) listH = ooThis();

        if (eventH == NULL) {
         cerr <<"CopyTrRecHit-E- eventH  == NULL "<<endl;
         return oocError;
        }

         for (integer j=0; j<6; j++) {
          if (contTrLayerH[j] == NULL) {
           rstatus = CheckContainer(trlayerCont[j], mode, contTrLayerH[j]);
           if (rstatus != oocSuccess) return rstatus;
          }
         }

         eventH -> pTrRecHitS(trRecHitItr, mode);

         while (trRecHitItr.next()) {
          if (dbread_only != 0) {
           AMSTrRecHit* p = new  AMSTrRecHit();
           trRecHitItr -> copy(p);

           integer posCont= trRecHitItr -> getPosition();
           p -> setContPos(posCont);

           integer gid  = trRecHitItr -> getgid();
           char*   name = trRecHitItr -> getname();

           AMSgSen *psen;
           psen = (AMSgSen*)AMSJob::gethead()->getgeomvolume(AMSID(name,gid));
           if (psen) {
             if (dbg_prtout) cout<<"AMSEventList::CopyTrRecHit -I- "
                                 <<"set pointer to sensor with id "<<gid
                                 <<", name "<<name<<endl;
             p -> setSensorP(psen);
           } else {
             cout<<"AMSEventList::CopyTrRecHit -E- cannot getgeomvolume "
                 <<"for id "<<gid<<", name "<<name<<endl;
             cout<<"cannot set sensor pointer"<<endl;
           }

           integer layer  = trRecHitItr -> getLayer();
           AMSEvent::gethead()->addnext(AMSID("AMSTrRecHit",layer-1),p);
          }
          ihits++;
         }
         if (dbg_prtout == 1) {
          cout <<"AMSEventList::CopyTrRecHit:: hits "<<ihits
             <<" total size "<<sizeof(AMSTrRecHitD)*ihits<<endl;
         }
        event_size = event_size + sizeof(AMSTrRecHitD)*ihits;
        return rstatus;
}
ooStatus      AMSEventList::
                  CopyTrCluster(ooHandle(AMSEventD)& eventH, ooMode mode)

{
	ooStatus	       rstatus = oocSuccess;	// Return status
        integer                iclusters = 0;
        ooHandle(AMSEventList)  listH = ooThis();

        if (eventH == NULL) {
          cerr <<"Error - eventH  == NULL "<<endl;
         return oocError;
        }

         for (integer j =0; j<2; j++) {
          if(contClusterH[j] == NULL ) {
           rstatus = CheckContainer(trclusterCont[j], mode, contClusterH[j]);
           if (rstatus != oocSuccess) return rstatus;
          }
         }


         number      pvalues[100];
         number*     pValues;
         eventH -> pCluster(trClusterItr, mode);
         while (trClusterItr.next()) {
          if (dbread_only != 0) {
            //AMSTrCluster* p = new AMSTrCluster();
           //trClusterItr -> copy(p);
            if (dbg_prtout) cout <<"CopyTrCluster nelem, nelemL, nelemR "
                                 <<trClusterItr -> getnelem()<<", "
                                 <<trClusterItr -> getnelemL()<<", "
                                 <<trClusterItr -> getnelemR()<<endl;
           integer nelem = trClusterItr -> getnelem();
           if(nelem > 0) {
            if (nelem < 100) {
             trClusterItr -> getValues(pvalues);
             //p -> setValues(pvalues);
            }
           }
           if (dbg_prtout) {
             cout <<"CopyTrCluster nelem, nelemL, nelemR "
                  <<trClusterItr -> getnelem()<<", "
                  <<trClusterItr -> getnelemL()<<", "
                  <<trClusterItr -> getnelemR()<<endl;
             for (int j=0; j < nelem; j++) { cout <<"j,pvalues "<<j<<", "
                                                  <<pvalues[j]<<endl; }
           }
           AMSTrCluster* p = new AMSTrCluster(
                                              trClusterItr -> getidsoft(), 
                                              trClusterItr -> getstatus(), 
                                              trClusterItr -> getnelemL(), 
                                              trClusterItr -> getnelemR(), 
                                              trClusterItr -> getVal(), 
                                              trClusterItr -> getSigma(),
                                              trClusterItr -> getcofg(), 
                                              trClusterItr -> getRms(),
                                              pvalues);
           integer     posCont=  trClusterItr -> getPosition();
           p -> setContPos(posCont);
           integer side = trClusterItr -> getSide();
           AMSEvent::gethead() -> 
                        addnext(AMSID("AMSTrCluster",side),p);
          }
          iclusters++;
         }
         if (dbg_prtout == 1) {
          cout 
          <<"AMSEventList::CopyTrCluster: Layers X&Y "<<" clusters "<<iclusters
          <<" total size "<<sizeof(AMSTrClusterD)*iclusters<<endl;
         }
        event_size = event_size + sizeof(AMSTrClusterD)*iclusters;
        return rstatus;
}

ooStatus      AMSEventList::
                  LinkHitClusterD(const integer N,ooHandle(AMSEventD)& eventH)

{
	ooStatus	       rstatus = oocSuccess;	// Return status
        ooMode                 mode  = oocUpdate;

// find event
        if (eventH == NULL) {
          cerr <<"Error - eventH  == NULL "<<endl;
         return oocError;
        }

// Get head of hit's container
        //AMSTrRecHit* p = AMSTrRecHit::gethead(N);
        AMSTrRecHit* p = (AMSTrRecHit*)AMSEvent::gethead() -> 
                                                   getheadC("AMSTrRecHit",N);
        while ( p != NULL) {
         integer  posH    = p -> getContPos();     // position of hit 
         for (integer j=0; j<2; j++) {
          AMSTrCluster* pC = p -> getClusterP(j);  // pointer to assoc cluster
          if (pC != NULL) {
           integer  posXY   = pC -> getContPos();  // position of clusterX/Y
           integer  side    = pC -> getSide();
           if( posH != 0 && posXY != 0) {
           // find cluster 
           char pred[32];
           (void) sprintf(pred,"_Position=%d",posXY);
           eventH -> pCluster(trClusterItr,mode,oocAll,pred);
           while (trClusterItr.next() != NULL) {
            if (trClusterItr -> getSide() == side) {
             (void) sprintf(pred,"_Position=%d && _Layer=%d",posH,N+1);
              eventH -> pTrRecHitS(trRecHitItr,mode,oocAll,pred);
              while (trRecHitItr.next() != NULL) {
               if(side == 0) rstatus = trRecHitItr -> 
                                                  set_pClusterX(trClusterItr);
               if(side == 1) rstatus = trRecHitItr -> 
                                                  set_pClusterY(trClusterItr);
               if (rstatus != oocSuccess) {
                cerr << "AMSEventList:LinkHitClusterD: -E- cannot set the "
                     << "Cluster to Hit association." << endl;
                return rstatus;
               }
              } //end while trRecHitItr
             } // getSide() == side
            } // end while trClusterItr.next()
           }
          } else {
           if (dbg_prtout) cout <<"AMSEventList:LinkHitClusterD:  hit "<<posH
                                <<" point to cluster NULL"<<endl;}
         } 
         p = p -> next();
        }
        return rstatus;
}

ooStatus      AMSEventList::LinkHitClusterM(ooHandle(AMSEventD)& eventH)

{

	ooStatus	       rstatus = oocSuccess;	// Return status
        ooMode                 mode    = oocRead;
// find event
        if (eventH == NULL) {
          cerr <<"Error - eventH  == NULL "<<endl;
         return oocError;
        }

// find containers
        for (integer l=0; l<6; l++) {
// All containers and events are found
// don't be confused all hits on a layer belongs to the same cluster
// Get head of hit's container
          //AMSTrRecHit* p = AMSTrRecHit::gethead(l);
         AMSTrRecHit* p = (AMSTrRecHit*)AMSEvent::gethead() -> 
                                                   getheadC("AMSTrRecHit",l);
         while ( p != NULL) {
          integer  posH    = p -> getContPos();         // position of hit 
          if (posH != 0 ) {                             // pos =0 means
           integer  layer    = p -> getLayer();         // layer
           char pred[32];                               // hit is not in dbase 
                                                        // find it in dbase
           (void) sprintf(pred,"_Position=%d && _Layer=%d",posH,layer);    
           eventH -> pTrRecHitS(trRecHitItr, mode, oocAll, pred);
           if(trRecHitItr.next() != NULL) {             // found
            for (integer j=0; j<2; j++) {
             if(j==0) clusterH = trRecHitItr -> pClusterX();
             if(j==1) clusterH = trRecHitItr -> pClusterY();
             if(clusterH != NULL) {
              integer pos = clusterH -> getPosition(); 
              // find the same cluster in Upool
              AMSTrCluster* pU = (AMSTrCluster*)AMSEvent::gethead() -> 
                                                getheadC("AMSTrCluster",j);
              while (pU != NULL ) {
               integer posU = pU -> getContPos();  //position cluster in Upool
               if (pos == posU) {
                if (dbg_prtout == 1)
                 cout <<" found hit pos "<<posH<<" Layer "<<layer
                      <<" cldbase "<<pos<<" cl upool "<<posU<<endl;
                break;
               }
               pU = pU -> next();
              }

              if (pU != NULL) {
               p -> setClusterP(pU,j);
              } else{ 
               cout <<"AMSEventList::LinkHitClusterM - W - cannot find an "
                  <<" associated cluster"<<endl;
              }
             } else {
              cout<<"for hit pos "<<posH<<" no association with cluster"<<endl;
             }
            }
           }
          }
          p = p -> next();
         }
        }
        return rstatus;
}

ooStatus      AMSEventList::AddTrTrack(ooHandle(AMSEventD) & eventH)

{
	ooStatus	        rstatus = oocSuccess;	// Return status
        ooHandle(AMSEventList)  listH = ooThis();

        if (eventH == NULL) {
          cerr <<"Error - eventH  == NULL "<<endl;
         return oocError;
        }

        if (contTrackH == NULL) {
         rstatus = CheckContainer(trtrackCont, oocUpdate, contTrackH);
         if (rstatus != oocSuccess) return rstatus;
        }

        // get number of tracks so far
        integer tracks  = listH -> getNTracks();
        if (dbg_prtout == 1) {
         cout <<"AMSEventList::AddTrTrack -I- found tracks in TrTracks "
              <<tracks<<endl;
         cout <<"AMSEventList::AddTrTrack -I- number of patterns "<<npat<<endl;
        }
        if (npat < 1) {
         cout <<"AMSEventList::AddTrTrack -I- Quit (npat < 1) "<<endl;
         return oocSuccess;
        }
        // get first track of first container
        for (integer i=0; i<npat; i++) {
         AMSContainer* pCont = AMSEvent::gethead() -> getC("AMSTrTrack",i);
         if (pCont != NULL) {
          integer nelem = pCont -> getnelem();
          if (nelem == 0) {
           if(dbg_prtout != 0) cout <<"AMSEventList::AddTrTrack-I- pattern "
                                   <<i<<" elements "<<nelem<<endl;
          }
          if ( nelem > 0) {
           AMSTrTrack* p = (AMSTrTrack*)AMSEvent::gethead() -> 
                                                 getheadC("AMSTrTrack",i);
           while (p != NULL) {
            trackH = new(contTrackH) AMSTrTrackD(p);
            tracks++;
            p -> setContPos(tracks);
            listH  -> incNTracks();
            trackH -> setPosition(tracks);
            rstatus = trackH -> set_pEventT(eventH);
            //           rstatus = eventH -> add_pTrack(trackH);
            eventH -> incTracks();
            if (dbg_prtout) {
              cout<<"AddTrTrack -I- add track pos "<<p->getpos()
                   <<" ContPos "<<p -> getContPos()<<endl;
              for (int j=0; j<6; j++){
               if(p->getphit(j))
                cout<<"track->hit->getpos() "<<p->getphit(j)->getpos()
                    <<" track->hit->ContPos "<<p->getphit(j)->getContPos()
                    <<", layer "<<p->getphit(j)->getLayer() <<endl;
              }
            }
            p = p -> next();
           }
          }
         } else {
           if (dbg_prtout) cout <<"AMSEventList::AddTrTrack-I- for pattern "
                                 <<i<<" AMSTrTrack* p == NULL"<<endl;
         }
        }
        return rstatus;
}

ooStatus  
        AMSEventList::LinkTrackHitD(ooHandle(AMSEventD)& eventH)
{
	ooStatus	       rstatus = oocSuccess;	// Return status
        ooMode                 mode    = oocUpdate;
        integer                i;

// find event
        if (eventH == NULL) {
          cerr <<"Error - eventH  == NULL "<<endl;
         return oocError;
        }

// find track for all patterns
        for (i =0; i<npat; i++) {
// Get head of track's container
        AMSContainer* pCont = AMSEvent::gethead() -> getC("AMSTrTrack",i);
        integer nelem = pCont -> getnelem();
        if ( nelem > 0) {
          AMSTrTrack* p = (AMSTrTrack*)AMSEvent::gethead() -> 
                                                 getheadC("AMSTrTrack",i);
        while ( p != NULL) {
          integer  posT    = p -> getContPos();     // position of track
          char pred[32];
          (void) sprintf(pred,"_Position=%d",posT);
          if (dbg_prtout) cout<<"LinkTrackHit -I- search for track "
                              <<pred<<endl;
          eventH -> pTrack(trTrackItr,mode,oocAll,pred);
          integer layer = 0;
          for (integer j=0; j<6; j++) {             
           AMSTrRecHit* pH = p -> getphit(j);      // pointer to assoc hit
           if (pH != NULL) {
            integer  posH   = pH -> getContPos();  // position of Hit
            if (dbg_prtout) cout<<"LinkTrackHit -I- assoc hit pos "
                                <<p ->getpos()<<", ContPos "<<posH<<endl;
            if (posH > 0) { // was if (posH != 0)
             layer = pH -> getLayer();
             // find hit
             char pred[32];
             (void) sprintf(pred,"_Position=%d && _Layer=%d",posH,layer);
              if (dbg_prtout) cout<<"LinkTrackHit -I- search for hit "
                                   <<pred<<endl;
             eventH -> pTrRecHitS(trRecHitItr,mode,oocAll,pred);
             while (trRecHitItr.next() != NULL) {
              rstatus = trTrackItr -> add_pTrRecHitT(trRecHitItr);
              if (rstatus != oocSuccess) {
               cerr << "AMSEventList:LinkTrackHitD: Error - cannot set the ";
               cerr << "Track to Hit association." << endl;
               return rstatus;
              }
             }// while trRecHitItr - should be only one hit 
            } // posH != 0
           }  // pH   != NULL
           if (layer == 6) break;
          }   // end of layers scan
         p = p -> next();
        }
       }
      }
        return rstatus;
}

ooStatus  AMSEventList::CopyTrTrack(ooHandle(AMSEventD)& eventH, ooMode mode)

{
	ooStatus	       rstatus = oocSuccess;	// Return status
        integer                itracks = 0;

        if (eventH == NULL) {
          cerr <<"Error - eventH  == NULL "<<endl;
         return oocError;
        }

         eventH -> pTrack(trTrackItr, mode);

         while (trTrackItr.next()) {
          if (dbread_only != 0) {
           AMSTrTrack* p = new AMSTrTrack();
           trTrackItr -> copy(p);
           integer     pattern = trTrackItr  -> getPattern();
           integer     posCont =  trTrackItr -> getPosition();
           p -> setContPos(posCont);
           AMSEvent::gethead() -> addnext(AMSID("AMSTrTrack",pattern),p);
          }
          itracks++;
         }
         if (dbg_prtout == 1) {
          cout <<"AMSEventList::CopyTrTrack: Patterns  "<<npat
             <<" "<<itracks<<"Total size "<<sizeof(AMSTrTrackD)*itracks<<endl;
         }
         event_size = event_size + sizeof(AMSTrTrackD)*itracks;
        return rstatus;
}

ooStatus AMSEventList::LinkTrackHitM(ooHandle(AMSEventD)& eventH)
{
	ooStatus	       rstatus = oocSuccess;	// Return status
        ooMode                 mode    = oocRead;
        integer                i;

// find event
        if (eventH == NULL) {
          cerr <<"Error - eventH  == NULL "<<endl;
         return oocError;
        }

// All containers and events are found
        // find track for all patterns
        for (i =0; i<npat; i++) {
        integer layer;
// Get head of track's container
        AMSContainer* pCont = AMSEvent::gethead() -> getC("AMSTrTrack",i);
        integer nelem = pCont -> getnelem();
        if ( nelem > 0) {
          AMSTrTrack* p = (AMSTrTrack*)AMSEvent::gethead() -> 
                                                 getheadC("AMSTrTrack",i);
        while ( p != NULL) {
         integer  posT    = p -> getContPos();     // position of track
         if(posT != 0) {                            
          char pred[32];
          (void) sprintf(pred,"_Position=%d",posT);
          eventH -> pTrack(trTrackItr,mode,oocAll,pred);
          if(trTrackItr.next() != NULL) {                  //    in dbase
           integer Npos = 0;                              //keep it for hits
           trTrackItr -> pTrRecHitT(trRecHitItr, mode);
           while (trRecHitItr.next()) {                   // get position of  
             integer posH = trRecHitItr -> getPosition(); // assoc.hit in dbase
                     layer = trRecHitItr-> getLayer();
             if (dbg_prtout) cout <<"Pattern, Track, Hit position "<<i<<" "
                                  <<posT<<" "<<posH<<" layer "<<layer<<endl;
            // find the same hit in Upool
            AMSTrRecHit* pH = (AMSTrRecHit*)AMSEvent::gethead() -> 
                                              getheadC("AMSTrRecHit",layer-1);
            while (pH != NULL ) {
             if (posH != pH -> getContPos()) {
               pH = pH -> next();
             } else{
               break;
             }
            }
            if (pH != NULL) {
             p -> setHitP(pH,Npos);
             Npos++;
            } else{ 
             cout <<"AMSEventList::LinkTrackHitM - W - cannot find an "
                  <<" associated hit "<<endl;
            }
           }
          }
          if (layer == 6) break;
         }
          p = p -> next();
          } //
         } 
        }
        return rstatus;
}

ooStatus  AMSEventList::GetKeepingList(ooHandle(AMSEventList)& mylistH)
{
  ooStatus rstatus = oocSuccess;
  ooHandle(ooDBObj)   databaseH;

  //if (mylistH == NULL ) {
  //cout <<" AMSEventList::GetKeepingList -I- mylistH is NULL, get it"<<endl;
   ooThis().containedIn(databaseH);
   if (!mylistH.exist(databaseH, "EventKeeping", oocUpdate)) {
    cout << "AMSEventList::GetKeepingList: EventKeeping does not exist"<<endl;
    return oocError;
   }
   //}

  return rstatus;

}

ooStatus AMSEventList::AddTrMCCluster(ooHandle(AMSEventD) & eventH)

{
	ooStatus	        rstatus = oocSuccess;	// Return status
        ooHandle(AMSEventList)  listH = ooThis();

        if (eventH == NULL) {
          cerr <<"Error - eventH  == NULL "<<endl;
         return oocError;
        }

        if (contMCClusterH == NULL) {
         rstatus = CheckContainer(trmcclusterCont, oocUpdate, contMCClusterH);
         if (rstatus != oocSuccess) return rstatus;
        }
        // get number of MCClusters so far
        integer mcclusters  = listH ->   getNMCClusters();
        if (dbg_prtout == 1)
         cout <<"found TrMCClusters  "<<mcclusters<<endl;

        // get first cluster
        AMSTrMCCluster* p = (AMSTrMCCluster*)AMSEvent::gethead() -> 
                                               getheadC("AMSTrMCCluster",0);
        if (p == NULL) {
        cout <<"AMSEventList::AddMCCluster-I- AMSTrMCCluster* p == NULL"<<endl;
        return oocSuccess;
        }

        if (dbg_prtout) cout<<"AddTrMCCluster -I- nelements "
                   <<AMSEvent::gethead()->getC("AMSTrMCCluster",0)->getnelem()
                            <<endl;
        while ( p != NULL) {
         MCClusterH = new(contMCClusterH) AMSTrMCClusterD(p);
         mcclusters++;
         p -> setContPos(mcclusters);
         MCClusterH -> setPosition(mcclusters);
         listH -> incNMCClusters();
         rstatus = eventH -> add_pMCCluster(MCClusterH);
         //---rstatus = MCClusterH -> set_pEventMC(eventH);
         if (rstatus != oocSuccess) {
          cerr << "AMSEventList:AddTrMCCluster: Error - cannot set the ";
          cerr << "MCCluster to Event 'pEventMC' association." << endl;
          return rstatus;
         }
         if (dbg_prtout==2) cout
                     <<"AMSEventList::AMSTrMCCluster -I- add cluster"
                     <<" with position "<<p -> getpos()<<endl;
         p = p -> next();
       }
        return rstatus;
}

ooStatus AMSEventList::
             CopyTrMCCluster(ooHandle(AMSEventD) & eventH, ooMode mode)

{
	ooStatus	        rstatus = oocSuccess;	// Return status
        ooHandle(AMSEventList)  listH = ooThis();

        if (eventH == NULL) {
          cerr <<"Error - eventH  == NULL "<<endl;
         return oocError;
        }

        if (contMCClusterH == NULL) {
         rstatus = CheckContainer(trmcclusterCont, mode, contMCClusterH);
         if (rstatus != oocSuccess) return rstatus;
        }

         eventH -> pMCCluster(trMCClusterItr, mode);
         integer imcs=0;

         while (trMCClusterItr.next()) {
          if (dbread_only != 0) {
           AMSTrMCCluster* p = new AMSTrMCCluster();
           trMCClusterItr -> copy(p);
           integer    pos = trMCClusterItr -> getPosition();
           p -> setContPos(pos);
           if (AMSEvent::gethead() != NULL) {
            AMSEvent::gethead() -> addnext(AMSID("AMSTrMCCluster",0),p);
           } else {
            cout<<"AMSEventList::CopyTrMCCluster -E- AMSEvent::gethead is NULL"
                <<endl;
            return oocError;
           }
          }
          imcs++;
         }
         if (dbg_prtout == 1) {
          cout <<"AMSEventList::CopyTrMCCluster:  clusters "<<imcs
              <<" Total size "<<sizeof(AMSTrMCCluster)*imcs<<endl;
         }
         event_size = event_size + sizeof(AMSTrMCClusterD)*imcs;
        return rstatus;
}

ooStatus      AMSEventList::
                 AddTOFCluster(const integer N, ooHandle(AMSEventD)& eventH) 

{
	ooStatus	         rstatus = oocSuccess;	// Return status
        ooHandle(AMSTOFClusterD) layerH;
        ooHandle(AMSEventList)   listH = ooThis();

        if (N > 4) {
         cerr <<"AMSEventList::AddTOFCluster - E - Invalid layer number "
              <<N<<endl;
         return oocError;
        }

        if (eventH == NULL) {
          cerr <<"Error - eventH  == NULL "<<endl;
         return oocError;
        }

        if (contLayerSH[N] == NULL) {
         rstatus = CheckContainer(sclayerCont[N], oocUpdate, contLayerSH[N]);
         if (rstatus != oocSuccess) return rstatus;
        }

        integer   tofcl  = listH -> getNTOFCl(N);
        if (dbg_prtout == 1)
        cout <<"AMSEventList::AddTOFCluster -I- found tof clusters for "
             <<nameS[N]<<" "<<tofcl<<endl;
        
        //AMSTOFCluster * p = AMSTOFCluster::gethead(N);
        AMSTOFCluster* p = (AMSTOFCluster*)AMSEvent::gethead() -> 
                                                   getheadC("AMSTOFCluster",N);
        if (p == NULL) {cout <<
              "AMSEventList::AddTOFCluster-I- AMSTOFCluster* p == NULL"<<endl;
                return oocSuccess;
        }
        while ( p != NULL) {
         layerH = new(contLayerSH[N]) AMSTOFClusterD(p);
         //---rstatus = layerH -> set_pEventTOF(eventH);
         rstatus = eventH -> add_pTOFCluster(layerH);
         if (rstatus != oocSuccess) {
          cerr << "AMSEventList:AddTOFCluster: Error - cannot set the "
               << "TOFCluster to Event 'pTOFCluster' association." << endl;
          return rstatus;
         }
         tofcl++;
         eventH -> incTOFClusters();
         p -> setContPos(tofcl);
         layerH -> setPosition(tofcl);
         listH  -> incNTOFCl(N);
         p = p -> next();
        }
        return rstatus;
}

ooStatus      AMSEventList::
                 CopyTOFCluster(ooHandle(AMSEventD)& eventH, ooMode mode) 

{
	ooStatus	         rstatus = oocSuccess;	// Return status
        ooHandle(AMSTOFClusterD) layerH;
        ooHandle(AMSEventList)   listH = ooThis();
        integer                  itofcl = 0;


        if (eventH == NULL) {
          cerr <<"Error - eventH  == NULL "<<endl;
         return oocError;
        }

        for (integer i =0; i<4; i++) {
         if (contLayerSH[i] == NULL) {
          rstatus = CheckContainer(sclayerCont[i], mode, contLayerSH[i]);
          if (rstatus != oocSuccess) return rstatus;
         }
        }


        eventH -> pTOFCluster(tofClusterItr, mode);

        while (tofClusterItr.next()) {
         if (dbread_only != 0) {
           //AMSTOFCluster* p = new  AMSTOFCluster();
           //tofClusterItr -> copy(p);
           AMSTOFCluster* p = new  AMSTOFCluster(
                                                 tofClusterItr -> getstatus(),
                                                 tofClusterItr -> getntof(),
                                                 tofClusterItr -> getplane(),
                                                 tofClusterItr -> getedep(),
                                                 tofClusterItr -> getcoo(),
                                                 tofClusterItr -> getecoo(),
                                                 tofClusterItr -> gettime(),
                                                 tofClusterItr -> getetime()
                                                 );
           
          integer posCont= tofClusterItr -> getPosition();
          p -> setContPos(posCont);
          integer ntof = tofClusterItr -> getntof();
          AMSEvent::gethead()->addnext(AMSID("AMSTOFCluster",ntof-1),p);
         }
         itofcl++;
        }
         if (dbg_prtout == 1) {
          cout <<"AMSEventList::CopyTOFCluster: clusters "<<itofcl
             <<" total size "<<sizeof(AMSTOFClusterD)*itofcl<<endl;
         }
        event_size = event_size + sizeof(AMSTOFClusterD)*itofcl;
        return rstatus;
}

ooStatus      AMSEventList::AddBeta(ooHandle(AMSEventD) & eventH)

{
	ooStatus	        rstatus = oocSuccess;	// Return status
        ooMode                  mode    = oocUpdate;

        ooHandle(AMSEventList)  listH = ooThis();

        if (eventH == NULL) {
          cerr <<"Error - eventH  == NULL "<<endl;
         return oocError;
        }

        if (contBetaH == NULL) {
         rstatus = CheckContainer(betaCont, oocUpdate, contBetaH);
         if (rstatus != oocSuccess) return rstatus;
        }

        for (integer j = 0; j<4; j++)
         {
          if (contLayerSH[j] == NULL) {
           rstatus = CheckContainer(sclayerCont[j], oocUpdate, contLayerSH[j]);
           if (rstatus != oocSuccess) return rstatus; 
          }
         }

        // get number of betas so far
        integer betas  = listH -> getNBetas();
        if (dbg_prtout == 1) {
          cout <<"AMSEventList:AddBeta-I- found betas in Beta "<<betas<<endl;
          cout <<"AMSEventList:AddBeta-I- number of patterns "<<npatb<<endl;
        }
        if (npatb < 1) {
         cout <<"AMSEventList:AddBeta-W- Quit (npatb<1) "<<endl;
         return oocSuccess;
        }
        for (integer i=0; i<npatb; i++) {
          if (dbg_prtout == 1) cout <<" do AddBeta for container "<<i<<endl;
         // get the first beta in the container
         AMSContainer* pCont = AMSEvent::gethead() -> getC("AMSBeta",i);
         if (pCont != NULL) { 
          integer nelem = pCont -> getnelem();
          if (nelem > 0) {
           AMSBeta* p = (AMSBeta*)AMSEvent::gethead() -> 
                                               getheadC("AMSBeta",i);
          while ( p != NULL) {
           betaH = new(contBetaH) AMSBetaD(p);
           rstatus = betaH -> set_pEventB(eventH);
           if (rstatus != oocSuccess) {
            cerr << "AMSEventList:AddBeta: Error - cannot set the "
                << "Beta to Event 'pEventB' association." << endl;
            return rstatus;
           }
           // set Beta <-> Track link
           AMSTrTrack* pTr = p -> getptrack();      // pointer to assoc. track
           if (pTr != NULL) {
            integer  posT   = pTr -> getContPos();  // position of Track
            if (dbg_prtout == 1) 
                       cout <<" set track/beta for track pos "<<posT<<endl;
            if (posT != 0) {
             // find track
             char pred[32];
             (void) sprintf(pred,"_Position=%d",posT);
             eventH -> pTrack(trTrackItr,mode,oocAll,pred);
             while (trTrackItr.next()) {                   // get position of  
              rstatus = betaH -> set_pTrackBeta(trTrackItr);
              if (rstatus != oocSuccess) {
               cerr << "AMSEventList:AddBeta: Error - cannot set the "
                    << "Beta to Track 'pTrackBeta' association." << endl;
               return rstatus;
              }
             }
            }
           }

          // set Beta <-> TOF Cluster[] link
           for (j=0; j<4; j++) {
            AMSTOFCluster* pTOF = p -> getpcluster(j);   // TOFCl.
            if (pTOF != NULL) {
             if(dbg_prtout) cout<<"AddBeta -I- pTOF != NULL, layer "<<j<<endl;
             integer  posT   = pTOF -> getContPos();     // position of TOFCl.
             integer  ntof   = pTOF -> getntof();
             if (posT != 0) {
              // find TOF Cluster
              char pred[32];
              (void) sprintf(pred,"_Position=%d && _ntof=%d",posT,ntof);
              eventH -> pTOFCluster(tofClusterItr,mode,oocAll,pred);
              while (tofClusterItr.next()) {               // get position of  
               rstatus = betaH -> add_pTOFBeta(tofClusterItr);
               if (rstatus != oocSuccess) {
                cerr << "AMSEventList:AddBeta: Error - cannot set the "
                     << "Beta to TOFCluster 'pTOFBeta' association." << endl;
                return rstatus;
               }
              }
             }
           } else {
            if(dbg_prtout) cout<<"AddBeta -I- pTOF == NULL, layer "<<j<<endl;
           }
           }
           betas++;
           p -> setContPos(betas);
           betaH -> setPosition(betas);
           listH  -> incNBetas();
           eventH -> incBetas();
           p = p -> next();
          }
         } else { // nelem < 1
          if (dbg_prtout == 1) 
           cout <<"AMSEventList::AddBeta-I- number of elements of Container "
                <<i<<" equal to "<<nelem<<endl;
         }        
       } else {
        cout <<"AMSEventList::AddBeta-I- pCont == NULL of container "<<i<<endl;
       }
      }
        return rstatus;
}

ooStatus      AMSEventList::CopyBeta(ooHandle(AMSEventD) & eventH, ooMode mode)

{
	ooStatus	        rstatus = oocSuccess;	// Return status
        ooHandle(AMSEventList)  listH = ooThis();
        integer                 ibeta = 0;
        integer                 patternB;

        if (eventH == NULL) {
          cerr <<"Error - eventH  == NULL "<<endl;
         return oocError;
        }

        eventH -> pBeta(betaItr, mode);
        while (betaItr.next()) {
         if (dbread_only != 0) {
          AMSBeta* p = new AMSBeta();
          betaItr -> copy(p);
          integer posCont = betaItr -> getPosition();
          p -> setContPos(posCont);
          patternB = betaItr -> getpattern();
          AMSEvent::gethead() -> addnext(AMSID("AMSBeta",patternB),p);
         }
         ibeta++;
        }
         if (dbg_prtout == 1) cout <<"AMSEventList:: CopyBeta : betas "<<ibeta
                              <<" total size "<< sizeof(AMSBetaD)*ibeta<<endl;
        event_size = event_size + sizeof(AMSBetaD)*ibeta;

        if (dbread_only != 0) {
         if (ibeta > 0) {
           for (int ib=0; ib<npatb; ib++) {
             AMSContainer* pCont = AMSEvent::gethead() -> 
                                                     getC("AMSBeta",ib);
             integer nelem = pCont -> getnelem();
             if (nelem > 0) {
              // set Beta <-> Track association
              // get head of Beta Container
              AMSBeta* p = (AMSBeta*)AMSEvent::gethead() 
                                                   -> getheadC("AMSBeta",ib);
              while (p != NULL) {
               integer posB = p -> getContPos();
               if (posB != 0) {
                char pred[32];
                (void) sprintf(pred,"_Position=%d",posB);
                eventH -> pBeta(betaItr,mode,oocAll,pred);     // find the same
                if (betaItr.next() != NULL) {                  // beta in db
                 trackH = betaItr -> pTrackBeta();
                 integer posT = trackH -> getPosition();
                 integer patternT = trackH -> getPattern();
                 AMSTrTrack* pT = (AMSTrTrack*)AMSEvent::gethead() ->
                                           getheadC("AMSTrTrack",patternT);
                 while (pT != NULL) {
                  if (posT != pT -> getContPos()) {
                   pT = pT -> next();
                  } else {
                   break;
                  }
                 }
                 if (pT != NULL) {
                  p -> setTrackP(pT);
                  if (dbg_prtout == 1) cout << 
                     "AMSEventList:: CopyBeta -I- find an "
                                                   <<" associated track"<<endl;
                  } else {
                   cout <<"AMSEventList:: CopyBeta - W - cannot find an "
                            <<" associated track"<<endl;
                 }
                }
               }
               p = p -> next();
              }   
         // set Beta <-> TOFCluster[] association
         p = (AMSBeta*)AMSEvent::gethead() -> getheadC("AMSBeta",ib);
         while (p != NULL) {
          integer posB = p -> getContPos();
          if (posB != 0) {
           char pred[32];
           (void) sprintf(pred,"_Position=%d",posB);
           eventH -> pBeta(betaItr,mode,oocAll,pred);      // find the same
           if (betaItr.next() != NULL) {                   // beta in db
            betaItr -> pTOFBeta(tofClusterItr, mode);
            while (tofClusterItr.next()) {
              integer posC = tofClusterItr -> getPosition();
              integer ntof = tofClusterItr -> getntof();
              AMSTOFCluster* pC = (AMSTOFCluster*)AMSEvent::gethead() ->
                                    getheadC("AMSTOFCluster",ntof-1);
              while (pC != NULL) {
               if (posC != pC -> getContPos()){
                  pC = pC -> next();
                } else {
                  break;
               }
              }
              if (pC != NULL) {
               p -> setTOFClP(pC, ntof);
               if (dbg_prtout == 1) cout 
                  <<"AMSEventList:: CopyBeta -I- find an "
                            <<" associated TOF cluster"<<endl;
              } else {
                cout <<"AMSEventList:: CopyBeta - W - cannot find an "
                     <<" associated TOF cluster"<<endl;
              }               
            }
           }
          }
          p = p -> next();
         }
         } // nelem > 0
           } // for cycle 
         }   // ibeta > 0
        } // dbread_only
        return rstatus;
}

ooStatus      AMSEventList::AddCharge(ooHandle(AMSEventD) & eventH)

{
	ooStatus	        rstatus = oocSuccess;	// Return status
        ooHandle(AMSEventList)  listH = ooThis();
        ooMode                  mode  = oocUpdate;

        if (eventH == NULL) {
          cerr <<"Error - eventH  == NULL "<<endl;
         return oocError;
        }

        if (contChargeH == NULL) {
         rstatus = CheckContainer(chargeCont, oocUpdate, contChargeH);
         if (rstatus != oocSuccess) return rstatus;
        }

        // get number of charges so far
        integer charges  = listH   -> getNCharges();
        if (dbg_prtout == 1)
        cout <<"found charges in Charge "<<charges<<endl;
        // get first charge
        AMSCharge* p = (AMSCharge*)AMSEvent::gethead() -> 
                                               getheadC("AMSCharge",0);
        if (p == NULL) {
        cout <<"AMSEventList::AddCharge-I- AMSCharge* p == NULL"<<endl;
        return oocSuccess;
        }
        while ( p != NULL) {
          chargeH = new(contChargeH) AMSChargeD(p);
          //rstatus = chargeH -> set_pEventCh(eventH);
          rstatus = eventH -> add_pChargeE(chargeH);
          if (rstatus != oocSuccess) {
          cerr << "AMSEventList:AddCharge: Error - cannot set the "
               << "Charge to Event 'pEventCh' association." << endl;
          return rstatus;
          }
         
         // set Beta <-> Charge link
           AMSBeta* pB = p -> getpbeta();          // pointer to assoc. beta
           if (pB != NULL) {
            integer  posB   = pB -> getContPos();  // position of Beta
            if (posB != 0) {
             // find Beta
             char pred[32];
             (void) sprintf(pred,"_Position=%d",posB);
             eventH -> pBeta(betaItr,oocUpdate,oocAll,pred);
             while (betaItr.next()) {                   // get position of  
              integer posBd = betaItr -> getPosition(); // assoc. beta in db
               rstatus = chargeH -> set_pBetaCh(betaItr);
               if (rstatus != oocSuccess) {
                cerr << "AMSEventList:AddCharge: Error - cannot set the "
                 << "Charge to Beta 'pChargeB' association." << endl;
                return rstatus;
               }
             }
            }
          charges++;
          p -> setContPos(charges);
          chargeH -> setPosition(charges);
          listH  -> incNCharges();
          eventH -> incCharges();
         }
          p = p -> next();
       }
        return rstatus;
}

ooStatus      AMSEventList::
                   CopyCharge(ooHandle(AMSEventD) & eventH, ooMode mode)

{
	ooStatus	        rstatus = oocSuccess;	// Return status
        ooHandle(AMSEventList)  listH = ooThis();

        if (eventH == NULL) {
          cerr <<"Error - eventH  == NULL "<<endl;
         return oocError;
        }

        if (contChargeH == NULL) {
         rstatus = CheckContainer(chargeCont, mode, contChargeH);
         if (rstatus != oocSuccess) return rstatus;
        }

        //chargeH = eventH -> pChargeE();
        eventH -> pChargeE(chargeItr, mode);
        while (chargeItr.next()) {
         if (dbread_only != 0) {
          AMSCharge* p = new AMSCharge();
          chargeItr -> copy(p);
          integer posCont = chargeItr -> getPosition();
          p -> setContPos(posCont);
          AMSEvent::gethead() -> addnext(AMSID("AMSCharge",0),p);
          if (dbg_prtout == 1) 
          cout <<"AMSEventList::CopyCharge : addnext done" <<endl;
          // set Charge <-> Beta association
          // get head of Charge Container
          AMSContainer* pCont = AMSEvent::gethead() -> getC("AMSCharge",0);
          integer nelem = pCont -> getnelem();
          if (nelem > 0) {
            AMSCharge* p = (AMSCharge*)AMSEvent::gethead() -> 
                                                  getheadC("AMSCharge",0);
           while (p != NULL) {
            integer posCh = p -> getContPos();
            if (posCh != 0) {
             betaH = chargeItr -> pBetaCh();                  // in dbase
             integer posB     = betaH -> getPosition();
             integer patternB = betaH -> getpattern();
             AMSBeta* pB = (AMSBeta*)AMSEvent::gethead() ->
                                                  getheadC("AMSBeta",patternB);
             while (pB != NULL) {
              if (posB != pB -> getContPos()) {
               pB = pB -> next();
               } else {
                break;
               }
             }
              if (pB != NULL) {
               p -> setBetaP(pB);
               if (dbg_prtout == 1) 
                 cout <<"AMSEventList:: CopyCharge -I- find an "
                    <<" associated Beta"<<endl;
              } else {
               cout <<"AMSEventList:: CopyCharge - W - cannot find an "
                            <<" associated beta"<<endl;
              }
            }
           p = p -> next();
           }
          }
         }
         event_size = event_size + sizeof(AMSChargeD);
        }
        return rstatus;
}        

ooStatus      AMSEventList::AddParticle(ooHandle(AMSEventD) & eventH)

{
	ooStatus	        rstatus = oocSuccess;	// Return status
        ooHandle(AMSEventList)  listH = ooThis();

        if (eventH == NULL) {
         cerr <<"Error - eventH  == NULL "<<endl;
         return oocError;}

        if (contParticleH == NULL) {
         rstatus = CheckContainer(particleCont, oocUpdate, contParticleH);
         if (rstatus != oocSuccess) return rstatus; }

        // get number of particle so far
        integer particles  = listH   -> getNParticles();
        if (dbg_prtout == 1)
        cout <<"found particles in Container "<<particles<<endl;
        // get first charge
        AMSParticle* p = (AMSParticle*)AMSEvent::gethead() -> 
                                               getheadC("AMSParticle",0);
        if (p == NULL) {
        cout <<"AMSEventList::AddParticle-I- AMSParticle* p == NULL"<<endl;
        return oocSuccess;
        }

         particleH = new(contParticleH) AMSParticleD(p);
         //rstatus = particleH -> set_pEventP(eventH);
         rstatus = eventH -> add_pParticleE(particleH);
         if (rstatus != oocSuccess) {
          cerr << "AMSEventList:AddParticle: Error - cannot set the "
               << "Particle to Event 'pEventP' association." << endl;
          return rstatus;
         }
         // set Beta <-> Particle link
           AMSBeta* pB = p -> getpbeta();          // pointer to assoc. beta
           if (pB != NULL) {
            integer  posB   = pB -> getContPos();  // position of Beta
            if (posB != 0) {
             // find Beta
             char pred[32];
             (void) sprintf(pred,"_Position=%d",posB);
             eventH -> pBeta(betaItr,oocUpdate,oocAll,pred);
             while (betaItr.next()) {                   // get position of  
               rstatus = particleH -> set_pBetaP(betaItr);
               if (rstatus != oocSuccess) {
                cerr << "AMSEventList:AddParticle: Error - cannot set the "
                 << "Particle to Beta 'pChargeP' association." << endl;
                return rstatus;
               }
              // set Particle <-> Charge link
              chargeH   = betaItr -> pChargeB();
              if (chargeH != NULL) {
               rstatus = particleH -> set_pChargeP(chargeH);
               if (rstatus != oocSuccess) return rstatus;
              } else {
               cout << "AMSEventList:AddParticle: Error - cannot find an "
                    <<" assoc. charge "<<endl;
              }
             // set Particle <-> Track link
             trackH  = betaItr -> pTrackBeta();
             if (trackH != NULL) {
              rstatus = particleH -> set_pTrackP(trackH);
              if (rstatus != oocSuccess) return rstatus;
             } else {
              cout << "AMSEventList:AddParticle: Error - cannot find an "
                    <<" assoc. track "<<endl;
              }
             }
            }
           }
            // set particle -> ctc link
            for (int icnt=0; icnt<CTCDBc::getnlay(); icnt++) {
             AMSCTCCluster* pCTC = p -> getpctc(icnt);
               if (pCTC != NULL) {
                integer posCTC = pCTC -> getContPos();
                if (posCTC != 0) {
                // find CTC
                char pred[32];
                (void) sprintf(pred,"_Position=%d",posCTC);
                eventH -> pCTCCluster(ctcClusterItr,oocUpdate,oocAll,pred);
                while (ctcClusterItr.next()) {          // get position of  
                  rstatus = particleH -> add_pCTCClusterP(ctcClusterItr);
                  if (rstatus != oocSuccess) {
                   cerr << "AMSEventList:AddParticle-E- cannot set the "
                        << "Particle to CTC 'pChargeP' association." << endl;
                   return rstatus;
                  }
                  break;
                }
                }
               }
            }
          particles++;
          p -> setContPos(particles);
          particleH -> setPosition(particles);
          listH  -> incNParticles();
          eventH -> incParticles();

        return rstatus;
}           

ooStatus      AMSEventList::
                   CopyParticle(ooHandle(AMSEventD) & eventH, ooMode mode)

{
	ooStatus	        rstatus = oocSuccess;	// Return status
        ooHandle(AMSEventList)  listH = ooThis();
        integer                 iparticle = 0;

        if (eventH == NULL) {
          cerr <<"Error - eventH  == NULL "<<endl;
         return oocError;
        }

        if (contParticleH == NULL) {
         rstatus = CheckContainer(particleCont, mode, contParticleH);
         if (rstatus != oocSuccess) return rstatus;
        }

        if (contChargeH == NULL) {
         rstatus = CheckContainer(chargeCont, mode, contChargeH);
         if (rstatus != oocSuccess) return rstatus;
        }

        if (contBetaH == NULL) {
         rstatus = CheckContainer(betaCont, mode, contBetaH);
         if (rstatus != oocSuccess) return rstatus;
        }

        if (contTrackH == NULL) {
         rstatus = CheckContainer(trtrackCont, mode, contTrackH);
         if (rstatus != oocSuccess) return rstatus;
        }

        eventH -> pParticleE(particleItr, mode);
        while (particleItr.next()) {
         if (dbread_only != 0) {
          AMSParticle* p = new AMSParticle();
          particleItr -> copy(p);
          integer posCont = particleItr -> getPosition();
          p -> setContPos(posCont);
          AMSEvent::gethead() -> addnext(AMSID("AMSParticle",0),p);
          if (dbg_prtout == 1) 
           cout <<"AMSEventList::CopyParticle : addnext done" <<endl;
          // get head of Particle Container
          AMSContainer* pCont = AMSEvent::gethead() -> getC("AMSParticle",0);
          integer nelem = pCont -> getnelem();
          if (nelem > 0) {
           AMSParticle* p = (AMSParticle*)AMSEvent::gethead() -> 
                                                  getheadC("AMSParticle",0);
           while (p != NULL) {
            integer posP = p -> getContPos();
            if (posP != 0) {
             // set particle <-> beta link
             betaH = particleItr -> pBetaP();                  // in dbase
             integer posB      = betaH -> getPosition();
             integer patternB  = betaH -> getpattern();
             AMSBeta* pB = (AMSBeta*)AMSEvent::gethead() ->
                                                getheadC("AMSBeta",patternB);
             while (pB != NULL) {
              if (posP != pB -> getContPos()) {
               pB = pB -> next();
               } else {
                break;
               }
              }
              if (pB != NULL) {
               p -> setpbeta(pB);
               if (dbg_prtout == 1) 
                 cout <<"AMSEventList:: CopyParticle -I- found an "
                    <<" associated Beta"<<endl;
              } else {
               cout <<"AMSEventList:: CopyParticle - W - cannot find an "
                            <<" associated beta"<<endl;
              }
             // set particle <-> charge link
              chargeH = particleItr -> pChargeP();
              integer posC = chargeH -> getPosition();
              AMSCharge* pC = (AMSCharge*)AMSEvent::gethead() ->
                                                      getheadC("AMSCharge",0);
              while (pC != NULL) {
                if (posC != pC -> getContPos()) {
                  pC = pC -> next();
                } else {
                  break;
                }
              }
              if (pC != NULL) {
                p -> setpcharge(pC);
               if (dbg_prtout == 1) 
               cout <<"AMSEventList:: CopyParticle -I- found an "
                    <<" associated Charge"<<endl;
              } else {
               cout <<"AMSEventList:: CopyParticle - W - cannot find an "
                            <<" associated charge"<<endl;
              }
              // set particle <-> track link
              trackH = particleItr -> pTrackP();
              integer posT = trackH -> getPosition();
              integer patternT = trackH -> getPattern();
              AMSTrTrack* pT = (AMSTrTrack*)AMSEvent::gethead() ->
                                              getheadC("AMSTrTrack",patternT);
              while (pT != NULL) {
               if (posT != pT -> getContPos()) {
                 pT = pT -> next();
               } else {
                 break;
               }
              }
              if (pT != NULL) {
               p -> setptrack(pT);
               if (dbg_prtout == 1) cout <<"AMSEventList::CopyParticle -I- "
                                         <<"found an associated track"<<endl;
               } else {
                cout <<"AMSEventList::CopyParticle - W - cannot find an "
                            <<" associated track"<<endl;
              }
            }

            particleItr -> pCTCClusterP(ctcClusterItr, mode);
            while (ctcClusterItr.next() != NULL) {
              int posCTC = ctcClusterItr -> getPosition();
              for (int ictc=0; ictc<CTCDBc::getnlay(); ictc++) {
               AMSContainer* pCont = AMSEvent::gethead() 
                                              -> getC("AMSCTCCluster",ictc);
               if (pCont-> getnelem() > 0) {
                AMSCTCCluster* pCTC = (AMSCTCCluster*) AMSEvent::gethead() ->
                                               getheadC("AMSCTCCluster",ictc);
                while (pCTC != NULL) {
                  if (posCTC == pCTC -> getContPos()) {
                    p -> setpctc(pCTC,ictc);
                    break;
                  }
                 pCTC = pCTC -> next();
                }
               }
              }
            }
           p = p -> next();
           } // p != NULL
          }  // nelem > 0
         }   // dbread_only
        event_size = event_size + sizeof(AMSParticleD);
        }
        return rstatus;
}


ooStatus AMSEventList::AddTOFMCCluster(ooHandle(AMSEventD) & eventH)
{
	ooStatus	        rstatus = oocSuccess;	// Return status
        ooHandle(AMSEventList)  listH = ooThis();

        if (eventH == NULL) {
          cerr <<"Error - eventH  == NULL "<<endl;
         return oocError;
        }


        if (contTOFMCClusterH == NULL) {
         rstatus = CheckContainer
                             (tofmcclusterCont, oocUpdate, contTOFMCClusterH);
         if (rstatus != oocSuccess) return rstatus;
        }

        // get number of MCClusters so far
        integer tofmcclusters  = listH ->   getNTOFMCClusters();
        if (dbg_prtout == 1)
        cout <<"found TOFMCClusters  "<<tofmcclusters<<endl;
        // get first cluster
        AMSTOFMCCluster* p = (AMSTOFMCCluster*)AMSEvent::gethead() -> 
                                            getheadC("AMSTOFMCCluster",0);
        if (p == NULL) {
        cout <<
        "AMSEventList::AddTOFMCCluster-I- AMSTOFMCCluster* p == NULL"<<endl;
        return oocSuccess;
        }
        while ( p != NULL) {
         TOFMCClusterH = new(contTOFMCClusterH) AMSTOFMCClusterD(p);
         tofmcclusters++;
         p -> setContPos(tofmcclusters);
         TOFMCClusterH -> setPosition(tofmcclusters);
         listH -> incNTOFMCClusters();
         rstatus = eventH -> add_pTOFMCCluster(TOFMCClusterH);
         //---rstatus = TOFMCClusterH -> set_pEventTMC(eventH);
         if (rstatus != oocSuccess) {
          cerr << "AMSEventList:AddTOFMCCluster: Error - cannot set the ";
          cerr << "MCCluster to Event 'pEventTMC' association." << endl;
          return rstatus;
         }
         p = p -> next();
       }
        return rstatus;
}

ooStatus AMSEventList::
               CopyTOFMCCluster(ooHandle(AMSEventD) & eventH, ooMode mode)

{
        ooHandle(AMSEventList) listH = ooThis();
	ooStatus	        rstatus = oocSuccess;	// Return status

        if (eventH == NULL) {
         cerr <<"Error - eventH  == NULL "<<endl;
         return oocError;
        }

        if (contTOFMCClusterH == NULL) {
         rstatus = CheckContainer
                              (tofmcclusterCont, mode, contTOFMCClusterH);
         if (rstatus != oocSuccess) return rstatus;
        }

         eventH -> pTOFMCCluster(TOFMCClusterItr, mode);
         integer imcs=0;

         while (TOFMCClusterItr.next()) {
          if (dbread_only != 0) {
           AMSTOFMCCluster* p = new AMSTOFMCCluster();
           TOFMCClusterItr -> copy(p);
           integer    pos = TOFMCClusterItr -> getPosition();
           p -> setContPos(pos);
           AMSEvent::gethead() -> addnext(AMSID("AMSTOFMCCluster",0),p);
          }
          imcs++;
         }
         if (dbg_prtout == 1) 
          cout <<"AMSEventList::CopyTOFMCCluster:  clusters "<<imcs
                <<" total size "<< sizeof(AMSTOFMCClusterD)*imcs<<endl;
         event_size = event_size + sizeof(AMSTOFMCClusterD)*imcs;
        return rstatus;
}

ooStatus AMSEventList::CheckAllContainers(ooMode mode)
{
        ooStatus  rstatus = oocSuccess;
        ooHandle(AMSEventList) listH = ooThis();
        integer   i;

        for (i=0; i<2; i++) {
         rstatus = CheckContainer
                               (trclusterCont[i], mode, contClusterH[i]);
         if (rstatus != oocSuccess) return rstatus;
        }

        for (i=0; i<6; i++) {
         rstatus = CheckContainer(trlayerCont[i], mode, contTrLayerH[i]);
         if (rstatus != oocSuccess) return rstatus;
        }

        rstatus = CheckContainer(trtrackCont, mode, contTrackH);
        if (rstatus != oocSuccess) return rstatus;

        rstatus = CheckContainer(trmcclusterCont, mode, contMCClusterH);
        if (rstatus != oocSuccess) return rstatus;

        for (i=0; i<4; i++) {
         rstatus = CheckContainer(sclayerCont[i], mode, contLayerSH[i]);
         if (rstatus != oocSuccess) return rstatus;
        }

        rstatus = CheckContainer(particleCont, mode, contParticleH);
        if (rstatus != oocSuccess) return rstatus;

        rstatus = CheckContainer(chargeCont, mode, contChargeH);
        if (rstatus != oocSuccess) return rstatus;

        rstatus = CheckContainer(betaCont, mode, contBetaH);
        if (rstatus != oocSuccess) return rstatus;

        rstatus = CheckContainer(tofmcclusterCont, mode, contTOFMCClusterH);
        if (rstatus != oocSuccess) return rstatus;

        return rstatus;
}

ooStatus AMSEventList::CheckContainer(char* name, ooMode mode, 
                                      ooHandle(ooContObj)& containerH)
{
     ooStatus rstatus = oocSuccess;
     ooHandle(ooDBObj)   databaseH;
     ooThis().containedIn(databaseH);

     if (name) {
      if (dbg_prtout == 1)
       cout << "AMSEventList:: CheckContainer : container "<<name
            << ", mode "<<mode<<endl;
      if (!containerH.exist(databaseH,name,mode)) {
       cout <<"AMSEventList::CheckContainer -W- Cannot find or open"
            <<"  container "<<name<<" in mode "<<mode<<endl;
       rstatus = oocError;
      }
     } else {
       cout <<"AMSEventList::CheckContainer -W- container name is NULL"<<endl;
       rstatus = oocError;
     }
      return rstatus;
}

ooStatus AMSEventList::CheckContainer(char* name, ooMode mode, 
                                      ooHandle(ooContObj)& containerH, 
                                      ooHandle(ooDBObj)& dbH)
{
     ooStatus rstatus = oocSuccess;

     if (name) {
      if (dbg_prtout == 1)
       cout << "AMSEventList:: CheckContainer : container "<<name
            << ", mode "<<mode<<endl;
      if (!containerH.exist(dbH,name,mode)) {
       cout <<"AMSEventList::CheckContainer -W- Cannot find or open"
            <<"  container "<<name<<" in mode "<<mode<<endl;
       rstatus = oocError;
      }
     } else {
       cout <<"AMSEventList::CheckContainer -W- container name is NULL"<<endl;
       rstatus = oocError;
     }
      return rstatus;
}


ooStatus AMSEventList::
                    CopyEvent(ooHandle(AMSEventD)& eventH, ooMode mode)
{
  ooStatus  rstatus = oocError;
  ooHandle(AMSEventList) listH = ooThis();

   if (eventH == NULL) {
    cout <<"AMSEventList::CopyEvent -E- eventH is NULL"<<endl;
     return rstatus;
  }

   rstatus = listH -> CopyTrCluster(eventH, mode);
   if(rstatus != oocSuccess) goto error;
 
   rstatus = listH -> CopyTrRecHit(eventH, mode);
   if(rstatus != oocSuccess) goto error;

   if (dbread_only != 0) {
    rstatus = listH -> LinkHitClusterM(eventH);
    if(rstatus != oocSuccess) goto error;
   } 

   rstatus = listH -> CopyTrTrack(eventH, mode);
   if(rstatus != oocSuccess) goto error;

   if (dbread_only != 0) {
    rstatus = listH -> LinkTrackHitM(eventH);
    if(rstatus != oocSuccess) goto error;
   }

   rstatus = listH -> CopyTOFCluster(eventH, mode);
   if(rstatus != oocSuccess) goto error;

   rstatus = listH -> CopyCTCCluster(eventH, mode);
   if(rstatus != oocSuccess) goto error;

   rstatus = listH -> CopyBeta(eventH, mode);
   if(rstatus != oocSuccess) goto error;

   rstatus = listH -> CopyCharge(eventH, mode);
   if(rstatus != oocSuccess) goto error;

   rstatus = listH -> CopyParticle(eventH, mode);
   if(rstatus != oocSuccess) goto error;

   event_size=event_size+sizeof(AMSEventD);
   //cout <<"CopyEvent : Event size "<<event_size<<endl;

error :
   if(rstatus != oocSuccess) cout <<"AMSEventList::CopyEvent -E- Error"<<endl;

   return rstatus;

}
ooStatus AMSEventList::
            CopyMCEvent(ooHandle(AMSEventD)& eventH, ooMode mode)
{
  ooStatus  rstatus = oocError;
  ooHandle(AMSEventList) listH = ooThis();

  if(eventH == NULL) {
   cout <<"AMSEventList::CopyMCEvent -E- eventH is NULL"<<endl;
    return rstatus;
  }

  //copy MC banks
   rstatus =  listH -> CopyTrMCCluster(eventH, mode);
  if (rstatus == oocSuccess) rstatus = 
                             listH -> CopyTOFMCCluster(eventH, mode);
  if (rstatus == oocSuccess) rstatus = 
                             listH -> CopyAntiMCCluster(eventH, mode);
  if (rstatus == oocSuccess) rstatus = 
                             listH -> CopyCTCMCCluster(eventH, mode);

  if (dbg_prtout !=0 && rstatus == oocSuccess) 
                          cout<<"CopyMCEvent : Event size "<<event_size<<endl;
   if (rstatus != oocSuccess) 
                          cout<<"CopyMCEvent : Fail to copy MC banks"<<endl;
   return rstatus;
}

ooStatus AMSEventList::
                CopyEventHeader(ooHandle(AMSEventD)& eventH, ooMode mode)
{
  ooStatus  rstatus = oocError;
  ooHandle(AMSEventList) listH = ooThis();

  if(eventH == NULL) {
   cout <<"AMSEventList::CopyEventHeader -E- eventH is NULL"<<endl;
    return rstatus;
  }

  //copy event header
  integer  run, runtype;
  uinteger eventn;
  time_t   time;

  eventH -> getNumbers(run, runtype, eventn, time);
  if (dbread_only != 0) {
  
   if(dbg_prtout == 1) cout <<" AMSEventList::CopyEventHeader -I- add event "
                            <<eventn<<" of run "<<run<<endl;

   AMSEvent::sethead((AMSEvent*)AMSJob::gethead()->add(
                      new AMSEvent(AMSID("Event",eventn),run,runtype,time)));
  }

   return rstatus;
}

ooStatus AMSEventList::
                CopyMCeventg(ooHandle(AMSEventD)& eventH, ooMode mode)
{
  ooStatus  rstatus = oocError;
  ooHandle(AMSEventList) listH = ooThis();

  if(eventH == NULL) {
   cout <<"AMSEventList::CopyMCEventg -E- eventH is NULL"<<endl;
    return rstatus;
  }


  //copy MC banks
  rstatus = listH -> CopymcEventg(eventH, mode);
  if (dbg_prtout !=0 && rstatus == oocSuccess) 
                          cout<<"CopyMCEvent : Event size "<<event_size<<endl;

   return rstatus;
}


ooStatus AMSEventList::AddGeometry(ooHandle(ooDBObj)& dbH)
{
  ooStatus  rstatus = oocError;
  ooMode    mode    = oocUpdate;
  ooItr(AMSgvolumeD)       geometryItrT;           // and iterator
  AMSgvolume *             cur;
  integer                  i=0;
  integer                  pos;

  AMSgvolume * pg = AMSJob::gethead() -> getgeom();
  if (pg == NULL) return rstatus;
 
  ooHandle(ooDBObj)    databaseH;

  ooThis().containedIn(databaseH);


  // get setup and container names
  char* setup = AMSJob::gethead() -> getsetup();
  if(dbg_prtout) cout <<"AMSEventList::AddGeometry -I- setup "<<setup<<endl;
  char* contName = new char[strlen(setup)+10];
  strcpy(contName,"Geometry_");
  strcat(contName,setup);
  
  // check container
  rstatus = CheckContainer(contName, mode, contGeometryH, dbH);
  if (rstatus != oocSuccess) 
    {
      contGeometryH = new(contName,1,0,0,dbH) ooContObj;
      cout<<"AMSEventList::AddGeometry -I- Create container "<<contName<< endl;
    } else {
      cout<<"AMSEventList::AddGeometry -I- Found container "<<contName<< endl;
      geometryItr.scan(contGeometryH, mode);
      if (geometryItr.next()) {
        cout <<"AMSEventList::AddGeometry -W- container "<<contName
             <<" is not empty. Quit"<<endl;
        return oocSuccess;
      }
    }

  integer   id;
  char*     name;

  ooHandle(AMSgvolumeD) *geometryHT;

// save the whole tree first
  integer nobj =  0;     // number of geometry objects
  for (;;)
  {
    cur =  (AMSgvolume*)AMSgObj::GVolMap.getid(nobj);
    if (cur != NULL) nobj++;
    if (cur == NULL) break;
  }

  cout<<"AMSEventList::AddGeometry - I - found "<<nobj<<" geometry objects"
      <<endl;

  geometryHT = new ooHandle(AMSgvolumeD)[nobj];

  for (;;)
  {
    cur =  (AMSgvolume*)AMSgObj::GVolMap.getid(i);
    if (cur != NULL) {
     id = cur -> getid();
     name = cur -> getname();
     i++;
     cur -> setContPos(i);
     geometryH = new(contGeometryH) AMSgvolumeD(id, cur, name, i);
     geometryHT[i-1] = geometryH;
  } else {
      break;
   }
  }

// and now set pointers
  i=0;

  geometryItr.scan(contGeometryH, mode);

  while (geometryItr.next())
  {
    cur =  (AMSgvolume*)AMSgObj::GVolMap.getid(i);
    if (cur != NULL) {
     if (cur -> next() != NULL) {
       pos = cur -> next() -> getContPos(); geometryItr -> setNext(pos); 
       
     }
     if (cur -> prev() != NULL) {
      pos = cur -> prev() -> getContPos(); geometryItr -> setPrev(pos);
     }
     if (cur -> up()   != NULL) {
      pos = cur -> up()   -> getContPos(); geometryItr -> setUp(pos);
     }
     if (cur -> down() != NULL) {
      pos = cur -> down() -> getContPos(); geometryItr -> setDown(pos);
     }
     i++;    
    } 
  }

  geometryItr.scan(contGeometryH, mode);
  while (geometryItr.next())
  {
   pos = geometryItr -> getNext();
   if (pos > 0) geometryItr -> _Next = geometryHT[pos-1];
  
   pos = geometryItr -> getPrev();
   if (pos > 0) geometryItr -> _Prev = geometryHT[pos-1];

   pos = geometryItr -> getUp();
   if (pos > 0) geometryItr -> _Up = geometryHT[pos-1];

   pos = geometryItr -> getDown();
   if (pos > 0) geometryItr -> _Down = geometryHT[pos-1];

  }

  return oocSuccess;
}

ooStatus AMSEventList::CopyGeometry(ooMode mode, ooHandle(ooDBObj)& dbH)
{
  ooStatus                 rstatus = oocError;

  AMSgvolume *             cur;
  AMSgvolume *             ptr;
 
  cout <<"AMSEventList::CopyGeometry - let's copy geometry"<<endl;
 
  NN = 0;

  // get setup name
  char* setup = AMSJob::gethead() -> getsetup();
  cout <<"AMSEventList::CopyGeometry -I- setup name "<<setup<<endl;

  char* contName = new char[strlen(setup)+10];
  strcpy(contName,"Geometry_");
  strcat(contName,setup);

  cout <<"AMSEventList::CopyGeometry -I- container "<<contName<<endl;

  rstatus = CheckContainer(contName, mode, contGeometryH, dbH);
  if (rstatus != oocSuccess) {
   cout <<"AMSEventList::CopyGeometry -E- cannot find or open container "
        <<contName<<endl;
   exit (1);
  }

  integer nobj = 0;
  geometryItr.scan(contGeometryH, mode);
  while (geometryItr.next())
  {
    nobj++;
  }
  
  NOBJ = nobj;
  cout <<"AMSEventList::CopyGeometry -I- found "<<NOBJ<<" objects"<<endl;


  geometryHT = new ooHandle(AMSgvolumeD)[nobj+1];
  geometryP  = new integer[nobj+1];
  ptrsGV     = new AMSgvolume*[nobj+1];
  
  // find mother (or virtual top)
  geometryItr.scan(contGeometryH, mode);
  while (geometryItr.next())
  {
    if (geometryItr -> getUp() == 0  && geometryItr -> _Up == NULL) 
    {
        cout <<"mother  "<<geometryItr -> getid()<<" "
             <<geometryItr -> _name<<endl;
        //cout <<"position "<<geometryItr -> getContPos()<<endl;
        if (geometryItr -> _Next != 0) {
         geometryItr -> _Next = NULL;
         geometryItr -> setNext(0); 
         cout 
          <<"AMSEventList::CopyGeometry -I- Next ptr of mother set to 0"<<endl;
        }
        CopyByPos(geometryItr, mode);
        break;
    }
  }

  //integer     j;
  AMSNode*    p;
  AMSgvolume* mother;

   for (integer j = 0; j<NN; j++)
  {
   AMSgvolume* p;
   AMSPoint                 coo[1];
   geant                    par[6];
   geant                    cooG[3];
   number                   nbuff0[9];
   //number                   nrm[3][3];
   number                   inrm[3][3];
   char                     gonly[5];
   char                     shape[5];

   integer id     = geometryHT[j] -> getid();
   integer matter = geometryHT[j] -> getmatter();
   integer rotmno = geometryHT[j] -> getrotmno();
   char*   name   = geometryHT[j] -> getname();
                    geometryHT[j] -> getshape(shape);
                    geometryHT[j] -> getpar(par);

                    geometryHT[j] -> getcoo(coo);
                    cooG[0] = coo[0] [0];
                    cooG[1] = coo[0] [1];
                    cooG[2] = coo[0] [2];

                    geometryHT[j] -> getgonly(gonly);
   integer posp   = geometryHT[j] -> getposp();
   integer gid    = geometryHT[j] -> getgid();
   integer npar   = geometryHT[j] -> getnpar();
   //geometryHT[j] -> getnrm(nbuff0);
   //UCOPY(nbuff0,nrm,sizeof(number)*9/4);
                    geometryHT[j] -> getinrm(nbuff0);
                    UCOPY(nbuff0,inrm,sizeof(number)*9/4);

   integer rel    = 0;
   p = new AMSgvolume (matter, rotmno, name, shape, par, npar, 
                       cooG, inrm, gonly, posp, gid);
   integer pos = geometryHT[j] -> getContPos();
   p -> setContPos(pos);

   ptrsGV[j] = p;

   if (dbg_prtout > 1) cout <<"# "<<j<<"  "<<id<<" "<<name<<" "<<pos;

   if( geometryHT[j] -> getUp() != 0) 
    {
     integer posUp = geometryHT[j] -> _Up -> getContPos();
     for (integer l=0; l<j;l++)
      { 
       if (geometryP[l] == posUp) 
        {
         AMSgvolume* motherT = ptrsGV[l];
         motherT -> add(p);
         break;
        } 
      } 
    } else { 
    if (dbg_prtout) cout <<" virtual top"<<endl; 
    AMSJob::gethead() -> addup(p);
    mother = p;
    }
  }

  cout <<"AMSEventList::CopyGeometry -I- Copy done "<<endl;
  GGCLOS();
  AMSgObj::GVolMap.map(*mother);

  return oocSuccess;
}

void AMSEventList::CopyByPos(ooHandle(AMSgvolumeD)& ptr, ooMode mode)
{
  ooHandle(AMSgvolumeD) cur;
  ooHandle(AMSgvolumeD) tmp;
  //static integer nnn = 0;

  //if (ptr != NULL ) cout <<"ptr -> Pos "<< ptr -> getContPos()<<endl;
  //cout <<"in "<<++nnn<<endl;
  cur = ptr;
  while(cur != NULL) {
    geometryHT[NN] = cur;
    geometryP[NN]  = cur -> getContPos();
    NN++;
    //cout <<"NN... "<<NN<<" contPos "<<cur -> getContPos();
    //cout<<"down "<<cur-> _Down<<", next "<<cur-> _Next<<endl;
    tmp = cur -> _Down;
    if (tmp != NULL) CopyByPos(tmp, mode);
    cur = cur -> _Next;
  }   
  // cout <<"exit "<<nnn--<<endl;
}

ooStatus AMSEventList::AddMaterial(ooHandle(ooDBObj)& dbH)
{
  ooStatus  rstatus = oocError;
  ooMode    mode    = oocUpdate;
  AMSNode*  p;
  AMSgmat*  pp;
  integer   id;
  geant     a[3];
  geant     z[3];
  geant     w[3];
  char*     name;


  // get setup and container names
  char* setup = AMSJob::gethead() -> getsetup();
  cout <<"setup "<<setup<<endl;
  char* contName = new char[strlen(setup)+11];
  strcpy(contName,"Materials_");
  strcat(contName,setup);

  // check container
  rstatus = CheckContainer(contName, mode, contgmatH, dbH);
  if (rstatus != oocSuccess) 
    {
      contgmatH = new(contName,1,0,0,dbH) ooContObj;
      cout << "AMSEventList:: -I- Create container "<<contName<< endl;
    } else {
      cout << "AMSEventList:: -I- Found container "<<contName<< endl;
      p = AMSJob::gethead()->getnodep(AMSID("Materials:",0));
      if (p != NULL) {
       p = p -> down();
       integer nobj = 0;
       gmatItr.scan(contgmatH, mode);
       while (gmatItr.next()) {
        if (p == NULL) {
         cout<<"AMSEventList::AddMaterials -E- comparison failed"<<endl;
         cout<<"AMSEventList::AddMaterials -E- AMSgmat == NULL"<<endl;
         return oocError;
        }
        pp = (AMSgmat*)p;
        id = p -> getid();
        rstatus = gmatItr -> CmpMaterials(id, pp);
        if (rstatus != oocSuccess) {
         cout<<"AMSEventList::AddMaterials -E- comparison failed"<<endl;
         cout<<"AMSEventList::AddMaterials -E- please write new setup"<<endl;
         return oocError;
        }
        p = p -> next();
        nobj++;
       }
       if (nobj == 0) {
         cout<<"AMSEventList::AddMaterials -W- container is empty"<<endl;
         cout<<"AMSEventList::AddMaterials -I- write new Materials objects"
             <<endl;
         goto newsetup;
       }
      } else {
      cout <<"AddMaterial - E - cannot find the virtual top of gmat"<<endl; 
      return oocError;
      }
      return oocSuccess;
    }

newsetup:
 // get pointer to the top

  p = AMSJob::gethead()->getnodep(AMSID("Materials:",0));
  if (p != NULL) {
   p = p -> down();
   while (p != NULL) {
     if (dbg_prtout > 1) cout<<p -> getid() <<" "<<p -> getname();
     pp = (AMSgmat*)p;
     integer npar = pp -> getnpar();
     id = p -> getid();
     name = p -> getname();
     pp -> getNumbers(a, z, w);
     gmatH = new(contgmatH) AMSgmatD(id, pp, a, z, w, name);
     p = p -> next();
   }
  } else {
    cout <<"AddMaterial - E - cannot find the virtual top of gmat"<<endl; 
    return oocError;
  }
 
  return oocSuccess;
}

ooStatus AMSEventList::AddTMedia(ooHandle(ooDBObj)& dbH)
{
  ooStatus  rstatus = oocError;
  ooMode    mode    = oocUpdate;
  integer   id;
  char*     name;
  AMSNode*  p;
  AMSgtmed* pp;

  // get setup and container names
  char* setup = AMSJob::gethead() -> getsetup();
  if(dbg_prtout) cout <<"AMSEventList::AddTMedia -I-cout setup "<<setup<<endl;
  char* contName = new char[strlen(setup)+8];
  strcpy(contName,"TMedia_");
  strcat(contName,setup);
  
  // check container
  rstatus = CheckContainer(contName, mode, contgtmedH, dbH);
  if (rstatus != oocSuccess) 
    {
      contgtmedH = new(contName,1,0,0,dbH) ooContObj;
      cout << "AMSEventList:: -I- Create container "<<contName<< endl;
    } else {
      cout << "AMSEventList:: -I- Found container "<<contName<< endl;
      p = AMSJob::gethead()->getnodep(AMSID("TrackingMedia:",0));
      if (p != NULL) {
      p = p -> down();
      integer nobj = 0;
      gtmedItr.scan(contgtmedH, mode);
      while (gtmedItr.next()) {
       if (p == NULL) {
        cout<<"AMSEventList::AddTMedia -E- comparison failed"<<endl;
        cout<<"AMSEventList::AddTMedia -E- AMSgtmed == NULL"<<endl;
        return oocError;
       }
       pp = (AMSgtmed*)p;
       id = p -> getid();
       rstatus = gtmedItr -> CmpTrMedia(id, pp);
       if (rstatus != oocSuccess) {
        cout<<"AMSEventList::AddTrMedia -E- comparison failed"<<endl;
        cout<<"AMSEventList::AddTrMedia -E- please write new setup"<<endl;
        return oocError;
       }
       p = p -> next();
       nobj++;
      }
      if (nobj == 0) {
       cout <<"AddTMedia - W - container is empty"<<endl; 
       cout <<"AddTMedia - I - write new TMedia objects "<<endl;
       goto newsetup;
      }
     } else {
      cout <<"AddTMedia - E - cannot find the virtual top of gtmed"<<endl; 
      return oocError;
     }
     return oocSuccess;
    }

newsetup:
  // get pointer to the top
  p = AMSJob::gethead()->getnodep(AMSID("TrackingMedia:",0));
  if (p != NULL) {
   p = p -> down();
   while (p != NULL) {
    cout<<p -> getid() <<" "<<p -> getname();
     pp = (AMSgtmed*)p;
     id = p -> getid();
     name = p -> getname();
     gtmedH = new(contgtmedH) AMSgtmedD(id, pp, name);

     p = p -> next();
   }
  } else {
    cout <<"AddTMedia - W - cannot find the virtual top of gtmed"<<endl; 
    return oocSuccess;
  }
 
  return oocSuccess;
}

ooStatus AMSEventList::CopyMaterial(ooMode mode, ooHandle(ooDBObj)& dbH)
{
  ooStatus  rstatus = oocError;

  // get setup name
  char* setup = AMSJob::gethead() -> getsetup();
  if(dbg_prtout) cout <<"AMSEventList::CopyMaterial -I- setup "<<setup<<endl;

  char* contName = new char[strlen(setup)+11];

  strcpy(contName,"Materials_");
  strcat(contName,setup);

  if(dbg_prtout) cout <<"AMSEventList::CopyMaterial -I-  container "
                      <<contName<<endl;

  rstatus = CheckContainer(contName, mode, contgmatH, dbH);
  if (rstatus != oocSuccess) {
   cout <<"AMSEventList::CopyMaterial -E-  cannot find or open container "
                      <<contName<<endl;
   exit(1);
  }

  integer              id, imate;
  geant                rho, radl, absl;
  geant                a[3];
  geant                z[3];
  geant                w[3];
  integer              npar;

  static AMSgmat mat;

  mat.setname("Materials:");
  AMSJob::gethead() -> addup(&mat);

  gmatItr.scan(contgmatH, mode);
  while (gmatItr.next())
   {
     npar = gmatItr -> getnpar();
     if (npar > 3) {
       cout <<"AMSEventList::CopyMaterial - W - npar gt 3. QUIT "<<endl;
       return rstatus;
     }
     gmatItr -> getNumbers(id, imate, rho, radl, absl, a, z, w);
     char* name = gmatItr -> getname();

     cout <<" Add "<<name<<" with id "<<id<<" imate "<<imate<<endl;

     if (npar > 1) mat.add (new AMSgmat(imate, name, 
                                        a, z, w, npar, 
                                        rho, radl, absl));
     if (npar < 2) mat.add (new AMSgmat(imate, name, a[0], z[0], 
                                        rho, radl, absl));
   }
  cout <<" AMSEventList::CopyMaterial - I - Materials added "<<endl;
  return rstatus;
}

ooStatus AMSEventList::CopyTMedia(ooMode mode, ooHandle(ooDBObj)& dbH)
{
  ooStatus  rstatus = oocError;

  cout <<" AMSEventList::CopyTMedia - I - add TMedia "<<endl;
  char* setup = AMSJob::gethead() -> getsetup();
  if(dbg_prtout) cout <<"AMSEventList::CopyTMedia -I- setup "<<setup<<endl;

  char* contName = new char[strlen(setup)+8];
  strcpy(contName,"TMedia_");
  strcat(contName,setup);

  if(dbg_prtout) 
        cout <<"AMSEventList::CopyTMedia -I- container "<<contName<<endl;

  rstatus = CheckContainer(contName, mode, contgtmedH, dbH);
  if (rstatus != oocSuccess) {
   cout <<"AMSEventList::CopyTMedia -E- cannot find or open container "
                      <<contName<<endl;
   exit(1);
  }

  integer              ibuff[4];
  geant                gbuff[10], birks[3];
  char                 yb;

  static AMSgtmed  tmed;
  tmed.setname("TrackingMedia:");
  AMSJob::gethead() -> addup(&tmed);

  gtmedItr.scan(contgtmedH, mode);
  while (gtmedItr.next())
   {
     gtmedItr -> getNumbers(ibuff, gbuff);
     yb = gtmedItr -> getyb();
     char* name = gtmedItr -> getname();

     cout <<" Add "<<name<<endl;
     for (integer i=0; i<3; i++) {birks[i] = gbuff[7+i];}
     tmed.add (new AMSgtmed(ibuff[1], name, ibuff[2], ibuff[0], yb, 
               birks, ibuff[3], gbuff[0], gbuff[1], gbuff[2],
               gbuff[3], gbuff[4], gbuff[5]));
   }
  AMSgObj::GTrMedMap.map(tmed);
  cout <<" AMSEventList::CopyTMedia - I - TMedia added "<<endl;
  return rstatus;
}

ooStatus      AMSEventList::AddCTCCluster(ooHandle(AMSEventD)& eventH) 
{
	ooStatus	         rstatus = oocSuccess;	// Return status
        ooHandle(AMSCTCClusterD) ctcclusterH;
        ooHandle(AMSEventList)   listH = ooThis();

        if (eventH == NULL) {
          cerr <<"Error - eventH  == NULL "<<endl;
         return oocError;
        }

        if (contCTCClusterH == NULL) {
         rstatus = CheckContainer(ctcclusterCont, oocUpdate, contCTCClusterH);
         if (rstatus != oocSuccess) return rstatus;
        }

        integer   ctccl  = listH -> getNCTCClusters();
        if (dbg_prtout == 1)
        cout <<"AMSEventList::AddCTCCluster -I- found CTC clusters for "
             <<ctccl<<endl;
        
        for (int icnt=0; icnt<CTCDBc::getnlay(); icnt++) {
         AMSCTCCluster * p = (AMSCTCCluster*)AMSEvent::gethead() ->
                                               getheadC("AMSCTCCluster",icnt);
         while ( p != NULL) {
          ctcclusterH = new(contCTCClusterH) AMSCTCClusterD(p);
          rstatus = eventH -> add_pCTCCluster(ctcclusterH);
          if (rstatus != oocSuccess) {
           cerr << "AMSEventList:AddCTCCluster: Error - cannot set the "
                << "CTCCluster to Event 'pCTCCluster' association." << endl;
           return rstatus;
          }
          ctccl++;
          eventH -> incCTCClusters();
          p -> setContPos(ctccl);
          ctcclusterH -> setPosition(ctccl);
          listH  -> incNCTCClusters();
          p = p -> next();
         }
        }
        return rstatus;
}

ooStatus      AMSEventList::
                    CopyCTCCluster(ooHandle(AMSEventD)& eventH, ooMode mode) 

{
	ooStatus	         rstatus = oocSuccess;	// Return status
        ooHandle(AMSCTCClusterD) ctcclusterH;
        integer                  ictccl = 0;
        ooHandle(AMSEventList)  listH = ooThis();


        if (eventH == NULL) {
          cerr <<"Error - eventH  == NULL "<<endl;
         return oocError;
        }

         if (contCTCClusterH == NULL) {
          rstatus = CheckContainer(ctcclusterCont, mode, contCTCClusterH);
          if (rstatus != oocSuccess) return rstatus;
         }


        eventH -> pCTCCluster(ctcClusterItr, mode);

        while (ctcClusterItr.next()) {
         if (dbread_only != 0) {
          AMSCTCCluster* p = new AMSCTCCluster();
          ctcClusterItr -> copy(p);
          integer posCont = ctcClusterItr -> getPosition();
          p -> setContPos(posCont);
          int l = ctcClusterItr -> getLayer();
          AMSEvent::gethead()->addnext(AMSID("AMSCTCCluster",l-1),p);
         }
         ictccl++;
        }
         if (dbg_prtout == 1) {
          cout <<"AMSEventList::CopyCTCCluster: clusters "<<ictccl
             <<" total size "<<sizeof(AMSCTCClusterD)*ictccl<<endl;
         }
        event_size = event_size + sizeof(AMSCTCClusterD)*ictccl;
        return rstatus;
}

ooStatus AMSEventList::AddCTCMCCluster(ooHandle(AMSEventD) & eventH)
{
	ooStatus	        rstatus = oocSuccess;	// Return status
        ooHandle(AMSEventList)  listH = ooThis();


        if (eventH == NULL) {
          cerr <<"Error - eventH  == NULL "<<endl;
         return oocError;
        }

        if (contCTCMCClusterH == NULL) {
        rstatus = CheckContainer(
                              ctcmcclusterCont, oocUpdate, contCTCMCClusterH);
        if (rstatus != oocSuccess) return rstatus;
        }

        // get number of MCClusters so far
        integer ctcmcclusters  = listH ->   getNCTCMCClusters();
        if (dbg_prtout == 1)
         cout <<"AMSEventList::AddCTCMCCluster-I-found CTCMCClusters  "
              <<ctcmcclusters<<endl;
        // get first cluster
         AMSCTCMCCluster* p = (AMSCTCMCCluster*)AMSEvent::gethead() -> 
                                            getheadC("AMSCTCMCCluster",0);
        if (p == NULL && dbg_prtout) {
        cout <<
         "AMSEventList::AddCTCMCCluster-I- AMSCTCMCCluster* p == NULL"<<endl;
         return oocSuccess;
        }
        while ( p != NULL) {
         CTCMCClusterH = new(contCTCMCClusterH) AMSCTCMCClusterD(p);
         ctcmcclusters++;
         p -> setContPos(ctcmcclusters);
         CTCMCClusterH -> setPosition(ctcmcclusters);
         listH -> incNCTCMCClusters();
         rstatus = eventH -> add_pCTCMCCluster(CTCMCClusterH);
         if (rstatus != oocSuccess) {
          cerr << "AMSEventList:AddCTCMCCluster: Error - cannot set the ";
          cerr << "MCCluster to Event 'pEventTMC' association." << endl;
          return rstatus;
         }
         p = p -> next();
       }
        return rstatus;
}

ooStatus AMSEventList::
              CopyCTCMCCluster(ooHandle(AMSEventD) & eventH, ooMode mode)
{
	ooStatus	        rstatus = oocSuccess;	// Return status
        ooHandle(AMSEventList)  listH = ooThis();

        if (eventH == NULL) {
          cerr <<"Error - eventH  == NULL "<<endl;
         return oocError;
        }

        if (contCTCMCClusterH == NULL) {
         rstatus = CheckContainer(ctcmcclusterCont, mode, contCTCMCClusterH);
         if (rstatus != oocSuccess) return rstatus;
        }

         eventH -> pCTCMCCluster(CTCMCClusterItr, mode);
         integer imcs=0;

         while (CTCMCClusterItr.next()) {
          if (dbread_only != 0) {
           AMSCTCMCCluster* p = new AMSCTCMCCluster();
           CTCMCClusterItr -> copy(p);
           integer pos = CTCMCClusterItr -> getPosition();
           p -> setContPos(pos);
           AMSEvent::gethead() -> addnext(AMSID("AMSCTCMCCluster",0),p);
           imcs++;
          }
         }
         if (dbg_prtout == 1) 
          cout <<"AMSEventList::CopyCTCMCCluster:  clusters "<<imcs
                <<" total size "<< sizeof(AMSCTCMCClusterD)*imcs<<endl;
         event_size = event_size + sizeof(AMSCTCMCClusterD)*imcs;
        return rstatus;
}

ooStatus AMSEventList::AddAntiMCCluster(ooHandle(AMSEventD) & eventH)
{
	ooStatus	        rstatus = oocSuccess;	// Return status
        ooHandle(AMSEventList)  listH = ooThis();

        if (eventH == NULL) {
          cerr <<"Error - eventH  == NULL "<<endl;
         return oocError;
        }

        if (contAntiMCClusterH == NULL) {
         rstatus = CheckContainer(
                            antimcclusterCont, oocUpdate, contAntiMCClusterH);
         if (rstatus != oocSuccess) return rstatus;
        }

        // get number of MCClusters so far
        integer antimcclusters  = listH ->   getNAntiMCClusters();
        if (dbg_prtout == 1)
        cout <<"AMSEventList::AddAntiCluster: found AntiMCClusters  "
             <<antimcclusters<<endl;
        // get first cluster
        AMSAntiMCCluster* p = (AMSAntiMCCluster*)AMSEvent::gethead() ->
                                           getheadC("AMSAntiMCCluster",0);
        if (p == NULL && dbg_prtout == 1)  {
         cout <<
         "AMSEventList::AddAntiMCCluster-I- AMSAntiMCCluster* p == NULL"<<endl;
        return oocSuccess;
        }
        while ( p != NULL) {
         integer  idsoft;
         AntiMCClusterH = new(contAntiMCClusterH) AMSAntiMCClusterD(p);
         antimcclusters++;
         p -> setContPos(antimcclusters);
         AntiMCClusterH -> setPosition(antimcclusters);
         listH -> incNAntiMCClusters();
         rstatus = eventH -> add_pAntiMCCluster(AntiMCClusterH);
         if (rstatus != oocSuccess) {
          cerr << "AMSEventList:AddAntiMCCluster: Error - cannot set the "
               << "MCCluster to Event 'pEventTMC' association." << endl;
          return rstatus;
         }
         p = p -> next();
       }
        return rstatus;
}

ooStatus AMSEventList::
                CopyAntiMCCluster(ooHandle(AMSEventD) & eventH, ooMode mode)
{
	ooStatus	        rstatus = oocSuccess;	// Return status
        ooHandle(AMSEventList) listH = ooThis();

        if (eventH == NULL) {
          cerr <<"Error - eventH  == NULL "<<endl;
         return oocError;
        }

        if (contAntiMCClusterH == NULL) {
         rstatus = 
          CheckContainer(antimcclusterCont, mode, contAntiMCClusterH);
          if (rstatus != oocSuccess) return rstatus;
        }

         eventH -> pAntiMCCluster(AntiMCClusterItr, mode);
         integer imcs=0;
         while (AntiMCClusterItr.next()) {
          if (dbread_only != 0) {
           AMSAntiMCCluster* p = new AMSAntiMCCluster();
           AntiMCClusterItr -> copy(p);
           integer    pos = AntiMCClusterItr -> getPosition();
           p -> setContPos(pos);
           AMSEvent::gethead() -> addnext(AMSID("AMSAntiMCCluster",0),p);
          }
          imcs++;
         }
         if (dbg_prtout == 1) 
          cout <<"AMSEventList::CopyAntiMCCluster:  clusters "<<imcs
                <<" total size "<< sizeof(AMSAntiMCClusterD)*imcs<<endl;
         event_size = event_size + sizeof(AMSAntiMCClusterD)*imcs;
        return rstatus;
}

ooStatus      AMSEventList::AddmcEventg(ooHandle(AMSEventD)& eventH) 
{
	ooStatus	         rstatus = oocSuccess;	// Return status
        ooHandle(AMSEventList)   listH = ooThis();

        if (eventH == NULL) {
          cerr <<"Error - eventH  == NULL "<<endl;
         return oocError;
        }

        if (contmceventgH == NULL) {
         rstatus = CheckContainer(mceventgCont, oocUpdate, contmceventgH);
         if (rstatus != oocSuccess) return rstatus;
        }

        integer   mceventg  = listH -> getNmcEventg();
        if (dbg_prtout == 1)
        cout <<"AMSEventList::AddmcEventg -I- found mcEventg "
             <<mceventg<<endl;
        
        AMSmceventg * p = (AMSmceventg*)AMSEvent::gethead() ->
                                                 getheadC("AMSmceventg",0);
        if (p == NULL) {cout <<
              "AMSEventList::AddmcEventg-I- AMSmceventg* p == NULL"<<endl;
                return oocSuccess;
        }
        while ( p != NULL) {
         mceventgH = new(contmceventgH) AMSmceventgD(p);
         rstatus = eventH -> add_pmcEventg(mceventgH);
         if (rstatus != oocSuccess) {
          cerr << "AMSEventList:AddmcEventg: Error - cannot set the "
               << "mceventg to Event 'pmcEventg' association." << endl;
          return rstatus;
         }
         mceventg++;
         p -> setContPos(mceventg);
         mceventgH -> setPosition(mceventg);
         listH  -> incNmcEventg();
         p = p -> next();
        }
        return rstatus;
}
ooStatus  AMSEventList::
                 CopymcEventg(ooHandle(AMSEventD)& eventH, ooMode mode) 
{
	ooStatus	         rstatus = oocSuccess;	// Return status
        ooHandle(AMSEventList)   listH = ooThis();
        integer               imceventg = 0;

        if (eventH == NULL) {
          cerr <<"Error - eventH  == NULL "<<endl;
         return oocError;
        }

        if (contmceventgH == NULL) {
         rstatus = CheckContainer(mceventgCont, mode, contmceventgH);
         if (rstatus != oocSuccess) return rstatus;
        }


        eventH -> pmcEventg(mceventgItr, mode);

        while (mceventgItr.next()) {
          if (dbread_only != 0) {
           AMSmceventg* p = new AMSmceventg();
           mceventgItr -> copy(p);
           integer pos = mceventgItr -> getPosition();
           p->setContPos(pos);
           AMSEvent::gethead() -> addnext(AMSID("AMSmceventg",0),p);
           if (imceventg == 0) p -> InitSeed();
           p->run();
          }
          imceventg++;
         }
         if (dbg_prtout == 1) 
          cout <<"AMSEventList::CopymcEventg:  events "<<imceventg
                <<" total size "<< sizeof(AMSmceventgD)*imceventg<<endl;
         event_size = event_size + sizeof(AMSmceventgD)*imceventg;
        return rstatus;
}

ooStatus AMSEventList::Addamsdbc(ooHandle(ooDBObj)& dbH)
{
  ooStatus  rstatus = oocError;
  ooMode    mode    = oocUpdate;


  // get setup and container names
  char* setup = AMSJob::gethead() -> getsetup();
  cout <<"AMSEventList::Addamsdb -I- setup name "<<setup<<endl;
  char* contName = new char[strlen(setup)+8];
  strcpy(contName,"amsdbc_");
  strcat(contName,setup);

  // check container
  rstatus = CheckContainer(contName, mode, contH, dbH);
  if (rstatus != oocSuccess) 
    {
      contH = new(contName,1,0,0,dbH) ooContObj;
      cout << "AMSEventList:: -I- Create container "<<contName<< endl;
      amsdbcH  = new(contH) AMSDBcD();
      commonsH = new(contH) AMScommonsD();
      ctcdbcH  = new(contH) CTCDBcD();
      tofdbcH  = new(contH) TOFDBcD();
    } else {
      cout << "AMSEventList:: -I- Found container "<<contName<< endl;
      amsdbcItr.scan(contH, mode);
      if (amsdbcItr.next()) {
       cout <<"AMSEventList::Addamsdbc -I- check amsdbc"<<endl;
       rstatus = amsdbcItr -> CmpConstants();
       if (rstatus != oocSuccess) {
        cout <<"AMSEventList:: -E- Quit. amsdbc comparison failed"<<endl;
        cout <<"AMSEventList:: -I- please, write new setup."<<endl;
        return rstatus;
       }
      } else {
       amsdbcH  = new(contH) AMSDBcD();
      }

      integer Write = 1;
      commonsItr.scan(contH, mode);
      if (commonsItr.next()) {
       cout <<"AMSEventList::Addamsdbc -I- check commons"<<endl;
       rstatus = commonsItr -> CmpConstants(Write);
       if (rstatus != oocSuccess) {
        cout <<"AMSEventList:: -E- Quit. commons are different"<<endl;
        cout <<"AMSEventList:: -I- please, write new setup."<<endl;
        return rstatus;
       }
      } else {
       commonsH  = new(contH) AMScommonsD();
      }

      ctcdbcItr.scan(contH, mode);
      if (ctcdbcItr.next()) {
       cout <<"AMSEventList::Addamsdbc -I- check ctcdbc"<<endl;
       rstatus = ctcdbcItr -> CmpConstants();
       if (rstatus != oocSuccess) {
        cout <<"AMSEventList:: -E- Quit. ctcdbc comparison failed"<<endl;
        cout <<"AMSEventList:: -I- write new setup."<<endl;
        return rstatus;
       }
      }

      tofdbcItr.scan(contH, mode);
      if (tofdbcItr.next()) {
       cout <<"AMSEventList::Addamsdbc -I- check tofdbc"<<endl;
       rstatus = tofdbcItr -> CmpConstants();
       if (rstatus != oocSuccess) {
        cout <<"AMSEventList:: -E- Quit. tofdbc comparison failed"<<endl;
        cout <<"AMSEventList:: -I- write new setup."<<endl;
        return rstatus;
       }
      }
    }

  return oocSuccess;
}

ooStatus AMSEventList::DeleteEventList()
{
  ooStatus               rstatus = oocSuccess;
  ooHandle(AMSEventList) listH = ooThis();
  ooMode                 mode = oocUpdate;

  rstatus = DeleteAllContainers();
  return rstatus;
}

ooStatus AMSEventList::DeleteAllContainers()
{
        ooStatus               rstatus = oocSuccess;
        ooHandle(AMSEventList) listH = ooThis();
        integer                i;
        ooMode                 mode = oocUpdate;

        for (i=0; i<2; i++) {
         rstatus = CheckContainer(trclusterCont[i], mode, contClusterH[i]);
         if (rstatus == oocSuccess) ooDelete(contClusterH[i]);
        }

        for (i=0; i<6; i++) {
         rstatus = CheckContainer(trlayerCont[i], mode, contTrLayerH[i]);
         if (rstatus == oocSuccess) ooDelete(contTrLayerH[i]);
        }

        rstatus = CheckContainer(trtrackCont, mode, contTrackH);
        if (rstatus == oocSuccess)  ooDelete(contTrackH);

        rstatus = CheckContainer(trmcclusterCont, mode, contMCClusterH);
        if (rstatus == oocSuccess) ooDelete(contMCClusterH);

        for (i=0; i<4; i++) {
         rstatus = CheckContainer(sclayerCont[i], mode, contLayerSH[i]);
         if (rstatus == oocSuccess) ooDelete(contLayerSH[i]);
        }

        rstatus = CheckContainer(particleCont, mode, contParticleH);
        if (rstatus == oocSuccess) ooDelete(contParticleH);

        rstatus = CheckContainer(chargeCont, mode, contChargeH);
        if (rstatus == oocSuccess) ooDelete(contChargeH);

        rstatus = CheckContainer(betaCont, mode, contBetaH);
        if (rstatus == oocSuccess) ooDelete(contBetaH);

        rstatus = CheckContainer(tofmcclusterCont, mode, contTOFMCClusterH);
        if (rstatus == oocSuccess) ooDelete(contTOFMCClusterH);

        rstatus = 
             CheckContainer(antimcclusterCont, oocUpdate, contAntiMCClusterH);
        if (rstatus == oocSuccess) ooDelete(contAntiMCClusterH);

        rstatus = CheckContainer(ctcmcclusterCont, mode, contCTCMCClusterH);
        if (rstatus == oocSuccess) ooDelete(contCTCMCClusterH);

        rstatus = CheckContainer(ctcclusterCont, mode, contCTCClusterH);
        if (rstatus == oocSuccess) ooDelete(contCTCClusterH);

        rstatus = CheckContainer(mceventgCont, oocUpdate, contmceventgH);
        if (rstatus == oocSuccess) ooDelete(contmceventgH);

        return oocSuccess;
}

ooStatus AMSEventList::PrintListStatistics(char* printMode)
{
     ooStatus               rstatus = oocSuccess;
     ooItr(AMSEventD)       eventItr;                 
     ooHandle(AMSEventList) listH = ooThis();

     cout<<"List :  "<<_listName<<", list type "<<_listType
         <<", event type "<<_eventType<<endl;
     cout<<"with setup... "<<_Setup <<" has "<<_nEvents<<" events"<<endl;

     if (strcmp(printMode, "F") == 0 || strcmp(printMode, "f")== 0 ) {
      if (!listH.isUpdated()) {
       rstatus = eventItr.scan(listH,oocRead);
       if (rstatus == oocSuccess) {
        integer run     = 0;
        integer nevents = 0;
        while(eventItr.next()) {
         integer r = eventItr -> getrun();
         if (r == run) {
          nevents++;
         } else {
          if (run != 0) cout<<"run "<<run<<", events "<<nevents<<endl;
          run = r;
          nevents = 1; 
         }
        }
         cout<<"run "<<run<<", events "<<nevents<<endl;
       } else {
         cout<<"AMSEventList::PrintListStatistics -E- scan failed "<<endl;
       }
      } else {
        cout <<"AMSEventList::PrintListStatistics -I- cannot provide more "
             <<", list is opened in Update mode by another process"<<endl; 
      }      
     }
   return rstatus;
}

ooStatus AMSEventList::PrintListStatistics_mc(char* printMode)
{
     ooStatus               rstatus = oocSuccess;
     ooItr(AMSmceventD)     mceventItr;                 
     ooHandle(AMSEventList) listH = ooThis();

     cout<<"List :  "<<_listName<<", list type "<<_listType
         <<", event type "<<_eventType<<endl;
     cout<<"with setup... "<<_Setup <<" has "<<_nEvents<<" events"<<endl;

     if (strcmp(printMode, "F") == 0 || strcmp(printMode, "f")== 0 ) {
      if (!listH.isUpdated()) {
       rstatus = mceventItr.scan(listH,oocRead);
       if (rstatus == oocSuccess) {
        integer run     = 0;
        integer nevents = 0;
        while(mceventItr.next()) {
         integer r = mceventItr -> getrun();
         if (r == run) {
          nevents++;
         } else {
          if (run != 0) cout<<"run "<<run<<", events "<<nevents<<endl;
          run = r;
          nevents = 1; 
         }
        }
         cout<<"run "<<run<<", events "<<nevents<<endl;
       } else {
         cout<<"AMSEventList::PrintListStatistics -E- scan failed "<<endl;
       }
      } else {
        cout <<"AMSEventList::PrintListStatistics -I- cannot provide more "
             <<", list is opened in Update mode by another process"<<endl; 
      }      
     }
   return rstatus;
}

ooBoolean AMSEventList::CheckListSstring(char* sstring) 
{
  if (sstring) {
   if (strstr(_listName,sstring) != NULL) {
     return oocTrue;
   } 
  }
  return oocFalse;
}

ooStatus      AMSEventList::AddMCEvent
                 (integer _run, uinteger eventNumber, integer eventW)
{
  // _run         - run number
  // eventNumber  - event number
  // eventW       - match to AMSFFKEY.Write datacard
  //
  	 ooStatus	       rstatus = oocError;	// Return status
         ooHandle(AMSmceventD) eventH;
         char                  err_mess[80];
         integer               i;
         time_t                time;

        ooHandle(AMSEventList) listH = ooThis();

        if (AMSEvent::gethead() == NULL) return rstatus;

        time     = AMSEvent::gethead() -> gettime();

        // Add Event Header

        integer nevts   = listH -> getNEvents();
        integer nevtsP  = listH -> getNEventsP();

        if (dbg_prtout == 1) cout <<"found events in "<<nevts<<endl;

        eventH = new(listH) AMSmceventD(_run, eventNumber, time);
        listH -> incNEvents();
        listH -> incNEventsP();
        nevtsP++;
        eventH -> setPosition(nevtsP);

        AMSmceventg * p = (AMSmceventg*)AMSEvent::gethead() ->
                                                 getheadC("AMSmceventg",0);
        if (p == NULL) {cout <<
              "AMSEventList::AddmcEventg-I- AMSmceventg* p == NULL"<<endl;
                return oocSuccess;
        }

        if ( p != NULL) {
          eventH -> add(p);
           rstatus = oocSuccess;
        }

        return rstatus;
}

ooStatus AMSEventList::FindEvent(integer runNumber, uinteger eventNumber, 
                       ooMode mode, ooHandle(AMSmceventD)& eventH)
{
     ooStatus rstatus = oocError;
     ooItr(AMSmceventD)   eventItr;

     ooHandle(AMSEventList) listH = ooThis();

     ooLookupKey               lookupKey(ooTypeN(AMSmceventD),2);

     ooEqualLookupField        
        runELookupField(ooTypeN(AMSmceventD), "_runNumber", &runNumber);
     ooGreaterThanLookupField  
        runGLookupField(ooTypeN(AMSmceventD), "_runNumber", &runNumber);

     ooEqualLookupField        
      eventELookupField(ooTypeN(AMSmceventD), "_eventNumber", &eventNumber);
     ooGreaterThanLookupField  
      eventGLookupField(ooTypeN(AMSmceventD), "_eventNumber", &eventNumber);

     if (runNumber > 0) { lookupKey.addField(runELookupField); }
     if (runNumber < 1) { lookupKey.addField(runGLookupField); }

     if (eventNumber > 0) { lookupKey.addField(eventELookupField); }
     if (eventNumber < 1) { lookupKey.addField(eventGLookupField); }

     ooStatus status = eventItr.scan(listH,lookupKey);
     if (status == oocSuccess) {
       while (eventItr.next()) {
        eventH = eventItr;
        if (dbg_prtout) cout 
             << "AMSEventList::FindEvent: found the event with run # "
             << runNumber<<" and event # "<<eventNumber<<endl;
        rstatus = oocSuccess;
        return rstatus;
       }
     } 

     return rstatus;
}

ooStatus      AMSEventList::AddDummyMCEvent
                 (integer _run, uinteger eventNumber, integer eventW)
{
  	 ooStatus	       rstatus = oocError;	// Return status
         ooHandle(AMSmceventD) eventH;
         char                  err_mess[80];
         integer               i;
         time_t                time;

        ooHandle(AMSEventList) listH = ooThis();


        // Add Event Header

        integer nevts   = listH -> getNEvents();
        integer nevtsP  = listH -> getNEventsP();

        if (dbg_prtout == 1) cout <<"found events in "<<nevts<<endl;

        eventH = new(listH) AMSmceventD(_run, eventNumber, time);
        listH -> incNEvents();
        listH -> incNEventsP();
        nevtsP++;
        eventH -> setPosition(nevtsP);
        // add event ID to the event map

        return oocSuccess;
}

ooStatus AMSEventList::CopyMCEventD(ooHandle(AMSmceventD)& eventH, ooMode mode)
{
  ooStatus  rstatus = oocError;
  ooHandle(AMSEventList) listH = ooThis();

  if(eventH == NULL) {
   cout <<"AMSEventList::CopyMCEventD -E- eventH is NULL"<<endl;
    return rstatus;
  }

  //copy event header
  integer run    = eventH -> getrun();
  integer eventn = eventH -> getevent();
  time_t  time   = eventH -> gettime();
  integer runtype = 0;

  if (dbread_only != 0) {
   if(dbg_prtout == 1) cout <<" AMSEventList::CopyMCEventD -I- copy event "
                            <<eventn<<" of run "<<run<<endl;
   AMSEvent::sethead((AMSEvent*)AMSJob::gethead()->add(
                      new AMSEvent(AMSID("Event",eventn),run,runtype,time)));
  }
  //copy mceventg part
        integer   pos;
        if (dbread_only != 0) {
         AMSmceventg* p = new AMSmceventg();
         eventH -> copy(p);
         pos = eventH -> getPosition();
         p -> setContPos(pos);
         AMSEvent::gethead() -> addnext(AMSID("AMSmceventg",0),p);
         p -> InitSeed();
         p->run();
       }

       event_size = event_size + sizeof(AMSmceventD);

       return oocSuccess;
}
