//  $Id: tofsim02.C,v 1.27 2004/09/27 15:00:32 choumilo Exp $
// Author Choumilov.E. 10.07.96.
// Modified to work with width-divisions by Choumilov.E. 19.06.2002
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
#include <trigger302.h>
#include <mceventg.h>
#include <ecalrec.h>
#include <tofid.h>
//
TOFWScan TOFWScan::scmcscan[TOF2GC::SCBTPN];
//
uinteger TOF2RawEvent::trpatt[TOF2GC::SCLRS]={0,0,0,0};
uinteger TOF2RawEvent::trpatt1[TOF2GC::SCLRS]={0,0,0,0};
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
//  <--- for inp. Y define position in scan array (i1/i2) and correction ratio R.
//
void TOF2Scan::getybin(const geant y, integer &i1, integer &i2, geant &r)
{
  integer i=AMSbins(scanp,y,nscpnts);
  if(i==0){
    i1=0;
    i2=1;
    r=(y-scanp[i1])/(scanp[i2]-scanp[i1]);//r<0 -> y below 1st sc.point
  }
  else if(i<0){
    if(i==-nscpnts){// r>1 -> y above last sc.point
      i1=nscpnts-2;
      i2=nscpnts-1;
      r=(y-scanp[i1])/(scanp[i2]-scanp[i1]);
      }
    else{// <- normal case when y is inside of sc.points array
      i1=-i-1;
      i2=-i;
      r=(y-scanp[i1])/(scanp[i2]-scanp[i1]);
    }
  }
  else{ // exact matching inside sc.points array(i1=i2, r=0.)
    i1=i-1;
    i2=i1;
    r=0.;
  }
}
//------------------------------------------------------------------
geant TOF2Scan::gettm1(const geant r, const int i1, const int i2){
  geant rnd,t1,t2,dummy(-1);
//
  rnd=RNDM(dummy);
  t1=tdist1[i1].gettime(rnd);
  t2=tdist1[i2].gettime(rnd);
  return t1+(t2-t1)*r>0?t1+(t2-t1)*r:0;

};    
geant TOF2Scan::gettm2(const geant r, const int i1, const int i2){
  geant rnd,t1,t2,dummy(-1);
//
  rnd=RNDM(dummy);
  t1=tdist2[i1].gettime(rnd);
  t2=tdist2[i2].gettime(rnd);
  return t1+(t2-t1)*r>0?t1+(t2-t1)*r:0;

};    
//------------------------------------------------------------------
// function to create TOFWScan objects for all sc. bars :
void TOFWScan::build(){
//
  int i,ic,nverst,ivers,dig,nctot(0);
  int brfnam[TOF2GC::SCBTPN];
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
  cout<<"TOFWScan::build: Open file  "<<fname<<'\n';
  ifstream vlfile(fname,ios::in); // open needed tdfmap-file for reading
  if(!vlfile){
    cerr <<"TOF2Scan::build(): missing verslist-file "<<fname<<endl;
    exit(1);
  }
  vlfile >> nverst;// total number of files in the list
  vlfile >> ivers;// first number in first line (vers.# for tdisfmap-file)
  vlfile.close();
  ivers=(ivers%1000);
  cout<<"TOFWScan::build(): use MC-timescan map-file version="<<ivers<<'\n';
//  
//                    <-- first read TScanFileNames vs BType (map) file:
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
  cout<<"       Open file : "<<fname<<'\n';
  ifstream tcfile(fname,ios::in); // open needed tdisfmap-file for reading
  if(!tcfile){
    cerr <<"TOFWScan::build(): missing tof2smap-file "<<fname<<endl;
    exit(1);
  }
  for(ic=0;ic<TOF2GC::SCBTPN;ic++) tcfile >> brfnam[ic];
  tcfile.close();
//-------------------
//                                  <-- now read t-distr. files
 int j,ila,ibr,brt,ibrm,isp,nsp,ibt,cnum,dnum,dnumm,mult,dnumo;
 integer nb;
 int iscp[TOF2GC::SCANPNT];
 geant scp[TOF2GC::SCANPNT];
 geant nft,bl,bw;
 geant arr[TOFGC::AMSDISL];
 geant ef1[TOF2GC::SCANPNT],ef2[TOF2GC::SCANPNT];
 geant rg1[TOF2GC::SCANPNT],rg2[TOF2GC::SCANPNT];
 geant ps1[TOF2GC::SCANPNT][TOF2GC::PMTSMX],ps2[TOF2GC::SCANPNT][TOF2GC::PMTSMX];
 TOFTpoints td1[TOF2GC::SCANPNT];
 TOFTpoints td2[TOF2GC::SCANPNT];
 int nwdivs(0);
 int npmts(0);
 TOF2Scan swdscan[TOF2GC::SCANWDS];
 geant wdivxl[TOF2GC::SCANWDS];
 geant wdivxh[TOF2GC::SCANWDS];
 integer idiv;
 geant eff1,eff2;
//
  for(brt=0;brt<TOF2GC::SCBTPN;brt++){   // <-------- loop bar-types
      dnum=brfnam[brt];// 4-digits t-distr. file name (VLBB)
      mult=1000;
      strcpy(name,"tof2c");
      dnumm=dnum;
      for(i=3;i>=0;i--){//create 4-letters file name
        j=dnumm/mult;
        in[0]=inum[j];
        strcat(name,in);
        dnumm=dnumm%mult;
        mult=mult/10;
      }
      strcat(name,".tsfn");
      if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
      if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
      strcat(fname,name);
      cout<<"       Open file : "<<fname<<'\n';
      ifstream tcfile(fname,ios::in); // open needed t-calib. file for reading
      if(!tcfile){
        cerr <<"AMSTOFScan::build(): missing MC-t_scan file "<<fname<<endl;
        exit(1);
      }
      tcfile >> npmts; // read numb.of PMTs per side
//
      tcfile >> nwdivs; // read tot.number of X(width) divisions
//
      for(int div=0;div<nwdivs;div++){// <--- divisions loop
//
        tcfile >> idiv; // read numb.of current div.
//
        tcfile >> nsp; // read tot.number of scan-point in this div.
        if(nsp>TOF2GC::SCANPNT){
          cerr<<"Sitofinitjob: wrong # of MC Y-scan point ! "<<nsp<<'\n';
          exit(1);
        } 
        tcfile >> wdivxl[div]; // read xl
        tcfile >> wdivxh[div]; // read xh
//
// <-- fill errays scp,ef1,ef2,rg1,rg2 for given division
//
        for(isp=0;isp<nsp;isp++){ // <--- sp. points loop to prepare arr. of t-distr
          tcfile >> iscp[isp];
          tcfile >> scp[isp];
          tcfile >> nft;   // for PM-1
          tcfile >> nb;
          tcfile >> bl;
          tcfile >> bw;
          tcfile >> ef1[isp];
          tcfile >> rg1[isp];
	  for(i=0;i<TOF2GC::PMTSMX;i++)tcfile >> ps1[isp][i];
          for(i=0;i<nb;i++){arr[i]=0.;}
          for(i=0;i<nb;i++){tcfile >> arr[i];}
          td1[isp]=TOFTpoints(nb,bl,bw,arr);
          tcfile >> nft;   // for PM-2
          tcfile >> nb;
          tcfile >> bl;
          tcfile >> bw;
          tcfile >> ef2[isp];
          tcfile >> rg2[isp];
	  for(i=0;i<TOF2GC::PMTSMX;i++)tcfile >> ps2[isp][i];
          for(i=0;i<nb;i++){arr[i]=0.;}
          for(i=0;i<nb;i++){tcfile >> arr[i];}
          td2[isp]=TOFTpoints(nb,bl,bw,arr);
//
        } // <--- end of scan points loop -----
//
        swdscan[div]=TOF2Scan(nsp,iscp,scp,ef1,ef2,rg1,rg2,ps1,ps2,td1,td2);// create single div. scan obj
//
      } // <--- end of divisions loop -----
//
      tcfile.close(); // close file
      scmcscan[brt]=TOFWScan(npmts,nwdivs,wdivxl,wdivxh,swdscan);// create complete scan obj
  } // --- end of bar-types loop --->
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
  integer id,idN,idd,ibar,ilay,ibtyp,cnum;
  static integer cnumo;
  static geant tslice1[TOF2GC::SCTBMX+1]; //  flash ADC array for side-1
  static geant tslice2[TOF2GC::SCTBMX+1]; //  flash ADC array for side-2
  static geant tslice[TOF2GC::SCTBMX+1]; //  flash ADC array
  geant bnw,bwid; 
  integer warr[TOFGC::AMSDISL]; 
  int i,i0,j,jj,ij,jm,k,stat(0),nelec,ierr(0),lbn,nbm;
  int status[2]={0,0};
  integer nhitl[TOF2GC::SCLRS];
  geant edepl[TOF2GC::SCLRS],edepb;
  geant dummy(-1);
  geant time,qtime,x,y,z,am,am0,am2,sig,r,rnd,eff,rgn,de,tm,eps(0.002);
  geant vl,vh;
  geant nel0,nel,nelb,nesig,nphot;
  geant shar1[TOF2GC::PMTSMX],shar2[TOF2GC::PMTSMX];
  geant wshar1[TOF2GC::PMTSMX],wshar2[TOF2GC::PMTSMX];
  AMSPoint cloc(999.,999.,999.);
  AMSgvolume *p;
  AMSTOFMCCluster *ptr;
  AMSTOFMCCluster *ptrN;
  int i1,i2,idivx,nwdivs,npmts;
  static geant bthick;
//
  static integer first=0;
  static integer npshbn;
  static geant pmplsh[1000];  // PM s.e. pulse shape
  static geant fadcb,ifadcb,trts,tmax,zlmx,convr,seres,sesig,sesave,sessig,sesrat;
//
  int ii,kk;
  char inum[11];
  char in[2]="0";
  char vname[5];
  strcpy(inum,"0123456789");
//
  if(first++==0){
    AMSmceventg::SaveSeeds();
//    AMSmceventg::PrintSeeds(cout);
    AMSmceventg::SetSeed(0);
//    cout <<" first in"<<endl;
    bthick=0.5*TOF2DBc::plnstr(6);//half bar-thickness
    cnumo=0;
    seres=TOF2DBc::seresp();
    sesig=seres*TOF2DBc::seresv();
    TOF2Tovt::inipsh(npshbn,pmplsh); // prepare PM sing.electron PulseShape array
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
    if(TFMCFFKEY.mcprtf[2]!=0)HPRINT(1099);
    am/=5000.;
    am2/=5000.;
    sesave=am;//asimptotic average(MP of "gaussized" sum of 5000 s.e.spectra)
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
    AMSmceventg::RestoreSeeds();
    
//    cout <<"first out "<<endl;
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
  for(i=0;i<TOF2GC::PMTSMX;i++){
    shar1[i]=0;
    shar2[i]=0;
    wshar1[i]=0;
    wshar2[i]=0;
  }
//
  edepb=0.;// Edep in given bar
  while(ptr){//       <------------- loop over geant hits
    id=ptr->idsoft; 
    ilay=id/100-1;
    nhitl[ilay]+=1;
    ibar=id%100-1;
    ibtyp=TOF2DBc::brtype(ilay,ibar)-1;//0->10
    de=ptr->edep;
    edepl[ilay]+=de;
    edepb+=de;
    time=(1.e+9)*(ptr->tof); // conv. to "ns"
    x=ptr->xcoo[0];
    y=ptr->xcoo[1];
    z=ptr->xcoo[2];
//  cout<<"MChit id="<<id<<" edep="<<de<<" /x/y/z="<<x<<" "<<y<<" "<<z<<endl;
    AMSPoint cglo(x,y,z);
    strcpy(vname,"TF");
    kk=ilay*TOF2GC::SCMXBR+ibar+1;//bar ID used in the volume name
    ii=kk/10;
    in[0]=inum[ii];
    strcat(vname,in);
    ii=kk%10;
    in[0]=inum[ii];
    strcat(vname,in);
    p=AMSJob::gethead()->
           getgeomvolume(AMSID(vname,id));// pointer to volume "TFnn",id
#ifdef __AMSDEBUG__
    assert(p != NULL);
#endif
    cloc=p->gl2loc(cglo);// convert global coord. to local
    x=cloc[0];
    y=cloc[1];
    z=cloc[2];
    TOF2JobStat::addmc(12);
//    HF1(1080,geant(time),1.);
    if((time>=tmax) || (time<0.)){
      TOF2JobStat::addmc(11);
//            cout<<"TOF: Bad G-hit, id="<<id<<" t/de="<<time<<" "<<de*1000<<'\n';
      ptr=ptr->next(); // take next hit
      continue;
    }
    qtime=time*ifadcb;
    cnum=TOF2DBc::barseqn(ilay,ibar);// solid sequential numbering(0->33)
    nwdivs=TOFWScan::scmcscan[ibtyp].getndivs();//real # of wdivs
    npmts=TOFWScan::scmcscan[ibtyp].getnpmts();//real # of pmts per side
    idivx=TOFWScan::scmcscan[ibtyp].getwbin(x);// x-div #
    if(idivx<0 || (fabs(z)-bthick)>0.01){
      if(cnum!=cnumo){//do not count repeated bad hits from the same volume  
#ifdef __AMSDEBUG__
        cout<<"---> TOF2Tovt::build: OutOfVolError:ID="<<id<<" cnum="<<cnum<<" Vol.name:"<<vname<<endl;
        if(idivx<0){
          cout<<"     Out-of-width hit !"<<endl;
          nbm=TOF2DBc::getbppl(ilay);
	  if(ibar==0 || ibar==(nbm-1))bwid=0.5*TOF2DBc::outcp(ilay,1);
	  else bwid=0.5*TOF2DBc::plnstr(5);                      
	  HF1(1078,geant(fabs(x)-bwid),1.);
        }
        if((fabs(z)-bthick)>0.01){
          cout<<"     Out-of-thickness hit !"<<endl;
	  HF1(1079,geant(fabs(z)-bthick),1.);
        }    
        cout<<"   xyzloc="<<x<<" "<<y<<" "<<z<<endl;
        cout<<"   xyzglo="<<cglo[0]<<" "<<cglo[1]<<" "<<cglo[2]<<" de="<<de<<endl;
        cout<<"   Evnum="<<GCFLAG.IEVENT<<endl;
#endif
        TOF2JobStat::addmc(3);
        cnumo=cnum;
      }
      ptr=ptr->next(); // take next hit
      continue;
    }
    TOFWScan::scmcscan[ibtyp].getybin(idivx,y,i1,i2,r);//y-bin #
    nel0=de*convr;// -> photons
//
// PM(side=1) actions --->
//
    eff=TOFWScan::scmcscan[ibtyp].getef1(idivx,r,i1,i2);//eff for PM-1
    rgn=TOFWScan::scmcscan[ibtyp].getgn1(idivx,r,i1,i2);//gain for PM-1
    for(i=0;i<npmts;i++){//to calc.later average(hit-energy-weighted) Dpms signals sharing
      shar1[i]+=(de*TOFWScan::scmcscan[ibtyp].getps1(i,idivx,r,i1,i2));//dyn.sharing
      wshar1[i]+=de;
    }
    nel=nel0*eff;// mean total number of photoelectrons
    POISSN(nel,nelec,ierr);// fluctuations
    for(i=0;i<TOFGC::AMSDISL;i++)warr[i]=0;
//    <-------- create phel. arrival-time distribution(PM-1) ---<<<
    for(i=0;i<nelec;i++){
      tm=TOFWScan::scmcscan[ibtyp].gettm1(idivx,r,i1,i2);//phel.arrival time from interpol.distr.
      uinteger ii=uinteger(floor(tm*ifadcb));
      if(ii<TOFGC::AMSDISL)warr[ii]+=1;
    }
    i0=integer(floor(qtime));// 1st bin pos. in abs. scale (for tslice1)
    if((qtime-geant(i0))>0.5)i0+=1;// to compensate(partially) binning influence
//    <-------- Loop over distr.bins ---<<<
    for(i=0;i<TOFGC::AMSDISL;i++){
      uinteger ii=i0+i;
      if(ii>TOF2GC::SCTBMX)break;//ignore "out of range"(above last=safety bin) 
      nelec=warr[i];
      if(nelec!=0){
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
//
// PM(side=2) actions --->
//
    eff=TOFWScan::scmcscan[ibtyp].getef2(idivx,r,i1,i2);//eff for PM-2
    rgn=TOFWScan::scmcscan[ibtyp].getgn2(idivx,r,i1,i2);//gain for PM-2
    for(i=0;i<npmts;i++){//to calc.later average(hit-energy-weighted) Dpms signals sharing
      shar2[i]+=(de*TOFWScan::scmcscan[ibtyp].getps2(i,idivx,r,i1,i2));//dyn.sharing
      wshar2[i]+=de;
    } 
    nel=nel0*eff;// mean total number of photoelectrons
    POISSN(nel,nelec,ierr);// fluctuations
    for(i=0;i<TOFGC::AMSDISL;i++)warr[i]=0;
//    <-------- create phel. arrival-time distribution(PM-2) ---<<<
    for(i=0;i<nelec;i++){
      tm=TOFWScan::scmcscan[ibtyp].gettm2(idivx,r,i1,i2);//phel.arrival time from interpol.distr.
      uinteger ii=uinteger(floor(tm*ifadcb));
      if(ii<TOFGC::AMSDISL)warr[ii]+=1;
    }
    i0=integer(floor(qtime));// 1st bin pos. in abs. scale (for tslice2)
    if((qtime-geant(i0))>0.5)i0+=1;// to compensate(partially) binning influence
//    <-------- Loop over distr.bins ---<<<
    for(i=0;i<TOFGC::AMSDISL;i++){
      uinteger ii=i0+i;
      if(ii>TOF2GC::SCTBMX)break;//ignore "out of range"
      nelec=warr[i];
      if(nelec!=0){
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
//-----------------------------------
//      ptrN=ptr->next();
//      idN=0; 
//      if(ptrN)idN=ptrN->getid();// id of the next G-hit
//      if(idN != id){ // <---- last or new bar -> create Tovt-objects :
//
      if(ptr->testlast()){ // <---- last or new bar -> create Tovt-objects :
        edepb*=1000.;// --->MeV
	if(edepb>0.)TOF2JobStat::addmc(4);//count fired bars
        if(edepb>TFMCFFKEY.Thr){// process only bar with Edep>Ethr
// PM(side=1) loop to apply pulse shape :
          am0=fabs(TFMCFFKEY.blshift*rnormx());//base line shift simulation
          idd=id*10+1;//LBBS
          for(i=0;i<TOF2GC::SCTBMX+1;i++)tslice[i]=am0;
          for(i=0;i<TOF2GC::SCTBMX;i++){
            am=tslice1[i];
            if(am>0){
	      for(j=0;j<npshbn;j++){
	        ij=i+j;
                if(ij>TOF2GC::SCTBMX)break;
                tslice[ij]+=am*pmplsh[j];
	      }
            }
          }        
          for(i=0;i<npmts;i++)shar1[i]/=wshar1[i];//calc. average Dpms-signals sharing
          TOF2Tovt::totovt(idd,edepb,tslice,shar1);// Tovt-obj for side(PM)-1
          if(TFMCFFKEY.mcprtf[1] != 0)
                        TOF2Tovt::displ_a(idd,20,tslice);//print PMT pulse
//        
// PM(side=2) loop to apply pulse shape :
          am0=fabs(TFMCFFKEY.blshift*rnormx());//base line shift simulation
          idd=id*10+2;
          for(i=0;i<TOF2GC::SCTBMX+1;i++)tslice[i]=am0;
          for(i=0;i<TOF2GC::SCTBMX;i++){
            am=tslice2[i];
            if(am>0){
	      for(j=0;j<npshbn;j++){
	        ij=i+j;
                if(ij>TOF2GC::SCTBMX)break;
                tslice[ij]+=am*pmplsh[j];
	      }
            }
          }        
          for(i=0;i<npmts;i++)shar2[i]/=wshar2[i];//calc. average Dpms-signals sharing
          TOF2Tovt::totovt(idd,edepb,tslice,shar2);// Tovt-obj for side(PM)-2
          if(TFMCFFKEY.mcprtf[1] != 0)
                        TOF2Tovt::displ_a(idd,20,tslice);//print PMT pulse
        }// ---> end of counter Edep check
//
        for(i=0;i<TOF2GC::SCTBMX+1;i++)tslice1[i]=0.;//clear flash ADC arrays for next bar
        for(i=0;i<TOF2GC::SCTBMX+1;i++)tslice2[i]=0.;
        edepb=0.;// clear Edep
        for(i=0;i<TOF2GC::PMTSMX;i++){//clear Dpms-sharing arrays
          shar1[i]=0;
          shar2[i]=0;
          wshar1[i]=0;
          wshar2[i]=0;
        }
      }// ---> end of next/last bar check
//
    ptr=ptr->next();
  }// ------ end of geant hits loop ---->
//
  if(TFMCFFKEY.mcprtf[2]!=0){
    for(i=0;i<TOF2GC::SCLRS;i++)HF1(1050+i,geant(nhitl[i]),1.);
    HF1(1060,1000.*edepl[0],1.);
    HF1(1061,1000.*edepl[1],1.);
    HF1(1062,1000.*edepl[2],1.);
    HF1(1063,1000.*edepl[3],1.);
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
  integer i,io,ib,lastiob(0);
  geant bl,bh,bol,boh,ao1,ao2,tgo,al,ah,tota;
  tota=0.;
  io=0;
  nbn=0;
  for(ib=0;ib<TOF2GC::SCTBMX;ib++){
    bl=ib*TOF2DBc::fladctb();
    bh=bl+TOF2DBc::fladctb();
    bol=pmpsb[io];
    boh=pmpsb[io+1];
    arr[ib]=0.;
    if(bl>=bol && bh<=boh){
      ao1=pmpsh[io];
      ao2=pmpsh[io+1];
      tgo=(ao2-ao1)/(boh-bol);
      al=ao1+tgo*(bl-bol);
      ah=ao1+tgo*(bh-bol);
      arr[ib]=(al+ah)/2.;
      tota+=arr[ib];
    }
    else if(bl<boh && bh>boh){
      ao1=pmpsh[io];
      ao2=pmpsh[io+1];
      tgo=(ao2-ao1)/(boh-bol);
      al=ao1+tgo*(bl-bol);
      io+=1;//next io-bin
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
      else{                 // last io-bin
        lastiob=1;
        ah=0.;
        arr[ib]=(al+ah)/2.;
        tota+=arr[ib];
	nbn=ib+1;
      }
    }
    else{//bl>=boh
      io+=1;//next io-bin
      if((io+1)<tbins)ib-=1;//to continue loop with the same ib
      else{                // last io-bin
        lastiob=1;
	nbn=ib;
      }
    }
    if(lastiob){//<-- last iobin test
      if(TFMCFFKEY.mcprtf[0] != 0){
        tota=tota*TOF2DBc::fladctb()/50.;
        printf("TOF-PM pulse shape:nbins=% 3d, Integral(pC) = %4.2e\n",nbn,tota);
        for(i=1;i<=nbn;i++){
          if(i%10 != 0)
                       printf("% 4.2e",arr[i-1]);
          else
                       printf("% 4.2e\n",arr[i-1]);
        }
        if(nbn%10 !=0)printf("\n");
      }
      return;
    }//--->endof last iobin test
  }//--->endof ib loop
//
  cerr<<"TOF2Tovt-inipulsh-Fatal: PM-pulse length exeeds Flash-ADC range !?"<<'\n';
  cerr<<"                 nbn="<<nbn<<endl;
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
    HBOOK1(1000,"PMT flash-ADC pulse (MC)",100,0.,100*tb,0.);
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
void TOF2Tovt::totovt(integer idd, geant edepb, geant tslice[], geant shar[])
{
  integer i,ii,j,ij,ilay,ibar,isid,id,_sta,stat(0);
  geant tm,a,am,am0,amd,tmp,amp,amx,amxq;
  geant iq0,it0,itau;
  integer _ntr1,_ntr2,_ntr3,_nftdc,_nstdc,_nadca,_nadcal,_nadcd,_nadcdl;
  number _ttr1[TOF2GC::SCTHMX1],_ttr2[TOF2GC::SCTHMX1],_ttr3[TOF2GC::SCTHMX1];
  number _tftdc[TOF2GC::SCTHMX2],_tftdcd[TOF2GC::SCTHMX2];
  number _tstdc[TOF2GC::SCTHMX3];
  number _adca,_adcal;
  number _adcd[TOF2GC::PMTSMX],_adcdl[TOF2GC::PMTSMX];
  number tovt,aqin;
  int upd1,upd2,upd3; // up/down flags for 3 fast discr. (z>=1;z>1;z>2)
  int upd11,upd21,upd22,upd31;
  int updsh;
  int imax,imin;
  geant a2dr[2],adc2q;
  static geant a2ds[2],dh2ls[2],adc2qs;
  geant tshd,tshup,charge,saturf;
  geant td1b1,td2b1,td2b2,td2b2d,td3b1;
  geant tmd1u,tmd1d,tmd2u,tmd2d,tmd3u,tmd3d,tbn,w,bo1,bo2,bn1,bn2,tmark;
  static integer first=0;
  static integer nshbn,mxcon,mxshc,mxshcg;
  static geant fladcb,shapb,cconv;
  geant daqt0,daqt1,daqt2,daqt3,daqt4,fdaqt0;
  static geant daqp0,daqp1,daqp2,daqp3,daqp4,daqp5,daqp6,daqp7,daqp8,daqp9,daqp10;
  static geant daqp11,daqp12;
  number adcs;
  int stackof;
//
  id=idd/10;// LBB
  isid=idd%10-1;// side
  ilay=id/100-1;
  ibar=id%100-1;
//
  if(first++==0){
    fladcb=TOF2DBc::fladctb();          //prepare some time-stable parameters
    shapb=TOF2DBc::shaptb();
    cconv=fladcb/50.; // for mV->pC (50 Ohm load)
    daqp0=TOF2DBc::daqpwd(0);// fixed duration of FT(Z>=1) pulse
    daqp1=TOF2DBc::daqpwd(1);// max "duration" of sTDC triang. pulse
    daqp2=TOF2DBc::daqpwd(2);// fixed duration of Z>2 pulse 
    daqp3=TOF2DBc::daqpwd(3);//dead time(dbl-pulse resol) for fast-TDC(history) signals 
    daqp4=TOF2DBc::daqpwd(4);//sTDC buzy time(from previous "up" till  possible next "up")
    daqp7=TOF2DBc::daqpwd(0)
         +TOF2DBc::daqpwd(7);// buzy time(pwid+dead) for z>=1(FT) branch of discr-2(HiThr)
    daqp8=TOF2DBc::daqpwd(8);//dead time of discr.itself(any discr)
    daqp9=TOF2DBc::daqpwd(2)
         +TOF2DBc::daqpwd(9);// buzy time(pwid+dead) for z>2 branch of discr-3(SHiThr) 
    daqp10=TOF2DBc::daqpwd(10);// intrinsic t-dispersion of comparator(on low thr.outp)
    daqp11=TOF2DBc::daqpwd(11);// min.duration time of discriminator pulse
  }
// time-dependent parameters(thresholds mainly) !!! :
  daqt0=TOF2Varp::tofvpar.daqthr(0); //discr-1 thresh(Low) for sTDC(stretcher) channel 
  fdaqt0=0.1*daqt0;// lowered threshold to select "working" part of pulse(m.b. loose some charge !!!)
  daqt1=TOF2Varp::tofvpar.daqthr(1); //discr-2 thresh(High) for FT(Z>=1)/History channel
  daqt2=TOF2Varp::tofvpar.daqthr(2);//discr-3 thresh(VeryHigh) for "Z>2" channel  
//
// -----> create/fill summary Tovt-object for idsoft=idd :
//
        if(tslice[TOF2GC::SCTBMX]>daqt0){//test last bin(i.e.ovfl) of flash-ADC
          TOF2JobStat::addmc(5);
//#ifdef __AMSDEBUG__
          cout<<"SITOFTovt-W: MC_flash-ADC range overflow, id="<<idd<<
             "  AmpLastBin(ovfl)= "<<tslice[TOF2GC::SCTBMX]<<'\n';
          if(TFMCFFKEY.mcprtf[2]>1)TOF2Tovt::displ_a(idd,100,tslice);//print PMT pulse
//#endif
        }
        for(i=TOF2GC::SCTBMX-1;i>0;i--){
	  imax=i;
          if(tslice[i]>fdaqt0 && tslice[i-1]>fdaqt0)break;//find high limit
	}
        imax=imax+1;
        for(i=0;i<TOF2GC::SCTBMX;i++){
	  imin=i;
	  if(tslice[i]>fdaqt0)break;//find low limit
	}
//
        charge=0.;
        _ntr1=0;
        _ntr2=0;
        _ntr3=0;
        _nftdc=0;
        _nstdc=0;
//
        upd1=0; //up/down flag for discr-1(LowThr, has 1 branch: sTDC)
	tmd1u=-9999.;//up-time of discr-1 
        tmd1d=-9999.;// down-time of discr-1
	upd11=0;//up/down flag for branche-1
        td1b1=-9999.;//branche-1 previous "up"-time
//
        upd2=0; //up/down flag for discr-2(HiThr, has 2 branches: FT/fTDC(Hist))
	tmd2u=-9999.;//up-time of discr-2 
        tmd2d=-9999.;// down-time of discr-2
        upd21=0;// up/down flag for FT(z>=1) branch of discr-2
        td2b1=-9999.;//branch-1 previous "up"-time
        upd22=0;// up/down flag for fTDC branch of discr-2
        td2b2=-9999.;//branch-2  "up"-time
        td2b2d=-9999.;//branch-2 previous "down"-time
//
        upd3=0;//up/down flag for discr-3(SHiThr, has 1 branch: Z>=2 trig)
	tmd3u=-9999.;//up-time of discr-3 
        tmd3d=-9999.;//down-time of discr-3
        upd31=0;// up/down flag for z>2 branch of discr.3 (dinode)
        td3b1=-9999.;// branch-1 previous up-time of  duscr-3(z>2)
//	 
//      _
//    _| |_
//___|     |___  PMT-pulse
//   ^tmp
//     ^tm
//
        stackof=0;//LIFO stack overfl.flag(for fTDC barnch)
        amx=0.;
        amp=0.;
        tmp=fladcb*imin;
        tm=tmp;
        for(i=imin;i<imax;i++){  //  <--- time bin loop for time measurements ---
          tm+=fladcb;
          am=tslice[i];
          if(am>amx)amx=am;//to find max amplitude
          am=am+TFMCFFKEY.hfnoise*rnormx();//tempor high freq. noise
          charge+=am;
//--------------------------          
// discr-1(anode comparator(LowThr) up/down setting for slowTDC(stretcher) branch:
          if(am>=daqt0){
            if(upd1 ==0 && (tm-tmd1d)>daqp8){// up-time( + dead-time check)
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
// discr-2( anode comparator(HiThr) up/down setting for FT(z>=1)/History branch  :
          if(am>=daqt1){
            if(upd2 ==0 && (tm-tmd2d)>daqp8){// up-time( + dead-time check)
              tmd2u=tm;// up time of discr.2 (FT/History)
              upd2=1;
            }
          }
          else{
            if(upd2!=0 && (tm-tmd2u)>daqp11){ // down time (+min.duration) check
              upd2=0;
              tmd2d=tm;
            }
          }
//----------
// discr-3( anode comparator(SHiThr) up/down setting for "z>=2" branch:
          if(am>=daqt2){
            if(upd3 ==0 && (tm-tmd3d)>daqp8){// up-time( + dead-time check)
              tmd3u=tm;// up time of discr.3 ("Z>=2")
              upd3=1;
            }
          }
          else{
            if(upd3!=0 && (tm-tmd3u)>daqp11){ // down time (+min.duration check)
              upd3=0;
              tmd3d=tm;
            }
          }
//----------
          amp=am;// store ampl to use as "previous" one in next i-loop
          tmp=tm;// .......
//
//-------------------------
//
// ===> try set branch-1 of discr-1  when it is up: 
          if(upd1>0){
//--------
// branch-1 "slow TDC logic" :        
            if(upd11==0){
              if((tmd1u-td1b1)>daqp4){ // "buzy" check for s-TDC channel
                upd11=1;  // set flag for branch s-TDC 
                td1b1=tm;//store up-time to use as previous in next "up" set
                if(_nstdc<TOF2GC::SCTHMX3){//store upto SCTHMX3(=4) up-edges
                  _tstdc[_nstdc]=tmark;//store precise(interpolated) up-time
                  _nstdc+=1;
                }
                else{
                  cerr<<"TOF2Tovt::build-W: sTDC ovfl(hits number) "<<_nstdc<<endl;
                  upd11=2;//blockade on overflow
                }
              } 
            }
          }//<-- end of discr-1 up-check
//--------
// ===> try set branch-1/2 of discr-2  when it is up: 
          if(upd2>0){
//
// branch-1 "z>=1(FT)" logic (with HIGH thresh.!!!) :
            if(upd21==0){
              if((tm-td2b1)>daqp7){//buzy time(pw+dead) check for z>=1(FT) signal
                upd21=1;  // set flag for branch z>=1
		td2b1=tm;//store up-time to use as previous in next "up" set
                if(_ntr1<TOF2GC::SCTHMX1){//store upto SCTHMX1(=8) up-edges
                  _ttr1[_ntr1]=tm;//don't need accurate up-time for trigger
                  _ntr1+=1;
                }
                else{
                  cerr<<"TOF2Tovt::build-W: FT ovfl(hits number)"<<_ntr1<<endl;
                  upd21=2;//blockade on overflow for safety
                }
              } 
            }
//        
// branch-2 "fast(history) TDC logic" :        
            if(upd22==0){
              if((tm-td2b2d)>daqp3){ //dead-time(=fTDC dbl.resol ?)-check
                upd22=1;  // set flag for branch fTDC
                td2b2=tm;//
              }
            }
          }//--->endof discr-2 up-check 
//--------
// ===> try set branch-1(Z>=2 trig) of discr-3  when it is up: 
          if(upd3>0){
            if(upd31==0){
              if((tm-td3b1)>daqp9){//buzy time check 
                upd31=1;  // set flag for branch z>=2
		td3b1=tm;//store up-time to use as previous in next "up" set
                if(_ntr3<TOF2GC::SCTHMX1){//store upto SCTHMX1(=8) up-edges
                  _ttr3[_ntr3]=tm;//up-time
                  _ntr3+=1;
                }
                else{
                  cerr<<"TOF2Tovt::build-W: Z>2 ovfl(hits number) "<<_ntr3<<endl;
                  upd31=2;//blockade on overflow  for safety
                }
              } 
            }
          }
//------------------------------------------
// try reset all branches:
//--------
// s-tdc
          if(upd11==1){ // sTDC self-reset in daqp1(ns) after prev."up"
            if((tm-td1b1)>daqp1)upd11=0;
          }
//--------
// "z>=1(FT)"
          if(upd21==1){// "z>=1 logic" self-reset(independ.of discr-2 state)
            if((tm-_ttr1[_ntr1-1])>daqp0){//min duration (fixed pulse width) check
              upd21=0;                    // for self-clear
            }
          }
//--------
// f-tdc 
          if(upd22==1){ 
            if(upd2 ==0 || i==imax-1){//"f-TDC" clear when discr-2 goes down, or out of time 
              upd22=0;
              td2b2d=tm;//store down-time to use it  for next "up" check
	      for(ii=_nftdc-1;ii>=0;ii--){//move up/down-times in the LIFO-stack
	        if(ii+1<TOF2GC::SCTHMX2){
	          _tftdc[ii+1]=_tftdc[ii];
	          _tftdcd[ii+1]=_tftdcd[ii];
	        }
	      }
	      _tftdc[0]=td2b2;//write latest(current) up-time
	      _tftdcd[0]=td2b2d;//write latest down-time
              if(_nftdc<TOF2GC::SCTHMX2)_nftdc+=1;//LIFO stack store upto SCTHMX2(=8) up(d)-edges
              else{
                if(stackof==0){//1st stack ovfl for given id
                  TOF2JobStat::addmc(9);
#ifdef __AMSDEBUG__
	        cout<<"TOF2Tovt::build-W: fTDC(hist) stack ovfl,id="<<idd<<endl;
#endif
	        }
	        stackof=1;
              }
            }//--->endof "clear ?" check
          }//--->endof "branch-2 is up" check
//--------
// Z>2
          if(upd31==1){// "z>2 logic" self-reset
            if((tm-_ttr3[_ntr3-1])>daqp2){//min duration(fixed pulse width) check
              upd31=0;                    // for self-clear
            }
          }
//---------------------------        
        } //      --- end of time bin loop for time measurements --->
//         
        charge=charge*fladcb/50.; // get total charge (pC)
        saturf=TOF2Tovt::pmsatur(charge);//true charge reduction fact. due to satur.
        if(TFMCFFKEY.mcprtf[2]!=0){
          if(idd==1041)HF1(1073,amx,1.);// ampl.spectrum for ref.bar
	  if(idd==1041)HF1(1070,float(charge),1.);
	}
//**************************************************************************
        number ped,sig,gain,eqs;
	geant ah2l[2],pmgn,dh2lr,sdh2l(0),dsum(0),asum(0),rrr;
	integer brtyp,npmts;
	brtyp=TOF2DBc::brtype(ilay,ibar)-1;//0-10
        npmts=TOFWScan::scmcscan[brtyp].getnpmts();//real # of pmts per side
        _adca=0;
        _adcal=0;
        _nadcd=0;
        _nadcdl=0;
        aqin=number(charge);// anode signal in pC (ref.bar !)
	aqin*=TOFBrcalMS::scbrcal[ilay][ibar].getagain(isid);//in current bars("seed" gains)
//anode(h):
	ped=TOFBPeds::scbrped[ilay][ibar].apeda(isid);// aver.ped in adc-chann. units(float)
	sig=TOFBPeds::scbrped[ilay][ibar].asiga(isid);// .... sig
        TOFBrcalMS::scbrcal[ilay][ibar].q2a2q(0,isid,0,adcs,aqin);// Qa(pC)->Ah(adc,float) 
	if(TFMCFFKEY.mcprtf[2]!=0)
	                          if(idd==1041)HF1(1074,float(adcs),1.);
        _adca=adcs+ped+sig*rnormx();// Ah adc+ped (float)
//
//anode(l):
	ped=TOFBPeds::scbrped[ilay][ibar].apedal(isid);// in adc-chann. units(float)
	sig=TOFBPeds::scbrped[ilay][ibar].asigal(isid);
        TOFBrcalMS::scbrcal[ilay][ibar].q2a2q(0,isid,1,adcs,aqin);// Qa(pC)->Al(adc,float) 
	if(TFMCFFKEY.mcprtf[2]!=0)
	                          if(idd==1041)HF1(1054,float(adcs),1.);
        _adcal=adcs+ped+sig*rnormx();// Al adc+ped (float)
//
//dynode(h):
        TOFBrcalMS::scbrcal[ilay][ibar].q2a2q(0,isid,2,eqs,aqin);//Qa(pC)->Dh(adc,equil.sum,float)
	if(TFMCFFKEY.mcprtf[2]!=0)
	                          if(idd==1041)HF1(1075,float(eqs/npmts),1.);
	for(int ipm=0;ipm<npmts;ipm++){
	  pmgn=TOFBrcalMS::scbrcal[ilay][ibar].getgnd(isid,ipm);//Dh(pm) rel.gain(wrt side aver)
	  adcs=eqs*shar[ipm]*pmgn;//Dh(eqs)->Dh(pm)
	  ped=TOFBPeds::scbrped[ilay][ibar].apedd(isid,ipm);
	  sig=TOFBPeds::scbrped[ilay][ibar].asigd(isid,ipm);
          _adcd[_nadcd]=adcs+ped+sig*rnormx();// Dh(pm) adc+ped (float)
          _nadcd+=1;//really it is number of PMTs/side because all adcd's accepted
	}
//dynode(l):
	for(int ipm=0;ipm<npmts;ipm++){
	  dh2lr=TOFBrcalMS::scbrcal[ilay][ibar].getdh2l(isid,ipm);//Dh/Dl ratio vs pm
	  sdh2l+=dh2lr;
	  adcs=eqs*shar[ipm]*pmgn/dh2lr;//Dh(eqs)->Dh(pm)->Dl(pm)
	  ped=TOFBPeds::scbrped[ilay][ibar].apeddl(isid,ipm);
	  sig=TOFBPeds::scbrped[ilay][ibar].asigdl(isid,ipm);
          _adcdl[_nadcdl]=adcs+ped+sig*rnormx();// Dl(pm) adc+ped (float))
          _nadcdl+=1;
	}
	eqs/=sdh2l;//average Dl(pm) for histogram
	if(TFMCFFKEY.mcprtf[2]!=0)
	                          if(idd==1041)HF1(1064,float(eqs),1.);
//
//------------------------------
// now create/fill TOF2Tovt object (id=idd) with above digi-data:
      if(_ntr1>0){// if counter_side FT-signal exists->create object 
        _sta=0;    
        stat=0;
        if(AMSEvent::gethead()->addnext(AMSID("TOF2Tovt",ilay), new
             TOF2Tovt(idd,_sta,charge,edepb,
             _ntr1,_ttr1,_ntr3,_ttr3,
             _nftdc,_tftdc,_tftdcd,_nstdc,_tstdc,
             _adca,_adcal,
	     _nadcd,_adcd,_nadcdl,_adcdl
	                                             )))stat=1;
      }
//
      return;
//
}
//
//---------------------------------------------------------------------
//
// function to get absolute time of the FIRST trigger(coincidence) "z>=1";
// trcode - trigger code (L-pattern) =-1/n, n>=0 -> MissingPlaneCode(=missing
// planeN if <=4), =-1 -> no trigger. AND create pattern also!
//
number TOF2Tovt::tr1time(int &trcode, uinteger toftrp[]){
  integer i1,i2,isd,isds(0),first(0);
  integer i,j,ilay,ibar,ntr,idd,id,idN,stat,intrig;
  uinteger sbt,lsbit(1);
  number ftime,ttr[TOF2GC::SCTHMX1];
  geant t1,t2;
  AMSBitstr trbs[2];
  AMSBitstr trbi;
  AMSBitstr trbl[TOF2GC::SCLRS];
  TOF2Tovt *ptr;
//
  geant trigb=TOF2DBc::trigtb();
  geant pwid=TOF2DBc::daqpwd(0);
  for(i=0;i<TOF2GC::SCLRS;i++)toftrp[i]=0;
//
  for(ilay=0;ilay<TOF2GC::SCLRS;ilay++){// <--- layers loop (Tovt containers) ---
    ptr=(TOF2Tovt*)AMSEvent::gethead()->
                               getheadC("TOF2Tovt",ilay,0);
    isds=0;
    trbl[ilay].bitclr(1,0);
    trbs[0].bitclr(1,0);
    trbs[1].bitclr(1,0);
    while(ptr){// <--- Tovt-hits loop in layer ---
      trbi.bitclr(1,0);
      idd=ptr->getid();//LBBS
      isd=idd%10-1;
      id=idd/10;// short id=LBB
      ibar=id%100-1;
      intrig=TriggerLVL302::TOFInFastTrigger(uinteger(ibar),uinteger(ilay));
      if(intrig==1 || (intrig>1 && (intrig-2)!=isd)){//<--bar/side in trigger(not masked)
//
        ntr=ptr->gettr1(ttr);// get number and times of trig1 ("z>=1")
        for(j=0;j<ntr;j++){// <--- trig-hits loop ---
          t1=geant(ttr[j]);
          t2=t1+pwid;
          i1=integer(t1/trigb);
          i2=integer(t2/trigb);
          if(i1>=TOFGC::SCBITM)i1=TOFGC::SCBITM-1;
          if(i2>=TOFGC::SCBITM)i2=TOFGC::SCBITM-1;
          trbi.bitset(i1,i2);//set bits according to hit-time and pulse width
	  trbs[isd]=trbs[isd] | trbi;//make side-OR
        }// --- end of trig-hits loop --->
//
      }//--> endof "in trig" check
//
      ptr=ptr->next();// take next Tovt-hit
// 
    }//--- end of Tovt-hits loop in layer --->
//
// Now create both side  bit pattern for CURRENT layer
    if(TGL1FFKEY.tofsc == 0)
                                trbl[ilay]=trbs[0] & trbs[1];// 2-sides AND
    else
                                trbl[ilay]=trbs[0] | trbs[1];// 2-sides OR
//
  } //--- end of layer loop --->
//
  trcode=-99;
  t1=0.;
  AMSBitstr coinc1234,rr1,rr2;
  rr1=trbl[0]&trbl[1];
  rr2=trbl[2]&trbl[3];
  coinc1234=rr1&rr2;
  coinc1234.testbit(i1,i2);
  if(i2>=i1){// found 4-fold coincidence
    trcode=0;
    t1=i1*trigb;
    goto Exit1;
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
    if(imin!=9999){ // found 3-fold coincidence
      t1=imin*trigb;
      goto Exit1;
    }
    else{ // check 2-fold coincidence:
      imin=9999;
      integer misl1,misl2;
      AMSBitstr coinc13=trbl[0]&trbl[2];
      AMSBitstr coinc14=trbl[0]&trbl[3];
      AMSBitstr coinc23=trbl[1]&trbl[2];
      AMSBitstr coinc24=trbl[1]&trbl[3];
      AMSBitstr coinc12=trbl[0]&trbl[1];
      coinc13.testbit(i1,i2);
      if((i2>=i1) && (i1<imin)){
        imin=i1;
        trcode=5;
	misl1=2;
	misl2=4;
      }
      coinc14.testbit(i1,i2);
      if((i2>=i1) && (i1<imin)){
        imin=i1;
        trcode=6;
	misl1=2;
	misl2=3;
      }
      coinc23.testbit(i1,i2);
      if((i2>=i1) && (i1<imin)){
        imin=i1;
        trcode=7;
	misl1=1;
	misl2=4;
      }
      coinc24.testbit(i1,i2);
      if((i2>=i1) && (i1<imin)){
        imin=i1;
        trcode=8;
	misl1=1;
	misl2=3;
      }
      coinc12.testbit(i1,i2);
      if((i2>=i1) && (i1<imin)){
        imin=i1;
        trcode=9;
	misl1=3;
	misl2=4;
      }
      if(imin!=9999){ // found 2-fold coincidence
        t1=imin*trigb;
        goto Exit1;
      }
    } // <--- end of 2-coinc. check
  } // <--- end of 3-coinc. check
Exit1:
  ftime=t1;
  if(trcode==-99)return ftime;//NoFT: fail of any request
  trcode=-trcode;//invert code
  if(TGL1FFKEY.toflc==1 && trcode<=-1)return ftime;//NoFT: fail of "4of4" request
  if(TGL1FFKEY.toflc==0 && trcode<=-5)return ftime;//NoFT: fail of "at least 3of4" request
  if(TGL1FFKEY.toflc==2 && trcode<-8)return ftime;//NoFT: fail of "at least 2of4" request  
  if(TGL1FFKEY.toflc==3 && trcode<-9)return ftime;//NoFT: fail of "at least 2top" request  
  trcode=-trcode;//back to normal(>=0)
//
  geant cgate=TOF2DBc::daqpwd(5);//gate for tof-pattern creation(z>=1)
  geant tg1=ftime+TOF2Varp::tofvpar.ftdelf();//gate_start_time=FTime+fix.delay
  geant tg2=tg1+cgate;//gate_end_time
  int ncoins;
// 
// ---> loop again over trig-hits to create trig.pattern:
//
  for(ilay=0;ilay<TOF2GC::SCLRS;ilay++){// <--- layers loop (Tovt containers) ---
    ptr=(TOF2Tovt*)AMSEvent::gethead()->
                               getheadC("TOF2Tovt",ilay,0);
    while(ptr){// <--- Tovt-hits loop in layer ---
      idd=ptr->getid();//LBBS
      isd=idd%10-1;
      id=idd/10;// short id=LBB
      ibar=id%100-1;
      ncoins=0;
      intrig=TriggerLVL302::TOFInFastTrigger(uinteger(ibar),uinteger(ilay));
      if(intrig==1 || (intrig>1 && (intrig-2)!=isd)){//bar/side in trigger(not masked)
        ntr=ptr->gettr1(ttr);// get number and times of trig1 ("z>=1")
        for(j=0;j<ntr;j++){// <--- trig-hits loop ---
          t1=geant(ttr[j]);
          t2=t1+pwid;
	  if(tg2<=t1 || tg1>=t2)continue;
	  ncoins+=1;
        }// --- end of trig-hits loop --->
	if(ncoins>0){
          if(isd==0)sbt=lsbit<<ibar;
	  else sbt=lsbit<<(16+ibar);
          toftrp[ilay]|=sbt;//add bit of coinsided side(toftrp:b(1-14)->s1;b(17-30)->s2)
	}
      }//endof "in trig" test
//  
      ptr=ptr->next();// take next Tovt-hit
// 
    }//         --- end of Tovt-hits loop in layer --->
//
  } //                               --- end of layer loop --->
//
  return ftime;
}
//---------------------------------------------------------------------
//
// function to get absolute time of the FIRST trigger(coincidence) "z>=2";
// trcode - trigger code (L-pattern) =-1/n, n>=0 -> MissingPlaneCode(=missing
// planeN if <=4), =-1 -> no trigger. It creates pattern also!
//
number TOF2Tovt::tr2time(int &trcode, uinteger toftrp[]){
  integer i1,i2,isd,isds(0),first(0);
  integer i,j,ilay,ibar,ntr,idd,id,idN,stat,intrig;
  uinteger sbt,lsbit(1);
  number ftime,ttr[TOF2GC::SCTHMX1];
  geant t1,t2;
  AMSBitstr trbs[2];
  AMSBitstr trbi;
  AMSBitstr trbl[TOF2GC::SCLRS];
  TOF2Tovt *ptr;
//
  geant trigb=TOF2DBc::trigtb();
  geant pwid=TOF2DBc::daqpwd(2);//pulse-width for top(bot)-layers coincidence
  geant pwext=TOF2DBc::daqpwd(13);//pulse-width for top/bot-sum coincidence
  for(i=0;i<TOF2GC::SCLRS;i++)toftrp[i]=0;
//
  for(ilay=0;ilay<TOF2GC::SCLRS;ilay++){// <--- layers loop (Tovt containers) ---
    ptr=(TOF2Tovt*)AMSEvent::gethead()->
                               getheadC("TOF2Tovt",ilay,0);
    isds=0;
    trbl[ilay].bitclr(1,0);
    trbs[0].bitclr(1,0);
    trbs[1].bitclr(1,0);
    while(ptr){// <--- Tovt-hits loop in layer ---
      trbi.bitclr(1,0);
      idd=ptr->getid();//LBBS
      isd=idd%10-1;
      id=idd/10;// short id=LBB
      ibar=id%100-1;
      intrig=TriggerLVL302::TOFInFastTrigger(uinteger(ibar),uinteger(ilay));
      if(intrig==1 || (intrig>1 && (intrig-2)!=isd)){//<--bar/side in trigger(not masked)
//
        ntr=ptr->gettr3(ttr);// get number and times of trig3 ("z>=2")
        for(j=0;j<ntr;j++){// <--- trig-hits loop ---
          t1=geant(ttr[j]);
          t2=t1+pwid;
          i1=integer(t1/trigb);
          i2=integer(t2/trigb);
          if(i1>=TOFGC::SCBITM)i1=TOFGC::SCBITM-1;
          if(i2>=TOFGC::SCBITM)i2=TOFGC::SCBITM-1;
          trbi.bitset(i1,i2);//set bits according to hit-time and pulse width
	  trbs[isd]=trbs[isd] | trbi;//make 1plane/1side-OR
        }// --- end of trig-hits loop --->
//
      }//--> endof "in trig" check
//
      ptr=ptr->next();// take next Tovt-hit
// 
    }//--- end of Tovt-hits loop in layer --->
//
// Now create both side  bit pattern for CURRENT layer
    if(TGL1FFKEY.tofsc == 0)
                                trbl[ilay]=trbs[0] & trbs[1];// 2-sides AND
    else
                                trbl[ilay]=trbs[0] | trbs[1];// 2-sides OR
//
  } //--- end of layer loop --->
//
  trcode=-99;
  t1=0.;
//             <---check 4-fold coincidence:
  AMSBitstr coinc1234,top,bot,etop,ebot;
  etop.bitclr(1,0);
  ebot.bitclr(1,0);
  top=trbl[0]&trbl[1];
  top.testbit(i1,i2);
  if(i2>=i1){
    t1=i1*trigb;
    t2=t1+pwext;
    i2=integer(t2/trigb);
    if(i2>=TOFGC::SCBITM)i2=TOFGC::SCBITM-1;
    etop.bitset(i1,i2);//extended top-coinc
  }
  bot=trbl[2]&trbl[3];
  bot.testbit(i1,i2);
  if(i2>=i1){
    t1=i1*trigb;
    t2=t1+pwext;
    i2=integer(t2/trigb);
    if(i2>=TOFGC::SCBITM)i2=TOFGC::SCBITM-1;
    ebot.bitset(i1,i2);//extended bot-coinc
  }
  coinc1234=etop&ebot;
  coinc1234.testbit(i1,i2);
  if(i2>=i1){// found 4-fold coincidence
    trcode=0;
    t1=i1*trigb;
    goto Exit1;
  }
//----
  else{   // <---check 3-fold coincidence:
//---0234c
    int imin=9999;
    top=trbl[1];
    etop.bitclr(1,0);
    top.testbit(i1,i2);
    if(i2>=i1){
      t1=i1*trigb;
      t2=t1+pwext;
      i2=integer(t2/trigb);
      if(i2>=TOFGC::SCBITM)i2=TOFGC::SCBITM-1;
      etop.bitset(i1,i2);//extended top-coinc
    }
    AMSBitstr coinc0234=etop&ebot;
    coinc0234.testbit(i1,i2);
    if((i2>=i1) && (i1<imin)){
      imin=i1;
      trcode=1;
    }
//---1034c
    top=trbl[0];
    etop.bitclr(1,0);
    top.testbit(i1,i2);
    if(i2>=i1){
      t1=i1*trigb;
      t2=t1+pwext;
      i2=integer(t2/trigb);
      if(i2>=TOFGC::SCBITM)i2=TOFGC::SCBITM-1;
      etop.bitset(i1,i2);//extended top-coinc
    }
    AMSBitstr coinc1034=etop&ebot;
    coinc1034.testbit(i1,i2);
    if((i2>=i1) && (i1<imin)){
      imin=i1;
      trcode=2;
    }
//---1204c
    top=trbl[0]&trbl[1];
    etop.bitclr(1,0);
    top.testbit(i1,i2);
    if(i2>=i1){
      t1=i1*trigb;
      t2=t1+pwext;
      i2=integer(t2/trigb);
      if(i2>=TOFGC::SCBITM)i2=TOFGC::SCBITM-1;
      etop.bitset(i1,i2);//extended top-coinc
    }
    bot=trbl[3];
    ebot.bitclr(1,0);
    bot.testbit(i1,i2);
    if(i2>=i1){
      t1=i1*trigb;
      t2=t1+pwext;
      i2=integer(t2/trigb);
      if(i2>=TOFGC::SCBITM)i2=TOFGC::SCBITM-1;
      ebot.bitset(i1,i2);//extended bot-coinc
    }
    AMSBitstr coinc1204=etop&ebot;
    coinc1204.testbit(i1,i2);
    if((i2>=i1) && (i1<imin)){
      imin=i1;
      trcode=3;
    }
//---1230c
    bot=trbl[2];
    ebot.bitclr(1,0);
    bot.testbit(i1,i2);
    if(i2>=i1){
      t1=i1*trigb;
      t2=t1+pwext;
      i2=integer(t2/trigb);
      if(i2>=TOFGC::SCBITM)i2=TOFGC::SCBITM-1;
      ebot.bitset(i1,i2);//extended bot-coinc
    }
    AMSBitstr coinc1230=etop&ebot;
    coinc1230.testbit(i1,i2);
    if((i2>=i1) && (i1<imin)){
      imin=i1;
      trcode=4;
    }
    if(imin!=9999){ // found 3-fold coincidence
      t1=imin*trigb;
      goto Exit1;
    }
//----
    else{ // <---check 2-fold coincidence:
      imin=9999;
      integer misl1,misl2;
//---13c
      top=trbl[0];
      etop.bitclr(1,0);
      top.testbit(i1,i2);
      if(i2>=i1){
        t1=i1*trigb;
        t2=t1+pwext;
        i2=integer(t2/trigb);
        if(i2>=TOFGC::SCBITM)i2=TOFGC::SCBITM-1;
        etop.bitset(i1,i2);//extended top-coinc
      }
      bot=trbl[2];
      ebot.bitclr(1,0);
      bot.testbit(i1,i2);
      if(i2>=i1){
        t1=i1*trigb;
        t2=t1+pwext;
        i2=integer(t2/trigb);
        if(i2>=TOFGC::SCBITM)i2=TOFGC::SCBITM-1;
        ebot.bitset(i1,i2);//extended bot-coinc
      }
      AMSBitstr coinc13=etop&ebot;
      coinc13.testbit(i1,i2);
      if((i2>=i1) && (i1<imin)){
        imin=i1;
        trcode=5;
	misl1=2;
	misl2=4;
      }
//---14c
      bot=trbl[3];
      ebot.bitclr(1,0);
      bot.testbit(i1,i2);
      if(i2>=i1){
        t1=i1*trigb;
        t2=t1+pwext;
        i2=integer(t2/trigb);
        if(i2>=TOFGC::SCBITM)i2=TOFGC::SCBITM-1;
        ebot.bitset(i1,i2);//extended bot-coinc
      }
      AMSBitstr coinc14=etop&ebot;
      coinc14.testbit(i1,i2);
      if((i2>=i1) && (i1<imin)){
        imin=i1;
        trcode=6;
	misl1=2;
	misl2=3;
      }
//---24c
      top=trbl[1];
      etop.bitclr(1,0);
      top.testbit(i1,i2);
      if(i2>=i1){
        t1=i1*trigb;
        t2=t1+pwext;
        i2=integer(t2/trigb);
        if(i2>=TOFGC::SCBITM)i2=TOFGC::SCBITM-1;
        etop.bitset(i1,i2);//extended top-coinc
      }
      AMSBitstr coinc24=etop&ebot;
      coinc24.testbit(i1,i2);
      if((i2>=i1) && (i1<imin)){
        imin=i1;
        trcode=8;
	misl1=1;
	misl2=3;
      }
//---23c
      bot=trbl[2];
      ebot.bitclr(1,0);
      bot.testbit(i1,i2);
      if(i2>=i1){
        t1=i1*trigb;
        t2=t1+pwext;
        i2=integer(t2/trigb);
        if(i2>=TOFGC::SCBITM)i2=TOFGC::SCBITM-1;
        ebot.bitset(i1,i2);//extended bot-coinc
      }
      AMSBitstr coinc23=etop&ebot;
      coinc23.testbit(i1,i2);
      if((i2>=i1) && (i1<imin)){
        imin=i1;
        trcode=7;
	misl1=1;
	misl2=4;
      }
      if(imin!=9999){ // found 2-fold coincidence
        t1=imin*trigb;
        goto Exit1;
      }
    } // <--- end of 2-coinc. check
  } // <--- end of 3-coinc. check
Exit1:
  ftime=t1;
  if(trcode==-99)return ftime;//NoFT: fail of any request
  trcode=-trcode;//invert code
  if(TGL1FFKEY.tfhzlc==1){//"top-AND" requested
    if(!(trcode==0 || trcode==-3 || trcode==-4))return ftime;//NoFT: fail of "top-AND" request
  }
  if(TGL1FFKEY.tfhzlc==2){//"bot-AND" requested
    if(!(trcode==0 || trcode==-1 || trcode==-2))return ftime;//NoFT: fail of "bot-AND" request
  }
  if(TGL1FFKEY.tfhzlc==3){//"top-AND && bot-AND" request
    if(trcode!=0)return ftime;//NoFT: fail of "4of4" request
  }  
  trcode=-trcode;//back to normal(>=0)
//
  geant cgate=TOF2DBc::daqpwd(6);//gate for tof-pattern creation(z>=2)
  geant tg1=ftime+TOF2Varp::tofvpar.ftdelf();//gate_start_time=FTime+fix.delay
  geant tg2=tg1+cgate;//gate_end_time
  int ncoins;
// 
// ---> loop again over trig-hits to create trig.pattern:
//
  for(ilay=0;ilay<TOF2GC::SCLRS;ilay++){// <--- layers loop (Tovt containers) ---
    ptr=(TOF2Tovt*)AMSEvent::gethead()->
                               getheadC("TOF2Tovt",ilay,0);
    while(ptr){// <--- Tovt-hits loop in layer ---
      idd=ptr->getid();//LBBS
      isd=idd%10-1;
      id=idd/10;// short id=LBB
      ibar=id%100-1;
      ncoins=0;
      intrig=TriggerLVL302::TOFInFastTrigger(uinteger(ibar),uinteger(ilay));
      if(intrig==1 || (intrig>1 && (intrig-2)!=isd)){//bar/side in trigger(not masked)
        ntr=ptr->gettr3(ttr);// get number and times of trig3 ("z>=2")
        for(j=0;j<ntr;j++){// <--- trig-hits loop ---
          t1=geant(ttr[j]);
          t2=t1+pwid;
	  if(tg2<=t1 || tg1>=t2)continue;
	  ncoins+=1;
        }// --- end of trig-hits loop --->
	if(ncoins>0){
          if(isd==0)sbt=lsbit<<ibar;
	  else sbt=lsbit<<(16+ibar);
          toftrp[ilay]|=sbt;//add bit of coinsided side(toftrp:b(1-14)->s1;b(17-30)->s2)
	}
      }//endof "in trig" test
//  
      ptr=ptr->next();// take next Tovt-hit
// 
    }//         --- end of Tovt-hits loop in layer --->
//
  } //                               --- end of layer loop --->
//
  return ftime;
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
  integer nftdc,nstdc,nadca,nadcal,nadcd,nadcdl;
  int16u _nftdc,_nstdc,_nadcd,_nadcdl,itt;
  number  tftdc[TOF2GC::SCTHMX2],tftdcd[TOF2GC::SCTHMX2],tstdc[TOF2GC::SCTHMX3];
  number tadca,tadcal;
  number tadcd[TOF2GC::PMTSMX],tadcdl[TOF2GC::PMTSMX];
  int16u  ftdc[TOF2GC::SCTHMX2*2],stdc[TOF2GC::SCTHMX3*4];
  int16u adca,adcal;
  int16u adcd[TOF2GC::PMTSMX],adcdl[TOF2GC::PMTSMX];
  number t,t1,t2,t3,t4,tprev,ttrig1,ttrig2,dt,tlev1,tl1d,ftrig,ecftrig;
  geant charge,edep,strr[2][2],str,offs;
  geant daqt1,rrr;
  number pedv,peds,amp;
  integer iamp;
  int trcode1,trcode2;
  integer trflag(0);
  uinteger ectrfl(0);
  uinteger trpatt1[TOF2GC::SCLRS]={0,0,0,0};
  uinteger trpatt2[TOF2GC::SCLRS]={0,0,0,0};
  integer it,it1,it2,it3,it4,it0;
  int16u phbit,maxv;
  static geant ftpw=TOF2DBc::daqpwd(12);// dummy gap in stretcher sequence (ns)
  TOF2Tovt *ptr;
  TOF2Tovt *ptrN;
  status=1;// bad
  phbit=TOF2GC::SCPHBP;
  maxv=phbit-1;//max possible TDC value (16383)
  daqt1=TOF2Varp::tofvpar.daqthr(3);//daq readout thr (ped sigmas)
//
  trflag=-1;
  TOF2RawEvent::settrfl(trflag);// reset  TOF-trigger flag
  TOF2RawEvent::setpatt(trpatt1);// reset  TOF-trigger pattern(z>=1)
  TOF2RawEvent::setpatt1(trpatt2);// reset  TOF-trigger pattern(z>=2)
//-----
  if(TGL1FFKEY.trtype<256){//<=== not external trigger
//
  ttrig1=TOF2Tovt::tr1time(trcode1,trpatt1);//get abs.trig1(FT"z>=1") signal time/patt
  if(TFMCFFKEY.mcprtf[2]!=0)HF1(1069,geant(trcode1),1.);
  if(trcode1>=0){// <---- use own(TOF) FT 
    status=0;
    trflag=trcode1;// ok: h/w trigger(z>=1) present -> do digitization:
    TOF2RawEvent::setpatt(trpatt1);// set trigger pattern(z>=1)
    if(TFMCFFKEY.mcprtf[2]!=0){
      for(int il=0;il<TOF2GC::SCLRS;il++){// pattern histogr
      for(int ib=0;ib<TOF2DBc::getbppl(il);ib++){
        if((trpatt1[il]&(1<<ib)) > 0)HF1(1065,geant(il*20+ib),1.);
        if((trpatt1[il]&(1<<(16+ib))) > 0)HF1(1066,geant(il*20+ib),1.);
      }
      }
    }    
//
    ttrig2=TOF2Tovt::tr2time(trcode2,trpatt2);//get abs.trig3("z>=2") signal time/patt
    if(trcode2>=0){
      if(TFMCFFKEY.mcprtf[2]!=0)HF1(1069,geant(trcode2)+10.,1.);
      trflag=10+trcode2;//mark z>=2
      TOF2RawEvent::setpatt1(trpatt2);// set trigger pattern(z>=2)
      if(TFMCFFKEY.mcprtf[2]!=0){
        for(int il=0;il<TOF2GC::SCLRS;il++){// pattern histogr
        for(int ib=0;ib<TOF2DBc::getbppl(il);ib++){
          if((trpatt2[il]&(1<<ib)) > 0)HF1(1067,geant(il*20+ib),1.);
          if((trpatt2[il]&(1<<(16+ib))) > 0)HF1(1068,geant(il*20+ib),1.);
        }
        }
      }    
      if(ttrig2<ttrig1){//should not be a real case, but not forbidden in principle(jitters) 
        ttrig1=ttrig2; // because FT z>=1 and z>=2 (+ECFT) are ORed on "TrigBox" output
      }
    }
//
    TOF2RawEvent::settrfl(trflag);// set final trigger flag
    ftrig=ttrig1+TOF2Varp::tofvpar.ftdelf();//FT abs time(+fixed delay) as it came to SFET from "TrBox"
    TOF2RawEvent::settrtime(ftrig);// store FTrigger time 
    tlev1=ftrig+TOF2DBc::accdel();// "common stop"-signal abs.time
    if(TFMCFFKEY.mcprtf[2]!=0){
      ectrfl=AMSEcalRawEvent::gettrfl();
      HF1(1076,geant(ectrfl),1.); 
      if(ectrfl>0){
        ecftrig=AMSEcalRawEvent::gettrtm();
	HF1(1077,geant(ftrig-ecftrig),1.);
      }
    }
  }
  else{// <---- have to use FT from ECAL(if any)
    ectrfl=AMSEcalRawEvent::gettrfl(); 
    if(ectrfl<=0)return;//no EC trigger also -> no chance to digitize TOF
    ftrig=AMSEcalRawEvent::gettrtm();//(includes delay = TOF-delay)
    tlev1=ftrig+TOF2DBc::accdel();// "common stop"-signal abs.time
  }
//
  }//===> endof "NotExtTrigger" check
//-----
  else{//<==== external trigger
    ttrig1=0.;//tempor (true ExtTrigSignal-time)
    ftrig=ttrig1+TOF2Varp::tofvpar.ftdelf();// FTrigger abs time (fixed delay added)
    tlev1=ftrig+TOF2DBc::accdel();// "common stop"-signal abs.time
  }
//-----  
//
  for(ilay=0;ilay<TOF2GC::SCLRS;ilay++){// <--- layers loop (Tovt containers) ---
    ptr=(TOF2Tovt*)AMSEvent::gethead()->
                               getheadC("TOF2Tovt",ilay,0);
    while(ptr){// <--- Tovt-hits loop in layer ---
      idd=ptr->getid();// LBBS
      id=idd/10;// short id=LBB
      ibar=id%100-1;
      TOFBrcalMS::scbrcal[ilay][ibar].gtstrat(strr);// get "MC Seed"str-ratios for two sides
      isd=idd%10-1;
      str=strr[isd][0];// str-ratio for given TovT-hit
      offs=strr[isd][1];// offset in str-ratio for given TovT-hit
      _sta=ptr->getstat();
      charge=ptr->getcharg();
      edep=ptr->getedep();
//
      nftdc=ptr->getftdc(tftdc,tftdcd);// get number and times of fast-TDC hits
      _nftdc=0;
    if(TOFBrcalMS::scbrcal[ilay][ibar].FchOK(isd)){//Fast-ch alive in "MC Seeds" DB
      tl1d=tlev1+TOF2Varp::tofvpar.sftdcd();// just to simulate sTDC delay wrt fTDC
      for(j=0;j<nftdc;j++){//        <--- ftdc-hits loop ---
        jj=nftdc-j-1;// LIFO readout mode (last stored - first read out)
        t1=tftdc[jj];
        t2=tftdcd[jj];
        dt=tl1d-t2;// follow LIFO mode of readout : down-edge - first hit
        it=integer(floor(dt/TOF2DBc::tdcbin(0))); // conv. to fast-TDC (history) t-binning
        if(it>maxv){
          cout<<"TOF2RawEvent_mc: warning : Hist-TDC overflow(down edge) !!!"<<'\n';
          it=maxv;
        }
        itt=int16u(it);
        if(!TOF2DBc::pbonup())itt=itt|phbit;//add phase bit if necessary
        ftdc[_nftdc]=itt;
        _nftdc+=1;
        dt=tl1d-t1;// follow LIFO mode of readout : leading(up) edge - second
        it=integer(floor(dt/TOF2DBc::tdcbin(0))); // conv. to fast-TDC (history) t-binning
        if(it>maxv){
          cout<<"TOF2RawEvent_mc: warning : Hist-TDC overflow(up edge) !!!"<<'\n';
          it=maxv;
        }
        itt=int16u(it);
        if(TOF2DBc::pbonup())itt=itt|phbit;//add phase bit if necessary
        ftdc[_nftdc]=itt;
        _nftdc+=1;
      }//--- end of ftdc-hits loop --->
    }//--> end of alive check
//------------
      nstdc=ptr->getstdc(tstdc);// get number and times of slow-TDC hits
      _nstdc=0;
    if(TOFBrcalMS::scbrcal[ilay][ibar].SchOK(isd)){//Slow-ch alive in "MC Seeds" DB
      for(j=0;j<nstdc;j++){//       <--- stdc-hits loop ---
        jj=nstdc-j-1;// LIFO readout mode (last stored - first read)
        t1=tstdc[jj]+rnormx()*TOF2DBc::strjit1();//"start"-signal time + jitter
        t2=ftrig+rnormx()*TOF2DBc::strjit2();//"stop"-signal = ftrig + jitter
        t3=t2+ftpw;//"dummy"-signal time (fixed delay = FT_coinc-pulse width)
        t4=t2+offs+(t2-t1)*str;//"end-mark"_signal + offs
        t4+=+rnormx()*TOF2DBc::strflu();// + end-mark fluct
        dt=ftrig-t1;// total FT-delay wrt t1
        if(dt<=TOF2DBc::ftdelm()){ // check max. delay of "FT" signal
          it0=integer(floor(tlev1/TOF2DBc::tdcbin(1)));//"C.stop" abs.time (integer)  
//follow LIFO mode of readout: stretcher "end-mark" edge - first hit; "start" - last:
//  prepare 4-th component("end-mark") of "4-edge" TOF-hit:
          it4=integer(floor(t4/TOF2DBc::tdcbin(1)));
          it=it0-it4;// time wrt "C.stop" signal
          if(it>maxv){
            TOF2JobStat::addmc(6);
#ifdef __AMSDEBUG__
            cout<<"TOF2RawEvent_mc-W: Stretcher-TDC overflow(4th edge) !!!"<<'\n';
#endif
            it=maxv;
          }
          itt=int16u(it);
          if(!TOF2DBc::pbonup())itt=itt|phbit;//add phase bit on down-edge,if necessary
          stdc[_nstdc]=itt;
          _nstdc+=1;
//  prepare 3-d component("dummy"):
          it3=integer(floor(t3/TOF2DBc::tdcbin(1)));
          it=it0-it3;// time wrt "C.stop" signal
          if(it>maxv){
            cout<<"TOF2RawEvent_mc-W: Stretcher-TDC overflow(3rd edge) !!!"<<'\n';
            it=maxv;
          }
          itt=int16u(it);
          if(TOF2DBc::pbonup())itt=itt|phbit;//add phase bit on up-edge,if necessary
          stdc[_nstdc]=itt;
          _nstdc+=1;
//  prepare 2-nd component("stop"):
          it2=integer(floor(t2/TOF2DBc::tdcbin(1)));
          it=it0-it2;// time wrt "C.stop" signal
          if(it>maxv){
            cout<<"TOF2RawEvent_mc-W: Stretcher-TDC overflow(2nd edge) !!!"<<'\n';
            it=maxv;
          }
          itt=int16u(it);
          if(!TOF2DBc::pbonup())itt=itt|phbit;//add phase bit on down-edge,if necessary
          stdc[_nstdc]=itt;
          _nstdc+=1;
//  prepare 1-st component("start") of "4-edge" TOF-hit:
          it1=integer(floor(t1/TOF2DBc::tdcbin(1)));
          it=it0-it1;// time wrt "C.stop" signal
          if(it>maxv){
            cout<<"TOF2RawEvent_mc-W: Stretcher-TDC overflow(1st edge) !!!"<<'\n';
            it=maxv;
          }
          itt=int16u(it);
          if(TOF2DBc::pbonup())itt=itt|phbit;//add phase bit on up-edge,if necessary
          stdc[_nstdc]=itt;
          _nstdc+=1;
        }//--endof max.delay check-->
      }//--- end of stdc-hits loop --->
    }//--> end of alive check
//----------------
//anode(h):
    adca=0;
    if(TOFBrcalMS::scbrcal[ilay][ibar].AHchOK(isd)){//A(h)-ch alive in "MC Seeds" DB
      pedv=TOFBPeds::scbrped[ilay][ibar].apeda(isd);
      peds=TOFBPeds::scbrped[ilay][ibar].asiga(isd);
      tadca=ptr->getadca();//get ADC-counts of A(h)
      amp=tadca;// here charge is quantized by "adc2q" but not "integerized"
      iamp=integer(floor(amp));//go to real ADC-channels("integerization")
      if(iamp>TOF2GC::SCADCMX){//A(h)overflow - not transmitted according Diego
        TOF2JobStat::addmc(7);
#ifdef __AMSDEBUG__
        cout<<"TOF2RawEvent_mc-W: Anode(h)-ADC overflow,id="<<idd<<'\n';
#endif
      }
      else{
	amp=number(iamp)-pedv;// subtract pedestal (loose "integerization" !)
//cout<<"    Ah: ped/sig="<<pedv<<" "<<peds<<" iamp/-ped="<<iamp<<" "<<amp<<endl;
	if(amp>daqt1*peds){// DAQ readout threshold
	  iamp=floor(amp/TOF2DBc::tdcbin(2)+0.5);// floatADC -> int DAQ bins
          itt=int16u(iamp);
          adca=itt;
	}
      }
//cout<<"      Ah: adca="<<adca<<endl;
    }//--> end of alive check
//----------------
//anode(l):
    adcal=0;
    if(TOFBrcalMS::scbrcal[ilay][ibar].ALchOK(isd)){//A(l)-ch alive in "MC Seeds" DB
      pedv=TOFBPeds::scbrped[ilay][ibar].apedal(isd);
      peds=TOFBPeds::scbrped[ilay][ibar].asigal(isd);
      tadcal=ptr->getadcal();//get ADC-counts of A(h)
      amp=tadcal;// here charge is quantized by "adc2q" but not "integerized"
      iamp=integer(floor(amp));//go to real ADC-channels("integerization")
      if(iamp>TOF2GC::SCADCMX){
        TOF2JobStat::addmc(13);
#ifdef __AMSDEBUG__
        cout<<"TOF2RawEvent_mc-W: Anode-ADC overflow,id="<<idd<<'\n';
#endif
        iamp=TOF2GC::SCADCMX;//A(l)-ovfl is always transmitted according to Diego
      }
      amp=number(iamp)-pedv;// subtract pedestal (loose "integerization" !)
//cout<<"    Al: ped/sig="<<pedv<<" "<<peds<<" iamp/-ped="<<iamp<<" "<<amp<<endl;
      if(amp>daqt1*peds){// DAQ readout threshold 
	iamp=floor(amp/TOF2DBc::tdcbin(2)+0.5);// floatADC -> int DAQ bins
        itt=int16u(iamp);
        adcal=itt;
      }
//cout<<"      Al: adcal="<<adcal<<endl;
    }//--> end of alive check
//--------------------
//dynode(h):
    _nadcd=0;
    nadcd=ptr->getadcd(tadcd);// get number of D(h)-pmts(upto PMTSMX) and its ADCs(incl=0)
    for(j=0;j<nadcd;j++){// <--- D(h)-pmts loop ---
      adcd[j]=0;
      if(TOFBrcalMS::scbrcal[ilay][ibar].DHchOK(isd,j)){//D(h)-PMch alive in "MC Seeds" DB
	amp=tadcd[j];// here charge is quantized by "adc2q" but not "integerized"
        iamp=integer(floor(amp));//go to real ADC-channels("integerization")
        if(iamp>TOF2GC::SCADCMX){
          TOF2JobStat::addmc(8);
#ifdef __AMSDEBUG__
          cout<<"TOF2RawEvent::mc_build warning: D(h) ADC overflow,id="<<idd<<'\n';
#endif
        }
	else{//only non-ovfl are transmitted according Diego
          pedv=TOFBPeds::scbrped[ilay][ibar].apedd(isd,j);
          peds=TOFBPeds::scbrped[ilay][ibar].asigd(isd,j);
	  amp=number(iamp)-pedv;// subtract pedestal (loose "integerization" !)
//cout<<"    Dh: pm/ped/sig="<<j<<" "<<pedv<<" "<<peds<<" iamp/-ped="<<iamp<<" "<<amp<<endl;
	  if(amp>daqt1*peds){// DAQ readout threshold
	    iamp=floor(amp/TOF2DBc::tdcbin(2)+0.5);// floatADC -> DAQ bins
            itt=int16u(iamp);
            adcd[j]=itt;
            _nadcd+=1;//number of nonzero(>thr) pmts !!!
	  }
	}
      }//--> end of alive check
//cout<<"       adcd="<<adcd[j]<<endl;
    }//--- end of D(h)-pm loop --->
//---------------------
//dynode(l):
    _nadcdl=0;
    nadcdl=ptr->getadcdl(tadcdl);// get number of D(l)-pmts(upto PMTSMX) and its ADCs
    for(j=0;j<nadcdl;j++){// <--- D(l)-pmts loop ---
      adcdl[j]=0;
      if(TOFBrcalMS::scbrcal[ilay][ibar].DLchOK(isd,j)){//D(l)-ch alive in "MC Seeds" DB
	amp=tadcdl[j];// here charge is quantized by "adc2q" but not "integerized"
        iamp=integer(floor(amp));//go to real ADC-channels("integerization")
        if(iamp>TOF2GC::SCADCMX){
          TOF2JobStat::addmc(10);
#ifdef __AMSDEBUG__
          cout<<"TOF2RawEvent::mc_build warning: D(l) ADC overflow,id="<<idd<<'\n';
#endif
          iamp=TOF2GC::SCADCMX;//D(l)-ovfl is always transmitted according to Diego
        }
        pedv=TOFBPeds::scbrped[ilay][ibar].apeddl(isd,j);
        peds=TOFBPeds::scbrped[ilay][ibar].asigdl(isd,j);
	amp=number(iamp)-pedv;// subtract pedestal (loose "integerization" !)
//cout<<"    Dl: pm/ped/sig="<<j<<" "<<pedv<<" "<<peds<<" iamp/-ped="<<iamp<<" "<<amp<<endl;
	if(amp>daqt1*peds){// DAQ readout threshold
	  iamp=floor(amp/TOF2DBc::tdcbin(2)+0.5);// floatADC -> DAQ bins
          itt=int16u(iamp);
          adcdl[j]=itt;
          _nadcdl+=1;
	}
      }//--> end of alive check
//cout<<"       adcdl="<<adcdl[j]<<endl;
    }//--- end of D(l)-pm loop --->
//---------------------
//     ---> create/fill RawEvent-object :
      stat=0;
      if(AMSEvent::gethead()->addnext(AMSID("TOF2RawEvent",0), new
           TOF2RawEvent(idd,_sta,charge,edep,_nftdc,ftdc,_nstdc,stdc,
                                                        adca,adcal,
			                                _nadcd,adcd,
			                                _nadcdl,adcdl
			                                             )))stat=1;
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
  number _adca,
  number _adcal,
  integer _nadcd, number _adcd[],
  integer _nadcdl, number _adcdl[]):
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
    adca=_adca;
    adcal=_adcal;
    nadcd=_nadcd;
    for(i=0;i<nadcd;i++){
      adcd[i]=_adcd[i];
    }
    nadcdl=_nadcdl;
    for(i=0;i<nadcdl;i++){
      adcdl[i]=_adcdl[i];
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
number TOF2Tovt::getadca(){
  return adca;
}
number TOF2Tovt::getadcal(){
  return adcal;
}
integer TOF2Tovt::getadcd(number arr[]){
  for(int i=0;i<nadcd;i++){
    arr[i]=adcd[i];
  }
  return nadcd;
}
integer TOF2Tovt::getadcdl(number arr[]){
  for(int i=0;i<nadcdl;i++){
    arr[i]=adcdl[i];
  }
  return nadcdl;
}





TOF2RawEvent::TOF2RawEvent(int16u _ids, int16u _sta, geant _charge, geant _edep,
   int16u _nftdc, int16u _ftdc[],
   int16u _nstdc, int16u _stdc[],
   int16u _adca, int16u _adcal,
   int16u _nadcd, int16u _adcd[],
   int16u _nadcdl, int16u _adcdl[]
   ):idsoft(_ids),status(_sta)
   {
     int i;
     nftdc=_nftdc;
     for(i=0;i<nftdc;i++)ftdc[i]=_ftdc[i];
     nstdc=_nstdc;
     for(i=0;i<nstdc;i++)stdc[i]=_stdc[i];
     adca=_adca;
     adcal=_adcal;
     nadcd=_nadcd;
     nadcdl=_nadcdl;
     for(i=0;i<TOF2GC::PMTSMX;i++){
       adcd[i]=_adcd[i];
       adcdl[i]=_adcdl[i];
     }
     charge=_charge;
     edep=_edep;
   }

integer TOF2RawEvent::getnztdc(){
  integer nz(0);
  if(nftdc>0)nz+=1;
  if(nstdc>0)nz+=1;
  if(adca>0)nz+=1;
  if(adcal>0)nz+=1;
  if(nadcd>0)nz+=nadcd;
  if(nadcdl>0)nz+=nadcdl;
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


int16u TOF2RawEvent::getadcd(int16u arr[]){
  for(int i=0;i<TOF2GC::PMTSMX;i++)arr[i]=adcd[i];
  return nadcd;
}

void TOF2RawEvent::putadcd(int16u nelem, int16u arr[]){
  nadcd=nelem;
  for(int i=0;i<nadcd;i++)adcd[i]=arr[i];
}

int16u TOF2RawEvent::getadcdl(int16u arr[]){
  for(int i=0;i<TOF2GC::PMTSMX;i++)arr[i]=adcdl[i];
  return nadcdl;
}

void TOF2RawEvent::putadcdl(int16u nelem, int16u arr[]){
  nadcdl=nelem;
  for(int i=0;i<nadcdl;i++)adcdl[i]=arr[i];
}
//--------------
integer TOF2RawEvent::lvl3format(int16 *ptr, integer rest){
  integer ilay,ibar,isid;
  int i,j,nrwt;
  int statdb[2];
  int16u pbitn,pbanti,pbup,pbdn,pbup1,pbdn1,hwid,crat,otyp(0),mtyp(1);
  int16u id,rwt[10],rwtn[10];
  int16 rawt;
//
  pbitn=TOF2GC::SCPHBP;//phase bit position
  pbanti=pbitn-1;// mask to avoid it.
  id=idsoft/10;// short id=LBB, where L=1,4 BB=1,12
  ilay=id/100-1;
  ibar=id%100-1;
  isid=idsoft%10-1;
  AMSSCIds tofid(ilay,ibar,isid,otyp,mtyp);//mtyp=0 to find crate/slot for needed slowTDC
  nrwt=0;
  if(TOF2Brcal::scbrcal[ilay][ibar].SideOK(isid)){ // side OK(F&S&A), read out it
    hwid=tofid.gethwid();//CSRR(Crate|Slot|Readout_channel)
    crat=hwid/1000-1;
    for(i=0;i<nstdc-3;i++){// <--- find all correct 4's of up/down bits settings
      pbdn=(stdc[i]&pbitn);//check p-bit of 2-nd down-edge (come first, LIFO mode !!!)
      pbup=(stdc[i+1]&pbitn);//check p-bit of 2-nd up-edge (come second)
      pbdn1=(stdc[i+2]&pbitn);//check p-bit of 1-st down-edge (come third)
      pbup1=(stdc[i+3]&pbitn);//check p-bit of 1-st up-edge (come fourth)
      if(TOF2DBc::pbonup()==1){
        if(pbup==0||pbup1==0||pbdn!=0||pbdn1!=0)continue;//wrong sequence, take next "4" 
      }
      else{
        if(pbup!=0||pbup1!=0||pbdn==0||pbdn1==0)continue;//wrong  sequence, take next "4" 
      }
      if(nrwt<10){
        rwt[nrwt]=(stdc[i+3]&pbanti);//1-st (up)-edge (in real time)
        rwtn[nrwt]=(stdc[i+2]&pbanti);//2nd (down=FT)-edge (in real time)
      }
      nrwt+=1;
//
      i+=3;// to go to next 4 good edges
    }//--->endof correct 4's of up/down bits settings search
//---  
  }// --->endof DBstat check
//---
  if(nrwt>0){
    if (rest < 3) return 0;
    rawt=rwt[0]-rwtn[0];//t1-t2(FT) from last(real time) "4's"
    *(ptr)=hwid;
    *(ptr+1)=idsoft;
    *(ptr+2)=rawt;
    return 3; 
  }
  else return 0; 
}










