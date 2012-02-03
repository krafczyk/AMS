//  $Id: trdrec.C,v 1.62 2012/02/03 14:49:16 choutko Exp $
#include "trdrec.h"
#include "event.h"
#include "ntuple.h"
#include "extC.h"
#include"trigger302.h"
#include "tofrec02.h"
#ifdef _PGTRACK_
#include "TrFit.h"
#else
#include "trrec.h"
#endif

AMSTRDCluster * AMSTRDCluster::_Head[trdconst::maxlay]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
integer AMSTRDCluster::build(int rerun){
for(int mode=0;mode<2*AMSTRDIdSoft::ncrates();mode+=AMSTRDIdSoft::ncrates()){
//for(int mode=0;mode<AMSTRDIdSoft::ncrates();mode+=AMSTRDIdSoft::ncrates()){
    number Thr1A=TRDCLFFKEY.Thr1A/TRDCLFFKEY.ADC2KeV;
    number Thr1S=TRDCLFFKEY.Thr1S/TRDCLFFKEY.ADC2KeV;
    number Thr1R=TRDCLFFKEY.Thr1R;
    number Thr1H=TRDCLFFKEY.Thr1H/TRDCLFFKEY.ADC2KeV;
    number adc[trdconst::maxtube+3];
    AMSTRDRawHit * ptra[trdconst::maxtube+3];
    VZERO(adc,sizeof(adc)/sizeof(integer));
    VZERO(ptra,sizeof(ptra)/sizeof(integer));
    for (int n=mode;n<mode+AMSTRDIdSoft::ncrates();n++){
     AMSTRDRawHit * ptr=
     (AMSTRDRawHit*)AMSEvent::gethead()->getheadC("AMSTRDRawHit",n,2);  
     while(ptr){
       AMSTRDIdSoft id(ptr->getidsoft());
       integer ilay=id.getlayer();
       integer ilad=id.getladder();
       if(ptr->testlast()){
        if(!id.checkstatus(AMSDBc::BAD))adc[ptr->getidsoft().gettube()]=ptr->Amp()/(id.getgain()>0?id.getgain():1);
        ptra[ptr->getidsoft().gettube()]=ptr;
#ifdef __AMSDEBUG__
        if(id.getgain()<=0){
          cerr <<"AMSTRDCluster::build-E-zero gain for "<<id<<" "<<id.getgain()<<endl;
        }
#endif
        number ref=-FLT_MAX;
        int status=0;
        for(int i=0;i<trdconst::maxtube+1;i++){
         if(adc[i]<ref){
          if( adc[i]< adc[i+1] && adc[i+1]> Thr1A){
             status=AMSDBc::WIDE;
          }
          else if(adc[i+1]<adc[i+2] && adc[i+2]>Thr1A){
             status=AMSDBc::NEAR;
          }
          else {
            status=0;
          }
          // find left & right
          int hmul=0;
          int center=i-1;
          int left=0;
          int right=trdconst::maxtube;
          for(int k=center-1;k>=0;k--){
           id.upd(k);
           if(adc[k]<=0 || id.checkstatus(AMSDBc::BAD) || center-k>TRDCLFFKEY.MaxHitsInCluster){
            left=k+1;
            break;
           }
          }
          if(status==AMSDBc::WIDE || status==AMSDBc::NEAR){
            right=adc[i]==0?center:center+1;
          }
          else right=center+2;
          if(right>trdconst::maxtube-1)right=trdconst::maxtube-1;
          for( int k=right;k>center;k--){
           id.upd(k);
           if(adc[k]<=0 || id.checkstatus(AMSDBc::BAD)|| k-left>TRDCLFFKEY.MaxHitsInCluster)right--;
           else break;
          }
          i=right+1;
          number sum=0;
          number ssum=0;
          number pos=0;
          number rms=0;
           AMSTRDRawHit* ptrmx=ptr;
           number adcmx=-1;
          for (int j=left;j<right+1;j++){
           id.upd(j);
           if(status==AMSDBc::WIDE && j==right){
            if(adc[j]/2>Thr1H)hmul++;
            sum+=adc[j]/2;
            ssum+=id.getsig()*id.getsig();
            pos+=(j-center)*adc[j]/2;
            rms+=(j-center)*(j-center)*adc[j]/2;
             if(adc[j]/2>adcmx && ptra[j]){
               adcmx=adc[j]/2;
               ptrmx=ptra[j];
              }

            adc[j]=adc[j]/2;
           }            
           else{
            if(adc[j]>Thr1H)hmul++;
              sum+=adc[j];
              ssum+=id.getsig()*id.getsig();
              pos+=(j-center)*adc[j];
              rms+=(j-center)*(j-center)*adc[j];
              if(adc[j]>adcmx && ptra[j]){
               adcmx=adc[j];
               ptrmx=ptra[j];
              }
              adc[j]=0;
           }
          }
          if(sum !=0){
           rms=sqrt(fabs(rms*sum-pos*pos))/sum; 
           pos=pos/sum;
           ssum=sqrt(ssum);
          }
          ref=-FLT_MAX;
          id.upd(center);
          AMSTRDIdGeom idg(id);
          AMSgvolume * pv=AMSJob::gethead()->getgeomvolume(idg.crgid());
          if(pv){
           AMSPoint coo(pv->getcooA(0),pv->getcooA(1),pv->getcooA(2));
           if(AMSJob::gethead()->isRealData()){
            if(strstr(AMSJob::gethead()->getsetup(),"PreAss")){
             coo[0]+=0.03;
             coo[1]+=-0.09;
             coo[2]+=-0.003;
            }
            else{
             coo[0]+=-0.03;
             coo[1]+=-0.12;
             coo[2]+=0.045;
            }
           }
           number rad=pv->getpar(1);
           number z=pv->getpar(2);
           AMSDir xdir(fabs(pv->getnrmA(0,0)),fabs(pv->getnrmA(1,0)),fabs(pv->getnrmA(2,0)));
           AMSDir ydir(fabs(pv->getnrmA(0,1)),fabs(pv->getnrmA(1,1)),fabs(pv->getnrmA(2,1)));
           AMSDir cdir=xdir[2]<ydir[2]?xdir:ydir;
           coo=coo+cdir*pos;           
           AMSDir zdir(fabs(pv->getnrmA(0,2)),fabs(pv->getnrmA(1,2)),fabs(pv->getnrmA(2,2)));
           ptr->setstatus(AMSDBc::USED);
           for (int j=left;j<right+1;j++){
             if(ptra[j]){
              (ptra[j])->setstatus(AMSDBc::USED);
             }
           }
           status=status | AMSDBc::GOOD;
           //cout <<" center "<<center<<" "<<ptra[center]<<endl;
           if(adcmx<Thr1A){
		     //cout <<" pisec "<<adcmx<<" "<<sum<<endl;
		   }
		   if(sum>Thr1A){
                   //cout <<" trdcl "<<sum*TRDCLFFKEY.ADC2KeV<<" "<<sum<<" "<<TRDCLFFKEY.ADC2KeV<<endl;
                   AMSTRDCluster *ph=new AMSTRDCluster(status,ilay,coo,rad,z,zdir,sum*TRDCLFFKEY.ADC2KeV,right-left+1,hmul,ptrmx);
                   if(mode)ph->setstatus(AMSDBc::RECOVERED);
  		    AMSEvent::gethead()->addnext(AMSID("AMSTRDCluster",ilay),ph);
     		  }
	}
		  else {
		   cerr<< "AMSTRDCluster::build-S-Nogeomvolumefound "<<idg.crgid()<<endl;
		  }          
		 }
		 else if(adc[i]>Thr1A){
		  id.upd(i);
		  if(id.checkstatus(AMSDBc::BAD)==0 && 
		   id.getsig() < Thr1S && 
		   adc[i]/id.getsig() >Thr1R){
		   ref=adc[i];
		  }
		 }
	       }
		 VZERO(adc,sizeof(adc)/sizeof(integer));
	      }
	      else{
                 adc[ptr->getidsoft().gettube()]=ptr->Amp()/(id.getgain()>0?id.getgain():1);
                 ptra[ptr->getidsoft().gettube()]=ptr;
                }
		ptr=ptr->next();
	      }
	    }
         }
	return 1;
	}



	void AMSTRDCluster::_writeEl(){
	  integer flag =    (IOPA.WriteAll%10==1)
			 || (checkstatus(AMSDBc::USED));
	  if(AMSTRDCluster::Out(flag) ){
	#ifdef __WRITEROOT__
	    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
	#endif
/*
	  TRDClusterNtuple* TrN = AMSJob::gethead()->getntuple()->Get_trdcl();

	   if (TrN->Ntrdcl>=MAXTRDCL) return;
	   TrN->Status[TrN->Ntrdcl]=_status;
	   TrN->Layer[TrN->Ntrdcl]=_layer;
	   for(int i=0;i<3;i++)TrN->Coo[TrN->Ntrdcl][i]=_Coo[i];
	   for(int i=0;i<3;i++)TrN->CooDir[TrN->Ntrdcl][i]=_CooDir[i];
	   TrN->Multip[TrN->Ntrdcl]=_Multiplicity;
	   TrN->HMultip[TrN->Ntrdcl]=_HighMultiplicity;
	   TrN->EDep[TrN->Ntrdcl]=_Edep;
	   TrN->pRawHit[TrN->Ntrdcl]=_pmaxhit->getpos();
	   if(AMSTRDRawHit::Out(IOPA.WriteAll%10==1)){
	      for(int i=0;i<_pmaxhit->getidsoft().getcrate();i++){
		AMSContainer *pc=AMSEvent::gethead()->getC("AMSTRDRawHit",i);
		TrN->pRawHit[TrN->Ntrdcl]+=pc->getnelem();
	      }
	   }
	    else{
	      //Write only USED hits
	      for(int i=0;i<_pmaxhit->getidsoft().getcrate();i++){
		AMSTRDRawHit *ptr=(AMSTRDRawHit*)AMSEvent::gethead()->getheadC("AMSTRDRawHit",i);
		while(ptr && ptr->checkstatus(AMSDBc::USED) ){
		 TrN->pRawHit[TrN->Ntrdcl]++;
		  ptr=ptr->next();
		}
	      }
	    }
	   TrN->Ntrdcl++;
*/
	  }

	}

	void AMSTRDCluster::_copyEl(){
	#ifdef __WRITEROOT__
	 if(PointerNotSet())return;
	   TrdClusterR & ptr=AMSJob::gethead()->getntuple()->Get_evroot02()->TrdCluster(_vpos);
	    if (_pmaxhit) ptr.fTrdRawHit= _pmaxhit->GetClonePointer();
	    else ptr.fTrdRawHit=-1;
	#endif
	}

	void AMSTRDCluster::_printEl(ostream &o){
o<<_layer<<" "<<"Multiplicity "<<_Multiplicity<<" Ampl "<<_Edep<<" Dir&Coo "<<_CooDir<<_Coo<<endl;
}







integer AMSTRDCluster::Out(integer status){
static integer init=0;
static integer WriteAll=0;
//#pragma omp threadprivate(init,WriteAll)
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
  for(int n=0;n<ntrig;n++){
    if(strcmp("AMSTRDCluster",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
    }
  }
}
return (WriteAll || status);
}


number AMSTRDCluster::getHit(uinteger orr){
// or - 0 - perp to tube orientation
// or - 1 - z
if(orr==0)return _Coo[0]*_CooDir[1]-_Coo[1]*_CooDir[0];
else return _Coo[2];
}

integer AMSTRDSegment::_case=0;
integer AMSTRDTrack::_case=0;

  number AMSTRDSegment::par[2]={0,0};
 AMSTRDCluster * AMSTRDSegment::phit[trdconst::maxhits];

integer AMSTRDSegment::build(int rerun){
  int nseg=0;
 for(_case=0;_case<2;_case++){
for(int iseg=0;iseg<TRDDBc::nlayS();iseg++){  
  bool      WeakCaseWanted=true;
  integer NTrackFound=-1;
  for ( pat=0;pat<TRDDBc::npatH(iseg);pat++){
    if(TRDDBc::patallowH(pat,iseg) || (TRDDBc::patallow2H(pat,iseg) && WeakCaseWanted)){
      int fp=TRDDBc::patpointsH(pat,iseg)-1;    
      // Try to make StrLine Fit
      integer first=TRDDBc::patconfH(pat,0,iseg)-1;
      integer second=TRDDBc::patconfH(pat,fp,iseg)-1;
      phit[0]=AMSTRDCluster::gethead(first);
      while( phit[0]){
       if(phit[0]->Good(AMSDBc::RECOVERED,_case)){
       phit[fp]=AMSTRDCluster::gethead(second);
       while( phit[fp]){
        if(phit[fp]->Good(AMSDBc::RECOVERED,_case)){
        par[0]=(phit[fp]-> getHit(0)-phit[0]-> getHit(0))/
               (phit[fp]-> getHit(1)-phit[0]-> getHit(1));
        par[1]=phit[0]-> getHit(0)-par[0]*phit[0]-> getHit(1);
        if(NTrackFound<0)NTrackFound=0;
        // Search for others
        integer npfound=_TrSearcher(1,iseg);
        if(npfound){
           NTrackFound++;
           if(TRDDBc::patallowH(pat,iseg))WeakCaseWanted=false;
         goto out;
        }

        }         
        phit[fp]=phit[fp]->next();
       }
       }  
out:
       phit[0]=phit[0]->next();
      }
      
        
    }
  }
if(NTrackFound>0)nseg++;
}
}
return nseg;
}







integer AMSTRDSegment::pat=0;
integer AMSTRDSegment::_TrSearcher(int icall,uinteger iseg){
           
           phit[icall]=AMSTRDCluster::gethead(TRDDBc::patconfH(pat,icall,iseg)-1);
           while(phit[icall]){
             if(phit[icall]->Good(AMSDBc::RECOVERED,_case) && !Distance1D(par,phit[icall])){
              if(TRDDBc::patpointsH(pat,iseg) >icall+2){         
                return _TrSearcher(++icall,iseg); 
              }                
              else {
                // icall+2 point combination found
                if(_addnext(pat,TRDDBc::patpointsH(pat,iseg),iseg,phit)){
                  return TRDDBc::patpointsH(pat,iseg);
                }
              }
             }
             phit[icall]=phit[icall]->next();
           }
           return 0;


}


integer AMSTRDSegment::_addnext(integer pat, integer nhit, uinteger iseg, AMSTRDCluster* pthit[]){

#ifdef __UPOOL__
cerr<< "AMSTRSegment::_addnext-F-UPOOL not supported yet"<<endl;
abort();
#else
     
    AMSTRDSegment *ptrack=   new AMSTRDSegment(iseg,pat, nhit ,TRDDBc::oriseg(iseg),pthit);
    if(pthit[0]->checkstatus(AMSDBc::RECOVERED))ptrack->setstatus(AMSDBc::RECOVERED);
#endif

    ptrack->Fit();
    if(ptrack->_Chi2< TRDRECFFKEY.Chi2StrLine){
          ptrack->_addnextR(iseg);
          return 1;
       }
    delete ptrack;  
    return 0;
}



void AMSTRDSegment::_addnextR(uinteger iseg){

         int i;
         // Mark hits as USED
         for( i=0;i<_NHits;i++){
           if(_pCl[i]->checkstatus(AMSDBc::USED))
            _pCl[i]->setstatus(AMSDBc::AMBIG);
           else _pCl[i]->setstatus(AMSDBc::USED);
         }
         // permanently add;
          AMSEvent::gethead()->addnext(AMSID("AMSTRDSegment",iseg),this);
}


bool AMSTRDSegment::Distance1D(number par[2], AMSTRDCluster *ptr){

   number maxdist=ptr->getEHit()*3;
   return fabs(par[1]+par[0]*ptr->getHit(1)-ptr->getHit(0))/
            sqrt(1+par[0]*par[0]) > ptr->getEHit()*3;
}


void AMSTRDSegment::Fit(){

number x=0;
number x2=0;
number y=0;
number xy=0;
for (int i=0;i<_NHits;i++){
 x+=_pCl[i]->getHit(1);
 y+=_pCl[i]->getHit(0);
 xy+=_pCl[i]->getHit(1)*_pCl[i]->getHit(0);
 x2+=_pCl[i]->getHit(1)*_pCl[i]->getHit(1);
}
 x/=_NHits;
 y/=_NHits;
 xy/=_NHits;
 x2/=_NHits;
 _FitPar[0]=(xy-x*y)/(x2-x*x);
 _FitPar[1]=y-_FitPar[0]*x;
 _Chi2=0;
 for(int i=0;i<_NHits;i++){
  number d=(_pCl[i]->getHit(0)-_FitPar[0]*_pCl[i]->getHit(1)-_FitPar[1]);
  _Chi2+=d*d/_pCl[i]->getEHit()/_pCl[i]->getEHit();
 }
 _Chi2/=_NHits-1;
}



void AMSTRDSegment::_writeEl(){
  integer flag =    (IOPA.WriteAll%10==1)
                 || (checkstatus(AMSDBc::USED));
  if(Out(flag) ){
#ifdef __WRITEROOT__
   AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
/*
   TRDSegmentNtuple* TrN = AMSJob::gethead()->getntuple()->Get_trdseg();
   if (TrN->Ntrdseg>=MAXTRDSEG) return;
   TrN->Status[TrN->Ntrdseg]=_status;
   TrN->Orientation[TrN->Ntrdseg]=_Orientation;
   TrN->Chi2[TrN->Ntrdseg]=_Chi2;
   TrN->Pattern[TrN->Ntrdseg]=_Pattern;
   TrN->Nhits[TrN->Ntrdseg]=_NHits;
   for(int i=0;i<2;i++)TrN->FitPar[TrN->Ntrdseg][i]=_FitPar[i];

   for (int i=0;i<trdconst::maxhits;i++)TrN->PCl[TrN->Ntrdseg][i]=0;
   for (int i=0;i<_NHits;i++){
    TrN->PCl[TrN->Ntrdseg][i]=_pCl[i]->getpos();
   if(AMSTRDCluster::Out(IOPA.WriteAll%10==1)){
      for(int j=0;j<_pCl[i]->getlayer();j++){
        AMSContainer *pc=AMSEvent::gethead()->getC("AMSTRDCluster",j);
        TrN->PCl[TrN->Ntrdseg][i]+=pc->getnelem();
      }
   }
    else{
      //Write only USED hits
      for(int j=0;j<_pCl[i]->getlayer();j++){
        AMSTRDCluster *ptr=(AMSTRDCluster*)AMSEvent::gethead()->getheadC("AMSTRDCluster",j);
        while(ptr && ptr->checkstatus(AMSDBc::USED) ){
         TrN->PCl[TrN->Ntrdseg][i]++;
          ptr=ptr->next();
        }
      }
    }
   }
   TrN->Ntrdseg++;
*/
  }
}

void AMSTRDSegment::_copyEl(){
#ifdef __WRITEROOT__
 if(PointerNotSet())return;
  TrdSegmentR & ptr =AMSJob::gethead()->getntuple()->Get_evroot02()->TrdSegment(_vpos); 
   
    for (int i=0; i<_NHits; i++) {
      if (_pCl[i]) ptr.fTrdCluster.push_back(_pCl[i]->GetClonePointer());
    }
#endif
}

void AMSTRDSegment::_printEl(ostream &o){
o <<_Pattern<<" Nhits "<<_NHits<<endl;
}

integer AMSTRDSegment::Out(integer status){
static integer init=0;
static integer WriteAll=1;
//#pragma omp threadprivate(init,WriteAll)
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
  for(int n=0;n<ntrig;n++){
    if(strcmp("AMSTRDSegment",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
    }
  }
}
return (WriteAll || status);
}


 AMSTRDCluster * AMSTRDCluster::gethead(uinteger i){
   if(i<TRDDBc::nlay()){
    if(!_Head[i])_Head[i]=(AMSTRDCluster*)AMSEvent::gethead()->getheadC("AMSTRDCluster",i,1);
    return _Head[i];
   }
   else {
#ifdef __AMSDEBUG__
    cerr <<"AMSTRDCluster:gethead-S-Wrong Head "<<i;
#endif
    return 0;
   }   
}

 AMSTRDSegment * AMSTRDSegment::gethead(uinteger i){
   if(i<TRDDBc::nlayS()){
    return (AMSTRDSegment*)AMSEvent::gethead()->getheadC("AMSTRDSegment",i,1);
   }
   else {
#ifdef __AMSDEBUG__
    cerr <<"AMSTRDSegment:gethead-S-Wrong Head "<<i;
#endif
    return 0;
   }   
}


integer AMSTRDTrack::build(int rerun){
   Relax=false;
  _Start();

  int nrh=0;
  for(int i=0;i<TRDDBc::nlayS();i++){
    nrh+= (AMSEvent::gethead()->getC("AMSTRDSegment",i))->getnelem();
  }
  if(nrh>=TRDFITFFKEY.MaxSegAllowed){
    static int qprint=0; 
    if(qprint++<100)cerr <<"AMSTRDTrack::build-E-Too many segments "<<nrh<<endl;
    TriggerLVL302 *plvl3;
    plvl3 = (TriggerLVL302*)AMSEvent::gethead()->getheadC("TriggerLVL3",0);
    if(!plvl3 || plvl3->skip()){
      AMSEvent::gethead()->seterror();
      return 0;
    }
    cout <<" but lvl3 says continue "<<endl;
  }
   
again:  
  integer NTrackFound=-1;

  for( _case=0;_case<2;_case++){
    bool      WeakCaseWanted=false;
    for (pat=0;pat<TRDDBc::npatS();pat++){
      if(TRDDBc::patallowS(pat) || (TRDDBc::patallow2S(pat) && WeakCaseWanted)){
	int fp=TRDDBc::patpointsS(pat)-1;    
	// Try to make StrLine Fit
	integer first=TRDDBc::patconfS(pat,0)-1;
	integer second=TRDDBc::patconfS(pat,fp)-1;
	phit[0]=AMSTRDSegment::gethead(first);
	while( phit[0]){
	  if(phit[0]->Good(AMSDBc::RECOVERED,_case)){
	    phit[fp]=AMSTRDSegment::gethead(second);
	    while( phit[fp]){
	      if(phit[fp]->Good(AMSDBc::RECOVERED,_case)){
		if(NTrackFound<0)NTrackFound=0;
		// Search for others
		integer npfound=_TrSearcher(1);
		if(npfound){
		  NTrackFound++;
		  if(TRDDBc::patallowS(pat))WeakCaseWanted=false;
		  goto out;
		}

	      }         
	      phit[fp]=phit[fp]->next();
	    }
	  }
	out:
	  phit[0]=phit[0]->next();
	}
      
        
      }
    }
    //  Add proj coord to corr trd clusters
    for(  AMSTRDTrack* ptr=(AMSTRDTrack*)AMSEvent::gethead()->getheadC("AMSTRDTrack",0,0);ptr;ptr=ptr->next()){
      for(int i=0;i<ptr->_Base._NHits;i++){
	//        int proj=ptr->_Base._PCluster[i]->getCooDir()[0]>0.9?0:1;
	int lay=ptr->_Base._PCluster[i]->getlayer();
	int proj= (lay<4 || lay>15)?0:1;
        if(ptr->_Real._FitDone){
	  AMSDir dir(ptr->_Real._Theta, ptr->_Real._Phi);
	  ptr->_Base._PCluster[i]->getCoo()[proj]=
	    ptr->_Real._Coo[proj] +  dir[proj] /dir[2] * (ptr->_Base._PCluster[i]->getCoo()[2]-ptr->_Real._Coo[2]);
	}
	else{
	  AMSDir dir(ptr->_StrLine._Theta,ptr->_StrLine._Phi);
	  ptr->_Base._PCluster[i]->getCoo()[proj]=
	    ptr->_StrLine._Coo[proj] + dir[proj]/dir[2]*(ptr->_Base._PCluster[i]->getCoo()[2]-ptr->_StrLine._Coo[2]);
	}
      }
    }
  }
    if(NTrackFound==0 && !Relax){
       Relax=true;
       _Start();
       goto again;
     }

  return NTrackFound;
}



void AMSTRDTrack::_writeEl(){
  integer flag =    (IOPA.WriteAll%10==1)
                 || (checkstatus(AMSDBc::USED));
  if(Out(flag) ){
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
/*
  TRDTrackNtuple* TrN = AMSJob::gethead()->getntuple()->Get_trdtrk();

  if (TrN->Ntrdtrk>=MAXTRDTRK) return;
   TrN->Status[TrN->Ntrdtrk]=_status;
   for (int i=0;i<3;i++)TrN->Coo[TrN->Ntrdtrk][i]=_StrLine._Coo[i];
   for (int i=0;i<3;i++)TrN->ErCoo[TrN->Ntrdtrk][i]=_StrLine._ErCoo[i];
   TrN->Theta[TrN->Ntrdtrk]=_StrLine._Theta;
   TrN->Phi[TrN->Ntrdtrk]=_StrLine._Phi;
   TrN->Chi2[TrN->Ntrdtrk]=_StrLine._Chi2;
   TrN->NSeg[TrN->Ntrdtrk]=_BaseS._NSeg;
   TrN->Pattern[TrN->Ntrdtrk]=_BaseS._Pattern;

   for (int i=0;i<TRDDBc::nlayS();i++)TrN->pSeg[TrN->Ntrdtrk][i]=0;
   for (int i=0;i<_BaseS._NSeg;i++){
    TrN->pSeg[TrN->Ntrdtrk][i]=_BaseS._PSeg[i]->getpos();
   if(AMSTRDSegment::Out(IOPA.WriteAll%10==1)){
      for(int j=0;j<_BaseS._PSeg[i]->getslayer();j++){
        AMSContainer *pc=AMSEvent::gethead()->getC("AMSTRDSegment",j);
        TrN->pSeg[TrN->Ntrdtrk][i]+=pc->getnelem();
      }
   }
    else{
      //Write only USED hits
      for(int j=0;j<_BaseS._PSeg[i]->getslayer();j++){
        AMSTRDSegment *ptr=(AMSTRDSegment*)AMSEvent::gethead()->getheadC("AMSTRDSegment",j);
        while(ptr && ptr->checkstatus(AMSDBc::USED) ){
         TrN->pSeg[TrN->Ntrdtrk][i]++;
          ptr=ptr->next();
        }
      }
    }
   }
   TrN->Ntrdtrk++;
*/
  }
}

void AMSTRDTrack::_copyEl(){
#ifdef __WRITEROOT__
 if(PointerNotSet())return;
  TrdTrackR & ptr = AMSJob::gethead()->getntuple()->Get_evroot02()->TrdTrack(_vpos);
    for (int i=0; i<_BaseS._NSeg; i++) {
       if(_BaseS._PSeg[i])ptr.fTrdSegment.push_back(_BaseS._PSeg[i]->GetClonePointer()); 
    }
#endif
}

void AMSTRDTrack::_printEl(ostream &o){
o <<_Base._Pattern<<" Nhits "<<_Base._NHits<<endl;
}

integer AMSTRDTrack::pat=0;
 AMSTRDSegment * AMSTRDTrack::phit[trdconst::maxseg];

integer AMSTRDTrack::_TrSearcher(int icall){
           if( _NoMoreTime()){
            throw amsglobalerror(" AMSTRDTrack::_TrSearcher-E-Cpulimit Exceeded ",2);
           }
           
           if(TRDDBc::patpointsS(pat)> icall+1)phit[icall]=AMSTRDSegment::gethead(TRDDBc::patconfS(pat,icall)-1);
           while(phit[icall]){
             if(phit[icall]->Good(AMSDBc::RECOVERED,_case) ){
              if(TRDDBc::patpointsS(pat) >icall+2){         
                return _TrSearcher(++icall); 
              }                
              else{  
                // icall+2 point combination found
                if(_addnext(pat,TRDDBc::patpointsS(pat),phit)){
                  return TRDDBc::patpointsS(pat);
                }
                else if(TRDDBc::patpointsS(pat) <icall+2)return 0;
              }
             }
             phit[icall]=phit[icall]->next();
           }
           return 0;


}


integer AMSTRDTrack::_addnext(integer pat, integer nhit, AMSTRDSegment* pthit[]){

#ifdef __UPOOL__
cerr<< "AMSTRDTrack::_addnext-F-UPOOL not supported yet"<<endl;
abort();
#else
    AMSTRDTrack::TrackBaseS s(pat,nhit,pthit);
    AMSTRDTrack *ptrack=   new AMSTRDTrack(s);
    if(pthit[0]->checkstatus(AMSDBc::RECOVERED))ptrack->setstatus(AMSDBc::RECOVERED);

#endif

    ptrack->StrLineFit();
    if( ptrack->_StrLine._FitDone && ptrack->_StrLine._Chi2< (Relax?10*TRDFITFFKEY.Chi2StrLine:2*TRDFITFFKEY.Chi2StrLine) && ptrack->TOFOK()){
          ptrack->_addnextR();
          return 1;
       }
    delete ptrack;  
    return 0;
}



void AMSTRDTrack::_addnextR(){

         int i;
         // Mark hits as USED
         for( i=0;i<_BaseS._NSeg;i++){
           if(_BaseS._PSeg[i]->checkstatus(AMSDBc::USED))
            _BaseS._PSeg[i]->setstatus(AMSDBc::AMBIG);
           else _BaseS._PSeg[i]->setstatus(AMSDBc::USED);
         }
         RealFit();                  
         // permanently add;
          AMSEvent::gethead()->addnext(AMSID("AMSTRDTrack",0),this);
}


number AMSTRDTrack::Distance3D(AMSPoint p, AMSDir d, AMSTRDCluster *ptr){
  return 1.;
}



integer AMSTRDTrack::Out(integer status){
static integer init=0;
static integer WriteAll=1;
//#pragma omp threadprivate(init,WriteAll)
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
  for(int n=0;n<ntrig;n++){
    if(strcmp("AMSTRDTrack",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
    }
  }
}
return (WriteAll || status);
}


extern "C" void e04ccf_(int &n, number xc[], number &fc, number &tol, int &iw, number w1[],number w2[], number w3[], number w4[], number w5[], number w6[],void * alfun, void * monit, int & maxcal, int &ifail, void * p);

void AMSTRDTrack::StrLineFit(bool upd){

    void (*palfun)(int &n, double xc[], double &fc, AMSTRDTrack *p)=&AMSTRDTrack::alfun;
    void (*pmonit)(number &a, number &b, number sim[], int &n, int &s, int &nca)=&AMSTRDTrack::monit;
    _Base._NHits=0;    
     _StrLine._FitDone=false;
  // Fit Here
    const integer mp=40;
    number f,x[mp],w1[mp],w2[mp],w3[mp],w4[mp],w5[mp+1],w6[mp*(mp+1)];
    number yy=0;
    number xx=0;
    number yz=0;
    number xz=0;
    number yz2=0;
    number xz2=0;
    number nyz=0;
    number nxz=0;
   if(_BaseS._NSeg>2){
    for (int i=0;i<_BaseS._NSeg;i++){
     for(int k=0;k<_BaseS._PSeg[i]->getNHits();k++){
      if(fabs(_BaseS._PSeg[i]->getpHit(k)->getCooDir()[0])>0.9){
        yz+=-_BaseS._PSeg[i]->getFitPar(0);      
        yy+=-_BaseS._PSeg[i]->getFitPar(1);      
        yz2+=(_BaseS._PSeg[i]->getFitPar(0))*(_BaseS._PSeg[i]->getFitPar(0));          nyz++;
      }
      else if(fabs(_BaseS._PSeg[i]->getpHit(k)->getCooDir()[1])>0.9){
        xz+=_BaseS._PSeg[i]->getFitPar(0);      
        xx+=_BaseS._PSeg[i]->getFitPar(1);      
        xz2+=(_BaseS._PSeg[i]->getFitPar(0))*(_BaseS._PSeg[i]->getFitPar(0));          nxz++;
      }
      _Base._PCluster[_Base._NHits++]=_BaseS._PSeg[i]->getpHit(k);
     }
    } 
    if(_Base._NHits<TRDFITFFKEY.MinFitPoints)return;
    if(nxz){
     xz/=nxz;
     xx/=nxz;
     xz2/=nxz;
     if(sqrt(fabs(xz2-xz*xz))>(Relax?TRDFITFFKEY.TwoSegMatch*6:TRDFITFFKEY.TwoSegMatch))return ;
    }     
    if(nyz){
     yz/=nyz;
     yy/=nyz;
     yz2/=nyz;
     if(sqrt(fabs(yz2-yz*yz))>(Relax?TRDFITFFKEY.TwoSegMatch*6:TRDFITFFKEY.TwoSegMatch))return ;
    }     

    if(!nyz || !nxz){
     cerr<<"AMSTRDTrack::StrLineFit-F-TRDIsRotated-FindSmarterAlgorithmPlease"<<endl;
     //abort();     
    }
   }

else{
     number yz,yy,xz,xx;
    for (int i=0;i<_BaseS._NSeg;i++){
      if(fabs(_BaseS._PSeg[i]->getpHit(0)->getCooDir()[0])>0.9){
        yz=-_BaseS._PSeg[i]->getFitPar(0);      
        yy=-_BaseS._PSeg[i]->getFitPar(1);      
      }
      else if(fabs(_BaseS._PSeg[i]->getpHit(0)->getCooDir()[1])>0.9){
        xz=_BaseS._PSeg[i]->getFitPar(0);      
        xx=_BaseS._PSeg[i]->getFitPar(1);      
      }
     for(int k=0;k<_BaseS._PSeg[i]->getNHits();k++){
      _Base._PCluster[_Base._NHits++]=_BaseS._PSeg[i]->getpHit(k);
     }
    } 
    if(_Base._NHits<TRDFITFFKEY.MinFitPoints)return;
}
    AMSPoint _Exit(0,0,_Base._PCluster[0]->getCoo()[2]);
    AMSPoint _Entry(0,0,_Base._PCluster[_Base._NHits-1]->getCoo()[2]);
    _Entry[0]=xx+xz*_Entry[2];
    _Exit[0]=xx+xz*_Exit[2];
    _Entry[1]=yy+yz*_Entry[2];
    _Exit[1]=yy+yz*_Exit[2];
    AMSDir Dir=_Exit-_Entry;
    AMSPoint Center=_Exit+_Entry;
    Center=Center*0.5;   
    x[0]=Center[0];
    x[1]=Center[1];
    x[2]=Center[2];
    x[3]=Dir.getphi();
    x[4]=Dir.gettheta();
      
  // number of parameters to fit
    integer n=5;
    integer iw=n+1;
    integer ifail=1;
    integer maxcal=1000;
    number tol=2.99e-2;
    int i,j;
    _update=false;
#ifndef NO_NAG    
    e04ccf_(n,x,f,tol,iw,w1,w2,w3,w4,w5,w6,(void*)palfun,(void*)pmonit,maxcal,ifail,(void*)this);
#endif
    if(ifail==0){
     _update=true;
     _StrLine._Coo[0]=x[0];
     _StrLine._Coo[1]=x[1];
     _StrLine._Coo[2]=x[2];
     alfun(n,x,f,this);
     if(upd )_StrLine._Chi2=f;
     for(int i=0;i<3;i++)_StrLine._ErCoo[i]=_Base._PCluster[0]->getEHit()*sqrt
(_StrLine._Chi2/(_Base._NHits-2)+1.);
      AMSDir s(x[4],x[3]);
//    Default go in
     if(s[2]>0)s=s*(-1);
      number z2=(_Base._PCluster[0]->getCoo()[2]+_Base._PCluster[_Base._NHits-1]->getCoo()[2])/2;
      _StrLine._Coo=_StrLine._Coo+s*(z2-_StrLine._Coo[2])/s[2];
     _StrLine._Phi=s.getphi();
     _StrLine._Theta=s.gettheta();
     _StrLine._InvRigidity=0;
     _StrLine._FitDone=true;
    }
    else{
     _StrLine._Chi2=FLT_MAX;
    }
}


void AMSTRDTrack::alfun(integer &n, number xc[], number &fc, AMSTRDTrack *p){
  fc=0;
/*
  for (int i=3;i<6;i++){
   if(fabs(xc[i]>1)){
    fc=FLT_MAX;
    return;
   }
  }
*/
  integer nh=0;
  AMSDir sdir(xc[4],xc[3]);
  AMSPoint sp(xc[0],xc[1],xc[2]);
  for (int i=0;i<p->_Base._NHits;i++){
   if(!p->_Base._PCluster[i]->checkstatus(AMSDBc::DELETED)){
    nh++;
     if(!p->_update){
      AMSPoint dc=sp-p->_Base._PCluster[i]->getCoo();
      AMSDir gamma=sdir.cross(p->_Base._PCluster[i]->getCooDir());
     number d;
     if(gamma.norm()!=0){
      d=dc.prod(gamma);
     }
     else{
       AMSPoint beta= sdir.crossp(dc);
       d=beta.norm();
     }
      fc+=d*d/p->_Base._PCluster[i]->getEHit()/p->_Base._PCluster[i]->getEHit();
    }
    else{
     AMSPoint dc=sp-p->_Base._PCluster[i]->getCoo();
     AMSPoint alpha=sdir.crossp(p->_Base._PCluster[i]->getCooDir());
     AMSPoint beta= sdir.crossp(dc);
     number t=alpha.prod(beta)/alpha.prod(alpha);
      number delta=5;
     if(p->_Base._PCluster[i]->getmult()==1){
     if(t>p->_Base._PCluster[i]->getHitL()+delta)t=p->_Base._PCluster[i]->getHitL()+delta;
     else if (t<-p->_Base._PCluster[i]->getHitL()-delta)t=-p->_Base._PCluster[i]->getHitL()-delta;
     }
      p->_Base._Hit[i]=(p->_Base._PCluster[i]->getCoo()+p->_Base._PCluster[i]->getCooDir()*t);
      p->_Base._EHit[i]=AMSPoint(p->_Base._PCluster[i]->getEHit(),p->_Base._PCluster[i]->getEHit(),p->_Base._PCluster[i]->getEHit());
      AMSPoint dc2=sp-p->_Base._Hit[i];
      number d1=sdir.prod(dc2);
      d1=dc2.prod(dc2)-d1*d1;
      if(p->_Base._PCluster[i]->getEHit())fc+=d1/p->_Base._PCluster[i]->getEHit()/p->_Base._PCluster[i]->getEHit();
      
    }
    }   
  }
  if(nh>1)fc/=nh-1;
}

void AMSTRDTrack::RealFit(){
  _Real._FitDone=false;
  integer fit=0;
  integer ims=0;
  integer npt=_Base._NHits;
  const integer maxhits=trdconst::maxlay;
  geant hits[maxhits][3];
  geant sigma[maxhits][3];
  geant normal[maxhits][3];
  integer layer[maxhits];
  integer ialgo=1;
  geant out[40];
  for(int i=0;i<npt;i++){
    normal[i][0]=0;
    normal[i][1]=0;
    normal[i][2]=-1;
    layer[npt-1-i]=_Base._PCluster[i]->getlayer();
    for(int j=0;j<3;j++){
      hits[npt-1-i][j]=_Base._Hit[i][j];
      sigma[npt-1-i][j]=_Base._EHit[i][j];
    }
  }
  int ipart=14;
#ifdef _PGTRACK_
  //PZ FIXME 
  TrFit fitg;
  float zmax=0;
  for (int ii=0;ii<npt;ii++){
    fitg.Add(hits[ii][0],hits[ii][1],hits[ii][2],
             sigma[ii][0],sigma[ii][1],sigma[ii][2],0.,0.,0.);
    if(hits[ii][2]>zmax) hits[ii][2]=zmax;
  }
   
  
  fitg.LinearFit();
  out[7]=fitg.GetChisq();
 
 
  _Real._FitDone=true;
  _Real._Chi2=fitg.GetChisq();

  _Real._InvRigidity=fitg.GetRigidity();
  _Real._ErrInvRigidity=fitg.GetErrRinv();
  _Real._Theta= fitg.GetTheta();
  _Real._Phi=fitg.GetPhi();
  
  fitg.Propagate(zmax);
  _Real._Coo=AMSPoint(fitg.GetP0x(),
                      fitg.GetP0y(),
                      fitg.GetP0z());
 
 
#else
  zeromagstat_();
  TKFITG(npt,hits,sigma,normal,ipart,ialgo,ims,layer,out);
  resetmagstat_();
  _Real._Chi2=out[6];
  if(out[7] != 0)_Real._Chi2=FLT_MAX;
  else{
    _Real._FitDone=true;
    _Real._Chi2=out[6];
  }
  _Real._InvRigidity=1/out[5];
  _Real._ErrInvRigidity=out[8];
  _Real._Theta=out[3];
  _Real._Phi=out[4];
  _Real._Coo=AMSPoint(out[0],out[1],out[2]);
#endif

}

geant AMSTRDTrack::_Time=0;
geant AMSTRDTrack::_TimeLimit=0;


bool AMSTRDTrack::_NoMoreTime(){
return _TimeLimit>0? _CheckTime()>_TimeLimit: _CheckTime()>AMSFFKEY.CpuLimit;
}



bool AMSTRDTrack::IsEcalCandidate(){

// very stupid function;
// should be replaced by smth more reasonable
   int hmul=0;
   int mul=0;
   for(int i=0;i<_Base._NHits;i++){
    mul+=_Base._PCluster[i]->getmult();
    hmul+=_Base._PCluster[i]->gethmult();
   }
   if(hmul>trdconst::maxlay/5 && hmul<trdconst::maxlay*4/5+1)return true;
   else return false;   

}


bool AMSTRDTrack::IsHighGammaTrack(){
 uinteger hmul=0;
 uinteger threshold=3;
 for (int i=0;i<_Base._NHits;i++){
  if(_Base._PCluster[i]->getmult()==1)hmul+=_Base._PCluster[i]->gethmult();
 }
 return hmul>threshold;
}




bool AMSTRDTrack::Veto(int last){
 for (int i=0;i<_Base._NHits;i++){
  if(_Base._PCluster[i]->getlayer()>last)return false;
 }
 return true;
}


void AMSTRDTrack::ComputeCharge(double beta){
 vector <double> edepc;
 if(_StrLine._FitDone){
 for (int i=0;i<_Base._NHits;i++){
   AMSTRDCluster *pcl=_Base._PCluster[i];
   if(pcl->getmult()<3){
      double R=TRDDBc::TubeInnerDiameter()/2;
      AMSPoint  c1=pcl->getCoo();
AMSPoint c2=_StrLine._Coo;
AMSPoint delta=c1-c2;
AMSDir n=pcl->getCooDir();
AMSDir l(_StrLine._Theta,_StrLine._Phi);
double corr1=n[1]>0?l[1]:l[0];
corr1=fabs(l[2])/sqrt(l[2]*l[2]+corr1*corr1);
double nl=n.prod(l);
double dn=delta.prod(n);
double dl=delta.prod(l);
double d2=delta.prod(delta);
double a=1-nl*nl;
double b=dn*nl-dl;
double c=d2-dn*dn-R*R;
double rmin2=-b*b/a+(d2-dn*dn);
double d=b*b-a*c;
double range=d<0?0:2*sqrt(d)/a;
            double norm=AMSTRDCluster::RangeCorr(0.57,1.);        
            double e=pcl->getEdep()/AMSTRDCluster::RangeCorr(range,norm);
            edepc.push_back(e);         

   }
 }

 if(edepc.size()>TRDFITFFKEY.MinFitPoints){

   sort(edepc.begin(),edepc.end());
   double medianc=0;            
   if(edepc.size()%2)for(int k=edepc.size()/2-2;k<edepc.size()/2+3;k++)medianc+=edepc[k]/5;
   else for(int k=edepc.size()/2-2;k<edepc.size()/2+2;k++)medianc+=edepc[k]/4;
   _Charge.Q=sqrt(medianc/beta)*TRDFITFFKEY.QP[0]+TRDFITFFKEY.QP[1];
 }


   //lkhd
   const int span =sizeof(TrackCharge::ChargePDF)/sizeof(TrackCharge::ChargePDF[0])/(sizeof(_Charge.Charge)/sizeof(_Charge.Charge[0]));
   for (int k=0;k<sizeof(_Charge.Charge)/sizeof(_Charge.Charge[0]);k++){
    if(TrackCharge::ChargePDF[k*span+span-1]){
    _Charge.Charge[k]=TrackCharge::ChargePDF[k*span+span-2];  
    _Charge.ChargeP[k]=0;
    for(int i=0;i<edepc.size();i++){
      int ch=edepc[i]/beta/TrackCharge::ChargePDF[k*span+span-3];
      if(ch<0)ch=0;
      if(ch>span-3)ch=span-3;
      _Charge.ChargeP[k]+=-log(TrackCharge::ChargePDF[k*span+ch]);
    }
    }
    else{
      for(int l=k;l>0;l--){
        if(TrackCharge::ChargePDF[l*span+span-1]){
         _Charge.Charge[k]=TrackCharge::ChargePDF[k*span+span-2];  
         _Charge.ChargeP[k]=0;
         float factor=_Charge.Charge[l]/_Charge.Charge[k];
          for(int i=0;i<edepc.size();i++){
            int ch=edepc[i]*factor*factor/beta/TrackCharge::ChargePDF[l*span+span-3];
            if(ch<0)ch=0;
            if(ch>span-3)ch=span-3;
              _Charge.ChargeP[k]+=-log(TrackCharge::ChargePDF[l*span+ch]);
          }
         break;
        }
      }
    }
    
   }
   double prsum=0;
   for(int k=0;k<sizeof(_Charge.Charge)/sizeof(_Charge.Charge[0]);k++){
     prsum+=exp(-double(_Charge.ChargeP[k]));
   }
    
   for(int k=0;k<sizeof(_Charge.Charge)/sizeof(_Charge.Charge[0]);k++){
     _Charge.ChargeP[k]+=log(prsum);
   }

   multimap<float,short int> chmap;
   for(int k=0;k<sizeof(_Charge.Charge)/sizeof(_Charge.Charge[0]);k++){
     chmap.insert(make_pair(_Charge.ChargeP[k],_Charge.Charge[k]));
   }
   int l=0;
   for(multimap<float,short int>::iterator k=chmap.begin();k!=chmap.end();k++){
        _Charge.Charge[l]=k->second;
        _Charge.ChargeP[l++]=k->first;
   }

   _Charge.Nused = edepc.size();
 
}



 

}

double AMSTRDCluster::RangeCorr(double range, double norm){
//  double p[3]={2.83105,-2.56767,+4.53374};
//  double b[2]={0.3,0.75};
   double corr=1;
    double rng=range;
  if(rng<TRDCLFFKEY.RNGB[0])rng=TRDCLFFKEY.RNGB[0];
  if(rng>TRDCLFFKEY.RNGB[1])rng=TRDCLFFKEY.RNGB[1];
  
  for(int i=0;i<sizeof(TRDCLFFKEY.RNGP)/sizeof(TRDCLFFKEY.RNGP[0]);i++){
    corr+=TRDCLFFKEY.RNGP[i]*pow(rng,double(i));
  }
  return corr/norm;
}
float AMSTRDTrack::TrackCharge::ChargePDF[10030];
bool AMSTRDTrack::CreatePDF(char *fnam){

ifstream file;

if(fnam)file.open(fnam);
else if(getenv("TRDChargePDFFile")){
file.open(getenv("TRDChargePDFFile"));
}
else{
 cerr<<"AMSTRDTrack::Charge::ChargePDF-E-FileNotDefined"<<endl;
return false;
}
if(!file){
 cerr<<"AMSTRDTrack::Charge::ChargePDF-E-UnableToOpen "<<fnam <<" "<<getenv("TRDChargePDFFile")<<endl;
return false;
}
for(int k=0;k<sizeof(TrackCharge::ChargePDF)/sizeof(TrackCharge::ChargePDF[0]);k++)TrackCharge::ChargePDF[k]=0;
int ptr=0;
const int span=1003;
while(file.good() && !file.eof()){

file>>TrackCharge::ChargePDF[ptr*span+span-3];  //e2c
file>>TrackCharge::ChargePDF[ptr*span+span-2];  //id
file>>TrackCharge::ChargePDF[ptr*span+span-1];  //ok
for(int k=0;k<span-3;k++)file>>TrackCharge::ChargePDF[ptr*span+k];
double smax=0;
for(int k=0;k<span-3;k++){
  float a=TrackCharge::ChargePDF[ptr*span+k];
  if(a<=0){
    float sum=0;
    for(int j=k;j<span-3;j++){
      if(TrackCharge::ChargePDF[ptr*span+j]<10)sum+=TrackCharge::ChargePDF[ptr*span+j];
    }
    for(int j=k;j<span-3;j++){
        if(TrackCharge::ChargePDF[ptr*span+j]<10)TrackCharge::ChargePDF[ptr*span+j]=(sum==0?1:sum)/(span-3-k);
      }
    }
 }
for(int k=0;k<span-3;k++){
smax+=TrackCharge::ChargePDF[ptr*span+k];
}
for(int k=0;k<span-3;k++)TrackCharge::ChargePDF[ptr*span+k]/=smax;
if(TrackCharge::ChargePDF[ptr*span+span-1])ptr++;


}

cout <<"AMSTRDTrack::Charge::ChargePDF-I- "<<ptr<<" pdf loaded for ";
for(int k=0;k<ptr;k++)cout<<TrackCharge::ChargePDF[k*span+span-2]<<" ";
cout <<endl;
if(ptr<3){
 cerr<<"AMSTRDTrack::Charge::ChargePDF-E-minimal 3 PDF Needed "<<endl;
 return false;
}
for(int i=ptr;i<sizeof(TrackCharge::ChargePDF)/sizeof(TrackCharge::ChargePDF[0])/span;i++){
  TrackCharge::ChargePDF[i*span+span-2]=i;
}

return true;
}

bool AMSTRDTrack::ResolveAmb(AMSTrTrack *_ptrack){


if (AMSEvent::gethead()->getC("AMSTRDTrack",0)->getnelem()<2)return false;

//try to find two projections of two tracks which match with track

double theta,phi,sleng;
  AMSPoint cmin(0,0,85.);
  AMSPoint cmax(0,0,140.);
  AMSDir dir(0,0,1.);
  AMSPoint cmini,cmaxi;
 #ifdef _PGTRACK_
    _ptrack->Interpolate(cmin[2], cmini,dir);
    _ptrack->Interpolate(cmax[2], cmaxi,dir);
#else
_ptrack->interpolate(cmin,dir,cmini,theta,phi,sleng);
 _ptrack->interpolate(cmax,dir,cmaxi,theta,phi,sleng);
#endif
  AMSTRDTrack* pc[2]={0,0};
   number searchreg[2]={4,6}; 
for( AMSTRDTrack* ptr=(AMSTRDTrack*)AMSEvent::gethead()->getheadC("AMSTRDTrack",0,0);ptr;ptr=ptr->next()){
    dir=ptr->getCooDirStr();  
    AMSPoint coo=ptr->getCooStr();  
    AMSPoint cplus=AMSPoint(1,1,1)*dir;
    cmax=coo+cplus*(cmaxi[2]-coo[2])/dir[2];
    cmin=coo+cplus*(cmini[2]-coo[2])/dir[2];
    for(int k=0;k<2;k++){
      if(fabs(cmin[k]-cmini[k])<searchreg[k] && fabs(cmax[k]-cmaxi[k])<searchreg[k]){
       searchreg[k]=max(fabs(cmin[k]-cmini[k]),fabs(cmin[k]-cmini[k]));
       pc[k]=ptr;
     }
    }
}
if(pc[0] && pc[1]){
if(pc[0]==pc[1]){
  // some inconsistency
  static int err=0;
  if(err++<100){
   cerr<<"AMSTRDTrack::ResolveAmb-E-OneTrack Match "<<searchreg[0]<<" "<<searchreg[1]<<endl;
 }
}
else{
// make two new tracks  by interchanging segments
AMSTRDTrack* p[2]={0,0};
for(int i=0;i<2;i++){
AMSTRDSegment *pthit[10];
int nseg=0;
for(int k=0;k<2;k++){
for(int l=0;l<pc[k]->_BaseS._NSeg;l++){
 if(pc[k]->_BaseS._PSeg[l]->getori()==(i==0?k:1-k)){
  pthit[nseg++]=pc[k]->_BaseS._PSeg[l];
 }

}
}
    AMSTRDTrack::TrackBaseS s(-1,nseg,pthit);
    p[i]=new AMSTRDTrack(s);
    p[i]->setstatus(AMSDBc::RECOVERED);
    p[i]->StrLineFit(false);
    if(p[i]->_StrLine._FitDone && p[i]->_StrLine._Chi2< 2*TRDFITFFKEY.Chi2StrLine){
          p[i]->_addnextR();


      for(int ii=0;ii<p[i]->_Base._NHits;ii++){
	int lay=p[i]->_Base._PCluster[ii]->getlayer();
	int proj= (lay<4 || lay>15)?0:1;
        if(p[i]->_Real._FitDone){
	  AMSDir dir(p[i]->_Real._Theta, p[i]->_Real._Phi);
	  p[i]->_Base._PCluster[ii]->getCoo()[proj]=
	    p[i]->_Real._Coo[proj] +  dir[proj] /dir[2] * (p[i]->_Base._PCluster[ii]->getCoo()[2]-p[i]->_Real._Coo[2]);
	}
	else{
	  AMSDir dir(p[i]->_StrLine._Theta,p[i]->_StrLine._Phi);
	  p[i]->_Base._PCluster[ii]->getCoo()[proj]=
	    p[i]->_StrLine._Coo[proj] + dir[proj]/dir[2]*(p[i]->_Base._PCluster[ii]->getCoo()[2]-p[i]->_StrLine._Coo[2]);
	}
      }





  }
   else{
     delete p[i];
     p[i]=0;
   }
}
if(p[0] && p[1]){
for(int k=0;k<2;k++){
 pc[k]->setstatus(AMSDBc::DELETED);
}
}
if(p[0] || p[1])return true;
}
}


return false;
}

bool AMSTRDTrack::Relax=false;



integer AMSTRDTrack::TOFOK(){

    if (TRDFITFFKEY.UseTOF && !Relax){
   // Cycle thru all TOF clusters;
   // at least UseTOF of them should be matched with the track
   integer i;
   integer matched=0;
   // Take cut from beta defs
   AMSPoint SearchReg(BETAFITFFKEY.SearchReg[0],BETAFITFFKEY.SearchReg[1],
   BETAFITFFKEY.SearchReg[2]);
    AMSDir dir=getCooDirStr();  
    AMSPoint coo=getCooStr();  
    AMSPoint cplus=AMSPoint(1,1,1)*dir;

   for(i=0;i<4;i++){
    AMSTOFCluster * phit = AMSTOFCluster::gethead(i);
    while (phit){
      AMSPoint Res=coo+cplus*(phit->getcoo()[2]-coo[2])/dir[2];
     if( ((phit->getcoo()-Res)/phit->getecoo()).abs()< SearchReg){
     if(phit->getntof() < 3)matched+=1;  
     else matched+=10;
     }
     phit=phit->next();
    } 
   }
   switch (TRDFITFFKEY.UseTOF){
   case 0:
    return 1;
   case 1:
    if(matched/10 <1 && matched%10 <1)return 0;
    break;
   case 2:
    if(matched/10 <1 || matched%10<1)return 0;
    break;
   case 3:
    if(matched/10 <2 && matched%10<2)return 0;
    break;
   case 4:
    if(matched/10 <2 || matched%10<2)return 0;
    break;
   }
   return 1;  
    }
  else return 1;
}

