
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Anti Counter Cluster                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"
#include "AMSAntiCluster.h"
#include "AMSRoot.h"
#include "AMSVirtualDisplay.h"

ClassImp(AMSAntiCluster)


//_____________________________________________________________________________
AMSAntiCluster::AMSAntiCluster(Int_t status, Int_t sector, 
                                Float_t signal, 
                                Float_t * coo, Float_t * ercoo,
                                Int_t ntracks, TObjArray * tracks)
	: AMS3DCluster(coo,ercoo,0,5)
{
   m_Status     = status;
   m_Sector      = sector;
   m_Signal     = signal;
   m_NTracks    = ntracks;
   m_Tracks     = tracks;

   SetDirection(0.0, 0.0);
   SetLineWidth(3);
   SetLineColor(6);             // light blue
   SetFillColor(6);
   SetFillStyle(1001);          // solid filling (not working now....)

   debugger.Print("AMSAntiCluster(%lx,%d bytes): position (%f,%f,%f)+-(%f,%f,%f)\n",
	this, sizeof(*this), coo[0], coo[1], coo[2], ercoo[0], ercoo[1], ercoo[2]);
}

//______________________________________________________________________________
char *AMSAntiCluster::GetObjectInfo(Int_t px, Int_t py)
{
   static char info[90];
   sprintf(info,"Anti S%d: Signal=%4.1f, at (%5.1f,%5.1f,%5.1f)+-(%5.1f,%5.1f,%5.1f)",
		m_Sector, m_Signal, 
		fX, fY, fZ,
		fDx, fDy, fDz );
	//	m_Position[0], m_Position[1], m_Position[2],
	//	m_ErrPosition[0], m_ErrPosition[1], m_ErrPosition[2] );
   return info;
}

//_____________________________________________________________________________
void AMSAntiCluster::Paint(Option_t *option)
{
  /*
  if (gAMSRoot->Display()->DrawClusters()) {
    gAMSRoot->Display()->PaintFruit(this, m_Position, m_ErrPosition, 5, option);
  }
  */

  AMS3DCluster::Paint(option);

}

void AMSAntiCluster::Clear(Option_t *option)
{
  m_Status = -1;
  m_Sector  = 0;
  m_Tracks = 0;
}

/*
//______________________________________________________________________________
void AMSAntiCluster::Sizeof3D() const
{
   gAMSRoot->Display()->SizeFruit();
}
*/
