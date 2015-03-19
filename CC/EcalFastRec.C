
//! EcalFastRec class

/*!
 * Class for fast reconstruction of ECAL Shower
 * Two corrections applied:
 * 		1. Signal time dependent calibration
 * 		2. Attenuation correction
 * Input:   AMS Event
 * Output:  Energy estimation
 * \author: Hai Chen(hachen@cern.ch), Andrei Kounine, Zuhao Li, Weiwei Xu, Cheng Zhang
*/

#include <iostream>
#include "TString.h"
#include "TMath.h"
#include "EcalFastRec.h"

using namespace std;
using namespace Ecal3DRecDB;

//==================================================================
//-->EcalFastRec Class Definition
//==================================================================

//==================================================================
//-->Reset Cell Status
//==================================================================
int EcalFastRec::Reset(){

	Z0 = X0 = Y0 = KX = KY = 0;
	N_Shwr = 0;

	memset( Status, 0, sizeof(Status) );
	memset( Edep, 0, sizeof(Edep) );
	memset( EdepAttC, 0, sizeof(EdepAttC) );
	memset( LayAmp, 0, sizeof(LayAmp) );
	memset( LayCrd, 0, sizeof(LayCrd) );

	memset(ShwrX0, 0, sizeof(ShwrX0));
	memset(ShwrY0, 0, sizeof(ShwrY0));
	memset(ShwrZ0, 0, sizeof(ShwrZ0));
	memset(ShwrKX, 0, sizeof(ShwrKX));
	memset(ShwrKY, 0, sizeof(ShwrKY));
	memset(ShwrNhits, 0, sizeof(ShwrNhits));

	return 0;
}

//==================================================================
//-->Read ADC Map from AMSEvent
//==================================================================
int EcalFastRec::ReadADCMapFromAMSEvent(AMSEventR *pev){

	if(!pev) return -1;

	memset( ADCH, 0, sizeof(ADCH) );
	memset( ADCL, 0, sizeof(ADCL) );
	memset( PedH, 0, sizeof(PedH) );
	memset( PedL, 0, sizeof(PedL) );

	Time = pev->UTime();
	//-->DataType 
	if( pev->nMCEventg() > 0 ){ 
		DataType = REALMC;
	}else{
		if( Time < 1305759600 ) DataType = BTDATA;
		else                    DataType = ISSDATA;
	}

	//-->ADC/Ped Array
	if(pev->nEcalHit()<=0) return -2;
	for(Int_t ihit=0; ihit<pev->nEcalHit(); ihit++){
		phit = pev->pEcalHit(ihit);
		ADCH[phit->Plane][phit->Cell] = phit->ADC[0];
		ADCL[phit->Plane][phit->Cell] = phit->ADC[1];
		PedH[phit->Plane][phit->Cell] = phit->GetECALPed( phit->Plane, phit->Cell, 0 );
		PedL[phit->Plane][phit->Cell] = phit->GetECALPed( phit->Plane, phit->Cell, 1 );
	}
	return 0;
}

//==================================================================
//-->Read ADC Map from ToyEvent
//==================================================================
int EcalFastRec::ReadADCMapFromToyEvent(Double_t _time, Float_t _edep[][nCell], Int_t _stat[][nCell]){
	Time = _time;
	DataType = TOYMC;
	memcpy( Edep, _edep, sizeof(Edep) );
	memcpy( Status, _stat, sizeof(Status) );
	return 0;
}


//==================================================================
//-->Define Cell Status
//==================================================================
int EcalFastRec::DefineCellStatus(){

	for(int l=0; l<kNL; l++){
		for(int c=0; c<nCell; c++){
			// Bad cell
			if( Time > 1308130000 && (l==6||l==7) && (c==16||c==17) ) Status[l][c] |= DEADCELL;
			if( (l==14)&&(c==65) ) Status[l][c] |= DEADCELL;
			// Saturation
			if( ADCH[l][c] + PedH[l][c] > ADCHighGainSatu ) Status[l][c] |= HIGHGAINSATU;
			if( ADCL[l][c] + PedL[l][c] > ADCLowGainSatu  ) Status[l][c] |= LOWGAINSATU;
		}
	}

	return 0;
}

//==================================================================
//-->Perform Gain Calibration:
//----->Dynamic High Gain
//----->Static High/Low Gain Ratio
//==================================================================
int EcalFastRec::GainCalibration(){

	for(int l=0; l<kNL; l++){
		for(int c=0; c<nCell; c++){
			if( ADCH[l][c]>0 || ADCL[l][c]>0 ){
				if( Status[l][c] & (DEADCELL|PEDESTALERROR|HLGAINERROR) ) 
					Edep[l][c] = 0.;
				else if( Status[l][c] & HIGHGAINSATU ) // use low gain
					Edep[l][c] = ADCL[l][c]*GetGainRatioCorr(l,c);
				else Edep[l][c] = ADCH[l][c];

				Edep[l][c] = Edep[l][c]*ADC2MeV_HC/GetHighGainCorr(l, c)*0.001; // ADC->MeV->GeV
			}
		}
	}

	return 0;
}

//==================================================================
//-->Initial Database
//==================================================================
int EcalFastRec::InitDataBase(Double_t asktime){
	//---
	int tdvstat=0;

	int realdata=(DataType==ISSDATA || DataType==BTDATA);

	tdvstat=EcalTDVManager::GetHead(realdata)->Validate(asktime);
	if(tdvstat!=0)cerr<<">>>>>>>>>Error EcalCalibration TDV Par Init Error<<<<<<<<"<<endl;

	return tdvstat;
}

//==================================================================
//-->Get High Gain correction factor
//==================================================================
float EcalFastRec::GetHighGainCorr(int layer, int cell){
	////////////////////////////////
	//---->HighGain Equilization
	////////////////////////////////

	if(EcalEquilPar::Head==0)
		cerr<<">>>>>>>>Error EcalEquilPar Head not Initial !!!!<<<<<<<<"<<endl;
	EcalEquilPar *Equil=EcalEquilPar::GetHead();
	if(!Equil->Isload){
		cerr<<">>>>>>>>Error EcalEquilPar not Load yet!!!! <<<<<<<<"<<endl; 
		return -1;
	}

	integer tdvid=layer*100+cell;
	float equilpar=Equil->equil[tdvid];

	if(equilpar>0.0){
		return equilpar;
	}
	else{
		bool bad1=(layer==6)&&(cell==16);
		bool bad2=(layer==6)&&(cell==17);
		bool bad3=(layer==7)&&(cell==16);
		bool bad4=(layer==7)&&(cell==17);
		bool bad5=(layer==14)&&(cell==64);
		bool bad6=(layer==14)&&(cell==65);
		bool bad7=(layer==15)&&(cell==64);
		bool bad8=(layer==15)&&(cell==65);

		if(!(bad1 || bad2 || bad3 || bad4 || bad5 || bad6 || bad7 || bad8))
			cerr<<">>>>>>>>High Gain Time Dependence Correction not valid!!!<<<<<<<<"<<endl;
		return 1.0;
	}
}

//==================================================================
//-->Get High/Low gain ratio factor
//==================================================================
float EcalFastRec::GetGainRatioCorr(int layer, int cell){
	////////////////////////////////
	//---->GainRatio correction
	////////////////////////////////

	if(EcalGainRatioPar::Head==0)
		cerr<<">>>>>>>>>Error EcalGainRatioPar Head not Initial !!!!<<<<<<<<<<"<<endl;

	EcalGainRatioPar *GainRatioPar=EcalGainRatioPar::GetHead();
	if(!GainRatioPar->Isload)
		cerr<<">>>>>>>>>Error EcalGainRatioPar is not loaded yet !!!!<<<<<<<<<"<<endl;

	integer tdvid=layer*100+cell;
	float gainratiopar=GainRatioPar->gain[tdvid]; // No offset for gain ratio 

	if(gainratiopar>0.0){
		return gainratiopar;
	}
	else{
		bool bad1=(layer==6)&&(cell==16);
		bool bad2=(layer==6)&&(cell==17);
		bool bad3=(layer==7)&&(cell==16);
		bool bad4=(layer==7)&&(cell==17);
		bool bad5=(layer==14)&&(cell==64);
		bool bad6=(layer==14)&&(cell==65);
		bool bad7=(layer==15)&&(cell==64);
		bool bad8=(layer==15)&&(cell==65);

		if(!(bad1 || bad2 || bad3 || bad4 || bad5 || bad6 || bad7 || bad8))
			cerr<<">>>>>>>>Gain Ratio Time Dependence Correction not valid!!!<<<<<<<<"<<endl;
		return 33.0;
	}

}


//==================================================================
int EcalFastRec::ClearCellStatus(Int_t l, Int_t c, Int_t stat_ask){
	Status[l][c] &= ~stat_ask;
	return 0;
}

//==================================================================
int EcalFastRec::CheckCellStatus(Int_t l, Int_t c, Int_t stat_ask){
	return (Status[l][c] & stat_ask)>0;
}

//==================================================================
int EcalFastRec::ClearAllCellStatus(Int_t stat_ask){
	for(int l=0; l<kNL; l++){
		for(int c=0; c<nCell; c++){
			Status[l][c] &= ~stat_ask;
		}
	}
	return 0;
}

//==================================================================
//-->Find Shower
//==================================================================
int EcalFastRec::ShowerSeedFinding(Int_t iter){

	Double_t v, val;
	Int_t lx, mx, ly, my, nm, nl;
	Int_t l, m;
	KX=0;
	KY=0;
	Int_t nKY=0, nKX=0;
	Int_t nw=3;

	// seed layer in X projection
define_mX:

	v  = Cutoff_MIP;// maximum energy
	lx = -1; // lay
	mx = -1; // cell

	for (l=0; l<18; l++) {
		SeedCell[l] = -1;
		if( l%4 < 2 ) continue;
		for (m=0; m<72; m++) {
			if ( EdepBuffer[l][m]>v ) {
				v = EdepBuffer[l][m];
				lx = l;
				mx = m;
			}
		}
	}

	// check the consistence of the found X seed
	if ( lx==2 || lx==15) nm = 1;
	else                  nm = 0;
	if ( mx >= 0 ) {
		val = 0.;
		if( lx/2 == l/2 ) nw = 1;
		else              nw = 3;

		for (l=2; l<16; l+=4) {
			for (m=TMath::Max(0,mx-nw); m<=TMath::Min(71,mx+nw); m++){
				if ( ((l != lx) || (m != mx)) && EdepBuffer[l][m]>0. ){
					nm  += 1;
					val += EdepBuffer[l][m];
				}
			}
			for (m=TMath::Max(0,mx-nw); m<=TMath::Min(71,mx+nw); m++) {
				if ( (((l+1) != lx) || (m != mx)) && EdepBuffer[l+1][m]>0. ) {
					nm  += 1;
					val += EdepBuffer[l+1][m];
				}
			}
		}
		if ((nm < 4) || (v > 2*val) ) {
			EdepBuffer[lx][mx] = Cutoff_MIP;
			goto define_mX;
		}
	}
	fEX = nm;	

	// define X footprint
	if ( mx >= 0 ) {
		dEX = v;
		// define KX use the two layers in the same superlayer of seed
		v=0.;

		// go upstream
		nm = mx;
		for (l=(lx&0xFE)+1; l>1; l-=4) {
			v = 0.;
			//			if( nKX>0 ) mx = (l-lx)*KX+SeedCell[lx];
			for (m=mx-15; m<=mx+15; m++) {
				if ( m>=0 && m<=71 && EdepBuffer[l][m]>v ) {
					v = EdepBuffer[l][m];
					SeedCell[l] = m;
				}
			}
			if ( SeedCell[l]   > 0 ) mx = SeedCell[l];

			v = 0.;
			for (m=mx-4; m<=mx+4; m++) {
				if ( m>=0 && m<=71 && EdepBuffer[l-1][m]>v ) {
					v = EdepBuffer[l-1][m];
					SeedCell[l-1] = m;
				}
			}

			if ( SeedCell[l-1] > 0 ) mx = SeedCell[l-1];
		}

		// go downstream
		mx = nm;
		for (l=(lx&0xFE)+4; l<16; l+=4) {
			v = 0.;
			for (m=mx-15; m<=mx+15; m++) {
				if ( m>=0 && m<=71 && EdepBuffer[l][m]>v ) {
					v = EdepBuffer[l][m];
					SeedCell[l] = m;
				}
			}
			if ( SeedCell[l]   > 0 ) mx = SeedCell[l];

			v = 0.;
			for (m=mx-4; m<=mx+4; m++) {
				if ( m>=0 && m<=71 && EdepBuffer[l+1][m]>v ) {
					v = EdepBuffer[l+1][m];
					SeedCell[l+1] = m;
				}
			}

			if ( SeedCell[l+1] > 0 ) mx = SeedCell[l+1];
		}
		mx = nm;                                            // restore mx
	} else {
		dEX = 0.;
	}

	//
	// maximal amplitudes in Y-projection
	//

define_mY:
	ly = -1;
	my = -1;
	v  = Cutoff_MIP;

	for (l=0; l<18; l+=4) {
		SeedCell[l]   = -1;
		SeedCell[l+1] = -1;
		for (m=0; m<72; m++) {
			if ( EdepBuffer[l][m]>v ) {
				v = EdepBuffer[l][m];
				ly = l;
				my = m;
			}
			if ( EdepBuffer[l+1][m]>v ) {
				v = EdepBuffer[l+1][m];
				ly = l+1;
				my = m;
			}
		}
	}

	// consistency of the found Y maximum 
	if ( ly==0 || ly==17) nm = 1;
	else                  nm = 0;
	nm = 0;
	if ( my >= 0 ) {
		val = 0.;
		for (l=0; l<18; l+=4) {
			if( ly/2 == l/2 ) nw = 1;
			else nw = 3;	
			for (m=TMath::Max(0,my-nw); m<=TMath::Min(71,my+nw); m++) {
				if ( (l != ly) || (m != my) ) {
					if ( EdepBuffer[l][m]>0. ) {
						nm  += 1;
						val += EdepBuffer[l][m];
						//							printf("LY %02d, MY %02d, E %8.3f\n", l, m, EdepBuffer[l][m]);
					}
				}
			}
			for (m=TMath::Max(0,my-nw); m<=TMath::Min(71,my+nw); m++) {
				if ( (((l+1) != ly) || (m != my)) && EdepBuffer[l+1][m]>0. ) {
					nm  += 1;
					val += EdepBuffer[l+1][m];
					//printf("LY %02d, MY %02d, E %8.3f\n", l+1, m, EdepBuffer[l][m]);
				}
			}
		}
		if ((nm < 4) || (v > 2*val) ) {
			EdepBuffer[ly][my] = Cutoff_MIP;
			//			printf("LY %02d, MY %02d, Nhits %02d, Emax %8.3f, Eout %8.3f\n", ly, my, nm, v, val);
			goto define_mY;
		}
	}
	fEY = nm;

	// define Y footprint
	if ( my >= 0 ) {
		dEY = v;
		// define KY use the two layers in the same superlayer of seed
		v=0.;

		// go upstream
		nm = my;
		for (l=(ly&0xFE)+1; l>0; l-=4) {
			v = 0.;
			//			if( nKY>0 ) my = (l-ly)*KY+SeedCell[ly];// need extrapolation in case of large angle
			//			cout << l << ": " << SeedCell[ly] << ", " << my << endl;
			for (m=my-15; m<=my+15; m++) {
				if ( m>=0 && m<=71 &&EdepBuffer[l][m]>v ) {
					v = EdepBuffer[l][m];
					SeedCell[l] = m;
				}
			}
			if ( SeedCell[l]   > 0 ) my = SeedCell[l];
			//			cout << "Layer=" << l << ", my=" << my << ", v= "<< v << endl;

			v = 0.;
			for (m=my-4; m<=my+4; m++) {
				if ( m>=0 && m<=71 &&EdepBuffer[l-1][m]>v ) {
					v = EdepBuffer[l-1][m];
					SeedCell[l-1] = m;
				}
			}

			if      ( SeedCell[l-1] > 0 ) my = SeedCell[l-1];
			//			cout << "Layer=" << l-1 << ", my=" << my << ", v= "<< v << endl;
			//			else if ( SeedCell[l]   > 0 ) my = SeedCell[l];
		}

		// go downstream
		my = nm;
		for (l=(ly&0xFE)+4; l<18; l+=4) {
			v = 0.;
			//			if( nKY>0 ) my = (l-ly)*KY+SeedCell[ly];// need extrapolation in case of large angle
			//			cout << l << ": " << SeedCell[ly] << ", " << my << endl;
			for (m=my-15; m<=my+15; m++) {
				if ( m>=0 && m<=71 &&EdepBuffer[l][m]>v ) {
					v = EdepBuffer[l][m];
					SeedCell[l] = m;
				}
			}
			if ( SeedCell[l]   > 0 ) my = SeedCell[l];

			v = 0.;
			for (m=my-4; m<=my+4; m++) {
				if ( m>=0 && m<=71 &&EdepBuffer[l+1][m]>v ) {
					v = EdepBuffer[l+1][m];
					SeedCell[l+1] = m;
				}
			}

			if( SeedCell[l+1] > 0 ) my = SeedCell[l+1];
		}
		my = nm;
	} else {
		dEY = 0.;
	}

	//	XSeed = mx; // to be used in DefineFootprint
	//	YSeed = my;
	
	return 0;
	
}

//==================================================================
//-->Do Clusterization
//==================================================================
int EcalFastRec::ShowerClustering(Int_t iter){

	memset(Cell_Min, 0, sizeof(Cell_Min));
	memset(Cell_Max, 0, sizeof(Cell_Max));

	EX = EY = 0.;
	for(Int_t l=0; l<kNL; l++){
		if(SeedCell[l]>=0){
			// search right
			if( SeedCell[l]==71 ) Cell_Max[l] = 71;
			else {
				for(Int_t m=SeedCell[l]+1; m<72; m++){
					if( EdepBuffer[l][m] <= 0 && EdepBuffer[l][m-1]<=0 ){
						Cell_Max[l] = m;
						break;
					}else if( m>=2 && EdepBuffer[l][m]>1.5*EdepBuffer[l][m-1] && EdepBuffer[l][m-1]>1.2*EdepBuffer[l][m-2] && EdepBuffer[l][m]>Cutoff_SEED && EdepBuffer[l][m-2]>0){
						//					cout << "m=" << m <<", "<< EdepBuffer[l][m] << ", m-1=" << EdepBuffer[l][m-1] << ", m-2=" << EdepBuffer[l][m-2] << endl;
						Cell_Max[l] = m-2;
						break;
					}else if( m==71 ) 
						Cell_Max[l] = 71;
				}
			}

			// search left
			if( SeedCell[l]==0 ) Cell_Min[l] = 0;
			else {
				for(Int_t m=SeedCell[l]-1; m>=0; m--){
					if( EdepBuffer[l][m] <= 0 && EdepBuffer[l][m+1]<=0 ){
						Cell_Min[l] = m+1;
						break;
					}else if( m<=69 && EdepBuffer[l][m]>1.5*EdepBuffer[l][m+1] && EdepBuffer[l][m+1]>1.2*EdepBuffer[l][m+2] && EdepBuffer[l][m]>Cutoff_SEED && EdepBuffer[l][m+2]>0){
						//					cout << "m=" << m <<", "<< EdepBuffer[l][m] << ", m+1=" << EdepBuffer[l][m+1] << ", m+2=" << EdepBuffer[l][m+2] << endl;
						Cell_Min[l] = m+2;
						break;
					}else if( m==0 )
						Cell_Min[l] = 0;
				}
			}
			for(Int_t m=Cell_Min[l]; m<=Cell_Max[l]; m++){
				if( EdepBuffer[l][m]>0 ){
					if( l%4<2 ) EY += EdepBuffer[l][m];
					else        EX += EdepBuffer[l][m];
					//					cout << "l=" << l << ", m=" << m << ", e=" << EdepBuffer[l][m] << endl;
				}
			}
		}
		//cout << "iter=" << iter << ", N_Shwr=" << N_Shwr << ", l=" << l << ", Seed=" << SeedCell[l] << ", CMIN=" << Cell_Min[l] << ", CMAX=" << Cell_Max[l] << endl;
	}
	return 0;
}

//==================================================================
int EcalFastRec::CalculateLayerCOG(Int_t iter){
	fX0 = fX1 = 0;
	fY0 = fY1 = 0;
	Int_t l, nm=0;
	for (l=0; l<18; l++) {
		nm = SeedCell[l];
		//		cout << nm << endl;
		if ( nm >= 0 ) {
			if( nm > 0 && nm<71 ){
				LayAmp[l] = EdepBuffer[l][nm-1] + EdepBuffer[l][nm] + EdepBuffer[l][nm+1];
				LayCrd[l] = Cell_Size*((nm-36.5)*EdepBuffer[l][nm-1] + (nm-35.5)*EdepBuffer[l][nm] + (nm-34.5)*EdepBuffer[l][nm+1])/LayAmp[l];
			}else if(nm==0){
				LayAmp[l] = EdepBuffer[l][nm] + EdepBuffer[l][nm+1];
				LayCrd[l] = ((nm-35.5)*EdepBuffer[l][nm] + (nm-34.5)*EdepBuffer[l][nm+1])/LayAmp[l];
			}else if(nm==71){
				LayAmp[l] = EdepBuffer[l][nm-1] + EdepBuffer[l][nm];
				LayCrd[l] = Cell_Size*((nm-36.5)*EdepBuffer[l][nm-1] + (nm-35.5)*EdepBuffer[l][nm])/LayAmp[l];
			}
			// check the quality of the individual projections using the remaining energy depositions
			//		cout << nm << ", EdepBuffer[l][nm]=" << EdepBuffer[l][nm] << ", 0.8*EdepBuffer[l][nm+1]=" << 0.8*EdepBuffer[l][nm+1] << endl;
			if ( (nm>0 && nm<71 && EdepBuffer[l][nm]>0.8*EdepBuffer[l][nm-1] && EdepBuffer[l][nm]>0.8*EdepBuffer[l][nm+1] )
					|| (nm==0 && EdepBuffer[l][nm]>0.8*EdepBuffer[l][nm+1] )
					||  (nm==71 && EdepBuffer[l][nm]>0.8*EdepBuffer[l][nm-1] )
				 ) {
				if ( (l/2)%2 == 1 ) fX1 += 1;
				else                fY1 += 1;
			}
			if( iter==0 ){
				if ( (nm>0 && nm<71 && Edep[l][nm]>0.8*Edep[l][nm-1] && Edep[l][nm]>0.8*Edep[l][nm+1] )
						|| (nm==0 && Edep[l][nm]>0.8*Edep[l][nm+1] )
						||  (nm==71 && Edep[l][nm]>0.8*Edep[l][nm-1] )
					 ) { // original energy map
					if ( (l/2)%2 == 1 ) fX0 += 1;
					else                fY0 += 1;
				}
			}else{
				if ( (nm>0 && nm<71 && EdepAttC[l][nm]>0.8*EdepAttC[l][nm-1] && EdepAttC[l][nm]>0.8*EdepAttC[l][nm+1] )
						|| (nm==0 && EdepAttC[l][nm]>0.8*EdepAttC[l][nm+1] )
						||  (nm==71 && EdepAttC[l][nm]>0.8*EdepAttC[l][nm-1] )
					 ) { // original energy map
					if ( (l/2)%2 == 1 ) fX0 += 1;
					else                fY0 += 1;
				}
			}
		} else {
			LayAmp[l] = 0.;
			LayCrd[l] = 0.;
		}
	}
	return 0;
}

//==================================================================
//-->Get Shower maximum
//==================================================================
int EcalFastRec::ParabolaZ0Fit(){
	//
	// Define coordinate of the shower maximum
	//
	//	fZ0 = 1;
	//	fB0 = 1;

	Int_t l, nm = 0;
	Double_t v  = 0.;
	for (l=0; l<18; l++) {
		if ( LayAmp[l] > v ) {
			v  = LayAmp[l];
			nm = l;
		}
	}

	// calculate inverse matrix a_ij
	Double_t m11, m12, m22, m23, m33, b1, b2, b3, a11, a12, a13, a22, a23, a33, det;
	Double_t val, arg;
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

		//		fZ0 = 0;
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
//-->Linear Fit of shower axis
//==================================================================
int EcalFastRec::ShowerAxisFit(){
	vector<Double_t> vx, vz, vw;
	// fit XZ
	for(Int_t l=0; l<18; l++){
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

	vx.clear();
	vz.clear();
	vw.clear();
	for(Int_t l=0; l<18; l++){
		if( l%4<2 && SeedCell[l]>0 ){
			vx.push_back( LayCrd[l] );
			vz.push_back( EcalZ[l]-Z0 );
			vw.push_back( LayAmp[l] );
			//cout << "l=" << l << ", LayCrd=" << LayCrd[l] << ", LayAmp=" << LayAmp[l] << endl;
		}
	}
	if( vx.size() > 1 )
		LinearFit(vx.size(), vx, vz, vw, KY, Y0);
	else 
		KY = Y0 = 0.;

	return 0;
}

//==================================================================
int EcalFastRec::LinearFit(Int_t n, vector<Double_t> vx, vector<Double_t> vy, vector<Double_t> vw, Double_t &kx, Double_t &x0){

	Double_t m11, m12, m22, b1, b2, det;
	m11 = m12 = m22 = b1 = b2 = 0.;

	for(Int_t i=0; i<n; i++){
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
	return 0;
}

//==================================================================
//--> Do Attenuation correction
//==================================================================
int EcalFastRec::AttenuationCorrection(){
	float attcorr=0.0;
	float attpar[3];
	double posfiber=0.;
	Nhits=0;
	for(int l=0;l<18;l++){
		for(int c=Cell_Min[l];c<=Cell_Max[l];c++){
			if(CheckCellStatus(l, c, USED)){
				//				cout << "l=" << l << ", c=" << c << " USED " << endl;
				continue;
			}
			if(Edep[l][c]<=0){
				//				cout << "l=" << l << ", c=" << c << " ZERO" << endl;
				continue;
			}
			GetAttenuationCorr(l, c, attpar);
			if(l/2%2==0)posfiber = X0 + KX*(EcalZ[l] - Z0);//Y Projection
			else posfiber = Y0 + KY*(EcalZ[l] - Z0);        //X Projection

			attcorr=attpar[0]*TMath::Exp(-1.0*posfiber/attpar[1])+(1.0-attpar[0])*TMath::Exp(-1.0*posfiber/attpar[2]);
			if(attcorr<=0.0) attcorr = 1.0;
			EdepAttC[l][c] = Edep[l][c]/attcorr;

			Status[l][c] |= USED;
			Nhits++;
		}
	}
	return 0;
}


//==================================================================
int EcalFastRec::GetAttenuationCorr(int layer, int cell, float attpar[3]){

	attpar[0]=0.0;
	attpar[1]=0.0;
	attpar[2]=0.0;

	////////////////////////////////
	//---->Attenuation Correction
	////////////////////////////////
	if(EcalAttPar::Head==0)
		cerr<<">>>>>>>>Error EcalAttPar Head not Initial !!!!<<<<<<<<"<<endl;
	EcalAttPar *Att=EcalAttPar::GetHead();
	if(!Att->Isload){
		cerr<<">>>>>>>>Error EcalAttPar not Load yet!!!! <<<<<<<<"<<endl; 
	}

	integer tdvid=layer*100+cell;
	attpar[0]=Att->attpar[0][tdvid];
	attpar[1]=Att->attpar[1][tdvid];
	attpar[2]=Att->attpar[2][tdvid];

	if(attpar[2]==0.0){
		bool bad=(layer==14)&&(cell==65);
		if(!(bad)){
			cerr<<">>>>>>>>Attenuation Correction not valid!!!<<<<<<<<"<<endl;
			cout<<"layer: "<<layer<<"  cell: "<<cell<<endl;
		}
	}
	return 0;
}


//==================================================================
int EcalFastRec::IsolatedHitsAttCor(){
	float posfiber;
	float hlfiber = 36*0.9;
	float fattcorr;
	float attpar[3];
	Double_t dist, mindist=FLT_MAX;
	Double_t d1, d2;
	double attcorr;

	for(int l=0;l<18;l++){
		for(int c=0;c<=71;c++){
			if( Edep[l][c]<=0 || CheckCellStatus(l,c,USED) ) continue;
			//			cout << l << " " << c << " Recovered as isolated hit" << endl;

			GetAttenuationCorr(l, c, attpar);

			// find the nearest cluster
			Int_t I_shwr=0;
			mindist=FLT_MAX;
			for(int i_shwr=0; i_shwr<N_Shwr; i_shwr++){
				d1 = TMath::Abs( c - ShwrCellMin[i_shwr][l] );
				d2 = TMath::Abs( c - ShwrCellMax[i_shwr][l] );
				dist = TMath::Min( d1, d2 );
				if( dist*dist < mindist ){
					mindist = dist*dist;
					I_shwr = i_shwr;
				}
			}
			if(l/2%2==0) posfiber = ShwrX0[I_shwr] + ShwrKX[I_shwr]*(EcalZ[l] - ShwrZ0[I_shwr]);
			else         posfiber = ShwrY0[I_shwr] + ShwrKY[I_shwr]*(EcalZ[l] - ShwrZ0[I_shwr]);

			attcorr=attpar[0]*TMath::Exp(-1.0*posfiber/attpar[1])+(1.0-attpar[0])*TMath::Exp(-1.0*posfiber/attpar[2]);
			if(attcorr<=0.0) attcorr = 1.0;
			EdepAttC[l][c] = Edep[l][c]/attcorr;

			Status[l][c] |= USED;
			Nhits++;
		}
	}
	return 0;
}


//==================================================================
//-->Get Energy estimation: 
//----->with Attenuation correction
//----->with Rear Leakage correction
//==================================================================
double EcalFastRec::GetEnergyEstimate(){

	E0=0.0;
	EL2L=0.0;
	for(int ipl=0;ipl<kNL;ipl++){
		for(int icell=0;icell<nCell;icell++){
			if(EdepAttC[ipl][icell]>0.0){
				E0+=EdepAttC[ipl][icell];
				if(ipl>=16)
					EL2L+=EdepAttC[ipl][icell];
			}
		}
	}

	double rearcorr=GetRearLeakCorr();

	if(rearcorr<0.0)	return E0;
	else							return E0/rearcorr;
}

//==================================================================
double EcalFastRec::GetRearLeakCorr(){

	double rearcorr=1.0;
	double rearpar=1.0;

	//-->Parameterization
	if(E0<=0.0)
		return 0.0;
	else if(E0<1.0)
		rearpar=0.0;
	else if(E0>100000.0)
		rearpar=3.37;
	else
		rearpar=0.24+0.34*TMath::Log(E0);

	rearcorr=1.0-rearpar*EL2L/E0;
	return rearcorr;
}

//==================================================================
//-->Fast Reconstruction to do Attenuation correction on cells
//==================================================================
int EcalFastRec::DoFastShowerRec(AMSEventR *pev){

	//-->Do data calibration-->Edep[][]
	Reset();
	ReadADCMapFromAMSEvent(pev);
	DefineCellStatus();

	InitDataBase(Time);
	GainCalibration();

	//-->Fast reconstruction
	int iter=0;
	N_Shwr=0;
	Int_t flag_x, flag_y;

	//-->Two iteration of Axis <--> Attenuation
	while( iter < 2 ){
		N_Shwr = 0;

		memset(ShwrNhits, 0, sizeof(ShwrNhits));
		memset(ShwrCellMin, 0, sizeof(ShwrCellMin));
		memset(ShwrCellMax, 0, sizeof(ShwrCellMax));

		memset(ShwrX0, 0, sizeof(ShwrX0));
		memset(ShwrY0, 0, sizeof(ShwrY0));
		memset(ShwrKX, 0, sizeof(ShwrKX));
		memset(ShwrKY, 0, sizeof(ShwrKY));
		memset(ShwrZ0, 0, sizeof(ShwrZ0));

		while( N_Shwr<4 ){
			// Only search the most energetic shower in current available(NOT yet used) hits
			if( iter==0 ){
				memcpy( EdepBuffer, Edep, sizeof(EdepBuffer) );
			}
			else{
				memcpy( EdepBuffer, EdepAttC, sizeof(EdepBuffer) );
			}

			//-->Set Status
			if( N_Shwr == 0 ){
				ClearAllCellStatus( USED );
			}else{
				for(int l=0; l<kNL; l++)
					for(int c=0; c<nCell; c++)
						if( CheckCellStatus( l, c, USED ) ) EdepBuffer[l][c] = 0;
			}

			//-->Shower Finding
			ShowerSeedFinding(iter);
			ShowerClustering(iter);
			CalculateLayerCOG(iter);
			ParabolaZ0Fit();
			ShowerAxisFit();

			//-->Quality of the new shower
			if      ( (dEX>Cutoff_SEED) && (fX1>=Cutoff_NPL)                      ) flag_x = 2;
			else if ( (dEX>Cutoff_SEED) && (fEX>=Cutoff_NNB) && (fX0>=Cutoff_NPL) ) flag_x = 2;
			else if ( (dEX>Cutoff_SEED) && (fEX>=Cutoff_NNB  ||  fX0>=Cutoff_NPL) ) flag_x = 1;
			else if ( (dEX>Cutoff_MIP)  && (EX>2.*dEX)                            ) flag_x = 1;
			else                                                                    flag_x = 0;

			if      ( (dEY>Cutoff_SEED) && (fY1>=Cutoff_NPL)                      ) flag_y = 2;
			else if ( (dEY>Cutoff_SEED) && (fEY>=Cutoff_NNB) && (fY0>=Cutoff_NPL) ) flag_y = 2;
			else if ( (dEY>Cutoff_SEED) && (fEY>=Cutoff_NNB  ||  fY0>=Cutoff_NPL) ) flag_y = 1;
			else if ( (dEY>Cutoff_MIP) && (EY>2.*dEY)                             ) flag_y = 1;
			else                                                                    flag_y = 0;

			if ( flag_x<1 || flag_y<1 ) break;

			//-->Do Attenuation
			AttenuationCorrection();

			ShwrX0[N_Shwr] = X0;
			ShwrY0[N_Shwr] = Y0;
			ShwrZ0[N_Shwr] = Z0;
			ShwrKX[N_Shwr] = KX;
			ShwrKY[N_Shwr] = KY;

			for(Int_t l=0; l<18; l++){
				ShwrCellMin[N_Shwr][l] = Cell_Min[l];
				ShwrCellMax[N_Shwr][l] = Cell_Max[l];
			}
			ShwrNhits[N_Shwr] = Nhits;
			N_Shwr++;
		}
		if( N_Shwr==0 ) break;
		if( N_Shwr>0 ){
			IsolatedHitsAttCor();
		}
		iter++;
	}
	return 0;
}
