// Author E.Choumilov 2.06.97
//
#include <typedefs.h>
#include <commons.h>
#include <amsdbc.h>
#include <antidbc.h>
//
//
//======> just memory reservation :
// (real values are initialized at run-time from data cards in setgeom())
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
//  member functions :
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
  void ANTIDBc::setgeom(){ //get parameters from data cards now
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
//
  
