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
// Apr  08, 1997. ak. one directional assoc. event/beta, event/track
//                    merge copytrtrack and linktrackhitm
//
// !!!                still default constructor for AMSTrRecHit
//                                                  AMSBeta
//                                                  AMSCharge
//                                                  AMSParticle
//                                                  AMSmceventg
// May   , 1997. ak. move MC part to mclist.C, setup to dbA_Setup.C
//                   leave only methods for reconstructed objects
//
// Jun 01, 1997. ak. add anticlusters
// July 01, 1997. ah. USED flag used to write only USED hits etc
//
// last edit Jun 01, 1997, ak.
//
//
#include <iostream.h>
#include <string.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>

#include <db_comm.h>
#include <rd45.h>

#include <rclist_ref.h>
#include <rclist.h>

#include <anticlusterV_ref.h>
#include <chargeD_ref.h>
#include <ctcrecD_ref.h>
#include <recevent_ref.h>
#include <particleD_ref.h>
#include <tbeta_ref.h>
#include <tcluster_ref.h>
#include <tofrecD_ref.h>
#include <trrechit_ref.h>
#include <ttrack_ref.h>

#include <anticlusterV.h>
#include <chargeD.h>
#include <ctcrecD.h>
#include <recevent.h>
#include <particleD.h>
#include <tbeta.h>
#include <tcluster.h>
#include <tofrecD.h>
#include <trrechit.h>
#include <ttrack.h>

#include <antirec.h>
#include <trrec.h>
#include <tofrec.h>
#include <beta.h>
#include <charge.h>
#include <particle.h>
#include <mccluster.h>

// container  names
  char       nameTrL[6][9] = {
        "TrLayer1","TrLayer2","TrLayer3","TrLayer4","TrLayer5","TrLayer6"};
  char       nameC[2][11] = {"TrClusterX","TrClusterY"};
  char       nameS[4][9] = {"ScLayer1","ScLayer2","ScLayer3","ScLayer4"};

  static char* anticlusterCont;
  static char* betaCont;
  static char* chargeCont;
  static char* ctcclusterCont;
  static char* particleCont;
  static char* sclayerCont[4];
  static char* trclusterCont[2];
  static char* trlayerCont[6];
  static char* trtrackCont;
  static integer recevent_size;

//
static integer NO_AMS = 0;
//
// Containers
ooHandle(ooContObj)  contAntiClusterH;        // betas  
ooHandle(ooContObj)  contBetaH;               // betas  
ooHandle(ooContObj)  contChargeH;             // charges  
ooHandle(ooContObj)  contClusterH[2];         // tracker cluster 
ooHandle(ooContObj)  contCTCClusterH;         // CTC cluster 
ooHandle(ooContObj)  contLayerSH[4];          // TOF
ooHandle(ooContObj)  contParticleH;           // Particle
ooHandle(ooContObj)  contTrackH;              // tracks 
ooHandle(ooContObj)  contTrLayerH[6];         // tracker layers

// ooObj classes

ooHandle(AMSAntiClusterV)  anticlusterH;
ooHandle(AMSBetaD)         betaH;
ooHandle(AMSChargeD)       chargeH;
ooHandle(AMSTrClusterD)    clusterH;
ooHandle(AMSParticleD)     particleH;
ooHandle(AMSTrTrackD)      trackH;
ooHandle(AMSTrRecHitD)     trRecHitH;             


// Iterators
ooItr(AMSBetaD)        betaItr;                 // beta
ooItr(AMSChargeD)      chargeItr;               // charge
ooItr(AMSCTCClusterD)  ctcClusterItr;           // ctc cluster
ooItr(AMSParticleD)    particleItr;             // particle
ooItr(AMSTOFClusterD)  tofClusterItr;           // tof cluster
ooItr(AMSTrClusterD)   trClusterItr;            // cluster iterator
ooItr(AMSTrRecHitD)    trRecHitItr;             // hit iterator
ooItr(AMSTrTrackD)     trTrackItr;              // track iterator



AMSEventList::AMSEventList(char* listname, char* prefix)
{
  setlistname(listname);
  resetCounters();
  resetRCCounters();
  SetContainersNames(prefix);
}

AMSEventList::AMSEventList(char* listname, char* setup, char* prefix)
{
  setlistname(listname);
  setsetupname(setup);
  resetCounters();
  resetRCCounters();
  SetContainersNames(prefix);
}

void AMSEventList::resetRCCounters()
{
  int i;

  for (i=0; i<2; i++)         _nClusters[i]    = 0;
  for (i=0; i<6; i++)         _nHits[i]        = 0;
  for (i=0; i<4; i++)         _nTOFClusters[i] = 0;
  _nAntiClusters = 0;
  _nBetas    = 0;
  _nCharges  = 0;
  _nParticles= 0;
  _nTracks   = 0;
}

void AMSEventList::SetContainersNames(char* prefix)
{
  char*   name;
  integer i;

//AntiClusters
   if (!anticlusterCont)  anticlusterCont =  StrCat("AntiCluster_",prefix);

//TrLayer
   char       nameTrL[6][10] = {
     "TrLayer1_","TrLayer2_","TrLayer3_","TrLayer4_","TrLayer5_","TrLayer6_"};
   for (i=0; i<6; i++) {
     if (!trlayerCont[i]) trlayerCont[i] = StrCat(nameTrL[i],prefix);
   }

//TrCluster
   if (!trclusterCont[0]) trclusterCont[0] =  StrCat("TrClusterX_",prefix);
   if (!trclusterCont[1]) trclusterCont[1] =  StrCat("TrClusterY_",prefix);

//ScLayer
   char  nameSc[4][10] = {"ScLayer1_","ScLayer2_","ScLayer3_","ScLayer4_"};
   for (i=0; i<4; i++) {
    if (!sclayerCont[i]) sclayerCont[i] =  StrCat(nameSc[i],prefix);
   }

//CTCCluster
   if (!ctcclusterCont)  ctcclusterCont =  StrCat("CTCCluster_",prefix);

//TrTracks
   if (!trtrackCont)     trtrackCont =  StrCat("TrTracks_",prefix);

//Beta
   if (!betaCont)        betaCont = StrCat("Beta_",prefix);

//Charge
   if (!chargeCont)      chargeCont =  StrCat("Charge_",prefix);

//Particle
   if (!particleCont)    particleCont =  StrCat("Particle_",prefix);
} 

ooStatus      AMSEventList::AddTrCluster(
                               const integer N, ooHandle(AMSeventD) & eventH)

{
	ooStatus	        rstatus = oocError;	// Return status
        ooHandle(AMSEventList)  listH = ooThis();

        if (eventH == NULL) {
         Error("AddTrCluster: eventH  == NULL ");
         return rstatus;
        }

        if (contClusterH[N] == NULL) {
         rstatus= CheckContainer(trclusterCont[N], oocUpdate, contClusterH[N]);
         if (rstatus != oocSuccess) return rstatus;
        }

        // get number of clusters so far
        integer tclusters  = listH -> getNClusters(N);
        if (dbg_prtout == 1)
         cout <<"found clusters in "<<nameC[N]<<" "<<tclusters<<endl;

        rstatus = oocSuccess;
        if (NO_AMS == 1) return rstatus;
        // get first cluster
        AMSTrCluster* p = (AMSTrCluster*)AMSEvent::gethead() -> 
                                                   getheadC("AMSTrCluster",N);
        if (p == NULL && dbg_prtout == 1) {
         Message("AMSEventList::AddTrCluster-I- AMSTrCluster* p == NULL");
         return oocSuccess;
        }

        if (dbg_prtout) cout <<"AddTrCluster -I- nelements "
                   <<AMSEvent::gethead()->getC("AMSTrCluster",N)->getnelem()
                   <<" for AMSTrCluster Cont "<<N<<endl;
        while ( p ) {
         if(p->checkstatus(AMSDBc::USED))
        {
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
         rstatus = eventH -> add_pCluster(clusterH);
         if (rstatus != oocSuccess) {
          Error
            ( "AddTrCluster - cannot set Cluster to Event 'pCluster' assoc");
          return rstatus;
         }
        tclusters++;
        eventH -> incTrClusters();
        p -> setContPos(tclusters);
        clusterH -> setPosition(tclusters);
        listH -> incNClusters(N);
       }//end of USED test
        p = p -> next();
       }
        return rstatus;
}

ooStatus      AMSEventList::
                AddTrRecHit(const integer N, ooHandle(AMSeventD)& eventH) 
{
	ooStatus	       rstatus = oocSuccess;	// Return status
        ooHandle(AMSTrRecHitD) layerH;
        ooHandle(AMSEventList) listH = ooThis();

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
        rstatus = oocSuccess;
        if (NO_AMS == 1) return rstatus;
        AMSTrRecHit* p = (AMSTrRecHit*)AMSEvent::gethead() -> 
                                                   getheadC("AMSTrRecHit",N);
        if (p == NULL && dbg_prtout == 1) {
          cout <<"AMSEventList::AddTrRecHit-I-  "<<nameTrL[N]
               <<" AMSTrRecHit* p == NULL"<<endl;
           return oocSuccess;
        }
        while ( p ) {
        if(p->checkstatus(AMSDBc::USED))
         {
         layerH = new(contTrLayerH[N]) AMSTrRecHitD(p);
         AMSgSen* pSen = p -> getpsen();
         integer gid  = pSen -> getid();
         char*   name = pSen -> getname();
         if (dbg_prtout) cout<<"AddTrRecHit -I- psensor -> id "<<gid
                             <<", psensor -> name "<<name<<endl;
         layerH -> setgid(gid);
         layerH -> setname(name);
         rstatus = eventH -> add_pTrRecHitS(layerH);
         if (rstatus != oocSuccess) {
          Error
            ("AddTrrecHit - cannot set the TrRecHit to Event 'pEventL' assoc");
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
        }//end of USED test
         p = p -> next();
        }
        return rstatus;
}
ooStatus      AMSEventList::
                 AddTOFCluster(const integer N, ooHandle(AMSeventD)& eventH)

{
        ooStatus                 rstatus = oocSuccess;  // Return status
        ooHandle(AMSTOFClusterD) layerH;
        ooHandle(AMSEventList)   listH = ooThis();

        if (N > 4) {
         Error("AddTOFCluster - E - Invalid layer number ");
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

        if (NO_AMS == 1) return oocSuccess;
        AMSTOFCluster* p = (AMSTOFCluster*)AMSEvent::gethead() ->
                                                   getheadC("AMSTOFCluster",N);
        if (p == NULL && dbg_prtout == 1) {cout <<
              "AMSEventList::AddTOFCluster-I- AMSTOFCluster* p == NULL"<<endl;
                return oocSuccess;
        }
        while ( p ) {
         layerH = new(contLayerSH[N]) AMSTOFClusterD(p);
         rstatus = eventH -> add_pTOFCluster(layerH);
         if (rstatus != oocSuccess) {
          Error("AddTOFCluster - cannot set assoc TOFCluster to Event");
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

ooStatus      AMSEventList::AddTrTrack(ooHandle(AMSeventD) & eventH)

{
	ooStatus	        rstatus = oocSuccess;	// Return status
        ooHandle(AMSEventList)  listH = ooThis();


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
         Warning("AMSEventList::AddTrTrack -I- Quit (npat < 1) ");
         return oocSuccess;
        }
        if (NO_AMS == 1) return oocSuccess;

        // get first track of first container
        for (integer i=0; i<npat; i++) {
         AMSContainer* pCont = AMSEvent::gethead() -> getC("AMSTrTrack",i);
         if (pCont ) {
          integer nelem = pCont -> getnelem();
          if (nelem == 0) {
           if(dbg_prtout != 0) cout <<"AMSEventList::AddTrTrack-I- pattern "
                                   <<i<<" elements "<<nelem<<endl;
          }
          if ( nelem > 0) {
           AMSTrTrack* p = (AMSTrTrack*)AMSEvent::gethead() -> 
                                                 getheadC("AMSTrTrack",i);
           while (p ) {
           if(p->checkstatus(AMSDBc::USED))
            {
            trackH = new(contTrackH) AMSTrTrackD(p);
            tracks++;
            p -> setContPos(tracks);
            listH  -> incNTracks();
            trackH -> setPosition(tracks);
            rstatus = eventH -> add_pTrack(trackH);
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
           }//end of USED
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

ooStatus AMSEventList::LinkTrackHitD(ooHandle(AMSeventD)& eventH)
{
	ooStatus	       rstatus = oocSuccess;	// Return status
        ooMode                 mode    = oocUpdate;
        integer                i;


// find track for all patterns
        for (i =0; i<npat; i++) {
// Get head of track's container
        AMSContainer* pCont = AMSEvent::gethead() -> getC("AMSTrTrack",i);
        integer nelem = pCont -> getnelem();
        if ( nelem > 0) {
          AMSTrTrack* p = (AMSTrTrack*)AMSEvent::gethead() -> 
                                                 getheadC("AMSTrTrack",i);
        while ( p ) {
          if(p->checkstatus(AMSDBc::USED))
          {
          integer  posT    = p -> getContPos();     // position of track
          char pred[32];
          (void) sprintf(pred,"_Position=%d",posT);
          if (dbg_prtout) cout<<"LinkTrackHit -I- search for track "
                              <<pred<<endl;
          eventH -> pTrack(trTrackItr,mode,oocAll,pred);
          integer layer = 0;
          for (integer j=0; j<6; j++) {             
           AMSTrRecHit* pH = p -> getphit(j);      // pointer to assoc hit
           if (pH ) {
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
          }  //end of USED
         p = p -> next();
        }
       }
      }
        return rstatus;
}

ooStatus      AMSEventList::AddBeta(ooHandle(AMSeventD) & eventH)

{
	ooStatus	        rstatus = oocSuccess;	// Return status
        ooMode                  mode    = oocUpdate;

        ooHandle(AMSEventList)  listH = ooThis();

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

        if (NO_AMS == 1) return oocSuccess;

        if (npatb < 1) {
         Warning("AddBeta: Quit (npatb<1) ");
         return oocSuccess;
        }
        for (integer i=0; i<npatb; i++) {
          if (dbg_prtout == 1) cout <<" do AddBeta for container "<<i<<endl;
         // get the first beta in the container
         AMSContainer* pCont = AMSEvent::gethead() -> getC("AMSBeta",i);
         if (pCont ) { 
          integer nelem = pCont -> getnelem();
          if (nelem > 0) {
           AMSBeta* p = (AMSBeta*)AMSEvent::gethead() -> 
                                               getheadC("AMSBeta",i);
          while ( p ) {
           betaH = new(contBetaH) AMSBetaD(p);
           rstatus = eventH -> add_pBeta(betaH);
           if (rstatus != oocSuccess) {
            Error("AddBeta: cannot set Beta to Event association.");
            return rstatus;
           }
           // set Beta <-> Track link
           AMSTrTrack* pTr = p -> getptrack();      // pointer to assoc. track
           if (pTr ) {
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
               Error("AddBeta: cannot set Beta to Track association.");
               return rstatus;
              }
             }
            }
           }

          // set Beta <-> TOF Cluster[] link
           for (j=0; j<4; j++) {
            AMSTOFCluster* pTOF = p -> getpcluster(j);   // TOFCl.
            if (pTOF ) {
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
                Error("AddBeta: cannot set Beta to TOFCluster association.");
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

ooStatus      AMSEventList::AddCharge(ooHandle(AMSeventD) & eventH)

{
	ooStatus	        rstatus = oocSuccess;	// Return status
        ooHandle(AMSEventList)  listH = ooThis();
        ooMode                  mode  = oocUpdate;

        if (contChargeH == NULL) {
         rstatus = CheckContainer(chargeCont, oocUpdate, contChargeH);
         if (rstatus != oocSuccess) return rstatus;
        }

        // get number of charges so far
        integer charges  = listH   -> getNCharges();
        if (dbg_prtout == 1) cout <<"found charges in Charge "<<charges<<endl;

        if (NO_AMS == 1) return oocSuccess;

        // get first charge
        AMSCharge* p = (AMSCharge*)AMSEvent::gethead() -> 
                                               getheadC("AMSCharge",0);
        if (p == NULL && dbg_prtout == 1) {
         Message("AddCharge : AMSCharge* p == NULL");
         return oocSuccess;
        }

        while ( p ) {
          chargeH = new(contChargeH) AMSChargeD(p);
          rstatus = eventH -> add_pChargeE(chargeH);
          if (rstatus != oocSuccess) {
           Error("AddCharge: cannot set Charge to Event association.");
           return rstatus;
          }
         
         // set Beta <-> Charge link
           AMSBeta* pB = p -> getpbeta();          // pointer to assoc. beta
           if (pB ) {
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
                Error("AddCharge: cannot set Charge to Beta association.");
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

ooStatus      AMSEventList::AddCTCCluster(ooHandle(AMSeventD)& eventH) 
{
	ooStatus	         rstatus = oocSuccess;	// Return status
        ooHandle(AMSCTCClusterD) ctcclusterH;
        ooHandle(AMSEventList)   listH = ooThis();

        if (contCTCClusterH == NULL) {
         rstatus = CheckContainer(ctcclusterCont, oocUpdate, contCTCClusterH);
         if (rstatus != oocSuccess) return rstatus;
        }

        integer   ctccl  = listH -> getNCTCClusters();
        if (dbg_prtout == 1)
        cout <<"AMSEventList::AddCTCCluster -I- found CTC clusters for "
             <<ctccl<<endl;
        
        if (NO_AMS == 1) return oocSuccess;

        for (int icnt=0; icnt<CTCDBc::getnlay(); icnt++) {
         AMSCTCCluster * p = (AMSCTCCluster*)AMSEvent::gethead() ->
                                               getheadC("AMSCTCCluster",icnt);
         while ( p ) {
          if(p->checkstatus(AMSDBc::USED))
          {
          ctcclusterH = new(contCTCClusterH) AMSCTCClusterD(p);
          rstatus = eventH -> add_pCTCCluster(ctcclusterH);
          if (rstatus != oocSuccess) {
           Error("AddCTCCluster: cannot set CTCCluster to Event assoc.");
           return rstatus;
          }
          ctccl++;
          eventH -> incCTCClusters();
          p -> setContPos(ctccl);
          ctcclusterH -> setPosition(ctccl);
          listH  -> incNCTCClusters();
          } //end of USED
          p = p -> next();
         }
        }
        return rstatus;
}

ooStatus      AMSEventList::AddParticle(ooHandle(AMSeventD) & eventH)

{
	ooStatus	        rstatus = oocSuccess;	// Return status
        ooHandle(AMSEventList)  listH = ooThis();

        if (contParticleH == NULL) {
         rstatus = CheckContainer(particleCont, oocUpdate, contParticleH);
         if (rstatus != oocSuccess) return rstatus; }

        // get number of particle so far
        integer particles  = listH   -> getNParticles();
        if (dbg_prtout == 1)
        cout <<"found particles in Container "<<particles<<endl;

        if (NO_AMS == 1) return oocSuccess;

        // get first particle
        AMSParticle* p = (AMSParticle*)AMSEvent::gethead() -> 
                                               getheadC("AMSParticle",0);
        if (p == NULL && dbg_prtout == 1) {
         Message("AddParticle : AMSParticle* p == NULL");
         return oocSuccess;
        }

        while (p ) {
         particleH = new(contParticleH) AMSParticleD(p);
         rstatus = eventH -> add_pParticleE(particleH);
         if (rstatus != oocSuccess) {
          Error("AddParticle: cannot set Particle to Event assoc.");
          return rstatus;
         }
         // set Beta <-> Particle link
           AMSBeta* pB = p -> getpbeta();          // pointer to assoc. beta
           if (pB ) {
            integer  posB   = pB -> getContPos();  // position of Beta
            if (posB != 0) {
             // find Beta
             char pred[32];
             (void) sprintf(pred,"_Position=%d",posB);
             eventH -> pBeta(betaItr,oocUpdate,oocAll,pred);
             while (betaItr.next()) {                   // get position of  
               rstatus = particleH -> set_pBetaP(betaItr);
               if (rstatus != oocSuccess) {
                Error("AddParticle: cannot set Particle to Beta assoc.");
                return rstatus;
               }
             // set Particle <-> Track link
             trackH  = betaItr -> pTrackBeta();
             if (trackH != NULL) {
              rstatus = particleH -> set_pTrackP(trackH);
              if (rstatus != oocSuccess) return rstatus;
             } else {
              Warning("AddParticle: cannot find an associated track ");
              }
             }
            }
           }

           AMSCharge* pC = p -> getpcharge();      // pointer to assoc. charge
           if (pC ) {
            integer  posC   = pC -> getContPos();  // position of Charge
            if (posC != 0) {
             // find Charge
             char pred[32];
             (void) sprintf(pred,"_Position=%d",posC);
             eventH -> pChargeE(chargeItr,oocUpdate,oocAll,pred);
             while (chargeItr.next()) {                   
               rstatus = particleH -> set_pChargeP(chargeItr);
               if (rstatus != oocSuccess) {
                Error("AddParticle : cannot set Particle to Charge assoc.");
                return rstatus;
               }
             }
            }
           }
            // set particle -> ctc link
            for (int icnt=0; icnt<CTCDBc::getnlay(); icnt++) {
             AMSCTCCluster* pCTC = p -> getpctc(icnt);
               if (pCTC ) {
                integer posCTC = pCTC -> getContPos();
                if (posCTC != 0) {
                // find CTC
                char pred[32];
                (void) sprintf(pred,"_Position=%d",posCTC);
                eventH -> pCTCCluster(ctcClusterItr,oocUpdate,oocAll,pred);
                while (ctcClusterItr.next()) {          // get position of  
                 rstatus = particleH -> add_pCTCClusterP(ctcClusterItr);
                 if (rstatus != oocSuccess) {
                  Error("AddParticle : cannot set Particle to CTC assoc.");
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
          p = p -> next();
        }
        return rstatus;
}           

ooStatus      AMSEventList::
                  LinkHitClusterD(const integer N,ooHandle(AMSeventD)& eventH)

{
	ooStatus	       rstatus = oocSuccess;	// Return status
        ooMode                 mode  = oocUpdate;

// Get head of hit's container
        AMSTrRecHit* p = (AMSTrRecHit*)AMSEvent::gethead() -> 
                                                   getheadC("AMSTrRecHit",N);
        while ( p ) {
         if(p->checkstatus(AMSDBc::USED))
         {
         integer  posH    = p -> getContPos();     // position of hit 
         for (integer j=0; j<2; j++) {
          AMSTrCluster* pC = p -> getClusterP(j);  // pointer to assoc cluster
          if (pC ) {
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
                Error("LinkHitClusterD: cannot set Cluster to Hit assoc.");
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
         } //end of USED
         p = p -> next();
        }
        return rstatus;
}

ooStatus      AMSEventList::AddParticleS(ooHandle(AMSeventD) & eventH)

{
	ooStatus	        rstatus = oocSuccess;	// Return status
        ooHandle(AMSEventList)  listH = ooThis();

        if (contParticleH == NULL) {
         rstatus = CheckContainer(particleCont, oocUpdate, contParticleH);
         if (rstatus != oocSuccess) return rstatus; }

        if (NO_AMS == 1) return oocSuccess;

        // get first particle
        AMSParticle* p = (AMSParticle*)AMSEvent::gethead() -> 
                                               getheadC("AMSParticle",0);
        if (p == NULL && dbg_prtout == 1) {
         Message("AddParticle : AMSParticle* p == NULL");
         return oocSuccess;
        }

        int npart = 0;
        while (p ) {
          npart++;
          p = p -> next();
        }
        ParticleS *particles = new ParticleS[npart];

        p = (AMSParticle*)AMSEvent::gethead() -> getheadC("AMSParticle",0);

        int i = 0;
        ParticleS particle;
        while (p ) {
         particle.setAll(
                            p -> getgpart(), p -> getmass(), p -> geterrmass(),
                            p -> getmomentum(), p -> geterrmomentum(), 
                            p -> getcharge(), p -> gettheta(), p -> getphi(),
                            p -> getcoo());
         particles[i] = particle;
         i++;
         p = p -> next();
        }
        
        eventH -> addParticle(npart, particles);

        rstatus = oocSuccess;

        return rstatus;
}

ooStatus      AMSEventList::AddAntiCluster(ooHandle(AMSeventD) & eventH)

{
	ooStatus	        rstatus = oocSuccess;	// Return status
        ooHandle(AMSEventList)  listH = ooThis();


        if (contAntiClusterH == NULL) {
         rstatus = CheckContainer(anticlusterCont,oocUpdate,contAntiClusterH);
         if (rstatus != oocSuccess) return rstatus;
        }

        // get number of clusters so far
        integer clusters  = listH -> getNAntiClusters();
        if (dbg_prtout) cout 
            <<"AddAntiClusters -I-  anticlusters in the list "<<clusters<<endl;
        if (NO_AMS == 1) return oocSuccess;

        AMSContainer* pCont = AMSEvent::gethead() -> getC("AMSAntiCluster",0);
        if (pCont ) {
         integer nelem = pCont -> getnelem();
         if (nelem >0) {
// create VArray to store nelem AntiClusters
// AMSAntiClusterV(nelem) will invoke default constructor of AMSAntiClusterD 

          anticlusterH = new(contAntiClusterH) AMSAntiClusterV(nelem);
          rstatus = eventH -> set_pAntiCluster(anticlusterH);
          if (rstatus != oocSuccess) {
           Error("AddAntiCluster: cannot set the AntiCluster to Event assoc.");
           return rstatus;
          }
         // get first cluster
         AMSAntiCluster* p = (AMSAntiCluster*)AMSEvent::gethead() -> 
                                                 getheadC("AMSAntiCluster",0);
//         AMSAntiRawCluster* pR = (AMSAntiRawCluster*)AMSEvent::gethead() ->
//                                              getheadC("AMSAntiRawCluster",0);

          integer i = 0;
          while ( p ) {
          if(p->checkstatus(AMSDBc::USED))
           {
           AMSAntiClusterD   anticluster;
           AMSAntiClusterD  *pD;
           pD = &anticluster;
        if(pD ) cout << "!!!Warning AMSAntiClusterD is not NULL " << i << endl;
//        if(pR == NULL) cout << "!!!Warning AMSAntiRawCluster is NULL" << endl;
           pD -> add(p);
           anticlusterH -> add(i,anticluster);
           i++;
           listH  -> incNAntiClusters();
           eventH -> incAntiClusters();
           } //end of USED
           p = p -> next();
          }
//check how many of them have actually been written if none delete the  AMSAntiClusterV
       AMSAntiClusterV *pAntiV =  anticlusterH;
       if (i == 0 ) {
                       rstatus = eventH -> del_pAntiCluster();
                       delete pAntiV;
                      }
         } else { 
         if(dbg_prtout == 1)
          Message("AddAntiCluster: No AntiClusters for this event");
         }
        } else {
        Warning("AddAntiCluster: pointer to AMSAntiCluster container is NULL");
        }
        return rstatus;
}

ooStatus   AMSEventList::CopyTrRecHit(ooHandle(AMSeventD)& eventH)
{
	ooStatus	       rstatus = oocSuccess;	// Return status
        integer                ihits = 0;
        ooMode                 mode = oocRead;
        ooHandle(AMSEventList) listH = ooThis();

        if (eventH == NULL) {
         Error("CopyTrRecHit: eventH  == NULL ");
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
             cout<<"CopyTrRecHit -E- cannot getgeomvolume, id "<<gid
                 <<", name "<<name<<endl;
             Message("cannot set sensor pointer");
           }
            for (integer j=0; j<2; j++) {
             if(j==0) clusterH = trRecHitItr -> pClusterX();
             if(j==1) clusterH = trRecHitItr -> pClusterY();
             if(clusterH != NULL) {
              integer pos = clusterH -> getPosition(); 
              integer status = trRecHitItr -> getStatus();
              p -> setstatus(status);
              AMSTrCluster* pU = (AMSTrCluster*)AMSEvent::gethead() -> 
                                                getheadC("AMSTrCluster",j);
              while (pU != NULL ) {
               if (pos == pU -> getContPos()) {
                if (dbg_prtout == 1) cout <<" found cluster pos "<<pos
                                          <<", X/Y "<<j<<" for hit pos "
                                          <<posCont<<endl;
                break;
               }
               pU = pU -> next();
              }
              if (pU != NULL) {
               p -> setClusterP(pU,j);
              } else{ 
               Warning("CopyTrRecHit : cannot find an associated cluster");
              }
             } else {
              cout<<"for hit pos "<<posCont<<" no associated cluster"<<endl;
             }
            }
           integer layer  = trRecHitItr -> getLayer();
           AMSEvent::gethead()->addnext(AMSID("AMSTrRecHit",layer-1),p);
          }
          ihits++;
         }
         if (dbg_prtout) cout<<"CopyTrRecHit:: hits "<<ihits<<" total size "
                             <<sizeof(AMSTrRecHitD)*ihits<<endl;
        recevent_size = recevent_size + sizeof(AMSTrRecHitD)*ihits;

        return rstatus;
}

ooStatus      AMSEventList::CopyTrCluster(ooHandle(AMSeventD)& eventH)

{
	ooStatus	        rstatus = oocSuccess;	// Return status
        integer                 iclusters = 0;
        ooHandle(AMSEventList)  listH = ooThis();
        ooMode                  mode = oocRead;  

        if (eventH == NULL) {
         Error("CopyTrCluster: eventH  == NULL ");
         return oocError;
        }

         for (integer j =0; j<2; j++) {
          if(contClusterH[j] == NULL ) {
           rstatus = CheckContainer(trclusterCont[j], mode, contClusterH[j]);
           if (rstatus != oocSuccess) return rstatus;
          }
         }

         number        pvalues[100];
         number*       pValues;
         AMSTrCluster* p;
         eventH -> pCluster(trClusterItr, mode);
         while (trClusterItr.next()) {
          if (dbread_only != 0) {
            if (dbg_prtout) cout <<"CopyTrCluster nelem, nelemL, nelemR "
                                 <<trClusterItr -> getnelem()<<", "
                                 <<trClusterItr -> getnelemL()<<", "
                                 <<trClusterItr -> getnelemR()<<endl;
           integer nelem = trClusterItr -> getnelem();
           if(nelem > 0 && nelem < 100) trClusterItr -> getValues(pvalues);
           if(nelem > 100) {
             pValues = new number[nelem];
             trClusterItr -> getValues(pValues);
           }
           if (nelem < 100) {
                        p = new AMSTrCluster(
                                              trClusterItr -> getidsoft(), 
                                              trClusterItr -> getstatus(), 
                                              trClusterItr -> getnelemL(), 
                                              trClusterItr -> getnelemR(), 
                                              trClusterItr -> getVal(), 
                                              trClusterItr -> getSigma(),
                                              trClusterItr -> getcofg(), 
                                              trClusterItr -> getRms(),
                                              pvalues);
           } else {
                         p = new AMSTrCluster(
                                              trClusterItr -> getidsoft(), 
                                              trClusterItr -> getstatus(), 
                                              trClusterItr -> getnelemL(), 
                                              trClusterItr -> getnelemR(), 
                                              trClusterItr -> getVal(), 
                                              trClusterItr -> getSigma(),
                                              trClusterItr -> getcofg(), 
                                              trClusterItr -> getRms(),
                                              pValues);
             delete [] pValues;
           }
           integer     posCont=  trClusterItr -> getPosition();
           p -> setContPos(posCont);
           integer side = trClusterItr -> getSide();
           AMSEvent::gethead() -> addnext(AMSID("AMSTrCluster",side),p);
          }
          iclusters++;
         }
         if (dbg_prtout == 1) cout 
            <<"AMSEventList::CopyTrCluster: Layers X&Y "<<" clusters "
            <<iclusters<<" total size "<<sizeof(AMSTrClusterD)*iclusters<<endl;

        recevent_size = recevent_size + sizeof(AMSTrClusterD)*iclusters;
        return rstatus;
} 

ooStatus  AMSEventList::CopyTrTrack(ooHandle(AMSeventD)& eventH)

{
	ooStatus	       rstatus = oocSuccess;	// Return status
        integer                itracks = 0;
        ooMode                 mode = oocRead;

        if (eventH == NULL) {
         Error("CopyTrTrack : eventH  == NULL ");
         return oocError;
        }

         eventH -> pTrack(trTrackItr, mode);

         while (trTrackItr.next()) {
          if (dbread_only != 0) {
           AMSTrRecHit * phit[6];
           integer       Npos;    
           Npos = 0;
           for (int i=0; i<6; i++) {phit[i] = NULL;}
           trTrackItr -> pTrRecHitT(trRecHitItr, mode);
           while (trRecHitItr.next()) {                   // get position of  
            integer posH  = trRecHitItr -> getPosition(); // assoc.hit in dbase
            integer layer = trRecHitItr -> getLayer();
            if (dbg_prtout) cout <<"Hit position, layer "<<posH<<", "
                                                         <<layer<<endl;
            // find the same hit in Upool
            AMSTrRecHit* pH = (AMSTrRecHit*)AMSEvent::gethead() -> 
                                              getheadC("AMSTrRecHit",layer-1);
            while (pH ) {
             if (posH != pH -> getContPos()) {
               pH = pH -> next();
             } else{
               break;
             }
            }
            if (pH ) {
             phit[Npos] = pH;
             Npos++;
            } else{ 
             Warning("AddTrTrack : cannot find an associated hit ");
            }
           }
           integer     pattern = trTrackItr  -> getPattern();
           AMSTrTrack* p = new AMSTrTrack(
                                          pattern,
                                          trTrackItr -> getnhits(),
                                          phit
                                          );
           integer     posCont =  trTrackItr -> getPosition();
           p -> setContPos(posCont);
           trTrackItr -> copy(p);
           AMSEvent::gethead() -> addnext(AMSID("AMSTrTrack",pattern),p);
          }
          itracks++;
         }
         if (dbg_prtout == 1) cout <<"AMSEventList::AddTrTrack: Total size "
                                   <<sizeof(AMSTrTrackD)*itracks<<endl;
        recevent_size = recevent_size + sizeof(AMSTrTrackD)*itracks;

        return rstatus;
}

ooStatus      AMSEventList::CopyTOFCluster(ooHandle(AMSeventD)& eventH)

{
	ooStatus	         rstatus = oocSuccess;	// Return status
        ooHandle(AMSTOFClusterD) layerH;
        ooHandle(AMSEventList)   listH = ooThis();
        integer                  itofcl = 0;
        ooMode                   mode = oocRead;

        if (eventH == NULL) {
         Error("CopyTOFCluster : eventH  == NULL ");
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
         if (dbg_prtout) cout <<"CopyTOFCluster: clusters "<<itofcl
                              <<" total size "<<sizeof(AMSTOFClusterD)*itofcl
                              <<endl;

        recevent_size = recevent_size + sizeof(AMSTOFClusterD)*itofcl;

        return rstatus;
}


ooStatus      AMSEventList::CopyBeta(ooHandle(AMSeventD) & eventH)

{
	ooStatus	        rstatus = oocSuccess;	// Return status
        ooHandle(AMSEventList)  listH = ooThis();
        integer                 ibeta = 0;
        integer                 patternB;
        ooMode                  mode = oocRead;

        if (eventH == NULL) {
         Error("CopyBeta: eventH  == NULL ");
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
        recevent_size = recevent_size + sizeof(AMSBetaD)*ibeta;

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
              while (p ) {
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
                 while (pT ) {
                  if (posT != pT -> getContPos()) {
                   pT = pT -> next();
                  } else {
                   break;
                  }
                 }
                 if (pT ) {
                  p -> setTrackP(pT);
                  if (dbg_prtout == 1) cout << 
                     "AMSEventList:: CopyBeta -I- find an "
                                                   <<" associated track"<<endl;
                  } else {
                   Warning("CopyBeta : cannot find an associated track");
                 }
                }
               }
               p = p -> next();
              }   
         // set Beta <-> TOFCluster[] association
         p = (AMSBeta*)AMSEvent::gethead() -> getheadC("AMSBeta",ib);
         while (p ) {
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
              while (pC ) {
               if (posC != pC -> getContPos()){
                  pC = pC -> next();
                } else {
                  break;
               }
              }
              if (pC ) {
               p -> setTOFClP(pC, ntof);
               if (dbg_prtout == 1) cout 
                  <<"AMSEventList:: CopyBeta -I- find an "
                            <<" associated TOF cluster"<<endl;
              } else {
               Warning("CopyBeta : - cannot find an associated TOF cluster");
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

ooStatus      AMSEventList::CopyCharge(ooHandle(AMSeventD) & eventH)

{
	ooStatus	        rstatus = oocSuccess;	// Return status
        ooHandle(AMSEventList)  listH = ooThis();
        ooMode                  mode = oocRead;

        if (eventH == NULL) {
         Error("CopyCharge : eventH  == NULL ");
         return oocError;
        }

        if (contChargeH == NULL) {
         rstatus = CheckContainer(chargeCont, mode, contChargeH);
         if (rstatus != oocSuccess) return rstatus;
        }

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
           while (p ) {
            integer posCh = p -> getContPos();
            if (posCh != 0) {
             betaH = chargeItr -> pBetaCh();                  // in dbase
             integer posB     = betaH -> getPosition();
             integer patternB = betaH -> getpattern();
             AMSBeta* pB = (AMSBeta*)AMSEvent::gethead() ->
                                                  getheadC("AMSBeta",patternB);
             while (pB ) {
              if (posB != pB -> getContPos()) {
               pB = pB -> next();
               } else {
                break;
               }
             }
              if (pB ) {
               p -> setBetaP(pB);
               if (dbg_prtout == 1) 
                Message("CopyCharge : find an associated Beta");
              } else {
                Warning("CopyCharge : cannot find an associated beta");
              }
            }
           p = p -> next();
           }
          }
         }
         recevent_size = recevent_size + sizeof(AMSChargeD);
        }
        return rstatus;
}        
ooStatus      AMSEventList::CopyParticle(ooHandle(AMSeventD) & eventH)

{
	ooStatus	        rstatus = oocSuccess;	// Return status
        ooHandle(AMSEventList)  listH = ooThis();
        integer                 iparticle = 0;
        ooMode                  mode = oocRead;

        if (eventH == NULL) {
         Error("CopyParticle : eventH  == NULL ");
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
           while (p ) {
            integer posP = p -> getContPos();
            if (posP != 0) {
             // set particle <-> beta link
             betaH = particleItr -> pBetaP();                  // in dbase
             integer posB      = betaH -> getPosition();
             integer patternB  = betaH -> getpattern();
             AMSBeta* pB = (AMSBeta*)AMSEvent::gethead() ->
                                                getheadC("AMSBeta",patternB);
             while (pB ) {
              if (posP != pB -> getContPos()) {
               pB = pB -> next();
               } else {
                break;
               }
              }
              if (pB ) {
               p -> setpbeta(pB);
               if (dbg_prtout) 
                           Message("CopyParticle : found an associated Beta");
              } else {
               Warning("CopyParticle : cannot find an associated beta");
              }
             // set particle <-> charge link
              chargeH = particleItr -> pChargeP();
              integer posC = chargeH -> getPosition();
              AMSCharge* pC = (AMSCharge*)AMSEvent::gethead() ->
                                                      getheadC("AMSCharge",0);
              while (pC ) {
                if (posC != pC -> getContPos()) {
                  pC = pC -> next();
                } else {
                  break;
                }
              }
              if (pC ) {
               p -> setpcharge(pC);
               if (dbg_prtout) 
                Message("CopyParticle : found an associated Charge");
              } else {
                Warning("CopyParticle : cannot find an associated charge");
              }
              // set particle <-> track link
              trackH = particleItr -> pTrackP();
              integer posT = trackH -> getPosition();
              integer patternT = trackH -> getPattern();
              AMSTrTrack* pT = (AMSTrTrack*)AMSEvent::gethead() ->
                                              getheadC("AMSTrTrack",patternT);
              while (pT ) {
               if (posT != pT -> getContPos()) {
                 pT = pT -> next();
               } else {
                 break;
               }
              }
              if (pT ) {
               p -> setptrack(pT);
               if (dbg_prtout) 
                Message("CopyParticle found an associated track");
               } else {
                Warning("CopyParticle : cannot find an associated track");
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
                while (pCTC ) {
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
        recevent_size = recevent_size + sizeof(AMSParticleD);
        }
        return rstatus;
}

ooStatus      AMSEventList::CopyCTCCluster(ooHandle(AMSeventD)& eventH)

{
	ooStatus	         rstatus = oocSuccess;	// Return status
        ooHandle(AMSCTCClusterD) ctcclusterH;
        integer                  ictccl = 0;
        ooHandle(AMSEventList)  listH = ooThis();
        ooMode                  mode = oocRead;

        if (eventH == NULL) {
         Error("CopyCTCCluster: eventH  == NULL ");
         return oocError;
        }

         if (contCTCClusterH == NULL) {
          rstatus = CheckContainer(ctcclusterCont, mode, contCTCClusterH);
          if (rstatus != oocSuccess) return rstatus;
         }


        eventH -> pCTCCluster(ctcClusterItr, mode);

        while (ctcClusterItr.next()) {
         if (dbread_only != 0) {
          AMSCTCCluster* p = new AMSCTCCluster(
                                               ctcClusterItr -> getstatus(),
                                               ctcClusterItr -> getLayer(),
                                               ctcClusterItr -> getcoo(),
                                               ctcClusterItr -> getecoo(),
                                               ctcClusterItr -> getrawsignal(),
                                               ctcClusterItr -> geterror()
                                               );
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
        recevent_size = recevent_size + sizeof(AMSCTCClusterD)*ictccl;
        return rstatus;
}

ooStatus      AMSEventList::CopyAntiCluster(ooHandle(AMSeventD)& eventH)

{
	ooStatus	         rstatus = oocSuccess;	// Return status
        ooItr(AMSAntiClusterV)   anticlusterItr;
        integer                  ictccl = 0;
        ooHandle(AMSEventList)  listH = ooThis();
        ooMode                  mode = oocRead;

        if (eventH == NULL) {
         Error("CopyAntiCluster: eventH  == NULL ");
         return oocError;
        }

         if (contAntiClusterH == NULL) {
          rstatus = CheckContainer(anticlusterCont, mode, contAntiClusterH);
          if (rstatus != oocSuccess) return rstatus;
         }


        eventH -> pAntiCluster(anticlusterItr, mode);
        integer imcs = 0;
        while (anticlusterItr.next()) {
         if (dbread_only != 0) {
          integer nelem = anticlusterItr -> getnelem();
          if (nelem > 0) {
            AMSAntiClusterD anticlusterD = anticlusterItr -> get(imcs);
            AMSAntiClusterD* pD = & anticlusterD;
            AMSAntiCluster* p = new AMSAntiCluster(
                                                   pD -> getstatus(),
                                                   pD -> getsector(),
                                                   pD -> getedep(),
                                                   pD -> getcoo(),
                                                   pD -> getecoo()
                                                  );
            AMSEvent::gethead() -> addnext(AMSID("AMSAntiCluster",0),p);

          }
         }
         imcs++;
        }
       if (dbg_prtout==1) cout<<"ReadAntiCluster:  clusters "
                                <<imcs<<" total size "
                                << sizeof(AMSAntiClusterD)*imcs<<endl;
         recevent_size = recevent_size + sizeof(AMSAntiClusterD)*imcs;

        return rstatus;
}
