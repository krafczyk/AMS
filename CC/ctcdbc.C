#include <ctcdbc.h>
integer CTCDBc::_geomId=0;
  number CTCDBc::getagsize(integer index) {
#ifdef __AMSDEBUG__
assert (index >=0 && index <=2);
#endif
return CTCGEOMFFKEY.agsize[index];
}
  number CTCDBc::getwlsth()  {return CTCGEOMFFKEY.wlsth;}
  integer CTCDBc::getnblk()   {return CTCGEOMFFKEY.nblk;}
  integer CTCDBc::getnwls()   {return CTCGEOMFFKEY.nwls;}
  void CTCDBc::setnwls(integer nwls)   {CTCGEOMFFKEY.nwls=nwls;}
  number CTCDBc::getwallth()   {return CTCGEOMFFKEY.wallth;}
  number CTCDBc::getsupzc()   {return CTCGEOMFFKEY.supzc;}
  number CTCDBc::getagap()   {return CTCGEOMFFKEY.agap;}
  number CTCDBc::getwgap()   {return CTCGEOMFFKEY.wgap;}
  number CTCDBc::gethcsize(integer index)  {
#ifdef __AMSDEBUG__
assert (index >=0 && index <=2);
#endif
return CTCGEOMFFKEY.hcsize[index];
}

