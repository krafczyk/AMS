#ifndef TrdKSIMPLEHIT_H
#define TrdKSIMPLEHIT_H
#include "TObject.h"

class TrdKSimpleHit:public TObject{
public:
    TrdKSimpleHit(){}
    ~TrdKSimpleHit(){}

    int TRDHit_Layer;
    int tubeid;
    int mf;
    int TRDHit_Direction;  //0 for X, 1 for Y

    float TRDRawHit_Z;
    float TRDRawHit_R;

    float TRDHit_GainCorrection;
    float TRDHit_Amp;

    double TRDHit_Distance;
    double TRDHit_PathLength;
    float TRDHit_x;
    float TRDHit_y;
    float TRDHit_z;

    double TRDHit_TRDTrack_Distance;
    double TRDHit_TRDTrack_PathLength;
    float TRDHit_TRDTrack_x;
    float TRDHit_TRDTrack_y;
    float TRDHit_TRDTrack_z;

    double TRDHit_MaxSpan_Distance;
    double TRDHit_MaxSpan_PathLength;
    float TRDHit_MaxSpan_x;
    float TRDHit_MaxSpan_y;
    float TRDHit_MaxSpan_z;



    const  bool operator > ( const TrdKSimpleHit rhs) const { return (TRDHit_Layer > rhs.TRDHit_Layer); }
    const  bool operator < ( const TrdKSimpleHit rhs) const { return (TRDHit_Layer < rhs.TRDHit_Layer); }



    ClassDef(TrdKSimpleHit,4)
};


#endif // TrdKSIMPLEHIT_H
