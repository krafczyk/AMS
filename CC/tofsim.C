// Author Choumilov.E. 10.07.96.
#include <iostream.h>
#include <stdio.h>
#include <typedefs.h>
#include <cern.h>
#include <extC.h>
#include <point.h>
#include <amsgobj.h>
#include <event.h>
#include <amsstl.h>
#include <commons.h>
#include <tofdbc.h>
#include <tofsim.h>
#include <mccluster.h>
//
extern AMSTOFScan scmcscan[SCBTPN];
//
number AMSDistr::getrand(const number &rnd)
{
  number val;
  integer i=AMSbins(distr,rnd,nbin);
  if(i==0){val=bnl+bnw*rnd/distr[i];}
  else if(i>0){val=bnl+bnw*i;}
  else
  {
    i=-i;
    val=bnl+bnw*i+bnw*(rnd-distr[i-1])/(distr[i]-distr[i-1]);
  }
  return val;
}
//------------------------------------------------------------------------------
//  <--- for inp. X define position in scan array (i1/i2) and correction ratio R.
//
void AMSTOFScan::getxbin(const number x, integer &i1, integer &i2, number &r)
{
  integer i=AMSbins(scanp,x,SCANPNT);
  if(i==0){
    i1=0;
    i2=1;
    r=(x-scanp[i1])/(scanp[i2]-scanp[i1]);
  }
  else if(i<0){
    if(i==-SCANPNT){
      i1=SCANPNT-2;
      i2=SCANPNT-1;
      r=(x-scanp[i1])/(scanp[i2]-scanp[i1]);
      }
    else{
      i1=-i-1;
      i2=-i;
      r=(x-scanp[i1])/(scanp[i2]-scanp[i1]);
    }
  }
  else{
    i1=i-1;
    i2=i1;
    r=0.;
  }
}
//------------------------------------------------------------------
// <--- functions to get efficiency ( getxbin should be called first!!!)
number AMSTOFScan::getef1(const number r, const int i1, const int i2)
{
  if(i1==i2){return eff1[i1];}
  else{return (eff1[i1]+(eff1[i2]-eff1[i1])*r);}
}
number AMSTOFScan::getef2(const number r, const int i1, const int i2)
{
  if(i1==i2){return eff2[i1];}
  else{return (eff2[i1]+(eff2[i2]-eff2[i1])*r);}
}
//===================================================================
//
void AMSTOFPhel::build()
{
  AMSgObj::BookTimer.start("SITOFPHEL");
//      <--- prepare PM single electron pulse height distribution:
  static number arr[19]={57.8,40.7,39.7,46.5,52.3,54.6,52.7,47.6,40.7,
                          32.5,23.9,16.7,11.,7.,4.5,3.,1.5,1.,0.5};
  static AMSDistr scpmsesp(19,0.,0.18,arr);// p/h spectrum ready
//
  integer i1,i2,id,idd,ibar,ilay,ibtyp,nphot;
  int i,stat(0),nelec,ierr(0);
  number dummy(-1),x,y,z,time,de,eff,r,rnd,tm,am;
  float nel;
  AMSPoint cloc(999.,999.,999.);
  AMSTOFMCCluster *ptr=(AMSTOFMCCluster*)AMSEvent::gethead()->
                                      getheadC("AMSTOFMCCluster",0);
//
  while(ptr){//       <------------- loop over geant hits
    id=ptr->idsoft; 
    ibar=id/100-1;
    ilay=id%100-1;
    de  =ptr->edep;
    time=ptr->tof;
    x=ptr->xcoo[0];
    y=ptr->xcoo[1];
    z=ptr->xcoo[2];
    AMSPoint cglo(x,y,z);
    AMSgvolume *p=(AMSgvolume*)AMSgObj::GVolMap.getp(AMSID("TOFS",id));
    if(p)cloc=p->gl2loc(cglo);// convert global coord. to local
    x=cloc[0];
    y=cloc[1];
    z=cloc[2];
#ifdef __AMSDEBUG__
  assert(ilay>=0 && ilay<SCLRS);
  assert(ibar>=0 && ibar<SCBRS[ilay]);
  assert(z<=TOFDBc::plnstr(6)/2. && z>=-TOFDBc::plnstr(6)/2.);
  assert(de>0 && de<1.);
#endif
    ibtyp=TOFDBc::brtype(ilay,ibar)-1;
    scmcscan[ibtyp].getxbin(y,i1,i2,r);//y-bin # (longit.(x !)-coord. in LTRANS )
// PM-1 actions --->
    eff=scmcscan[ibtyp].getef1(r,i1,i2);//eff for PM-1
    nphot=1000.*de*TOFMCFFKEY.edep2ph;//  Edep to phot. number
    nel=nphot*eff*TOFMCFFKEY.pmqeff;// mean number of photoelectrons
    POISSN(nel,nelec,ierr);// fluctuations
//    <-------- Loop over photoelectrons(PM-1) ---<<<
    idd=id*10+1;
    for(i=1;i<=nelec;i++){
      rnd=RNDM(dummy);
      tm=scmcscan[ibtyp].gettm1(rnd,r,i1,i2);//ph.arrival time from interpol. distr.
      tm=tm+(4.+rnormx())*TOFMCFFKEY.trtspr;// fixed trans. time delay + fluct.
      rnd=RNDM(dummy);
      am=scpmsesp.getrand(rnd);//amplitude from single elect. spectrum
//       <--- store time/ampl
      stat=0;
      stat=AMSEvent::gethead()->addnext(AMSID("AMSTOFPhel",ilay)
                                        ,new AMSTOFPhel(idd,tm,am)); // store time/ampl
    } //   <-------- end of PM-1 loop
//
// PM-2 actions --->
    eff=scmcscan[ibtyp].getef2(r,i1,i2);//eff for PM-2
    nphot=1000.*de*TOFMCFFKEY.edep2ph;//  Edep to phot. number
    nel=nphot*eff*TOFMCFFKEY.pmqeff;// mean number of photoelectrons
    POISSN(nel,nelec,ierr);// fluctuations
//    <-------- Loop over photoelectrons(PM-2) ---<<<
    idd=id*10+2;
    for(i=1;i<=nelec;i++){
      rnd=RNDM(dummy);
      tm=scmcscan[ibtyp].gettm2(rnd,r,i1,i2);//ph.arrival time from interpol. distr.
      tm=tm+(4.+rnormx())*TOFMCFFKEY.trtspr;// fixed trans. time delay + fluct.
      rnd=RNDM(dummy);
      am=scpmsesp.getrand(rnd);//amplitude from single elect. spectrum
//       <--- store time/ampl
      stat=0;
      stat=AMSEvent::gethead()->addnext(AMSID("AMSTOFPhel",ilay)
                                        ,new AMSTOFPhel(idd,tm,am)); // store time/ampl
    } //   ------- end of PM-2 loop ------>
//
  ptr=ptr->next();}// ------ end of geant hits loop ---->
//
  AMSgObj::BookTimer.stop("SITOFPHEL");
}
//=========================================================================
//
// function below creates PMT pulse shape array arr[] with binning fladctb :
//  arr[] total integral = 1.
void AMSTOFTovt::inipsh(integer &nbn, number arr[])
{
  static number pmpsh[15]={0.,0.026,0.175,0.422,0.714,0.942,1.,0.935,
       0.753,0.584,0.442,0.305,0.182,0.078,0.};// pulse hights at time points
  static number pmpsb[15]={0.,0.5,1.,1.5,2.,2.5,3.,3.5,4.,4.5,5.,
       5.5,6.,6.5,7.}; // time points (min. step should be above SCTBWD)
  integer i,io,ib;
  number bl,bh,bol,boh,ao1,ao2,tgo,al,ah,tota;
  tota=0.;
  io=0;
  for(ib=0;ib<200;ib++){
    bl=ib*TOFMCFFKEY.fladctb;
    bh=bl+TOFMCFFKEY.fladctb;
    bol=pmpsb[io];
    boh=pmpsb[io+1];
    arr[ib]=0.;
    if(bl>=bol && bh<boh){
      ao1=pmpsh[io];
      ao2=pmpsh[io+1];
      tgo=(ao2-ao1)/(boh-bol);
      al=ao1+tgo*(bl-bol);
      ah=ao1+tgo*(bh-bol);
      arr[ib]=(al+ah)/2.;
      tota+=arr[ib];
    }
    else if(bl<boh && bh>=boh){
      ao1=pmpsh[io];
      ao2=pmpsh[io+1];
      tgo=(ao2-ao1)/(boh-bol);
      al=ao1+tgo*(bl-bol);
      io+=1;
      if((io+1)<15){
        bol=pmpsb[io];
        boh=pmpsb[io+1];
        ao1=pmpsh[io];
        ao2=pmpsh[io+1];
        tgo=(ao2-ao1)/(boh-bol);
        ah=ao1+tgo*(bh-bol);
        arr[ib]=(al+ah)/2.;
        tota+=arr[ib];
      }
      else{                 // last bin
        ah=0.;
        arr[ib]=(al+ah)/2.;
        tota+=arr[ib];
        nbn=ib+1;
        for(i=0;i<nbn;i++)arr[i]/=tota;// normalization to integral=1
#ifdef __AMSDEBUG__
        cout<<"PM pulse shape ready : nbins="<<nbn<<'\n';
        for(i=1;i<=nbn;i++){
          if(i%10 != 0)
                       printf("%10.6e",arr[i-1]);
          else
                       printf("%10.6e\n",arr[i-1]);
        }
        if(nbn%10 !=0)printf("\n");
#endif
        return;
      }
    }
  }
  cout<<"AMSTOFTovt-inipsh-Fatal: wrong pulse shape !?"<<'\n';
  exit(1);
}
//--------------------------------------------------------------------------
//
// function below creates shaper stand. pulse shape array arr[] with bin shaptb :
// (this array is shaper responce to unit pulse with width=shaptb;
//  shaper rise/fall times are shrtim<<shftim; nbn is array length,defined as
//  boundary where the rest of pulse integral is less than 0.0001 of current
//  integral value)
//
void AMSTOFTovt::inishap(integer &nbn, number arr[])
{
  number tr,tf,tmb,toti,rest,t1,t2,anr;
  integer i,j,ifl(0);
  tr=TOFMCFFKEY.shrtim;
  tf=TOFMCFFKEY.shftim;
  tmb=TOFMCFFKEY.shaptb;
  arr[0]=tmb-tr*(1.-exp(-tmb/tr));// fast charge (neglect by discharge !)
  anr=1.-exp(-tmb/tr);
  toti=arr[0];
  for(i=1;i<400;i++){ // <--- time loop ---
    t1=i*tmb;
    t2=t1+tmb;
    rest=anr*tf*exp(-t2/tf);
    arr[i]=anr*tf*(exp(-t1/tf)-exp(-t2/tf));
    toti=toti+arr[i];
    if(rest<(0.0001*toti)){
      nbn=i+1;
#ifdef __AMSDEBUG__
      cout<<"Shaper pulse shape ready : nbins="<<nbn<<'\n';
      for(j=1;j<=nbn;j++){
        if(j%10 != 0)
                     printf("%10.6e",arr[j-1]);
        else
                     printf("%10.6e\n",arr[j-1]);
      }
      if(nbn%10 !=0)printf("\n");
#endif
      return;
    }
  } // --- end if time loop --->
  cout<<"AMSTOFTovt-inishap-Fatal: bad shaper rise/fall times ?!"<<'\n';
  cout<<"tr/tf="<<tr<<tf<<" nbins="<<i<<'\n';
  exit(1);
}
//--------------------------------------------------------------------------
//
// function below simulate PMT saturation, it returns nonsat. factor <=1. :
//
number AMSTOFTovt::pmsatur(const number am){
  number gain=1.; //tempor. no saturation
  return gain;
}
//--------------------------------------------------------------------------
//
// function to display PMT pulse (flash-ADC array arr[]) :
//
void AMSTOFTovt::displ_a(const int id, const int mf, const number arr[]){
  integer i,ifrs(0);
  number a(0.);
  number tb,tm;
  if(ifrs++==0){
    tb=float(mf)*TOFMCFFKEY.fladctb;
    HBOOK1(1000,"PMT flash-ADC pulse (MC)",80,0.,80*tb,0.);
  }
  cout<<"counter-id = "<<id<<" (LBBS, L-layer, BB-bar_number, S-side)"<<'\n';
  HRESET(1000," ");
  for(i=1;i<=SCTBMX;i++){
    if(i%mf==0){
      a+=arr[i-1];
      tm=i*TOFMCFFKEY.fladctb-0.5*tb;
      HF1(1000,tm,a/float(mf));
      a=0.;
    }
    else{
      a+=arr[i-1];
    }
  }
  HPRINT(1000);
  return ;
}
//--------------------------------------------------------------------------
//
// function to display SHAPER pulse ( array arr[]) :
//
void AMSTOFTovt::displ_as(const int id, const int mf, const number arr[]){
  integer i,ifrs(0);
  number a(0.);
  number tb,tm;
  if(ifrs++==0){
    tb=float(mf)*TOFMCFFKEY.shaptb;
    HBOOK1(1001,"Shaper pulse (anode,MC)",80,0.,80*tb,0.);
  }
  cout<<"counter-id = "<<id<<" (LBBS, L-layer, BB-bar_number, S-side)"<<'\n';
  HRESET(1001," ");
  for(i=1;i<=SCTBMX;i++){
    if(i%mf==0){
      a+=arr[i-1];
      tm=i*TOFMCFFKEY.shaptb-0.5*tb;
      HF1(1001,tm,a/float(mf));
      a=0.;
    }
    else{
      a+=arr[i-1];
    }
  }
  HPRINT(1001);
  return ;
}
//--------------------------------------------------------------------------
//
void AMSTOFTovt::build()
{
  integer i,j,ij,ilay,last,ibar,id,idd,iddN,isd,_sta,stat(0);
  number tm,a,am,amd,tmp,amp;
  integer _ntr1,_ntr2,_ntr3,_nftdc,_nstdc,_nadca,_nadcd;
  number _ttr1[SCTHMX1],_ttr2[SCTHMX1],_ttr3[SCTHMX1];
  number  _tftdc[SCTHMX2],_tstdc[SCTHMX3],_tadca[SCTHMX4],_tadcd[SCTHMX4];
  int upd1,upd2,upd3; // up/down flags for 3 fast discr. (z>=1;z>1;z>2)
  int upd11,upd12,upd13,upd21,upd31;
  int updsh;
  number tshd;
  number td1b1,td1b2,td1b3;
  number td2b1,td3b1;
  number tmd1u,tbn,w,bo1,bo2,bn1,bn2;
  number tslice[SCTBMX+1]; // temporary flash ADC array
  number tshap1[SCTBMX+1]; // temporary shaper pulse array
  number tshap2[SCTBMX+1]; // temporary shaper pulse array
  AMSTOFPhel *ptr;
  AMSTOFPhel *ptrN;
  static integer first=0;
  static integer npshbn;
  static number pmplsh[200];  // PM s.e. pulse shape
  static integer nshbn;
  static number shplsh[400];  // Shaper standard pulse shape array
//
  AMSgObj::BookTimer.start("SITOFTOVT");
//
  if(first++==0){
    AMSTOFTovt::inipsh(npshbn,pmplsh); // prepare PM s.e. pulse shape arr.
    AMSTOFTovt::inishap(nshbn,shplsh); // prepare Shaper stand. pulse shape arr.
  }
//
//              <----- loop over layers (Phel-containers)-------
//
  for(ilay=0;ilay<SCLRS;ilay++){
    ptr=(AMSTOFPhel*)AMSEvent::gethead()->
                                      getheadC("AMSTOFPhel",ilay,1);
//    (above i use sort-option for AMSTOFPhel objects !!!)                                      
    for(i=0;i<SCTBMX+1;i++)tslice[i]=0.;
    while(ptr){  // <--- loop over Phel-hits in container ---
      idd=ptr->getid();
      isd=idd%10; // pmt side (1,2)
      id=idd/10; 
      ibar=id/100-1;
      ilay=id%100-1;
      ptr->getval(tm,am); // extract time and amplitude of photoelectron
//    summing of all Phel's (with the same idd) as fixed shape pulses:
      i=tm/TOFMCFFKEY.fladctb;
      for(j=0;j<npshbn;j++){
        if((i+j)<(SCTBMX-1))
             tslice[i+j]+=am*pmplsh[j];
        else 
             tslice[SCTBMX]+=am*pmplsh[j];
      }
      ptrN=ptr->next();
      iddN=0; 
      if(ptrN)iddN=ptrN->getid();
      if(iddN != idd){ // if last or new , then
// -----> create/fill summary Tovt-object for idsoft=idd :
//
        if(tslice[SCTBMX]>0.)
           cout<<"SITOFTOVT-build-warning: out of time range, id="<<idd<<
                 "  A-last= "<<tslice[SCTBMX]<<'\n';
        if(TOFMCFFKEY.mcprtf != 0)
                      AMSTOFTovt::displ_a(idd,20,tslice);//print PMT pulse
        _ntr1=0;
        _ntr2=0;
        _ntr3=0;
        _nftdc=0;
        _nstdc=0;
        upd1=0; //up/down flag for discr.1
        upd2=0;
        upd3=0;
        td1b1=-9999.;
        td1b2=-9999.;
        td1b3=-9999.;
        upd11=0;// up/down flag for z>=1 branch of discr.1
        upd12=0;// ............ fast TDC .................
        upd13=0;// ............ slow TDC .................
        upd21=0;// up/down flag for z>1 branch of discr.2
        td2b1=-9999.;
        upd31=0;// up/down flag for z>2 branch of discr.3 (dinode)
        td3b1=-9999.;
//
        for(i=0;i<SCTBMX;i++){  //  <--- time bin loop ---
          am=tslice[i];
          tm=(i+1)*TOFMCFFKEY.fladctb;
          if(i>0){amp=tslice[i-1];
                  tmp=i*TOFMCFFKEY.fladctb;}
          else{amp=0.;
               tmp=0.;}
//--------------------------          
// discr.1 up/down setting :
          if(am>=TOFMCFFKEY.daqthr[0] && upd1 ==0){//  
            tmd1u=tmp+(tm-tmp)*(TOFMCFFKEY.daqthr[0]-amp)/(am-amp);// up time of discr.1
            upd1=1;
          }
          if(am<TOFMCFFKEY.daqthr[0] && upd1!=0)upd1=0;
//
// branch "z>=1 logic" :
          if(upd1==1 && upd11==0
                     && (tm-td1b1)>TOFMCFFKEY.daqpwd[7]){//dead time check for z>=1 signal
            upd11=1;  // set flag for branch z>=1
            if(_ntr1<SCTHMX1){
              _ttr1[_ntr1]=tm;
              _ntr1+=1;
            }
            else{
              cout<<"AMSTOFTovt::build-warning: trig1 hits ovfl"<<'\n';
              upd11=2;//blockade on overflow
            } 
          }
          if(upd11==1 && (tm-_ttr1[_ntr1-1])>TOFMCFFKEY.daqpwd[0]){//pulse width check
            upd11=0;                            // self clear
            td1b1=tm;
          } 
//        
// branch "fast TDC logic" :        
          if(upd1==1 && upd12==0
                     && (tm-td1b2)>TOFMCFFKEY.daqpwd[3]){ //"buzy" check for f-TDC channel
            upd12=1;  // set flag for branch f-TDC,if ready.
            if(_nftdc<SCTHMX2){
              td1b2=tm;
              _tftdc[_nftdc]=tm;
              _nftdc+=1;
            }
            else{
              cout<<"AMSTOFTovt::build-warning: f-TDC hits ovfl"<<'\n';
              upd12=2;//blockade on overflow
            } 
          }
          if(upd12==1 && (tm-td1b2)>TOFMCFFKEY.daqpwd[3])upd12=0;// clear "buzy"
//        
// branch "slow TDC logic" :        
          if(upd1==1 && upd13==0
                     && (tmd1u-td1b3)>TOFMCFFKEY.daqpwd[4]){ // "buzy" check for f-TDC channel
            upd13=1;  // set flag for branch s-TDC,if ready 
            if(_nstdc<SCTHMX3){
              td1b3=tm;
              _tstdc[_nstdc]=tm;
              _nstdc+=1;
            }
            else{
              cout<<"AMSTOFTovt::build-warning: s-TDC hits ovfl"<<'\n';
              upd13=2;//blockade on overflow
            } 
          }
          if(upd13==1 && (tm-td1b3)>TOFMCFFKEY.daqpwd[4])upd13=0;// clear "buzy"
//---------------------------        
// discr.2 up/down setting :
          if(am>=TOFMCFFKEY.daqthr[1] && upd2 ==0){//  
            upd2=1;
          }
          if(am<TOFMCFFKEY.daqthr[1] && upd2!=0)upd2=0;
//
// branch "z>1 logic" :
          if(upd2==1 && upd21==0
                     && (tm-td2b1)>TOFMCFFKEY.daqpwd[8]){//dead time check for z>1 signal
            upd21=1;  // set flag for branch z>1
            if(_ntr2<SCTHMX1){
              _ttr2[_ntr2]=tm;
              _ntr2+=1;
            }
            else{
              cout<<"AMSTOFTovt::build-warning: trig2 hits ovfl"<<'\n';
              upd21=2;//blockade on overflow
            } 
          }
          if(upd21==1 && (tm-_ttr2[_ntr2-1])>TOFMCFFKEY.daqpwd[1]){//pulse width check
            upd21=0;                            // self clear
            td2b1=tm;
          } 
//--------------------------        
// discr.3 up/down setting (dinode) :
          amd=am*TOFMCFFKEY.di2anr;
          if(amd>=TOFMCFFKEY.daqthr[2] && upd3 ==0){//  
            upd3=1;
          }
          if(amd<TOFMCFFKEY.daqthr[2] && upd3!=0)upd3=0;
//
// branch "z>2 logic" :
          if(upd3==1 && upd31==0
                     && (tm-td3b1)>TOFMCFFKEY.daqpwd[9]){//dead time check for z>2 signal
            upd31=1;  // set flag for branch z>2
            if(_ntr3<SCTHMX1){
              _ttr3[_ntr3]=tm;
              _ntr3+=1;
            }
            else{
              cout<<"AMSTOFTovt::build-warning: trig3 hits ovfl"<<'\n';
              upd31=2;//blockade on overflow
            } 
          }
          if(upd31==1 && (tm-_ttr3[_ntr3-1])>TOFMCFFKEY.daqpwd[2]){//pulse width check
            upd31=0;                            // self clear
            td3b1=tm;
          } 
//        
        } //                --- end of time bin loop --->         
//------------------------------
//     convert fast flash ADC array tslice to shaper pulse (anode):
//                 
// rebinning of tslice[] into shaper binned tshap1[]
        for(j=0;j<SCTBMX;j++)tshap1[j]=0.; // clear shaper output array
        j=0;                 
        for(i=0;i<SCTBMX;i++){
          a=tslice[i];
          am=a*AMSTOFTovt::pmsatur(a); //saturation for anode signal
          bo1=i*TOFMCFFKEY.fladctb;
          bo2=bo1+TOFMCFFKEY.fladctb;
          bn1=j*TOFMCFFKEY.shaptb;
          bn2=bn1+TOFMCFFKEY.shaptb;
          if(bo1>=bn1 && bo2<bn2)tshap1[j]+=am;
          else if(bo1<bn2 && bo2>=bn2){
            w=(bn2-bo1)/TOFMCFFKEY.fladctb;
            tshap1[j]+=w*am;
            tshap1[j+1]+=(1.-w)*am;
            j+=1;
          }
        }
// tshap1[] --- shaping ---> tshap2[] conversion :        
        for(i=0;i<SCTBMX+1;i++)tshap2[i]=0.;
        for(i=0;i<SCTBMX;i++){
          am=tshap1[i];
          if(am>0.){
            for(j=0;j<nshbn;j++){
              if((i+j)<SCTBMX)
                             tshap2[i+j]+=am*shplsh[j];
              else
                             tshap2[SCTBMX]+=am*shplsh[j];
            }
          }
        }
//
        if(TOFMCFFKEY.mcprtf != 0)
                      AMSTOFTovt::displ_as(idd,2,tshap2);//print Shaper pulse
//
// Time_over_threshold measurement for anode :
        _nadca=0;
        updsh=0;
        tshd=-9999.;
        for(i=0;i<SCTBMX;i++){  //  <--- time bin loop ---
          am=tshap2[i];
          tm=(i+1)*TOFMCFFKEY.shaptb;
          if(am>=TOFMCFFKEY.daqthr[3] && updsh==0
                && (tm-tshd)>TOFMCFFKEY.daqpwd[5]){ //dead time check
            updsh=1;
          }
          if(am<TOFMCFFKEY.daqthr[3] && updsh==1){
            updsh=0;
            tshd=tm;
            if(_nadca<SCTHMX4){
              _tadca[_nadca]=tm;
              _nadca+=1;
            }
            else{
              cout<<"AMSTOFTovt::build-warning: a-ADC hits ovfl"<<'\n';
              updsh=2;//blockade on overflow
            } 
          }
        }                    // --- end of time bin loop ---> 
          
//------------------------------  
//     convert fast flash ADC array tslice to shaper pulse (dinode):
//                 
// rebinning of tslice[] into shaper binned tshap1[]
        for(j=0;j<SCTBMX;j++)tshap1[j]=0.; // clear shaper output array
        j=0;                 
        for(i=0;i<SCTBMX;i++){
          a=tslice[i];
          am=a*TOFMCFFKEY.di2anr; //no saturation for dinode signal, just reduction
          bo1=i*TOFMCFFKEY.fladctb;
          bo2=bo1+TOFMCFFKEY.fladctb;
          bn1=j*TOFMCFFKEY.shaptb;
          bn2=bn1+TOFMCFFKEY.shaptb;
          if(bo1>=bn1 && bo2<bn2)tshap1[j]+=am;
          else if(bo1<bn2 && bo2>=bn2){
            w=(bn2-bo1)/TOFMCFFKEY.fladctb;
            tshap1[j]+=w*am;
            tshap1[j+1]+=(1.-w)*am;
            j+=1;
          }
        }
// tshap1[] --- shaping ---> tshap2[] conversion :        
        for(i=0;i<SCTBMX+1;i++)tshap2[i]=0.;
        for(i=0;i<SCTBMX;i++){
          am=tshap1[i];
          if(am>0.){
            for(j=0;j<nshbn;j++){
              if((i+j)<SCTBMX)
                             tshap2[i+j]+=am*shplsh[j];
              else
                             tshap2[SCTBMX]+=am*shplsh[j];
            }
          }
        }
// Time_over_threshold measurement for dinode :
        _nadcd=0;
        updsh=0;
        tshd=-9999.;
        for(i=0;i<SCTBMX;i++){  //  <--- time bin loop ---
          am=tshap2[i];
          tm=(i+1)*TOFMCFFKEY.shaptb;
          if(am>=TOFMCFFKEY.daqthr[4] && updsh==0
                && (tm-tshd)>TOFMCFFKEY.daqpwd[6]){ //dead time check
            updsh=1;
          }
          if(am<TOFMCFFKEY.daqthr[4] && updsh==1){
            updsh=0;
            tshd=tm;
            if(_nadcd<SCTHMX4){
              _tadcd[_nadcd]=tm;
              _nadcd+=1;
            }
            else{
              cout<<"AMSTOFTovt::build-warning: a-ADC hits ovfl"<<'\n';
              updsh=2;//blockade on overflow
            } 
          }
        }                    // --- end of time bin loop ---> 
          
//------------------------------
// now create/fill AMSTOFTovt object (id=idd) with above digi-data:
        _sta=0.;// tempor (to be taken from DB later)    
        stat=0;
        stat=AMSEvent::gethead()->addnext(AMSID("AMSTOFTovt",ilay), new
             AMSTOFTovt(idd,_sta,_ntr1,_ttr1,_ntr2,_ttr2,_ntr3,_ttr3,
                _nftdc,_tftdc,_nstdc,_tstdc,_nadca,_tadca,_nadcd,_tadcd));
//
// prepare(clear) flash-ADC array for next id 
        for(i=0;i<SCTBMX+1;i++)tslice[i]=0.; 
      } // end of "if last or new"
      ptr=ptr->next(); // <===== take next phel-object
    } //  ----- end Phel-hits loop --------->
//
  }  //         -------- end of layer loop ------------------->
//
  AMSgObj::BookTimer.stop("SITOFTOVT");
}
//---------------------------------------------------------------------
//
// function to get absolute time of the FIRST trigger(coincidence) "z>=1";
// trcode - trigger code (pattern) =-1/n, n>=0 -> missing plane #, 
// =-1 -> no trigger.
//
number AMSTOFTovt::tr1time(int &trcode){
  int i1,i2,isds(0);
  integer i,j,ilay,ntr,idd,id,idN,stat;
  number ttr[SCTHMX1],t1,t2;
  AMSBitstr trbs[2];
  AMSBitstr trbc;
  AMSBitstr trbl[SCLRS];
  AMSTOFTovt *ptr;
  AMSTOFTovt *ptrN;
//
  for(ilay=0;ilay<SCLRS;ilay++){// <--- layers loop (Tovt containers) ---
    ptr=(AMSTOFTovt*)AMSEvent::gethead()->
                               getheadC("AMSTOFTovt",ilay,0);
    isds=0;
    while(ptr){// <--- Tovt-hits loop in layer ---
      idd=ptr->getid();
      id=idd/10;// short id=LBB
      stat=ptr->getstat();
      ntr=ptr->gettr1(ttr);// get number and times of trig1 ("z>=1")
      for(j=0;j<ntr;j++){// <--- trig-hits loop ---
        t1=ttr[j];
        t2=t1+TOFMCFFKEY.daqpwd[0];
        i1=t1/TOFMCFFKEY.trigtb;
        i2=t2/TOFMCFFKEY.trigtb;
        if(isds<2)
          if(stat==0)trbs[isds].setbit(i1,i2);//set bits acc. to pulse width
        else
            cout<<"AMSTOFTovt::tr1time: error: >2sides !!!"<<'\n';
        isds+=1; 
      }//             --- end of trig-hits loop --->
      ptrN=ptr->next();
      idN=0;
      if(ptrN)idN=(ptrN->getid())/10; //next hit short id
//
      if(idN != id){ // next hit is OTHER counter hit or last hit,
//       so create both side (counter) bit pattern for CURRENT counter
        if(TOFMCFFKEY.trlogic[0] == 0)
                                  trbc=trbs[0] & trbs[1];// 2-sides AND
        else
                                  trbc=trbs[0] | trbs[1];// 2-sides OR
        trbl[ilay]=trbl[ilay] | trbc; // summing OR within the layer
        isds=0; // reset c.sides variables ...
        trbs[0].clrbit(1,0);
        trbs[1].clrbit(1,0);
      }
      ptr=ptr->next();// take next Tovt-hit
// 
    }//         --- end of Tovt-hits loop in layer --->
//
    trbl[ilay].testbit(i1,i2);// check ilay-plane "OR"
    if(i2>=i1){// make stand.logic pulse of daqpwd[0] width
      i2=TOFMCFFKEY.daqpwd[0]/TOFMCFFKEY.trigtb;
      i2+=i1;
      trbl[ilay].clrbit(1,0);
      trbl[ilay].setbit(i1,i2);
    }
  } //                               --- end of layer loop --->
//
  trcode=-1;
  AMSBitstr coinc1234=trbl[0]&trbl[1]&trbl[2]&trbl[3];
  coinc1234.testbit(i1,i2);
  if(i2>=i1){// have 4-fold coincidence
    trcode=0;
    t1=i1*TOFMCFFKEY.trigtb;
    return t1;
  }
  else{   // check 3-fold coincidence:
    int imin=9999;
    AMSBitstr coinc0234=trbl[1]&trbl[2]&trbl[3];
    AMSBitstr coinc1034=trbl[0]&trbl[2]&trbl[3];
    AMSBitstr coinc1204=trbl[0]&trbl[1]&trbl[3];
    AMSBitstr coinc1230=trbl[0]&trbl[1]&trbl[2];
    coinc0234.testbit(i1,i2);
    if((i2>=i1) && (i1<imin)){
      imin=i1;
      trcode=1;
    }
    coinc1034.testbit(i1,i2);
    if((i2>=i1) && (i1<imin)){
      imin=i1;
      trcode=2;
    }
    coinc1204.testbit(i1,i2);
    if((i2>=i1) && (i1<imin)){
      imin=i1;
      trcode=3;
    }
    coinc1230.testbit(i1,i2);
    if((i2>=i1) && (i1<imin)){
      imin=i1;
      trcode=4;
    }
    t1=0.;
    if(imin!=9999)t1=i1*TOFMCFFKEY.trigtb;
    return t1;
  }
}
//=====================================================================
//
//  function to set bits in AMSBitstr objects (from bit il till bit ih):
//   (bits are counting starting from 0 (left edge of bit-array))
//   (if il>ih, function sets all available bits)
//
void AMSBitstr::setbit(const int il, const int ih){
//
  static unsigned short int allone(0xFFFF);
  static unsigned short int setone[16]={
       0x8000,0x4000,0x2000,0x1000,0x0800,0x0400,0x0200,0x0100,
       0x0080,0x0040,0x0020,0x0010,0x0008,0x0004,0x0002,0x0001};
  int i,iw1,iw2,i1,i2,nw;
  iw1=il/16;
  i1=il-16*iw1;
  if(il==ih){
    bitstr[iw1]|=setone[i1];
    return;
  }
  if(il>ih){
    for(i=0;i<bslen;i++)bitstr[i]=allone;//set all bits
    return;
  }
  iw2=ih/16;
  i2=ih-16*iw2;
  nw=iw2-iw1;
  if(nw==0){
    for(i=i1;i<=i2;i++)bitstr[iw1]|=setone[i];
  }
  else{
    for(i=i1;i<=15;i++)bitstr[iw1]|=setone[i];
    for(i=0;i<=i2;i++)bitstr[iw2]|=setone[i];
    for(i=iw1+1;i<iw2;i++){bitstr[i]=allone;}
  }
  return;
}
//----------------------------------------------------------------------
//  function to clear bits in AMSBitstr objects (from bit il upto bit ih):
//   (bits are counting starting from 0 (left edge of bit-array))
//   (if il>ih, function clear all available bits)
//
void AMSBitstr::clrbit(const int il, const int ih){
//
  static unsigned short int allzer(0x0000);
  static unsigned short int setzer[16]={
        0x7FFF,0xBFFF,0xDFFF,0xEFFF,0xF7FF,0xFBFF,0xFDFF,0xFEFF,
        0xFF7F,0xFFBF,0xFFDF,0xFFEF,0xFFF7,0xFFFB,0xFFFD,0xFFFE};
  int i,iw1,iw2,i1,i2,nw;
  iw1=il/16;
  i1=il-16*iw1;
  if(il==ih){
    bitstr[iw1]&=setzer[i1];
    return;
  }
  if(il>ih){
    for(i=0;i<bslen;i++)bitstr[i]=allzer;//clear all bits
    return;
  }
  iw2=ih/16;
  i2=ih-16*iw2;
  nw=iw2-iw1;
  if(nw==0){
    for(i=i1;i<=i2;i++)bitstr[iw1]&=setzer[i];
  }
  else{
    for(i=i1;i<=15;i++)bitstr[iw1]&=setzer[i];
    for(i=0;i<=i2;i++)bitstr[iw2]&=setzer[i];
    for(i=iw1+1;i<iw2;i++){bitstr[i]=allzer;}
  }
  return;
}
//-----------------------------------------------------------------------
// function to test "up" bits in AMSBitstr object. It returns i1/i2 as
// first/last "up" bits of the FIRST continious bunch of "up" bits 
// in the object.  i1>i2 (1>0) if no "up" bits.
//
void AMSBitstr::testbit(int &i1, int &i2){
  static unsigned short int setone[16]={
       0x8000,0x4000,0x2000,0x1000,0x0800,0x0400,0x0200,0x0100,
       0x0080,0x0040,0x0020,0x0010,0x0008,0x0004,0x0002,0x0001};
  int i,iw;
  int ifl(0);
  unsigned short int ia;
  i1=1;
  i2=0;
  for(iw=0;iw<bslen;iw++){
    for(i=0;i<16;i++){
      ia=bitstr[iw] & setone[i];
      if((ia != 0) && (ifl == 0)){
        i1=iw*16+i;
        ifl=1;
      }
      if((ia == 0) && (ifl == 1)){
        i2=iw*16+i-1;
        ifl=0;
        return;
      }
    }
  }
  if(ifl==1){// case when bits are "up" till the end of bitstream
    i2=16*bslen-1;
    return;
  }
}
//-----------------------------------------------------------------------
AMSBitstr operator &(const AMSBitstr &b, const AMSBitstr &c){
  int lenb=b.bslen;
  int lenc=c.bslen;
  unsigned short int arr[64];
  if(lenb != lenc){
    cout<<"AMSBitstr:AND-wrong_length"<<lenb<<" "<<lenc<<'\n';
    exit(1);
  }
  for(int i=0;i<lenb;i++)arr[i]= b.bitstr[i] & c.bitstr[i];
  return AMSBitstr(lenb,arr);
} 
//-----------------------------------------------------------------------
AMSBitstr operator |(const AMSBitstr &b, const AMSBitstr &c){
  int lenb=b.bslen;
  int lenc=c.bslen;
  unsigned short int arr[64];
  if(lenb != lenc){
    cout<<"AMSBitstr:OR-wrong_length"<<lenb<<" "<<lenc<<'\n';
    exit(1);
  }
  for(int i=0;i<lenb;i++)arr[i] = b.bitstr[i] | c.bitstr[i];
  return AMSBitstr(lenb,arr);
} 
//-----------------------------------------------------------------------
void AMSBitstr::display(char comment[]){
    printf("AMSBitstr: %s ;  length=%d\n",comment,bslen);
    for(int i=1;i<=bslen;i++){
      if(i%10 != 0)
                  printf(" %#x",bitstr[i-1]);
      else 
                  printf(" %#x\n",bitstr[i-1]);
    }
    if(bslen%10 !=0)printf("\n");
  }
//
//=====================================================================
//
//  function to build RawEvent-objects from MC Tovt-objects
//               (very preliminary !!!)
void AMSTOFRawEvent::mc_build()
{
  integer i,j,ilay,last,ibar,id,idd,iddN,isd,_sta,stat(0);
  integer nftdc,nstdc,nadca,nadcd;
  int16u _nftdc,_nstdc,_nadca,_nadcd;
  number  tftdc[SCTHMX2],tstdc[SCTHMX3],tadca[SCTHMX4],tadcd[SCTHMX4];
  int16u  ftdc[SCTHMX2],stdc[SCTHMX3],adca[SCTHMX4],adcd[SCTHMX4];
  number t,ttrig1,dt;
  int trcode;
  int16u it;
  AMSTOFTovt *ptr;
  AMSTOFTovt *ptrN;
//
  ttrig1=AMSTOFTovt::tr1time(trcode);//get abs. trig1("z>=1" accept) signal time
  if(trcode<0)return; // no trigger
//  
  for(ilay=0;ilay<SCLRS;ilay++){// <--- layers loop (Tovt containers) ---
    ptr=(AMSTOFTovt*)AMSEvent::gethead()->
                               getheadC("AMSTOFTovt",ilay,0);
    while(ptr){// <--- Tovt-hits loop in layer ---
      idd=ptr->getid();
      id=idd/10;// short id=LBB
      stat=ptr->getstat();
//
      nftdc=ptr->getftdc(tftdc);// get number and times of fast-TDC hits
      _nftdc=0;
      for(j=0;j<nftdc;j++){// <--- ftdc-hits loop ---
        t=tftdc[j];
        dt=ttrig1+TOFMCFFKEY.accdel[0]-t;//const.delay of lev-1 "accept" signal is added
        if(dt<=TOFMCFFKEY.accdelmx[0]){ // check max. delay of lev-1 "accept" signal
          it=dt/TOFMCFFKEY.tdcbin[0]; // conv. to fast-TDC (history) t-binning
          ftdc[_nftdc]=it;
          _nftdc+=1;
        }
      }//--- end of ftdc-hits loop --->
//
      nstdc=ptr->getstdc(tstdc);// get number and times of slow-TDC hits
      _nstdc=0;
      for(j=0;j<nstdc;j++){// <--- stdc-hits loop ---
        t=tstdc[j];
        dt=ttrig1+TOFMCFFKEY.accdel[1]-t;// const.delay is added
        if(dt<=TOFMCFFKEY.accdelmx[1]){ // check max. delay of "accept" signal
          it=dt*TOFMCFFKEY.strrat/
                TOFMCFFKEY.tdcbin[1];//conv. to stratcher-TDC t-binning 
          stdc[_nstdc]=it;
          _nstdc+=1;
        }
      }//--- end of stdc-hits loop --->
//
      nadca=ptr->getadca(tadca);// get number and times of anode-ADC hits
      for(j=0;j<nadca;j++){// <--- adca-hits loop ---
        t=tadca[j];
        it=t/TOFMCFFKEY.tdcbin[2];
        adca[j]=it;
        _nadca=nadca;
      }//--- end of adca-hits loop --->
//
      nadcd=ptr->getadcd(tadcd);// get number and times of dinode-ADC hits
      for(j=0;j<nadcd;j++){// <--- adcd-hits loop ---
        t=tadcd[j];
        it=t/TOFMCFFKEY.tdcbin[2];
        adcd[j]=it;
        _nadcd=nadcd;
      }//--- end of adcd-hits loop --->
//
//     ---> create/fill RawEvent-object :
      stat=0;
      stat=AMSEvent::gethead()->addnext(AMSID("AMSTOFRawEvent",0), new
           AMSTOFRawEvent(idd,stat,_nftdc,ftdc,_nstdc,stdc,
                               _nadca,adca,_nadcd,adcd));
//
      ptr=ptr->next();// take next Tovt-hit
// 
    }//         --- end of Tovt-hits loop in layer --->
//
  } //                               --- end of layer loop --->
//
}
