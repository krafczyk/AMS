
#include <complex>
#include <sstream>

#include "TRD_GammaXTRadiator.hh"

#include "Randomize.hh"
#include "commonsi.h"
#include "commons.h"
#include "G4Gamma.hh"

using namespace std;

////////////////////////////////////////////////////////////////////////////
//
// Constructor, destructor

TRD_GammaXTRadiator::TRD_GammaXTRadiator(G4Region *anEnvelope,
					 G4double alphaPlate,
					 G4double alphaGas,
					 G4Material* foilMat,G4Material* gasMat, 
					 G4double a, G4double b, G4int n,
					 const G4String& processName) :
  TRD_VXTenergyLoss(anEnvelope,foilMat,gasMat,a,b,n,processName)
{
  if(fVerbose)G4cout<<"Gammma distributed X-ray TR radiator model is called"<<G4endl ;

  // Build energy and angular integral spectra of X-ray TR photons from
  // a radiator


  fAlphaPlate = alphaPlate ;
  fAlphaGas   = alphaGas   ;
  if(fVerbose)G4cout<<"fAlphaPlate = "<<fAlphaPlate<<" ; fAlphaGas = "<<fAlphaGas<<G4endl ;

  // BuildTable() ;

  // compose a more or less unique filename
  stringstream name;
  if(TRDMCFFKEY.debug>0){
    name << "table_" << processName << "_" 
	 << anEnvelope->GetName()
	 << "_" << alphaPlate << "_" 
	 << alphaGas << "_"
	 << foilMat->GetName() 
	 << "_" << gasMat->GetName() 
	 << "_" << a << "_" 
	 << b << "_" 
	 << n << ".txt";
      fFilename = "/scratch/mmilling/gbatch/g4/"+name.str();
  }
  else{
    //    name << "trd_gammaxtrad_table.txt";
    // MM 20111005
    name<<"table_GammaXTRadiator_5_0.66_0.012_0.1699_118.txt";
    //name<<"table_GammaXTRadiator_TrdRadRegion_5_0.66_TrdRadUtil_Galactic_0.012_0.1699_109.txt";
    
    fFilename = AMSDATADIR.amsdatadir+name.str();
  }

}

///////////////////////////////////////////////////////////////////////////

TRD_GammaXTRadiator::~TRD_GammaXTRadiator()
{
  ;
}



///////////////////////////////////////////////////////////////////////////
//
/// Rough approximation for radiator interference factor for the case of
/// fully GamDistr radiator. The plate and gas gap thicknesses are distributed 
/// according to exponent. The mean values of the plate and gas gap thicknesses 
/// are supposed to be about XTR formation zones but much less than 
/// mean absorption length of XTR photons in coresponding material.
G4double 
TRD_GammaXTRadiator::GetStackFactor( G4double energy, 
				     G4double gamma, G4double varAngle )
{
  G4double result, Za, Zb, Ma, Mb ;
  
  Za = GetPlateFormationZone(energy,gamma,varAngle) ;
  Zb = GetGasFormationZone(energy,gamma,varAngle) ;

  Ma = GetPlateLinearPhotoAbs(energy) ;
  Mb = GetGasLinearPhotoAbs(energy) ;


  G4complex Ca(1.0+0.5*fPlateThick*Ma/fAlphaPlate,fPlateThick/Za/fAlphaPlate) ; 
  G4complex Cb(1.0+0.5*fGasThick*Mb/fAlphaGas,fGasThick/Zb/fAlphaGas) ; 

  G4complex Ha = pow(Ca,-fAlphaPlate) ;  
  G4complex Hb = pow(Cb,-fAlphaGas) ;
  G4complex H  = Ha*Hb ;

  G4complex F1 =   (1.0 - Ha)*(1.0 - Hb )/(1.0 - H)
    * G4double(fPlateNumber) ;

  G4complex F2 =   (1.0-Ha)*(1.0-Ha)*Hb/(1.0-H)/(1.0-H)
    * (1.0 - pow(H,fPlateNumber)) ;

  G4complex R  = (F1 + F2)*OneInterfaceXTRdEdx(energy,gamma,varAngle) ;

  result       = 2.0*real(R) ;
  
  return      result ;
}


//
//
////////////////////////////////////////////////////////////////////////////
