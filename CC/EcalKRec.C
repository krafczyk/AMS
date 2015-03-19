
#include <math.h>
#include "VCon.h"
#include <algorithm>

#include <iostream>
#include "TString.h"
#include "TObject.h"

#include "EcalKRec.h"


//==================================================================
//--->EcalKRec class definition
//==================================================================

EcalFastRec* EcalKRec::pEcalFastRec = 0;
Ecal3DRec* EcalKRec::pEcal3DRec = 0;
EcalKRec* EcalKRec::pHead = 0;
AMSEventR* EcalKRec::pev = 0;

//==================================================================
EcalKRec::EcalKRec(){

}
//==================================================================
EcalKRec::~EcalKRec(){

}

//==================================================================
EcalKRec* EcalKRec::GetHead(){

	if(pHead==NULL)
		pHead = new EcalKRec();
	if(pEcalFastRec==NULL)
		pEcalFastRec = new EcalFastRec();
	if(pEcal3DRec==NULL)
		pEcal3DRec = new Ecal3DRec();

	return pHead;
}


//==================================================================
void EcalKRec::ReBuildShowerFast(){

	if(AMSEventR::Head()!=NULL)
		pev=AMSEventR::Head();
	else
		cerr<<"==========No AMSEventR Head for EcalKRec::ReBuildShower()=========="<<endl;

	pEcalFastRec->DoFastShowerRec(pev);

	return;
}

//==================================================================
void EcalKRec::ReBuildShower(){

	//-->Fast Reconstruction
	//ReBuildShowerFast();
	//-->3D Fit
	pEcal3DRec->SetCluster(pEcalFastRec->EdepAttC,pEcalFastRec->Status,1);
	pEcal3DRec->ShowerRec(0);
	pEcal3DRec->ShowerCombineFit();
	pEcal3DRec->Summary();

	return;
}

//==================================================================
//-->Add energy estimation for proton!!!!
//
void EcalKRec::GetEnergyFast(double &energy00){

	energy00=pEcalFastRec->GetEnergyEstimate();

	return;
}

//==================================================================
void EcalKRec::GetEnergy3DRec(double &energyk, int particle){

	if( particle == kElectron || particle == kPhoton ) 
		energyk = pEcal3DRec->EleEne;
	else 
		energyk = 3.0*pEcal3DRec->EleEne;

	return;
}

//==================================================================
void EcalKRec::GetEMLikelihood(double &lkhd0, double &lkhd1){
	lkhd0 = pEcal3DRec->EmLkhd[0];
	lkhd1 = pEcal3DRec->EmLkhd[1];

	return;
}

//==================================================================
void EcalKRec::GetNShower(int &nshower){
	nshower=pEcal3DRec->N_Shwr;
	return;
}

//==================================================================
void EcalKRec::GetShowerAxis(int iShwr,AMSPoint &pos, AMSDir &dir){
	if( iShwr > pEcal3DRec->N_Shwr ){ 
		cerr << "--ERROR--EcalKRec::GetShowerAxix()--Shower-Index-Exceeds-Maximum-Value " << endl;
		return;
	}
	pos = AMSPoint( pEcal3DRec->ShwrPar[iShwr][3][0], pEcal3DRec->ShwrPar[iShwr][4][0], pEcal3DRec->ShwrPar[iShwr][5][0] );
	dir = AMSDir( pEcal3DRec->ShwrPar[iShwr][6][0], pEcal3DRec->ShwrPar[iShwr][7][0], 1 );

	return;
}

void EcalKRec::GetShowerParameters(double par[6]){

	//par[0]=pEcal3DRec->E0f;
	//par[1]=pEcal3DRec->El2f;
	par[2]=pEcal3DRec->TotEne;
	par[3]=pEcal3DRec->L2Ene;
	//par[2]=pEcal3DRec->T0;
	//par[3]=pEcal3DRec->Z0;
	par[4]=pEcal3DRec->KX;
	par[5]=pEcal3DRec->KY;

	return;
}

