// Oct 08, 1996. ak. first try with Objectivity
//                   method source file of object TOFDBcD
//
// Jan 21, 1997. ak. 
//                     TOFDBcD doens't match to the latest E.Ch. version
//
// Feb 28, 1997. ak.   TOFDBcD up to date
// Mar 24, 1997. ak.   CmpConstants is significantly modified
//
//Last Edit : Mar 24, 1997. ak.
//

#include <iostream.h>
#include <string.h>
#include <cern.h>

#include <tofdbc.h>
#include <tofdbcD.h>
//

TOFDBcD::TOFDBcD() 
{
  integer i;
  
  for (i=0; i<SCBLMX; i++) {
    _brtypeD[i] = TOFDBc::_brtype[i];
  }

  for (i=0; i<SCLRS; i++) {
    _plrotmD[i] = TOFDBc::_plrotm[i];
  }

  for (i=0; i<SCBTPN; i++) {
    _brlenD[i] = TOFDBc::_brlen[i];
  }

  for (i=0; i<10; i++) {
    _supstrD[i] = TOFDBc::_supstr[i];
  }

  for (i=0; i<15; i++) {
    _plnstrD[i] = TOFDBc::_plnstr[i];
  }


  _edep2phD = TOFDBc::_edep2ph;
  _fladctbD     = TOFDBc::_fladctb;
  _shaptbD      = TOFDBc::_shaptb;
  _shrtimD      = TOFDBc::_shrtim;
  _shftimD      = TOFDBc::_shftim;
  for (i=0; i<4; i++) {
  _tdcbinD[i]   = TOFDBc::_tdcbin[i];
  }
  _trigtbD      = TOFDBc::_trigtb;
  _strfluD      = TOFDBc::_strflu;
  for (i=0; i<2; i++) {
  _accdelD[i]   = TOFDBc::_accdel[i];
  }
  for (i=0; i<2; i++) {
  _accdelmxD[i] = TOFDBc::_accdelmx[i];
  }

  for (i=0; i<15; i++) {
  _daqpwdD[i]  = TOFDBc::_daqpwd[i];
  }
  _di2anrD      = TOFDBc::_di2anr;
  _strratD      = TOFDBc::_strrat;
}
ooStatus TOFDBcD::CmpConstants() 
{
  integer i;
  ooStatus rstatus = oocSuccess;  
  
  cout<<"TOFDBcD::CmpConstants -I- start comparison "<<endl;

  for (i=0; i<SCBLMX; i++) {
    if (_brtypeD[i] != TOFDBc::_brtype[i]) {
      rstatus = oocError;
      cout<<"TOFDBc::CmpConstants -I- _brtype is different "<<endl;
      cout<<"i,_brtype: dbase, mem "<<i<<", "<<_brtypeD[i]<<", "
          <<TOFDBc::_brtype[i]<<endl;
    }
   if (rstatus == oocError) break;
  }
  if (rstatus == oocError) {
    cout<<"TOFDBc::CmpConstants -I- _brtype will be set as in dbase "<<endl;
    for (i=0; i<SCBLMX; i++) {TOFDBc::_brtype[i] = _brtypeD[i];}
    rstatus = oocSuccess;
  }


  for (i=0; i<SCLRS; i++) {
    if (_plrotmD[i] != TOFDBc::_plrotm[i]) {
      rstatus = oocError;
      cout<<"TOFDBc::CmpConstants -I- _plrotm is different "<<endl;
      cout<<"i, _plrotm: dbase, mem "<<i<<", "<<_plrotmD[i]<<", "
          <<TOFDBc::_plrotm[i]<<endl;
    }
   if (rstatus == oocError) break;
  }
  if (rstatus == oocError) {
    cout<<"TOFDBc::CmpConstants -I- _plrotm will be set as in dbase "<<endl;
    for (i=0; i<SCLRS; i++) {TOFDBc::_plrotm[i] = _plrotmD[i];}
    rstatus = oocSuccess;
  }

  for (i=0; i<SCBTPN; i++) {
    if(_brlenD[i] != TOFDBc::_brlen[i]) {
      rstatus = oocError;
      cout<<"TOFDBc::CmpConstants -I- _brlen is different "<<endl;
      cout<<"i, _brlen: dbase, mem "<<i<<", "<<_brlenD[i]<<", "
          <<TOFDBc::_brlen[i]<<endl;
    }
   if (rstatus == oocError) break;
  }
  if (rstatus == oocError) {
    cout<<"TOFDBc::CmpConstants -I- _brlen will be set as in dbase "<<endl;
    for (i=0; i<SCBTPN; i++) {TOFDBc::_brlen[i] = _brlenD[i];}
    rstatus = oocSuccess;
  }


  for (i=0; i<10; i++) {
    if (_supstrD[i] != TOFDBc::_supstr[i]) {
      rstatus = oocError;
      cout<<"TOFDBc::CmpConstants -E- _supstr is different "<<endl;
      cout<<"i, _supstr: dbase, mem "<<i<<", "<<_supstrD[i]<<", "
          <<TOFDBc::_supstr[i]<<endl;
    }
    if (rstatus == oocError) break;
  }
  if (rstatus == oocError) {
    cout<<"TOFDBc::CmpConstants -I- _supstr will be set as in dbase "<<endl;
    for (i=0; i<10; i++) {TOFDBc::_supstr[i] = _supstrD[i];}
    rstatus = oocSuccess;
  }

  for (i=0; i<15; i++) {
    if (_plnstrD[i] != TOFDBc::_plnstr[i]) {
      rstatus = oocError;
      cout<<"TOFDBc::CmpConstants -I- _plnstr is different "<<endl;
      cout<<"i, _plnstr: dbase, mem "<<i<<", "<<_plnstrD[i]<<", "
          <<TOFDBc::_plnstr[i]<<endl;
    }
    if (rstatus == oocError) break;
  }
  if (rstatus == oocError) {
    cout<<"TOFDBc::CmpConstants -I- _plnstr will be set as in dbase "<<endl;
    for (i=0; i<15; i++) {TOFDBc::_plnstr[i] = _plnstrD[i];}
    rstatus = oocSuccess;
  }


  if (_edep2phD != TOFDBc::_edep2ph) {
      rstatus = oocError;
      cerr<<"TOFDBc::CmpConstants -E- _edep2ph is different "<<endl;
      cerr<<" _edep2ph: dbase, mem "<<_edep2phD<<", "<<TOFDBc::_edep2ph<<endl;
  }


  if (_fladctbD     != TOFDBc::_fladctb) {
      rstatus = oocError;
      cerr<<"TOFDBc::CmpConstants -E- _fladctb is different "<<endl;
      cerr<<" _faldctb: dbase, mem "<<_fladctbD<<", "<<TOFDBc::_fladctb<<endl;
  }

  if (_shaptbD      != TOFDBc::_shaptb) {
      rstatus = oocError;
      cerr<<"TOFDBc::CmpConstants -E- _shaptb is different "<<endl;
      cerr<<" dbase, mem "<<_shaptbD<<", "<<TOFDBc::_shaptb<<endl;
  }

  if (_shrtimD      != TOFDBc::_shrtim) {
      rstatus = oocError;
      cerr<<"TOFDBc::CmpConstants -E- _shrtim is different "<<endl;
      cerr<<" _shrtim: dbase, mem "<<_shrtimD<<", "<<TOFDBc::_shrtim<<endl;
  }

  //exit
  if (_shftimD      != TOFDBc::_shftim) {
      rstatus = oocError;
      cerr<<"TOFDBc::CmpConstants -E- _shftim is different "<<endl;
      cerr<<" _shftim: dbase, mem "<<_shftimD<<", "<<TOFDBc::_shftim<<endl;
  }

  for (i=0; i<4; i++) {
    if (_tdcbinD[i]   != TOFDBc::_tdcbin[i]) {
      rstatus = oocError;
      if (i==0) cerr<<"TOFDBc::CmpConstants -E- _tdcbin is different "<<endl;
      cerr<<"i, dbase, mem "<<i<<", "<<_tdcbinD[i]<<", "
          <<TOFDBc::_tdcbin[i]<<endl;
    }
  }

  if(_trigtbD      != TOFDBc::_trigtb) {
      rstatus = oocError;
      if (i==0) cerr<<"TOFDBc::CmpConstants -E- _trigtb is different "<<endl;
      cerr<<" _tdcbin: dbase, mem "<<_trigtbD<<", "<<TOFDBc::_trigtb<<endl;
  }

  if (_strfluD      != TOFDBc::_strflu) {
      rstatus = oocError;
      if (i==0) cerr<<"TOFDBc::CmpConstants -E- _strflu is different "<<endl;
      cerr<<" _strflu: dbase, mem "<<_strfluD<<", "<<TOFDBc::_strflu<<endl;
  }

  for (i=0; i<2; i++) {
    if (_accdelD[i]   != TOFDBc::_accdel[i]) {
      rstatus = oocError;
      if (i==0) cerr<<"TOFDBc::CmpConstants -E- _accdel is different "<<endl;
      cerr<<"i, _accdel: dbase, mem "<<i<<", "<<_accdelD[i]<<", "
          <<TOFDBc::_accdel[i]<<endl;
   }
  }


  for (i=0; i<2; i++) {
    if (_accdelmxD[i] != TOFDBc::_accdelmx[i]) {
      rstatus = oocError;
      if (i==0) cerr<<"TOFDBc::CmpConstants -E- _accdelmx is different "<<endl;
      cerr<<"i, _accdelmx: dbase, mem "<<i<<", "<<_accdelmxD[i]<<", "
          <<TOFDBc::_accdelmx[i]<<endl;
   }
  }


  for (i=0; i<15; i++) {
    if (_daqpwdD[i]  != TOFDBc::_daqpwd[i]) {
      rstatus = oocError;
      if (i==0) cerr<<"TOFDBc::CmpConstants -E- _daqpwd is different "<<endl;
      cerr<<"i, _daqpwd: dbase, mem "<<i<<", "<<_daqpwdD[i]<<", "
          <<TOFDBc::_daqpwd[i]<<endl;
   }
  }

  if (_di2anrD      != TOFDBc::_di2anr) {
      rstatus = oocError;
      cerr<<"TOFDBc::CmpConstants -E- _di2anr is different "<<endl;
      cerr<<" _di2anr: dbase, mem "<<_di2anrD<<", "<<TOFDBc::_di2anr<<endl;
  }


  if (_strratD      != TOFDBc::_strrat) {
      rstatus = oocError;
      cerr<<"TOFDBc::CmpConstants -E- _strrat is different "<<endl;
      cerr<<" _strrat: dbase, mem "<<_strratD<<", "<<TOFDBc::_strrat<<endl;
  }

 cout<<"TOFDBcD::CmpConstants -I- comparison done "<<endl;
 return rstatus;

}
