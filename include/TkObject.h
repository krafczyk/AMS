// $Id$

#ifndef __TkObject__
#define __TkObject__

//////////////////////////////////////////////////////////////////////////
///
///
///\class TkObject
///\brief An AMS Tracker class for the generic tracker object in the TKDBc
///\ingroup tkdbc
///
/// This class contains only the name of the object, 
/// an AMSPoint for the position of the object
/// and a AMSRotMat to define the position.
/// The TkObject class must not be used directly but instead you should
/// derive a new specific class for the object the you want to represent.
///
/// author P.Zuccon -- INFN Perugia 20/11/2007 
///
///\date  2008/01/17 PZ  First version
///\date  2008/01/23 SH  Some comments are added
///\date  2008/02/21 PZ  Updates for alignment correction
///\date  2008/04/02 SH  putin/putout updated for the alignment correction
///$Date$
///
///$Revision$
///
//////////////////////////////////////////////////////////////////////////

#include "point.h"
#include <iostream>
#include "TObject.h"

class TkObject : public TObject {

protected:
  //! function  needed  to guaarantee the virtual inheritance of the operator << ( you can safely ignore)
  virtual ostream& putout(ostream& s) const { return s<<name<<endl <<pos <<endl<<rot
						             <<posA<<endl<<rotA;}

  virtual ostream& putoutA(ostream& s) { return s<<name<<endl<<posA<<endl<<rotA;}
  virtual ostream& putoutT(ostream& s) { return s<<name<<endl<<posT<<endl<<rotT;}

  //! function  needed  to guaarantee the virtual inheritance of the operator >> ( you can safely ignore)
  virtual istream& putin(istream& s);

  //! function  needed  to guaarantee the virtual inheritance of the operator >> ( you can safely ignore)
  virtual istream& putinA(istream& s);

  //! function  needed  to guaarantee the virtual inheritance of the operator >> ( you can safely ignore)
  virtual istream& putinT(istream& s);

public:
  //! the name of the TkObject
  char name[50];
protected:
  //! the 3D position of the TkObject 
  AMSPoint   pos;
  //! the orientation in space of the TkObject
  AMSRotMat  rot;

  //! Alignement Correction to  3D position of the TkObject 
  AMSPoint   posA;
  //! Alignement Correction to  the orientation in space of the TkObject
  AMSRotMat  rotA;

  //! MC (Dis)Alignement Correction to  3D position of the TkObject 
  AMSPoint   posT;
  //! MC (Dis)Alignement Correction to  the orientation in space of the TkObject
  AMSRotMat  rotT;

public:
  //! explicit constructor
  TkObject(const char* namein,AMSPoint posin,AMSRotMat rotin);
  //! Standard constructor. It sets the position to (0,0,0) and rotmat to the identy matrix
  TkObject(const char* namein="");
  //! The destructor
  virtual ~TkObject(){}


  //! it returns a copy of the AMSPoint defining the position of the TkObject
  const AMSPoint&  GetPos()  const { return pos;}
  //! it returns a copy of the AMSRotMat defining the orientation of the TkObject
  const AMSRotMat& GetRotMat()const{ return rot;}
  //! get the pointer to pos
  AMSPoint&  UpdatePos(){return pos;}
  //! get the pointer to rot
  AMSRotMat& UpdateRot() {return rot;}


  virtual const AMSPoint&  GetPosA() const  { return posA;}
  //! it returns a copy of the AMSRotMat defining Alignment correction to the orientation of the TkObject
  virtual const AMSRotMat& GetRotMatA() const { return rotA;}
  //! get the pointer to posA
  virtual AMSPoint&  UpdatePosA(int kk=0){return posA;}
  //! get the pointer to rotA
  virtual AMSRotMat& UpdateRotA(int kk=0) {return rotA;}


  //! it returns a copy of the AMSPoint defining MC (Dis)Alignment correction to the position of the TkObject
  const AMSPoint&  GetPosT() const  { return posT;}
  //! it returns a copy of the AMSRotMat defining MC (Dis)Alignment correction to the orientation of the TkObject
  const AMSRotMat& GetRotMatT() const { return rotT;}
  //! get the pointer to posT
  AMSPoint&  UpdatePosT(){return posT;}
  //! get the pointer to rotT
  AMSRotMat& UpdateRotT() {return rotT;}



  //! Create a rotation matrix according to a rotation sequence: alpha(XY) beta(XZ) gamma(YZ) all angles are increasing with the right hand notation
  void SetRotAngles(double alpha, double beta, double gamma){
    rot.SetRotAngles(alpha, beta, gamma);
  }
  //!copy the rot mat to a 3x3 array
  void RotToMat(number nrm[][3]);


  //! Create a rotation matrix( MC (Dis)alignemnt) according to a rotation sequence: alpha(XY) beta(XZ) gamma(YZ) all angles are increasing with the right hand notation
  void SetRotAnglesT(double alpha, double beta, double gamma){
    rotT.SetRotAngles(alpha, beta, gamma);
  }
  
  //! Create a rotation matrix(alignemnt) according to a rotation sequence: alpha(XY) beta(XZ) gamma(YZ) all angles are increasing with the right hand notation
  virtual void SetRotAnglesA(double alpha, double beta, double gamma){
    rotA.SetRotAngles(alpha, beta, gamma);
  }
  

  //! Get the angles corresponding to te current rotation matrix according to a rotation sequence: alpha(XY) beta(XZ) gamma(YZ) all angles are increasing with the right hand notation
  void GetRotAngles(double& alpha, double& beta, double& gamma){
    rot.GetRotAngles(alpha, beta, gamma);
  }

  //! Get the angles corresponding to te current  ( MC (Dis)aligment) rotation matrix according to a rotation sequence: alpha(XY) beta(XZ) gamma(YZ) all angles are increasing with the right hand notation
  void GetRotAnglesT(double& alpha, double& beta, double& gamma){
    rotT.GetRotAngles(alpha, beta, gamma);
  }

  //! Get the angles corresponding to te current  (aligment) rotation matrix according to a rotation sequence: alpha(XY) beta(XZ) gamma(YZ) all angles are increasing with the right hand notation
  virtual void GetRotAnglesA(double& alpha, double& beta, double& gamma){
    rotA.GetRotAngles(alpha, beta, gamma);
  }
  
  //! operator to stream out (stdout or a file) the data of the class
  friend ostream &operator << (ostream &o,  TkObject& b)
  {return b.putout(o);}
  
  //! operator to read the class data from an input  stream  (stdin or a file) 
  friend istream &operator >> (istream &o,  TkObject& b)
  {return b.putin(o);}
  
  void ReadA(istream &o){putinA(o);}
  void WriteA(ostream &o){putoutA(o);}

  void ReadT(istream &o){putinT(o);}
  void WriteT(ostream &o){putoutT(o);}
  void PrintT();

  void Align2Lin(float * off);
  void Lin2Align(float * off);
  void MCAlign2Lin(float * off);
  void Lin2MCAlign(float * off);
  static int GetSize(){return 6;}

  ClassDef(TkObject,2);
};




#endif


