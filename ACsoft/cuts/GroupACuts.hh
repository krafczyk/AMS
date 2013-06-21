#ifndef GROUPACUTS_HH
#define GROUPACUTS_HH

#include "Cut.hh"

#ifndef __CINT__
#include "AnalysisParticle.hh"
#include "Event.h"
#include "RunHeader.h"
#include "TrackerTrackFit.h"
#include "SplineTrack.hh"
#endif

namespace Cuts {
 
  /** Cut on Science Run Tag. */
  class CutEventErrors : public Cut {
  public:
    CutEventErrors() : Cut("Event Errors") {}

    virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {
      if (!p.HasRawEventData()) return true; // This cut is used within ACROOT production. (Find a way to guarantee that!)
      return !(p.RawEvent()->EventHeader().Status() >> 30 & 0x1);
    }

    ClassDef(Cuts::CutEventErrors,1)
  };

  /** Cut on Science Run Tag. */
  class CutScienceRunTag : public Cut {
  public:
    CutScienceRunTag() : Cut("Science Run Tag") {}

    virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {
      AssureCutIsAppliedToACQtFile(p);
      return p.RawEvent()->RunHeader()->RunTag() >= 0xF000;
    }

    ClassDef(Cuts::CutScienceRunTag,1)
  };

  /** Cut on Good HW. */
  class CutGoodHW : public Cut {
  public:
    CutGoodHW() : Cut("Good HW") {}

    virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {
      if (!p.HasRawEventData()) return true; // This cut is used within ACROOT production. (Find a way to guarantee that!)
      const ACsoft::AC::DAQ& daq = p.RawEvent()->DAQ();
      const ACsoft::AC::DAQ::JINJStatusVector& jinj_status = daq.JINJStatus();
      const ACsoft::AC::DAQ::SlaveStatusVector& slave_status = daq.SlaveStatus();

      unsigned int bitstocheck = 0x7E;// Reply Status Word from 9 to 14.

      bool errors=false;
      for (int ii=0; ii<4; ii++ ) {
        bool ret = (jinj_status.at(ii)>>8) & bitstocheck;
        // if (ret)
        //   printf("Error detected in JINJ[%d]: 0x%02x\n", ii, jinj_status.at(ii)>>8);
        errors |= ret;
      }
      for (int ii=0; ii<24; ii++) {
        if (ii!=10 && ii!=11) {
          bool ret = slave_status.at(ii) & bitstocheck;
          // if (ret)
          //   printf("Error detected in SLAVE[%d]: 0x%02x\n", ii, slave_status.at(ii));
          errors |= ret;
        }
      }

      return !errors;
    }

    ClassDef(Cuts::CutGoodHW,1)
  };

  /** Cut on solar array collision. */
  class CutIsNotInSolarArrayShadow : public Cut {
  public:
    CutIsNotInSolarArrayShadow() : Cut("Is not in Solar Array shadow") {}

    virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {
      if (!p.HasRawEventData()) return true; // This cut is used within ACROOT production. (Find a way to guarantee that!)

	  // particles from below will have the IsInSolarArrayShadow bit set, although it doesn't make sense for them
      // -> checking for this here.
      if (!p.HasTofBeta()) return true;
      if (p.BetaTof() < 0) return true;
      return !p.AmsParticle()->IsInSolarArrayShadow();
    }

    ClassDef(Cuts::CutIsNotInSolarArrayShadow,1)
  };

  /** Make sure selected tracker track passes through ECAL acceptance. */
  class CutInEcalAcceptance : public Cut {
  public:
    CutInEcalAcceptance() : Cut( "Tracker track in ECAL acceptance" ) {}

    virtual bool TestCondition(const ACsoft::Analysis::Particle &p) {
      AssureCutIsAppliedToACQtFile(p);

      const ACsoft::Analysis::SplineTrack* spline = p.GetSplineTrack();
      if (!spline)
        return true;

      TVector3 upper_ecal_pos = spline->InterpolateToZ(ACsoft::AC::AMSGeometry::ZECALUpper);
      TVector3 lower_ecal_pos = spline->InterpolateToZ(ACsoft::AC::AMSGeometry::ZECALLower);

      bool Entry_in_32_4 = fabs(upper_ecal_pos.x()) < 32.4 && fabs(upper_ecal_pos.y()) < 32.4;
      bool Entry_in_31_5 = fabs(upper_ecal_pos.x()) < 31.5 && fabs(upper_ecal_pos.y()) < 31.5;
      bool Exit_in_32_4  = fabs(lower_ecal_pos.x()) < 32.4 && fabs(lower_ecal_pos.y()) < 32.4;
      bool Exit_in_31_5  = fabs(lower_ecal_pos.x()) < 31.5 && fabs(lower_ecal_pos.y()) < 31.5;

      return (Exit_in_32_4 && Entry_in_32_4) && ( Exit_in_31_5 || Entry_in_31_5);
    }

    ClassDef(Cuts::CutInEcalAcceptance,1)
  };

  /** Make sure selected tracker track passes through TRD acceptance. */
  class CutInTrdAcceptance : public Cut {
  public:
    CutInTrdAcceptance() : Cut( "Tracker track in TRD acceptance" ) {}

    virtual bool TestCondition(const ACsoft::Analysis::Particle &p) {
      AssureCutIsAppliedToACQtFile(p);

      const ACsoft::Analysis::SplineTrack* spline = p.GetSplineTrack();
      if (!spline)
        return true;

      int nTrdBottom = 9;
      float AccepBottomX[] = {+40, +78, +78, +40, -40, -78, -78, -40, +40};
      float AccepBottomY[] = {+76, +35, -35, -76, -76, -35, +35, +76, +76};
      float TrdBottomZ = 86.725;

      int nTrdTop = 37;
      float AccepTopX[] = {-99.0,-89.0,-89.0,-78.7,-78.7,-67.8,-67.8,-57.7,-57.7, 57.7, 57.7, 67.8, 67.8, 78.7, 78.7, 89.0, 89.0, 99.0,
                           99.0, 89.0, 89.0, 78.7, 78.7, 67.8, 67.8, 57.7, 57.7,-57.7,-57.7,-67.8,-67.8,-78.7,-78.7,-89.0,-89.0,-99.0,-99.0};
      float AccepTopY[] = { 54.5, 54.5, 62.5, 62.5, 74.0, 74.0, 84.0, 84.0, 95.3, 95.3, 84.0, 84.0, 74.0, 74.0, 62.5, 62.5, 54.5, 54.5,
                            -51.7,-51.7,-62.2,-62.2,-72.0,-72.0,-82.5,-82.5,-92.5,-92.5,-82.5,-82.5,-72.0,-72.0,-62.2,-62.2,-51.7,-51.7, 54.5};
      float TrdTopZ = 141.825; //

      TVector3 pBottom = spline->InterpolateToZ(TrdBottomZ);
      TVector3 pTop= spline->InterpolateToZ(TrdTopZ);

      bool passTrdBottom = TMath::IsInside((float)pBottom.x(), (float)pBottom.y(), nTrdBottom, AccepBottomX, AccepBottomY);
      bool passTrdTop    = TMath::IsInside((float)pTop.x(),    (float)pTop.y(),    nTrdTop,    AccepTopX,    AccepTopY);

      return passTrdBottom && passTrdTop;
    }

    ClassDef(Cuts::CutInTrdAcceptance,1)
  };

  /** Check if tracker track and ECAL shower match. */
  class CutTrackerEcalMatching : public Cut {
  public:
    CutTrackerEcalMatching() : Cut( "Tracker / ECAL matching" ) {}

    virtual bool TestCondition(const ACsoft::Analysis::Particle &p) {
      AssureCutIsAppliedToACQtFile(p);

      const ACsoft::Analysis::SplineTrack* spline = p.GetSplineTrack();
      if (!spline)
        return true;

      const ACsoft::AC::ECALShower* shower = p.EcalShower();
      if (!shower)
        return true;

      TVector3 trackerPos = spline->InterpolateToZ(shower->Z());
      double delta_x = fabs(trackerPos.X() - shower->X());
      double delta_y = fabs(trackerPos.Y() - shower->Y());

      return (delta_x < 3.6) && (delta_y < 7.2);
    }

    ClassDef(Cuts::CutTrackerEcalMatching,1)
  };

  class CutTofTrackerMatching: public Cut {
  public:
    CutTofTrackerMatching() : Cut( "TOF / Tracker matching" ) {}

    virtual bool TestCondition(const ACsoft::Analysis::Particle &p) {
      AssureCutIsAppliedToACQtFile(p);

      const ACsoft::Analysis::SplineTrack* spline = p.GetSplineTrack();
      if (spline == 0)
        return true;

      const ACsoft::AC::Event* rawEvent = p.RawEvent();
      const ACsoft::AC::TOF tof = rawEvent->TOF();

      float LONGCUT[4][10]={ {18.5,15.9,14.1,13.9,14.6,13.3,14.3,17.2,0.,0.},
                             {22.,12.,12.2,15.9,12.2,17.6,11.4,20.9,0.,0.},
                             {6.8,10.4,10.5,12.,10.6,10.7,11.1,10.1,10.9,14.6},
                             {16.6,13.4,11.6,11.2,12.6,10.3,12.6,17.8,0.,0.} }; // cm (~100% efficiency cut on tof-track longitudinal coord.)
      float TRANCUT[4][10]={ {11.5,6.6,6.4,6.8,6.7,6.6,6.7,11.3,0.,0.},
                             {13.6,7.1,6.5,6.7,6.6,6.7,6.6,13.8,0.,0.},
                             {10.8,6.4,6.7,6.4,6.9,6.6,6.9,6.3,6.9,10.8},
                             {14.8,6.6,6.4,6.4,6.3,6.5,6.5,13.7,0.,0.} }; // cm (~100% efficiency cut on toftrack transverse coord.)

      bool goodstatus=false;
      bool goodmatch=false;
      bool goodlayer[4]={false,false,false,false};
      // match with cluster
      for (unsigned int icl=0; icl<tof.Clusters().size(); icl++) {
        ACsoft::AC::TOFCluster tof_cl = tof.Clusters().at(icl);
        goodstatus=true;
        int layer=tof_cl.Layer();
        int bar=tof_cl.Bar();
        // check cluster status
        for (int i = 6; i < 10; ++i)
          if (((tof_cl.Status() >> i) & 1) == 1)
            goodstatus = false;
        if (((tof_cl.Status() >> 4) & 1) == 1)
          goodstatus = false;
        // check track match with TOF clusters
        TVector3 trackerpos = spline->InterpolateToZ(tof_cl.Z());
        double d_long,d_tran;
        if (layer == 0 || layer == 3) {
          d_long = tof_cl.X() - trackerpos.X();
          d_tran = tof_cl.Y() - trackerpos.Y();
        }
        else {
          d_long = tof_cl.Y() - trackerpos.Y();
          d_tran = tof_cl.X() - trackerpos.X();
        }
        if(fabs(d_long)<LONGCUT[layer][bar] && fabs(d_tran)<TRANCUT[layer][bar])
          goodmatch=true;
        if (goodstatus && goodmatch) goodlayer[layer]=true;
      }
      int nGoodLayers=0;
      for (int i=0; i<4; i++) if(goodlayer[i]) nGoodLayers++;
      return (nGoodLayers==3 || nGoodLayers==4);
    }

    ClassDef(Cuts::CutTofTrackerMatching,1)
  };

}

#endif /* GROUPACUTS_HH */
