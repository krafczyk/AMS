//  $Id: AMSR_Track.cxx,v 1.2 2001/01/22 17:32:35 choutko Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_Root Track class                                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"

#include "AMSR_Track.h"

ClassImp(AMSR_Track)


//_____________________________________________________________________________
AMSR_Track::AMSR_Track(Int_t status, Int_t pattern)
{
   m_Status     = status;
   m_Pattern    = pattern;
}

//_____________________________________________________________________________
void AMSR_Track::SetHelix()
{
   Double_t Bfield = -0.15*0.95;	// in minus-x direction of AMS
   Double_t P0[3];
   Double_t V0[3];
   Double_t Axis[3]={-1,0,0};
   Double_t Range[2]={-120,120};
   if ( m_GeaneFitDone ) {
     P0[0]=m_GP0[0];
     P0[1]=m_GP0[1];
     P0[2]=m_GP0[2];
     V0[0]=m_GRigidity * TMath::Sin(m_GTheta) * TMath::Cos(m_GPhi);
     V0[1]=m_GRigidity * TMath::Sin(m_GTheta) * TMath::Sin(m_GPhi);
     V0[2]=m_GRigidity * TMath::Cos(m_GTheta);
   } 
   else {
     P0[0]=m_FP0[0];
     P0[1]=m_FP0[1];
     P0[2]=m_FP0[2];
     V0[0]=m_FRigidity * TMath::Sin(m_FTheta) * TMath::Cos(m_FPhi);
     V0[1]=m_FRigidity * TMath::Sin(m_FTheta) * TMath::Sin(m_FPhi);
     V0[2]=m_FRigidity * TMath::Cos(m_FTheta);
   }          
     THelix::SetHelix(P0,V0,0.3*Bfield/100,Range,kHelixX,Axis);

   SetLineColor(14);	// dark grey
   SetLineWidth(1);

   return;
}

//_____________________________________________________________________________
void AMSR_Track::Paint(Option_t *option)
{
  // Paint an AMS track 
  //

  // special treatment on dummy tracks which has status word 17 (a la Vitaly)
  //
  //if ( m_Status != 17 )
     THelix::Paint(option);

}

//_____________________________________________________________________________
char * AMSR_Track::GetObjectInfo(Int_t px, Int_t py)
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

