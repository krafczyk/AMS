
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Cerenkov Threshold Counter Cluster                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"
#include "AMSR_CTCCluster.h"
#include "AMSR_Root.h"
#include "AMSR_VirtualDisplay.h"

ClassImp(AMSR_CTCCluster)


//_____________________________________________________________________________
AMSR_CTCCluster::AMSR_CTCCluster(Int_t status, Int_t plane, Int_t rawsignal,
                                Float_t signal, Float_t errsignal,
                                Float_t * coo, Float_t * ercoo,
                                Int_t ntracks, TObjArray * tracks)
	: AMSR_3DCluster(coo,ercoo,0,1)
{
   m_Status     = status;
   m_Plane      = plane;
   m_RawSignal  = rawsignal;
   m_Signal     = signal;
   m_ErrSignal  = errsignal;
// for ( int i=0; i<3; i++) {
//   m_Position[i]    = coo[i];
//   m_ErrPosition[i] = ercoo[i];
// }
   m_NTracks    = ntracks;
   m_Tracks     = tracks;

   SetDirection(0.0, 0.0);
   SetLineWidth(3);
   SetLineColor(7);             // light blue
   SetFillColor(7);
   SetFillStyle(3001);          // solid filling (not working now....)

   debugger.Print("AMSR_CTCCluster(%lx,%d bytes): position (%f,%f,%f)+-(%f,%f,%f)\n",
	this, sizeof(*this), coo[0], coo[1], coo[2], ercoo[0], ercoo[1], ercoo[2]);
}

//______________________________________________________________________________
char *AMSR_CTCCluster::GetObjectInfo(Int_t px, Int_t py)
{
   static char info[90];
   sprintf(info,"CTC S%d: Signal=%4.1f, at (%5.1f,%5.1f,%5.1f)+-(%5.1f,%5.1f,%5.1f)",
		m_Plane, m_Signal, 
		fX, fY, fZ,
		fDx, fDy, fDz );
	//	m_Position[0], m_Position[1], m_Position[2],
	//	m_ErrPosition[0], m_ErrPosition[1], m_ErrPosition[2] );
   return info;
}

//_____________________________________________________________________________
void AMSR_CTCCluster::Paint(Option_t *option)
{
  /*
  if (gAMSR_Root->Display()->DrawClusters()) {
    gAMSR_Root->Display()->PaintFruit(this, m_Position, m_ErrPosition, 5, option);
  }
  */

  AMSR_3DCluster::Paint(option);

}

void AMSR_CTCCluster::Clear(Option_t *option)
{
  m_Status = -1;
  m_Plane  = 0;
  m_Tracks = 0;
}

/*
//______________________________________________________________________________
void AMSR_CTCCluster::Sizeof3D() const
{
   gAMSR_Root->Display()->SizeFruit();
}
*/
