#include "AMSNtuple.h"
ClassImp(AMSNtuple)

  AMSNtuple::AMSNtuple(TFile *file){
    _h1=  (TTree *)file->Get("h1");
    if(_h1){
     // Event 
    _h1->SetBranchAddress("eventno",&_Event.EventNo);
    _h1->SetBranchAddress("run",&_Event.Run);
    _h1->SetBranchAddress("runtype",&_Event.RunType);
    _h1->SetBranchAddress("time",&_Event.Time);
 
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
         ReadOneEvent(0);



    }
    else{
      cerr<<"AMSNtuple::AMSNtuple-S-UnableInitializeNtuple"<<endl;
      _Event.Run=0;
      _Event.Time=0;
    }
}
