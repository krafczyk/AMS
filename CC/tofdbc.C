// Author E.Choumilov 14.06.96.
#include <math.h>
#include <tofdbc.h>
#include <stdio.h>
#include <iostream.h>
//
//  =====> TOFDBc class variables definition :
//
integer TOFDBc::debug=1;
//
// Initialize TOF geometry parameters
//
//---> bar lengthes (cm) for each of SCBTPN types :
const number TOFDBc::_brlen[SCBTPN]={
  72.6,99.6,118.6,130.6,135.4
};
//---> bar types  in layers (4 layers top->bot):          
const integer TOFDBc::_brtype[SCLRS][SCMXBR]={
  {1,2,3,4,5,5,5,5,5,5,4,3,2,1},
  {1,2,3,4,5,5,5,5,5,5,4,3,2,1},
  {1,2,3,4,5,5,5,5,5,5,4,3,2,1},
  {1,2,3,4,5,5,5,5,5,5,4,3,2,1}
};
//---> name list for bar t-calibration files:
const char TOFDBc::_brfnam[SCBTPN][6]={
  {'1','0','7','2','6','\0'},
  {'1','0','9','9','6','\0'},
  {'1','1','1','8','6','\0'},
  {'1','1','3','0','6','\0'},
  {'1','1','3','5','4','\0'}
};
//---> plane rotation mask for 4 layers (1/0 -> rotated/not):
const integer TOFDBc::_plrotm[SCLRS]={
  0,1,1,0
};
//---> honeycomb supporting str. data:
const number TOFDBc::_mcfpar[10]={
  8000.,   // i=1  Mev to photons convertion factor in scintillator.
  0.19,    // i=2  PMT mean quantum efficiency
  0.14,    // i=3  PMT transition time spread (ns)
  0.,0.,0.,0.,0.,0.,0.
};
const number TOFDBc::_supstr[10]={
   59.9,    // i=1    Z-coo of TOP honeycomb (BOT-side)
  -59.9,    //  =2    Z-coo    BOT           (TOP     )
   0.,0.,  //  =3,4  X/Y-shifts of TOP honeycomb centre in Mother r.s.
   0.,0.,  //  =5,6                BOT
   10.,78.,//  =7,8  DZ/Router of both honeycomb supports
   0.,0.   //  =9,10 spare
};
//---> sc. plane design parameters:
const number TOFDBc::_plnstr[15]={
  0.,0.,  // i=1,2  Z-gap between honeycomb and outer/inner scint. planes
    1.25,  // i=3    Z-shift of even/odd bars in plane (0 -> flat plane)
     0.6,  // i=4    X(Y)-overlaping(incl. sc_cover) of even/odd bars (0 for flat)
     11., // i=5    sc. bar width
      1., // i=6    sc. bar thickness
    0.05, // i=7    sc_cover thickness (carb.fiber)
 10.,10.,2.5, // i=8-10 width/length/thickness (x/y/z) of PMT shielding box.
  5.,         // i=11 dist. in x-y plane from sc. edge to PMT box.
  0.7,        // i=12 Z-shift of sc.bar/PMT-box centers (depends on par. i=10)
  0.,0.,0.    // i=13-15 spare
};
//---> Initialize TOF MC/RECO parameters :
//
  const number TOFDBc::_edep2ph={8000.};    // edep(Mev)-to-Photons convertion
  const number TOFDBc::_pmqeff=0.19;      // PM mean quantum efficiency
  const number TOFDBc::_trtspr=0.14;      // PM transition time spread (ns)
  const number TOFDBc::_fladctb=0.05;     // MC flash-ADC time binning (ns)
  const number TOFDBc::_shaptb=2.;        // MC shaper pulse time binning
  const number TOFDBc::_di2anr=0.1;       // dinode-to-anode signal ratio
  const number TOFDBc::_shrtim=0.5;       // MC shaper pulse rise time (ns)
  const number TOFDBc::_shftim=50.;       // MC shaper pulse fall time (ns)
  const number TOFDBc::_accdel[2]={
    50.,                       // "accept" fix-delay(ns) for fast(history) TDC
    20.                        // "accept" fix-delay(ns) for slow TDC(stratcher)
  };
  const number TOFDBc::_accdelmx[2]={
    1000.,                     //max. "accept" delay(ns) for fast(history) TDC
    100.                       //max. "accept" delay(ns) for slow TDC(stratcher)
  };
  const number TOFDBc::_fstdw=5.; //t-window for the same hit search in f/s_tdc
  const number TOFDBc::_hiscutb=50.; //"befor"-cut in time history (ns)
  const number TOFDBc::_hiscuta=150.;//"after"-cut in time history (ns)
  const number TOFDBc::_strrat=20.;  // MC stratcher ratio 
  const number TOFDBc::_tdcbin[4]={
    1.,                            // pipe/line TDC binning for fast-tdc meas.
    1.,                            // pipe/line TDC binning for slow-tdc meas.
    2.,                            // pipe/line TDC binning for adc-anode meas.
    2.                             // pipe/line TDC binning for adc-dinode meas.
  };
  const number TOFDBc::_daqthr[5]={
    40.,             // MC threshold(mV) for discr. of "z>=1"-trig,fast/slow_TDC
    100.,            // MC threshold for discr. of "z>1"-trig
    200.,            // MC threshold for discr. of "z>2"-trig
    10.,           // MC threshold(pC) for anode Time_over_Thresh(TovT) measurement
    10.            // MC threshold for dinode Time_over_thresh measurement
  };
  const number TOFDBc::_daqpwd[10]={
    50.,           // pulse width of "z>=1" trig. signal (ns)
    50.,           // pulse width of "z>1" trig. signal
    50.,           // pulse width of "z>2" trig. signal
    10.,           // double pulse resolution of fast(history) TDC (ns)
    500.,          // min. double pulse resolution of slow TDC (ns)
    500.,          // dead time of anode TovT measurements (ns)
    500.,          // dead time of dinode TovT measurements (ns)
    2.,            // discr. dead time of "z>=1" trig. (ns)
    2.,            // discr. dead time of "z>1" trig. (ns)
    2.             // discr. dead time of "z>2" trig. (ns)
  };
  const number TOFDBc::_trigtb=0.5;// MC t-bin in logic(trig) pulse handling (ns)
//===============================================================================
//
//  TOFBrcal static variables definition (just memory reservation):
//     (real values will be stored later by TOFBrcal::init() )
//
number TOFBrcal::slope;
//---
number TOFBrcal::td2pos[2];
//---
number TOFBrcal::logqin[SCACRFP];//  Log(inp_charge(pC)) ref.points
//---
number TOFBrcal::tovta[SCACRFP];// measured(Tovt) A-amplitude points(ns)
//---
number TOFBrcal::tovtd[SCACRFP];// measured(Tovt) D-amplitude points(ns)
//---
number TOFBrcal::asatl;//anode-chain saturation boundary (MeV)
//-------------------
//  TOFBrcal class functions :
//
number TOFBrcal::ama2mip(int am[2]){ // 2-side A-Tovt's -> Mev 
  static int nmx=SCACRFP-1;
  int isd,i;
  number amf[2],qt,lnq;
  amf[0]=am[0]*TOFDBc::tdcbin(2); // conv. to ns
  amf[1]=am[1]*TOFDBc::tdcbin(2); // conv. to ns
  qt=0.;
  for(isd=0;isd<2;isd++){
    if(amf[isd]>=tovta[nmx])lnq=logqin[nmx]+(logqin[nmx]-logqin[nmx-1])
                           *(amf[isd]-tovta[nmx])/(tovta[nmx]-tovta[nmx-1]);
    if(amf[isd]<tovta[0])lnq=logqin[0]+(logqin[1]-logqin[0])
                           *(amf[isd]-tovta[0])/(tovta[1]-tovta[0]);
    for(i=0;i<nmx;i++){
      if(amf[isd]>=tovta[i] && amf[isd]<tovta[i+1])
        lnq=logqin[i]+(logqin[i+1]-logqin[i])
           *(amf[isd]-tovta[i])/(tovta[i+1]-tovta[i]);
    }
    lnq=lnq+log(qathr);// add  Log(Qthresh) (anode) 
    qt=qt+exp(lnq)/gaina[isd];// LogQa->Qa_gain_corrected (pC)
  }
  qt=qt/mip2q; // Q(pC)->Mev
  return qt;
}
//---
number TOFBrcal::amd2mip(int am[2]){ // 2-sides D-Tovt's -> Mev
  static int nmx=SCACRFP-1;
  int isd,i;
  number amf[2],qt,lnq;
  amf[0]=am[0]*TOFDBc::tdcbin(3); // conv. to ns
  amf[1]=am[1]*TOFDBc::tdcbin(3); // conv. to ns
  qt=0.;
  for(isd=0;isd<2;isd++){
    if(amf[isd]>=tovtd[nmx])lnq=logqin[nmx]+(logqin[nmx]-logqin[nmx-1])
                           *(amf[isd]-tovtd[nmx])/(tovtd[nmx]-tovtd[nmx-1]);
    if(amf[isd]<tovtd[0])lnq=logqin[0]+(logqin[1]-logqin[0])
                           *(amf[isd]-tovtd[0])/(tovtd[1]-tovtd[0]);
    for(i=0;i<nmx;i++){
      if(amf[isd]>=tovtd[i] && amf[isd]<tovtd[i+1])
        lnq=logqin[i]+(logqin[i+1]-logqin[i])
           *(amf[isd]-tovtd[i])/(tovtd[i+1]-tovtd[i]);
    }
    lnq=lnq+log(qdthr);// add  Log(Qthresh) (dinode)
    qt=qt+exp(lnq)*an2dir/gaind[isd];// LogQd->Qa->Qa_gain_corrected (pC)
  }
  qt=qt/mip2q; // Q(pc)->Mev
  return qt;
}
//---
number TOFBrcal::poscor(number point){
//(return light-out corr.factor, input 'point' is Y-coord. in bar loc.r.s.)
  static int nmx=SCANPNT-1;
  int i;
  number corr;
  if(point >= yscanp[nmx])corr=relout[nmx]+(relout[nmx]-relout[nmx-1])
                         *(point-yscanp[nmx])/(yscanp[nmx]-yscanp[nmx-1]);
  if(point < yscanp[0])corr=relout[0]+(relout[1]-relout[0])
                         *(point-yscanp[0])/(yscanp[1]-yscanp[0]);
  for(i=0;i<nmx;i++){
    if(point>=yscanp[i] && point<yscanp[i+1])
      corr=relout[i]+(relout[i+1]-relout[i])*(point-yscanp[i])
          /(yscanp[i+1]-yscanp[i]);
  }
  return corr;//you should devide signal by this value later
}
//---
number TOFBrcal::tm2t(int tm[2], int am[2]){ // (2-sides_times+Tovt)->Time (ns)
  number tmf[2],amf[2],time,shft;
  shft=TOFDBc::shftim();
  amf[0]=am[0]*TOFDBc::tdcbin(3); // conv. to ns
  amf[1]=am[1]*TOFDBc::tdcbin(3); // conv. to ns
  tmf[0]=tm[0]*TOFDBc::tdcbin(1)/strat;// to real-time scale (ns) 
  tmf[1]=tm[1]*TOFDBc::tdcbin(1)/strat;// to real-time scale (ns)
  time=0.5*(tmf[0]+tmf[1])+tzero
                     +slope*(exp(-amf[0]/shft)+exp(-amf[1]/shft));
  return time; 
}
//==========================================================================
//
//   TOFJobStat static variables definition (memory reservation):
//
integer TOFJobStat::mccount[SCJSTA];
integer TOFJobStat::recount[SCJSTA];
integer TOFJobStat::chcount[SCCHMX][SCCSTA];
integer TOFJobStat::brcount[SCBLMX][SCCSTA];
//
// function to print Job-statistics at the end of JOB(RUN):
//
void TOFJobStat::print(){
  int il,ib,ic;
  float rc;
  printf("\n");
  printf("    ============ JOB TOF-statistics =============\n");
  printf("\n");
  printf(" MC-entries              : % 6d\n",mccount[0]);
  printf(" RECO-entries            : % 6d\n",recount[0]);
  printf(" DAQ->RawEvent OK        : % 6d\n",recount[1]);
  printf(" Entries to TZSl-calibr. : % 6d\n",recount[2]);
  printf(" TZSl: multiplicity OK   : % 6d\n",recount[3]);
  printf(" TZSl: beta OK           : % 6d\n",recount[4]);
  printf(" RawEvent->RawCluster OK : % 6d\n",recount[5]);
  printf(" RawCluster->Cluster OK  : % 6d\n",recount[6]);
  printf("\n\n");
//
  printf("=====> Channels status :\n\n");
//
  printf("H/w-status OK :\n\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      printf(" % 6d",chcount[ic][0]);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      printf(" % 6d",chcount[ic][0]);
    }
    printf("\n\n");
  }
//
  printf("Hist-TDC 'ON' :\n");
  printf("\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      rc=float(chcount[ic][0]);
      if(rc>0.)rc=float(chcount[ic][1])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      rc=float(chcount[ic][0]);
      if(rc>0.)rc=float(chcount[ic][1])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
  }
//
  printf("Slow-TDC 'ON' :\n");
  printf("\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      rc=float(chcount[ic][0]);
      if(rc>0.)rc=float(chcount[ic][2])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      rc=float(chcount[ic][0]);
      if(rc>0.)rc=float(chcount[ic][2])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
  }
//
  printf("Anode-ADC 'ON' :\n");
  printf("\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      rc=float(chcount[ic][0]);
      if(rc>0.)rc=float(chcount[ic][3])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      rc=float(chcount[ic][0]);
      if(rc>0.)rc=float(chcount[ic][3])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
  }
//
  printf("Dinode-ADC 'ON' :\n");
  printf("\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      rc=float(chcount[ic][0]);
      if(rc>0.)rc=float(chcount[ic][4])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      rc=float(chcount[ic][0]);
      if(rc>0.)rc=float(chcount[ic][4])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
  }
//
  printf("History-TDC '1 hit' :\n");
  printf("\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      rc=float(chcount[ic][0]);
      if(rc>0.)rc=float(chcount[ic][5])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      rc=float(chcount[ic][0]);
      if(rc>0.)rc=float(chcount[ic][5])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
  }
//
  printf("Slow-TDC '1 hit' :\n");
  printf("\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      rc=float(chcount[ic][0]);
      if(rc>0.)rc=float(chcount[ic][6])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      rc=float(chcount[ic][0]);
      if(rc>0.)rc=float(chcount[ic][6])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
  }
//
  printf("Anode-ADC '1 hit' :\n");
  printf("\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      rc=float(chcount[ic][0]);
      if(rc>0.)rc=float(chcount[ic][7])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      rc=float(chcount[ic][0]);
      if(rc>0.)rc=float(chcount[ic][7])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
  }
//
  printf("Dinode-ADC '1 hit' :\n");
  printf("\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      rc=float(chcount[ic][0]);
      if(rc>0.)rc=float(chcount[ic][8])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      rc=float(chcount[ic][0]);
      if(rc>0.)rc=float(chcount[ic][8])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
  }
//

}
//==========================================================================

