
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSRoot Particle class                                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"
#include "AMSRoot.h"
#include "AMSTrack.h"
#include "AMSParticle.h"
#ifndef ROOT_TClonesArray
#include <TClonesArray.h>
#endif

ClassImp(AMSParticle)


/*
//_____________________________________________________________________________
AMSParticle::AMSParticle(Int_t status, Int_t pattern)
{
   m_Status     = status;
   m_Pattern    = pattern;
}
*/

//_____________________________________________________________________________




void AMSParticle::SetHelix(){

   Double_t Bfield = -0.15*0.95;	// in minus-x direction of AMS
   Double_t P[3];
   Double_t V[3];
   Double_t Range[2]={-120.,120.};
   Double_t Axis[3]={-1,0,0};
   P[0]=m_Position[0];
   P[1]=m_Position[1];
   P[2]=m_Position[2];
   V[0]=m_Charge*m_Momentum * TMath::Sin(m_Theta) * TMath::Cos(m_Phi);
   V[1]=m_Charge*m_Momentum * TMath::Sin(m_Theta) * TMath::Sin(m_Phi);
   V[2]=m_Charge*m_Momentum * TMath::Cos(m_Theta);
     THelix::SetHelix(P,V,0.3*Bfield/100,Range,kHelixX,Axis);
   



   SetLineColor(2);
   SetLineWidth(1);

   return;
}


//_____________________________________________________________________________
void AMSParticle::Paint(Option_t *option)
{
   // Paint a particle... just paint its helix part
   //

   Color_t c = this->GetLineColor();
   Width_t w = this->GetLineWidth();
   Int_t   n = this->GetN();
   Float_t * p = this->GetP();
   //debugger.Print("::::: AMSParticle::Paint() called: color = %d, width = %d.\n", c, w);
   //debugger.Print("::::: %d points, coordinates:", n);
   //for (Int_t i=0; i<n; i++) debugger.Print(" (%f,%f,%f) ", p[i*3], p[i*3+1], p[i*3+2]);
   THelix::Paint(option);

}

//_____________________________________________________________________________
char * AMSParticle::GetObjectInfo(Int_t px, Int_t py)
{
   static char info[255];
   info[0]='\0';
   sprintf(info,"GID=%d  m=%4.1f+-%4.2g, p=%4.1f+-%4.2g, pat=%d, status=%x, beta=%4.2f+-%4.2g, betapat=%d, charge=%4.1f",
		 m_GeantID, m_Mass, m_ErrMass, m_Momentum, m_ErrMomentum,
                 m_pattern,m_trstatus,m_beta,m_errbeta,m_betapattern,m_Charge);

   return info;

}

