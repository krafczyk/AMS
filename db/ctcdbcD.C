//
// 13.Jan.1997 - call setgeom from CmpConstants()
//
// Last Edit : Mar 06, 1997. ak. add nlay
//
#include <ctcdbcD.h>
#include <ctcdbc.h>

CTCDBcD::CTCDBcD() {
  _geomIdD = CTCDBc::_geomId;
  _nlayD   = CTCDBc::_nlay;
}

ooStatus CTCDBcD::CmpConstants() {

 ooStatus  rstatus = oocSuccess;

 CTCDBc::setgeom(_geomIdD);
 cout <<" CTCDBcD::CmpConstants -I- CTCDBc::setgeom(_geomIdD) done"<<endl;
 if (_nlayD != CTCDBc::_nlay) 
  cout <<" CTCDBcD::CmpConstants -W- nlay "<<_nlayD<<", "<<CTCDBc::_nlay<<endl;
 return rstatus;
}
