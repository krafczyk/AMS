//  $Id: ctcdbcD.C,v 1.5 2001/01/22 17:32:27 choutko Exp $
//
// 13.Jan.1997 - call setgeom from CmpConstants()
//
// Last Edit : Nov 03, 1997. ak. 
//
#include <ctcdbcD.h>
#include <ctcdbc.h>

CTCDBcD::CTCDBcD() {
  _geomId = CTCDBc::_geomId;
  _nlay   = CTCDBc::_nlay;
  _tdcabw = CTCDBc::_tdcabw;
  _ftpulw = CTCDBc::_ftpulw;

}

ooStatus CTCDBcD::CmpConstants() {

 ooStatus  rstatus = oocSuccess;

 CTCDBc::setgeom(_geomId);
 cout <<" CTCDBcD::CmpConstants -I- CTCDBc::setgeom(_geomId) done"<<endl;

 if (_nlay != CTCDBc::_nlay) 
  cout <<" CTCDBcD::CmpConstants -W- nlay "<<_nlay<<", "<<CTCDBc::_nlay<<endl;

 if (_tdcabw != CTCDBc::_tdcabw) 
  cout <<" CTCDBcD::CmpConstants -W- tdcabw "<<_tdcabw<<", "<<CTCDBc::_tdcabw<<endl;

 if (_ftpulw != CTCDBc::_ftpulw) 
  cout <<" CTCDBcD::CmpConstants -W- ftpulw "<<_ftpulw<<", "<<CTCDBc::_ftpulw<<endl;

 return rstatus;
}
