#include <ctcdbcD.h>
#include <ctcdbc.h>

CTCDBcD::CTCDBcD() {
  _geomIdD = CTCDBc::_geomId;
}

ooStatus CTCDBcD::CmpConstants() {

 ooStatus  rstatus = oocSuccess;

 if(_geomIdD != CTCDBc::_geomId) {
   cout <<" CTCDBcD::CmpConstants -W- _geomId "<<_geomIdD<<", "
        << CTCDBc::_geomId<<endl;
   cout <<" CTCDBcD::CmpConstants -I- _geomId will be set to "<<_geomIdD
        <<endl;
   CTCDBc::_geomId = _geomIdD;
   rstatus = oocError;
 }
 return rstatus;
}
