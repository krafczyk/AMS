
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Time of Flight Cluster                                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"

#include "AMSToFCluster.h"
#include "AMSRoot.h"
#include "AMSVirtualDisplay.h"

ClassImp(AMSToFCluster)


//_____________________________________________________________________________
AMSToFCluster::AMSToFCluster(Int_t status, Int_t plane, Int_t bar,
			     Float_t energy, Float_t time, Float_t ertime,
                             Float_t * coo, Float_t * ercoo,
                             Int_t ncells, Int_t nparticles,
                             Int_t ntracks, TObjArray * tracks)
	: AMS3DCluster(coo, ercoo, 0, 5)
{
   m_Status     = status;
   m_Plane      = plane;
   m_Bar        = bar;
   m_Energy     = energy;
   m_Time       = time;
   m_ErTime     = ertime;
// for ( int i=0; i<3; i++) {
//   m_Position[i]    = coo[i];
//   m_ErrPosition[i] = ercoo[i];
// }
   m_Ncells  = ncells;
   m_Nparticles = nparticles;
   m_NTracks    = ntracks;
   m_Tracks     = tracks;

   SetDirection(0.0, 0.0);
   SetLineWidth(1);
   SetLineColor(3);             // light green
   SetFillColor(3);
   SetFillStyle(1001);          // solid filling (not working now....)

   debugger.Print("AMSToFCluster::AMSToFCluster(): position (%f,%f,%f)+-(%f,%f,%f)\n",
	coo[0], coo[1], coo[2], ercoo[0], ercoo[1], ercoo[2]);
}

//______________________________________________________________________________
char *AMSToFCluster::GetObjectInfo(Int_t px, Int_t py)
{
   static char info[64];
   sprintf(info,"ToF S%dB%d: time=%3.1f+-%3.1f, Edep=%4.1f, at (%5.1f,%5.1f,%5.1f)+-(%5.1f,%5.1f,%5.1f)",
		m_Plane, m_Bar, m_Energy, m_Time, m_ErTime,
		fX, fY, fZ,
		fDx, fDy, fDz );
   return info;
}

//_____________________________________________________________________________
void AMSToFCluster::Paint(Option_t *option)
{

  /*
  if (gAMSRoot->Display()->DrawClusters()) {
    gAMSRoot->Display()->PaintFruit(this, m_Position, m_ErrPosition, 2, option);
  }
  */

  AMS3DCluster::Paint(option);
}

void AMSToFCluster::Clear(Option_t *option)
{
  m_Status = -1;
  m_Plane  = m_Bar = 0;
  m_Energy = 0;
  m_Tracks = 0;
}


/*
//______________________________________________________________________________
void AMSToFCluster::Sizeof3D() const
{
   gAMSRoot->Display()->SizeFruit();
}
*/
