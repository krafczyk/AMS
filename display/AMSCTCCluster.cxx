
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Cerenkov Threshold Counter Cluster                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"
#include "AMSCTCCluster.h"
#include "AMSRoot.h"
#include "AMSVirtualDisplay.h"

ClassImp(AMSCTCCluster)


//_____________________________________________________________________________
AMSCTCCluster::AMSCTCCluster(Int_t status, Int_t plane, Int_t rawsignal,
                                Float_t signal, Float_t errsignal,
                                Float_t * coo, Float_t * ercoo,
                                Int_t ntracks, TObjArray * tracks)
	: AMS3DCluster(coo,ercoo,0,5)
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
   SetLineWidth(1);
   SetLineColor(7);             // light blue
   SetFillColor(7);
   SetFillStyle(1001);          // solid filling (not working now....)

   debugger.Print("AMSCTCCluster(%lx,%d bytes): position (%f,%f,%f)+-(%f,%f,%f)\n",
	this, sizeof(*this), coo[0], coo[1], coo[2], ercoo[0], ercoo[1], ercoo[2]);
}

//______________________________________________________________________________
char *AMSCTCCluster::GetObjectInfo(Int_t px, Int_t py)
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
void AMSCTCCluster::Paint(Option_t *option)
{
  /*
  if (gAMSRoot->Display()->DrawClusters()) {
    gAMSRoot->Display()->PaintFruit(this, m_Position, m_ErrPosition, 5, option);
  }
  */

  AMS3DCluster::Paint(option);

}

void AMSCTCCluster::Clear(Option_t *option)
{
  m_Status = -1;
  m_Plane  = 0;
  m_Tracks = 0;
}

/*
//______________________________________________________________________________
void AMSCTCCluster::Sizeof3D() const
{
   gAMSRoot->Display()->SizeFruit();
}
*/
