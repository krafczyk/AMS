// class AMSTrCluster
// May     1996. First try with Objectivity 
//               add AMSTrIdGeom.
//               AMSTrIdGeom = NULL
// Jun     1996. ak. Set/Get Numbers
// Aug     1996. ak. _Side (X/Y)
//                   change the place of members in the class
// Sep 10, 1996. ak. V1.25, _pValues
// Nov 15, 1996. al. NelemL, NelemR
// Mar 06, 1997. ak. up to date
//                   use uni directional assoc between cluster and hits
//                   add AMSTrIdSoft
//
// Last Edit : Apr 8, 1997. ak
//

#include <typedefs.h>
#include <trid.h>
#include <trrec.h>

declare (ooVArray, number);

class AMSTrClusterD : public ooObj {

 private:

   AMSTrIdSoft  _Id; // center
   number       _Sum;
   number       _Sigma;
   number       _Mean;
   number       _Rms;
   number       _ErrorMean;

   integer      _Status;
   integer      _NelemL;
   integer      _NelemR;
   integer      _Position;

   ooVArray(number) _pValues;

 protected:
   void         _printEl();

 public:

// Assosiations

// Constructor
  AMSTrClusterD();
  AMSTrClusterD(AMSTrCluster* p);

// Get Methods
  integer  getstatus()   {return _Status;}
  integer  getnelem()    {return -_NelemL + _NelemR;}
  integer  getnelemL()   {return _NelemL;}
  integer  getnelemR()   {return _NelemR;}
  number   getSigma()    {return _Sigma;}
  number   getRms()      {return _Rms;}
  number   getVal()      {return _Sum;}
  number   getcofg()     {return _Mean;}
  number   getecofg()    {return _ErrorMean;}
  integer  getPosition() {return _Position;}
  integer  getSide()     {return _Id.getside();}

  AMSTrIdSoft  getidsoft()   {return _Id;}

  void     copy(AMSTrCluster* p);
  void     getValues(number* values);

// Set Methods
  void     setPosition(integer pos) {_Position = pos;}
  void     setValues(number* values);

};




