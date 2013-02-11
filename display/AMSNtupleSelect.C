//#define _PGTRACK_
#include "AMSNtupleHelper.h"
#include "../include/EcalChi2CY.h"
#include "../include/root_setup.h"
#include "../include/DynAlignment.h"
#include "../include/TrInnerDzDB.h"
#include <fstream>
#include "../include/TrdSCalib.h"
#include "../include/TrdKCluster.h"
#include "../include/Tofrec02_ihep.h"
#include "../include/bcorr.h"
 AMSNtupleHelper * fgHelper=0;

extern "C" AMSNtupleHelper * gethelper();

void* __dso_handle=0;
static ofstream iftxtp("./positrons.txt");
static ofstream iftxte("./electorns.txt");
class AMSNtupleSelect: public AMSNtupleHelper{
public:
  AMSNtupleSelect(){};
  bool IsGolden(AMSEventR *ev){
  float temp;
  float bcorr;
  MagnetVarp::btempcor(bcorr);
  vector<float>value;
if(ev && ev->nParticle() && ev->nEcalShower()==1&& ev->Particle(0).iEcalShower()>=0){
int s2=  AMSSetupR::gethead()->fSlowControl.GetData("Port",ev->UTime(),0.5,value);
 cout <<" res2 "<<s2<<" "<<value.size()<<" value "<<endl;
 for(int k=0;k<value.size();k++){
cout << " k "<<k<<value[k]<<endl;
 }
s2=  AMSSetupR::gethead()->fSlowControl.GetData("Starboard",ev->UTime(),0.5,value);
 cout <<" res2 "<<s2<<" "<<value.size()<<" value "<<endl;
 for(int k=0;k<value.size();k++){
cout << " k "<<k<<value[k]<<endl;
 }
s2=  AMSSetupR::gethead()->fSlowControl.GetData("Ram",ev->UTime(),0.5,value);
 cout <<" res2 "<<s2<<" "<<value.size()<<" value "<<endl;
 for(int k=0;k<value.size();k++){
cout << " k "<<k<<value[k]<<endl;
 }
s2=  AMSSetupR::gethead()->fSlowControl.GetData("Wake",ev->UTime(),0.5,value);
 cout <<" res2 "<<s2<<" "<<value.size()<<" value "<<endl;
 for(int k=0;k<value.size();k++){
cout << " k "<<k<<value[k]<<endl;
 }
s2=  AMSSetupR::gethead()->fSlowControl.GetData("M-3X:31:UCF3",ev->UTime(),0.5,value);
 cout <<" res2 "<<s2<<" "<<value.size()<<" value "<<endl;
 for(int k=0;k<value.size();k++){
cout << " k "<<k<<value[k]<<endl;
}
return 1;
AMSDir dir=AMSDir(ev->EcalShower(0).Dir[0],ev->EcalShower(0).Dir[1],ev->EcalShower(0).Dir[2]);
double GalCoo_AMS_Long, GalCoo_AMS_Lat;
int GalCoo_Result;
AMSDir dir1=dir;
int     GalCoo_Return;
for(int k=0;k<1000;k++){
 for(int j=0;j<1000;j++){
  AMSPoint a(dir1[0]+0.001*(k-500),dir1[1]-0.001*(j-500),dir1[2]);
  dir=AMSDir(a);
        GalCoo_Return= ev->GetGalCoo(GalCoo_Result,GalCoo_AMS_Long, GalCoo_AMS_Lat,dir.gettheta(),dir.getphi(),1,4,3,0,2);
if(fabs(GalCoo_AMS_Long+83.87)<0.05 && fabs(GalCoo_AMS_Lat+13.7)<0.05){
  cout <<" i j"<<k<<" "<<j<<endl;
  AMSDir dir2=dir;
  for(int l=0;l<6;l++){
    for(int m=0;m<6;m++){
     AMSPoint a(dir2[0]+0.00005*(l-3),dir2[1]-0.00005*(m-3),dir2[2]);
  dir=AMSDir(a);  
cout<< "l m "<<l<<" "<<m<<endl;
     GalCoo_Return= ev->GetGalCoo(GalCoo_Result,GalCoo_AMS_Long, GalCoo_AMS_Lat,dir.gettheta(),dir.getphi());
cout<< "glat glong "<<GalCoo_AMS_Long<<" "<<GalCoo_AMS_Lat<<endl;
        GalCoo_Return= ev->GetGalCoo(GalCoo_Result,GalCoo_AMS_Long, GalCoo_AMS_Lat,dir.gettheta(),dir.getphi(),1,4,3,0,1);
cout<< "glat glong "<<GalCoo_AMS_Long<<" "<<GalCoo_AMS_Lat<<endl;
        GalCoo_Return= ev->GetGalCoo(GalCoo_Result,GalCoo_AMS_Long, GalCoo_AMS_Lat,dir.gettheta(),dir.getphi(),1,4,3,0,2);
cout<< "ra "<<GalCoo_AMS_Long<<" "<<GalCoo_AMS_Lat<<endl;
}
}
}
}
}
        static EcalAxis*paxis =new EcalAxis;
paxis->process(&(ev->EcalShower(0)),2);
  ParticleR & part=ev->Particle(0);
 float cogz=part.TRDCoo[0][2];
  AMSPoint pnt;
  paxis->interpolate(cogz,pnt,dir);
    dir=AMSDir(-dir[0],-dir[1],-dir[2]);
cout <<"run event "<< ev->Run()<<" "<<ev->Event()<<endl;
cout<< "glat glong "<<GalCoo_AMS_Long<<" "<<GalCoo_AMS_Lat<<endl;
       GalCoo_Return= ev->GetGalCoo(GalCoo_Result,GalCoo_AMS_Long, GalCoo_AMS_Lat,dir.gettheta(),dir.getphi());
 
cout <<"glat2 glong2  "<< GalCoo_AMS_Long<<" "<<GalCoo_AMS_Lat<<endl;

return true;
}

if(ev && ev->nParticle() && ev->nTrTrack()>0 && ev->Particle(0).iTrTrack()>=0){

/*       ParticleR & part=ev->Particle(0);
       double g1=part.GetGeoCutoff(ev);
       double g2=part.GetGeoCutoff(0,0);
       cout <<" mom event c1 c2  "<<part.Momentum<<" "<<ev->Event()<<" "<<g1<<" "<<g2<<endl;
*/

}
if(ev && ev->nParticle() && ev->nEcalShower()==1&& ev->Particle(0).iEcalShower()>=0){
AMSDir dir=AMSDir(ev->EcalShower(0).Dir[0],ev->EcalShower(0).Dir[1],ev->EcalShower(0).Dir[2]);
double GalCoo_AMS_Long, GalCoo_AMS_Lat;
int GalCoo_Result;
   int     GalCoo_Return= ev->GetGalCoo(GalCoo_Result,GalCoo_AMS_Long, GalCoo_AMS_Lat,dir.gettheta(),dir.getphi());

cout <<"glat glong  "<< GalCoo_AMS_Long<<" "<<GalCoo_AMS_Lat<<endl;

dir==AMSDir(ev->EcalShower(0).EMDir[0],ev->EcalShower(0).EMDir[1],ev->EcalShower(0).EMDir[2]);
       GalCoo_Return= ev->GetGalCoo(GalCoo_Result,GalCoo_AMS_Long, GalCoo_AMS_Lat,dir.gettheta(),dir.getphi());

cout <<"2glat glong  "<< GalCoo_AMS_Long<<" "<<GalCoo_AMS_Lat<<endl;

static EcalAxis*paxis =new EcalAxis;
paxis->process(&(ev->EcalShower(0)),2);
  float cogz=120;
  AMSPoint pnt;
  paxis->interpolate(cogz,pnt,dir);
  for(int k=0;k<3;k++)dir[k]=-dir[k];
       GalCoo_Return= ev->GetGalCoo(GalCoo_Result,GalCoo_AMS_Long, GalCoo_AMS_Lat,dir.gettheta(),dir.getphi());
cout <<"3glat glong  "<< GalCoo_AMS_Long<<" "<<GalCoo_AMS_Lat<<endl;
}
return 1;



static int begin=1;
if(begin==0){
cout << " open a"<<endl;
iftxtp.open("./positrons.txt");
cout << " open b"<<endl;
iftxte.open("./electrons.txt");
begin=1;
}
unsigned long long status =ev->fStatus;

/*
bool skip = !(status & 3) ||  !((status>>2)&1) || !((status>>3)&1)   || ((status>>4)&1) || !((status>>6)&1)  || !((status>>17)&1) ;
if(skip){
cout <<" sta "<<status<<" "<<(status & 3) <<  "  "<<((status>>2)&1) <<" "<<((status>>3)&1)   <<" "<< ((status>>4)&1) <<" "<<((status>>6)&1)  <<" "<<((status>>17)&1)<<endl ;
return true;
}
else return false;
*/
//DynAlManager::ignoreAlignment=true;
       AMSEventR*pev=ev;
       float dxy[2]={0,0};
       float gtheta[2]={0,0};
       float gphi[2]={0,0};
       if(pev && pev->nParticle()){
        
//       cout <<" here "<<endl;
       ParticleR & part=pev->Particle(0);

//int res;
//    double gln,gla;
//    float th= part.Theta;
//    float ph= part.Phi;
//    cout << "STK:  ret= " << pev->GetGalCoo(res,gln,gla,th,ph, true)<<endl;;
//    cout << "STKF:  ret= " << pev->GetGalCoo(res,gln,gla,th,ph, false)<<endl;;
int a=1;
    if(part.Charge>2 && part.iTrTrack()<0){
     TofRecH::ReBuild();
    int r2=   part.ReBuildTrdTOF();
    if(!r2)return true ;
    //cout <<r2<<endl;
    
    }
    return false;

        int result;
        double glat,glong,time;
        double rpto[3];
        if(part.iBeta()>=0){
            BetaR& beta=pev->Beta(part.iBeta());
            cout <<"  **beta** "<<beta.Beta<<" "<<beta.Pattern<<endl;
            return true;
        }
        double momentum=fabs(part.Momentum);
        int charge=part.Momentum>0?part.Charge:-part.Charge;
        double beta=fabs(part.Beta);
         int GalCoo_Return;
//        int GalCoo_Return= pev->DoBacktracing(result,glat,glong,rpto,time,part.Theta,part.Phi,momentum,beta,charge,false,false,false);
         bool over=(result>>5)&1;
        bool under=(result>>6)&1;
        bool trap=(result>>7)&1;
        cout << "result "<<result<<" "<<GalCoo_Return<<" "<<glat<<" "<<glong<<" "<<time<<" "<<over<<" "<<under<<" "<<trap<<endl;
       return true;
       part.ReBuildTrdEcal();
       if(part.iTrTrack()>=0){
       float charge=part.Charge;
       float cf=part.Cutoff;
       int ntr=pev->nTrTrack();
       int ntrd=pev->nTrdTrack();
       int ntof=pev->nTofCluster();
       int nanti=pev->nAntiCluster();
       static int clear=0;
       if(!clear++){
       pev->getsetup()->fISSCTRS.clear();
       pev->getsetup()->fISSGTOD.clear();
       }
       TrTrackR & tr=pev->TrTrack(part.iTrTrack());
            

                Double_t GalCoo_AMS_Long=0;
                Double_t GalCoo_AMS_Lat=0;

                Int_t GalCoo_Result;
                Int_t GalCoo_Return;
        float theta=0;
        float phi=0;
        float coo[3]={0,0,0};
        int pattern=tr.Pattern(111111111);
        int itav=tr.iTrTrackPar(1,pattern,21);
        if(itav>=0){
           AMSPoint a;
           AMSDir dir;
           double z=160;
           tr.Interpolate(z,a,dir,itav);
           for(int k=0;k<3;k++)coo[k]=a[k];
           gtheta[0]=part.ThetaGl*180/3.1415926;
           gphi[0]=part.PhiGl*180/3.1415926;
           cout <<"  old gtheta "<<gtheta[0]<<" "<<gphi[0]<<" "<<pev->Run()<<" "<<pev->Event()<<endl;
           part.Loc2Gl(pev);
           cout <<"  new gtheta "<<part.ThetaGl*180/3.1415926<<" "<<part.PhiGl*180/3.1415926<<endl;
           if(dir[2]*cos(part.Theta)>0){
              for(int k=0;k<3;k++)dir[k]=-dir[k];
            }
           AMSDir dir2;
              for(int k=0;k<3;k++)dir2[k]=-dir[k];
 
           phi=dir.getphi();
           theta=dir.gettheta();
        GalCoo_Return= pev->GetGalCoo(GalCoo_Result,GalCoo_AMS_Long, GalCoo_AMS_Lat,dir2.gettheta(),dir2.getphi(),false,false,false);
        float glat=GalCoo_AMS_Lat;
        float glong=GalCoo_AMS_Long;
        int gret=GalCoo_Return;
        double gt1,gp1;
//        pev->GetGTODCoo(GalCoo_Result,gt1, gp1,dir.gettheta(),dir.getphi(),false,true,false);
           if(gp1<0)gp1=gp1+360;
        gtheta[1]=gt1;
        gphi[1]=gp1;
          float Roll,Pitch,Yaw;
           double  xtime=double(pev->UTime())+pev->Frac()-15;    
          int ok=pev->getsetup()->getISSAtt(Roll,Pitch,Yaw,xtime);
          cout <<"  rpt "<<ok<<endl;     
        double zenith=pev->fHeader.Zenith();
        if(part.iEcalShower()>=0){
           EcalShowerR & ecal=pev->EcalShower(part.iEcalShower());
           float dirz=ecal.Dir[2]; 
           z=dirz<0?ecal.Entry[2]:ecal.Exit[2];
           for(int k=0;k<2;k++)dxy[k]=dirz<0?ecal.Entry[k]:ecal.Exit[k];
           tr.Interpolate(z,a,dir,itav);
           for(int k=0;k<2;k++)dxy[k]-=a[k];
           
         }
         double r=3.1415926/180;
         double cc=cos(gtheta[0]*r)*cos(gtheta[1]*r)+sin(gtheta[0]*r)*sin(gtheta[1]*r)*cos(gphi[0]*r-gphi[1]*r);
         double cc1=sin(pev->fHeader.ThetaS)*cos(gtheta[0]*r)+cos(pev->fHeader.ThetaS)*sin(gtheta[0]*r)*cos(pev->fHeader.PhiS-gphi[0]*r);
         double cc2=sin(pev->fHeader.ThetaS)*cos(gtheta[1]*r)+cos(pev->fHeader.ThetaS)*sin(gtheta[1]*r)*cos(pev->fHeader.PhiS-gphi[1]*r);
        cout << gtheta[0]<<" "<<gtheta[1]<<" "<<gphi[0]<<" "<<gphi[1]<<" "<<part.Theta<<" "<<part.Phi<<" "<<dir.gettheta()<<" "<<dir.getphi()<<" cc "<<cc<<" "<<zenith<<" "<<cc1<<" "<<cc2<<" "<<Pitch<<" "<<Yaw<<" "<<Roll<<" "<<ok<<" "<<pev->fHeader.Pitch<<" "<<pev->fHeader.Yaw<<" "<<pev->fHeader.Roll<<endl; 
 AMSSetupR::ISSCTRSR b; 
 AMSSetupR::ISSGTOD ab; 
 AMSPoint d2l;
 d2l[0]=pev->fHeader.RadS*cos(pev->fHeader.ThetaS)*cos(pev->fHeader.PhiS);
 d2l[1]=pev->fHeader.RadS*cos(pev->fHeader.ThetaS)*sin(pev->fHeader.PhiS);
 d2l[2]=pev->fHeader.RadS*sin(pev->fHeader.ThetaS);
 cout <<" fh "<<pev->fHeader.RadS<<" "<<pev->fHeader.ThetaS<<" "<<pev->fHeader.PhiS<<endl; 
//       pev->getsetup()->fISSCTRS.clear();
//       pev->getsetup()->fISSGTOD.clear();
if(!pev->getsetup()->getISSCTRS(b,xtime)){
 AMSPoint dc;
   dc[0]=b.r*cos(b.theta)*cos(b.phi);
   dc[1]=b.r*cos(b.theta)*sin(b.phi);
   dc[2]=b.r*sin(b.theta);
 
   AMSPoint da=dc-d2l;
   double diff=sqrt(da.prod(da))/1e5;
   double eps=0.1;
   pev->getsetup()->getISSCTRS(b,xtime+eps);
   dc[0]=b.r*cos(b.theta)*cos(b.phi);
   dc[1]=b.r*cos(b.theta)*sin(b.phi);
   dc[2]=b.r*sin(b.theta);
   da=dc-d2l;
   double diffp=sqrt(da.prod(da))/1e5;
   double grad=-(diffp-diff)/eps;
   double dt=diffp/grad;
   int ntry=0;
   double xtimed=xtime;
   cout <<" ntry "<<ntry<<" "<<diffp<<" "<<dt<<" "<<xtimed-xtime<<endl;

   while(fabs(dt)>0.0001  && ntry<20){
     diff=diffp;
     ntry++;
     pev->getsetup()->getISSCTRS(b,xtimed+dt);
     dc[0]=b.r*cos(b.theta)*cos(b.phi);
     dc[1]=b.r*cos(b.theta)*sin(b.phi);
     dc[2]=b.r*sin(b.theta);
     da=dc-d2l;
     diffp=sqrt(da.prod(da))/1e5;
     grad=-(diffp-diff)/dt/2;
     xtimed+=dt;
     double dt2=diffp/grad;
     if(dt2>fabs(dt)){
        dt2=fabs(dt*0.5);
      }
        else if(dt2<-fabs(dt)){
        dt2=-fabs(dt*0.5);
     }
     dt=dt2;
     cout <<" ntry "<<ntry<<" "<<diffp<<" "<<dt<<" "<<xtimed-xtime<<endl;
     
   }
   cout <<"  xtimed "<<xtimed-xtime<<" "<<diffp<<" "<<ntry<<" "<<endl; 
   cout <<" ctrs "<<diff<<" "<<b.r<<" "<<b.phi<<" "<<b.theta<<" "<<b.vphi<<" "<<b.vtheta<<" "<<" "<<pev->fHeader.VelTheta<<" "<<pev->fHeader.VelPhi<<endl; 
  }
 if(!pev->getsetup()->getISSGTOD(ab,xtime)){
    AMSPoint dc;
   dc[0]=ab.r*cos(ab.theta)*cos(ab.phi);
   dc[1]=ab.r*cos(ab.theta)*sin(ab.phi);
   dc[2]=ab.r*sin(ab.theta);
   AMSPoint da=dc-d2l;
   double diff=sqrt(da.prod(da))/1e5;
   cout <<" gtod "<<ab.r<<" "<<ab.theta<<" "<<ab.phi<<endl;
   double eps=0.1;
   pev->getsetup()->getISSGTOD(ab,xtime+eps);
   dc[0]=ab.r*cos(ab.theta)*cos(ab.phi);
   dc[1]=ab.r*cos(ab.theta)*sin(ab.phi);
   dc[2]=ab.r*sin(ab.theta);
   da=dc-d2l;
   double diffp=sqrt(da.prod(da))/1e5;
   double grad=-(diffp-diff)/eps;
   double dt=diffp/grad;
   diff=diffp;
   int ntry=0;
   double xtimed=xtime;
   cout <<" ntry "<<ntry<<" "<<diff<<" "<<diffp<<" "<<dt<<" "<<xtimed-xtime<<endl;
   cout <<" header " <<d2l<<endl;
   cout <<" grod " <<dc<<" frac "<<pev->Frac()<<" "<<pev->UTime()<<endl;
   while(fabs(dt)>0.0001 && diff>0.05 && ntry<100){
     ntry++;
     pev->getsetup()->getISSGTOD(ab,xtimed+dt);
     dc[0]=ab.r*cos(ab.theta)*cos(ab.phi);
     dc[1]=ab.r*cos(ab.theta)*sin(ab.phi);
     dc[2]=ab.r*sin(ab.theta);
     da=dc-d2l;
     diffp=sqrt(da.prod(da))/1e5;
     grad=-(diffp-diff)/dt/2;
     xtimed+=dt;
     double dt2=diffp/grad;
     if(dt2>fabs(dt)){
        dt2=fabs(dt*0.5);
      }
        else if(dt2<-fabs(dt)){
        dt2=-fabs(dt*0.5);
     }
     dt=dt2;
     diff=diffp;
     cout <<" ntry "<<ntry<<" "<<diffp<<" "<<dt<<" "<<xtimed-xtime<<endl;
     
   }
   cout <<"  xtimed "<<xtimed-xtime<<" "<<diffp<<" "<<ntry<<" "<<endl; 

   cout <<"  gtod "<<diff<<" "<<ab.r<<" "<<ab.phi<<" "<<ab.theta<<" "<<ab.vphi<<" "<<ab.vtheta<<" "<<ab.v<<" "<<pev->fHeader.VelTheta<<" "<<pev->fHeader.VelPhi<<endl; 
  }

  else cerr<<"gtod err "<<pev->getsetup()->getISSGTOD(ab,xtime)<<endl;
  
    cout <<"header "<<pev->fHeader.ThetaS<<" "<<pev->fHeader.PhiS<<" "<<pev->fHeader.RadS<<endl; 

       }

     }
}

return true;
TrInnerDzDB::ForceFromTDV=1;
/*
(if(ev && ev->nParticle()  && ev->Particle(0).iTrdTrack()>=0&&   ev->nTrdTrack()==1 && ev->Particle(0).iEcalShower()>=0 && ev->EcalShower(0).EnergyC>0 )
{
int ret=-1;
int ret2=-1;
cout <<ev->Particle(0).iTrTrack()<< "begore"<<ev->Particle(0).iVertex()<<" "<<ev->Particle(0).iCharge()<<" "<<ev->Particle(0).iBeta()<<endl;
ret=ev->Particle(0).ReBuildTrdEcal();
cout <<ev->Particle(0).iTrTrack()<< "after"<<endl;
  cout <<" ret "<<ret<<endl;
bool rtn=ret!=7 || ret2>=0;
return rtn; 
	}
else return 0;
if(  ev && ev->nParticle()){
  int ret=0;
  TkDBc::GetFromTDV(1311300000,3);
//  ret=ev->Particle(0).ReBuildTrdEcal(50,10,1,2,true);
  ret=ev->Particle(0).ReBuildTrdEcal(20,10,1,3);
  cout <<" ret "<<ret<<endl;
  return 1;
}
*/
if(ev && ev->nParticle() && ev->Particle(0).iTrTrack()>=0 && ev->Particle(0).iTrdTrack()>=0 && ev->Particle(0).iEcalShower()>=0 && ev->nTrTrack()==1 && ev->nTrdTrack()==1){

EcalShowerR & ecal=ev->EcalShower(0);
ParticleR & part=ev->Particle(0);
TrTrackR & track=ev->TrTrack(part.iTrTrack());
TrdTrackR &trdt=ev->TrdTrack(part.iTrdTrack());
cout <<" trdlikh "<<part.TrdSH_E2P_Likelihood<<" "<<part.TRDHLikelihood<<" "<<" "<<part.TrdSH_E2He_Likelihood<<endl;

double s1,s2,s3;
if(part.iTrdHTrack()<0){
cout<< " notrdhtrack"<<endl;
}
else{
TrdHTrackR &trdht=ev->TrdHTrack(part.iTrdHTrack());
double fmom=fabs(part.Momentum);
if(fmom<2)fmom=2.00001;
if(fmom>600)fmom=600-0000.1;
TrdSCalibR *h=TrdSCalibR::gethead();
if(h){
int ret=h->BuildTrdSCalib(ev->UTime(),fmom,&trdht,&track,s1,s2,s3,0);
cout <<" trdscalib ret "<<ret<<" "<<s1<<endl;
}
else cout<<"trdscalibgetheaqd zero "<<endl;
}
 

cout<<" trdcha "<<trdt.Charge[0]<<" "<<trdt.ChargeP[0]<<endl;

if(trdt.Charge[0]>1)return false;

   double energyd=ecal.EnergyD/1000;
   const UInt_t nENERGYBINs = 14;
   Float_t EnergyBin[nENERGYBINs + 1] = { 2.00, 3.12, 4.86, 7.57, 11.81, 18.41, 28.69,
      44.72, 69.71, 108.66, 169.38, 264.03, 411.56, 641.53, 1000.00 };

   // 90% efficiency cut in energy bins defined above
   Float_t cuts[nENERGYBINs] = { 0.550000, 0.710000, 0.812000, 0.866000, 0.896000, 0.898000,
      0.916000, 0.908000, 0.928000, 0.948000, 0.966000, 0.968000, 0.970000, 0.968000 };
 
   Float_t offset;

   if (energyd < EnergyBin[0]) offset = cuts[0];
   for (unsigned int ibin = 0; ibin < nENERGYBINs; ++ibin)
   {
      if (EnergyBin[ibin] <= energyd && energyd < EnergyBin[ibin + 1])
      {
         offset = cuts[ibin];
         break;
      }
   }
   if (energyd >= EnergyBin[nENERGYBINs]) offset = cuts[nENERGYBINs - 1];


float ecalbdt=ecal.GetEcalBDT();
float v2=ecal.EcalStandaloneEstimatorV2();

cout <<"  ecal bdt "<<ecalbdt-offset<<" "<<v2<<" "<<ecal.RearLeak<<endl;

int NTRDHit=ev->NTrdRawHit();
int Track_Fitcode_Max=track.iTrTrackPar(1,0,0); // Get any prefered fit code

double LikelihoodRatio[3]={-1,-1,-1};  //To be filled with 3 LikelihoodRatio :  e/P, e/H, P/H
if(Track_Fitcode_Max>0 && NTRDHit>0){

int NHits=0;  //To be filled with number of hits taken into account in Likelihood Calculation
float threshold=15;  //ADC above which will be taken into account in Likelihood Calculation,  15 ADC is the recommended value for the moment.

TrdKCluster _trdcluster= TrdKCluster(ev,&track,Track_Fitcode_Max);  

// Get the status of the Calculation
int IsReadAlignmentOK=_trdcluster.IsReadAlignmentOK;  // 0: Alignment not performed,  1: Static Alignment of Layer level,  2: Dynamic Alignment for entire TRD 
int IsReadCalibOK=_trdcluster.IsReadCalibOK;  // 0: Gain Calibration not performed,  1: Gain Calibration Succeeded

// Calculate Likelihood Ratio, Fill the LikelihoodRatio, NHits according to Track selection, and return validity 
int isvalid = _trdcluster.GetLikelihoodRatio_TrTrack(threshold,LikelihoodRatio,NHits); 
//Get Number of surrounding fired tubes, excluding the ones crossed by the current TrTrack predic
cout<<" trdk "<<isvalid<<" "<<LikelihoodRatio[0]<<" "<<LikelihoodRatio[1]<<" "<<LikelihoodRatio[2]<<endl;

double e,p,h;
e=1/(exp(LikelihoodRatio[0])+exp(LikelihoodRatio[1])-1);
cout <<" eprob "<<e<<endl;
if(e<0.5)return false;
}

    // Get hits on the track in outer layers
    TrRecHitR *hit1=track.GetHitLJ(1);
    TrRecHitR *hit9=track.GetHitLJ(9);
    int l19=0;
    // Get the difference between the two alignments
    AMSPoint diff1,diff9,diff1a,diff9a;
    if(hit1) {
         l19=1;
        diff1=TrExtAlignDB::GetAlDist(hit1);
        cout <<" hi1 "<<diff1*10000<<endl;
        diff1a=hit1->GetCoord(-1,4);
        cout <<" hi1a "<<diff1a*10000<<endl;
        if(fabs(diff1a[1])>30e-4)return false;
    }
    if(hit9) {
      l19+=10;
      diff9=TrExtAlignDB::GetAlDist(hit9);
        cout <<" hi9 "<<diff9*10000<<endl;
        diff9a=hit9->GetCoord(-1,4);
        cout <<" hi9a "<<diff9a*10000<<endl;
        if(fabs(diff9a[1])>30e-4)return false;
    }
      int it1=-1;
      int it2=-1;
      int it3=-1;
      int it4=-1;
      int it5=-1;
      int it6=-1;
      int it7=-1;
      int it8=-1;
      int it9=-1;
      int it10=-1;
{
       TrTrackR & tr=track;
       it1=tr.iTrTrackPar(21,1,1);
       it2=tr.iTrTrackPar(11,2,1);
       it3=tr.iTrTrackPar(1,3,1);
       it4=tr.iTrTrackPar(2,3,1);
       it5=tr.iTrTrackPar(4,3,1);
       it6=tr.iTrTrackPar(11,3,1);
       int pattern=tr.Pattern(1111111);
       it7=tr.iTrTrackPar(1,pattern,1);
       pattern=tr.Pattern(11111111);
       it8=tr.iTrTrackPar(21,pattern,1);
       pattern=tr.Pattern(101111111);
       it9=tr.iTrTrackPar(21,pattern,1);
       pattern=tr.Pattern(111111111);
       it10=tr.iTrTrackPar(21,pattern,1);
       int it11=tr.iTrTrackPar(1,pattern,11);
       int it12=tr.iTrTrackPar(3,pattern,21);
       int itav=tr.iTrTrackPar(1,pattern,21);
       int itplus=tr.iTrTrackPar(2,pattern,21,TrFit::Mproton,1,0,ecal.EnergyC);
      double Rigidity=it3>=0?tr.gTrTrackPar(it3).Rigidity:-1;
      double Rigidity8=it8>=0?tr.gTrTrackPar(it8).Rigidity:-1;
      double Rigidity9=it9>=0?tr.gTrTrackPar(it9).Rigidity:-1;
      double Rigidity10=it10>=0?tr.gTrTrackPar(it10).Rigidity:-1;
      double erig=it3>=0?tr.gTrTrackPar(it3).ErrRinv*Rigidity:-1;
      double ChiRigidity=it12>=0?tr.gTrTrackPar(it12).Rigidity:-1;
      double PiRigidity=it4>=0?tr.gTrTrackPar(it4).Rigidity:-1;
      double GRigidity=it5>=0?tr.gTrTrackPar(it5).Rigidity:-1;
      double rpi=PiRigidity/Rigidity;
      double rchi=GRigidity/Rigidity;
      double rmd=it11>=0?tr.gTrTrackPar(it11).Rigidity:-1;
      double rav=itav>=0?tr.gTrTrackPar(itav).Rigidity:-1;
      double rplus=itplus>=0?tr.gTrTrackPar(itplus).Rigidity:-1;
      double chisq=itav>=0?tr.gTrTrackPar(itav).Chisq:-1;
      if(chisq>5)return false;
      cout <<" chisq "<<chisq<<" "<<tr.gTrTrackPar(it11).Chisq<<" "<<tr.gTrTrackPar(it3).Chisq<<" "<<tr.gTrTrackPar(it7).Chisq<<endl;
      double chisq_plus=itplus>=0?tr.gTrTrackPar(itplus).Chisq:-1;
      double chisqx_plus=itplus>=0?tr.gTrTrackPar(itplus).ChisqX:-1;
      double chisqy_plus=itplus>=0?tr.gTrTrackPar(itplus).ChisqY:-1;
      int itminus=tr.iTrTrackPar(2,pattern,22,TrFit::Mproton,1,0,-ecal.EnergyC);
      double rminus=itminus>=0?tr.gTrTrackPar(itminus).Rigidity:-1;
      double chisqx_minus=itminus>=0?tr.gTrTrackPar(itminus).ChisqX:-1;
      double chisqy_minus=itminus>=0?tr.gTrTrackPar(itminus).ChisqY:-1;
      double chisq_minus=itminus>=0?tr.gTrTrackPar(itminus).Chisq:-1;
      double r=fabs(ecal.EnergyC/ChiRigidity);
      if(r<0.5 || r>10)return false;
      cout <<"qwa "<<tr.GetChisq(itminus)<<endl;
         if(Rigidity8/Rigidity9<0)return false;
      cout <<" Rigidity "<<Rigidity8<< " "<<Rigidity9<<" "<<Rigidity10<<" "<<Rigidity<<" "<<rmd<<" "<<rav<<" "<<ChiRigidity<<endl;
      cout <<" chisq "<<ecal.EnergyC<<" "<<rplus<< " "<<rminus<<" "<<chisq_plus<<" "<<chisq_minus<<endl;
      if(fabs(chisq_minus-chisq_plus)<1)return false;
if(0){
      if(rav>0){
        cerr<<ev->Run()<<" "<<ev->Event()<<" "<<ecal.EnergyC<<" "<<rav<<" "<<l19<<" "<<endl;
      }
      else{
        cerr<<ev->Run()<<" "<<ev->Event()<<" "<<ecal.EnergyC<<" "<<rav<<" "<<l19<<" "<<endl;
      }
}
        cerr<<ev->Run()-1305600000<<" "<<ev->Event()<<" "<<ecal.EnergyC<<" "<<rav<<" "<<l19<<" "<<ecalbdt<<" "<<v2<<" "<<ecal.RearLeak<<" "<<LikelihoodRatio[0]<<" "<<s1<<" "<<chisq<<" "<<ev->fHeader.RunType<<" "<<ev->LiveTime()<<endl;

}
    cout <<" processsetup "<<AMSEventR::ProcessSetup<<endl;
cout <<"AMSSetupR::ReadHeader-I-"<<ev->getsetup()->fScalers.size()<<" ScalersEntriesFound "<<endl;
        cout<<"AMSSetupR::ReadHeader-I-"<<ev->getsetup()->getAllTDV(ev->UTime())<<" TDVNamesFound"<<endl;
static int iqq=0;
return iqq++%100==0?true:false;
}

return false;


if(ev->getsetup()){
string reason="";
cout <<ev->getsetup()->fHeader.Run<<endl;
//cout << " badrun "<<ev->getsetup()->IsBadRun(reason,ev->UTime())<<" "<<reason<<endl;
//cout << " badrun3 "<<ev->IsBadRun("")<<" "<<endl;
return true;
}
if(ev && ev->nParticle() && ev->Particle(0).iTrTrack()>=0){
   
   TrTrackPar algref;
  Int_t Trfcode[5]={-1,-1,-1,-1,-1};
  int itrktr = ev->Particle(0).iTrTrack();
    TrTrackR trktr=ev->TrTrack(itrktr);
  for (int k=1;k<4;k++){
  Trfcode[k]=trktr.iTrTrackPar(k,0,2);
  if(Trfcode[k]>=0){
    algref=trktr.gTrTrackPar(Trfcode[k]);
    cout <<" fit "<<k<<" "<<algref.Rigidity<<" "<<algref.Chisq<<" "<<trktr.GetRigidity(4)<<endl;
  }
  }
  return 1;
}
else return 0;

if(ev){
cout<<ev->LiveTime()<<" lt "<<endl;
return 1;
}
if(ev && ev->nParticle() && ev->Particle(0).iTrTrack()>=0 && ev->Particle(0).iTrdTrack()>=0 && ev->Particle(0).iBeta()>=0 &&ev->Particle(0).iEcalShower()>=0 &&ev->nEcalShower()==1 && ev->EcalShower(0).EnergyC*(1-ev->EcalShower(0).RearLeak)>2){
unsigned int gps_time_sec=0;
unsigned int gps_time_nsec=0;
int ret=ev->GetGPSTime( gps_time_sec, gps_time_nsec);
//cout <<"time ret "<<ret<<" "<< gps_time_sec<< " "<< gps_time_nsec<<endl;
for(int k=0;k<ev->fHeader.GPSTime.size();k++){
 // cout <<" gpstime k "<<k<<" "<<ev->fHeader.GPSTime[k]<<endl;
}
 ret=ev->fHeader.GetGPSEpoche( gps_time_sec, gps_time_nsec);
//cout <<" ret "<<ret<<" "<< gps_time_sec<< " "<< gps_time_nsec<<endl;
cout <<"  status bits "<<((ev->fStatus)&3) <<" trd "<<((ev->fStatus>>2)&1)<<" tof "<<((ev->fStatus>>3)&1)<<" trk "<<((ev->fStatus>>4)&1)<<" ecal "<<((ev->fStatus>>6)&1)<<" necal "<<((ev->fStatus>>17)&3)<<" ene "<<((ev->fStatus>>37)&3)<<endl;



double xtime=double(ev->UTime())+ev->Frac();
cout <<ev->UTime()<<endl;
cout <<ev->Frac()<<endl;

cout <<setprecision(14);
cout <<" xtime "<<xtime<<endl;
AMSSetupR::ISSCTRSR setup;
if(ev->getsetup()){
int k=ev->getsetup()->getISSCTRS(setup, xtime);
cout <<" ret "<<k<<" "<<setup.r<<" "<<xtime<<endl;
xtime=xtime+30000;
 k=ev->getsetup()->getISSCTRS(setup, xtime);
cout <<" ret "<<k<<" "<<setup.r<<" "<<xtime<<endl;
return true;
AMSEventR::getsetup()->fHeader.FEventTime=xtime-10000;
AMSEventR::getsetup()->fHeader.LEventTime=xtime+10000;
AMSEventR::getsetup()->fHeader.Run=xtime;
AMSEventR::getsetup()->fISSCTRS.clear();
 k=ev->getsetup()->getISSCTRS(setup, xtime);
cout <<" ret "<<k<<" "<<setup.r<<" "<<xtime<<endl;
}

AMSEventR::getsetup()->fISSCTRS.clear();
return true;
}
else return false;
if(ev->getsetup()){

AMSSetupR::GPS gps=ev->getsetup()->GetGPS(ev->Run(),ev->Event());
cout <<" gps "<<ev->Event()<<" "<<gps.Event<<" "<<ev->getsetup()->fGPS.size()<<endl;

for( AMSSetupR::GPS_ri i=ev->getsetup()->fGPS.rbegin();i!=ev->getsetup()->fGPS.rend();i++){
   cout <<i->first<<" "<<i->second.Event<<" "<<i->second.EventLast<<endl;
 }

}

return 1;
if(ev && ev->nParticle() && ev->Particle(0).iTrTrack()>=0){

   TrTrackPar algref;
  Int_t Trfcode[5]={-1,-1,-1,-1,-1};
  int itrktr = ev->Particle(0).iTrTrack();
    TrTrackR trktr=ev->TrTrack(itrktr);
  for (int k=1;k<4;k++){
  Trfcode[k]=trktr.iTrTrackPar(k,0,0);
  if(Trfcode[k]>=0){
    algref=trktr.gTrTrackPar(Trfcode[k]);
    cout <<" fit "<<k<<" "<<algref.Rigidity<<" "<<algref.Chisq<<" "<<trktr.GetRigidity(4)<<endl;
  }
  }
  return 1;
}
else return 0;
/*
if(ev && ev->nParticle() && ev->Particle(0).iTrTrack()>=0 && ev->Particle(0).iEcalShower()>=0 && ev->EcalShower(0).EnergyC>90 && fabs(ev->TrTrack(ev->Particle(0).iTrTrack()).Rigidityf())<5){
return 1;
}

else return 0;
if(ev && ev->nParticle()){
for (int pindex=0;pindex<ev->nParticle();pindex++){
int itrktr = ev->Particle(pindex).iTrTrack();
 if(itrktr>=0){
   TrTrackR trktr=ev->TrTrack(itrktr);
    cout <<"  trying "<<endl;
    AMSPoint LayIntP=trktr.InterpolateLayerJ(3,17);

    cout <<LayIntP<<endl;
 }
}
*/
{
unsigned int eventSec, eventNanoSec;
ev->GetGPSTime(eventSec, eventNanoSec);
double xtime=ev->UTime()+ev->Frac();
double xtime0=ev->UTime()+ev->Frac();
cout <<setprecision(14);
cout <<" xtime "<<xtime<<endl;
AMSSetupR::ISSCTRSR setup;
AMSSetupR::GPSWGS84R gps;
if(ev->getsetup()){
ev->getsetup()->getISSCTRS(setup, xtime);
ofstream ftxt;
ftxt.clear();
ftxt.open("/f2users/choutko/ctrs.txt");
ftxt<<setprecision(14);
cout <<" qq "<<setup.r<<" "<<setup.v<<xtime<<endl;
for(int k=0;k<20001;k++){
  double step=0.005;
  xtime+=step;
ev->getsetup()->getISSCTRS(setup, xtime);
cout <<"r "<<xtime-xtime0<<" "<<setup.r<<" "<<setup.v<<endl;
ftxt<<xtime-xtime0<<" "<<setup.r<<" "<<setup.v<<" "<<ev->Run()<<" "<<ev->Event()<<endl;
}
cout <<" GOOD "<<ftxt.good()<<endl;
ftxt.close();
ev->getsetup()->getGPSWGS84(gps, eventSec+double(eventNanoSec)/1.e9);
cout <<" qqq "<<gps.r<<" "<<gps.v<<" "<<xtime<<" "<<eventSec+double(eventNanoSec)/1.e9<<endl;

}
}
return 1;
if(ev && ev->nParticle() && ev->Particle(0).iTrTrack()>=0 && ev->Particle(0).iEcalShower()>=0 && ev->EcalShower(0).EnergyC>2  ){
AMSSetupR::SlowControlR::ReadFromExternalFile=false;
 TrTrackR tr=ev->TrTrack(ev->Particle(0).iTrTrack());
   cout <<" ok "<<AMSSetupR::gethead()<<" "<<ev->UTime()<<endl;
  return true;
  cout << " fheader "<<AMSSetupR::gethead()->fHeader.FEventTime<<" "<<AMSSetupR::gethead()->fHeader.LEventTime<<endl;
vector<float>value;
int s2=  AMSSetupR::gethead()->fSlowControl.GetData("L03T+01X+08 / L06T+02Y-03",ev->UTime(),100.,value);
 cout <<" res2 "<<s2<<" "<<value.size()<<" value "<<endl;
 int s=ev->GetSlowControlData("L03T+01X+08 / L06T+02Y-03",value,1);
 cout <<" res "<<s<<" "<<value.size()<<" value "<<endl;
AMSSetupR::gethead()->fSlowControl.printElementNames("L03T+01X+08 / L06T+02Y-03");

AMSSetupR::TDVR tdv;
    int r = AMSSetupR::gethead()->getTDV("TRDStatus2",ev->UTime(),tdv);



vector<float> values;
 for(AMSSetupR::SlowControlR::etable_i it=AMSSetupR::gethead()->fSlowControl.fETable.begin();it!=AMSSetupR::gethead()->fSlowControl.fETable.end();it++){
   
    if(strstr(it->second.NodeName.Data(),"USCM_UG")&&it->second.datatype==25&&it->second.subtype<202){

      int stat=ev->getsetup()->fSlowControl.GetData(it->first.c_str(), ev->UTime(), 100., values);
      cout<<stat<<' '<<it->second.NodeName.Data()<<' '<<it->second.BranchName.Data()<<endl;
      cout<<it->first.c_str()<<"  -- stat "<<stat<<endl;

    }
  }

 cout <<" DataMc" <<tdv.DataMC<<" "<<tdv.FilePath<<  endl;


 AMSSetupR::DynAlignment_m &pars=AMSSetupR::gethead()->fDynAlignment;
 cout <<" size "<<pars[1].FitParameters.size()<<" "<<pars[9].FitParameters.size()<<endl;
  int k=tr.iTrTrackPar(1,0,5);
cout <<k<<" "<<ev->Particle(0).TRDCoo[0][2]<<" "<<ev->Particle(0).TRDCoo[1][2]<<endl;
return true;
}else return false;
if(ev && ev->nEcalShower() &&   ev->EcalShower(0).EnergyC>180 &&    ev->EcalShower(0).EnergyC<220 && ev->EcalShower(0).EnergyC/fabs(ev->Particle(0).Momentum)>1 ){
      cout <<ev->Event()<<" "<<ev->EcalShower(0).EnergyC<<endl;
return true;
}
else return false;
  if(ev && ev->nParticle()){
    cout <<"0 "<<ev->Particle(0).TRDCoo[0][2]<<endl;
    cout <<"1 "<<ev->Particle(0).TRDCoo[1][2]<<endl;
    for(int k=0;k<9;k++)cout <<k<<" "<<ev->Particle(0).TrCoo[k][1]<<" "<<ev->Particle(0).TrCoo[k][2]<<endl;

    return 1;
  }
  
 //cout<<ev->getsetup()->fScalers[1]._LiveTime[0]<<" "<<ev->getsetup()->fScalers[1]._LiveTime[1]<<endl;
// cout <<ev->fHeader.Roll<<" "<<ev->fHeader.Pitch<<" "<<ev->fHeader.Yaw<<endl;
 unsigned int gps,gpsn;
 //int ret=ev->GetGPSTime(gps,gpsn);
 //cout <<"  ret "<<endl;
 //for(int k=0;k<ev->fHeader.GPSTime.size();k++){
 //  cout <<" GPS "<<k<<" "<<ev->fHeader.GPSTime[k]<<endl;
// }
 //cout <<" gps "<<ret<<" "<<gps<<" "<<gpsn<<" "<<ev->UTime()<<" "<<ev->fHeader.Time[1]<<endl; 
//return true;
   if(ev && ev->nLevel1()   && ev->nParticle()  && ev->nTrTrack() && ev->nTrdTrack() && ev->Particle(0).iTrTrack()>=0 && ev->Particle(0).iEcalShower()>=0 && ev->EcalShower(0).EnergyC>200 ){

//   if(ev && ev->nLevel1()   && ev->nParticle()  && ev->nTrTrack() && ev->nTrdTrack() && ev->Particle(0).iTrTrack()>=0 && ev->Particle(0).iEcalShower()>=0 && ev->EcalShower(0).EnergyC>20 && ev->EcalShower(0).S13R!=0 && ev->EcalShower(0).RearLeak<0.99 ){
   //if(ev && ev->nLevel1()   && ev->nParticle()  && ev->Particle(0).Beta>0 && ev->nTrTrack()  && ev->Particle(0).Charge>1 && ev->Particle(0).iTrTrack()>=0 &&  ev->Particle(0).iTrdTrack()<0){
//   cout<<ev->EcalShower(0).AttLeak<<" "<<ev->EcalShower(0).S13Leak<<" "<<ev->EcalShower(0).S13R<<endl;
  cout <<ev->fHeader.Roll<<" "<<ev->fHeader.Pitch<<" "<<ev->fHeader.Yaw<<endl;
  return true;
  }
  else return false;
    static int a=0;
    cout <<ev->getsetup()<<endl;
    cout<<ev->getsetup()->fScalers[0]._LiveTime[0]<<endl;
    vector<float> aa;
    int k=ev->getsetup()->fSlowControl.GetData("B-Field P0 Component X",ev->UTime(),ev->Frac(),aa);
    cout <<"size "<<aa.size()<<" res "<<k<<endl;
     if(aa.size())cout <<"  ee "<<aa[0]<<endl;
    k=ev->getsetup()->fSlowControl.GetData("Zenith PT1000 4-3B (outlet)",ev->UTime(),ev->Frac(),aa);
 //   return true;
    // 
    // This is a user function to be modified
    //  return true if event has to be drawn false otherwise
//   if(ev && (ev->fStatus &3) &&( (ev->fStatus>>4 &1)))return true;
//   else return false;
     if(ev && ev->nLevel1()   && ev->nParticle()  && ev->nTrTrack() && ev->Particle(0).iTrTrack()>=0 && ev->nBeta() && fabs(ev->Beta(0).Beta)==1)return true;
else return false;
if(ev && ev->nLevel1()){
  Level1R l1=ev->Level1(0);
    if(1 || (l1.EcalFlag/10)>=2 || (l1.EcalFlag%10)>1){
      cout << "EFlag "<<l1.EcalFlag<<endl;
      cout <<ev->Run()<<" "<<ev->Event()<<endl;
      for (int i=0;i<16;i++){
        cout <<i<<" "<<(((l1.JMembPatt)>>i)&1)<<endl;
}
       return true;
    }
    else return false;
}
      if(ev){
   a++;
        if(ev->nMCTrack()){
           double step=0;
           for(int i=0;i<ev->nMCTrack();i++){
            MCTrackR tr= ev->MCTrack(i);
             step+=tr.RadL;
             if(strstr(tr.VolName,"STK6"))cout <<tr.VolName<<" "<<tr.Pos[1]<<" "<<tr.Pos[2]<<" "<<tr.RadL<<endl;
          }
         cout <<" got "<<step<<" "<<a<<endl;
         return true;
        }     
  }
  return false;
     
}
/*
    int          membpat=ev->pLevel1(0)->JMembPatt;
    int b15=(membpat>>15)&1;
    int b14=(membpat>>14)&1;
         if(!ev->nEcalShower() && ev->nEcalCluster()){
           cout<<" "<<ev->nEcalCluster()<<" "<<ev->nEcal2DCluster()<<" "<<ev->nEcalHit()<<endl;
           for(int i=0;i<ev->nEcalCluster();i++){
                EcalClusterR ec=ev->EcalCluster(i);
                cout <<" i "<<ec.Status<<" "<<ec.Plane<<" "<<ec.Coo[0]<<" "<<ec.Coo[1]<<" "<<ec.Coo[2]<<" "<<ec.Edep<<endl;
            }
           return true;
        }
           else return false;
    //      if(! (b14 && b15))return false; 
          cout <<ev->nTrTrack()<<" "<<ev->NTrTrack()<<endl;
           TrTrackR tr=ev->TrTrack(ev->Particle(0).iTrTrack());
           cout <<" q "<<tr.NTrRecHit()<<endl;
           return true;
           for(int i=0;i<tr.NTrRecHit();i++){
              TrRecHitR rh=ev->TrRecHit(tr.iTrRecHit(i));
              bool ok=rh.lay()==3 && rh.half()==0 && rh.lad()==2;
//              ok =ok || (rh.lay()==5 && rh.half()==1 && rh.lad()==3);
              if(ok)return true;
            }
            return false;           
           return true;
         }
     else return false;
  
  try{
    if(ev && ev->nDaqEvent() && ev->nParticle() &&  !ev->nTrdTrack() && !ev->nTrTrack() &&ev->nEcalShower() ){
       
     if(ev->EcalShower(0).EnergyC*(1-ev->EcalShower(0).RearLeak)>1 && ev->pDaqEvent(0)->Length<5000){
        cout <<++a<<endl;
return false; 
      }
      else return false;
    if(ev->Beta(0).Pattern==0 && ev->Particle(0).Mass>1.5 && fabs(ev->Particle(0).Momentum)<5 && ev->Particle(0).Beta>0 && ev->Particle(0).Beta<0.92){
cout <<ev->Event()<<endl;
return true;
} 
      if(ev->pLevel1(0)->TrigTime[4]<160 || ev->pDaqEvent(0)->Tdr>9000)return false;
      else if(ev &&ev->nParticle() && ev->nTrTrack() &&ev->nEcalShower() &&ev->EcalShower(0).EnergyC*(1-ev->EcalShower(0).RearLeak)>0.1)return false;
      else if (ev &&ev->nParticle() && ev->nTrTrack() && ev->Particle(0).Charge>1 && ev->nTofCluster()>2)return false;
       else return false;
    }
    else{
      return false;
    }
  }
  catch (...)
{
return false;
}
*/
};

//
//  The code below should not be modified
//
#ifndef WIN32
extern "C" void fgSelect(){
  AMSNtupleHelper::fgHelper=new AMSNtupleSelect(); 
  cout <<"  Handle Loadedd "<<endl;
}

#else
#include <windows.h>
BOOL DllMain(HINSTANCE hinstDLL,  // DLL module handle
	     DWORD fdwReason,              // reason called 
	     LPVOID lpvReserved)           // reserved 
{ 
 
  switch (fdwReason) 
    { 
      // The DLL is loading due to process 
      // initialization or a call to LoadLibrary. 
 
    case DLL_PROCESS_ATTACH: 
      fgHelper=new AMSNtupleSelect();
      cout <<"  Handle Loadedd "<<fgHelper<<endl;
      break;

    case DLL_PROCESS_DETACH: 
      if(fgHelper){
	delete fgHelper;
	fgHelper=0;
      } 
 
      break; 
 
    default: 
      break; 
    } 
  return TRUE;
}
extern "C" AMSNtupleHelper * gethelper(){return fgHelper;}

#endif
