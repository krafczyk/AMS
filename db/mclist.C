// method source file for the object AMSEventList
// First try with Objectivity May 02, 1996
//
// last edit May 8, 1997, ak.
//
//
#include <iostream.h>
#include <string.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>

#include <rd45.h>

#include <db_comm.h>

#include <mclist_ref.h>
#include <mcevent_ref.h>

#include <mcanticlusterD_ref.h>
#include <mcctcclusterD_ref.h>
#include <mceventgD_ref.h>
#include <mctofclusterV_ref.h>
#include <tmcclusterV_ref.h>

#include <mclist.h>
#include <mcevent.h>

#include <mcanticlusterD.h>
#include <mcctcclusterD.h>
#include <mceventgD.h>
#include <mctofclusterV.h>
#include <mctofclusterD.h>
#include <tmcclusterV.h>


#include <job.h>
#include <event.h>
#include <mccluster.h>

// container  names
  static char* antimcclusterCont;
  static char* ctcmcclusterCont;
  static char* mceventgCont;
  static char* tofmcclusterCont;
  static char* trmcclusterCont;

//
ooHandle(ooContObj)  contAntiMCClusterH;      // Anti MC cluster 
ooHandle(ooContObj)  contCTCMCClusterH;       // CTC MC cluster 
ooHandle(ooContObj)  contmceventgH;           // MC Event
ooHandle(ooContObj)  contMCClusterH;          // MC cluster 
ooHandle(ooContObj)  contTOFMCClusterH;       // TOF MC cluster 

// ooObj classes
ooHandle(AMSAntiMCClusterD) AntiMCClusterH;
ooHandle(AMSCTCMCClusterD)  CTCMCClusterH;
ooHandle(AMSmceventgD)      mceventgH;
ooHandle(AMSTOFMCClusterV)  tofmcclusterH;
ooHandle(AMSTrMCClusterV)   trmcclusterH;

// Iterators
ooItr(AMSAntiMCClusterD) AntiMCClusterItr;        // Anti MC cluster
ooItr(AMSCTCMCClusterD)  CTCMCClusterItr;         // CTC MC cluster
ooItr(AMSmceventgD)      mceventgItr;             // mc event
ooItr(AMSTOFMCClusterV)  tofmcclusterItr;         // TOF MC cluster
ooItr(AMSTrMCClusterV)    trmcclusterItr;          // MC cluster

static integer mcevent_size;

AMSMCEventList::AMSMCEventList(char* listname)
{
  setlistname(listname);
  resetCounters();
  resetMCCounters();
  SetContainersNames(listname);
}

AMSMCEventList::AMSMCEventList(char* listname, char* setup)
{
  setlistname(listname);
  setsetupname(setup);
  resetCounters();
  resetMCCounters();
  SetContainersNames(listname);
}

void AMSMCEventList::resetMCCounters()
{
  _nMCClusters     = 0;
  _nTOFMCClusters  = 0;
  _nAntiMCClusters = 0;
  _nCTCMCClusters  = 0;
  _nmcEventg       = 0;
}

void AMSMCEventList::SetContainersNames(char* listname)
{
//mceventg
  if (!mceventgCont) mceventgCont =  StrCat("mceventg_",listname);
//AntiMCCluster
   if (!antimcclusterCont) 
                    antimcclusterCont =  StrCat("AntiMCCluster_",listname);
//CTCMCCluster
   if (!ctcmcclusterCont) 
                    ctcmcclusterCont =  StrCat("CTCMCCluster_",listname);
//TOFMCCluster
   if (!tofmcclusterCont) 
                    tofmcclusterCont =  StrCat("TOFMCCluster_",listname);
//trMCCluster
   if (!trmcclusterCont) trmcclusterCont =  StrCat("TrMCCluster_",listname);
} 

ooStatus      AMSMCEventList::Addmceventg(ooHandle(AMSmcevent)& eventH)
{
  	ooStatus	          rstatus = oocSuccess;	// Return status
        ooHandle(AMSMCEventList)  listH = ooThis();

        if (contmceventgH == NULL) {
         rstatus = CheckContainer(mceventgCont, oocUpdate, contmceventgH);
         if (rstatus != oocSuccess) return rstatus;
        }

        integer   mceventg  = listH -> getNmcEventg();
        if (dbg_prtout == 1)
                     cout <<"AddmcEventg -I- found mcEventg "<<mceventg<<endl;
        AMSmceventg * p = (AMSmceventg*)AMSEvent::gethead() ->
                                                 getheadC("AMSmceventg",0);
        if (p == NULL) {
         Warning("AddmcEventg : AMSmceventg* p == NULL");
         return oocSuccess;
        }
        while ( p != NULL) {
         mceventgH = new(contmceventgH) AMSmceventgD(p);
         rstatus = eventH -> set_pmcEventg(mceventgH);
         if (rstatus != oocSuccess) {
          Error("AddmcEventg: cannot set the mceventg to Event association.");
          return rstatus;
         }
         mceventg++;
         listH  -> incNmcEventg();
         p = p -> next();
        }
        return rstatus;
}

ooStatus AMSMCEventList::Addtrmccluster(ooHandle(AMSmcevent) & eventH)

{
	ooStatus	          rstatus = oocSuccess;	// Return status
        ooHandle(AMSMCEventList)  listH = ooThis();

        if (contMCClusterH == NULL) {
         rstatus = CheckContainer(trmcclusterCont, oocUpdate, contMCClusterH);
         if (rstatus != oocSuccess) return rstatus;
        }

        // get number of MCClusters so far
        integer mcclusters  = listH ->   getNMCClusters();
        if (dbg_prtout == 1) cout <<"found TrMCClusters  "<<mcclusters<<endl;

        // get head of AMSTrMCCluster Container
        AMSContainer* pCont = AMSEvent::gethead() -> getC("AMSTrMCCluster",0);
        integer nelem = pCont -> getnelem();
        if (nelem > 0) {
          // create VArray to store TOFMCClusters
          trmcclusterH = new(contMCClusterH) AMSTrMCClusterV(nelem);
          rstatus = eventH -> set_pMCCluster(trmcclusterH);
          if (rstatus != oocSuccess) {
           Error("AddTrMCCluster : cannot set the MCCluster to Event assoc.");
           return rstatus;
          }
          // get first cluster
          AMSTrMCCluster* p = (AMSTrMCCluster*)AMSEvent::gethead() -> 
                                               getheadC("AMSTrMCCluster",0);
          integer i = 0;
          while( p != NULL) {
           AMSTrMCClusterD  trmccluster;
           AMSTrMCClusterD* pD;
           pD = &trmccluster;
           pD -> add(p);
           trmcclusterH -> add(i,trmccluster);
           i++;
           mcclusters++;
           listH -> incNMCClusters();
           p = p -> next();
          }
        } //nelem > 0

        return rstatus;
}

ooStatus AMSMCEventList::Addtofmccluster(ooHandle(AMSmcevent) & eventH)
{
	ooStatus	          rstatus = oocSuccess;	// Return status
        ooHandle(AMSMCEventList)  listH = ooThis();


        if (contTOFMCClusterH == NULL) {
         rstatus = CheckContainer
                            (tofmcclusterCont, oocUpdate, contTOFMCClusterH);
         if (rstatus != oocSuccess) return rstatus;
        }

        // get number of MCClusters so far
        integer tofmcclusters  = listH ->   getNTOFMCClusters();
        if (dbg_prtout==1) cout <<"found TOFMCClusters  "<<tofmcclusters<<endl;

        // get head of AMSTOFMCCluster Container
        AMSContainer* pCont = AMSEvent::gethead() -> getC("AMSTOFMCCluster",0);
        integer nelem = pCont -> getnelem();
        if (nelem > 0) {
          // create VArray to store TOFMCClusters
         tofmcclusterH = new(contTOFMCClusterH) AMSTOFMCClusterV(nelem);
         rstatus = eventH -> set_pTOFMCCluster(tofmcclusterH);
         if (rstatus != oocSuccess) {
          Error("AddTOFMCCluster: cannot set the MCCluster to Event assoc.");
          return rstatus;
         }
         // get first cluster
         AMSTOFMCCluster* p = (AMSTOFMCCluster*)AMSEvent::gethead() -> 
                                            getheadC("AMSTOFMCCluster",0);
         integer i =0;
         while ( p != NULL) {
          AMSTOFMCClusterD  tofmccluster;
          AMSTOFMCClusterD* pD;
          pD = &tofmccluster;
          pD -> add(p);
          tofmcclusterH -> add(i,tofmccluster);
          i++;
          tofmcclusters++;
          listH -> incNTOFMCClusters();
          p = p -> next();
         }
        } //nelem > 0
        return rstatus;
}

ooStatus AMSMCEventList::Addctcmccluster(ooHandle(AMSmcevent) & eventH)
{
	ooStatus	          rstatus = oocSuccess;	// Return status
        ooHandle(AMSMCEventList)  listH = ooThis();

        if (contCTCMCClusterH == NULL) {
        rstatus = CheckContainer(
                              ctcmcclusterCont, oocUpdate, contCTCMCClusterH);
        if (rstatus != oocSuccess) return rstatus;
        }

        // get number of MCClusters so far
        integer ctcmcclusters  = listH ->   getNCTCMCClusters();
        if (dbg_prtout == 1)
         cout <<"AddCTCMCCluster-I-found CTCMCClusters  "<<ctcmcclusters<<endl;
        // get first cluster
         AMSCTCMCCluster* p = (AMSCTCMCCluster*)AMSEvent::gethead() -> 
                                            getheadC("AMSCTCMCCluster",0);
        if (p == NULL && dbg_prtout) {
         Message("AddCTCMCCluster : AMSCTCMCCluster* p == NULL");
         return oocSuccess;
        }
        while ( p != NULL) {
         CTCMCClusterH = new(contCTCMCClusterH) AMSCTCMCClusterD(p);
         ctcmcclusters++;
         listH -> incNCTCMCClusters();
         rstatus = eventH -> add_pCTCMCCluster(CTCMCClusterH);
         if (rstatus != oocSuccess) {
           Error("AddCTCMCCluster: cannot set the MCCluster to Event assoc.");
           return rstatus;
         }
         p = p -> next();
       }
        return rstatus;
}

ooStatus AMSMCEventList::Addantimccluster(ooHandle(AMSmcevent) & eventH)
{
	ooStatus	        rstatus = oocSuccess;	// Return status
        ooHandle(AMSMCEventList)  listH = ooThis();

        if (contAntiMCClusterH == NULL) {
         rstatus = CheckContainer(
                            antimcclusterCont, oocUpdate, contAntiMCClusterH);
         if (rstatus != oocSuccess) return rstatus;
        }

        // get number of MCClusters so far
        integer antimcclusters  = listH ->   getNAntiMCClusters();
        if (dbg_prtout == 1)
         cout <<"AddAntiCluster: found AntiMCClusters  "<<antimcclusters<<endl;
        // get first cluster
        AMSAntiMCCluster* p = (AMSAntiMCCluster*)AMSEvent::gethead() ->
                                           getheadC("AMSAntiMCCluster",0);
        if (p == NULL && dbg_prtout == 1)  {
         Message("AddAntiMCCluster : AMSAntiMCCluster* p == NULL");
         return oocSuccess;
        }
        while ( p != NULL) {
         integer  idsoft;
         AntiMCClusterH = new(contAntiMCClusterH) AMSAntiMCClusterD(p);
         antimcclusters++;
         listH -> incNAntiMCClusters();
         rstatus = eventH -> add_pAntiMCCluster(AntiMCClusterH);
         if (rstatus != oocSuccess) {
          Error("AddAntiMCCluster: cannot set the MCCluster to Event assoc.");
          return rstatus;
         }
         p = p -> next();
       }
        return rstatus;
}

ooStatus AMSMCEventList::Readantimccluster(ooHandle(AMSmcevent) & eventH)
{
	ooStatus	        rstatus = oocSuccess;	// Return status
        ooHandle(AMSMCEventList) listH = ooThis();

        ooMode mode = oocRead;

        if (contAntiMCClusterH == NULL) {
         rstatus = 
          CheckContainer(antimcclusterCont, mode, contAntiMCClusterH);
          if (rstatus != oocSuccess) return rstatus;
        }

         eventH -> pAntiMCCluster(AntiMCClusterItr, mode);
         integer imcs=0;
         while (AntiMCClusterItr.next()) {
          if (dbread_only != 0) {
           AMSAntiMCCluster* p = new AMSAntiMCCluster(
                                       AntiMCClusterItr -> getid(),
                                       AntiMCClusterItr -> getcoo(),
                                       AntiMCClusterItr -> getedep(),
                                       AntiMCClusterItr -> gettof()
                                      );
           AMSEvent::gethead() -> addnext(AMSID("AMSAntiMCCluster",0),p);
          }
          imcs++;
         }
         if (dbg_prtout == 1) 
          cout <<"ReadAntiMCCluster:  clusters "<<imcs
                <<" total size "<< sizeof(AMSAntiMCClusterD)*imcs<<endl;
         mcevent_size = mcevent_size + sizeof(AMSAntiMCClusterD)*imcs;
        return rstatus;
}

ooStatus AMSMCEventList::Readctcmccluster(ooHandle(AMSmcevent) & eventH)
{
	ooStatus	        rstatus = oocSuccess;	// Return status
        ooHandle(AMSMCEventList)  listH = ooThis();

        ooMode mode = oocRead;

        if (contCTCMCClusterH == NULL) {
         rstatus = CheckContainer(ctcmcclusterCont, mode, contCTCMCClusterH);
         if (rstatus != oocSuccess) return rstatus;
        }

         eventH -> pCTCMCCluster(CTCMCClusterItr, mode);
         integer imcs=0;

         while (CTCMCClusterItr.next()) {
          if (dbread_only != 0) {
           AMSCTCMCCluster* p = new AMSCTCMCCluster(
                                               CTCMCClusterItr -> getid(),
                                               CTCMCClusterItr -> getxcoo(),
                                               CTCMCClusterItr -> getxdir(),
                                               CTCMCClusterItr -> getcharge(),
                                               CTCMCClusterItr -> getstep(),
                                               CTCMCClusterItr -> getbeta(),
                                               CTCMCClusterItr -> getedep()
                                               );
           AMSEvent::gethead() -> addnext(AMSID("AMSCTCMCCluster",0),p);
           imcs++;
          }
         }
         if (dbg_prtout == 1) 
          cout <<"ReadCTCMCCluster:  clusters "<<imcs
                <<" total size "<< sizeof(AMSCTCMCClusterD)*imcs<<endl;
         mcevent_size = mcevent_size + sizeof(AMSCTCMCClusterD)*imcs;
        return rstatus;
}

ooStatus AMSMCEventList::Readtofmccluster(ooHandle(AMSmcevent) & eventH)
{
        ooHandle(AMSMCEventList) listH = ooThis();
	ooStatus	        rstatus = oocSuccess;	// Return status

        ooMode mode = oocRead;

        if (contTOFMCClusterH == NULL) {
         rstatus = CheckContainer(tofmcclusterCont, mode, contTOFMCClusterH);
         if (rstatus != oocSuccess) return rstatus;
        }

         eventH -> pTOFMCCluster(tofmcclusterItr, mode);
         integer           imcs=0;
         while (tofmcclusterItr.next()) {
          if (dbread_only != 0) {
            integer nelem = tofmcclusterItr -> getnelem();
            if (nelem > 0) {
             AMSTOFMCClusterD tofmcclusterD = tofmcclusterItr -> get(imcs);
             AMSTOFMCClusterD* pD = & tofmcclusterD;
             AMSTOFMCCluster* p = new AMSTOFMCCluster(
                                                      pD -> getid(),
                                                      pD -> getxcoo(),
                                                      pD -> getedep(),
                                                      pD -> gettof()
                                                     );
             AMSEvent::gethead() -> addnext(AMSID("AMSTOFMCCluster",0),p);
            }
          }
          imcs++;
         }
         if (dbg_prtout==1) cout<<"ReadTOFMCCluster:  clusters "
                                <<imcs<<" total size "
                                << sizeof(AMSTOFMCClusterD)*imcs<<endl;
         mcevent_size = mcevent_size + sizeof(AMSTOFMCClusterD)*imcs;

        return rstatus;
}

ooStatus AMSMCEventList::Readtrmccluster(ooHandle(AMSmcevent) & eventH)
{
	ooStatus	          rstatus = oocSuccess;	// Return status
        ooHandle(AMSMCEventList)  listH = ooThis();

        ooMode mode = oocRead;

        if (contMCClusterH == NULL) {
         rstatus = CheckContainer(trmcclusterCont, mode, contMCClusterH);
         if (rstatus != oocSuccess) return rstatus;
        }
         eventH -> pMCCluster(trmcclusterItr, mode);
         integer imcs=0;
         while (trmcclusterItr.next()) {
          if (dbread_only != 0) {
           integer nelem = trmcclusterItr -> getnelem();
           if (nelem > 0) {
            AMSTrMCClusterD trmcclusterD = trmcclusterItr -> get(imcs);
            AMSTrMCClusterD* pD;
            pD = & trmcclusterD;
            AMSTrMCCluster* p = new AMSTrMCCluster(
                                                   pD -> getid(),
                                                   pD -> getxca(),
                                                   pD -> getxcb(),
                                                   pD -> getxgl(),
                                                   pD -> getsum(),
                                                   pD -> getitra()
                                                   );
            AMSEvent::gethead() -> addnext(AMSID("AMSTrMCCluster",0),p);
           }
          }
          imcs++;
         }
        mcevent_size = mcevent_size + sizeof(AMSTrMCClusterD)*imcs;
        return rstatus;
}

ooStatus  AMSMCEventList::Readmceventg(ooHandle(AMSmcevent)& eventH)
{
	ooStatus	         rstatus = oocSuccess;	// Return status
        ooHandle(AMSMCEventList)   listH = ooThis();
        integer               imceventg = 0;

        ooMode mode = oocRead;

        if (contmceventgH == NULL) {
         rstatus = CheckContainer(mceventgCont, mode, contmceventgH);
         if (rstatus != oocSuccess) return rstatus;
        }


        eventH -> pmcEventg(mceventgItr, mode);

        while (mceventgItr.next()) {
          if (dbread_only != 0) {
           AMSmceventg* p = new AMSmceventg();
           mceventgItr -> copy(p);
           //integer pos = mceventgItr -> getPosition();
           //p->setContPos(pos);
           AMSEvent::gethead() -> addnext(AMSID("AMSmceventg",0),p);
           if (imceventg == 0) p -> InitSeed();
           p->run();
          }
          imceventg++;
         }
         if (dbg_prtout == 1) 
          cout <<"Readmceventg:  events "<<imceventg
                <<" total size "<< sizeof(AMSmceventgD)*imceventg<<endl;
         mcevent_size = mcevent_size + sizeof(AMSmceventgD)*imceventg;
        return rstatus;
}

