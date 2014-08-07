//  $Id$
// Author V. Choutko 4-june-1996
// 31.07.98 E.Choumilov. Cluster Time recovering(for 1-sided counters) added.
//
#include "tofdbc02.h"
#include "tofrec02.h"
#include "beta.h"
#include "commons.h"
#include <math.h>
#include <limits.h>
#include "amsgobj.h"
#include "extC.h"
#include "upool.h"
#include "ntuple.h"
#include "trigger302.h"
#include "event.h"
#include "trdrec.h"
#include "HistoMan.h"
#ifdef _PGTRACK_
#include "tkdcards.h"
#include "tkpatt.h"
#include "TrRecon.h"
#endif

extern "C" void rzerowrapper_(number & z0, number & zb, number & x0, number & zmin,int & ierr);

integer AMSBeta::patconf[npatb][4]={  1,2,3,4,        // 1234  0
                                      1,2,3,0,        // 123   1
                                      1,2,4,0,        // 124   2
                                      1,3,4,0,        // 134   3
                                      2,3,4,0,        // 234   4
                                      1,3,0,0,        // 13    5
                                      1,4,0,0,        // 14    6
                                      2,3,0,0,        // 23    7
                                      2,4,0,0,        // 24    8
                                      1,2,0,0,        // 12    9
                                      3,4,0,0};       // 34   10    
integer AMSBeta::patpoints[npatb]={4,3,3,3,3,2,2,2,2,2,2};


#ifdef _PGTRACK_
#define PZDEBUG 0 
integer AMSBeta::build(integer refit){
  int master_new=1;
  int master_old=0;

  bool new_build  = ((BETAFITFFKEY.OldNew%10)&0x1)>0;
  bool old_build  = ((BETAFITFFKEY.OldNew%10)&0x2)>0;

  if(BETAFITFFKEY.OldNew/10 >0|| (!new_build && old_build)) {master_new=0; master_old=1;}

  int built=0;
  int bfound=0;
  if(old_build)  built=build_old (refit, master_old);
  if(new_build){

    //check if a good TrTrack exists
    int nTrtracks=AMSEvent::gethead()->getC(AMSID("AMSTrTrack",0))->getnelem();
    
    if(nTrtracks>0){ // Build Beta from a real TrTrack!!
      AMSTrTrack *ptrack=(AMSTrTrack*)AMSEvent::gethead()->getheadC("AMSTrTrack",0,1);
      for ( ; ptrack ; ptrack=ptrack->next()) {   
	bfound+=BuildBeta(ptrack,master_new);
      }
      if(PZDEBUG)    printf("Reconstructed %d Beta from tracks!\n",bfound);
    }
  
    // if no beta found with TrTracks try with (extrapolated) TRD tracks
    AMSContainer *ccont=AMSEvent::gethead()->getC(AMSID("AMSTRDTrack",0));
    if(!bfound && ccont && ccont->getnelem()>0){
      for ( AMSTRDTrack *ptrackT=(AMSTRDTrack*)AMSEvent::gethead()->getheadC("AMSTRDTrack",0,1)
	      ; ptrackT ; ptrackT=ptrackT->next()) {
	if(!BETAFITFFKEY.FullReco && ptrackT->checkstatus(AMSDBc::USED)) continue;
      
	// Make False Tr Track from TRD-Track       
	AMSTrTrack *ptrack = new AMSTrTrack(ptrackT->gettheta(),ptrackT->getphi(), ptrackT->getcoo());
	ptrack->setstatus(AMSDBc::TRDTRACK); 
	bfound+=BuildBeta(ptrack,master_new);
	if(PZDEBUG)        printf("Reconstructed %d Beta from TRD track!\n",bfound);
      }
    }
  
    // Try build beat w/o a track if LVL3 is OK
    if(!bfound){

      AMSTrTrack * ptrack=FindFalseTrackForBeta(refit);
      if(ptrack) bfound+=BuildBeta(ptrack,master_new);
      if(PZDEBUG)       printf("Reconstructed Beta W/O track %d Beta!\n",bfound);
    }
    if(PZDEBUG)   printf("Reconstructed total %d Beta!\n",bfound);
  }

  if(master_old && old_build )
    return built;
  else 
    return bfound;
}




AMSTrTrack * AMSBeta::FindFalseTrackForBeta(int refit){
  // Pattern recognition + fit

 
  // TEMP LVL3 is not well defined  
  // SKIP THIS FOR THE MOMENT
  // Now get Beta without track if and only if lvl3 is ok 
  //   TriggerLVL302 *ptr=(TriggerLVL302*)AMSEvent::gethead()->getheadC("TriggerLVL3",0);
  //   AMSTrTrack * ptrackF=0;
  //   if(ptr && ptr->TOFOK() && LVL3FFKEY.Accept==0){
  AMSTrTrack* ptrackF=0;
  // Loop on TOF patterns
  for ( int pat=0; pat<npatb; pat++){
    AMSTOFCluster * phit[4]={0,0,0,0};
    if(BETAFITFFKEY.pattern[pat]){
      int fp=patpoints[pat]-1;    
      // Try to make StrLine Fit
      integer first=patconf[pat][0]-1;
      integer second=patconf[pat][fp]-1;
      phit[0]=AMSTOFCluster::gethead(first);
      while( phit[0]){
	if(!phit[0]->checkstatus(AMSDBc::BAD)){
	  phit[fp]=AMSTOFCluster::gethead(second);
	  while( phit[fp]){
	    if(!phit[fp]->checkstatus(AMSDBc::BAD)){
	      number dist=(phit[fp]->getcoo()).dist(phit[0]->getcoo());
	      number u=(phit[fp]-> getcoo()[0]-phit[0]-> getcoo()[0])/dist;
	      number v=(phit[fp]-> getcoo()[1]-phit[0]-> getcoo()[1])/dist;
	      number w=(phit[fp]-> getcoo()[2]-phit[0]-> getcoo()[2])/dist;
	      AMSDir dir(u,v,w);
	      ptrackF=new AMSTrTrack(dir,phit[0]->getcoo());
	      ptrackF->setstatus(AMSDBc::NOTRACK);
	      //track found, go out
	      return ptrackF;
	    }
	    phit[fp]=phit[fp]->next(); 
	  }
	}
	phit[0]=phit[0]->next(); 
      }
    }
  }
  //  }
  return 0;
}

class mtof_hit{
public:
  AMSTOFCluster * phit;
  AMSPoint min_d;
  number sleng;
  bool Xmatch;
  bool Ymatch;
  bool Tmatch;
  bool Lmatch;
  AMSPoint checker;
  mtof_hit():phit(0),min_d(AMSPoint(0,0,0)),sleng(0),
	     Xmatch(false),Ymatch(false),Tmatch(false),Lmatch(false)
  {
  //                    Transversal              Longitudinal  
    checker=AMSPoint(BETAFITFFKEY.SearchReg[0], BETAFITFFKEY.SearchReg[1],
	     BETAFITFFKEY.SearchReg[2]);  //dummy
  }
  mtof_hit(mtof_hit &orig):phit(orig.phit),min_d(orig.min_d),sleng(orig.sleng),
			   Xmatch(orig.Xmatch),Ymatch(orig.Ymatch),
			   Tmatch(orig.Tmatch),Lmatch(orig.Lmatch),checker(orig.checker){}
  ~mtof_hit(){phit=0;}
  void MatchCheck();
  number distT() {return (phit->getplane()==1 ||phit->getplane()==4)? min_d[1]:min_d[0];}
}; 

void mtof_hit::MatchCheck(){
  if(!phit) return;
  bool YY=(phit->getplane()==1 ||phit->getplane()==4);
  if (YY){
    Xmatch= (  min_d[0]   <=  checker[1]*phit->getnmemb());
    Ymatch= (  min_d[1]   <=  checker[0]*phit->getnmemb());
    Tmatch=Ymatch; 
    Lmatch=Xmatch;
  }
  else   { 
    Xmatch= (  min_d[0]   <=  checker[0]*phit->getnmemb());
    Ymatch= (  min_d[1]   <=  checker[1]*phit->getnmemb());    
    Tmatch=Xmatch; 
    Lmatch=Ymatch;
  }
}


int FindCloserTOF(TrTrackR* ptrack,mtof_hit* tmhit){

  // 0 -- Search for the geometrically matching TOF Hits  
  for (int TOFlay=0;TOFlay<4;TOFlay++){
	(void) AMSEvent::gethead()->getC(AMSID("AMSTOFCluster",TOFlay))->getnelem();
    tmhit[TOFlay].min_d= AMSPoint(1000,1000,1000);

    // 1 -- FIND on each TOF layer the hit closer to the track
    for ( AMSTOFCluster * tofhit=AMSTOFCluster::gethead(TOFlay) ; tofhit; tofhit=tofhit->next()) {
      number td, ssleng;
      if(tofhit->checkstatus(AMSDBc::BAD)) continue;
      // it is a normalized distance with respect the TOF incertitude,
      // sleng is signed coo along the track
      AMSPoint dst=AMSBeta::Distance(tofhit->getcoo(),tofhit->getecoo(),
				     (AMSTrTrack*)ptrack,ssleng,td);
      
      // search the closest along Y only
      if (fabs(dst[1]) <= tmhit[TOFlay].min_d[1]){
	//cout<<" The cluster "<<clnum-1<<" is good"<<endl; 
	tmhit[TOFlay].min_d = dst.abs(); 
	tmhit[TOFlay].phit  = tofhit;
	tmhit[TOFlay].sleng = ssleng;
      }
    }
    // 2 -- CHECK if the hit is at a resonable distance if the selected hit has a resonable distance count it!
    tmhit[TOFlay].MatchCheck();
  }

  return 0;
}


bool TkTOFMatch(TrTrackR* tr,int select_tag=0);
bool TkTOFMatch(TrTrackR* ptrack,int select_tag) {
  mtof_hit tofhit[4];  
  FindCloserTOF( ptrack,tofhit);
  bool g_YMatch=tofhit[0].Ymatch && tofhit[3].Ymatch;
  bool g_XMatch=tofhit[1].Xmatch || tofhit[2].Xmatch;
  if ( ( (g_YMatch)&&(!g_XMatch) )||
       ( (g_YMatch)&&(log10(ptrack->GetNormChisqX(ptrack->Gettrdefaultfit()))>TRCLFFKEY.logChisqXmax) ) 
     ) { // try to move 

    AMSPoint p0(tofhit[0].phit->getcoo());
    AMSPoint p1(tofhit[3].phit->getcoo());
    AMSDir dir(p0-p1);
    return TrRecon::MoveTrTrack(ptrack, p0, dir,  3.8, select_tag);

//     // Try to move TrTrack along X
//     int left = 0, right = 0;
//     ptrack->GetMaxShift(left, right);
    
//     int moved = 0;
//     for (int mm = left; mm <= right; mm++){
//       if (mm == 0) continue;      
//       TrTrackR test(*ptrack);
//       test.Move(mm, mfit);
//       mtof_hit tofhit_test[4];
//       FindCloserTOF(&test,tofhit);
//       bool g_XMatch=tofhit[1].Xmatch && tofhit[2].Xmatch;
//       if(g_XMatch) {moved=mm; break;}
//     }
//     if (moved != 0){
//       ptrack->Move(moved);
//       ptrack->EstimateDummyX();
//       return true;
//     }
  }
  return false;

}


int AMSBeta::BuildBeta(AMSTrTrack* ptrack, integer Master){
  
  int bfound=0;
  AMSTOFCluster * phit[4]={0,0,0,0};
  number chi2space=0;
  integer tofpatt=0;

  mtof_hit tofhit[4];

  // 1 -- FIND on each TOF layer the hit closer to the track
  // 2 -- CHECK if the hit is at a resonable distance, if the selected hit has a resonable distance count it!
  FindCloserTOF( ptrack,tofhit);

  // 3 -- Select the matched hit for Beta
  mtof_hit tofhit2[4];
  int idx=0;
  for (int ii=0;ii<4;ii++){
    if(tofhit[ii].Tmatch){ //good space match
      
      // 3.1 -- Check if the time(that is the long coo) is consistent with the track passing point
      int status=tofhit[ii].phit->getstatus();
      if((status&TOFGC::SCBADB2)!=0 &&  (status&TOFGC::SCBADB5)!=0){    //tempor  use now only TOF-recovered
	tofhit[ii].phit->recovers2(ptrack);
	tofhit[ii].MatchCheck();
      }
      if(tofhit[ii].Lmatch){ // time determination compatible with the track position
	tofpatt|=(1<<ii);
	chi2space+=pow(tofhit[ii].distT(),2);
	tofhit2[idx++]=tofhit[ii];
      }
    }
  }
  
  // Loop on TOF patterns
  int sel_patt=-1;
  for ( int patb=0; patb<npatb; patb++){
    if(!BETAFITFFKEY.pattern[patb]) continue;
    int testpatt=0x1e&(
		       (1<<patconf[patb][0])|
		       (1<<patconf[patb][1])|
		       (1<<patconf[patb][2])|
		       (1<<patconf[patb][3]));
    if (((tofpatt<<1)&0x1e)==(testpatt&0x1e)){
      sel_patt=patb;
      break;
    }
  }
  number sleng[4]={0,0,0,0};
  for (int ii=0;ii<idx;ii++) { phit[ii]=tofhit2[ii].phit; sleng[ii]=tofhit2[ii].sleng;}

  if(sel_patt>=0&& sel_patt<9)  
    if(AMSBeta::_addnextP(sel_patt,patpoints[sel_patt],sleng,phit,ptrack,chi2space,Master))
      bfound++;
  
  return bfound;
  
}



//====================================================================================
//====================================================================================
// Get the distance between track and TOF normalized to the error
AMSPoint AMSBeta::Distance(AMSPoint coo, AMSPoint ecoo, AMSTrTrack *ptr,
			   number & sleng, number & theta){
  AMSDir dir(0,0,1.);
  AMSPoint outp;
  number phi;
  ptr->interpolate(coo,dir,outp,theta,phi,sleng);
  if(ptr->IsFake()){
    ecoo[0]+=1.;  // add one cm fot trd/tof
    ecoo[1]+=1.;  // add one cm fot trd/tof
  }
  outp=((outp-coo)/ecoo).abs();
  return outp;
}



integer AMSBeta::build_old(integer refit,integer Master){
  
  // Pattern recognition + fit
  AMSPoint SearchReg(BETAFITFFKEY.SearchReg[0],BETAFITFFKEY.SearchReg[1],
		     BETAFITFFKEY.SearchReg[2]);
  
  // Decide the order here
  // Sonly = 0 => vertex tracks (TOFFORGAMMA)
  // Sonly = 1 => tracks with K hits (!FalseTOFX && !TOFFORGAMMA)
  // Sonly = 2 => tracks without K hits (FalseTOFX && !TOFFORGAMMA)
  
  int bfound=0;
  for( int Sonly=0; Sonly<3; Sonly++){
    
    
    // Loop on TOF patterns
    for ( int patb=0; patb<npatb; patb++){
      if(!BETAFITFFKEY.pattern[patb]) continue;
      number td;
      

      // Loop on track patterns
      
      // Loop on tracks
      AMSTrTrack *ptrack=(AMSTrTrack*)AMSEvent::gethead()->getheadC("AMSTrTrack",0,1);
      for ( ; ptrack ; ptrack=ptrack->next()) {
	
	if (Sonly==0) {
	  if (!ptrack->checkstatus(AMSDBc::TOFFORGAMMA)) continue;
	} else if (Sonly==1) {
	  if (ptrack->checkstatus(AMSDBc::TOFFORGAMMA)) continue;
	  if (ptrack->checkstatus(AMSDBc::FalseTOFX)) continue;
	} else if (Sonly==2) {
	  if (ptrack->checkstatus(AMSDBc::TOFFORGAMMA)) continue;
	  if (!ptrack->checkstatus(AMSDBc::FalseTOFX)) continue;
	}
       
	if(        !BETAFITFFKEY.FullReco 
		   && ptrack->checkstatus(AMSDBc::USED)
		   && ptrack->checkstatus(AMSDBc::TOFFORGAMMA)==0) continue;
	AMSTOFCluster * phit[4]={0,0,0,0};
	number sleng[4];
       
	// Loop on first TOF plane
	phit[0]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][0]-1);
	for ( ; phit[0]; phit[0]=phit[0]->next()) {
	  number chi2space=0;
	  if(phit[0]->checkstatus(AMSDBc::BAD)&& BadBetaAlreadyExists(patb)) continue;
	  if (!BETAFITFFKEY.FullReco && (!ptrack->checkstatus(AMSDBc::FalseTOFX)|| ptrack->checkstatus(AMSDBc::RELEASED))) {
	    if (phit[0]->checkstatus(AMSDBc::USED)) continue;
	  }
	  AMSPoint dst=AMSBeta::Distance(phit[0]->getcoo(),phit[0]->getecoo(),
					 ptrack,sleng[0],td);

	  if (!(dst<=SearchReg*phit[0]->getnmemb())) continue;
	  chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);

	  // Loop on second TOF plane
	  phit[1]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][1]-1);
	  for ( ; phit[1]; phit[1]=phit[1]->next()) {
	    if(phit[1]->checkstatus(AMSDBc::BAD)&& BadBetaAlreadyExists(patb)) continue;
	    if (!BETAFITFFKEY.FullReco && (!ptrack->checkstatus(AMSDBc::FalseTOFX)|| ptrack->checkstatus(AMSDBc::RELEASED))) {
	      if (phit[1]->checkstatus(AMSDBc::USED)) continue;
	    }
	    AMSPoint dst=AMSBeta::Distance(phit[1]->getcoo(),phit[1]->getecoo(),
					   ptrack,sleng[1],td);
	    if (!(dst<=SearchReg*phit[1]->getnmemb())) continue;
	    chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);
	    // 2-point combination found
	    if(AMSBeta::patpoints[patb]==2){
	      if(AMSBeta::_addnext(patb,2,sleng,phit,ptrack,td,chi2space,Master)){
		bfound++;
		goto nexttrack;
	      }
	      continue;
	    }
	   
	    // Loop on third TOF plane
	    phit[2]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][2]-1);
	    for ( ; phit[2]; phit[2]=phit[2]->next()) {
	      if(phit[2]->checkstatus(AMSDBc::BAD)&& BadBetaAlreadyExists(patb)) continue;
	      if (!BETAFITFFKEY.FullReco && (!ptrack->checkstatus(AMSDBc::FalseTOFX)|| ptrack->checkstatus(AMSDBc::RELEASED))) {
		if (phit[2]->checkstatus(AMSDBc::USED)) continue;
	      }
	      AMSPoint dst=AMSBeta::Distance(phit[2]->getcoo(),phit[2]->
					     getecoo(),ptrack,sleng[2],td);
	      if(!(dst<=SearchReg*phit[2]->getnmemb())) continue;
	      chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);
	      // 3-point combination found
	      if(AMSBeta::patpoints[patb]==3){
		if(AMSBeta::_addnext(patb,3,sleng,phit,ptrack,td,chi2space,Master)){
		  bfound++;
		  goto nexttrack;
		}
		continue;
	      }
	     
	      // Loop on fourth TOF plane
	      phit[3]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][3]-1);
	      for ( ; phit[3]; phit[3]=phit[3]->next()) {
		if(phit[3]->checkstatus(AMSDBc::BAD)&& BadBetaAlreadyExists(patb)) continue;
		if (!BETAFITFFKEY.FullReco && (!ptrack->checkstatus(AMSDBc::FalseTOFX)|| ptrack->checkstatus(AMSDBc::RELEASED))){
		  if (phit[3]->checkstatus(AMSDBc::USED)) continue;
		}
		AMSPoint dst=AMSBeta::Distance(phit[3]->getcoo(),phit[3]->
					       getecoo(),ptrack,sleng[3],td);
		if(!(dst<=SearchReg*phit[3]->getnmemb())) continue;
		chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);
		// 4-point combination found
		if(AMSBeta::_addnext(patb,4,sleng,phit,ptrack,td,chi2space,Master)){
		  bfound++;
		  goto nexttrack;
		}
		continue;
	      }
	    }
	  }
	}
      nexttrack: 
	continue;
      }
    }
    if(bfound && Sonly==1)break; 
  }
  
  
  {
    
    // get beta from TRD info (eventually if TriggerLVL302 is ok)
    
    if( !bfound ){
      // Loop on TOF patterns
      for ( int patb=0; patb<npatb; patb++){
	if(!BETAFITFFKEY.pattern[patb]) continue;
	number td;
	
	
	// Loop on track patterns
	
	// Loop on tracks
	AMSTRDTrack *ptrackT=(AMSTRDTrack*)AMSEvent::gethead()->getheadC("AMSTRDTrack",0,1);
	for ( ; ptrackT ; ptrackT=ptrackT->next()) {
	  
	  if(!BETAFITFFKEY.FullReco && ptrackT->checkstatus(AMSDBc::USED)) continue;
	  AMSTOFCluster * phit[4]={0,0,0,0};
	  number sleng[4];
	  // Make False Track       
	  AMSTrTrack *ptrack = new AMSTrTrack(ptrackT->gettheta(),ptrackT->getphi(), ptrackT->getcoo());
	  ptrack->setstatus(AMSDBc::TRDTRACK); 
	  
	  
	  // Loop on first TOF plane
	  phit[0]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][0]-1);
	  for ( ; phit[0]; phit[0]=phit[0]->next()) {
	    number chi2space=0;
	    if(phit[0]->checkstatus(AMSDBc::BAD)&& BadBetaAlreadyExists(patb)) continue;
	    if (!BETAFITFFKEY.FullReco ) {
	      if (phit[0]->checkstatus(AMSDBc::USED)) continue;
	    }
	    AMSPoint dst=AMSBeta::Distance(phit[0]->getcoo(),phit[0]->getecoo(),
					   ptrack,sleng[0],td);
	    if (!(dst<=SearchReg*phit[0]->getnmemb())) continue;
	    chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);
	    
	    // Loop on second TOF plane
	    phit[1]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][1]-1);
	    for ( ; phit[1]; phit[1]=phit[1]->next()) {
	      if(phit[1]->checkstatus(AMSDBc::BAD)&& BadBetaAlreadyExists(patb)) continue;
	      if (!BETAFITFFKEY.FullReco) {
		if (phit[1]->checkstatus(AMSDBc::USED)) continue;
	      }
	      AMSPoint dst=AMSBeta::Distance(phit[1]->getcoo(),phit[1]->getecoo(),
					     ptrack,sleng[1],td);
	      if (!(dst<=SearchReg*phit[1]->getnmemb())) continue;
	      chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);
	      // 2-point combination found
	      if(AMSBeta::patpoints[patb]==2){
		if(AMSBeta::_addnext(patb,2,sleng,phit,ptrack,chi2space,Master)){
		  bfound++;
		  ptrackT->setstatus(AMSDBc::USED);
		  ptrack->setstatus(AMSDBc::USED);
		  
		  goto nexttrack2;
		}
		continue;
	      }
	      
	      // Loop on third TOF plane
	      phit[2]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][2]-1);
	      for ( ; phit[2]; phit[2]=phit[2]->next()) {
		if(phit[2]->checkstatus(AMSDBc::BAD)&& BadBetaAlreadyExists(patb)) continue;
		if (!BETAFITFFKEY.FullReco ) {
		  if (phit[2]->checkstatus(AMSDBc::USED)) continue;
		}
		AMSPoint dst=AMSBeta::Distance(phit[2]->getcoo(),phit[2]->
					       getecoo(),ptrack,sleng[2],td);
		if(!(dst<=SearchReg*phit[2]->getnmemb())) continue;
		chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);
		// 3-point combination found
		if(AMSBeta::patpoints[patb]==3){
		  if(AMSBeta::_addnext(patb,3,sleng,phit,ptrack,chi2space,Master)){
		  bfound++;
		    ptrackT->setstatus(AMSDBc::USED);
		    ptrack->setstatus(AMSDBc::USED);
		    goto nexttrack2;
		  }
		  continue;
		}
		
		// Loop on fourth TOF plane
		phit[3]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][3]-1);
		for ( ; phit[3]; phit[3]=phit[3]->next()) {
		  if(phit[3]->checkstatus(AMSDBc::BAD)&& BadBetaAlreadyExists(patb)) continue;
		  if (!BETAFITFFKEY.FullReco ){
		    if (phit[3]->checkstatus(AMSDBc::USED)) continue;
		  }
		  AMSPoint dst=AMSBeta::Distance(phit[3]->getcoo(),phit[3]->
						 getecoo(),ptrack,sleng[3],td);
		  if(!(dst<=SearchReg*phit[3]->getnmemb())) continue;
		  chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);
		  // 4-point combination found
		  if(AMSBeta::_addnext(patb,4,sleng,phit,ptrack,chi2space,Master)){
		  bfound++;
		    ptrackT->setstatus(AMSDBc::USED);
		    ptrack->setstatus(AMSDBc::USED);
		    goto nexttrack2;
		  }
		  continue;
		}
	      }
	    }
	  }
	  delete ptrack;
	nexttrack2: 
	  continue;
	}
      }
    }
  }
  

  // Now get Beta without track if and only if lvl3 is ok


  // Loop on TOF patterns
  (void) AMSEvent::gethead()->getheadC("TriggerLVL3",0);
  AMSTrTrack * ptrackF=0;
//  if(1 || (ptr && ptr->TOFOK() && LVL3FFKEY.Accept==0)){
    if(!bfound){
    for ( int pat=0; pat<npatb; pat++){
      AMSTOFCluster * phit[4]={0,0,0,0};
      if(BETAFITFFKEY.pattern[pat]){
	int fp=patpoints[pat]-1;    
	// Try to make StrLine Fit
	integer first=patconf[pat][0]-1;
	integer second=patconf[pat][fp]-1;
	phit[0]=AMSTOFCluster::gethead(first);
	while( phit[0]){
	  if(!phit[0]->checkstatus(AMSDBc::BAD)){
	    phit[fp]=AMSTOFCluster::gethead(second);
	    while( phit[fp]){
	      if(!phit[fp]->checkstatus(AMSDBc::BAD)){
		number dist=(phit[fp]->getcoo()).dist(phit[0]->getcoo());
		number u=(phit[fp]-> getcoo()[0]-phit[0]-> getcoo()[0])/dist;
		number v=(phit[fp]-> getcoo()[1]-phit[0]-> getcoo()[1])/dist;
		number w=(phit[fp]-> getcoo()[2]-phit[0]-> getcoo()[2])/dist;
		AMSDir dir(u,v,w);
		ptrackF=new AMSTrTrack(dir,phit[0]->getcoo());
		ptrackF->setstatus(AMSDBc::NOTRACK);
		//track found, go out
		goto found;
	      }
	      phit[fp]=phit[fp]->next(); 
	    }
	  }
	  phit[0]=phit[0]->next(); 
	}
      }
    }
  }
 found:
  if(ptrackF){

    // Loop on TOF patterns
    for ( int patb=0; patb<npatb; patb++){
      if(!BETAFITFFKEY.pattern[patb]) continue;
      number td;



      // Loop on tracks
      AMSTrTrack *ptrack=ptrackF;

      AMSTOFCluster * phit[4]={0,0,0,0};
      number sleng[4];

      // Loop on first TOF plane
      phit[0]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][0]-1);
      for ( ; phit[0]; phit[0]=phit[0]->next()) {
	number chi2space=0;
	if(phit[0]->checkstatus(AMSDBc::BAD)) continue;
	AMSPoint dst=AMSBeta::Distance(phit[0]->getcoo(),phit[0]->getecoo(),
				       ptrack,sleng[0],td);
	if (!(dst<=SearchReg*phit[0]->getnmemb())) continue;
	chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);

	// Loop on second TOF plane
	phit[1]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][1]-1);
	for ( ; phit[1]; phit[1]=phit[1]->next()) {
	  if(phit[1]->checkstatus(AMSDBc::BAD)) continue;
	  AMSPoint dst=AMSBeta::Distance(phit[1]->getcoo(),phit[1]->getecoo(),
					 ptrack,sleng[1],td);
	  if (!(dst<=SearchReg*phit[1]->getnmemb())) continue;
	  chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);
	  // 2-point combination found
	  if(AMSBeta::patpoints[patb]==2){
	    if(AMSBeta::_addnext(patb,2,sleng,phit,ptrack,chi2space,Master))
	      return 1;
	    continue;
	  }

	  // Loop on third TOF plane
	  phit[2]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][2]-1);
	  for ( ; phit[2]; phit[2]=phit[2]->next()) {
	    if(phit[2]->checkstatus(AMSDBc::BAD)) continue;
	    AMSPoint dst=AMSBeta::Distance(phit[2]->getcoo(),phit[2]->
					   getecoo(),ptrack,sleng[2],td);
	    if(!(dst<=SearchReg*phit[2]->getnmemb())) continue;
	    chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);
	    // 3-point combination found
	    if(AMSBeta::patpoints[patb]==3){
	      if(AMSBeta::_addnext(patb,3,sleng,phit,ptrack,chi2space,Master))
		return 1;
	      continue;
	    }

	    // Loop on fourth TOF plane
	    phit[3]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][3]-1);
	    for ( ; phit[3]; phit[3]=phit[3]->next()) {
	      if(phit[3]->checkstatus(AMSDBc::BAD)) continue;
	      AMSPoint dst=AMSBeta::Distance(phit[3]->getcoo(),phit[3]->
					     getecoo(),ptrack,sleng[3],td);
	      if(!(dst<=SearchReg*phit[3]->getnmemb())) continue;
	      chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);
	      // 4-point combination found
	      if(AMSBeta::_addnext(patb,4,sleng,phit,ptrack,chi2space,Master))
		return 1;
	      continue;
	    }
	  }
	}
      }
    }



  }
  // no beta found;
  delete ptrackF;
  return 1;
}


//====================================================================================
//====================================================================================
//====================================================================================
#else  // STD GBATCH
//====================================================================================
//====================================================================================
//====================================================================================


integer AMSBeta::build(integer refit){

// Pattern recognition + fit
 AMSPoint SearchReg(BETAFITFFKEY.SearchReg[0],BETAFITFFKEY.SearchReg[1],
 BETAFITFFKEY.SearchReg[2]);

// Decide the order here
// Sonly = 0 => vertex tracks (TOFFORGAMMA)
// Sonly = 1 => tracks with K hits (!FalseTOFX && !TOFFORGAMMA)
// Sonly = 2 => tracks without K hits (FalseTOFX && !TOFFORGAMMA)

 int bfound=0;
  for( int Sonly=0; Sonly<3; Sonly++){


// Loop on TOF patterns
  for ( int patb=0; patb<npatb; patb++){
   if(!BETAFITFFKEY.pattern[patb]) continue;
   number td;


// Loop on track patterns

// Loop on tracks
     AMSTrTrack *ptrack=(AMSTrTrack*)AMSEvent::gethead()->getheadC("AMSTrTrack",0,1);
     for ( ; ptrack ; ptrack=ptrack->next()) {

       if (Sonly==0) {
            if (!ptrack->checkstatus(AMSDBc::TOFFORGAMMA)) continue;
       } else if (Sonly==1) {
            if (ptrack->checkstatus(AMSDBc::TOFFORGAMMA)) continue;
            if (ptrack->checkstatus(AMSDBc::FalseTOFX)) continue;
       } else if (Sonly==2) {
            if (ptrack->checkstatus(AMSDBc::TOFFORGAMMA)) continue;
            if (!ptrack->checkstatus(AMSDBc::FalseTOFX)) continue;
       }

       if(        !BETAFITFFKEY.FullReco 
               && ptrack->checkstatus(AMSDBc::USED)
               && ptrack->checkstatus(AMSDBc::TOFFORGAMMA)==0) continue;
       AMSTOFCluster * phit[4]={0,0,0,0};
       number sleng[4];

// Loop on first TOF plane
       phit[0]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][0]-1);
       for ( ; phit[0]; phit[0]=phit[0]->next()) {
         number chi2space=0;
         if(phit[0]->checkstatus(AMSDBc::BAD)&& BadBetaAlreadyExists(patb)) continue;
         if (!BETAFITFFKEY.FullReco && (!ptrack->checkstatus(AMSDBc::FalseTOFX)|| ptrack->checkstatus(AMSDBc::RELEASED))) {
           if (phit[0]->checkstatus(AMSDBc::USED)) continue;
         }
         AMSPoint dst=AMSBeta::Distance(phit[0]->getcoo(),phit[0]->getecoo(),
         ptrack,sleng[0],td);

         if (!(dst<=SearchReg*phit[0]->getnmemb())) continue;
         chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);

// Loop on second TOF plane
         phit[1]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][1]-1);
         for ( ; phit[1]; phit[1]=phit[1]->next()) {
           if(phit[1]->checkstatus(AMSDBc::BAD)&& BadBetaAlreadyExists(patb)) continue;
           if (!BETAFITFFKEY.FullReco && (!ptrack->checkstatus(AMSDBc::FalseTOFX)|| ptrack->checkstatus(AMSDBc::RELEASED))) {
             if (phit[1]->checkstatus(AMSDBc::USED)) continue;
           }
           AMSPoint dst=AMSBeta::Distance(phit[1]->getcoo(),phit[1]->getecoo(),
           ptrack,sleng[1],td);
           if (!(dst<=SearchReg*phit[1]->getnmemb())) continue;
           chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);
// 2-point combination found
           if(AMSBeta::patpoints[patb]==2){
             if(AMSBeta::_addnext(patb,2,sleng,phit,ptrack,td,chi2space)){
               bfound++;
               goto nexttrack;
             }
             continue;
           }

// Loop on third TOF plane
           phit[2]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][2]-1);
           for ( ; phit[2]; phit[2]=phit[2]->next()) {
             if(phit[2]->checkstatus(AMSDBc::BAD)&& BadBetaAlreadyExists(patb)) continue;
             if (!BETAFITFFKEY.FullReco && (!ptrack->checkstatus(AMSDBc::FalseTOFX)|| ptrack->checkstatus(AMSDBc::RELEASED))) {
             if (phit[2]->checkstatus(AMSDBc::USED)) continue;
               }
             AMSPoint dst=AMSBeta::Distance(phit[2]->getcoo(),phit[2]->
             getecoo(),ptrack,sleng[2],td);
             if(!(dst<=SearchReg*phit[2]->getnmemb())) continue;
               chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);
// 3-point combination found
             if(AMSBeta::patpoints[patb]==3){
               if(AMSBeta::_addnext(patb,3,sleng,phit,ptrack,td,chi2space)){
                bfound++;
                 goto nexttrack;
     }
               continue;
  }

// Loop on fourth TOF plane
             phit[3]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][3]-1);
             for ( ; phit[3]; phit[3]=phit[3]->next()) {
               if(phit[3]->checkstatus(AMSDBc::BAD)&& BadBetaAlreadyExists(patb)) continue;
               if (!BETAFITFFKEY.FullReco && (!ptrack->checkstatus(AMSDBc::FalseTOFX)|| ptrack->checkstatus(AMSDBc::RELEASED))){
                 if (phit[3]->checkstatus(AMSDBc::USED)) continue;
 }
               AMSPoint dst=AMSBeta::Distance(phit[3]->getcoo(),phit[3]->
               getecoo(),ptrack,sleng[3],td);
               if(!(dst<=SearchReg*phit[3]->getnmemb())) continue;
               chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);
// 4-point combination found
               if(AMSBeta::_addnext(patb,4,sleng,phit,ptrack,td,chi2space)){
                 bfound++;
                 goto nexttrack;
               }
               continue;
             }
           }
         }
       }
nexttrack: 
       continue;
     }
  }
  if(bfound && Sonly==1)break; 
}


{

// get beta from TRD info (eventually if TriggerLVL302 is ok)

if( !bfound ){
// Loop on TOF patterns
 for ( int patb=0; patb<npatb; patb++){
   if(!BETAFITFFKEY.pattern[patb]) continue;
   number td;
               

// Loop on track patterns

// Loop on tracks
     AMSTRDTrack *ptrackT=(AMSTRDTrack*)AMSEvent::gethead()->getheadC("AMSTRDTrack",0,1);
     for ( ; ptrackT ; ptrackT=ptrackT->next()) {

       if(!BETAFITFFKEY.FullReco && ptrackT->checkstatus(AMSDBc::USED)) continue;
       AMSTOFCluster * phit[4]={0,0,0,0};
       number sleng[4];
// Make False Track       
       AMSTrTrack *ptrack = new AMSTrTrack(ptrackT->gettheta(),ptrackT->getphi(), ptrackT->getcoo());
        ptrack->setstatus(AMSDBc::TRDTRACK); 
  

// Loop on first TOF plane
       phit[0]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][0]-1);
       for ( ; phit[0]; phit[0]=phit[0]->next()) {
         number chi2space=0;
         if(phit[0]->checkstatus(AMSDBc::BAD)&& BadBetaAlreadyExists(patb)) continue;
         if (!BETAFITFFKEY.FullReco ) {
           if (phit[0]->checkstatus(AMSDBc::USED)) continue;
   }
         AMSPoint dst=AMSBeta::Distance(phit[0]->getcoo(),phit[0]->getecoo(),
         ptrack,sleng[0],td);
         if (!(dst<=SearchReg*phit[0]->getnmemb())) continue;
         chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);

// Loop on second TOF plane
         phit[1]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][1]-1);
         for ( ; phit[1]; phit[1]=phit[1]->next()) {
           if(phit[1]->checkstatus(AMSDBc::BAD)&& BadBetaAlreadyExists(patb)) continue;
           if (!BETAFITFFKEY.FullReco) {
             if (phit[1]->checkstatus(AMSDBc::USED)) continue;
 }
           AMSPoint dst=AMSBeta::Distance(phit[1]->getcoo(),phit[1]->getecoo(),
           ptrack,sleng[1],td);
           if (!(dst<=SearchReg*phit[1]->getnmemb())) continue;
           chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);
// 2-point combination found
           if(AMSBeta::patpoints[patb]==2){
             if(AMSBeta::_addnext(patb,2,sleng,phit,ptrack,chi2space)){
		  bfound++;
               ptrackT->setstatus(AMSDBc::USED);
               ptrack->setstatus(AMSDBc::USED);
  
               goto nexttrack2;
             }
             continue;
}

// Loop on third TOF plane
           phit[2]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][2]-1);
           for ( ; phit[2]; phit[2]=phit[2]->next()) {
             if(phit[2]->checkstatus(AMSDBc::BAD)&& BadBetaAlreadyExists(patb)) continue;
             if (!BETAFITFFKEY.FullReco ) {
             if (phit[2]->checkstatus(AMSDBc::USED)) continue;
               }
             AMSPoint dst=AMSBeta::Distance(phit[2]->getcoo(),phit[2]->
             getecoo(),ptrack,sleng[2],td);
             if(!(dst<=SearchReg*phit[2]->getnmemb())) continue;
             chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);
// 3-point combination found
             if(AMSBeta::patpoints[patb]==3){
               if(AMSBeta::_addnext(patb,3,sleng,phit,ptrack,chi2space)){
		  bfound++;
                 ptrackT->setstatus(AMSDBc::USED);
                 ptrack->setstatus(AMSDBc::USED);
                 goto nexttrack2;
               }
               continue;
             }

// Loop on fourth TOF plane
             phit[3]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][3]-1);
             for ( ; phit[3]; phit[3]=phit[3]->next()) {
               if(phit[3]->checkstatus(AMSDBc::BAD)&& BadBetaAlreadyExists(patb)) continue;
               if (!BETAFITFFKEY.FullReco ){
                 if (phit[3]->checkstatus(AMSDBc::USED)) continue;
               }
               AMSPoint dst=AMSBeta::Distance(phit[3]->getcoo(),phit[3]->
               getecoo(),ptrack,sleng[3],td);
               if(!(dst<=SearchReg*phit[3]->getnmemb())) continue;
               chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);
// 4-point combination found
               if(AMSBeta::_addnext(patb,4,sleng,phit,ptrack,chi2space)){
		  bfound++;
                 ptrackT->setstatus(AMSDBc::USED);
                 ptrack->setstatus(AMSDBc::USED);
                 goto nexttrack2;
               }
               continue;
             }
           }
         }
       }
       delete ptrack;
nexttrack2: 
       continue;
     }
   }
 }
}


// Now get Beta without track if and only if lvl3 is ok


// Loop on TOF patterns
TriggerLVL302 *ptr=(TriggerLVL302*)AMSEvent::gethead()->getheadC("TriggerLVL3",0);
 AMSTrTrack * ptrackF=0;
// if(1 || (ptr && ptr->TOFOK() && LVL3FFKEY.Accept==0)){
if(!bfound){
   for ( int pat=0; pat<npatb; pat++){
     AMSTOFCluster * phit[4]={0,0,0,0};
     number sleng[4];
     if(BETAFITFFKEY.pattern[pat]){
      int fp=patpoints[pat]-1;    
      // Try to make StrLine Fit
      integer first=patconf[pat][0]-1;
      integer second=patconf[pat][fp]-1;
      phit[0]=AMSTOFCluster::gethead(first);
      while( phit[0]){
       if(!phit[0]->checkstatus(AMSDBc::BAD)){
        phit[fp]=AMSTOFCluster::gethead(second);
        while( phit[fp]){
         if(!phit[fp]->checkstatus(AMSDBc::BAD)){
          number dist=(phit[fp]->getcoo()).dist(phit[0]->getcoo());
          number u=(phit[fp]-> getcoo()[0]-phit[0]-> getcoo()[0])/dist;
          number v=(phit[fp]-> getcoo()[1]-phit[0]-> getcoo()[1])/dist;
          number w=(phit[fp]-> getcoo()[2]-phit[0]-> getcoo()[2])/dist;
          AMSDir dir(u,v,w);
          ptrackF=new AMSTrTrack(dir,phit[0]->getcoo());
          ptrackF->setstatus(AMSDBc::NOTRACK);
          //track found, go out
          goto found;
         }
         phit[fp]=phit[fp]->next(); 
        }
       }
       phit[0]=phit[0]->next(); 
      }
     }
    }
   }
found:
   if(ptrackF){

// Loop on TOF patterns
 for ( int patb=0; patb<npatb; patb++){
   if(!BETAFITFFKEY.pattern[patb]) continue;
   number td;



// Loop on tracks
     AMSTrTrack *ptrack=ptrackF;

       AMSTOFCluster * phit[4]={0,0,0,0};
       number sleng[4];

// Loop on first TOF plane
       phit[0]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][0]-1);
       for ( ; phit[0]; phit[0]=phit[0]->next()) {
         number chi2space=0;
         if(phit[0]->checkstatus(AMSDBc::BAD)) continue;
         AMSPoint dst=AMSBeta::Distance(phit[0]->getcoo(),phit[0]->getecoo(),
         ptrack,sleng[0],td);
         if (!(dst<=SearchReg*phit[0]->getnmemb())) continue;
         chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);

// Loop on second TOF plane
         phit[1]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][1]-1);
         for ( ; phit[1]; phit[1]=phit[1]->next()) {
           if(phit[1]->checkstatus(AMSDBc::BAD)) continue;
           AMSPoint dst=AMSBeta::Distance(phit[1]->getcoo(),phit[1]->getecoo(),
           ptrack,sleng[1],td);
           if (!(dst<=SearchReg*phit[1]->getnmemb())) continue;
           chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);
// 2-point combination found
           if(AMSBeta::patpoints[patb]==2){
             if(AMSBeta::_addnext(patb,2,sleng,phit,ptrack,chi2space))
               return 1;
             continue;
           }

// Loop on third TOF plane
           phit[2]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][2]-1);
           for ( ; phit[2]; phit[2]=phit[2]->next()) {
             if(phit[2]->checkstatus(AMSDBc::BAD)) continue;
             AMSPoint dst=AMSBeta::Distance(phit[2]->getcoo(),phit[2]->
             getecoo(),ptrack,sleng[2],td);
             if(!(dst<=SearchReg*phit[2]->getnmemb())) continue;
             chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);
// 3-point combination found
             if(AMSBeta::patpoints[patb]==3){
               if(AMSBeta::_addnext(patb,3,sleng,phit,ptrack,chi2space))
                 return 1;
               continue;
             }

// Loop on fourth TOF plane
             phit[3]=AMSTOFCluster::gethead(AMSBeta::patconf[patb][3]-1);
             for ( ; phit[3]; phit[3]=phit[3]->next()) {
               if(phit[3]->checkstatus(AMSDBc::BAD)) continue;
               AMSPoint dst=AMSBeta::Distance(phit[3]->getcoo(),phit[3]->
               getecoo(),ptrack,sleng[3],td);
               if(!(dst<=SearchReg*phit[3]->getnmemb())) continue;
               chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);
// 4-point combination found
               if(AMSBeta::_addnext(patb,4,sleng,phit,ptrack,chi2space))
                 return 1;
               continue;
             }
           }
         }
       }
    }



   }
// no beta found;
   delete ptrackF;
   return 1;
}


AMSPoint AMSBeta::Distance(AMSPoint coo, AMSPoint ecoo, AMSTrTrack *ptr,
			   number & sleng, number & theta){
  AMSDir dir(0,0,1.);
  AMSPoint outp;
  number phi;
  ptr->interpolate(coo,dir,outp,theta,phi,sleng);
  if(ptr->getpattern()<0){
    ecoo[0]+=1.;  // add one cm fot trd/tof
    ecoo[1]+=1.;  // add one cm fot trd/tof
  }
  outp=((outp-coo)/ecoo).abs();
  return outp;
}
#endif



integer AMSBeta::_addnext(integer pat, integer nhit, number sleng[],
			  AMSTOFCluster* pthit[4],AMSTrTrack * ptrackc, number theta, number c2s,int Master){
  c2s=c2s/nhit;
  number slengc[4];
  //   find optimal ptrack
  AMSTrTrack *ptrack=(AMSTrTrack*)AMSEvent::gethead()->getheadC("AMSTrTrack",0,1);
  for(;ptrack;ptrack=ptrack->next()){

#ifdef _PGTRACK_
    if(patt->GetHitPatternNmask(ptrack->getpattern())!=patt->GetHitPatternNmask(ptrackc->getpattern()))continue;    
#else
    if(TKDBc::patpoints(ptrack->getpattern())!=TKDBc::patpoints(ptrackc->getpattern()))continue;    
#endif


    //PZ FIXME TRACK STATUS
    if(ptrackc->checkstatus(AMSDBc::FalseTOFX) && !ptrack->checkstatus(AMSDBc::FalseTOFX))continue;
    if(ptrack->checkstatus(AMSDBc::FalseTOFX) && !ptrackc->checkstatus(AMSDBc::FalseTOFX))continue;
    if(ptrack->checkstatus(AMSDBc::FalseX) && !ptrackc->checkstatus(AMSDBc::FalseX))continue;
    number td;
    number chi2space=0;
    for (int k=0;k<nhit;k++){
      AMSPoint dst=Distance(pthit[k]->getcoo(),pthit[k]->getecoo(),
			    ptrack,slengc[k],td);
      chi2space+=sqrt(dst[0]*dst[0]+dst[1]*dst[1]);
    }
    chi2space/=nhit;
    if(chi2space<c2s){
      c2s=chi2space;
      ptrackc=ptrack;
      for(int k=0;k<4;k++)sleng[k]=slengc[k];
    }
  }

     
#ifdef __UPOOL__
  AMSBeta beta(pat,  pthit, ptrackc, c2s,sleng);
  AMSBeta *pbeta=   &beta;
#else
  AMSBeta *pbeta=new AMSBeta(pat,  pthit, ptrackc,c2s,sleng);
#endif


  //----> recover 1-sided TOFRawCluster/TOFClusters using track info 
  int nh;
  integer status;

  (void) AMSEvent::gethead()->getheadC("TriggerLVL3",0); 

  if(!ptrackc->checkstatus(AMSDBc::FalseTOFX)   &&  
     !ptrackc->checkstatus(AMSDBc::WEAK)        )
//     ((!ptrackc->checkstatus(AMSDBc::FalseX) ) || 
//      (!plvl3 || plvl3->LVL3HeavyIon() )          )
//     )
    { 
      for(nh=0;nh<nhit;nh++){
	status=pthit[nh]->getstatus();
	if((status&TOFGC::SCBADB2)!=0 && 
	   (status&TOFGC::SCBADB5)!=0)    {//tempor  use now only TOF-recovered
	  
	  pbeta->setstatus(AMSDBc::RECOVERED);
	  pthit[nh]->recovers2(ptrackc);
	  
	}
      }
    }

  pbeta->SimpleFit(nhit, sleng);
  if(pat==9){
    if(pbeta->_Beta>0)pbeta->_Beta=1;
    else pbeta->_Beta=-1;
    pbeta->_InvErrBeta=1;
  }
  if(pbeta->getchi2()< BETAFITFFKEY.Chi2 ){
    // Mark Track as used
    /*
      if(ptrackc->checkstatus(AMSDBc::USED)) 
      ptrackc->setstatus(AMSDBc::AMBIG);
      if(ptrackc->checkstatus(AMSDBc::RELEASED)) 
      ptrackc->setstatus(AMSDBc::AMBIG);
    */
    ptrackc->setstatus(AMSDBc::USED);
    // Mark TOF hits as USED
    int i;
    for( i=0;i<nhit;i++){
      if(pthit[i]->checkstatus(AMSDBc::USED)) 
	pthit[i]->setstatus(AMSDBc::AMBIG);
      if(pthit[i]->checkstatus(AMSDBc::RELEASED))
	pthit[i]->setstatus(AMSDBc::AMBIG);
      pthit[i]->setstatus(AMSDBc::USED);
    }


    /* Set the ambiguity bit for tracks sharing the same TOF hits */
    /* This is necessary for FalseTOFX tracks */
    for (i=0;i<nhit;i++){
      if (pthit[i]->checkstatus(AMSDBc::AMBIG) && !pthit[i]->checkstatus(AMSDBc::RELEASED)) {
	pbeta->setstatus(AMSDBc::AMBIG);
	break;  
      }
    }


    /* RELEASE hits for 1234, 123, 234 patters if beta is too low */
    /* and wait for use with pattern 23 */
    if(fabs(pbeta->getbeta()) < BETAFITFFKEY.LowBetaThr && pat !=7 &&
       !pbeta->checkstatus(AMSDBc::AMBIG)){
      //release track
      ptrackc->clearstatus(AMSDBc::USED);
      ptrackc->setstatus(AMSDBc::RELEASED);
      // release hits if pat # 7 and low beta
      for( i=0;i<nhit;i++){
	if(pthit[i]->getntof() ==2)pthit[i]->clearstatus(AMSDBc::USED);
	if(pthit[i]->getntof() ==3)pthit[i]->clearstatus(AMSDBc::USED);
	if(pthit[i]->getntof() ==2)pthit[i]->setstatus(AMSDBc::RELEASED);
	if(pthit[i]->getntof() ==3)pthit[i]->setstatus(AMSDBc::RELEASED);
      }
      // set AMBIG flag on beta here if pat = 0,1 or 4
      if(pat==0 || pat==1 || pat==4)pbeta->setstatus(AMSDBc::AMBIG);
    }                  


    // permanently add;
         
#ifdef __UPOOL__
    pbeta=new AMSBeta(beta);
#endif
    //    pbeta->type=0;
    if(Master)
      AMSEvent::gethead()->addnext(AMSID("AMSBeta",pat),pbeta);
    else{
      pbeta->SetAlternateContainer();
      AMSEvent::gethead()->addnext(AMSID("AMSBetaB",pat),pbeta);
    }
    return 1;
  }
#ifndef __UPOOL__
  delete pbeta;
#endif
  return 0;
}


integer AMSBeta::_addnextP(integer pat, integer nhit, number sleng[],
			   AMSTOFCluster* pthit[4],AMSTrTrack * ptrack, number c2s,int Master){
  c2s=c2s/nhit;
  // The track has already been found !!!!! -->  drop find optimal ptrack
#ifdef __UPOOL__
  AMSBeta beta(pat,  pthit, ptrack, c2s,sleng);
  AMSBeta *pbeta=   &beta;
#else
  AMSBeta *pbeta=new AMSBeta(pat,  pthit, ptrack,c2s,sleng);
#endif

 
  //   DONE Outside now!!!
//   //----> recover 1-sided TOFRawCluster/TOFClusters using track info 
//   for(int nh=0;nh<nhit;nh++){
//     int status=pthit[nh]->getstatus();
//     if((status&TOFGC::SCBADB2)!=0 && 
//        (status&TOFGC::SCBADB5)!=0)    {//tempor  use now only TOF-recovered
//       pbeta->setstatus(AMSDBc::RECOVERED);
//       pthit[nh]->recovers2(ptrack);
//     }
//   }
  
  
  pbeta->SimpleFit(nhit, sleng);
  
  if(pat==9){
    if(pbeta->_Beta>0)pbeta->_Beta=1;
    else pbeta->_Beta=-1;
    pbeta->_InvErrBeta=1;
  }
  if(pbeta->getchi2()< BETAFITFFKEY.Chi2 ){
    // Mark Track as used
    /*
      if(ptrack->checkstatus(AMSDBc::USED)) 
      ptrack->setstatus(AMSDBc::AMBIG);
      if(ptrack->checkstatus(AMSDBc::RELEASED)) 
      ptrack->setstatus(AMSDBc::AMBIG);
    */
    ptrack->setstatus(AMSDBc::USED);
    // Mark TOF hits as USED
    int i;
    for( i=0;i<nhit;i++){
      if(pthit[i]->checkstatus(AMSDBc::USED)) 
	pthit[i]->setstatus(AMSDBc::AMBIG);
      if(pthit[i]->checkstatus(AMSDBc::RELEASED))
	pthit[i]->setstatus(AMSDBc::AMBIG);
      pthit[i]->setstatus(AMSDBc::USED);
    }


    /* Set the ambiguity bit for tracks sharing the same TOF hits */
    /* This is necessary for FalseTOFX tracks */
    for (i=0;i<nhit;i++){
      if (pthit[i]->checkstatus(AMSDBc::AMBIG) && !pthit[i]->checkstatus(AMSDBc::RELEASED)) {
	pbeta->setstatus(AMSDBc::AMBIG);
	break;  
      }
    }


    /* RELEASE hits for 1234, 123, 234 patters if beta is too low */
    /* and wait for use with pattern 23 */
    if(fabs(pbeta->getbeta()) < BETAFITFFKEY.LowBetaThr && pat !=7 &&
       !pbeta->checkstatus(AMSDBc::AMBIG)){
      //release track
      ptrack->clearstatus(AMSDBc::USED);
      ptrack->setstatus(AMSDBc::RELEASED);
      // release hits if pat # 7 and low beta
      for( i=0;i<nhit;i++){
	if(pthit[i]->getntof() ==2)pthit[i]->clearstatus(AMSDBc::USED);
	if(pthit[i]->getntof() ==3)pthit[i]->clearstatus(AMSDBc::USED);
	if(pthit[i]->getntof() ==2)pthit[i]->setstatus(AMSDBc::RELEASED);
	if(pthit[i]->getntof() ==3)pthit[i]->setstatus(AMSDBc::RELEASED);
      }
      // set AMBIG flag on beta here if pat = 0,1 or 4
      if(pat==0 || pat==1 || pat==4)pbeta->setstatus(AMSDBc::AMBIG);
    }                  


    // permanently add;
         
#ifdef __UPOOL__
    pbeta=new AMSBeta(beta);
#endif
    //    pbeta->type=1;
    if(Master)
      AMSEvent::gethead()->addnext(AMSID("AMSBeta",pat),pbeta);
    else{
      pbeta->SetAlternateContainer();
      AMSEvent::gethead()->addnext(AMSID("AMSBetaB",pat),pbeta);
    }
    return 1;
  }
#ifndef __UPOOL__
  delete pbeta;
#endif
  return 0;
}






integer AMSBeta::_addnext(integer pat, integer nhit, number sleng[],
        AMSTOFCluster* pthit[4],AMSTrTrack * ptrack, number c2s,int Master){
        c2s=c2s/nhit;
#ifdef __UPOOL__
	AMSBeta beta(pat,  pthit, ptrack, c2s,sleng);
    AMSBeta *pbeta=   &beta;
#else
    AMSBeta *pbeta=new AMSBeta(pat,  pthit, ptrack,c2s,sleng);
#endif

    pbeta->SimpleFit(nhit, sleng);
    if(pat==9){
     if(pbeta->_Beta>0)pbeta->_Beta=1;
     else pbeta->_Beta=-1;
     pbeta->_InvErrBeta=1;
    }
    if(pbeta->getchi2()< BETAFITFFKEY.Chi2 ){
       if(ptrack->checkstatus(AMSDBc::NOTRACK)){
         pbeta->setstatus(AMSDBc::NOTRACK);
       }
       if(ptrack->checkstatus(AMSDBc::TRDTRACK)){
         pbeta->setstatus(AMSDBc::TRDTRACK);

         for( int i=0;i<nhit;i++){
           pthit[i]->setstatus(AMSDBc::USED);
         }



       }
         // permanently add;
#ifdef __UPOOL__
          pbeta=new AMSBeta(beta);
#endif
//           for(int i=0;i<npatb;i++){
//             if((AMSEvent::gethead()->getC("AMSBeta",i))->getnelem()){
//              pbeta->setstatus(AMSDBc::AMBIG);
//              break;
//             } 
//           }
//	  pbeta->type=0;
	  if(Master)
          AMSEvent::gethead()->addnext(AMSID("AMSBeta",pat),pbeta);
	  else{
	    pbeta->SetAlternateContainer();
	    AMSEvent::gethead()->addnext(AMSID("AMSBetaB",pat),pbeta);
	  }
          return 1;
   }
#ifndef __UPOOL__
       delete pbeta;
#endif
    return 0;
}






void AMSBeta::SimpleFit(integer nhit, number x[]){
 number y[4];
 number ey[4];
 number xy=0;
 number x2=0;
 number xa=0;
 number ya=0;
 number e2=0;
 number a,b;   //y=ax+b
 const number c=2.99792e10; //speed of light
 int i;
 for( i=0;i<nhit;i++){
  y[i]=_pcluster[i]->gettime()*c;
  ey[i]=_pcluster[i]->getetime()*c;
 }
 for(i=0;i<nhit;i++){
  xy+=x[i]*y[i]/ey[i]/ey[i];
  x2+=x[i]*x[i]/ey[i]/ey[i];
  xa+=x[i]/ey[i]/ey[i];
  ya+=y[i]/ey[i]/ey[i];
  e2+=1/ey[i]/ey[i];
 }
 xy=xy/e2;
 x2=x2/e2;
 xa=xa/e2;
 ya=ya/e2;
 double kk=(x2-xa*xa);
 if(kk==0) kk=0.000001;
 a=(xy-xa*ya)/kk;

 b=ya-a*xa;
 if (a==0)
   _Beta=100;
 else
   _Beta=1/a;
if(fabs(_Beta)>2){
 static int nerr=0;
 if(nhit>2 && nerr++<100){
   TOF2JobStat::addre(7);    
   cout<<"<--- AMSBeta::SimpleFit-W-BetaOutOfRange "<<_Beta<<
     " run/event="<<AMSEvent::gethead()->getrun()<<" "<<AMSEvent::gethead()->getid()<<endl;
   cout<<"     Nhits="<<nhit<<" LenArr:"<<x[0]<<" "<<x[1]<<" "<<x[2]<<" "<<x[3]<<endl;
   cout<<"                 TimeArr(ns):"<<y[0]/c*1.e9<<" "<<y[1]/c*1.e9<<" "<<y[2]/c*1.e9<<" "<<y[3]/c*1.e9<<endl;
 }
 if(_Beta>0)_Beta=2;
 else _Beta=-2;
}
 _Chi2=0;
 _InvErrBeta=0;
 for(i=0;i<nhit;i++)_Chi2+=(y[i]-a*x[i]-b)/ey[i]*(y[i]-a*x[i]-b)/ey[i];
 if(nhit>2)_Chi2=_Chi2/(nhit-2);
 else _Chi2=0;
 _InvErrBeta=sqrt(1./e2)/sqrt(kk);   

//  Corrected Beta
  if(_InvErrBeta==_InvErrBeta && fabs(_InvErrBeta)<FLT_MAX && nhit>2){
  number xibme=fabs(1/_Beta); 
  number z0=(xibme-1)/_InvErrBeta/sqrt(2.);
  number zint=DERFC(z0);
  number zprim=betacorr(zint,z0,0.5);
  number xibcor=zprim*sqrt(2.)*_InvErrBeta+1;
  _BetaC=1/xibcor;
  if(_Beta<0)_BetaC*=-1;
  number zprima=betacorr(zint,z0,1./6.);
  number zprimb=betacorr(zint,z0,5./6.);
  number xibcora=zprima*sqrt(2.)*_InvErrBeta+1;
  number xibcorb=zprimb*sqrt(2.)*_InvErrBeta+1;
  _InvErrBetaC=fabs(xibcorb-xibcora)/2;
  }
  else _InvErrBetaC=0;
  if(_InvErrBetaC==0){
    _BetaC=_Beta;
    _InvErrBetaC=_InvErrBeta;
  }
}




void AMSBeta::_writeEl(){

#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
}

void AMSBeta::_copyEl(){
#ifdef __WRITEROOT__
  if(PointerNotSet())return;
  BetaR & ptr = AMSJob::gethead()->getntuple()->Get_evroot02()->Beta(_vpos);
  if( IsAlternateContainer())
    ptr = AMSJob::gethead()->getntuple()->Get_evroot02()->BetaB(_vpos);
  if (_ptrack) ptr.fTrTrack= _ptrack->GetClonePointer();
  else ptr.fTrTrack=-1;
  ptr.fTofCluster.clear();
  for (int i=0; i<4; i++) {
    if(_pcluster[i])ptr.fTofCluster.push_back(_pcluster[i]->GetClonePointer());
  }
#endif
}


void AMSBeta::print(){
int i;
for( i=0;i<npatb;i++){
 AMSContainer *p =AMSEvent::gethead()->getC("AMSBeta",i);
 if(p && BETAFITFFKEY.pattern[i])p->printC(cout);
}
}


void AMSBeta::_printEl(ostream & stream){ stream << " Pattern " << getpattern() << " Beta "<<_Beta<<" ErrBeta" <<_InvErrBeta<<" Chi2 "<<_Chi2<<" Chi2S "<<_Chi2Space<<endl;}


number AMSBeta::betacorr(number zint,number z0,number part){
 if(zint<1./FLT_MAX){
   return 1;
 }
 number zmin=zint*part;
 number zb=z0+1;
 number x0;
 integer ntry=0;
 integer ntrymax=99;
 while(DERFC(zb)>zmin && ntry++<ntrymax)zb=zb+1;
 integer ierr;
 rzerowrapper_(z0,zb,x0,zmin,ierr);
 if(ierr || ntry>ntrymax){
  cerr<<"AMSBeta::betacorr-E-UnableToCorrectBeta "<<zint<<" "<<z0<<" "<<part<<" "<<ierr<<" "<<ntry<<endl;
  setstatus(AMSDBc::BAD);
  return 1;
 }
 else     return x0;
}

AMSBeta::~AMSBeta(){
}


bool AMSBeta::BadBetaAlreadyExists(int patb){
  //
  // check number of good tof clusters available
  //
  int ngood=0;
  for(int k=0;k<4;k++){
       int ngh=0;
       AMSTOFCluster *phit=AMSTOFCluster::gethead(k);
       for ( ; phit; phit=phit->next()) {
         if(!phit->checkstatus(AMSDBc::BAD))ngh++;
       }   
       if(ngh)ngood++;
  }

  if(ngood>=2)return true;
  else{
    for(int k=0;k<npatb;k++){
    if(AMSEvent::gethead()->getheadC("AMSBeta",k))return true;
    }
    return false;
   }
 }

double AMSBeta::GetTRDBetaCorr(){
//              double p[3]={0.522677,-0.16927,0.676221};
              double betamc1=0;
              for(int k=0;k<sizeof(BETAFITFFKEY.TRDP)/sizeof(BETAFITFFKEY.TRDP[0]);k++)betamc1+=BETAFITFFKEY.TRDP[k]*pow(fabs(_Beta),k);
              double x=(betamc1+fabs(_Beta))/2;
              if(x>0.96)x=0.96;
//              double norm=1.8377e-1;
              double norm=BETAFITFFKEY.TRDNorm;
              double corr=norm/x/x*(log(x*x/(1-x*x))-x*x+(8*log(0.511e6*2/10/54)+2*log(0.511e6*2/10/6)+4*log(0.511e6*2/10/8))/14.);
              return corr;
}




























