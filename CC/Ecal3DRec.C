
//! Ecal3DRec class

/*!
 * Class for 3D reconstruction of ECAL Shower
 * Based on shower parameterization in Longitudinal and Transverse shape
 *
 * Input:   AMSEvent
 * Output:  Energy/Axis/EM Likelihood
 * \author: Hai Chen(hachen@cern.ch), Andrei Kounine, Zuhao Li, Weiwei Xu, Cheng Zhang
*/

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
#include "Ecal3DRec.h"

int Index;
int DEBUG=0;

//==========================================================
int Ecal3DRec::SetCluster(float ene_cell[][72], int ecal_cellstat[][72], bool IsGeV){

	Reset();
	SetCellStat();
	NhitSat=0;

	for(int i=0; i<18; i++){
		for(int j=0; j<4; j++){
			CellDep[i][j] = 0;
			CellFlag[i][j] = 4;
			CellMask[i][j] = 1;
			CellDepS0[i][j] = CellDepS1[i][j] = CellDep[i][j] = 0;
		}
		for(int j=4; j<76; j++){
			if( !IsGeV )
				ene_cell[i][j-4] *= 1e-3;

			CellDepS0[i][j] = CellDepS1[i][j] = CellDep[i][j] = ene_cell[i][j-4];
			CellFlag[i][j] = 0;
			CellMask[i][j] = 0;

			//Set Mask for bad Cell, Ped wrong Cell and high gain Satuated Cell
			//j-4 because there is 4 fake cell., stat_cell ==1 bad, 2,Ped wrong, 4 h/l wrong
			//16 Low Gain Saturation
			//			if((CellStat[i][j-4]&7)>0 || (CellStat[i][j-4]&16)>0) CellMask[i][j] =1; 
			// ad-hoc check of low gain saturation, need to be improved
			//if( CellDep[i][j] > 40 )
			//			if( ecal_cellstat[i][j-4] & AOVERFLOW ) // use EcalHit status

			if( ecal_cellstat[i][j-4] & 16 ) // use status defined in Zuhao calib, for ISS data
			{
				NhitSat++;
				CellMask[i][j] = 2;
			}
			if( ecal_cellstat[i][j-4] & 7 ){ // bad cell
				CellMask[i][j] = 1;
			}
			if( CellStat[i][j-4] ) CellMask[i][j] = 1; // 2 noisy PMTs
		}
		for(int j=76; j<80; j++){
			CellDep[i][j] = 0;
			CellFlag[i][j] = 4;
			CellMask[i][j] = 1;
			CellDepS0[i][j] = CellDepS1[i][j] = CellDep[i][j] = 0;
		}
	}

	// mask the maximum cell, test the saturation recovery algo.
	int lmax=0, cmax=0;
	double emax = 0;
	for(int l=0; l<18; l++){
		for(int c=0; c<80; c++){
			if( CellDep[l][c] > emax ){
				emax = CellDep[l][c];
				lmax = l;
				cmax = c;
			}
		}
	}

	//	CellMask[lmax][cmax] = 1;
	//	cout << Form("l=%d, c=%d is masked", lmax, cmax) << endl;

	return 0;
}


//==========================================================
//-->Reset the initial values
int Ecal3DRec::Reset(){
	memset(EmVar,       0,    sizeof(EmVar));
	memset(EmLkhd,       0,    sizeof(EmLkhd));
	memset(Cell_Med,     0,    sizeof(Cell_Med));
	memset(Shwr_Med,     0,    sizeof(Shwr_Med));
	memset(CellStat, 		0, 	sizeof(CellStat));
	memset(CellDep, 		0, 	sizeof(CellDep));
	memset(CellDepS0, 	0, 	sizeof(CellDepS0));
	memset(CellDepS1, 	0, 	sizeof(CellDepS1));
	memset(CellExp, 		0, 	sizeof(CellExp));
	memset(CellEtr, 		0, 	sizeof(CellEtr));
	memset(CellDepth, 	0, 	sizeof(CellDepth));
	memset(CellDist, 		0, 	sizeof(CellDist));
	memset(CellBeta, 		0, 	sizeof(CellBeta));
	memset(CellProb, 		0, 	sizeof(CellProb));
	memset(CellProbNorm,	0, 	sizeof(CellProbNorm));

	memset(ShwrDep,      0,    sizeof(ShwrDep));
	memset(ShwrFprt,     0,    sizeof(ShwrFprt));
	memset(ShwrExp,      0,    sizeof(ShwrExp));
	memset(ShwrDepth,    0,    sizeof(ShwrDepth));
	memset(ShwrDist,     0,    sizeof(ShwrDist));
	memset(ShwrEtr,      0,    sizeof(ShwrEtr));
	memset(ShwrBeta,     0,    sizeof(ShwrBeta));
	memset(ShwrPar,      0,    sizeof(ShwrPar));
	memset(ShwrParI,     0,    sizeof(ShwrParI));
	memset(ShwrLkhd,     0,    sizeof(ShwrLkhd));
	memset(ShwrLkhdI,    0,    sizeof(ShwrLkhdI));

	memset(ShwrEb,       0,    sizeof(ShwrEb));
	memset(ShwrApexI,    0,    sizeof(ShwrApexI));

	E0 = dE0 = 0.;
	B0 = dB0 = 0.;
	T0 = dT0 = 0.;
	A0 = dA0 = 0.;
	Z0 = dZ0 = 0.;
	X0 = dX0 = 0.;
	Y0 = dY0 = 0.;
	KX = dKX = 0.;
	KY = dKY = 0.;

	N_Shwr = I_Shwr = N_Dep = 0;
	Apex = 0;

	Z0 = 1;
	EleEne = 0.;
	return 0;
}

//==========================================================
//-->Set Cell Status
//==========================================================
int Ecal3DRec::SetCellStat(){
	memset(CellStat, 0, sizeof(CellStat));
	CellStat[6][16] = CellStat[6][17] = CellStat[7][16] = CellStat[7][17] = 1;    
	CellStat[14][64] = CellStat[14][65] = CellStat[15][64] = CellStat[15][65] = 1;
	return 0;
}


//==========================================================
//-->Reconstruct the shower
//==========================================================

int Ecal3DRec::ShowerRec(int n){

	int    l, m, flag_x, flag_y, Iter;
	double l_start, l_end;

	N_Shwr = n;

	while( N_Shwr < 3 ) {

		//-->Define new shower
		if(ShowerEstimate() == 0) break;

		SetLatPar( N_Shwr, E0 );
		SetPdfPar( N_Shwr, E0 );

		//-->Quality of the new shower
		if      ( (dEX>Cutoff_SEED) && (fX1>=Cutoff_NPL)                      ) flag_x = 2;
		else if ( (dEX>Cutoff_SEED) && (fEX>=Cutoff_NNB) && (fX0>=Cutoff_NPL) ) flag_x = 2;
		else if ( (dEX>Cutoff_SEED) && (fEX>=Cutoff_NNB  ||  fX0>=Cutoff_NPL) ) flag_x = 1;
		else                                                                    flag_x = 0;

		if      ( (dEY>Cutoff_SEED) && (fY1>=Cutoff_NPL)                      ) flag_y = 2;
		else if ( (dEY>Cutoff_SEED) && (fEY>=Cutoff_NNB) && (fY0>=Cutoff_NPL) ) flag_y = 2;
		else if ( (dEY>Cutoff_SEED) && (fEY>=Cutoff_NNB  ||  fY0>=Cutoff_NPL) ) flag_y = 1;
		else                                                                    flag_y = 0;

		//-->Is there a compelling shower?
		if ( N_Shwr==0 && (flag_x<1 && flag_y<1) ) break;
		if ( N_Shwr>0  && (flag_x+flag_y)<2      ) break;

		//-->Define parameters of the new shower
		if ( N_Shwr == 0 ) {
			// no dependences for the first shower
			ShwrDep[N_Shwr][0] = 0;
			ShwrDep[N_Shwr][1] = 0;
		} else {

			// Corrections for not-well-defined showers - X dependences
			if ( flag_x==2 ) {
				if ( TMath::Abs(ShwrPar[0][3][0]-X0) > 3. ) {
					// dependences
					ShwrDep[N_Shwr][0] = 0;
				} else {
					ShwrDep[0][0]      = 1;
					ShwrDep[N_Shwr][0] = 1;
					// KX correction
					KX  = ShwrPar[0][6][0];
					dKX = ShwrPar[0][6][1];
					// combine footprints
					for (l=2; l<18; l+=4) {
						Cell_Min[l]   = TMath::Min( Cell_Min[l],   ShwrFprt[0][l][0]);
						Cell_Max[l]   = TMath::Max( Cell_Max[l],   ShwrFprt[0][l][1]);
						Cell_Min[l+1] = TMath::Min( Cell_Min[l+1], ShwrFprt[0][l+1][0]);
						Cell_Max[l+1] = TMath::Max( Cell_Max[l+1], ShwrFprt[0][l+1][1]);
					}
				}
			} else if ( flag_x==1 ) {
				// dependences
				ShwrDep[0][0]      = 1;
				ShwrDep[N_Shwr][0] = 1;
				// KX correction
				KX  = ShwrPar[0][6][0];
				dKX = ShwrPar[0][6][1];
				// combine footprints
				for (l=2; l<18; l+=4) {
					Cell_Min[l]   = TMath::Min( Cell_Min[l],   ShwrFprt[0][l][0]);
					Cell_Max[l]   = TMath::Max( Cell_Max[l],   ShwrFprt[0][l][1]);
					Cell_Min[l+1] = TMath::Min( Cell_Min[l+1], ShwrFprt[0][l+1][0]);
					Cell_Max[l+1] = TMath::Max( Cell_Max[l+1], ShwrFprt[0][l+1][1]);
				}
			} else {
				// dependences
				ShwrDep[0][0]      = 1;
				ShwrDep[N_Shwr][0] = 1;
				// KX correction
				KX  = ShwrPar[0][6][0];
				dKX = ShwrPar[0][6][1];
				// X0 correction
				if ( EX>Cutoff_SEED && TMath::Abs(ShwrPar[0][3][0]-X1)<2.) {
					X0   = 0.5*(X0+X1);
					dX0 += 0.5;
					// combine footprints
					for (l=2; l<18; l+=4) {
						Cell_Min[l]   = TMath::Min( Cell_Min[l],   ShwrFprt[0][l][0]);
						Cell_Max[l]   = TMath::Max( Cell_Max[l],   ShwrFprt[0][l][1]);
						Cell_Min[l+1] = TMath::Min( Cell_Min[l+1], ShwrFprt[0][l+1][0]);
						Cell_Max[l+1] = TMath::Max( Cell_Max[l+1], ShwrFprt[0][l+1][1]);
					}
				} else {
					X0  = ShwrPar[0][3][0];
					dX0 = ShwrPar[0][3][1]+0.5;
					// redefine footprint
					for (l=2; l<18; l+=4) {
						Cell_Min[l]   = ShwrFprt[0][l][0];
						Cell_Max[l]   = ShwrFprt[0][l][1];
						Cell_Min[l+1] = ShwrFprt[0][l+1][0];
						Cell_Max[l+1] = ShwrFprt[0][l+1][1];
					}
				}
			}

			// Corrections for not-well-defined showers - Y dependences
			if ( flag_y==2 ) {
				if ( TMath::Abs(ShwrPar[0][4][0]-Y0) > 3. ) {
					ShwrDep[N_Shwr][1] = 0;
				} else {
					ShwrDep[0][1]      = 1;
					ShwrDep[N_Shwr][1] = 1;
					// KX correction
					KY  = ShwrPar[0][7][0];
					dKY = ShwrPar[0][7][1];
					// combine footprints
					for (l=0; l<18; l+=4) {
						Cell_Min[l]   = TMath::Min( Cell_Min[l],   ShwrFprt[0][l][0]);
						Cell_Max[l]   = TMath::Max( Cell_Max[l],   ShwrFprt[0][l][1]);
						Cell_Min[l+1] = TMath::Min( Cell_Min[l+1], ShwrFprt[0][l+1][0]);
						Cell_Max[l+1] = TMath::Max( Cell_Max[l+1], ShwrFprt[0][l+1][1]);
					}
				}
			} else if ( flag_y==1 ) {
				// dependences
				ShwrDep[0][1]      = 1;
				ShwrDep[N_Shwr][1] = 1;
				// KY correction
				KY  = ShwrPar[0][7][0];
				dKY = ShwrPar[0][7][1];
				// combine footprints
				for (l=0; l<18; l+=4) {
					Cell_Min[l]   = TMath::Min( Cell_Min[l],   ShwrFprt[0][l][0]);
					Cell_Max[l]   = TMath::Max( Cell_Max[l],   ShwrFprt[0][l][1]);
					Cell_Min[l+1] = TMath::Min( Cell_Min[l+1], ShwrFprt[0][l+1][0]);
					Cell_Max[l+1] = TMath::Max( Cell_Max[l+1], ShwrFprt[0][l+1][1]);
				}
			} else {
				// dependences
				ShwrDep[0][1]      = 1;
				ShwrDep[N_Shwr][1] = 1;
				// KY correction
				KY  = ShwrPar[0][7][0];
				dKY = ShwrPar[0][7][1];
				// Y0 correction
				if ( EY>Cutoff_SEED && TMath::Abs(ShwrPar[0][4][0]-Y1)<2.) {
					Y0   = 0.5*(Y0+Y1);
					dY0 += 0.5;
					// combine footprints
					for (l=0; l<18; l+=4) {
						Cell_Min[l]   = TMath::Min( Cell_Min[l],   ShwrFprt[0][l][0]);
						Cell_Max[l]   = TMath::Max( Cell_Max[l],   ShwrFprt[0][l][1]);
						Cell_Min[l+1] = TMath::Min( Cell_Min[l+1], ShwrFprt[0][l+1][0]);
						Cell_Max[l+1] = TMath::Max( Cell_Max[l+1], ShwrFprt[0][l+1][1]);
					}
				} else {
					Y0  = ShwrPar[0][4][0];
					dY0 = ShwrPar[0][4][1]+0.5;
					// redefine footprint
					for (l=0; l<18; l+=4) {
						Cell_Min[l]   = ShwrFprt[0][l][0];
						Cell_Max[l]   = ShwrFprt[0][l][1];
						Cell_Min[l+1] = ShwrFprt[0][l+1][0];
						Cell_Max[l+1] = ShwrFprt[0][l+1][1];
					}
				}
			}

		}

		// perform initial fit of the shower
		I_Shwr = N_Shwr; // index of current shower
		ShwrApexI[I_Shwr] = Apex;
		double L0, L1, L2;
		Iter = 0;
rec_refit:
		Iter++;

		// E0 fit
		if(DEBUG) printf("-->ShowerRec(%d): Fit E0 \n", I_Shwr);
		TransFit( E0,  dE0, L0, L1, L2);
		l_start = L0;

		// Z0 fit
		if(DEBUG) printf("-->ShowerRec(%d): Fit Z0 \n", I_Shwr);
		TransFit( Z0,  dZ0, L0, L1, L2);

		// X0 fit
		if(DEBUG) printf("-->ShowerRec(%d): Fit X0 \n", I_Shwr);
		TransFit( X0,  dX0, L0, L1, L2);

		// KX fit, for first shower and non-overlaped secondary shower
		if(DEBUG) printf("-->ShowerRec(%d): Fit KX \n", I_Shwr);
		if ( ShwrDep[N_Shwr][0]==0 ) TransFit( KX, dKX, L0, L1, L2);

		// Y0 fit
		if(DEBUG) printf("-->ShowerRec(%d): Fit Y0 \n", I_Shwr);
		TransFit( Y0,  dY0, L0, L1, L2);

		// KY fit, for first shower and non-overlaped secondary shower
		if(DEBUG) printf("-->ShowerRec(%d): Fit KY \n", I_Shwr);
		if ( ShwrDep[N_Shwr][1]==0 ) TransFit( KY, dKY, L0, L1, L2);

		// A0 fit
		if(DEBUG) printf("-->ShowerRec(%d): Fit A0 \n", I_Shwr);
		TransFit( A0,  dA0, L0, L1, L2);

		l_end = L1;

		if(DEBUG)printf("First fit, Shower %d, Iteration %04d, Likelihood Start: %8.3f, End %8.3f, Wall %8.3f\n", N_Shwr, Iter, l_start, l_end, L2);
		if ( l_start - l_end > MinDeltaLkhd && Iter < MaxIteration ) goto rec_refit;

		// **************************
		//  store shower parameters
		// **************************

		// E0
		ShwrPar[N_Shwr][0][0] =  E0;
		ShwrPar[N_Shwr][0][1] = dE0;
		// B0
		ShwrPar[N_Shwr][1][0] =  B0;
		ShwrPar[N_Shwr][1][1] = dB0;
		// A0
		ShwrPar[N_Shwr][2][0] =  A0;
		ShwrPar[N_Shwr][2][1] = dA0;
		// X0
		ShwrPar[N_Shwr][3][0] =  X0;
		ShwrPar[N_Shwr][3][1] = dX0;
		// Y0
		ShwrPar[N_Shwr][4][0] =  Y0;
		ShwrPar[N_Shwr][4][1] = dY0;
		// Z0
		ShwrPar[N_Shwr][5][0] =  Z0;
		ShwrPar[N_Shwr][5][1] = dZ0;
		// KX
		ShwrPar[N_Shwr][6][0] =  KX;
		ShwrPar[N_Shwr][6][1] = dKX;
		// KY
		ShwrPar[N_Shwr][7][0] =  KY;
		ShwrPar[N_Shwr][7][1] = dKY;

		// Likelihood
		ShwrLkhd[N_Shwr]      =  L2;
		ShwrLkhdI[N_Shwr]     =  L1;

		// Buffer E0
		ShwrEb[N_Shwr][0] = E0;
		ShwrEb[N_Shwr][1] = dE0;

		// Cell Min and Max, clear ShwrExp[]
		for (l=0; l<18; l++) {
			ShwrFprt[N_Shwr][l][0] = Cell_Min[l];
			ShwrFprt[N_Shwr][l][1] = Cell_Max[l];
			for (m=3; m<=76; m++) ShwrExp[N_Shwr][l][m] = 0.;
		}

		// increment number of showers
		N_Shwr += 1;

		// test whether more showers needed (energy asymmetry, MIP near apex, ....)
		if ( N_Shwr>0 ) {

			// general
			if ( E0<=0. || Z0<EcalZ[17] || Z0>EcalZ[0] || A0<EcalZ[4] ) break;

			// position of the shower maximum
			if ( 1.360*(-145.65-1.066*log(E0)-Z0) > 4. ) break;
		}

		// subtract the newly defined shower
		// expand  the footprint for fit, which is defined in ShowerEstimate()
		ShowerFootprint();
		ShowerModel(0, 0);

		for (l=0; l<18; l++) {
			for (m=Cell_Min[l]; m<=Cell_Max[l]; m++) {
				ShwrExp[N_Shwr-1][l][m] = CellExp[l][m];
				ShwrEtr[N_Shwr-1][l][m] = CellEtr[l][m];
				ShwrBeta[N_Shwr-1][l][m] = CellBeta[l][m];
			}
		}

		// ****************************
		//		ShowerDisplay(N_Shwr-1);
		// ****************************/

		for (l=0; l<18; l++) {
			for (m=Cell_Min[l]; m<=Cell_Max[l]; m++) {
				// subtract defined shower
				if ( CellBeta[l][m] > 0. ) {
					CellDepS1[l][m] -= CellExp[l][m] + 2.*sqrt(CellExp[l][m]/CellBeta[l][m]); // to suppress fluctuations
					CellDepS0[l][m] -= CellExp[l][m];
					// subtract more for central cell
					if( m == Cell_Med[l] ){
						CellDepS1[l][m] -= 0.03;
						CellDepS0[l][m] -= 0.01;
					}
					if ( CellDepS1[l][m] < 0. ) CellDepS1[l][m] = 0.;
					if ( CellDepS0[l][m] < 0. ) CellDepS0[l][m] = 0.;
				} else {
					printf("E_ini[%d][%d] %8.3f, E_exp %8.3f, Nc %8.3f\n", l, m, CellDepS1[l][m], CellExp[l][m], CellBeta[l][m]);
				}
			}
		}

		// limit the entire reconstruction to the footprint of the most energetic shower
		if( N_Shwr==1 ){
			for(l=0; l<kNL; l++){
				for(m=4; m<Cell_Min[l]-9; m++)  CellDepS1[l][m] = 0.;
				for(m=Cell_Max[l]+9; m<76; m++) CellDepS1[l][m] = 0.;
			}
		}

	} // end of while loop

	if ( N_Shwr >= 1 ) {

		// restore CellDepS1[18][80], Why need this?
		for (l=0; l<18; l++) {
			for (m=0; m<80; m++) CellDepS1[l][m] = CellDep[l][m];
		}

	}

	if ( N_Shwr > 1 ) {

		// check whether combined fit is needed
		if ( ShwrDep[0][0]>0 || ShwrDep[0][1]>0 ) {

			// define common fit footprint for the first shower, the footprint of first shower is enlarged to contain the footprints of secondary showers
			for (l=0; l<18; l++) {
				Cell_Min[l] = 76;
				Cell_Max[l] = 3;
				if ( (l/2)%2 == 1 ) {
					// X projection
					for (n=0; n<N_Shwr; n++) {
						if ( ShwrDep[n][0]==1 ) {
							Cell_Min[l] = TMath::Min( Cell_Min[l], ShwrFprt[n][l][0]-1);
							Cell_Max[l] = TMath::Max( Cell_Max[l], ShwrFprt[n][l][1]+1);
						} else if ( ShwrDep[n][0]==0 ) {
							Cell_Min[l] = TMath::Min( Cell_Min[l], ShwrFprt[n][l][0]);
							Cell_Max[l] = TMath::Max( Cell_Max[l], ShwrFprt[n][l][1]);
						}
					}
				} else {
					// Y projection
					for (n=0; n<N_Shwr; n++) {
						if ( ShwrDep[n][1]==1 ) {
							Cell_Min[l] = TMath::Min( Cell_Min[l], ShwrFprt[n][l][0]-1);
							Cell_Max[l] = TMath::Max( Cell_Max[l], ShwrFprt[n][l][1]+1);
						} else if ( ShwrDep[n][1]==0 ) {
							Cell_Min[l] = TMath::Min( Cell_Min[l], ShwrFprt[n][l][0]);
							Cell_Max[l] = TMath::Max( Cell_Max[l], ShwrFprt[n][l][1]);
						}
					}
				}
				Cell_Min[l] = TMath::Max(  3, Cell_Min[l]);
				Cell_Max[l] = TMath::Min( 76, Cell_Max[l]);
				ShwrFprt[0][l][0] = Cell_Min[l];
				ShwrFprt[0][l][1] = Cell_Max[l];
				//				printf("Line2120:Layer %02d, Cmin %02d, Cmax %02d\n", l, Cell_Min[l], Cell_Max[l]);
			}
		}

	}
	// restore single shower parameters
	memcpy(ShwrParI, ShwrPar, sizeof(ShwrPar));
	return 0;
}


//==================================================================
//-->Estimate shower
//==================================================================
int Ecal3DRec::ShowerEstimate(){
	ShowerClustering();
	CalcCoG();
	ParabolaZ0();
	ShowerAxis();
	DefineFootprint();

	return XSeed+YSeed;
}

//==================================================================
//--->Shower Clustering
//==================================================================
int Ecal3DRec::ShowerClustering(){
	double v, val;
	int lx, mx, ly, my, nm;
	int l, m;
	int nw=0;

	// seed layer in X projection

define_mX:
	v  = Cutoff_MIP;// maximum energy
	lx = 0; // lay
	mx = 0; // cell
	for (l=0; l<18; l++) {
		SeedCell[l] = 0;
		if( l%4 < 2 ) continue;
		for (m=4; m<76; m++) {
			if ( CellDepS1[l][m]>v ) {
				v = CellDepS1[l][m];
				lx = l;
				mx = m;
			}
		}
	}

	// check the consistence of the found X seed
	if ( lx==2 || lx==15) nm = 1;
	else                  nm = 0;
	if ( mx > 0 ) {
		val = 0.;
		for (l=2; l<16; l+=4) {
			if( l/2 == lx/2 ) nw = 1;
			else              nw = 3;
			//			if ( l>(lx-5) && l<(lx+5) ) {
			for (m=TMath::Max(3,mx-nw); m<=TMath::Min(76,mx+nw); m++) {
				if ( ((l != lx) || (m != mx)) && CellDepS1[l][m]>0. ) {
					nm  += 1;
					val += CellDepS1[l][m];
				}
			}
			//			}
			//			if ( (l+1)>(lx-5) && (l+1)<(lx+5) ) {
			for (m=TMath::Max(3,mx-nw); m<=TMath::Min(76,mx+nw); m++) {
				if ( (((l+1) != lx) || (m != mx)) && CellDepS1[l+1][m]>0. ) {
					nm  += 1;
					val += CellDepS1[l+1][m];
				}
			}
			//			}
		}
		if ((nm < 3) || (v > 2.*val) ) {
			CellDepS1[lx][mx] = Cutoff_MIP;
			goto define_mX;
		}
	}
	fEX = nm;	

	// define X footprint
	if ( mx > 0 ) {
		dEX = v;
		// go upstream
		nm = mx;
		for (l=(lx&0xFE)+1; l>1; l-=4) {
			v = 0.;
			for (m=mx-3; m<=mx+3; m++) {
				if ( CellDepS1[l][m]>v ) {
					v = CellDepS1[l][m];
					SeedCell[l] = m;
				}
			}

			v = 0.;
			for (m=mx-2; m<=mx+2; m++) {
				if ( CellDepS1[l-1][m]>v ) {
					v = CellDepS1[l-1][m];
					SeedCell[l-1] = m;
				}
			}

			if      ( SeedCell[l-1] > 0 ) mx = SeedCell[l-1];
			else if ( SeedCell[l]   > 0 ) mx = SeedCell[l];
		}

		// go downstream
		mx = nm;
		for (l=(lx&0xFE)+4; l<16; l+=4) {
			v = 0.;
			for (m=mx-3; m<=mx+3; m++) {
				if ( CellDepS1[l][m]>v ) {
					v = CellDepS1[l][m];
					SeedCell[l] = m;
				}
			}

			v = 0.;
			for (m=mx-2; m<=mx+2; m++) {
				if ( CellDepS1[l+1][m]>v ) {
					v = CellDepS1[l+1][m];
					SeedCell[l+1] = m;
				}
			}

			if      ( SeedCell[l+1] > 0 ) mx = SeedCell[l+1];
			else if ( SeedCell[l]   > 0 ) mx = SeedCell[l];
		}
		mx = nm;                                            // restore mx
	} else {
		dEX = 0.;
	}

	//
	// maximal amplitudes in Y-projection
	//
define_mY:
	ly = 0;
	my = 0;
	v  = Cutoff_MIP;
	for (l=0; l<18; l+=4) {
		SeedCell[l]   = 0;
		SeedCell[l+1] = 0;
		for (m=4; m<76; m++) {
			if ( CellDepS1[l][m]>v ) {
				v = CellDepS1[l][m];
				ly = l;
				my = m;
			}
			if ( CellDepS1[l+1][m]>v ) {
				v = CellDepS1[l+1][m];
				ly = l+1;
				my = m;
			}
		}
	}

	// consistency of the found Y maximum 
	if ( ly==0 || ly==17) nm = 1;
	else                  nm = 0;
	nm = 0;
	if ( my > 0 ) {
		val = 0.;
		for (l=0; l<18; l+=4) {
			//      printf("LY %02d, MY %02d, E-1 %8.3f, E %8.3f, E+1 %8.3f\n", l, my, CellDepS1[l][my-1], CellDepS1[l][my], CellDepS1[l][my+1]);
			//      printf("LY %02d, MY %02d, E-1 %8.3f, E %8.3f, E+1 %8.3f\n", l+1, my, CellDepS1[l+1][my-1], CellDepS1[l+1][my], CellDepS1[l+1][my+1]);
			if( l/2 == ly/2 ) nw = 1;
			else              nw = 3;
			//			if ( l>(ly-5) && l<(ly+5) ) {
			for (m=TMath::Max(3,my-nw); m<=TMath::Min(76,my+nw); m++) {
				if ( (l != ly) || (m != my) ) {
					if ( CellDepS1[l][m]>0. ) {
						nm  += 1;
						val += CellDepS1[l][m];
					}
				}
				//				}
		}
		//			if ( (l+1)>(ly-5) && (l+1)<(ly+5) ) {
		for (m=TMath::Max(3,my-nw); m<=TMath::Min(76,my+nw); m++) {
			if ( (((l+1) != ly) || (m != my)) && CellDepS1[l+1][m]>0. ) {
				nm  += 1;
				val += CellDepS1[l+1][m];
			}
		}
		//			}
		}
		//    printf("LY %02d, MY %02d, Nhits %02d, Emax %8.3f, Eout %8.3f\n", ly, my, nm, CellDepS1[ly][my], val);
		if ((nm < 3) || (v > 2*val) ) {
			CellDepS1[ly][my] = Cutoff_MIP;
			goto define_mY;
		}
	}
	fEY = nm;

	// define Y footprint
	if ( my > 0 ) {
		dEY = v;
		// go upstream
		nm = my;
		for (l=(ly&0xFE)+1; l>0; l-=4) {
			v = 0.;
			for (m=my-3; m<=my+3; m++) {
				if ( CellDepS1[l][m]>v ) {
					v = CellDepS1[l][m];
					SeedCell[l] = m;
				}
			}

			v = 0.;
			for (m=my-2; m<=my+2; m++) {
				if ( CellDepS1[l-1][m]>v ) {
					v = CellDepS1[l-1][m];
					SeedCell[l-1] = m;
				}
			}

			if      ( SeedCell[l-1] > 0 ) my = SeedCell[l-1];
			else if ( SeedCell[l]   > 0 ) my = SeedCell[l];
		}

		// go downstream
		my = nm;
		for (l=(ly&0xFE)+4; l<18; l+=4) {
			v = 0.;
			for (m=my-3; m<=my+3; m++) {
				if ( CellDepS1[l][m]>v ) {
					v = CellDepS1[l][m];
					SeedCell[l] = m;
				}
			}

			v = 0.;
			for (m=my-2; m<=my+2; m++) {
				if ( CellDepS1[l+1][m]>v ) {
					v = CellDepS1[l+1][m];
					SeedCell[l+1] = m;
				}
			}

			if      ( SeedCell[l+1] > 0 ) my = SeedCell[l+1];
			else if ( SeedCell[l]   > 0 ) my = SeedCell[l];
		}
		my = nm;                                            // restore my
	} else {
		dEY = 0.;
	}

	XSeed = mx; // to be used in DefineFootprint
	YSeed = my;

	return 0;
}

//==================================================================
//--->Calcuate Center of Gravity
//==================================================================

int Ecal3DRec::CalcCoG(){
	fX0 = fX1 = 0;
	fY0 = fY1 = 0;
	int l, nm=0;
	double lrrpar, cr;
	for (l=0; l<18; l++) {
		nm = SeedCell[l];
		if ( nm > 0 ) {
			LayAmp[l] = CellDepS1[l][nm-1] + CellDepS1[l][nm] + CellDepS1[l][nm+1];
			LayCrd[l] = Cell_Size*((nm-40.5)*CellDepS1[l][nm-1] + (nm-39.5)*CellDepS1[l][nm] + (nm-38.5)*CellDepS1[l][nm+1])/LayAmp[l];

			//-->check the quality of the individual projections using the original energy depositions
			// energy map after subtruction of identified showers
			if ( CellDepS0[l][nm]>0.8*CellDepS0[l][nm-1] && CellDepS0[l][nm]>0.8*CellDepS0[l][nm+1]) {
				if ( (l/2)%2 == 1 ) fX0 += 1;
				else                fY0 += 1;
			}
			// original energy map
			if ( CellDepS1[l][nm]>0.8*CellDepS1[l][nm-1] && CellDepS1[l][nm]>0.8*CellDepS1[l][nm+1]) {
				if ( (l/2)%2 == 1 ) fX1 += 1;
				else                fY1 += 1;
			}

		} else {
			LayAmp[l] = 0.;
			LayCrd[l] = 0.;
		}
	}
	return 0;
}


//==================================================================
//==================================================================
int Ecal3DRec::ParabolaZ0(){
	//
	// Define coordinate of the shower maximum
	//
	Apex = 18;

	int l, nm = 0;
	double v  = 0.;
	for (l=0; l<18; l++) {
		if ( LayAmp[l] > v ) {
			v  = LayAmp[l];
			nm = l;
		}
	}
	for(l=0; l<18; l++) {
		//		cout<<"l="<<l<<", amp=" << LayAmp[l] << endl;
		if( LayAmp[l] > Cutoff_MIP*3 ){ Apex = l; break; }
	}
	// calculate inverse matrix a_ij
	double m11, m12, m22, m23, m33, b1, b2, b3, a11, a12, a13, a22, a23, a33, det;
	double val, arg;
	m11 = m12 = m22 = m23 = m33 = b1 = b2 = b3 = 0.;

	if ( nm < 3 ) {
		val = v;
		arg =  1.0 + EcalZ[0] - EcalZ[nm];
		//    printf("L -1, arg %8.3f, val %8.3f\n", arg, val);
		m11 += arg*arg*arg*arg;
		m12 += arg*arg*arg;
		m22 += arg*arg;
		m23 += arg;
		m33 += 1.;
		b1  += arg*arg*val;
		b2  += arg*val;
		b3  += val;
	}

	// fit Z0 using +-3 layers
	for(l=TMath::Max(0,nm-3); l<TMath::Min(18,nm+4); l++ ) {
		val = v - LayAmp[l];
		arg = EcalZ[l] - EcalZ[nm];
		//    printf("L %2d, arg %8.3f, val %8.3f\n", l, arg, val);
		m11 += arg*arg*arg*arg;
		m12 += arg*arg*arg;
		m22 += arg*arg;
		m23 += arg;
		m33 += 1.;
		b1  += arg*arg*val;
		b2  += arg*val;
		b3  += val;
	}

	if ( nm > 15 ) { 
		val = v- LayAmp[2*nm-18];
		arg =  EcalZ[17] - 1.0 - EcalZ[nm];
		//    printf("L 18, arg %8.3f, val %8.3f\n", arg, val);
		m11 += arg*arg*arg*arg;
		m12 += arg*arg*arg;
		m22 += arg*arg;
		m23 += arg;
		m33 += 1.;
		b1  += arg*arg*val;
		b2  += arg*val;
		b3  += val;

	}

	// m22 == m13 is used explicitely
	det = m11*m22*m33 + 2.*m12*m22*m23 - m22*m22*m22 - m12*m12*m33 - m23*m23*m11;

	if ( det != 0. ) {
		a11 = m22*m33 - m23*m23;
		a12 = m22*m23 - m12*m33;
		a13 = m12*m23 - m22*m22;
		a22 = m11*m33 - m22*m22;
		a23 = m12*m22 - m11*m23;
		a33 = m11*m22 - m12*m12;

		arg = b1*a11 + b2*a12 + b3*a13;
		if ( arg > 0. ) {
			val = -0.5*(b1*a12 + b2*a22 + b3*a23)/arg;
		} else {
			val = 0.;
		}
	} else {
		val = 0.;
	}

	if ( TMath::Abs(val)<2.5 ) {
		Z0 = val + EcalZ[nm];
		dZ0 = 1.2;
	} else {
		Z0 = EcalZ[nm];
		dZ0 = 2.5;
	}

	return 0;
}


//==================================================================
//==================================================================
int Ecal3DRec::ShowerAxis(){
	vector<double> vx, vz, vw;
	// fit XZ
	for(int l=0; l<18; l++){
		if( l%4>=2 && SeedCell[l]>0 ){
			vx.push_back( LayCrd[l] );
			vz.push_back( EcalZ[l]-Z0 );
			vw.push_back( LayAmp[l] );
		}
	}
	if( vx.size() > 1 )
		LinearFit(vx.size(), vx, vz, vw, KX, X0);
	else 
		KX = X0 = 0.;
	//		KX = X0 = -90.;

	vx.clear();
	vz.clear();
	vw.clear();
	for(int l=0; l<18; l++){
		if( l%4<2 && SeedCell[l]>0 ){
			vx.push_back( LayCrd[l] );
			vz.push_back( EcalZ[l]-Z0 );
			vw.push_back( LayAmp[l] );
		}
	}
	if( vx.size() > 1 )
		LinearFit(vx.size(), vx, vz, vw, KY, Y0);
	else 
		KY = Y0 = 0.;
	//		KY = Y0 = -90.;

	return 0;
}

//==================================================================
//==================================================================
void Ecal3DRec::LinearFit(int n, vector<double> vx, vector<double> vy, vector<double> vw, double &kx, double &x0){

	double m11, m12, m22, b1, b2, det;
	m11 = m12 = m22 = b1 = b2 = 0.;

	for(int i=0; i<n; i++){
		if ( vw[i] > 0 ) {
			m11 += vy[i]*vy[i]*vw[i];
			m12 += vy[i]*vw[i];
			m22 += vw[i];
			b1  += vx[i]*vy[i]*vw[i];
			b2  += vx[i]*vw[i];
		}
	}
	det = m11*m22 - m12*m12;
	if ( det != 0. ) {
		kx = (b1*m22 - b2*m12) / det;
		x0  = (b2*m11 - b1*m12) / det;
	} else {
		kx = -1.;
		x0 =  0.;
	}
	return;
}

//==================================================================
//==================================================================
int Ecal3DRec::DefineFootprint(){

	double e_x, e_y, v;
	double x, y, z;
	int l, k;
	int nm, m;

	static const int nw_fl = 1, // front layers (layer<3)
							 nw    = 1; // number of cells around the shower axis to be used in fit
	int nww;

	E0 = EX = EY = 0.;
	El2f = 0.;
	X1 = Y1 = e_x = e_y = 0.;
	for (l=0; l<18; l++) {
		nww = l<3?nw_fl:nw;

		z = EcalZ[l];
		x = X0 + KX*(z-Z0);
		y = Y0 + KY*(z-Z0);
		nm = 0;
		if ( TMath::Abs(x)<36. && TMath::Abs(y)<36. ) {
			if ( (l/2)%2 && XSeed >0 )           nm = (int) ((x +40*Cell_Size)/Cell_Size);
			else if ( ((l/2)%2)==0 && YSeed >0 ) nm = (int) ((y +40*Cell_Size)/Cell_Size);
		}

		m =  SeedCell[l];
		if ( m > 0 ) {
			if (nm>0 && nm<m) Cell_Min[l] = TMath::Max( 3, nm-nww);
			else              Cell_Min[l] = TMath::Max( 3,  m-nww);
			if (nm>0 && nm>m) Cell_Max[l] = TMath::Min(76, nm+nww);
			else              Cell_Max[l] = TMath::Min(76,  m+nww);
		} else if ( nm>0 ) {
			Cell_Min[l] = TMath::Max( 3, nm-nww);
			Cell_Max[l] = TMath::Min(76, nm+nww);
		} else {
			Cell_Min[l] = 76;
			Cell_Max[l] = 3;
		}

		v = 0.;
		for (k=Cell_Min[l]; k<=Cell_Max[l]; k++) v += CellDepS1[l][k];
		E0 += v;
		if( l>=16 ) El2f += v;
		if ( TMath::Abs(z-Z0) < 5. ) {
			if ( (l/2)%2 == 1 ) {
				EX += v;
				for (k=Cell_Min[l]; k<=Cell_Max[l]; k++) {
					X1 += (k-39.5)*CellDepS0[l][k]; 
					e_x += CellDepS0[l][k];
				}
			} else {
				EY += v;
				for (k=Cell_Min[l]; k<=Cell_Max[l]; k++) {
					Y1 += (k-39.5)*CellDepS0[l][k]; 
					e_y += CellDepS0[l][k];
				}
			}
		}

	}

	if ( e_x > 0. ) X1 /= e_x;
	if ( e_y > 0. ) Y1 /= e_y;


	double par0=0.85;
	double par1=0.6;
	double par2=0.2;

	double enecorr;
	if(E0>0.0)
		enecorr=par1+par2*TMath::Log(E0);
	if((par0-enecorr*El2f/E0)>0.0)
		E0=E0/(par0-enecorr*El2f/E0);

	double sqrt_E0 = sqrt(E0);

	dE0 = 0.2*sqrt_E0+0.03*E0;

	//B0  = 0.663-0.0078*log(E0);  
	B0 = 0.62;
	dB0 = 0.08;

	A0 = -141.0;
	//A0 = EcalZ[Apex]+0.5;
	dA0 = 0.5;

	T0 = (A0-Z0)*sqrt(1+KX*KX+KY*KY);
	dT0 = 0.5;

	dX0 = 1.0/sqrt_E0+0.05;
	dY0 = 1.0/sqrt_E0+0.05;
	dKX = 0.1/sqrt_E0+0.01;
	dKY = 0.1/sqrt_E0+0.01;

	return 0;
}



//==================================================================
//==================================================================
void Ecal3DRec::SetLatPar(int ish, double ene){
	if( ish > 2 ){
		cerr << "--Ecal3DRec--Error--SetLatPar--:Exceed maximum number of showers" << endl;
		return ;
	};
	// will be called at the beginning of fitting
	// the parameters will be fixed to speed up
	if( 0 ){ // use fixed parameters
		/*
		// Done by Andrei
		// ! need to understand energy dependence - so far done for 180 GeV TBeam electrons !
		double f_core[20] = {.805, .851, .903, .927, .936, .940, .932, .920, .890, .850, 
		.813, .786, .751, .711, .669, .625, .581, .537, .494, .452};
		double r_core[20] = {.220, .220, .220, .242, .269, .296, .324, .351, .379, .406,
		.433, .460, .488, .516, .543, .570, .598, .625, .653, .680};
		double r_tail[20] = {2.74, 2.56, 2.40, 2.26, 2.12, 2.00, 1.90, 1.82, 1.75, 1.70, 
		1.68, 1.70, 1.76, 1.85, 1.96, 2.08, 2.22, 2.37, 2.54, 2.73};
		*/
		// 350-400

		double f_corex[30]={
			0.768, 0.811, 0.845, 0.871, 0.888, 0.897, 0.898, 0.892, 0.880, 0.862, 
			0.839, 0.812, 0.782, 0.750, 0.716, 0.681, 0.645, 0.609, 0.574, 0.539, 
			0.505, 0.472, 0.441, 0.410, 0.381, 0.354, 0.328, 0.304, 0.281, 0.259, 
		};
		double r_corex[30]={
			0.160, 0.160, 0.160, 0.160, 0.203, 0.246, 0.289, 0.332, 0.375, 0.418, 
			0.461, 0.504, 0.547, 0.590, 0.633, 0.676, 0.719, 0.762, 0.805, 0.848, 
			0.891, 0.934, 0.977, 1.020, 1.063, 1.106, 1.149, 1.192, 1.235, 1.278, 
		};
		double r_tailx[30]={
			10.000, 9.274, 4.525, 3.050, 2.400, 2.080, 1.925, 1.866, 1.865, 1.905, 
			1.971, 2.059, 2.161, 2.274, 2.397, 2.528, 2.664, 2.805, 2.950, 3.098, 
			3.250, 3.404, 3.560, 3.718, 3.878, 4.039, 4.201, 4.365, 4.529, 4.695, 
		};

		for(int i=0; i<30; i++){
			Fcx[ish][i] = f_corex[i];
			Rcx[ish][i] = r_corex[i];
			Rtx[ish][i] = r_tailx[i];

			Fcy[ish][i] = f_corex[i];
			Rcy[ish][i] = r_corex[i];
			Rty[ish][i] = r_tailx[i];
		}
	}
	else{ // tuned by ISS data from 20GeV to 400GeV
		double logene = log(ene+1);
		if(logene<0) logene=0;
		double qc_p0 = 2.6409-0.0313*logene,
					 qc_p1 = 0.8349-0.0433*logene,
					 qc_p2 = 1.1,
					 rc_p0 = 0.015,
					 rc_p1 = 0.3712+5.142e-6*ene*sqrt(ene+1),
					 rt_p0 = 0.6568-0.2277*logene,
					 rt_p1 = 0.5611+0.1902*logene,
					 rt_p2 = 0;

		if( qc_p1<0. ) qc_p1=0.0;
		if( qc_p0<1. ) qc_p0=1.0;

		for(int i=0; i<30; i++){
			double t = 0.1*i+0.001;
			Fcx[ish][i] = qc_p0*exp( (qc_p1-t)/qc_p2 - exp((qc_p1-t)/qc_p2) );
			Rcx[ish][i] = rc_p0 + rc_p1*t;
			Rtx[ish][i] = rt_p0 + rt_p1*t + 1/(t+rt_p2);
			if( Rtx[ish][i] > 10 ) Rtx[ish][i] = 10.;
			Fcy[ish][i] = Fcx[ish][i];
			Rcy[ish][i] = Rcx[ish][i];
			Rty[ish][i] = Rtx[ish][i];
		}

	}
	return;
}

//==================================================================
//==================================================================
void Ecal3DRec::SetPdfPar(int ish, double ene){
	if( ish > 2 ){
		cerr << "--Ecal3DRec--Error--SetPdfPar--:Exceed maximum number of showers" << endl;
		return ;
	};
	if(1){
		/*	
				double p_0[20] = {80.0, 80.0, 80.0, 80.0, 80.0, 80.0, 80.0, 79.6, 79.0, 78.2,
				77.2, 75.8, 74.4, 72.8, 71.0, 68.8, 66.2, 63.0, 58.0, 52.0};
				double p_1[20] = {12.3, 12.3, 12.3, 12.3, 12.3, 12.3, 12.3, 12.2, 12.2, 12.1,
				11.9, 11.7, 11.4, 11.1, 10.8, 10.5, 10.0,  9.5,  8.7,  7.7};
				double p_2[20] = {.250, .250, .250, .250, .252, .254, .256, .260, .265, .270, 
				.276, .282, .288, .295, .301, .306, .312, .316, .318, .320};
				*/

		double norm = TMath::Gamma(7.5);
		for(int i=0; i<30; i++){
			//P0x[ish][i] = 0.5+50*exp(-ene/100)+40*exp(-ene/800);//MC
			//P0x[ish][i] = 0.5+32*exp(-ene/100)+60.2*exp(-ene/800);//ISS
			P0x[ish][i] = 0.5+28*exp(-ene/65)+68*exp(-ene/560);//ISS
			//P0x[ish][i] = 60.;
			//P0x[ish][i] = TMath::Max(0.2, 150-20*log(ene));
			//P1x[ish][i] = 5.4*TMath::Landau(0.1*i, 0.9, 0.25, 1);
			P1x[ish][i] = (4+0.005*ene)*6.5*exp(-0.65*i)*pow(0.65*i, 6.5)/norm;
			//			P1x[ish][i] = 4*6.5*exp(-0.65*i)*pow(0.65*i, 6.5)/norm;
			P2x[ish][i] = 0.3;

			P0y[ish][i] = P0x[ish][i];
			P1y[ish][i] = P1x[ish][i];
			P2y[ish][i] = P2x[ish][i];
			/*
				 P0x[ish][i] = px_0[i];
				 P1x[ish][i] = p_1[i];
				 P2x[ish][i] = p_2[i];

				 P0y[ish][i] = py_0[i];
				 P1y[ish][i] = p_1[i];
				 P2y[ish][i] = p_2[i];
				 */
		}
	} else{
		if( ene < 1 ) ene = 1;
		double logene = log(ene);
		//		if( ene > 5e3 ) ene = 5e3;
		double P0x_p0 = 40.9 + 9.55*logene,
					 P0x_p1 = 150 - P0x_p0,
					 P0x_p2 = 0.267+2.94/ene;
		//		double P1x_p0 = 9.0,
		//					P1x_p1 = 0.80,
		//					P1x_p2 = 0.40;
		double P1x_p0 = 1.1 - 0.06*logene,
					 P1x_p1 = 0.23,
					 P1x_p2 = ene<55?4:0.253 + 0.935*logene;
		double P2x_p0 = 0.40;

		//		double P0y_p0 = 57.7 + 8.83*logene,
		//					P0y_p1 = 150 - P0y_p0,
		//					P0y_p2 = 0.438+2.66/ene;
		//		double P1y_p0 = 9.0,
		//					P1y_p1 = 0.80,
		//					P1y_p2 = 0.40;
		double P1y_p0 = ene<10?0.603+0.156*logene:1.1-0.06*logene,
					 P1y_p1 = 0.23,
					 P1y_p2 = ene<55?4:0.253 + 0.935*logene;
		double P2y_p0 = 0.40;

		double tT0=0;
		for(int i=0; i<30; i++){
			tT0 = 0.1*i;
			//			P0x[ish][i] = P0x_p0*exp(-tT0/P0x_p2)+P0x_p1;
			P0x[ish][i] = 0.5+14.6*exp(-ene/20)+84.7*exp(-ene/400);//ISS
			P1x[ish][i] = P1x_p2*TMath::Landau(tT0, P1x_p0, P1x_p1, 1);
			//			P1x[ish][i] = P1x_p0*exp( (P1x_p1-tT0)/P1x_p2 - exp((P1x_p1-tT0)/P1x_p2 ) );
			P2x[ish][i] = P2x_p0;

			P0y[ish][i] = P0x[ish][i];
			P1y[ish][i] = P1x[ish][i];
			P2y[ish][i] = P2x[ish][i];

			//			P0y[ish][i] = P0y_p0*exp(-tT0/P0y_p2)+P0y_p1;
			//			P1y[ish][i] = P1y_p2*TMath::Landau(tT0, P1y_p0, P1y_p1, 1);
			//			P1y[ish][i] = P1y_p0*exp( (P1y_p1-tT0)/P1y_p2 - exp( (P1y_p1-tT0)/P1y_p2 ) );
			//			P2y[ish][i] = P2y_p0;

			//			printf("E %.2f, i=%d, P0x %.2f, P1x %.3f, P2x %.3f, P0y %.2f, P1y %.3f, P2y %.3f\n", ene, i, P0x[i], P1x[i], P2x[i], P0y[i], P1y[i], P2y[i]);
			//			printf("%.2f, ", P0y[i]);		if( (i+1)%10==0 ) cout<<endl;
		}
	}
}



//==================================================================
//==================================================================
int Ecal3DRec::TransFit(double &par, double &par_err, double &L0, double &L1, double &L2){

	int i, n1, n2, flag, iter;

	double param[7], lhood[7], pmin, lmin;

	double lkhd1, lkhd2;

	flag = iter = 0;
	if(KX==-90||X0==-90||KY==-90||Y0==-90)return -1;

fit_repeat:

	// parameter - central value
	param[3] = par;

	ShowerModel();
	lhood[3] = GetLkhd(lkhd1, lkhd2);

	if(DEBUG) printf("-->TransFit(): param[%d]=%8.4f, l1=%8.4f, l2=%8.4f\n", 3, par, lkhd1, lkhd2);
	//	if(lhood[3] ==-9999) return -1;
	if ( iter == 0 ) L0 = lhood[3];
	if ( par_err == 0 ){ L1 = L0; L2 = lkhd2; return 0; }

	// parameter - low values
	n1 = 3;
	while ( n1 > 0 ) {
		n1 -= 1;
		param[n1] = param[n1+1] - (3.-n1)*(par_err);
		par = param[n1];
		ShowerModel();
		lhood[n1] = GetLkhd(lkhd1, lkhd2);
		if(DEBUG) printf("-->TransFit(): param[%d]=%8.4f, l1=%8.4f, l2=%8.4f\n", n1, par, lkhd1, lkhd2);
		if ( lhood[n1] > lhood[n1+1] ) break;
	}

	// parameter - high values
	n2 = 3;
	while ( n2 < 6 ) {
		n2 += 1;
		param[n2] = param[n2-1] + (n2-3.)*(par_err);
		par = param[n2];
		ShowerModel();	
		lhood[n2] = GetLkhd(lkhd1, lkhd2);
		if(DEBUG) printf("-->TransFit(): param[%d]=%8.4f, l1=%8.4f, l2=%8.4f\n", n2, par,lkhd1, lkhd2);
		if ( lhood[n2] > lhood[n2-1] ) break;
	}
	// normalize parameters and likelihoods
	pmin = param[3];
	lmin = lhood[3];
	for (i=n1; i<=n2; i++ ) {
		if ( lhood[i] < lmin ) {
			pmin = param[i];
			lmin = lhood[i];
		}
	}
	for (i=n1; i<=n2; i++ ) { 
		param[i] -= pmin;
		lhood[i] -= lmin;
	}

	flag = ParabolaMin(n1, n2, param, lhood, L1, par, par_err);
	par += pmin;
	L1  += lmin;

	if(DEBUG) printf("-->TransFit(): iter=%02d, par=%8.3f +/- %8.3f\n", iter, par, par_err);
	iter++;

	if( flag == 0 && iter < 10 ) goto fit_repeat;

	// check the lkhd calculated from fitted parameter
	ShowerModel();
	L1 = GetLkhd(lkhd1, lkhd2);
	if ( L1 > lmin ) {
		par      = pmin;
		par_err *= 0.7;
		ShowerModel();
		L1 = GetLkhd(lkhd1, lkhd2);
		if(DEBUG) printf("-->TransFit(): L1=%8.3f, L2=%8.3f, par=%8.3f +/- %8.3f\n", L1, L2, par, par_err);
	}
	L2 = lkhd2;

	return 0;
}

/**************************************************************************************
 *                                                                                    *
 *         Calculate expected energy deposition for                                   *
 *         an electromagnetic shower                                                  *
 *                                                                                    *
 *         Input: - X0, Y0, Z0, KX, XY0 defines shower axis                           *
 *                                                                                    *
 *         Output: - ene_cell_exp[18][72], dpt_cell_exp[18][72], dst_cell_exp[18][72] *
 *                     expected energy depositions, average depth                     *
 *                     and average radius for exposed cells                           *       
 *                                                                                    *
 **************************************************************************************/

int Ecal3DRec::ShowerModel(int index, int flag){
	// flag = 1, calculate ideal energy deposition, for energy estimation
	// flag = 0, only calculate "real" energy deposition, for fit
	Index = index;
	int i, j, k, l, m;
	double x, y, z, v, d, eff, t, c, a;

	int F_Min, F_Max;
	if( KX==-90 || X0==-90|| KY==-90|| Y0==-90)return -1;

	//   double L_eff[5]   = {0.7, 1.0, 1.0, 1.0, 0.7};

	// shower footprint based on data
	memset(CellExp, 0, sizeof(CellExp));
	memset(CellEtr, 0, sizeof(CellEtr));
	memset(CellDepth, 0, sizeof(CellDepth));
	memset(CellDist, 0, sizeof(CellDist));
	memset(CellBeta, 0, sizeof(CellBeta));

	double sqrt_1_KXKX = sqrt(1+KX*KX);
	double sqrt_1_KYKY = sqrt(1+KY*KY);
	double sqrt_1_KXKX_KYKY = sqrt(1+KX*KX+KY*KY);

	T0 = A0>Z0?(A0-Z0)*sqrt_1_KXKX_KYKY:0.1;

	Alpha = B0*T0;
	GammaAlpha = TMath::Gamma( Alpha + 1 );

	// calculate energy deposition in the footprint
	for (l=0; l<90; l++) {
		z = F_Zcoord[l];
		x = X0 + KX*(z-Z0);
		y = Y0 + KY*(z-Z0);
		int l_ecal = l/5;
		if( Cell_Min[l_ecal] > Cell_Max[l_ecal] ) continue;
		//      printf("C_Min %d, C_Max %d, kX0 %f, X0 %f, kY0 %f, Y0 %f, Z0 %f, Energy %f\n", Cell_Min[l/5], Cell_Max[l/5], KX, X0, KY, Y0, Z0, E0);
		//		cout << "ShowerModel: Cell_Min "<<Cell_Min[l/5] <<", Cell_Max " << Cell_Max[l/5] << endl;
		//		continue;

		if ( (l/10)%2 ) {
			// X projection - wires along Y, 4 superlayers
			F_Min = (int) ((Cell_Size*(Cell_Min[l_ecal]-40) + y*F_Rotation[l_ecal] - F_Offset[l])/F_Diameter + 267.5);
			if ( F_Min <   0 ) F_Min =   0;
			if ( F_Min > 534 ) F_Min = 534;
			F_Max = (int) ((Cell_Size*(Cell_Max[l_ecal]-39) + y*F_Rotation[l_ecal] - F_Offset[l])/F_Diameter + 266.5);
			if ( F_Max <   0 ) F_Max =   0;
			if ( F_Max > 534 ) F_Max = 534;
			//			cout << "F_Min=" << F_Min << ", F_Max="<< F_Max << endl;
			//			continue;

			for (k=F_Min; k<=F_Max; k++) {
				v = F_Diameter*(k-266.5) - y*F_Rotation[l_ecal] + F_Offset[l];    // fiber coordinate
				c = y + KX*KY*(v-x)/(KX*KX+1.);                         // Y coordinate on the fiber
				m = (int) ((v +40*Cell_Size)/Cell_Size);                     // Cell ID
				d = TMath::Abs(v-x)/sqrt_1_KXKX;                          // distance to the shower axis
				t = (KX*(X0-v)+(Z0-z))*sqrt_1_KXKX_KYKY/(KX*KX+1.); // shower depth, could be negative, point-line distance 
				if ( m < Cell_Min[l_ecal] ) {
					printf("X Layer %d, fiber %d, cell %d, cell_min %d, coord %f\n", l, k, m, Cell_Min[l_ecal], v);
					printf("F_Min %d, F_Max %d, kX0 %f, X0 %f, kY0 %f, Y0 %f, Energy %f\n", F_Min, F_Max, KX, X0, KY, Y0, E0);
				}
				if ( m > Cell_Max[l_ecal] ){
					printf("X Layer %d, fiber %d, cell %d, cell_max %d, coord %f\n", l, k, m, Cell_Max[l_ecal], v);
					printf("F_Min %d, F_Max %d, kX0 %f, X0 %f, kY0 %f, Y0 %f, Energy %f\n", F_Min, F_Max, KX, X0, KY, Y0, E0);
					//exit(-1);
				}

				//calculate efficiency
				v = TMath::Abs(v/Cell_Size-m+39.5)-L_Eff_PW; // PW=0.32, original value, AK
				if ( v > 0. ) eff = L_Eff[l%5]*(1. - L_Eff_Par*v); // --> minimum efficiency 28%, AK
				else          eff = L_Eff[l%5];
				if( eff < 0 ) eff = 0;

				// deposited energy and weighted coordinates of the cell
				a = eff*ShowerShape(l/10, t, d);
				// apply correction of inclination, lateral leakage 
				float cf = 1.0; 

				if( l/10%2 == m/2%2 ) // PMT at "-" end
					cf = getCorrEs2Ed((t/T0+1), d, c, KY, B0)*getKyCorrEs2Ed((t/T0+1), d, KY, KX);
				else  // PMT at "+" end
					cf = getCorrEs2Ed((t/T0+1), d, -c, -KY, B0)*getKyCorrEs2Ed((t/T0+1), d, -KY, KX);

				a *= cf;
				CellExp[l_ecal][m] += a;                                          // collected light
				CellDepth[l_ecal][m] += a*t;                                        // amplitude-averaged depth
				CellDist[l_ecal][m] += a*d;                                        // amplitude-averaged distance to axis

				if(flag){ // called for energy calculation, after fits are done
					// ideal energy deposition
					if( l/10%2 == m/2%2 ) // PMT at "-" end
						cf = getCorrEd2Ei((t/T0+1), d, c, KY, B0);
					else  // PMT at "+" end
						cf = getCorrEd2Ei((t/T0+1), d, -c, -KY, B0);
					CellEtr[l_ecal][m] += a/eff*cf;
				}
			}
		} else {
			// Y projection - wires along X, 5 superlayers
			F_Min = (int) ((Cell_Size*(Cell_Min[l_ecal]-40) + x*F_Rotation[l_ecal] - F_Offset[l])/F_Diameter + 267.5);
			F_Max = (int) ((Cell_Size*(Cell_Max[l_ecal]-39) + x*F_Rotation[l_ecal] - F_Offset[l])/F_Diameter + 266.5);
			for (k=F_Min; k<=F_Max; k++) {
				v = F_Diameter*(k-266.5) - x*F_Rotation[l_ecal] + F_Offset[l];    // fiber coordinate 
				m = (int) ((v +40*Cell_Size)/Cell_Size);                     // Cell ID
				c = x + KX*KY*(v-y)/(KY*KY+1.);                            // X coordinate on the fiber, pedal
				d = TMath::Abs(v-y)/sqrt_1_KYKY;                          // distance to the shower axis
				t = (KY*(Y0-v)+(Z0-z))*sqrt_1_KXKX_KYKY/(KY*KY+1.); // shower depth, could be negative

				if ( m < Cell_Min[l_ecal] ){
					printf("Y Layer %d, fiber %d, cell %d, cell_min %d, coord %f, F_Min=%d\n", l, k, m, Cell_Min[l_ecal], v, F_Min);
					printf("F_Min %d, F_Max %d, kX0 %f, X0 %f, kY0 %f, Y0 %f, Energy %f\n", F_Min, F_Max, KX, X0, KY, Y0, E0);
				}
				if ( m > Cell_Max[l_ecal] ){
					printf("Y Layer %d, fiber %d, cell %d, cell_max %d, coord %f, F_Max=%d\n", l, k, m, Cell_Max[l_ecal], v, F_Max);
					printf("F_Min %d, F_Max %d, kX0 %f, X0 %f, kY0 %f, Y0 %f, Energy %f\n", F_Min, F_Max, KX, X0, KY, Y0, E0);
				}

				//	    eff = pmteff_map(coor_res[l%5], TMath::Abs(v/Cell_Size-m+35.5));
				//calculate efficiency
				v = TMath::Abs(v/Cell_Size-m+39.5)-L_Eff_PW; // original PW=0.32
				if ( v > 0. ) eff = L_Eff[l%5]*(1. - L_Eff_Par*v); // --> minimum efficiency 28%, AK
				else          eff = L_Eff[l%5];
				if( eff < 0 ) eff = 0;

				// deposited energy and weighted coordinates of the cell
				a = eff*ShowerShape(l/10, t, d);
				if( a < 0 ){
					printf("-->ShowerModel(): t=%.3f, d=%.3f, c=%.3f, eff=%.3f, a=%.5f\n", t, d, c, eff, a);
				}
				// apply correction of inclination, lateral leakage 
				float cf = 1.0; 
				// considering the PMT location

				if( l/10%2 == m/2%2 ) // PMT at "-" end
					cf = getCorrEs2Ed((t/T0+1), d, c, KX, B0)*getKyCorrEs2Ed((t/T0+1), d, KX, KY);
				else // PMT at "+" end
					cf = getCorrEs2Ed((t/T0+1), d, -c, -KX, B0)*getKyCorrEs2Ed((t/T0+1), d, -KX, KY);
				//				if( cf <=0 ) {
				//					cout << Form("t/T0+1=%g, d=%d, c=%d, KX=%g, KY=%g", t/T0+1, d, c, KX, KY) << endl;
				//				}
				
				a *= cf;
				CellExp[l_ecal][m] += a;                                          // collected light
				CellDepth[l_ecal][m] += a*t;                                        // amplitude-averaged depth
				CellDist[l_ecal][m] += a*d;                                        // amplitude-averaged distance to axis

				if(flag){ // ideal energy deposition
					
					if( l/10%2 == m/2%2 ) // PMT at "-" end
						cf = getCorrEd2Ei((t/T0+1), d, c, KX, B0);
					else // PMT at "+" end
						cf = getCorrEd2Ei((t/T0+1), d, -c, -KX, B0);
					CellEtr[l_ecal][m] += a/eff*cf;
				}
			}
		}
	}

	// position of the cell wrt the shower maximum
	int i1, i2;
	double p0, p1, p2, reg;
	for (i=0; i<18; i++) {
		for (j=Cell_Min[i]; j<=Cell_Max[i]; j++) {
			if ( TMath::IsNaN( CellExp[i][j] ) ){
				cout << "1540: I_Shwr=" << I_Shwr << ", l=" << i << ", m=" << j << endl;
			}
			if ( CellExp[i][j] > 0. ) {
				CellDepth[i][j] /= CellExp[i][j];
				CellDist[i][j] /= CellExp[i][j];

				reg = CellDepth[i][j];                            // shower depth
				/*
					 i1 = (int)(reg+9.);
					 if ( i1 < 0  ) i1 = 0;
					 if ( i1 > 18 ) i1 = 18;
					 i2 = i1 +1;
					 reg = (reg - i1+9.);
					 if ( reg > 1. ) reg = 1.;
					 */
				i1 = int((1+reg/T0)*10);
				if(i1<0) i1=0;
				if(i1>28) i1=28;
				i2=i1+1;
				reg = ((1+reg/T0)*10 - i1);
				if(reg>1) reg=1;
				if(reg<-1) reg=-1;


				// parameters
				if( i%4<2 ){ // y-projection
					p0 = P0y[I_Shwr][i1] + reg*(P0y[I_Shwr][i2]-P0y[I_Shwr][i1]);
					p1 = P1y[I_Shwr][i1] + reg*(P1y[I_Shwr][i2]-P1y[I_Shwr][i1]);
					if(p1<0) p1=0;
					p2 = P2y[I_Shwr][i1] + reg*(P2y[I_Shwr][i2]-P2y[I_Shwr][i1]);
				} else{
					p0 = P0x[I_Shwr][i1] + reg*(P0x[I_Shwr][i2]-P0x[I_Shwr][i1]);
					p1 = P1x[I_Shwr][i1] + reg*(P1x[I_Shwr][i2]-P1x[I_Shwr][i1]);
					if(p1<0) p1=0;
					p2 = P2x[I_Shwr][i1] + reg*(P2x[I_Shwr][i2]-P2x[I_Shwr][i1]);
				}

				reg = CellDist[i][j];                            // distance from the shower axis
				//	    if( reg < 0.2 ) reg = 0.2;

				CellBeta[i][j] = p0/(1. + p1*exp(-reg/p2));
				if( CellBeta[i][j]<0 ) 
					printf("-->ShowerModel(): L=%d, C=%d, Dep=%8.3f, Exp=%.3f, Beta=%.3f\n", i, j, CellDepS1[i][j], CellExp[i][j], CellBeta[i][j]);
				//		cout <<"ShowerModel: e "<<CellExp[i][j]<<" t "<<CellDepth[i][j] << " d " << CellDist[i][j] << endl;
			} else {
				printf("-->ShowerModel(): L=%d, C=%d, Dep=%8.3f, Exp=%.3f\n", i, j, CellDepS1[i][j], CellExp[i][j]);
			}
		}
	}

	return 0;
}




double Ecal3DRec::ShowerShape(int slay, double depth, double dist){

	if( E0 <= 0 ) return 0.00001;

	int i1, i2;
	double qc, rc, rt, l1, l2, varc, vart;

	//	if ( T0 + depth < 0.0 ){
	//      cout<<"Ecal3DRec::ShowerShape: T0="<<T0<<", depth="<<depth<<endl;
	//		return 0.;
	//	}
	/*	// use t parameterization
			vart   = depth;
	// define derivative for linear interpolation
	i1 = (int)(vart+9.);
	if ( i1 < 0  ) i1 = 0;
	if ( i1 > 18 ) i1 = 18;
	i2 = i1 +1;
	varc = (vart - i1+9.);
	if ( varc > 1. ) varc = 1.;
	*/	

	//	use t/T0 parameterization

	vart   = depth/T0+1; // parameterized as a function of t/T0, 0~2
	i1 = int(vart*10);
	if(i1<0) i1=0;
	if(i1>28) i1=28;
	i2 = i1+1;
	varc = vart*10 - i1;
	if( varc > 1 ) varc = 1.;

	depth += T0;
	if( depth < 0.01 ) depth = 0.01; // to process cells before apex

	if( slay%2 ){ // x projection
		// Core fraction
		qc = Fcx[I_Shwr][i1] + varc*(Fcx[I_Shwr][i2] - Fcx[I_Shwr][i1]);
		// Core radius
		rc = Rcx[I_Shwr][i1] + varc*(Rcx[I_Shwr][i2] - Rcx[I_Shwr][i1]);
		// Tail radius
		rt = Rtx[I_Shwr][i1] + varc*(Rtx[I_Shwr][i2] - Rtx[I_Shwr][i1]);
	}else{
		qc = Fcy[I_Shwr][i1] + varc*(Fcy[I_Shwr][i2] - Fcy[I_Shwr][i1]);
		rc = Rcy[I_Shwr][i1] + varc*(Rcy[I_Shwr][i2] - Rcy[I_Shwr][i1]);
		rt = Rty[I_Shwr][i1] + varc*(Rty[I_Shwr][i2] - Rty[I_Shwr][i1]);
	}
	// to fit the lateral parameters
	static const double rl = 0.95, rh = 1.05;
	if( Index == 1 ) qc *= rl;
	if( Index == 2 ) qc *= rh;
	if( Index == 3 ) rc *= rl;
	if( Index == 4 ) rc *= rh;
	if( Index == 5 ) rt *= 0.9;
	if( Index == 6 ) rt *= 1.1;

	if( qc > 1 ) qc = 1.;
	if( qc < 0 ) qc = 0.;
	if( rc < 0.1 ) rc = 0.1;
	if( rt > 10 ) rt = 10.;

	return ShowerShapeIdeal( depth, dist, qc, rc, rt );
}


/****************************************************************************
 *                                                                          *
 *            Expected light output of a single fiber at depth, distance    *
 *            and coord in the electromagnetic shower defined by parameters *
 *            E0, B0, T0                                                    *
 *                                                                          *
 *               Input: -  depth, dist, coord, E0, B0, T0                   *
 *                                                                          *
 *               Output: - light output - return value                      *       
 *                                                                          *
 ****************************************************************************/
double Ecal3DRec::ShowerShapeIdeal(double depth, double dist, double fc, double rc, double rt){
	// no lateral leakage along fiber
	//	if( fc > 1. ) fc = 1.;
	//	if( fc < 0. ) fc = 0.;
	//	if( rc < 0.1 ) rc = 0.1;
	double rc2 = rc*rc,
				 rt2 = rt*rt,
				 d2  = dist*dist;
	double varc = 3.14159265358979312*rc2/(rc2+d2)/sqrt(rc2+d2);
	double vart = 3.14159265358979312*rt2/(rt2+d2)/sqrt(rt2+d2);
	//	double varc = TMath::Pi()*rc*rc/(rc*rc+dist*dist)/sqrt(rc*rc+dist*dist);
	//	double vart = TMath::Pi()*rt*rt/(rt*rt+dist*dist)/sqrt(rt*rt+dist*dist);

	//GammaAlpha = TMath::Gamma(Alpha+1)
	//double e = fNorm*E0*B0/GammaAlpha*pow(B0*depth, Alpha)*exp(-B0*depth)*(fc*varc + (1.-fc)*vart);
	//	double e = fNorm*E0*B0*GammaFast(depth, T0)*(fc*varc + (1.-fc)*vart);
	double val=0, e=0;
	if ( E0 > 0.01 ) {
		val = 0.; // reset to 0, 17-Jun-2014
		//		val = 0.020*pow(E0,0.3)*pow(2.5*depth,3.25)*exp(-2.5*depth);           // shower beginning
		e = fNorm*(val+E0*B0*GammaFast(depth, T0))*(fc*varc+(1.-fc)*vart);
		//		e = fNorm*(val+E0*B0/GammaAlpha*pow(B0*depth, Alpha)*exp(-B0*depth))*(fc*varc+(1.-fc)*vart);
		if( TMath::IsNaN( e ) ){
			cout << "E0=" << E0 << ", B0=" << B0 << ", Alpha=" << Alpha << ", Z0=" << Z0 << ", depth=" << depth << endl;
		}
	} else {
		val = 0.005*pow(2.5*depth,3.25)*exp(-2.5*depth);                       // shower beginning
		e = fNorm*val*(fc*varc+(1.-fc)*vart);
	}
	//if( e < 0 ){
	//		printf("-->ShowerShapeIdeal(): fNorm=%g, E0=%g, B0=%g, TMath::Gamma(Alpha+1.)=%g, pow(B0*depth, Alpha)=%g, exp(-B0*depth)=%g, (fc*varc + (1.-fc)*vart)=%g\n", fNorm, E0, B0, TMath::Gamma(Alpha+1.), pow(B0*depth, Alpha), exp(-B0*depth), (fc*varc + (1.-fc)*vart) );
	//	e = 0.000001;
	//}
	return e;
}

//==================================================================
int Ecal3DRec::GammaFast_FirstCall = 1;
double Ecal3DRec::GammaFast(double depth, double t0){
	// use table to calculate gamma function
	// within 0<T0<20, 0<t<20
	static double gamma_table[200][200];
	if( GammaFast_FirstCall ){ // generate table
		printf("Call GammaFast() First Time, Generating table\n");
		double _T0, _Alpha, _B0=0.62, _GammaAlpha;
		double _depth;
		for(int i=0; i<200; i++){
			_T0 = 0.1*(i)+1;
			_Alpha = _B0*_T0;
			_GammaAlpha = TMath::Gamma(_Alpha+1);
			for(int j=0; j<200; j++){
				_depth = 0.1*(j);
				gamma_table[i][j] = pow(_B0*_depth, _Alpha)*exp(-_B0*_depth)/_GammaAlpha;
			}
		}
		GammaFast_FirstCall = 0;
	}
	/*
		 int i1 = int(10*t0);
		 if( i1>199 ) i1 = 199;
		 if( i1<0   ) i1 = 0;
		 int j1 = int(10*depth);
		 if( j1>199 ) j1 = 199;
		 if( j1<0   ) j1 = 0;
		 return gamma_table[i1][j1];
		 */
	t0=10*(t0-1);
	depth*=10;
	int i1, j1,
			i2, j2;
	if( t0 <= 0 ){
		t0 = 0.;
		i1 = i2 = 0;
	} else if( t0>=199 ){
		i1 = i2 = 199;
	} else{
		i1 = int(t0);
		i2 = i1 + 1;
	}

	if( depth >= 199 ){
		j1 = j2 = 199;
	} else {
		j1 = int(depth);
		j2 = j1 + 1;
	}
	/*
		 double di1, di2;
		 double dj1, dj2;
	// derivatives for T0
	if ( i1 == i2 ) {
	if( i1==0 )
	di1 = di2 = (t0-i1)*(gamma_table[i1+1][j1] - gamma_table[i1][j1]);
	else
	di1 = di2 = (t0-i1)*(gamma_table[i1][j1] - gamma_table[i1-1][j1]);
	} else {
	di1 = (t0-i1)*(gamma_table[i1+1][j1] - gamma_table[i1][j1]);
	di2 = (t0-i2)*(gamma_table[i2][j2] - gamma_table[i2-1][j2]);
	}                 
	// derivatives for distance d 
	if ( j1 == j2 ) {
	dj1 = dj2 = 0;
	} else {
	dj1 = (depth-j1)*(gamma_table[i1][j1+1] - gamma_table[i1][j1]);
	dj2 = (depth-j2)*(gamma_table[i2][j2] - gamma_table[i2][j2-1]);
	}

	double dist0 = di1*di1 + dj1*dj1,
	dist1 = di2*di2 + dj2*dj2;
	double par0 = (dist1*(gamma_table[i1][j1] + di1 + dj1) + dist0*(gamma_table[i2][j2] + di2 + dj2))/(dist0+dist1);
	return par0;
	*/
	double b1 = gamma_table[i1][j1],
				 b2 = gamma_table[i2][j1] - gamma_table[i1][j1],
				 b3 = gamma_table[i1][j2] - gamma_table[i1][j1],
				 b4 = gamma_table[i2][j2] - gamma_table[i1][j2] - gamma_table[i2][j1] + gamma_table[i1][j1];
	return b1 + b2*(t0-i1) + b3*(depth-j1) + b4*(t0-i1)*(depth-j1);
}


//==================================================================
float Ecal3DRec::cf_es2ed[3628800]={0};
bool  Ecal3DRec::is_cf_es2ed_loaded=0;
double Ecal3DRec::getCorrEs2Ed(double t, double d, double c, double kx, double beta, int flag){
	//  t: shower depth, t/zmax*10
	//  d: distance to shower axis, 0 - 8
	//  c: pedal coordinates, -32.4 - 32.4
	// kx: slope in fiber plane, -0.4 - 0.4
	if( !is_cf_es2ed_loaded ){
		cout << "Try to load correction table" << endl;
		TFile *ftmp = gFile;
		TFile *finput=new TFile("/afs/cern.ch/user/h/hachen/public/Common/attpar_es2ed.root");

		if( finput == NULL ){
			cerr << "No Correction Table, Exit" << endl;
			exit(-1);
		}
		TTree *tree = dynamic_cast<TTree*>(finput->Get("attpar"));
		tree->SetBranchAddress("cf", cf_es2ed);
		tree->GetEntry(0);
		is_cf_es2ed_loaded=1;
		finput->Close();
		if(ftmp) ftmp->cd();
	}
	int i1, i2, i3, i4,
			j1, j2, j3, j4;
	if( t <= 0 ){
		i1 = j1 = 0;
	} else if( t>=2.9 ){
		i1 = j1 = 29;
	} else{
		i1 = int(t*10);
		j1 = i1 + 1;
	}

	if( d >= 7.8 ){
		i2 = j2 = 39;
	} else {
		i2 = int(5*d);
		j2 = i2 + 1;
	}

	if( c<= -31.95){
		i3 = j3 = 0;
	} else if(c>=31.95){
		i3 = j3 = 143;
	} else{
		i3 = int((c+32.4)/0.45);
		j3 = i3 + 1;
	}

	if( kx <= -0.4 ){
		i4 = j4 = 0;
	} else if( kx >= 0.4 ){
		i4 = j4 = 20;
	} else{
		i4 = int( 25*kx+10 );
		j4 = i4+1;
	}

	int n1 = i1*120960 + i2*3024 + i3*21 + i4;
	int m1 = j1*120960 + j2*3024 + j3*21 + j4;

	double di1, di2, di3, di4, di5;
	double dj1, dj2, dj3, dj4, dj5;
	// derivatives for depth t
	if ( i1 == j1 ) {
		di1 = dj1 = 0;//(t*10-i1)*(cf_es2ed[n1+120960] - cf_es2ed[n1]);
	} else {
		di1 = (t*10-i1)*(cf_es2ed[n1+120960] - cf_es2ed[n1]);
		dj1 = (t*10-j1)*(cf_es2ed[m1] - cf_es2ed[m1-120960]);
	}
	// derivatives for distance d 
	if ( i2 == j2 ) {
		di2 = dj2 = 0.;//(5*d - i2)*(cf_es2ed[n1] - cf_es2ed[n1-2877]);
	} else {
		di2 = (5*d-i2)*(cf_es2ed[n1+3024] - cf_es2ed[n1]);
		dj2 = (5*d-j2)*(cf_es2ed[m1] - cf_es2ed[m1-3024]);
	}
	// derivatives for coordinate c
	if ( i3 == j3 ) {
		di3 = dj3 = 0.;
	} else {
		di3 = ((c+32.4)/0.45-i3)*(cf_es2ed[n1+21] - cf_es2ed[n1]);
		dj3 = ((c+32.4)/0.45-j3)*(cf_es2ed[m1] - cf_es2ed[m1-21]);
	}
	// derivatives for tangent kx
	if ( i4 == j4 ) {
		di4 = dj4 = 0.;
	} else {
		di4 = (25*kx+10-i4)*(cf_es2ed[n1+1] - cf_es2ed[n1]);
		dj4 = (25*kx+10-j4)*(cf_es2ed[m1] - cf_es2ed[m1-1]);
	}

	double dist0 = di1*di1 + di2*di2 + di3*di3 + di4*di4,
				 dist1 = dj1*dj1 + dj2*dj2 + dj3*dj3 + dj4*dj4;
	double par0;
	if( dist1 + dist0 > 0 )
		par0 = (dist1*(cf_es2ed[n1] + di1 + di2 + di3 + di4) + dist0*(cf_es2ed[m1] + dj1 + dj2 + dj3 + dj4))/(dist0+dist1);
	else 
		par0 = 0.5*(cf_es2ed[n1]+cf_es2ed[m1]);
	if( flag > 0 || par0<=0 || TMath::IsNaN(par0)){
		cout << "================ getCorrEs2Ed ===========================" << endl;
		cout << Form("t %f, d %f, c %f, kx %f", t, d, c, kx) << endl;
		cout << Form("Index 0: %d %d %d %d ", i1, i2, i3, i4) << endl;
		cout << Form("Index 1: %d %d %d %d ", j1, j2, j3, j4) << endl;
		cout << Form("Base Values: %d %d %f %f", n1, m1, cf_es2ed[n1], cf_es2ed[m1])<<endl;
		cout << Form("Value 0: %f %f %f %f %f %f %f ", cf_es2ed[n1], di1, di2, di3, di4, dist1/(dist0+dist1), cf_es2ed[n1] + di1 + di2 + di3 + di4 )<<endl;
		cout << Form("Value 1: %f %f %f %f %f %f %f ", cf_es2ed[m1], dj1, dj2, dj3, dj4, dist0/(dist0+dist1), cf_es2ed[m1] + dj1 + dj2 + dj3 + dj4 )<<endl;
		cout << Form("Correction: %f ", par0)<<endl;
	}

	return par0;
}



//==================================================================
float Ecal3DRec::cf_ed2ei[3628800]={0};
bool  Ecal3DRec::is_cf_ed2ei_loaded=0;
double Ecal3DRec::getCorrEd2Ei(double t, double d, double c, double kx, double beta, int flag){
	//  t: shower depth, t/zmax*10
	//  d: distance to shower axis, 0 - 8
	//  c: pedal coordinates, -32.4 - 32.4
	// kx: slope in fiber plane, -0.4 - 0.4
	if( !is_cf_ed2ei_loaded ){
		cout << "Try to load correction table" << endl;
		TFile *ftmp = gFile;
		TFile *finput=new TFile("/afs/cern.ch/user/h/hachen/public/Common/attpar_ed2ei.root");

		if( finput == NULL ){
			cerr << "No Correction Table, Exit" << endl;
			exit(-1);
		}
		TTree *tree = dynamic_cast<TTree*>(finput->Get("attpar"));
		tree->SetBranchAddress("cf", cf_ed2ei);
		tree->GetEntry(0);
		is_cf_ed2ei_loaded=1;
		finput->Close();
		if(ftmp) ftmp->cd();
	}
	int i1, i2, i3, i4,
			j1, j2, j3, j4;
	if( t <= 0 ){
		i1 = j1 = 0;
	} else if( t>=2.9 ){
		i1 = j1 = 29;
	} else{
		i1 = int(t*10);
		j1 = i1 + 1;
	}

	if( d >= 7.8 ){
		i2 = j2 = 39;
	} else {
		i2 = int(5*d);
		j2 = i2 + 1;
	}

	if( c<= -31.95){
		i3 = j3 = 0;
	} else if(c>=31.95){
		i3 = j3 = 143;
	} else{
		i3 = int((c+32.4)/0.45);
		j3 = i3 + 1;
	}

	if( kx <= -0.4 ){
		i4 = j4 = 0;
	} else if( kx >= 0.4 ){
		i4 = j4 = 20;
	} else{
		i4 = int( 25*kx+10 );
		j4 = i4+1;
	}

	int n1 = i1*120960 + i2*3024 + i3*21 + i4;
	int m1 = j1*120960 + j2*3024 + j3*21 + j4;

	double di1, di2, di3, di4, di5;
	double dj1, dj2, dj3, dj4, dj5;
	// derivatives for depth t
	if ( i1 == j1 ) {
		if( i1==0 )
			di1 = dj1 = 0;//(10*t-i1)*(cf_ed2ei[n1+120960] - cf_ed2ei[n1]);
		else
			di1 = dj1 = 0;//(10*t-i1)*(cf_ed2ei[n1] - cf_ed2ei[n1-120960]);
	} else {
		di1 = (10*t-i1)*(cf_ed2ei[n1+120960] - cf_ed2ei[n1]);
		dj1 = (10*t-j1)*(cf_ed2ei[m1] - cf_ed2ei[m1-120960]);
	}
	// derivatives for distance d 
	if ( i2 == j2 ) {
		di2 = dj2 = 0.;//(5*d - i2)*(cf_ed2ei[n1] - cf_ed2ei[n1-2877]);
	} else {
		di2 = (5*d-i2)*(cf_ed2ei[n1+3024] - cf_ed2ei[n1]);
		dj2 = (5*d-j2)*(cf_ed2ei[m1] - cf_ed2ei[m1-3024]);
	}
	// derivatives for coordinate c
	if ( i3 == j3 ) {
		di3 = dj3 = 0.;
	} else {
		di3 = ((c+32.4)/0.45-i3)*(cf_ed2ei[n1+21] - cf_ed2ei[n1]);
		dj3 = ((c+32.4)/0.45-j3)*(cf_ed2ei[m1] - cf_ed2ei[m1-21]);
	}
	// derivatives for tangent kx
	if ( i4 == j4 ) {
		di4 = dj4 = 0.;
	} else {
		di4 = (25*kx+10-i4)*(cf_ed2ei[n1+1] - cf_ed2ei[n1]);
		dj4 = (25*kx+10-j4)*(cf_ed2ei[m1] - cf_ed2ei[m1-1]);
	}

	double dist0 = di1*di1 + di2*di2 + di3*di3 + di4*di4,
				 dist1 = dj1*dj1 + dj2*dj2 + dj3*dj3 + dj4*dj4;
	double par0;
	if( dist1 + dist0 > 0 )
		par0 = (dist1*(cf_ed2ei[n1] + di1 + di2 + di3 + di4) + dist0*(cf_ed2ei[m1] + dj1 + dj2 + dj3 + dj4))/(dist0+dist1);
	else 
		par0 = cf_ed2ei[n1];

	if( flag > 0 || par0<=0 || TMath::IsNaN(par0) ){
		cout << "===========================================" << endl;
		cout << Form("t %f, d %f, c %f, kx %f", t, d, c, kx) << endl;
		cout << Form("Index 0: %d %d %d %d", i1, i2, i3, i4) << endl;
		cout << Form("Index 1: %d %d %d %d", j1, j2, j3, j4) << endl;
		cout << Form("Base Values: %d %f %d %f", n1, cf_ed2ei[n1], m1, cf_ed2ei[m1])<<endl;
		printf("di3: cf_ed2ei[%d]=%g, cf_ed2ei[%d]=%g\n", n1+21, cf_ed2ei[n1+21], m1-21, cf_ed2ei[m1-21]);
		printf("di4: cf_ed2ei[%d]=%g, cf_ed2ei[%d]=%g\n", n1+1, cf_ed2ei[n1+1], m1-1, cf_ed2ei[m1-1]);
		cout << Form("Value 0: %f %f %f %f %f %f %f", cf_ed2ei[n1], di1, di2, di3, di4, dist1/(dist0+dist1), cf_ed2ei[n1] + di1 + di2 + di3 + di4 )<<endl;
		cout << Form("Value 1: %f %f %f %f %f %f %f", cf_ed2ei[m1], dj1, dj2, dj3, dj4, dist0/(dist0+dist1), cf_ed2ei[m1] + dj1 + dj2 + dj3 + dj4 )<<endl;
		cout << Form("Correction: %f ", par0)<<endl;
		par0 = 0.5*(cf_ed2ei[n1] + cf_ed2ei[m1]);
	}

	return par0;
}

//==================================================================
float Ecal3DRec::cf_ky_es2ed[25200] = {0};
bool  Ecal3DRec::is_cf_ky_es2ed_loaded = 0;
double Ecal3DRec::getKyCorrEs2Ed(double t, double d, double kx, double ky, int flag){
	//  t: shower depth, t/zmax*10
	//  d: distance to shower axis, 0 - 8
	// kx: slope in fiber plane, -0.4 - 0.4
	if( !is_cf_ky_es2ed_loaded ){
		cout << "Try to load correction table" << endl;
		TFile *ftmp = gFile;
		TFile *finput=new TFile("/afs/cern.ch/user/h/hachen/public/Common/attpar_es2ed_kycorr.root");

		if( finput == NULL ){
			cerr << "No Correction Table, Exit" << endl;
			exit(-1);
		}
		TTree *tree = dynamic_cast<TTree*>(finput->Get("t"));
		tree->SetBranchAddress("cf", cf_ky_es2ed);
		tree->GetEntry(0);
		is_cf_ky_es2ed_loaded=1;
		finput->Close();
		if(ftmp) ftmp->cd();
	}
	int i1, i2, i3,
			j1, j2, j3;
	if( t <= 0 ){
		i1 = j1 = 0;
	} else if( t>=2.9 ){
		i1 = j1 = 29;
	} else{
		i1 = int(t*10);
		j1 = i1 + 1;
	}

	if( d >= 7.8 ){
		i2 = j2 = 39;
	} else {
		i2 = int(5*d);
		j2 = i2 + 1;
	}

	if( kx <= -0.4 ){
		i3 = j3 = 0;
	} else if( kx >= 0.4 ){
		i3 = j3 = 20;
	} else{
		i3 = int( 25*kx+10 );
		j3 = i3+1;
	}

	int n1 = i1*840 + i2*21 + i3;
	int m1 = j1*840 + j2*21 + j3;

	double di1, di2, di3;
	double dj1, dj2, dj3;
	// derivatives for depth t
	if ( i1 == j1 ) {
		if( i1==0 )
			di1 = dj1 = (t*10-i1)*(cf_ky_es2ed[n1+840] - cf_ky_es2ed[n1]);
		else
			di1 = dj1 = (t*10-i1)*(cf_ky_es2ed[n1] - cf_ky_es2ed[n1-840]);
	} else {
		di1 = (t*10-i1)*(cf_ky_es2ed[n1+840] - cf_ky_es2ed[n1]);
		dj1 = (t*10-j1)*(cf_ky_es2ed[m1] - cf_ky_es2ed[m1-840]);
	}
	// derivatives for distance d 
	if ( i2 == j2 ) {
		di2 = dj2 = 0.;
	} else {
		di2 = (5*d-i2)*(cf_ky_es2ed[n1+21] - cf_ky_es2ed[n1]);
		dj2 = (5*d-j2)*(cf_ky_es2ed[m1] - cf_ky_es2ed[m1-21]);
	}
	// derivatives for tangent kx
	if ( i3 == j3 ) {
		di3 = dj3 = 0.;
	} else {
		di3 = (25*kx+10-i3)*(cf_ky_es2ed[n1+1] - cf_ky_es2ed[n1]);
		dj3 = (25*kx+10-j3)*(cf_ky_es2ed[m1] - cf_ky_es2ed[m1-1]);
	}

	double dist0 = di1*di1 + di2*di2 + di3*di3,
				 dist1 = dj1*dj1 + dj2*dj2 + dj3*dj3;
	double par0;
	if( dist0 + dist1 > 0 ) 
		par0 = (dist1*(cf_ky_es2ed[n1] + di1 + di2 + di3) + dist0*(cf_ky_es2ed[m1] + dj1 + dj2 + dj3))/(dist0+dist1);
	else 
		par0 = cf_ky_es2ed[n1];
	if( flag > 0 ){
		cout << "===========================================" << endl;
		cout << Form("t %f, d %f, kx %f", t, d, kx) << endl;
		cout << Form("Index 0: %d %d %d ", i1, i2, i3) << endl;
		cout << Form("Index 1: %d %d %d ", j1, j2, j3) << endl;
		cout << Form("Base Values: %d %d %f %f", n1, m1, cf_ky_es2ed[n1], cf_ky_es2ed[m1])<<endl;
		cout << Form("Value 0: %f %f %f %f %f %f ", cf_ky_es2ed[n1], di1, di2, di3, dist1/(dist0+dist1), cf_ky_es2ed[n1] + di1 + di2 + di3 )<<endl;
		cout << Form("Value 1: %f %f %f %f %f %f ", cf_ky_es2ed[m1], dj1, dj2, dj3, dist0/(dist0+dist1), cf_ky_es2ed[m1] + dj1 + dj2 + dj3 )<<endl;
		cout << Form("Correction: %f ", par0)<<endl;
	}

	return TMath::Max( 1+par0*ky*ky, 0.1 );	
}


//==================================================================
//==================================================================
double Ecal3DRec::GetLkhd(double &L1, double &L2){
	// L1: from shower fluctuation
	// L2: from boundary conditions
	ShowerProbability();
	int i, j;
	double x, y, z, val=0;
	L1 = L2 = 0;
	for (i=0; i<18; i++){
		if( Cell_Max[i] < Cell_Min[i] ) continue;
		for (j=Cell_Min[i]; j<=Cell_Max[i]; j++) {
			if ( CellProb[i][j] > 0. )	L1 += CellProb[i][j]; 
		}
		// ad hoc boundary conditions, to contain shower axis within footprint
		z = EcalZ[i];
		x = X0 + KX*(z-Z0);
		y = Y0 + KY*(z-Z0);
		if ( (i/2)%2 ) {
			if      ( (val=x-(Cell_Min[i]-40)*Cell_Size) < 0. )  L2 += val*val;
			else if ( (val=(Cell_Max[i]-39)*Cell_Size-x) < 0. )  L2 += val*val;
		} else {
			if      ( (val=y-(Cell_Min[i]-40)*Cell_Size) < 0. )  L2 += val*val;
			else if ( (val=(Cell_Max[i]-39)*Cell_Size-y) < 0. )  L2 += val*val;
		}
	}
	// ad hoc boundary conditions
	if ( (val=A0-Z0-3.)  < 0. ) L2 += val*val;
	if ( (val=A0+141.)   > 0. ) L2 += val*val;
	if ( (val=Z0+142.)   > 0. ) L2 += val*val;
	if ( (val=Z0+160.)   < 0. ) L2 += val*val;
	if ( (val=E0-0.20)   < 0. ) L2 += val*val*1e4;
	if ( (val=B0-0.05)   < 0. ) L2 += val*val*1e4;
	if ( (val=B0-1.95)   > 0. ) L2 += val*val*1e4;
	return L1+L2;
}

/******************************************************************************
 *                                                                         	*
 *     Probability according to Poisson distribution for the        	 			*
 *     actual e-m energy deposition ene_cell_dep[18][80] and expected   		*
 *     deposition of ene_cell_exp[18][80] at depth dpt_cell_exp[18][80] and   *
 *     radial distance of depth dst_cell_exp[18][80]                     		*
 *                                                                  				*
 *       Input: -  ene_cell_dep[18][80], ene_cell_exp[18][80], 					*
 *       			 dpt_cell_exp[18][80]  and  dst_cell_exp[18][80] 				*
 *                                                                  				*
 *       Output: - prob_cell[18][80]                                   			*       
 *                                                                          	*
 *****************************************************************************/
int Ecal3DRec::ShowerProbability() {

	int k, l, m;

	double arg, val, reg, p0, p1, p2, beta;

	// !!!!!!!!!!!!!! what to do with cells(t<0)?, i.e. CellBeta[][]=0
	//	double ltot=0.;

	for (l=0; l<18; l++) {
		for (m=0; m<80; m++) {
			CellProb[l][m]=0.;
		}
		for (m=Cell_Min[l]; m<=Cell_Max[l]; m++) {
			if ( CellMask[l][m]==0 || CellMask[l][m] ) {

				arg = CellBeta[l][m]*CellExp[l][m];              // expected number of particles from the current shower
				val = CellExp[l][m];                             // expected energy from the current shower

				// estimated number of particles and average beta
				for (k=0; k<N_Shwr; k++) {
					if(k==I_Shwr)continue;	
					val += ShwrExp[k][l][m];
					arg += ShwrBeta[k][l][m]*ShwrExp[k][l][m];
				}

				if ( val == 0. ) {
					printf("-->ShowerProbability(): L %d, Cell %d, Eexp %f, Npart %f\n", l, m, val, arg);
					arg  = 0.0001;
					beta = 80.;
				} else {
					beta = arg/val;
				}

				reg = beta*CellDep[l][m];                       // measured energy
				if( CellMask[l][m] ) reg = arg; // overflow/bad/noisy

				// probability calculation
				val = arg - reg*log(arg);
				if ( reg < 10. ) CellProb[l][m] = val + TMath::LnGamma(reg+1.);//log( TMath::Gamma(reg+1.) );
				else             CellProb[l][m] = val + reg*log(reg) - reg + log(reg*(1.+4.*reg*(1.+2.*reg)))/6. + 0.57236494;

				// likelihood ratio
				val =  arg - reg;
				if ( reg > 0. ) val += reg * (log(reg) - log(arg));
				CellProbNorm[l][m] = val;
				//				if( val < 0 ) printf("-->ShowerProbability(): L %d, Cell %d, Eexp %f, Npart %f\n", l, m, val, arg);

				//				ltot+=CellProb[l][m];
				if(DEBUG>10) printf("-->ShowerProbability():L %d, Cell %d, E %f, Eexp %f, Prob %f, N %f, Nexp %f\n", l, m, CellDep[l][m], CellExp[l][m], CellProb[l][m], reg, arg);

			}
		}
	}
	//	if( DEBUG>10 ) printf("-->ShowerProbability(): Lkhd=%.3f\n", ltot);
	return 0;
}

//==================================================================
//==================================================================
int Ecal3DRec::ParabolaMin(int n1, int n2, double param[], double lhood[], double &L1, double &par, double &error){
	int i, j, flag = 0;

	double m11, m12, m22, m23, m33, b1, b2, b3, a11, a12, a13, a22, a23, a33, det, alpha, val, err;
	// calculate inverse matrix a_ij
	m11 = m12 = m22 = m23 = m33 = b1 = b2 = b3 = 0.;
	for (i=n1; i<=n2; i++ ) {
		m11 += param[i]*param[i]*param[i]*param[i];
		m12 += param[i]*param[i]*param[i];
		m22 += param[i]*param[i];
		m23 += param[i];
		m33 += 1.;
		b1  += param[i]*param[i]*lhood[i];
		b2  += param[i]*lhood[i];
		b3  += lhood[i];
		//    printf("Ind %d, Par %9.4f / %8.3f, Likelihood %8.3f / %8.3f\n", i, param[i], param[i]+pmin, lhood[i], lhood[i]+lmin);
	}
	// m22 == m13 is used explicitely
	det = m11*m22*m33 + 2.*m12*m22*m23 - m22*m22*m22 - m12*m12*m33 - m23*m23*m11;

	if ( det != 0. ) {
		a11 = m22*m33 - m23*m23;
		a12 = m22*m23 - m12*m33;
		a13 = m12*m23 - m22*m22;
		a22 = m11*m33 - m22*m22;
		a23 = m12*m22 - m11*m23;
		a33 = m11*m22 - m12*m12;

		alpha = b1*a11 + b2*a12 + b3*a13;
		if ( alpha > 0. ) {
			val = -0.5*(b1*a12 + b2*a22 + b3*a23)/alpha;
			err = 0.7*sqrt(det/alpha);
			L1  = (b1*a13 + b2*a23 + b3*a33 - val*val*alpha)/det;
			//      printf("alpha %8.3f, det %f, val %8.3f, err %8.3f, L1 %8.3f\n", alpha, det, val, err, L1);
			flag = 1;
		} else {
			if ( lhood[n1] < lhood[n2] ) {
				val = param[n1];
				L1  = lhood[n1];
			} else if ( lhood[n2] < lhood[n1] ) {
				val = param[n2];
				L1  = lhood[n2];
			} else {
				val = param[3];
				L1  = lhood[3];
			}
			err = 2.*error;
		}
	} else {
		if ( lhood[n1] < lhood[n2] ) {
			val = param[n1];
			L1  = lhood[n1];
		} else if ( lhood[n2] < lhood[n1] ) {
			val = param[n2];
			L1  = lhood[n2];
		} else {
			val = param[3];
			L1  = lhood[3];
		}
		err = 2.*error;
	}

	if ( TMath::Abs(val-param[3]) > 6.*(error) ) {
		flag = 0;
		if ( lhood[n1] < lhood[n2] ) {
			val = param[n1];
			L1  = lhood[n1];
		} else {
			val = param[n2];
			L1  = lhood[n2];
		}
		err = 2.*error;
	}

	par = val;
	error = err;
	return flag;
}

int Ecal3DRec::ShowerFootprint() {

	int    k, l, m;
	double x, y, z, v;

	//
	// Define shower footprint
	//
	k = TMath::Max(4, (int) (4.0 + 2.6*TMath::Log(E0+1)));
	double sqrt_1_KXKX_KYKY = sqrt(1.+KX*KX+KY*KY);
	for (l=0; l<18; l++) {

		z = EcalZ[l];
		x = X0 + KX*(z-Z0);
		y = Y0 + KY*(z-Z0);
		if ( TMath::Abs(x)<36. && TMath::Abs(y)<36. ) {
			if ( (l/2)%2 ) m = (int) ((x +40*Cell_Size)/Cell_Size);
			else           m = (int) ((y +40*Cell_Size)/Cell_Size);

			Cell_Med[l] = m;
			Shwr_Med[I_Shwr][l] = m;
			v = (Z0-z)*sqrt_1_KXKX_KYKY;
			if ( v < -5. ) {
				Cell_Min[l] = TMath::Max( 0, m-k);
				Cell_Max[l] = TMath::Min(79, m+k);
			} else if (  v < 5. ) {
				Cell_Min[l] = TMath::Max( 0, m-k);
				Cell_Max[l] = TMath::Min(79, m+k);
			} else {
				Cell_Min[l] = TMath::Max( 0, m-k);
				Cell_Max[l] = TMath::Min(79, m+k);
			}

		} else {

			Cell_Min[l] = 79;
			Cell_Max[l] = 0;

		}

		//    printf("Layer %02d, Cell_Min %d, Cell_Max %d\n", l, Cell_Min[l], Cell_Max[l] );

	}
	return 0;
}


int Ecal3DRec::TransFitKx(double &L0, double &L1, double &L2){
	int i, n1, n2, flag, iter;

	double param[7], lhood[7], pmin, lmin;

	double lkhd1, lkhd2;

	flag = iter = 0;
	if(KX==-90||X0==-90||KY==-90||Y0==-90)return -1;

fit_repeat:

	// parameter - central value
	param[3] = KX;

	lhood[3] = GetLkhd(lkhd1, lkhd2);
	//	if(lhood[3] ==-9999) return -1;
	if ( iter == 0 ) L0 = lhood[3];

	// parameter - low values
	n1 = 3;
	while ( n1 > 0 ) {
		n1 -= 1;
		param[n1] = param[n1+1] - (3.-n1)*(dKX);
		for (i=N_Shwr-1; i>-1; i--) {                                         // order matters
			if (ShwrDep[i][0]==1) {
				ShwrPar[i][6][0] = param[n1];
				EcalShwrDef(i);
			}
		}
		lhood[n1] = GetLkhd(lkhd1, lkhd2);
		if ( lhood[n1] > lhood[n1+1] ) break;
	}

	// parameter - high values
	n2 = 3;
	while ( n2 < 6 ) {
		n2 += 1;
		param[n2] = param[n2-1] + (n2-3.)*(dKX);
		for (i=N_Shwr-1; i>-1; i--) {                                         // order matters
			if (ShwrDep[i][0]==1) {
				ShwrPar[i][6][0] = param[n1];
				EcalShwrDef(i);
			}
		}
		lhood[n2] = GetLkhd(lkhd1, lkhd2);
		if ( lhood[n2] > lhood[n2-1] ) break;
	}
	// normalize parameters and likelihoods
	pmin = param[3];
	lmin = lhood[3];
	for (i=n1; i<=n2; i++ ) {
		if ( lhood[i] < lmin ) {
			pmin = param[i];
			lmin = lhood[i];
		}
	}
	for (i=n1; i<=n2; i++ ) { 
		//		if( DEBUG ) printf("-->TransFitKx(): param[%d]=%8.3f, lhood[%d]=%8.3f\n", i, param[i], i, lhood[i] ); 
		param[i] -= pmin;
		lhood[i] -= lmin;
	}

	flag = ParabolaMin(n1, n2, param, lhood, L1, KX, dKX);
	KX += pmin;
	L1 += lmin;
	for (i=N_Shwr-1; i>-1; i--) {                                           // order matters
		if (ShwrDep[i][0]==1) {
			ShwrPar[i][6][0] = KX;
			ShwrPar[i][6][1] = dKX;
		}
	}
	iter++;
	for (i=N_Shwr-1; i>-1; i--) {                                         // order matters
		if (ShwrDep[i][0]==1) {
			EcalShwrDef(i);
		}
	}
	if( flag == 0 && iter < 10 ) goto fit_repeat;

	// check the lkhd calculated from fitted parameter
	L1 = GetLkhd(lkhd1, lkhd2);
	if ( L1 > lmin ) {
		KX   = pmin;
		dKX *= 0.7;
		for (i=N_Shwr-1; i>-1; i--) {                                         // order matters
			if (ShwrDep[i][0]==1) {
				ShwrPar[i][6][0] = KX;
				ShwrPar[i][6][1] = dKX;
				EcalShwrDef(i);
			}
		}
		L1 = GetLkhd(lkhd1, lkhd2);
	}
	L2 = lkhd2;

	return 0;
}

int Ecal3DRec::TransFitKy(double &L0, double &L1, double &L2){
	int i, n1, n2, flag, iter;

	double param[7], lhood[7], pmin, lmin;

	double lkhd1, lkhd2;

	flag = iter = 0;
	if(KX==-90||X0==-90||KY==-90||Y0==-90)return -1;

fit_repeat:

	// parameter - central value
	param[3] = KY;

	lhood[3] = GetLkhd(lkhd1, lkhd2);
	//	if(lhood[3] ==-9999) return -1;
	if ( iter == 0 ) L0 = lhood[3];

	// parameter - low values
	n1 = 3;
	while ( n1 > 0 ) {
		n1 -= 1;
		param[n1] = param[n1+1] - (3.-n1)*(dKY);
		for (i=N_Shwr-1; i>-1; i--) {                                         // order matters
			if (ShwrDep[i][1]==1) {
				ShwrPar[i][7][0] = param[n1];
				EcalShwrDef(i);
			}
		}
		lhood[n1] = GetLkhd(lkhd1, lkhd2);
		if ( lhood[n1] > lhood[n1+1] ) break;
	}

	// parameter - high values
	n2 = 3;
	while ( n2 < 6 ) {
		n2 += 1;
		param[n2] = param[n2-1] + (n2-3.)*(dKY);
		for (i=N_Shwr-1; i>-1; i--) {                                         // order matters
			if (ShwrDep[i][1]==1) {
				ShwrPar[i][7][0] = param[n1];
				EcalShwrDef(i);
			}
		}
		lhood[n2] = GetLkhd(lkhd1, lkhd2);
		if ( lhood[n2] > lhood[n2-1] ) break;
	}
	// normalize parameters and likelihoods
	pmin = param[3];
	lmin = lhood[3];
	for (i=n1; i<=n2; i++ ) {
		if ( lhood[i] < lmin ) {
			pmin = param[i];
			lmin = lhood[i];
		}
	}
	for (i=n1; i<=n2; i++ ) { 
		//		if( DEBUG ) printf("-->TransFitKy(): param[%d]=%8.3f, lhood[%d]=%8.3f\n", i, param[i], i, lhood[i] ); 
		param[i] -= pmin;
		lhood[i] -= lmin;
	}

	flag = ParabolaMin(n1, n2, param, lhood, L1, KY, dKY);
	KY += pmin;
	L1 += lmin;
	for (i=N_Shwr-1; i>-1; i--) {                                           // order matters
		if (ShwrDep[i][1]==1) {
			ShwrPar[i][7][0] = KY;
			ShwrPar[i][7][1] = dKY;
		}
	}
	iter++;

	for (i=N_Shwr-1; i>-1; i--) {                                         // order matters
		if (ShwrDep[i][1]==1) {
			EcalShwrDef(i);
		}
	}
	if( flag == 0 && iter < 10 ) goto fit_repeat;

	// check the lkhd calculated from fitted parameter
	L1 = GetLkhd(lkhd1, lkhd2);
	if ( L1 > lmin ) {
		KY   = pmin;
		dKX *= 0.7;
		for (i=N_Shwr-1; i>-1; i--) {                                         // order matters
			if (ShwrDep[i][1]==1) {
				ShwrPar[i][7][0] = KY;
				ShwrPar[i][7][1] = dKY;
				EcalShwrDef(i);
			}
		}
		L1 = GetLkhd(lkhd1, lkhd2);
	}
	L2 = lkhd2;

	return 0;
}

double Ecal3DRec::ShowerEnergy(int id){
	if( id==0 )    return ElectronEnergy();
	else if(id==1) return ProtonEnergy();
	else if(id==2) return PhotonEnergy();
	else{
		cerr << "==Ecal3DRec::ShowerEnergy()==Option NOT defined"<<endl;
		return -1;
	}
	return 0.0;
}

double Ecal3DRec::ElectronEnergy(){
	return 0.0;
}

double Ecal3DRec::PhotonEnergy(){
	return 0.0;
}

double Ecal3DRec::ProtonEnergy(){
	return 0.0;
}

void Ecal3DRec::Summary(){
// 1. re-define footprint according to the most energetic shower
	//-->(1) find the most energetic shower

	I_Shwr = 0; // index of the most energetic shower
	E0 = 0.;    // Total fitted shower energy 
	E1 = 0.;    // highest fitted shower energy

	int DEBUG2=0;
	if(DEBUG2) cout << "--Ecal3DRec::Summary: N_Shwr=" << N_Shwr << endl;

	for(int k=0; k<N_Shwr; k++){
		E0 += ShwrPar[k][0][0];
		if( E1 < ShwrPar[k][0][0] ){
			I_Shwr = k;
			E1 = ShwrPar[k][0][0];
		}
	}

	if(DEBUG2)cout << "--Ecal3DRec::Summary: E0=" << E0 << ", E1=" << E1 << endl;

	//-->(2) re-define footprint

	B0 = ShwrPar[I_Shwr][1][0];
	A0 = ShwrPar[I_Shwr][2][0];
	X0 = ShwrPar[I_Shwr][3][0];
	Y0 = ShwrPar[I_Shwr][4][0];
	Z0 = ShwrPar[I_Shwr][5][0];
	KX = ShwrPar[I_Shwr][6][0];
	KY = ShwrPar[I_Shwr][7][0];
	T0  = (A0-Z0)*sqrt(1+KX*KX+KY*KY);

	if(DEBUG2)cout << "X0=" << X0 << ", Y0=" << Y0 << endl;

	ShowerFootprint();

// 2. calculate energy deposition of each shower
	//-->(1) low energy shower first
	for(int k=0; k<N_Shwr; k++){
		// reset shower shape and fluctuation parameters according to reconstructed energy
		SetLatPar(k, ShwrPar[k][0][0]);
		SetPdfPar(k, ShwrPar[k][0][0]);
		if( k != I_Shwr ){
			if(DEBUG2){
				printf("A0=%g, X0=%g, Y0=%g, Z0=%g, KX=%g, KY=%g, E0=%g\n", ShwrPar[I_Shwr][2][0], ShwrPar[I_Shwr][3][0], ShwrPar[I_Shwr][4][0], ShwrPar[I_Shwr][5][0], ShwrPar[I_Shwr][6][0], ShwrPar[I_Shwr][7][0], ShwrPar[I_Shwr][0][0]);
			}
			EcalShwrDef( k, 1 ); // calculate ideal energy deposition
		}
	}

	//-->(2) the most energetic one
	EcalShwrDef( I_Shwr, 1 ); // calculate ideal energy deposition

// 3. calculate final probability
	ShowerProbability();

// 4. sum expected energy from identified showers
	memset(CellExp, 0, sizeof(CellExp));
	memset(CellEtr, 0, sizeof(CellEtr));
	memset(CellBeta, 0, sizeof(CellBeta));

	for(int l=0; l<kNL; l++){
		for(int m=Cell_Min[l]; m<=Cell_Max[l]; m++){
			for(int k=0; k<N_Shwr; k++){
				if( TMath::IsNaN( ShwrExp[k][l][m] )){
					cout << Form("3034: k=%d, l=%d, m=%d", k, l, m) << endl;
				}
				CellExp[l][m] += ShwrExp[k][l][m];
				CellEtr[l][m] += ShwrEtr[k][l][m];
				CellBeta[l][m] += ShwrBeta[k][l][m]*ShwrExp[k][l][m]; // number of particles
			}
			// apply correction to each cell, to be used to reconstruct particle energy
		}
	}

// 5. calculate EM energy 
	EMEnergy();

// 6. calculate EM likelihood
	EMEstimatorVars();
	EMEstimatorLkhd();

}

void Ecal3DRec::EMEnergy(){
	// (1) correct energy in cells
	// transversal leakage cross fiber is accounted for by adding more cell
	// transversal leakage along fiber by infinite integral
	TotEne = 0.;
	TotDep = 0.;
	L2Ene  = 0.;
	EneSat = 0.;

	memset(CellDepCor, 0, sizeof(CellDepCor));

	for(int l=0; l<kNL; l++){
		for(int m=Cell_Min[l]; m<=Cell_Max[l]; m++){
			if( m<4 || m>75 ) CellDepCor[l][m] = CellEtr[l][m];
			else if( CellMask[l][m] ){
				CellDepCor[l][m] = CellEtr[l][m];
				EneSat += CellExp[l][m] - CellDep[l][m];
			}
			else if( CellExp[l][m] > 0 )	// cell-level energy correction
				CellDepCor[l][m] = CellDep[l][m]*CellEtr[l][m]/CellExp[l][m];
			//			cout << Form("%5.3f ", CellEtr[l][m]/CellExp[l][m]);
			TotEne += CellDepCor[l][m];
			TotDep += CellDep[l][m];
			if(l>=16) L2Ene += CellDepCor[l][m];
		}
	}

	//-->Last 2 Layers correction
	EleEne = TotEne/GetRearLeakageL2L(TotEne,L2Ene);

}

double Ecal3DRec::GetRearLeakageL2L(double totene,double l2ene){

	double rearcorr=1.0;
	double rearpar=1.0;

//-->Parameterization
	if(totene<1.0)
		rearpar=0.0;
	else if(totene>10000.0)
		rearpar=3.4;
	else
		rearpar=0.20+0.35*TMath::Log(totene);

//-->Last 2 layer correction
	if(totene<=0.0)
		rearpar=1.0;
	else
		rearcorr=1.08-rearpar*l2ene/totene;

	if(rearcorr<=0.0)
		rearcorr=0.333333;
	return rearcorr;
}


void Ecal3DRec::EMEstimatorVars(){
	// reset variables
	LkhdAxis = DepAxis = ExpAxis = 0.;
	LkhdCore = DepCore = ExpCore = 0.;
	LkhdTail = DepTail = ExpTail = 0.;
	LkhdBump = DepBump = ExpBump = 0.;
	NhitAxis = NhitCore = NhitTail = NhitBump = 0;
	E0M = E1M = E2M = 0.;
	Sx = Sy = 0;
	SizeX = SizeY = 0;
	memset(LayLkhd, 0, sizeof(LayLkhd));
	memset(LayNhit, 0, sizeof(LayNhit));
	memset(LayDep,  0, sizeof(LayDep) );
	memset(LayExp,  0, sizeof(LayExp) );
	memset(LaySize, 0, sizeof(LaySize));
	NhitAll = 0.; // Any cell
	EdepAll = 0.; // Any cell
	for(int l=0; l<kNL; l++){
		if( Cell_Med[l]<40 ){
			for(int c=Cell_Med[l]+1; c<kNC; c++){
				if(CellDep[l][c]>0){
					NhitAll+=2;
					EdepAll+=2*CellDep[l][c];
				}
			}
		}else{
			for(int c=0; c<Cell_Med[l]; c++){
				if(CellDep[l][c]>0){
					NhitAll+=2;
					EdepAll+=2*CellDep[l][c];
				}
			}
		}
		NhitAll++;
		EdepAll+=CellDep[l][Cell_Med[l]];
	}

	// calculate discriminate variables
	int cell_rad = 0;
	for(int l=0; l<kNL; l++){
		// check the largest radius
		cell_rad = TMath::Max( Cell_Med[l] - Cell_Min[l], Cell_Max[l] - Cell_Med[l] );
		for(int m=Cell_Med[l]-cell_rad; m<=Cell_Med[l]+cell_rad; m++){
			if( m>= Cell_Min[l] && m<=Cell_Max[l] ){ // no leak
				if(CellMask[l][m]==0 ){ 
					LayLkhd[l] += CellProbNorm[l][m];
					LayNhit[l] ++;
				}else{
					int m_m = 2*Cell_Med[l]-m,// mirror to shower axis, if not exceeding ECAL range
							l_m = l+1-2*(l%2),    // use the neighboring layer in the same superlayer
							l_n = l+1+(1+l)%2;    // use the next superlayer
					if( l>=16 )	l_n -= 4;
					int m_n = Cell_Med[l]+Cell_Med[l_n]-m, // same distance to shower axis
							m_n_m = 2*Cell_Med[l_n] - m_n;
					if( m_n<Cell_Min[l_n] || m_n>Cell_Max[l_n] ||m_n<4 || m_n>75 ){
						m_n = m_n_m; // mirror
						if( m_n <Cell_Min[l_n] || m_n>Cell_Max[l_n] ){
							m_n = 0;
							//cout << "--ERROR--EMEstimatorVars()--EXCEEDING NORMAL CELL RANGE"<<endl;
							//cout<<Form("l=%d, m=%d, l_n=%d, m_n=%d, Min=%d, Max=%d", l, m, l_n, m_n, Cell_Min[l_n], Cell_Max[l_n]) << endl;
						}
					}

					if( m==Cell_Med[l] ){ // on axis, can not use mirror
						if( CellMask[l_m][m]==0 ){
							LayLkhd[l] += CellProbNorm[l_m][m];
							LayNhit[l] ++;
						}else if( CellMask[l_n][m_n]==0 ){
							LayLkhd[l] += CellProbNorm[l_n][m_n];
							LayNhit[l] ++;
						}else if( CellMask[l_n+1][m_n]==0 ){
							LayLkhd[l] += CellProbNorm[l_n+1][m_n];
							LayNhit[l] ++;
						}else{
							//cout << "--WARNING--EMEstimatorVars()--CAN-NOT-RECOVER--ALGO--0--" << endl;
							//cout<<Form("l=%d, m=%d, l_n=%d, m_n=%d, Min=%d, Max=%d", l, m, l_n, m_n, Cell_Min[l_n], Cell_Max[l_n]) << endl;
						}
					}else{ // not on axis, use mirror
						if( m_m>=Cell_Min[l] && m_m<=Cell_Max[l] ){
							if( CellMask[l][m_m]==0 ){ // mirror in this layer
								LayLkhd[l] += CellProbNorm[l][m_m];
								LayNhit[l] ++;
							}else if( CellMask[l_m][m]==0 ){ // neighboring layer in the same super layer
								LayLkhd[l] += CellProbNorm[l_m][m];
								LayNhit[l] ++;
							}else if( CellMask[l_m][m_m]==0 ){ // mirror in n neighboring layer
								LayLkhd[l] += CellProbNorm[l_m][m_m];
								LayNhit[l] ++;
							}else if( CellMask[l_n][m_n]==0 ){ // next superlayer
								LayLkhd[l] += CellProbNorm[l_n][m_n];
								LayNhit[l] ++;
							}else if(CellMask[l_n+1][m_n]==0 ){ // next superlayer
								LayLkhd[l] += CellProbNorm[l_n+1][m_n];
								LayNhit[l] ++;
							}else{
								//cout << "--WARNING--EMEstimatorVars()--CAN-NOT-RECOVER--ALGO--1--" << endl;
								//cout << Form("l=%d,Med=%d,Min=%d,Max=%d, m=%d, m_m=%d, l_n=%d, m_n=%d", l, Cell_Med[l], Cell_Min[l], Cell_Max[l], m, m_m, l_n, m_n) << endl;
							}
						}
					}
				}
			} else { // leak, use mirror
				int m_m = 2*Cell_Med[l]-m,// mirror to shower axis, if not exceeding ECAL range
						l_m = l+1-2*(l%2),    // use the neighboring layer in the same superlayer
						l_n = l+1+(1+l)%2;    // use the next superlayer
				if( l>=16 )	l_n -= 4;
				int m_n = Cell_Med[l]+Cell_Med[l_n]-m, // same distance to shower axis
						m_n_m = 2*Cell_Med[l_n] - m_n;
				if( m_n<Cell_Min[l_n] || m_n>Cell_Max[l_n] ){
					m_n = m_n_m; // mirror
					if( m_n <Cell_Min[l_n] || m_n>Cell_Max[l_n] ){
						m_n = 0;
						//cout << "--ERROR--EMEstimatorVars()--EXCEEDING NORMAL CELL RANGE"<<endl;
						//cout<<Form("l=%d, m=%d, l_n=%d, m_n=%d, Min=%d, Max=%d", l, m, l_n, m_n, Cell_Min[l_n], Cell_Max[l_n]) << endl;
					}
				}

				if( m_m>=Cell_Min[l] && m_m<=Cell_Max[l] ){
					if( CellMask[l][m_m]==0 ){ // mirror in this layer
						LayLkhd[l] += CellProbNorm[l][m_m];
						LayNhit[l] ++;
					}else if( CellMask[l_m][m]==0 ){ // neighboring layer in the same super layer
						LayLkhd[l] += CellProbNorm[l_m][m];
						LayNhit[l] ++;
					}else if( CellMask[l_m][m_m]==0 ){ // mirror in n neighboring layer
						LayLkhd[l] += CellProbNorm[l_m][m_m];
						LayNhit[l] ++;
					}else if( CellMask[l_n][m_n]==0 ){ // next superlayer
						LayLkhd[l] += CellProbNorm[l_n][m_n];
						LayNhit[l] ++;
					}else if(CellMask[l_n+1][m_n]==0 ){ // next superlayer
						LayLkhd[l] += CellProbNorm[l_n+1][m_n];
						LayNhit[l] ++;
					}else{
						//cout << "--WARNING--EMEstimatorVars()--CAN-NOT-RECOVER--ALGO--2--" << endl;
					}
				}else{
					//cout << "--WARNING--EMEstimatorVars()--CAN-NOT-RECOVER--ALGO--3--" << endl;
				}
			}
		}
		// 
	}

	for(int l=0; l<kNL; l++){
		for(int m=Cell_Min[l]; m<=Cell_Max[l]; m++){
			LayDep[l]  += CellDep[l][m];
			LayExp[l]  += CellExp[l][m];

			if( (l/2)%2 ) Sx += CellDep[l][m];
			else          Sy += CellDep[l][m];

			for(int k=0; k<N_Shwr; k++){
				LaySize[l] += CellDep[l][m]*ShwrExp[k][l][m]*ShwrDist[k][l][m]/CellExp[l][m];
				if( (l/2)%2 ) SizeX += CellDep[l][m]*ShwrExp[k][l][m]*sqrt( ShwrDist[k][l][m]*ShwrDist[k][l][m] + ShwrDepth[k][l][m]*ShwrDepth[k][l][m] )/CellExp[l][m];
				else          SizeY += CellDep[l][m]*ShwrExp[k][l][m]*sqrt( ShwrDist[k][l][m]*ShwrDist[k][l][m] + ShwrDepth[k][l][m]*ShwrDepth[k][l][m] )/CellExp[l][m];
			}

			bool IsCore(0), IsTail(1), IsAxis(0);

			// Check all fitted showers
			for(int ish=0; ish<N_Shwr; ish++){
				if( m==Shwr_Med[ish][l] || m==Shwr_Med[ish][l]-1 || m==Shwr_Med[ish][l]+1 ){
					IsCore = 1;
					IsTail = 0;
				}
				//	if( m==Shwr_Med[ish][l] ) IsAxis = 1;
			}

			// Only check the most energetic shower
			//			if( m==Cell_Med[l] || m==Cell_Med[l]-1 || m==Cell_Med[l]+1 ){
			//				IsCore = 1;
			//				IsTail = 0;
			//			}
			if( m==Cell_Med[l] ) IsAxis = 1;

			if( IsAxis ){

				if( l>=2 ){
					if( CellMask[l][m]==0 ){
						LkhdAxis += CellProbNorm[l][m];
						NhitAxis ++;
					}
					if(CellMask[l][m]){
						DepAxis  += CellExp[l][m];
					}else{
						DepAxis  += CellDep[l][m];
					}
					ExpAxis  += CellExp[l][m];
				}
			}
			//			if( m==Cell_Med[l] || m==Cell_Med[l]-1 || m==Cell_Med[l]+1 )
			if( IsCore ){
				// core region
				NhitCore++;
				LkhdCore += CellProbNorm[l][m];
				if(CellMask[l][m]){ 
					DepCore  += CellExp[l][m];
				}else{
					DepCore  += CellDep[l][m];
				}
				ExpCore  += CellExp[l][m];
			} 
			if( IsTail ){
				// tail region
				NhitTail++;
				LkhdTail += CellProbNorm[l][m];
				if(CellMask[l][m]){
					DepTail  += CellExp[l][m];
				}else{
					DepTail  += CellDep[l][m];
				}
				ExpTail  += CellExp[l][m];

				if( CellDep[l][m] > 0.004 && CellDep[l][m] > CellExp[l][m] + 3*sqrt( CellExp[l][m]*CellExp[l][m]/CellBeta[l][m] ) ){
					// large bump, "neutrons"
					NhitBump++;
					LkhdBump += CellProbNorm[l][m];
					if(CellMask[l][m]){
						DepBump  += CellExp[l][m];
					}else{
						DepBump  += CellDep[l][m];
					}
					ExpBump  += CellExp[l][m];
				}
			}
		} // loop of cells

		int m = Cell_Med[l]; // Only the most energetic shower
		if( l==0 ){
			E0M += CellDep[l][m];
			if( CellDep[l][m+1] > CellDep[l][m-1] ) E0M += CellDep[l][m+1];
			else                                    E0M += CellDep[l][m-1];
		}else if( l==1 ){
			E1M += CellDep[l][m];
			if( CellDep[l][m+1] > CellDep[l][m-1] ) E1M += CellDep[l][m+1];
			else                                    E1M += CellDep[l][m-1];
		}else if( l==2 ){
			E2M += CellDep[l][m];
			if( CellDep[l][m+1] > CellDep[l][m-1] ) E2M += CellDep[l][m+1];
			else                                    E2M += CellDep[l][m-1];
		}
		if( LayDep[l] > 0 ) LaySize[l] /= LayDep[l];
	} // loop of layers
	if( Sx>0 ) SizeX /= Sx;
	if( Sy>0 ) SizeY /= Sy;

}

void Ecal3DRec::EMEstimatorLkhd(){
	int i, j, l;
	double p0, p1, p2, p3, p4, v, t, V=0, W=0;
	double r0, r1;
	double p1_0, p1_1, p1_2,
				 p2_0, p2_1, p2_2,
				 p3_0, p3_1, p3_2;
	double logene = log(EleEne);
	//0: Z0 
	//	p1 = 1.04;
	//	p2 = 1.27+0.749*logene+0.0416*logene*logene;//0.603 + 1.09*logene;
	//	p3 = 1.60;
	/*	v = -145 - Z0;
			p1 = 0.675+0.102*logene;
			p2 = 1.604+0.605*logene+0.058*logene*logene;
			p3 = 2.32-0.193*logene;
			if(p3<0.5) p3 = 0.5;
			*/
	if(N_Shwr<1){
		EmLkhd[0] = EmLkhd[1] = 0.;
		return ;
	}
	v = -Z0*(1+0.03*KX*KX)*(1+0.03*KY*KY);
	p1 = 0.971-0.347*exp(-0.191*EleEne);
	p2 = 146.5 + 0.704*logene + 0.0471*logene*logene;
	p3 = 2.0;
	v = p3*( exp(p1*(p2-v)) - p1*(p2-v) - 1.);
	EmVar[0] = v;
	V += v;

	// Rotate ecal_pr[0] and ecal_pr[1] to make them uncorrelated
	if(0&& LayDep[0] + LayDep[1] > 0 && LayExp[0] + LayExp[1] > 0 ){
		p0 = LayExp[0]/sqrt(LayExp[0]*LayExp[0]+LayExp[1]*LayExp[1]);
		p1 = LayExp[1]/sqrt(LayExp[0]*LayExp[0]+LayExp[1]*LayExp[1]);
		r0 = p1*LayDep[0] - p0*LayDep[1];
		r1 = p1*LayDep[0] + p0*LayDep[1];
		R0 = r0;
		R1 = r1;

		//1: R0 
		v = r0;
		p1 = TMath::Max( -0.0034, -0.0438 + 0.0167*logene ); // -0.0438 + 0.0167*logene;
		p2 = 0.0278+ 0.00967*logene;//0.0312 + 0.00842*logene;
		v = 0.5*(v-p1)*(v-p1)/p2/p2; // Gaussian distribution
		EmVar[1] = v;
		//		V += v;

		//2: R1 
		v = r1;
		p1 = 39.5*pow(EleEne,-0.24);//26.5 - 2.86*logene;
		p2 = 0.052*pow(EleEne, 0.28);//1/(12.2 - 1.49*logene);
		p3 = 0.5;
		v = p3*( exp(p1*(p2-v)) - p1*(p2-v) - 1.);
		EmVar[2] = v;
		//		V += v;
	} else {
		EmVar[1] = EmVar[2] = 99999.;
	}

	//3: LkhdAxis/NhitAxis
	v = log(1+LkhdAxis);
	p1 = 10.6 + 8.28*exp(-0.0431*EleEne);
	if(EleEne<10)
		p2 = 2.121-0.374*exp(-0.609*EleEne);
	else 
		p2 = 1.972+0.253*exp(-0.052*EleEne);
	p2 *= p1;
	v  = p2*log(p2) - p2 - p2*log(p1*v) + p1*v;
	EmVar[3] = v;
	V += v;

	//4: LkhdTail/NhitTail
	v = NhitTail>0?LkhdTail/NhitTail:99999.;
	p1 = 4.73 + 3.86*logene;
	p2 = EleEne<30?0.288-0.00256*(logene-4.16)*(logene-4.16):0.293-0.011*(logene-2.73)*(logene-2.73);
	if( p2<0.03 ) p2 = 0.03; 
	p3 = 3.0;
	v = p3*( exp(p1*(p2-v)) - p1*(p2-v) - 1.);
	EmVar[4] = v;
	V += v;

	//5: DepAxis/ExpAxis
	// 1/sqrt(2*pi) = 0.39894228;
	// exp(-1) = 0.367879441  	
	v = ExpAxis>0?DepAxis/ExpAxis:99999.;
	p1 = (60-3*logene)/(1+4.81*exp(-0.0459*EleEne));
	p2 = 1.017-0.276/(1+EleEne)+0.309*exp(-0.318*EleEne);
	p3 = 0.0153+0.496/(1+EleEne)-0.104*exp(-0.26*EleEne);
	p4 = 0.5;
	v = 0.39894228*p4/p3*exp(-0.5*(v-p2)*(v-p2)/p3/p3)+(1-p4)*p1*exp(p1*(v-p2)-exp(p1*(v-p2)));
	if( v <= 1e-200 ) 
		v = log(0.39894228*p4/p3+0.367879441*(1-p4)*p1) + 454;
	else
		v = log(0.39894228*p4/p3+0.367879441*(1-p4)*p1) - log(v);
	EmVar[5] = v;
	V += v;
	//	cout << DepAxis/ExpAxis << " " <<  p1 << " " << p2 << " " << p3 << " " << p4 << " " << v << endl;

	//6: DepTail/ExpTail
	v = ExpTail>0.?DepTail/ExpTail:99999.;
	p1 = 2.77-0.535*logene+0.182*logene*logene;
	p2 = 0.975+0.29*pow(EleEne, -0.565);
	p3 = 16.5/(1+60.*pow(1+EleEne, -1.6));
	v = p3*( exp(p1*(p2-v)) - p1*(p2-v) - 1.);
	EmVar[6] = v;
	V += v;

	//7: E0M + E1M: First 2 layer deposition
	/*
		 v = E0M + E1M;
		 if( E0M+E1M <= 0 ) 
		 v = 99999.;
		 p1 = 4.94+64.8*pow(1+EleEne, -0.716);
		 p2 = 3.5;
		 */
	v = (E0M + E1M)/(1+0.4*logene*(KX*KX+KY*KY)); // to correct angular dependence
	if( E0M+E1M <= 0 ) 
		v = 99999.;
	p1 = 2.84+ 40.5/sqrt(EleEne+1);
	p2 = 3.04/(1+0.65*exp(-0.31*EleEne));
	v = p2*log(p2) - p2 - p2*log(p1*v) + p1*v;
	EmVar[7] = v;
	V += v;

	// E2M
	/*	if(E2M>0) v = E2M;
			else      v = 99999.;
			p1 = 1.82+79.9*pow(EleEne+1.35, -0.732);
			p2 = (6.72-0.3*logene)/(1+1.11*exp(-0.76*EleEne));
			*/
	if(E2M>0) v = E2M/(1+0.4*logene*(KX*KX+KY*KY)); // to correct angular dependence
	else      v = 99999.;
	p1 = 0.847+24.2/sqrt(EleEne+2)+64.2/(EleEne+2);
	p2 = (5.94-0.381*logene)/(1+1.91*exp(-0.76*EleEne));
	v  = p2*log(p2) - p2 - p2*log(p1*v) + p1*v;
	EmVar[8] = v;
	V += v;
	/*
	// NhitBump
	p1 = 0.8;
	p2 = EleEne<30?7.1/(1+8.93*pow(EleEne+1, -2.58)):10.46-0.942*logene;
	p2 *= p1;
	if( NhitBump<1 ) v = 0.5;
	else             v = NhitBump;
	v = p2*log(p2) - p2 - p2*log(p1*v) + p1*v;
	EmVar[9] = v;
	*/
	// NhitAll
	//	NhitAll = NhitAll*(1+4e-5*(X0*X0+Y0*Y0));
	v = NhitAll;
	/*
		 p1 = 0.003 + 0.074/sqrt(EleEne);
		 p2 = 65.5 + 34.1*logene + 5.43*logene*logene;
		 p3 = 30/(1+40/EleEne);
		 v = p3*( exp(p1*(p2-v)) - p1*(p2-v) - 1.);
		 */
	p1 = -129 + 184*pow( EleEne, 0.193 );
	if(p1<15) p1 = 15;
	p2 = 0.061 + 0.068*exp(-0.235*EleEne) + 0.0644/sqrt(EleEne);
	p2 *= p1;
	v = 0.5*(v-p1)*(v-p1)/p2/p2; // Gaussian distribution
	EmVar[9] = v;
	V += v;
	//
	//==== Re-weight layer likelihoods
	// LayLkhd is fitted by function: f(x)=[0]*[1]*exp(-[1]*x)*pow([1]*x,[2])/TMath::Gamma([2]+1)
	// Integral f(x) = [0]
	// [1]: Scale factor
	// [2]: Degree of freedom

	//	double logene = TMath::Log( EleEne );
	memset( LayLkhdReNorm, 0, sizeof( LayLkhdReNorm ));
	p1_0 = 2.44/(1+2.51/(EleEne+0.775));
	p1_1 = EleEne<100?0.388+74*pow(EleEne+1,-1.38):0.464+0.17*(logene-5.14)*(logene-5.14);
	p1_2 = 7.57*pow(EleEne+1,-0.317);

	p2_0 = 0.207+0.566/sqrt(EleEne+1.65);
	p2_1 = 0.302+0.0306*(logene-2.75)*(logene-2.75);
	p2_2 = 1.75-0.0805*logene;
	for(l=2; l<18; l++){
		t = (Z0 - EcalZ[l])/(A0-Z0)+1; // depth/T0 + 1
		if( t < 0.3 ) t = 0.3;
		if( t > 3.5 ) t = 3.5;
		/*
		// use chi2 distribution, new tuning 2014-Jul-31
		//-> energy and depth dependence of parameter [1]
		// depth dependence: 
		// scale
		p1 = TMath::Max(1.0, 0.428+0.205*log(EleEne+1));
		// NDF
		if( EleEne < 50 )         
		p2_0 = 1.71 + 2.08*logene;
		else 
		p2_0 = 10.4 - 0.954*(logene-4.85)*(logene-4.85);

		p2_1 = 0.984+6.85/sqrt(EleEne+1)+17.7/(EleEne+1);
		p2_2 = 4.17-0.424*log(EleEne+1);
		if( p2_2 < 0.5 ) p2_2 = 0.5; // ~@6TeV
		// depth dependence
		p2 = p2_0/(1 + p2_1*exp(-p2_2*t) ); 
		if( t>1.75 ) 
		p2 = p2_0/(1 + p2_1*exp(-p2_2*(3.5-t)) );
		// ad-hoc correction for Y projection, increase efficiency for low energy
		if( l%4<2 )	p1 *= 0.6+(1-0.6)/15*(l-2);
		if( l<4 )	p1 *= 0.6+(1-0.6)/15*(l-2);
		// ad-hoc correction for layer 16 and 17
		if(l==16){
		p1 *= 0.9;
		p2 *= 1.05;
		}
		if(l==17){
		p1 *= 0.8;
		p2 *= 1.1;
		}

		p2 *= p1;

		v = LayLkhd[l];
		if( v <= 0 ) v = 9999999.;
		v = p2*log(p2) - p2 - p2*log(p1*v) + p1*v;
		*/
		// use gaussian distribution of log(LayLkhd), 2014-Jul-31
		//		p1_0 = EleEne<100?2.46/(1+2.72/(EleEne+1)):EleEne<300?2.41:4.48-0.363*logene;

		p1 = p1_0/(1+p1_1*exp(-t*p1_2));
		p2 = p2_0*(1+p2_1*(t-p2_2)*(t-p2_2));
		// ad-hoc corrections for lower energy, shower tail
		if( l>11+EleEne ){
			p1 -= 0.3*(l-11-EleEne);
		}
		// ad-hoc corrections for first layers
		if(l<6){ // recover efficiency due to back-splash
			p2 *= (1.0+0.005*logene*(6-l));
		}else if(l==17){
			p1 += 0.1;
			p2 *= 1.05;
		}

		if( LayLkhd[l]>0 ){
			v = log(LayLkhd[l]);
			v = 0.5*(v-p1)*(v-p1)/p2/p2; // Gaussian distribution
		}else{
			v=1000.;
		}

		LayLkhdReNorm[l] = v;
		W += v; // renormalized total likelihood
	}
	//EmVar[9] = W;

	EmLkhd[0] = V;
	EmLkhd[1] = W;

}

int Ecal3DRec::ShowerCombineFit(){
	int    l, m, k, n, Iter=0;
	double l_start, l_end;
	double L0, L1, L2;

	if ( N_Shwr>1 && (ShwrDep[0][0]>0 || ShwrDep[0][1]>0) ) {

		if(DEBUG) printf("Check footprint: Cell_Min[0]=%d, Cell_Max[0]=%d, Cell_Min[15]=%d, Cell_Max[15]=%d,\n Cell_Min[2]=%d, Cell_Max[2]=%d, Cell_Min[17]=%d, Cell_Max[17]=%d\n", Cell_Min[0], Cell_Max[0], Cell_Min[15], Cell_Max[15], Cell_Min[2], Cell_Max[2], Cell_Min[17], Cell_Max[17]);
		// define energy deposition and fluctuation for all the showers, the footprint is common for all the showers
		for (n=0; n<N_Shwr; n++) {
			if ( ShwrDep[n][0]>0 || ShwrDep[n][1]>0 ) EcalShwrDef(n);
			if ( ShwrDep[n][0]>0 ) N_Dep += 10;
			if ( ShwrDep[n][1]>0 ) N_Dep += 1;
		}

		Iter = 0;

shower_refit:
		//    printf("\n");

		Iter += 1;

		for (n=0; n<N_Shwr; n++) {

			if ( ShwrDep[n][0]>0 || ShwrDep[n][1]>0 ) {

				// E0
				E0  = ShwrPar[n][0][0];
				dE0 = ShwrPar[n][0][1];
				// B0
				B0  = ShwrPar[n][1][0];
				dB0 = ShwrPar[n][1][1];
				// A0
				A0  = ShwrPar[n][2][0];
				dA0 = ShwrPar[n][2][1];
				// X0
				X0  = ShwrPar[n][3][0];
				dX0 = ShwrPar[n][3][1];
				// Y0
				Y0  = ShwrPar[n][4][0];
				dY0 = ShwrPar[n][4][1];
				// Z0
				Z0  = ShwrPar[n][5][0];
				dZ0 = ShwrPar[n][5][1];
				// KX
				KX  = ShwrPar[n][6][0];
				dKX = ShwrPar[n][6][1];
				// KY
				KY  = ShwrPar[n][7][0];
				dKY = ShwrPar[n][7][1];

				I_Shwr = n;

				//				printf("-->ShowerCombineFit() Begin: Shower %02d, X0 %8.3f, KX %8.3f, Y0 %8.3f, KY %8.3f, Z0 %8.3f, E0 %8.3f+-%3.3f, A0 %8.3f, B0 %8.3f, L1=%8.3f, L2=%8.3f\n", n, X0, KX, Y0, KY, Z0, E0, dE0, A0, B0, ShwrLkhd1[I_Shwr], ShwrLkhd[I_Shwr]);

				// X0 fit
				//				if(DEBUG) printf("-->ShowerCombineFit(%d): Fit X0\n", I_Shwr);
				TransFit( X0,  dX0, L0, L1, L2);
				ShwrPar[n][3][0] =  X0;
				ShwrPar[n][3][1] = dX0;
				if(DEBUG)printf("-->ShowerCombineFit() Fit X0:Shower %02d, X0 %8.3f, KX %8.3f, Y0 %8.3f, KY %8.3f, Z0 %8.3f, E0 %8.2f, A0 %8.3f, B0 %8.3f, L0 %8.3f, L1 %8.3f, L2 %8.3f\n", n, X0, KX, Y0, KY, Z0, E0, A0, B0, L0, L1, L2);

				// KX fit
				//				if(DEBUG) printf("-->ShowerCombineFit(%d): Fit KX\n", I_Shwr);
				if ( ShwrDep[n][0] == 0 ) TransFit( KX, dKX, L0, L1, L2);
				else if ( n == 0 )         TransFitKx(L0, L1, L2); // only fit for first shower, commonly applied to other showers
				ShwrPar[n][6][0] =  KX;
				ShwrPar[n][6][1] = dKX;
				if(DEBUG)printf("-->ShowerCombineFit() Fit KX:Shower %02d, X0 %8.3f, KX %8.3f, Y0 %8.3f, KY %8.3f, Z0 %8.3f, E0 %8.2f, A0 %8.3f, B0 %8.3f, L0 %8.3f, L1 %8.3f, L2 %8.3f\n", n, X0, KX, Y0, KY, Z0, E0, A0, B0, L0, L1, L2);

				// Y0 fit
				//				if(DEBUG) printf("-->ShowerCombineFit(%d): Fit Y0\n", I_Shwr);
				TransFit( Y0,  dY0, L0, L1, L2);
				ShwrPar[n][4][0] =  Y0;
				ShwrPar[n][4][1] = dY0;
				if(DEBUG)printf("-->ShowerCombineFit() Fit Y0:Shower %02d, X0 %8.3f, KX %8.3f, Y0 %8.3f, KY %8.3f, Z0 %8.3f, E0 %8.2f, A0 %8.3f, B0 %8.3f, L0 %8.3f, L1 %8.3f, L2 %8.3f\n", n, X0, KX, Y0, KY, Z0, E0, A0, B0, L0, L1, L2);

				// KY fit
				//				if(DEBUG) printf("-->ShowerCombineFit(%d): Fit KY\n", I_Shwr);
				if ( ShwrDep[n][1] == 0 ) TransFit( KY, dKY, L0, L1, L2);
				else if ( n == 0 )         TransFitKy(L0, L1, L2);
				ShwrPar[n][7][0] =  KY;
				ShwrPar[n][7][1] = dKY;
				if(DEBUG)printf("-->ShowerCombineFit() Fit KY:Shower %02d, X0 %8.3f, KX %8.3f, Y0 %8.3f, KY %8.3f, Z0 %8.3f, E0 %8.2f, A0 %8.3f, B0 %8.3f, L0 %8.3f, L1 %8.3f, L2 %8.3f\n", n, X0, KX, Y0, KY, Z0, E0, A0, B0, L0, L1, L2);

				// E0 fit
				//				if( E0 < 6*dE0 ) dE0 = E0/10;
				//				if(DEBUG) printf("-->ShowerCombineFit(%d): Fit E0\n", I_Shwr);
				TransFit( E0,  dE0, L0, L1, L2);
				ShwrPar[n][0][0] =  E0;
				ShwrPar[n][0][1] = dE0;
				if(DEBUG)printf("-->ShowerCombineFit() Fit E0:Shower %02d, X0 %8.3f, KX %8.3f, Y0 %8.3f, KY %8.3f, Z0 %8.3f, E0 %8.2f, A0 %8.3f, B0 %8.3f, L0 %8.3f, L1 %8.3f, L2 %8.3f\n", n, X0, KX, Y0, KY, Z0, E0, A0, B0, L0, L1, L2);

				if ( n==0 ) {
					l_start = L0;
					for (k=1; k<N_Shwr; k++) if ( ShwrDep[k][0]>0 || ShwrDep[k][1]>0 ) l_start += ShwrLkhd[k];
				}

				// Z0 fit
				//				if(DEBUG) printf("-->ShowerCombineFit(%d): Fit Z0\n", I_Shwr);
				TransFit( Z0,  dZ0, L0, L1, L2);
				ShwrPar[n][5][0] =  Z0;
				ShwrPar[n][5][1] = dZ0;
				if(DEBUG)printf("-->ShowerCombineFit() Fit Z0:Shower %02d, X0 %8.3f, KX %8.3f, Y0 %8.3f, KY %8.3f, Z0 %8.3f, E0 %8.2f, A0 %8.3f, B0 %8.3f, L0 %8.3f, L1 %8.3f, L2 %8.3f\n", n, X0, KX, Y0, KY, Z0, E0, A0, B0, L0, L1, L2);
				/*
				//B0 fit
				TransFit( B0,  dB0, L0, L1, L2);
				ShwrPar[n][1][0] =  B0;
				ShwrPar[n][1][1] = dB0;
				*/
				// A0 fit
				if(DEBUG) printf("-->ShowerCombineFit(%d): Fit A0\n", I_Shwr);
				TransFit( A0,  dA0, L0, L1, L2);
				ShwrPar[n][2][0] =  A0;
				ShwrPar[n][2][1] = dA0;

				if(DEBUG)printf("-->ShowerCombineFit() Fit A0:Shower %02d, X0 %8.3f, KX %8.3f, Y0 %8.3f, KY %8.3f, Z0 %8.3f, E0 %8.2f, A0 %8.3f, B0 %8.3f, L0 %8.3f, L1 %8.3f, L2 %8.3f\n", n, X0, KX, Y0, KY, Z0, E0, A0, B0, L0, L1, L2);
				// copy expected values
				for (l=0; l<18; l++) {
					for (m=Cell_Min[l]; m<=Cell_Max[l]; m++) {
						ShwrExp[n][l][m] = CellExp[l][m];
						ShwrBeta[n][l][m] = CellBeta[l][m];
					}
				}

				ShwrLkhd[n] = L2; // restore boundary likelihoods

				//    ShowerDisplay(n);
			}
		}


		l_end = L1 - L2; // only the cell fluctuation
		for (k=0; k<N_Shwr; k++) if ( ShwrDep[k][0]>0 || ShwrDep[k][1]>0 ) l_end += ShwrLkhd[k]; // including boundary conditions

		if(DEBUG)printf("-->ShowerCombineFit(): Iteration %04d, Likelihood Start: %8.3f, End %8.3f\n", Iter, l_start, l_end);
		if ( l_start - l_end > MinDeltaLkhd && Iter < MaxIteration ) goto shower_refit;

		//  printf("Niter %d, l_start %8.3f, l_end %8.3f\n", k, l_start, l_end); 

		//
		// E0 - most energetic shower
		E0  = ShwrPar[0][0][0];
		dE0 = ShwrPar[0][0][1];
		// B0
		B0  = ShwrPar[0][1][0];
		dB0 = ShwrPar[0][1][1];
		// A0
		A0  = ShwrPar[0][2][0];
		dA0 = ShwrPar[0][2][1];
		// X0
		X0  = ShwrPar[0][3][0];
		dX0 = ShwrPar[0][3][1];
		// Y0
		Y0  = ShwrPar[0][4][0];
		dY0 = ShwrPar[0][4][1];
		// Z0
		Z0  = ShwrPar[0][5][0];
		dZ0 = ShwrPar[0][5][1];
		// KX
		KX  = ShwrPar[0][6][0];
		dKX = ShwrPar[0][6][1];
		// KY
		KY  = ShwrPar[0][7][0];
		dKY = ShwrPar[0][7][1];

		// re-define footprint
		//  ShowerFootprint();

		for (k=0; k<N_Shwr; k++) {

			// define shower, calculate expected energy map using the fitted parameters and common footprint
			EcalShwrDef(k);

			for (l=0; l<18; l++) {
				for (m=Cell_Min[l]; m<=Cell_Max[l]; m++) {
					ShwrEtr[k][l][m] = CellEtr[l][m];
				}
			}

		}

	}
	return 0;
}

//==================================================================
int Ecal3DRec::EcalShwrDef(int k, int flag) {
	// by default, flag=0
	// if flag==1, re-define footprint, calling ShowerFootprint()
	// calculate energy deposition for given shower
	int l, m;

	// E0
	E0  = ShwrPar[k][0][0];
	dE0 = ShwrPar[k][0][1];
	// B0
	B0  = ShwrPar[k][1][0];
	dB0 = ShwrPar[k][1][1];
	// A0
	A0  = ShwrPar[k][2][0];
	dA0 = ShwrPar[k][2][1];
	// X0
	X0  = ShwrPar[k][3][0];
	dX0 = ShwrPar[k][3][1];
	// Y0
	Y0  = ShwrPar[k][4][0];
	dY0 = ShwrPar[k][4][1];
	// Z0
	Z0  = ShwrPar[k][5][0];
	dZ0 = ShwrPar[k][5][1];
	// KX
	KX  = ShwrPar[k][6][0];
	dKX = ShwrPar[k][6][1];
	// KY
	KY  = ShwrPar[k][7][0];
	dKY = ShwrPar[k][7][1];

	//	if( flag==1 ) ShowerFootprint(); // redefine footprint

	ShowerModel(0, flag); // default lateral parameters

	// copy expected values
	for (l=0; l<18; l++) {
		for (m=Cell_Min[l]; m<=Cell_Max[l]; m++) {
			ShwrExp[k][l][m] = CellExp[l][m];
			ShwrBeta[k][l][m] = CellBeta[l][m];
			if(flag==1){ // after all the fits done
				ShwrEtr[k][l][m] = CellEtr[l][m];
				ShwrDepth[k][l][m] = CellDepth[l][m];
				ShwrDist[k][l][m] = CellDist[l][m];
			}
		}
	}
	return 0;
}
