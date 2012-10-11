///  This is an example to use Root-Mode TofRecH::ReBuild: Select_Tof()
/*!
  First need TofRecH::ReBuild() \n
  BetaHR and TofClusterHR can be accessed from AMSEventR, ParticleR or ChargeR  \n
  BetaH Version Software Based on IHEP version Calibration and Reconstruction \n
  Both Support gbatch and Root-Mode  \n
*/
// -----------------------------------------------------------
//        Created:       2012-Aug-17  Q.Yan  qyan@cern.ch
//        Add:           2010-Oct-11  Adding BetaHR charge example GetQ GetQL
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
   int          tof_nrawcl[NTOFL];
   int          tof_barid[NTOFL];
   float        tof_tkco[NTOFL][3];
   float        tof_tkdir[NTOFL][3];
   float        tof_adca[NTOFL][NTOFS];
   float        tof_adcd[NTOFL][NTOFS];
//----
   float        tof_q;
   int          tof_charge;
//--calibration 
   unsigned int tof_status[NTOFL];
   unsigned int tof_bstatus[NTOFL][2];
   float        tof_adcar[NTOFL][NTOFS];
   float        tof_adcdr[NTOFL][NTOFS][3];
   float        tof_sdtr[NTOFL][NTOFS]; 
   float        tof_lenr[NTOFL]; 
   float        tof_edep[NTOFL][2];//anode+dynode
   float        tof_edepm[2][NTOFL][2];//anode+dynode
//---
   int          tof_baridm[2][NTOFL];
//--
   float        tof_beta;
   float        tof_betah;
   int          tof_nhith;
   int          tof_hsumh;
   int          tof_hsumhu;
   int          tof_nhithl[4];

//---BetaH Charge
   float        tof_hqt;
   float        tof_hqg;
   float        tof_hql[4];

//--Time
   float        tof_t0;
   float        tof_tmin[4];
   int          tof_bartmin[4];
   float        tof_tmax[4];
   int          tof_bartmax[4];
   float        tof_time[4];
//---
   float        tof_cres[4][2];
   float        tof_tres[4];
   float        tof_temp[4][2];
   float        tof_tempC[4][2];
   float        tof_tempP[4][2];
   float        tof_lcoo[4];
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
  tout->Branch("tof_status",tof_status,"tof_status[4]/i");
  tout->Branch("tof_bstatus",tof_bstatus,"tof_bstatus[4][2]/i");
  tout->Branch("tof_tkco",tof_tkco,"tof_tkco[4][3]/F");  
  tout->Branch("tof_tkdir",tof_tkdir,"tof_tkdir[4][3]/F");
   
//----
  tout->Branch("tof_adcar",tof_adcar,"tof_adcar[4][2]/F");
  tout->Branch("tof_adcdr",tof_adcdr,"tof_adcdr[4][2][3]/F");
  tout->Branch("tof_sdtr",tof_sdtr,"tof_sdtr[4][2]/F");
  tout->Branch("tof_lenr",tof_lenr,"tof_lenr[4]/F");
  tout->Branch("tof_edep",tof_edep,"tof_edep[4][2]/F");
//---
  tout->Branch("tof_edepm",tof_edepm,"tof_edepm[2][4][2]/F");//layer max(not inclue betah counter) Anode+Dynode Edep
  tout->Branch("tof_baridm",tof_baridm,"tof_baridm[2][4]/I");//anode or dynode max bar
//---
  
  tout->Branch("tof_beta",&tof_beta,"tof_beta/F");
  tout->Branch("tof_betah",&tof_betah,"tof_betah/F");
  tout->Branch("tof_nhith",&tof_nhith,"tof_nhith/I");//ev->nTofClusterHs
  tout->Branch("tof_hsumh",&tof_hsumh,"tof_hsumh/I"); //BetaH
  tout->Branch("tof_hsumhu",&tof_hsumhu,"tof_hsumhu/I");//
  tout->Branch("tof_nhithl",tof_nhithl,"tof_nhithl[4]/I");

//---BetaH Charge
  tout->Branch("tof_hqt",&tof_hqt, "tof_hqt/F");//Q- Truncate Mean //Better For Low Charge(Pr He Li)
  tout->Branch("tof_hqg",&tof_hqg, "tof_hqg/F");//Q- Global Gaus Mean  //Better For High Charge
  tout->Branch("tof_hql", tof_hql, "tof_hql[4]/F");//Q for each Layer

//---Time
  tout->Branch("tof_t0",   &tof_t0,    "tof_t0/F");
  tout->Branch("tof_time", tof_time,   "tof_time[4]/F");
  tout->Branch("tof_tmin", tof_tmin,   "tof_tmin[4]/F");
  tout->Branch("tof_bartmin", tof_bartmin,   "tof_bartmin[4]/I");
  tout->Branch("tof_tmax", tof_tmax,   "tof_tmax[4]/F");
  tout->Branch("tof_bartmax", tof_bartmax,   "tof_bartmax[4]/I");

//---
  tout->Branch("tof_cres", tof_cres,"tof_cres[4][2]/F");
  tout->Branch("tof_tres", tof_tres,"tof_tres[4]/F");
  tout->Branch("tof_temp", tof_temp,"tof_temp[4][2]/F");
  tout->Branch("tof_tempC", tof_tempC,"tof_tempC[4][2]/F");
  tout->Branch("tof_tempP", tof_tempP,"tof_tempP[4][2]/F");
  tout->Branch("tof_lcoo",  tof_lcoo, "tof_lcoo[4]/F");
  tout->Branch("tof_chist",&tof_chist,"tof_chist/F");
  tout->Branch("tof_chisc",&tof_chisc,"tof_chisc/F");

  tout->Branch("tof_q",&tof_q,"tof_q/F");
  tout->Branch("tof_charge",&tof_charge,"tof_charge/I");

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

//     BetaR *beta=pev->pParticle(iparindex)->pBeta();
//     if(beta)tof_beta=beta->Beta; 

     AMSPoint tofpnt;AMSDir tofdir;
//---
     TofRecH::ReBuild();
     tof_nhith=pev->nTofClusterH();
     BetaHR *betah=pev->pParticle(iparindex)->pBetaH();
//     if(!betah)return false;
     if(betah){
       tof_betah=betah->GetBeta();
///--BetaH TOF Q
       int nlay; float qrms;
       tof_hqt=betah->GetQ(nlay,qrms);//Trancate Mean
       tof_hqg=betah->GetQ(nlay,qrms,2,TofClusterHR::DefaultQOpt,-1);//Gaus Mean
//---
       tof_chist=betah->GetChi2T();
       tof_chisc=betah->GetChi2C();
       tof_hsumh=betah->GetSumHit();
       tof_hsumhu=betah->GetUseHit();
       tof_t0=betah->GetT0();
       for(int ilay=0;ilay<NTOFL;ilay++){
//--begin test
         if(betah->TestExistHL(ilay)){
///---TOF QLayer
           tof_hql[ilay]=betah->GetQL(ilay);
           tof_lenr[ilay]=betah->GetTkTFLen(ilay);
           tof_cres[ilay][0]=betah->GetCResidual(ilay,0);
           tof_cres[ilay][1]=betah->GetCResidual(ilay,1);
           tof_tres[ilay]=betah->GetTResidual(ilay);
           tof_barid[ilay]=betah->GetClusterHL(ilay)->Bar;
           tof_status[ilay]=betah->GetClusterHL(ilay)->Status;
           tof_nhithl[ilay]=betah->GetAllFireHL(ilay);
           tof_time[ilay]=betah->GetTime(ilay);
           tof_edep[ilay][0]=betah->GetClusterHL(ilay)->AEdep;
           tof_edep[ilay][1]=betah->GetClusterHL(ilay)->DEdep;
           tof_tmin[ilay]=tof_time[ilay];
           tof_tmax[ilay]=tof_time[ilay];
           if(tof_nhithl[ilay]>1){
             float maxedep[2]={0};
             for(int iclh=0;iclh<tof_nhith;iclh++){
               TofClusterHR *tofclh=pev->pTofClusterH(iclh);
               if((tofclh->Layer==ilay)&&(tofclh->Bar!=tof_barid[ilay])){
                 if(tofclh->AEdep>maxedep[0]){
                   tof_baridm[0][ilay]=tofclh->Bar;
                   maxedep[0]=tofclh->AEdep;
                   tof_edepm[0][ilay][0]=maxedep[0];
                   tof_edepm[0][ilay][1]=tofclh->DEdep;
                  }
                 if(tofclh->DEdep>maxedep[1]){
                   tof_baridm[1][ilay]=tofclh->Bar;
                   maxedep[1]=tofclh->AEdep;
                   tof_edepm[1][ilay][0]=maxedep[1];
                   tof_edepm[1][ilay][1]=tofclh->DEdep;
                   }
//---both side require
                  if(tofclh->IsGoodSide(0)&&tofclh->IsGoodSide(1)&&tofclh->IsGoodTime()){
                    if(tofclh->Time<tof_tmin[ilay]){tof_tmin[ilay]=tofclh->Time;tof_bartmin[ilay]=tofclh->Bar;}
                    if(tofclh->Time>tof_tmax[ilay]){tof_tmax[ilay]=tofclh->Time;tof_bartmax[ilay]=tofclh->Bar;}
                   }
//-----
                }
              }
           }
           tof_lcoo[ilay]=betah->GetClusterHL(ilay)->Coo[TRANS[ilay]];
           for(int is=0;is<2;is++){
             tof_bstatus[ilay][is]=betah->GetClusterHL(ilay)->SideBitPat[is];
             tof_sdtr[ilay][is]=betah->GetClusterHL(ilay)->Rtime[is];
             tof_adcar[ilay][is]=betah->GetClusterHL(ilay)->Aadc[is];
             for(int ipm=0;ipm<3;ipm++)tof_adcdr[ilay][is][ipm]=betah->GetClusterHL(ilay)->Dadc[is][ipm];
             if(betah->GetClusterHL(ilay)->TestExistHS(is)){
                tof_temp[ilay][is]=betah->GetClusterHL(ilay)->GetRawSideHS(is)->temp;
                tof_tempC[ilay][is]=betah->GetClusterHL(ilay)->GetRawSideHS(is)->tempC;
                tof_tempP[ilay][is]=betah->GetClusterHL(ilay)->GetRawSideHS(is)->tempP;
              }
           }
          pev->pParticle(iparindex)->pTrTrack()->Interpolate(betah->GetClusterHL(ilay)->Coo[2],tofpnt,tofdir);
          for(int ic=0;ic<3;ic++){tof_tkdir[ilay][ic]=tofdir[ic]; tof_tkco[ilay][ic]=tofpnt[ic];}
        }
//--Test
      }
   }

//---Tof Charge   
     ChargeR *charge=pev->pParticle(iparindex)->pCharge();
     if(!charge)return false;
     ChargeSubDR *zTof=charge->getSubD("AMSChargeTOF");
     if(zTof){
         tof_charge = TMath::Max(Int_t(zTof->ChargeI[0]),1);
         tof_q = zTof->Q;
     }
    else {
      tof_charge=-1;
      tof_q=-1;
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
   tof_beta=-3;
   tof_betah=-3;
   tof_hqt=-1;
   tof_hqg=-1;
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
      tof_nrawcl[itfl]=0;
      tof_barid[itfl]=-1;
      tof_status[itfl]=0;
      tof_lenr[itfl]=0;
      tof_baridm[0][itfl]=-1;
      tof_baridm[1][itfl]=-1;
      tof_bartmin[itfl]=-1;
      tof_bartmax[itfl]=-1;
      for(int itfs=0;itfs<NTOFS;itfs++){
        tof_adcar[itfl][itfs]=-1;
        tof_sdtr[itfl][itfs]=-1; 
        for(int ipm=0;ipm<3;ipm++)tof_adcdr[itfl][itfs][ipm]=-1;
       }
      for(int itfc=0;itfc<3;itfc++){tof_tkco[itfl][itfc]=-1000;tof_tkdir[itfl][itfc]=-1000;}
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
