// class AMSTrCluster
// May     1996. First try with Objectivity 
//               add AMSTrIdGeom.
//               AMSTrIdGeom = NULL
// Jun     1996. ak. Set/Get Numbers
// Aug     1996. ak. _Side (X/Y)
//                   change the place of members in the class
// Sep 10, 1996. ak. V1.25, _pValues
//
// Last Edit : Oct 10, 1996. ak
//

#include <typedefs.h>
#include <trid.h>
#include <trrec.h>

declare (ooVArray, number);

class AMSEventD;
class AMSTrRecHitD;

class AMSTrClusterD : public ooObj {

 private:

//   AMSTrIdSoft  _Id;

   number       _Sum;
   number       _Sigma;
   number       _Mean;
   number       _Rms;
   number       _ErrorMean;

   integer      _Status;
   integer      _Nelem;
   integer      _Position;
   integer      _Side;

   ooVArray(number) _pValues;

 protected:
   void         _printEl();

 public:

   static const integer BAD;
   static const integer WIDE;
   static const integer NEAR;

// Assosiations
   ooRef(AMSTrRecHitD) pTrRecHitX[] <-> pClusterX;
   ooRef(AMSTrRecHitD) pTrRecHitY[] <-> pClusterY;

// Constructor
  AMSTrClusterD();
  AMSTrClusterD(AMSTrCluster* p);
// Get Methods
//  AMSTrIdSoft  getid()   {return _Id;}
  integer  getstatus()   {return _Status;}
  integer  getnelem()    {return _Nelem;}
  number   getSigma()    {return _Sigma;}
  number   getRms()      {return _Rms;}
  number   getVal()      {return _Sum;}
  number   getcofg()     {return _Mean;}
  number   getecofg()    {return _ErrorMean;}
  integer  getPosition() {return _Position;}
  integer  getSide()     {return _Side;}
  void     copy(AMSTrCluster* p);
  void     getValues(number* values);
// Set Methods
  void     setPosition(integer pos) {_Position = pos;}
  void     setSide(integer side) {_Side = side;}
  void     setValues(number* values);

// Add Methods

};




