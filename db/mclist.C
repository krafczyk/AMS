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
#include <trigger1D_ref.h>
#include <trigger3D_ref.h>

#include <mclist.h>
#include <mcevent.h>

#include <mcanticlusterD.h>
#include <mcctcclusterD.h>
#include <mceventgD.h>
#include <mctofclusterV.h>
#include <mctofclusterD.h>
#include <tmcclusterV.h>
#include <trigger1D.h>
#include <trigger3D.h>


#include <job.h>
#include <event.h>
#include <mccluster.h>

// container  names
  static char* antimcclusterCont;
  static char* ctcmcclusterCont;
  static char* mceventgCont;
  static char* tofmcclusterCont;
  static char* trmcclusterCont;
  static char* triggerlvl1Cont;
  static char* triggerlvl3Cont;


// ooContObj classes
ooHandle(ooContObj)  contAntiMCClusterH;      // Anti MC cluster 
ooHandle(ooContObj)  contCTCMCClusterH;       // CTC MC cluster 
ooHandle(ooContObj)  contmceventgH;           // MC Event
ooHandle(ooContObj)  contMCClusterH;          // MC cluster 
ooHandle(ooContObj)  contTOFMCClusterH;       // TOF MC cluster 
ooHandle(ooContObj)  conttriggerlvl1H;       //  Level1 trigger 
ooHandle(ooContObj)  conttriggerlvl3H;       // Level3 trigger 

// ooObj classes
ooHandle(AMSAntiMCClusterD) AntiMCClusterH;
ooHandle(AMSCTCMCClusterD)  CTCMCClusterH;
ooHandle(AMSmceventgD)      mceventgH;
ooHandle(AMSTOFMCClusterV)  tofmcclusterH;
ooHandle(AMSTrMCClusterV)   trmcclusterH;
ooHandle(Triggerlvl1D)    triggerlvl1H;
ooHandle(Triggerlvl3D)    triggerlvl3H;

// Iterators
ooItr(AMSAntiMCClusterD) AntiMCClusterItr;        // Anti MC cluster
ooItr(AMSCTCMCClusterD)  CTCMCClusterItr;         // CTC MC cluster
ooItr(AMSmceventgD)      mceventgItr;             // mc event
ooItr(AMSTOFMCClusterV)  tofmcclusterItr;         // TOF MC cluster
ooItr(AMSTrMCClusterV)    trmcclusterItr;          // MC cluster
ooItr(Triggerlvl1D)     triggerlvl1Itr;       // for trigger1   
ooItr(Triggerlvl3D)     triggerlvl3Itr;       // for trigger3

static integer mcevent_size;

AMSMCEventList::AMSMCEventList(char* listname, char* prefix)
{
  setlistname(listname);
  resetCounters();
  resetMCCounters();
  SetContainersNames(prefix);
}

AMSMCEventList::AMSMCEventList(char* listname, char* setup, char* prefix)
{
  setlistname(listname);
  setsetupname(setup);
  resetCounters();
  resetMCCounters();
  SetContainersNames(prefix);
}

void AMSMCEventList::resetMCCounters()
{
}

void AMSMCEventList::SetContainersNames(char* prefix)
{
//mceventg
  if (!mceventgCont) mceventgCont =  StrCat("mceventg_",prefix);
//AntiMCCluster
   if (!antimcclusterCont) antimcclusterCont = StrCat("AntiMCCluster_",prefix);
//CTCMCCluster
   if (!ctcmcclusterCont) ctcmcclusterCont =  StrCat("CTCMCCluster_",prefix);
//TOFMCCluster
   if (!tofmcclusterCont) tofmcclusterCont =  StrCat("TOFMCCluster_",prefix);
//trMCCluster
   if (!trmcclusterCont) trmcclusterCont =  StrCat("TrMCCluster_",prefix);
//trigger1
   if (!triggerlvl1Cont) triggerlvl1Cont =  StrCat("Triggerlvl1_",prefix);
//trigger3
   if (!triggerlvl3Cont) triggerlvl3Cont =  StrCat("Triggerlvl3_",prefix);
}


ooStatus      AMSMCEventList::Addmceventg(ooHandle(AMSmcevent)& eventH)
{
  	ooStatus	          rstatus = oocSuccess;	// Return status
        ooHandle(AMSMCEventList)  listH = ooThis();

        if (contmceventgH == NULL) {
         rstatus = CheckContainer(mceventgCont, oocUpdate, contmceventgH);
         if (rstatus != oocSuccess) return rstatus;
        }

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

        // get first cluster
         AMSCTCMCCluster* p = (AMSCTCMCCluster*)AMSEvent::gethead() -> 
                                            getheadC("AMSCTCMCCluster",0);
        if (p == NULL && dbg_prtout) {
         Message("AddCTCMCCluster : AMSCTCMCCluster* p == NULL");
         return oocSuccess;
        }
        while ( p != NULL) {
         CTCMCClusterH = new(contCTCMCClusterH) AMSCTCMCClusterD(p);
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
         rstatus = eventH -> add_pAntiMCCluster(AntiMCClusterH);
         if (rstatus != oocSuccess) {
          Error("AddAntiMCCluster: cannot set the MCCluster to Event assoc.");
          return rstatus;
         }
         p = p -> next();
       }
        return rstatus;
}

ooStatus AMSMCEventList::Addtriggerlvl1(ooHandle(AMSmcevent) & eventH)
{
        ooStatus                rstatus = oocSuccess;   // Return status
        ooHandle(AMSMCEventList)  listH = ooThis();

        if (conttriggerlvl1H == NULL) {
         rstatus = CheckContainer(
                            triggerlvl1Cont, oocUpdate, conttriggerlvl1H);
         if (rstatus != oocSuccess) return rstatus;
        }

        // get triggerlvl1 pointer 
        TriggerLVL1* p = (TriggerLVL1*)AMSEvent::gethead() ->
                                           getheadC("TriggerLVL1",0);
        if (p == NULL && dbg_prtout == 1)  {
//        if (p == NULL )  {
         Message("AddtriggerLVL1 : TriggerLVL1* p == NULL");
         return oocSuccess;
        }
//        while ( p != NULL) {
         if ( p != NULL) {
         triggerlvl1H = new(conttriggerlvl1H) Triggerlvl1D(p);
         rstatus = eventH -> set_pTriggerlvl1(triggerlvl1H);
         if (rstatus != oocSuccess) {
          Error("Addtriggerlvl1: cannot set the Triggerlvl1 to Event assoc.");
          return rstatus;
         }
//         p = p -> next();
       }
        return rstatus;
}
//
ooStatus AMSMCEventList::Addtriggerlvl3(ooHandle(AMSmcevent) & eventH)
{
        ooStatus                rstatus = oocSuccess;   // Return status
        ooHandle(AMSMCEventList)  listH = ooThis();

        if (conttriggerlvl3H == NULL) {
         rstatus = CheckContainer(
                            triggerlvl3Cont, oocUpdate, conttriggerlvl3H);
         if (rstatus != oocSuccess) return rstatus;
        }

        // get triggerlvl3 pointer
        TriggerLVL3* p = (TriggerLVL3*)AMSEvent::gethead() ->
                                           getheadC("TriggerLVL3",0);
        if (p == NULL && dbg_prtout == 1)  {
         Message("AddtriggerLVL3 : TriggerLVL3* p == NULL");
         return oocSuccess;
        }
         if ( p != NULL) {
         triggerlvl3H = new(conttriggerlvl3H) Triggerlvl3D(p);
         rstatus = eventH -> set_pTriggerlvl3(triggerlvl3H);
         if (rstatus != oocSuccess) {
          Error("Addtriggerlvl3: cannot set the Triggerlvl3 to Event assoc.");
          return rstatus;
         }
                        }
        return rstatus;
}

//

ooStatus AMSMCEventList::Readtriggerlvl1(ooHandle(AMSmcevent) & eventH)
{
        ooStatus                rstatus = oocSuccess;   // Return status
        ooHandle(AMSMCEventList) listH = ooThis();

        ooMode mode = oocRead;

        if (conttriggerlvl1H == NULL) {
         rstatus =
          CheckContainer(triggerlvl1Cont, mode, conttriggerlvl1H);
          if (rstatus != oocSuccess) return rstatus;
        }
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

