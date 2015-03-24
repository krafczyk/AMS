#ifndef _ECALFASTREC_
#define _ECALFASTREC_

//! EcalFastRec class

/*!
 * Class for fast reconstruction of ECAL Shower
 * Two corrections applied:
 * 		1. Signal time dependent calibration
 * 		2. Attenuation correction
 * Input:   AMS Event
 * Output:  Edep Map, shower energy estimation
 * 
 * Usage:
 * 		AMSEventR *pev=ch->GetEvent();
 * 		EcalFastRec *pecalfast=new EcalFastRec();
 * 		pecalfast->DoFastShowerRec(pev);
 *		
 *		pecalfast->GetEdep(layer,cell);  //return cell edep
 *		pecalfast->GetEnergyEstimate();  //return shower energy
 *
 * \author: Hai Chen(hachen@cern.ch), Andrei Kounine, Zuhao Li, Weiwei Xu, Cheng Zhang
*/

#include <iostream>
#include "TString.h"
#include "TMath.h"
#include "EcalKDbc.h"
#include <root.h>

using namespace Ecal3DRecDB;

enum EnumDataType{
	ISSDATA,   // ISS data
	BTDATA,    // Beam test data
	REALMC,    // Real MC simulation
	TOYMC      // Toy MC simulation
};

enum EnumCellStatType{

	DEADCELL=1,      // Dead cell
	PEDESTALERROR=2, // Pedestal error
	HLGAINERROR=4,   // Gain ratio error
	HIGHGAINSATU=8,  // High gain saturation
	LOWGAINSATU=16,  // Low gain saturation

	USED=32,         // Associated with cluster
	OVERLAP=64,      // Overlap between 2 clusters
	ORPHAN=128       // Not associated with cluster
};

enum EnumCellMaskType{
	FAKCELL, // Fake cell, used only for transversal leakage correction
	BADCELL, // Bad cell, due to hardware failure
	SATCELL  // Low gain saturated cell
};

class EcalFastRec{

//! Functions
	public:
		//! A Constructor
		EcalFastRec(){};

	public:
		//! Interface to Perform Fast Reconstruction
		int   DoFastShowerRec(AMSEventR *pev);

		//! Reset variables
		int   Reset();
		//! Read ADC map from AMSEvent
		int   ReadADCMapFromAMSEvent(AMSEventR *pev);
		//! Read ADC map from Toy Event
		int   ReadADCMapFromToyEvent( Double_t _time, Float_t _edep[][nCell], Int_t _stat[][nCell] );
		//! Define Cell Status
		int   DefineCellStatus();

	public:
		//! Initial TDV
		int   InitDataBase(Double_t asktime);
		//! Signal Calibration
		int   GainCalibration();
		float GetHighGainCorr(int layer, int cell);
		float GetGainRatioCorr(int layer, int cell);
		
		//! Clear Cell Status
		int   ClearCellStatus(Int_t l, Int_t c, Int_t stat_ask);
		//! Check Cell Status
		int   CheckCellStatus(Int_t l, Int_t c, Int_t stat_ask);
		//! Clear Cell Status
		int   ClearAllCellStatus(Int_t stat_ask);

		//! Shower recognition
		int   ShowerSeedFinding(Int_t iter=0);
		int   ShowerClustering(Int_t iter=0);
		int   CalculateLayerCOG(Int_t iter=0); // Axis reconstruction, 0-> COG, 1->Cell ratio
		int   ParabolaZ0Fit();
		int   ShowerAxisFit();
		int   LinearFit(Int_t n, vector<Double_t> vx, vector<Double_t> vy, vector<Double_t> vw, Double_t &kx, Double_t &x0);

		//! Attenuation correction
		int   AttenuationCorrection();
		int   GetAttenuationCorr(int layer, int cell, float attpar[3]);

		//! Attenuation correction for isolated hits
		int   IsolatedHitsAttCor();

		//! Get Energy Estimation
		float GetEnergyEstimate();
		//! Do Rear Leakage correction
		float GetRearLeakCorr();

//! Data accessor
	public:
		//! Data accessor:Calibrated Edep
		float  GetEdep(Int_t lay, Int_t cell){ return Edep[lay][cell]; }
		//! Data accessor:Calibrated and Attenuation corrected Edep
		float  GetEdepAttC(Int_t lay, Int_t cell){ return EdepAttC[lay][cell]; }

//! Data members
	public:
		EcalHitR *phit;

		int    DataType;
		double Time;

		//-->Hit Level
		float  ADCH[kNL][nCell], // High gain ADC
					 ADCL[kNL][nCell], // Low gain ADC
					 PedH[kNL][nCell], // High gain pedestal
					 PedL[kNL][nCell]; // Low gain pedestal

		//! Cell Status
		int    Status[kNL][nCell];
		
		//! Calibrated Edep
		float  Edep[kNL][nCell];
		//! Calibrated and Attenuation Corrected Edep
		float  EdepAttC[kNL][nCell];
		//! Edep Buffer
		float  EdepBuffer[kNL][nCell]; // Buffer used for shower clustering

		//! Coordinate and Amplitude per layer
		float  LayCrd[kNL],
					 LayAmp[kNL];
		//! Seed Cell per layer
		int    SeedCell[kNL]; // Seed cell id in each layer
		//! Footprint range per layer
		int    Cell_Min[kNL],
					 Cell_Max[kNL];

		//! Shower maximum position and error
		double Z0, X0, Y0;
		double dZ0, dX0, dY0;
		//! Shower axis and error
		double KX, KY; 
		double dKX, dKY;
		//! Number of Shower
		int    N_Shwr;

		//! Shower parameters
		float  EX,		// total energy in x-projection footprint for fit
					 EY,		// total energy in y-projection footprint for fit
					 dEX,		// maximum cell energy in x-projection
					 dEY,		// maximum cell energy in y-projection
					 X1,		// center of gravity in X 
					 Y1;		// center of gravity in Y

		int    fEX;		// number of X cells associated for footprint recognition
		int    fEY;		// number of Y cells associated for footprint recognition
		int		 fX0;		// number of X layers with good local seed, with subtracted deposition
		int		 fY0;  	// number of Y layers with good local seed, with subtracted deposition
		int		 fX1;		// number of X layers with good local seed, with original deposition
		int 	 fY1;  	// number of Y layers with good local seed, with original deposition

		int    Nhits;

		//! Buffer of shower parameters
		int    ShwrCellMin[4][kNL], ShwrCellMax[4][kNL];
		int    ShwrNhits[4];
		float  ShwrZ0[4], ShwrX0[4], ShwrY0[4];
		float  ShwrKX[4], ShwrKY[4];

		//! Energy and Rear leakage correction
		float  E0;
		float  EL2L;

		ClassDef(EcalFastRec,1);
};

#endif
