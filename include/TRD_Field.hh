#ifndef TRD_Field_h
#define TRD_Field_h
#include "globals.hh"
#include <G4ThreeVector.hh>
#include <TF3.h>

class TRD_Field
{

private:
  G4double ax,bx;
  G4double ay,by;
  G4double az,bz;
  G4double pcenter;
  TF3 *f;

public: 
  TRD_Field();
  ~TRD_Field();
  G4double  GetFieldValue( G4ThreeVector pos );

  void  InitFun();

  G4bool IsSet();

  void  SetFieldX( G4double ax_,
		   G4double bx_){ax=ax_;bx=bx_;}

  void  SetFieldY( G4double ay_,
		   G4double by_){ay=ay_;by=by_;}

  void  SetFieldZ( G4double az_,
		   G4double bz_){az=az_;bz=bz_;}

  void  SetFieldXa( G4double ax_){ax=ax_;}

  void  SetFieldYa( G4double ay_){ay=ay_;}

  void  SetFieldZa( G4double az_){az=az_;}

  void  SetFieldXb( G4double bx_){bx=bx_;}

  void  SetFieldYb( G4double by_){by=by_;}

  void  SetFieldZb( G4double bz_){bz=bz_;}

  void  SetFieldCenter(G4double pcent){pcenter=pcent;}

  G4double GetTmax();
  G4double GetTmin();
  G4double GetTcent();
  G4double GetTspan();
};

#endif
