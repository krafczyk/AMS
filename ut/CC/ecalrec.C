// v0.0 28.09.1999 by E.Choumilov
//
#include <iostream.h>
#include <stdio.h>
#include <typedefs.h>
#include <cern.h>
#include <extC.h>
#include <math.h>
#include <point.h>
#include <amsgobj.h>
#include <event.h>
#include <amsstl.h>
#include <commons.h>
#include <ntuple.h>
#include <ecaldbc.h>
#include <ecalrec.h>
#include <mccluster.h>
#include <trigger102.h>
#include <trigger3.h>
//
uinteger AMSEcalRawEvent::trigfl=0;// just memory reservation/initialization for static
//----------------------------------------------------
void AMSEcalRawEvent::validate(int &stat){ //Check/correct RawEvent-structure
  uinteger ecalflg;
  integer tofflg;
  Trigger2LVL1 *ptr;
//
  ptr=(Trigger2LVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
  if(ptr){
    tofflg=ptr->gettoflg();
    ecalflg=ptr->getecflg();
    HF1(ECHIST+30,geant(ecalflg),1.);
  }
//
  stat=0;
}
//----------------------------------------------------
void AMSEcalRawEvent::mc_build(int &stat){
  int i,j,k;
  integer fid,cid,cidar[4],nhits,nraw,il,pm,sc,proj,rdir;
  number x,y,z,coo,hflen,pmdis,edep,edepr,edept,edeprt,emeast,time,timet(0.);
  number attf,ww[4],anen,dyen;
  number sum[ECPMSMX][4],pmtmap[ECSLMX][ECPMSMX],pmlprof[ECSLMX];
  integer zhitmap[ECSLMX];
  integer adch,adcm,adcl;
  geant radc,a2dr,h2lr,mev2adc,pmrgn,scgn;
  geant pedh[4],pedl[4],sigh[4],sigl[4];
  AMSEcalMCHit * ptr;
  integer id,sta,adc[2];
  number dyresp,dyrespt;// dynode resp. in mev(~mV) (for trigger)
  number an4resp,an4respt;// (for trigger)
//
  stat=1;//bad
  trigfl=0;//reset tigger-flag
  edept=0.;
  edeprt=0.;
  emeast=0.;
  nhits=0;
  nraw=0;
  an4respt=0;
  dyrespt=0;
  timet=0.;
  for(il=0;il<ECSLMX;il++){
    pmlprof[il]=0.;
    zhitmap[il]=0;
    for(i=0;i<ECPMSMX;i++)pmtmap[il][i]=0.;
  }
//
  for(il=0;il<ECALDBc::slstruc(3);il++){ // <-------------- super-layer loop
    ptr=(AMSEcalMCHit*)AMSEvent::gethead()->
               getheadC("AMSEcalMCHit",il,0);
    for(i=0;i<ECALDBc::slstruc(4);i++)
                                     for(k=0;k<4;k++)sum[i][k]=0.;
    while(ptr){ // <------------------- geant-hits loop in superlayer:
      nhits+=1;
      fid=ptr->getid();//SSLLFFF
      edep=ptr->getedep()*1000;// MeV(dE/dX)
      edept+=edep;
      time=(1.e+9)*(ptr->gettime());// geant-hit time in ns
      timet+=edep*time;
      x=ptr->getcoo(0);// global coord.
      y=ptr->getcoo(1);
      x=x-ECALDBc::gendim(5);// go to local (ECAL-radiator) system
      y=y-ECALDBc::gendim(6);
      proj=(1-2*(il%2))*(1-2*ECALDBc::slstruc(1));//proj=+1/-1=>Y/X
      if(proj>0){ // <-- fiber from Y-proj
        coo=x;// get X-coord(along fiber)
        hflen=ECALDBc::gendim(1)/2.;// 0.5*fiber length in X-dir
      }
      else{                                          //<-- fiber from X-proj
        coo=y;// get Y-coord(along fiber)
        hflen=ECALDBc::gendim(2)/2.;// 0.5*fiber length in Y-dir
      }
//
      for(k=0;k<4;k++){
        cidar[k]=0;
	ww[k]=0.;
      }
      ECALDBc::fid2cida(fid,cidar,ww);//cidar=SSPPC
//
      for(j=0;j<4;j++){ // <-- loop over coupled PM's
        cid=cidar[j];
        if(cid>0){
          sc=cid%10-1; // SubCell(pixel)
          pm=(cid/10)%100-1; // PM
//
          if(proj>0)rdir=(1-2*(pm%2))*ECALDBc::slstruc(6);//+-1 readout dir(along pos/neg Y)
          else rdir=(1-2*(pm%2))*ECALDBc::slstruc(5);//+-1 readout dir(along pos/neg X)
//
          pmdis=coo+hflen;//to count from "-" edge of fiber (0-2*hflen)
          if(rdir<0)pmdis=2.*hflen-pmdis;
          attf=(1.-ECALDBc::rdcell(3))*exp(-pmdis/ECALDBc::rdcell(1))
            +ECALDBc::rdcell(3)*exp(-pmdis/ECALDBc::rdcell(2));//fiber attenuation factor
          edepr=edep*attf*ww[j];
          sum[pm][sc]+=edepr;
          edeprt+=edepr;
	}
      }// -----> end of the coupled PM's loop
//
        ptr=ptr->next(); 
    } // ---------------> end of geant-hit-loop in superlayer
//
    for(i=0;i<ECALDBc::slstruc(4);i++){ // <------- loop over PM's in this(il) S-layer
      a2dr=ECcalib::ecpmcal[il][i].an2dyr();//take some param.from DB
      mev2adc=1./ECcalib::ecpmcal[il][i].adc2mev();
      pmrgn=ECcalib::ecpmcal[il][i].pmrgain();
      ECPMPeds::pmpeds[il][i].getpedh(pedh);
      ECPMPeds::pmpeds[il][i].getsigh(sigh);
      ECPMPeds::pmpeds[il][i].getpedl(pedl);
      ECPMPeds::pmpeds[il][i].getsigl(sigl);
//
      an4resp=0;//PM Anode-resp(4cells,tempor in mev)
      dyresp=0;//PM Dynode-resp(tempor in mev)
      anen=0.;
      dyen=0.;
      for(k=0;k<4;k++){//<--- loop over 4-subcells in PM
        h2lr=ECcalib::ecpmcal[il][i].hi2lowr(k);//PM subcell high2/low ratio from DB
	scgn=ECcalib::ecpmcal[il][i].pmscgain(k);//PM SubCell relative(to average) gain from DB
        edepr=sum[i][k]*ECALDBc::mev2mev();//Geant_dE/dX(Mev)->Emeas(Mev)
	emeast+=edepr;
	anen+=edepr;
	dyen+=edepr;
// ---------> digitization+DAQ-scaling:
// High-gain channel:
	radc=edepr*pmrgn*scgn*mev2adc+pedh[k]+sigh[k]*rnormx();//Em(mev)->Em(adc)+noise
	adch=floor(radc);//"digitization"
	if(adch>=4095)adch=4095;//"ADC-saturation (12 bit)"
	radc=number(adch)-pedh[k];// ped-subtraction
        if(radc>=ECALVarp::ecalvpar.daqthr(0)){// use only hits above DAQ-readout threshold
	  adch=floor(radc*ECALDBc::scalef());//DAQ scaling
	}
	else{ adch=0;}
// Low-gain channel:
	radc=edepr*pmrgn*scgn*mev2adc/h2lr+pedl[k]+sigl[k]*rnormx();//Em(mev)->Em/h2lr(adc)+noise
	adcl=floor(radc);//"digitization")
	if(adcl>=4095)adcl=4095;//"ADC-saturation (12 bit)"
	radc=number(adcl)-pedl[k];// ped-subtraction
        if(radc>=ECALVarp::ecalvpar.daqthr(4)){// use only hits above DAQ-readout threshold
	  adcl=floor(radc*ECALDBc::scalef());//DAQ scaling
	}
	else{ adcl=0;}
// <---------
	if(adch>0 || adcl>0){
	  nraw+=1;
	  id=(k+1)+10*(i+1)+1000*(il+1);// SSPPC
	  sta=0;
	  adc[0]=adch;
	  adc[1]=adcl;
          AMSEvent::gethead()->addnext(AMSID("AMSEcalRawEvent",il), new
                                             AMSEcalRawEvent(id,sta,adc));
	}
      }//<---- end of PMSubcell-loop
//
      an4resp=anen;//tempor! anode resp(true mev ~ mV !!!)(later to add some factor to conv. to mV)
      dyresp=dyen/a2dr;//tempor! dynode resp(false mev ~ real mV !!!)
      an4respt+=an4resp;
      dyrespt+=dyresp;
      if(ECMCFFKEY.mcprtf==1){
        HF1(ECHIST+5,geant(dyresp),1.);
        if(dyresp>0.)HF1(ECHIST+6,geant(an4resp/dyresp),1.);
      }
//            arrays for trigger study:
      pmtmap[il][i]=an4resp;//tempor 4xAnode-resp(later dynode-resp)
      pmlprof[il]+=an4resp;// 
//
    }//-------> end of PM-loop in superlayer
//
  } // ------------> end of super-layer loop
//
//                          <--- some variables for "electromagneticity" calc.
  geant e4x0,epeaka,etaila,rrr;
  e4x0=pmlprof[0]+pmlprof[1];//energy in 1st 4X0's(sl1+sl2) of Z-profile
  epeaka=(pmlprof[1]+pmlprof[2])/2.;//aver.energy in peak(sl2+sl3) of Z-profile
  etaila=pmlprof[ECALDBc::slstruc(3)-1];//aver.energy in tail(sl9 now) ...
  rrr=0.;
  if(epeaka>0.)rrr=etaila/epeaka;
  if(rrr>0.99)rrr=0.99;
//
  if(ECMCFFKEY.mcprtf==1){
    HF1(ECHIST+1,geant(nhits),1.);
    HF1(ECHIST+2,geant(edept),1.);
    HF1(ECHIST+3,geant(edeprt),1.);
    HF1(ECHIST+4,geant(emeast),1.);
    HF1(ECHIST+7,e4x0,1.);
    HF1(ECHIST+8,rrr,1.);
  }
//  ---> create ECAL H/W-trigger(Ec<MIP,Ec>=MIP"em-type",HighEn in EC):
//
  if(an4respt>ECALVarp::ecalvpar.daqthr(1)){
    trigfl=1;// at least MIP (some non-zero activity in EC)
    if(e4x0>ECALVarp::ecalvpar.daqthr(2)){
      trigfl=2;// EM-object !
      if(an4respt<ECALVarp::ecalvpar.daqthr(5) &&
                       rrr>ECALVarp::ecalvpar.daqthr(6))trigfl=1;//cut on Etail/Epeak !
      if(an4respt<ECALVarp::ecalvpar.daqthr(7))trigfl=1;//cut on Etot !
      if(an4respt>ECALVarp::ecalvpar.daqthr(3))trigfl+=10;// High-En in ECAL
    }
  }
  if(trigfl==0)return;//No signal in ECAL
  stat=0;
//
//---
  if(ECMCFFKEY.mcprtf==1)HF1(ECHIST+9,geant(trigfl),1.);
  return;
}
//---------------------------------------------------
void AMSEcalHit::build(int &stat){
  int i,j,k;
  integer sta,status,dbstat,padc[2],id,idd,isl,pmc,subc,nraw,nhits;
  integer proj,plane,cell,icont,adcmx;
  number radc[2],edep,adct,fadc,emeast,coot,cool,cooz; 
  geant pedh[4],pedl[4],sigh[4],sigl[4],h2lr;
  integer ovfl[2]={0,0};
  AMSEcalRawEvent * ptr;
//
  stat=1;//bad
  adcmx=4095.;
  nhits=0;
  nraw=0;
  adct=0.;
  emeast=0.;
//
  for(isl=0;isl<ECALDBc::slstruc(3);isl++){ // <-------------- super-layer loop
    ptr=(AMSEcalRawEvent*)AMSEvent::gethead()->
               getheadC("AMSEcalRawEvent",isl,0);
    while(ptr){ // <--- RawEvent-hits loop in superlayer:
      nraw+=1;
      id=ptr->getid();//SSPPC
      idd=id/10;
      subc=id%10-1;//SubCell(0-3)
      pmc=idd%100-1;//PMCell(0-...)
      ptr->getpadc(padc);
      ECPMPeds::pmpeds[isl][pmc].getpedh(pedh);
      ECPMPeds::pmpeds[isl][pmc].getsigh(sigh);
      ECPMPeds::pmpeds[isl][pmc].getpedl(pedl);
      ECPMPeds::pmpeds[isl][pmc].getsigl(sigl);
      h2lr=ECcalib::ecpmcal[isl][pmc].hi2lowr(subc);
      radc[0]=number(padc[0])/ECALDBc::scalef();//DAQ-format-->ADC-high-chain
      radc[1]=number(padc[1])/ECALDBc::scalef();//DAQ-format-->ADC-low-chain
      if(radc[0]>0.)if((adcmx-(radc[0]+pedh[subc]))<4.*sigh[subc])ovfl[0]=1;// mark as ADC-Overflow
      if(radc[1]>0.)if((adcmx-(radc[1]+pedl[subc]))<4.*sigl[subc])ovfl[1]=1;// mark as ADC-Overflow
// take decision which chain to use for energy calc.(Hi or Low):
      sta=0;
      if(radc[0]>0. && ovfl[0]==0)fadc=radc[0];
      else if(radc[1]>0. && ovfl[1]==0){
        fadc=radc[1]*h2lr;//rescale LowG-chain to HighG
	if(ovfl[1]==1)sta|=(65536*64);// mark overflow status bit
      }
      else fadc=0.;
      edep=fadc/ECcalib::ecpmcal[isl][pmc].pmrgain()
                                    /ECcalib::ecpmcal[isl][pmc].pmscgain(subc);//gain corr.
      if(ECREFFKEY.reprtf[0]==1){
        HF1(ECHISTR+16,geant(edep),1.);
        HF1(ECHISTR+17,geant(edep),1.);
      }
      adct+=edep;//tot.adc
      edep=edep*ECcalib::ecpmcal[isl][pmc].adc2mev();// ADCch->Emeasured(MeV)
      emeast+=edep;//tot.Mev
      dbstat=ECcalib::ecpmcal[isl][pmc].getstat(subc);//status from DB (0=ok)
      if(dbstat>=0 && fadc>0.){// use only good(according DB) channels
        nhits+=1;
        ECALDBc::getscinfoa(isl,pmc,subc,proj,plane,cell,coot,cool,cooz);// get SubCell info
	icont=plane;//container number for storing of EcalHits(= plane number)
        AMSEvent::gethead()->addnext(AMSID("AMSEcalHit",icont), new
                       AMSEcalHit(sta,id,padc,proj,plane,cell,edep,coot,cool,cooz));
      }
      ptr=ptr->next();  
    } // ---> end of RawEvent-hits loop in superlayer
//
  } // ---> end of super-layer loop
//
  if(ECREFFKEY.reprtf[0]==1){
    HF1(ECHISTR+10,geant(nraw),1.);
    HF1(ECHISTR+11,geant(adct),1.);
    HF1(ECHISTR+12,geant(adct),1.);
    HF1(ECHISTR+13,geant(nhits),1.);
    HF1(ECHISTR+14,geant(emeast),1.);
    HF1(ECHISTR+15,geant(emeast),1.);
  }
  if(nhits>0)stat=0;
}
//---------------------------------------------------
void AMSEcalCluster::build(int &stat){
  int i,j,k;
  integer maxpl,maxcl,sta,dbstat,nhits,nclus,nmemb;
  integer ipl,proj,cell,isl,status;
  number edep,edept,edepthr,cool,coot,cooz,eclust;
  AMSPoint coo;
  AMSPoint ecoo; 
  AMSEcalHit * ptr;
  AMSEcalHit * membp[2*ECPMSMX];
  number x,y,z,cogt,ecogt,cogl,ecogl,cogz,ecogz,vr;
  geant tprof[2*ECPMSMX],zprof[2*ECSLMX];
  static integer nprz(0);
//
  stat=1;//bad
  nhits=0;
  nclus=0;
  eclust=0.;
  edepthr=ECALVarp::ecalvpar.rtcuts(0);//thresh.(mev/cell) for EcalHit(~2.adc, at mip/pl=5adc(m.p.))
  maxpl=2*ECALDBc::slstruc(3);//real planes
  maxcl=2*ECALDBc::slstruc(4);//real SubCells per plane
  ecogz=0.01;//(cm) hope not more
//
  for(i=0;i<(2*ECSLMX);i++){
    zprof[i]=0.;// clear z-profile array
  }
//
  for(ipl=0;ipl<maxpl;ipl++){ // <-------------- SubCell-plane loop
    ptr=(AMSEcalHit*)AMSEvent::gethead()->
                               getheadC("AMSEcalHit",ipl,0);
    nmemb=0;
    cogt=0.;
    ecogt=0.;
    edept=0.;
    for(i=0;i<(2*ECPMSMX);i++)tprof[i]=0.;// clear t-profile array
    while(ptr){ // <--- EcalHit-hits loop in SubCell-planes:
      nhits+=1;
      edep=ptr->getedep();
      proj=ptr->getproj();//0/1->X/Y (have to be corresponding to ipl)
      cell=ptr->getcell();// 0,...
      coot=ptr->getcoot();
      cool=ptr->getcool();
      cooz=ptr->getcooz();
      status=ptr->getstatus();
      tprof[cell]+=edep;//Gev-profile
      zprof[ipl]+=edep;
      EcalJobStat::zprofa[ipl]+=edep;
      if(edep>edepthr){ //<-- for primitive cluster
        cogt+=coot*edep;
	ecogt+=coot*coot*edep;
	edept+=edep;
	ptr->setstatus(AMSDBc::USED);//set hit-status "used"
	membp[nmemb]=ptr; 
        nmemb+=1;  
      }
//
//
      ptr=ptr->next();  
    } // -------> end of EcalHit-hits loop in plane
//
//                                     <--- primitive(single) cluster calc.
    if(edept>0.){
      cogt/=edept;
      ecogt/=edept;
      vr=cogt*cogt;
      if(ecogt>vr)ecogt=sqrt(ecogt-vr);
      else ecogt=0.;
      if(nmemb==1)ecogt=ECALDBc::rdcell(5)/sqrt(12.);// bin_size/sqrt(12)
      cogz=cooz;//imply all z in layer are the same
      if(proj==0){// X-pr
        cogl=ECALDBc::gendim(6);//0+rad.y-shift
        coo=AMSPoint(cogt,cogl,cogz);
	ecogl=ECALDBc::gendim(2)/sqrt(12.);//rad_y_size(==flen)/sqrt(12)
	ecoo=AMSPoint(ecogt,ecogl,ecogz);
      }
      else{       // Y-pr
        cogl=ECALDBc::gendim(5);//0+rad.x-shift
        coo=AMSPoint(cogl,cogt,cogz);
	ecogl=ECALDBc::gendim(1)/sqrt(12.);
	ecoo=AMSPoint(ecogl,ecogt,ecogz);
      }
      nclus+=1;
      eclust+=edept;
      sta=0.;
      AMSEvent::gethead()->addnext(AMSID("AMSEcalCluster",ipl), new
                       AMSEcalCluster(sta,proj,ipl,edept,coo,ecoo,nmemb,membp));
    }
//
    if(ECREFFKEY.reprtf[1]==1){//<--- print t-profiles
      if(ipl==7){
	if(nprz<9){
          HPAK(ECHISTR+19,tprof);
	  HPRINT(ECHISTR+19);
	  HRESET(ECHISTR+19," ");
	}
      }
      if(ipl==8){
	if(nprz<9){
          HPAK(ECHISTR+20,tprof);
	  HPRINT(ECHISTR+20);
	  HRESET(ECHISTR+20," ");
	}
      }
    }
//
  } // --------> end of SubCell-planes loop
//
  if(ECREFFKEY.reprtf[1]==1){//<--- print z-profiles
    if(nprz<9){
      HPAK(ECHISTR+21,zprof);
      HPRINT(ECHISTR+21);
      HRESET(ECHISTR+21," ");
      nprz+=1;
    }
  }
//  
  if(ECREFFKEY.reprtf[0]==1){
    HF1(ECHISTR+18,geant(nclus),1.);
    HF1(ECHISTR+22,geant(eclust),1.);
    HF1(ECHISTR+23,geant(eclust),1.);
  }
  if(nclus>0)stat=0;
}
//---------------------------------------------------
void AMSEcalCluster::_writeEl(){
  EcalClusterNtuple* TN = AMSJob::gethead()->getntuple()->Get_ecclust();

  if (TN->Neccl>=MAXECCLUST) return;

// Fill the ntuple
//  if(AMSEcalCluster::Out( IOPA.WriteAll%10==1 ||  checkstatus(AMSDBc::USED ))){
  if(AMSEcalCluster::Out( IOPA.WriteAll%10==1 )){
    TN->Status[TN->Neccl]=_status;
    TN->Proj[TN->Neccl]=_proj;
    TN->Plane[TN->Neccl]=_plane;
    TN->Nmemb[TN->Neccl]=_nmemb;
    TN->Edep[TN->Neccl]=_edep;
    int i;
    for(i=0;i<3;i++)TN->Coo[TN->Neccl][i]=_Coo[i];
    for(i=0;i<3;i++)TN->ErrCoo[TN->Neccl][i]=_ErrorCoo[i];
//    for(i=0;i<3;i++)cout <<TN->ErrCoo[TN->Neccl][i]<<" "<<_ErrorCoo[i]<<endl;
    TN->Neccl++;
  }
}
//---------------------------------------------------
void AMSEcalCluster::_copyEl(){
//
}
//---------------------------------------------------
integer AMSEcalCluster::Out(integer status){
static integer init=0;
static integer WriteAll=1;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
 for(int n=0;n<ntrig;n++){
   if(strcmp("AMSEcalCluster",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
   }
 }
}
return (WriteAll || status);
}
//---------------------------------------------------
void AMSEcalHit::_writeEl(){
  EcalHitNtuple* TN = AMSJob::gethead()->getntuple()->Get_ecalhit();
  if (TN->Necht>=MAXECHITS || ECREFFKEY.relogic[0]==0) return;

// Fill the ntuple
  if(AMSEcalHit::Out( IOPA.WriteAll%10==1 ||  checkstatus(AMSDBc::USED ))){
    TN->Status[TN->Necht]=_status;
    TN->Idsoft[TN->Necht]=_idsoft;
    TN->Proj[TN->Necht]=_proj;
    TN->Plane[TN->Necht]=_plane;
    TN->Cell[TN->Necht]=_cell;
    TN->Edep[TN->Necht]=_edep;
    if(_proj){ //<-- y-proj
      TN->Coo[TN->Necht][0]=_cool;
      TN->Coo[TN->Necht][1]=_coot;
    }
    else{     //<-- x-proj
      TN->Coo[TN->Necht][0]=_coot;
      TN->Coo[TN->Necht][1]=_cool;
    }
    TN->Coo[TN->Necht][2]=_cooz;
    TN->Necht++;
  }
}
//---------------------------------------------------
void AMSEcalHit::_copyEl(){
}
//---------------------------------------------------
integer AMSEcalHit::Out(integer status){
static integer init=0;
static integer WriteAll=1;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
 for(int n=0;n<ntrig;n++){
   if(strcmp("AMSEcalHit",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
   }
 }
}
return (WriteAll || status);
}
//---------------------------------------------------
