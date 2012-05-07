// $Id: TkLadder.h,v 1.12 2012/05/07 09:02:35 pzuccon Exp $

#ifndef __TkLadder__
#define __TkLadder__

//////////////////////////////////////////////////////////////////////////
///
///
///\class TkLadder
///\brief An AMS Tracker class for the ladder specific information
///\ingroup tkdbc
///
/// An AMS Tracker class used to put the ladder specific 
/// information in the TKDBc
///
/// author P.Zuccon -- INFN Perugia 20/11/2007 
///
///\date  2008/01/17 PZ  First version
///\date  2008/01/23 SH  Some comments are added
///\date  2008/03/17 SH  Some utils for MC geometry are added
///\date  2008/04/02 SH  Update for alignment correction
///$Date: 2012/05/07 09:02:35 $
///
///$Revision: 1.12 $
///
//////////////////////////////////////////////////////////////////////////
#include <cstdlib>
#include "TkObject.h"
class TkPlane;

namespace trconst{
  //! maximal number of sensors in one ladder
  const integer maxsen=15;
}

class TkLadder :public TkObject{

public:
  //                  0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
  static int pwg[24];
  //                   0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
  static int pwgp[24];
  //! function  needed  to guaarantee the virtual inheritance of the operator << ( you can safely ignore)
  ostream& putout(ostream& s) const;
  //! function  needed  to guaarantee the virtual inheritance of the operator >> ( you can safely ignore)
  istream& putin(istream& s);


  //! function  needed  to guaarantee the virtual inheritance of the operator << ( you can safely ignore)
  ostream& putoutA(ostream& s);
  //! function  needed  to guaarantee the virtual inheritance of the operator >> ( you can safely ignore)
  istream& putinA(istream& s);

  //! function  needed  to guaarantee the virtual inheritance of the operator << ( you can safely ignore)
  ostream& putoutT(ostream& s);
  //! function  needed  to guaarantee the virtual inheritance of the operator >> ( you can safely ignore)
  istream& putinT(istream& s);

  //! function  needed  to guaarantee the virtual inheritance of the operator << ( you can safely ignore)
  ostream& putoutS(ostream& s);
  //! function  needed  to guaarantee the virtual inheritance of the operator >> ( you can safely ignore)
  istream& putinS(istream& s);

private:
  //! the layer the ladder belongs to [1-8]
  int _layer;
  //! the slot on the layer [1-15]
  /*! the notation is that slot 1 has the largest Y coordinate. 
    A slot can contain up to two ladders which are disposed one 
    facing the other. Positive slot value means a ladder with 
    electronics on the positive X side and negative slot number
    means a ladder with electronics on the negative X side.
   */
  int _slot;
  //! The crate number the ladder is connected to
  int _crate;
  //! The TDR number within the crate
  int _tdr;
  //! The number of sensors composing the ladder(0xFF) 
  //! IsAlignment(LASER)  (0x800), IsK7 (0x400)
  int _nsensors;

public:
  //! version >=2 support sensor alignment (XYZ) TDV and file IO interface 
  //! version <2 old interface (before feb 2012)
  static int version;
  //! Sensor alignment correction (X in cm)
  float _sensx[trconst::maxsen];

  //! Sensor alignment correction (Y in cm)
  float _sensy[trconst::maxsen];
  //! Sensor alignment correction (Z in cm)
  float _sensz[trconst::maxsen];
  //! Default constructor set all properties to zero
  TkLadder();
  //! explicit constructor
  TkLadder(char* name,int layer,int slot,int crate,int tdr,int nsensors);
  //! explicit constructor with TkId= signof(slot) * (abs(slot)+100*layer) and HwId= tdr+ crate*100
  TkLadder(char* name,int TkId,int HwId,int nsensors);
  ~TkLadder(){}

  //! it returns the layer OLD scheme
  int GetLayer()const{return _layer;}
  //! it returns the layer J-scheme
  int GetLayerJ()const;
  //! it returns the pointer to the plane object
  TkPlane* GetPlane()const;
  
  int GetPlaneId()const{
    if(_layer==1) return 1;
    else if(_layer==2 || _layer==3) return 2;
    else if(_layer==4 || _layer==5) return 3;
    else if(_layer==6 || _layer==7) return 4;
    else if(_layer==8) return 5;
    else if(_layer==9) return 6;
    else
      return -1;
  }


  //! it returns the number of sensors
  int GetNSensors() {return _nsensors & 0xff;}
  //! it returns the slot
  int GetSlot()const{return _slot;}
  //! it returns the side. +1 positive X side  0 negative X side
  int GetSide()const{return  _slot>0?1:0;}
  //! it returns the octant in the Philippe notation
  int GetOctant();
  //! it returns the crate number
  int GetCrate()const{return _crate;}
  //! it returns the TDR number [0-23]
  int GetTdr()const{return _tdr;}
  //! it returns the TkId =  signof(slot) * (abs(slot)+100*layer)
  int GetTkId()const{return abs(_slot)/_slot*(abs(_slot)+100*_layer);}
  //! Returns the power group
  int GetPwGroup()const{return pwg[_tdr];}
  //! Returns the position within the power group
  int GetPwgroupPos()const{return pwgp[_tdr];}
  //! Returns the JMDC absolute node number
  int GetJMDCNum()const{ return 282+24*_crate+_tdr;}

  //! it returns the AssemblyId = the slot numbering in the assembly drawings + 100 * layer
  int GetAssemblyId()const{return abs(_slot)/_slot*( GetSide()*16 -_slot  +100*_layer);}
  //! it returns the HwId= tdr+ crate*100
  int GetHwId()const{return _crate*100+_tdr;}
  //! it returns PGid= pwgp*100+pwpos
  int GetPgId()const{return GetPwGroup()*100+GetPwgroupPos(); }


  //! Sets the Sensor align pars to XY Metrology data (dz=0)
  void ResetSensorAlign();

  //! it sets the Hwid
  void SetHwId(int HwId);
  //! it sets the TkId
  void SetTkId(int Tkid);
  //! returns true is the ladder is used in the laser alignment
  bool IsAlignemnt()const{return ((_nsensors  & 0x800)>0);}
  bool IsLaser()const{return ((_nsensors  & 0x800)>0);}
  //! sets the laser alignment flag
  void SetLaserFlag(){ _nsensors|=0x800;}

  //! Check if it is K7
  bool IsK7() {return ((_nsensors &0x400)>0);}
  //! Set if it is K7
  void SetAsK7() { _nsensors|=0x400;   }

  //! Check if it is an active ladder
  bool IsActive() { return (GetHwId()>=0); }

  void Print(Option_t* = "")const{ cout << putout(cout);}
  //! stream out the content of a the class
  friend ostream &operator << (ostream &o,  TkLadder& b)
  {return b.putout(o);}
  //! fill the class form a stream
  friend istream &operator >> (istream &o,  TkLadder& b)
  {return b.putin(o);}

  //! reads in the Alignement pars from file
  void ReadA(istream &o){putinA(o);} 
  //! writes out the Alignement pars to file
  void WriteA(ostream &o){putoutA(o);}

  //! reads in the Alignement pars from file
  void ReadT(istream &o){putinT(o);} 
  //! writes out the Alignement pars to file
  void WriteT(ostream &o){putoutT(o);}

  //! reads in the sensor Alignement pars from file
  void ReadS(istream &o){putinS(o);} 
  //! writes out the Alignement pars to file
  void WriteS(ostream &o){putoutS(o);}

  void Align2Lin(float * off);
  void Lin2Align(float * off);
  static int GetSize(){return (version<2)?6:6+trconst::maxsen*3;}
  ClassDef(TkLadder,5);
};
#endif

