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
#include <trigger3.h>
//
extern ECcalib ecpmcal[ECSLMX][ECPMSMX];// ECAL indiv.channel calibration data
extern ECALVarp ecalvpar;// ECAL run-time parameters
integer AMSEcalRawEvent::trigfl=0;// just memory reservation/initialization for static
//----------------------------------------------------
void AMSEcalRawEvent::validate(int &stat){ //Check/correct RawEvent-structure
  stat=0;
}
//----------------------------------------------------
void AMSEcalRawEvent::mc_build(int &stat){
  int i,j,k;
  integer fid,cid,cidar[4],nhits,nraw,il,pm,sc,proj,rdir;
  number x,y,z,coo,hflen,pmdis,edep,edepr,edept,edeprt,emeast,time,timet(0.);
  number attf,ww[4],sum[ECPMSMX][4];
  AMSEcalMCHit * ptr;
  integer id,sta,adc,adctot;
//
  stat=1;//bad
  trigfl=0;//reset tigger-flag
  edept=0.;
  edeprt=0.;
  emeast=0.;
  nhits=0;
  nraw=0;
  adctot=0;
  timet=0.;
  for(il=0;il<ECALDBc::slstruc(3);il++){ // <-------------- super-layer loop
    ptr=(AMSEcalMCHit*)AMSEvent::gethead()->
               getheadC("AMSEcalMCHit",il,0);
    for(i=0;i<ECALDBc::slstruc(4);i++)for(k=0;k<4;k++)sum[i][k]=0.;
    while(ptr){ // <--- geant-hits loop in superlayer:
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
    } // ---------------> end of hit-loop in superlayer
//
    for(i=0;i<ECALDBc::slstruc(4);i++){ // <-- create RawEvent objects from this S-layer
      for(k=0;k<4;k++){
        edepr=sum[i][k]*ECALDBc::mev2mev();//dE/dX(Mev)->Emeas(Mev)
	emeast+=edepr;
	adc=integer(edepr*ECALDBc::mev2adc());//Emeas(Mev)->Emeas(adc) ("digitization")
	adctot+=adc;
        if(adc>=ecalvpar.daqthr(0)){// use only hits above DAQ-readout threshold
	  if(adc>=65536)adc=65536;//"ADC-saturation (16 bit)"
	  nraw+=1;
	  id=(k+1)+10*(i+1)+1000*(il+1);
	  sta=0;
          AMSEvent::gethead()->addnext(AMSID("AMSEcalRawEvent",il), new
                                             AMSEcalRawEvent(id,sta,adc));
	}
      }
    }
//
  } // ---> end of super-layer loop
//
  if(ECMCFFKEY.mcprtf==1){
    HF1(ECHIST+1,geant(nhits),1.);
    HF1(ECHIST+2,geant(edept),1.);
    HF1(ECHIST+3,geant(edeprt),1.);
    HF1(ECHIST+4,geant(emeast),1.);
    HF1(ECHIST+5,geant(emeast),1.);
    if(edeprt>0.)HF1(ECHIST+6,geant(timet/edeprt),1.);
  }
//  ---> set trigger flag:
  if(adctot>ecalvpar.daqthr(1)){
    if(adctot<ecalvpar.daqthr(2))trigfl=1;// MIP
    else if(adctot<ecalvpar.daqthr(3))trigfl=2;// Low-energy EM-object
    else trigfl=3;// High-energy EM-object
  }
  if(trigfl>0)stat=0;
}
//---------------------------------------------------
void AMSEcalHit::build(int &stat){
  int i,j,k;
  integer sta,status,dbstat,padc,id,idd,isl,pmc,subc,nraw,nhits;
  integer proj,plane,cell,icont;
  number edep,adct,emeast,coot,cool,cooz; 
  AMSEcalRawEvent * ptr;
//
  stat=1;//bad
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
      padc=ptr->getpadc();
      status=ptr->getstatus();//status of hit (used,recoverd,....)
      edep=padc*ecpmcal[isl][pmc].getchg()*ecpmcal[isl][pmc].getscg(subc);//gain corr.
      if(ECREFFKEY.reprtf[0]==1){
        HF1(ECHIST+16,geant(edep),1.);
        HF1(ECHIST+17,geant(edep),1.);
      }
      adct+=edep;//tot.adc
      edep=edep*ecpmcal[isl][pmc].adc2mev();// ADCch->Emeasured(MeV)
      emeast+=edep;//tot.Mev
      dbstat=ecpmcal[isl][pmc].getstat();//status from DB (0=ok)
      if(dbstat==0){// use only good(according DB) channels
        nhits+=1;
        ECALDBc::getscinfoa(isl,pmc,subc,proj,plane,cell,coot,cool,cooz);// get SubCell info
	icont=plane;//container number for storing of EcalHits(= plane number)
	sta=0;//tempor
        AMSEvent::gethead()->addnext(AMSID("AMSEcalHit",icont), new
                       AMSEcalHit(sta,proj,plane,cell,edep,coot,cool,cooz));
      }
      ptr=ptr->next();  
    } // ---> end of RawEvent-hits loop in superlayer
//
  } // ---> end of super-layer loop
//
  if(ECREFFKEY.reprtf[0]==1){
    HF1(ECHIST+10,geant(nraw),1.);
    HF1(ECHIST+11,geant(adct),1.);
    HF1(ECHIST+12,geant(adct),1.);
    HF1(ECHIST+13,geant(nhits),1.);
    HF1(ECHIST+14,geant(emeast),1.);
    HF1(ECHIST+15,geant(emeast),1.);
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
  edepthr=ecalvpar.rtcuts(0);//thresh.(mev/cell) for EcalHit(~2.adc, at mip/pl=5adc(m.p.))
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
	if(nprz<5){
          HPAK(ECHIST+19,tprof);
	  HPRINT(ECHIST+19);
	  HRESET(ECHIST+19," ");
	}
      }
      if(ipl==8){
	if(nprz<5){
          HPAK(ECHIST+20,tprof);
	  HPRINT(ECHIST+20);
	  HRESET(ECHIST+20," ");
	}
      }
    }
//
  } // --------> end of SubCell-planes loop
//
  if(ECREFFKEY.reprtf[1]==1){//<--- print z-profiles
    if(nprz<5){
      HPAK(ECHIST+21,zprof);
      HPRINT(ECHIST+21);
      HRESET(ECHIST+21," ");
      nprz+=1;
    }
  }
//  
  if(ECREFFKEY.reprtf[0]==1){
    HF1(ECHIST+18,geant(nclus),1.);
    HF1(ECHIST+22,geant(eclust),1.);
    HF1(ECHIST+23,geant(eclust),1.);
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
