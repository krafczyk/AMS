// $Id: TrTrackSelection.C,v 1.2 2011/02/24 09:01:42 shaino Exp $
#include "TrTrackSelection.h"
#include "TMath.h"
#include "tkdcards.h"

ClassImp(TrTrackSelection);

int TrTrackSelection::GetSpanFlags(TrTrackR *track)
{
  if (!track) return 0;

  bool becal = false;
  AMSPoint pnt;
  AMSDir   dir;
  track->Interpolate(TrFit::TkDBc()->GetZlayer(9), pnt, dir);
  if (TMath::Abs(pnt.x()) < 33 && TMath::Abs(pnt.y()) < 33) becal = true;

  int flag = 0;
  int hpat = track->GetBitPattern();
  if ((hpat & 0x001) && (hpat & 0x060)) flag |= kMaxInt;
  if ((hpat & 0x080) && (hpat & 0x060)) flag |= kHalfL1N;
  if ((hpat & 0x101) == 0x101 && becal) flag |= kHalfL9;
  if ((hpat & 0x180) == 0x180 && becal) flag |= kMaxSpan;
  if ((hpat & 0x001) && (hpat & 0x006) &&
      (hpat & 0x018) && (hpat & 0x060)) flag |= kAllPlane;

  return flag;
}

double TrTrackSelection::GetHalfRdiff(TrTrackR *track, int span, 
				                       int algo, int refit)
{
  if (!track) return -1;

  if (span == 3) span = kMaxInt;
  if (span == 5) span = kHalfL1N;
  if (span == 6) span = kHalfL9;
  if (span == 7) span = kMaxSpan;

  int mfu = -1, mfl = -1, mfr = -1;
  double ehr = 0;
  double cor = 1;
  double *cp = 0;
  double cpar[18] = { 1.00, 0.00,  0.70, -1.45, 0.52, 0.69,
		      1.70, 1.05, -0.17, -0.67, 1.05, 0.75,
		      0.99, 1.62, -0.32,  0.75, 4.83, 1.67 };

  if (span == kMaxInt) {
    mfu = track->iTrTrackPar(algo, 1, refit);  // Inner upper half
    mfl = track->iTrTrackPar(algo, 2, refit);  // Inner lower half
    double eriu = (mfu > 0) ? track->GetErrRinv(mfu) : -1;
    double eril = (mfl > 0) ? track->GetErrRinv(mfl) : -1;
    if (eriu > 0 && eril > 0)
      ehr = TMath::Sqrt(eriu*eriu+eril*eril)*1.4;
  }
  else if (span == kHalfL1N) {
    mfr = track->iTrTrackPar(algo, 5, refit);  // With Layer 1N
    mfu = track->iTrTrackPar(algo, 5, refit);  // With Layer 1N
    mfl = track->iTrTrackPar(algo, 3, refit);  // Inner only
    ehr = 2.7e-3;                              // Normalization factor
    cp  = &cpar[0];
  }
  else if (span == kHalfL9) {
    mfu = track->iTrTrackPar(algo, 3, refit);  // Inner only
    mfl = track->iTrTrackPar(algo, 6, refit);  // With Layer 9
    mfr = track->iTrTrackPar(algo, 6, refit);  // With Layer 9
    ehr = 2.7e-3;                              // Normalization factor
    cp  = &cpar[6];
  }
  else if (span == kMaxSpan) {
    mfu = track->iTrTrackPar(algo, 5, refit);  // With Layer 1N
    mfl = track->iTrTrackPar(algo, 6, refit);  // With Layer 9
    mfr = track->iTrTrackPar(algo, 7, refit);  // Max span
    ehr = 2.7e-3;                              // Normalization factor
    cp  = &cpar[12];
  }

  if (mfr > 0 && cp) {
    double rgt = track->GetRigidity(mfr);
    double lgr = TMath::Log10(TMath::Abs(rgt));
    cor = TMath::Sqrt(cp[0]*cp[0]+cp[1]*cp[1]/rgt/rgt)
         *(1+cp[2]+cp[3]*TMath::Exp(-cp[4]*(lgr-cp[5])*(lgr-cp[5])));
  }

  if (mfu > 0 && mfl > 0 && ehr > 0 && cor > 0) {
    double rgtu = track->GetRigidity(mfu);
    double rgtl = track->GetRigidity(mfl);
    if (rgtu != 0 && rgtl != 0)
      return (1/rgtu-1/rgtl)*(1/rgtu-1/rgtl)/ehr/ehr/cor;
  }

  return -1;
}

double TrTrackSelection::GetHalfRessq(TrTrackR *track, int span, 
				                       int algo, int refit)
{
  if (!track) return -1;

  if (span == 3) span = kMaxInt;
  if (span == 5) span = kHalfL1N;
  if (span == 6) span = kHalfL9;
  if (span == 7) span = kMaxSpan;

  // For the moment only max span is supported
  if (span != kMaxSpan) return -1;

  int mf8 = track->iTrTrackPar(algo, 5, refit);  // With Layer 1N
  int mf9 = track->iTrTrackPar(algo, 6, refit);  // With Layer 9
  int mff = track->iTrTrackPar(algo, 7, refit);  // With Layer 1N and 9
  if (mf8 <= 0 || mf9 <= 0 || mff <= 0) return -2;

  double rgt = TMath::Abs(track->GetRigidity(mff));
  if (rgt == 0) return -3;

  double fms8 = TRFITFFKEY.FitwMsc[7]/rgt;
  double fms9 = TRFITFFKEY.FitwMsc[8]/rgt;
  double ery8 = TMath::Sqrt( 9+fms8*fms8)*TRFITFFKEY.ErrX*1.2;
  double ery9 = TMath::Sqrt(15+fms9*fms9)*TRFITFFKEY.ErrY*1.2;
  if (ery8 <= 0 || ery9 <= 0) return -4;

  double res8 = track->GetResidual(7, mf9).y();
  double res9 = track->GetResidual(8, mf8).y();
  if (res8 == 0 || res9 == 0) return -5;

  return res8*res8/ery8/ery8+res9*res9/ery9/ery9;
}
