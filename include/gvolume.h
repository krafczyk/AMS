// Author V. Choutko 24-may-1996
//
// Aug 08, 1996. ak.  First try with Objectivity.
//                    Add set, get functions and one more constructor
// Last Edit: Aug 08, 1996. ak.
// 
#ifndef __AMSGVOLUME__
#define __AMSGVOLUME__
#include <node.h>
#include <typedefs.h>
#include <string.h>
#include <cern.h>
#include <amsdbc.h>
#include <point.h>
class AMSgvolume : public AMSNode 
{
 protected:
   void _init();
   AMSgvolume (const AMSgvolume&);   // do not want cc
   AMSgvolume &operator=(const AMSgvolume&); // do not want ass

   integer _matter;     //   ! geant tracking med
   integer _rotmno;     //   ! geant rot matrix no
   integer _posp;       //   ! Pos by posp or by pos 
   integer _gid;        //   ! geant id
   integer _npar;       //   ! geant number of par     
   geant  _par[6];      //   ! geant volume par
   AMSPoint _coo;       //   ! geant volume relative coord
   AMSPoint _cooA;     //    ! geant volume abs coordinate
   number  _nrm[3][3];  //   ! normales  with resp to mother
   number  _inrm[3][3]; //   ! norm absolute
   char    _gonly[5];   //   ! should be 'MANY' or 'ONLY'
   char    _shape[5];   //   ! geant voulme shape

   integer _ContPos;

   virtual ostream & print(ostream &)const;

 public:
  static void amsgeom();
  AMSgvolume (): AMSNode(0){};


  AMSgvolume (integer matter,integer rotmno,const char name[],
           const char shape[] ,   geant par[] , integer npar,
            geant coo[] ,  number nrm[][3] , const char gonly[] ,
           integer posp,integer gid);

  AMSgvolume (char matter[], integer rotmno,const char name[], 
           const char shape[] ,   geant par[] , integer npar, 
            geant coo[] ,  number nrm[][3] , const char gonly[] , 
           integer posp,integer gid);
  number getinrm(integer i ,integer j)const{return _inrm[i][j];}
  number getcoo(integer i) {return _coo[i];}
  number getcooA(integer i){return _cooA[i];}
  number getpar(integer i)const {return i>=0 && i<_npar ? _par[i]:0;}
  AMSPoint gl2loc(AMSPoint);  // conversion global->local
  AMSPoint loc2gl(AMSPoint);  // conversion global->local
  AMSgvolume * next(){return (AMSgvolume *)AMSNode::next();}
  AMSgvolume * prev(){return (AMSgvolume *)AMSNode::prev();}
  AMSgvolume * up(){return   (AMSgvolume *)AMSNode::up();}
  AMSgvolume * down(){return (AMSgvolume *)AMSNode::down();}
  static integer debug;
  static geant dgeant;
//+
#ifdef __DB__
   friend class AMSgvolumeD;
#endif
void    setContPos(integer ContPos) {_ContPos = ContPos;}
integer getContPos()                {return _ContPos;}
   void getNumbers
  (integer* ibuff, geant* gbuff, AMSPoint* coo, number* nbuff0, number* nbuffi,
   char gonly[], char shape[])
{
  UCOPY( &_matter, ibuff, sizeof(integer)*5/4);
  UCOPY( &_par, gbuff, sizeof(geant)*6/4);
  UCOPY( &_coo, coo, sizeof(AMSPoint)*1/4);
  UCOPY( &_nrm, nbuff0, sizeof(number)*3*3/4);
  UCOPY( &_inrm,nbuffi, sizeof(number)*3*3/4);
  if(_shape)strcpy(shape,_shape);
  if(_gonly)strcpy(gonly,_gonly);
}
void setNumbers
  (integer* ibuff, geant* gbuff, AMSPoint* coo, number* nbuff0, number* nbuffi,
   const char name[], const char gonly[], const char shape[])
{
  UCOPY(ibuff, &_matter, sizeof(integer)*5/4);
  UCOPY(gbuff, &_par, sizeof(geant)*6/4);
  UCOPY(coo, &_coo, sizeof(AMSPoint)*1/4);
  UCOPY(nbuff0, &_nrm, sizeof(number)*3*3/4);
  UCOPY(nbuffi, &_inrm, sizeof(number)*3*3/4);
  if(shape)strcpy(_shape,shape);
  if(gonly)strcpy(_gonly,gonly);
  setname(name);
}
//-
};

#endif
