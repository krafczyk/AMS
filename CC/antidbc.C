// Author E.Choumilov 2.06.97
//
#include <typedefs.h>
#include <commons.h>
#include <job.h>
#include <amsdbc.h>
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <antidbc.h>
//
ANTIPcal antisccal[MAXANTI];// create empty array of antipaddles calibr. objects
//
//======> just memory reservation for ANTIDBc class variables:
// (real values are initialized at run-time from data cards in setgeom() or in...)
// 
  geant ANTIDBc::_scradi=0.;
  geant ANTIDBc::_scinth=0.;
  geant ANTIDBc::_scleng=0.;
  geant ANTIDBc::_wrapth=0.;
  geant ANTIDBc::_groovr=0.;
  geant ANTIDBc::_pdlgap=0.;
  geant ANTIDBc::_stradi=0.;
  geant ANTIDBc::_stleng=0.;
  geant ANTIDBc::_stthic=0.;
//
//  ANTIDBc class member functions :
//
  geant ANTIDBc::scradi(){return _scradi;}
  geant ANTIDBc::scinth(){return _scinth;}
  geant ANTIDBc::scleng(){return _scleng;}
  geant ANTIDBc::wrapth(){return _wrapth;}
  geant ANTIDBc::groovr(){return _groovr;}
  geant ANTIDBc::pdlgap(){return _pdlgap;}
  geant ANTIDBc::stradi(){return _stradi;}
  geant ANTIDBc::stleng(){return _stleng;}
  geant ANTIDBc::stthic(){return _stthic;}
//
  void ANTIDBc::setgeom(){ //get parameters from data cards (for now)
    _scradi=ANTIGEOMFFKEY.scradi;
    _scinth=ANTIGEOMFFKEY.scinth;
    _scleng=ANTIGEOMFFKEY.scleng;
    _wrapth=ANTIGEOMFFKEY.wrapth;
    _groovr=ANTIGEOMFFKEY.groovr;
    _pdlgap=ANTIGEOMFFKEY.pdlgap;
    _stradi=ANTIGEOMFFKEY.stradi;
    _stleng=ANTIGEOMFFKEY.stleng;
    _stthic=ANTIGEOMFFKEY.stthic;
  }
//======================================================================
// ANTIPcal class member functions:
//
void ANTIPcal::build(){ // fill array of objects with data
  integer i,j;
  integer sta[2]={0,0}; // all  are alive
  geant thr; // MC trigger threshold for one side.(p.e. for now)
  if(AMSJob::gethead()->isMCData()){ //            =====> For MC data:
    for(i=0;i<MAXANTI;i++){
      thr=ANTIMCFFKEY.trithr; // take trig.threshold from data card for now
      antisccal[i]=ANTIPcal(i,sta,thr);
    }
  }
//---------------------------------------------------------------------
  else{ //                                         =====> For Real Data :
  }
}
//=====================================================================  
