// class AMSTrRecHit
// May  09, 1996. First try with Objectivity 
// Aug  22, 1996. Set unidirectional assoc to AMSEventD
//                change the place of members in the class
// Mar  07, 1997. use unidirectional assoc. between track and hits
//                use unidirectional assoc. between clustwer and hits.
//                add gid and name to get pointer to sensor in copy time
//
// Last Edit : Mar 19, 1997. ak.
//

#include <typedefs.h>
#include <point.h>
#include <trrec.h>

class AMSTrClusterD;

class AMSTrRecHitD : public ooObj {

 private:

   AMSPoint     _Hit;
   AMSPoint     _EHit;

   number       _Sum;
   number       _DifoSum;

   integer      _Status;
   integer      _Layer;
   integer      _Position;  

   integer      _gid;       // gvolume ID, from *sen
   char         _name[80];  // gvolume name, from *sen

 public:

// Assosiations
   ooRef(AMSTrClusterD) pClusterX : copy(delete);
   ooRef(AMSTrClusterD) pClusterY : copy(delete);

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
   integer  getgid()      {return _gid;}
   char*    getname()     {return _name;}

   void     copy(AMSTrRecHit* p);

// Set Methods
   void     setgid(integer gid) {_gid = gid;}
   void     setname(char* name) {if (name) strcpy (_name,name);}
   void     setPosition(integer pos ) {_Position = pos;}

// Add Methods

};




