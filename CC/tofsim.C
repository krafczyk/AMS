// Author Choumilov.E. 10.07.96.
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
#include <tofdbc.h>
#include <tofsim.h>
#include <mccluster.h>
//
extern TOFVarp tofvpar;
extern TOFBrcal scbrcal[SCLRS][SCMXBR];// calibration data
AMSTOFScan scmcscan[SCBLMX];
//
integer AMSTOFRawEvent::trpatt[SCLRS]={0,0,0,0};
integer AMSTOFRawEvent::trflag=0;
number AMSTOFRawEvent::trtime=0.;
//------------------------------------------------------------------------------
geant AMSDistr::getrand(const geant &rnd)
{
  geant val;
  integer i=AMSbins(distr,rnd,nbin);
  if(i<0){
    i=-i;
    val=bnl+bnw*i+bnw*(rnd-distr[i-1])/(distr[i]-distr[i-1]);
  }
  else if(i==0){val=bnl+bnw*rnd/distr[i];}
  else {val=bnl+bnw*i;}
  return val;
}
//------------------------------------------------------------------------------
//  <--- for inp. X define position in scan array (i1/i2) and correction ratio R.
//
void AMSTOFScan::getxbin(const geant x, integer &i1, integer &i2, geant &r)
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
geant AMSTOFScan::getef1(const geant r, const int i1, const int i2)
{
  if(i1==i2){return eff1[i1];}
  else{return (eff1[i1]+(eff1[i2]-eff1[i1])*r);}
}
geant AMSTOFScan::getef2(const geant r, const int i1, const int i2)
{
  if(i1==i2){return eff2[i1];}
  else{return (eff2[i1]+(eff2[i2]-eff2[i1])*r);}
}
//-------------------------------------------------------------------
// function to create AMSTOFScan objects for all sc. bars :
void AMSTOFScan::build(){
//
  int i,ic,nverst,ivers,dig;
  int brfnam[SCBLMX];
  char fname[80];
  char name[80];
  char in[2]="0";
  char inum[11];
  char verlst[20]="verslist.dat";
  char tdisfn[20]="tdisfmap";
//
  strcpy(inum,"0123456789");
//
// ---> read version number of "tdisfmap"-file from verslist-file:
//
  strcpy(fname,AMSDATADIR.amsdatadir);
//  strcpy(fname,"/afs/cern.ch/user/c/choumilo/public/ams/AMS/tofca/");//tempor
  strcat(fname,verlst);
  cout<<"AMSTOFScan::build: Open file  "<<fname<<'\n';
  ifstream vlfile(fname,ios::in); // open needed tdfmap-file for reading
  if(!vlfile){
    cerr <<"AMSTOFScan::build(): missing verslist-file "<<fname<<endl;
    exit(1);
  }
  vlfile >> nverst;// total number of files in the list
  vlfile >> ivers;// first number in first line (vers.# for tdisfmap-file)
  vlfile.close();
  ivers=(ivers%100);
  cout<<"AMSTOFScan::build(): use tdisfmap-file version="<<ivers<<'\n';
//  
//                                  <-- first read t-distr. map-file
  strcpy(name,tdisfn);
  dig=ivers/10;
  in[0]=inum[dig];
  strcat(name,in);
  dig=ivers%10;
  in[0]=inum[dig];
  strcat(name,in);
  strcat(name,".dat");
  strcpy(fname,AMSDATADIR.amsdatadir);    
//  strcpy(fname,"/afs/cern.ch/user/c/choumilo/public/ams/AMS/tofca/");//tempor
  strcat(fname,name);
  cout<<"Open file : "<<fname<<'\n';
  ifstream tcfile(fname,ios::in); // open needed tdisfmap-file for reading
  if(!tcfile){
    cerr <<"AMSTOFScan::build(): missing tdisfmap-file "<<fname<<endl;
    exit(1);
  }
  for(ic=0;ic<SCBLMX;ic++) tcfile >> brfnam[ic];
  tcfile.close();
//-------------------
//                                  <-- now read t-distr. files
 int j,ila,ibr,brt,ibrm,isp,nsp,ibt,cnum,dnum,mult;
 integer nb;
 geant scp[SCANPNT];
 geant nft,bl,bw;
 geant arr[AMSDISL];
 geant ef1[SCANPNT],ef2[SCANPNT];
 AMSDistr td1[SCANPNT];
 AMSDistr td2[SCANPNT];
 geant eff1,eff2;
//
  for(ila=0;ila<SCLRS;ila++){   // <-------- loop over layers
  for(ibr=0;ibr<SCMXBR;ibr++){  // <-------- loop over bar in layer
    brt=TOFDBc::brtype(ila,ibr);
    if(brt==0)continue; // skip missing counters
    cnum=ila*SCMXBR+ibr; // sequential counter numbering(0-55)
    dnum=brfnam[cnum];// 4-digits t-distr. file name (VLBB)
    mult=1000;
    strcpy(name,"");
    for(i=3;i>=0;i--){//create 4-letters file name
      j=dnum/mult;
      in[0]=inum[j];
      strcat(name,in);
      dnum=dnum%mult;
      mult=mult/10;
    }
    strcat(name,".dat");
    strcpy(fname,AMSDATADIR.amsdatadir);
//    strcpy(fname,"/afs/cern.ch/user/c/choumilo/public/ams/AMS/tofca/");//tempor
    strcat(fname,name);
    cout<<"Open file : "<<fname<<'\n';
    ifstream tcfile(fname,ios::in); // open needed t-calib. file for reading
    if(!tcfile){
      cerr <<"AMSTOFScan::build(): missing MC-t_distr. file "<<fname<<endl;
      exit(1);
    }
// <-- fill errays scp,ef1,ef2 from file
//
    tcfile >> nsp;// read # of calibr. points
    if(nsp!=SCANPNT){
      cerr<<"Sitofinitjob: bad # of MC Y-scan point ! "<<nsp<<'\n';
      exit(1);
    } 
    for(isp=0;isp<SCANPNT;isp++){ // sp. points loop to prepare arr. of t-distr
      tcfile >> scp[isp];
      tcfile >> nft;   // for PM-1
      tcfile >> nb;
      tcfile >> bl;
      tcfile >> bw;
      tcfile >> ef1[isp];
      for(i=0;i<nb;i++){arr[i]=0.;}
      for(i=0;i<nb;i++){tcfile >> arr[i];}
      td1[isp]=AMSDistr(nb,bl,bw,arr);
      tcfile >> nft;   // for PM-2
      tcfile >> nb;
      tcfile >> bl;
      tcfile >> bw;
      tcfile >> ef2[isp];
      for(i=0;i<nb;i++){arr[i]=0.;}
      for(i=0;i<nb;i++){tcfile >> arr[i];}
      td2[isp]=AMSDistr(nb,bl,bw,arr);
//
    } // <--- end of scan points loop -----
//
    scmcscan[cnum]=AMSTOFScan(scp,ef1,ef2,td1,td2);// create bar MC-t-scan obj
    tcfile.close(); // close file
  } // --- end of bar loop --->
  } // --- end of layer loop --->
}
//===================================================================
//
void AMSTOFTovt::build()
{
//      <--- prepare PM single electron pulse height distribution:
  static geant arr[19]={50.,20.,39.7,46.5,52.3,54.6,52.7,47.6,40.7,
                          32.5,23.9,16.7,11.,7.,4.5,3.,1.5,1.,0.5};
  static AMSDistr scpmsesp(19,0.,0.279,arr);// p/h spectrum ready
//                   ( scale in mV@50ohm to have 1.5mV as m.p. value!!!)
//
  integer i1,i2,id,idN,idd,ibar,ilay,ibtyp,cnum;
  static geant tslice1[SCTBMX+1]; //  flash ADC array for side-1
  static geant tslice2[SCTBMX+1]; //  flash ADC array for side-2
  static geant tslice[SCTBMX+1]; //  flash ADC array 
  int i,ii,j,jj,jm,k,stat(0),nelec,ierr(0);
  int status[2];
  integer nhitl[SCLRS];
  geant edepl[SCLRS],edepb;
  geant dummy(-1);
  geant time,x,y,z,am,am2,r,rnd,eff,de,tm,eps(0.002);
  geant vl,vh;
  geant nel0,nel,nphot;
  AMSPoint cloc(999.,999.,999.);
  AMSgvolume *p;
  AMSTOFMCCluster *ptr;
  AMSTOFMCCluster *ptrN;
//
  static integer first=0;
  static integer npshbn;
  static geant pmplsh[1000];  // PM s.e. pulse shape
  static geant fadcb,ifadcb,trts,tmax,zlmx,convr,seres,sesig;
//
  if(first++==0){
    seres=TOFDBc::seresp();
    sesig=seres*TOFDBc::seresv();
    AMSTOFTovt::inipsh(npshbn,pmplsh); // prepare PM s.e. pulse shape arr.
    HBOOK1(1111,"Single electron spectrum,mV",65,0.,13.,0.);
    for(i=0;i<3000;i++){
//      rnd=RNDM(dummy);
//      am=scpmsesp.getrand(rnd);//amplitude from single elect. spectrum
      am=seres+sesig*rnormx();
      if(am<0)am=0;
      HF1(1111,am,1.);
    }
    HPRINT(1111);
    HDELET(1111);
    for(i=0;i<SCTBMX+1;i++)tslice1[i]=0.;// clear flash ADC arrays
    for(i=0;i<SCTBMX+1;i++)tslice2[i]=0.;
    for(i=0;i<SCTBMX+1;i++)tslice[i]=0.;
    fadcb=TOFDBc::fladctb();
    ifadcb=1./fadcb;
    tmax=SCTBMX*fadcb;
    zlmx=TOFDBc::plnstr(6)/2.+eps;
    convr=1000.*TOFDBc::edep2ph();
  }
//
  ptr=(AMSTOFMCCluster*)AMSEvent::gethead()->
                                      getheadC("AMSTOFMCCluster",0,1);
//                                        ( sort by idsoft done)
//
  for(i=0;i<SCLRS;i++){
    nhitl[i]=0;
    edepl[i]=0.;
  }
//
  edepb=0.;// Edep in given bar
  while(ptr){//       <------------- loop over geant hits
    id=ptr->idsoft; 
    ilay=id/100-1;
    nhitl[ilay]+=1;
    ibar=id%100-1;
    de  =ptr->edep;
    edepl[ilay]+=de;
    edepb+=de;
    time=(1.e+9)*(ptr->tof); // conv. to "ns"
    x=ptr->xcoo[0];
    y=ptr->xcoo[1];
    z=ptr->xcoo[2];
    AMSPoint cglo(x,y,z);
    p=AMSJob::gethead()->
           getgeomvolume(AMSID("TOFS",id));// pointer to volume "TOFS",id
#ifdef __AMSDEBUG__
    assert(p != NULL);
#endif
    cloc=p->gl2loc(cglo);// convert global coord. to local
    x=cloc[0];
    y=cloc[1];
    z=cloc[2];
    if(time>=tmax){
      //      cerr<<"TOF: Bad G-hit, id="<<id<<" t/de="<<time<<" "<<de<<'\n';
      ptr=ptr->next(); // next hit
      continue;
    }
    cnum=ilay*SCMXBR+ibar;// sequential counter number
    scmcscan[cnum].getxbin(y,i1,i2,r);//y-bin # (longit.(x !)-coord. in LTRANS )
    scbrcal[ilay][ibar].getbstat(status);//get status of two ends
    nel0=de*convr;// -> photons
// PM-1 actions --->
  if(status[0]==0){// alive side
    eff=scmcscan[cnum].getef1(r,i1,i2);//eff for PM-1
    nel=nel0*eff;// mean number of photoelectrons
    POISSN(nel,nelec,ierr);// fluctuations
//    <-------- Loop over photoelectrons(PM-1) ---<<<
    for(i=1;i<=nelec;i++){
      rnd=RNDM(dummy);
      tm=scmcscan[cnum].gettm1(rnd,r,i1,i2);//phel.arrival time from interpol.distr.
      tm=tm+time;// TOF+delay(incl.trans.time spread from LTRANS progr.)
//      rnd=RNDM(dummy);
//      am=scpmsesp.getrand(rnd);//amplitude from single elect. spectrum
      am=seres+sesig*rnormx();//amplitude from single elect. spectrum
//----->  summing of all amplitudes from photoelectrons :
      if(am>0){
        ii=integer(tm*ifadcb);
        if(ii>SCTBMX)ii=SCTBMX;
        tslice1[ii]+=am;
      }
    } // >>>----- end of PM-1 loop ------>
  }
//
// PM-2 actions --->
  if(status[1]==0){// alive side
    eff=scmcscan[cnum].getef2(r,i1,i2);//eff for PM-2
    nel=nel0*eff;// mean number of photoelectrons
    POISSN(nel,nelec,ierr);// fluctuations
//    <-------- Loop over photoelectrons(PM-2) ---<<<
    for(i=1;i<=nelec;i++){
      rnd=RNDM(dummy);
      tm=scmcscan[cnum].gettm2(rnd,r,i1,i2);//phel.arrival time from interpol. distr.
      tm=tm+time;// TOF+delay(incl.trans.time spread from LTRANS progr.)
//      rnd=RNDM(dummy);
//      am=scpmsesp.getrand(rnd);//amplitude from single elect. spectrum
      am=seres+sesig*rnormx();//amplitude from single elect. spectrum
//----->  summing of all amplitudes from photoelectrons :
      if(am>0){
        ii=integer(tm*ifadcb);
        if(ii>SCTBMX)ii=SCTBMX;
        tslice2[ii]+=am;
      }
    } // >>>----- end of PM-2 loop ------>
  }
//-----------------------------------
      ptrN=ptr->next();
      idN=0; 
      if(ptrN)idN=ptrN->getid();// id of the next G-hit
      if(idN != id){ // <---- last or new bar -> create Tovt-objects :
        edepb*=1000.;// --->MeV
        if(edepb>TOFMCFFKEY.Thr){// process only bar with Edep>Ethr
// PM-1 loop to apply pulse shape :
        if(status[0]==0){// alive side
          idd=id*10+1;//LBBS
          for(i=0;i<SCTBMX+1;i++)tslice[i]=0.;
          for(i=0;i<SCTBMX;i++){
            am=tslice1[i];
            if(am>0){
              jm=i+npshbn;
              if(jm>SCTBMX)jm=SCTBMX+1;
              for(j=i;j<jm;j++)
                          tslice[j]+=am*pmplsh[j-i];
            }
          }        
          AMSTOFTovt::totovt(idd,edepb,tslice);// Tovt-obj for side(PM)-1
          if(TOFMCFFKEY.mcprtf[1] != 0)
                        AMSTOFTovt::displ_a(idd,20,tslice);//print PMT pulse
        }
//        
// PM-2 loop to apply pulse shape :
        if(status[1]==0){// alive side
          idd=id*10+2;
          for(i=0;i<SCTBMX+1;i++)tslice[i]=0.;
          for(i=0;i<SCTBMX;i++){
            am=tslice2[i];
            if(am>0){
              jm=i+npshbn;
              if(jm>SCTBMX)jm=SCTBMX+1;
              for(j=i;j<jm;j++)
                          tslice[j]+=am*pmplsh[j-i];
            }
          }        
          AMSTOFTovt::totovt(idd,edepb,tslice);// Tovt-obj for side(PM)-2
          if(TOFMCFFKEY.mcprtf[1] != 0)
                        AMSTOFTovt::displ_a(idd,20,tslice);//print PMT pulse
        }
        }// ---> end of edep check
//
        for(i=0;i<SCTBMX+1;i++)tslice1[i]=0.;//clear flash ADC arrays for next bar
        for(i=0;i<SCTBMX+1;i++)tslice2[i]=0.;
        edepb=0.;// clear Edep
      }// ---> end of next/last bar check
//
    ptr=ptr->next();
  }// ------ end of geant hits loop ---->
//
  if(TOFMCFFKEY.mcprtf[2]!=0){
    for(i=0;i<SCLRS;i++)HF1(1050+i,geant(nhitl[i]),1.);
    HF1(1060,1000.*edepl[0],1.);
    HF1(1061,1000.*edepl[0],1.);
    HF1(1062,1000.*edepl[2],1.);
    HF1(1063,1000.*edepl[2],1.);
  }

//
}
//=========================================================================
//
// function below creates PMT pulse shape array arr[] with binning fladctb :
//  peak value = 1.(e.g. 1mV@50ohm)
void AMSTOFTovt::inipsh(integer &nbn, geant arr[])
{
  static integer tbins=12;
//  static geant pmpsh[15]={0.,0.026,0.175,0.422,0.714,0.942,1.,0.935,
//       0.753,0.584,0.442,0.305,0.182,0.078,0.};// pulse heights at time points
//  static geant pmpsb[15]={0.,0.5,1.,1.5,2.,2.5,3.,3.5,4.,4.5,5.,
//       5.5,6.,6.5,7.}; // time points (min. step should be above fladctb)
  static geant pmpsh[12]={0.,0.132,0.85,1.,0.83,0.68,0.35,0.15,0.078,
       0.059,0.029,0.};// pulse heights at time points
  static geant pmpsb[12]={0.,1.,2.25,3.5,4.75,6.,8.5,11.,13.5,16.,18.5,
                      21.}; // time points (min. step should be above fladctb)
  integer i,io,ib;
  geant bl,bh,bol,boh,ao1,ao2,tgo,al,ah,tota;
  tota=0.;
  io=0;
  for(ib=0;ib<1000;ib++){
    bl=ib*TOFDBc::fladctb();
    bh=bl+TOFDBc::fladctb();
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
      if((io+1)<tbins){
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
        if(TOFMCFFKEY.mcprtf[0] != 0){
          tota=tota*TOFDBc::fladctb()/50.;
          printf("PM pulse shape:nbins=% 3d, Integral(pC) = %4.2e\n",nbn,tota);
          for(i=1;i<=nbn;i++){
            if(i%10 != 0)
                         printf("% 4.2e",arr[i-1]);
            else
                         printf("% 4.2e\n",arr[i-1]);
          }
          if(nbn%10 !=0)printf("\n");
        }
        return;
      }
    }
  }
  cerr<<"AMSTOFTovt-inipsh-Fatal: wrong pulse shape !?"<<'\n';
  exit(1);
}
//--------------------------------------------------------------------------
//
// function below creates shaper stand. pulse shape array arr[] with bin=shaptb :
// (this array is shaper responce to 1pC injection with duration=shaptb;
//  shaper rise/fall times are shrtim<<shftim; nbn is array length,defined as
//  boundary where charge becomes less than 0.001 of initial value(~6.91*Tfall)
//  of the peak value)
//
void AMSTOFTovt::inishap(integer &nbn, geant arr[])
{
  geant tr,tf,tmb,toti,rest,t1,t2,anr;
  integer i,imax,j,ifl(0);
  tr=TOFDBc::shrtim();
  tf=TOFDBc::shftim();
  tmb=TOFDBc::shaptb();
  imax=500;
  arr[0]=1.-exp(-tmb/tr);// fast charge (neglect by discharge !)
  for(i=1;i<imax;i++){ // <--- time loop ---
    t1=i*tmb;
    arr[i]=arr[0]*exp(-t1/tf);
    if(arr[i]<(0.001*arr[0])){
      nbn=i+1;
      if(TOFMCFFKEY.mcprtf[0] != 0){
        printf("Shaper pulse shape ready : nbins=% 3d\n",nbn);
        for(j=1;j<=nbn;j++){
          if(j%10 != 0)
                       printf("% 4.2e",arr[j-1]);
          else
                       printf("% 4.2e\n",arr[j-1]);
        }
        if(nbn%10 !=0)printf("\n");
      }
      return;
    }
  } // --- end if time loop --->
  cerr<<"AMSTOFTovt-inishap-Fatal: array size too small ?!"<<'\n';
  cerr<<"trise/tfall= "<<tr<<" "<<tf<<" nbins="<<i<<'\n';
  exit(1);
}
//--------------------------------------------------------------------------
//
// function below simulate PMT saturation, it returns nonsat. factor <=1. :
//
geant AMSTOFTovt::pmsatur(const geant am){
  geant gain=1.; //tempor. no saturation
  return gain;
}
//--------------------------------------------------------------------------
//
// function to display PMT pulse (flash-ADC array arr[]) :
//
void AMSTOFTovt::displ_a(const int id, const int mf, const geant arr[]){
  integer i;
  static integer ifrs(0);
  geant tm,a(0.);
  static geant tb,tbi;
  if(ifrs++==0){
    tb=geant(mf)*TOFDBc::fladctb();
    tbi=TOFDBc::fladctb();
    HBOOK1(1000,"PMT flash-ADC pulse (MC)",80,0.,80*tb,0.);
  }
  HRESET(1000," ");
  cerr<<"PMT:counter-id = "<<id<<" (LBBS, L-layer, BB-bar, S-side)"<<'\n';
  for(i=1;i<=SCTBMX;i++){
    if(i%mf==0){
      a+=arr[i-1];
      tm=i*tbi-0.5*tb;
      HF1(1000,tm,a/geant(mf));
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
void AMSTOFTovt::displ_as(const int id, const int mf, const geant arr[]){
  integer i;
  static integer ifrs(0);
  geant tm,a(0.);
  static geant tb,tbi;
  if(ifrs++==0){
    tb=geant(mf)*TOFDBc::shaptb();
    tbi=TOFDBc::shaptb();
    HBOOK1(1001,"Shaper pulse (MC)",80,0.,80*tb,0.);
  }
  cerr<<"SHP:counter-id = "<<id<<" (LBBS, L-layer, BB-bar, S-side)"<<'\n';
  HRESET(1001," ");
  for(i=1;i<=SCSBMX;i++){
    if(i%mf==0){
      a+=arr[i-1];
      tm=i*tbi-0.5*tb;
      HF1(1001,tm,a/geant(mf));
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
void AMSTOFTovt::totovt(integer idd, geant edepb, geant tslice[])
{
  integer i,j,ij,ilay,ibar,isid,id,_sta,stat(0);
  geant tm,a,am,amd,tmp,amp,amx,amxq;
  geant iq0,it0,itau;
  integer _ntr1,_ntr2,_ntr3,_nftdc,_nstdc,_nadca,_nadcd;
  number _ttr1[SCTHMX1],_ttr2[SCTHMX1],_ttr3[SCTHMX1];
  number _tftdc[SCTHMX2],_tftdcd[SCTHMX2];
  number _tstdc[SCTHMX3];
  number _tadca[SCTHMX4],_tadcad[SCTHMX4],_tadcd[SCTHMX4],_tadcdd[SCTHMX4];
  number tovt;
  int upd1,upd2,upd3; // up/down flags for 3 fast discr. (z>=1;z>1;z>2)
  int upd11,upd12,upd13,upd21,upd31;
  int updsh;
  int imax,imin;
  geant a2dr[2],d2a;
  geant tshd,tshup,charge,saturf;
  geant td1b1,td1b2,td1b2d,td1b3;
  geant td2b1,td3b1;
  geant tmd1u,tmd1d,tmd3u,tmd3d,tbn,w,bo1,bo2,bn1,bn2,tmark;
  geant tshap1[SCSBMX+1]; // temporary shaper pulse array
  geant tshap2[SCSBMX+1]; // temporary shaper pulse array
  static integer first=0;
  static integer nshbn,mxcon,mxshc;
  static geant fladcb,shapb,cconv;
  geant daqt0,daqt1,daqt2,daqt3,daqt4;
  static geant daqp0,daqp2,daqp3,daqp4,daqp5,daqp6,daqp7,daqp8,daqp9,daqp10;
  static geant daqp11,daqp12;
  static geant shplsh[500];  // Shaper standard pulse shape array
//
  id=idd/10;// LBB
  isid=idd%10-1;// side
  ilay=id/100-1;
  ibar=id%100-1;
//
  if(first++==0){
    AMSTOFTovt::inishap(nshbn,shplsh); // prepare Shaper stand. pulse shape arr.
    fladcb=TOFDBc::fladctb();          // and other time-stable parameters
    shapb=TOFDBc::shaptb();
    cconv=fladcb/50.; // for mV->pC (50 Ohm load)
    daqp0=TOFDBc::daqpwd(0);//max(=min now) duration of FT(Z>=1) trig.pulse 
    daqp2=TOFDBc::daqpwd(2);//max(=min now) duration of Z>2 trig.pulse 
    daqp3=TOFDBc::daqpwd(3);//dead time for fast-TDC signal (dbl-pulse resol)
    daqp4=TOFDBc::daqpwd(4);
    daqp5=TOFDBc::daqpwd(5);
    daqp6=TOFDBc::daqpwd(6);
    daqp7=TOFDBc::daqpwd(7);// dead time for z>=1(FT) branch of adode-discr.
    daqp9=TOFDBc::daqpwd(9);// dead time for z>2 branch of dinode-discr.
    daqp10=TOFDBc::daqpwd(10);// intrinsic t-dispersion of comparator(on low thr.outp)
    daqp11=TOFDBc::daqpwd(11);// min.duration time of comparator(..................)
  }
// time-dependent parameters !!! :
  daqt0=tofvpar.daqthr(0); // fast discr. thresh. for slow/fast TDC branch
  daqt1=tofvpar.daqthr(1); // fast discr. thresh. for z>=1 (FT) branch
  scbrcal[ilay][ibar].geta2dr(a2dr);//get anode_to_dinode ratio
  daqt2=tofvpar.daqthr(2)*a2dr[isid];//dinode discr.("Z>2") thresh., multipl. by a2d ratio
  d2a=1./a2dr[isid];//dinode-to-anode ratio
//
  itau=scbrcal[ilay][ibar].getaipar(isid,0);// get A-integrator parameters
  it0=scbrcal[ilay][ibar].getaipar(isid,1);
  iq0=scbrcal[ilay][ibar].getaipar(isid,2);
  daqt3=exp(it0/itau);// threshold for Anode TovT measurement (pC)
  itau=scbrcal[ilay][ibar].getdipar(isid,0);// get D-integrator parameters
  it0=scbrcal[ilay][ibar].getdipar(isid,1);
  iq0=scbrcal[ilay][ibar].getdipar(isid,2);
  daqt4=exp(it0/itau);// threshold for Dinode TovT measurement (pC)
//
// -----> create/fill summary Tovt-object for idsoft=idd :
//
        if(tslice[SCTBMX]>0){
          cerr<<"SITOFTovt-warning: MC_flash-ADC overflow, id="<<idd<<
             "  A-last= "<<tslice[SCTBMX]<<'\n';
//          if(TOFMCFFKEY.mcprtf[2])AMSTOFTovt::displ_a(idd,125,tslice);//print PMT pulse
        }
        for(i=SCTBMX-1;i>0;i--)
            if(tslice[i]!=0. && tslice[i-1]!=0. && tslice[i-2]!=0.)break;//find high limit
        imax=i+1;
        imin=0;
//
        charge=0.;
        _ntr1=0;
        _ntr2=0;
        _ntr3=0;
        _nftdc=0;
        _nstdc=0;
        upd1=0; //up/down flag for discr.1
        tmd1d=-9999.;// down-time of discr-1 branch-1(z>=1)
        td1b1=-9999.;
        td1b2=-9999.;//discr1_brabch2 (fast TDC) up-time
        td1b2d=-9999.;// ......................down-time
        td1b3=-9999.;
        upd11=0;// up/down flag for z>=1 branch of discr.1
        upd12=0;// ............ fast TDC .................
        upd13=0;// ............ slow TDC .................
        upd2=0;//up/down flag for discr.2
        upd21=0;// up/down flag for z>1 branch of discr.2
        td2b1=-9999.;
        upd3=0;//up/down flag for discr.3
        upd31=0;// up/down flag for z>2 branch of discr.3 (dinode)
        tmd3d=-9999.;
        td3b1=-9999.;// down-time of discr-3 branch-1(z>2)
//
        amx=0.;
        tm=0.;
        amp=0.;
        tmp=0.;
        for(i=0;i<imax;i++){  //  <--- time bin loop for time measurements ---
          tm+=fladcb;
          am=tslice[i];
          if(am>amx)amx=am;// tempor
//          am=am+8.*rnormx();//tempor to test noncorr. noise 8mv(rms)
          charge+=am;
//--------------------------          
// discr-1(anode,fast comparator for TDC/z>=1 ) up/down setting with LOW thr.:
          if(am>=daqt0){
            if(upd1 ==0){
              tmd1u=tmp+fladcb*(daqt0-amp)/(am-amp);// up time of discr.1
              tmark=tmd1u+daqp10*rnormx();// add intrinsic t-dispersion
              upd1=1;
            }
          }
          else{
            if(upd1!=0 && (tm-tmd1u)>daqp11){ // down time (+min.duration check)
              upd1=0;
              tmd1d=tm;
            }
          }
//----------
// discr-3( dinode comp.for "z>2") up/down setting :
          if(am>=daqt2){
            if(upd3 ==0){
              tmd3u=tm;// up time of discr.3 ("Z>2")
              upd3=1;
            }
          }
          else{
            if(upd3!=0 && (tm-tmd3u)>daqp11){ // down time (+min.duration check)
              upd3=0;
              tmd3d=tm;
            }
          }
          amp=am;// store ampl to use as "previous" one in next i-loop
          tmp=tm;// .......
//--------------------------
// try set all branches of discr-1  when it is up: 
          if(upd1>0){
//
// branch-1 "z>=1(FT)" logic (with HIGH thresh.!!!) :
            if(upd11==0 && am>=daqt1){
              if((tm-td1b1)>daqp7){//dead time check for z>=1 signal
                upd11=1;  // set flag for branch z>=1
                if(_ntr1<SCTHMX1){
                  _ttr1[_ntr1]=tm;//don't need accurate up-time for trigger
                  _ntr1+=1;
                }
                else{
                  cerr<<"AMSTOFTovt::build-warning: trig1 hits ovfl"<<'\n';
                  upd11=2;//blockade on overflow
                }
              } 
            }
//        
// branch-2 "fast TDC logic" :        
            if(upd12==0){
              if((tm-td1b2d)>daqp3){ //dead-time(=f_TDC dbl.resol ?)-check, f-TDC channel
                upd12=1;  // set flag for branch f-TDC
                td1b2=tmark;//store up-time
              }
            } 
//        
// branch-3 "slow TDC logic" :        
            if(upd13==0){
              if((tmd1u-td1b3)>daqp4){ // "buzy" check for f-TDC channel
                upd13=1;  // set flag for branch s-TDC,if ready 
                if(_nstdc<SCTHMX3){
                  td1b3=tm;
                  _tstdc[_nstdc]=tmark;
                  _nstdc+=1;
                }
                else{
                  cerr<<"AMSTOFTovt::build-warning: s-TDC hits ovfl"<<'\n';
                  upd13=2;//blockade on overflow
                }
              } 
            }
          }//<-- end of discr-1 up-check
//---------------------------
// try reset all branches:
// "z>=1(FT)"
          if(upd11==1){// "z>=1 logic" reset
            if((tm-_ttr1[_ntr1-1])>daqp0){//min duration (pulse width) check
              upd11=0;                    // for self-clear
              td1b1=tm;
            }
          }
// f-tdc 
          if(upd12==1){ 
            if(upd1 ==0){//"f-TDC" clear when discr.1 down (tempor)
              upd12=0;
              td1b2d=tm;
              if(_nftdc<SCTHMX2){
                _tftdc[_nftdc]=td1b2;//write up-time
                _tftdcd[_nftdc]=td1b2d;//write down-time
                _nftdc+=1;
              }
              else{
                cerr<<"AMSTOFTovt::build-warning: f-TDC hits ovfl"<<'\n';
                upd12=2;//blockade on overflow
              }
            }
          }
// s-tdc
          if(upd13==1){ // "slow TDC" clear "buzy"
            if((tm-td1b3)>daqp4)upd13=0;
          }
//---------------------------
//  try set/reset branch "Z>2" of discr-3 :
//  set :
          if(upd3>0){
            if(upd31==0){
              if((tm-td3b1)>daqp9){//dead time check for z>2 signal
                upd31=1;  // set flag for branch z>2
                if(_ntr3<SCTHMX1){
                  _ttr3[_ntr3]=tmd3u;//up-time
                  _ntr3+=1;
                }
                else{
                  cerr<<"AMSTOFTovt::build-warning: trig3 hits ovfl"<<'\n';
                  upd31=2;//blockade on overflow
                }
              } 
            }
          }
// reset :
          if(upd31==1){// "z>2 logic" reset
            if((tm-_ttr3[_ntr3-1])>daqp2){//pulse width check
              upd31=0;                    // for self-clear
              td3b1=tm;
            }
          }
//---------------------------        
        } //      --- end of time bin loop for time measurements --->
//         
        charge=charge*fladcb/50.; // get total charge (pC)
        saturf=AMSTOFTovt::pmsatur(charge);//true charge reduction fact. due to satur.
//
//**************************************************************************
//
        mxcon=integer(imax*(fladcb/shapb))+1;// flash-adc useful t-range in shaper scale
        mxshc=mxcon+nshbn;// max. useful shaper channels
//
// ====> convert flash-ADC array "tslice" to shaper pulse array "tshap2":
//                 
// a) rebinning of "tslice[]" into shaper-binned "tshap1[]"  
        for(j=0;j<mxcon;j++)tshap1[j]=0.; // clear shaper input array
        j=0;                 
        for(i=0;i<imax;i++){
          am=tslice[i];//mV in fladcb-bin
          bo1=i*fladcb;
          bo2=bo1+fladcb;
          bn1=j*shapb;
          bn2=bn1+shapb;
          if(bo1>=bn1 && bo2<bn2)tshap1[j]+=am;
          else if(bo1<bn2 && bo2>=bn2){
            w=(bn2-bo1)/fladcb;
            tshap1[j]+=w*am;
            tshap1[j+1]+=(1.-w)*am;
            j+=1;
          }
        }
// b) tshap1[] --- shaping(integration) ---> tshap2[] conversion :        
        for(i=0;i<(SCSBMX+1);i++)tshap2[i]=0.;
        for(i=0;i<mxcon;i++){
          am=tshap1[i];
          if(am>0.){
            am=am*cconv;// mV->pC (charge per bin(=shapb)) 
            for(j=0;j<nshbn;j++){
              if((i+j)<mxshc)
                             tshap2[i+j]+=am*shplsh[j];
              else
                             tshap2[mxshc]+=am*shplsh[j];
            }
          }
        }
//----------
        if(TOFMCFFKEY.mcprtf[1] != 0)
             AMSTOFTovt::displ_as(idd,2,tshap2);//print Shaper pulse (no saturation)
//----------
//
// ====> Time_over_threshold measurement for ANODE :
//
        daqt3=daqt3/saturf;// to avoid multiplying am*saturf in the loop
        _nadca=0;
        updsh=0;
        tshd=-9999.;
        tm=0;
        amp=0.;
        tmp=0.;
        amxq=0.;
        for(i=0;i<mxshc;i++){  //  <--- time bin loop ---
          am=tshap2[i];
          if(am>amxq)amxq=am;
          tm+=shapb;
          if(am>=daqt3){
            if(updsh==0){
              tmark=tmp+shapb*(daqt3-amp)/(am-amp);//interpolation to compensate coarse binning
              if((tm-tshd)>daqp5){ //dead time check
                updsh=1;
                tshup=tmark;
              }
            }
          }
          else{
            if(updsh==1){
              updsh=0;
              tmark=tmp+shapb*(daqt3-amp)/(am-amp);
              tshd=tmark;
              if(_nadca<SCTHMX4){
                _tadca[_nadca]=tshup;// up-time
                _tadcad[_nadca]=tshd;  // down-time
                _nadca+=1;
              }
              else{
                cerr<<"AMSTOFTovt::build-warning: a-ADC hits ovfl"<<'\n';
                updsh=2;//blockade on overflow
              }
            } 
          }
          amp=am;// store as "previous" value for next loop
          tmp=tm;
        }                    // --- end of time bin loop --->
//
      if(TOFMCFFKEY.mcprtf[2]!=0){// hist.for Qa<->TovT
        tovt=0;
        if(_nadca>0)tovt=_tadcad[0]-_tadca[0];// TovT for first hit
        if(charge>0)HF2(1070,geant(tovt),log(charge),1.);
        if(idd==1081)HF1(1073,amx,1.);// ampl.spectrum for ref.bar
        HF1(1074,amxq,1.);
        if(idd==1081)HF1(1075,geant(tovt),1.);
      }
// 
//                      
// ====> Time_over_threshold measurement for DINODE :
//
        daqt4=daqt4/d2a;// to avoid multiplying am*d2a in the loop
        _nadcd=0;
        updsh=0;
        tshd=-9999.;
        tm=0;
        amp=0.;
        tmp=0.;
        for(i=0;i<mxshc;i++){  //  <--- time bin loop ---
          am=tshap2[i];
          tm+=shapb;
          if(am>=daqt4){
            if(updsh==0){
              tmark=tmp+shapb*(daqt4-amp)/(am-amp);
              if((tm-tshd)>daqp6){ //dead time check
                updsh=1;
                tshup=tmark;
              }
            }
          }
          else{
            if(updsh==1){
              updsh=0;
              tmark=tmp+shapb*(daqt4-amp)/(am-amp);
              tshd=tmark;
              if(_nadcd<SCTHMX4){
                _tadcd[_nadcd]=tshup;
                _tadcdd[_nadcd]=tshd;
                _nadcd+=1;
              }
              else{
                cerr<<"AMSTOFTovt::build-warning: d-ADC hits ovfl"<<'\n';
                updsh=2;//blockade on overflow
              }
            } 
          }
          amp=am;
          tmp=tm;
        }                    // --- end of time bin loop --->
//
//------------------------------
// now create/fill AMSTOFTovt object (id=idd) with above digi-data:
      if(_ntr1>0 && _nftdc>0){ 
        _sta=0;    
        stat=0;
        stat=AMSEvent::gethead()->addnext(AMSID("AMSTOFTovt",ilay), new
             AMSTOFTovt(idd,_sta,charge,edepb,
             _ntr1,_ttr1,_ntr3,_ttr3,
             _nftdc,_tftdc,_tftdcd,_nstdc,_tstdc,
             _nadca,_tadca,_tadcad,_nadcd,_tadcd,_tadcdd));
      }
        return;
//
//
}
//---------------------------------------------------------------------
//
// function to get absolute time of the FIRST trigger(coincidence) "z>=1";
// trcode - trigger code (pattern) =-1/n, n>=0 -> missing plane #, 
// =-1 -> no trigger.
//
number AMSTOFTovt::tr1time(int &trcode, integer toftrp[]){
  integer i1,i2,isds(0),first(0);
  integer i,j,ilay,ibar,ntr,idd,id,idN,stat;
  integer npdpl[SCLRS]={0,0,0,0};
  integer sbt,lsbit(1);
  number ttr[SCTHMX1];
  geant t1,t2;
  AMSBitstr trbs[2];
  AMSBitstr trbc;
  AMSBitstr trbl[SCLRS];
  AMSTOFTovt *ptr;
  AMSTOFTovt *ptrN;
//
  geant trigb=TOFDBc::trigtb();
  geant pwid=TOFDBc::daqpwd(0);
  for(i=0;i<SCLRS;i++)toftrp[i]=0;
//
  for(ilay=0;ilay<SCLRS;ilay++){// <--- layers loop (Tovt containers) ---
    ptr=(AMSTOFTovt*)AMSEvent::gethead()->
                               getheadC("AMSTOFTovt",ilay,0);
    isds=0;
    trbs[0].bitclr(1,0);
    trbs[1].bitclr(1,0);
    while(ptr){// <--- Tovt-hits loop in layer ---
      idd=ptr->getid();
      id=idd/10;// short id=LBB
      ibar=id%100-1;
      stat=ptr->getstat();// ok status
      ntr=ptr->gettr1(ttr);// get number and times of trig1 ("z>=1")
//      cerr<<"id= "<<idd<<" Ntrpulses= "<<ntr<<'\n'; // tempor
      for(j=0;j<ntr;j++){// <--- trig-hits loop ---
        t1=geant(ttr[j]);
        t2=t1+pwid;
        i1=integer(t1/trigb);
        i2=integer(t2/trigb);
        if(i1>=SCBITM)i1=SCBITM-1;
        if(i2>=SCBITM)i2=SCBITM-1;
        if(isds<2)
          if(stat==0)trbs[isds].bitset(i1,i2);//set bits according to pulse width
        else
            cerr<<"AMSTOFTovt::tr1time: error: >2sides !!!"<<'\n';
      }// --- end of trig-hits loop --->
      isds+=1; 
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
//                                  
        trbl[ilay]=trbl[ilay] | trbc; // summing OR within the layer
        trbc.testbit(i1,i2);// bit-pattern of the fired counters in layer:
        if(i2>=i1){
          npdpl[ilay]+=1;
          sbt=lsbit<<ibar;
          toftrp[ilay]|=sbt;
        }
        isds=0; // reset c.sides variables ...
        trbs[0].bitclr(1,0);
        trbs[1].bitclr(1,0);
      }
      ptr=ptr->next();// take next Tovt-hit
// 
    }//         --- end of Tovt-hits loop in layer --->
//
  } //                               --- end of layer loop --->
//
  trcode=-1;
  t1=0.;
  AMSBitstr coinc1234,rr1,rr2;
  rr1=trbl[0]&trbl[1];
  rr2=trbl[2]&trbl[3];
  coinc1234=rr1&rr2;
  coinc1234.testbit(i1,i2);
  if(i2>=i1){// have 4-fold coincidence
    trcode=0;
    t1=i1*trigb;
    return t1;
  }
  else{   // check 3-fold coincidence:
    if(TOFMCFFKEY.trlogic[1] == 1)return t1;// if ALL4 was requested
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
    if(imin!=9999){
      t1=imin*trigb;
      toftrp[trcode-1]=-toftrp[trcode-1];// mark missing layer
    }
    return t1;
  }
}
//---------------------------------------------------------------------
//
// function to get absolute time of the FIRST trigger(coincidence) "z>2";
// trcode - trigger code (pattern) =-1/n, n>=0 -> missing plane #, 
// =-1 -> no trigger.
//
number AMSTOFTovt::tr3time(int &trcode, integer toftrp[]){
  integer i1,i2,isds(0),first(0);
  integer i,j,ilay,ibar,ntr,idd,id,idN,stat;
  integer npdpl[SCLRS]={0,0,0,0};
  integer sbt,lsbit(1);
  number ttr[SCTHMX1];
  geant t1,t2;
  AMSBitstr trbs[2];
  AMSBitstr trbc;
  AMSBitstr trbl[SCLRS];
  AMSTOFTovt *ptr;
  AMSTOFTovt *ptrN;
//
  geant trigb=TOFDBc::trigtb();
  geant pwid=TOFDBc::daqpwd(0);
  for(i=0;i<SCLRS;i++)toftrp[i]=0;
//
  for(ilay=0;ilay<SCLRS;ilay++){// <--- layers loop (Tovt containers) ---
    ptr=(AMSTOFTovt*)AMSEvent::gethead()->
                               getheadC("AMSTOFTovt",ilay,0);
    isds=0;
    trbs[0].bitclr(1,0);
    trbs[1].bitclr(1,0);
    while(ptr){// <--- Tovt-hits loop in layer ---
      idd=ptr->getid();
      id=idd/10;// short id=LBB
      ibar=id%100-1;
      stat=ptr->getstat();// ok status
      ntr=ptr->gettr3(ttr);// get number and times of trig3 ("z>2")
      for(j=0;j<ntr;j++){// <--- trig-hits loop ---
        t1=geant(ttr[j]);
        t2=t1+pwid;
        i1=integer(t1/trigb);
        i2=integer(t2/trigb);
        if(i1>=SCBITM)i1=SCBITM-1;
        if(i2>=SCBITM)i2=SCBITM-1;
        if(isds<2)
          if(stat==0)trbs[isds].bitset(i1,i2);//set bits according to pulse width
        else
            cerr<<"AMSTOFTovt::tr3time: error: >2sides !!!"<<'\n';
      }// --- end of trig-hits loop --->
      isds+=1; 
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
//                                  
        trbl[ilay]=trbl[ilay] | trbc; // summing OR within the layer
        trbc.testbit(i1,i2);// bit-pattern of the fired counters in layer:
        if(i2>=i1){
          npdpl[ilay]+=1;
          sbt=lsbit<<ibar;
          toftrp[ilay]|=sbt;
        }
        isds=0; // reset c.sides variables ...
        trbs[0].bitclr(1,0);
        trbs[1].bitclr(1,0);
      }
      ptr=ptr->next();// take next Tovt-hit
// 
    }//         --- end of Tovt-hits loop in layer --->
//
  } //                               --- end of layer loop --->
//
  trcode=-1;
  t1=0.;
  AMSBitstr coinc1234,rr1,rr2;
  rr1=trbl[0]&trbl[1];
  rr2=trbl[2]&trbl[3];
  coinc1234=rr1&rr2;
  coinc1234.testbit(i1,i2);
  if(i2>=i1){// have 4-fold coincidence
    trcode=0;
    t1=i1*trigb;
    return t1;
  }
  else{   // check 3-fold coincidence:
    if(TOFMCFFKEY.trlogic[1] == 1)return t1;// if ALL4 was requested
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
    if(imin!=9999){
      t1=imin*trigb;
      toftrp[trcode-1]=-toftrp[trcode-1];// mark missing layer
    }
    return t1;
  }
}
//=====================================================================
//
//  function to set bits in AMSBitstr objects (from bit il till bit ih):
//   (bits are counting starting from 0 (left edge of bit-array))
//   (if il>ih, function sets all available bits)
//
void AMSBitstr::bitset(const int il, const int ih){
//
  static unsigned short int allzer(0x0000);
  static unsigned short int allone(0xFFFF);
  static unsigned short int setone[16]={
       0x8000,0x4000,0x2000,0x1000,0x0800,0x0400,0x0200,0x0100,
       0x0080,0x0040,0x0020,0x0010,0x0008,0x0004,0x0002,0x0001};
  int i,iw1,iw2,i1,i2,nw;
#ifdef __AMSDEBUG__
  assert(il>=0 && ih>=0);
  assert(il<SCBITM && ih<SCBITM);
#endif
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
void AMSBitstr::bitclr(const int il, const int ih){
//
  static unsigned short int allzer(0x0000);
  static unsigned short int setzer[16]={
        0x7FFF,0xBFFF,0xDFFF,0xEFFF,0xF7FF,0xFBFF,0xFDFF,0xFEFF,
        0xFF7F,0xFFBF,0xFFDF,0xFFEF,0xFFF7,0xFFFB,0xFFFD,0xFFFE};
  int i,iw1,iw2,i1,i2,nw;
#ifdef __AMSDEBUG__
  assert(il>=0 && ih>=0);
  assert(il<SCBITM && ih<SCBITM);
#endif
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
  int len=lenb;
  unsigned short int arr[SCWORM];
  if(lenb != lenc){
    cout<<"AMSBitstr:AND-warning:len1 != len2 "<<lenb<<" "<<lenc<<'\n';
    if(lenc<lenb && lenb<SCWORM)len=lenc;
    else if(lenb<lenc && lenc<SCWORM)len=lenb;
    else exit(1);
  }
  for(int i=0;i<len;i++)arr[i]= b.bitstr[i] & c.bitstr[i];
  return AMSBitstr(len,arr);
} 
//-----------------------------------------------------------------------
AMSBitstr operator |(const AMSBitstr &b, const AMSBitstr &c){
  int lenb=b.bslen;
  int lenc=c.bslen;
  int len=lenb;
  unsigned short int arr[SCWORM];
  if(lenb != lenc){
    cout<<"AMSBitstr:OR-warning:len1 != len2 "<<lenb<<" "<<lenc<<'\n';
    if(lenc<lenb && lenb<SCWORM)len=lenc;
    else if(lenb<lenc && lenc<SCWORM)len=lenb;
    else exit(1);
  }
  for(int i=0;i<len;i++)arr[i] = b.bitstr[i] | c.bitstr[i];
  return AMSBitstr(len,arr);
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
//               
void AMSTOFRawEvent::mc_build(int &status)
{
  integer i,j,jj,ilay,last,ibar,id,idd,iddN,isd,_sta,stat(0);
  integer nftdc,nstdc,nadca,nadcd;
  int16u _nftdc,_nstdc,_nadca,_nadcd,itt;
  number  tftdc[SCTHMX2],tftdcd[SCTHMX2],tstdc[SCTHMX3];
  number tadca[SCTHMX4],tadcad[SCTHMX4],tadcd[SCTHMX4],tadcdd[SCTHMX4];
  int16u  ftdc[SCTHMX2*2],stdc[SCTHMX3*4],adca[SCTHMX4*2],adcd[SCTHMX4*2];
  number t,t1,t2,t3,t4,tprev,ttrig1,ttrig3,dt,tlev1,tl1d,ftrig;
  geant charge,edep,strr[2],str;
  int trcode,trcode3;
  integer trflag(0),trpatt3[SCLRS];
  integer trpatt[SCLRS]={0,0,0,0};
  integer it,it1,it2,it3,it4,it0;
  int16u phbit,maxv;
  static geant ftpw=TOFDBc::daqpwd(12);// FT_coincidence-pulse width
  AMSTOFTovt *ptr;
  AMSTOFTovt *ptrN;
  status=1;// bad
  phbit=SCPHBP;
  maxv=phbit-1;//max possible TDC value (16383)
//
  trflag=0;
  ttrig1=AMSTOFTovt::tr1time(trcode,trpatt);//get abs.trig1(FT"z>=1") signal time/patt
  if(trcode<0){
    AMSTOFRawEvent::settrfl(trflag);// set trig_flag to 0 ("no trigger")
    AMSTOFRawEvent::setpatt(trpatt);//set 0 trigger pattern 
    return;
  } 
  status=0;
  trflag=1;// ok: h/w trigger(z>=1) present -> do digitization:
//
  ttrig3=AMSTOFTovt::tr3time(trcode3,trpatt3);//get abs.trig3("z>2") signal time/patt
  if(trcode3>=0){
    trflag=3;
    if(ttrig3<ttrig1){ // tempor: priority is not clear now !
      ttrig1=ttrig3;
      for(i=0;i<SCLRS;i++)trpatt[i]=trpatt3[i];
    }
  }
// 
  AMSTOFRawEvent::settrfl(trflag);// set final trigger flag
  AMSTOFRawEvent::setpatt(trpatt);// set trigger pattern
  ftrig=ttrig1+TOFDBc::ftdelf();// FTrigger abs time
  AMSTOFRawEvent::settrtime(ftrig);// set final FTrigger time
//
  tlev1=ftrig+TOFDBc::accdel();// Lev-1 accept-signal abs.time
  tl1d=tlev1-TOFDBc::fatdcd();// just to simulate A-tdc-pulse delay wrt ftdc-pulse
//
  for(ilay=0;ilay<SCLRS;ilay++){// <--- layers loop (Tovt containers) ---
    ptr=(AMSTOFTovt*)AMSEvent::gethead()->
                               getheadC("AMSTOFTovt",ilay,0);
    while(ptr){// <--- Tovt-hits loop in layer ---
      idd=ptr->getid();// LBBS
      id=idd/10;// short id=LBB
      ibar=id%100-1;
      scbrcal[ilay][ibar].gtstrat(strr);// get str-ratios for two sides
      isd=idd%10-1;
      str=strr[isd];// str-ratio for given TovT-hit
      _sta=ptr->getstat();
      charge=ptr->getcharg();
      edep=ptr->getedep();
//
      nftdc=ptr->getftdc(tftdc,tftdcd);// get number and times of fast-TDC hits
      _nftdc=0;
      for(j=0;j<nftdc;j++){//        <--- ftdc-hits loop ---
        jj=nftdc-j-1;// LIFO readout mode (last stored - first read)
        t1=tftdc[jj];
        t2=tftdcd[jj];
        dt=tlev1-t2;// follow LIFO mode of readout : down-edge - first hit
        it=integer(dt/TOFDBc::tdcbin(0)); // conv. to fast-TDC (history) t-binning
        if(it>maxv){
          cout<<"TOFRawEvent_mc: warning : Hist-TDC down-time overflow !!!"<<'\n';
          it=maxv;
        }
        itt=int16u(it);
        if(!TOFDBc::pbonup())itt=itt|phbit;//add phase bit if necessary
        ftdc[_nftdc]=itt;
        _nftdc+=1;
        dt=tlev1-t1;// follow LIFO mode of readout : leading(up) edge - second
        it=integer(dt/TOFDBc::tdcbin(0)); // conv. to fast-TDC (history) t-binning
        if(it>maxv){
          cout<<"TOFRawEvent_mc: warning : Hist-TDC up-time overflow !!!"<<'\n';
          it=maxv;
        }
        itt=int16u(it);
        if(TOFDBc::pbonup())itt=itt|phbit;//add phase bit if necessary
        ftdc[_nftdc]=itt;
        _nftdc+=1;
      }//--- end of ftdc-hits loop --->
//------------
      nstdc=ptr->getstdc(tstdc);// get number and times of slow-TDC hits
      _nstdc=0;
      for(j=0;j<nstdc;j++){//       <--- stdc-hits loop ---
        jj=nstdc-j-1;// LIFO readout mode (last stored - first read)
        t1=tstdc[jj]+rnormx()*TOFDBc::strjit1();//"start"-signal time + jitter
        t2=ftrig+rnormx()*TOFDBc::strjit2();//"stop"-signal = ftrig + jitter
        t3=t2+ftpw;//"dummy"-signal time (fixed delay = FT_coinc-pulse width)
        t4=t2+(t2-t1)*str+rnormx()*TOFDBc::strflu();//"end-mark"-signal + fluct 
        dt=ftrig-t1;// total FT-delay wrt t1
        if(dt<=TOFDBc::ftdelm()){ // check max. delay of "fast-trigger" signal
          it0=integer((tlev1-TOFDBc::fstdcd())/TOFDBc::tdcbin(1));//sTDC is delayed wrt fTDC
//follow LIFO mode of readout: stretcher "end-mark" edge - first hit; "start" - last:
//  prepare 4-th component("end-mark") of "4-edge" TOF-hit:
          it4=integer(t4/TOFDBc::tdcbin(1));
          it=it0-it4;// time wrt lev-1 signal
          if(it>maxv){
            cout<<"TOFRawEvent_mc: warning : 4-th edge TDC overflow !!!"<<'\n';
            it=maxv;
          }
          itt=int16u(it);
          if(!TOFDBc::pbonup())itt=itt|phbit;//add phase bit on down-edge,if necessary
          stdc[_nstdc]=itt;
          _nstdc+=1;
//  prepare 3-d component("dummy"):
          it3=integer(t3/TOFDBc::tdcbin(1));
          it=it0-it3;// time wrt lev-1 signal
          if(it>maxv){
            cout<<"TOFRawEvent_mc: warning : 3-rd edge TDC overflow !!!"<<'\n';
            it=maxv;
          }
          itt=int16u(it);
          if(TOFDBc::pbonup())itt=itt|phbit;//add phase bit on up-edge,if necessary
          stdc[_nstdc]=itt;
          _nstdc+=1;
//  prepare 2-nd component("stop"):
          it2=integer(t2/TOFDBc::tdcbin(1));
          it=it0-it2;// time wrt lev-1 signal
          if(it>maxv){
            cout<<"TOFRawEvent_mc: warning : 2-nd edge TDC overflow !!!"<<'\n';
            it=maxv;
          }
          itt=int16u(it);
          if(!TOFDBc::pbonup())itt=itt|phbit;//add phase bit on down-edge,if necessary
          stdc[_nstdc]=itt;
          _nstdc+=1;
//  prepare 1-st component("start") of "4-edge" TOF-hit:
          it1=integer(t1/TOFDBc::tdcbin(1));
          it=it0-it1;// time wrt lev-1 signal
          if(it>maxv){
            cout<<"TOFRawEvent_mc: warning : 1-st edge TDC overflow !!!"<<'\n';
            it=maxv;
          }
          itt=int16u(it);
          if(TOFDBc::pbonup())itt=itt|phbit;//add phase bit on up-edge,if necessary
          stdc[_nstdc]=itt;
          _nstdc+=1;
        }
      }//--- end of stdc-hits loop --->
//----------------
      _nadca=0;
      nadca=ptr->getadca(tadca,tadcad);// get number and up/down times of anode-ADC hits
      for(j=0;j<nadca;j++){// <--- adca-hits loop ---
        jj=nadca-j-1;// LIFO readout mode (last stored - first read)
        t1=tadca[jj];
        t2=tadcad[jj];
        dt=tl1d-t2;// follow LIFO mode of readout : down-edge - first hit
        it=integer(dt/TOFDBc::tdcbin(2));
        if(it>maxv){
          cout<<"TOFRawEvent_mc: warning :  anode down-time out of range !!!"<<'\n';
          it=maxv;
        }
        itt=int16u(it);
        if(!TOFDBc::pbonup())itt=itt|phbit;//add phase bit if necessary
        adca[_nadca]=itt;
        _nadca+=1;
        dt=tl1d-t1;// follow LIFO mode of readout : up-edge - second hit
        it=integer(dt/TOFDBc::tdcbin(2));
        if(it>maxv){
          cout<<"TOFRawEvent_mc: warning :  anode up-time out of range !!!"<<'\n';
          it=maxv;
        }
        itt=int16u(it);
        if(TOFDBc::pbonup())itt=itt|phbit;//add phase bit if necessary
        adca[_nadca]=itt;
        _nadca+=1;
      }//--- end of adca-hits loop --->
//--------------------
      _nadcd=0;
      nadcd=ptr->getadcd(tadcd,tadcdd);// get number and up/down times of dinode-ADC hits
      for(j=0;j<nadcd;j++){// <--- adcd-hits loop ---
        jj=nadcd-j-1;// LIFO readout mode (last stored - first read)
        t1=tadcd[jj];
        t2=tadcdd[jj];
        dt=tl1d-t2;// follow LIFO mode of readout : down-edge - first hit
        it=integer(dt/TOFDBc::tdcbin(3));
        if(it>maxv){
          cout<<"TOFRawEvent_mc: warning :  dinode down-time out of range !!!"<<'\n';
          it=maxv;
        }
        itt=int16u(it);
        if(!TOFDBc::pbonup())itt=itt|phbit;//add phase bit if necessary
        adcd[_nadcd]=itt;
        _nadcd+=1;
        dt=tl1d-t1;// follow LIFO mode of readout : up-edge - second hit
        it=integer(dt/TOFDBc::tdcbin(3));
        if(it>maxv){
          cout<<"TOFRawEvent_mc: warning :  dinode up-time out of range !!!"<<'\n';
          it=maxv;
        }
        itt=int16u(it);
        if(TOFDBc::pbonup())itt=itt|phbit;//add phase bit if necessary
        adcd[_nadcd]=itt;
        _nadcd+=1;
      }//--- end of adcd-hits loop --->
//
//     ---> create/fill RawEvent-object :
      stat=0;
      stat=AMSEvent::gethead()->addnext(AMSID("AMSTOFRawEvent",0), new
           AMSTOFRawEvent(idd,_sta,charge,edep,_nftdc,ftdc,_nstdc,stdc,
                               _nadca,adca,_nadcd,adcd));
//
      ptr=ptr->next();// take next Tovt-hit
// 
    }//         --- end of Tovt-hits loop in layer --->
//
  } //                               --- end of layer loop --->
}
//================================================================
