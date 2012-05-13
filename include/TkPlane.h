// $Id: TkPlane.h,v 1.5 2012/05/13 21:59:38 pzuccon Exp $

#ifndef __TkPlane__
#define __TkPlane__

//////////////////////////////////////////////////////////////////////////
///
///
///\class TkPlane
///\brief An AMS Tracker class for the support planes specific information
///\ingroup tkdbc
///
/// An AMS Tracker class used to put the support planes specific 
/// information in the TKDBc
///
/// author P.Zuccon -- INFN Perugia 20/11/2007 
///
///\date  2008/01/17 PZ  First version
///\date  2008/01/23 SH  Some comments are added
///$Date: 2012/05/13 21:59:38 $
///
///$Revision: 1.5 $
///
//////////////////////////////////////////////////////////////////////////

#include "TkObject.h"

class TkPlane :public TkObject{

protected:
  

  virtual ostream& putout(ostream& s) const;
  virtual istream& putin(istream& s);

  virtual ostream& putoutA(ostream& s);
  virtual istream& putinA(istream& s);

  virtual ostream& putoutT(ostream& s);
  virtual istream& putinT(istream& s);

public:
  //! plane number
  short int _pnumber;
  //! number of available slots on the two faces [0] up [1] down
  short int _nslot[2];

  //! the Standard constructor. It put all the variables to 0
  TkPlane();
 
  //! explicit constructor. 
  /*! @param name the name of the plane object
    @param number the plane number
    @param slot  a integer array[2] containing the number of slots on the top face[0] and on bottom face[1] 
  */
  TkPlane(char* name,int number, short int nslot[2]);
  //! the standard destructor
  virtual ~TkPlane(){}

  
  //! operator to stream out (stdout or a file) the data of the class
  friend ostream &operator << (ostream &o,  TkPlane& b)
  {return b.putout(o);}
  
  //! operator to read the class data from an input  stream  (stdin or a file) 
  friend istream &operator >> (istream &o,  TkPlane& b)
  {return b.putin(o);}

  //! reads in the Alignement pars from file
  virtual void ReadA(istream &o){putinA(o);} 
  //! writes out the Alignement pars to file
  virtual void WriteA(ostream &o){putoutA(o);}

  //! reads in the MC (Dis)Alignement pars from file
  virtual void ReadT(istream &o){putinT(o);} 
  //! writes out the MC (Dis)Alignement pars to file
  virtual void WriteT(ostream &o){putoutT(o);}

  ClassDef(TkPlane,1);
};



// class TkPlaneExt :public TkPlane{

// private:
//   static int AlKind;
// #pragma omp threadprivate(AlKind)
  
//   AMSPoint  posMA;
//   AMSRotMat rotMA;
  
//   ostream& putout(ostream& s) const;
//   istream& putin(istream& s);

//   ostream& putoutA(ostream& s);
//   istream& putinA(istream& s);
  
//   AMSPoint& posvA(int kk){
//     return (kk==0)?posA: posMA;
//   }
//   AMSRotMat& rotvA(int kk){
//     return (kk==0)? rotA : rotMA;
//   }
// public:
//   //! the Standard constructor. It put all the variables to 0
//   TkPlaneExt():TkPlane(){
//   }

//   TkPlaneExt(const TkPlaneExt& orig):TkPlane(orig){
//     posMA=orig.posMA;
//     rotMA=orig.rotMA;
//   }
 
//   //! explicit constructor. 
//   /*! @param name the name of the plane object
//     @param number the plane number
//     @param slot  a integer array[2] containing the number of slots on the top face[0] and on bottom face[1] 
//   */
//   TkPlaneExt(char* name,int number, short int nslot[2]):TkPlane(name,number,nslot){
//   }

//   //! the standard destructor
//   ~TkPlaneExt(){
//   }
  
//   const AMSPoint&  GetPosA() const    {  return (AlKind==0)?posA: posMA;}
//   //! it returns a copy of the AMSRotMat defining Alignment correction to the orientation of the TkObject
//   const AMSRotMat& GetRotMatA() const {  return (AlKind==0)? rotA : rotMA;}
//   //! get the pointer to posA
//   AMSPoint&  UpdatePosA(int kind=-1){
//     int rr=0;
//     if(kind==-1)rr=AlKind;
//     else if(kind>0)  rr=1;
//     else rr=0;
//     return (posvA(rr));
//   }
//   //! get the pointer to rotA
//   AMSRotMat& UpdateRotA(int kind=-1) {
//     int rr=0;
//     if(kind==-1)rr=AlKind;
//     else if(kind>0)  rr=1;
//     else rr=0;
//     return (rotvA(rr));
//   }

//  //! Get the angles corresponding to te current  (aligment) rotation matrix according to a rotation sequence: alpha(XY) beta(XZ) gamma(YZ) all angles are increasing with the right hand notation
//   virtual void GetRotAnglesA(double& alpha, double& beta, double& gamma){
//     rotvA(AlKind).GetRotAngles(alpha, beta, gamma);
//   }

//   //! Create a rotation matrix(alignemnt) according to a rotation sequence: alpha(XY) beta(XZ) gamma(YZ) all angles are increasing with the right hand notation
//   virtual void SetRotAnglesA(double alpha, double beta, double gamma){
//     rotvA(AlKind).SetRotAngles(alpha, beta, gamma);
//   }
  
//   //! reads in the Alignement pars from file
//   void ReadA(istream &o){putinA(o);} 
//   //! writes out the Alignement pars to file
//   void WriteA(ostream &o){putoutA(o);}



//   TkPlaneExt& operator=(TkPlaneExt const& orig){
//     if (this == &orig) return *this;
//     TkPlane::operator=(orig);
//     posMA=orig.posMA;
//     rotMA=orig.rotMA;
//     return *this;
//   }
  


//   static void SetAlKind(int aa) {if(aa==0) AlKind==0;else AlKind==1; return;}
//   static int  GetAlKind() {return AlKind;}

//   ClassDef(TkPlaneExt,1);
// };

#endif
