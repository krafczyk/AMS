///  This is an example to use Root-Mode TofRecH::ReBuild and TofH software : Select_Tof()
/*!
  First need TofRecH::ReBuild() \n
  BetaHR and TofClusterHR can be accessed from AMSEventR, ParticleR or ChargeR  \n
  BetaH Version Software Based on IHEP version Calibration and Reconstruction \n
  Both Support gbatch and Root-Mode  \n
  Provide both best TOF-Charge and Beta Measurment \n
*/
// -----------------------------------------------------------
//        Created:       2012-Aug-17  Q.Yan  qyan@cern.ch
//        Add:           2012-Oct-11  Adding BetaHR charge example GetQ GetQL
//        Add:           2012-Nov-17  Adding example TofChargeHR Likelihood Charge-Z Prob-Z:  PDF validate for all charge Z=1~Z>26
// -----------------------------------------------------------

#include <signal.h>
#include <TChain.h>
#include <TLine.h>
#include <TSystem.h>
#include <TStyle.h>
#include <iostream>
#include <iomanip>
#include  "TMath.h"
#include "TMinuit.h"
#include "TTree.h"
#include <sstream>
#include "TString.h"
#include <fstream.h>
#include <iostream>
#include "root_RVSP.h"
#include "amschain.h"
#include "Tofrec02_ihep.h"
#include "Tofcharge_ihep.h"
#include "readfile.C"


namespace tofconst{
  const int NTKL=9;
  const int NTKS=2;
  const int NTKDIS=4;
  const float TKDIS[NTKDIS]={1,2,4,6};//dis=cm
  const int NTOFL=4;
  const int NTOFBM=10;
  const int NTOFS=2;
  const int NBAR[NTOFL]={8,8,10,8};
  const float TOFZ[NTOFL]={65.2,62.1,-62.1,-65.2};//central cm  
  const int TRANS[NTOFL]={0,1,1,0};//trans pos 
  int           sstat[10]={0};
}
using namespace tofconst;


class AMSAnalysis{
  public:
   AMSAnalysis():fout(0),tout(0),pev(0){};
   ~AMSAnalysis(){};
   void BookFile(char *ofile);
   void Save();
//---
   void InitEvent(AMSEventR *ev);   
   bool Select();
   bool Select_Trigger();
   bool Select_Trd();
   bool Select_Tof();
   bool Select_Trk(int pre=0);//pre selction 1 or 0
   bool Select_Rich();
   bool Select_Ecal();  
   bool Select_Anti();
   void Fill();
//--
   TFile *fout;
   TTree *tout;

   AMSEventR *pev;
   int iparindex;
   unsigned int run;
   unsigned int time[2];
   unsigned int event;
//---
   float        thetam;//magnetic theta
   float        phim;
   float        thetas;
   float        phis;
   float        livetime;
   int          nlevel1;
//---
   int          nparticle;
   int          ntrack;
   int          ntrdtrack;
   int          ntrdseg;
   int          ntrdrawh;
   int          ntrdcl;
   int          itrdtrack;
   int          nitrdseg;
   float        distrd[3];///0x ,1y, 2theta
   float        trdcle[5];//0-20 layer divide by 5*4(Fragment)
   float        charge;
   float        cutoff;
//----tk
   int          tk_nhit;//YX
   int          tk_hitb[2];//XY
   float        tk_q;
   int          tk_charge;
   float        tk_rigidity;//raw inner
   float        tk_chis[3];//rawx inner chisx+y+s
   float        tk_rigidityUL[2];//rawx inner u+d rigidity
//----
   float        tk_rigidityI[4];//inner rigidity+mulscatter+charge+beta
   float        tk_chisI[4][3];//inner chis
   float        tk_rigidityIUL[4][2];
   float        tk_dedx[9][2];//trakc dedx(X+Y)
   float        tk_dedx_ns[9][2][NTKDIS]; //near 1cm +2cm+4cm+8cm sum all
   float        tk_dir[9];
//---tof
   int          tof_barid[NTOFL];
   float        tof_tkdir[NTOFL][3];
   float        tof_adca[NTOFL][NTOFS];
   float        tof_adcd[NTOFL][NTOFS];
//--calibration 
   unsigned int tof_bstatus[NTOFL][2];
   float        tof_adcdr[NTOFL][NTOFS][3];
//--
   float        tof_betah;
   float        tof_betahs;
   int          tof_nhith;
   int          tof_hsumh;
   int          tof_hsumhu;
   int          tof_nhithl[4];

//---BetaH Charge
   float        tof_hqt;
   float        tof_hqg;
   float        tof_hql[4];
   float        tof_hqc[4];
   int          tof_hz;
   float        tof_hprobz;
   int          tof_hzu;//Up Tof Z: TofChargeHR Support Dynamic Likelihood ReFit For select Pattern
   int          tof_hzd;//Dow Tof Z
   float        tof_hq;
   float        tof_hlikq;
   float        tof_hprobz6;//Prob to be Carbon Z=6: User Can Use Prob Select Each Nucleus
//--Time
   float        tof_t0;
   float        tof_htl[4];
   float        tof_htc[4];
//---
   float        tof_temp[4][2];
   float        tof_tempC[4][2];
   float        tof_tempP[4][2];
   float        tof_chist;
   float        tof_chisc;
//---trd
   float        trd_q; 
   int          trd_charge;
//---anti
   int          anti_nhit;
//---ecal
  float         ecal_edep;
};


void AMSAnalysis::BookFile(char *ofile){
  fout=new TFile(ofile,"RECREATE");
//--book tree
  tout=new TTree("amstree","amstree");
  tout->Branch("run",&run,"run/i");
  tout->Branch("event",&event,"event/i");
  tout->Branch("thetam",&thetam,"thetam/F");
  tout->Branch("phim",&phim,"phim/F");
  tout->Branch("thetas",&thetas,"thetas/F");
  tout->Branch("phis",&phis,"phis/F");
  tout->Branch("nlevel1",&nlevel1,"nlevel1/I");
  tout->Branch("livetime",&livetime,"livetime/F");
//---
  tout->Branch("time",time,"time[2]/i");
  tout->Branch("nparticle",&nparticle,"nparticle/I");
  tout->Branch("ntrack",&ntrack,"ntrack/I");
  tout->Branch("ntrdtrack",&ntrdtrack,"ntrdtrack/I");
  tout->Branch("ntrdseg",&ntrdseg,"ntrdseg/I");
  tout->Branch("ntrdrawh",&ntrdrawh,"ntrdrawh/I");
  tout->Branch("ntrdcl",&ntrdcl,"ntrdcl/I");
  tout->Branch("itrdtrack",&itrdtrack,"itrdtrack/I");
  tout->Branch("nitrdseg",&nitrdseg,"nitrdseg/I");
  tout->Branch("distrd",  distrd,   "distrd[3]/F");
  tout->Branch("trdcle",  trdcle,   "trdcle[5]/F");  

  tout->Branch("charge",&charge,"charge/F");
  tout->Branch("cutoff",&cutoff,"cutoff/F");

//--Track
  tout->Branch("tk_nhit",&tk_nhit,"tk_nhit/I"); 
  tout->Branch("tk_hitb",tk_hitb,"tk_hitb[2]/I");
  tout->Branch("tk_q",&tk_q,"tk_q/F");
  tout->Branch("tk_charge",&tk_charge,"tk_charge/I");
  tout->Branch("tk_rigidity",&tk_rigidity,"tk_rigidity/F");
  tout->Branch("tk_chis",tk_chis,"tk_chis[3]/F");
  tout->Branch("tk_rigidityUL",tk_rigidityUL,"tk_rigidityUL[2]/F");
//---Vital+Al+Ch
  tout->Branch("tk_rigidityI",tk_rigidityI,"tk_rigidityI[4]/F");
  tout->Branch("tk_chisI",tk_chisI,"tk_chisI[4][3]/F");
  tout->Branch("tk_rigidityIUL",tk_rigidityIUL,"tk_rigidityIUL[4][2]/F");
  tout->Branch("tk_dedx",tk_dedx,"tk_dedx[9][2]/F");
  tout->Branch("tk_dedx_ns",tk_dedx_ns,"tk_dedx_ns[9][2][4]/F");
  tout->Branch("tk_dir",tk_dir,"tk_dir[9]/F");
//--Tof 
  tout->Branch("tof_barid",tof_barid,"tof_barid[4]/I");
  tout->Branch("tof_bstatus",tof_bstatus,"tof_bstatus[4][2]/i");
  tout->Branch("tof_tkco",tof_tkco,"tof_tkco[4][3]/F");  
  tout->Branch("tof_tkdir",tof_tkdir,"tof_tkdir[4][3]/F");
   
//----
  tout->Branch("tof_adcdr",tof_adcdr,"tof_adcdr[4][2][3]/F");
//---
  
  tout->Branch("tof_betah",&tof_betah,"tof_betah/F");
  tout->Branch("tof_betahs",&tof_betahs,"tof_betahs/F");
  tout->Branch("tof_nhith",&tof_nhith,"tof_nhith/I");//ev->nTofClusterHs
  tout->Branch("tof_hsumh",&tof_hsumh,"tof_hsumh/I"); //BetaH
  tout->Branch("tof_hsumhu",&tof_hsumhu,"tof_hsumhu/I");//
  tout->Branch("tof_nhithl",tof_nhithl,"tof_nhithl[4]/I");

//---BetaH Charge
  tout->Branch("tof_hqt",&tof_hqt, "tof_hqt/F");//Q- Truncate Mean //Better For Low Charge(Pr He Li)
  tout->Branch("tof_hqg",&tof_hqg, "tof_hqg/F");//Q- Global Gaus Mean  //Better For High Charge
  tout->Branch("tof_hql", tof_hql, "tof_hql[4]/F");//Q for each Layer
  tout->Branch("tof_hqc", tof_hqc, "tof_hqc[4]/F");//Q for TofClusterHR(without Beta-Corr+Atten-Dis by Tof-TLCoo Self)
  tout->Branch("tof_hz", &tof_hz, "tof_hz/I");// TOF-Most Prob Charge
  tout->Branch("tof_hprobz", &tof_hprobz, "tof_hprobz/F");//TOF-Mose Prob Charge ProbZ
  tout->Branch("tof_hzu", &tof_hzu, "tof_hzu/I");// Up-TOF-Most Prob Charge ///TofChargeHR Support Dynamic Likelihood ReFit For select Pattern
  tout->Branch("tof_hzd", &tof_hzd, "tof_hzd/I");// Dow-TOF-Most Prob Charge
  tout->Branch("tof_hq", &tof_hq, "tof_hq/F");//Q From TofChargeHR should be better than From BetaH For Z~3-8, Due to tuning better Threshold Between Anode and Dynode)
  tout->Branch("tof_hlikq", &tof_hlikq, "tof_hlikq/F");//Q-Using Likelihood Method to Get Float-Q(More Gaus)
  tout->Branch("tof_hprobz6", &tof_hprobz6, "tof_hprobz6/F");// User Can Use Prob Select Each Nucleus
   
//---BetaH Time
  tout->Branch("tof_t0",   &tof_t0,    "tof_t0/F");
  tout->Branch("tof_htl", tof_htl,   "tof_htl[4]/F");
  tout->Branch("tof_htc", tof_htc,   "tof_htc[4]/F");

//---
  tout->Branch("tof_temp", tof_temp,"tof_temp[4][2]/F");
  tout->Branch("tof_tempC", tof_tempC,"tof_tempC[4][2]/F");
  tout->Branch("tof_tempP", tof_tempP,"tof_tempP[4][2]/F");
  tout->Branch("tof_chist",&tof_chist,"tof_chist/F");
  tout->Branch("tof_chisc",&tof_chisc,"tof_chisc/F");

//---Trd  
  tout->Branch("trd_q",&trd_q,"trd_q/F");
  tout->Branch("trd_charge",&trd_charge,"trd_charge/I");

// anti counter
  tout->Branch("anti_nhit",&anti_nhit,"anti_nhit/I"); 

//--ecal sumdep
  tout->Branch("ecal_edep",&ecal_edep,"ecal_edep/F"); 
}

bool AMSAnalysis::Select(){
   nparticle=pev->nParticle();
   for(int ipa=0;ipa<nparticle;ipa++){//on track
         if(pev->pParticle(ipa)->iTrTrack()>=0){iparindex=ipa;break;}
    }
   run=pev->Run();
   event=pev->Event();
   time[0]=pev->fHeader.Time[0];
   time[1]=pev->fHeader.Time[1];
   thetas=pev->fHeader.ThetaS*180/3.1415926;
   phis=pev->fHeader.PhiS*180/3.1415926;
   thetam=pev->fHeader.ThetaM*180/3.1415926;
   phim=pev->fHeader.PhiM*180/3.1415926;
   sstat[0]++;
   //if(pev->nTrTrack()<1)return false;
//   sstat[1]++;
   if(pev->nParticle()<1||iparindex<0)return false;
   charge=pev->pParticle(iparindex)->Charge;
   cutoff=pev->pParticle(iparindex)->Cutoff;
   sstat[1]++;
//--Preselection
   if(!Select_Trk(1))return false;
   sstat[2]++;
//--Strict Selection
   if(!Select_Trigger())return false;
   sstat[3]++;
   if(!Select_Tof())return false;
   sstat[4]++;
   if(!Select_Trk(0))return false;
   sstat[5]++;
   if(!Select_Trd())return false;
   sstat[6]++;
   if(!Select_Rich())return false;
   sstat[7]++;
   if(!Select_Ecal())return false; 
   sstat[8]++;
   if(!Select_Anti())return false;    
   sstat[9]++;
   return true;
}

bool AMSAnalysis::Select_Trigger(){
    nlevel1=pev->nLevel1();
    if(nlevel1>0){
      Level1R *lv=pev->pLevel1(0);
      livetime=lv->LiveTime;
    } 
    return true;
}
bool AMSAnalysis::Select_Trk(int pre){
    int algo  = 1; // 1:Choutko  2:Alcaraz  (+10 no multiple scattering)
    int patt  = 3; // 3:Inner only 5:w/ L1N 6:w/ L9 7:Full span
    int refit = 2; // 
    int mfit;
    TrTrackR *trk=pev->pParticle(iparindex)->pTrTrack();
   if(pre){
     ntrack=pev->nTrTrack();
//---first charge
     ChargeR *charge=pev->pParticle(iparindex)->pCharge();
     if(!charge)return false;
     ChargeSubDR *zTrk=charge->getSubD("AMSChargeTracker");
     if(!zTrk)return false;
     tk_charge = TMath::Max(Int_t(zTrk->ChargeI[0]),1);
     tk_q = zTrk->Q;
//     if(tk_charge<2)return false;

//--Tk quality
//---Select track for particle
     if(trk->GetNhits()<=4||trk->GetNhitsXY()<=3)return false;
     tk_nhit=trk->GetNhitsY()*10+trk->GetNhitsX();
     tk_hitb[0]=trk->GetBitPatternXYJ(); tk_hitb[1]=trk->GetBitPatternJ();
//---
     mfit = trk->iTrTrackPar(algo, patt, refit);
     if(mfit<0)return false;
//    if(trk->GetNormChisqX(mfit)>50||trk->GetNormChisqY(mfit)>10)return false;
     tk_chis[0]=trk->GetNormChisqX(mfit);
     tk_chis[1]=trk->GetNormChisqY(mfit);
     tk_chis[2]=trk->GetChisq(mfit); 
     tk_rigidity=trk->GetRigidity(mfit);
     patt=1;//inner up
     mfit = trk->iTrTrackPar(algo, patt, refit);
     if(mfit<0){tk_rigidityUL[0]=9999;}
     else       tk_rigidityUL[0]=trk->GetRigidity(mfit);
     patt=2;//inner down
     mfit = trk->iTrTrackPar(algo, patt, refit);
     if(mfit<0){tk_rigidityUL[1]=9999;}
     else       tk_rigidityUL[1]=trk->GetRigidity(mfit); 
   }
  
  else {
//-----Inner Tracker
//--Vitaly+Alcaraz+ChikanF+ChikanC
    for(int ialg=1;ialg<=4;ialg++){
       patt=3;//inter
       mfit = trk->iTrTrackPar(ialg, patt, refit,0,tk_charge,tof_betah);
       if(mfit<0){
         tk_rigidityI[ialg-1]=9999;
         tk_chisI[ialg-1][0]=tk_chisI[ialg-1][1]=tk_chisI[ialg-1][2]=9999;
       }
       else      { 
         tk_rigidityI[ialg-1]=trk->GetRigidity(mfit);
         tk_chisI[ialg-1][0]=trk->GetNormChisqX(mfit);
         tk_chisI[ialg-1][1]=trk->GetNormChisqY(mfit);
         tk_chisI[ialg-1][2]=trk->GetChisq(mfit); 
       }
       patt=1;//inner up
       mfit = trk->iTrTrackPar(ialg, patt, refit,0,tk_charge,tof_betah);
       if(mfit<0){tk_rigidityIUL[ialg-1][0]=9999;}
       else       tk_rigidityIUL[ialg-1][0]=trk->GetRigidity(mfit);
       patt=2;//inn/r down
       mfit = trk->iTrTrackPar(ialg, patt, refit,0,tk_charge,tof_betah);
       if(mfit<0){tk_rigidityIUL[ialg-1][1]=9999;}
       else       tk_rigidityIUL[ialg-1][1]=trk->GetRigidity(mfit);
    }

//--Track Edep
    for(int ilay=0;ilay<NTKL;ilay++){
//---
      AMSPoint postr;AMSDir dirtr;
      algo=1; patt=3;//inner
      mfit = trk->iTrTrackPar(algo, patt, refit);
      trk->InterpolateLayerJ(ilay+1,postr,dirtr,mfit);
      tk_dir[ilay]=dirtr[2];
//---
      TrRecHitR *tkhit=trk->GetHitLJ(ilay+1);
      TrClusterR *tkcl[2]={0};
      if(tkhit){
        tkcl[0]=tkhit->GetXCluster();tkcl[1]=tkhit->GetYCluster();
        for(int ixy=0;ixy<NTKS;ixy++){
          if(tkcl[ixy])tk_dedx[ilay][ixy]=tkcl[ixy]->GetEdep(); 
         }
       } 
      for(int icl=0;icl<pev->nTrCluster();icl++){
         TrClusterR *cltr=pev->pTrCluster(icl);
         if((cltr==0)||(cltr==tkcl[0])||(cltr==tkcl[1])||cltr->GetLayerJ()!=ilay+1){continue;}//used 
         float cldis=999999;
         for(int m=0;m<cltr->GetMultiplicity();m++){//reso multi
            float muldis=cltr->GetGCoord(m)-postr[cltr->GetSide()];
            if(fabs(muldis)<cldis){cldis=fabs(muldis);}
         }
         for(int idis=0;idis<NTKDIS;idis++){
            if(cldis<TKDIS[idis]){tk_dedx_ns[ilay][cltr->GetSide()][idis]+=cltr->GetEdep();}
         }
       }
//----
    }
//----

  }//end else
   return true;
}

bool AMSAnalysis::Select_Trd(){
    ntrdtrack=pev->nTrdTrack();
    ntrdseg=pev->nTrdSegment();
    ntrdrawh=pev->nTrdRawHit();
    ntrdcl=pev->nTrdCluster();
    for(int itrdcl=0;itrdcl<ntrdcl;itrdcl++){
       TrdClusterR *trdcl=pev->pTrdCluster(itrdcl);
       trdcle[trdcl->Layer/4]+=trdcl->EDep;
    }
//---Trd Charge
    itrdtrack=pev->pParticle(iparindex)->iTrdTrack();
    if(itrdtrack>=0){
       TrdTrackR *trdtrack=pev->pParticle(iparindex)->pTrdTrack();
       nitrdseg=trdtrack->NTrdSegment();
//---TrdTrack Match
       AMSDir dir;AMSPoint pos;
       TrTrackR *trk=pev->pParticle(iparindex)->pTrTrack();
       trk->Interpolate(trdtrack->Coo[2],pos,dir);
       distrd[0]=trdtrack->Coo[0]-pos.x();
       distrd[1]=trdtrack->Coo[1]-pos.y();
       AMSDir trddir(trdtrack->Theta,trdtrack->Phi);
       distrd[2]=acos(fabs(trddir.prod(dir)))*180/3.1415926;
//-----     
      ChargeR *charge=pev->pParticle(iparindex)->pCharge();
      if(!charge)return false;
      ChargeSubDR *zTrd=charge->getSubD("AMSChargeTRD");
      if(zTrd){
         trd_charge = TMath::Max(Int_t(zTrd->ChargeI[0]),1);
         trd_q = zTrd->Q;
      }
      else {
       trd_charge=-1;
       trd_q=-1;
     }
   }
    return true;
}

bool AMSAnalysis::Select_Tof(){

//---
     TofRecH::ReBuild();
     tof_nhith=pev->nTofClusterH();//Sum of TofClusterHR (Tof-Counter Number Fired)
     BetaHR *betah=pev->pParticle(iparindex)->pBetaH();

//     if(!betah)return false;
     if(betah&&betah->IsTkTofMatch()){//Tk Not Match with TOF, Cut This Event

       tof_betah=betah->GetBeta();//Beta Measurement
       tof_betahs=betah->GetBetaS();//Reselect Good TOF-Layer For Beta Fit ->Serious Backsplish Recommend to Use This Beta
///--BetaH TOF Q and TOFChargeHR Likelihood integer Z , Prob, LikelihoodQ
       int nlay; float qrms;
       tof_hqt=betah->GetQ(nlay,qrms);//TOF Trancate Mean Q
       tof_hqg=betah->GetQ(nlay,qrms,2,TofClusterHR::DefaultQOpt,-1);//TOF Gaus Mean Q
//--To TofChargeHR
       TofChargeHR *tofcharge=betah->pTofChargeH();//Access To TofChargeHR
       tof_hz=tofcharge->GetZ(nlay,tof_hprobz);//Max-Prob Integer Z+ProbZ
       float probu,probd;
       tof_hzu=tofcharge->GetZ(nlay,probu,0,1100); //Using Up TOF-Two-Layer Likelihood To PID ///TofChargeHR Support Dynamic Likelihood ReFit For select Pattern
       tof_hzd=tofcharge->GetZ(nlay,probd,0,11); //Using Down TOF-Two-Layer Likelihood To PID
       tof_hq=tofcharge->GetQ(nlay,qrms);//Float Q From TofChargeHR should be better than From BetaH For Z~3-8, Due to tuning better Threshold Between Anode and Dynode)
       tof_hlikq=tofcharge->GetLikeQ(nlay);//Float Q-Using Likelihood Method (More Gaus)
       tof_hprobz6=tofcharge->GetProbZ(6); //Can Using Prob>Th to Select Carbon
       
//---TOF Useful cut
       tof_chist=betah->GetChi2T();//Time Chis
       tof_chisc=betah->GetChi2C();// Space Chis
       tof_hsumh=betah->GetSumHit();// Sum Hit Match by BetaH
       tof_hsumhu=betah->GetUseHit();//Use TOF-Layer For Beta-Fit

//Layer Level
       for(int ilay=0;ilay<NTOFL;ilay++){
          tof_nhithl[ilay]=betah->GetAllFireHL(ilay);//Number of Bar of ilay-TOF fired
//--Test if This Layer Exist-By BetaH
         if(betah->TestExistHL(ilay)){
           tof_hql[ilay]=betah->GetQL(ilay);//Q-Measurment for Each TOF-Layer
           if(betah->IsBetaUseHL(ilay))tof_htl[ilay]=betah->GetTime(ilay);//Time-Measurment for Each TOF-Layer
//---To TofClusterHR
           TofClusterHR *tofclh=betah->GetClusterHL(ilay);//Access to TofClusterHR
           tof_barid[ilay]=tofclh->Bar;//TOF-Barid
           tof_hqc[ilay]=  tofclh->GetQSignal();//Q Measurement From TofClusterHR(From BetaH is much better)
           if(tofclh->IsGoodTime())tof_htc[ilay]=tofclh->Time;//Time-Measurment From TofClusterHR
           for(int is=0;is<2;is++){
             tof_bstatus[ilay][is]=tofclh->SideBitPat[is];
             for(int ipm=0;ipm<3;ipm++)tof_adcdr[ilay][is][ipm]=tofclh->Dadc[is][ipm];
//--To TofRawSideHR
             if(betah->GetClusterHL(ilay)->TestExistHS(is)){
                tof_temp[ilay][is]= tofclh->GetRawSideHS(is)->temp;//Assess to TofRawSideR
                tof_tempC[ilay][is]=tofclh->GetRawSideHS(is)->tempC;
                tof_tempP[ilay][is]=tofclh->GetRawSideHS(is)->tempP;
              }
           }
        }
//--Test
      }//End Layer

//---
   }

  return true; 
}

bool AMSAnalysis::Select_Rich(){
  return true;
}

bool AMSAnalysis::Select_Ecal(){
   for(int i=0;i<pev->nEcalHit();i++){
        ecal_edep+=pev->pEcalHit(i)->Edep/1000.;
    }
   //if(sumedep>1)return 0;
   return true;
}

bool AMSAnalysis::Select_Anti(){
  anti_nhit=pev->nAntiCluster();
  return true;
}


void AMSAnalysis::InitEvent(AMSEventR *ev){
   run=0;event=0;time[0]=0;time[1]=0;
   nparticle=0;
   ntrack=0;
   ntrdtrack=0;
   iparindex=-1;
   tk_nhit=0;
   tk_charge=-1;
   tk_q=-1.;
   tk_rigidity=0;
   tof_betah=-3;
   tof_betahs=-3;
   tof_hqt=-1;
   tof_hqg=-1;
   tof_hprobz=tof_hprobz6=tof_hq=tof_hlikq=tof_hzu=tof_hzd=tof_hz=-1;
//---
   ecal_edep=0;
   for(int itkl=0;itkl<NTKL;itkl++){
     tk_dir[itkl]=0;
     for(int ixy=0;ixy<NTKS;ixy++){
         tk_dedx[itkl][ixy]=0;
         for(int idis=0;idis<NTKDIS;idis++)tk_dedx_ns[itkl][ixy][idis]=0;
       }
    }
   
   for(int itrdv=0;itrdv<5;itrdv++){trdcle[itrdv]=0;}
   for(int itfl=0;itfl<NTOFL;itfl++){
      tof_hql[itfl]=-1;
      tof_htl[itfl]=-1;
      tof_hqc[itfl]=-1;
      tof_htc[itfl]=-1;
      tof_barid[itfl]=-1;
      for(int itfs=0;itfs<NTOFS;itfs++){
        for(int ipm=0;ipm<3;ipm++)tof_adcdr[itfl][itfs][ipm]=-1;
       }
   }
  pev=ev;
}

void AMSAnalysis::Fill(){
 tout->Fill();
}

void AMSAnalysis::Save(){
    fout->cd();
    tout->Write();
    fout->Close();
}

int stop=0;
void shandler(int sig){
  stop=1;
  return;
}

int main(int argc,char ** argv){//in_file ofile num
    char ifile[1000];
    char ofile[1000];
    int rmode=0;//readmode 0 txt 1 root
    int num=-1;
    cout<<"argc="<<argc<<endl;
    if(argc>=3){
      sprintf(ifile,"%s",argv[1]);
      sprintf(ofile,"%s",argv[2]);
    }
    else {
        printf("Usage: %s  <infile_txt> <ofile.root> <max_events_number>\n",argv[0]);
        exit(-1);
    }

    if (argc>=4){
       rmode=atoi(argv[3]);
       cout<<"rmode="<<rmode<<endl;
    }
    if(argc>=5){
       num=atoi(argv[4]);
       if(num==0)num=-1;
       cout<<"num="<<num<<endl;
    }

    signal(SIGTERM, shandler);
    signal(SIGINT, shandler);

    AMSChain ams;
    int nfile=-1;
    if(rmode==1){
      ams.Add(ifile);
    }
    else{
       if(readfile(ifile,ams,nfile)!=0){
         printf("error files read\n");
         return -1;
       }
    }//read file

//---
    AMSAnalysis *anlysis =new AMSAnalysis();
    anlysis->BookFile(ofile);
//---
    Long64_t num2= ((num==-1)||(num>ams.GetEntries()))?ams.GetEntries():num;
    cout<<"num2="<<num2<<endl;
    for(Long64_t entry=0; entry<num2; entry++){
        AMSEventR *ev=(AMSEventR *)ams.GetEvent();
        if(ev==NULL)continue;
        anlysis->InitEvent(ev);
        if(!anlysis->Select())continue;
        anlysis->Fill();
        if(stop==1)break;
    }
    for(int icut=0;icut<10;icut++){cout<<"stat"<<icut<<"="<<sstat[icut]<<" ";}
    anlysis->Save();
    return 0;
}
