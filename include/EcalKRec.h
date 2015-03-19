
//! EcalKRec class

/*!
 * Class for interface the AMSEvent object with 3D Reconstruction
 * First EcalFastRec class is used to get energy and calibrate edep
 * Then  Ecal3DRec   class is used to calculate EM likelihood and energy
 *
 * Input:   AMSEvent
 * Output:  Energy/Axis/EM Likelihood
 *
 * Usage:
 * 	BeforeLoop:
 * 		AMSEventR *pev;
 * 		EcalKRec *pecalk=EcalKRec::GetHead();
 *
 * 	EventLoop:
 * 		pecalk->ProcessAMSEvent(pev);
 *
 * 		pecalk->GetEnergyFast(energy00);
 * 		pecalk->GetEnergy3DRec(energyk);
 * 		pecalk->GetEMLikelihood(lkhd0,lkhd1);
 *
 * \author: Hai Chen(hachen@cern.ch), Andrei Kounine, Zuhao Li, Weiwei Xu, Cheng Zhang
*/


#ifndef __ECALKREC__
#define __ECALKREC__

#include <iostream>
#include "TString.h"
#include "TObject.h"

#include "root.h"
#include "amschain.h"

#include "EcalFastRec.h"
#include "Ecal3DRec.h"

enum EnumParticleType{
	kElectron,
	kProton,
	kPhoton
};

class EcalKRec : public TObject {
	//! Functions
	public:

		//! A Constructor
		EcalKRec();
		//! A Destructor
		~EcalKRec();

		//! Create objects
		static EcalKRec *GetHead();

		//! ReBuild Shower
		static void ReBuildShower();
		//! ReBuild only Fast Shower energy without 3D Fitting
		static void ReBuildShowerFast();

		//! Get Energy from Fast Reconstruction
		void GetEnergyFast(double &energy00);

		//! Get Number of shower
		void GetNShower(int &nshower);
		//! Get Energy from 3D Reconstruction
		void GetEnergy3DRec(double &energyk, int particle=kElectron);
		//! Get the Likelihood of the shower to be Electromagnetic type
		void GetEMLikelihood(double &lkhd0, double &lkhd1);
		//! Get the Shower axis information
		void GetShowerAxis(int iShwr, AMSPoint &position, AMSDir &direction);

		void GetShowerParameters(double par[6]);

	private:

		static AMSEventR   *pev; 
		static EcalKRec    *pHead;
		static EcalFastRec *pEcalFastRec;
		static Ecal3DRec   *pEcal3DRec;

		ClassDef(EcalKRec,1);
};
#endif

