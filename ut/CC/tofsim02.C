// Author Choumilov.E. 10.07.96.
#include <tofdbc02.h>
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
#include <tofsim02.h>
#include <mccluster.h>
#include <trigger3.h>
//
TOF2Scan TOF2Scan::scmcscan[TOF2GC::SCBLMX];
//
integer TOF2RawEvent::trpatt[TOF2GC::SCLRS]={0,0,0,0};
integer TOF2RawEvent::trflag=0;
number TOF2RawEvent::trtime=0.;
number TOF2Tovt::itovts[TOF2GC::SCMCIEVM];
number TOF2Tovt::icharg[TOF2GC::SCMCIEVM];
integer TOF2Tovt::ievent;
//============================================================
AMSDistr::AMSDistr(int _nbin, geant _bnl, geant _bnw, geant arr[]){// constructor
  nbin=_nbin;
  bnl=_bnl;
  bnw=_bnw;
  geant tot(0.);
  for(int i=0;i<nbin;i++)tot+=arr[i];// calc. integral
  distr[0]=arr[0]/tot;             // prepare cumul. spectrum
  for(int j=1;j<nbin;j++)distr[j]=distr[j-1]+arr[j]/tot;
}
//---------------------------------------------------
geant AMSDistr::getrand(const geant &rnd){
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
//===========================================================
TOFTpoints::TOFTpoints(int nbin, geant bnl, geant bnw, geant arr[]){//constructor
  int i,it,nb,nbf,fst,nbl;
  geant cint,low,hig,tot(0.);
  geant bl,bh,cdis[TOFGC::AMSDISL];
//
  fst=0;
  for(i=0;i<nbin;i++){
    if(fst==0 && arr[i]>0.){
      fst=1;
      nbf=i;// 1st nonzero bin
    }
    if(arr[i]>0.)nbl=i;// last nonzero bin
    tot+=arr[i];// calc. integral
  }
  cdis[0]=arr[0]/tot;             // prepare cumul. spectrum
  for(i=1;i<nbin;i++)cdis[i]=cdis[i-1]+arr[i]/tot;
  bini=1./geant(TOF2GC::TOFPNMX);
//
  times[0]=bnl+bnw*nbf;
  times[TOF2GC::TOFPNMX]=bnl+bnw*(nbl+1);
  nb=nbf;
  for(it=1;it<TOF2GC::TOFPNMX;it++){// <--- loop over integral bins
    cint=it*bini;
    while(nb<=nbl){// <--- loop over cumul.distr. bins
      bl=bnl+nb*bnw;
      bh=bl+bnw;
      if(nb==nbf)low=0.;
      else low=cdis[nb-1];
      hig=cdis[nb];
      if(cint>=low && cint<hig){
        times[it]=bl+bnw*(cint-low)/(hig-low);
	break;
      }
      else nb+=1;
    }
  }
}
//===========================================================
//  <--- for inp. X define position in scan array (i1/i2) and correction ratio R.
//
void TOF2Scan::getxbin(const geant x, integer &i1, integer &i2, geant &r)
{
  integer i=AMSbins(scanp,x,nscpnts);
  if(i==0){
    i1=0;
    i2=1;
    r=(x-scanp[i1])/(scanp[i2]-scanp[i1]);//r<0 -> x below 1st sc.point
  }
  else if(i<0){
    if(i==-nscpnts){// r>1 -> x above last sc.point
      i1=nscpnts-2;
      i2=nscpnts-1;
      r=(x-scanp[i1])/(scanp[i2]-scanp[i1]);
      }
    else{// <- normal case when x is inside of sc.points array
      i1=-i-1;
      i2=-i;
      r=(x-scanp[i1])/(scanp[i2]-scanp[i1]);
    }
  }
  else{ // exact matching inside sc.points array(i1=i2, r=0.)
    i1=i-1;
    i2=i1;
    r=0.;
  }
}
//------------------------------------------------------------------
geant TOF2Scan::gettm1(geant r, int i1, int i2){
  geant rnd,t1,t2,dummy(-1);
//
  rnd=RNDM(dummy);
  t1=tdist1[i1].gettime(rnd);
  t2=tdist1[i2].gettime(rnd);
  return t1+(t2-t1)*r;

};    
geant TOF2Scan::gettm2(geant r, int i1, int i2){
  geant rnd,t1,t2,dummy(-1);
//
  rnd=RNDM(dummy);
  t1=tdist2[i1].gettime(rnd);
  t2=tdist2[i2].gettime(rnd);
  return t1+(t2-t1)*r;

};    
//------------------------------------------------------------------
// function to create TOF2Scan objects for all sc. bars :
void TOF2Scan::build(){
//
  int i,ic,nverst,ivers,dig;
  int brfnam[TOF2GC::SCBLMX];
  char fname[80];
  char name[80];
  char in[2]="0";
  char inum[11];
  char tdisfn[20]="tof2smap";
//
  strcpy(inum,"0123456789");
//
// ---> read version number of "tof2smap"-file from verslist-file:
//
  integer cfvn;
  cfvn=TFCAFFKEY.cfvers%1000;
  strcpy(name,"tof2cvlist");// basic name of file for cal-files list
  dig=cfvn/100;
  in[0]=inum[dig]; 
  strcat(name,in);
  dig=(cfvn%100)/10;
  in[0]=inum[dig]; 
  strcat(name,in);
  dig=cfvn%10;
  in[0]=inum[dig]; 
  strcat(name,in);
  strcat(name,".dat");
//
  if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"TOF2Scan::build: Open file  "<<fname<<'\n';
  ifstream vlfile(fname,ios::in); // open needed tdfmap-file for reading
  if(!vlfile){
    cerr <<"TOF2Scan::build(): missing verslist-file "<<fname<<endl;
    exit(1);
  }
  vlfile >> nverst;// total number of files in the list
  vlfile >> ivers;// first number in first line (vers.# for tdisfmap-file)
  vlfile.close();
  ivers=(ivers%1000);
  cout<<"TOF2Scan::build(): use MC timescan map-file version="<<ivers<<'\n';
//  
//                                  <-- first read t-distr. map-file
  strcpy(name,tdisfn);
  dig=ivers/100;
  in[0]=inum[dig];
  strcat(name,in);
  dig=(ivers%100)/10;
  in[0]=inum[dig];
  strcat(name,in);
  dig=ivers%10;
  in[0]=inum[dig];
  strcat(name,in);
  strcat(name,".dat");
  if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"Open file : "<<fname<<'\n';
  ifstream tcfile(fname,ios::in); // open needed tdisfmap-file for reading
  if(!tcfile){
    cerr <<"TOF2Scan::build(): missing tof2smap-file "<<fname<<endl;
    exit(1);
  }
  for(ic=0;ic<TOF2GC::SCBLMX;ic++) tcfile >> brfnam[ic];
  tcfile.close();
//-------------------
//                                  <-- now read t-distr. files
 int j,ila,ibr,brt,ibrm,isp,nsp,ibt,cnum,dnum,mult;
 integer nb;
 geant scp[TOF2GC::SCANPNT];
 geant nft,bl,bw;
 geant arr[TOFGC::AMSDISL];
 geant ef1[TOF2GC::SCANPNT],ef2[TOF2GC::SCANPNT];
 geant rg1[TOF2GC::SCANPNT],rg2[TOF2GC::SCANPNT];
 TOFTpoints td1[TOF2GC::SCANPNT];
 TOFTpoints td2[TOF2GC::SCANPNT];
 geant eff1,eff2;
//
  for(ila=0;ila<TOF2GC::SCLRS;ila++){   // <-------- loop over layers
  for(ibr=0;ibr<TOF2GC::SCMXBR;ibr++){  // <-------- loop over bar in layer
    brt=TOF2DBc::brtype(ila,ibr);
    if(brt==0)continue; // skip missing counters
    cnum=ila*TOF2GC::SCMXBR+ibr; // sequential counter numbering(0-55)
    dnum=brfnam[cnum];// 4-digits t-distr. file name (VLBB)
    mult=1000;
    strcpy(name,"tof2c");
    for(i=3;i>=0;i--){//create 4-letters file name
      j=dnum/mult;
      in[0]=inum[j];
      strcat(name,in);
      dnum=dnum%mult;
      mult=mult/10;
    }
    strcat(name,".tsf");
    if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
    if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
    strcat(fname,name);
    cout<<"Open file : "<<fname<<'\n';
    ifstream tcfile(fname,ios::in); // open needed t-calib. file for reading
    if(!tcfile){
      cerr <<"AMSTOFScan::build(): missing MC-t_scan file "<<fname<<endl;
      exit(1);
    }
// <-- fill errays scp,ef1,ef2,rg1,rg2 from file
//
    tcfile >> nsp;// read # of calibr. points
    if(nsp>TOF2GC::SCANPNT){
      cerr<<"Sitofinitjob: bad # of MC Y-scan point ! "<<nsp<<'\n';
      exit(1);
    } 
    for(isp=0;isp<nsp;isp++){ // sp. points loop to prepare arr. of t-distr
      tcfile >> scp[isp];
      tcfile >> nft;   // for PM-1
      tcfile >> nb;
      tcfile >> bl;
      tcfile >> bw;
      tcfile >> ef1[isp];
      tcfile >> rg1[isp];
      for(i=0;i<nb;i++){arr[i]=0.;}
      for(i=0;i<nb;i++){tcfile >> arr[i];}
      td1[isp]=TOFTpoints(nb,bl,bw,arr);
      tcfile >> nft;   // for PM-2
      tcfile >> nb;
      tcfile >> bl;
      tcfile >> bw;
      tcfile >> ef2[isp];
      tcfile >> rg2[isp];
      for(i=0;i<nb;i++){arr[i]=0.;}
      for(i=0;i<nb;i++){tcfile >> arr[i];}
      td2[isp]=TOFTpoints(nb,bl,bw,arr);
//
    } // <--- end of scan points loop -----
//
    scmcscan[cnum]=TOF2Scan(nsp,scp,ef1,ef2,rg1,rg2,td1,td2);// create bar MC-t-scan obj
    tcfile.close(); // close file
  } // --- end of bar loop --->
  } // --- end of layer loop --->
}
//===================================================================
//
void TOF2Tovt::build()
{
//      <--- prepare PM single electron pulse height distribution:
  static geant arr[19]={50.,20.,39.7,46.5,52.3,54.6,52.7,47.6,40.7,
                          32.5,23.9,16.7,11.,7.,4.5,3.,1.5,1.,0.5};
  static AMSDistr scpmsesp(19,0.,0.279,arr);// p/h spectrum ready
//                   ( scale in mV@50ohm to have 1.5mV as m.p. value!!!)
//
  integer i1,i2,id,idN,idd,ibar,ilay,ibtyp,cnum;
  static geant tslice1[TOF2GC::SCTBMX+1]; //  flash ADC array for side-1
  static geant tslice2[TOF2GC::SCTBMX+1]; //  flash ADC array for side-2
  static geant tslice[TOF2GC::SCTBMX+1]; //  flash ADC array
  geant bnw; 
  integer warr[TOFGC::AMSDISL]; 
  int i,i0,ii,j,jj,jm,k,stat(0),nelec,ierr(0),lbn,nbm;
  int status[2];
  integer nhitl[TOF2GC::SCLRS];
  geant edepl[TOF2GC::SCLRS],edepb;
  geant dummy(-1);
  geant time,qtime,x,y,z,am,am0,am2,sig,r,rnd,eff,rgn,de,tm,eps(0.002);
  geant vl,vh;
  geant nel0,nel,nelb,nesig,nphot;
  AMSPoint cloc(999.,999.,999.);
  AMSgvolume *p;
  AMSTOFMCCluster *ptr;
  AMSTOFMCCluster *ptrN;
//
  static integer first=0;
  static integer npshbn;
  static geant pmplsh[1000];  // PM s.e. pulse shape
  static geant fadcb,ifadcb,trts,tmax,zlmx,convr,seres,sesig,sesave,sessig,sesrat;
//
  if(first++==0){
    seres=TOF2DBc::seresp();
    sesig=seres*TOF2DBc::seresv();
    TOF2Tovt::inipsh(npshbn,pmplsh); // prepare PM s.e. pulse shape arr.
    HBOOK1(1099,"Single electron spectrum,mV",65,0.,13.,0.);
    am=0.;
    am2=0.;
    for(i=0;i<5000;i++){
//      rnd=RNDM(dummy);
//      am0=scpmsesp.getrand(rnd);//amplitude from single elect. spectrum
      am0=seres+sesig*rnormx();// tempor use simple goussian
      if(am0<0.)am0=0.;
      am+=am0;
      am2+=am0*am0;
      HF1(1099,am0,1.);
    }
    HPRINT(1099);
    am/=5000.;
    am2/=5000.;
    sesave=am;
    sessig=sqrt(am2-am*am);
    sesrat=sessig/sesave;
    cout<<"S.E. Specrtum Aver/Sigm="<<sesave<<" "<<sessig<<" ratio="<<sesrat<<endl;
    HDELET(1099);
//
    for(i=0;i<TOF2GC::SCTBMX+1;i++)tslice1[i]=0.;// clear flash ADC arrays
    for(i=0;i<TOF2GC::SCTBMX+1;i++)tslice2[i]=0.;
    for(i=0;i<TOF2GC::SCTBMX+1;i++)tslice[i]=0.;
    fadcb=TOF2DBc::fladctb();
    ifadcb=1./fadcb;
    tmax=TOF2GC::SCTBMX*fadcb-1.;// ns
    zlmx=TOF2DBc::plnstr(6)/2.+eps;
    convr=1000.*TOF2DBc::edep2ph();
  }
//
  ptr=(AMSTOFMCCluster*)AMSEvent::gethead()->
                                      getheadC("AMSTOFMCCluster",0,1);
//                                        ( sort by idsoft done)
//
  for(i=0;i<TOF2GC::SCLRS;i++){
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
//  cout<<"MChit id="<<id<<" edep="<<de<<" /x/y/z="<<x<<" "<<y<<" "<<z<<endl;
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
//  cout<<"local coo="<<x<<" "<<y<<" "<<z<<" G-time="<<time<<endl;
    if((time>=tmax) || (time<0.)){
//            cout<<"TOF: Bad G-hit, id="<<id<<" t/de="<<time<<" "<<de<<'\n';
      ptr=ptr->next(); // next hit
      continue;
    }
    qtime=time*ifadcb;
    cnum=ilay*TOF2GC::SCMXBR+ibar;// sequential counter number
    TOF2Scan::scmcscan[cnum].getxbin(y,i1,i2,r);//y-bin # (longit.(x !)-coord. in LTRANS )
    TOF2Brcal::scbrcal[ilay][ibar].getbstat(status);//get status of two ends (from DB)
    nel0=de*convr;// -> photons
//  cout<<"scanbin="<<i1<<" "<<i2<<" r="<<r<<" nphot="<<nel0<<endl;
//
// PM-1 actions --->
//
  if(status[0]>=0){// alive side
    eff=TOF2Scan::scmcscan[cnum].getef1(r,i1,i2);//eff for PM-1
    rgn=TOF2Scan::scmcscan[cnum].getgn1(r,i1,i2);//gain for PM-1
    nel=nel0*eff;// mean total number of photoelectrons
    POISSN(nel,nelec,ierr);// fluctuations
    for(i=0;i<TOFGC::AMSDISL;i++)warr[i]=0;
//    <-------- create phel. arrival-time distribution(PM-1) ---<<<
    for(i=0;i<nelec;i++){
      tm=TOF2Scan::scmcscan[cnum].gettm1(r,i1,i2);//phel.arrival time from interpol.distr.
      ii=integer(floor(tm*ifadcb));
      if(ii<TOFGC::AMSDISL)warr[ii]+=1;
    }
    i0=integer(floor(qtime));// 1st bin pos. in abs. scale (for tslice1)
    if((qtime-geant(i0))>0.5)i0+=1;// to compensate partially binning influence
//    <-------- Loop over distr.bins ---<<<
    for(i=0;i<TOFGC::AMSDISL;i++){
      nelec=warr[i];
      if(nelec!=0){
      ii=i0+i;
      if(ii>TOF2GC::SCTBMX)ii=TOF2GC::SCTBMX;
      if(nelec<30){
        for(k=0;k<nelec;k++){//<-- summing of all amplitudes from photoelectrons 
          am=seres+sesig*rnormx();//amplitude from single elect. spectrum
          if(am>0.)tslice1[ii]+=am*rgn;
        }
      }
      else{
        am=sesave*nelec;
        sig=sessig*sqrt(geant(nelec));
        tslice1[ii]+=(am+sig*rnormx())*rgn;
      }
      }
    } // >>>----- end of PM-1 loop ------>
  }
//
// PM-2 actions --->
//
  if(status[1]>=0){// alive side
    eff=TOF2Scan::scmcscan[cnum].getef2(r,i1,i2);//eff for PM-2
    rgn=TOF2Scan::scmcscan[cnum].getgn2(r,i1,i2);//gain for PM-2
    nel=nel0*eff;// mean total number of photoelectrons
    POISSN(nel,nelec,ierr);// fluctuations
    for(i=0;i<TOFGC::AMSDISL;i++)warr[i]=0;
//    <-------- create phel. arrival-time distribution(PM-2) ---<<<
    for(i=0;i<nelec;i++){
      tm=TOF2Scan::scmcscan[cnum].gettm2(r,i1,i2);//phel.arrival time from interpol.distr.
      ii=integer(floor(tm*ifadcb));
      if(ii<TOFGC::AMSDISL)warr[ii]+=1;
    }
    i0=integer(floor(qtime));// 1st bin pos. in abs. scale (for tslice2)
    if((qtime-geant(i0))>0.5)i0+=1;// to compensate partially binning influence
//    <-------- Loop over distr.bins ---<<<
    for(i=0;i<TOFGC::AMSDISL;i++){
      nelec=warr[i];
      if(nelec!=0){
      ii=i0+i;
      if(ii>TOF2GC::SCTBMX)ii=TOF2GC::SCTBMX;
      if(nelec<30){
        for(k=0;k<nelec;k++){//<-- summing of all amplitudes from photoelectrons 
          am=seres+sesig*rnormx();//amplitude from single elect. spectrum
          if(am>0.)tslice2[ii]+=am*rgn;
        }
      }
      else{
        am=sesave*nelec;
        sig=sessig*sqrt(geant(nelec));
        tslice2[ii]+=(am+sig*rnormx())*rgn;
      }
      }
    } // >>>----- end of PM-2 loop ------>
  }
//-----------------------------------
      ptrN=ptr->next();
      idN=0; 
      if(ptrN)idN=ptrN->getid();// id of the next G-hit
      if(idN != id){ // <---- last or new bar -> create Tovt-objects :
        edepb*=1000.;// --->MeV
        if(edepb>TFMCFFKEY.Thr){// process only bar with Edep>Ethr
// PM-1 loop to apply pulse shape :
        if(status[0]>=0){// alive side
          am0=fabs(TFMCFFKEY.blshift*rnormx());//base line shift simulation
          idd=id*10+1;//LBBS
          for(i=0;i<TOF2GC::SCTBMX+1;i++)tslice[i]=am0;
          for(i=0;i<TOF2GC::SCTBMX;i++){
            am=tslice1[i];
            if(am>0){
              jm=i+npshbn;
              if(jm>TOF2GC::SCTBMX)jm=TOF2GC::SCTBMX+1;
              for(j=i;j<jm;j++)
                          tslice[j]+=am*pmplsh[j-i];
            }
          }        
          TOF2Tovt::totovt(idd,edepb,tslice);// Tovt-obj for side(PM)-1
          if(TFMCFFKEY.mcprtf[1] != 0)
                        TOF2Tovt::displ_a(idd,20,tslice);//print PMT pulse
        }
//        
// PM-2 loop to apply pulse shape :
        if(status[1]>=0){// alive side
          am0=fabs(TFMCFFKEY.blshift*rnormx());//base line shift simulation
          idd=id*10+2;
          for(i=0;i<TOF2GC::SCTBMX+1;i++)tslice[i]=am0;
          for(i=0;i<TOF2GC::SCTBMX;i++){
            am=tslice2[i];
            if(am>0){
              jm=i+npshbn;
              if(jm>TOF2GC::SCTBMX)jm=TOF2GC::SCTBMX+1;
              for(j=i;j<jm;j++)
                          tslice[j]+=am*pmplsh[j-i];
            }
          }        
          TOF2Tovt::totovt(idd,edepb,tslice);// Tovt-obj for side(PM)-2
          if(TFMCFFKEY.mcprtf[1] != 0)
                        TOF2Tovt::displ_a(idd,20,tslice);//print PMT pulse
        }
        }// ---> end of edep check
//
        for(i=0;i<TOF2GC::SCTBMX+1;i++)tslice1[i]=0.;//clear flash ADC arrays for next bar
        for(i=0;i<TOF2GC::SCTBMX+1;i++)tslice2[i]=0.;
        edepb=0.;// clear Edep
      }// ---> end of next/last bar check
//
    ptr=ptr->next();
  }// ------ end of geant hits loop ---->
//
  if(TFMCFFKEY.mcprtf[2]!=0){
    for(i=0;i<TOF2GC::SCLRS;i++)HF1(1050+i,geant(nhitl[i]),1.);
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
void TOF2Tovt::inipsh(integer &nbn, geant arr[])
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
    bl=ib*TOF2DBc::fladctb();
    bh=bl+TOF2DBc::fladctb();
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
      //next line to prevent stupid egcs optimize io away.
      float jdum=RNDM(io); 
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
        if(TFMCFFKEY.mcprtf[0] != 0){
          tota=tota*TOF2DBc::fladctb()/50.;
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
  cerr<<"TOF2Tovt-inipsh-Fatal: wrong pulse shape !?"<<'\n';
  exit(1);
}
//--------------------------------------------------------------------------
//
// function below creates shaper stand. pulse shape array arr[] with bin=shaptb :
// (this array is shaper responce to 1pC injection with duration=shaptb;
//  shaper rise/fall times are shrtim<<shftim; nbn is array length,defined as
//  boundary where charge becomes less than 0.0005 of initial value(~7.1*Tfall)
//
void TOF2Tovt::inishap(integer &nbn, geant arr[])
{
  geant tr,tf,tmb,tmax,toti,rest,t1,t2,anr;
  integer i,imax,j,ifl(0);
  tr=TOF2DBc::shrtim();
  tf=TOF2DBc::shftim();// tempor->change later to tf from TOF2Brcal::scbrcal-obj
  tmb=TOF2DBc::shaptb();
  imax=600;// max range
  arr[0]=1.-exp(-tmb/tr);// fast charge (neglect by discharge !)
  for(i=1;i<imax;i++){ // <--- time loop ---
    t1=i*tmb;
    arr[i]=arr[0]*exp(-t1/tf);
    if(arr[i]<(0.0005*arr[0])){
      nbn=i+1;
      if(TFMCFFKEY.mcprtf[0] != 0){
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
  cerr<<"TOF2Tovt-ini_shaper-Fatal: pulse tail out of range !"<<'\n';
  cerr<<"tr/tf= "<<tr<<" "<<tf<<" nbins="<<i<<'\n';
  exit(1);
}
//--------------------------------------------------------------------------
//
// function below simulate PMT saturation, it returns nonsat. factor <=1. :
//
geant TOF2Tovt::pmsatur(const geant am){
  geant gain=1.; //tempor. no saturation
  return gain;
}
//--------------------------------------------------------------------------
//
// function to display PMT pulse (flash-ADC array arr[]) :
//
void TOF2Tovt::displ_a(const int id, const int mf, const geant arr[]){
  integer i;
  static integer ifrs(0);
  geant tm,a(0.);
  static geant tb,tbi;
  if(ifrs++==0){
    tb=geant(mf)*TOF2DBc::fladctb();
    tbi=TOF2DBc::fladctb();
    HBOOK1(1000,"PMT flash-ADC pulse (MC)",80,0.,80*tb,0.);
  }
  HRESET(1000," ");
  cout<<"PMT:counter-id = "<<id<<" (LBBS, L-layer, BB-bar, S-side)"<<'\n';
  for(i=1;i<=TOF2GC::SCTBMX;i++){
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
void TOF2Tovt::displ_as(const int id, const int mf, const geant arr[]){
  integer i;
  static integer ifrs(0);
  geant tm,a(0.);
  static geant tb,tbi;
  if(ifrs++==0){
    tb=geant(mf)*TOF2DBc::shaptb();
    tbi=TOF2DBc::shaptb();
    HBOOK1(1001,"Shaper pulse (MC)",80,0.,80*tb,0.);
  }
  cout<<"SHP:counter-id = "<<id<<" (LBBS, L-layer, BB-bar, S-side)"<<'\n';
  HRESET(1001," ");
  for(i=1;i<=TOF2GC::SCSBMX;i++){
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
void TOF2Tovt::totovt(integer idd, geant edepb, geant tslice[])
{
  integer i,j,ij,ilay,ibar,isid,id,_sta,stat(0);
  geant tm,a,am,am0,amd,tmp,amp,amx,amxq;
  geant iq0,it0,itau;
  integer _ntr1,_ntr2,_ntr3,_nftdc,_nstdc,_nadch,_nadcl;
  number _ttr1[TOF2GC::SCTHMX1],_ttr2[TOF2GC::SCTHMX1],_ttr3[TOF2GC::SCTHMX1];
  number _tftdc[TOF2GC::SCTHMX2],_tftdcd[TOF2GC::SCTHMX2];
  number _tstdc[TOF2GC::SCTHMX3];
  number _adch[TOF2GC::SCTHMX4],_adcl[TOF2GC::SCTHMX4];
  number tovt,aqin;
  int upd1,upd2,upd3; // up/down flags for 3 fast discr. (z>=1;z>1;z>2)
  int upd11,upd12,upd13,upd21,upd31;
  int updsh;
  int imax,imin;
  geant ahlr[2],lhr;
  geant tshd,tshup,charge,saturf;
  geant td1b1,td1b2,td1b2d,td1b3;
  geant td2b1,td3b1;
  geant tmd1u,tmd1d,tmd3u,tmd3d,tbn,w,bo1,bo2,bn1,bn2,tmark;
  geant tshap1[TOF2GC::SCSBMX+1]; // temporary shaper pulse array
  geant tshap2[TOF2GC::SCSBMX+1]; // temporary shaper pulse array
  static integer first=0;
  static integer nshbn,mxcon,mxshc,mxshcg;
  static geant fladcb,shapb,cconv;
  geant daqt0,daqt1,daqt2,daqt3,daqt4,fdaqt0;
  static geant daqp0,daqp2,daqp3,daqp4,daqp5,daqp6,daqp7,daqp8,daqp9,daqp10;
  static geant daqp11,daqp12;
  geant adccf;
  geant peds[4],sigs[4],noise,rrr;
  number adcs;
  static geant shplsh[600];  // Shaper standard pulse shape array
//
  id=idd/10;// LBB
  isid=idd%10-1;// side
  ilay=id/100-1;
  ibar=id%100-1;
//
  if(first++==0){
    TOF2Tovt::inishap(nshbn,shplsh); // prepare Shaper stand. pulse shape arr.
    cout<<"TOF2Tovt::build:shaper stand.pulse length="<<nshbn<<endl;
    fladcb=TOF2DBc::fladctb();          // and other time-stable parameters
    shapb=TOF2DBc::shaptb();
    cconv=fladcb/50.; // for mV->pC (50 Ohm load)
    daqp0=TOF2DBc::daqpwd(0);//max(=min now) duration of FT(Z>=1) trig.pulse
    daqp2=TOF2DBc::daqpwd(2);//max(=min now) duration of Z>2 trig.pulse 
    daqp3=TOF2DBc::daqpwd(3);//dead time for fast-TDC signal (dbl-pulse resol)
    daqp4=TOF2DBc::daqpwd(4);
    daqp5=TOF2DBc::daqpwd(5);
    daqp6=TOF2DBc::daqpwd(6);
    daqp7=TOF2DBc::daqpwd(7);// dead time for z>=1(FT) branch of adode-discr.
    daqp9=TOF2DBc::daqpwd(9);// dead time for z>2 branch of dinode-discr.
    daqp10=TOF2DBc::daqpwd(10);// intrinsic t-dispersion of comparator(on low thr.outp)
    daqp11=TOF2DBc::daqpwd(11);// min.duration time of comparator(..................)
  }
// time-dependent parameters !!! :
  daqt0=TOF2Varp::tofvpar.daqthr(0); // fast discr. thresh. for slow/fast TDC branch
  fdaqt0=0.1*daqt0;// lowered threshold to select "working" part of pulse(m.b. loose some charge !!!)
  daqt1=TOF2Varp::tofvpar.daqthr(1); // fast discr. thresh. for z>=1 (FT) branch
  TOF2Brcal::scbrcal[ilay][ibar].getahlr(ahlr);//get high/low(a2d) ratio
  TOFBPeds::scbrped[ilay][ibar].getped(peds);//get peds
  TOFBPeds::scbrped[ilay][ibar].getsig(sigs);//get sigmas
  adccf=TOF2Brcal::scbrcal[ilay][ibar].getadccf();
  daqt2=TOF2Varp::tofvpar.daqthr(2)*ahlr[isid];//dinode discr.("Z>2") thresh., multipl. by a2d ratio
  lhr=1./ahlr[isid];//low/high(d2a) ratio
//
// -----> create/fill summary Tovt-object for idsoft=idd :
//
        if(tslice[TOF2GC::SCTBMX]>daqt0){
          cout<<"SITOFTovt-warning: MC_flash-ADC overflow, id="<<idd<<
             "  A-last= "<<tslice[TOF2GC::SCTBMX]<<'\n';
//          if(TFMCFFKEY.mcprtf[2])TOF2Tovt::displ_a(idd,125,tslice);//print PMT pulse
        }
        for(i=TOF2GC::SCTBMX-1;i>0;i--)
            if(tslice[i]>fdaqt0 && tslice[i-1]>fdaqt0)break;//find high limit
        imax=i+1;
        for(i=0;i<TOF2GC::SCTBMX;i++)if(tslice[i]>fdaqt0)break;//find low limit
        imin=i;
        if(((imax-imin)*fladcb) > TOF2DBc::shctim())
                              imax=imin+int(TOF2DBc::shctim()/fladcb);// protection against
//                                              too long pulses (max=max.integration_time)
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
//      _
//    _| |_
//___|     |___  PMT-pulse
//   ^tmp
//     ^tm
//
        amx=0.;
        amp=0.;
        tmp=fladcb*imin;
        tm=tmp;
        for(i=imin;i<imax;i++){  //  <--- time bin loop for time measurements ---
          tm+=fladcb;
          am=tslice[i];
          if(am>amx)amx=am;// tempor
          am=am+TFMCFFKEY.hfnoise*rnormx();//tempor high freq. noise
          charge+=am;
//--------------------------          
// discr-1(anode,fast comparator for TDC/z>=1 ) up/down setting with LOW thr.:
          if(am>=daqt0){
            if(upd1 ==0){
              tmd1u=tmp+fladcb*(daqt0-amp)/(am-amp);// up time of discr.1(bin-width compencated)
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
                if(_ntr1<TOF2GC::SCTHMX1){
                  _ttr1[_ntr1]=tm;//don't need accurate up-time for trigger
                  _ntr1+=1;
                }
                else{
                  cerr<<"TOF2Tovt::build-warning: trig1 hits ovfl"<<'\n';
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
                if(_nstdc<TOF2GC::SCTHMX3){
                  td1b3=tm;
                  _tstdc[_nstdc]=tmark;
                  _nstdc+=1;
                }
                else{
                  cerr<<"TOF2Tovt::build-warning: s-TDC hits ovfl"<<'\n';
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
              if(_nftdc<TOF2GC::SCTHMX2){
                _tftdc[_nftdc]=td1b2;//write up-time
                _tftdcd[_nftdc]=td1b2d;//write down-time
                _nftdc+=1;
              }
              else{
                cerr<<"TOF2Tovt::build-warning: f-TDC hits ovfl"<<'\n';
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
                if(_ntr3<TOF2GC::SCTHMX1){
                  _ttr3[_ntr3]=tmd3u;//up-time
                  _ntr3+=1;
                }
                else{
                  cerr<<"TOF2Tovt::build-warning: trig3 hits ovfl"<<'\n';
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
        saturf=TOF2Tovt::pmsatur(charge);//true charge reduction fact. due to satur.
        if(TFMCFFKEY.mcprtf[2]!=0){
          if(idd==1071)HF1(1073,amx,1.);// ampl.spectrum for ref.bar
	  if(idd==1071)HF1(1070,float(charge),1.);
	}
//**************************************************************************
        number ped,sig;
        _nadch=0;
        _nadcl=0;
	ped=peds[2*isid];// in adc-chann. units(float)
	sig=sigs[2*isid];
	noise=sig*rnormx();// in ADC-channels
        aqin=number(charge)+noise*adccf;// signal+noise in pC
        TOF2Brcal::scbrcal[ilay][ibar].q2t2q(0,isid,0,adcs,aqin);// high q->adc-chan(no "integerization") 
	if(TFMCFFKEY.mcprtf[2]!=0)
	                          if(idd==1071)HF1(1074,float(adcs),1.);
	if(adcs>0.){
          _adch[_nadch]=adcs+ped;// ADC channels+ped(no "integerization")
          _nadch+=1;
        }
	ped=peds[1+2*isid];
	sig=sigs[1+2*isid];
        TOF2Brcal::scbrcal[ilay][ibar].q2t2q(0,isid,1,adcs,aqin);// low q->adc-chan(no "integerization")
//  (assume noise as sitting only at PM load-resistor ! So use the same aqin=charge+noise) 
	if(TFMCFFKEY.mcprtf[2]!=0)
	                          if(idd==1071)HF1(1075,float(adcs),1.);
	if(adcs>0.){
          _adcl[_nadcl]=adcs+ped;// ADC channels+ped(no "integerization")
          _nadcl+=1;
        }
//
//------------------------------
// now create/fill TOF2Tovt object (id=idd) with above digi-data:
      if(_nftdc>0 && _nstdc>0){ 
        _sta=0;    
        stat=0;
        if(AMSEvent::gethead()->addnext(AMSID("TOF2Tovt",ilay), new
             TOF2Tovt(idd,_sta,charge,edepb,
             _ntr1,_ttr1,_ntr3,_ttr3,
             _nftdc,_tftdc,_tftdcd,_nstdc,_tstdc,
             _nadch,_adch,_nadcl,_adcl)))stat=1;
      }
        return;
//
//
}
//---------------------------------------------------------------------
void TOF2Tovt::aintfit(){
  int i,ier;  
  int ifit[TOF2GC::SCIPAR];
  char pnam[TOF2GC::SCIPAR][6],pnm[6];
  number argl[10];
  int iargl[10];
  number start[TOF2GC::SCIPAR],pstep[TOF2GC::SCIPAR],plow[TOF2GC::SCIPAR],phigh[TOF2GC::SCIPAR];
  strcpy(pnam[0],"dtime");
  strcpy(pnam[1],"tzero");
  strcpy(pnam[2],"offst");
  start[0]=64.;
  start[1]=145.;
  start[2]=2.;
   pstep[0]=5.;
   pstep[1]=10.;
   pstep[2]=1.;
  plow[0]=10.;
  plow[1]=1.;
  plow[2]=0.;
   phigh[0]=100.;
   phigh[1]=500.;
   phigh[2]=9.;
  for(i=0;i<TOF2GC::SCIPAR;i++)ifit[1]=1;
// ------------> initialize parameters for Minuit:
  MNINIT(5,6,6);
  MNSETI("MC A-integrator calibration for TOF-system");
  for(i=0;i<TOF2GC::SCIPAR;i++){
    strcpy(pnm,pnam[i]);
    ier=0;
    MNPARM((i+1),pnm,start[i],pstep[i],plow[i],phigh[i],ier);
    if(ier!=0){
      cout<<"MC A-int. calib: Param-init problem for par-id="<<pnam[i]<<'\n';
      exit(10);
    }
    argl[0]=number(i+1);
    if(ifit[i]==0){
      ier=0;
      MNEXCM(mfun,"FIX",argl,1,ier,0);
      if(ier!=0){
        cout<<"MC A-int. calib: Param-fix problem for par-id="<<pnam[i]<<'\n';
        exit(10);
      }
    }
  }
//----
  cout<<"MC A-int. calib: found "<<ievent<<"  events !!!"<<endl;
  if(ievent>100){
    cout<<"MC A-int. calib: start minimization ..."<<endl;  
    argl[0]=0.;
    ier=0;
    MNEXCM(mfun,"MINIMIZE",argl,0,ier,0);
    if(ier!=0){
      cout<<"MC A-int. calib: MINIMIZE problem !"<<'\n';
      exit(10);
    }  
    MNEXCM(mfun,"MINOS",argl,0,ier,0);
    if(ier!=0){
      cout<<"MC A-int. calib: MINOS problem !"<<'\n';
      exit(10);
    }
    argl[0]=number(3);
    ier=0;
    MNEXCM(TOF2Tovt::mfun,"CALL FCN",argl,1,ier,0);
    if(ier!=0){
      cout<<"MC A-int. calib: final CALL_FCN problem !"<<'\n';
      exit(10);
    }
  }
}
//-----
// This is standard Minuit FCN :
void TOF2Tovt::mfun(int &np, number grad[], number &f, number x[]
                                                        , int &flg, int &dum){
  int i,j;
  number ff;
  f=0.;
  for(i=0;i<ievent;i++){
    ff=itovts[i]-(x[0]*log(icharg[i]-x[2])-x[1]);
    f+=(ff*ff);
  }
  if(flg==3){
    f=sqrt(f/number(ievent));
    cout<<"MC A-integrator: funct/events="<<f<<endl;
    for(i=0;i<TOF2GC::SCIPAR;i++){
      cout<<" np/par="<<i<<" "<<x[i]<<endl;
    }
  }
}
//
//---------------------------------------------------------------------
//
// function to get absolute time of the FIRST trigger(coincidence) "z>=1";
// trcode - trigger code (pattern) =-1/n, n>=0 -> missing plane #, 
// =-1 -> no trigger.
//
number TOF2Tovt::tr1time(int &trcode, integer toftrp[]){
  integer i1,i2,isds(0),first(0);
  integer i,j,ilay,ibar,ntr,idd,id,idN,stat,intrig;
  integer npdpl[TOF2GC::SCLRS]={0,0,0,0};
  integer sbt,lsbit(1);
  number ttr[TOF2GC::SCTHMX1];
  geant t1,t2;
  AMSBitstr trbs[2];
  AMSBitstr trbc;
  AMSBitstr trbl[TOF2GC::SCLRS];
  TOF2Tovt *ptr;
  TOF2Tovt *ptrN;
//
  geant trigb=TOF2DBc::trigtb();
  geant pwid=TOF2DBc::daqpwd(0);
  for(i=0;i<TOF2GC::SCLRS;i++)toftrp[i]=0;
//
  for(ilay=0;ilay<TOF2GC::SCLRS;ilay++){// <--- layers loop (Tovt containers) ---
    ptr=(TOF2Tovt*)AMSEvent::gethead()->
                               getheadC("TOF2Tovt",ilay,0);
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
        if(i1>=TOFGC::SCBITM)i1=TOFGC::SCBITM-1;
        if(i2>=TOFGC::SCBITM)i2=TOFGC::SCBITM-1;
        if(isds<2)
          if(stat==0)trbs[isds].bitset(i1,i2);//set bits according to pulse width
        else
            cerr<<"TOF2Tovt::tr1time: error: >2sides !!!"<<'\n';
      }// --- end of trig-hits loop --->
      isds+=1; 
      ptrN=ptr->next();
      idN=0;
      if(ptrN)idN=(ptrN->getid())/10; //next hit short id
//
      if(idN != id){ // next hit is OTHER counter hit or last hit,
//       so create both side (counter) bit pattern for CURRENT counter
        if(TFMCFFKEY.trlogic[0] == 0)
                                  trbc=trbs[0] & trbs[1];// 2-sides AND
        else
                                  trbc=trbs[0] | trbs[1];// 2-sides OR
//
        intrig=TriggerLVL3::TOFInFastTrigger(uinteger(ibar),uinteger(ilay));
        if(intrig<1)trbc.bitclr(1,0); // counter not in trigger !!!                                  
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
    if(TFMCFFKEY.trlogic[1] == 1)return t1;// if ALL4 was requested
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
number TOF2Tovt::tr3time(int &trcode, integer toftrp[]){
  integer i1,i2,isds(0),first(0);
  integer i,j,ilay,ibar,ntr,idd,id,idN,stat,intrig;
  integer npdpl[TOF2GC::SCLRS]={0,0,0,0};
  integer sbt,lsbit(1);
  number ttr[TOF2GC::SCTHMX1];
  geant t1,t2;
  AMSBitstr trbs[2];
  AMSBitstr trbc;
  AMSBitstr trbl[TOF2GC::SCLRS];
  TOF2Tovt *ptr;
  TOF2Tovt *ptrN;
//
  geant trigb=TOF2DBc::trigtb();
  geant pwid=TOF2DBc::daqpwd(0);
  for(i=0;i<TOF2GC::SCLRS;i++)toftrp[i]=0;
//
  for(ilay=0;ilay<TOF2GC::SCLRS;ilay++){// <--- layers loop (Tovt containers) ---
    ptr=(TOF2Tovt*)AMSEvent::gethead()->
                               getheadC("TOF2Tovt",ilay,0);
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
        if(i1>=TOFGC::SCBITM)i1=TOFGC::SCBITM-1;
        if(i2>=TOFGC::SCBITM)i2=TOFGC::SCBITM-1;
        if(isds<2)
          if(stat==0)trbs[isds].bitset(i1,i2);//set bits according to pulse width
        else
            cerr<<"TOF2Tovt::tr3time: error: >2sides !!!"<<'\n';
      }// --- end of trig-hits loop --->
      isds+=1; 
      ptrN=ptr->next();
      idN=0;
      if(ptrN)idN=(ptrN->getid())/10; //next hit short id
//
      if(idN != id){ // next hit is OTHER counter hit or last hit,
//       so create both side (counter) bit pattern for CURRENT counter
        if(TFMCFFKEY.trlogic[0] == 0)
                                  trbc=trbs[0] & trbs[1];// 2-sides AND
        else
                                  trbc=trbs[0] | trbs[1];// 2-sides OR
//                                  
        intrig=TriggerLVL3::TOFInFastTrigger(uinteger(ibar),uinteger(ilay));
        if(intrig<1)trbc.bitclr(1,0); // counter not in trigger !!!                                  
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
    if(TFMCFFKEY.trlogic[1] == 1)return t1;// if ALL4 was requested
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
  assert(il<TOFGC::SCBITM && ih<TOFGC::SCBITM);
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
  assert(il<TOFGC::SCBITM && ih<TOFGC::SCBITM);
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
  unsigned short int arr[TOFGC::SCWORM];
  if(lenb != lenc){
    cout<<"AMSBitstr:AND-warning:len1 != len2 "<<lenb<<" "<<lenc<<'\n';
    if(lenc<lenb && lenb<TOFGC::SCWORM)len=lenc;
    else if(lenb<lenc && lenc<TOFGC::SCWORM)len=lenb;
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
  unsigned short int arr[TOFGC::SCWORM];
  if(lenb != lenc){
    cout<<"AMSBitstr:OR-warning:len1 != len2 "<<lenb<<" "<<lenc<<'\n';
    if(lenc<lenb && lenb<TOFGC::SCWORM)len=lenc;
    else if(lenb<lenc && lenc<TOFGC::SCWORM)len=lenb;
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
void TOF2RawEvent::mc_build(int &status)
{
  integer i,j,jj,ilay,last,ibar,id,idd,iddN,isd,_sta,stat(0);
  integer nftdc,nstdc,nadch,nadcl;
  int16u _nftdc,_nstdc,_nadch,_nadcl,itt;
  number  tftdc[TOF2GC::SCTHMX2],tftdcd[TOF2GC::SCTHMX2],tstdc[TOF2GC::SCTHMX3];
  number tadch[TOF2GC::SCTHMX4],tadcl[TOF2GC::SCTHMX4];
  int16u  ftdc[TOF2GC::SCTHMX2*2],stdc[TOF2GC::SCTHMX3*4],adch[TOF2GC::SCTHMX4],adcl[TOF2GC::SCTHMX4];
  number t,t1,t2,t3,t4,tprev,ttrig1,ttrig3,dt,tlev1,tl1d,ftrig;
  geant charge,edep,strr[2][2],str,offs;
  geant peds[4],daqt1,rrr;
  number pedv,amp;
  integer iamp;
  int trcode,trcode3;
  integer trflag(0),trpatt3[TOF2GC::SCLRS];
  integer trpatt[TOF2GC::SCLRS]={0,0,0,0};
  integer it,it1,it2,it3,it4,it0;
  int16u phbit,maxv;
  static geant ftpw=TOF2DBc::daqpwd(12);// dummy gap in stretcher sequence (ns)
  TOF2Tovt *ptr;
  TOF2Tovt *ptrN;
  status=1;// bad
  phbit=TOF2GC::SCPHBP;
  maxv=phbit-1;//max possible TDC value (16383)
  daqt1=TOF2Varp::tofvpar.daqthr(3);
//
  trflag=0;
  ttrig1=TOF2Tovt::tr1time(trcode,trpatt);//get abs.trig1(FT"z>=1") signal time/patt
  if(trcode<0){
    TOF2RawEvent::settrfl(trflag);// set trig_flag to 0 ("no trigger")
    TOF2RawEvent::setpatt(trpatt);//set 0 trigger pattern 
    return;
  } 
  status=0;
  trflag=1;// ok: h/w trigger(z>=1) present -> do digitization:
//
  ttrig3=TOF2Tovt::tr3time(trcode3,trpatt3);//get abs.trig3("z>2") signal time/patt
  if(trcode3>=0){
    trflag=3;
    if(ttrig3<ttrig1){ // tempor: priority is not clear now !
      ttrig1=ttrig3;
      for(i=0;i<TOF2GC::SCLRS;i++)trpatt[i]=trpatt3[i];
    }
  }
//
  if(trcode==0)trflag+=10;// mark 4-layer trigger 
  TOF2RawEvent::settrfl(trflag);// set final trigger flag
  TOF2RawEvent::setpatt(trpatt);// set trigger pattern
  ftrig=ttrig1+TOF2Varp::tofvpar.ftdelf();// FTrigger abs time (fixed delay added)
  TOF2RawEvent::settrtime(ftrig);// set final FTrigger time
//
  tlev1=ftrig+TOF2DBc::accdel();// Lev-1 accept-signal abs.time
//
  for(ilay=0;ilay<TOF2GC::SCLRS;ilay++){// <--- layers loop (Tovt containers) ---
    ptr=(TOF2Tovt*)AMSEvent::gethead()->
                               getheadC("TOF2Tovt",ilay,0);
    while(ptr){// <--- Tovt-hits loop in layer ---
      idd=ptr->getid();// LBBS
      id=idd/10;// short id=LBB
      ibar=id%100-1;
      TOF2Brcal::scbrcal[ilay][ibar].gtstrat(strr);// get str-ratios for two sides
      TOFBPeds::scbrped[ilay][ibar].getped(peds);// get peds
      isd=idd%10-1;
      str=strr[isd][0];// str-ratio for given TovT-hit
      offs=strr[isd][1];// offset in str-ratio for given TovT-hit
      _sta=ptr->getstat();
      charge=ptr->getcharg();
      edep=ptr->getedep();
//
      nftdc=ptr->getftdc(tftdc,tftdcd);// get number and times of fast-TDC hits
      _nftdc=0;
      tl1d=tlev1+TOF2Varp::tofvpar.sftdcd();// just to simulate sTDC delay wrt fTDC
      for(j=0;j<nftdc;j++){//        <--- ftdc-hits loop ---
        jj=nftdc-j-1;// LIFO readout mode (last stored - first read)
        t1=tftdc[jj];
        t2=tftdcd[jj];
        dt=tl1d-t2;// follow LIFO mode of readout : down-edge - first hit
        it=integer(floor(dt/TOF2DBc::tdcbin(0))); // conv. to fast-TDC (history) t-binning
        if(it>maxv){
          cout<<"TOF2RawEvent_mc: warning : Hist-TDC down-time overflow !!!"<<'\n';
          it=maxv;
        }
        itt=int16u(it);
        if(!TOF2DBc::pbonup())itt=itt|phbit;//add phase bit if necessary
        ftdc[_nftdc]=itt;
        _nftdc+=1;
        dt=tl1d-t1;// follow LIFO mode of readout : leading(up) edge - second
        it=integer(floor(dt/TOF2DBc::tdcbin(0))); // conv. to fast-TDC (history) t-binning
        if(it>maxv){
          cout<<"TOF2RawEvent_mc: warning : Hist-TDC up-time overflow !!!"<<'\n';
          it=maxv;
        }
        itt=int16u(it);
        if(TOF2DBc::pbonup())itt=itt|phbit;//add phase bit if necessary
        ftdc[_nftdc]=itt;
        _nftdc+=1;
      }//--- end of ftdc-hits loop --->
//------------
      nstdc=ptr->getstdc(tstdc);// get number and times of slow-TDC hits
      _nstdc=0;
      for(j=0;j<nstdc;j++){//       <--- stdc-hits loop ---
        jj=nstdc-j-1;// LIFO readout mode (last stored - first read)
        t1=tstdc[jj]+rnormx()*TOF2DBc::strjit1();//"start"-signal time + jitter
        t2=ftrig+rnormx()*TOF2DBc::strjit2();//"stop"-signal = ftrig + jitter
        t3=t2+ftpw;//"dummy"-signal time (fixed delay = FT_coinc-pulse width)
        t4=t2+offs+(t2-t1)*str+rnormx()*TOF2DBc::strflu();//"end-mark"_signal + offs + fluct
        dt=ftrig-t1;// total FT-delay wrt t1
        if(dt<=TOF2DBc::ftdelm()){ // check max. delay of "fast-trigger" signal
          it0=integer(floor(tlev1/TOF2DBc::tdcbin(1)));//Lev-1 abs.time (integer)  
//follow LIFO mode of readout: stretcher "end-mark" edge - first hit; "start" - last:
//  prepare 4-th component("end-mark") of "4-edge" TOF-hit:
          it4=integer(floor(t4/TOF2DBc::tdcbin(1)));
          it=it0-it4;// time wrt lev-1 signal
          if(it>maxv){
            cout<<"TOF2RawEvent_mc: warning : 4-th edge TDC overflow !!!"<<'\n';
            it=maxv;
          }
          itt=int16u(it);
          if(!TOF2DBc::pbonup())itt=itt|phbit;//add phase bit on down-edge,if necessary
          stdc[_nstdc]=itt;
          _nstdc+=1;
//  prepare 3-d component("dummy"):
          it3=integer(floor(t3/TOF2DBc::tdcbin(1)));
          it=it0-it3;// time wrt lev-1 signal
          if(it>maxv){
            cout<<"TOF2RawEvent_mc: warning : 3-rd edge TDC overflow !!!"<<'\n';
            it=maxv;
          }
          itt=int16u(it);
          if(TOF2DBc::pbonup())itt=itt|phbit;//add phase bit on up-edge,if necessary
          stdc[_nstdc]=itt;
          _nstdc+=1;
//  prepare 2-nd component("stop"):
          it2=integer(floor(t2/TOF2DBc::tdcbin(1)));
          it=it0-it2;// time wrt lev-1 signal
          if(it>maxv){
            cout<<"TOF2RawEvent_mc: warning : 2-nd edge TDC overflow !!!"<<'\n';
            it=maxv;
          }
          itt=int16u(it);
          if(!TOF2DBc::pbonup())itt=itt|phbit;//add phase bit on down-edge,if necessary
          stdc[_nstdc]=itt;
          _nstdc+=1;
//  prepare 1-st component("start") of "4-edge" TOF-hit:
          it1=integer(floor(t1/TOF2DBc::tdcbin(1)));
          it=it0-it1;// time wrt lev-1 signal
          if(it>maxv){
            cout<<"TOF2RawEvent_mc: warning : 1-st edge TDC overflow !!!"<<'\n';
            it=maxv;
          }
          itt=int16u(it);
          if(TOF2DBc::pbonup())itt=itt|phbit;//add phase bit on up-edge,if necessary
          stdc[_nstdc]=itt;
          _nstdc+=1;
        }
      }//--- end of stdc-hits loop --->
//----------------
      _nadch=0;
      pedv=peds[2*isd];
      nadch=ptr->getadch(tadch);// get number and counts of high(anode)-ADC hits
      for(j=0;j<nadch;j++){// <--- adch-hits loop ---
        jj=nadch-j-1;// LIFO readout mode (last stored - first read)
	amp=tadch[jj];// here charge is quantized by "adccf" but not "integerized"
        iamp=integer(floor(amp));//go to real ADC-channels("integerization")
        if(iamp>TOF2GC::SCADCMX){
          cout<<"TOF2RawEvent::mc_build warning: high ADC overflow,id="<<idd<<'\n';
          iamp=TOF2GC::SCADCMX;
        }
	amp=number(iamp)-pedv;// subtract pedestal (loose "integerization" !)
	if(amp>daqt1){// DAQ readout threshold
	  iamp=integer(floor(amp/TOF2DBc::tdcbin(2)));// again "integerization" but to DAQ binning
          itt=int16u(iamp);
          adch[_nadch]=itt;
          _nadch+=1;
	}
      }//--- end of adch-hits loop --->
//--------------------
      _nadcl=0;
      pedv=peds[1+2*isd];
      nadcl=ptr->getadcl(tadcl);// get number and counts of high(anode)-ADC hits
      for(j=0;j<nadcl;j++){// <--- adch-hits loop ---
        jj=nadcl-j-1;// LIFO readout mode (last stored - first read)
	amp=tadcl[jj];
        iamp=integer(floor(amp));
        if(iamp>TOF2GC::SCADCMX){
          cout<<"TOF2RawEvent::mc_build warning: low ADC overflow, id="<<idd<<'\n';
          iamp=TOF2GC::SCADCMX;
        }
	amp=number(iamp)-pedv;// subtr. pedestal (loose quantization !)
	if(amp>daqt1){// DAQ readout threshold
	  iamp=integer(floor(amp/TOF2DBc::tdcbin(2)));// in DAQ binning
          itt=int16u(iamp);
          adcl[_nadcl]=itt;
          _nadcl+=1;
	}
      }//--- end of adch-hits loop --->
//
//     ---> create/fill RawEvent-object :
      stat=0;
      if(AMSEvent::gethead()->addnext(AMSID("TOF2RawEvent",0), new
           TOF2RawEvent(idd,_sta,charge,edep,_nftdc,ftdc,_nstdc,stdc,
                               _nadch,adch,_nadcl,adcl)))stat=1;
//
      ptr=ptr->next();// take next Tovt-hit
// 
    }//         --- end of Tovt-hits loop in layer --->
//
  } //                               --- end of layer loop --->
}
//================================================================





TOF2Tovt::TOF2Tovt(integer _ids, integer _sta, number _charga, number _tedep,
  integer _ntr1, number _ttr1[], integer _ntr3, number _ttr3[],
  integer _nftdc, number _tftdc[], number _tftdcd[], integer _nstdc, number _tstdc[],
  integer _nadch, number _adch[],
  integer _nadcl, number _adcl[]):
  idsoft(_ids),status(_sta)
  {
    int i;
    ntr1=_ntr1;
    for(i=0;i<ntr1;i++)ttr1[i]=_ttr1[i];
    ntr3=_ntr3;
    for(i=0;i<ntr3;i++)ttr3[i]=_ttr3[i];
    charga=_charga;
    tedep=_tedep;
    nftdc=_nftdc;
    for(i=0;i<nftdc;i++){
      tftdc[i]=_tftdc[i];
      tftdcd[i]=_tftdcd[i];
    }
    nstdc=_nstdc;
    for(i=0;i<nstdc;i++)tstdc[i]=_tstdc[i];
    nadch=_nadch;
    for(i=0;i<nadch;i++){
      adch[i]=_adch[i];
    }
    nadcl=_nadcl;
    for(i=0;i<nadcl;i++){
      adcl[i]=_adcl[i];
    }
  }

integer TOF2Tovt::gettr1(number arr[]){
  for(int i=0;i<ntr1;i++)arr[i]=ttr1[i];
  return ntr1;
}
integer TOF2Tovt::gettr3(number arr[]){
  for(int i=0;i<ntr3;i++)arr[i]=ttr3[i];
  return ntr3;
}
integer TOF2Tovt::getftdc(number arr1[], number arr2[]){
  int i;
  for(i=0;i<nftdc;i++){
    arr1[i]=tftdc[i];
    arr2[i]=tftdcd[i];
  }
  return nftdc;
}
integer TOF2Tovt::getstdc(number arr[]){
  for(int i=0;i<nstdc;i++)arr[i]=tstdc[i];
  return nstdc;
}
integer TOF2Tovt::getadch(number arr[]){
  for(int i=0;i<nadch;i++){
    arr[i]=adch[i];
  }
  return nadch;
}
integer TOF2Tovt::getadcl(number arr[]){
  for(int i=0;i<nadcl;i++){
    arr[i]=adcl[i];
  }
  return nadcl;
}





TOF2RawEvent::TOF2RawEvent(int16u _ids, int16u _sta, geant _charge, geant _edep,
   int16u _nftdc, int16u _ftdc[],
   int16u _nstdc, int16u _stdc[], int16u _nadch, int16u _adch[],
   int16u _nadcl, int16u _adcl[]):idsoft(_ids),status(_sta)
   {
     int i;
     nftdc=_nftdc;
     for(i=0;i<nftdc;i++)ftdc[i]=_ftdc[i];
     nstdc=_nstdc;
     for(i=0;i<nstdc;i++)stdc[i]=_stdc[i];
     nadch=_nadch;
     for(i=0;i<nadch;i++)adch[i]=_adch[i];
     nadcl=_nadcl;
     for(i=0;i<nadcl;i++)adcl[i]=_adcl[i];
     charge=_charge;
     edep=_edep;
   }

integer TOF2RawEvent::getnztdc(){
  integer nz(0);
  if(nftdc>0)nz+=1;
  if(nstdc>0)nz+=1;
  if(nadch>0)nz+=1;
  if(nadcl>0)nz+=1;
  return nz;
}

int16u TOF2RawEvent::getftdc(int16u arr[]){
  for(int i=0;i<nftdc;i++)arr[i]=ftdc[i];
  return nftdc;
}

void TOF2RawEvent::putftdc(int16u nelem, int16u arr[]){
  nftdc=nelem;
  for(int i=0;i<nftdc;i++)ftdc[i]=arr[i];
}

int16u TOF2RawEvent::getstdc(int16u arr[]){
  for(int i=0;i<nstdc;i++)arr[i]=stdc[i];
  return nstdc;
}

void TOF2RawEvent::putstdc(int16u nelem, int16u arr[]){
  nstdc=nelem;
  for(int i=0;i<nstdc;i++)stdc[i]=arr[i];
}

int16u TOF2RawEvent::getadch(int16u arr[]){
  for(int i=0;i<nadch;i++)arr[i]=adch[i];
  return nadch;
}

void TOF2RawEvent::putadch(int16u nelem, int16u arr[]){
  nadch=nelem;
  for(int i=0;i<nadch;i++)adch[i]=arr[i];
}

int16u TOF2RawEvent::getadcl(int16u arr[]){
  for(int i=0;i<nadcl;i++)arr[i]=adcl[i];
  return nadcl;
}

void TOF2RawEvent::putadcl(int16u nelem, int16u arr[]){
  nadcl=nelem;
  for(int i=0;i<nadcl;i++)adcl[i]=arr[i];
}
