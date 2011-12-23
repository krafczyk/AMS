//--------------------------------------------------------------------------------------------------
/// TrdSCalib.C
// 
//  Source file of AC_TrdHits and TrdSCalibR class
//  2011.09.11 first results prestend by Prof.Schael(RWTH Aachen) at AMS meeting  
//  2011.09.23 imported codes from Aachen calibration 
// 
//  Updated:
//  2011.10.11 compatible with TrdTrack and TrdHTrack
//  2011.10.28 FirstCalRunTime and LastCalRunTime
//  2011.11.02 Ver.1 validation results are posted on
//             http://accms04.physik.rwth-aachen.de/~chchung/HowtoUseTrdSCalib/
//  2011.11.07 Init(int CalibLevel, int Debug) for short initialization
//  2011.11.08 released version 2 for new PDFs using TGraphSmooth 
//  2011.11.15 public members: Lprod_Proton, Lprod_Helium, Lprod_Electron
//  2011.12.23 released version 3 for new calibration data 
//             period: 1305815610 - 1324099094
//  2011.12.23 BuildTrdSCalib included in the gbatch(ParticleR) 
//--------------------------------------------------------------------------------------------------

#include "TrdSCalib.h"

#include "root.h"
#include "timeid.h"
#include "amsdbc.h"

ClassImp(AC_TrdHits);
ClassImp(TrdSCalibR);

TrdSCalibR *TrdSCalibR::head=0;

const char *TrdSCalibR::TrdDBUpdateDir[] =
  {
    "/afs/cern.ch/exp/ams/Offline/AMSDataDir/DataBase/TrdCalNew",
    "/afs/ams.cern.ch/user/chchung/CAL"
  };

const char *TrdSCalibR::TrdCalDBUpdate[] =
  {
    "TrdGasCirCalibration_v02.root",
    "TrdModuleCalibration_Calib1_v09.root",
    /// version 3 newly generated on 2011.12.19
    "TrdCalibration_v03.root",
    /// version 4 newly generated on 2011.12.19
    "TrdCalibration_v04.root"
  };

const char *TrdSCalibR::TrdElectronProtonHeliumLFname[] =
  {
    /// version 1: separate LFs
    "TrdElectronLF_v08.root", "TrdProtonLF_v08.root", "TrdHeliumLF_v08.root", 
    /// version 2: combined LF 
    "TrdLikelihood_v09.root",
    /// version 3 newly generated on 2011.12.19
    "TrdLikelihood_Scalib-03_SLR-10.root",
    /// version 4 newly generated on 2011.12.19
    "TrdLikelihood_Scalib-04_SLR-10.root"
  };

const char *TrdSCalibR::TrdLLname[] =
  {
    "Likelihood(Electron/Proton)", "Likelihood(Helium/Proton)", "Likelihood(Electron/Helium)"
  };

const char *TrdSCalibR::TrdSum8name[] = {"TrdSum8Raw", "TrdSum8New"};

const char *TrdSCalibR::EvtPartID[]   = {"Proton", "Helium","Electron","Positron","AntiProton"};

const char *TrdSCalibR::TrdTrackTypeName[] = {"TrdHTrack", "TrdTrack"};
    

//--------------------------------------------------------------------------------------------------

void AC_TrdHits::PrintTrdHit(Option_t *) const{
  std::cout << Form("Lad=%5d Lay=%5d Tub=%5d Mod=%5d Straw=%d GC=%5d ",
	       Lad, Lay, Tub, Mod, Straw, GC);
  std::cout << std::endl;

  std::cout << Form("EadcR=%5.1f, EadcCM=%5.1f, EadcC1=%5.1f, EadcCS=%5.1f,",
		    EadcR, EadcCM, EadcC1, EadcCS);
  
  std::cout << Form("TrdD=%5.1f, TrkD=%5.1f, Len2D=%5.1f, Len3D=%5.1f",
		    TrdD, TrkD, Len2D, Len3D);

  std::cout << std::endl;

};
//--------------------------------------------------------------------------------------------------

vector<int> TrdSCalibR::nTrdModulesPerLayer(20);
vector< vector<int> > TrdSCalibR::mStraw(18, vector<int>(20)); 

vector<int> TrdSCalibR::aP(328);   

void TrdSCalibR::GenmStrawMatrix(int Debug) {
  std::cout << "TrdSCalib:: Generate mStraw-Matrix." << std::endl;
  
  int iLay0 = 0;
  for (int iLay = iLay0; iLay<iLay0+4; iLay++) {
    nTrdModulesPerLayer[iLay] = 14;
    int n = iLay0 + iLay;
    for (int iLadr=0; iLadr<14; iLadr++) {
      if (iLadr>=0 && iLadr<trdconst::nTrdLadders && iLay>=0 && iLay< trdconst::nTrdLayers) 
	mStraw[iLadr][iLay] =   n;
      n += 4;
    }
  }

  iLay0 = 4;
  for (int iLay = iLay0; iLay<iLay0+4; iLay++) {
    nTrdModulesPerLayer[iLay] = 16;
    int n = (56-iLay0) + iLay;
    for (int iLadr=0; iLadr<16; iLadr++) {
      if (iLadr>=0 && iLadr<trdconst::nTrdLadders && iLay>=0 && iLay<trdconst::nTrdLayers) 
	mStraw[iLadr][iLay] =   n;
      n += 4;
    }
  }

  iLay0 = 8;
  for (int iLay = iLay0; iLay<iLay0+4; iLay++) {
    int n = (120-iLay0) + iLay;
    nTrdModulesPerLayer[iLay] = 16;
    for (int iLadr=0; iLadr<16; iLadr++) {
      if (iLadr>=0 && iLadr<trdconst::nTrdLadders && iLay>=0 && iLay<trdconst::nTrdLayers) 
	mStraw[iLadr][iLay] =   n;
      n += 4;
    }
  }
  
  iLay0 = 12;
  for (int iLay = iLay0; iLay<iLay0+4; iLay++) {
    int n = (184-iLay0) + iLay;
    nTrdModulesPerLayer[iLay] = 18;
    for (int iLadr=0; iLadr<18; iLadr++) {
      if (iLadr>=0 && iLadr<trdconst::nTrdLadders && iLay>=0 && iLay<trdconst::nTrdLayers) 
	mStraw[iLadr][iLay] =   n;
      n += 4;
    }
  }
  
  iLay0 = 16;
  for (int iLay = iLay0; iLay<iLay0+4; iLay++) {
    int n = (256-iLay0) + iLay;
    nTrdModulesPerLayer[iLay] = 18;
    for (int iLadr=0; iLadr<18; iLadr++) {
      if (iLadr>=0 && iLadr<trdconst::nTrdLadders && iLay>=0 && iLay<trdconst::nTrdLayers) 
	mStraw[iLadr][iLay] =   n;
      n += 4;
    }
  }
  if(Debug > 1)
    std::cout << "nTrdModulesPerLayer[19]= " << nTrdModulesPerLayer[19] 
	      << " mStraw[15][19] = " << mStraw[15][19]  << std::endl;
  
};
//--------------------------------------------------------------------------------------------------
/// Straw 0 - 5247
int AC_TrdHits::GetTrdStrawNumber(TrdRawHitR *rhit) {
  static int straw   = -1;
  int module  = GetTrdModule(rhit->Layer, rhit->Ladder);
  
  if(module >= 0 && module < nTrdModules)
    straw = module*16 + rhit->Tube;
  
  return straw;
}

int AC_TrdHits::GetTrdStrawNumber() {
  static int straw   = -1;
  int module  = GetTrdModule(Lay, Lad);
  
  if(module >= 0 && module < nTrdModules)
    straw = module*16 + Tub;
  
  return straw;
}

//--------------------------------------------------------------------------------------------------
/// Module 0 -327
int AC_TrdHits::GetTrdModule(int layer, int ladder) {     
 
  static int module 	= -1;
  
  if (layer<0 || layer>=trdconst::nTrdLayers || ladder<0 || ladder>=trdconst::nTrdLadders) 
    std::cerr << "Error  GetTrdModule: Lay=" << layer 
	      << " Lad = " << ladder << std::endl;
  else {
    module 	= TrdSCalibR::mStraw[ladder][layer];
    if (module<0 || module>=328) 
      std::cerr << "Error  GetTrdModule: Lay=" << layer 
		<< " Lad = " << ladder << std::endl;
  }
  
  return module; 
}

int AC_TrdHits::GetTrdModule() {     
  
  static int module 	= -1;
  
  if (Lay<0 || Lay>=trdconst::nTrdLayers || Lad<0 || Lad>=trdconst::nTrdLadders) 
    std::cerr << "Error  GetTrdModule: Lay=" << Lay 
	      << " Lad = " << Lad << std::endl;
  else {
    module 	= TrdSCalibR::mStraw[Lad][Lay];
    if (module<0 || module>=328) 
      std::cerr << "Error  GetTrdModule: Lay=" << Lay 
		<< " Lad = " << Lad << std::endl;
  }
  
  return module; 
}

//--------------------------------------------------------------------------------------------------
int AC_TrdHits::TrdStraw2Module(int Straw) { 
  return Straw/nTrdTubes; 
}

//--------------------------------------------------------------------------------------------------
/// Straw 0 - 5247 => Lay 0 - 19
int AC_TrdHits::TrdStraw2LayLad(int Straw, int &Lad, int &Lay) {	
  Lad = -1;
  Lay = -1;
 
  int Module = TrdStraw2Module(Straw);

  for (int lay=nTrdLayers-1; lay>=0; lay--) {
    if (Module < TrdSCalibR::mStraw[0][lay]) continue;
    for (int lad=0; lad<TrdSCalibR::nTrdModulesPerLayer[lay]; lad++) {
      if (Module == TrdSCalibR::mStraw[lad][lay]) {
	Lay = lay;
	Lad = lad;
	break;
      }
    }
    if (Lay>=0) break;
  }
	
  if (Lay>=0) return 0;
  else return -1;
}

//--------------------------------------------------------------------------------------------------
/// Straw 0 - 5247 => Lay 0 - 19
int AC_TrdHits::TrdStraw2Layer(int Straw) {
  static int Lad = -1;
  static int Lay = -1;
  TrdStraw2LayLad(Straw, Lad, Lay);
  return Lay;
}

//--------------------------------------------------------------------------------------------------
/// Straw 0 - 5247 => Lad 0 - 17
int AC_TrdHits::TrdStraw2Ladder(int Straw) { 
  static int Lad = -1;
  static int Lay = -1;
  TrdStraw2LayLad(Straw, Lad, Lay);
  return Lad;
}

//--------------------------------------------------------------------------------------------------
/// TRD Gas Group from 0-40
int AC_TrdHits::GetTrdGasGroup(int layer, int ladder) {
  static int GasGroup = -1;
  if (layer>19)	{
    std::cerr << "Error  GetTrdGasGroup: layer 19+ found! layers should be 0-19" << std::endl;
    return GasGroup;
  }
  int templadder = ladder;
  
  if (layer>3) 	{
    templadder+=14;
    if (layer>7) 	{
      templadder+=16;
      if (layer>11) {
	templadder+=16;
	if (layer>15) {
	  templadder+=18;
	}
      }
    }
  }
  
  if (templadder%2==1) {
    GasGroup =(templadder-1)/2;
  }	else {
    GasGroup =templadder/2;
  }
  
  return GasGroup;
}

int AC_TrdHits::GetTrdGasGroup(void) {
  static int GasGroup = -1;
  if (Lay>19)	{
    std::cerr << "Error  GetTrdGasGroup: layer 19+ found! layers should be 0-19" << std::endl;
    return GasGroup;
  }
  int templadder = Lad;
  
  if (Lay>3) 	{
    templadder+=14;
    if (Lay>7) 	{
      templadder+=16;
      if (Lay>11) {
	templadder+=16;
	if (Lay>15) {
	  templadder+=18;
	}
      }
    }
  }
  
  if (templadder%2==1) {
    GasGroup =(templadder-1)/2;
  }	else {
    GasGroup =templadder/2;
  }

  return GasGroup;
}



//--------------------------------------------------------------------------------------------------
///TRD Gas Circuits from 0-9
int  AC_TrdHits::GetTrdGasCircuit(int layer, int ladder) {
  int iGasGroup = GetTrdGasGroup(layer,ladder) + 1;
  int iGasCir= 0;
  
  // + y-Side, Wall 3
  if (iGasGroup==39 || iGasGroup==29 || iGasGroup==16 || iGasGroup==14) {
    iGasCir = 1;
  } else if (iGasGroup==41 || iGasGroup==18 || iGasGroup==12 || iGasGroup==6) {
    iGasCir = 2;
  } else if (iGasGroup==31 || iGasGroup==20 || iGasGroup==10 || iGasGroup==4) {
    iGasCir = 3;
  } else if (iGasGroup==38 || iGasGroup==27 || iGasGroup==8 || iGasGroup==7) {
    iGasCir = 4;
  } else if (iGasGroup==40 || iGasGroup==25 || iGasGroup==22 || iGasGroup==5) {
    iGasCir = 5;
    
    // -y Side, Wall 7	
  } else if (iGasGroup==30 || iGasGroup==26 || iGasGroup==11 || iGasGroup==3) {
    iGasCir = 6;
  } else if (iGasGroup==36 || iGasGroup==24 || iGasGroup==17 || iGasGroup==13) {
    iGasCir = 7;
  } else if (iGasGroup==34 || iGasGroup==33 || iGasGroup==28 || iGasGroup==23 || iGasGroup==9) {
    iGasCir = 8;		
  } else if (iGasGroup==37 || iGasGroup==21 || iGasGroup==15 || iGasGroup==2) {
    iGasCir = 9;
  } else if (iGasGroup==35 || iGasGroup==32 || iGasGroup==19 || iGasGroup==1) {
    iGasCir = 10;		
  } else {
    std::cerr << "Error in GetTrdGasCircuit: Unkown Gas Circuit " << iGasGroup
	      << "  for Layer= " << layer 
	      << "  Ladder= "    << ladder << std::endl;
  }
  return iGasCir-1;
}
//--------------------------------------------------------------------------------------------------
int  AC_TrdHits::GetTrdGasCircuit(void) {
  int iGasGroup = GetTrdGasGroup() + 1;
  int iGasCir= 0;
  
  // + y-Side, Wall 3
  if (iGasGroup==39 || iGasGroup==29 || iGasGroup==16 || iGasGroup==14) {
    iGasCir = 1;
  } else if (iGasGroup==41 || iGasGroup==18 || iGasGroup==12 || iGasGroup==6) {
    iGasCir = 2;
  } else if (iGasGroup==31 || iGasGroup==20 || iGasGroup==10 || iGasGroup==4) {
    iGasCir = 3;
  } else if (iGasGroup==38 || iGasGroup==27 || iGasGroup==8 || iGasGroup==7) {
    iGasCir = 4;
  } else if (iGasGroup==40 || iGasGroup==25 || iGasGroup==22 || iGasGroup==5) {
    iGasCir = 5;
    
    // -y Side, Wall 7	
  } else if (iGasGroup==30 || iGasGroup==26 || iGasGroup==11 || iGasGroup==3) {
    iGasCir = 6;
  } else if (iGasGroup==36 || iGasGroup==24 || iGasGroup==17 || iGasGroup==13) {
    iGasCir = 7;
  } else if (iGasGroup==34 || iGasGroup==33 || iGasGroup==28 || iGasGroup==23 || iGasGroup==9) {
    iGasCir = 8;		
  } else if (iGasGroup==37 || iGasGroup==21 || iGasGroup==15 || iGasGroup==2) {
    iGasCir = 9;
  } else if (iGasGroup==35 || iGasGroup==32 || iGasGroup==19 || iGasGroup==1) {
    iGasCir = 10;		
  } else {
    std::cerr << "Error in GetTrdGasCircuit: Unkown Gas Circuit " << iGasGroup
	      << "  for Layer= " << Lay 
	      << "  Ladder= "    << Lad << std::endl;
  }
  return iGasCir-1;
}

//--------------------------------------------------------------------------------------------------
/// return correction in cm
double  AC_TrdHits::GetTrdModuleAlignmentCorrection(int Layer, int Module) {
  static double  Corr 	= 0.0;

  // Trd Alignment relative to tracker
  if (Layer>=0 && Layer<4) {
    Corr = -0.03127;
  } else if (Layer>=4 && Layer<16) {
    Corr = -0.05385;			
  } else if (Layer>=16 && Layer<20) {
    Corr = -0.01453;			
  }
  
  if (Module>=0 && Module<nTrdModules) 
    Corr += TrdCorr[Module]/10.0;
  return Corr;
}
//--------------------------------------------------------------------------------------------------
double AC_TrdHits::DistanceFromLine(double cx, double cy, 
				    double ax, double ay, 
				    double bx, double by) {

  double r_numerator   = (cx-ax)*(bx-ax) + (cy-ay)*(by-ay);
  double r_denomenator = (bx-ax)*(bx-ax) + (by-ay)*(by-ay);
  double r 	       	= r_numerator / r_denomenator;
  
  double px = ax + r*(bx-ax);
  double py = ay + r*(by-ay);
  
  double s  = ((ay-cy)*(bx-ax)-(ax-cx)*(by-ay) ) / r_denomenator;
  
  double distanceLine = std::fabs(s)*sqrt(r_denomenator);
  
  if (px>cx) distanceLine *= -1;
  
  return distanceLine;
};

//--------------------------------------------------------------------------------------------------
double AC_TrdHits::GetTrdHitTrkDistance(int d, float xy, float z, AMSPoint cPtrk, AMSDir dPtrk) {	
  static double Distance = -1.0;
 
  double   xl 	= (z + 20.0 - cPtrk[2]) / dPtrk[2];
  
  AMSPoint bPtrk;
  bPtrk[0] = cPtrk[0] + xl*dPtrk[0];
  bPtrk[1] = cPtrk[1] + xl*dPtrk[1];
  bPtrk[2] = cPtrk[2] + xl*dPtrk[2];

  if (d==0) {             /// xz plane
    Distance = DistanceFromLine(xy,z,cPtrk[0],cPtrk[2],bPtrk[0],bPtrk[2]);
  } else if (d==1) {      /// yz plane
    Distance = DistanceFromLine(xy,z,cPtrk[1],cPtrk[2],bPtrk[1],bPtrk[2]);
  }
  return Distance;
};
//--------------------------------------------------------------------------------------------------
double AC_TrdHits::GetTrdPathLen3D(int lay, float xy, float z, AMSPoint cP, AMSDir dP) {

  TVector3 c(cP[0], cP[1], cP[2]); 
  TVector3 d(dP[0], dP[1], dP[2]);
  
  TVector3  v(0.0), q(0.0);
  if( (lay >= 16) || (lay <= 3) ) {
    ///Measure Y-Coordinate, Straws are running along X
    v.SetXYZ(0, xy, z);
    q.SetXYZ(1.0, 0.0, 0.0);
  } else {
    ///Measure X-Coordinate, Straws are running along Y
    v.SetXYZ(xy, 0.0, z);
    q.SetXYZ(0.0, 1.0, 0.0);
  }
  TVector3 a = c-v;
  TVector3 e = d - (d*q) * q;
  TVector3 f = a - (a*q) * q;
  
  double A = e*e;
  double B = 2.0*(e*f);
  double C = (f*f) - TMath::Power(trdconst::TrdStrawRadius,2);
  
  /// solve A*x^2 + B*x + C = 0.0
  double xr = B*B - 4.0*A*C;
  double x1 = 0.0, x2 = 0.0;
  if (xr>=0.0) {
    x1 = (-B - TMath::Sqrt(xr))/(2.0*A);
    x2 = (-B + TMath::Sqrt(xr))/(2.0*A);
  }
  TVector3 	P1 		= c + x1*d;
  TVector3 	P2 		= c + x2*d;
  TVector3	Dist 		= P1 - P2;
  double        len3d		= Dist.Mag(); 
  

  if (len3d<=0.0) return -1.0;
  
  return len3d;
}



//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
///(Z= upper TOF)
bool TrdSCalibR::GetcTrd(TrdHTrackR *trdht) {
  if(! trdht) return false; 
  c0Trd[0]=trdht->Coo[0]; c0Trd[1]=trdht->Coo[1]; c0Trd[2]=trdht->Coo[2];
  static float cpx = 0; 
  static float cpy = 0;
  trdht->propagateToZ(trdconst::ToFLayer1Z, cpx,  cpy);
  cTrd[0] = cpx; cTrd[1] = cpy;
  cTrd[2] = trdconst::ToFLayer1Z;
  return true;
}

bool TrdSCalibR::GetcTrd(TrdTrackR *trdt) {
  if(! trdt) return false; 
  cTrd[0] = trdt->Coo[0]+dTrd[0]/dTrd[2]*(trdconst::ToFLayer1Z - trdt->Coo[2]);
  cTrd[1] = trdt->Coo[1]+dTrd[1]/dTrd[2]*(trdconst::ToFLayer1Z - trdt->Coo[2]);
  cTrd[2] = trdconst::ToFLayer1Z;
  return true;
}

//--------------------------------------------------------------------------------------------------
bool TrdSCalibR::GetdTrd(TrdHTrackR *trdht) {
  if(! trdht) return false;
  dTrd[0] = sin(trdht->Theta())*cos(trdht->Phi());
  dTrd[1] = sin(trdht->Theta())*sin(trdht->Phi());
  dTrd[2] = cos(trdht->Theta());
  return true;
}
bool TrdSCalibR::GetdTrd(TrdTrackR *trdt) {
  if(! trdt) return false;
  dTrd[0] = sin(trdt->Theta)*cos(trdt->Phi);
  dTrd[1] = sin(trdt->Theta)*sin(trdt->Phi);
  dTrd[2] = cos(trdt->Theta);
  return true;
}
//--------------------------------------------------------------------------------------------------
bool TrdSCalibR::GetcTrkdTrk(TrTrackR *trt){
  if(! trt) return false;
  int fitcode = trt->iTrTrackPar(algo, patt, refit);
  trt->Interpolate(trdconst::ToFLayer1Z,cTrk,dTrk, fitcode);
  return true;
}
//--------------------------------------------------------------------------------------------------
bool TrdSCalibR::MatchingTrdTKtrack(float Pabs, int Debug){
  double TrdTkD  = TMath::Sqrt(TMath::Power(cTrk.x()-cTrd.x(),2)+
			   TMath::Power(cTrk.y()-cTrd.y(),2)+
			   TMath::Power(cTrk.z()-cTrd.z(),2));

  double TrdTkDx = cTrk.x() - c0Trd.x() + trdconst::TrdOffsetDx;
  double TrdTkDy = cTrk.y() - c0Trd.y() + trdconst::TrdOffsetDy;
  double SigmaTrdTrackerX = fTrdSigmaDx->Eval(Pabs);
  double SigmaTrdTrackerY = fTrdSigmaDy->Eval(Pabs);
  double TrdTkDr 	  = TMath::Sqrt( TMath::Power(TrdTkDx/SigmaTrdTrackerX,2) + 
			       TMath::Power(TrdTkDy/SigmaTrdTrackerY,2) );

  double CutTrdTkDa       = 10.0*fTrd95Da->Eval(Pabs);

  double TrdTkDa = dTrk.x()*(-1*dTrd.x()) +  dTrk.y()*(-1*dTrd.y()) +  dTrk.z()*(-1*dTrd.z());
  TrdTkDa        = TMath::ACos(TrdTkDa)*180/TMath::Pi();

  if(Debug)
    std::cout << Form("MatchingTrdTk: D=%.2f Ds =%.2f CutDa=%.2f Da= %.2f ", 
		      TrdTkD, TrdTkDr, CutTrdTkDa, TrdTkDa) << std::endl;
  //if(TrdTkD >3. || TrdTkDa > CutTrdTkDa) return false;  
  if(TrdTkD >4. || TrdTkDa > CutTrdTkDa) return false;

  return true;
}
//--------------------------------------------------------------------------------------------------
void TrdSCalibR::ReadLFname(void){
  for(int i = 0; i < 6; i++)
    std::cout << Form("%s", TrdElectronProtonHeliumLFname[i]) << std::endl;
}
//--------------------------------------------------------------------------------------------------
int TrdSCalibR::GetEvthTime(AMSEventR *ev, int Debug){
  static int hTime = 0;
  time_t  EvtTime = ev->fHeader.Time[0];
  struct tm*  TimeInfo = gmtime ( &EvtTime );
  int 	Day     = TimeInfo->tm_yday;
  int	Hour	= TimeInfo->tm_hour;
  int   Minute 	= TimeInfo->tm_min;
  int   Second	= TimeInfo->tm_sec;

  hTime	= Day*24 + Hour; 

  if(Debug > 1)
    std::cout << Form("EvtTime  Day= %3d Time= %02d:%02d:%02d hTime=%8d", Day, Hour,Minute, Second, hTime) 
	      << std::endl;

  return hTime; 
}

//--------------------------------------------------------------------------------------------------
int TrdSCalibR::GetEvthTime(time_t EvtTime, int Debug){
  static int hTime = 0;
  struct tm*  TimeInfo = gmtime ( &EvtTime );
  int 	Day     = TimeInfo->tm_yday;
  int	Hour	= TimeInfo->tm_hour;
  int   Minute 	= TimeInfo->tm_min;
  int   Second	= TimeInfo->tm_sec;

  hTime	= Day*24 + Hour; 

  if(Debug > 1)
    std::cout << Form("EvtTime  Day= %3d Time= %02d:%02d:%02d hTime=%8d", Day, Hour,Minute, Second, hTime) 
	      << std::endl;

  return hTime; 
}
//--------------------------------------------------------------------------------------------------
float TrdSCalibR::GetEvtxTime(AMSEventR *ev, int Debug){
  static float xTime = 0;
  time_t  EvtTime = ev->fHeader.Time[0];
  struct tm*  TimeInfo = gmtime ( &EvtTime );
  int 	Day     = TimeInfo->tm_yday;
  int	Hour	= TimeInfo->tm_hour;
  int   Minute 	= TimeInfo->tm_min;
  int   Second	= TimeInfo->tm_sec;

  int	hTime	= Day*24 + Hour;
  int	mTime	= hTime*60 + Minute;
  int	sTime	= mTime*60 + Second;
  
  xTime = sTime / 86400.0; 

  if(Debug > 2)
    std::cout << Form("EvtTime  Day= %3d Time= %02d:%02d:%02d xTime=%5.2f", Day, Hour,Minute, Second, xTime) 
	      << std::endl;

  return xTime; 
}
//--------------------------------------------------------------------------------------------------
float TrdSCalibR::GetEvtxTime(time_t EvtTime, int Debug){
  static float xTime = 0;
  struct tm*  TimeInfo = gmtime ( &EvtTime );
  int 	Day     = TimeInfo->tm_yday;
  int	Hour	= TimeInfo->tm_hour;
  int   Minute 	= TimeInfo->tm_min;
  int   Second	= TimeInfo->tm_sec;

  int	hTime	= Day*24 + Hour;
  int	mTime	= hTime*60 + Minute;
  int	sTime	= mTime*60 + Second;
  
  xTime = sTime / 86400.0; 

  if(Debug > 2)
    std::cout << Form("EvtTime  Day= %3d Time= %02d:%02d:%02d xTime=%5.2f", Day, Hour,Minute, Second, xTime) 
	      << std::endl;
  return xTime; 
}

//--------------------------------------------------------------------------------------------------

int TrdSCalibR::GetModCalibTimePeriod(TString fname, TString hname, int Debug) {
	
  TFile *input = TFile::Open(fname);
  if (!input->IsOpen()) {  
    std::cout << "GetCalibTimePeriod: No " << fname << std::endl;
    return -1;
  }
  
  TH1D *h_Nevts = (TH1D*) input->Get(hname); /// center bottom module
  if(h_Nevts->GetNbinsX() == 0) return -2;

  int nBin = h_Nevts->GetNbinsX();
  for (int iBin=1; iBin<=nBin; iBin++) 
    {
      unsigned int iBCnt  = h_Nevts->GetBinContent(iBin);
      float        iTime  = h_Nevts->GetBinCenter(iBin);
      if(iBCnt == 0) continue;
      
      if(Debug > 1) 
	std::cout << Form("GetCalibTimePeriod iBin=%4u iTime=%5.2f iBCnt=%10u", iBin, iTime, iBCnt) << std::endl;
      
      if (iTime < FirstCalRunTime)  FirstCalRunTime = iTime;
      if (iTime > LastCalRunTime )  LastCalRunTime  = iTime; 
    }	
  input->Close();
  delete input;

  return 0;
}


//--------------------------------------------------------------------------------------------------

TObjArray* TrdSCalibR::ObjTrdGasCirMPV_list = new TObjArray();
TObjArray* TrdSCalibR::ObjTrdModuleMPV_list = new TObjArray();


bool TrdSCalibR::Get01TrdCalibration(TString fname, int Debug) {

  //  h_TrdGasCirMPV = new vector<TH1F*>();

  TFile *input = TFile::Open(fname);
  if (!input->IsOpen()) {
    std::cerr << "TrdHTrack:: No Trd 1 Calibration file " << std::endl;
    return false;
  }
  int nLost = 0;
  char hName[80];
  for (int iTrdGasCir=0; iTrdGasCir < trdconst::nTrdGasCir; iTrdGasCir++) 
    {
      sprintf(hName,"hTrdMPV_%d",iTrdGasCir+1);  
      TH1F *pgDum = (TH1F*) input->Get(hName);
      pgDum->SetDirectory(gROOT);
      pgDum->SetNameTitle(hName, hName);
      h_TrdGasCirMPV.push_back(pgDum);
     
      //ObjTrdGasCirMPV_list->AddAt(pDum, iTrdGasCir);

      if(Debug > 0)
	std::cout << Form("%s [%02d] M=%d", hName, iTrdGasCir, (int) pgDum->GetMean()) 
		  << std::endl;
      if (!pgDum) nLost ++;

    }

  input->Close();
  delete input;

  /*
  /// use TObjArray instead of vector <TH1F*>
  TObjArrayIter next(ObjTrdGasCirMPV_list);
  TObject* object;
  TH1F *p = NULL;
  int ii = 0;
  while ( ( object = next() ) ) {
    p = (TH1F*) object;
    if(!p) break;
    if(Debug > 0)
    std::cout << Form("+-+- %s [%02d] M=%d", p->GetName(), ii++, (int) p->GetMean()) 
	      << std::endl;
  } 
  delete object;
  delete p;
  */

  if (nLost==0) {
    std::cout << "Trd 1. Calibration file " << fname 
	      << " is sucessfully loaded "  << std::endl;
    return true;
  } else {
    std::cout << "Trd 1. Calibration file " << fname 
	      << " will be used. nLost=" << nLost << std::endl;
    return false;
  }

}

//--------------------------------------------------------------------------------------------------
bool TrdSCalibR::Get02TrdCalibration(TString fname, int Debug) { 

  TFile *input = TFile::Open(fname);
  if (!input->IsOpen()) {
    std::cerr << "TrdHTrack:: No Trd 2 Calibration file " << std::endl;
    return false;
  }
  int nLost = 0;
  char hName[80];

  for (int iMod=0; iMod < nTrdModules; iMod++) 
    {
      sprintf(hName,"h_ModMPV_%d",iMod);
      TH1F *pmDum = (TH1F*) input->Get(hName);
      pmDum->SetDirectory(gROOT);
      pmDum->SetNameTitle(hName, hName);
      h_TrdModuleMPV.push_back(pmDum);

      //ObjTrdModuleMPV_list->AddAt(pmDum, iMod);

      if(Debug > 0)
	std::cout << Form("%s [%02d] M=%d", hName, iMod, (int) pmDum->GetMean()) 
		  << std::endl;
      if (!pmDum) nLost ++;
    }

  input->Close();
  delete input;
 

  if (nLost==0) {
    std::cout << "Trd 2. Calibration file " << fname 
	      << " is sucessfully loaded "  << std::endl;
    return true;
  } else {
    std::cout << "Trd 2. Calibration file " << fname 
	      << " will be used. nLost=" << nLost << std::endl;
    return false;
  }

}
//--------------------------------------------------------------------------------------------------
TObjArray* TrdSCalibR::ObjTrdCalibMPV = new TObjArray();

bool TrdSCalibR::Get03TrdCalibration(TString fname, int Debug) {
	
  TFile *input = TFile::Open(fname);
  if (!input->IsOpen()) {
    std::cerr << "TrdHTrack:: No Trd V3 Calibration file " << std::endl;
    return false;
  }

  int nLost = 0;
  char hName[80];
  for (int iMod=0; iMod < nTrdModules; iMod++) 
    {
      sprintf(hName,"hModMpv_0_%d", iMod);
      TH1F *pDum0 = (TH1F*)input->Get(hName);
     
      sprintf(hName,"hModMpv_1_%d", iMod);
      TH1F *pDum1  = (TH1F*)input->Get(hName);
     
      sprintf(hName,"hModChi2_0_%d",iMod);
      TH1F *pDum2  = (TH1F*)input->Get(hName);
      
      if (!pDum0 || !pDum1 || !pDum2) nLost ++;
      if(Debug > 1)
	std::cout << Form("V3Calib Mod[%03d] Mean0=%d Mean1=%d Chisq=%d", 
			  iMod, (int) pDum0->GetMean(), (int) pDum1->GetMean(), (int) pDum2->GetMean()) 
		  << std::endl;     

      int nBinX = pDum0->GetNbinsX();
      vector<double> vecX, vecY, vecEy;
      for (int iBin=1; iBin<=nBinX; iBin++) 
	{
	  if (pDum2->GetBinContent(iBin)<=0.0) continue;
	  double x_0 	= pDum0->GetBinCenter(iBin);
	  double y_0 	= pDum0->GetBinContent(iBin);
	  double ey_0   = pDum0->GetBinError(iBin);
	  double y_t 	= pDum1->GetBinContent(iBin);
	  double ey_t   = pDum1->GetBinError(iBin);
	  if (y_t<TrdMeanMPV-2 || y_t>TrdMeanMPV+2) continue;
	  
	  vecX.push_back(x_0);
	  vecY.push_back(y_0);
	  vecEy.push_back(ey_0);
	}
      
      TGraphErrors *grTrdCalibMpv = new TGraphErrors(vecX.size(),&vecX[0],&vecY[0],0,&vecEy[0]);
      g_TrdCalibMPV.push_back(grTrdCalibMpv);
      
      ObjTrdCalibMPV->Add(grTrdCalibMpv);

      vecX.clear();
      vecY.clear();
      vecEy.clear();
    }
  

  input->Close();
  delete input;
 

  if (nLost==0) {
    std::cout << "Trd 3. Calibration file " << fname 
	      << " is sucessfully loaded "  << std::endl;
    return true;
  } else {
    std::cout << "Trd 3. Calibration file " << fname 
	      << " will be used. nLost=" << nLost << std::endl;
    return false;
  }


}
//--------------------------------------------------------------------------------------------------
bool TrdSCalibR::Init(int CalibLevel, int Debug){

  /// generate mStraw-Matrix
  GenmStrawMatrix(Debug);
  
  /// get TRD calibration histos / each gas circuit and each module level (v2)
  if(CalibLevel < 3) GetTrdCalibHistos(CalibLevel, Debug);
  
  /// get TRD calibration histos / each module level (v3)
  else if(CalibLevel < 4) GetTrdV3CalibHistos(CalibLevel, Debug);

  /// get TRD calibration graphs / each module level (v4)
  else GetTrdV4CalibHistos(CalibLevel, Debug);

  /// initiate TRD loglikelihood calculator
  TrdLR_CalcIni(Debug);

  /// initiate ToyMC loglikelihood calculator
  TrdLR_MC_CalcIni(Debug);

  /// check each initiate procedure
  if(FirstCall || FirstLLCall || FirstMCCall)
    return false;

  else return true;
}
//--------------------------------------------------------------------------------------------------
bool TrdSCalibR::GetTrdCalibHistos(int CalibLevel, int Debug) {
  
  FirstCall = true;  TrdCalib_01 = false;  TrdCalib_02 = false;
  /// Get TRD Calibration Histograms
  if (FirstCall) {
    if(Debug) 
      std::cout << "Get TRD Calibration Histograms: CalibLevel=" << CalibLevel << std::endl;
    cout << "----------------------------------------" << endl;
    char tdname[200]; 
    if (CalibLevel>0) 
      {
	sprintf(tdname, "%s/%s", TrdDBUpdateDir[0], TrdCalDBUpdate[0]);
	TrdCalib_01 = Get01TrdCalibration(tdname, Debug);
      }
    if (CalibLevel>1) 
      {
	sprintf(tdname, "%s/%s", TrdDBUpdateDir[0], TrdCalDBUpdate[1]);
	TrdCalib_02 = Get02TrdCalibration(tdname, Debug);

	/// check module calibration time period
	int ct = GetModCalibTimePeriod(tdname, "h_ModMPV_26", Debug);
	std::cout << Form("### ModCalibFirsthTime=%6u (hour) ModCalibLasthTime=%6u (hour)",
			  FirstCalRunTime, LastCalRunTime) << std::endl;
      }

    if(Debug) 
      std::cout <<Form("TrdCalib1=%s TrdCalib2=%s",
		       TrdCalib_01?"true":"false",
		       TrdCalib_01?"true":"false") <<std::endl;
    FirstCall = false;
    
  }

  if (  (TrdCalib_01 && CalibLevel>0) || (TrdCalib_02 && CalibLevel>1) ) 
      return true;
  else return false;
    
}
//-------------------------------------------------------------------------------------------------
bool TrdSCalibR::GetTrdV3CalibHistos(int CalibLevel, int Debug) {
  
  FirstCall = true;  TrdCalib_01 = false;  TrdCalib_02 = false; TrdCalib_03 = false;
  /// Get TRD Calibration Histograms
  if (FirstCall) {
    if(Debug) 
      std::cout << "Get TRD Calibration Histograms: CalibLevel=" << CalibLevel << std::endl;
    cout << "----------------------------------------" << endl;
    char tdname[200]; 
   
    if (CalibLevel>2) 
      {
	sprintf(tdname, "%s/%s", TrdDBUpdateDir[0], TrdCalDBUpdate[2]);
	TrdCalib_03 = Get03TrdCalibration(tdname, Debug);

	/// check module calibration time period
	int ct = GetModCalibTimePeriod(tdname, "hModMpv_0_26", Debug);
	std::cout << Form("### ModCalibFirsthTime=%6u (day) ModCalibLasthTime=%6u (day)",
			  FirstCalRunTime, LastCalRunTime) << std::endl;
      }
    
    
    if(Debug) 
      std::cout <<Form("TrdCalib1=%s TrdCalib2=%s TrdCalib3=%s",
		       TrdCalib_01?"true":"false",
		       TrdCalib_02?"true":"false",
		       TrdCalib_03?"true":"false") <<std::endl;
    FirstCall = false;
   
  }

  if (  TrdCalib_03 && CalibLevel>2 ) 
    return true;
  else return false;
    
}
//-------------------------------------------------------------------------------------------------
		
void TrdSCalibR::GetBinGasCirModule(int hTime, int CalibLevel, int iCir, int iMod, int Debug) {
  	
  iBinGasCir 	= -1;
  iBinModule 	= -1;

  if(Debug > 1)
    std::cout << "h_TrdGasCirMPV.size= "   << h_TrdGasCirMPV.size() 
	      << "  h_TrdModuleMPV.size= " << h_TrdModuleMPV.size()
	      << std::endl;
  
  if(Debug > 1)
    std::cout << Form("TrdCalib_01=%s TrdCalib_02=%s CalibLevel=%d", 
		      TrdCalib_01?"true":"false",TrdCalib_02?"true":"false", CalibLevel)
	      << std::endl;
  if (TrdCalib_01 && CalibLevel>0) 
      iBinGasCir = h_TrdGasCirMPV.at(iCir)->FindBin(double(hTime));

  if (TrdCalib_02 && CalibLevel>1) 
      iBinModule = h_TrdModuleMPV.at(iMod)->FindBin(double(hTime)); 
  
  if(Debug > 1)
  std::cout << "hTime=" << (int)hTime 
	    << " iBinGasCir= " << iBinGasCir 
	    << " iBinModule= " << iBinModule 
	    << std::endl; 


  if (TrdCalib_02 && CalibLevel>1 && Debug > 1) {
    TAxis *axis    = h_TrdModuleMPV.at(iMod)->GetXaxis();
    Double_t xmin  = axis->GetXmin();
    Double_t xmax  = axis->GetXmax();
    Int_t nbins    = axis->GetNbins();
    Float_t bwid   = axis->GetBinWidth(1);
    Int_t bin      = axis->FindBin(hTime);   
    double ibinmod = h_TrdModuleMPV.at(iMod)->GetBinContent(h_TrdModuleMPV.at(iMod)->FindBin(hTime));
    std::cout << Form("hTime=%6d iCir=%2d iMod=%3d xmin=%6.0f xmax=%6.0f nbins=%6d bwid=%4.1f bin=%6d ibinmod=%4.1f", 
		      hTime, iCir, iMod, xmin, xmax, nbins, bwid, bin, ibinmod) << std::endl;
  }
  
  return;
}

//--------------------------------------------------------------------------------------------------
vector<float> TrdSCalibR::GetTrdSum8(vector<AC_TrdHits*> TrdHits, int Debug) {

  vector<float> TrdSum8;
  /// consider only two subset from raw and cs values
  TrdSum8.assign(2,0.0);
  
  if (TrdHits.size()>8) {
    vector<float> TrdEadcR, TrdEadcCS;  //TrdEadcCM

    vector<AC_TrdHits*>::iterator iter;
    for (iter = TrdHits.begin(); iter != TrdHits.end(); ++iter) {
      TrdEadcR.push_back((*iter)->EadcR);
      TrdEadcCS.push_back((*iter)->EadcCS);
    }
    
    if(Debug > 0) {
      std::cout << "TrdEadcR contains:";
      for(vector<float>::iterator it = TrdEadcR.begin(); it != TrdEadcR.end(); ++it )
	std::cout << " " << *it;
      std::cout << std::endl;
    }
    
    //std::sort(&TrdEadcR[0],&TrdEadcR[0]+TrdEadcR.size());
    sort(TrdEadcR.begin(),TrdEadcR.begin()+ TrdEadcR.size());
 
    for (unsigned int i=TrdEadcR.size()-8; i<TrdEadcR.size(); i++) 
      TrdSum8.at(0) += TrdEadcR.at(i);
    
    sort(TrdEadcCS.begin(),TrdEadcCS.begin()+ TrdEadcCS.size());
    for (unsigned int i=TrdEadcCS.size()-8; i<TrdEadcCS.size(); i++) 
      TrdSum8.at(1) += TrdEadcCS.at(i);
  }

  
  if(Debug) {
    int i = 0;
    for(vector<float>::iterator is = TrdSum8.begin(); is != TrdSum8.end(); ++is )
      std::cout << Form("%s= %6.2f  ",TrdSum8name[i++], *is);
    std::cout << std::endl;
  }

  return TrdSum8;
}
//--------------------------------------------------------------------------------------------------

bool TrdSCalibR::TrdLR_GetParameters(string fName, string hName, 
				     int nPar, vector<double> &xMin, 
				     vector< vector<double> > &Par) {
  
  const char *fname = fName.c_str();
  const char *hname = hName.c_str();
  TFile *input = TFile::Open(fname);
  if (!input->IsOpen()) {
    std::cerr << "Error: Open TrdLR_GetParameters" << fname << std::endl;
    return false;
  }

  TH1D *pDum = NULL;
  for (int i=0; i<nPar; i++) 
    { 
      TString hn = Form("%s%d", hname, i);
      pDum  = (TH1D*)input->Get(hn);
      int nBinX = pDum->GetNbinsX();		
      
      /// Get the Binning
      if (i==0) {
	for (int iBinX = 1; iBinX<=nBinX; iBinX++) {
	  double xLow = pDum->GetXaxis()->GetBinLowEdge(iBinX);
	  xMin.push_back(xLow);
	}
	double xHigh = pDum->GetXaxis()->GetBinLowEdge(nBinX);
	xHigh       += pDum->GetXaxis()->GetBinWidth(nBinX); 
	xMin.push_back(xHigh);
      }
      /// Get the Content
      vector<double> ParBin;
      for (int iBinX = 1; iBinX<=nBinX; iBinX++) 
	ParBin.push_back(pDum->GetBinContent(iBinX));
      
      Par.push_back(ParBin);
    }
  input->Close();

  return true;
}
//--------------------------------------------------------------------------------------------------

void TrdSCalibR::TrdLR_CalcIni_v01(int Debug) {

  FirstLLCall = true;

  std::string hname;
  
  char fname[100];

  if (FirstLLCall) {
    /// electron parameters
    sprintf(fname, "%s/%s", TrdDBUpdateDir[0],TrdElectronProtonHeliumLFname[0]); 
    hname.assign("hElectron_Par");
    TrdLR_GetParameters(fname, hname, trdconst::TrdLR_Elec_nPar, TrdLR_xElec, TrdLR_pElec);
    TrdLR_nElec.assign(TrdLR_xElec.size(),1.0);
    if (Debug>0) {
      std::cout << " TRD_LR Electrons -----------------------------" << std::endl;
      for (unsigned int iLay=0; iLay<TrdLR_xElec.size()-1; iLay++) 
	{
	  std::cout << Form("Layer = %6.0f    ",(TrdLR_xElec.at(iLay)+TrdLR_xElec.at(iLay+1))/2); 
	  for (int j=0; j<trdconst::TrdLR_Elec_nPar; j++)
	    std::cout << Form(" %+10.4E ",TrdLR_pElec[j][iLay]);
	  std::cout << std::endl;
      }
      std::cout << std::endl;
    }
    hname.clear();

    /// proton parameters
    sprintf(fname, "%s/%s", TrdDBUpdateDir[0],TrdElectronProtonHeliumLFname[1]); 
    hname.assign("hProton_Par");
    TrdLR_GetParameters(fname, hname, trdconst::TrdLR_Prot_nPar, TrdLR_xProt, TrdLR_pProt);
    TrdLR_nProt.assign(TrdLR_xProt.size(),1.0);
    if (Debug>0) {		
      std::cout << " TRD_LR Protons -------------------------------" << std::endl;
      for (unsigned int iP=0; iP<TrdLR_xProt.size()-1; iP++) 
	{
	  std::cout << Form("P = [%6.2f,%6.2f]",TrdLR_xProt.at(iP),TrdLR_xProt.at(iP+1));
	  for (int j=0; j<TrdLR_Prot_nPar; j++) 
	    std::cout << Form(" %+10.4E ",TrdLR_pProt[j][iP]);
	  std::cout << std::endl;
	}
      std::cout << std::endl;
    }
    hname.clear();
    
    ///  helium parameters
    sprintf(fname, "%s/%s", TrdDBUpdateDir[0],TrdElectronProtonHeliumLFname[2]); 
    hname.assign("hHelium_Par");
    TrdLR_GetParameters(fname, hname, trdconst::TrdLR_Heli_nPar, TrdLR_xHeli, TrdLR_pHeli);
    TrdLR_nHeli.assign(TrdLR_xHeli.size(),1.0);
    if (Debug>0) {
      std::cout << " TRD_LR Helium -------------------------------" << std::endl;
      for (unsigned int iP=0; iP<TrdLR_xHeli.size()-1; iP++) 
	{
	  std::cout << Form("P = [%6.2f,%6.2f]",TrdLR_xHeli.at(iP),TrdLR_xHeli.at(iP+1));
	  for (int j=0; j<TrdLR_Heli_nPar; j++)
	    std::cout << Form(" %+10.4E ",TrdLR_pHeli[j][iP]);
	  std::cout << std::endl;
	}
      std::cout << std::endl;
    }
    hname.clear();

    FirstLLCall = false;
  }
  
  return;
}

//--------------------------------------------------------------------------------------------------

vector<double> TrdSCalibR::TrdLR_Calc_v01(float Pabs, vector<AC_TrdHits*> TrdHits, int iFlag, int Debug) {

  vector<double> TrdLR;
  TrdLR.assign(3,-1.0);
  
  /// Which Momentum Bin for Protons & Helium ?
  int iP     = -1;
  int iMax   = TrdLR_xProt.size()-1;
  for (unsigned int i=0; i<iMax; i++) {
    if (Pabs >= TrdLR_xProt.at(i) && Pabs < TrdLR_xProt.at(i+1)) {
      iP = i;
      break;
    }
  }
  
  if(Debug > 1) 
    std::cout << "Pmin= " << TrdLR_xProt.at(0) << " Pmax= " << TrdLR_xProt.at(iMax) << std::endl;

  if (Pabs >= TrdLR_xProt.at(iMax))  iP = iMax-1;
  if (Pabs <  TrdLR_xProt.at(0))     iP = 0;
  
  if (iP<0) {
    std::cout << "Error in TrdLR_Calc: invalid momentum P=" << Pabs << std::endl;
    exit(-1);
  }
  
  double 	Lprod_Proton    = 1.0;
  double 	Lprod_Helium 	= 1.0;
  double 	Lprod_Electron 	= 1.0;
  double	x[1], par[1];
  
  int nCount = 0;

  vector<AC_TrdHits*>::iterator iter;
  for (iter = TrdHits.begin(); iter != TrdHits.end(); ++iter) {

    int 	  Layer = (*iter)->Lay;
    double	  Len	= (*iter)->Len2D;
    if (iFlag==3) Len   = (*iter)->Len3D;
    if (Len < 0.05) continue;
    double 	  scale = 2.0*trdconst::TrdStrawRadius/Len;
    double 	  Eadc  = (*iter)->EadcCS*scale;  

    x[0] 		= Eadc;
    par[0] 	        = iP;	

    //double L_Proton 	= max(1E-8, TrdSCalibR::TrdLR_fProton(x,par));
    myPair <double> myProtonL  (1E-8, TrdSCalibR::TrdLR_fProton(x,par));
    myPair <double> myHeliumL  (1E-8, TrdSCalibR::TrdLR_fHelium(x,par));
    myPair <double> myElectronL(1E-8, TrdSCalibR::TrdLR_fElectron(x,par));
    double L_Proton   = myProtonL.GetMax();
    double L_Helium   = myHeliumL.GetMax();
    double L_Electron = myElectronL.GetMax();

    if(Debug > 1) 
      std::cout << Form("i=%3d EadcR=%4d EadcCS=%4d EadcPL=%4d L_Proton=%12.4E L_Helium=%12.4f L_Electron=%12.4f",
			nCount, (int)(*iter)->EadcR, (int)(*iter)->EadcCS, (int) Eadc,L_Proton,L_Helium,L_Electron) << std::endl;
      
    Lprod_Proton 	*= L_Proton;
    Lprod_Helium 	*= L_Helium;
    Lprod_Electron 	*= L_Electron;
    nCount ++;
  }

  if(Debug > 1) 
    std::cout << Form("Step 1. |P|=%6.2f(GeV/c) Lprod_Electron=%12.4E Lprod_Helium=%12.4E Lprod_Proton=%12.4E", 
		      Pabs, Lprod_Electron, Lprod_Helium, Lprod_Proton) << std::endl;
  
  
  Lprod_Proton 		= TMath::Power(Lprod_Proton,  1.0/float(nCount));
  Lprod_Helium 		= TMath::Power(Lprod_Helium,  1.0/float(nCount));
  Lprod_Electron 	= TMath::Power(Lprod_Electron,1.0/float(nCount));

  if(Debug > 1) 
    std::cout << Form("Step 2. |P|=%6.2f(GeV/c) Lprod_Electron=%12.4E Lprod_Helium=%12.4E Lprod_Proton=%12.4E", 
		      Pabs, Lprod_Electron, Lprod_Helium, Lprod_Proton) << std::endl;
  
  
  double LR_Elec_Prot   = Lprod_Electron/(Lprod_Electron+Lprod_Proton);
  double LR_Heli_Prot	= Lprod_Helium/(Lprod_Helium+Lprod_Proton);
  double LR_Elec_Heli	= Lprod_Electron/(Lprod_Electron+Lprod_Helium);
  
  TrdLR.at(0) = -log(LR_Elec_Prot);
  TrdLR.at(1) = -log(LR_Heli_Prot);
  TrdLR.at(2) = -log(LR_Elec_Heli);

  if(Debug > 1) {
    int j = 0;
    for(vector<double>::iterator lr = TrdLR.begin(); lr != TrdLR.end(); ++lr )
      {
	std::cout << Form("%s=%4.2f  ", TrdLLname[j++], *lr);
      }
    std::cout << std::endl;
  }

  
  return TrdLR;
}

//--------------------------------------------------------------------------------------------------

vector<double> TrdSCalibR::GenLogBinning(int nBinLog, double Tmin, double Tmax) {

  vector<double> xMin;
  xMin.push_back(Tmin);
  for (int i=0; i<nBinLog+1; i++) {
    double arg  = TMath::Log(xMin[i])+(TMath::Log(Tmax)-TMath::Log(Tmin))/nBinLog;
    double xMax = TMath::Exp(arg);
    xMin.push_back(xMax);
  }
  return xMin;
}

//--------------------------------------------------------------------------------------------------
///	x[0] = Eadc, par[0] = iP = Momentum Bin
double TrdSCalibR::TrdLR_fProton(double *x, double *par) {
	
  static double L_Proton = 0.0;
  int	 iP  		 = (int) par[0];
  if (iP<0 || iP>=TrdLR_xProt.size()) {
    std::cout << "Error in TrdLR_fProton: iP=" << iP << std::endl;
    exit(-1);
  }
  
  static bool Graph[100] = {false};
  static TGraph *gr_L[100];
  if (Graph[iP]) 
    {
      L_Proton = gr_L[iP]->Eval(x[0]);
      
    } else {
    vector<double> parx;
    for (int i=0; i<TrdLR_Prot_nPar; i++) 
      parx.push_back(TrdLR_pProt[i][iP]);

    TF1* fTrdLR = new TF1("fTrdLR", this, &TrdSCalibR::Fun2LandauGausExpo, 0.0,4000.0, parx.size());
    for (int i=0; i<parx.size(); i++) fTrdLR->SetParameter(i,parx.at(i));

    fTrdLR->SetNpx(1000);
    TrdLR_nProt.at(iP) = fTrdLR->Integral(trdconst::TrdMinAdc,trdconst::TrdMaxAdc);

    vector<double> vecAdc, vecLR;
    vector<double> xMin = GenLogBinning(1000,0.1, trdconst::TrdMaxAdc);
    for (unsigned int i=0; i<xMin.size(); i++) 
      {
	double xadc[1], lr;
	xadc[0] = xMin.at(i);
	lr 	= TrdSCalibR::Fun2LandauGausExpo(xadc,&parx[0]) * 1.0/TrdLR_nProt[iP];
	vecAdc.push_back(xadc[0]);
	vecLR.push_back(lr);
      }
    gr_L[iP]    = new TGraph(vecAdc.size(),&vecAdc[0],&vecLR[0]);
    Graph[iP] 	= true;
    L_Proton 	= gr_L[iP]->Eval(x[0]);
  }
  
  return L_Proton;
}

//--------------------------------------------------------------------------------------------------
///	x[0] = Eadc, par[0] = iP = Momentum Bin
double TrdSCalibR::TrdLR_fHelium(double *x, double *par) {
	
  double L_Helium 	= 0.0;
  int 	 iP 		= (int) par[0];
  if (iP<0 || iP>=TrdLR_xHeli.size()) {
    std::cout << "Error in TrdLR_fHelium: iP=" << iP << std::endl;
    exit(-1);
  }
  
  static bool Graph[100] = {false};
  static TGraph *gr_L[100];
  if (Graph[iP]) {
    L_Helium = gr_L[iP]->Eval(x[0]);
    
  } else {
    vector<double> parx;
    for (int i=0; i<TrdLR_Heli_nPar; i++) parx.push_back(TrdLR_pHeli[i][iP]);
   
    TF1* fTrdLR = new TF1("fTrdLR", this, &TrdSCalibR::FunLandauGaus3Exp, 0.0,4000.0,parx.size());
    for (int i=0; i<parx.size(); i++) fTrdLR->SetParameter(i,parx.at(i));
    fTrdLR->SetNpx(1000);
    TrdLR_nHeli.at(iP) = fTrdLR->Integral(trdconst::TrdMinAdc, trdconst::TrdMaxAdc);

    vector<double> vecAdc, vecLR;
    vector<double> xMin = GenLogBinning(1000,0.1, trdconst::TrdMaxAdc);
    for (unsigned int i=0; i<xMin.size(); i++) {
      double xadc[1], lr;
      xadc[0] = xMin.at(i);
      lr      = TrdSCalibR::FunLandauGaus3Exp(xadc,&parx[0]) * 1.0/TrdLR_nHeli.at(iP);
      vecAdc.push_back(xadc[0]);
      vecLR.push_back(lr);
    }
    gr_L[iP]  = new TGraph(vecAdc.size(),&vecAdc[0],&vecLR[0]);
    Graph[iP] = true;
    L_Helium  = gr_L[iP]->Eval(x[0]);
  }
  
  return L_Helium;
}

//--------------------------------------------------------------------------------------------------
///	x[0] = Eadc, par[0] = Layer
double TrdSCalibR::TrdLR_fElectron(double *x, double *par) {
	
  static bool Graph[nTrdLayers] = {false};
  static TGraph *gr_L[nTrdLayers];
  
  double  L_Electron 	= 0.0;
  int 	  Layer 	= (int) par[0];
  
  if (Graph[Layer]) {
    L_Electron = gr_L[Layer]->Eval(x[0]);
    
  } else {
    vector<double> parx;
    for (int i=0; i<TrdLR_Elec_nPar; i++) parx.push_back(TrdLR_pElec[i][Layer]);

    TF1* fTrdLR = new TF1("fTrdLR", this, &TrdSCalibR::Fun2Landau3Expo, 0.0,4000.0,parx.size());
    for (unsigned int i=0; i<parx.size(); i++) fTrdLR->SetParameter(i,parx.at(i));
    fTrdLR->SetNpx(1000);
    TrdLR_nElec.at(Layer) = fTrdLR->Integral((double)trdconst::TrdMinAdc,(double)trdconst::TrdMaxAdc);
    vector<double> vecAdc, vecLR;
    vector<double> xMin = GenLogBinning(1000,(double)trdconst::TrdMinAdc,(double)trdconst::TrdMaxAdc);
    for (unsigned int i=0; i<xMin.size(); i++) {
      double xadc[1], lr;
      xadc[0] = xMin.at(i);
      lr 	  = TrdSCalibR::Fun2Landau3Expo(xadc,&parx[0]) * 1.0/TrdLR_nElec.at(Layer);
      vecAdc.push_back(xadc[0]);
      vecLR.push_back(lr);
    }
    gr_L[Layer]   = new TGraph(vecAdc.size(),&vecAdc[0],&vecLR[0]);
    Graph[Layer]  = true;
    L_Electron 	  = gr_L[Layer]->Eval(x[0]);
  }
  
  return L_Electron;
}
//--------------------------------------------------------------------------------------------------
vector<double> TrdSCalibR::RootGetYbinsTH2(TH2 *hist, int Debug) {
  
  if(Debug > 1) std::cout << Form("H2: %s nBinY=%3d ", hist->GetName(), hist->GetNbinsY());

  int nBinY 	 = hist->GetNbinsY();
  vector<double> yMin;
  for (int iBinY = 1; iBinY<=nBinY; iBinY++) {
    double yL = hist->GetYaxis()->GetBinLowEdge(iBinY);
    if(Debug > 1) std::cout << Form("[%02d] %4.1f ", iBinY, yL);
    yMin.push_back(yL);
  }
  double yH = hist->GetYaxis()->GetBinLowEdge(nBinY) + hist->GetYaxis()->GetBinWidth(nBinY); 
  if(Debug > 1) std::cout << Form("[%02d] %4.1f", nBinY, yH) << std::endl;
  yMin.push_back(yH);
  return yMin;
}

//--------------------------------------------------------------------------------------------------
bool TrdSCalibR::TrdLR_CalcIni_v02(int Debug) {

  if( SCalibLevel > 4 ) return  false;  

  FirstLLCall = true;
  
  std::string hname;
  char aname[100];
  char fname[100];
  if (FirstLLCall) {
    /// version 2
    if(SCalibLevel < 3)
      sprintf(fname, "%s/%s", TrdDBUpdateDir[0],TrdElectronProtonHeliumLFname[3]);

    /// version 3
    else if(SCalibLevel < 4) 
      sprintf(fname, "%s/%s", TrdDBUpdateDir[0],TrdElectronProtonHeliumLFname[4]);
    
    /// version 4
    else 
      sprintf(fname, "%s/%s", TrdDBUpdateDir[0],TrdElectronProtonHeliumLFname[4]);
 
    TFile *input = new TFile(fname);
    if (!input->IsOpen()) {
      std::cerr << "Error: Open Trd Likelihood root file " << fname << std::endl;
      return false;
    }    
    std::cout << "### Trd LiklihoodFunctions read in from " << fname << std::endl;

    TH2D* h_TrdProt  = (TH2D*) input->Get("h_TrdProt");
    TH2D* h_TrdElec  = (TH2D*) input->Get("h_TrdElec");
    TH2D* h_TrdHeli  = (TH2D*) input->Get("h_TrdHeli");
    TH1D* h_Norm     = (TH1D*) input->Get("h_Norm");
    
    int nBinY=0;
    //------------------- Protons -----------------------------------------------------------
    TrdLR_xProt 	= RootGetYbinsTH2(h_TrdProt, Debug);
    nBinY 		= h_TrdProt->GetNbinsY();
    for (int iBinY=1; iBinY<=nBinY; iBinY++) {
      char hpName[80];
      sprintf(hpName,"h_TrdLR_Prot_%d",iBinY);
      h_TrdLR_Prot.push_back(h_TrdProt->ProjectionX(hpName,iBinY,iBinY));
    }
    
    TrdLR_nProt.assign(h_TrdLR_Prot.size(),1.0);
    for (unsigned int i=0; i<h_TrdLR_Prot.size(); i++) {
      h_TrdLR_Prot.at(i)->Sumw2();
      h_TrdLR_Prot.at(i)->Divide(h_Norm);
      h_TrdLR_Prot.at(i)->Scale(1.0/h_TrdLR_Prot.at(i)->Integral());
      TGraph 		*grin 	= new TGraph(h_TrdLR_Prot.at(i));
      TGraphSmooth 	*gs 	= new TGraphSmooth("normal");
      TrdLR_Gr_Prot.push_back(gs->SmoothSuper(grin,"",6,0.025));			    
      TF1 *fTrdLR = new TF1("fTrdLR", this, &TrdSCalibR::TrdLR_fProton_v02,0.0,4000.0,1);
      fTrdLR->SetParameter(0,double(i));	
      fTrdLR->SetNpx(1000);	
      TrdLR_nProt.at(i) = fTrdLR->Integral((double)trdconst::TrdMinAdc,(double)trdconst::TrdMaxAdc);
    }
    
    //------------------- Electrons -----------------------------------------------------------
    TrdLR_xElec 	= RootGetYbinsTH2(h_TrdElec, Debug);
    nBinY 		= h_TrdElec->GetNbinsY();
    for (int iBinY=1; iBinY<=nBinY; iBinY++) {
      char hpName[80];
      sprintf(hpName,"h_TrdLR_Elec_%d",iBinY);
      h_TrdLR_Elec.push_back(h_TrdElec->ProjectionX(hpName,iBinY,iBinY));
    }
    
    TrdLR_nElec.assign(h_TrdLR_Elec.size(),1.0);
    for (unsigned int i=0; i<h_TrdLR_Elec.size(); i++) {
      h_TrdLR_Elec.at(i)->Sumw2();
      h_TrdLR_Elec.at(i)->Divide(h_Norm);
      h_TrdLR_Elec.at(i)->Scale(1.0/h_TrdLR_Elec.at(i)->Integral());
      TGraph 		*grin 	= new TGraph(h_TrdLR_Elec.at(i));
      TGraphSmooth 	*gs 	= new TGraphSmooth("normal");
      TrdLR_Gr_Elec.push_back(gs->SmoothSuper(grin,"",6,0.025));			 	
      TF1 *fTrdLR = new TF1("fTrdLR", this, &TrdSCalibR::TrdLR_fElectron_v02,0.0,4000.0,1);
      fTrdLR->SetParameter(0,double(i));	
      fTrdLR->SetNpx(1000);	
      TrdLR_nElec.at(i) = fTrdLR->Integral((double)trdconst::TrdMinAdc,(double)trdconst::TrdMaxAdc);
    }
    
    //------------------- Helium -----------------------------------------------------------
    TrdLR_xHeli 	= RootGetYbinsTH2(h_TrdHeli, Debug);
    nBinY 		= h_TrdHeli->GetNbinsY();
    for (int iBinY=1; iBinY<=nBinY; iBinY++) {
      char hpName[80];
      sprintf(hpName,"h_TrdLR_Heli_%d",iBinY);
      h_TrdLR_Heli.push_back(h_TrdHeli->ProjectionX(hpName,iBinY,iBinY));
    }
    
    TrdLR_nHeli.assign(h_TrdLR_Heli.size(),1.0);
    for (unsigned int i=0; i<h_TrdLR_Heli.size(); i++) {
      h_TrdLR_Heli.at(i)->Sumw2();
      h_TrdLR_Heli.at(i)->Divide(h_Norm);
      h_TrdLR_Heli.at(i)->Scale(1.0/h_TrdLR_Heli.at(i)->Integral());
      TGraph 		*grin 	= new TGraph(h_TrdLR_Heli.at(i));
      TGraphSmooth 	*gs 	= new TGraphSmooth("normal");
      TrdLR_Gr_Heli.push_back(gs->SmoothSuper(grin,"",6,0.025));			
      TF1 *fTrdLR = new TF1("fTrdLR", this, &TrdSCalibR::TrdLR_fHelium_v02,0.0,4000.0,1);
      fTrdLR->SetParameter(0,double(i));	
      fTrdLR->SetNpx(1000);	
      TrdLR_nHeli.at(i) = fTrdLR->Integral((double)trdconst::TrdMinAdc,(double)trdconst::TrdMaxAdc);
    }
    FirstLLCall = false;
  }
  
  return true;
}

//--------------------------------------------------------------------------------------------------

vector<double> TrdSCalibR::TrdLR_Calc(float Pabs, vector<AC_TrdHits*> TrdHits, int iFlag, int Debug) {

  vector<double> TrdLR;
  TrdLR.assign(3, -1.0);
  
  // Which Momentum Bin for Protons & Helium ?
  int iP 	= -1;
  int iMax 	= TrdLR_xProt.size()-1;
  for (unsigned int i=0; i<iMax; i++) {
    if (Pabs >= TrdLR_xProt.at(i) && Pabs < TrdLR_xProt.at(i+1)) {
      iP = i;
      break;
    }
  }
  if (Pabs >= TrdLR_xProt.at(iMax)) 	iP = iMax-1;
  if (Pabs <  TrdLR_xProt.at(0)) 	iP = 0;
  
  if (iP<0) {
    std::cerr << "Error in TrdLR_Calc: invalid momentum P=" << Pabs << std::endl;
    return TrdLR;
  }
  
 
  double	x[1], par[1];
  
  int nCount = 0;

  Lprod_Proton =  Lprod_Helium = Lprod_Electron = 1.0;

  vector<AC_TrdHits*>::iterator iter;
  for (iter = TrdHits.begin(); iter != TrdHits.end(); ++iter) {

    int 	  Layer = (*iter)->Lay;
    double	  Len   = (*iter)->Len2D;
    if (iFlag==3) Len   = (*iter)->Len3D;
    if (Len < 0.05) continue;
    double	scale 	= 2.0*trdconst::TrdStrawRadius/Len;
    double 	Eadc	= (*iter)->EadcCS*scale;
    
    x[0] 		= Eadc;
    par[0] 	        = iP;	
    myPair <double> myProtonL  (1E-8, TrdSCalibR::TrdLR_fProton_v02(x,par));
    myPair <double> myHeliumL  (1E-8, TrdSCalibR::TrdLR_fHelium_v02(x,par));
    par[0]              = Layer;  
    myPair <double> myElectronL(1E-8, TrdSCalibR::TrdLR_fElectron_v02(x,par));
    double L_Proton   = myProtonL.GetMax();
    double L_Helium   = myHeliumL.GetMax();
    double L_Electron = myElectronL.GetMax();

    if(Debug > 1) 
      std::cout << Form("i=%3d EadcR=%4d EadcCS=%4d EadcPL=%4d L_Proton=%12.4E L_Helium=%12.4f L_Electron=%12.4f",
			nCount, (int)(*iter)->EadcR, (int)(*iter)->EadcCS, (int) Eadc,L_Proton,L_Helium,L_Electron) << std::endl;

    Lprod_Proton 	*= L_Proton;
    Lprod_Helium 	*= L_Helium;
    Lprod_Electron 	*= L_Electron;
    nCount ++;
  }
  
  if(Debug > 1) 
    std::cout << Form("Step 1. |P|=%6.2f(GeV/c) Lprod_Electron=%12.4E Lprod_Helium=%12.4E Lprod_Proton=%12.4E", 
		      Pabs, Lprod_Electron, Lprod_Helium, Lprod_Proton) << std::endl;

  Lprod_Proton 		= TMath::Power(Lprod_Proton,  1.0/float(nCount));
  Lprod_Helium 		= TMath::Power(Lprod_Helium,  1.0/float(nCount));
  Lprod_Electron 	= TMath::Power(Lprod_Electron,1.0/float(nCount));

  if(Debug > 1) 
    std::cout << Form("Step 2. |P|=%6.2f(GeV/c) Lprod_Electron=%12.4E Lprod_Helium=%12.4E Lprod_Proton=%12.4E", 
		      Pabs, Lprod_Electron, Lprod_Helium, Lprod_Proton) << std::endl;

   
  double LR_Elec_Prot   = Lprod_Electron/(Lprod_Electron+Lprod_Proton);
  double LR_Heli_Prot	= Lprod_Helium/(Lprod_Helium+Lprod_Proton);
  double LR_Elec_Heli	= Lprod_Electron/(Lprod_Electron+Lprod_Helium);
  
  TrdLR.at(0) = -log(LR_Elec_Prot);
  TrdLR.at(1) = -log(LR_Heli_Prot);
  TrdLR.at(2) = -log(LR_Elec_Heli);

  if(Debug > 1) 
    {
      int j = 0;
      for(vector<double>::iterator lr = TrdLR.begin(); lr != TrdLR.end(); ++lr )
	{
	  std::cout << Form("%s=%4.2f  ", TrdLLname[j++], *lr);
	}
      std::cout << std::endl;
    }

  return TrdLR;
}
//--------------------------------------------------------------------------------------------------
bool TrdSCalibR::TrdLR_MC_CalcIni(int Debug) {
  
  char fName[80];

  if (FirstMCCall) {
    for (int iP=0; iP<nParLR; iP++) {
      sprintf(fName,"fTrdLR_fProton_%d",iP);
      TF1 *fpTrdLR = new TF1(fName, this, &TrdSCalibR::TrdLR_fProton_v02, 0.0,4000.0,1);
      fTrdLR_fProton.push_back(fpTrdLR);
      fTrdLR_fProton.at(iP)->SetNpx(1000);
      fTrdLR_fProton.at(iP)->SetLineColor(kBlack);
      fTrdLR_fProton.at(iP)->SetParameter(0,double(iP));
      fTrdLR_fProton.at(iP)->SetMinimum(1E-8);

      sprintf(fName,"fTrdLR_fElectron_%d",iP);
      TF1 *feTrdLR = new TF1(fName, this, &TrdSCalibR::TrdLR_fElectron_v02, 0.0,4000.0,1);
      fTrdLR_fElectron.push_back(feTrdLR);
      fTrdLR_fElectron.at(iP)->SetNpx(1000);
      fTrdLR_fElectron.at(iP)->SetLineColor(kRed);
      fTrdLR_fElectron.at(iP)->SetParameter(0,double(iP));
      fTrdLR_fElectron.at(iP)->SetMinimum(1E-8);

      sprintf(fName,"fTrdLR_fHelium_%d",iP);
      TF1 *fHeTrdLR = new TF1(fName, this, &TrdSCalibR::TrdLR_fHelium_v02, 0.0,4000.0,1);
      fTrdLR_fHelium.push_back(fHeTrdLR);
      fTrdLR_fHelium.at(iP)->SetNpx(1000);
      fTrdLR_fHelium.at(iP)->SetLineColor(kBlue);
      fTrdLR_fHelium.at(iP)->SetParameter(0,double(iP));
      fTrdLR_fHelium.at(iP)->SetMinimum(1E-8);
    }

    FirstMCCall = false;
  }
  
  if(Debug > 1)
    std::cout << Form("### Check LR Calc_MC Electrons=%12.4E Protons=%12.4E Helium=%12.4E",
		      fTrdLR_fElectron.at(0)->Integral((double)TrdMinAdc,(double)TrdMaxAdc),
		      fTrdLR_fProton.at(0)->Integral((double)TrdMinAdc,(double)TrdMaxAdc),
		      fTrdLR_fHelium.at(0)->Integral((double)TrdMinAdc,(double)TrdMaxAdc))  
	      << std::endl;
  
  return true;

}
//--------------------------------------------------------------------------------------------------
vector<double> TrdSCalibR::TrdLR_MC_Calc(float Pabs,  
					     vector<AC_TrdHits*> TrdHits, vector<bool> PartId, 
					     int iFlag, int Debug) {

  vector<double> TrdLR_MC;
  TrdLR_MC.assign(3,-1.0);

  if(FirstMCCall) return TrdLR_MC;
  
  
  // Which Momentum Bin for Protons & Helium ?
  int iP 	= -1;
  int iMax 	= TrdLR_xProt.size()-1;
  for (unsigned int i=0; i<iMax; i++) {
    if (Pabs >= TrdLR_xProt.at(i) && Pabs < TrdLR_xProt.at(i+1)) {
      iP = i;
      break;
    }
  }
  if (Pabs >= TrdLR_xProt.at(iMax)) 	iP = iMax-1;
  if (Pabs <  TrdLR_xProt.at(0)) 	iP = 0;
  
  if (iP<0) {
    std::cerr << "Error in TrdLR_Calc: invalid momentum P=" << Pabs << std::endl;
    return TrdLR_MC;
  }
  
  int j = 0;
  bool IsProton, IsHelium, IsElectron, IsPositron, IsAntiProton;
  for(vector<bool>::iterator id = PartId.begin(); id != PartId.end(); ++id ) {
    if(j == 0)   IsProton	= *id;
    if(j == 1)   IsHelium	= *id;
    if(j == 2)   IsElectron	= *id;
    if(j == 3)   IsPositron	= *id;
    if(j == 4)   IsAntiProton	= *id;
    j++;
  }
  if( !IsProton && !IsHelium && !IsElectron) return TrdLR_MC;
  
  double x[1], par[1];
  int nCount = 0;

  vector<AC_TrdHits*>::iterator iter;
  for (iter = TrdHits.begin(); iter != TrdHits.end(); ++iter) 
    {
      if (std::fabs((*iter)->TrkD) > 0.999*trdconst::TrdStrawRadius) continue;
      int 	    Layer = (*iter)->Lay;
      double	    Len   = (*iter)->Len2D;
      if (iFlag==3) Len   = (*iter)->Len3D;
      if (Len < 0.05) continue;
      
      double Eadc = 0.0;
      
      if (IsElectron)   Eadc = fTrdLR_fElectron.at(Layer)->GetRandom();
      if (IsProton) 	Eadc = fTrdLR_fProton.at(iP)->GetRandom();
      if (IsHelium) 	Eadc = fTrdLR_fHelium.at(iP)->GetRandom();
    
      x[0] 		= Eadc;
      par[0] 	        = iP;	
      myPair <double> myProtonL  (1E-8, TrdSCalibR::TrdLR_fProton_v02(x,par));
      myPair <double> myHeliumL  (1E-8, TrdSCalibR::TrdLR_fHelium_v02(x,par));
      par[0]            = Layer;  
      myPair <double> myElectronL(1E-8, TrdSCalibR::TrdLR_fElectron_v02(x,par));
      double L_Proton   = myProtonL.GetMax();
      double L_Helium   = myHeliumL.GetMax();
      double L_Electron = myElectronL.GetMax();

      if(Debug > 1) 
	std::cout << Form("i=%3d EadcR=%4d EadcCS=%4d EadcMC=%4d L_Proton=%12.4E L_Helium=%12.4f L_Electron=%12.4f",
			  nCount, (int)(*iter)->EadcR, (int)(*iter)->EadcCS, (int)Eadc, L_Proton,L_Helium,L_Electron) 
		  << std::endl;
     
      Lprod_ProtonMC 	*= L_Proton;
      Lprod_HeliumMC 	*= L_Helium;
      Lprod_ElectronMC 	*= L_Electron;
      nCount ++;
    }

    if(Debug > 1) 
    std::cout << Form("Step 1. |P|=%6.2f(GeV/c) MC Lprod_ElectronMC=%12.4E Lprod_HeliumMC=%12.4E Lprod_ProtonMC=%12.4E", 
		      Pabs, Lprod_ElectronMC, Lprod_HeliumMC, Lprod_ProtonMC) << std::endl;
  
    Lprod_ProtonMC 	= TMath::Power(Lprod_ProtonMC,  1.0/float(nCount));
    Lprod_HeliumMC 	= TMath::Power(Lprod_HeliumMC,  1.0/float(nCount));
    Lprod_ElectronMC 	= TMath::Power(Lprod_ElectronMC,1.0/float(nCount));
    
    if(Debug > 1) 
      std::cout << Form("Step 2. |P|=%6.2f(GeV/c) MC Lprod_ElectronMC=%12.4E Lprod_HeliumMC=%12.4E Lprod_ProtonMC=%12.4E", 
			Pabs, Lprod_ElectronMC, Lprod_HeliumMC, Lprod_ProtonMC) << std::endl;
    
    double LR_Elec_Prot = Lprod_ElectronMC/(Lprod_ElectronMC+Lprod_ProtonMC);
    double LR_Heli_Prot	= Lprod_HeliumMC/(Lprod_HeliumMC+Lprod_ProtonMC);
    double LR_Elec_Heli	= Lprod_ElectronMC/(Lprod_ElectronMC+Lprod_HeliumMC);
    
    TrdLR_MC.at(0) = -log(LR_Elec_Prot);
    TrdLR_MC.at(1) = -log(LR_Heli_Prot);
    TrdLR_MC.at(2) = -log(LR_Elec_Heli);
    
    if(Debug > 1) 
      {
	int j = 0;
	std::cout << "MC " ;
	for(vector<double>::iterator lr = TrdLR_MC.begin(); lr != TrdLR_MC.end(); ++lr )
	  {
	    std::cout << Form("%s=%4.2f  ", TrdLLname[j++], *lr);
	  }
	std::cout << std::endl;
      }
  
  return TrdLR_MC;
}
//--------------------------------------------------------------------------------------------------
double TrdSCalibR::GetProtonBetheBlochCorrFactor(float Pabs){
  
  TF1 *fBetheBlochProton = new TF1("fBetheBlochProton",this, &TrdSCalibR::FunBetheBloch,0.1,1000.0,5);
  fBetheBlochProton->SetParameters(2.97171e+00,9.67198e+04,-3.14408e+00,trdconst::mProt,+1.0);  
  return fBetheBlochProton->Eval(Pabs);
}

double TrdSCalibR::GetHeliumBetheBlochCorrFactor(float Pabs){

  TF1 *fBetheBlochHelium = new TF1("fBetheBlochHelium",this, &TrdSCalibR::FunBetheBloch,0.1,1000.0,5);
  fBetheBlochHelium->SetParameters(3.32153e+00,9.62920e+07,-3.21979e+00,trdconst::mHeli,+2.0);
  return  fBetheBlochHelium->Eval(Pabs);
}
//--------------------------------------------------------------------------------------------------

int TrdSCalibR::RootTGraph2VectorX(TGraph *gr, vector<double> &vx) {

  int nDone = gr->GetN();
  for (int i=0; i<nDone; i++) {
    double x,y;
    gr->GetPoint(i,x,y);
    vx.push_back(x);
  }
  return 0;

}

int TrdSCalibR::RootTGraph2VectorY(TGraph *gr, vector<double> &vy) {

  int nDone = gr->GetN();
  for (int i=0; i<nDone; i++) {
    double x,y;
    gr->GetPoint(i,x,y);
    vy.push_back(y);
  }
  return 0;

}
//--------------------------------------------------------------------------------------------------

//vector< vector<double> > TrdSCalibR::TrdScalibXdays(trdconst::nTrdModules, vector<double>(15000)); 
//vector< vector<double> > TrdSCalibR::TrdScalibMpv(trdconst::nTrdModules, vector<double>(15000));   

bool TrdSCalibR::Get04TrdCalibration(TString fname, int Debug) {
	
  TFile *input = new TFile(fname);
  if (!input->IsOpen()) {
    std::cerr << "Error: Open Trd Calibration root file " << fname << std::endl;
    return false;
  }

  // read in calibration files
  vector<double> Runs;
  TGraph* grTrdCalibRuns = (TGraph*) input->Get("grTrdCalibRuns");
  int 	nCalibRuns = 0;
  
  if(grTrdCalibRuns) {
    RootTGraph2VectorY(grTrdCalibRuns, Runs);
    nCalibRuns 	        = Runs.size();
    FirstCalRunTime	= TMath::Nint( Runs.at(0) );
    LastCalRunTime	= TMath::Nint( Runs.at(nCalibRuns-1) ); 
    FirstXday 	        = GetEvtxTime((time_t) FirstCalRunTime, Debug); //GetStime((time_t)FirstRun)/86400.0;
    LastXday 	        = GetEvtxTime((time_t) LastCalRunTime,  Debug); //GetStime((time_t)LastRun)/86400.0;
  }
  //if(Debug)
    std::cout << Form("### Trd Calibration Runs=%6d Interval %5d - %5d <=> %7.4f - %7.4f days ###", 
		      nCalibRuns, FirstCalRunTime, LastCalRunTime, FirstXday, LastXday) << std::endl;
  
  if(FirstXday < 0 || LastXday < 0) return false;
  
  int nLost = 0;
  char grName[80];
  vector<double> iTrdScalibXdays, iTrdScalibMpv;
  for (int iMod = 0; iMod < nTrdModules; iMod++) 
    {
      iTrdScalibXdays.clear();
      iTrdScalibMpv.clear();
      sprintf(grName,"grTrdCalibMpv_%d",iMod);		
      TGraph *grTrd = (TGraph*)input->Get(grName);
      if (!grTrd) {
	std::cout << "Error in GetTrdCalibration_v04: TGraph " 
		  << grName << " not found !" << std::endl;
	nLost++;
      }
      //RootTGraph2VectorX(grTrd,TrdScalibXdays[Module]);
      //RootTGraph2VectorY(grTrd,TrdScalibMpv[Module]);
      
      RootTGraph2VectorX(grTrd, iTrdScalibXdays);
      RootTGraph2VectorY(grTrd, iTrdScalibMpv);

      TrdScalibXdays.push_back(iTrdScalibXdays);
      TrdScalibMpv.push_back(iTrdScalibMpv);
    }
  input->Close();
  delete input;

 
  if (nLost==0) {
    std::cout << "Trd 4. Calibration file " << fname 
	      << " is sucessfully loaded "  << std::endl;
    return true;
  } else {
    std::cout << "Trd 4. Calibration file " << fname 
	      << " will be used. nLost=" << nLost << std::endl;
    return false;
  }



}
//--------------------------------------------------------------------------------------------------
bool TrdSCalibR::GetTrdV4CalibHistos(int CalibLevel, int Debug) {
  
  FirstCall = true;  TrdCalib_01 = false;  TrdCalib_02 = false; TrdCalib_03 = false; TrdCalib_04 = false;
  /// Get TRD Calibration Histograms
  if (FirstCall) {
    if(Debug) 
      std::cout << "Get TRD Calibration Histograms: CalibLevel=" << CalibLevel << std::endl;
    cout << "----------------------------------------" << endl;
    char tdname[200]; 
   
    if (CalibLevel>3) 
      {
	sprintf(tdname, "%s/%s", TrdDBUpdateDir[0], TrdCalDBUpdate[2]);
	TrdCalib_04 = Get04TrdCalibration(tdname, Debug);

	std::cout << Form("### ModCalibTime= %10u - %10u <=> %5.3f - %5.3f days",
			  FirstCalRunTime, LastCalRunTime, FirstXday, LastXday) << std::endl;
      }
    
    
    if(Debug) 
      std::cout <<Form("TrdCalib1=%s TrdCalib2=%s TrdCalib3=%s",
		       TrdCalib_01?"true":"false",
		       TrdCalib_02?"true":"false",
		       TrdCalib_03?"true":"false") <<std::endl;
    FirstCall = false;
   
  }

  if (  TrdCalib_04 && CalibLevel>3 ) 
    return true;
  else return false;
    
}
//-------------------------------------------------------------------------------------------------
bool TrdSCalibR::TrdScalibMinDist(double xDayRef, int aP, int iMod) {
  double dist0 = std::fabs(TrdScalibXdays[iMod].at(std::max(0, aP-1))-xDayRef);
  double dist1 = std::fabs(TrdScalibXdays[iMod].at(aP)-xDayRef);
  double dist2 = std::fabs(TrdScalibXdays[iMod].at(std::min(aP+1,int(TrdScalibXdays[iMod].size()-1)))-xDayRef);
  
  if (dist1<=dist0 && dist1<=dist2) return true;
  return false;
}

//--------------------------------------------------------------------------------------------------
// find a calibration constant within +/- 6 hours of key
int TrdSCalibR::TrdScalibBinarySearch(double key, int iMod, int Debug) {
  int 		first = 0;
  int 		last  = TrdScalibXdays[iMod].size()-1; 
  
  if (key<=TrdScalibXdays[iMod].at(first)) return first;
  if (key>=TrdScalibXdays[iMod].at(last))  return last;
  
  int iP = (first + last) / 2;
  while (last-first>1) 
    {
      iP = (first + last) / 2;  // compute mid point.
      if (key > TrdScalibXdays[iMod].at(iP)) {
	first = iP;  // repeat search in top half.
      } else if (key < TrdScalibXdays[iMod].at(iP)) {
	last = iP; // repeat search in bottom half.
      } else {
	break;
      }
    }
  
  
  // find the closest day
  double Distance1 = 0, Distance2 = 0;
  if (TrdScalibXdays[iMod].at(iP)<key && iP<TrdScalibXdays[iMod].size()-2) 
    {
      do 
	{
	  Distance1 = std::fabs(TrdScalibXdays[iMod].at(iP)-key);
	  iP++;
	  Distance2 = std::fabs(TrdScalibXdays[iMod].at(iP)-key);
	  if(Debug > 1)
	    std::cout << Form("1. iP=%4d %8.4f Distance1=%8.4f Distance2=%8.4f\n",
			      iP,TrdScalibXdays[iMod].at(iP),Distance1,Distance2) << std::endl;
	} while (Distance2<Distance1 && iP<TrdScalibXdays[iMod].size()-2);
      iP--;
    } else if (TrdScalibXdays[iMod].at(iP)>key && iP>0 )
    {
      do 
	{
	  Distance1 = std::fabs(TrdScalibXdays[iMod].at(iP)-key);
	  iP--;
	  Distance2 = std::fabs(TrdScalibXdays[iMod].at(iP)-key);
	  if(Debug > 1)
	    std::cout << Form("2. iP=%4d %8.4f Distance1=%8.4f Distance2=%8.4f\n",
			    iP,TrdScalibXdays[iMod].at(iP),Distance1,Distance2) << std::endl;
	} while (Distance2<Distance1 && iP>0);
      iP++;
    }
  return iP;
}
//--------------------------------------------------------------------------------------------------

double TrdSCalibR::TrdScalibInterpolate(int iMod, double xDayRef, int &aP, int Debug) {

  if ( aP >= 0 && aP < int(TrdScalibXdays[iMod].size()-1) ) 
    {
      if (TrdScalibMinDist(xDayRef, aP, iMod)) {
      } else if (TrdScalibMinDist(xDayRef,aP+1, iMod)) {
	aP ++;
      } else {
	aP = TrdScalibBinarySearch(xDayRef, iMod, Debug);		
      }
    } else {
    aP = TrdScalibBinarySearch(xDayRef, iMod, Debug);				
  }

  
  double mpv = TrdMeanMPV;
  if (aP<0 || aP>TrdScalibXdays[iMod].size()-1) return mpv;
  
  mpv = TrdScalibMpv[iMod][aP];
    
  double x1=0,x2=0, y1=0,y2=0;
  if (TrdScalibXdays[iMod][aP]<xDayRef && aP<TrdScalibXdays[iMod].size()-2) {
    x1  = TrdScalibXdays[iMod][aP];
    y1  = TrdScalibMpv[iMod][aP];
    x2  = TrdScalibXdays[iMod][aP+1];
    y2  = TrdScalibMpv[iMod][aP+1];
    mpv = y2/(x2-x1)*(xDayRef-x1) + y1/(x2-x1)*(x2-xDayRef);
  } else if (TrdScalibXdays[iMod][aP]>xDayRef && aP>0) {
    x1  = TrdScalibXdays[iMod][aP-1];
    y1  = TrdScalibMpv[iMod][aP-1];
    x2  = TrdScalibXdays[iMod][aP];
    y2  = TrdScalibMpv[iMod][aP];
    mpv = y2/(x2-x1)*(xDayRef-x1) + y1/(x2-x1)*(x2-xDayRef);
  }

  if(Debug > 1) {
    std::cout << Form("<< aP[%03d]=%4d xDayRef=%8.4f xDay.at(aP)=%8.4f ",
		      iMod, aP,xDayRef,TrdScalibXdays[iMod].at(aP));
    std::cout << Form("MPV=%8.4f >>", mpv) 
	      << std::endl;
  }
  
  return mpv;
}
//--------------------------------------------------------------------------------------------------

int TrdSCalibR::BuildTrdSCalib(time_t evut, double fMom, TrdHTrackR *TrdHtrk, TrTrackR *Trtrk, double &s1, double &s2, double &s3, int Debug){
 
  s1 = s2 = s3 = 0.0;

  if(fMom < 3.0 || fMom > 100)  return 1;

  Htime = GetEvthTime(evut, Debug); 
  Xtime = GetEvtxTime(evut, Debug); 

  if ( SCalibLevel == 3 && (Xtime < FirstCalRunTime || Xtime > LastCalRunTime) ) 
    return 3;
  if ( SCalibLevel == 4 && (Xtime < FirstXday || Xtime > LastXday) ) 
    return 4;

  if(Debug > 1) {
    std::cout << Form("TrdSCalibLevel=%d TrdTrackLevel=%d TrdiFlag=%d ", SCalibLevel, TrdTrackLevel, iFlag)
	      << Form("Pabs=%6.3f Htime=%6d Xtime=%8.4f", Pabs, Htime, Xtime)
	      << std::endl;
  }


  
  if( !GetdTrd(TrdHtrk) ||  !GetcTrd(TrdHtrk) || !GetcTrkdTrk(Trtrk) ) return 2;
  
  if(TrdHtrk->NTrdHSegment() != 2) return 3;
	   
  if(Debug > 1) 
    std::cout << Form("TrdHTrack Coo=(%+5.2f, %5.2f %5.2f)",
		      TrdHtrk->Coo[0],TrdHtrk->Coo[1],TrdHtrk->Coo[2]) << std::endl;

  nTrdHitLayer.assign(trdconst::nTrdLayers, 0);

  TrdSHits.clear();

  aP.assign(trdconst::nTrdModules, 0);
	  
  int itrdhit = 0;
  for (unsigned int iseg=0; iseg < TrdHtrk->NTrdHSegment(); iseg++) 
    {
      if(!TrdHtrk->pTrdHSegment(iseg)) continue;
      
      int itrdhit = 0;
      for(int ir=0; ir < (int)TrdHtrk->pTrdHSegment(iseg)->fTrdRawHit.size();ir++)
	{
	  TrdRawHitR* rhit=TrdHtrk->pTrdHSegment(iseg)->pTrdRawHit(ir);
	  
	  if(!rhit) continue;
	  if( rhit->Amp < trdconst::TrdMinAdc ) continue;	       
	  
	  AC_TrdHits * AC = new AC_TrdHits();
	  AC->GetHitInfo(rhit);
	  AC->SetHitPos();

	  AC->TrkD    = AC->GetTrdHitTrkDistance(AC->hitD, AC->hitXY, AC->hitZ, cTrk, dTrk);
	  AC->TrdD    = AC->GetTrdHitTrkDistance(AC->hitD, AC->hitXY, AC->hitZ, cTrd, dTrd);
	
	  if(fabs(AC->TrkD) > 0.35 && fabs(AC->TrdD) > 0.32) continue; 
	 
	  /// calculate path length correction
	  AC->Len2D = 2.0*TMath::Sqrt(TMath::Power(trdconst::TrdStrawRadius,2) - TMath::Power(AC->TrkD,2));
	  AC->Len3D = AC->GetTrdPathLen3D(AC->Lay,AC->hitXY, AC->hitZ, cTrk, dTrk);
	  if( AC->Len3D < trdconst::TrdMinPathLen3D ) continue;   
	 
	  double TrdCalibMPV = 0;
	  if(SCalibLevel < 3) {
	    GetBinGasCirModule(Htime, SCalibLevel, AC->GC, AC->Mod, Debug); 

	    /// calibration in the gas circuit level 
	    double TrdMPVofTheDay   = h_TrdGasCirMPV.at(AC->GC)->GetBinContent(iBinGasCir);
	    if (TrdMPVofTheDay>= 10.0 && TrdMPVofTheDay<=200.0)	
	      AC->EadcC1 = AC->EadcR * trdconst::TrdMeanMPV / TrdMPVofTheDay;
	    if(Debug > 1) 
	      std::cout << Form("+-+-+-+- Htime=%5d iBinGasCir=%2d TrdMPVofTheDay=%3.1f Scale=%3.1f ", 
				Htime, iBinGasCir, TrdMPVofTheDay, trdconst::TrdMeanMPV / TrdMPVofTheDay); 
	    
	    /// calibration in the module level 
	    if (iBinModule>0) {
	      double ModMPVofTheDay = h_TrdModuleMPV.at(AC->Mod)->GetBinContent(iBinModule);
	      if (ModMPVofTheDay>=10 && ModMPVofTheDay<=200.0) 
		AC->EadcCS = AC->EadcC1 * trdconst::TrdMeanMPVSC / ModMPVofTheDay; 
	      if(Debug > 1) 
		std::cout << Form("iBinModule=%4d ModMPVofTheDay=%3.1f Scale=%3.1f ", 
				  iBinModule, ModMPVofTheDay, trdconst::TrdMeanMPVSC / ModMPVofTheDay) 
			  << std::endl;
	    }
	  } else if (SCalibLevel < 4) /// CalibLevel = 3 for version 3
	    {
	      TrdCalibMPV = g_TrdCalibMPV.at(AC->Mod)->Eval(Xtime);
	      if (TrdCalibMPV>10 && TrdCalibMPV<200.0) 
		AC->EadcCS = AC->EadcR * trdconst::TrdMeanMPV / TrdCalibMPV; 
	    }
	  else {
	    TrdCalibMPV = TrdScalibInterpolate(AC->Mod, Xtime, aP.at(AC->Mod), Debug);
	    if (TrdCalibMPV>10 && TrdCalibMPV<200.0) 
	      AC->EadcCS = AC->EadcR * trdconst::TrdMeanMPV / TrdCalibMPV;
	  }
	  
	  if (AC->EadcCS < trdconst::TrdMinAdc || AC->EadcCS > trdconst::TrdMaxAdc) continue;
	  
	 
	  if(Debug > 1) { 
	    std::cout << std::endl
		      << Form("+-+ [%02d] iSeg=%d Layer=%2d Ladder=%2d Tube=%2d EadcR=%4d D=%d XY=%5.1f Z=%5.1f ", 
			      itrdhit, iseg, AC->Lay, AC->Lad, AC->Tub, (int)AC->EadcR, AC->hitD, AC->hitXY, AC->hitZ); 
	    std::cout << Form("DHitTrk=%+6.2f DHitTrd=%+6.2f ", AC->TrkD, AC->TrdD);	   
	    std::cout << Form("StrawNr=%4d Mod=%3d GasCir=%2d ", AC->Straw, AC->Mod, AC->GC );	   		   
	    std::cout << Form("Len2D=%5.2f Len3D=%5.2f ", AC->Len2D, AC->Len3D);
	    std::cout << Form("EadcR=%4d EadcC1=%4d EadcCS=%4d ", (int)AC->EadcR, (int) AC->EadcC1, (int) AC->EadcCS) 
		      << std::endl;
	  }
  
	  TrdSHits.push_back(AC);
	  nTrdHitLayer[AC->Lay]++;        

	  itrdhit++;
	}
    }

  



  TrdSum8Amp = GetTrdSum8(TrdSHits, Debug);
  
  TrdLRs     = TrdLR_Calc(fMom, TrdSHits, iFlag, Debug); 
  
  if ( TrdSHits.size() != 0) 
    for(int i=0; i< TrdSHits.size()-1;i++)  delete TrdSHits[i];

  s1 = TrdLRs[0]; //TrdLRLprod_Proton;
  s2 = TrdLRs[1]; //Lprod_Helium;
  s3 = TrdLRs[2]; //Lprod_Electron;

 

  return 0;
}

//--------------------------------------------------------------------------------------------------

int TrdSCalibR::ProcessTrdHit(TrdHTrackR *TrdHtrk, TrTrackR *Trtrk, int Debug){
  
  
  if( !GetdTrd(TrdHtrk) ||  !GetcTrd(TrdHtrk) || !GetcTrkdTrk(Trtrk) ) return 1;
  
  if(TrdHtrk->NTrdHSegment() != 2) return 2;
	   
  if (!MatchingTrdTKtrack(Pabs, Debug) ) return 3; 

  if(Debug > 1) 
    std::cout << Form("TrdHTrack Coo=(%+5.2f, %5.2f %5.2f)",
		      TrdHtrk->Coo[0],TrdHtrk->Coo[1],TrdHtrk->Coo[2]) << std::endl;

  nTrdHitLayer.assign(trdconst::nTrdLayers, 0);

  TrdSHits.clear();

  aP.assign(trdconst::nTrdModules, 0);
	  
  for (unsigned int iseg=0; iseg < TrdHtrk->NTrdHSegment(); iseg++) 
    {
      if(!TrdHtrk->pTrdHSegment(iseg)) continue;
      
      int itrdhit = 0;
      for(int ir=0; ir < (int)TrdHtrk->pTrdHSegment(iseg)->fTrdRawHit.size();ir++)
	{
	  TrdRawHitR* rhit=TrdHtrk->pTrdHSegment(iseg)->pTrdRawHit(ir);
	  
	  if(!rhit) continue;
	  if( rhit->Amp < trdconst::TrdMinAdc ) continue;	       
	  
	  AC_TrdHits * AC = new AC_TrdHits();
	  AC->GetHitInfo(rhit);
	  AC->SetHitPos();

	  AC->TrkD    = AC->GetTrdHitTrkDistance(AC->hitD, AC->hitXY, AC->hitZ, cTrk, dTrk);
	  AC->TrdD    = AC->GetTrdHitTrkDistance(AC->hitD, AC->hitXY, AC->hitZ, cTrd, dTrd);
	
	  if(fabs(AC->TrkD) > 0.35 && fabs(AC->TrdD) > 0.32) continue; 
	  
	  if(Debug > 1) { 
	    std::cout << std::endl
		 << Form("+-+ [%02d] iSeg=%d Layer=%2d Ladder=%2d Tube=%2d EadcR=%4d D=%d XY=%5.1f Z=%5.1f ", 
			 itrdhit, iseg, AC->Lay, AC->Lad, AC->Tub, (int)AC->EadcR, AC->hitD, AC->hitXY, AC->hitZ); 
	    std::cout << Form("DHitTrk=%5.2f DHitTrd=%5.2f ", AC->TrkD, AC->TrdD);	   
	    std::cout << Form("StrawNr=%4d Mod=%3d GasCir=%2d ", 
			      AC->Straw, AC->Mod, AC->GC );	   		   
	    
	  }
	  
	  /// calculate path length correction
	  AC->Len2D = 2.0*TMath::Sqrt(TMath::Power(trdconst::TrdStrawRadius,2) - TMath::Power(AC->TrkD,2));
	  AC->Len3D = AC->GetTrdPathLen3D(AC->Lay,AC->hitXY, AC->hitZ, cTrk, dTrk);
	  if( AC->Len3D < trdconst::TrdMinPathLen3D ) continue;   
	  
	  if(Debug > 1) std::cout << Form("Len2D=%5.2f Len3D=%5.2f ", AC->Len2D, AC->Len3D);

	  double TrdCalibMPV = 0;
	  if(SCalibLevel < 3) {
	    GetBinGasCirModule(Htime, SCalibLevel, AC->GC, AC->Mod, Debug); 

	    /// calibration in the gas circuit level 
	    double TrdMPVofTheDay   = h_TrdGasCirMPV.at(AC->GC)->GetBinContent(iBinGasCir);
	    if (TrdMPVofTheDay>= 10.0 && TrdMPVofTheDay<=200.0)	
	      AC->EadcC1 = AC->EadcR * trdconst::TrdMeanMPV / TrdMPVofTheDay;
	    if(Debug > 1) 
	      std::cout << Form("+-+-+-+- Htime=%5d iBinGasCir=%2d TrdMPVofTheDay=%3.1f Scale=%3.1f ", 
				Htime, iBinGasCir, TrdMPVofTheDay, trdconst::TrdMeanMPV / TrdMPVofTheDay); 
	    
	    /// calibration in the module level 
	    if (iBinModule>0) {
	      double ModMPVofTheDay = h_TrdModuleMPV.at(AC->Mod)->GetBinContent(iBinModule);
	      if (ModMPVofTheDay>=10 && ModMPVofTheDay<=200.0) 
		AC->EadcCS = AC->EadcC1 * trdconst::TrdMeanMPVSC / ModMPVofTheDay; 
	      if(Debug > 1) 
		std::cout << Form("iBinModule=%4d ModMPVofTheDay=%3.1f Scale=%3.1f ", 
				  iBinModule, ModMPVofTheDay, trdconst::TrdMeanMPVSC / ModMPVofTheDay) 
			  << std::endl;
	    }
	  } else if (SCalibLevel < 4) /// CalibLevel = 3 for version 3
	    {
	      TrdCalibMPV = g_TrdCalibMPV.at(AC->Mod)->Eval(Xtime);
	      if (TrdCalibMPV>10 && TrdCalibMPV<200.0) 
		AC->EadcCS = AC->EadcR * trdconst::TrdMeanMPV / TrdCalibMPV; 
	    }
	  else {
	    TrdCalibMPV = TrdScalibInterpolate(AC->Mod, Xtime, aP.at(AC->Mod), Debug);
	    if (TrdCalibMPV>10 && TrdCalibMPV<200.0) 
	      AC->EadcCS = AC->EadcR * trdconst::TrdMeanMPV / TrdCalibMPV;
	  }
	  
	  if (AC->EadcCS < trdconst::TrdMinAdc || AC->EadcCS > trdconst::TrdMaxAdc) continue;
	  
	  if(Debug > 1) 
	    std::cout << Form("EadcR=%4d EadcC1=%4d EadcCS=%4d ", (int)AC->EadcR, (int) AC->EadcC1, (int) AC->EadcCS) 
		      << std::endl;
  
	  TrdSHits.push_back(AC);
	  nTrdHitLayer[AC->Lay]++;        

	  itrdhit++;
	}
    }


  TrdSum8Amp = GetTrdSum8(TrdSHits, Debug);
  
  TrdLRs     = TrdLR_Calc(Pabs, TrdSHits, iFlag, Debug); 
 
  if ( TrdSHits.size() != 0) 
    for(int i=0; i< TrdSHits.size()-1;i++)  delete TrdSHits[i];


  return 0;
}

//--------------------------------------------------------------------------------------------------
int TrdSCalibR::BuildTrdSCalib(time_t evut, double fMom, TrdTrackR *Trdtrk, TrTrackR *Trtrk, double &s1, double &s2, double &s3, int Debug){
 
  Htime = GetEvthTime(evut, Debug); 
  Xtime = GetEvtxTime(evut, Debug); 

  if ( SCalibLevel == 3 && (Xtime < FirstCalRunTime || Xtime > LastCalRunTime) ) 
    return 3;
  if ( SCalibLevel == 4 && (Xtime < FirstXday || Xtime > LastXday) ) 
    return 4;

  if(Debug > 1) {
    std::cout << Form("TrdSCalibLevel=%d TrdTrackLevel=%d TrdiFlag=%d ", SCalibLevel, TrdTrackLevel, iFlag)
	      << Form("Pabs=%6.3f Htime=%6d Xtime=%8.4f", Pabs, Htime, Xtime)
	      << std::endl;
  }


  if( !GetdTrd(Trdtrk) ||  !GetcTrd(Trdtrk) || !GetcTrkdTrk(Trtrk) ) return 1;

  if(Trdtrk->NTrdSegment() != 4 ) return 2;   

  if (!MatchingTrdTKtrack(Pabs, Debug) ) return 3; 
	   
  if(Debug > 1) 
    std::cout << Form("TrdTrack Coo=(%+5.2f, %5.2f %5.2f)",
		      Trdtrk->Coo[0],Trdtrk->Coo[1],Trdtrk->Coo[2]) 
	      << std::endl;

  nTrdHitLayer.assign(trdconst::nTrdLayers, 0);
  
  TrdSHits.clear();
  
  aP.assign(trdconst::nTrdModules, 0);
	  
  for (int iseg = 0; iseg < Trdtrk->NTrdSegment(); iseg++)
    {		
      TrdSegmentR *trdseg = Trdtrk->pTrdSegment(iseg);
      if(!trdseg) continue;
      
      int itrdhit = 0;
      for (int j = 0; j <  trdseg->NTrdCluster(); j++)
	{
	  TrdClusterR  *trdcl = trdseg->pTrdCluster(j);
	  if(!trdcl) continue;
	  
	  TrdRawHitR  *trdrh  = trdcl->pTrdRawHit();
	  if(!trdrh) continue;	  
	  if( trdrh->Amp < trdconst::TrdMinAdc ) continue;
	  
	  /// fill AC_Hits object members
	  AC_TrdHits * AC = new AC_TrdHits();
	  AC->GetHitInfo(trdrh);
	  AC->SetHitPos();
	  
	  AC->TrkD    = AC->GetTrdHitTrkDistance(AC->hitD, AC->hitXY, AC->hitZ, cTrk, dTrk);
	  AC->TrdD    = AC->GetTrdHitTrkDistance(AC->hitD, AC->hitXY, AC->hitZ, cTrd, dTrd);
	  
	  if(fabs(AC->TrkD) > 0.35 && fabs(AC->TrdD) > 0.32) continue;                 

	  /// check AC_Hits 
	  if(Debug > 1) { 
	    cout << Form("+-+ [%02d] iSeg=%d Layer=%2d Ladder=%2d Tube=%2d EadcR=%4d D=%d XY=%5.1f Z=%5.1f ", 
			 itrdhit, iseg, AC->Lay, AC->Lad, AC->Tub, (int)AC->EadcR, AC->hitD, AC->hitXY, AC->hitZ); 
	    cout << Form("DHitTrk=%5.2f DHitTrd=%5.2f ", AC->TrkD, AC->TrdD);	   
	    cout << Form("StrawNr=%4d Mod=%3d GasCir=%2d ", 
			 AC->Straw, AC->Mod, AC->GC );	   		   
	  }
	  
	  /// calculate path length correction
	  AC->Len2D = 2.0*TMath::Sqrt(TMath::Power(trdconst::TrdStrawRadius,2) - TMath::Power(AC->TrkD,2));
	  AC->Len3D = AC->GetTrdPathLen3D(AC->Lay,AC->hitXY, AC->hitZ, cTrk, dTrk);
	  if( AC->Len3D < trdconst::TrdMinPathLen3D ) continue;   

	  if(Debug > 1) std::cout << Form("Len2D=%5.2f Len3D=%5.2f ", AC->Len2D, AC->Len3D);
	  
	  double TrdCalibMPV = 0;
	  if(SCalibLevel < 3) {
	    GetBinGasCirModule(Htime, SCalibLevel, AC->GC, AC->Mod, Debug); 
	    
	    /// calibration in the gas circuit level 
	    double TrdMPVofTheDay   = h_TrdGasCirMPV.at(AC->GC)->GetBinContent(iBinGasCir);
	    if (TrdMPVofTheDay>= 10.0 && TrdMPVofTheDay<=200.0)	
	      AC->EadcC1 = AC->EadcR * trdconst::TrdMeanMPV / TrdMPVofTheDay;
	    if(Debug > 1) 
	      std::cout << Form("+-+-+-+- Htime=%5d iBinGasCir=%2d TrdMPVofTheDay=%3.1f Scale=%3.1f ", 
				Htime, iBinGasCir, TrdMPVofTheDay, trdconst::TrdMeanMPV / TrdMPVofTheDay); 
	    
	    /// calibration in the module level 
	    if (iBinModule>0) {
	      double ModMPVofTheDay = h_TrdModuleMPV.at(AC->Mod)->GetBinContent(iBinModule);
	      if (ModMPVofTheDay>=10 && ModMPVofTheDay<=200.0) 
		AC->EadcCS = AC->EadcC1 * trdconst::TrdMeanMPVSC / ModMPVofTheDay; 
	      if(Debug > 1) 
		std::cout << Form("iBinModule=%4d ModMPVofTheDay=%3.1f Scale=%3.1f ", 
				  iBinModule, ModMPVofTheDay, trdconst::TrdMeanMPVSC / ModMPVofTheDay) 
			  << std::endl;
	    }
	  } else if (SCalibLevel < 4) /// CalibLevel = 3 for version 3
	    {
	      TrdCalibMPV = g_TrdCalibMPV.at(AC->Mod)->Eval(Xtime);
	      if (TrdCalibMPV>10 && TrdCalibMPV<200.0) 
		AC->EadcCS = AC->EadcR * trdconst::TrdMeanMPV / TrdCalibMPV; 
	    }
	  else {
	    TrdCalibMPV = TrdScalibInterpolate(AC->Mod, Xtime, aP.at(AC->Mod), Debug);
	    if (TrdCalibMPV>10 && TrdCalibMPV<200.0) 
	      AC->EadcCS = AC->EadcR * trdconst::TrdMeanMPV / TrdCalibMPV;
	  }
	  
	  if (AC->EadcCS < trdconst::TrdMinAdc || AC->EadcCS > trdconst::TrdMaxAdc) continue;
	  
	  if(Debug > 1) 
	    std::cout << Form("EadcR=%4d EadcC1=%4d EadcCS=%4d ", (int)AC->EadcR, (int) AC->EadcC1, (int) AC->EadcCS) 
		      << std::endl;
  
	  TrdSHits.push_back(AC);
	  nTrdHitLayer[AC->Lay]++;       

	  itrdhit++;

	}
    }
  
  
  TrdSum8Amp = GetTrdSum8(TrdSHits, Debug);
 
  TrdLRs     = TrdLR_Calc(Pabs, TrdSHits, iFlag, Debug); 
  
  if ( TrdSHits.size() != 0) 
    for(int i=0; i< TrdSHits.size()-1;i++)  delete TrdSHits[i];

  s1 = TrdLRs[0]; //TrdLRLprod_Proton;
  s2 = TrdLRs[1]; //Lprod_Helium;
  s3 = TrdLRs[2]; //Lprod_Electron;
 

  return 0;
}

//--------------------------------------------------------------------------------------------------

int TrdSCalibR::ProcessTrdHit(TrdTrackR *Trdtrk, TrTrackR *Trtrk, int Debug){

  if( !GetdTrd(Trdtrk) ||  !GetcTrd(Trdtrk) || !GetcTrkdTrk(Trtrk) ) return 1;
  
  if(Trdtrk->NTrdSegment() != 4 ) return 2;   

  if (!MatchingTrdTKtrack(Pabs, Debug) ) return 3; 
	   
  if(Debug > 1) 
    std::cout << Form("TrdTrack Coo=(%+5.2f, %5.2f %5.2f)",
		      Trdtrk->Coo[0],Trdtrk->Coo[1],Trdtrk->Coo[2]) 
	      << std::endl;
    

  nTrdHitLayer.assign(trdconst::nTrdLayers, 0);
  
  TrdSHits.clear();

  aP.assign(trdconst::nTrdModules, 0);

  for (int iseg = 0; iseg < Trdtrk->NTrdSegment(); iseg++)
    {		
      TrdSegmentR *trdseg = Trdtrk->pTrdSegment(iseg);
      if(!trdseg) continue;
      
      int itrdhit = 0;
      for (int j = 0; j <  trdseg->NTrdCluster(); j++)
	{
	  TrdClusterR  *trdcl = trdseg->pTrdCluster(j);
	  if(!trdcl) continue;
	  
	  TrdRawHitR  *trdrh  = trdcl->pTrdRawHit();
	  if(!trdrh) continue;	  
	  if( trdrh->Amp < trdconst::TrdMinAdc ) continue;
	  
	  /// fill AC_Hits object members
	  AC_TrdHits * AC = new AC_TrdHits();
	  AC->GetHitInfo(trdrh);
	  AC->SetHitPos();
	  
	  AC->TrkD    = AC->GetTrdHitTrkDistance(AC->hitD, AC->hitXY, AC->hitZ, cTrk, dTrk);
	  AC->TrdD    = AC->GetTrdHitTrkDistance(AC->hitD, AC->hitXY, AC->hitZ, cTrd, dTrd);
	  
	  if(fabs(AC->TrkD) > 0.35 && fabs(AC->TrdD) > 0.32) continue;                 

	  /// check AC_Hits 
	  if(Debug > 1) { 
	    cout << Form("+-+ [%02d] iSeg=%d Layer=%2d Ladder=%2d Tube=%2d EadcR=%4d D=%d XY=%5.1f Z=%5.1f ", 
			 itrdhit, iseg, AC->Lay, AC->Lad, AC->Tub, (int)AC->EadcR, AC->hitD, AC->hitXY, AC->hitZ); 
	    cout << Form("DHitTrk=%5.2f DHitTrd=%5.2f ", AC->TrkD, AC->TrdD);	   
	    cout << Form("StrawNr=%4d Mod=%3d GasCir=%2d ", 
			 AC->Straw, AC->Mod, AC->GC );	   		   
	  }
	  
	  /// calculate path length correction
	  AC->Len2D = 2.0*TMath::Sqrt(TMath::Power(trdconst::TrdStrawRadius,2) - TMath::Power(AC->TrkD,2));
	  AC->Len3D = AC->GetTrdPathLen3D(AC->Lay,AC->hitXY, AC->hitZ, cTrk, dTrk);
	  if( AC->Len3D < trdconst::TrdMinPathLen3D ) continue;   

	  if(Debug > 1) std::cout << Form("Len2D=%5.2f Len3D=%5.2f ", AC->Len2D, AC->Len3D);

	  double TrdCalibMPV = 0;
	  if(SCalibLevel < 3) {
	    GetBinGasCirModule(Htime, SCalibLevel, AC->GC, AC->Mod, Debug); 

	    /// calibration in the gas circuit level 
	    double TrdMPVofTheDay   = h_TrdGasCirMPV.at(AC->GC)->GetBinContent(iBinGasCir);
	    if (TrdMPVofTheDay>= 10.0 && TrdMPVofTheDay<=200.0)	
	      AC->EadcC1 = AC->EadcR * trdconst::TrdMeanMPV / TrdMPVofTheDay;
	    if(Debug > 1) 
	      std::cout << Form("+-+-+-+- Htime=%5d iBinGasCir=%2d TrdMPVofTheDay=%3.1f Scale=%3.1f ", 
				Htime, iBinGasCir, TrdMPVofTheDay, trdconst::TrdMeanMPV / TrdMPVofTheDay); 
	    
	    /// calibration in the module level 
	    if (iBinModule>0) {
	      double ModMPVofTheDay = h_TrdModuleMPV.at(AC->Mod)->GetBinContent(iBinModule);
	      if (ModMPVofTheDay>=10 && ModMPVofTheDay<=200.0) 
		AC->EadcCS = AC->EadcC1 * trdconst::TrdMeanMPVSC / ModMPVofTheDay; 
	      if(Debug > 1) 
		std::cout << Form("iBinModule=%4d ModMPVofTheDay=%3.1f Scale=%3.1f ", 
				  iBinModule, ModMPVofTheDay, trdconst::TrdMeanMPVSC / ModMPVofTheDay) 
			  << std::endl;
	    }
	  } else if (SCalibLevel < 4) /// CalibLevel = 3 for version 3
	    {
	      TrdCalibMPV = g_TrdCalibMPV.at(AC->Mod)->Eval(Xtime);
	      if (TrdCalibMPV>10 && TrdCalibMPV<200.0) 
		AC->EadcCS = AC->EadcR * trdconst::TrdMeanMPV / TrdCalibMPV; 
	    }
	  else {
	    TrdCalibMPV = TrdScalibInterpolate(AC->Mod, Xtime, aP.at(AC->Mod), Debug);
	    if (TrdCalibMPV>10 && TrdCalibMPV<200.0) 
	      AC->EadcCS = AC->EadcR * trdconst::TrdMeanMPV / TrdCalibMPV;
	  }
	  
	  if (AC->EadcCS < trdconst::TrdMinAdc || AC->EadcCS > trdconst::TrdMaxAdc) continue;
	  
	  if(Debug > 1) 
	    std::cout << Form("EadcR=%4d EadcC1=%4d EadcCS=%4d ", (int)AC->EadcR, (int) AC->EadcC1, (int) AC->EadcCS) 
		      << std::endl;
  
	  TrdSHits.push_back(AC);
	  nTrdHitLayer[AC->Lay]++;       

	  itrdhit++;

	}
    }
  
  
  TrdSum8Amp = GetTrdSum8(TrdSHits, Debug);
 
  TrdLRs     = TrdLR_Calc(Pabs, TrdSHits, iFlag, Debug); 
  
  if ( TrdSHits.size() != 0) 
    for(int i=0; i< TrdSHits.size()-1;i++)  delete TrdSHits[i];

  return 0;
}

//--------------------------------------------------------------------------------------------------

int TrdSCalibR::ProcessTrdEvt(AMSEventR *pev, int Debug) {

  if( pev->nParticle() != 1 ) return 1; 
  Pabs = std::fabs(pev->pParticle(0)->Momentum);

  if( Pabs<CalMomMin || Pabs>CalMomMax ) return 1; 
   
  if(!pev->pParticle(0)->pTrTrack())    return 2;  
  TrTrackR    *Trtrk   = pev->pParticle(0)->pTrTrack();
 
  Htime = GetEvthTime(pev, Debug); //== used ver.1 & ver.2 only
  Xtime = GetEvtxTime(pev, Debug); //== used ver.3

  if ( SCalibLevel == 3 && (Xtime < FirstCalRunTime || Xtime > LastCalRunTime) ) 
    return 3;
  if ( SCalibLevel == 4 && (Xtime < FirstXday || Xtime > LastXday) ) 
    return 4;

  if(Debug > 1) {
    std::cout << Form("TrdSCalibLevel=%d TrdTrackLevel=%d TrdiFlag=%d ", SCalibLevel, TrdTrackLevel, iFlag)
	      << Form("Pabs=%6.3f Htime=%6d Xtime=%8.4f", Pabs, Htime, Xtime)
	      << std::endl;
  }

  if (TrdTrackLevel < 1) 
    {
      if( pev->nTrdHTrack() == 0 ) return 2;
      if(!pev->pParticle(0)->pTrdHTrack())  return 3;
      TrdHTrackR  *TrdHtrk = pev->pParticle(0)->pTrdHTrack();
      if ( TrdHtrk->Chi2 <= 0.0 || (TrdHtrk->Chi2/TrdHtrk->Nhits) > 3.0) return 4;
      if( ProcessTrdHit(TrdHtrk, Trtrk, Debug) ) return 5;
    } else 
    {
      if( pev->nTrdTrack() == 0 ) return 2;
      if(!pev->pParticle(0)->pTrdTrack())   return 3;
      TrdTrackR  *Trdtrk = pev->pParticle(0)->pTrdTrack();
      if ( Trdtrk->Chi2 <= 0.0 || Trdtrk->Chi2 > 1.5) return 4;
      if( ProcessTrdHit(Trdtrk,  Trtrk, Debug) ) return 5;
    }
  
  return 0;

}
//--------------------------------------------------------------------------------------------------

int TrdSCalibR::InitTrdSCalib(int CalVer, int TrdTrackType, int Debug) {

  SCalibLevel   = CalVer;
  TrdTrackLevel = TrdTrackType;
  iFlag         = 3;

  std::cout << Form("### Initiate ... TrdCalibLevel = %d   ", SCalibLevel )
	    << Form("TrdTrackType  = %s  iFlag=%d ###", TrdTrackTypeName[TrdTrackLevel], iFlag )
	    << std::endl;
 

  if( !Init(SCalibLevel, Debug) ) {
    cerr << "Error in TrdSCalib Initialization" << endl;
    return 1;
  }
  
  return 0;
}
//--------------------------------------------------------------------------------------------------
