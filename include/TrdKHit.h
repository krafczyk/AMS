//**************************************************************//
// TRDKLikelihood methods
//
// For detailed information:
// https://twiki.cern.ch/twiki/bin/view/AMS/TRDKLikelihood
//
//
// 2012-06-18  v1.0,  First commit on cvs
//
//**************************************************************//


#ifndef TrdKHIT_H
#define TrdKHIT_H
#include "TObject.h"
#include "TrdKCalib.h"
#include <TrdRawHit.h>
#include <TrdHCalib.h>
#include <point.h>


class TrdKHit:public TObject
{
public:
    TrdKHit();
    TrdKHit(TrdRawHitR *_trdhit, float zshit=0);
    ~TrdKHit();

    void DoGainCalibration(float corr);
    void DoAlignment(TRDAlignmentPar *par);
    void PredictCrossing(const AMSPoint *P0, const AMSDir *u, const int tracktype=0);
    double Tube_Track_Distance_3D(const AMSPoint *C2, const AMSDir *D2) const;
    double Tube_Track_Distance_2D(const AMSPoint *C2, const AMSDir *D2) const;
    double Tube_Track_3DLength(const AMSPoint *C2, const AMSDir *D2) const;
    double Tube_Track_3DLength_New(const double d, const AMSDir *Dir) const;
    double Tube_Track_3DLength_New(const AMSPoint *X0, const AMSDir *Dir) const;

    double Time;

    float TRDHit_Amp;
    float TRDHit_GainCorrection;
    float TRDRawHit_Z;
    float TRDRawHit_R;
    int TRDHit_Layer;
    int TRDHit_Ladder;
    int tubeid;
    int mf;
    int TRDHit_Direction;  //0 for X, 1 for Y


    float TRDHit_x;
    float TRDHit_y;
    float TRDHit_z;

    float TRDHit_TRDTrack_x;
    float TRDHit_TRDTrack_y;
    float TRDHit_TRDTrack_z;

    AMSPoint TRDTube_Center_Raw;
    AMSDir TRDTube_Dir_Raw;

    AMSPoint TRDTube_Center;
    AMSDir TRDTube_Dir;

    AMSRotMat Aligned_RotationMatrix;
    AMSPoint Aligned_Translation;
    AMSPoint Aligned_Center;

    bool IsCalibrated;
    bool IsAligned;

//    TrdRawHitR* _rawhit;

    static float Tube_radius;

    ClassDef(TrdKHit,2)

};


#endif // TrdKHIT_H
