#include "TrdKHit.h"

ClassImp(TrdKHit)


float TrdKHit::Tube_radius=0.303;

TrdKHit::TrdKHit(){}
TrdKHit::~TrdKHit(){}

TrdKHit::TrdKHit(TrdRawHitR *_trdhit, float zshit){

    TRDHit_Layer=_trdhit->Layer;
    TRDHit_Ladder=_trdhit->Ladder;
    TrdHCalibR::gethead()->GetTubeIdFromLLT(_trdhit->Layer,_trdhit->Ladder,_trdhit->Tube,tubeid);
    mf=TrdHCalibR::gethead()->GetManifold(tubeid/128);
    if(TRDHit_Layer<=3 || TRDHit_Layer>=16)TRDHit_Direction=1;
    else TRDHit_Direction=0;
    IsCalibrated=0;

    TRDHit_Amp=_trdhit->Amp;
    TRDHitRZD rzd(*_trdhit);
    TRDHit_z=rzd.z;
    TRDRawHit_Z=rzd.z+zshit;
    TRDRawHit_R=rzd.r;
    if(TRDHit_Direction==0){
        TRDHit_x=rzd.r;
        TRDHit_y=0;
        TRDTube_Center = AMSPoint(TRDHit_x,0,TRDHit_z);
        TRDTube_Dir = AMSPoint(0,1,0);
    }else{
        TRDHit_y=rzd.r;
        TRDHit_x=0;
        TRDTube_Center = AMSPoint(0,TRDHit_y,TRDHit_z);
        TRDTube_Dir = AMSPoint(1,0,0);
    }

    TRDTube_Center_Raw=TRDTube_Center;
    TRDTube_Dir_Raw=TRDTube_Dir;

    IsAligned=0;


//    _rawhit=_trdhit;

}

void TrdKHit::DoGainCalibration(float corr){
    TRDHit_GainCorrection=corr;
    TRDHit_Amp*=corr;
    IsCalibrated=1;
}

void TrdKHit::DoAlignment(TRDAlignmentPar *par){

    AMSPoint T(par->dX,par->dY,par->dZ);

    AMSRotMat R;
    R.SetRotAngles(-1*par->alpha,-1*par->beta,-1*par->gamma);

    //    cout<<"Rotation: "<<R<<endl;
    AMSPoint Center(par->RotationCenter_X,par->RotationCenter_Y,par->RotationCenter_Z);

    //    AMSPoint temp_TRDHit(TRDHit_x,TRDHit_y,TRDHit_z);
    //    AMSPoint TRDHit_Local=temp_TRDHit-Center;
    //    AMSPoint TRDHit_Aligned=R*TRDHit_Local+T+Center;
    //    TRDHit_x=TRDHit_Aligned.x();
    //    TRDHit_y=TRDHit_Aligned.y();
    //    TRDHit_z=TRDHit_Aligned.z();


    AMSPoint TRDTube_Local=TRDTube_Center-Center;
    TRDTube_Center=R*TRDTube_Local+T+Center;
    TRDTube_Dir=R*TRDTube_Dir;


    Aligned_RotationMatrix=R;
    Aligned_Translation=T;
    Aligned_Center = Center;


    IsAligned=1;
}



void TrdKHit::PredictCrossing(const AMSPoint *P0, const AMSDir *u, const int tracktype){

    //=========
    //
    // http://softsurfer.com/Archive/algorithm_0106/algorithm_0106.htm
    //
    //
    //==============

    AMSPoint w0=*P0-TRDTube_Center;

    float a=1;
    float b=(*u).prod(TRDTube_Dir);
    float c=1;
    float d=(*u).prod(w0);
    float e=TRDTube_Dir.prod(w0);

    float denorm=a*c-b*b;
    if(denorm==0)cout<<"ERROR: Track Tube in Parrarel orientation"<<endl;
    float tc=(a*e-b*d)/denorm;

    AMSPoint hit=TRDTube_Center+TRDTube_Dir*tc;
    //    cout<<"Tube: Position:"<<TRDTube_Center<<", Dir: "<<TRDTube_Dir<<endl;
    //    cout<<"Track: Position:"<<(*P0)<<", Dir: "<<(*u)<<endl;
    //    cout<<"Hit: "<<hit<<endl;

    if(tracktype==0){
        TRDHit_x=hit.x();
        TRDHit_y=hit.y();
        TRDHit_z=hit.z();
    }else if(tracktype==1){
        TRDHit_TRDTrack_x=hit.x();
        TRDHit_TRDTrack_y=hit.y();
        TRDHit_TRDTrack_z=hit.z();
    }else{
        cout<<"~~~WARNING~~~TrdKHit from TrdKCluster, warn track type when getting Hit position~~~"<<endl;
    }

    return;

}

double TrdKHit::Tube_Track_Distance_3D(const AMSPoint *C2, const AMSDir *D2) const {
    AMSPoint diff=(*C2)-TRDTube_Center;
    AMSPoint cross=(*D2).crossp(TRDTube_Dir);
    return ((diff.prod(cross)))/cross.norm();
}

double TrdKHit::Tube_Track_Distance_2D(const AMSPoint *C2, const AMSDir *D2) const {

    if(TRDHit_Direction==0){
        double kx=D2->x()/D2->z();
        double x=C2->x()+kx*(TRDRawHit_Z-C2->z());
        double l=TMath::Abs(x-TRDRawHit_R);
        return l*sqrt(1/(1+kx*kx));
    }else if(TRDHit_Direction==1){
        double ky=D2->y()/D2->z();
        double y=C2->y()+ky*(TRDRawHit_Z-C2->z());
        double l=TMath::Abs(y-TRDRawHit_R);
        return l*sqrt(1/(1+ky*ky));
    }

    return 999;
}

double TrdKHit::Tube_Track_3DLength_New(const double d, const AMSDir *Dir) const {
    double ll=Tube_radius*Tube_radius-d*d;
    if(ll<=0)return 0;
    double l=sqrt(ll);
    double kx=Dir->x()/Dir->z();
    double ky=Dir->y()/Dir->z();
    if(TRDHit_Direction==0)    return 2*sqrt(l*l*(1+ky*ky/(1+kx*kx)));
    else return 2*sqrt(l*l*(1+kx*kx/(1+ky*ky)));
}

double TrdKHit::Tube_Track_3DLength_New(const AMSPoint *X0, const AMSDir *Dir) const {
    double d=Tube_Track_Distance_2D(X0,Dir);
    return Tube_Track_3DLength_New(d,Dir);
}

double TrdKHit::Tube_Track_3DLength(const AMSPoint *X0, const AMSDir *Dir) const {

    float tubelength=190;  //======Relistic tube lenght dimention needed====

    AMSRotMat RCor=Aligned_RotationMatrix;
    RCor.Invert();

    AMSPoint X0prime=RCor*(*X0-Aligned_Translation-Aligned_Center)+Aligned_Center-TRDTube_Center_Raw;
    AMSDir DirPRIME=RCor*(*Dir);

    double x0 = 0,y0 = 0,dx = 0,dy =0;
    if(TRDHit_Direction==1){
        x0=X0prime.y();
        y0=X0prime.z();
        dx=DirPRIME.y();
        dy=DirPRIME.z();
    }else if(TRDHit_Direction==0){
        x0=X0prime.x();
        y0=X0prime.z();
        dx=DirPRIME.x();
        dy=DirPRIME.z();
    }

    long double t1,t2;
    long double A=pow(dx,2)+pow(dy,2);
    long double B=2*(y0*dy + x0*dx);
    long double C = pow(x0,2) + pow(y0,2) - pow(Tube_radius,2);
    long double D = pow(B,2) - 4*A*C;


    //    cout<<"A,B,C,D: "<<A<<", "<<B<<", "<<C<<", "<<D<<endl;

    if(D<=0)return 0;
    t1=(-1*B+sqrt(D))/(2*A);
    t2=(-1*B-sqrt(D))/(2*A);
    //    cout<<"t1,t2: "<<t1<<", "<<t2<<endl;
    AMSPoint P1=X0prime+DirPRIME*t1;
    AMSPoint P2=X0prime+DirPRIME*t2;

    //    cout<<"P1: "<<P1<<"  P2: "<<P2<<endl;


    if(TRDHit_Direction==1){
        if(fabs(P1.x())>tubelength && fabs(P2.x())>tubelength){
            return 0;
        }
        if(fabs(P1.x())>tubelength){
            if(P1.x()>0){
                float deltax=fabs(P1.x()-tubelength);
                P1=P1-DirPRIME*(deltax/DirPRIME.x());
            }
        }
        if(fabs(P2.x())>tubelength){
            if(P2.x()>0){
                float deltax=fabs(P2.x()-tubelength);
                P2=P2-DirPRIME*(deltax/DirPRIME.x());
            }
        }
    }

    if(TRDHit_Direction==0){
        if(fabs(P1.y())>tubelength && fabs(P2.y())>tubelength){
            return 0;
        }
        if(fabs(P1.y())>tubelength){
            if(P1.y()>0){
                long double deltax=fabs(P1.y()-tubelength);
                P1=P1-DirPRIME*(deltax/DirPRIME.y());
            }
        }
        if(fabs(P2.y())>tubelength){
            if(P2.y()>0){
                long double deltax=fabs(P2.y()-tubelength);
                P2=P2-DirPRIME*(deltax/DirPRIME.y());
            }
        }
    }


    long double distance=(P1-P2).norm();



    return distance;
}
