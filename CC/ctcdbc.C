#include <ctcdbc.h>
integer CTCDBc::_geomId=0;
integer CTCDBc::_nlay=0;
  number CTCDBc::getagsize(integer index) {
#ifdef __AMSDEBUG__
assert (index >=0 && index <=2);
#endif
return CTCGEOMFFKEY.agsize[index];
}
  number CTCDBc::getwlsth()  {return CTCGEOMFFKEY.wlsth;}
  integer CTCDBc::getnblk()   {return CTCGEOMFFKEY.nblk;}
  integer CTCDBc::getnwls()   {
    if(_geomId <2)return CTCGEOMFFKEY.nwls;
    else return getnagel();
  }
  void CTCDBc::setnwls(integer nwls) {CTCGEOMFFKEY.nwls=nwls;}
  number CTCDBc::getwallth()         {return CTCGEOMFFKEY.wallth;}
  number CTCDBc::getsupzc() {return CTCGEOMFFKEY.supzc;}
  number CTCDBc::getagap() {return CTCGEOMFFKEY.agap;}
  number CTCDBc::getwgap() {return CTCGEOMFFKEY.wgap;}
  number CTCDBc::getptfeth() {return CTCGEOMFFKEY.ptfx;}
  
  number CTCDBc::getptfesize(integer index) {
#ifdef __AMSDEBUG__
assert (index >=0 && index <=2);
#endif
return CTCGEOMFFKEY.ptfe[index];
}

 number CTCDBc::getthcsize(integer index) {
#ifdef __AMSDEBUG__
assert (index >=0 && index <=2);
#endif
return CTCGEOMFFKEY.thcsize[index];
}

 number CTCDBc::getupsize(integer index) {
#ifdef __AMSDEBUG__
assert (index >=0 && index <=2);
#endif
return CTCGEOMFFKEY.upper[index];
}
number CTCDBc::getwallsize(integer index) {
#ifdef __AMSDEBUG__
assert (index >=0 && index <=2);
#endif
return CTCGEOMFFKEY.wall[index];
} 
number CTCDBc::getcellsize(integer index) {
#ifdef __AMSDEBUG__
assert (index >=0 && index <=2);
#endif
return CTCGEOMFFKEY.cell[index];
} 
number CTCDBc::getygapsize(integer index) {
#ifdef __AMSDEBUG__
assert (index >=0 && index <=2);
#endif
return CTCGEOMFFKEY.ygap[index];
} 
number CTCDBc::getagelsize(integer index) {
#ifdef __AMSDEBUG__
assert (index >=0 && index <=2);
#endif
return CTCGEOMFFKEY.agel[index];
} 
number CTCDBc::getpmtsize(integer index) {
#ifdef __AMSDEBUG__
assert (index >=0 && index <=2);
#endif
return CTCGEOMFFKEY.pmt[index];
} 
  number CTCDBc::gethcsize(integer index) {
#ifdef __AMSDEBUG__
assert (index >=0 && index <=2);
#endif
return CTCGEOMFFKEY.hcsize[index];
  }

void CTCDBc::setgeom(integer iflag){
_geomId=iflag;
  if(iflag==2){
   CTCDBc::setnlay(1);
   CTCMCFFKEY.Edep2Phel[1]=0;
  }
  else if(iflag==1){
    CTCDBc::setnwls(CTCDBc::getnblk());
    CTCDBc::setnlay(2);     // Hardwired no of layers
  }
  else {
   CTCDBc::setnlay(1);
   CTCMCFFKEY.Edep2Phel[1]=0;
  }
}

  integer CTCDBc::getnagel()   {
   if(_geomId==0)return getnblk();
   else if(_geomId==1)return 2*getnblk();
   else return getnx()*getny();
  }

