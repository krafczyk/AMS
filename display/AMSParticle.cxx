
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
void AMSParticle::SetHelix()
{

   //debugger.Print(":::::: particle points to track #%d\n", m_PTrack);

   AMSMaker * maker = (AMSMaker *) gAMSRoot->TrackMaker();
   TClonesArray &tracks = *(TClonesArray*) maker->Fruits();
//   TObject * obj = maker->Fruits();
   THelix * helix = (THelix *)tracks[m_PTrack-1];	// m_PTrack starts at 1
   helix->Copy(*this);		// copy *helix to *this

   //debugger.Print(":::::: particle helix: (x0,y0,z0)=(%5.1f,%5.1f,%5.1f)\n",
   //	  fX0, fY0, fZ0);
   //debugger.Print(":::::: particle helix: axis=(%5.1f,%5.1f,%5.1f), fVt, fW, fVt/fW = %f, %f, %f\n",
   //	  fAxis[0], fAxis[1], fAxis[2], fVt, fW, fVt/fW);
          
   AMSTrack * trk = (AMSTrack *)tracks[m_PTrack];
   //debugger.Print(":::::: track: %s\n", trk->GetObjectInfo(0,0));

   SetLineColor(2);
   SetLineWidth(1);

   //debugger.Print("<=== returning from AMSParticle::SetHelix()\n");
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
   static char info[80];
   sprintf(info,"Track #%d (Geant ID=%d): m=%4.1f+-%4.2g, p=%4.1f+-%4.2g",
		 m_PTrack, m_GeantID, m_Mass, m_ErrMass, m_Momentum, m_ErrMomentum);

   return info;

}

