//  $Id: trdsim.h,v 1.1 2001/04/27 21:50:33 choutko Exp $
#ifndef __AMSTRDSIM__
#define __AMSTRDSIM__

#include <trdid.h>
#include <mccluster.h>
#include <commons.h>
#include <link.h>
#include <cont.h>


class AMSTRDRawHit: public AMSlink {
AMSTRDIdSoft _id;  
uinteger _Amp;
protected:
void _init(){};
void _copyEl();
void _printEl(ostream & stream);
void _writeEl();

public:
AMSTRDRawHit( const AMSTRDIdSoft & id,uinteger amp):AMSlink(),_id(id),_Amp(amp){};
uinteger getid() const {return _id.cmpt();}
static integer Out(integer status);
AMSTRDIdSoft & getidsoft()  {return _id;}
number  Amp(){return number(_Amp)/TRDMCFFKEY.f2i;} 
integer operator < (AMSlink & o) const {
  AMSTRDRawHit * p= dynamic_cast<AMSTRDRawHit*>(&o);

  if (checkstatus(AMSDBc::USED) && !(o.checkstatus(AMSDBc::USED)))return 1;
  else if(!checkstatus(AMSDBc::USED) && (o.checkstatus(AMSDBc::USED)))return 0
;

  else return !p || _id.cmpta() < p->_id.cmpta();
}

 AMSTRDRawHit *  next(){return (AMSTRDRawHit*)_next;}   
//interface with mc
static void sitrddigi();
static void sitrdnoise();



//interface withdaq

static void builddaq(int n, int16u* p);
static void buildraw(int i, int n, int16u*p);
};


#endif
