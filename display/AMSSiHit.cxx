
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Silicon Tracker Hits                                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "AMSSiHit.h"
#include "AMSRoot.h"
#include "AMSVirtualDisplay.h"
#include "Debugger.h"

ClassImp(AMSSiHit)


//_____________________________________________________________________________
AMSSiHit::AMSSiHit(Int_t status, Int_t plane, Int_t px, Int_t py,
                                Float_t * hit, Float_t * errhit, 
                                Float_t ampl, Float_t asym,
                                TObjArray * tracks)
	: AMS3DCluster(hit, errhit, 0, 50)
{
   m_Status      = status;
   m_Plane       = plane;
   m_X           = px;
   m_Y           = py;
   for (Int_t i=0; i<3; i++) {
//   m_Position[i]    = hit[i];
     m_ErrPosition[i] = errhit[i];			// the real size
   }
   m_Amplitude   = ampl;
   m_AsymAmpl    = asym;

   m_Tracks      = tracks;

   SetDirection(0.0, 0.0);
						// magnify for visual effect
   TMarker3DBox::SetSize(errhit[0]*300.0, errhit[1]*300.0, errhit[2]*300.0);
   SetLineWidth(1);
   SetLineColor(4);		// dark blue
   SetFillColor(4);
   SetFillStyle(1001);		// solid filling (not working now....)

}

//______________________________________________________________________________
char *AMSSiHit::GetObjectInfo(Int_t px, Int_t py)
{
   static char info[64];
   sprintf(info,"Si Layer %d: Ampl=%4.1f, at (%5.1f,%5.1f,%5.1f)+-(%5.3f,%5.3f,%5.3f)",
		m_Plane, 		// m_X, m_Y, 
		m_Amplitude, 
		fX, fY, fZ,
		m_ErrPosition[0], m_ErrPosition[1], m_ErrPosition[2]);
   return info;
}

//_____________________________________________________________________________
void AMSSiHit::Paint(Option_t *option)
{

  /*
  if (gAMSRoot->Display()->DrawClusters()) {
    Float_t erxyz[3];
    for (Int_t i=0; i<3; i++) 
      erxyz[i] = m_ErrPosition[i] * 100;  // have to enlarge
    gAMSRoot->Display()->PaintFruit(this, m_Position, erxyz, 4, option);
  }
  */

  AMS3DCluster::Paint(option);

}

void AMSSiHit::Clear(Option_t *option)
{
  m_Status = -1;
  m_Plane  = m_X = m_Y = 0;
  m_Tracks = 0;
}


/*
//______________________________________________________________________________
void AMSSiHit::Sizeof3D() const
{
   gAMSRoot->Display()->SizeFruit();
}
*/
