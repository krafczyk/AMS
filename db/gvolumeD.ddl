// Author V. Choutko 24-may-1996
// 
// Aug 08, 1996. ak. First try with Objectivity
//                   Add _id and _name. The order of variables is changed
//
// Last Edit : Oct 10, 1996. ak.
// 

#include <iostream.h>
#include <string.h>
#include <cern.h>

#include <typedefs.h>
#include <point.h>
#include <gvolume.h>

class AMSgvolumeD : public ooObj 
{
 protected:

   integer _id;       //   ! get from AMSID
   integer _matter;   //   ! geant tracking med
   integer _rotmno;   //   ! geant rot matrix no
   integer _posp;     //   ! Pos by posp or by pos 
   integer _gid;      //   ! geant id
   integer _npar;     //   ! geant number of par     
   char    _gonly[5]; //   ! should be 'MANY' or 'ONLY'
   char    _shape[5]; //   ! geant voulme shape
   geant  _par[6];    //   ! geant volume par
   AMSPoint _coo;     //   ! geant volume coord
   number  _nrm[3][3];   //   ! normales  with resp to mother
   number  _inrm[3][3];  //   ! norm absolute

   integer _ContPos;  // position in the container
   integer _posN;     // pointer to the Next
   integer _posP;     //                Previous
   integer _posU;     //                Up
   integer _posD;     //                Down

 public:

   ooRef(AMSgvolumeD) _Next;
   ooRef(AMSgvolumeD) _Prev;
   ooRef(AMSgvolumeD) _Up;
   ooRef(AMSgvolumeD) _Down;

   char    _name[80]; //   ! get from AMSID

 // Constructors
 AMSgvolumeD();
 AMSgvolumeD  (integer id, AMSgvolume* p, char name[], integer pos);

 // Set/Get Methods
 void getnrm ( number* nbuff0);
 inline integer getid() {return _id;}
        char*   getname() {return _name;}
 inline integer getmatter() {return _matter;}
 inline integer getrotmno() {return _rotmno;}
 inline integer getgid()  {return _gid;}
 inline integer getposp() {return _posp;}
 inline integer getnpar() {return _npar;}
        void    getshape(char shape[]);
        void    getgonly(char gonly[]);
        void    getpar(geant* par);
        void    getcoo(AMSPoint* coo);

 inline integer getNext() {return _posN;}
 inline integer getPrev() {return _posP;}
 inline integer getUp()   {return _posU;}
 inline integer getDown() {return _posD;}
 inline void    setNext(integer pos) {_posN = pos;}
 inline void    setPrev(integer pos) {_posP = pos;}
 inline void    setUp(integer pos)   {_posU = pos;}
 inline void    setDown(integer pos) {_posD = pos;}
 inline integer getContPos()         {return _ContPos;}

};
