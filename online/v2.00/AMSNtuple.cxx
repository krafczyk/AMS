#include <stdlib.h>
#include "AMSNtuple.h"
ClassImp(AMSNtuple)

  AMSNtuple::AMSNtuple(TFile *file){
    _h1=  (TTree *)file->Get("h1");
    if(_h1){
     // Event 
    _h1->SetBranchAddress("eventno",&_Event.EventNo);
    _h1->SetBranchAddress("run",&_Event.Run);
    _h1->SetBranchAddress("runtype",&_Event.RunType);
    _h1->SetBranchAddress("time",_Event.Time);
    _h1->SetBranchAddress("Thetas",&_Event.Theta);
    _h1->SetBranchAddress("Phis",&_Event.Phi);
    _h1->SetBranchAddress("Rads",&_Event.Rad);
    _h1->SetBranchAddress("Velocitys",&_Event.Speed);
 
    //LVL1
 
        _h1->SetBranchAddress("Lvl1tofpatt",_LVL1.TOF);
        _h1->SetBranchAddress("Lvl1tofpatt1",_LVL1.TOFZ);
        _h1->SetBranchAddress("Lvl1antipatt",&_LVL1.Anti);
 
    //AxAMS
         _h1->SetBranchAddress("npart",        &_AxAMS.npart);
         _h1->SetBranchAddress("pctcp",         _AxAMS.pctcp);
         _h1->SetBranchAddress("pbetap",        _AxAMS.pbetap);
         _h1->SetBranchAddress("pchargep",      _AxAMS.pchargep);
         _h1->SetBranchAddress("ptrackp",       _AxAMS.ptrackp);
         _h1->SetBranchAddress("pid",           _AxAMS.pid);
         _h1->SetBranchAddress("pmass",         _AxAMS.pmass);
         _h1->SetBranchAddress("perrmass",      _AxAMS.perrmass);
         _h1->SetBranchAddress("pmom",          _AxAMS.pmom);
         _h1->SetBranchAddress("perrmom",       _AxAMS.perrmom);
         _h1->SetBranchAddress("pcharge",       _AxAMS.pcharge);
         _h1->SetBranchAddress("ptheta",        _AxAMS.ptheta);
         _h1->SetBranchAddress("pphi",          _AxAMS.pphi);
         _h1->SetBranchAddress("pcoo",          _AxAMS.pcoo);
         _h1->SetBranchAddress("signalctc",     _AxAMS.signalctc);
         _h1->SetBranchAddress("betactc",       _AxAMS.betactc);
         _h1->SetBranchAddress("errorbetactc",  _AxAMS.errorbetactc);
         _h1->SetBranchAddress("nbeta", &_AxAMS.nbeta);
         _h1->SetBranchAddress("beta", _AxAMS.beta);
         //Anti

              _h1->SetBranchAddress("nanti",&_ANTI.nanti);
              _h1->SetBranchAddress("Antistatus",_ANTI.AntiStatus);
              _h1->SetBranchAddress("Antisector",_ANTI.AntiSector);
              _h1->SetBranchAddress("Anticoo",_ANTI.AntiCoo);
              _h1->SetBranchAddress("Antiercoo",_ANTI.AntiErCoo);
              _h1->SetBranchAddress("Antiedep",_ANTI.AntiSignal);
              _h1->SetBranchAddress("nantiraw",&_ANTI.nantiraw);
              _h1->SetBranchAddress("antirawstatus",_ANTI.AntirawStatus);
              _h1->SetBranchAddress("antirawsector",_ANTI.AntirawSector);
              _h1->SetBranchAddress("antirawupdown",_ANTI.Antirawupdown);
              _h1->SetBranchAddress("antirawsignal",_ANTI.AntirawSignal);

     //TOF
           _h1->SetBranchAddress("ntof",     &_TOF.NToF);
          _h1->SetBranchAddress("Tofstatus", _TOF.Status);
          _h1->SetBranchAddress("plane",     _TOF.Plane);
          _h1->SetBranchAddress("bar",       _TOF.Bar);
          _h1->SetBranchAddress("Tofedep",   _TOF.Edep);
          _h1->SetBranchAddress("Toftime",   _TOF.Time);
          _h1->SetBranchAddress("Tofetime",  _TOF.ErTime);
          _h1->SetBranchAddress("Tofcoo",    _TOF.Coo);
          _h1->SetBranchAddress("Tofercoo",  _TOF.ErCoo);

         //Tracker
           _h1->SetBranchAddress("ntrraw",&_Tracker.ntrraw);
           _h1->SetBranchAddress("rawaddress",&_Tracker.rawaddress);
           _h1->SetBranchAddress("rawlength",&_Tracker.rawlength);
           _h1->SetBranchAddress("s2n",&_Tracker.s2n);


            _h1->SetBranchAddress("ntrrh",&_Tracker.ntrrh);
            _h1->SetBranchAddress("px",_Tracker.px);
            _h1->SetBranchAddress("py",_Tracker.py);
            _h1->SetBranchAddress("statusr",_Tracker.statusr);
            _h1->SetBranchAddress("Layer",_Tracker.Layer);
            _h1->SetBranchAddress("hitr",_Tracker.hitr);
            _h1->SetBranchAddress("ehitr",_Tracker.ehitr);
            _h1->SetBranchAddress("sumr",_Tracker.sumr);
            _h1->SetBranchAddress("difosum",_Tracker.difosum);

            _h1->SetBranchAddress("Ntrcl",&_Tracker.ntrcl);
            _h1->SetBranchAddress("Idsoft",&_Tracker.idsoft);
            _h1->SetBranchAddress("Statust",&_Tracker.statust);
            _h1->SetBranchAddress("Neleml",&_Tracker.neleml);
            _h1->SetBranchAddress("Nelemr",&_Tracker.nelemr);
            _h1->SetBranchAddress("Sumt",&_Tracker.sumt);
            _h1->SetBranchAddress("Sigmat",&_Tracker.sigmat);
            _h1->SetBranchAddress("Sigmat",&_Tracker.sigmat);
            _h1->SetBranchAddress("Rmst",&_Tracker.rmst);


        //LVL3
              _h1->SetBranchAddress("nlvl3",&_LVL3.nlvl3);
              _h1->SetBranchAddress("Lvl3toftr",&_LVL3.TOFTr);
              _h1->SetBranchAddress("Lvl3antitr",&_LVL3.AntiTr);
              _h1->SetBranchAddress("Lvl3trackertr",&_LVL3.TrackerTr);
              _h1->SetBranchAddress("Lvl3ntrhits",&_LVL3.NtrHits);
              _h1->SetBranchAddress("Lvl3npat",&_LVL3.NPat);
              _h1->SetBranchAddress("Lvl3pattern",_LVL3.Pattern);
              _h1->SetBranchAddress("Lvl3residual",_LVL3.Residual);
              _h1->SetBranchAddress("Lvl3eloss",&_LVL3.ELoss);

              //CTC
              _h1->SetBranchAddress("nctcht",&_CTC.nhit);
              _h1->SetBranchAddress("Ctchitlayer",_CTC.layer);
              _h1->SetBranchAddress("ctchitcolumn",_CTC.x);
              _h1->SetBranchAddress("ctchitrow",_CTC.y);
              _h1->SetBranchAddress("ctchitsignal",_CTC.signal);

         ReadOneEvent(0);

     

    }
    else{
      cerr<<"AMSNtuple::AMSNtuple-S-UnableInitializeNtuple"<<endl;
      exit(1);
      _Event.Run=0;
      _Event.Time[0]=0;
    }
}
