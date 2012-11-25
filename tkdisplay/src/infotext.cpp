// $Id: infotext.cpp,v 1.5 2012/11/25 15:10:06 shaino Exp $
#include "infotext.h"

#include "root.h"
#include "amsdbc.h"
#include "TrTrack.h"
#include "TrRecHit.h"
#include "TrCluster.h"
#include "TrRawCluster.h"

#include "TString.h"
#include "TMath.h"

#include <QDateTime>

QString &InfoText::EventInfo(AMSEventR *event)
{
  static QString str;

  str = "";
  if (!event) return str;

  QDateTime qdt;
  qdt.setTime_t(event->fHeader.Time[0]);

  int num[8][2];

  time_t *tt = (time_t*)event->fHeader.Time;
  str += Form("Run: %10d\nEvent: %7d\n"
	      "FineTime: %d\nTime: %s\n"
	      "TimeDiff: %d\n\n"
	      "nRawClusters: %3d\nnClusters: %3d\n"
	      "nRecHits: %3d\nnTracks: %3d\n",
	      event->Run(),  event->Event(), 
	      event->fHeader.Time[1], //ctime(tt),
	      qdt.toLocalTime().toString(Qt::SystemLocaleShortDate)
	         .toAscii().data(),
	      event->pLevel1()->TrigTime[4], 
	      event->nTrRawCluster(), event->nTrCluster(), 
	      event->nTrRecHit(), event->nTrTrack());

  str += "Tracks:\n";
  for (int i = 0; i < event->nTrTrack(); i++) {
    TrTrackR *trk = event->pTrTrack(i);
    if (!trk) continue;

    str += Form(" nHit: %d nHitXY: %d", trk->GetNhits(), trk->GetNhitsXY());
    str += Form(" Rigidity : %.2f", trk->GetRigidity());
    str += Form(" Chi2: %.2f\n", trk->GetChisq());
  }

  for (int i = 0; i < 8; i++) num[i][0] = num[i][1]=0;
  str += "RawClusters:\n";

  for (int i = 0; i < event->nTrRawCluster(); i++){
    TrRawClusterR *cc = event->pTrRawCluster(i);
    if (cc) num[cc->GetLayer()-1][cc->GetSide()]++; 
  }
  for (int i = 0; i < 8; i++)
    str += Form("Layer %d:    S: %3d    K: %3d  \n",
		i+1, num[i][0], num[i][1]);  
  str += "\n";

  for (int i = 0; i < 8; i++) num[i][0] = num[i][1]=0;
  str += "Clusters:\n";

  for (int i = 0; i < event->nTrCluster(); i++){
    TrClusterR* cc = event->pTrCluster(i);
    if (cc) num[cc->GetLayer()-1][cc->GetSide()]++; 
  }

  for (int i = 0; i < 8; i++)
    str += Form("Layer %d:    S: %3d    K: %3d  \n",
		i+1, num[i][0],num[i][1]);  
  str += "\n";

  for (int i = 0; i < 8; i++) num[i][0] = num[i][1]=0;
  str += "RecHits:\n";
  
  for (int i = 0; i< event->nTrRecHit(); i++){
    TrRecHitR* cc = event->pTrRecHit(i);
    if (cc) num[cc->GetLayer()-1][0]++; 
  }
  
  for (int i = 0; i < 8; i++)
    str += Form("Layer %d:     %3d     \n", i+1, num[i][0]);  

  return str;
}

QString &InfoText::LadderInfo(AMSEventR *event, int tkid)
{
  static QString str;
  str = "";

  if (!event) return str;

  TkLadder *lad = TkDBc::Head->FindTkId(tkid);
  if (!lad) return str;

  int nsen = lad->GetNSensors();
  int nclx = 0, ncly = 0;

  for (int i = 0; i < event->nTrCluster(); i++) {
    TrClusterR *cls = event->pTrCluster(i);
    if (!cls) continue;
    if (cls->GetTkId() != tkid) continue;

    int side = cls->GetSide();
    if (side == 0) nclx++;
    else           ncly++;
  }

  int nhit = 0, nhitt = 0, nhitg = 0;

  for (int i = 0; i < event->nTrRecHit(); i++) {
    TrRecHitR *hit = event->pTrRecHit(i);
    if (!hit) continue;
    if (hit->GetTkId() != tkid) continue;

    nhit++;
    if (hit->checkstatus(AMSDBc::USED)) nhitt++;
    else if (hit->OnlyY()) nhitg++;
  }

  str += Form("Ladder TkID: %d  nSen: %d\n", lad->GetTkId(), nsen);
  str += "\n";
  str += Form("Clusters:  nClsX: %d nClsY: %d\n", nclx, ncly);

  for (int i = 0; i < event->nTrCluster(); i++) {
    TrClusterR *cls = event->pTrCluster(i);
    if (!cls) continue;
    if (cls->GetTkId() != tkid) continue;

    str += "\n  ";
    str += cls->Info(i);
  }

  str += "\n";
  str += "\n";
  str += Form("RecHits:  nHit: %d nHitT: %d nHitG: %d\n", nhit, nhitt, nhitg);

  for (int i = 0; i < event->nTrRecHit(); i++) {
    TrRecHitR *hit = event->pTrRecHit(i);
    if (!hit) continue;
    if (hit->GetTkId() != tkid) continue;

    str += "\n  ";
    str += hit->Info(i);
  }

  return str;
}

QString &InfoText::TrackInfo(AMSEventR *event, int itrk)
{
  static QString str;
  str = "";

  if (!event) return str;

  TrTrackR *trk = event->pTrTrack(itrk);
  if (!trk) return str;

  int id1 = TrTrackR::kChoutko;
  int id2 = TrTrackR::kChoutko+TrTrackR::kMultScat;
  if (!trk->ParExists(id1)) id1 = TrTrackR::kLinear;
  if (!trk->ParExists(id2)) id2 = id1;
  if (!trk->ParExists(id1)) return str;

  int fpat[8];
  for (int i = 0; i < 8; i++) fpat[i] = 0;
  for (int i = 0; i < trk->GetNhits(); i++) {
    TrRecHitR *hit = trk->GetHit(i);
    int ily = hit->GetLayer()-1;
    if (hit->OnlyY()) fpat[ily] = 1;
    else fpat[ily] = 2;
  }
  TString spat;
  for (int i = 0; i < 8; i++) {
    if (fpat[i] == 0) spat += "_";
    if (fpat[i] == 1) spat += "Y";
    if (fpat[i] == 2) spat += "O";
  }

  str += Form("Track[%d]\n", itrk);
  str += Form("  nHit: %d\n",    trk->GetNhits());
  str += Form("  nHitXY: %d\n",  trk->GetNhitsXY());
  str += Form("  Pattern: %d [%s]\n", trk->GetPattern(), spat.Data());
  str += "\n";
  str += Form("  Rigidity (noMS): %.2f\n", trk->GetRigidity(id1));
  str += Form("  Rigidity (Fast): %.2f\n", trk->GetRigidity(id2));
  str += Form("   errRinv (Fast): %.2f\n", trk->GetErrRinv (id2));
  str += "\n";
  str += Form("  Chi2Fast:    %.2f\n",     trk->GetChisq   (id2));
  str += Form("    Chi2X/Ndf: %.2f/%d\n",  trk->GetChisqX  (id2), 
	                                  trk->GetNdofX   (id2));
  str += Form("    Chi2Y/Ndf: %.2f/%d\n",  trk->GetChisqY  (id2), 
	                                  trk->GetNdofY   (id2));
  str += "\n";
  str += Form("  P0:    (%.2f, %.2f, %.2f)\n",  
	      trk->GetP0x(id2), trk->GetP0y(id2), trk->GetP0z(id2));
  str += Form("  th/ph: (%.1f, %.1f)\n",  
	      trk->GetTheta(id2)*TMath::RadToDeg(),
	      trk->GetPhi  (id2)*TMath::RadToDeg());

  for (int i = 0; i < trk->GetNhits(); i++) {
    TrRecHitR *hit = trk->GetHit(i);
    if (!hit) continue;
    int lay = hit->GetLayer();
    str += Form("\nHit[%d]\n  ", i);
    str += hit->Info(i);
    str += Form("Residual: %.4f %.4f\n", trk->GetResidualO(lay).x(),
		                         trk->GetResidualO(lay).y());
  }

  return str;
}

QString &InfoText::HitInfo(AMSEventR *event, int ihit)
{
  static QString str;
  str = "";

  if (!event) return str;

  TrRecHitR *hit = event->pTrRecHit(ihit);
  if (!hit) return str;

  str += hit->Info(ihit);

  return str;
}

QString &InfoText::ClusterInfo(AMSEventR *event, int icls)
{
  static QString str;
  str = "";

  if (!event) return str;

  TrClusterR *cls = event->pTrCluster(icls);
  if (!cls) return str;

  str += cls->Info(icls);

  return str;
}

QString &InfoText::RawClusInfo(AMSEventR *event, int icls)
{
  static QString str;
  str = "";

  if (!event) return str;

  TrRawClusterR *cls = event->pTrRawCluster(icls);
  if (!cls) return str;

  for (int i = 0; i < event->nTrRawCluster(); i++) {
    TrRawClusterR *cc = event->pTrRawCluster(i);
    if (!cc || cc->GetTkId() != cls->GetTkId()) continue;
    str += (cc == cls) ? ">> " : "      ";
    str += cc->Info(i);
  }

  return str;
}
