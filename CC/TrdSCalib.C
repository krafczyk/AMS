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
    "/afs/cern.ch/exp/ams/Offline/AMSDataDir/DataBase",
    "/afs/ams.cern.ch/user/chchung/CAL"
  };

const char *TrdSCalibR::TrdCalDBUpdate[] =
  {
    "TrdGasCirCalibration_v02.root",
    "TrdModuleCalibration_Calib1_v09.root"
  };

const char *TrdSCalibR::TrdElectronProtonHeliumLFname[] =
  {
    /// version 1: separate LFs
    "TrdElectronLF_v08.root", "TrdProtonLF_v08.root", "TrdHeliumLF_v08.root", 
    /// version 2: combined LF 
    "TrdLikelihood_v09.root"
  };

const char *TrdSCalibR::TrdLLname[] =
  {
    "Likelihood(Electron/Proton)", "Likelihood(Helium/Proton)", "Likelihood(Electron/Helium)"
  };

const char *TrdSCalibR::TrdSum8name[] = {"TrdSum8Raw", "TrdSum8New"};

//--------------------------------------------------------------------------------------------------

void AC_TrdHits::PrintTrdHit(Option_t *) const{
  std::cout << Form("Lad=%5d Lay=%5d Tub=%5d Mod=%5d Straw=%d Man=%5d ",
	       Lad, Lay, Tub, Mod, Straw, Man);
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

void TrdSCalibR::GenmStrawMatrix() {
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
bool TrdSCalibR::MatchingTrdTKtrack(float P, int Debug){
  double TrdTkD  = TMath::Sqrt(TMath::Power(cTrk.x()-cTrd.x(),2)+
			   TMath::Power(cTrk.y()-cTrd.y(),2)+
			   TMath::Power(cTrk.z()-cTrd.z(),2));

  double TrdTkDx = cTrk.x() - c0Trd.x() + trdconst::TrdOffsetDx;
  double TrdTkDy = cTrk.y() - c0Trd.y() + trdconst::TrdOffsetDy;
  double SigmaTrdTrackerX = fTrdSigmaDx->Eval(TMath::Abs(P));
  double SigmaTrdTrackerY = fTrdSigmaDy->Eval(TMath::Abs(P));
  double TrdTkDr 	  = TMath::Sqrt( TMath::Power(TrdTkDx/SigmaTrdTrackerX,2) + 
			       TMath::Power(TrdTkDy/SigmaTrdTrackerY,2) );

  double CutTrdTkDa       = 10.0*fTrd95Da->Eval(TMath::Abs(P));

  double TrdTkDa = dTrk.x()*(-1*dTrd.x()) +  dTrk.y()*(-1*dTrd.y()) +  dTrk.z()*(-1*dTrd.z());
  TrdTkDa        = TMath::ACos(TrdTkDa)*180/TMath::Pi();

  if(Debug)
    std::cout << Form("MatchingTrdTk: D=%.2f Ds =%.2f CutDa=%.2f Da= %.2f ", 
		      TrdTkD, TrdTkDr, CutTrdTkDa, TrdTkDa) << std::endl;
  if(TrdTkD >3. || TrdTkDa > CutTrdTkDa) return false;
  
  return true;
}
//--------------------------------------------------------------------------------------------------
void TrdSCalibR::ReadLFname(void){
  for(int i = 0; i < 3; i++)
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

int TrdSCalibR::GetModCalibTimePeriod(TString fname, int Debug) {
	
  TFile *input = TFile::Open(fname);
  if (!input->IsOpen()) {  
    std::cout << "GetCalibTimePeriod: No " << fname << std::endl;
    return -1;
  }
  
  TH1D *h_Nevts = (TH1D*) input->Get("h_ModMPV_26"); /// center bottom module
  if(h_Nevts->GetNbinsX() == 0) return -2;

  int nBin = h_Nevts->GetNbinsX();
  for (int iBin=1; iBin<=nBin; iBin++) 
    {
      unsigned nEvt   = h_Nevts->GetBinContent(iBin);
      unsigned iTime  = h_Nevts->GetBinCenter(iBin);
      if(nEvt == 0) continue;
      
      if(Debug) 
	std::cout << Form("GetCalibTimePeriod iBin=%4u nEvt=%10u", iBin,nEvt) << std::endl;
      
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

  //h_TrdModuleMPV = new vector<TH1F*>();

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
  
  double distanceLine = fabs(s)*sqrt(r_denomenator);
  
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
	sprintf(tdname, "%s/%s", TrdDBUpdateDir[1], TrdCalDBUpdate[0]);
	TrdCalib_01 = Get01TrdCalibration(tdname, Debug);
      }
    if (CalibLevel>1) 
      {
	sprintf(tdname, "%s/%s", TrdDBUpdateDir[1], TrdCalDBUpdate[1]);
	TrdCalib_02 = Get02TrdCalibration(tdname, Debug);

	/// check module calibration time period
	int ct = GetModCalibTimePeriod(tdname, Debug);
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
		
void TrdSCalibR::GetBinGasCirModule(int hTime, int CalibLevel, int iCir, int iMod, int Debug) {
  	
  iBinGasCir 	= 0;
  iBinModule 	= 0;

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

void TrdSCalibR::TrdLR_CalcIni(int Debug) {

  FirstLLCall = true;

  std::string hname;
  
  char fname[100];

  if (FirstLLCall) {
    /// electron parameters
    sprintf(fname, "%s/%s", TrdDBUpdateDir[1],TrdElectronProtonHeliumLFname[0]); 
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
    sprintf(fname, "%s/%s", TrdDBUpdateDir[1],TrdElectronProtonHeliumLFname[1]); 
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
    sprintf(fname, "%s/%s", TrdDBUpdateDir[1],TrdElectronProtonHeliumLFname[2]); 
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

vector<double> TrdSCalibR::TrdLR_Calc(float P, vector<AC_TrdHits*> TrdHits, int iFlag, int Debug) {

  vector<double> TrdLR;
  TrdLR.assign(3,-1.0);
  
  /// Which Momentum Bin for Protons & Helium ?
  int iP     = -1;
  int iMax   = TrdLR_xProt.size()-1;
  for (unsigned int i=0; i<iMax; i++) {
    if (P>=TrdLR_xProt.at(i) && P<TrdLR_xProt.at(i+1)) {
      iP = i;
      break;
    }
  }
  
  if(Debug) std::cout << "Pmin= " << TrdLR_xProt.at(0) << " Pmax= " << TrdLR_xProt.at(iMax) << std::endl;

  if (P>=TrdLR_xProt.at(iMax)) 	iP = iMax-1;
  if (P< TrdLR_xProt.at(0))     iP = 0;
  
  if (iP<0) {
    std::cout << "Error in TrdLR_Calc: invalid momentum P=" << P << std::endl;
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
    std::cout << Form("Step 1. P=%6.2f(GeV/c) Lprod_Electron=%12.4E Lprod_Helium=%12.4E Lprod_Proton=%12.4E", 
		      P, Lprod_Electron, Lprod_Helium, Lprod_Proton) << std::endl;
  
  
  Lprod_Proton 		= TMath::Power(Lprod_Proton,  1.0/float(nCount));
  Lprod_Helium 		= TMath::Power(Lprod_Helium,  1.0/float(nCount));
  Lprod_Electron 	= TMath::Power(Lprod_Electron,1.0/float(nCount));

  if(Debug > 1) 
    std::cout << Form("Step 2. P=%6.2f(GeV/c) Lprod_Electron=%12.4E Lprod_Helium=%12.4E Lprod_Proton=%12.4E", 
		      P, Lprod_Electron, Lprod_Helium, Lprod_Proton) << std::endl;
  
  
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
    TrdLR_nProt.at(iP) = fTrdLR->Integral(TrdMinAdc,TrdMaxAdc);

    vector<double> vecAdc, vecLR;
    vector<double> xMin = GenLogBinning(1000,0.1,TrdMaxAdc);
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
    TrdLR_nHeli.at(iP) = fTrdLR->Integral(TrdMinAdc,TrdMaxAdc);

    vector<double> vecAdc, vecLR;
    vector<double> xMin = GenLogBinning(1000,0.1,TrdMaxAdc);
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
    TrdLR_nElec.at(Layer) = fTrdLR->Integral(TrdMinAdc,TrdMaxAdc);
    vector<double> vecAdc, vecLR;
    vector<double> xMin = GenLogBinning(1000,TrdMinAdc,TrdMaxAdc);
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

  FirstLLCall = true;
  
  std::string hname;
  char aname[100];
  char fname[100];
  if (FirstLLCall) {
    /// check root file
    sprintf(fname, "%s/%s", TrdDBUpdateDir[1],TrdElectronProtonHeliumLFname[3]);
 
    TFile *input = new TFile(fname);
    if (!input->IsOpen()) {
      std::cerr << "Error: Open Trd Likelihood root file " << fname << std::endl;
      return false;
    }
    
    if (Debug>1)  std::cout << "Trd LiklihoodFunctions read in from " << fname << std::endl;
    TH2D* h_TrdProt  = (TH2D*) input->Get("h_TrdProt");
    TH2D* h_TrdElec  = (TH2D*) input->Get("h_TrdElec");
    TH2D* h_TrdHeli  = (TH2D*) input->Get("h_TrdHeli");
    TH1D* h_Norm     = (TH1D*) input->Get("h_Norm");
    
    vector<TH1D*> h_TrdLR_Prot, h_TrdLR_Elec, h_TrdLR_Heli;
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
      TrdLR_nProt.at(i) = fTrdLR->Integral(TrdMinAdc,TrdMaxAdc);
    }
    
    //------------------- Electrons -----------------------------------------------------------
    TrdLR_xElec 	= RootGetYbinsTH2(h_TrdElec, Debug);
    nBinY 			= h_TrdElec->GetNbinsY();
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
      TrdLR_nElec.at(i) = fTrdLR->Integral(TrdMinAdc,TrdMaxAdc);
    }
    
    //------------------- Helium -----------------------------------------------------------
    TrdLR_xHeli 	= RootGetYbinsTH2(h_TrdHeli, Debug);
    nBinY 			= h_TrdHeli->GetNbinsY();
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
      TrdLR_nHeli.at(i) = fTrdLR->Integral(TrdMinAdc,TrdMaxAdc);
    }
    FirstLLCall = false;
  }
  
  return true;
}

//--------------------------------------------------------------------------------------------------

vector<double> TrdSCalibR::TrdLR_Calc_v02(float P, vector<AC_TrdHits*> TrdHits, int iFlag, int Debug) {

  vector<double> TrdLR;
  TrdLR.assign(3,-1.0);
  
  // Which Momentum Bin for Protons & Helium ?
  int iP 	= -1;
  int iMax 	= TrdLR_xProt.size()-1;
  for (unsigned int i=0; i<iMax; i++) {
    if (P>=TrdLR_xProt.at(i) && P<TrdLR_xProt.at(i+1)) {
      iP = i;
      break;
    }
  }
  if (P>=TrdLR_xProt.at(iMax)) 	iP = iMax-1;
  if (P< TrdLR_xProt.at(0)) 	iP = 0;
  
  if (iP<0) {
    std::cerr << "Error in TrdLR_Calc: invalid momentum P=" << P << std::endl;
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
    double	  Len   = (*iter)->Len2D;
    if (iFlag==3) Len   = (*iter)->Len3D;
    if (Len < 0.05) continue;
    double	scale 	= 2.0*trdconst::TrdStrawRadius/Len;
    double 	Eadc	= (*iter)->EadcCS*scale;
    
    x[0] 		= Eadc;
    par[0] 	        = iP;	
    myPair <double> myProtonL  (1E-8, TrdSCalibR::TrdLR_fProton_v02(x,par));
    myPair <double> myHeliumL  (1E-8, TrdSCalibR::TrdLR_fHelium_v02(x,par));
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
    std::cout << Form("Step 1. P=%6.2f(GeV/c) Lprod_Electron=%12.4E Lprod_Helium=%12.4E Lprod_Proton=%12.4E", 
		      P, Lprod_Electron, Lprod_Helium, Lprod_Proton) << std::endl;

  Lprod_Proton 		= TMath::Power(Lprod_Proton,  1.0/float(nCount));
  Lprod_Helium 		= TMath::Power(Lprod_Helium,  1.0/float(nCount));
  Lprod_Electron 	= TMath::Power(Lprod_Electron,1.0/float(nCount));

  if(Debug > 1) 
    std::cout << Form("Step 2. P=%6.2f(GeV/c) Lprod_Electron=%12.4E Lprod_Helium=%12.4E Lprod_Proton=%12.4E", 
		      P, Lprod_Electron, Lprod_Helium, Lprod_Proton) << std::endl;

   
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
