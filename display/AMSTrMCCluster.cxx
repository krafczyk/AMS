//  $Id: AMSTrMCCluster.cxx,v 1.2 2001/01/22 17:32:32 choutko Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS TrMC Counter Cluster                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"
#include "AMSTrMCCluster.h"
#include "AMSRoot.h"
#include "AMSVirtualDisplay.h"

ClassImp(AMSTrMCCluster)


//_____________________________________________________________________________
AMSTrMCCluster::AMSTrMCCluster(Int_t part, Float_t signal, 
                                Float_t * coo, Float_t * ercoo,
                                Int_t ntracks, TObjArray * tracks)
	: AMS3DCluster(coo,ercoo,0,5)
{
   m_Part     = part;
   m_Signal     = signal;
   m_NTracks    = ntracks;
   m_Tracks     = tracks;

   SetDirection(0.0, 0.0);
   SetLineWidth(1);
   SetLineColor(5);             // light blue
   SetFillColor(5);
   SetFillStyle(1001);          // solid filling (not working now....)

   debugger.Print("AMSTrMCCluster(%lx,%d bytes): position (%f,%f,%f)+-(%f,%f,%f)\n",
	this, sizeof(*this), coo[0], coo[1], coo[2], ercoo[0], ercoo[1], ercoo[2]);
}

//______________________________________________________________________________
char *AMSTrMCCluster::GetObjectInfo(Int_t px, Int_t py)
{
   static char info[90];
   sprintf(info,"TrMC: Particle=%d Signal=%4.1f, at (%5.1f,%5.1f,%5.1f)+-(%5.1f,%5.1f,%5.1f)",
		m_Part, m_Signal, 
		fX, fY, fZ,
		fDx, fDy, fDz );
	//	m_Position[0], m_Position[1], m_Position[2],
	//	m_ErrPosition[0], m_ErrPosition[1], m_ErrPosition[2] );
   return info;
}

//_____________________________________________________________________________
void AMSTrMCCluster::Paint(Option_t *option)
{
  /*
  if (gAMSRoot->Display()->DrawClusters()) {
    gAMSRoot->Display()->PaintFruit(this, m_Position, m_ErrPosition, 5, option);
  }
  */

  AMS3DCluster::Paint(option);

}

void AMSTrMCCluster::Clear(Option_t *option)
{
  m_Part = 0;
  m_Tracks = 0;
}

/*
//______________________________________________________________________________
void AMSTrMCCluster::Sizeof3D() const
{
   gAMSRoot->Display()->SizeFruit();
}
*/
