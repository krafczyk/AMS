// last modif. 10.12.96 by E.Choumilov - AMSTOFRawCluster::build added, 
//                                       AMSTOFCluster::build modified.
#include <point.h>
#include <event.h>
#include <amsgobj.h>
#include <commons.h>
#include <cern.h>
#include <mccluster.h>
#include <math.h>
#include <extC.h>
#include <tofdbc.h>
#include <tofsim.h>
#include <trrec.h>
#include <tofrec.h>
#include <tofcalib.h>
#include <ntuple.h>
//
extern TOFBrcal scbrcal[SCLRS][SCMXBR];// calibration data
extern TOFVarp tofvpar;// TOF general parameters
//
//
void AMSTOFRawCluster::sitofdigi(){
  AMSTOFMCCluster * ptr=(AMSTOFMCCluster*)
  AMSEvent::gethead()->
   getheadC("AMSTOFMCCluster",0,1); // last 1  to test sorted container
  static number xplane[4][SCMXBR];
  static number xtime[4][SCMXBR];
  static number xtimed[4][SCMXBR];
  static number xz[4][SCMXBR];
  integer plrot;
  number ama[2],amd[2],stm[2],edepd,etd;
  VZERO(xtime,SCMXBR*sizeof(number));
  VZERO(xtimed,SCMXBR*sizeof(number));
  VZERO(xplane,SCMXBR*sizeof(number));
  VZERO(xz,SCMXBR*sizeof(number));
  geant x,y;
  while(ptr){
   integer ntof,plane,status;
   ntof=(ptr->idsoft)/100-1;//ilay
   plane=(ptr->idsoft)%100-1;//ibar
#ifdef __AMSDEBUG__
   assert(plane>=0 && plane< SCMXBR);
   assert (ntof>=0 && ntof<4);
#endif
   xplane[ntof][plane]+=ptr->edep*1000;
   xtime[ntof][plane]+=ptr->tof*(ptr->edep)*1000;//tof*edep
   plrot=TOFDBc::plrotm(ntof);     // =0/1-unrotated/rotated TOF-plane
   integer ixy=0;
   if(plrot==0)ixy=1;
   xtimed[ntof][plane]+=ptr->xcoo[ixy]*(ptr->edep)*1000;//long_coo*edep
   xz[ntof][plane]=ptr->xcoo[2];
   status=0;
   integer last;
   last=ptr->testlast();
   ptr=ptr->next();  
   
  }
//
//
  for(integer kk=0;kk<SCLRS;kk++){
  for(integer i=0;i<SCMXBR;i++){
    if(xplane[kk][i]>TOFMCFFKEY.Thr){
     xtime[kk][i]=xtime[kk][i]/xplane[kk][i];
     xtimed[kk][i]=xtimed[kk][i]/xplane[kk][i];
     geant dummy(-1);
     const number c=1.7e10;    // eff. speed of light in scint.
     number ts1,ts2;
     if(RNDM(dummy)< TOFMCFFKEY.TimeProbability2){
       ts1=TOFMCFFKEY.TimeSigma2*rnormx();
       ts2=TOFMCFFKEY.TimeSigma2*rnormx();
       etd=TOFMCFFKEY.TimeSigma2*c*0.707;//err. on long.coord.
     }
     else {
       ts1=TOFMCFFKEY.TimeSigma*rnormx();
       ts2=TOFMCFFKEY.TimeSigma*rnormx();
       etd=TOFMCFFKEY.TimeSigma*c*0.707;
     }
     xtime[kk][i]+=(ts1+ts2)/2.;// add  time-error
     xtimed[kk][i]+=(ts1-ts2)/2.*c; // add  coordinate-error
//
     plrot=TOFDBc::plrotm(kk);
     if(plrot==1)xtimed[kk][i]=-xtimed[kk][i];//to agree with slow algorithm
     xtime[kk][i]=-(1.e9)*xtime[kk][i];//sec->-ns to agree with slow algorithm
     ama[0]=0.;
     ama[1]=0.;
     amd[0]=0.;
     amd[1]=0.;
     stm[0]=0.;
     stm[1]=0.;
     edepd=0.;
     AMSEvent::gethead()->addnext(AMSID("AMSTOFRawCluster",0),
     new AMSTOFRawCluster(0,kk+1,i+1,xz[kk][i],ama,amd,xplane[kk][i],
         edepd,stm,xtime[kk][i],xtimed[kk][i],etd));
    }

  }
  }
}
//------
void AMSTOFRawCluster::build(int &status){
  int16u nftdc[2],nstdc[2],nadca[2],nadcd[2];
  int16u  ftdc1[SCTHMX2],stdc1[SCTHMX3],adca1[SCTHMX4],adcd1[SCTHMX4];
  int16u  ftdc2[SCTHMX2],stdc2[SCTHMX3],adca2[SCTHMX4],adcd2[SCTHMX4];
  integer ilay,last,ibar,isid,isds;
  integer i,j,chnum,brnum,am[2],tmi[2],itmf[2],sta,st;
  int16u id,idd,idN,stat[2];
  number zc,ama[2],amd[2],qtota,qtotd,tmf[2],time,coo,ecoo;//input to RawCluster Constr
  number tm[2],tf,dt,fstd;
  number amf[2],timeD,strr,tamp;
  number charg[2];
  geant co,eco,point,brlm,pcorr,td2p,etd2p,clong[SCLRS];
  AMSTOFRawEvent *ptr;
  AMSTOFRawEvent *ptrN;
  integer nbrl[SCLRS],brnl[SCLRS];
  int bad;
// some variables for histogramming:
  geant tch,pch1[SCLRS],pch2[SCLRS];
  geant edepa[SCLRS],edepd[SCLRS],tcorr[SCLRS],tdiff[SCLRS];
//
  ptr=(AMSTOFRawEvent*)AMSEvent::gethead()
                                    ->getheadC("AMSTOFRawEvent",0);
  status=1;// bad
  isds=0;
  for(i=0;i<SCLRS;i++)nbrl[i]=0;
//
//                             <---- loop over TOF RawEvent hits -----
  while(ptr){
    idd=ptr->getid();
    id=idd/10;// short id=LBB, where L=1,4 BB=1,14
    ilay=id/100-1;
    ibar=id%100-1;
    isid=idd%10-1;
//    edep=ptr->getedep();
    chnum=ilay*SCMXBR*2+ibar*2+isid;//channels numbering
    brnum=ilay*SCMXBR+ibar;//bar numbering
    stat[isid]=ptr->getstat();
    if(stat[isid] == 0){  // channel alive
//       fill working arrays for given side:
      isds+=1;
      if(isid==0){
        charg[isid]=ptr->getcharg();
        nftdc[isid]=ptr->getftdc(ftdc1);
        nstdc[isid]=ptr->getstdc(stdc1);
        nadca[isid]=ptr->getadca(adca1);
        nadcd[isid]=ptr->getadcd(adcd1);
      } 
      if(isid==1){
        charg[isid]=ptr->getcharg();
        nftdc[isid]=ptr->getftdc(ftdc2);
        nstdc[isid]=ptr->getstdc(stdc2);
        nadca[isid]=ptr->getadca(adca2);
        nadcd[isid]=ptr->getadcd(adcd2);
      }
      TOFJobStat::addch(chnum,0);        // channel statistics :
      if(nftdc[isid]>0)TOFJobStat::addch(chnum,1);
      if(nstdc[isid]>0)TOFJobStat::addch(chnum,2);
      if(nadca[isid]>0)TOFJobStat::addch(chnum,3);
      if(nadcd[isid]>0)TOFJobStat::addch(chnum,4);
      if(nftdc[isid]==1)TOFJobStat::addch(chnum,5);
      if(nstdc[isid]==1)TOFJobStat::addch(chnum,6);
      if(nadca[isid]==1)TOFJobStat::addch(chnum,7);
      if(nadcd[isid]==1)TOFJobStat::addch(chnum,8);
    } 
//
    ptrN=ptr->next();
    idN=0;
    if(ptrN)idN=(ptrN->getid())/10; //next hit short_id
//------------------------------------------------------
    if(idN != id){ // both sides ready, next hit is OTHER_counter/last hit,
//       so process CURRENT counter data : 
//
      if(isds==2){//bar(2sides) h/w status ok-start pattern recognition in it
        isds=0;
        TOFJobStat::addbr(brnum,0);
// --------> select bar with 1-multiplicity in stdc/adca :
        if(nstdc[0]==1 && nstdc[1]==1
          && nadca[0]==1 && nadca[1]==1
          && nftdc[0] >0 && nftdc[1] >0){
          TOFJobStat::addbr(brnum,1);
//   --------> identify "corresponding" hit in fast TDC :
          tm[0]=stdc1[0]*TOFDBc::tdcbin(1)/scbrcal[ilay][ibar].gtstrat();
          tm[1]=stdc2[0]*TOFDBc::tdcbin(1)/scbrcal[ilay][ibar].gtstrat();
          tmf[0]=-1.;
          tmf[1]=-1.;
          if(TOFRECFFKEY.reprtf[2]>0 && nftdc[0] ==1 && nftdc[1]==1){// histogr.
            tf=ftdc1[0]*TOFDBc::tdcbin(0);
            dt=tf-tm[0];
            HF1(1100,dt,1.);
            tf=ftdc2[0]*TOFDBc::tdcbin(0);
            dt=tf-tm[1];
            HF1(1100,dt,1.);
          }
          fstd=scbrcal[ilay][ibar].gtfstrd(); // diff. in f/s trig. delay
          for(i=0;i<nftdc[0];i++){ // side-1
            tf=ftdc1[i]*TOFDBc::tdcbin(0);
            if(fabs(tf-tm[0]-fstd) < tofvpar.fstdw()){
              tmf[0]=tf;
              itmf[0]=i;
            }
          }                          
          for(i=0;i<nftdc[1];i++){ // side-2
            tf=ftdc2[i]*TOFDBc::tdcbin(0);
            if(fabs(tf-tm[1]-fstd) < tofvpar.fstdw()){
              tmf[1]=tf;
              itmf[1]=i;
            }
          }
//  --------> check the presence of f-TDC history hits and                          
//  --------> do "befor"/"after" cuts for both sides :
          int reject=0;
          if(tmf[0]<0. || tmf[1]<0.)reject=1;
//
          if(TOFRECFFKEY.reprtf[2]>0 && reject==0){ // histogr.
            j=itmf[0]-1; // befor S-1
            if(j >= 0){
              tf=ftdc1[j]*TOFDBc::tdcbin(0);
              HF1(1101,(tf-tmf[0]),1.);
            }
            j=itmf[1]-1; // befor S-2
            if(j >= 0){
              tf=ftdc2[j]*TOFDBc::tdcbin(0);
              HF1(1101,(tf-tmf[1]),1.);
            }
            j=itmf[0]+1; // after S-1
            if(j < nftdc[0]){
              tf=ftdc1[j]*TOFDBc::tdcbin(0);
              HF1(1102,(tmf[0]-tf),1.);
            }
            j=itmf[1]+1; // after S-2
            if(j < nftdc[1]){
              tf=ftdc2[j]*TOFDBc::tdcbin(0);
              HF1(1102,(tmf[1]-tf),1.);
            }
          }
//
          if(reject==0){   // side-1, "befor" check
            j=itmf[0]-1;
            if(j >= 0){
              tf=ftdc1[j]*TOFDBc::tdcbin(0);
              if((tf-tmf[0]) < tofvpar.hiscutb())reject=1;
//  ( check! later: in fTDC first hit is earliest and have highest value) 
            }
          }
          if(reject==0){   // side-1, "after"
            j=itmf[0]+1;
            if(j < nftdc[0]){
              tf=ftdc1[j]*TOFDBc::tdcbin(0);
              if((tmf[0]-tf) < tofvpar.hiscuta())reject=1;
//  ( check! later: in fTDC first hit is earliest and have highest value) 
            }
          }
          if(reject==0){   // side-2, "befor"
            j=itmf[1]-1;
            if(j >= 0){
              tf=ftdc2[j]*TOFDBc::tdcbin(0);
              if((tf-tmf[1]) < tofvpar.hiscutb())reject=1;
//  ( check! later: in fTDC first hit is earliest and have highest value) 
            }
          }
          if(reject==0){   // side-2, "after"
            j=itmf[1]+1;
            if(j < nftdc[1]){
              tf=ftdc2[j]*TOFDBc::tdcbin(0);
              if((tmf[1]-tf) < tofvpar.hiscuta())reject=1;
//  ( check! later: in fTDC first hit is earliest and have highest value) 
            }
          }
//----------->>> set status of sc.bar, proc. and store it :
          if(reject==1){
            sta=1; // problem with history
          }
          else{
            sta=0;  // ok
            TOFJobStat::addbr(brnum,2);
            tmi[0]=stdc1[0];
            tmi[1]=stdc2[0];
            am[0]=adca1[0]; // raw values
            am[1]=adca2[0];
            zc=TOFDBc::getzsc(ilay,ibar);
//------
            if(TOFRECFFKEY.relogic[0]<=1){// ====> Normal and TZSL_calib reconstruction :
//        Calc. longit. coord/err and position corr. to signal :
              scbrcal[ilay][ibar].tmd2p(tmi,am,co,eco);// get corrected coord/err
              brlm=0.5*TOFDBc::brlen(ilay,ibar)+3.*eco;//limit on max. coord
              if(fabs(co) > brlm){   //means "coord. is more than counter half length"
                pcorr=scbrcal[ilay][ibar].poscor(0.);// take position corr. as for "0"
              }
              else{
                pcorr=scbrcal[ilay][ibar].poscor(co);
              }
//        Find Eloss from anode :
              am[0]=adca1[0]; // take again raw values
              am[1]=adca2[0];
              qtota=scbrcal[ilay][ibar].ama2mip(am)
                                             /pcorr; //an-tot Edep(mev) with corrections
              ama[0]=am[0]*TOFDBc::tdcbin(2);// just TDC_counts->ns
              ama[1]=am[1]*TOFDBc::tdcbin(2);
//        Find counter time corrected by anode TovT :
              time=scbrcal[ilay][ibar].tm2t(tmi,am); // time with corrections
//        Find Eloss from dinode :
              qtotd=0.;
              amd[0]=0.;
              amd[1]=0.;
              if(nadcd[0]==1 && nadcd[1]==1){ 
                am[0]=adcd1[0]; // take again raw values
                am[1]=adcd2[0];
                qtotd=scbrcal[ilay][ibar].amd2mip(am)
                                              /pcorr;//di-tot Edep(mev) with corrections
                amd[0]=am[0]*TOFDBc::tdcbin(3);// raw amplitudes(ns) needed
                amd[1]=am[1]*TOFDBc::tdcbin(3);// for later calibration
              } 
              tmf[0]=tmi[0]*TOFDBc::tdcbin(1);// raw times(ns) needed
              tmf[1]=tmi[i]*TOFDBc::tdcbin(1);// for later calibration
              nbrl[ilay]+=1;
              edepa[ilay]=qtota;// store some number for histogramming
              edepd[ilay]=qtotd;
              tcorr[ilay]=time;
              tdiff[ilay]=0.5*(tmf[0]-tmf[1])/scbrcal[ilay][ibar].gtstrat();// raw time-diff.(ns)
              pch1[ilay]=charg[0];
              pch2[ilay]=charg[1];
              brnl[ilay]=ibar;
              clong[ilay]=co;
              st=0;
              coo=co;
              ecoo=eco;
              st=AMSEvent::gethead()->addnext(AMSID("AMSTOFRawCluster",0)
              ,new AMSTOFRawCluster(sta,ilay+1,ibar+1,zc,ama,amd,
                                  qtota,qtotd,tmf,time,coo,ecoo));//store values
            }
//------
          } // end of history status check
//----------->>>
        } // ---> end of "measurement-multiplicity" check
      } // ---> end of "both sides h/w status OK" check
    } // ---> end of "next COUNTER" or "last hit" check
//------------------------------------------------------
    ptr=ptr->next();// take next RawEvent hit
  }//                          ---- end of RawEvent hits loop ------->
// now check min. multiplicity :
  int nbrch[SCLRS];
  for(i=0;i<SCLRS;i++){
    nbrch[i]=0;
    if(nbrl[i]>0)nbrch[i]=1;
  }
  if((nbrch[0]+nbrch[1])<1 || (nbrch[2]+nbrch[3])<1)return; // remove low layer-mult.
  status=0;
  if(TOFRECFFKEY.reprtf[2]>0){// some hist. for bars/layer=1 events)
    bad=0;
    for(i=0;i<SCLRS;i++)if(nbrl[i] != 1)bad=1;
    if(bad==0){
      HF1(1529,edepa[0],1.); //layer=0 Anode-reconstructed Edep
      HF1(1526,edepa[0],1.); //layer=0 Anode-reconstructed Edep
      HF1(1530,edepa[2],1.); //layer=2 Anode-reconstructed Edep
      HF1(1527,edepa[2],1.); //layer=2 Anode-reconstructed Edep
      HF1(1531,edepd[0],1.); //layer=0 Dinode-reconstructed Edep
      HF1(1528,edepd[0],1.); //layer=0 Dinode-reconstructed Edep
      HF1(1532,(tcorr[0]-tcorr[2]),1.);//ToF for L0->L2
      HF1(1534,(tcorr[1]-tcorr[3]),1.);//ToF for L1->L3
      HF1(1533,tdiff[0],1.);//layer=0
      HF1(1543,clong[0],1.);
      if(AMSJob::gethead()->isSimulation()){
        geant tch;
        charg[0]=pch1[0];
        charg[1]=pch2[0];
        tch=charg[0];
        if(tch<=0.)tch=1.;
        HF1(1070,log(tch),1.);
        tch=charg[0]+charg[1];
        HF1(1071,tch,1.);
        HF1(1072,tch,1.);
      }
    }
    
  }
}
//-----------------------------------------------------------------------
void AMSTOFRawCluster::_writeEl(){
}
//------

void AMSTOFRawCluster::_copyEl(){
}

//------
void AMSTOFRawCluster::_printEl(ostream & stream){
  stream <<"AMSTOFRawCluster "<<_ntof<<" "<<_edep<<" "<<_plane<<endl;
}

AMSTOFCluster * AMSTOFCluster::_Head[4]={0,0,0,0};


void AMSTOFCluster::_writeEl(){
  // fill the ntuple
  if(AMSTOFCluster::Out( IOPA.WriteAll ||  checkstatus(AMSDBc::USED ))){
static TOFClusterNtuple TN;
static integer init=0;
if(init++==0){
  //book the ntuple block
  HBNAME(IOPA.ntuple,"TOFClust",TN.getaddress(),
  "TOFCluster:I*4,TOFStatus:I*4,Ntof:I*4,Plane:I*4, TOFEdep:R*4, TOFTime:R*4,TOFETime:R*4,TOFCoo(3):R*4,TOFErCoo(3):R*4");

}
TN.Event()=AMSEvent::gethead()->getid();
  TN.Status=_status;
  TN.Ntof=_ntof;
  TN.Plane=_plane;
  TN.Edep=_edep;
  TN.Time=_time;
  TN.ErrTime=_etime;
  int i;
  for(i=0;i<3;i++)TN.Coo[i]=_Coo[i];
  for(i=0;i<3;i++)TN.ErrorCoo[i]=_ErrorCoo[i];
  HFNTB(IOPA.ntuple,"TOFClust");
  }
}



void AMSTOFCluster::_copyEl(){
}


void AMSTOFCluster::_printEl(ostream & stream){
  stream <<"AMSTOFCluster "<<_status<<" "<<_ntof<<" "<<_plane<<" "<<_edep<<" "<<_Coo<<  " "<<_ErrorCoo<<" "<<_time<<" " <<_etime<<endl;
}


 
void AMSTOFCluster::print(){
for(int i=0;i<4;i++){
AMSContainer *p =AMSEvent::gethead()->getC("AMSTOFCluster",i);
 if(p)p->printC(cout);
}
}

//===========================================================================

void AMSTOFCluster::build(int &stat){
  AMSTOFRawCluster *ptr; 
  AMSTOFRawCluster *ptrr; 
  static AMSTOFRawCluster * xptr[SCMXBR+2];
  static number eplane[SCMXBR+2];
  geant dummy,edep,edepl,edepa,edepd,asatl,time,etime;
  integer ntof,barn,status,plrot;
  geant barl,barw,bars,cofg,cofgl,yloc,eyloc,ylocm,c0,ct,cl;
  geant ed;
  AMSPoint coo,ecoo;
  int i,j,il,ib,ilay,ibar;
  int nclust,cllay[SCLRS];
//-----
  stat=1; // bad
  for(i=0;i<SCLRS;i++)cllay[i]=0;
//
  for(il=1;il<SCLRS+1;il++){// <-------- TOF layers loop -----
    ptr=(AMSTOFRawCluster*)AMSEvent::gethead()->
                                     getheadC("AMSTOFRawCluster",0);
    VZERO(eplane,(SCMXBR+2)*sizeof(number)/4);
    VZERO(xptr,(SCMXBR+2)*sizeof(AMSTOFRawCluster*)/4);
    while (ptr){// scan to put all bars of layer "il" in buffer
      if(ptr->getntof()==il){
        ib=ptr->getplane();
#ifdef __AMSDEBUG__
        assert(ib>0 && ib <= SCMXBR);
#endif
        edepa=ptr->getedep();
        edepd=ptr->getedepd();
//    select between anode and dinode measurements:
        edep=edepa;
        asatl=scbrcal[il-1][ib-1].getasatl();
        if(edepd/2. > asatl)edep=edepd;
//
        eplane[ib]=edep;
        xptr[ib]=ptr;
      }
      ptr=ptr->next();
    }// ---> end of scan
//------
  nclust=0;
  for (i=1;i<=SCMXBR;i++){// <---- loop over buffer content (clust. search)
    if(   eplane[i]> TOFRECFFKEY.Thr1
       && eplane[i]>= eplane[i-1] 
       && eplane[i]>= eplane[i+1] ){ // <--- peak check (over 3-bars group)
      ptr=xptr[i];// peak pointer
#ifdef __AMSDEBUG__
      assert(ptr!=NULL);
#endif
      barn=i;// peak bar number
      ntof=ptr->getntof();// TOF-layer number
      ilay=ntof-1;
      ibar=barn-1;
      barl=TOFDBc::brlen(ilay,ibar);// peak bar length
      barw=TOFDBc::plnstr(5);//bar width
      bars=TOFDBc::plnstr(5)-TOFDBc::plnstr(4)
                            +2.*TOFDBc::plnstr(7);//sc.bar transv. step
      TOFMCFFKEY.padl=bars;//redef. datacard's sc.bar transv.step
      yloc=ptr->gettimeD();// get yloc/err for "peak" bar
      eyloc=ptr->getetimeD();
      ylocm=0.5*barl+3.*eyloc;// limit on max. coord.
      if(fabs(yloc) > ylocm){//out of bar size
        eyloc=barl/sqrt(12.);
        yloc=0.;//at bar center
      }
      status=ptr->getstatus();// tempor: How to use it here ?
      edep=0.;
      edepl=0.;
      cofg=0.;
      cofgl=0.;
      for(j=i-1;j<i+2;j++){// calc. clust. energy/COG-transv/COG-long
       ptrr=xptr[j];   
       if(ptrr){
        edep+=eplane[j];
        cofg+=eplane[j]*(j-i);//relative to "peak" bar
        yloc=ptrr->gettimeD();
        if(fabs(yloc) < ylocm){
         edepl+=eplane[j];
         cofgl+=eplane[j]*yloc;
        }
       }
      }
      time=ptr->gettime();// (ns)
      etime=TOFMCFFKEY.TimeSigma/sqrt(2.);  //(sec !!) tempor(later put in TOFBrcal needed data!)
//------
      if(edep>TOFRECFFKEY.ThrS){// <--- calc.clus.parameters if Ecl>Ethresh
        if(TOFRECFFKEY.reprtf[2]>0){
        if(il==1){
          HF1(1535,edep,1.);//Cluster energy distr.,L=1
          HF1(1537,edep,1.);//Cluster energy distr.,L=1
        }
        if(il==2){
          HF1(1539,edep,1.);//Cluster energy distr.,L=2
          HF1(1541,edep,1.);//Cluster energy distr.,L=2
        }
        if(il==3){
          HF1(1536,edep,1.);//Cluster energy distr.,L=3
          HF1(1538,edep,1.);//Cluster energy distr.,L=3
        }
        if(il==4){
          HF1(1540,edep,1.);//Cluster energy distr.,L=4
          HF1(1542,edep,1.);//Cluster energy distr.,L=4
        }
        }
        coo[2]=ptr->getz();          //cluster Z-coord.
        ecoo[2]=2.*TOFDBc::plnstr(6)/sqrt(12.);//2(for secur) *bar thickness/...   
        plrot=TOFDBc::plrotm(ilay);     // =0/1-unrotated/rotated TOF-plane
        c0=TOFDBc::gettsc(ilay,ibar);   //transv.pos. of "peak" bar
        ct=cofg/edep*bars+c0;           //cluster abs. transv. coord.
        cl=cofgl/edepl;    //cluster abs.longit. coord.(neglect counter's long.shift) 
//    Calculate 2-D coordinates of cluster COG according to plane rotation mask:
        if(plrot==0){ // non-rotated plane
          coo[0]=ct;//clust. X-coord.
          ecoo[0]=bars/2.7;// 2.7 for security
          coo[1]=cl;//clust. Y-coord.
          ecoo[1]=1.5*eyloc;// 1.5 for security
        }
        else{ // rotated plane
          coo[0]=-cl;//rotated plane has yloc=-xabs
          ecoo[0]=1.5*eyloc;
          coo[1]=ct;
          ecoo[1]=bars/2.7;
        }
        time=-time*1.e-9;// ( ns -> sec ,"-" for V.Shoutko fit)
        AMSEvent::gethead()->addnext(AMSID("AMSTOFCluster",ilay),
        new     AMSTOFCluster(status,ntof,barn,edep,coo,ecoo,time,etime));
        nclust+=1;
        eplane[i-1]=0.;// remove used bars
        eplane[i]=0.;
        eplane[i+1]=0.; 
        i=1;// to start clust. search from the beginning
      }// ---> end of accepted_cluster calculation
//------
    }// ---> end of peak check    
  }// ---> end of buffer (i) loop (clust.search)
  if(nclust>0)cllay[il-1]=1;
//------
  }// ---> end of TOF layers (il) loop
  if((cllay[0]+cllay[1])>0 && (cllay[2]+cllay[3])>0)stat=0;
// ( at least one cluster per top/bot TOF-subsystem)
}
//===============================================================================




// Old Choutko's version, it is not used now:

void AMSTOFCluster::build(){
  for(integer kk=1;kk<5;kk++){
    AMSTOFRawCluster *ptr=(AMSTOFRawCluster*)AMSEvent::gethead()->
getheadC("AMSTOFRawCluster",0);
integer const maxpl=22;
static number xplane[maxpl];
static AMSTOFRawCluster * xptr[maxpl];
VZERO(xplane,maxpl*sizeof(number)/4);
VZERO(xptr,maxpl*sizeof(AMSTOFRawCluster*)/4);
while (ptr){
  if(ptr->getntof()==kk){
   integer plane=ptr->getplane();
#ifdef __AMSDEBUG__
   assert(plane>0 && plane < maxpl-1);
#endif
   xplane[plane]=ptr->getedep();
   xptr[plane]=ptr;
  }
 ptr=ptr->next();
}

for (int i=0;i<maxpl;i++){ 
 if(xplane[i] > TOFRECFFKEY.Thr1 && xplane[i]>= xplane[i-1] 
    && xplane[i]>= xplane[i+1] ){
 ptr=xptr[i];
#ifdef __AMSDEBUG__
 assert(ptr!=NULL);
#endif
 integer ntof,plane,status;
 number time,edep,cofg,timed;
 AMSPoint coo,ecoo;
 plane=i;
 ntof=ptr->getntof();
 timed=ptr->gettimeD();
 status=ptr->getstatus();
 edep=0;
 cofg=0;
 for(int j=i-1;j<i+2;j++){
   edep+=xplane[j];
   cofg+=xplane[j]*(j-i);   
 }
 time=ptr->gettime();
 if(edep>TOFRECFFKEY.ThrS){
   number etime, etimed;
#if 1 
      // change to zero when Eugeni  writes a proper code
     etime=TOFMCFFKEY.TimeSigma/sqrt(2.);
     const number c=1.7e10;    //speed of light
     etimed=TOFMCFFKEY.TimeSigma/sqrt(2.)*c;
#else
     // Eugeni code here
#endif
 
   coo[2]=ptr->getz()+0.5;
   coo[0]=0;
   coo[1]=0;
   ecoo[2]=10000;   // big
   
   integer plrot=TOFDBc::plrotm(ntof-1);     // =0/1-unrotated/rotated TOF-plane
   integer ix=0;
   integer iy=0;
   if(plrot==0)ix=1;
   else iy=1;

   coo[ix]=timed;
   ecoo[ix]=etimed;
   
   geant padl=TOFDBc::plnstr(5);
   coo[iy]=cofg/edep*padl+TOFDBc::gettsc(ntof-1,i-1);
   ecoo[iy]=padl/2.7;
   AMSEvent::gethead()->addnext(AMSID("AMSTOFCluster",ntof-1),
   new     AMSTOFCluster(status,ntof,plane,edep,coo,ecoo,time,etime));
   xplane[i-1]=0;
   xplane[i]=0;
   xplane[i+1]=0; 
   i=0;
 }
 }    
 
}
  }
}





integer AMSTOFCluster::Out(integer status){
static integer init=0;
static integer WriteAll=0;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
 for(int n=0;n<ntrig;n++){
   if(strcmp("AMSTOFCluster",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
   }
 }
}
return (WriteAll || status);
}
