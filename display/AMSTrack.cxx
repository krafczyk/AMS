//  $Id: AMSTrack.cxx,v 1.3 2001/01/22 17:32:32 choutko Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSRoot Track class                                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"

#include "AMSTrack.h"

ClassImp(AMSTrack)


//_____________________________________________________________________________
AMSTrack::AMSTrack(Int_t status, Int_t pattern)
{
   m_Status     = status;
   m_Pattern    = pattern;
}

//_____________________________________________________________________________
void AMSTrack::SetHelix()
{
   Double_t Bfield = -0.15*0.95;	// in minus-x direction of AMS
   if ( m_GeaneFitDone ) {
     THelix::SetHelix(m_GP0[0], m_GP0[1], m_GP0[2], 
		      m_GRigidity * TMath::Sin(m_GTheta) * TMath::Cos(m_GPhi),
		      m_GRigidity * TMath::Sin(m_GTheta) * TMath::Sin(m_GPhi),
		      m_GRigidity * TMath::Cos(m_GTheta),
		      0.3*Bfield/100,
		      -120.0, 120.0, kHelixX,
		      -1.0, 0.0, 0.0);
     Double_t y = TMath::Sin(m_GTheta) * TMath::Sin(m_GPhi);
     Double_t z = TMath::Cos(m_GTheta);
//     debugger.Print("GRigidity_T = %lf, B = %lf\n", 
//	m_GRigidity * sqrt(y*y+z*z), Bfield);
   } else if ( m_FastFitDone ) {
     THelix::SetHelix(m_FP0[0], m_FP0[1], m_FP0[2], 
		      m_FRigidity * TMath::Sin(m_FTheta) * TMath::Cos(m_FPhi),
		      m_FRigidity * TMath::Sin(m_FTheta) * TMath::Sin(m_FPhi),
		      m_FRigidity * TMath::Cos(m_FTheta),
		      0.3*Bfield/100,
		      -120.0, 120.0, kHelixX,
		      -1.0, 0.0, 0.0);
     Double_t y = TMath::Sin(m_FTheta) * TMath::Sin(m_FPhi);
     Double_t z = TMath::Cos(m_FTheta);
//     debugger.Print("GRigidity_T = %lf, B = %lf\n", 
//	m_FRigidity * sqrt(y*y+z*z), Bfield);
   } else {
     Error("AMSTrack()", "neithe fast-fit nor Geane fit was done!");
     return;
   }

   SetLineColor(14);	// dark grey
   SetLineWidth(1);

   return;
}

//_____________________________________________________________________________
void AMSTrack::Paint(Option_t *option)
{
  // Paint an AMS track 
  //

  // special treatment on dummy tracks which has status word 17 (a la Vitaly)
  //
  //if ( m_Status != 17 )
     THelix::Paint(option);

}

//_____________________________________________________________________________
char * AMSTrack::GetObjectInfo(Int_t px, Int_t py)
{
   static char info[255];
   sprintf(info,"#%d, (Rigidity,theta,phi)=(%4.1lf+-%4.1lf,%4.1f,%4.1f), %d hits, pattern=%d", m_ID,
		 m_GeaneFitDone? m_GRigidity    : m_FRigidity, 
		 m_GeaneFitDone? m_GErrRigidity : m_FErrRigidity,
		 m_GeaneFitDone? m_GTheta       : m_FTheta, 
		 m_GeaneFitDone? m_GPhi         : m_FPhi, 
		 m_NHits, m_Pattern);

// debugger.Print("Track #%d, (fX0,fY0,fZ0)=(%lf,%lf,%lf), (fVt,fVz,fPhi0)=(%lf,%lf,%lf), fW=%lf, fRange=(%lf,%lf)\n",
// m_ID, fX0, fY0, fZ0, fVt, fVz, fPhi0, fW, fRange[0], fRange[1]);
   return info;

}

