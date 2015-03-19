
//! Ecal3DRec class

/*!
 * Class for 3D reconstruction of ECAL Shower
 * Based on shower parameterization in Longitudinal and Transverse shape
 *
 * Input:   AMSEvent
 * Output:  Energy/Axis/EM Likelihood
 * \author: Hai Chen(hachen@cern.ch), Andrei Kounine, Zuhao Li, Weiwei Xu, Cheng Zhang
*/

#ifndef __ECAL3DREC__
#define __ECAL3DREC__

#include <iostream>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <TMath.h>
#include <TString.h>
#include <TChain.h>
#include <TFile.h>
#include <TTree.h>

#include "EcalKDbc.h"

using namespace std;
using namespace Ecal3DRecDB;

class Ecal3DRec:public TObject
{

	//! Functions
	public:
		//! A Constructor
		Ecal3DRec(){};
		//! A Destructor
		~Ecal3DRec(){};

	public:
		//===================================================
		//! Init the Edep 18x72 map after attenuation correction
		int 		 SetCluster(float ene_cell[][72], int cellstat[][72], bool IsGeV=1);
		//! Reset the Edep map
		int 		 Reset();
		//! Set the cell status for dead cells
		int		   SetCellStat();

	public:
		//===================================================
		//! The function that all reconstruction is performed
		int		   ShowerRec(int n=0); 

		//=======================
		//! Estimate the shower energy and other parameters
		int		   ShowerEstimate();
		//! sub ShowerEstimate: to recognize a good shower
		int      ShowerClustering();
		//! sub ShowerEstimate: to calculate Center of Gravity by layer
		int		   CalcCoG();
		//! sub ShowerEstimate: to get shower maximum position
		int      ParabolaZ0();
		//! sub ShowerEstimate: to calculate shower axis
		int      ShowerAxis();
		//! sub ShowerAxis:     to perform linear fit
		void 		 LinearFit(int n, vector<double> vx, vector<double> vy, vector<double> vw, double &kx, double &x0);
		//! sub ShowerEstimate: to define the shower footprint and get energy estimate
		int      DefineFootprint();

		//=======================
		//! Set the shower shape parameters by energy
		void 		 SetLatPar(int ish, double ene);
		//! Set the shower fluctuation
		void     SetPdfPar(int ish, double ene);

		//=======================
		//! Perform shower fit by given parameter
		int		   TransFit(double &par, double &par_err, double &L0, double &L1, double &L2);

		//! Get Expected energy deposition map from shower model
		int		   ShowerModel(int index=0, int flag=0);
		//! sub ShowerModel:   to calculate energy deposition from certain depth and distance from axis
		double	 ShowerShape(int slay, double depth, double dist);
		//! sub ShowerShape:   to calculate light output from shower position
		double	 ShowerShapeIdeal(double depth, double dist, double rc, double rt, double fc);

		//! Get the likelihood from the shower model
		/*!
 		  L1: likelihood from cell fluctuation
			L2: likelihood from boundary conditions
		*/
		double	 GetLkhd(double &L1, double &L2);
		//! The probability to be EM shower accroding to Poisson stat, for measured and expected Edep Map
		int 	   ShowerProbability();

		//! Get the Parabola minimum in the TransFit
		int		   ParabolaMin(int n1, int n2, double param[], double lkhd[], double &l1, double &par, double &err);

		//=======================
		//! Define new shower footprint to extend
		int      ShowerFootprint();

	public:
		//===================================================
		//! Combine the shower fit of overlapped showers
		int		   ShowerCombineFit();

		//! Transverse Fit in X projection
		int		   TransFitKx(double &L0, double &L1, double &L2);
		//! Transverse Fit in Y projection
		int		   TransFitKy(double &L0, double &L1, double &L2);
		//! Assign shower parameters in order
		int		   EcalShwrDef(int k, int flag=0);

	public:
		//===================================================
		// Summary
	   void     Summary(); // place to calculate output variables related to likelihood

		//=======================
		//! Energy Reconstruction
		//! EM Energy from expected model, using RearLeak Correction
		void     EMEnergy();
		//! sub EMEnergy:  to get the Last 2 Layers Rear Leak Correction
		double   GetRearLeakageL2L(double totene, double l2ene);
		//! Shower Energy by particle type !!! still empty
		double   ShowerEnergy(int pid=0);
		//! Electron Energy
		double   ElectronEnergy();
		//! Hadron Energy
		double   ProtonEnergy();
		//! Photon Energy
		double   PhotonEnergy();

		//=======================
		//! Likelihood Reconstruction
		//! Assign Likelihood variables
		void     EMEstimatorVars();
		//! Combine Likelihood and reduce energy dependence
		void     EMEstimatorLkhd();

	public:
		//! sub ShowerShapeIdeal
		//! First call or not
		static int    GammaFast_FirstCall;
		//! Fast Gamma fuction evaluation
		static double GammaFast(double depth, double t0);

	public:
		//! sub ShowerModel
		//! Get the Attenuation correction for expected shower
		static double getCorrEs2Ed(double depth, double dist, double x, double kx, double beta, int flag=0);
		static double getCorrEd2Ei(double depth, double dist, double x, double kx, double beta, int flag=0);
		static double getKyCorrEs2Ed(double depth, double dist, double kx, double ky, int flag=0);


	//! Variables
	public:

		//===================================================
		//-->from SetCluster()

		//! Edep Map, 4 fake cells on each side, 4 + 72 + 4
		double  CellDep[kNL][kNC];
		//! Cell Status: dead cell/wrong pedestal/sataration
		int     CellStat[kNL][kNC];
		//! Cell Flag, 1 means fake cell, 0 means real cell
		short   CellFlag[kNL][kNC];
		//! Cell Mask, 1 means bad cell for any reason, not used in fit and energy calculation
		bool    CellMask[kNL][kNC];
		//! Edep after subtract identified showers
		double  CellDepS0[kNL][kNC];
		//! Edep after subtract identified showers + 2 sigma fluctuation
		double  CellDepS1[kNL][kNC];

	public:
		//===================================================
		//-->from ShowerEstimate()

		//-->from ShowerClustering()
		//! Seed in each layer
		int     SeedCell[kNL];
		//! Seed in X/Y Projection
		int		  XSeed, YSeed;

		//! Maximum Edep in X/Y Projection
		double  dEX,
					  dEY;
		//! Number of X/Y cells associated with shower footprint recognition
		int     fEX,
					  fEY;

		//===================================================
		//-->from CalcCoG()
		//! Amplitude in each layer
		double  LayAmp[kNL];
		//! Coordinate in each layer
		double  LayCrd[kNL];

		//! Number of X/Y layers with good local seed, based on subtracted deposition
		int		  fX0,
					  fY0;
		//! Number of X/Y layers with good local seed, based on original deposition
		int		  fX1,
					  fY1;

		//===================================================
		//-->from ParabolaZ0()
		//! Z coordinate of shower maximum
		double  Z0, dZ0;
		//! Shower Starting point, based on energy deposited in 3 adjacent cells
		int     Apex;

		//===================================================
		//-->from ShowerAxis()
		//! Axis KX/KY
		double  KX, dKX,
					  KY, dKY;
		//! Shower maximum X/Y Coordinate
		double  X0, dX0,
					  Y0, dY0;

		//===================================================
		//-->from DefineFootprint()
		//! Footprint min/max range
		int     Cell_Min[18],
					  Cell_Max[18],
					  Cell_Med[18];
		//! Axis cell for each shower each layer
		int     Shwr_Med[kNS][18];
		//! Total energy in X/Y proj footprint for fit
		double  EX,
					  EY;
		//! Center of gravity in X/Y proj	
		double  X1,
					  Y1;
		//! Last 2 Layers energy deposition for rear leakage
		double  El2f;

		//! Shower par: Energy
		double  E0, dE0;
		//! Shower par: Beta
		double  B0, dB0;
		//! Shower par: T0-distance between apex and maximum
		double  T0, dT0;
		//! Shower par: Z coordinate of apex
		double  A0, dA0;

	public:
		//===================================================
		//-->from SetLatPar() and SetPdfPar()

		//! Transverse shape parameters as a function of t/T0
		//! Core radius/tail radius/core fraction in X
		double  Rcx[3][30],
					  Rtx[3][30],
					  Fcx[3][30];
		//! Core radius/tail radius/core fraction in Y
		double  Rcy[3][30],
					  Rty[3][30],
					  Fcy[3][30];

		//! Transverse fluctuation parameters
		double  P0x[3][30], P1x[3][30], P2x[3][30];
		double  P0y[3][30], P1y[3][30], P2y[3][30];

		//===================================================
		//-->from ShowerRec()
		//! Total number of showers
		int     N_Shwr;
		//! Index of current shower
		int 	  I_Shwr;
		int     ShwrDep[kNS][2];
		//! Shower Footprint: [iShower][iLayer][min/max]
		int     ShwrFprt[kNS][kNL][2];
		//! Shower apex array
		int     ShwrApexI[kNS];
		//! Shower parameters and err
		double  ShwrPar[kNS][kNpar][2];
		//! Shower parameters from single fit
		double  ShwrParI[kNS][kNpar][2];

	public:
		//===================================================
		//-->from TransFit()

		//! Correction for inclination, attenuation, lateral leakage
		//! from simplified model to real 3d model
		static bool  is_cf_es2ed_loaded;
		static float cf_es2ed[3628800];
		static bool  is_cf_ed2ei_loaded;
		static float cf_ed2ei[3628800];
		static bool  is_cf_ky_es2ed_loaded;
		static float cf_ky_es2ed[25200];

		//-->from ShowerModel()
		//! Expected Edep with all effects applied(PMT, Att...)
		double  CellExp[kNL][kNC];
		//! Ideal energy deposition	
		double  CellEtr[kNL][kNC];
		//! Energy weighted depth
		double  CellDepth[kNL][kNC];
		//! Energy weighted distance to axis
		double  CellDist[kNL][kNC];
		//! Conversion factor
	  double  CellBeta[kNL][kNC];
		//! Alpha=B0*T0 as shower parameter
		double  Alpha, GammaAlpha;

		//! Cell probabilities
		double  CellProb[kNL][kNC];
		//! Substract reference probabilities
		double  CellProbNorm[kNL][kNC];
		//! Cell Edep after correction
		double  CellDepCor[kNL][kNC];

		//--> Buffer to handle multiple showers, at most 3 showers will be reconstructed
		//! Expected energy of given shower
		double  ShwrExp[kNS][kNL][kNC];
		//! Ideal energy of given shower
		double  ShwrEtr[kNS][kNL][kNC];
		//! Energy weighted depth
		double  ShwrDepth[kNS][kNL][kNC];
		//! Energy weighted distance to axis
		double  ShwrDist[kNS][kNL][kNC];
		//! Conversion factor for each shower
		double  ShwrBeta[kNS][kNL][kNC];

		//! Likelihood boundary conditions
		double  ShwrLkhd[kNS];
		//! Likelihood cells fluctuations
		double  ShwrLkhdI[kNS];
		//! Boundary conditions Energy E0/dE0 in ShowerCombineFit()
		double  ShwrEb[kNS][2];

		//! Most energetic shower from Fitting
		double  E1;

	public:

		//===================================================
		//-->from EMEstimatorVars()
		//! De-correlated energy in L0 and L1
		double  R0, R1;
		//! Likelihood along shower axis, in the core/tail region
		double  LkhdAxis, 
					  LkhdCore,
					  LkhdTail;
		//! Neutron energy in tail region
		double  LkhdBump;

		//! Characteristic size of X/Y projection
		double  SizeX,
					  SizeY;

		//! Total energy dep along axis/core/tail/bumps
		double  DepAxis,
						DepCore,
						DepTail,
						DepBump;

		//! Expected energy dep along axis/core/tail/bumps
		double  ExpAxis,
					  ExpCore,
					  ExpTail,
					  ExpBump;
		
		//! Likelihood per layer/normalized per layer
		double  LayLkhd[kNL],
					  LayLkhdReNorm[kNL];

		//! Real/Expected energy dep per layer 
		double  LayDep[kNL],
					  LayExp[kNL];
		//! Characteristic size per layer
		double  LaySize[kNL];

		//! Number of good cells in each layer
		int     LayNhit[kNL];
		//! Number of cells in apex/core/tail/bump regions
		int     NhitAxis,
					  NhitCore,
					  NhitTail,
					  NhitBump;

		//! All cells in X/Y proj
		double  Sx,
					  Sy;

		int     NhitAll;
		double  EdepAll;

	public:
		//===================================================
		//-->from ShowerCombineFit()
		//! Count of neighboring showers: 10*x_dep + y_dep
		int		 N_Dep;

		//! Normalized EM variables
		double EmVar[10];

		//! final EM likelihood
		/*! 0->integral value, 1->layer value combined */ 
		double EmLkhd[2];
	
		//! Reconstructed energy for electrons 
		double EleEne;

		//! Total energy corrected by lateral leak
		double TotEne;
		//! Raw energy without correction
		double TotDep;
		//! Last two layer energy
		double L2Ene; 

		//! Total energy of saturated cells
		double EneSat;
		//! Number of saturated cells
		int    NhitSat;


		//! Energy dep around shower axis for layer 0/1/2
		double E0M,
					 E1M,
					 E2M;

		ClassDef(Ecal3DRec,1);
};
#endif
