#ifndef __TRDHRec__
#define __TRDHRec__

#include "link.h"
#include "point.h"
#include "TrdHRecon.h"

/// interface class of TrdHSegmentR to AMS framework
class AMSTRDHSegment:public AMSlink,public TrdHSegmentR{ 
  public:
  void _init(){}
  void _printEl(ostream &o);
  void _copyEl();
  void _writeEl();

  static integer _addnext(integer d, float m, float em, float r, float er,float z, float w, integer nhits, TrdRawHitR* pthit[]);
  void _addnextR(uinteger iseg);

 AMSTRDHSegment(integer d_, float m_, float em_, float r_, float er_, float z_, float w_, integer nhits_, TrdRawHitR* pthit[])
   : AMSlink(),TrdHSegmentR(d_, m_, em_, r_, er_, z_ , w_, nhits_, pthit){};
  
 AMSTRDHSegment(TrdHSegmentR *seg)
   : AMSlink(),TrdHSegmentR(seg){};
  
 AMSTRDHSegment()
   : AMSlink(),TrdHSegmentR(){};
  
      
  integer operator < (AMSlink & o) const {
    AMSTRDHSegment * p= dynamic_cast<AMSTRDHSegment*>(&o);
    return !p||Chi2<p->Chi2;
  }

  AMSTRDHSegment *  next(){return (AMSTRDHSegment*)_next;}

  static AMSTRDHSegment * gethead(uinteger i=0);
  static integer Out(integer status);
};


/// interface class of TrdHTrackR to AMS framework
class AMSTRDHTrack:public AMSlink, public TrdHTrackR{
 protected:
  void _init(){}
  void _printEl(ostream &o);
  void _copyEl();
  void _writeEl();

 public:
  static integer _addnext(float pos[], float dir[], AMSTRDHSegment* pthit[]);
  void _addnextR(uinteger iseg);

 AMSTRDHTrack(TrdHTrackR *tr):AMSlink(),TrdHTrackR(tr){};

 integer operator < (AMSlink & o) const {
   AMSTRDHTrack * p= dynamic_cast<AMSTRDHTrack*>(&o);
   return !p||Chi2<p->Chi2;
 }
 
 const AMSPoint getCooStr()const {return AMSPoint(Coo[0],Coo[1],Coo[2]);}
 AMSTRDHTrack *  next(){return (AMSTRDHTrack*)_next;}

 static AMSTRDHTrack * gethead(uinteger i=0);
 static integer Out(integer status);
};

#endif
