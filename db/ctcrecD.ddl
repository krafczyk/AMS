// class AMSCTCClusterD
//
// Sep 27, 1996. First Objectivity version
//
// Last Edit: Apr 09, 1997 ak.
//
#include <typedefs.h>
#include <point.h>
#include <ctcrec.h>

class AMSCTCClusterD: public ooObj {
protected:

 AMSPoint _Coo;             // coordinates
 AMSPoint _ErrorCoo;        // Error to coordinates

 number   _Signal;          // signal in photoelectrons
 number   _CorrectedSignal; // corrected to cos (theta) signal
 number   _ErrorSignal;     // error in corrected signal to length and charge
                            // signal

 integer  _Layer;           // layer no.

 integer  _status;
 integer  _Position;        // position in the database container

public:

// Get/Set Methods
 inline void correct(number factor) {
               _CorrectedSignal=_CorrectedSignal*factor;
               _ErrorSignal=_ErrorSignal*factor;}

 number   getrawsignal()  const {return _Signal;}
 number   getsignal()     const {return _CorrectedSignal;}
 number   geterror()      const {return _ErrorSignal;}
 AMSPoint getcoo()        const {return _Coo;}
 AMSPoint getecoo()       const {return _ErrorCoo;}
 integer  getPosition()             {return _Position;}
 integer  getLayer()                {return _Layer;}

 integer  getstatus()              {return _status;}
 void     setPosition(integer pos ) {_Position = pos;}
 void     copy(AMSCTCCluster* p);


// Constructor
 AMSCTCClusterD();
 AMSCTCClusterD(AMSCTCCluster* p);
};

