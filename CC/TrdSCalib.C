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
//  2011.12.27 updated by VC
//  2012.01.12 solved multi-thread problem by MM
//  2012.01.12 include alignment correction from AC Roots format
//  2012.01.13 read time dependent external alignment DB
//  2012.01.20 change TrdStrawRadius from 0.30 to 0.31 cm
//  2012.01.21 include GetTruncatedMean for charge separation
//  2012.01.25 momentum cut is applied only when calling  TrdLR_Calc, TrdLR_MC_Calc
//  2012.01.26 include hitR coord. along the wire in AC_TrdHits
//  2012.02.03 solve memory leakage in managing TrdHits for LR, Sum8 and truncated mean
//  2012.02.17 remove limited momentum ranges but PDFs are same
//  2012.02.22 released version 4
//  2012.02.23 TrTracker refit = 0 within GBATCH from P.Zucoon
//  2012.02.24 TrdAlignMethod (1:default, 2:Z.Weng 3:V.Zhukov)
//  2012.02.29 update TrdAlignMethod 2 with new TDV
//  2012.03.01 include AC_TrdGeom considering shimming
//  2012.03.09 include multiple scattering correction routine developed by Blobel 
//  2012.03.26 add M.Millinger gain calibration method
//  2012.04.26 released version 5
//  2012.05.03 update fortran minization routine to TMinuit2 by M.Millinger
//  2012.06.20 add vector TrdLPs for e,p,He probability
//  2012.06.25 fix a bug in AC_ModAlign 
//  2012.07.10 move TrdCalDBs into $AMSDataDir/v5.00/TRD/
//  2012.07.11 update PDFs (p,e,He) in rigidity bin instead of momentum bin
//--------------------------------------------------------------------------------------------------

#include "TrdSCalib.h"

#include "root.h"
#ifndef __ROOTSHAREDLIBRARY__
#include "commons.h"
#endif

#include "timeid.h"
#include "amsdbc.h"
#include "TVirtualFitter.h"

ClassImp(AC_TrdHits);
ClassImp(TrdSCalibR);

//--------------------------------------------------------------------------------------------------
TrdSCalibR* TrdSCalibR::head=0;

TrdSCalibR* TrdSCalibR::gethead()
{
  if(!head){
#pragma omp critical (trdscalibrgethead)
{

    head = new TrdSCalibR();
#ifndef __ROOTSHAREDLIBRARY__
    //== version 4
    //head->InitTrdSCalib(TRDCALIB.TrdSCalibVersion,TRDCALIB.TrdSCalibTrack,TRDCALIB.TrdSCalibDebug);
    //== version 5
    head->InitNewTrdSCalib(TRDCALIB.TrdSCalibVersion, TRDCALIB.TrdSCalibTrack, 
			      TRDCALIB.TrdSCalibGainMethod, TRDCALIB.TrdSCalibAlignMethod, 
			      TRDCALIB.TrdSCalibDebug);
#else
head->InitNewTrdSCalib(5, 0,1,1,0);
#endif    
}    
  }
  return head;
}
//--------------------------------------------------------------------------------------------------

const char *TrdSCalibR::TrdDBUpdateDir[] = 
  {
    "v5.00/TRD",//"DataBase/TrdCalNew",
    "/afs/ams.cern.ch/user/chchung/CAL"          //== for debug
  };

const char *TrdSCalibR::TrdCalDBUpdate[] =
  {
    "TrdGasCirCalibration_v02.root",              //== 0
    "TrdModuleCalibration_Calib1_v09.root",       //== 1
    /// version 3 newly generated on 2011.12.19
    "TrdCalibration_v03.root",                    //== 2
    /// version 4 on 2012.01.12                   
    "TrdScalibGain_v03.root",                     //== 3
    /// version 4 on 2012.02.01
    "TrdScalibGain_v03i.root",                    //== 4
    /// version 5 on 2012.03.01
    //"TrdScalibGain_v07.root"                    //== 5
    //"TrdScalibGain_v09.root"                      //== 5
    "TrdScalibGain_v10.root"                      //== 5
  };

const char *TrdSCalibR::TrdAlignDBUpdate[] =
  {
    /// version 4 on 2012.02.01
    "TrdScalibAlign_v03.root",                    //== 0
    /// version 5 on 2012.03.01
    //"TrdScalibAlign_v07.root"                     //== 1
    //"TrdScalibAlign_v09.root"                     //== 1
    "TrdScalibAlign_v10.root"                     //== 1
  };

const char *TrdSCalibR::TrdAlignDBExtDir[] =
  {
    /// TrdAlignMethod = 2
    "ZW",
    /// TrdAlignMethod = 3
    "VZ"
  };

const char *TrdSCalibR::TrdElectronProtonHeliumLFname[] =
  {
    /// version 1: separate LFs
    "TrdElectronLF_v08.root", "TrdProtonLF_v08.root", "TrdHeliumLF_v08.root", //== 0 - 2
    /// version 2: combined LF 
    "TrdLikelihood_v09.root",                                                 //== 3
    /// version 3 newly generated on 2011.12.19
    "TrdLikelihood_Scalib-03_SLR-10.root",                                    //== 4
    /// version 4 newly generated on 2011.12.19
    "TrdLikelihood_Scalib-04_SLR-10.root",                                    //== 5
    /// version 4 on 2012.02.01 incluing Xe pressure 
    "TrdScalibPdfs_v10i.root",                                                //== 6
    /// version 5 on 2012.03.01 extended binsize
    //"TrdScalibPdfs_v11.root",                                                 //== 7
    //"TrdScalibPdfs_v12.root",                                                 //== 7
    //"TrdScalibPdfs_v13.root"                                                 //== 7
    //"TrdScalibPdfs_v16.root"                                                 //== 7
    "TrdScalibPdfs_v21.root"                                                 //== 7
  };

const char *TrdSCalibR::TrdLLname[] =
  {
    "Likelihood(Electron/Proton)", "Likelihood(Helium/Proton)", "Likelihood(Electron/Helium)"
  };

const char *TrdSCalibR::TrdSum8name[] = {"TrdSum8Raw", "TrdSum8Corr"};

const char *TrdSCalibR::EvtPartID[]   = {"Proton", "Helium","Electron","Positron","AntiProton"};

const char *TrdSCalibR::TrdTrackTypeName[] = {"TrdSTrack", "TrdSTrack", "TrdHTrack", "TrdTrack", "TrdKTrack"};

const char *TrdSCalibR::TrdGeomUpdateDir[] =
  {
    "v5.00/TRD/Geom",//"DataBase/TrdCalNew/Geom",
    "/afs/ams.cern.ch/user/chchung/CAL/Geom" //== for debug
  };

const char *TrdSCalibR::TrdGeomDB[] =
  {
    /// Trd shimming info
    "TRD_shim.lst",
    /// Trd rotation info
    "TRD_shift_rot_default.lst"
  };

std::vector<int> TrdSCalibR::nTrdModulesPerLayer(20);
std::vector< vector<int> > TrdSCalibR::mStraw(18, vector<int>(20)); 
std::vector<int> TrdSCalibR::aP(trdconst::nTrdModules);
std::vector<int> TrdSCalibR::aA(trdconst::nTrdModules);
std::vector<double> TrdSCalibR::xTrks(7);
std::vector<double> TrdSCalibR::yTrks(7);
std::vector<double> TrdSCalibR::zTrks(7);
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------

TrdSCalibR::TrdSCalibR(): SCalibLevel(5), TrdTrackLevel(1), iFlag(3), 
			  iPabs(0), iQabs(0), iRsigned(0),iRerrinv(0), iRabs(0), iChisq(-1), 
			  TrdTkD(0), TrdTkDa(0), 
			  algo(1), patt(0), refit(0),grTrdS_Xe(0),
			  TrdGainMethod(1),TrdAlignMethod(1),_ierror(0),
			  _nOntrackhit(0), _nOfftrackhit(0),
  dummy(0) {
    FirstCall   = true;
    FirstLLCall = true;
    FirstMCCall = true;
    SetTrdAlign = true;

    SetTrdGeom  = false;
    
    h_TrdGasCirMPV.clear();
    h_TrdModuleMPV.clear();

    g_TrdCalibMPV.clear();

    FirstCalRunTime   = 2000000000;
    LastCalRunTime    = 0;

    FirstCalXday = -1;
    LastCalXday  = -1;

    FirstAlignRunTime = 2000000000;
    LastAlignRunTime  = 0;

    FirstAlignXday   = -1;
    LastAlignXday    = -1;

    FirstPdfRunTime  = 2000000000;
    LastPdfRunTime   = 0;

    FirstPdfXday     = -1;
    LastPdfXday      = -1;
   
    Lprod_Proton     = 1.0; 
    Lprod_Helium     = 1.0;
    Lprod_Electron   = 1.0;

    Lprod_ProtonMC    = 1.0;
    Lprod_HeliumMC    = 1.0;
    Lprod_ElectronMC  = 1.0;

    ExtTrdMinAdc      = 0.0;

    for(int i=0;i<3;i++) cTrd[i]=dTrd[i]=cTrk[i]=dTrk[i]=0.0;

    /// trd geometry for splice
    Tht_L1 = 0; Phi_L1 = 0;  X_L1   = 0; Y_L1   = 0;
    Tht_L9 = 0; Phi_L9 = 0;  X_L9   = 0; Y_L9   = 0;
    X_TU   = 0; Y_TU   = 0;  X_TC   = 0; Y_TC   = 0; X_TL   = 0; Y_TL   = 0;
    Tht_UT = 0; Phi_UT = 0;  X_UT   = 0; Y_UT   = 0;
    Tht_RI = 0; Phi_RI = 0;  X_RI   = 0; Y_RI   = 0;

    TG[0]=0; TG[1]=0; 
    TS[0]=0; TS[1]=0;

    /// shift & rotation in TrdGeom
    for(int i=0;i<2;TRD_SHIFT[i++]=0.);  
    TRD_SHIFT[2]  =  1000.e-4;          // global TRD Z-Shift/cm (1mm)
    for(int i=0;i<3;TRD_ROT[i++]=0.); 

    /// matching trd and tk
    fTrdSigmaDy = new TF1("fTrdSigmaDy",this, &TrdSCalibR::FunTrdSigmaDy,0.0,1000.0,3);
    fTrdSigmaDy->SetParameters(1.686,0.1505,0.2347); 
    fTrdSigmaDx = new TF1("fTrdSigmaDx",this, &TrdSCalibR::FunTrdSigmaDy,0.0,1000.0,3);
    fTrdSigmaDx->SetParameters(2.484,0.1183,0.3487);
    fTrd95Da = new TF1("fTrd95Da",this, &TrdSCalibR::FunTrdSigmaDy,0.0,1000.0,3);  
    fTrd95Da->SetParameters(0.7729,0.7324,0.2005);

 
    TrdLR_Gr_Prot.clear();
    TrdLR_Gr_Elec.clear();
    TrdLR_Gr_Heli.clear();

    TrdPDF_xProt.clear();
    TrdPDF_nProt.clear();
    TrdPDF_xHeli.clear();
    TrdPDF_nHeli.clear();
    TrdPDF_nElec_Xe0.clear();
    TrdPDF_nElec_Xe1.clear();
    TrdPDF_nElec_Xe2.clear();
    TrdPDF_nElec_Xe3.clear();
    TrdPDF_nElec_Xe4.clear();
    TrdPDF_nElec_Xe5.clear();

    for(int i=0;i<nBinfPXe;i++) grTrdS_PDF_Prot[i].clear();
    for(int i=0;i<nBinfPXe;i++) grTrdS_PDF_Heli[i].clear();
    for(int i=0;i<nBinfPXe;i++) grTrdS_PDF_Elec[i].clear();
    grTrdS_PDF_Elec_Xe0.clear();
    grTrdS_PDF_Elec_Xe1.clear();
    grTrdS_PDF_Elec_Xe2.clear();
    grTrdS_PDF_Elec_Xe3.clear();
    grTrdS_PDF_Elec_Xe4.clear();
    grTrdS_PDF_Elec_Xe5.clear();
    
  
  }
  
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------

void AC_TrdHits::PrintTrdHit(Option_t *) const{
  std::cout << Form("Lad=%5d Lay=%5d Tub=%5d Mod=%5d Straw=%d GC=%5d ",
	       Lad, Lay, Tub, Mod, Straw, GC);
  std::cout << std::endl;

  std::cout << Form("EadcR=%5.1f, EadcC1=%5.1f, EadcCS=%5.1f,",
		    EadcR, EadcC1, EadcCS);
  
  std::cout << Form("TrdD=%5.1f, TrkD=%5.1f, Len2D=%5.1f, Len3D=%5.1f",
		    TrdD, TrkD, Len2D, Len3D);

  std::cout << std::endl;

};
//--------------------------------------------------------------------------------------------------
/// Straw 0 - 5247
int AC_TrdHits::GetTrdStrawNumber(TrdRawHitR *rhit) {
  int module  = GetTrdModule(rhit->Layer, rhit->Ladder);
  
  if(module >= 0 && module < nTrdModules)
    return module*16 + rhit->Tube;
  else return -1;  
}

int AC_TrdHits::GetTrdStrawNumber() {
  int module  = GetTrdModule(Lay, Lad);
  
  if(module >= 0 && module < nTrdModules)
    return module*16 + Tub;
  else return -1;
}

//--------------------------------------------------------------------------------------------------
/// Module 0 -327
int AC_TrdHits::GetTrdModule(int layer, int ladder) {     
 
  int module 	= -1;
  
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
  
  int module 	= -1;
  
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
  int Lad = -1;
  int Lay = -1;
  TrdStraw2LayLad(Straw, Lad, Lay);
  return Lay;
}

//--------------------------------------------------------------------------------------------------
/// Straw 0 - 5247 => Lad 0 - 17
int AC_TrdHits::TrdStraw2Ladder(int Straw) { 
  int Lad = -1;
  int Lay = -1;
  TrdStraw2LayLad(Straw, Lad, Lay);
  return Lad;
}

//--------------------------------------------------------------------------------------------------
/// TRD Gas Group from 0-40
int AC_TrdHits::GetTrdGasGroup(int layer, int ladder) {
  int GasGroup = -1;
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
  int GasGroup = -1;
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
  double Distance = -1.0;
 
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
double AC_TrdHits::GetTrdHitTrkDistance(AC_TrdHits *AC, AMSPoint cPtrk, AMSDir dPtrk) {	
  double Distance = -1.0;

  int d    = AC->hitD; 
  float xy = AC->hitXY;
  float z  = AC->hitZ;

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
  //std::cout << Form("len3d=%+6.3f ", len3d);

  if (len3d<=0.0) return -1.0;
  
  return len3d;
}

double AC_TrdHits::GetTrdPathLen3D(AC_TrdHits *AC, AMSPoint cP, AMSDir dP) {

  int lay  = AC->Lay;
  float xy = AC->hitXY;
  float z  = AC->hitZ;

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
  //std::cout << Form("len3d=%+6.3f ", len3d);

  if (len3d<=0.0) return -1.0;
  
  return len3d;
}

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

bool TrdSCalibR::AC_InitTrdGeom(int Debug){
  char tdname[200];
  sprintf(tdname, "%s/%s/%s", pPath, TrdGeomUpdateDir[0], TrdGeomDB[0]);
  if( AC_InitTrdShim( tdname, Debug ) ) return false;
    
  sprintf(tdname, "%s/%s/%s", pPath, TrdGeomUpdateDir[0], TrdGeomDB[1]);
  if( AC_InitTrdMove( tdname, Debug ) ) return false;

  std::cout << Form("TrdSCalibR::Init_TrdGeom-I- Successfully Loaded") << std::endl;
  return true;    
}
  

int TrdSCalibR::AC_InitTrdShim( char* fname, int Debug ){

  ifstream fp;
  fp.open(fname);
  if (! fp.good() ) return 1;

  int Imod=0;
  float Imod_Dz=0, Imod_Arz=0;
 
  while (fp.good()) {
    fp >> Imod >> Imod_Dz  >> Imod_Arz;
    Mod_Dz[ Imod-1] = Imod_Dz;
    Mod_Arz[Imod-1] = Imod_Arz;
  }
  fp.close();
  
  return 0;
}

int TrdSCalibR::AC_InitTrdMove( char* fname, int Debug ){
  
  ifstream fp;
  fp.open(fname);
  if (! fp.good() ) return 1;

  int Imod = 0;
  float Imod_Dz=0, Imod_Arz=0;

  while (fp.good()) {
    fp >> TRD_SHIFT[0] >> TRD_SHIFT[1] >> TRD_SHIFT[2]
       >> TRD_ROT[0]   >> TRD_ROT[1]   >> TRD_ROT[2]; 
  }
  fp.close();
  
  
  if(Debug) {
    std::cout << "TrdSCalibR::Init_TrdMove-I- Successfully Done" << std::endl;
    std::cout << Form("TRD_Sxyz: %+8.2f %+8.2f %+8.2f ", 
		      1.e4*TRD_SHIFT[0], 1.e4*TRD_SHIFT[1], 1.e4*TRD_SHIFT[2]);
    std::cout << Form("TRD_Rxyz: %+8.2f %+8.2f %+8.2f ", 
		      1.e6*TRD_ROT[0],   1.e6*TRD_ROT[1],    1.e6*TRD_ROT[2])  
	      << std::endl;
  }
  
  return 0;
}


void TrdSCalibR::AC_InitInterpolate2Z(float dZ_UT, float dZ_L1, float dY_UT, float dY_L1){

  //std::cout << Form("UT=(%+8.3f %+8.3f) TrdC(%+8.3f %+8.3f)", 
  //		    X_UT, Y_UT, X_TC, Y_TC) << endl;
  
  TG[0]  = new TGraph(7);
  TG[1]  = new TGraph(7);

  float M;
  
  /// XZ Interpolation:
  M = tan(Tht_UT)*cos(Phi_UT);
  TG[0]->SetPoint( 0, trdconst::Zutof-dZ_UT, X_UT-M*dZ_UT );
  TG[0]->SetPoint( 1, trdconst::Zutof,       X_UT         );
  
  TG[0]->SetPoint( 2, trdconst::ZtrdL,       X_TL         );
  TG[0]->SetPoint( 3, trdconst::ZtrdC,       X_TC         );
  TG[0]->SetPoint( 4, trdconst::ZtrdU,       X_TU         );
  
  M = tan(Tht_L1)*cos(Phi_L1);
  TG[0]->SetPoint( 5, trdconst::Z_l1,        X_L1         );
  TG[0]->SetPoint( 6, trdconst::Z_l1+dZ_L1,  X_L1+M*dZ_L1 );
  
  
  if(TS[0]){
    Double_t x,z;
    for(Int_t i=0;i<TG[0]->GetN();i++){
      TG[0]->GetPoint(i, z, x);
      TS[0]->SetPoint(i, z, x);
    }
  }else{
    TS[0]  = new TSpline3("TSx",TG[0]);
  }

  /// YZ Interpolation:
  M = tan(Tht_UT)*sin(Phi_UT);
  TG[1]->SetPoint( 0, trdconst::Zutof-dZ_UT, Y_UT-M*dZ_UT+M*dZ_UT*dY_UT/1000./iRabs*sqrt(1.+M*M) );
  TG[1]->SetPoint( 1, trdconst::Zutof,       Y_UT         );
  
  TG[1]->SetPoint( 2, trdconst::ZtrdL,       Y_TL         );
  TG[1]->SetPoint( 3, trdconst::ZtrdC,       Y_TC         );
  TG[1]->SetPoint( 4, trdconst::ZtrdU,       Y_TU         );
  
  M = tan(Tht_L1)*sin(Phi_L1);
  TG[1]->SetPoint( 5, trdconst::Z_l1,        Y_L1         );
  TG[1]->SetPoint( 6, trdconst::Z_l1+dZ_L1,  Y_L1+M*dZ_L1-M*dZ_L1*dY_L1/1000./iRabs*sqrt(1.+M*M) );
  

  if(TS[1]){
    Double_t y,z;
    for(Int_t i=0;i<TG[1]->GetN();i++){
      TG[1]->GetPoint(i, z, y);
      TS[1]->SetPoint(i, z, y);
    }
  }else{
    TS[1]  = new TSpline3("TSy",TG[1]);
  }
  
  return;
  
}

float TrdSCalibR::AC_Interpolate2Z(float Z, int D){ 
  if(TMath::IsNaN(TS[D]->Eval((Double_t)Z))) return 99999;
    return TS[D]->Eval((Double_t)Z);
}

void TrdSCalibR::AC_ClearInterpolate2Z(){   
  if(TG[0]){ delete TG[0]; TG[0] = 0; }
  if(TG[1]){ delete TG[1]; TG[1] = 0; }
  if(TS[0]){ delete TS[0]; TS[0] = 0; }
  if(TS[1]){ delete TS[1]; TS[1] = 0; }
}
    
int TrdSCalibR::AC_ModAlign(AC_TrdHits* &ACHit, int Debug){

  int Mod = ACHit->Mod;
  int D   = ACHit->hitD;
  
  float XYZ[3] = {0};

  /// known position
  if( D==1 ) XYZ[1] = ACHit->hitXY;
  else XYZ[0] = ACHit->hitXY;
  XYZ[2] = ACHit->hitZ;
 
  /// along the wire position
  //if( fabs(AC_Interpolate2Z(XYZ[2], 1-D))>(Z_l1+1.)) return 1; 
  XYZ[1-D] = AC_Interpolate2Z(XYZ[2], 1-D);
  
  Float_t Dx=0,Dy=0,Dz=0;
  XYZ[2] += Mod_Dz[ Mod-1]/1.e4;      // module Dz/mu-m     in shimmed octagon
  XYZ[2] += XYZ[1-D]*Mod_Arz[Mod-1]/1.e6;      // module Tilt/mu-rad along wire

  /// Global TRD Shifts:
  XYZ[0] += TRD_SHIFT[0];  // X
  XYZ[1] += TRD_SHIFT[1];  // Y
  XYZ[2] += TRD_SHIFT[2];  // Z

  /// Global TRD Rotations:
  /// XY corrections
  if(D==0){ //== XZ Measurement
    Dx = -(XYZ[2]-trdconst::ZtrdC)*sin(TRD_ROT[1])  // TRD Y-Rotation (XZ-Tilt)
      -XYZ[1]                 *sin(TRD_ROT[2]);  // TRD Z-Rotation
    Dy = 0.0;
  }else{    //== YZ Measurement

    Dx = 0.0;
    Dy = -(XYZ[2]-trdconst::ZtrdC)*sin(TRD_ROT[0])  // TRD X-Rotation (YZ-Tilt)
      +XYZ[0]                *sin(TRD_ROT[2]);  // TRD Z-Rotation
    
  }
  
  /// Z corrections
  Dz =     XYZ[0]      *sin(TRD_ROT[1])  // TRD Y-Rotation (XZ-Tilt)
    +XYZ[1]      *sin(TRD_ROT[0]);  // TRD X-Rotation (YZ-Tilt)
  
  XYZ[0] += Dx;
  XYZ[1] += Dy;
  XYZ[2] += Dz;
 
  // set also RLZ variables:
  ACHit->hitXY = XYZ[D];
  ACHit->hitR  = XYZ[1-D];
  ACHit->hitZ  = XYZ[2];

  if(Debug > 1) {
    if(D==1) std::cout << Form("==> RawHit=(%+8.3f %+8.3f) ", XYZ[1-D], ACHit->hitXYraw);
    else  std::cout << Form("==> RawHit=(%+8.3f %+8.3f) ", ACHit->hitXYraw, XYZ[1-D]);
    std::cout << Form(" ==> hitXY=%+8.3f hitR=%+8.3f hitZ=%+8.3f ", ACHit->hitXY, ACHit->hitR, ACHit->hitZ) << std::endl;
  }

  if( fabs(ACHit->hitXYraw - ACHit->hitXY) > 0.2 ) return 1;
  if( fabs(ACHit->hitZraw  - ACHit->hitZ)  > 0.2 ) return 2; 

  return 0;
}














//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

void TrdSCalibR::GenmStrawMatrix(int Debug) {
  std::cout << "TrdSCalibR::GenmStrawMatrix-I- Loading ..." << std::endl;
  
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

///(Z= upper TOF)
bool TrdSCalibR::GetcTrd(TrdHTrackR *trdht) {
  if(! trdht) return false; 
  c0Trd[0]=trdht->Coo[0]; c0Trd[1]=trdht->Coo[1]; c0Trd[2]=trdht->Coo[2];
   float cpx = 0; 
   float cpy = 0;
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
  if(fitcode < 0) {
    std::cout << Form("TrdSCalibR::ProcessTrdEvt-W-  TrackFit algo %d and patt %d Not AVAILABLE Fitcode=%d !!!!",
		      algo,patt,fitcode) << std::endl;
    return false;
  }
  trt->Interpolate(trdconst::ToFLayer1Z,cTrk,dTrk, fitcode);
  return true;
}
//--------------------------------------------------------------------------------------------------
bool TrdSCalibR::MatchingTrdTKtrack(float Rabs, int Debug){
  TrdTkD  = TMath::Sqrt(TMath::Power(cTrk.x()-cTrd.x(),2)+
			   TMath::Power(cTrk.y()-cTrd.y(),2)+
			   TMath::Power(cTrk.z()-cTrd.z(),2));

  double TrdTkDx = cTrk.x() - cTrd.x();// + trdconst::TrdOffsetDx;
  double TrdTkDy = cTrk.y() - cTrd.y();// + trdconst::TrdOffsetDy;
  double SigmaTrdTrackerX = fTrdSigmaDx->Eval(Rabs);
  double SigmaTrdTrackerY = fTrdSigmaDy->Eval(Rabs);
  double TrdTkDr 	  = TMath::Sqrt( TMath::Power(TrdTkDx/SigmaTrdTrackerX,2) + 
					 TMath::Power(TrdTkDy/SigmaTrdTrackerY,2) );

  double CutTrdTkDa       = 10.0*fTrd95Da->Eval(Rabs);
  TrdTkDa = fabs(dTrk.x()*(-1*dTrd.x())+dTrk.y()*(-1*dTrd.y())+dTrk.z()*(-1*dTrd.z()));
  TrdTkDa = TMath::ACos(TrdTkDa)*180/TMath::Pi();

  if(Debug > 1)
    std::cout << Form("MatchingTrdTk: D=%.2f Dr =%.2f CutDa=%.2f Da= %.2f ", 
		      TrdTkD, TrdTkDr, CutTrdTkDa, TrdTkDa) << std::endl;

  if(TrdTkDr > 4.) return false;
  if(TrdTkD  > 4. || TrdTkDa>CutTrdTkDa ) return false;
  
  if(Debug) 
    std::cout << Form("++++ Not:  iRabs=%8.2f TrdTkD=%5.2f  TrdTkDr=%5.2f TrdTkDa=%5.2f wrt CutTrdTkDa=%5.2f",
		      iRabs, TrdTkD, TrdTkDr, TrdTkDa, CutTrdTkDa) << std::endl;
		      
  return true;
}
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
int TrdSCalibR::GetEvthTime(AMSEventR *ev, int Debug){
  int hTime = 0;
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
  int hTime = 0;
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
double TrdSCalibR::GetEvtxTime(AMSEventR *ev, int Debug){
  double xTime = 0;
  time_t  EvtTime = ev->fHeader.Time[0];
  struct tm*  TimeInfo = gmtime ( &EvtTime );
  int 	Year 	= TimeInfo->tm_year;
  int 	Day 	= TimeInfo->tm_yday;
  
  // AMS Data Taking started on 19. May 2011
  for (int iYear=111; iYear<Year; iYear++) { 
    Day += 365;
    // February has 29 days
    if (iYear%4 == 0 && !(iYear%100 == 0 && iYear%400 != 0)) Day += 1;
  }

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
double TrdSCalibR::GetEvtxTime(time_t EvtTime, int Debug){
  double xTime = 0;
  struct tm*  TimeInfo = gmtime ( &EvtTime );
  int 	Year 	= TimeInfo->tm_year;
  int 	Day 	= TimeInfo->tm_yday;
  
  // AMS Data Taking started on 19. May 2011
  for (int iYear=111; iYear<Year; iYear++) { 
    Day += 365;
    // February has 29 days
    if (iYear%4 == 0 && !(iYear%100 == 0 && iYear%400 != 0)) Day += 1;
  }
 
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
    return 1;
  }
  
  TH1D *h_Nevts = (TH1D*) input->Get(hname); /// center bottom module
  if(h_Nevts->GetNbinsX() == 0) return 2;

  int nBin = h_Nevts->GetNbinsX();
  for (int iBin=1; iBin<=nBin; iBin++) 
    {
      unsigned int iBCnt  = (unsigned int) h_Nevts->GetBinContent(iBin);
      float        iTime  = h_Nevts->GetBinCenter(iBin);
      if(iBCnt == 0) continue;
      
      if(Debug > 1) 
	std::cout << Form("GetCalibTimePeriod iBin=%4u iTime=%5.2f iBCnt=%10u", iBin, iTime, iBCnt) << std::endl;
      
      if (iTime < FirstCalRunTime)  FirstCalRunTime = (unsigned int) iTime;
      if (iTime > LastCalRunTime )  LastCalRunTime  = (unsigned int) iTime; 
    }	
  input->Close();
  delete input;

  return 0;
}


//--------------------------------------------------------------------------------------------------

bool TrdSCalibR::Get01TrdCalibration(TString fname, int Debug) {

  //  h_TrdGasCirMPV = new vector<TH1F*>();

  for (vector<TH1F*>::iterator iter = h_TrdGasCirMPV.begin(); iter != h_TrdGasCirMPV.end(); ++iter) 
    if(*iter) delete *iter;
  h_TrdGasCirMPV.clear();

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

      if(Debug > 0)
	std::cout << Form("%s [%02d] M=%d", hName, iTrdGasCir, (int) pgDum->GetMean()) 
		  << std::endl;
      if (!pgDum) nLost ++;

    }

  input->Close();
  delete input;

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

  for (vector<TH1F*>::iterator iter = h_TrdModuleMPV.begin(); iter != h_TrdModuleMPV.end(); ++iter) 
    if(*iter) delete *iter;
  h_TrdModuleMPV.clear();

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

bool TrdSCalibR::Get03TrdCalibration(TString fname, int Debug) {
	
  for (vector<TGraphErrors*>::iterator iter = g_TrdCalibMPV.begin(); iter != g_TrdCalibMPV.end(); ++iter) 
    if(*iter) delete *iter;
  g_TrdCalibMPV.clear();

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

  bool ok_gain=false, ok_pdf=false;

  /// generate mStraw-Matrix
  GenmStrawMatrix(Debug);

  SetTrdGeom = AC_InitTrdGeom(Debug);
  if(!SetTrdGeom)  Warning("TrdSCalibR::Init-W- ", "Failed to Load TrdGeom");
  
  /// get TRD calibration histos / each gas circuit and each module level (v2)
  if(CalibLevel < 3)      ok_gain = GetTrdCalibHistos(CalibLevel, Debug);
  
  /// get TRD calibration histos / each module level (v3)
  else if(CalibLevel < 4) ok_gain = GetTrdV3CalibHistos(CalibLevel, Debug);

  /// get TRD calibration graphs / each module level (v4)
  else if(CalibLevel < 5) ok_gain = GetTrdV4CalibHistos(CalibLevel, Debug);

  /// get TRD calibration graphs / each module level (v5)
  else if(CalibLevel < 6) ok_gain = GetTrdV5CalibHistos(Debug);

  else return false;
  if(!ok_gain) Error("TrdSCalibR::Init", "Fail Trd Gain/Align Calibration");


  /// initiate TRD loglikelihood calculator
  //TrdLR_CalcIni(Debug);   
  ok_pdf = TrdLR_CalcIniPDF(Debug);  //==2012.02.02
  if(!ok_pdf) {Error("TrdSCalibR::Init", "Fail Loading Trd PDFs"); return false;}

  /// initiate ToyMC loglikelihood calculator
  if( !TrdLR_MC_CalcIniXe(Debug) ) return false;

  std::cout << Form("TrdSCalibR::Init-I- FirstCall=%s FirstLLCall=%s FirstMCCall=%s", 
		    FirstCall?"true":"false", 
		    FirstLLCall?"true":"false",
		    FirstMCCall?"true":"false" ) << std::endl;

  std::cout <<Form("TrdSCalibR::Init-I- TrdGainMethod_1=%s TrdGainMethod_2=%s TrdGainMethod_3=%s",
		   TrdGain_01?"true":"false",
		   TrdGain_02?"true":"false",
		   TrdGain_03?"true":"false") <<std::endl; 

  std::cout <<Form("TrdSCalibR::Init-I- SetTrdAlign=%s TrdAlignMethod_1=%s TrdAlignMethod_2=%s TrdAlignMethod_3=%s",
		   SetTrdAlign?"true":"false",
		   TrdAlign_01?"true":"false",
		   TrdAlign_02?"true":"false",
		   TrdAlign_03?"true":"false") <<std::endl; 

  /// check each initiate procedure
  if(FirstCall || FirstLLCall || FirstMCCall)
    return false;
  else return true;
}
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

bool TrdSCalibR::GetTrdCalibHistos(int CalibLevel, int Debug) {
  
  TrdCalib_01 = false;  TrdCalib_02 = false;
  /// Get TRD Calibration Histograms
 
  if(Debug) 
    std::cout << "Get TRD Calibration Histograms: CalibLevel=" << CalibLevel << std::endl;
  cout << "----------------------------------------" << endl;
  char tdname[200]; 
  if (CalibLevel>0) 
    {
      sprintf(tdname, "%s/%s/%s", pPath, TrdDBUpdateDir[0], TrdCalDBUpdate[0]);
      TrdCalib_01 = Get01TrdCalibration(tdname, Debug);
    }
  if (CalibLevel>1) 
    {
      sprintf(tdname, "%s/%s/%s", pPath, TrdDBUpdateDir[0], TrdCalDBUpdate[1]);
      TrdCalib_02 = Get02TrdCalibration(tdname, Debug);
      
      /// check module calibration time period
      if( GetModCalibTimePeriod(tdname, "h_ModMPV_26", Debug) ) return false;
      else 
	std::cout << Form("### ModCalibFirsthTime=%6u (hour) ModCalibLasthTime=%6u (hour)",
			  FirstCalRunTime, LastCalRunTime) << std::endl;
    }
  
  if(Debug) 
    std::cout <<Form("TrdCalib1=%s TrdCalib2=%s",
		     TrdCalib_01?"true":"false",
		     TrdCalib_01?"true":"false") <<std::endl;
  FirstCall = false;
  
  if (  (TrdCalib_01 && CalibLevel>0) || (TrdCalib_02 && CalibLevel>1) ) 
    return true;
  else return false;
    
}
//-------------------------------------------------------------------------------------------------
bool TrdSCalibR::GetTrdV3CalibHistos(int CalibLevel, int Debug) {
  
  TrdCalib_01 = false;  TrdCalib_02 = false; TrdCalib_03 = false;
  /// Get TRD Calibration Histograms
  
  if(Debug) 
    std::cout << "Get TRD Calibration Histograms: CalibLevel=" << CalibLevel << std::endl;
  cout << "----------------------------------------" << endl;
  char tdname[200]; 
  
  if (CalibLevel>2) 
    {
      sprintf(tdname, "%s/%s/%s", pPath, TrdDBUpdateDir[0], TrdCalDBUpdate[2]);
      TrdCalib_03 = Get03TrdCalibration(tdname, Debug);
      
      /// check module calibration time period
      if ( GetModCalibTimePeriod(tdname, "hModMpv_0_26", Debug) ) return false;
      else 
	std::cout << Form("### ModCalibFirsthTime=%6u (day) ModCalibLasthTime=%6u (day)",
			  FirstCalRunTime, LastCalRunTime) << std::endl;
    }
  
  
  if(Debug) 
    std::cout <<Form("TrdCalib1=%s TrdCalib2=%s TrdCalib3=%s",
		     TrdCalib_01?"true":"false",
		     TrdCalib_02?"true":"false",
		     TrdCalib_03?"true":"false") <<std::endl;
  FirstCall = false;
  

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
int TrdSCalibR::GetTrdSum8(int Debug) {

  //vector<float> TrdSum8;
  /// consider only two subset from raw and cs values
  TrdSum8Amp.assign(2, -1.);
  
  if (TrdSHits.size() >= trdconst::nTrdMinHits) {
    vector<float> TrdEadcR, TrdEadcCS; 

    vector<AC_TrdHits*>::iterator iter;
    for (iter = TrdSHits.begin(); iter != TrdSHits.end(); ++iter) {
      TrdEadcR.push_back((*iter)->EadcR);
      TrdEadcCS.push_back((*iter)->EadcCS);
    }
    
    if(Debug > 0) {
      std::cout << "TrdSum8::TrdEadcR contains:";
      for(vector<float>::iterator it = TrdEadcR.begin(); it != TrdEadcR.end(); ++it )
	std::cout << " " << *it;
      std::cout << std::endl;
    }
    
    /// before calib.
    sort(TrdEadcR.begin(),TrdEadcR.begin()+ TrdEadcR.size());
    for (unsigned int i=TrdEadcR.size()-8; i<TrdEadcR.size(); i++) 
      TrdSum8Amp.at(0) += TrdEadcR.at(i);
    
    /// after calib.
    sort(TrdEadcCS.begin(),TrdEadcCS.begin()+ TrdEadcCS.size());
    for (unsigned int i=TrdEadcCS.size()-8; i<TrdEadcCS.size(); i++) 
      TrdSum8Amp.at(1) += TrdEadcCS.at(i);

    
    if(Debug) {
      int i = 0;
      for(vector<float>::iterator is = TrdSum8Amp.begin(); is != TrdSum8Amp.end(); ++is )
	std::cout << Form("%s= %6.2f  ",TrdSum8name[i++], *is);
      std::cout << std::endl;
    }
    
    return 0;
    
  } else return 1;
  
}
//--------------------------------------------------------------------------------------------------
int TrdSCalibR::GetTruncatedMean(int Debug) {

  //vector<float> TruncatedMean;
  /// consider only two subset from raw and cs values 
  TruncatedMean.assign(2,0.0);
  
  if (TrdSHits.size() >= trdconst::nTrdMinHits) {
    vector<float> TrdEadcR, TrdEadcCS; 

    vector<AC_TrdHits*>::iterator iter;
    for (iter = TrdSHits.begin(); iter != TrdSHits.end(); ++iter) {
      TrdEadcR.push_back((*iter)->EadcR);
      TrdEadcCS.push_back((*iter)->EadcCS);
    }
    
    if(Debug > 0) {
      std::cout << "TruncdMean::TrdEadcR contains:";
      for(vector<float>::iterator it = TrdEadcR.begin(); it != TrdEadcR.end(); ++it )
	std::cout << " " << *it;
      std::cout << std::endl;
    }
    
    /// before calib.
    sort(TrdEadcR.begin(),TrdEadcR.end());    
    if(TrdEadcR.size()%2)
      for(unsigned int i=TrdEadcR.size()/2-2; i < TrdEadcR.size()/2 + 3; i++)
	TruncatedMean.at(0) += TrdEadcR.at(i)/5;
    else 
      for(unsigned int i=TrdEadcR.size()/2-2; i < TrdEadcR.size()/2  + 2; i++)
	TruncatedMean.at(0) += TrdEadcR.at(i)/4;
    TruncatedMean.at(0) = TMath::Sqrt(TruncatedMean.at(0));
    
    /// after calib.
    sort(TrdEadcCS.begin(),TrdEadcCS.end());
    if(TrdEadcCS.size()%2)
      for(unsigned int i=TrdEadcCS.size()/2-2; i < TrdEadcCS.size()/2 + 3; i++)
	TruncatedMean.at(1) += TrdEadcCS.at(i)/5;
    else 
      for(unsigned int i=TrdEadcCS.size()/2-2; i < TrdEadcCS.size()/2 + 2; i++)
	TruncatedMean.at(1) += TrdEadcCS.at(i)/4;
    TruncatedMean.at(1) = TMath::Sqrt(TruncatedMean.at(1));

    if(Debug) {
      int i = 0;
      for(vector<float>::iterator is = TruncatedMean.begin(); is != TruncatedMean.end(); ++is )
	std::cout << Form("TruncatedMean[%d]=%6.2f  ",i++, *is);
      std::cout << std::endl;
    }

    return 0;
   
  } else return 1;
   
 
} 

//--------------------------------------------------------------------------------------------------
int TrdSCalibR::GetTrdMedian(int Debug) {

  /// median for raw and cs values 
  TrdMedian.assign(2, -1.);
  
  if (TrdSHits.size() >= trdconst::nTrdMinHits) {
    vector<float> TrdEadcR, TrdEadcCS;

    vector<AC_TrdHits*>::iterator iter;
    for (iter = TrdSHits.begin(); iter != TrdSHits.end(); ++iter) {
      TrdEadcR.push_back((*iter)->EadcR);
      TrdEadcCS.push_back((*iter)->EadcCS);
    }
    
    if(Debug > 0) {
      std::cout << "TrdMedian::TrdEadcR contains:";
      for(vector<float>::iterator it = TrdEadcR.begin(); it != TrdEadcR.end(); ++it )
	std::cout << " " << *it;
      std::cout << std::endl;
    }
    
    /// before calib.
    sort(TrdEadcR.begin(),TrdEadcR.end());
    unsigned int im = (unsigned int) TrdEadcR.size()/2;
    if(TrdEadcR.size()%2)
      TrdMedian.at(0) =  TrdEadcR.at(im)/2;
    else 
      TrdMedian.at(0) = (TrdEadcR.at(im)+TrdEadcR.at(im-1))/2;
    
    /// after calib.
    sort(TrdEadcCS.begin(),TrdEadcCS.end());
    unsigned int ic = (unsigned int) TrdEadcCS.size()/2;
    if(TrdEadcCS.size()%2)
      TrdMedian.at(1) =  TrdEadcCS.at(ic)/2;
    else 
      TrdMedian.at(1) = (TrdEadcCS.at(ic)+TrdEadcCS.at(ic-1))/2;

      
    if(Debug) {
      int i = 0;
      for(vector<float>::iterator is = TrdMedian.begin(); is != TrdMedian.end(); ++is )
	std::cout << Form("TrdMedian[%d]=%6.2f  ",i++, *is);
      std::cout << std::endl;
    }

    return 0;
    
  } else return 1; 
  
} 
//--------------------------------------------------------------------------------------------------
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
//--------------------------------------------------------------------------------------------------
bool TrdSCalibR::TrdLR_CalcIniPDF(int Debug) {
  
  std::string hname;
  char aname[100];
  char fname[100];
  char grName[100];

  /// version 2
  if(SCalibLevel < 3)
    sprintf(fname, "%s/%s/%s", pPath, TrdDBUpdateDir[0],TrdElectronProtonHeliumLFname[3]);
  
  /// version 3
  else if(SCalibLevel < 4) 
    sprintf(fname, "%s/%s/%s", pPath, TrdDBUpdateDir[0],TrdElectronProtonHeliumLFname[4]);
  
  /// version 4
  else if(SCalibLevel < 5)  
    sprintf(fname, "%s/%s/%s", pPath, TrdDBUpdateDir[0],TrdElectronProtonHeliumLFname[6]); 
  
  else 
    sprintf(fname, "%s/%s/%s", pPath, TrdDBUpdateDir[0],TrdElectronProtonHeliumLFname[7]); 
  
  TFile *input = new TFile(fname);
  if (!input->IsOpen()) {
    Error("TrdSCalibR::TrdLR_CalcIniPDF-E- ", "Open Trd Likelihood file .. %s", fname);
    return false;
  }    
  std::cout << "TrdSCalibR::TrdLR_CalcIniPDF-I- Trd LiklihoodFunctions read in from " << fname << std::endl;
 

  // true if the normalisation of the PDF's should be checked
  bool CheckNormPDF = false;

  /// clean vectors
  for (vector<TGraph*>::iterator iter = TrdLR_Gr_Prot.begin(); iter != TrdLR_Gr_Prot.end(); ++iter) 
    if(*iter) delete *iter;

  for (vector<TGraph*>::iterator iter = TrdLR_Gr_Elec.begin(); iter != TrdLR_Gr_Elec.end(); ++iter) 
    if(*iter) delete *iter;

  for (vector<TGraph*>::iterator iter = TrdLR_Gr_Heli.begin(); iter != TrdLR_Gr_Heli.end(); ++iter) 
    if(*iter) delete *iter;

  // read in calibration files
  vector<double> Runs;
  TGraph* grTrdSLR_Runs = (TGraph*) input->Get("grTrdSLR_Runs");
  if (!grTrdSLR_Runs) {
    Error("TrdSCalibR::TrdLR_CalcIniPDF-E- ", "Error in TrdScalibIniPDF: TGraph grTrdSLR_Runs not found !");
    return false;
  }

  RootTGraph2VectorY(grTrdSLR_Runs,Runs);
  int nCalibRuns        = Runs.size();
  FirstPdfRunTime 	= TMath::Nint(Runs.at(0));
  LastPdfRunTime 	= TMath::Nint(Runs.at(nCalibRuns-1));
  FirstPdfXday 	        = GetEvtxTime((time_t) FirstPdfRunTime, Debug);
  LastPdfXday 	        = GetEvtxTime((time_t) LastPdfRunTime, Debug);

  //if(Debug)
  std::cout << Form("TrdSCalibR::TrdLR_CalcIniPDF-I- Trd PDFs Runs=%6d Interval %5d - %5d <=> %7.4f - %7.4f days ###", 
		      nCalibRuns, FirstPdfRunTime, LastPdfRunTime, FirstPdfXday, LastPdfXday) << std::endl; 

  if(FirstPdfXday < 0 || LastPdfXday < 0) return false;
 
  /// Read Protons PDF's  
  TH1D *h_Proton = (TH1D*)input->Get("h_Proton");
  if (!h_Proton) {
    std::cout << "TrdSCalibR::TrdLR_CalcIniPDF-I- Failed to get Proton PDFs Xbins!" << endl;
    return false;
  }
  TrdS_PDF_xProt = RootGetXbinsTH1(h_Proton);
  delete h_Proton; h_Proton = 0;
  std::cout << "TrdSCalibR::TrdLR_CalcIniPDF-I- NBins of Proton PDFs = " << TrdS_PDF_xProt.size() << std::endl;

  std::cout << "TrdSCalibR::TrdLR_CalcIniPDF-I- Read in Proton PDFs w/ Xe partial pressures" << std::endl;
  for (int iXe=0; iXe<nBinfPXe; iXe++) {
    TrdS_PDF_nProt[iXe].assign(TrdS_PDF_xProt.size()-1,1.0);
    for (unsigned int iV=0; iV<TrdS_PDF_xProt.size()-1; iV++) {
      sprintf(grName,"grTrdS_Prot_%d_%d",iXe,iV);
      grTrdS_PDF_Prot[iXe].push_back((TGraph*)input->Get(grName));
      if (grTrdS_PDF_Prot[iXe].at(iV)==NULL) {
	std::cout << Form("TrdSCalibR::TrdLR_CalcIniPDF-I- Failed to find %s", grName) << std::endl;
	return false;
      }
    }
  }
  std::cout << "TrdSCalibR::TrdLR_CalcIniPDF-I- Read in Proton PDFs: Done !" << std::endl;


  /// Read Helium PDF's
  TH1D *h_Helium = (TH1D*)input->Get("h_Helium");
  if (!h_Helium) {
    std::cout << "TrdSCalibR::TrdLR_CalcIniPDF-I- Failed to get Helium PDFs Xbins!" << endl;
    return false;
  }
  TrdS_PDF_xHeli = RootGetXbinsTH1(h_Helium);
  delete h_Helium; h_Helium = 0;
  std::cout << "TrdSCalibR::TrdLR_CalcIniPDF-I- NBins of Helium PDFs = " << TrdS_PDF_xHeli.size() << std::endl;

  std::cout << "TrdSCalibR::TrdLR_CalcIniPDF-I- Read in Helium PDFs w/ Xe partial pressure" << std::endl;
  for (int iXe=0; iXe<nBinfPXe; iXe++) {
    TrdS_PDF_nHeli[iXe].assign(TrdS_PDF_xHeli.size()-1,1.0);
    for (unsigned int iV=0; iV<TrdS_PDF_xHeli.size()-1; iV++) {
      sprintf(grName,"grTrdS_Heli_%d_%d",iXe,iV);
      grTrdS_PDF_Heli[iXe].push_back((TGraph*)input->Get(grName));
      if (grTrdS_PDF_Heli[iXe].at(iV)==NULL) {
	std::cout << Form("TrdSCalibR::TrdLR_CalcIniPDF-I- Failed to find %s", grName) << std::endl;
	return false;
      }
    }
  }
  std::cout << "TrdSCalibR::TrdLR_CalcIniPDF-I- Read in Helium PDFs: Done !" << std::endl;
  



  /// Read Electron PDF's
  TH1D *h_Electron = (TH1D*)input->Get("h_Electron");
  if (!h_Electron) {
    std::cout << "TrdSCalibR::TrdLR_CalcIniPDF-I- Failed to get Electron PDFs Xbins!" << std::endl;
    return false;
  }
  TrdS_PDF_xElec = RootGetXbinsTH1(h_Electron);
  delete h_Electron; h_Electron = 0;

  for (int iXe=0; iXe<nBinfPXe; iXe++) {
    TrdS_PDF_nElec[iXe].assign(trdconst::nTrdLayers,1.0);
    for (unsigned int iV=0; iV<trdconst::nTrdLayers; iV++) {
      sprintf(grName,"grTrdS_Elec_%d_%d",iXe,iV);
      grTrdS_PDF_Elec[iXe].push_back((TGraph*)input->Get(grName));
      if (grTrdS_PDF_Elec[iXe].at(iV)==NULL) {
	std::cout << Form("TrdSCalibR::TrdLR_CalcIniPDF-I- Failed to find %s", grName) << std::endl;
	return false;
      }
      
    }
  }
  std::cout << "TrdSCalibR::TrdLR_CalcIniPDF-I- Read in Electron PDFs: Done !" << std::endl;


  /// Slow Control Data, Xe partial pressure
  grTrdS_Xe = (TGraph*)input->Get("grTrdSlowControl_Xe");
  if (!grTrdS_Xe) {
    Error("TrdSCalibR::TrdLR_CalcIniPDF-E- ", "TGraph grTrdS_Xe not found !");
    return false;
  }
  std::cout << Form("TrdSCalibR::TrdLR_CalcIniPDF-I- Read Xe Partial Pressure History")<<std::endl;

  
  input->Close();
  delete input;

   
  FirstLLCall = false;

  return true;

}

//--------------------------------------------------------------------------------------------------
int TrdSCalibR::GetXeInterval(double xDay) {
  double Xe = grTrdS_Xe->Eval(xDay);
  int iXe = -1;
  if (Xe>500.0 && Xe<750.0) {
    iXe = 0;
  } else if (Xe>=750.0 && Xe<800.0) {
    iXe = 1;
  } else if (Xe>=800.0 && Xe<850.0) {
    iXe = 2;
  } else if (Xe>=850.0 && Xe<900.0) {
    iXe = 3;
  } else if (Xe>=900.0 && Xe<950.0) {
    iXe = 4;
  } else if (Xe>=950.0 && Xe<1200.0) {
    iXe = 5;
  } else {
    std::cout << "TrdS_LR_Calc: Invalid Xe pressure: Xe=" << Xe << std::endl;
  }
  return iXe;
}

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
int TrdSCalibR::TrdLR_CalcXe(double xDay, float Rabs, int iFlag, int Debug) {

  //vector<double> TrdLPs, TrdLRs;
  TrdLPs.assign(3, 0.0);
  TrdLRs.assign(3,-1.0);
  
  if( Rabs<CalMomMin || Rabs>CalMomMax ) return 21;

  /// Which Rigidity Bin for Protons & Helium ?
  int iR        = -1;
  //int iMax 	= TrdPDF_xProt.size()-1;
  int iMax 	= TrdS_PDF_xProt.size()-1;
  if (iMax<=0) {
    Error("TrdSCalibR::TrdLR_CalcXe-E- ", "iMax<=0 is wrong");
    Error("TrdSCalibR::TrdLR_CalcXe-E- ", "First call TrdLR_CalcXe");
    return 22;
  }
  for (unsigned int i=0; i<iMax; i++) {
    if (Rabs >= TrdS_PDF_xProt.at(i) && Rabs < TrdS_PDF_xProt.at(i+1)) {
      iR = i;
      break;
    }
  }
  if (Rabs >= TrdS_PDF_xProt.at(iMax)) iR = iMax-1;
  if (Rabs <  TrdS_PDF_xProt.at(0))    iR = 0;
  
  if (iR<0) {
    Error("TrdSCalibR::TrdLR_CalcXe-E- ", "Out of rigidity range R=%f (GeV)", Rabs);
    return 23;
  }

  if(Debug) 
    std::cout << Form("TrdLR_CalcXe: R=%6.1f iP=%4d HitSize=%d ", Rabs, iR, (int)TrdSHits.size());
	
  double 	Lprod_Proton 	= 1.0;
  double 	Lprod_Helium 	= 1.0;
  double 	Lprod_Electron 	= 1.0;
  double	x[1], par[2];
  
  /// read Xe partial pressure from xDay
  int iXe = GetXeInterval(xDay);
  if(Debug) std::cout << Form("xDay=%8.4f iXe=%4d",xDay,iXe) << std::endl;
  if (iXe<0 || iXe>5) {
    std::cerr << "Error in TrdLR_Calc: invalid Xenon pressure iXe=" << iXe 
	      << " for day " << xDay << std::endl;
    return 24;		
  }

  int nCount = 0;
  vector<AC_TrdHits*>::iterator iter;
  for (iter = TrdSHits.begin(); iter != TrdSHits.end(); ++iter) {
    int 	  Layer = (*iter)->Lay;
    double	  Len   = (*iter)->Len2D;
    if (iFlag==3) Len   = (*iter)->Len3D;
    if (Len < trdconst::TrdMinPathLen3D) continue;
    double	scale 	= 2.0*trdconst::TrdStrawRadius/Len;
    double 	Eadc	= (*iter)->EadcCS*scale;
    

    x[0] 		= Eadc;
    par[0] 	        = iXe; 
    par[1] 	        = iR;	
    myPair <double> myProtonL  (1E-8, fabs(TrdSCalibR::TrdS_PDF_fProton(x,par)));
    double L_Proton     = myProtonL.GetMax();

    myPair <double> myHeliumL  (1E-8, fabs(TrdSCalibR::TrdS_PDF_fHelium(x,par)));
    double L_Helium   = myHeliumL.GetMax();

    par[1]              = Layer;  
    myPair <double> myElectronL(1E-8, fabs(TrdSCalibR::TrdS_PDF_fElectron(x,par)));
    double L_Electron   = myElectronL.GetMax();

    if(Debug > 1) 
      std::cout << Form("i=%3d EadcR=%4d EadcCS=%4d EadcPL=%4d L_Proton=%12.4E L_Helium=%12.4f L_Electron=%12.4f",
			nCount, (int)(*iter)->EadcR, (int)(*iter)->EadcCS, (int) Eadc,L_Proton,L_Helium,L_Electron) << std::endl;

    Lprod_Proton 	*= L_Proton;
    Lprod_Helium 	*= L_Helium;
    Lprod_Electron 	*= L_Electron;
    nCount++;
  }
   
  if(Debug > 1) 
    std::cout << Form("Step 1. |R|=%6.2f(GeV/c) nCount=%d Lprod_Electron=%12.4E Lprod_Helium=%12.4E Lprod_Proton=%12.4E", 
		      Rabs, nCount, Lprod_Electron, Lprod_Helium, Lprod_Proton) << std::endl;

  if(nCount==0) return 25;
  if(Lprod_Electron == 1.0 && Lprod_Proton == 1.0 && Lprod_Helium == 1.0)  return 26;

  Lprod_Proton 		= TMath::Power(Lprod_Proton,  1.0/float(nCount));
  Lprod_Helium 		= TMath::Power(Lprod_Helium,  1.0/float(nCount));
  Lprod_Electron 	= TMath::Power(Lprod_Electron,1.0/float(nCount));
  
  if(Debug > 1) 
    std::cout << Form("Step 2. |P|=%6.2f(GeV/c) Lprod_Electron=%12.4E Lprod_Helium=%12.4E Lprod_Proton=%12.4E", 
		      Rabs, Lprod_Electron, Lprod_Helium, Lprod_Proton) << std::endl;
  
  double LR_Elec_Prot   = Lprod_Electron/(Lprod_Electron+Lprod_Proton);
  double LR_Heli_Prot	= Lprod_Helium/(Lprod_Helium+Lprod_Proton);
  double LR_Elec_Heli	= Lprod_Electron/(Lprod_Electron+Lprod_Helium);

  

  //== electron, proton and helium probability products 
  TrdLPs.at(0) = Lprod_Electron;
  TrdLPs.at(1) = Lprod_Proton;
  TrdLPs.at(2) = Lprod_Helium;

  TrdLRs.at(0) = -log(LR_Elec_Prot);
  TrdLRs.at(1) = -log(LR_Heli_Prot);
  TrdLRs.at(2) = -log(LR_Elec_Heli);

  if(Debug > 1) 
    {
      int j = 0;
      for(vector<double>::iterator lr = TrdLRs.begin(); lr != TrdLRs.end(); ++lr )
	{
	  std::cout << Form("%s=%4.2f  ", TrdLLname[j++], *lr);
	}
      std::cout << std::endl;
    }
  
  return 0;


}
//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
bool TrdSCalibR::TrdLR_MC_CalcIniXe(int Debug) {

  std::cout << Form("TrdSCalibR::TrdLR_MC_CalcIniXe-I-  Initialize MC Calculator") << std::endl;
  char fName[80];
  
  for (int iXe=0; iXe<nBinfPXe; iXe++) {
    vector<TF1*> fp;
    for (unsigned int iV=0; iV<TrdS_PDF_xProt.size()-1; iV++) {
      sprintf(fName,"fTrdLR_fProton_%d_%d",iXe, iV);
      TF1* fpTrdLR = new TF1(fName, this, &TrdSCalibR::TrdS_PDF_fProton, 0.0,TrdMaxAdcLen,2);
      fpTrdLR->SetParameter(0,double(iXe));	
      fpTrdLR->SetParameter(1,double(iV));	
      fpTrdLR->SetNpx(1000);
      fpTrdLR->SetMinimum(1E-8);
      fp.push_back(fpTrdLR);
      fpTrdLR->Delete();
    }
    fTrdLR_fProton.push_back(fp);

    fp.clear();
    for (unsigned int iV=0; iV<TrdS_PDF_xHeli.size()-1; iV++) {
      sprintf(fName,"fTrdLR_fHelium_%d_%d",iXe, iV);
      TF1* fHeTrdLR = new TF1(fName, this, &TrdSCalibR::TrdS_PDF_fHelium, 0.0,TrdMaxAdcLen,2);
      fHeTrdLR->SetParameter(0,double(iXe));	
      fHeTrdLR->SetParameter(1,double(iV));	
      fHeTrdLR->SetNpx(1000);
      fHeTrdLR->SetMinimum(1E-8);
      fp.push_back(fHeTrdLR);
      fHeTrdLR->Delete();
    }
    fTrdLR_fHelium.push_back(fp);

    fp.clear();
    for (unsigned int iV=0; iV<trdconst::nTrdLayers; iV++) {
      sprintf(fName,"fTrdLR_fElectron_%d_%d",iXe, iV);
      TF1* feTrdLR = new TF1(fName, this, &TrdSCalibR::TrdS_PDF_fElectron, 0.0,TrdMaxAdcLen,2);
      feTrdLR->SetParameter(0,double(iXe));	
      feTrdLR->SetParameter(1,double(iV));	
      feTrdLR->SetNpx(1000);
      feTrdLR->SetMinimum(1E-8);
      fp.push_back(feTrdLR);
      feTrdLR->Delete();
    }
    fTrdLR_fElectron.push_back(fp);
    fp.clear();
  }

  FirstMCCall = false;
  
  if(Debug > 1)
    std::cout << Form("### Check LR Calc_MC Electrons=%12.4E Protons=%12.4E Helium=%12.4E",
		      fTrdLR_fElectron[0][0]->Integral((double)TrdMinAdc,(double)TrdMaxAdc),
		      fTrdLR_fProton[0][0]->Integral((double)TrdMinAdc,(double)TrdMaxAdc),
		      fTrdLR_fHelium[0][0]->Integral((double)TrdMinAdc,(double)TrdMaxAdc))  
	      << std::endl;
  
  return true;

}
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
int TrdSCalibR::TrdLR_MC_CalcXe(double xDay, float Rabs, vector<bool> PartId, int Debug) {

  TrdLRs_MC.assign(3,-1.0);

  if(FirstMCCall) return 1;
  
  if( Rabs<CalMomMin || Rabs>CalMomMax ) return 2;

  /// Which Momentum Bin for Protons & Helium ?
  int iR 	= -1;
  int iMax      = TrdS_PDF_xProt.size()-1;
  if (iMax<=0) {
    _ierror++;
    Error("TrdSCalibR::TrdLR_MC_CalcXe-E- ", "iMax<=0 is wrong");
    Error("TrdSCalibR::TrdLR_MC_CalcXe-E- ", "First call TrdLR_MC_CalcIniXe");
    return 3;
  }
  for (unsigned int i=0; i<iMax; i++) {
    if (Rabs >= TrdS_PDF_xProt.at(i) && Rabs < TrdS_PDF_xProt.at(i+1)) {
      iR = i;
      break;
    }
  }
  if (Rabs >= TrdS_PDF_xProt.at(iMax)) iR = iMax-1;
  if (Rabs <  TrdS_PDF_xProt.at(0))    iR = 0;
  
  if (iR<0) {
    _ierror++;
    if(_ierror<trdconst::nMaxError)
      Error("TrdSCalibR::TrdLR_MC_CalcXe-E- ", "Out of rigidity range R=%f (GV)", Rabs);
    return 4;
  }

  if(Debug) 
    std::cout << Form("TrdLR_MC_CalcXe: R=%6.1f iP=%4d HitSize=%d ", Rabs, iR, (int)TrdSHits.size());

  
  int j = 0;
  bool IsProton, IsHelium, IsElectron, IsPositron, IsAntiProton;
  for(vector<bool>::iterator id = PartId.begin(); id != PartId.end(); ++id ) {
    
    if(Debug) std::cout << Form("[%d:%s] ", j, (*id)?"true":"false");

    if(j == 0)   IsProton	= *id;
    if(j == 1)   IsHelium	= *id;
    if(j == 2)   IsElectron	= *id;
    if(j == 3)   IsPositron	= *id;
    if(j == 4)   IsAntiProton	= *id;
    j++;
  }
  if( !IsProton && !IsHelium && !IsElectron ) return 5;
  
  
  double x[1], par[2];
  int nCount = 0;
  int iXe    = GetXeInterval(xDay);  
  if(Debug) std::cout << Form("xDay=%8.4f iXe=%4d",xDay,iXe) << std::endl;
  if (iXe<0 || iXe>5) {
    Warning("TrdSCalibR::TrdLR_MC_CalcXe-W- ", "Invalid iXe=%d  for day %5.2f", iXe, xDay);
    return 6;		
  }

  vector<AC_TrdHits*>::iterator iter;
  for (iter = TrdSHits.begin(); iter != TrdSHits.end(); ++iter) {
    
    int   Layer = (*iter)->Lay;
    if ((*iter)->Len3D < trdconst::TrdMinPathLen3D) continue;
   
    double Eadc = 0.0;
    if (IsProton || IsAntiProton) 	Eadc = fTrdLR_fProton[iXe][iR]->GetRandom();
    if (IsHelium)                       Eadc = fTrdLR_fHelium[iXe][iR]->GetRandom();
    if (IsElectron || IsPositron)       Eadc = fTrdLR_fElectron[iXe][Layer]->GetRandom();
    
    x[0] 	      = Eadc;
    par[0] 	      = iXe; 
    par[1] 	      = iR;	
   
    myPair <double> myProtonL  (1E-8, fabs(TrdSCalibR::TrdS_PDF_fProton(x,par)));
    myPair <double> myHeliumL  (1E-8, fabs(TrdSCalibR::TrdS_PDF_fHelium(x,par)));
    par[1]            = Layer;  
    myPair <double> myElectronL(1E-8, fabs(TrdSCalibR::TrdS_PDF_fElectron(x,par)));
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
    std::cout << Form("Step 1. |R|=%6.2f(GV) MC Lprod_ElectronMC=%12.4E Lprod_HeliumMC=%12.4E Lprod_ProtonMC=%12.4E", 
		      Rabs, Lprod_ElectronMC, Lprod_HeliumMC, Lprod_ProtonMC) << std::endl;
  
  Lprod_ProtonMC 	= TMath::Power(Lprod_ProtonMC,  1.0/float(nCount));
  Lprod_HeliumMC 	= TMath::Power(Lprod_HeliumMC,  1.0/float(nCount));
  Lprod_ElectronMC 	= TMath::Power(Lprod_ElectronMC,1.0/float(nCount));
  
  if(Debug > 1) 
    std::cout << Form("Step 2. |P|=%6.2f(GV) MC Lprod_ElectronMC=%12.4E Lprod_HeliumMC=%12.4E Lprod_ProtonMC=%12.4E", 
		      Rabs, Lprod_ElectronMC, Lprod_HeliumMC, Lprod_ProtonMC) << std::endl;
  
  double LR_Elec_Prot = Lprod_ElectronMC/(Lprod_ElectronMC+Lprod_ProtonMC);
  double LR_Heli_Prot = Lprod_HeliumMC/(Lprod_HeliumMC+Lprod_ProtonMC);
  double LR_Elec_Heli = Lprod_ElectronMC/(Lprod_ElectronMC+Lprod_HeliumMC);
  
  TrdLRs_MC.at(0) = -log(LR_Elec_Prot);
  TrdLRs_MC.at(1) = -log(LR_Heli_Prot);
  TrdLRs_MC.at(2) = -log(LR_Elec_Heli);
  
  if(Debug > 1) 
    {
      int j = 0;
      std::cout << "MC " ;
      for(vector<double>::iterator lr = TrdLRs_MC.begin(); lr != TrdLRs_MC.end(); ++lr )
	{
	  std::cout << Form("%s=%4.2f  ", TrdLLname[j++], *lr);
	}
      std::cout << std::endl;
    }
  
  return 0;
}
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
double TrdSCalibR::GetProtonBetheBlochCorrFactor(float Pabs){
  if(!fBetheBlochProton){
    fBetheBlochProton = new TF1("fBetheBlochProton",this, &TrdSCalibR::FunBetheBloch,0.1,1000.0,5);
    fBetheBlochProton->SetParameters(2.97171e+00,9.67198e+04,-3.14408e+00,trdconst::mProt,+1.0);  
  }
  return fBetheBlochProton->Eval(Pabs);
}

double TrdSCalibR::GetHeliumBetheBlochCorrFactor(float Pabs){
  if(!fBetheBlochHelium){
    fBetheBlochHelium = new TF1("fBetheBlochHelium",this, &TrdSCalibR::FunBetheBloch,0.1,1000.0,5);
    fBetheBlochHelium->SetParameters(3.32153e+00,9.62920e+07,-3.21979e+00,trdconst::mHeli,+2.0);
  }
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

vector<double> TrdSCalibR::RootGetXbinsTH1(TH1 *hist) {
  int nBinX = hist->GetNbinsX();
  vector<double> xMin;
  for (int iBinX = 1; iBinX<=nBinX; iBinX++) {
    double xL = hist->GetXaxis()->GetBinLowEdge(iBinX);
    xMin.push_back(xL);
  }
  double xH = hist->GetXaxis()->GetBinLowEdge(nBinX) + hist->GetXaxis()->GetBinWidth(nBinX); 
  xMin.push_back(xH);
  return xMin;
}

//--------------------------------------------------------------------------------------------------

bool TrdSCalibR::Get04TrdCalibration(TString fname, int Debug) {
	
  TFile *input = new TFile(fname);
  if (!input->IsOpen()) {
    _ierror++;
    if(_ierror<trdconst::nMaxError)
      Error("TrdSCalibR::Get04TrdCalibration-E- ","Unable Open Calibtration File");
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
    FirstCalXday        = GetEvtxTime((time_t) FirstCalRunTime, Debug); //GetStime((time_t)FirstRun)/86400.0;
    LastCalXday         = GetEvtxTime((time_t) LastCalRunTime,  Debug); //GetStime((time_t)LastRun)/86400.0;
  }
  delete grTrdCalibRuns; grTrdCalibRuns = 0;

  std::cout << Form("TrdSCalibR::Get04TrdCalibration-I- Calibration Runs=%6d Interval %5d - %5d <=> %7.4f - %7.4f days ###", 
		    nCalibRuns, FirstCalRunTime, LastCalRunTime, FirstCalXday, LastCalXday) << std::endl;
  
  if(FirstCalXday < 0 || LastCalXday < 0) return false;
  
  int nLost = 0;
  char grName[80];
  TGraph *grTrd = new TGraph();
  vector<double> iTrdScalibXdaysMpv, iTrdScalibMpv;

  for (int iMod = 0; iMod < nTrdModules; iMod++) 
    {
      iTrdScalibXdaysMpv.clear();
      iTrdScalibMpv.clear();

      sprintf(grName,"grTrdCalibMpv_%d",iMod);
      grTrd = (TGraph*)input->Get(grName);
      if (!grTrd) {
	_ierror++;
	if(_ierror<trdconst::nMaxError)
	  Error("TrdSCalibR::Get04TrdCalibration-E- ","No GraphName .. %s", grName);
	nLost++;
      }
      
      RootTGraph2VectorX(grTrd, iTrdScalibXdaysMpv);
      RootTGraph2VectorY(grTrd, iTrdScalibMpv);

      TrdScalibXdaysMpv.push_back(iTrdScalibXdaysMpv);
      TrdScalibMpv.push_back(iTrdScalibMpv);

      grTrd->Clear();

    }
  grTrd->Delete(); grTrd = 0;
  
  input->Close();
  delete input;
 
  if (nLost==0) {
    std::cout << "TrdSCalibR::Get04TrdCalibration-I- " << fname 
	      << "Calibration File is sucessfully loaded "  << std::endl;
    return true;
  } else {
    _ierror++;
    if(_ierror<trdconst::nMaxError)
      Error("TrdSCalibR::Get04TrdCalibration", " Lost Entry in Calibration=%d", nLost); 
    return false;
  }

}

//--------------------------------------------------------------------------------------------------
bool TrdSCalibR::GetTrdAlignment(TString fname, int Debug) {
	
  TFile *input = new TFile(fname);
  if (!input->IsOpen()) {
    _ierror++;
    Error("TrdSCalibR::GetTrdAlignment-E- ","Unable Open Alignment File");
    return false;
  }

  /// read in alignment file
  vector<double> Runs;
  TGraph* grTrdCalibRuns = (TGraph*) input->Get("grTrdCalibRuns");
  int 	nAlignRuns = 0; 

  if(grTrdCalibRuns) {
    RootTGraph2VectorY(grTrdCalibRuns, Runs);
    nAlignRuns 	        = Runs.size();  
    FirstAlignRunTime	= TMath::Nint( Runs.at(0) );
    LastAlignRunTime	= TMath::Nint( Runs.at(nAlignRuns-1) ); 
    FirstAlignXday 	= GetEvtxTime((time_t) FirstAlignRunTime, Debug); 
    LastAlignXday 	= GetEvtxTime((time_t) LastAlignRunTime,  Debug);
  } else return false;

  delete grTrdCalibRuns; grTrdCalibRuns = 0;

  std::cout << Form("TrdSCalibR::GetTrdAlignment-I- Trd Alignment Runs=%6d Interval %5d - %5d <=> %7.4f - %7.4f days ###", 
		    nAlignRuns, FirstAlignRunTime, LastAlignRunTime, FirstAlignXday, LastAlignXday) << std::endl;
  
  if(FirstAlignXday < 0 || LastAlignXday < 0) return false;

  int nLost = 0;
  char grName[80];
  TGraph *grTrd = new TGraph();
  vector <double> iTrdScalibXdaysPos, iTrdScalibPos;
  for (int iMod = 0; iMod < nTrdModules; iMod++) 
    {
      iTrdScalibXdaysPos.clear();
      iTrdScalibPos.clear();
      sprintf(grName,"grTrdCalibPos_%d",iMod);
      grTrd = (TGraph*)input->Get(grName);
      if (!grTrd) {
	_ierror++;
	Error("TrdSCalibR::GetTrdAlignment-E- ", "No TGraphName .. %s", grName);
	nLost++;
      }
      
      RootTGraph2VectorX(grTrd, iTrdScalibXdaysPos);
      RootTGraph2VectorY(grTrd, iTrdScalibPos);

      TrdScalibXdaysPos.push_back(iTrdScalibXdaysPos);
      TrdScalibPos.push_back(iTrdScalibPos);

      if(Debug>1 && iMod==8) {
	int j = 0;
	for(vector<double>::iterator lr = iTrdScalibPos.begin(); lr != iTrdScalibPos.end(); ++lr ) {
	  cout <<  Form("iTrdScalibPos[%d]= %+8.4f  %+8.4f ", j, *lr, TrdScalibPos[iMod][j]);
	  j++;
	}
	cout << endl; 
      }

      grTrd->Clear();
    }
  grTrd->Delete(); grTrd = 0;
  
  input->Close();
  delete input; 

  if (nLost==0) {
    std::cout << "TrdSCalibR::GetTrdAlignment-I-" << fname 
	      << " Loaded Successfully "  << std::endl;
    return true;
  } else {
    _ierror++;
    Error("TrdSCalibR::GetTrdAlignment"," Lost Entry in Alignment=%d", nLost); 
    return false;
  }


}
//--------------------------------------------------------------------------------------------------
bool TrdSCalibR::GetTrdV4CalibHistos(int CalibLevel, int Debug) {
  
  TrdCalib_01 = false;  TrdCalib_02 = false; TrdCalib_03 = false; TrdCalib_04 = false;
  TrdAlign_01 = false;  TrdAlign_02 = false; TrdAlign_03 = false;

  if(Debug) 
    std::cout << "Get TRD Calibration Histograms: CalibLevel=" << CalibLevel << std::endl;
  cout << "----------------------------------------" << endl;
  char tdname[200]; 
  
  /// Call TRD Calibration Histograms 
  if ( CalibLevel < 5 )  sprintf(tdname, "%s/%s/%s", pPath,TrdDBUpdateDir[0], TrdCalDBUpdate[4]);   
  else if ( CalibLevel < 6 )  sprintf(tdname, "%s/%s/%s", pPath,TrdDBUpdateDir[0], TrdCalDBUpdate[5]);
  else return false;

  TrdCalib_04 = Get04TrdCalibration(tdname, Debug);  
  std::cout << Form("TrdSCalibR::GetTrdV4CalibHistos-I- ModCalibTime= %10u - %10u <=> %7.4f - %7.4f days",
		    FirstCalRunTime, LastCalRunTime, FirstCalXday, LastCalXday) << std::endl;
  if(Debug) 
    std::cout << Form("TrdScalibXdaysMpv[8].size()= %6d TrdScalibMpv[8].size()= %6d TrdScalibMpv[8][10]=%8.3f", 
		      (int)TrdScalibXdaysMpv[8].size(), (int)TrdScalibMpv[8].size(), TrdScalibMpv[8][10]) << std::endl;
  
  
  if(SetTrdAlign) 
    {
      /// Call Trd Alignment DB 
      if(TrdAlignMethod == 1) { //== method from S.Schael & T.Siedenburg
	if ( CalibLevel < 5 ) sprintf(tdname, "%s/%s/%s", pPath,TrdDBUpdateDir[0], TrdAlignDBUpdate[0]); 
	else if ( CalibLevel < 6 )  sprintf(tdname, "%s/%s/%s", pPath,TrdDBUpdateDir[0], TrdAlignDBUpdate[1]); 

	TrdAlign_01 = GetTrdAlignment(tdname, Debug);
	
	std::cout << Form("TrdSCalibR::GetTrdV4CalibHistos-I- ModAlignTime= %10u - %10u <=> %7.4f - %7.4f days",
			  FirstAlignRunTime, LastAlignRunTime, FirstAlignXday, LastAlignXday) << std::endl;
	if(Debug)
	  std::cout << Form("TrdScalibXdaysPos[8].size()= %6d TrdScalibPos[8].size()= %6d TrdScalibPos[8][10]=%8.3f", 
			    (int)TrdScalibXdaysPos[8].size(), (int)TrdScalibPos[8].size(), TrdScalibPos[8][10]) << std::endl;
      }
      else if(TrdAlignMethod == 2) { //== method from Z.Weng & A.Kounine	
	/// using TDV 
	TrdAlign_02 = true;
      }
      else if(TrdAlignMethod == 3) {
	/*
	//thetrdz.blay  = 0; // for trd global
	thetrdz.blay  = 1; // for layer wise alignment
	thetrdz.Debug = 0;
	thetrdz.initdb("/afs/cern.ch/exp/ams/Offline/AMSDataDir/DataBase/TrdCalNew/VZ/calout1302.root");
	*/
	TrdAlign_03 = true;
      }
      else return false;
      
    }
  
  
  FirstCall = false;
  
  if ( TrdCalib_04 && (TrdAlign_01 || TrdAlign_02 || TrdAlign_03) ) 
    return true;
  else return false;
    
}
//--------------------------------------------------------------------------------------------------
bool TrdSCalibR::GetTrdV5CalibHistos(int Debug) {

  TrdGain_01 = false;  TrdGain_02 = false;  TrdGain_03 = false;
  TrdAlign_01 = false; TrdAlign_02 = false; TrdAlign_03 = false;

  if(SCalibLevel < 5) return false;
  if(TrdGainMethod  < 1 || TrdGainMethod  > 3) return false;
  if(TrdAlignMethod < 1 || TrdAlignMethod > 3) return false;

  char tdname[200]=""; 
  if(TrdGainMethod == 1) {  //== method from S.Schael
    /// Call TRD Calibration Histograms 
    sprintf(tdname, "%s/%s/%s", pPath, TrdDBUpdateDir[0], TrdCalDBUpdate[5]);
    TrdGain_01 = Get04TrdCalibration(tdname, Debug);  

    std::cout << Form("TrdSCalibR::GetTrdV5CalibHistos-I- ModCalibTime= %10u - %10u <=> %7.4f - %7.4f days",
		      FirstCalRunTime, LastCalRunTime, FirstCalXday, LastCalXday) << std::endl;
    if(Debug) 
      std::cout << Form("TrdScalibXdaysMpv[8].size()= %6d TrdScalibMpv[8].size()= %6d TrdScalibMpv[8][10]=%8.3f", 
			(int)TrdScalibXdaysMpv[8].size(), (int)TrdScalibMpv[8].size(), TrdScalibMpv[8][10]) << std::endl;

  } else if(TrdGainMethod ==2 ) {
    TrdGain_02 = true;
  } else if(TrdGainMethod ==3 ) {

    TrdGain_03 = true;
  } else return false;

  if(SetTrdAlign) 
    {
      /// Call Trd Alignment DB 
      if(TrdAlignMethod == 1) { //== method from S.Schael & T.Siedenburg
	sprintf(tdname, "%s/%s/%s", pPath, TrdDBUpdateDir[0], TrdAlignDBUpdate[1]); 
	TrdAlign_01 = GetTrdAlignment(tdname, Debug);
	
	std::cout << Form("TrdSCalibR::GetTrdV5CalibHistos-I- ModAlignTime= %10u - %10u <=> %7.4f - %7.4f days",
			  FirstAlignRunTime, LastAlignRunTime, FirstAlignXday, LastAlignXday) << std::endl;
	if(Debug)
	  std::cout << Form("TrdScalibXdaysPos[8].size()= %6d TrdScalibPos[8].size()= %6d TrdScalibPos[8][10]=%8.3f", 
			    (int)TrdScalibXdaysPos[8].size(), (int)TrdScalibPos[8].size(), TrdScalibPos[8][10]) << std::endl;
      }
      else if(TrdAlignMethod == 2) { //== method from Z.Weng & A.Kounine 
	TrdAlign_02 = true;
      }
      else if(TrdAlignMethod == 3) { //== method from V.Zhukov
	/*
	//thetrdz.blay  = 0; // for trd global
	thetrdz.blay  = 1; // for layer wise alignment
	thetrdz.Debug = 0;
	thetrdz.initdb("/afs/cern.ch/exp/ams/Offline/AMSDataDir/DataBase/TrdCalNew/VZ/calout1302.root");
	*/
	TrdAlign_03 = true;
      }
      else return false;
      
    }
  
  
  FirstCall = false;
  
  if ( (TrdGain_01 || TrdGain_02 || TrdGain_03) && (TrdAlign_01 || TrdAlign_02 || TrdAlign_03) ) 
    return true;
  else return false;
    
}
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
bool TrdSCalibR::TrdScalibMinDist(double xDayRef, int xP, int iMod) {
  double dist0 = std::fabs(TrdScalibXdaysMpv[iMod].at(std::max(0, xP-1))-xDayRef);
  double dist1 = std::fabs(TrdScalibXdaysMpv[iMod].at(xP)-xDayRef);
  double dist2 = std::fabs(TrdScalibXdaysMpv[iMod].at(std::min(xP+1,int(TrdScalibXdaysMpv[iMod].size()-1)))-xDayRef);
  if (dist1<=dist0 && dist1<=dist2) return true;
  return false;
}
//-------------------------------------------------------------------------------------------------
bool TrdSCalibR::TrdScalibMinDist(double xDayRef, int xP, vector<double> &TrdScalibXdays) {	
  double dist0 = std::fabs(TrdScalibXdays.at(std::max(0,xP-1))-xDayRef);
  double dist1 = std::fabs(TrdScalibXdays.at(xP)-xDayRef);
  double dist2 = std::fabs(TrdScalibXdays.at(std::min(xP+1,int(TrdScalibXdays.size()-1)))-xDayRef);
  if (dist1<=dist0 && dist1<=dist2) return true;
  return false;
}
//--------------------------------------------------------------------------------------------------
// find a calibration constant within +/- 6 hours of key
int TrdSCalibR::TrdScalibBinarySearch(double key, int iMod, int Debug) {
  int 		first = 0;
  int 		last  = TrdScalibXdaysMpv[iMod].size()-1; 
  
  if (key<=TrdScalibXdaysMpv[iMod].at(first)) return first;
  if (key>=TrdScalibXdaysMpv[iMod].at(last))  return last;
  
  int iP = (first + last) / 2;
  while (last-first>1) 
    {
      iP = (first + last) / 2;  // compute mid point.
      if (key > TrdScalibXdaysMpv[iMod].at(iP)) {
	first = iP;             // repeat search in top half.
      } else if (key < TrdScalibXdaysMpv[iMod].at(iP)) {
	last = iP;              // repeat search in bottom half.
      } else {
	break;
      }
    }
  
  
  /// find the closest day
  double Distance1 = 0, Distance2 = 0;
  if (TrdScalibXdaysMpv[iMod].at(iP)<key && iP<TrdScalibXdaysMpv[iMod].size()-2) 
    {
      do 
	{
	  Distance1 = std::fabs(TrdScalibXdaysMpv[iMod].at(iP)-key);
	  iP++;
	  Distance2 = std::fabs(TrdScalibXdaysMpv[iMod].at(iP)-key);
	  if(Debug > 1)
	    std::cout << Form("1. iP=%4d %8.4f Distance1=%8.4f Distance2=%8.4f\n",
			      iP,TrdScalibXdaysMpv[iMod].at(iP),Distance1,Distance2) << std::endl;
	} while (Distance2<Distance1 && iP<TrdScalibXdaysMpv[iMod].size()-2);
      iP--;
    } else if (TrdScalibXdaysMpv[iMod].at(iP)>key && iP>0 )
    {
      do 
	{
	  Distance1 = std::fabs(TrdScalibXdaysMpv[iMod].at(iP)-key);
	  iP--;
	  Distance2 = std::fabs(TrdScalibXdaysMpv[iMod].at(iP)-key);
	  if(Debug > 1)
	    std::cout << Form("2. iP=%4d %8.4f Distance1=%8.4f Distance2=%8.4f\n",
			    iP,TrdScalibXdaysMpv[iMod].at(iP),Distance1,Distance2) << std::endl;
	} while (Distance2<Distance1 && iP>0);
      iP++;
    }
  return iP;
}

//--------------------------------------------------------------------------------------------------
int TrdSCalibR::TrdScalibBinarySearch(double key, vector<double> &TrdScalibXdays, int Debug) {

  int 		first = 0;
  int 		last 	= int(TrdScalibXdays.size())-1; 
  
  if (key<=TrdScalibXdays.at(first)) return first;
  if (key>=TrdScalibXdays.at(last))  return last;
  
  int iP = (first + last) / 2;
  while (last-first>1) {
    iP = (first + last) / 2;  //== compute mid point.
    if (key > TrdScalibXdays.at(iP)) {
      first = iP;             //== repeat search in top half.
    } else if (key < TrdScalibXdays.at(iP)) {
      last = iP;              //== repeat search in bottom half.
    } else {
      break;
    }
  }
 
  /// find the closest day
  double Distance1, Distance2;
  if (TrdScalibXdays.at(iP)<key && iP+2<TrdScalibXdays.size()) {
    do {
      Distance1 = std::fabs(TrdScalibXdays.at(iP)-key);
      iP++;
      Distance2 = std::fabs(TrdScalibXdays.at(iP)-key);
    } while (Distance2<Distance1 && iP+2<TrdScalibXdays.size());
    iP--;
  } else if (TrdScalibXdays.at(iP)>key && iP>0){
    do {
      Distance1 = std::fabs(TrdScalibXdays.at(iP)-key);
      iP--;
      Distance2 = std::fabs(TrdScalibXdays.at(iP)-key);
    } while (Distance2<Distance1 && iP>0);
    iP++;
  }
  if(Debug > 1)
    std::cout << Form("BinarySearch    key=%8.4f iP=%6d TrdScalibXdays.at(iP)=%8.4f",
		      key,iP,TrdScalibXdays.at(iP)) << std::endl;
  return iP;
}
//--------------------------------------------------------------------------------------------------

double TrdSCalibR::TrdScalibInterpolate(int iMod, double xDayRef, int &xP, int Debug) {

  if ( xP >= 0 && xP < int(TrdScalibXdaysMpv[iMod].size()-1) ) 
    {
      if (TrdScalibMinDist(xDayRef, xP, iMod)) {
      } else if (TrdScalibMinDist(xDayRef,xP+1, iMod)) {
	xP ++;
      } else {
	xP = TrdScalibBinarySearch(xDayRef, iMod, Debug);		
      }
    } else {
    xP = TrdScalibBinarySearch(xDayRef, iMod, Debug);				
  }

  
  double mpv = TrdMeanMPV;
  if (xP<0 || xP>TrdScalibXdaysMpv[iMod].size()-1) return mpv;
  
  mpv = TrdScalibMpv[iMod][xP];
    
  double x1=0,x2=0, y1=0,y2=0;
  if (TrdScalibXdaysMpv[iMod][xP]<xDayRef && xP<TrdScalibXdaysMpv[iMod].size()-2) {
    x1  = TrdScalibXdaysMpv[iMod][xP];
    y1  = TrdScalibMpv[iMod][xP];
    x2  = TrdScalibXdaysMpv[iMod][xP+1];
    y2  = TrdScalibMpv[iMod][xP+1];
    mpv = y2/(x2-x1)*(xDayRef-x1) + y1/(x2-x1)*(x2-xDayRef);
  } else if (TrdScalibXdaysMpv[iMod][xP]>xDayRef && xP>0) {
    x1  = TrdScalibXdaysMpv[iMod][xP-1];
    y1  = TrdScalibMpv[iMod][xP-1];
    x2  = TrdScalibXdaysMpv[iMod][xP];
    y2  = TrdScalibMpv[iMod][xP];
    mpv = y2/(x2-x1)*(xDayRef-x1) + y1/(x2-x1)*(x2-xDayRef);
  }

  if(Debug > 1) {
    std::cout << Form("<< xP[%03d]=%4d xDayRef=%8.4f xDay.at(xP)=%8.4f ",
		      iMod, xP,xDayRef,TrdScalibXdaysMpv[iMod].at(xP));
    std::cout << Form("MPV=%8.4f >>", mpv) 
	      << std::endl;
  }
  
  return mpv;
}
//--------------------------------------------------------------------------------------------------
double TrdSCalibR::TrdScalibInterpolate(double xDayRef, int &xP,
					vector<double> &TrdScalibXdays, vector<double> &TrdScalibVal, int Debug) {
 
  if(Debug > 1) std::cout << "1++++++++++++++++++++++++++++" << std::endl;
  if (xP>=0 && xP+1<int(TrdScalibXdays.size()-1)) {
    if(Debug > 1) std::cout << "1.1++++++++++++++++++++++++++++" << std::endl;
    if (TrdScalibMinDist(xDayRef,xP,TrdScalibXdays)) {
    } else if (TrdScalibMinDist(xDayRef,xP+1,TrdScalibXdays)) {
      xP ++;
    } else {
      xP = TrdScalibBinarySearch(xDayRef,TrdScalibXdays, Debug);		
    }
  } else {
    if(Debug > 1) std::cout << "1.2++++++++++++++++++++++++++++" << std::endl;
    xP = TrdScalibBinarySearch(xDayRef,TrdScalibXdays, Debug);				
  }
  if(Debug > 1) std::cout << "2++++++++++++++++++++++++++++ xP=" << xP << std::endl;
  if (xP<0 || xP>TrdScalibXdays.size()-1) return -1E99;

  double val = TrdScalibVal.at(xP);
  if(Debug > 1) std::cout << "3++++++++++++++++++++++++++++ val=" << val << std::endl;
  
  //== linear interpolation
  double x1,x2, y1,y2;
  if (TrdScalibXdays.at(xP)<xDayRef && xP+2<TrdScalibXdays.size()) {
    if(Debug > 1) std::cout << "4.1++++++++++++++++++++++++++++ xP=" << xP 
    			<< " TrdScalibXdays.size()=" << TrdScalibXdays.size() << std::endl;
    x1  = TrdScalibXdays.at(xP);
    y1  = TrdScalibVal.at(xP);
    x2  = TrdScalibXdays.at(xP+1);
    y2  = TrdScalibVal.at(xP+1);
    val = y2/(x2-x1)*(xDayRef-x1) + y1/(x2-x1)*(x2-xDayRef);
  } else if (TrdScalibXdays.at(xP)>xDayRef && xP>0) {
    if(Debug > 1 ) std::cout << "4.2++++++++++++++++++++++++++++ xP=" << xP << std::endl;
    x1  = TrdScalibXdays.at(xP-1);
    y1  = TrdScalibVal.at(xP-1);
    x2  = TrdScalibXdays.at(xP);
    y2  = TrdScalibVal.at(xP);		
    val = y2/(x2-x1)*(xDayRef-x1) + y1/(x2-x1)*(x2-xDayRef);
  }
  if(Debug > 1)
    std::cout << Form("TrdScalibInterpolate:: xDayRef=%8.4f Val=%8.4f",xDayRef,val) << std::endl;

 
  return val;
 
}
//--------------------------------------------------------------------------------------------------
int TrdSCalibR::BuildTrdSCalib(time_t evut, double fMom, TrdHTrackR *TrdHtrk, TrTrackR *Trtrk, double &s1, double &s2, double &s3, int Debug){
 
  s1 = s2 = s3 = 0.0;

  //== disable momentum limits on 2012.02.21 
  //if(fMom < CalMomMin || fMom > CalMomMax)  return 1;

  if( !GetdTrd(TrdHtrk) ||  !GetcTrd(TrdHtrk) || !GetcTrkdTrk(Trtrk) ) return 2;
  
  if(TrdHtrk->NTrdHSegment() != 2) return 3;
	   
  Htime = GetEvthTime(evut, Debug); 
  Xtime = GetEvtxTime(evut, Debug); 

  if ( SCalibLevel == 3 && (Xtime < FirstCalRunTime || Xtime > LastCalRunTime) ) 
    return 4;
  if ( SCalibLevel >  3 && (Xtime < FirstCalXday    || Xtime > LastCalXday) ) 
    return 4;

  if(Debug > 1) {
    int thread=0;
#ifdef _OPENMP
    thread=omp_get_thread_num();
#endif
    std::cout << Form("TrdSCalibLevel=%d TrdTrackLevel=%d TrdiFlag=%d ", SCalibLevel, TrdTrackLevel, iFlag)
	      << Form("Pabs=%6.3f Htime=%6d Xtime=%8.4f", fMom, Htime, Xtime)
	      << std::endl;
  }
  

  nTrdHitLayer.assign(trdconst::nTrdLayers, 0);
  TrdSHits.clear();

  /// initiate 
  AC_InitInterpolate2Z(1.0, 1.0, 0.0, 0.0);

  double TrdCalibMPV, TrdMPVofTheDay, ModMPVofTheDay;
  AC_TrdHits * AC = 0;

  int itrdhit = 0;
  for (unsigned int iseg=0; iseg < TrdHtrk->NTrdHSegment(); iseg++) 
    {
      if(!TrdHtrk->pTrdHSegment(iseg)) continue;
      for(int ir=0; ir < (int)TrdHtrk->pTrdHSegment(iseg)->fTrdRawHit.size();ir++)
	{
	  TrdRawHitR* rhit=TrdHtrk->pTrdHSegment(iseg)->pTrdRawHit(ir);
	  
	  if(!rhit) continue;
	  
	  AC = new AC_TrdHits();	  
	  AC->GetHitInfo(rhit);
	  AC->SetHitPos();
	  AC->TrkDraw = AC->GetTrdHitTrkDistance(AC, cTrk, dTrk);
	  AC->TrdDraw = AC->GetTrdHitTrkDistance(AC, cTrd, dTrd);

	  if( AC_ModAlign(AC, Debug) ) continue; 
	  
	  if( GetThisTrdHit(AC, Debug) ) continue;
	  	
	  if (AC->EadcCS < trdconst::TrdMinAdc || AC->EadcCS > trdconst::TrdMaxAdc) continue;
	  AC->Dxy  = AC->hitXY - AC->hitXYraw;
	  AC->Dz   = AC->hitZ  - AC->hitZraw;
	  AC->TrkD = AC->GetTrdHitTrkDistance(AC, cTrk, dTrk);
	 
	  if (fabs(AC->TrkD)>1.0*trdconst::CutTrdTrkD) continue;

	  if(fabs(AC->TrdD) > trdconst::TrdStrawRadius) 
	    AC->TrdD = trdconst::TrdStrawRadius * TMath::Sign(1.0, AC->TrdD);  
	  if(fabs(AC->TrkD) > trdconst::TrdStrawRadius) 
	    AC->TrkD = trdconst::TrdStrawRadius * TMath::Sign(1.0, AC->TrkD);
	  
	  if (AC->TrkD < trdconst::TrdStrawRadius) {
	    AC->Len2D = 2.0*TMath::Sqrt(TMath::Power(trdconst::TrdStrawRadius,2) - TMath::Power(AC->TrkD,2));
	    AC->Len3D = AC->GetTrdPathLen3D(AC->Lay,AC->hitXY, AC->hitZ, cTrk, dTrk);	    
	  } 
	  
	  if( AC->Len3D < trdconst::TrdMinPathLen3D ) continue; 

	  if(Debug > 1) std::cout << Form("*** Len2D=%5.2f Len3D=%5.2f ", AC->Len2D, AC->Len3D);
	  if( AC->Len3D < trdconst::TrdMinPathLen3D ) continue; 
	  

	  TrdSHits.push_back(AC);

	  AC = 0;
	  
	  itrdhit++;
	}
    }
  
  /// clear TG, TS
  AC_ClearInterpolate2Z();

  if( TrdLR_CalcXe(Xtime, fMom, iFlag, Debug) ) return 7; 
  
  s1 = TrdLRs[0]; //TrdLRLprod_Proton;
  s2 = TrdLRs[1]; //Lprod_Helium;
  s3 = TrdLRs[2]; //Lprod_Electron;
 
  delete AC; AC = 0;

  ClearTrdSHits();

  return 0;
}

//--------------------------------------------------------------------------------------------------
int TrdSCalibR::ProcessTrdHit(TrdHTrackR *TrdHtrk, int Debug){

  //if(TrdHtrk->NTrdHSegment() != 2) return 10;
  
  if( !GetdTrd(TrdHtrk) ||  !GetcTrd(TrdHtrk) ) return 11;

  if(Debug > 1) 
    std::cout << Form("TrdSCalibR::ProcessTrdHit-I- TrdHTrack Coo=(%+5.2f, %5.2f %5.2f)",
		      TrdHtrk->Coo[0],TrdHtrk->Coo[1],TrdHtrk->Coo[2]) << std::endl;

  nTrdHitLayer.assign(trdconst::nTrdLayers, 0);


  ClearTrdSHits();
  TrdSHits.clear();

  /// initiate 
  AC_InitInterpolate2Z(1.0, 1.0, 0.0, 0.0);

  double TrdCalibMPV, TrdMPVofTheDay, ModMPVofTheDay;
  
  AC_TrdHits * AC = 0;

  int itrdhit = 0;  
  for (unsigned int iseg=0; iseg < TrdHtrk->NTrdHSegment(); iseg++) 
    {
      if(!TrdHtrk->pTrdHSegment(iseg)) continue;
      
      for(int ir=0; ir < (int)TrdHtrk->pTrdHSegment(iseg)->fTrdRawHit.size();ir++)
	{
	  TrdRawHitR* rhit=TrdHtrk->pTrdHSegment(iseg)->pTrdRawHit(ir);
	  
	  if(!rhit) continue;
	  
	  if( rhit->Amp < trdconst::TrdMinAdc || rhit->Amp > trdconst::TrdMaxAdc) continue;
	  if( ExtTrdMinAdc > 0 && rhit->Amp < ExtTrdMinAdc ) continue;
	  
	  AC = new AC_TrdHits();	  
	  AC->GetHitInfo(rhit);
	  AC->SetHitPos();
	  AC->TrkDraw = AC->GetTrdHitTrkDistance(AC, cTrk, dTrk);
	  AC->TrdDraw = AC->GetTrdHitTrkDistance(AC, cTrd, dTrd);


	  if( AC_ModAlign(AC, Debug) ) continue; 
	  
	  if( GetThisTrdHit(AC, Debug) ) continue;
	  	
	  if (AC->EadcCS < trdconst::TrdMinAdc || AC->EadcCS > trdconst::TrdMaxAdc) continue;
	  if( ExtTrdMinAdc > 0 &&  (AC->EadcR < ExtTrdMinAdc) ) continue;
	  AC->Dxy  = AC->hitXY - AC->hitXYraw;
	  AC->Dz   = AC->hitZ  - AC->hitZraw;
	  AC->TrkD = AC->GetTrdHitTrkDistance(AC, cTrk, dTrk);
	 
	  if (fabs(AC->TrkD)>1.0*trdconst::CutTrdTrkD) continue;

	  if(fabs(AC->TrdD) > trdconst::TrdStrawRadius) 
	    AC->TrdD = trdconst::TrdStrawRadius * TMath::Sign(1.0, AC->TrdD);  
	  if(fabs(AC->TrkD) > trdconst::TrdStrawRadius) 
	    AC->TrkD = trdconst::TrdStrawRadius * TMath::Sign(1.0, AC->TrkD);
	  
	  if (AC->TrkD < trdconst::TrdStrawRadius) {
	    AC->Len2D = 2.0*TMath::Sqrt(TMath::Power(trdconst::TrdStrawRadius,2) - TMath::Power(AC->TrkD,2));
	    AC->Len3D = AC->GetTrdPathLen3D(AC->Lay,AC->hitXY, AC->hitZ, cTrk, dTrk);	    
	  } 
	  
	  if( AC->Len3D < trdconst::TrdMinPathLen3D ) continue; 


	  if(Debug > 1) std::cout << Form("*** Len2D=%5.2f Len3D=%5.2f ", AC->Len2D, AC->Len3D);
	  if( AC->Len3D < trdconst::TrdMinPathLen3D ) continue; 
	  
	  nTrdHitLayer[AC->Lay]++; 

	  TrdSHits.push_back(AC);

	  AC = 0;
	  
	  itrdhit++;
	}
    }

  /// clear TG, TS
  AC_ClearInterpolate2Z();

  GetnTrdHitLayer(TrdSHits, Debug);

  if( TrdSHits.size() < trdconst::nTrdMinHits) return 14; 
  
  if( GetTrdSum8(Debug) ) return 15;
  
  if( GetTruncatedMean(Debug) ) return 16;
  
  if( TrdLR_CalcXe(Xtime, iRabs, iFlag, Debug) ) return 17; 
 

  delete AC; AC = 0;
  
  return 0;
}

//--------------------------------------------------------------------------------------------------

int TrdSCalibR::ProcessTrdHit(TrdTrackR *Trdtrk, int Debug){
  
  //if(Trdtrk->NTrdSegment() != 4 ) return 10;   
  
  //if (!MatchingTrdTKtrack(iRabs, Debug) ) return 11; 

  nTrdHitLayer.assign(trdconst::nTrdLayers, 0);
 
  ClearTrdSHits();
  TrdSHits.clear();

  /// initiate 
  AC_InitInterpolate2Z(1.0, 1.0, 0.0, 0.0);
 
  double TrdCalibMPV, TrdMPVofTheDay, ModMPVofTheDay;
  
  AC_TrdHits * AC = NULL;
  
  for (int iseg = 0; iseg < Trdtrk->NTrdSegment(); iseg++)
    {		
      TrdSegmentR *trdseg = Trdtrk->pTrdSegment(iseg);
      if(!trdseg) continue;
      
      int itrdhit = 0;
      for (int j = 0; j <  trdseg->NTrdCluster(); j++)
	{
	  TrdClusterR  *trdcl = trdseg->pTrdCluster(j);
	  if(!trdcl) continue;
	  
	  TrdRawHitR  *rhit  = trdcl->pTrdRawHit();
	  if(!rhit) continue;	 

	  AC = new AC_TrdHits();	  
	  AC->GetHitInfo(rhit);
	  AC->SetHitPos();
	  AC->TrkDraw = AC->GetTrdHitTrkDistance(AC, cTrk, dTrk);
	  AC->TrdDraw = AC->GetTrdHitTrkDistance(AC, cTrd, dTrd);

	  if( AC_ModAlign(AC, Debug) ) continue; 
	  
	  if( GetThisTrdHit(AC, Debug) ) continue;
	  	
	  if (AC->EadcCS < trdconst::TrdMinAdc || AC->EadcCS > trdconst::TrdMaxAdc) continue;
	  if( ExtTrdMinAdc > 0 &&  (AC->EadcR < ExtTrdMinAdc) ) continue;
	  AC->Dxy  = AC->hitXY - AC->hitXYraw;
	  AC->Dz   = AC->hitZ  - AC->hitZraw;
	  AC->TrkD = AC->GetTrdHitTrkDistance(AC, cTrk, dTrk);
	 
	  if (fabs(AC->TrkD)>1.0*trdconst::CutTrdTrkD) continue;

	  if(fabs(AC->TrdD) > trdconst::TrdStrawRadius) 
	    AC->TrdD = trdconst::TrdStrawRadius * TMath::Sign(1.0, AC->TrdD);  
	  if(fabs(AC->TrkD) > trdconst::TrdStrawRadius) 
	    AC->TrkD = trdconst::TrdStrawRadius * TMath::Sign(1.0, AC->TrkD);
	  
	  if (AC->TrkD < trdconst::TrdStrawRadius) {
	    AC->Len2D = 2.0*TMath::Sqrt(TMath::Power(trdconst::TrdStrawRadius,2) - TMath::Power(AC->TrkD,2));
	    AC->Len3D = AC->GetTrdPathLen3D(AC->Lay,AC->hitXY, AC->hitZ, cTrk, dTrk);	    
	  } 
	  
	  if( AC->Len3D < trdconst::TrdMinPathLen3D ) continue; 

	  if(Debug > 1) { 
	    std::cout << std::endl
		      << Form("+-+ [%02d] iSeg=%d Layer=%2d Ladder=%2d Tube=%2d D=%d XY=%5.1f Z=%5.1f ", 
			      itrdhit, iseg, AC->Lay, AC->Lad, AC->Tub, AC->hitD, AC->hitXY, AC->hitZ); 
	    std::cout << Form("DHitTrk=%5.2f DHitTrd=%5.2f ", AC->TrkD, AC->TrdD);	   
	    std::cout << Form("StrawNr=%4d Mod=%3d GasCir=%2d ", AC->Straw, AC->Mod, AC->GC );
	    std::cout << Form("Len2D=%5.2f Len3D=%5.2f ", AC->Len2D, AC->Len3D);
	    std::cout << Form("EadcR=%4d EadcCS=%4d ", (int)AC->EadcR, (int) AC->EadcCS) 
		      << std::endl;
	    
	  }
	  
	  nTrdHitLayer[AC->Lay]++; 

	  TrdSHits.push_back(AC);

	  AC = 0;

	
	  itrdhit++;
	  
	}
    }

   
  /// clear TG, TS
  AC_ClearInterpolate2Z();


  GetnTrdHitLayer(TrdSHits, Debug);

  if( TrdSHits.size() < trdconst::nTrdMinHits ) return 14; 

  if( GetTrdSum8(Debug) ) return 15;
  if( GetTrdMedian(Debug) ) return 15;

  if( GetTruncatedMean(Debug) ) return 16;
  
  if( TrdLR_CalcXe(Xtime, iRabs, iFlag, Debug) ) return 17; 

  delete AC; AC = 0;

  return 0;
}

//--------------------------------------------------------------------------------------------------


















//--------------------------------------------------------------------------------------------------
int TrdSCalibR::ProcessTrdHit(TrTrackR *Trtrk, int Debug){

  
  ClearTrdSHits();

  if(Debug) {
    int i = 0;
    for (vector<AC_TrdHits*>::iterator iter = TrdNHits.begin(); iter != TrdNHits.end(); ++iter) 
      {
	if(*iter)
	  cout << Form("*** %03d Lay=%02d Lad=%02d Tub=%02d Amp=%6.1f / %6.1f XY=%+8.3f Z=%8.3f --> XY=%+8.3f Z=%8.3f", 
		       i++,  (*iter)->Lay, (*iter)->Lad, (*iter)->Tub, (*iter)->EadcR, (*iter)->EadcCS, 
		       (*iter)->hitXYraw, (*iter)->hitZraw, (*iter)->hitXY, (*iter)->hitZ )  << endl;
      }
    }

  /// do wee need a ms correction ?
  bool needMScorr = NeedTrkSpline(Trtrk, Debug);
  if(Debug) std::cout << Form("*** needMScorr=%s", needMScorr?"true":"false") << std::endl; 
  
  int msflag = 1;
  if(needMScorr) {
    /// get tracker splines in both xz and zy plane
    if(Debug) std::cout << Form("*** GetTrkSplin in ProcessTrdHit") << std::endl;
    if( GetTrkSpline(msflag, Debug) ) return 11; 
    
    /// get new hits and push_back into TrdSHits
    if(Debug) std::cout << Form("*** GetTrdNewHits in ProcessTrdHit") << std::endl;
    if( GetTrdNewHits_ms(TrdNHits, Debug) ) return 12; //== AC geo, gain and align are executed --> TrdSHits
    
  } else {
    /// get new hits and push_back TrdSHits
    if(Debug) std::cout << Form("*** GetTrdNewHits with no MS corr.") << std::endl;
    if( GetTrdNewHits(TrdNHits, Debug) ) return 12; //== AC geo, gain and align are executed --> TrdSHits   
  }
  
  if(Debug) {
    int ii = 0;
    for (vector<AC_TrdHits*>::iterator iter = TrdSHits.begin(); iter != TrdSHits.end(); ++iter) 
      {
	if(*iter)
	  cout << Form("*** %3d Lay=%02d Lad=%02d Tub=%02d Amp=%6.1f / %6.1f XY=%+8.3f Z=%8.3f --> XY=%+8.3f Z=%8.3f TrkD=%+8.3f", 
		       ii++,  (*iter)->Lay, (*iter)->Lad, (*iter)->Tub, (*iter)->EadcR, (*iter)->EadcCS, 
		       (*iter)->hitXYraw, (*iter)->hitZraw, (*iter)->hitXY, (*iter)->hitZ, (*iter)->TrkD )  << endl;
      }
    }
  

  int TrdStrkLevel = 2;
  int nLayNearStrk=0, nLayOnStrk=0, nHitOnStrk=0, nHitNearStrk=0;
  vector<int> nTrdStat;
  nTrdStat.assign(3,0);
  if(Debug) std::cout << Form("*** CalPathLen3D in ProcessTrdHit") << std::endl;
  nTrdStat = CalPathLen3D(TrdSHits, Trtrk, TrdStrkLevel, Debug);
  nLayNearStrk 	= nTrdStat.at(0); 
  nLayOnStrk	= nTrdStat.at(1); 
  nHitOnStrk	= nTrdStat.at(2); 
  nHitNearStrk 	= TrdSHits.size(); 

  _nOntrackhit  = nHitOnStrk;
  _nOfftrackhit = nHitNearStrk-nHitOnStrk;

  GetnTrdHitLayer(TrdSHits, Debug);

  if(Debug) std::cout << Form("*** nHitOnStrk=%3d nHitNearStrk=%3d ", nHitOnStrk, nHitNearStrk) << std::endl;

  
  //if (nHitNearStrk-nHitOnStrk > 12)  return 13;
 
  if( TrdSHits.size() < trdconst::nTrdMinHits ) return 14; 

  if( GetTrdSum8(Debug) ) return 15;
  if( GetTrdMedian(Debug) ) return 15;

  if( GetTruncatedMean(Debug) ) return 16;
 

  if( TrdLR_CalcXe(Xtime, iRabs, iFlag, Debug) ) return 17; 

  

  return 0;
}
//--------------------------------------------------------------------------------------------------

int TrdSCalibR::GetnTrdHitLayer(vector<AC_TrdHits*> &TrdHits, int Debug){
  nTrdHitLayer.assign(trdconst::nTrdLayers, 0);

  for (unsigned int i=0; i<TrdHits.size(); i++) {
    if(Debug) cout << Form("*C* %3d Lay=%02d Lad=%02d Tub=%02d Amp=%6.1f / %6.1f XY=%+8.3f Z=%8.3f --> XY=%+8.3f Z=%8.3f TrkD=%+8.3f, Len3D=%8.3f", 
			   i,  TrdHits.at(i)->Lay, TrdHits.at(i)->Lad, TrdHits.at(i)->Tub, TrdHits.at(i)->EadcR, TrdHits.at(i)->EadcCS, 
			   TrdHits.at(i)->hitXYraw, TrdHits.at(i)->hitZraw, TrdHits.at(i)->hitXY, TrdHits.at(i)->hitZ, 
			   TrdHits.at(i)->TrkD, TrdHits.at(i)->Len3D)  << endl;  
    
    if ((fabs(TrdHits.at(i)->Len3D)<trdconst::TrdMinPathLen3D)) continue;
    nTrdHitLayer[TrdHits.at(i)->Lay]++; 
  }
  

  int TrdNlayer = 0;
  int TrdNhits  = 0;
  int ihitlayer[3];
  for(int i=0;i<3;i++) ihitlayer[i] = 0;

  for(int i = 0; i < trdconst::nTrdLayers;i++) {
    if(nTrdHitLayer[i] > 0) {
      TrdNlayer++;
      if(i < 4) ihitlayer[0]++;        
      else if(i<16) ihitlayer[2]++; 
      else ihitlayer[1]++;   
      
      TrdNhits += nTrdHitLayer[i];
    }		  
  }	  
  
  if(Debug)
  std::cout << Form("*** TrdNlayer=%d  TrdNhits=%d HitLayer=(%2d %2d %2d)", 
  		    TrdNlayer, TrdNhits, ihitlayer[0],ihitlayer[1],ihitlayer[2] ) << std::endl;
  
  return 0;
 
}
//--------------------------------------------------------------------------------------------------

vector<int> TrdSCalibR::TrdFillHits( vector<AC_TrdHits*> &TrdHits, int Debug) {
	
  AMSPoint lcTrk; AMSDir ldTrk;

  int nTrdHitsOnTrk = 0;
  vector<int> Elay1, Elay2;
  Elay1.assign(trdconst::nTrdLayers,0);
  Elay2.assign(trdconst::nTrdLayers,0);

  if(Debug)
    std::cout << Form("*** TrdSCalibR::TrdFillHits-I Before TrdHits.size=%d", (int)TrdHits.size()) << std::endl;
 
  int ii = 0;
  for (vector<AC_TrdHits*>::iterator iter = TrdHits.begin(); iter != TrdHits.end(); ++iter) 
    {
      int   iD       = (*iter)->hitD;
      int   iLay     = (*iter)->Lay;
      float iXY      = (*iter)->hitXY;
      float iZ       = (*iter)->hitZ; 
    
      double iTrkD  = (*iter)->GetTrdHitTrkDistance(iD, iXY, iZ, cTrk, dTrk);
      (*iter)->TrkD = iTrkD;

      Elay1.at(iLay)++;

      if (fabs(iTrkD) < trdconst::TrdStrawRadius) {
	double len2D =  2.0*sqrt(pow(trdconst::TrdStrawRadius,2) - pow(iTrkD,2));
	(*iter)->Len2D	= len2D;
	double len3D = (*iter)->GetTrdPathLen3D(iLay, iXY, iZ, cTrk, dTrk);
	(*iter)->Len3D  = len3D;
	if ( (*iter)->Len3D > trdconst::TrdMinPathLen3D) {
	  nTrdHitsOnTrk ++;
	  Elay2.at(iLay) ++;
	}

      }

      if(Debug)
      cout << Form("*** %3d Lay=%02d Lad=%02d Tub=%02d Amp=%6.1f / %6.1f XY=%+8.3f Z=%8.3f --> XY=%+8.3f Z=%8.3f TrkD=%+8.3f, Len3D=%8.3f", 
		   ii++,  (*iter)->Lay, (*iter)->Lad, (*iter)->Tub, (*iter)->EadcR, (*iter)->EadcCS, 
		   (*iter)->hitXYraw, (*iter)->hitZraw, (*iter)->hitXY, (*iter)->hitZ, (*iter)->TrkD, (*iter)->Len3D)  << endl;      
      
    }
  

  int nerase = 0;
  for (unsigned int i=0; i<TrdHits.size(); i++) {
    if ( fabs(TrdHits.at(i)->TrkD)>trdconst::CutTrdTrkD ) { 
      TrdHits.erase(TrdHits.begin() + i);
      nerase++;
      continue;
    }   
  }

  if(Debug)
    std::cout << Form("*** TrdSCalibR::TrdFillHits-I After  TrdHits.size=%d  Erased=%d", (int) TrdHits.size(), nerase) << std::endl;
  
  int nTrdLayNearTrk = 0, nTrdLayOnTrk = 0;
  for (int i=0; i<trdconst::nTrdLayers; i++) {
    if (Elay1.at(i)>0) nTrdLayNearTrk ++;
    if (Elay2.at(i)>0) nTrdLayOnTrk ++;
  }
  
  vector<int> nTrdStat;
  nTrdStat.push_back(nTrdLayNearTrk);
  nTrdStat.push_back(nTrdLayOnTrk);
  nTrdStat.push_back(nTrdHitsOnTrk);

  if(Debug)
    std::cout << Form("*** TrdSCalibR::TrdFillHits   nTrdLayNearTrk=%3d nTrdLayOnTrk=%3d nTrdHitsOnTrk=%3d", 
		      nTrdLayNearTrk, nTrdLayOnTrk, nTrdHitsOnTrk) << std::endl;
  
  return nTrdStat;
}
//--------------------------------------------------------------------------------------------------

vector<int> TrdSCalibR::TrdFillHits2( vector<AC_TrdHits*> &TrdHits, int Debug) {
	
  AMSPoint lcTrk; AMSDir ldTrk;

  int nTrdHitsOnTrk = 0;
  vector<int> Elay1, Elay2;
  Elay1.assign(trdconst::nTrdLayers,0);
  Elay2.assign(trdconst::nTrdLayers,0);

  if(Debug)
    std::cout << Form("*** TrdSCalibR::TrdFillHits2-I Before TrdHits.size=%d", (int) TrdHits.size()) << std::endl;
 
  int ii = 0;
  for (vector<AC_TrdHits*>::iterator iter = TrdHits.begin(); iter != TrdHits.end(); ++iter) 
    {
      int   iD       = (*iter)->hitD;
      int   iLay     = (*iter)->Lay;
      float iXY      = (*iter)->hitXY;
      float iZ       = (*iter)->hitZ; 
    
      if( GetLocalTrkVec(iZ, lcTrk, ldTrk, 0, 0, Debug) ) continue;
      double iTrkD  = (*iter)->GetTrdHitTrkDistance(iD, iXY, iZ, lcTrk, ldTrk);
      (*iter)->TrkD = iTrkD;

      Elay1.at(iLay)++;

      if (fabs(iTrkD) < trdconst::TrdStrawRadius) {
	double len2D =  2.0*sqrt(pow(trdconst::TrdStrawRadius,2) - pow(iTrkD,2));
	(*iter)->Len2D	= len2D;
	double len3D = (*iter)->GetTrdPathLen3D(iLay, iXY, iZ, lcTrk, ldTrk);
	(*iter)->Len3D  = len3D;
	if ( (*iter)->Len3D > trdconst::TrdMinPathLen3D) {
	  nTrdHitsOnTrk ++;
	  Elay2.at(iLay) ++;
	}

      }

      if(Debug)
      cout << Form("*** %3d Lay=%02d Lad=%02d Tub=%02d Amp=%6.1f / %6.1f XY=%+8.3f Z=%8.3f --> XY=%+8.3f R=%+8.3f Z=%8.3f TrkD=%+8.3f, Len3D=%8.3f", 
		   ii++,  (*iter)->Lay, (*iter)->Lad, (*iter)->Tub, (*iter)->EadcR, (*iter)->EadcCS, 
		   (*iter)->hitXYraw, (*iter)->hitZraw, (*iter)->hitXY, (*iter)->hitR, (*iter)->hitZ, (*iter)->TrkD, (*iter)->Len3D)  << endl;      
      
    }
  

  int nerase = 0;
  for (unsigned int i=0; i<TrdHits.size(); i++) {
    if ( fabs(TrdHits.at(i)->TrkD)>trdconst::CutTrdTrkD ) { 
      TrdHits.erase(TrdHits.begin()+i);
      nerase++;
      continue;
    }   
  }

  if(Debug)
    std::cout << Form("*** TrdSCalibR::TrdFillHits2-I After  TrdHits.size=%d  Erased=%d", (int) TrdHits.size(), nerase) << std::endl;
  
  int nTrdLayNearTrk = 0, nTrdLayOnTrk = 0;
  for (int i=0; i<trdconst::nTrdLayers; i++) {
    if (Elay1.at(i)>0) nTrdLayNearTrk ++;
    if (Elay2.at(i)>0) nTrdLayOnTrk ++;
  }
  
  vector<int> nTrdStat;
  nTrdStat.push_back(nTrdLayNearTrk);
  nTrdStat.push_back(nTrdLayOnTrk);
  nTrdStat.push_back(nTrdHitsOnTrk);

  if(Debug)
    std::cout << Form("*** TrdSCalibR::TrdFillHits2   nTrdLayNearTrk=%3d nTrdLayOnTrk=%3d nTrdHitsOnTrk=%3d", 
		      nTrdLayNearTrk, nTrdLayOnTrk, nTrdHitsOnTrk) << std::endl;
  
  return nTrdStat;
}

//--------------------------------------------------------------------------------------------------
// int TrdStrkLevel = 2;
// 0 Use tracker track extrapolated to TRD, 
// 1 use Plane 1N Hit to correct for Multiple scattering, 
// 2 scan the tracker track through the TRD

vector<int> TrdSCalibR::CalPathLen3D(vector<AC_TrdHits*> &TrdHits, TrTrackR *Trtrk, int TrdStrkLevel, int Debug) {

  int iFlagTrkSpline = 0;
  if (TrdStrkLevel>0) iFlagTrkSpline =  1;    // iFlag=1 correction for multiple scattering using plane 1N

  vector<int> nTrdStat;
  nTrdStat.assign(3,0);
  int nLayNearStrk = 0, nLayOnStrk = 0, nHitOnStrk = 0, nHitNearStrk = 0;

  bool needMScorr = NeedTrkSpline(Trtrk, Debug);
  if(!needMScorr) {
    nTrdStat.assign(3,0);
    nTrdStat = TrdFillHits(TrdHits, Debug);
    return nTrdStat;
  }


  ///================================ 0th
  if(Debug) 
    std::cout << Form("*** 0th TrdFillHits  in CalPathLen3D") << std::endl;
  nTrdStat = TrdFillHits(TrdHits, Debug);
  
  /// is an additional MS correction usefull ?
  nLayNearStrk 	= nTrdStat.at(0); 
  nLayOnStrk 	= nTrdStat.at(1); 
  nHitOnStrk 	= nTrdStat.at(2); 
  nHitNearStrk 	= TrdHits.size(); 
 
  if(Debug)
    std::cout << Form("*** 0th TrdFillHits  nLayNearStrk=%3d nLayOnStrk=%3d nHitOnStrk=%3d nHitNearStrk=%3d", 
		      nLayNearStrk, nLayOnStrk, nHitOnStrk, nHitNearStrk ) << std::endl;

  if (nHitOnStrk > 17) return nTrdStat;
 

  ///================================ 1st 
  
  if(Debug) 
    std::cout << Form("*** 1st GetTrkSpline in CalPathLen3D") << std::endl;
  if( GetTrkSpline(iFlagTrkSpline,Debug) ) return nTrdStat;
  
  if(Debug) 
    std::cout << Form("*** 1st TrdFillHits2 in CalPathLen3D") << std::endl;
  nTrdStat.assign(3,0);
  nTrdStat = TrdFillHits2(TrdHits, Debug);
  if (TrdStrkLevel<2) return nTrdStat;
  
  /// is an additional MS correction usefull ?
  nLayNearStrk 	= nTrdStat.at(0); 
  nLayOnStrk 	= nTrdStat.at(1); 
  nHitOnStrk 	= nTrdStat.at(2); 
  nHitNearStrk 	= TrdHits.size(); 
  if(Debug)
  std::cout << Form("*** 1st TrdFillHits  nLayNearStrk=%3d nLayOnStrk=%3d nHitOnStrk=%3d nHitNearStrk=%3d", 
		      nLayNearStrk, nLayOnStrk, nHitOnStrk, nHitNearStrk ) << std::endl;
  
  //if (nHitOnStrk > 15 && (nHitNearStrk-nHitOnStrk)<4) return nTrdStat;
  //if (nHitOnStrk>17 && (nHitNearStrk-nHitOnStrk)<4) return nTrdStat;
  if (nHitOnStrk > 15 && (nHitNearStrk-nHitOnStrk) < 8 ) return nTrdStat;

  ///================================ 2nd 
  iFlagTrkSpline = 0;
  if(Debug) 
    std::cout << Form("*** 2nd GetTrkSpline in CalPathLen3D") << std::endl;
  if( GetTrkSpline(iFlagTrkSpline, Debug) ) return nTrdStat;

  if(Debug) 
    std::cout << Form("*** 2nd TrdFillHits2 in CalPathLen3D") << std::endl;
  nTrdStat.assign(3,0);
  nTrdStat = TrdFillHits2(TrdHits, Debug);
  
  nLayNearStrk 	= nTrdStat.at(0); 
  nLayOnStrk 	= nTrdStat.at(1); 
  nHitOnStrk 	= nTrdStat.at(2); 
  nHitNearStrk 	= TrdHits.size(); 
  if(Debug)
  std::cout << Form("*** 2nd TrdFillHits  nLayNearStrk=%3d nLayOnStrk=%3d nHitOnStrk=%3d nHitNearStrk=%3d", 
		      nLayNearStrk, nLayOnStrk, nHitOnStrk, nHitNearStrk ) << std::endl;
 
  /// MM: minuit2 minimization via virtual fitter 
  vector<float> Results;
  int 	nStep 		= IterateTrk4MS_ROOT(iRabs, TrdHits, Results, Debug);
  if(Debug)
  std::cout << Form("*** IterateTrk4MS .... nStep=%d", nStep) << std::endl;

  double Chi2Opt	= Results.at(0);
  double DeltaX_Opt  	= Results.at(1);
  double DeltaY_Opt  	= Results.at(2);
  int	nTrdHits_Opt 	= (int) Results.at(3);
	

  ///================================ 3rd
  TrkXcors[5]		= TrkXcors[4] + DeltaX_Opt;
  TrkYcors[5]		= TrkYcors[4] + DeltaY_Opt;
  
  iFlagTrkSpline	=	1; // iFlag=1 correction for multiple scattering using plane 1N
  if(Debug) 
    std::cout << Form("*** 3rd GetTrkSpline in CalPathLen3D") << std::endl;
  if( GetTrkSpline(iFlagTrkSpline, Debug) ) return nTrdStat;

  if(Debug) 
    std::cout << Form("*** 3rd TrdFillHits2 in CalPathLen3D") << std::endl;
  nTrdStat.assign(3,0);
  nTrdStat = TrdFillHits2(TrdHits, Debug);
  nLayNearStrk 	= nTrdStat.at(0); 
  nLayOnStrk 	= nTrdStat.at(1); 
  nHitOnStrk 	= nTrdStat.at(2); 
  nHitNearStrk 	= TrdHits.size(); 
  if(Debug)
  std::cout << Form("*** 3rd TrdFillHits  nLayNearStrk=%3d nLayOnStrk=%3d nHitOnStrk=%3d nHitNearStrk=%3d", 
		      nLayNearStrk, nLayOnStrk, nHitOnStrk, nHitNearStrk ) << std::endl;
 
  
  return nTrdStat;
	
}

//--------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------
void fcn_sfit(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{
  
  TrdSChi2Fit *fit=&TrdSCalibR::gethead()->fit;

  double Chi2=0.;
  if(fit->TrdHits.size()>0){
    TrdSCalibR::gethead()->TrdTrkChi2(fit->TrdHits, par[0], par[1], Chi2, fit->nTrdHits, 0);
    if(Chi2<=0.||isnan(Chi2)||isinf(Chi2))Chi2=1.e6;
    Chi2 += pow(par[0]/(2.0*fit->RmsX),2) + pow(par[1]/(2.0*fit->RmsY),2);
    f=Chi2;
  }
  else f=1.e6;
}
//--------------------------------------------------------------------------------------------------
int TrdSCalibR::IterateTrk4MS_ROOT(float aRig, vector<AC_TrdHits*> &TrdHits, vector<float> &Results, int Debug) {

  if(Debug) 
    std::cout << Form("TrdSCalibR::IterateTrk4MS_ROOT-I- TMinuit .... ") << std::endl;

  fit.RmsX = 0.1*sqrt(pow(6.579/aRig,2) + pow(0.1927,2));
  fit.RmsY = 0.1*sqrt(pow(5.994/aRig,2) + pow(0.00027,2));

  /// start values for parameters

  for(int i=0;i<TrdHits.size();i++)
    fit.TrdHits.push_back(TrdHits.at(i));

  TVirtualFitter::SetDefaultFitter("Minuit2");
  TVirtualFitter *vfit=TVirtualFitter::Fitter(0,2);
  vfit->SetFCN(fcn_sfit);
  vfit->SetParameter(0,"p0",0.,0.001,0,0);
  vfit->SetParameter(1,"p1",0.,0.001,0,0);
  
  Double_t arglist[10];
  arglist[0] = -1;
  vfit->ExecuteCommand("SET PRINT",arglist,1);
  arglist[0] = 50000;
  arglist[1] = 0.0001;
  int stat=vfit->ExecuteCommand("MINIMIZE",arglist,2);
  
  double Chi2Opt=0.;
  int nTrdHits_Opt=0;
  double DeltaX_Opt=vfit->GetParameter(0);
  double DeltaY_Opt=vfit->GetParameter(1);

  TrdTrkChi2(TrdHits, DeltaX_Opt, DeltaY_Opt, Chi2Opt, nTrdHits_Opt, Debug);		
  Chi2Opt += pow(DeltaX_Opt/(2.0*fit.RmsX),2) + pow(DeltaY_Opt/(2.0*fit.RmsY),2);//== some damping as we expect 0 from multiple scattering
  
  Results.clear();
  Results.push_back(Chi2Opt);
  Results.push_back(DeltaX_Opt);
  Results.push_back(DeltaY_Opt);
  Results.push_back(nTrdHits_Opt);
  Results.push_back(stat);

  fit.TrdHits.clear();
  return vfit->GetMaxIterations();
}

//--------------------------------------------------------------------------------------------------
int TrdSCalibR::TrdTrkChi2(vector<AC_TrdHits*> TrdHits,
			   double DeltaX, double DeltaY, double &Chi2, int &nTrdHits, int Debug) {
  
  Chi2 		= 0.0;
  nTrdHits 	= 0;
  int nAll	= 0;
  AMSPoint lcTrk;  AMSDir ldTrk;
  
  for (vector<AC_TrdHits*>::iterator iter = TrdHits.begin(); iter != TrdHits.end(); ++iter) {
    if( GetLocalTrkVec((*iter)->hitZ, lcTrk, ldTrk, DeltaX, DeltaY, Debug) ) continue;
    double TrkD = (*iter)->GetTrdHitTrkDistance(*iter, lcTrk, ldTrk);
    Chi2 += pow(TrkD/0.17,2);
    if (fabs(TrkD)<=trdconst::TrdStrawRadius) nTrdHits ++;
  }

  return 0;
}

//--------------------------------------------------------------------------------------------------
bool TrdSCalibR::NeedTrkSpline(TrTrackR *Trtrk, int Debug){

  bitset<9> HitTkLayer;
  HitTkLayer.reset();

  AMSPoint trCooL1, trCooL9;
  for(int it=1; it<=9; it++){
    TrRecHitR *rhit = (TrRecHitR*) Trtrk->GetHitLJ(it);
    if(rhit) AMSPoint trCoo = rhit->GetCoord();
    else continue;
    HitTkLayer.set(it-1);
    if(it==1) trCooL1 = rhit->GetCoord();
    if(it==9) trCooL9 = rhit->GetCoord();
  }

  int fitcode = Trtrk->iTrTrackPar(algo, patt, refit);
  if( fitcode < 0)  {
    Warning("TrdSCalibR::NeedTrkSpline-W- ", "TrackFit algo %d and patt %d Not AVAILABLE Fitcode=%d !!", 
	    algo,patt,fitcode);
    return 1;
  }

  AMSPoint pL1, pL9, pUtof;
  AMSDir   dL1, dL9, dUtof;
  if ( !Trtrk->Interpolate(trdconst::Zutof, pUtof, dUtof, fitcode) ) return 2;
  if ( !Trtrk->InterpolateLayerJ(1, pL1, dL1, fitcode) ) return 3;
  if ( !Trtrk->InterpolateLayerJ(9, pL9, dL9, fitcode) ) return 4;

  /// do we have a hit in Layer 1 to correct for multiple scattering ?
  double DiffX = trCooL1[0] - pL1[0]; //(TrkXcors[5]-TrkXcors[4]);
  double DiffY = trCooL1[1] - pL1[1]; //(TrkYcors[5]-TrkYcors[4]);

  if (fabs(DiffX)<1.0 && fabs(DiffY)<1.0 && fabs(iRabs)<20.) return true;
  else return false;
}
//--------------------------------------------------------------------------------------------------

int TrdSCalibR::GetTrkSpline( int msFlag, int Debug) {

  xTrks.assign(7,0.0);
  yTrks.assign(7,0.0);
  zTrks.assign(7,0.0);
  zTrks.at(0) = trdconst::Zutof - 1.0;
  zTrks.at(1) = trdconst::Zutof;
  zTrks.at(2) = trdconst::ZtrdL;
  zTrks.at(3) = trdconst::ZtrdC;
  zTrks.at(4) = trdconst::ZtrdU;
  zTrks.at(5) = trdconst::ZL1N;
  zTrks.at(6) = trdconst::ZL1N + 1.0;		

  /*
    TrkXcors[0] = X_UT;
    TrkXcors[1] = X_TL;		
    TrkXcors[2] = X_TC;		
    TrkXcors[3] = X_TU;		
    TrkXcors[4] = X_L1;		
    TrkXcors[6] = X_L9;		
    TrkXcors[5] = trCooL1[0];      //== L1 hit coord.
    TrkXcors[7] = trCooL9[0];      //== L9 hit coord.
   */

  
  for (int i=0; i<5; i++) {
    xTrks.at(i+1) = TrkXcors[i];
    yTrks.at(i+1) = TrkYcors[i];
  }


  /// correct for multiple scattering using plane 1, if possible and msFlag==1
  double DiffX = (TrkXcors[5] - TrkXcors[4]);
  double DiffY = (TrkYcors[5] - TrkYcors[4]);
  if (fabs(DiffX)<1.0 && fabs(DiffY)<1.0 && msFlag==1) {
    for (int i=0; i<5; i++) {
      double dx = DiffX/(trdconst::ZL1N-trdconst::Zutof)*(zTrks.at(i+1)-trdconst::Zutof);
      double dy = DiffY/(trdconst::ZL1N-trdconst::Zutof)*(zTrks.at(i+1)-trdconst::Zutof);
      xTrks.at(i+1) += dx;
      yTrks.at(i+1) += dy;
    }
  }
  
  /// Tracker track extrapolated from upper tof: c+b*lambda = p
  /// Richtungsvektor b, Aufpunkt c
  TVector3	cTrk0(xTrks.at(1), yTrks.at(1), zTrks.at(1));
  TVector3 	bTrk0(sin(Tht_UT)*cos(Phi_UT), sin(Tht_UT)*sin(Phi_UT), cos(Tht_UT));
  double        lambda0 = (zTrks.at(0)-cTrk0.Z())/bTrk0.Z();
  xTrks.at(0) = cTrk0.X() + lambda0*bTrk0.X();
  yTrks.at(0) = cTrk0.Y() + lambda0*bTrk0.Y();
  
  TVector3	cTrk1(xTrks.at(5), yTrks.at(5), zTrks.at(5));
  TVector3 	bTrk1(sin(Tht_L1)*cos(Phi_L1), sin(Tht_L1)*sin(Phi_L1), cos(Tht_L1));
  double	lambda1 = (zTrks.at(6)-cTrk1.Z())/bTrk1.Z();
  xTrks.at(6) = cTrk1.X() + lambda1*bTrk1.X();
  yTrks.at(6) = cTrk1.Y() + lambda1*bTrk1.Y();

  /// Graph to be used to extrapolate the tracker track into the TRD
  grTrkXZ = TSpline3("grTrkXZ",&zTrks[0],&xTrks[0],int(zTrks.size()));
  grTrkYZ = TSpline3("grTrkYZ",&zTrks[0],&yTrks[0],int(zTrks.size()));
  
  for(int i=0;i<7;i++) {
    if( TMath::IsNaN(grTrkXZ.Eval(zTrks[i])) || TMath::IsNaN(grTrkYZ.Eval(zTrks[i])) ){
      return 1;
    }
  }
 
  return 0;
  
}

//--------------------------------------------------------------------------------------------------
int TrdSCalibR::GetLocalTrkVec(float zTrdCor,
			       AMSPoint &lcTrk, AMSDir &ldTrk, double DeltaX, double DeltaY, int Debug) {

  double z0 = zTrdCor - 0.05;
  double z1 = zTrdCor + 0.05;
  if( TMath::IsNaN(grTrkXZ.Eval(z0))  ||  TMath::IsNaN(grTrkYZ.Eval(z0)) || 
      TMath::IsNaN(grTrkXZ.Eval(z1))  ||  TMath::IsNaN(grTrkYZ.Eval(z1))  ) return 1;
  double x0 = grTrkXZ.Eval(z0) + DeltaX/(trdconst::ZL1N-trdconst::Zutof)*(z0-trdconst::Zutof);
  double x1 = grTrkXZ.Eval(z1) + DeltaX/(trdconst::ZL1N-trdconst::Zutof)*(z1-trdconst::Zutof);
  double y0 = grTrkYZ.Eval(z0) + DeltaY/(trdconst::ZL1N-trdconst::Zutof)*(z0-trdconst::Zutof);
  double y1 = grTrkYZ.Eval(z1) + DeltaY/(trdconst::ZL1N-trdconst::Zutof)*(z1-trdconst::Zutof);

  if(Debug)
  std::cout << Form("zTrdcor=%+8.3f UT=(%+8.3f %+8.3f) grTrkXZ=(%+8.3f %+8.3f) grTrkYZ=(%+8.3f %+8.3f) x=(%+8.3f %+8.3f) y=(%+8.3f %+8.3f)", 
		    zTrdCor, X_UT, Y_UT, grTrkXZ.Eval(z0), grTrkXZ.Eval(z1), grTrkYZ.Eval(z0), grTrkYZ.Eval(z1), x0,x1,y0,y1) << std::endl;
  
  double bx = (x1-x0)/(z1-z0);
  double by = (y1-y0)/(z1-z0);
  double ax = x0 - bx*z0;
  double ay = y0 - by*z0;
  
  double TrkTht = atan(sqrt(bx*bx + by*by));
  double TrkPhi = atan2( by, bx );
  
  double xUtof_TRD = ax + bx*(trdconst::Zutof);
  double yUtof_TRD = ay + by*(trdconst::Zutof);
  
  lcTrk[0] = xUtof_TRD; lcTrk[1] = yUtof_TRD, lcTrk[2] = trdconst::Zutof;
  ldTrk[0] = sin(TrkTht)*cos(TrkPhi); 
  ldTrk[1] = sin(TrkTht)*sin(TrkPhi);
  ldTrk[2] = cos(TrkTht);

  /*
  std::cout << Form("*** lcTrk=%+8.3f %+8.3f %+8.3f ldTrk=%+8.3f %+8.3f %+8.3f",
		    lcTrk[0],lcTrk[1],lcTrk[2], ldTrk[0],ldTrk[1],ldTrk[2]) << std::endl;
  std::cout << Form("***  cTrk=%+8.3f %+8.3f %+8.3f  dTrk=%+8.3f %+8.3f %+8.3f",
		    cTrk[0],cTrk[1],cTrk[2], dTrk[0],dTrk[1],dTrk[2]) << std::endl;
  */
  
  return 0;
}

//--------------------------------------------------------------------------------------------------

int TrdSCalibR::GetTrdNewHits_ms(vector<AC_TrdHits*> TrdHits, int Debug) {

  AMSPoint lcTrk; AMSDir ldTrk;
  for (vector<AC_TrdHits*>::iterator iter = TrdHits.begin(); iter != TrdHits.end(); ++iter) 
    {	   
      
      if( AC_ModAlign(*iter, Debug) ) return 1; 

      double iTrkD  = (*iter)->GetTrdHitTrkDistance(*iter, cTrk, dTrk);
      (*iter)->TrkD = iTrkD;
      if (fabs(iTrkD)>1.0*trdconst::CutTrdTrkD) continue;
      
      if( ((*iter)->EadcR < trdconst::TrdMinAdc) || ((*iter)->EadcR > trdconst::TrdMaxAdc) ) continue;
      if( ExtTrdMinAdc > 0 &&  ((*iter)->EadcR < ExtTrdMinAdc) ) continue;
      
      if( GetThisTrdHit(*iter, Debug) ) continue;
       
      if( GetLocalTrkVec((*iter)->hitZ, lcTrk, ldTrk, 0,0, Debug) ) continue;
      iTrkD  = (*iter)->GetTrdHitTrkDistance(*iter, lcTrk, ldTrk);
      (*iter)->TrkD = iTrkD;
      if (fabs(iTrkD)>1.0*trdconst::CutTrdTrkD) continue;

      (*iter)->Dxy  = (*iter)->hitXY - (*iter)->hitXYraw;
      (*iter)->Dz   = (*iter)->hitZ  - (*iter)->hitZraw;
      
      TrdSHits.push_back(*iter);
    }
  
  return 0;
}

//--------------------------------------------------------------------------------------------------

int TrdSCalibR::GetTrdNewHits(vector<AC_TrdHits*> TrdHits, int Debug) {

  for (vector<AC_TrdHits*>::iterator iter = TrdHits.begin(); iter != TrdHits.end(); ++iter) 
    {	   
      if( AC_ModAlign(*iter, Debug) ) return 1; 
      
      double iTrkD  = (*iter)->GetTrdHitTrkDistance(*iter, cTrk, dTrk);
      (*iter)->TrkD = iTrkD;
      if (fabs(iTrkD)>1.0*trdconst::CutTrdTrkD) continue;
         
      if( ((*iter)->EadcR < trdconst::TrdMinAdc) || ((*iter)->EadcR > trdconst::TrdMaxAdc) ) continue;
      if( ExtTrdMinAdc > 0 &&  ((*iter)->EadcR < ExtTrdMinAdc) ) continue;

      if( GetThisTrdHit(*iter, Debug) ) continue;
      
      (*iter)->Dxy  = (*iter)->hitXY - (*iter)->hitXYraw;
      (*iter)->Dz   = (*iter)->hitZ  - (*iter)->hitZraw;
      
      TrdSHits.push_back(*iter); 
    }
  
  return 0;
}

//--------------------------------------------------------------------------------------------------

int TrdSCalibR::GetThisTrdHit(AC_TrdHits* &TrdHit, int Debug) {
  
  aP.assign(trdconst::nTrdModules, 0);
  aA.assign(trdconst::nTrdModules, 0);

  double TrdCalibMPV, TrdMPVofTheDay, ModMPVofTheDay;

  int iMod = TrdHit->Mod;
  
  /// apply gain correction
  if(TrdGainMethod == 1) {         //== Default from AC

    TrdCalibMPV = TrdMPVofTheDay = ModMPVofTheDay = 0;
    if(SCalibLevel < 3) {
      GetBinGasCirModule(Htime, SCalibLevel, TrdHit->GC, TrdHit->Mod, Debug); 
      
      /// calibration in the gas circuit level 
      TrdMPVofTheDay   = h_TrdGasCirMPV.at(TrdHit->GC)->GetBinContent(iBinGasCir);
      if (TrdMPVofTheDay>= 10.0 && TrdMPVofTheDay<=200.0)	
	TrdHit->EadcC1 = TrdHit->EadcR * trdconst::TrdMeanMPV / TrdMPVofTheDay;
      /// calibration in the module level 
      if (iBinModule>0) {
	ModMPVofTheDay = h_TrdModuleMPV.at(TrdHit->Mod)->GetBinContent(iBinModule);
	if (ModMPVofTheDay>=10 && ModMPVofTheDay<=200.0) 
	  TrdHit->EadcCS = TrdHit->EadcC1 * trdconst::TrdMeanMPVSC / ModMPVofTheDay; 
      }
    } else if (SCalibLevel < 4) /// CalibLevel = 3 for version 3
      {
	TrdCalibMPV = g_TrdCalibMPV.at(TrdHit->Mod)->Eval(Xtime);
	if (TrdCalibMPV>10 && TrdCalibMPV<200.0) 
	  TrdHit->EadcCS = TrdHit->EadcR * trdconst::TrdMeanMPV / TrdCalibMPV; 
      }
    
    else if (SCalibLevel < 6) { /// CalibLevel = 4 and 5
      TrdCalibMPV = TrdScalibInterpolate(Xtime, aP.at(iMod), TrdScalibXdaysMpv[iMod],TrdScalibMpv[iMod], Debug);
      if (TrdCalibMPV>10 && TrdCalibMPV<200.0) 
	TrdHit->EadcCS = TrdHit->EadcR * trdconst::TrdMeanMPV / TrdCalibMPV;	    
    }
      
    
  } else if(TrdGainMethod == 2) {  //== MIT

    if(_DB_instance.readDB_Calibration(Atime))
      TrdHit->EadcCS = TrdHit->EadcR* _DB_instance.GetGainCorrectionFactorTube(TrdHit->Straw, Atime);
    else {
      Warning("TrdSCalibR::GetThisTrdHit-W- ","AK GainDB::Warning out of time=%d", (int) Atime);
      TrdHit->EadcCS = TrdHit->EadcR;
    }

  } else if(TrdGainMethod == 3) {  //== AC-MM

    TrdHit->EadcCS = TrdHit->EadcR*TrdHCalibR::gethead()->GetGainCorr(TrdHit->Lay, TrdHit->Lad, TrdHit->Tub); 
    
  } else {
    _ierror++;
    if(_ierror<trdconst::nMaxError)
      Error("TrdSCalibR::GetThisTrdHit-E- ", "SCalibLevel = %d", SCalibLevel);
    return 1;
  }

  /// apply alignment correction
  if(TrdAlignMethod == 1) {
    
    double TrdAlignDxy = 0.0;
    
    TrdAlignDxy = TrdScalibInterpolate(Xtime, aA.at(iMod), TrdScalibXdaysPos[iMod],TrdScalibPos[iMod], Debug);
    if (fabs(TrdAlignDxy)<0.2) TrdHit->hitXY -= TrdAlignDxy; 
    
    if(Debug > 1)
      std::cout << Form(" ==> TrdAlignDxy=%+8.3f hitXY=%+8.3f", TrdAlignDxy, TrdHit->hitXY) << std::endl;
  }
  else if (TrdAlign_02) {

    AMSPoint TRDTube_Center, TRDTube_Dir, temp_TRDHit;
    if( TrdHit->hitD==1 ) { //== YZ plane
      TRDTube_Center[0]=0; TRDTube_Center[1]=TrdHit->hitXYraw; TRDTube_Center[2]=TrdHit->hitZraw; 
      TRDTube_Dir[0]=0; TRDTube_Dir[1]=1; TRDTube_Dir[2]=0;
      temp_TRDHit[0]=TrdHit->hitR; temp_TRDHit[1]=TrdHit->hitXYraw; temp_TRDHit[2]=TrdHit->hitZraw; 
    } else  {              //== XZ plane
      TRDTube_Center[0]=TrdHit->hitXYraw; TRDTube_Center[1]=0; TRDTube_Center[2]=TrdHit->hitZraw; 
      TRDTube_Dir[0]=1; TRDTube_Dir[1]=0; TRDTube_Dir[2]=0;
      temp_TRDHit[0]=TrdHit->hitXYraw; temp_TRDHit[1]=TrdHit->hitR; temp_TRDHit[2]=TrdHit->hitZraw; 
    }
    
    if( _DB_instance.readDB_Alignment(Atime)>0 ) {
      TRDAlignmentPar par =_DB_instance.GetAlignmentPar(TrdHit->Lay, (int) Atime); 
      
      AMSPoint  T(par.dX, par.dY, par.dZ);
      AMSRotMat R;
      R.SetRotAngles(-1*par.alpha, -1*par.beta, -1*par.gamma);
      
      AMSPoint Center(par.RotationCenter_X, par.RotationCenter_Y, par.RotationCenter_Z);
      
      AMSPoint TRDTube_Local = TRDTube_Center - Center;
      TRDTube_Center = R*TRDTube_Local + T + Center;
      TRDTube_Dir    = R*TRDTube_Dir;
      
      AMSPoint TRDHit_Local   = temp_TRDHit - Center;
      AMSPoint TRDHit_Aligned = R*TRDHit_Local + T + Center;
      
      if( TrdHit->hitD==1 ) { TrdHit->hitXY = TRDHit_Aligned.y(); TrdHit->hitR = TRDHit_Aligned.x();}
      else { TrdHit->hitXY = TRDHit_Aligned.x(); TrdHit->hitR = TRDHit_Aligned.y(); } 
      TrdHit->hitZ = TRDHit_Aligned.z();
      
    } else {
      Warning("TrdSCalibR::GetThisTrdHit-W- ","ZW AlignDB::Warning out of time=%d", (int) Atime);
    }
    
  }
  else if (TrdAlign_03) {
    // to be implemented asap.
    
  } else {
    _ierror++;
    if(_ierror<trdconst::nMaxError)
      Error("TrdSCalibR::GetThisTrdHit-E- ", "TrdAlign_XX");
    return 2;
  }
  
  aP.clear();
  aA.clear();
  
  return 0;

}
//--------------------------------------------------------------------------------------------------
int TrdSCalibR::GetUnknownHitPos(TrTrackR *Trtrk, float &alx, float &aly, float gz) {
  
  if(! Trtrk)     return 1;
  int fitcode = Trtrk->iTrTrackPar(algo, patt, refit);
  if(fitcode < 0) return 2;
  AMSPoint itkpnt;
  AMSDir   itkdir;
  Trtrk->Interpolate(gz, itkpnt, itkdir, fitcode);

  alx = itkpnt[0]+itkdir[0]/itkdir[2]*(gz - itkpnt[2]);
  aly = itkpnt[1]+itkdir[1]/itkdir[2]*(gz - itkpnt[2]);

  return 0;
}
//--------------------------------------------------------------------------------------------------
int TrdSCalibR::GetUnknownHitPos(TrdTrackR *Trdtrk, float &alx, float &aly, float gz) {
  
  if(! Trdtrk) return 1;
  alx = Trdtrk->Coo[0]+dTrd[0]/dTrd[2]*(gz - Trdtrk->Coo[2]);
  aly = Trdtrk->Coo[1]+dTrd[1]/dTrd[2]*(gz - Trdtrk->Coo[2]);

  return 0;
}
//--------------------------------------------------------------------------------------------------
int TrdSCalibR::GetUnknownHitPos(TrdHTrackR *TrdHtrk, float &alx, float &aly, float gz) {
  
  if(! TrdHtrk) return 1;
  TrdHtrk->propagateToZ(gz, alx, aly); 

  return 0;
}
//--------------------------------------------------------------------------------------------------
int TrdSCalibR::ProcessAlignCorrection(unsigned int iTrdAlignMethod, TrdTrackR *Trdtrk, AC_TrdHits *ACHit, int Debug) {
 
  float alx=0, aly=0;
  float TRD_Ax=0, TRD_Ay=0;

  if( GetUnknownHitPos(Trdtrk, alx, aly, ACHit->hitZraw) ) return 1;

  if( ACHit->hitD==1 ) ACHit->hitR = alx;
  else ACHit->hitR = aly;

  if(Debug > 1)
  std::cout << Form("rawhit(%+8.3f %+8.3f) --> trdpnt(%+8.3f %+8.3f %8.3f) trddir(%+8.3f %+8.3f %+8.3f) hitXYZ(%+8.3f %+8.3f %+8.3f)", 
		    ACHit->hitXYraw, ACHit->hitZraw,
		    Trdtrk->Coo[0], Trdtrk->Coo[1], Trdtrk->Coo[2], dTrd[0], dTrd[1], dTrd[2], 
		    ACHit->hitXY,  ACHit->hitR, ACHit->hitZ) << std::endl;


  if(iTrdAlignMethod == 1) {
    /// correction Z coord. imported from ACroot3.h 
    ACHit->hitZ   = ACHit->hitZraw + trdconst::TRD_Dz;   	    
    if( ACHit->hitD==1 ){                                                  // y-measurement
      TRD_Ay      = trdconst::TRD_Ay0 + ACHit->GetXY()/100.0*trdconst::TRD_Ay1 + ACHit->GetXY()/100.0*ACHit->GetXY()/100.0*trdconst::TRD_Ay2;
      ACHit->hitZ += alx*sin(TRD_Ay);                                      // TRD Y-Rot (XZ-Tilt)	
      TRD_Ax      = trdconst::TRD_Ax0 + alx/100.0*trdconst::TRD_Ax1 + alx/100.0*alx/100.0*trdconst::TRD_Ax2;
      ACHit->hitZ += ACHit->GetXY()*sin(TRD_Ax);                          // TRD X-Rot (YZ-Tilt)
    }else{                                                                 // x-measurement     	
      TRD_Ay      = trdconst::TRD_Ay0 + aly/100.0*trdconst::TRD_Ay1 + aly/100.0*aly/100.0*trdconst::TRD_Ay2;
      ACHit->hitZ +=  ACHit->GetXY()*sin(TRD_Ay);                         // TRD Y-Rot (XZ-Tilt)
      TRD_Ax      = trdconst::TRD_Ax0 + ACHit->GetXY()/100.0*trdconst::TRD_Ax1 + ACHit->GetXY()/100.0*ACHit->GetXY()/100.0*trdconst::TRD_Ax2;
      ACHit->hitZ +=  aly*sin(TRD_Ax);                                     // TRD X-Rot (YZ-Tilt)
    }
    
    /// correction XY coord.	
    if( ACHit->hitD==1 ){                                                   // y-measurement
      ACHit->hitXY = ACHit->hitXYraw + trdconst::TRD_Dy;                    // global TRD Y-Shift
      TRD_Ax      = trdconst::TRD_Ax0 + alx/100.0*trdconst::TRD_Ax1 + alx/100.0*alx/100.0*trdconst::TRD_Ax2;
      ACHit->hitXY += -(ACHit->GetZ() - trdconst::Ztrd)*sin(TRD_Ax);       // TRD X-Rotation (YZ-Tilt)	
      ACHit->hitXY += alx*sin(trdconst::TRD_Az - trdconst::TRD_As/2.);      // TRD Z-Rotation
      ACHit->hitR  = alx;                                                   // Coor. along the wire
    }else{                                                                  // x-measurement     
      ACHit->hitXY = ACHit->hitXYraw + trdconst::TRD_Dx;                                     // global TRD X-Shift	
      TRD_Ay     = trdconst::TRD_Ay0 + aly/100.0*trdconst::TRD_Ay1 + aly/100.0*aly/100.0*trdconst::TRD_Ay2;
      ACHit->hitXY += -(ACHit->GetZ() - trdconst::Ztrd)*sin(TRD_Ay);       // TRD Y-Rotation (XZ-Tilt)	
      ACHit->hitXY +=  -aly*sin(trdconst::TRD_Az + trdconst::TRD_As/2.);    // TRD Z-Rotation
      ACHit->hitR  = aly;                                                   // Coor. along the wire
    }     
  }


  if(iTrdAlignMethod == 2) {
    
    AMSPoint TRDTube_Center, TRDTube_Dir, temp_TRDHit;
    if( ACHit->hitD==1 ) { //== YZ plane
      TRDTube_Center[0]=0; TRDTube_Center[1]=ACHit->hitXYraw; TRDTube_Center[2]=ACHit->hitZraw; 
      TRDTube_Dir[0]=0; TRDTube_Dir[1]=1; TRDTube_Dir[2]=0;
      temp_TRDHit[0]=ACHit->hitR; temp_TRDHit[1]=ACHit->hitXYraw; temp_TRDHit[2]=ACHit->hitZraw; 
    } else  {              //== XZ plane
      TRDTube_Center[0]=ACHit->hitXYraw; TRDTube_Center[1]=0; TRDTube_Center[2]=ACHit->hitZraw; 
      TRDTube_Dir[0]=1; TRDTube_Dir[1]=0; TRDTube_Dir[2]=0;
      temp_TRDHit[0]=ACHit->hitXYraw; temp_TRDHit[1]=ACHit->hitR; temp_TRDHit[2]=ACHit->hitZraw; 
    }

    TRDAlignmentPar par =_DB_instance.GetAlignmentPar(ACHit->Lay, (int) Atime); 

    AMSPoint  T(par.dX, par.dY, par.dZ);
    AMSRotMat R;
    R.SetRotAngles(-1*par.alpha, -1*par.beta, -1*par.gamma);
    
    AMSPoint Center(par.RotationCenter_X, par.RotationCenter_Y, par.RotationCenter_Z);

    AMSPoint TRDTube_Local = TRDTube_Center - Center;
    TRDTube_Center = R*TRDTube_Local + T + Center;
    TRDTube_Dir    = R*TRDTube_Dir;

    AMSPoint TRDHit_Local   = temp_TRDHit - Center;
    AMSPoint TRDHit_Aligned = R*TRDHit_Local + T + Center;

    if( ACHit->hitD==1 ) { ACHit->hitXY = TRDHit_Aligned.y(); ACHit->hitR = TRDHit_Aligned.x();}
    else { ACHit->hitXY = TRDHit_Aligned.x(); ACHit->hitR = TRDHit_Aligned.y(); } 
    ACHit->hitZ = TRDHit_Aligned.z();

  } 

  if(iTrdAlignMethod == 3) {
    /*
    thetrdz.updatelayparameters(Atime, ACHit->Lay);
    vector<double> vpar=thetrdz.getparams();
    
    AMSPoint T(vpar.at(0),vpar.at(1),vpar.at(2));
    AMSRotMat R;
    R.SetRotAngles(-1*vpar.at(5), -1*vpar.at(4), -1*vpar.at(3));    
    AMSPoint Center(0,0, ZTRDc);

   
    if(ACHit->hitD==1)
      thetrdz.alignhit((int) Atime, &ACHit->hitR, &ACHit->hitXY, &ACHit->hitZ);
    else thetrdz.alignhit((int) Atime, &ACHit->hitXY, &ACHit->hitR, &ACHit->hitZ);
    */

  }

    
  if(Debug > 1) {
    if(ACHit->hitD==1) std::cout << Form("==> RawHit=(%+8.3f %+8.3f %+8.3f) ", alx, ACHit->hitXYraw, ACHit->hitZraw);
    else  std::cout << Form("==> RawHit=(%+8.3f %+8.3f %+8.3f) ", ACHit->hitXYraw, aly, ACHit->hitZraw);
    std::cout << Form(" ==> hitXY=%+8.3f hitR=%+8.3f hitZ=%+8.3f ", ACHit->hitXY, ACHit->hitR, ACHit->hitZ) << std::endl;
  }
  
  
  if( fabs(ACHit->hitXYraw - ACHit->hitXY) > 0.2 ) return 2;
  if( fabs(ACHit->hitZraw  - ACHit->hitZ)  > 0.2 ) return 3; 
  
  return 0;

}
//--------------------------------------------------------------------------------------------------
int TrdSCalibR::ProcessAlignCorrection(TrdTrackR *Trdtrk, AC_TrdHits *ACHit, int Debug) {
  
  float alx, aly, TRD_Ax, TRD_Ay;
  alx = aly = TRD_Ax = TRD_Ay = 0.;
  if(dTrd[2] == 0) return 1;
  alx = Trdtrk->Coo[0]+dTrd[0]/dTrd[2]*(ACHit->hitZraw - Trdtrk->Coo[2]);
  aly = Trdtrk->Coo[1]+dTrd[1]/dTrd[2]*(ACHit->hitZraw - Trdtrk->Coo[2]);
  
  /// correction Z coord. imported from ACroot3.h 
  ACHit->hitZ   = ACHit->hitZraw + trdconst::TRD_Dz;   	    
  if( ACHit->hitD==1 ){                                                  // y-measurement
    TRD_Ay      = trdconst::TRD_Ay0 + ACHit->GetXY()/100.0*trdconst::TRD_Ay1 + ACHit->GetXY()/100.0*ACHit->GetXY()/100.0*trdconst::TRD_Ay2;
    ACHit->hitZ += alx*sin(TRD_Ay);                                      // TRD Y-Rot (XZ-Tilt)	
    TRD_Ax      = trdconst::TRD_Ax0 + alx/100.0*trdconst::TRD_Ax1 + alx/100.0*alx/100.0*trdconst::TRD_Ax2;
    ACHit->hitZ += ACHit->GetXY()*sin(TRD_Ax);                          // TRD X-Rot (YZ-Tilt)
  }else{                                                                 // x-measurement     	
    TRD_Ay      = trdconst::TRD_Ay0 + aly/100.0*trdconst::TRD_Ay1 + aly/100.0*aly/100.0*trdconst::TRD_Ay2;
    ACHit->hitZ +=  ACHit->GetXY()*sin(TRD_Ay);                         // TRD Y-Rot (XZ-Tilt)
    TRD_Ax      = trdconst::TRD_Ax0 + ACHit->GetXY()/100.0*trdconst::TRD_Ax1 + ACHit->GetXY()/100.0*ACHit->GetXY()/100.0*trdconst::TRD_Ax2;
    ACHit->hitZ +=  aly*sin(TRD_Ax);                                     // TRD X-Rot (YZ-Tilt)	
  }

  /// correction XY coord.	
  if( ACHit->hitD==1 ){                                                   // y-measurement
    ACHit->hitXY = ACHit->hitXYraw + trdconst::TRD_Dy;                    // global TRD Y-Shift
    TRD_Ax      = trdconst::TRD_Ax0 + alx/100.0*trdconst::TRD_Ax1 + alx/100.0*alx/100.0*trdconst::TRD_Ax2;
    ACHit->hitXY += -(ACHit->GetZ() - trdconst::Ztrd)*sin(TRD_Ax);       // TRD X-Rotation (YZ-Tilt)	
    ACHit->hitXY += alx*sin(trdconst::TRD_Az - trdconst::TRD_As/2.);      // TRD Z-Rotation
    ACHit->hitR  = alx;                                                   // Coor. along the wire
  }else{                                                                  // x-measurement     
    ACHit->hitXY = ACHit->hitXYraw + trdconst::TRD_Dx;                                     // global TRD X-Shift	
    TRD_Ay     = trdconst::TRD_Ay0 + aly/100.0*trdconst::TRD_Ay1 + aly/100.0*aly/100.0*trdconst::TRD_Ay2;
    ACHit->hitXY += -(ACHit->GetZ() - trdconst::Ztrd)*sin(TRD_Ay);       // TRD Y-Rotation (XZ-Tilt)	
    ACHit->hitXY +=  -aly*sin(trdconst::TRD_Az + trdconst::TRD_As/2.);    // TRD Z-Rotation
    ACHit->hitR  = aly;                                                   // Coor. along the wire
  }    
  
  if(Debug > 1) {
    if(ACHit->hitD==1) std::cout << Form("==> RawHit=(%+8.3f %+8.3f) ", alx, ACHit->hitXYraw);
    else  std::cout << Form("==> RawHit=(%+8.3f %+8.3f) ", ACHit->hitXYraw, aly);
    std::cout << Form(" ==> hitXY=%+8.3f hitR=%+8.3f hitZ=%+8.3f ", ACHit->hitXY, ACHit->hitR, ACHit->hitZ) << std::endl;
  }

  if( fabs(ACHit->hitXYraw - ACHit->hitXY) > 0.2 ) return 2;
  if( fabs(ACHit->hitZraw  - ACHit->hitZ)  > 0.2 ) return 3; 

  return 0;

}

//--------------------------------------------------------------------------------------------------
int TrdSCalibR::ProcessAlignCorrection(TrdHTrackR *TrdHtrk, AC_TrdHits *ACHit, int Debug) {
  
  float alx, aly, TRD_Ax, TRD_Ay;
  alx = aly = TRD_Ax = TRD_Ay = 0.;	
  TrdHtrk->propagateToZ(ACHit->hitZraw, alx, aly); 

  /// correction Z coord. imported from ACroot3.h 
  ACHit->hitZ   = ACHit->hitZraw + trdconst::TRD_Dz;   	    
  if( ACHit->hitD==1 ){                                                  // y-measurement
    TRD_Ay      = trdconst::TRD_Ay0 + ACHit->GetXY()/100.0*trdconst::TRD_Ay1 + ACHit->GetXY()/100.0*ACHit->GetXY()/100.0*trdconst::TRD_Ay2;
    ACHit->hitZ += alx*sin(TRD_Ay);                                      // TRD Y-Rot (XZ-Tilt)	
    TRD_Ax      = trdconst::TRD_Ax0 + alx/100.0*trdconst::TRD_Ax1 + alx/100.0*alx/100.0*trdconst::TRD_Ax2;
    ACHit->hitZ += ACHit->GetXY()*sin(TRD_Ax);                          // TRD X-Rot (YZ-Tilt)
  }else{                                                                 // x-measurement     	
    TRD_Ay      = trdconst::TRD_Ay0 + aly/100.0*trdconst::TRD_Ay1 + aly/100.0*aly/100.0*trdconst::TRD_Ay2;
    ACHit->hitZ +=  ACHit->GetXY()*sin(TRD_Ay);                         // TRD Y-Rot (XZ-Tilt)
    TRD_Ax      = trdconst::TRD_Ax0 + ACHit->GetXY()/100.0*trdconst::TRD_Ax1 + ACHit->GetXY()/100.0*ACHit->GetXY()/100.0*trdconst::TRD_Ax2;
    ACHit->hitZ +=  aly*sin(TRD_Ax);                                     // TRD X-Rot (YZ-Tilt)	
  }
  
  /// correction XY coord.	
  if( ACHit->hitD==1 ){                                                   // y-measurement
    ACHit->hitXY = ACHit->hitXYraw + trdconst::TRD_Dy;                    // global TRD Y-Shift
    TRD_Ax      = trdconst::TRD_Ax0 + alx/100.0*trdconst::TRD_Ax1 + alx/100.0*alx/100.0*trdconst::TRD_Ax2;
    ACHit->hitXY += -(ACHit->GetZ() - trdconst::Ztrd)*sin(TRD_Ax);       // TRD X-Rotation (YZ-Tilt)	
    ACHit->hitXY += alx*sin(trdconst::TRD_Az - trdconst::TRD_As/2.);      // TRD Z-Rotation
    ACHit->hitR  = alx;                                                   // Coor. along the wire
  }else{                                                                  // x-measurement     
    ACHit->hitXY = ACHit->hitXYraw + trdconst::TRD_Dx;                                     // global TRD X-Shift	
    TRD_Ay     = trdconst::TRD_Ay0 + aly/100.0*trdconst::TRD_Ay1 + aly/100.0*aly/100.0*trdconst::TRD_Ay2;
    ACHit->hitXY += -(ACHit->GetZ() - trdconst::Ztrd)*sin(TRD_Ay);       // TRD Y-Rotation (XZ-Tilt)	
    ACHit->hitXY +=  -aly*sin(trdconst::TRD_Az + trdconst::TRD_As/2.);    // TRD Z-Rotation
    ACHit->hitR  = aly;                                                   // Coor. along the wire
  }    
  
  if(Debug > 1) {
    if(ACHit->hitD==1) std::cout << Form("==> RawHit=(%+8.3f %+8.3f) ", alx, ACHit->hitXYraw);
    else  std::cout << Form("==> RawHit=(%+8.3f %+8.3f) ", ACHit->hitXYraw, aly);
    std::cout << Form(" ==> hitXY=%+8.3f hitR=%+8.3f hitZ=%+8.3f ", ACHit->hitXY, ACHit->hitR, ACHit->hitZ) << std::endl;
  }

  if( fabs(ACHit->hitXYraw - ACHit->hitXY) > 0.2 ) return 2;
  if( fabs(ACHit->hitZraw - ACHit->hitZ)   > 0.2 ) return 3; 

  return 0;
}	   
//--------------------------------------------------------------------------------------------------


int TrdSCalibR::InitiateTrdRawHit(AMSEventR *pev, vector<AC_TrdHits> &TrdHits, int Debug){  

  TrdHits.clear();
  for (int i=0; i<pev->nTrdRawHit(); i++) {
    AC_TrdHits TrdHit;
  
    TrdRawHitR* rhit = pev->pTrdRawHit(i);
    TrdHit.GetHitInfo(rhit); TrdHit.SetHitPos();
    
    TrdHits.push_back(TrdHit);
  }

  return TrdHits.size();
}


int TrdSCalibR::InitiateTrdRawHit(AMSEventR *pev, vector<AC_TrdHits*> &TrdHits, int Debug){  

  TrdHits.clear();
  AC_TrdHits* AC = NULL;
  for (int i=0; i<pev->nTrdRawHit(); i++) {
    TrdRawHitR* rhit = pev->pTrdRawHit(i);

    AC = new AC_TrdHits();
    AC->GetHitInfo(rhit); 
    AC->SetHitPos();
    
    TrdHits.push_back(AC);
    AC = 0;
  }
  
  delete AC; AC = 0;
  
  return TrdHits.size();
}

int TrdSCalibR::InitiateTrdRawHit(AMSEventR *pev, int Debug){  

  AC_TrdHits *AC = NULL;
  for (int i=0; i<pev->nTrdRawHit(); i++) {
    
    TrdRawHitR* rhit = pev->pTrdRawHit(i);
    
    AC = new AC_TrdHits(); 
    AC->GetHitInfo(rhit);  AC->SetHitPos();
    
    TrdNHits.push_back(AC); 
    AC = 0;
  }
  
  delete AC; AC = 0;

  return TrdNHits.size();
}


//--------------------------------------------------------------------------------------------------
int TrdSCalibR::GetTrkCoordinates(TrTrackR *Trtrk, int Debug) {  

  if(Debug)
    std::cout << Form("TrdSCalibR::GetTrkCoordinates-I- ") << std::endl;
  TrkXcors.assign(8,0.0); TrkYcors.assign(8,0.0);

  int fitcode = Trtrk->iTrTrackPar(algo, patt, refit);
  if(fitcode < 0) {
    _ierror++;
    if(_ierror<trdconst::nMaxError)
      Error("TrdSCalibR::GetTrkCoordinates-E- "," iTrTrackerPar is not available");
    return 1;
  }

  bool passL1 = false;
  bool passL9 = false;
  bitset<9> HitTkLayer;
  HitTkLayer.reset();
  AMSPoint trCooL1, trCooL9;
  for(int it=1; it<=9; it++){
    TrRecHitR *rhit = (TrRecHitR*) Trtrk->GetHitLJ(it);
    if(rhit) AMSPoint trCoo = rhit->GetCoord();
    else continue;
    HitTkLayer.set(it-1);
    if(it==1) {
      trCooL1 = rhit->GetCoord();
      passL1 = true;
    }
    if(it==9) {
      trCooL9 = rhit->GetCoord();
      passL9 = true;
    }
  } 

  AMSPoint pL1, pL9, pUtof, pTU, pTC, pTL, pRich;
  AMSDir   dL1, dL9, dUtof, dTU, dTC, dTL, dRich;
  if ( !Trtrk->Interpolate(trdconst::Zutof, pUtof, dUtof, fitcode) ) return 1;
  if ( !Trtrk->Interpolate(trdconst::ZtrdU, pTU, dTU, fitcode) )     return 2;
  if ( !Trtrk->Interpolate(trdconst::ZtrdC, pTC, dTC, fitcode) )     return 3;
  if ( !Trtrk->Interpolate(trdconst::ZtrdL, pTL, dTL, fitcode) )     return 4;
  if ( !Trtrk->Interpolate(trdconst::Zrich, pRich, dRich, fitcode) ) return 5;
  if ( !Trtrk->Interpolate(trdconst::Z_l1,  pL1, dL1, fitcode) )     return 6;
  if ( !Trtrk->Interpolate(trdconst::Z_l9,  pL9, dL9, fitcode) )     return 7;

  //if ( !Trtrk->InterpolateLayerJ(1, pL1, dL1, fitcode) )             return 6;
  //if ( !Trtrk->InterpolateLayerJ(9, pL9, dL9, fitcode) )             return 7;

  Tht_L1 = dL1.gettheta();   Phi_L1 = dL1.getphi();
  Tht_L9 = dL9.gettheta();   Phi_L9 = dL9.getphi();
  Tht_UT = dUtof.gettheta(); Phi_UT = dUtof.getphi();
  Tht_RI = dRich.gettheta(); Phi_RI = dRich.getphi();

  X_L1 = pL1[0];  Y_L1 = pL1[1]; X_L9 = pL9[0];  Y_L9 = pL9[1];
  X_UT = pUtof[0];  Y_UT = pUtof[1];
  X_TU = pTU[0];  Y_TU = pTU[1]; X_TC = pTC[0];  Y_TC = pTC[1];  X_TL = pTL[0];  Y_TL = pTL[1];  
  X_RI = pRich[0];  Y_RI = pRich[1];

  
  TrkXcors[0] = X_UT;
  TrkXcors[1] = X_TL;		
  TrkXcors[2] = X_TC;		
  TrkXcors[3] = X_TU;		
  TrkXcors[4] = X_L1;		
  TrkXcors[6] = X_L9;
		
  TrkYcors[0] = Y_UT;		
  TrkYcors[1] = Y_TL;		
  TrkYcors[2] = Y_TC;		
  TrkYcors[3] = Y_TU;		
  TrkYcors[4] = Y_L1;		
  TrkYcors[6] = Y_L9;		
  
  if(passL1) {
    TrkXcors[5] = trCooL1[0];      //== L1 hit coord.
    TrkYcors[5] = trCooL1[1];
  } else {
    TrkXcors[5] = -999.;  
    TrkYcors[5] = -999.;
  }
  if(passL9) {
    TrkXcors[7] = trCooL9[0];      //== L9 hit coord.
    TrkYcors[7] = trCooL9[1];
  } else {
    TrkXcors[7] = -999.;  
    TrkYcors[7] = -999.;
  }
 
  return 0;

}
//--------------------------------------------------------------------------------------------------

int TrdSCalibR::ProcessTrdEvt(AMSEventR *pev, int Debug) {
 
  /// check gain calibration time
  Htime = GetEvthTime(pev, Debug); //== used ver.1 & ver.2 only
  Xtime = GetEvtxTime(pev, Debug); //== used ver > 2
  Atime = (double) pev->fHeader.Time[0]; //== used for alignment 
  if ( SCalibLevel == 3 && (Xtime < FirstCalRunTime || Xtime > LastCalRunTime) ) 
    return 1;
  if ( SCalibLevel > 3 && TrdGainMethod==1 && (Xtime < FirstCalXday || Xtime > LastCalXday) ) 
    return 2;
 

  if( pev->nParticle() != 1 ) return 3;   
  
  iPabs = std::fabs(pev->pParticle(0)->Momentum);  
  iQabs = pev->pParticle(0)->Charge;
  
  TrTrackR    *Trtrk = NULL;
  TrdHTrackR  *TrdHtrk = NULL;
  TrdTrackR   *Trdtrk = NULL;

  /// clean vector
  Clear();

  /// check tracker object
  Trtrk   = pev->pParticle(0)->pTrTrack();
  if(Trtrk) {
    int itr = ProcessTrTrack(Trtrk);
    if(itr) return itr;
  } else return 4;

  /// get tracker track point and direction
  /// global cTrk, dTrk from given fitcode
  if( !GetcTrkdTrk(Trtrk) ) return 5;

  switch ( TrdTrackLevel ) {

  case 0: //== TrdS track from tracker track (AC)            
    return ProcessTrdEvtWithTrTrack(pev, Trtrk, Debug);
    break;   

  case 1: //== TrdS track from tracker track (AC)   same as 0         
    return ProcessTrdEvtWithTrTrack(pev, Trtrk, Debug);
    break;

  case 2: //== TrdHtrack (AC)         
    if( pev->nTrdHTrack() == 0 ) return 7;
    if(!pev->pParticle(0)->pTrdHTrack())  return 8;
    TrdHtrk = pev->pParticle(0)->pTrdHTrack();
    if( !GetdTrd(TrdHtrk) ||  !GetcTrd(TrdHtrk) ) return 9;
    return ProcessTrdHit(TrdHtrk, Debug); 
    break;

  case 3: //== Trdtrack   (MIT)
    if( pev->nTrdTrack() == 0 ) return 7;
    if(!pev->pParticle(0)->pTrdTrack())   return 8;
    Trdtrk = pev->pParticle(0)->pTrdTrack();
    if( !GetdTrd(Trdtrk) ||  !GetcTrd(Trdtrk) ) return 9;
    return ProcessTrdHit(Trdtrk,  Debug); 
    break;

  case 4: //== TrdZ track from tracker track (MIT)           
    return ProcessTrdZ(pev, Trtrk, Debug);
    break;

  default:            //
    std::cout << "TrdSCalibR::ProcessTrdEvt-E- No TrdTrackType= " << TrdTrackLevel << std::endl;
    return 3;
    break;
  }

}
//--------------------------------------------------------------------------------------------------
int TrdSCalibR::ProcessTrTrack(TrTrackR* Trtrk){

  int fitcode = Trtrk->iTrTrackPar(algo, patt, refit); 
  
  iRsigned=1000, iRerrinv=0;
  if( fitcode>=0) {iRsigned = Trtrk->GetRigidity(fitcode); iRerrinv = Trtrk->GetErrRinv(fitcode);} 
  else {
    _ierror++;
    if(_ierror<trdconst::nMaxError)
      std::cout << Form("TrdSCalibR::ProcessTrTrack-E- TrackFit Fails (algo=%d patt=%d refit=%d fitcode=%d)",
			algo,patt,refit,fitcode) << std::endl;
    return 4;
  }

  iRabs = fabs(iRsigned);
  if (iRabs <= CalMomMin) iRabs = CalMomMin + 1E-5;
  if (iRabs >= CalMomMax) iRabs = CalMomMax - 1E-5;

  
  return 0;


}
//--------------------------------------------------------------------------------------------------
int TrdSCalibR::ProcessTrdZ(AMSEventR *pev, TrTrackR *Trtrk, int Debug) {
  	 
  
  int fitcode = Trtrk->iTrTrackPar(algo, patt, refit); 
  if(fitcode<0) return 2;

  float threshold=15; 
  TrdKCluster _trdcluster = TrdKCluster(pev, Trtrk, fitcode);  

  int NHits[2] = {0,0};  
  double LikelihoodRatio[3]={-1,-1,-1};  //== e/P, e/H, P/H
  double LikelihoodRatioRefit[3]={-1,-1,-1};  //== e/P, e/H, P/H
 	  
  // 0: No Alignment,  1: Static Alignment at Layer level,  2: Dynamic Alignment for full TRD
  if(!_trdcluster.IsReadAlignmentOK) return 3; 

  // 0: Gain Calibration not performed,  1: Gain Calibration Succeeded
  if(!_trdcluster.IsReadCalibOK) return 4;    
  
  // Calculate Likelihood Ratio, Fill the LikelihoodRatio, NHits according to Track selection, and return validity 
  if(!_trdcluster.GetLikelihoodRatio_TrTrack(threshold, LikelihoodRatio, NHits[0]) ) return 5; 
  
  //Get Number of surrounding fired tubes, excluding the ones crossed by the current TrTrack prediction
  //_trdcluster.GetOffTrackHit_TrTrack(OffTrack_nhits[0],OffTrack_amps);
    
  int isLikValid = _trdcluster.GetLikelihoodRatio_TRDRefit(threshold,LikelihoodRatioRefit,NHits[1]);
  //Get Number of surrounding fired tubes, excluding the ones crossed by the current TrTrack prediction
  //_trdcluster.GetOffTrackHit_TRDRefit(OffTrack_nhits[1],OffTrack_amps);
 
  if(isLikValid) {
    TrdLRs.at(0) = LikelihoodRatioRefit[0];  TrdLRs.at(1) = LikelihoodRatioRefit[2]; TrdLRs.at(2) = LikelihoodRatioRefit[1]; 
  } else {
    TrdLRs.at(0) = LikelihoodRatio[0];  TrdLRs.at(1) = LikelihoodRatio[2]; TrdLRs.at(2) = LikelihoodRatio[1]; 
  }
  
  return 0;

}

//--------------------------------------------------------------------------------------------------
int TrdSCalibR::ProcessTrdEvtWithTrTrack(AMSEventR *pev, TrTrackR *Trtrk, int Debug){

  int itrdcalib = 0;

  /// clean vector
  Clear();

  
  /// extract trd geom. parameters from TrTrack 
  /// assign TrkXcors[], TrkYcors[] to manage multiple scattering
  if( GetTrkCoordinates(Trtrk, Debug) ) return 5;
  
  /// push_back to TrdRawHit -> TrdNHits
  TrdNHits.clear();
  int init_nhit = InitiateTrdRawHit(pev, Debug); 
  if(Debug) 
    std::cout << Form("TrdSCalibR::ProcessTrdEvtWithTrTrack-I- InitiateTrdRawHit= %d", init_nhit) << std::endl;
  if( !init_nhit || init_nhit > trdconst::nTrdMaxHits) {
    ClearTrdNHits(); //== clear TrdNHits
    return 6; //== no hit or too many hits
  }
  
  /// make AC splines from trd geom. paramters TS[2]
  AC_InitInterpolate2Z(1.0, 1.0, 0.0, 0.0); //== (float dZ_UT, float dZ_L1, float dY_UT, float dY_L1)
  
  /// use track track instead of trdtrack or trdhtrack
  itrdcalib = ProcessTrdHit(Trtrk, Debug);
  
  /// clear TG, TS
  AC_ClearInterpolate2Z();
  
  /// clear TrdNHits
  ClearTrdNHits();
  
  return itrdcalib;
     

}
//--------------------------------------------------------------------------------------------------

int TrdSCalibR::InitNewTrdSCalib(int CalVer, int TrdTrackType, int GainMethod, int AlignMethod, int Debug) {

  SCalibLevel    = CalVer;
  TrdTrackLevel  = TrdTrackType;
  TrdGainMethod  = GainMethod;
  TrdAlignMethod = AlignMethod;  
  iFlag          = 3;

  pPath = getenv("AMSDataDir");
  if (pPath!=NULL) 
    std::cout << Form("AMSDataDir is: %s",pPath) << std::endl;


  int thread=0;
#ifdef _OPENMP
  thread=omp_get_thread_num();
#endif
  
  std::cout << Form("TrdSCalibR::InitNewTrdSCalib-I- Initiate ... TrdCalibLevel = %d   ", SCalibLevel )
	    << Form("TrdGainMethod=%d TrdAlignMethod=%d TrdTrackType= %s thread %i  ###", 
		    TrdGainMethod, TrdAlignMethod, TrdTrackTypeName[TrdTrackLevel], thread )
	    << std::endl;

  bool ok=true;
  ok=Init(SCalibLevel,Debug);

  if( !ok ) {
    cerr << "Error in TrdSCalib Initialization" << endl;
    return 1;
  }
 

  return 0;
}
//--------------------------------------------------------------------------------------------------

int TrdSCalibR::InitTrdSCalib(int CalVer, int TrdTrackType, int Debug) {

  SCalibLevel   = CalVer;
  TrdTrackLevel = TrdTrackType;
  iFlag         = 3;

  pPath = getenv ("AMSDataDir");
  if (pPath!=NULL) std::cout << Form("AMSDataDir is: %s",pPath) << std::endl;

  int thread=0;
#ifdef _OPENMP
  thread=omp_get_thread_num();
#endif
  
  std::cout << Form("TrdSCalibR::InitTrdSCalib-I- Initiate ... TrdCalibLevel = %d   ", SCalibLevel )
	    << Form("TrdTrackType  = %s  iFlag=%d thread %i  ###", 
		    TrdTrackTypeName[TrdTrackLevel], iFlag, thread )
	    << std::endl;
 
  bool ok=true;
#pragma omp single
  ok=Init(SCalibLevel, Debug);

  if( !ok ) {
    cerr << "Error in TrdSCalib Initialization" << endl;
    return 1;
  }
 

  return 0;
}
//--------------------------------------------------------------------------------------------------

TrdSCalibR::~TrdSCalibR(){
  
  if(fTrdSigmaDx) delete fTrdSigmaDx;
  if(fTrdSigmaDy) delete fTrdSigmaDy;
  if(fTrd95Da)    delete fTrd95Da;
 
  for(int i=0; i< h_TrdGasCirMPV.size();i++) delete h_TrdGasCirMPV[i];
  for(int i=0; i< h_TrdModuleMPV.size();i++) delete h_TrdModuleMPV[i];
  for(int i=0; i< g_TrdCalibMPV.size();i++)  delete g_TrdCalibMPV[i];
  for(int i=0; i< h_TrdLR_Prot.size() ;i++)  delete h_TrdLR_Prot[i];
  for(int i=0; i< h_TrdLR_Elec.size();i++)   delete h_TrdLR_Elec[i];
  for(int i=0; i< h_TrdLR_Heli.size();i++)   delete h_TrdLR_Heli[i];
 
  TrdPDF_xProt.clear();
  TrdPDF_nProt.clear();
  TrdPDF_xHeli.clear();
  TrdPDF_nHeli.clear();
  TrdPDF_nElec_Xe0.clear();
  TrdPDF_nElec_Xe1.clear();
  TrdPDF_nElec_Xe2.clear(); 
  TrdPDF_nElec_Xe3.clear();
  TrdPDF_nElec_Xe4.clear();
  TrdPDF_nElec_Xe5.clear();

  TrdS_PDF_xProt.clear();
  TrdS_PDF_xHeli.clear();
  TrdS_PDF_xElec.clear();
  
  for(int i=0; i< TrdLR_Gr_Prot.size();i++)  delete TrdLR_Gr_Prot[i];
  for(int i=0; i< TrdLR_Gr_Elec.size();i++)  delete TrdLR_Gr_Elec[i];
  for(int i=0; i< TrdLR_Gr_Heli.size();i++)  delete TrdLR_Gr_Heli[i];

  for(int j=0; j<nBinfPXe; j++) {
    for(int i=0; i< grTrdS_PDF_Prot[j].size();i++) delete grTrdS_PDF_Prot[j][i];
    for(int i=0; i< grTrdS_PDF_Heli[j].size();i++) delete grTrdS_PDF_Heli[j][i];
    for(int i=0; i< grTrdS_PDF_Elec[j].size();i++) delete grTrdS_PDF_Elec[j][i];

    for(int i=0; i< fTrdLR_fProton[j].size();i++)  delete fTrdLR_fProton[j][i];
    for(int i=0; i< fTrdLR_fHelium[j].size();i++)  delete fTrdLR_fHelium[j][i];
    for(int i=0; i< fTrdLR_fElectron[j].size();i++) delete fTrdLR_fElectron[j][i];
  }

  if(grTrdS_Xe) delete grTrdS_Xe;
      
  TrkXcors.clear();
  TrkYcors.clear();
  
  for(int i=0; i< TrdNHits.size();i++) delete TrdNHits[i];
  for(int i=0; i< TrdSHits.size();i++) delete TrdSHits[i];
  
}
//--------------------------------------------------------------------------------------------------
