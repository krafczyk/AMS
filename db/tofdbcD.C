// Oct 08, 1996. ak. first try with Objectivity
//                   method source file of object TOFDBcD
//
// Jan 21, 1997. ak. 
//                     TOFDBcD doens't match to the latest E.Ch. version
//
// Feb 28, 1997. ak.   TOFDBcD up to date
// Mar 24, 1997. ak.   CmpConstants is significantly modified
//
//Last Edit : Nov 3, 1997. ak.
//

#include <iostream.h>
#include <string.h>
#include <cern.h>
#include <rd45.h>

#include <tofdbc.h>
#include <tofdbcD.h>
//

TOFDBcD::TOFDBcD() 
{
  integer i;
  
  for (i=0; i<SCBLMX; i++) {
    _brtype[i] = TOFDBc::_brtype[i];
  }

  for (i=0; i<SCLRS; i++) {
    _plrotm[i] = TOFDBc::_plrotm[i];
  }

  for (i=0; i<SCBTPN; i++) {
    _brlen[i] = TOFDBc::_brlen[i];
  }

  for (i=0; i<10; i++) {
    _supstr[i] = TOFDBc::_supstr[i];
  }

  for (i=0; i<15; i++) {
    _plnstr[i] = TOFDBc::_plnstr[i];
  }


  _edep2ph     = TOFDBc::_edep2ph;
  _seresp      = TOFDBc::_seresp;
  _seresv      = TOFDBc::_seresv;
  _fladctb     = TOFDBc::_fladctb;
  _shaptb      = TOFDBc::_shaptb;
  _shrtim      = TOFDBc::_shrtim;
  _shftim      = TOFDBc::_shftim;
  for (i=0; i<4; i++) {
  _tdcbin[i]   = TOFDBc::_tdcbin[i];
  }
  _trigtb      = TOFDBc::_trigtb;
  _strflu      = TOFDBc::_strflu;

  for (i=0; i<15; i++) {
  _daqpwd[i]  = TOFDBc::_daqpwd[i];
  }
  _di2anr      = TOFDBc::_di2anr;
  _strrat      = TOFDBc::_strrat;
  _strjit1     = TOFDBc::_strjit1;
  _strjit2     = TOFDBc::_strjit2;
  _ftdelf      = TOFDBc::_ftdelf;
  _ftdelm      = TOFDBc::_ftdelm;

  _accdel      = TOFDBc::_accdel;

  _fstdcd      = TOFDBc::_fstdcd;
  _fatdcd      = TOFDBc::_fatdcd;

  _pbonup      = TOFDBc::_pbonup;

}
ooStatus TOFDBcD::CmpConstants() 
{
  integer i;
  ooStatus rstatus = oocSuccess;  
  
  cout<<"TOFDBcD::CmpConstants -I- start comparison "<<endl;

  for (i=0; i<SCBLMX; i++) {
    if (_brtype[i] != TOFDBc::_brtype[i]) {
      rstatus = oocError;
      Warning("TOFDBc::CmpConstants :_brtype is different ");
      cout<<"i,_brtype: dbase, mem "<<i<<", "<<_brtype[i]<<", "
          <<TOFDBc::_brtype[i]<<endl;
    }
   if (rstatus == oocError) break;
  }
  if (rstatus == oocError) {
    Warning("TOFDBc::CmpConstants :_brtype will be set as in dbase ");
    for (i=0; i<SCBLMX; i++) {TOFDBc::_brtype[i] = _brtype[i];}
    rstatus = oocSuccess;
  }


  for (i=0; i<SCLRS; i++) {
    if (_plrotm[i] != TOFDBc::_plrotm[i]) {
      rstatus = oocError;
      Warning("TOFDBc::CmpConstants :_plrotm is different ");
      cout<<"i, _plrotm: dbase, mem "<<i<<", "<<_plrotm[i]<<", "
          <<TOFDBc::_plrotm[i]<<endl;
    }
   if (rstatus == oocError) break;
  }
  if (rstatus == oocError) {
    Warning("TOFDBc::CmpConstants :_plrotm will be set as in dbase ");
    for (i=0; i<SCLRS; i++) {TOFDBc::_plrotm[i] = _plrotm[i];}
    rstatus = oocSuccess;
  }

  for (i=0; i<SCBTPN; i++) {
    if(_brlen[i] != TOFDBc::_brlen[i]) {
      rstatus = oocError;
      Warning("TOFDBc::CmpConstants :_brlen is different ");
      cout<<"i, _brlen: dbase, mem "<<i<<", "<<_brlen[i]<<", "
          <<TOFDBc::_brlen[i]<<endl;
    }
   if (rstatus == oocError) break;
  }
  if (rstatus == oocError) {
    Warning("TOFDBc::CmpConstants :_brlen will be set as in dbase ");
    for (i=0; i<SCBTPN; i++) {TOFDBc::_brlen[i] = _brlen[i];}
    rstatus = oocSuccess;
  }


  for (i=0; i<10; i++) {
    if (_supstr[i] != TOFDBc::_supstr[i]) {
      rstatus = oocError;
      Warning("TOFDBc::CmpConstants : _supstr is different ");
      cout<<"i, _supstr: dbase, mem "<<i<<", "<<_supstr[i]<<", "
          <<TOFDBc::_supstr[i]<<endl;
    }
    if (rstatus == oocError) break;
  }
  if (rstatus == oocError) {
    Warning("TOFDBc::CmpConstants :_supstr will be set as in dbase ");
    for (i=0; i<10; i++) {TOFDBc::_supstr[i] = _supstr[i];}
    rstatus = oocSuccess;
  }

  for (i=0; i<15; i++) {
    if (_plnstr[i] != TOFDBc::_plnstr[i]) {
      rstatus = oocError;
      Warning("TOFDBc::CmpConstants :_plnstr is different ");
      cout<<"i, _plnstr: dbase, mem "<<i<<", "<<_plnstr[i]<<", "
          <<TOFDBc::_plnstr[i]<<endl;
    }
    if (rstatus == oocError) break;
  }
  if (rstatus == oocError) {
    Warning("TOFDBc::CmpConstants :_plnstr will be set as in dbase ");
    for (i=0; i<15; i++) {TOFDBc::_plnstr[i] = _plnstr[i];}
    rstatus = oocSuccess;
  }


  if (_edep2ph != TOFDBc::_edep2ph) {
      rstatus = oocError;
      Error("TOFDBc::CmpConstants :_edep2ph is different ");
      cerr<<" _edep2ph: dbase, mem "<<_edep2ph<<", "<<TOFDBc::_edep2ph<<endl;
  }

  if (_seresp != TOFDBc::_seresp) {
      rstatus = oocError;
      Error("TOFDBc::CmpConstants :_seresp is different ");
      cerr<<" _seresp: dbase, mem "<<_seresp<<", "<<TOFDBc::_seresp<<endl;
  }

  if (_seresv != TOFDBc::_seresv) {
      rstatus = oocError;
      Error("TOFDBc::CmpConstants :_seresv is different ");
      cerr<<" _seresv: dbase, mem "<<_seresv<<", "<<TOFDBc::_seresv<<endl;
  }


  if (_fladctb     != TOFDBc::_fladctb) {
      rstatus = oocError;
      Error("TOFDBc::CmpConstants :_fladctb is different ");
      cerr<<" _faldctb: dbase, mem "<<_fladctb<<", "<<TOFDBc::_fladctb<<endl;
  }

  if (_shaptb      != TOFDBc::_shaptb) {
      rstatus = oocError;
      Error("TOFDBc::CmpConstants :_shaptb is different ");
      cerr<<" dbase, mem "<<_shaptb<<", "<<TOFDBc::_shaptb<<endl;
  }

  if (_shrtim      != TOFDBc::_shrtim) {
      rstatus = oocError;
      Error("TOFDBc::CmpConstants :_shrtim is different ");
      cerr<<" _shrtim: dbase, mem "<<_shrtim<<", "<<TOFDBc::_shrtim<<endl;
  }


  if (_shftim      != TOFDBc::_shftim) {
      rstatus = oocError;
      Error("TOFDBc::CmpConstants :_shftim is different ");
      cerr<<" _shftim: dbase, mem "<<_shftim<<", "<<TOFDBc::_shftim<<endl;
  }

  for (i=0; i<4; i++) {
    if (_tdcbin[i]   != TOFDBc::_tdcbin[i]) {
      rstatus = oocError;
      if (i==0) Error("TOFDBc::CmpConstants :_tdcbin is different ");
      cerr<<"i, dbase, mem "<<i<<", "<<_tdcbin[i]<<", "
          <<TOFDBc::_tdcbin[i]<<endl;
    }
  }

  if(_trigtb      != TOFDBc::_trigtb) {
      rstatus = oocError;
      if (i==0) Error("TOFDBc::CmpConstants :_trigtb is different ");
      cerr<<" _tdcbin: dbase, mem "<<_trigtb<<", "<<TOFDBc::_trigtb<<endl;
  }

  if (_strflu      != TOFDBc::_strflu) {
      rstatus = oocError;
      if (i==0) Error("TOFDBc::CmpConstants :_strflu is different ");
      cerr<<" _strflu: dbase, mem "<<_strflu<<", "<<TOFDBc::_strflu<<endl;
  }


  for (i=0; i<15; i++) {
    if (_daqpwd[i]  != TOFDBc::_daqpwd[i]) {
      rstatus = oocError;
      if (i==0) Error("TOFDBc::CmpConstants : _daqpwd is different ");
      cerr<<"i, _daqpwd: dbase, mem "<<i<<", "<<_daqpwd[i]<<", "
          <<TOFDBc::_daqpwd[i]<<endl;
   }
  }

  if (_di2anr      != TOFDBc::_di2anr) {
      rstatus = oocError;
      Error("TOFDBc::CmpConstants :_di2anr is different ");
      cerr<<" _di2anr: dbase, mem "<<_di2anr<<", "<<TOFDBc::_di2anr<<endl;
  }


  if (_strrat      != TOFDBc::_strrat) {
      rstatus = oocError;
      Error("TOFDBc::CmpConstants :_strrat is different ");
      cerr<<" _strrat: dbase, mem "<<_strrat<<", "<<TOFDBc::_strrat<<endl;
  }


  if (_strjit1      != TOFDBc::_strjit1) {
      rstatus = oocError;
      Error("TOFDBc::CmpConstants :_strjit1 is different ");
      cerr<<" _strrat: dbase, mem "<<_strjit1<<", "<<TOFDBc::_strjit1<<endl;
  }

  if (_strjit2      != TOFDBc::_strjit2) {
      rstatus = oocError;
      Error("TOFDBc::CmpConstants :_strjit2 is different ");
      cerr<<" _strrat: dbase, mem "<<_strjit2<<", "<<TOFDBc::_strjit2<<endl;
  }


  if (_ftdelf      != TOFDBc::_ftdelf) {
      rstatus = oocError;
      Error("TOFDBc::CmpConstants :_ftdelf is different ");
      cerr<<" _strrat: dbase, mem "<<_ftdelf<<", "<<TOFDBc::_ftdelf<<endl;
  }

  if (_ftdelm      != TOFDBc::_ftdelm) {
      rstatus = oocError;
      Error("TOFDBc::CmpConstants :_ftdelm is different ");
      cerr<<" _strrat: dbase, mem "<<_ftdelm<<", "<<TOFDBc::_ftdelm<<endl;
  }



  if (_accdel      != TOFDBc::_accdel) {
      rstatus = oocError;
      Error("TOFDBc::CmpConstants :_accdel is different ");
      cerr<<" _strrat: dbase, mem "<<_accdel<<", "<<TOFDBc::_accdel<<endl;
  }

  if (_fstdcd != TOFDBc::_fstdcd) {
      rstatus = oocError;
      Error("TOFDBc::CmpConstants :_fstdcd is different ");
      cerr<<" _strrat: dbase, mem "<<_fstdcd<<", "<<TOFDBc::_fstdcd<<endl;
  }

  if (_fatdcd != TOFDBc::_fatdcd) {
      rstatus = oocError;
      Error("TOFDBc::CmpConstants : _fatdcd is different ");
      cerr<<" _strrat: dbase, mem "<<_fatdcd<<", "<<TOFDBc::_fatdcd<<endl;
  }

  if (_pbonup != TOFDBc::_pbonup) {
      rstatus = oocError;
      Error("TOFDBc::CmpConstants :_pbonup is different ");
      cerr<<" _strrat: dbase, mem "<<_pbonup<<", "<<TOFDBc::_pbonup<<endl;
  }


  if (rstatus == oocSuccess)
   Message("TOFDBcD::CmpConstants : comparison done ");
  else
   Error("TOFDBcD::CmpConstants : comparison done. CONSTANTS ARE DIFFERENT ");

  return rstatus;

}
