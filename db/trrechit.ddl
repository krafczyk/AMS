// class AMSTrRecHit
// May  09, 1996. First try with Objectivity 
// Aug  22, 1996. Set unidirectional assoc to AMSEventD
//                change the place of members in the class
// Last Edit : Oct 10, 1996. ak.
//

#include <typedefs.h>
#include <point.h>
#include <trrec.h>
class AMSTrTrackD;
class AMSTrClusterD;
//class AMSEventD;

class AMSTrRecHitD : public ooObj {

 private:

   AMSPoint     _Hit;
   AMSPoint     _EHit;

   number       _Sum;
   number       _DifoSum;

   integer      _Status;
   integer      _Layer;
   integer      _Position;

 public:

// Assosiations
   ooRef(AMSTrClusterD) pClusterX     <-> pTrRecHitX[];
   ooRef(AMSTrClusterD) pClusterY     <-> pTrRecHitY[];
   ooRef(AMSTrTrackD)   pTrackH[]     <-> pTrRecHitT[];

// Constructor
  AMSTrRecHitD();
  AMSTrRecHitD(AMSTrRecHit* p);

// Get Methods
   integer  getStatus()   {return _Status;}
   integer  getLayer()    {return _Layer;}
   AMSPoint getHits()     {return _Hit;}
   AMSPoint getEHit()     {return _EHit;}
   number   getSum()      {return _Sum;}
   number   getDSum()     {return _DifoSum;}
   integer  getPosition() {return _Position;}
   void     copy(AMSTrRecHit* p);

// Set Methods
   void     setPosition(integer pos ) {_Position = pos;}

// Add Methods

};




