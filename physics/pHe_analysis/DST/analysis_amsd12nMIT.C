//# $Id$
//////////////////////////////////////////////////////////////////////////
///
///\file analysis_amsd12nMIT.C, Q.Yan's DST version for Proton+Helium+Ion.... Analysis
///\date 2014/10/14 Author Qi Yan //qyan@cern.ch
///
////////////////////////////////////////////////////////////////////////////
#include <signal.h>
#include <TH1.h>
#include <TH2F.h>
#include <TF1.h>
#include <TLeaf.h>
#include <TGraphErrors.h>
#include <TApplication.h>
#include <TCanvas.h>
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
#include <fstream>
#include <iostream>
#include "root_RVSP.h"
#include "amschain.h"
#include "Tofrec02_ihep.h"
#include "TofTrack.h"
#include "TrdKCluster.h"
#include "TrdSCalib.h"
#include "bcorr.h"
#include <time.h>
#include "/afs/cern.ch/user/q/qyan/ihepsub/readfile1.C"


namespace tofconst{
  const int NECALL=18;
  const int NTKL=9;
  const int NTKS=2;
  const int NTKDIS=4;
  const float TKDIS[NTKDIS]={1,2,4,6};//dis=cm
  const int NTOFL=4;
  const int NTOFBM=10;
  const int NTOFS=2;
  const int NBAR[NTOFL]={8,8,10,8};
  const int TRANS[NTOFL]={0,1,1,0};//trans pos
  const float TOFZ[NTOFL]={65.2,62.1,-62.1,-65.2};//central cm  
  const float Sci_pt[NTOFL][NTOFBM]={
      -45.5,  -28.75, -17.25, -5.75, 5.75, 17.25, 28.75, 45.5,  0., 0.,
      -47.,   -28.75, -17.25, -5.75, 5.75, 17.25, 28.75, 47.,   0., 0.,
-55., -40.25, -28.75, -17.25, -5.75, 5.75, 17.25, 28.75, 40.25, 55.,
      -47.25, -28.75, -17.25, -5.75, 5.75, 17.25, 28.75, 47.25, 0., 0.,
   };
   const float qxlow[]={0.9, 1.9, 2.85, 3.8, 4.7, 5.6, 6.65, 7.6};
   const float qxhig[]={1.15,2.2, 3.2,  4.2, 5.2, 6.2, 7.2, 8.2};
   const float tk_pz[NTKL]={159.04,53.05,29.22,25.24,1.706,-2.292,-25.26,-29.25,-136.03};
   const double cirr[]={62,62,46,46,46,46,46,46,43};
   const double ciry[]={47,40,44,44,36,36,44,44,29};
//--Static information
   const int prunbias=20;//1/20 for unbias
   const int prunbiasmc=10;
   const int prunbiasmcpr=1;//Proton Only
//   const int prunbiasmc=20;
   int    prscale=1;
   int    prtime=0;
   int    sstat[100]={0};
   int    sstree[100]={0};
   double cput[100]={0};
}
using namespace tofconst;


class AMSAnalysis{
  public:
   AMSAnalysis():fout(0),pev(0){tout[0]=tout[1]=tout[2]=0;};
   ~AMSAnalysis(){};
   void BookFile(char *ofile);
   void LoadHeader();
//---
   int  GetTreeSize(TTree *tree);
   int  AddBranch_Header (TTree *tree);
   int  AddBranch_Trigger(TTree *tree);
   int  AddBranch_RTI(TTree *tree);
   int  AddBranch_MC (TTree *tree);
   int  AddBranch_Particle(TTree *tree);
   int  AddBranch_Trk (TTree *tree);
   int  AddBranch_Tof (TTree *tree);
   int  AddBranch_Trd (TTree *tree);
   int  AddBranch_Rich(TTree *tree);
   int  AddBranch_Ecal(TTree *tree);
   int  AddBranch_Anti(TTree *tree);
//---
   void Save();
   void InitEvent(AMSEventR *ev);   
   int  Select();//1 Write-Pre 2 Write-All
   bool Select_Header();
   bool Select_Trigger();
   bool Select_MC();
   bool Select_RTI();
   bool Select_Particle();
//----*
   bool Select_L1L9Particle();
   bool Select_MCL1L9Particle();
   bool Select_2ndParticle();
   bool Select_LinkParticle();
//----
   bool Select_Trk(int sel=0);//pre selction 1 or 0
   bool Select_Tof();
   bool Select_Trd();
   bool Select_Rich();
   bool Select_Ecal();  
   bool Select_Anti();
   void Fill(int it);
//--
   TFile *fout;
   TTree *tout[3];

   AMSEventR *pev;
   int iparindex;
//--Sum Control
   int          unbiasetag;
   int          unbiasemctag;
//--Header
   bool         isreal;
   unsigned int run;
   unsigned int event;
   unsigned int time[2];
   float        yaw;
   float        pitch;
   float        roll;
   float        thetam;//magnetic theta
   float        phim;
   bool         isbadrun;

//---Trigger
   int          nlevel1;
   int          ndaqev;
   int          ndaqerr;
   unsigned int daqlen;
   int          daqtyerr;
   float        livetime;
   int          physbpatt;
   int          jmembpatt;
   int          physbpatt1;
   int          jmembpatt1;
   int          ecalflag;
   int          tofflag[2];
   int          antipatt;

//---RTI
   float        zenith;
   float        thetas;
   float        phis;
   float        rads;
   float        glat;//galatic latti
   float        glong;//galatic longti
   int          rtigood;
   float        mcutoff[4][2];
   float        mcutoffi[4][2];

//--MC
   int          mpar;
   float        mmom;
   float        mch;
   int          mtof_pass;
   float        mevcoo[3];
   float        mevdir[3];
   float        mfcoo[3];
   float        mfdir[3];
   float        mpare[2];
   int          mparp[2];
   float        mparc[2];
   float        msume;
   float        msumc;
   float        mtrdmom[20];
   int          mtrdpar[20];
   float        mtrdcoo[20][3];
   float        mtrmom[9];
   int          mtrpar[9];
   float        mtrcoo[9][3];
   int          mtrpri[9];
//---
   float        mevcoo1[21][3];
   float        mevdir1[21][3];
   float        mevmom1[21];

//---L1L9Particle
   int          l1i;
   int          l9i;
   float        tk_l1q;
   float        tk_l9q;
   float        tk_l1qxy[2];
   float        tk_l9qxy[2];
   int          tk_l1qs;
   int          tk_l9qs;
   int          ibetahs;
   int          itrdtracks;
   int          tof_nhits;
   float        betahs;
   float        tof_chiscs;
   float        tof_chists;
   float        tof_qls[4];
   float        ecal_pos[3];
   float        ecal_ens;
   float        ecal_dis;
   float        tk_l1mds;
   float        tk_l9mds;
   float        tk_pos1s[9][3];
   float        tk_l1qvs;
   float        tk_l9qvs;
//---Particle
   int          nparticle;
   int          nbetah;
   int          nbeta;
   int          ncharge;
   int          ntrack;
   int          ntrdtrack;
   int          nshow;
   int          ntofclh;
   int          ntrdseg;
   int          ntrdrawh;
   int          ntrdcl;
   int          nrich;
   int          nrichb;
   int          nrichh;
   int          btstat;
   int          ibeta;
   int          ibetah;
   int          itrtrack;
   int          itrdtrack;
   int          icharge;
   int          ishow;
   int          irich;
   int          irichb;
   float        show_dis;
   float        ec_pos[3];
   float        ec_dir[3];
//---2nd Particle
  int          betah2p;//Pattern
  float        betah2q;//Q
  float        betah2r;//Rigidity
  int          betah2hb[2];//Hit
  float        betah2ch;//Chis
   
//---Track
   int          tk_hitb[2];//XY
   float        tk_q[2];
   float        tk_qrms[2];
   int          tk_qhit[2];
   int          tk_rz[2][2];
   float        tk_rpz[2][2];
   float        tk_ql[9];//Lay1+2+..9
   float        tk_ql2[9][2];
   int          tk_qls[9];
//---External Two Layers
   float        tk_exql[2][3];
   int          tk_exqls[2];
   float        tk_exdis[2];
//---Full+Inner+InnerL0+InnerL8+InnerUP+InnerDo
   float        tk_rigidity1[3][3][6];//Algo+PGCIEMAT+Span
   float        tk_erigidity1[3][3][6];
   float        tk_chis1[3][3][6][3];////Algo+PGCIEMAT+Span+XYXY
   float        tk_res[4][9][2];//Residual Span+Layer+XY
   float        tk_cdif[9][3];//PG-CIEMAT Layer+XYZ
   float        tk_pos[9][3];
   float        tk_oq[2];

//---TOF
   int          tof_btype;
   float        tof_beta;
   float        tof_betah;
   int          tof_hsumh;
   int          tof_hsumhu;
   float        tof_chist;
   float        tof_chisc;
   float        tof_chist_n;
   float        tof_chisc_n;
   float        tof_q[3][2];
   float        tof_qrms[3][2];
   int          tof_z[3];
   float        tof_pz[3];
   int          tof_zr;
   float        tof_pzr;
   int          tof_zud[3][2];
   float        tof_pud[3][2];
   int          tof_pass;
   int          tof_qs;//QStat 
   int          tof_nclhl[4];
   int          tof_barid[4];
   float        tof_ql[3][4];
   float        tof_ql1[3][4];
   float        tof_qlr[3][3][4];//All+Anode+Dynode From BetaH
   float        tof_tl[4];
   float        tof_oq[4][2];//Two Max Non-BetaH ClusterH
   int          tof_ob[4][2];

//--TrdTrack
   int          nitrdseg;
   float        distrd[3];///0x ,1y, 2theta
   float        trd_rq;
   int          trd_rz;
   float        trd_rpz;
   int          trd_pass;
   int          trd_nhitk;
   float        trd_lik[3];//e/p e/He p/He
   int          trd_statk;
   float        trd_qk[5];//A+U+D+Non-Dedx
   int          trd_qnhk[3];
   float        trd_ipch;
   float        trd_qrmsk[3];

//--Rich
   float        rich_beta[3];//getbeta+default+refit
   float        rich_ebeta;
   float        rich_ebeta1; 
   float        rich_pb;//Prob To Ring
   float        rich_udis;
   int          rich_hit;
   int          rich_used;
   int          rich_usedm;
   int          rich_stat;
   float        rich_q[2];//q2 from ring+q from ChargeR
   float        rich_width;
   float        rich_npe[3];//Collect and expect
   bool         rich_good;
   bool         rich_clean;
   bool         rich_NaF;
   float        rich_KolProb;
   int          rich_pmt;
   int          rich_pmt1;
   float        rich_PMTChargeConsistency;
   float        rich_BetaConsistency;
   float        rich_x;
   float        rich_y;
   int          rich_tile;
   float        rich_distb;
   int          rich_cstat;
   float        rich_betap;
   float        rich_likp;
   float        rich_pbp;
   int          rich_usedp;
   float        rich_qp;
   int          rich_statp;
   float        rich_pzp[3];//3most probZ
   int          rich_rz;
   float        rich_rq;
   float        rich_rpz;

//---Ecal
   float        ecal_en[3];//EnergyE EnergyA EnergyD
   float        ecal_bdt[3];//BDT+BDTA+BDTChis
   float        ecal_q;
   int          ecal_nhit[18];//Hit Number for each Layer
   float        ecal_el[18];//Energy for each Layer
   float        ecal_eh[18];//Max Cell Energy for each Layer
//---Anti
   int          anti_nhit;

};



void AMSAnalysis::BookFile(char *ofile){
  fout=new TFile(ofile,"RECREATE");

//--book tree
  char trnam[1000];
  for(int it=0;it<3;it++){
    if     (it==0)sprintf(trnam,"amstreeb");
    else if(it==1)sprintf(trnam,"amstreea");
    else if(it==2)sprintf(trnam,"amstreeu");
    tout[it]=new TTree(trnam,trnam);
    sstree[0]=AddBranch_Header(tout[it]);
    sstree[1]=AddBranch_Trigger(tout[it]);
    sstree[2]=AddBranch_RTI(tout[it]);
    sstree[3]=AddBranch_Particle(tout[it]);
    if(it==0)continue;
    sstree[4]=AddBranch_Trk(tout[it]);
    sstree[5]=AddBranch_Tof(tout[it]);
    sstree[7]=AddBranch_Trd(tout[it]);
    sstree[8]=AddBranch_Rich(tout[it]);
    sstree[9]=AddBranch_Ecal(tout[it]);
    sstree[10]=AddBranch_Anti(tout[it]);
 }

}

int AMSAnalysis::GetTreeSize(TTree *utree){
   int treelen=0;
   TObjArray* leaves=utree->GetListOfLeaves();
   int nleaves=leaves->GetEntriesFast();
   for (int i = 0; i < nleaves; i++) {
     TLeaf* leaf = (TLeaf*) leaves->UncheckedAt(i);
     int len = leaf->GetLen();
     treelen +=len;
   }
   return treelen;
}


int AMSAnalysis::AddBranch_Header(TTree *utree){//12
    int lenb=GetTreeSize(utree);
    utree->Branch("run",      &run,      "run/i");
    utree->Branch("isbadrun", &isbadrun, "isbadrun/O");
    utree->Branch("isreal",   &isreal,   "isreal/O");
    utree->Branch("event",    &event,    "event/i");
    utree->Branch("time",      time,     "time[2]/i");
    utree->Branch("thetas",   &thetas,   "thetas/F");
    utree->Branch("phis",     &phis,     "phis/F");
    utree->Branch("rads",     &rads,     "rads/F");
    utree->Branch("yaw",      &yaw,      "yaw/F");
    utree->Branch("pitch",    &pitch,    "pitch/F");
    utree->Branch("roll",     &roll,     "roll/F");
    int lene=GetTreeSize(utree);
    return lene-lenb;
}

int AMSAnalysis::AddBranch_Trigger(TTree *utree){//14
    int lenb=GetTreeSize(utree);
    utree->Branch("nlevel1",    &nlevel1,    "nlevel1/I");
    utree->Branch("ndaqev",     &ndaqev,     "ndaqev/I");
    utree->Branch("ndaqerr",    &ndaqerr,    "ndaqerr/I");
    utree->Branch("daqlen",     &daqlen,     "daqlen/i");
    utree->Branch("daqtyerr",   &daqtyerr,   "daqtyerr/I");
    utree->Branch("livetime",   &livetime,   "livetime/F");
    utree->Branch("physbpatt",  &physbpatt,  "physbpatt/I");
    utree->Branch("jmembpatt",  &jmembpatt,  "jmembpatt/I");
    utree->Branch("physbpatt1", &physbpatt1, "physbpatt1/I");
    utree->Branch("jmembpatt1", &jmembpatt1, "jmembpatt1/I");
    utree->Branch("ecalflag",   &ecalflag,   "ecalflag/I");
    utree->Branch("tofflag",     tofflag,    "tofflag[2]/I");
    utree->Branch("antipatt",   &antipatt,"   antipatt/I");
    int lene=GetTreeSize(utree);
    return lene-lenb;
}

int AMSAnalysis::AddBranch_RTI(TTree *utree){//22
    int lenb=GetTreeSize(utree);
    utree->Branch("zenith",   &zenith,   "zenith/F");
    utree->Branch("glong",    &glong,    "glong/F");
    utree->Branch("rtigood",  &rtigood,  "rtigood/I");
    utree->Branch("glat",     &glat,     "glat/F");
    utree->Branch("thetam",   &thetam,   "thetam/F");
    utree->Branch("phim",     &phim,     "phim/F");
    utree->Branch("mcutoff",   mcutoff,  "mcutoff[4][2]/F");//Minus+Plus 
    utree->Branch("mcutoffi",  mcutoffi, "mcutoffi[4][2]/F");//Minus+Plus 
    int lene=GetTreeSize(utree); 
    return lene-lenb;
}

int AMSAnalysis::AddBranch_MC(TTree *utree){
    int lenb=GetTreeSize(utree);
    utree->Branch("mpar",     &mpar,     "mpar/I");
    utree->Branch("mmom",     &mmom,     "mmom/F");
    utree->Branch("mch",      &mch,      "mch/F");
    utree->Branch("mtof_pass",&mtof_pass,"mtof_pass/I");
    utree->Branch("mevcoo",    mevcoo,   "mevcoo[3]/F");
    utree->Branch("mevdir",    mevdir,   "mevdir[3]/F");
    utree->Branch("mfcoo",     mfcoo,    "mfcoo[3]/F");
    utree->Branch("mfdir",     mfdir,    "mfdir[3]/F");
    utree->Branch("mpare",     mpare,    "mpare[2]/F");
    utree->Branch("mparp",     mparp,    "mparp[2]/I");
    utree->Branch("mparc",     mparc,    "mparc[2]/F");
    utree->Branch("msume",    &msume,    "msume/F");
    utree->Branch("msumc",    &msumc,    "msumc/F");
    utree->Branch("mtrdmom",   mtrdmom,  "mtrdmom[20]/F");
    utree->Branch("mtrdpar",   mtrdpar,  "mtrdpar[20]/I");
    utree->Branch("mtrdcoo",   mtrdcoo,  "mtrdcoo[20][3]/F");
    utree->Branch("mtrmom",    mtrmom,   "mtrmom[9]/F");
    utree->Branch("mtrpar",    mtrpar,   "mtrpar[9]/I");
    utree->Branch("mtrcoo",    mtrcoo,   "mtrcoo[9][3]/F");
    utree->Branch("mtrpri",    mtrpri,   "mtrpri[9]/I");
//---
    utree->Branch("mevcoo1",    mevcoo1,   "mevcoo1[21][3]/F");
    utree->Branch("mevdir1",    mevdir1,   "mevdir1[21][3]/F");
    utree->Branch("mevmom1",    mevmom1,   "mevmom1[21]/F");
    

//---
    int lene=GetTreeSize(utree);
    return lene-lenb;
}

int AMSAnalysis::AddBranch_Particle(TTree *utree){
    int lenb=GetTreeSize(utree);
    utree->Branch("tk_l1q",     &tk_l1q,     "tk_l1q/F");
    utree->Branch("tk_l9q",     &tk_l9q,     "tk_l9q/F");
    utree->Branch("tk_l1qxy",   tk_l1qxy,    "tk_l1qxy[2]/F");
    utree->Branch("tk_l9qxy",   tk_l9qxy,    "tk_l9qxy[2]/F");
    utree->Branch("tk_l1qs",    &tk_l1qs,    "tk_l1qs/I");
    utree->Branch("tk_l9qs",    &tk_l9qs,    "tk_l9qs/I");
    utree->Branch("ibetahs",    &ibetahs,    "ibetahs/I");
    utree->Branch("itrdtracks", &itrdtracks, "itrdtracks/I");
    utree->Branch("tof_nhits",  &tof_nhits,  "tof_nhits/I");
    utree->Branch("betahs",     &betahs,     "betahs/F");
    utree->Branch("tof_chiscs", &tof_chiscs, "tof_chiscs/F");
    utree->Branch("tof_chists", &tof_chists, "tof_chists/F");
    utree->Branch("tof_qls",     tof_qls,    "tof_qls[4]/F");
    utree->Branch("ecal_pos",    ecal_pos,   "ecal_pos[3]/F");
    utree->Branch("ecal_ens",   &ecal_ens,   "ecal_ens/F");
    utree->Branch("ecal_dis",   &ecal_dis,   "ecal_dis/F");
    utree->Branch("tk_l1mds",   &tk_l1mds,   "tk_l1mds/F");
    utree->Branch("tk_l9mds",   &tk_l9mds,   "tk_l9mds/F");
    utree->Branch("tk_pos1s",    tk_pos1s,   "tk_pos1s[9][3]/F");
    utree->Branch("tk_l1qvs",   &tk_l1qvs,   "tk_l1qvs/F");
    utree->Branch("tk_l9qvs",   &tk_l9qvs,   "tk_l9qvs/F");
   //--Particle //22
    utree->Branch("nparticle",  &nparticle,  "nparticle/I");
    utree->Branch("nbetah",     &nbetah,     "nbetah/I");
    utree->Branch("nbeta",      &nbeta,      "nbeta/I");
    utree->Branch("ntrack",     &ntrack,     "ntrack/I");
    utree->Branch("ntrdtrack",  &ntrdtrack,  "ntrdtrack/I");
    utree->Branch("nrich",      &nrich,      "nrich/I");
    utree->Branch("nrichb",     &nrichb,     "nrichb/I");
    utree->Branch("nrichh",     &nrichh,     "nrichh/I");
    utree->Branch("nshow",      &nshow,      "nshow/I");
    utree->Branch("ntofclh",    &ntofclh,    "ntofclh/I");
    utree->Branch("ntrdseg",    &ntrdseg,    "ntrdseg/I");
    utree->Branch("ntrdrawh",   &ntrdrawh,   "ntrdrawh/I");
    utree->Branch("ntrdcl",     &ntrdcl,     "ntrdcl/I");
    utree->Branch("btstat",     &btstat,     "btstat/I"); //Status 1:Over cutoff, 2:Under cutoff, 3:Trapped
    utree->Branch("ibeta",      &ibeta,      "ibeta/I");
    utree->Branch("ibetah",     &ibetah,     "ibetah/I");
    utree->Branch("icharge",    &icharge,    "icharge/I");
    utree->Branch("itrtrack",   &itrtrack,   "itrtrack/I");
    utree->Branch("itrdtrack",  &itrdtrack,  "itrdtrack/I");
    utree->Branch("ishow",      &ishow,      "ishow/I");
    utree->Branch("irich",      &irich,      "irich/I");
    utree->Branch("irichb",     &irichb,     "irichb/I");
    utree->Branch("iparindex",  &iparindex,  "iparindex/I");
    utree->Branch("show_dis",   &show_dis,   "show_dis/F");
    utree->Branch("ec_pos",      ec_pos,     "ec_pos[3]/F");
    utree->Branch("ec_dir",      ec_dir,     "ec_dir[3]/F");
    //--2ndParticle //6
    utree->Branch("betah2p",    &betah2p,    "betah2p/I");
    utree->Branch("betah2q",    &betah2q,    "betah2q/F");
    utree->Branch("betah2r",    &betah2r,    "betah2r/F");
    utree->Branch("betah2hb",    betah2hb,   "betah2hb[2]/I");
    utree->Branch("betah2ch",   &betah2ch,   "betah2ch/F");
    int lene=GetTreeSize(utree);
    return lene-lenb;
}

int AMSAnalysis::AddBranch_Trk(TTree *utree){//52+380-54
    int lenb=GetTreeSize(utree);
    utree->Branch("tk_hitb",       tk_hitb,        "tk_hitb[2]/I");
    utree->Branch("tk_rz",         tk_rz,          "tk_rz[2][2]/I");//All+Inner
    utree->Branch("tk_rpz",        tk_rpz,         "tk_rpz[2][2]/F");//
    utree->Branch("tk_q",          tk_q,           "tk_q[2]/F");//All+Inner
    utree->Branch("tk_qrms",       tk_qrms,        "tk_qrms[2]/F");
    utree->Branch("tk_qhit",       tk_qhit,        "tk_qhit[2]/I");
    utree->Branch("tk_ql",         tk_ql,          "tk_ql[9]/F");//Temp Only Layer1+2
    utree->Branch("tk_ql2",        tk_ql2,         "tk_ql2[9][2]/F");
    utree->Branch("tk_qls",        tk_qls,         "tk_qls[9]/I");//TK-QL Status
///--
    utree->Branch("tk_exql",       tk_exql,        "tk_exql[2][3]/F");
    utree->Branch("tk_exqls",      tk_exqls,       "tk_exqls[2]/I");//TK-QL Status
    utree->Branch("tk_exdis",      tk_exdis,       "tk_exdis[2]/F");
///-All Span Rigidity
    utree->Branch("tk_res",        tk_res,         "tk_res[4][9][2]/F");
    utree->Branch("tk_rigidity1",  tk_rigidity1,   "tk_rigidity1[3][3][6]/F");
//    utree->Branch("tk_erigidity1", tk_erigidity1,  "tk_erigidity1[3][3][6]/F");
    utree->Branch("tk_chis1",      tk_chis1,       "tk_chis1[3][3][6][3]/F");
    utree->Branch("tk_cdif",       tk_cdif,        "tk_cdif[9][3]/F");//Layer
    utree->Branch("tk_pos",        tk_pos,         "tk_pos[9][3]/F");
    utree->Branch("tk_oq",         tk_oq,          "tk_oq[2]/F");
    int lene=GetTreeSize(utree);
    return lene-lenb;
}

int AMSAnalysis::AddBranch_Tof(TTree *utree){//45
    int lenb=GetTreeSize(utree);
    utree->Branch("tof_btype",   &tof_btype,   "tof_btype/I");
    utree->Branch("tof_beta",    &tof_beta,    "tof_beta/F");
    utree->Branch("tof_betah",   &tof_betah,   "tof_betah/F");
    utree->Branch("tof_hsumh",   &tof_hsumh,   "tof_hsumh/I"); //BetaH
    utree->Branch("tof_hsumhu",  &tof_hsumhu,  "tof_hsumhu/I");//
    utree->Branch("tof_nclhl",    tof_nclhl,   "tof_nclhl[4]/I");
    utree->Branch("tof_barid",    tof_barid,   "tof_barid[4]/I");
    utree->Branch("tof_pass",    &tof_pass,    "tof_pass/I");
    utree->Branch("tof_oq",       tof_oq,      "tof_oq[4][2]/F");
    utree->Branch("tof_ob",       tof_ob,      "tof_ob[4][2]/I");
    utree->Branch("tof_chist",   &tof_chist,   "tof_chist/F");
    utree->Branch("tof_chisc",   &tof_chisc,   "tof_chisc/F");
    utree->Branch("tof_chist_n", &tof_chist_n, "tof_chist_n/F");
    utree->Branch("tof_chisc_n", &tof_chisc_n, "tof_chisc_n/F");
    utree->Branch("tof_z",        tof_z,       "tof_z[3]/I");
    utree->Branch("tof_pz",       tof_pz,      "tof_pz[3]/F");
    utree->Branch("tof_zr",      &tof_zr,      "tof_zr/I");
    utree->Branch("tof_pzr",     &tof_pzr,     "tof_pzr/F");
    utree->Branch("tof_zud",      tof_zud,     "tof_zud[3][2]/I");
    utree->Branch("tof_pud",      tof_pud,     "tof_pud[3][2]/F");
    utree->Branch("tof_q",        tof_q,       "tof_q[3][2]/F");//Max+Trun TofChargeH+BetaH
    utree->Branch("tof_qrms",     tof_qrms,    "tof_qrms[3][2]/F");
    utree->Branch("tof_ql",       tof_ql,      "tof_ql[3][4]/F");
    utree->Branch("tof_ql1",      tof_ql1,     "tof_ql1[3][4]/F");
    utree->Branch("tof_qlr",      tof_qlr,     "tof_qlr[3][3][4]/F");
    utree->Branch("tof_tl",       tof_tl,      "tof_tl[4]/F");
    utree->Branch("tof_qs"  ,    &tof_qs,      "tof_qs/I");//Q-Status
    int lene=GetTreeSize(utree);
    return lene-lenb;
}

int AMSAnalysis::AddBranch_Trd(TTree *utree){//25
    int lenb=GetTreeSize(utree);
    utree->Branch("nitrdseg",    &nitrdseg,   "nitrdseg/I");
    utree->Branch("distrd",       distrd,     "distrd[3]/F");
    utree->Branch("trd_pass",    &trd_pass,   "trd_pass/I");
    utree->Branch("trd_nhitk",   &trd_nhitk,  "trd_nhitk/I");
    utree->Branch("trd_lik",      trd_lik,    "trd_lik[3]/F");
    utree->Branch("trd_statk",   &trd_statk,  "trd_statk/I");
    utree->Branch("trd_qk",       trd_qk,     "trd_qk[5]/F");//All+Up+Down
    utree->Branch("trd_qnhk",     trd_qnhk,   "trd_qnhk[3]/I");
    utree->Branch("trd_ipch",    &trd_ipch,   "trd_ipch/F");
    utree->Branch("trd_qrmsk",    trd_qrmsk,  "trd_qrmsk[3]/F");
    utree->Branch("trd_rq",      &trd_rq,     "trd_rq/F");
    utree->Branch("trd_rz",      &trd_rz,     "trd_rz/I");
    utree->Branch("trd_rpz",     &trd_rpz,    "trd_rpz/F");
    int lene=GetTreeSize(utree);
    return lene-lenb;
}

int AMSAnalysis::AddBranch_Rich(TTree *utree){//25
    int lenb=GetTreeSize(utree);
    utree->Branch("rich_beta",     rich_beta,  "rich_beta[3]/F");
    utree->Branch("rich_ebeta",   &rich_ebeta, "rich_ebeta/F");
    utree->Branch("rich_ebeta1",   &rich_ebeta1, "rich_ebeta1/F");
    utree->Branch("rich_pb",      &rich_pb,    "rich_pb/F");
    utree->Branch("rich_udis",    &rich_udis,  "rich_udis/F");
    utree->Branch("rich_hit",     &rich_hit,  "rich_hit/I");
    utree->Branch("rich_used",    &rich_used,  "rich_used/I");
    utree->Branch("rich_usedm",   &rich_usedm, "rich_usedm/I");
    utree->Branch("rich_stat",    &rich_stat,  "rich_stat/I");
    utree->Branch("rich_q",        rich_q,     "rich_q[2]/F");
    utree->Branch("rich_width",   &rich_width, "rich_width/F");
    utree->Branch("rich_npe",      rich_npe,   "rich_npe[3]/F");
    utree->Branch("rich_good",    &rich_good,  "rich_good/O");
    utree->Branch("rich_clean",   &rich_clean, "rich_clean/O");
    utree->Branch("rich_NaF",     &rich_NaF,   "rich_NaF/O");
    utree->Branch("rich_KolProb", &rich_KolProb,"rich_KolProb/F");
    utree->Branch("rich_pmt",     &rich_pmt,    "rich_pmt/I");
    utree->Branch("rich_pmt1",    &rich_pmt1,   "rich_pmt1/I");
    utree->Branch("rich_PMTChargeConsistency",&rich_PMTChargeConsistency,"rich_PMTChargeConsistency/F");
    utree->Branch("rich_BetaConsistency",&rich_BetaConsistency,"rich_BetaConsistency/F");
    utree->Branch("rich_x",       &rich_x,      "rich_x/F");
    utree->Branch("rich_y",       &rich_y,      "rich_y/F");
    utree->Branch("rich_tile",    &rich_tile,   "rich_tile/I");
    utree->Branch("rich_distb",   &rich_distb,  "rich_distb/F");
    utree->Branch("rich_cstat",   &rich_cstat,  "rich_cstat/I");
//--Lip
    utree->Branch("rich_betap",   &rich_betap, "rich_betap/F");
    utree->Branch("rich_likp",    &rich_likp,  "rich_likp/F");
    utree->Branch("rich_pbp",     &rich_pbp,   "rich_pbp/F");
    utree->Branch("rich_usedp",   &rich_usedp, "rich_usedp/I");
    utree->Branch("rich_statp",   &rich_statp, "rich_statp/I");
    utree->Branch("rich_qp",      &rich_qp,    "rich_qp/F");
    utree->Branch("rich_pzp",      rich_pzp,   "rich_pzp[3]/F");
//--RICHZ
    utree->Branch("rich_rz",      &rich_rz,    "rich_rz/I");
    utree->Branch("rich_rq",      &rich_rq,    "rich_rq/F");
    utree->Branch("rich_rpz",     &rich_rpz,   "rich_rpz/F");
    int lene=GetTreeSize(utree);
    return lene-lenb;
}

int AMSAnalysis::AddBranch_Ecal(TTree *utree){
   int lenb=GetTreeSize(utree);
   utree->Branch("ecal_en",      ecal_en,      "ecal_en[3]/F"); //EnergyE EnergyA EnergyD
//   utree->Branch("ecal_bdt",     ecal_bdt,     "ecal_bdt[3]/F");
   utree->Branch("ecal_q",      &ecal_q,       "ecal_q/F");
   utree->Branch("ecal_nhit",    ecal_nhit,    "ecal_nhit[18]/I");
   utree->Branch("ecal_el",      ecal_el,      "ecal_el[18]/F");
   utree->Branch("ecal_eh",      ecal_eh,      "ecal_eh[18]/F");
   int lene=GetTreeSize(utree);
   return lene-lenb;
}

int AMSAnalysis::AddBranch_Anti(TTree *utree){
   int lenb=GetTreeSize(utree);
   utree->Branch("anti_nhit",   &anti_nhit,    "anti_nhit/I");
   int lene=GetTreeSize(utree);
   return lene-lenb;
}

void AMSAnalysis::LoadHeader(){

     TkDBc::UseFinal();
//--TkVersion
     TRMCFFKEY_DEF::ReadFromFile = 0;
     TRFITFFKEY_DEF::ReadFromFile = 0;
     TRFITFFKEY.magtemp = 0;
//----Add
     TRMCFFKEY.MCtuneDmax = 100e-4;
     TRMCFFKEY.MCtuneDs   = -3.0e-4;
     TRMCFFKEY.MCtuneDy9  = 0;
#ifdef _USEMCTKPr_
     TRMCFFKEY.MCtuneDs   = 2.e-4;
     TRMCFFKEY.MCtuneDy9  = 1.0e-4;//<B900
     TRMCFFKEY.MCscat[0] = -15;    // This is NEW
     TRMCFFKEY.MCscat[1] = -440.06;   // This is NEW
     TRMCFFKEY.MCscat[2] = -440.05;   // This is NEW
#endif
//----
     TRMCFFKEY.OuterSmearing[0][1] = -7.5e-4;//L1Y
     TRMCFFKEY.OuterSmearing[1][1] = -8.3e-4;//L9Y
     TRMCFFKEY.OuterSmearing[0][0] = 7.5e-4;//L1X
     TRMCFFKEY.OuterSmearing[1][0] = 8.5e-4;//L9X
//----
     cout<<"NewN TkVersion Used"<<endl;

}

int AMSAnalysis::Select(){

//---IsReal
   isreal=(pev->nMCEventg()==0);
   static bool hasev=0;
   if(hasev==0){
      for(int it=0;it<3;it++)if(isreal==0){sstree[2]+=AddBranch_MC(tout[it]);}
      hasev=1;
   }

//----
   clock_t tb,te;
   sstat[0]++;

//--Header 0
   tb=clock();
   bool selhead=Select_Header();
   te=clock();
   cput[0]+=te-tb;
   if(!selhead)return 0;
   sstat[1]++;

//---Trigger 1
   tb=te;
   bool seltrig=Select_Trigger();
   te=clock();
   cput[1]+=(te-tb);
   if(!seltrig)return 0;
   sstat[2]++;

//----RTI or MC
   tb=te;
   bool selrti=isreal?Select_RTI():Select_MC();
   te=clock();
   cput[2]+=(te-tb);
   if(!selrti)return 0;
   sstat[3]++;

//---Particle 2
   tb=te;
   bool selpart=Select_Particle();
   te=clock();
   cput[3]+=(te-tb); 
   if(!selpart)return 0;
   sstat[4]++;
  
//--Preselection Track 3
   tb=te;
   bool seltrk1=Select_Trk(0);
   te=clock();
   cput[4]+=(te-tb);
//   if(!seltrk1)return 1;
   sstat[5]++;

//--Strict Selection Tof 4
   tb=te;
   bool seltof=Select_Tof();
   te=clock();
   cput[5]+=(te-tb);
//   if(!seltof)return 1;
   sstat[6]++;

//---Track 5
   tb=te;
   bool seltrk0=Select_Trk(1);
   te=clock();
   cput[6]+=(te-tb);
//   if(!seltrk0)return 1;
   sstat[7]++;

//---Trd 6
   tb=te;
   bool seltrd=Select_Trd();
   te=clock();
   cput[7]+=(te-tb);
//   if(!seltrd)return 1;
   sstat[8]++;

//---Rich 7
   tb=te;
   bool selrich=Select_Rich();
   te=clock();
   cput[8]+=(te-tb);
//   if(!selrich)return 1;
   sstat[9]++;   

//---Ecal 8
   tb=te;
   bool selecal=Select_Ecal();
   te=clock();
   cput[9]+=(te-tb);
//   if(!selecal)return 1;
   sstat[10]++;

//--Anti 9
   tb=te;
   bool selanti=Select_Anti();
   te=clock();
   cput[10]+=(te-tb);
//   if(!selanti)return 1;
   sstat[11]++;

   return true;
}

//---Header
bool  AMSAnalysis::Select_Header(){

   run=pev->Run();
   event=pev->Event();
   time[0]=pev->fHeader.Time[0];
   time[1]=pev->fHeader.Time[1];
//--ISS
   yaw=   pev->fHeader.Yaw;
   pitch= pev->fHeader.Pitch;
   roll=  pev->fHeader.Roll;
//--Magnetic
   thetam=pev->fHeader.ThetaM;
   phim=pev->fHeader.PhiM;

//---only real data
   if(isreal==0)return true;

   isbadrun=pev->isBadRun(run);
   return true;
}

//---Trigger
bool AMSAnalysis::Select_Trigger(){

    nlevel1=pev->nLevel1();
    livetime=pev->LiveTime();
    if(nlevel1>0){
      Level1R *lv=pev->pLevel1(0);
//      livetime=  lv->LiveTime;
      physbpatt= lv->PhysBPatt;
      jmembpatt= lv->JMembPatt;
//Add
      jmembpatt1=physbpatt1=0;
      lv->RebuildTrigPatt(jmembpatt1,physbpatt1);
//--
      tofflag[0]=lv->TofFlag1;
      tofflag[1]=lv->TofFlag2;
      ecalflag=lv->EcalFlag;
      antipatt=  lv->AntiPatt;
    }
//--DAQ
    ndaqev=pev->nDaqEvent();
    ndaqerr=0;
    daqlen=0;
    daqtyerr=0;
    if(pev->fStatus&(1<<30))daqtyerr=1;
    if(ndaqev==1){
       DaqEventR *pdaq=pev->pDaqEvent(0);
       if(pdaq->HasHWError())ndaqerr++;
       daqlen=pdaq->Length;
       if(pdaq->L3RunError())daqtyerr|=(1<<1);
       if(pdaq->L3EventError())daqtyerr|=(1<<2);
       if(pdaq->L3ProcError())daqtyerr|=(1<<3);
       for(int ino=0;ino<=6;ino++){
         if(pdaq->L3NodeError(ino))daqtyerr|=(1<<(4+ino));
       }
    }
   
    return true;
}
//---MC
bool AMSAnalysis::Select_MC(){

//---
 if(isreal)return true;

 int mnpart=pev->nMCEventg();

//---MCEveng0
  MCEventgR *mcev=pev->pMCEventg(0);
  mpar=mcev->Particle;
  mmom=mcev->Momentum;
  mch= mcev->Charge;

//----Coo+Dir
  for(int ico=0;ico<3;ico++){
     mevcoo[ico]=mcev->Coo[ico]; mevdir[ico]=mcev->Dir[ico];
  }
  AMSPoint pnt(mevcoo[0],mevcoo[1],mevcoo[2]);
  AMSDir dir(mevdir[0],mevdir[1],mevdir[2]);

//----TOF Pass
  AMSPoint tofpnt;
  float disedge;double time;
  mtof_pass=0;
  int ntofp=0;
  for(int ilay=0;ilay<4;ilay++){
     int tk_pass=ParticleR::IsPassTOF(ilay,pnt,dir,tofpnt,disedge);
     if(tk_pass>=0){mtof_pass+=int(pow(10.,3-ilay));ntofp++;}
  }

//---MCEvengN Interaction Point Find Max Kine
  for(int ic=0;ic<3;ic++){mfcoo[ic]=-1000;mfdir[ic]=-1000;}
  for(int is=0;is<2;is++){
    mpare[is]=mparp[is]=mparc[is]=0;
 }
  for(int ipar=1;ipar<mnpart;ipar++){
    MCEventgR *mcev=pev->pMCEventg(ipar);
    int parid=mcev->Particle;
//    if(parid<0)cout<<"parid="<<parid<<endl;
    if(fabs(parid)==1||fabs(parid)==2||fabs(parid)==3)continue;//gamma+positron+electron
    if(mcev->Coo[2]<-138||mcev->Nskip==-2||fabs(mcev->Particle)==fabs(mpar))continue;
    if(mcev->parentID!=1)continue;
    if(mcev->Momentum>mpare[0]){
       mpare[0]=mcev->Momentum;
       for(int ic=0;ic<3;ic++){
         mfcoo[ic]=mcev->Coo[ic];
         mfdir[ic]=mcev->Dir[ic];
       }
    }
  }
//Again
   msume=msumc=0;
   for(int iz=0;iz<21;iz++){
      mevmom1[iz]=-1000;
      for(int ic=0;ic<3;ic++){mevcoo1[iz][ic]=-1000;mevdir1[iz][ic]=-1000;}
   }
   for(int ipar=1;ipar<mnpart;ipar++){
      MCEventgR *mcev=pev->pMCEventg(ipar);
      if(mcev->Coo[2]==mfcoo[2]){
         msume+=mcev->Momentum;
         msumc+=mcev->Charge;
         if(mcev->Momentum==mpare[0]){mpare[0]=mcev->Momentum;mparp[0]=mcev->Particle;mparc[0]=mcev->Charge;}//MaxE
         if(fabs(mcev->Charge)>mparc[1]||(fabs(mcev->Charge)==mparc[1]&&mcev->Momentum==mpare[0])){
            mpare[1]=mcev->Momentum;mparp[1]=mcev->Particle;mparc[1]=mcev->Charge;
          }//MaxCh
      }
//--AC
      int iskip=mcev->Nskip;
      if(iskip>=-1020&&iskip<=-1000){
         iskip=fabs(iskip)-1000;
         mevmom1[iskip]=mcev->Momentum;
         for(int ic=0;ic<3;ic++){
           mevcoo1[iskip][ic]=mcev->Coo[ic];
           mevdir1[iskip][ic]=mcev->Dir[ic];
        }
      }
//----
   }


//---TrdMCCluster MaxE
  for(int ilay=0;ilay<20;ilay++){mtrdmom[ilay]=0;mtrdpar[ilay]=0;for(int ic=0;ic<3;ic++)mtrdcoo[ilay][ic]=0;}
  for(int itrd=0;itrd<pev->nTrdMCCluster();itrd++){
     TrdMCClusterR *trdmc=pev->pTrdMCCluster(itrd);
     int ilay=trdmc->Layer;
     if(trdmc->Ekin>mtrdmom[ilay]){
        mtrdmom[ilay]=trdmc->Ekin;
        mtrdpar[ilay]=trdmc->ParticleNo;
        for(int ic=0;ic<3;ic++)mtrdcoo[ilay][ic]=trdmc->Xgl[ic];
     }
   }
//--TrackMCCluster MaxE
  for(int ilay=0;ilay<9;ilay++){mtrmom[ilay]=mtrpri[ilay]=mtrpar[ilay]=0;for(int ic=0;ic<3;ic++)mtrcoo[ilay][ic]=0;}
  for(int itr=0;itr<pev->nTrMCCluster();itr++){
     TrMCClusterR *trmc=pev->pTrMCCluster(itr);
     int il=fabs(trmc->GetTkId()/100);
     if     (il==8)il=1;
     else if(il<8)il=il+1;
     il--;
     if(trmc->GetMomentum()>mtrmom[il]){
       mtrmom[il]=trmc->GetMomentum();
       mtrpar[il]=trmc->GetPart();
       for(int ic=0;ic<3;ic++)mtrcoo[il][ic]=trmc->GetStartPoint()[ic];
       mtrpri[il]=trmc->IsPrimary()?1:0;
     }
  }
//---

  return true;

}


//---RTI
bool AMSAnalysis::Select_RTI(){

    if(isreal==0)return true;
    AMSSetupR::RTI::Version=3;
    AMSSetupR::RTI a;
    if(pev->GetRTI(a)!=0)return false;

//--Time Cut Provide By VC
     bool cut[10]={0};
     cut[0]=(a.ntrig/a.nev>0.98);
     cut[1]=(a.npart/a.ntrig>0.07/1600*a.ntrig&&a.npart/a.ntrig<0.25);
     cut[2]=(a.lf>0.5);
     cut[3]=(a.zenith<40);
     cut[3]=(a.nerr>=0&&a.nerr/a.nev<0.1);
     cut[4]=(a.npart>0&&a.nev<1800);
//----
     zenith=a.zenith;
     thetas=a.theta;
     phis=a.phi;
     rads=a.r;
     glat=a.glat;
     glong=a.glong;
//---cutoff
     for(int ifv=0;ifv<4;ifv++){
       for(int ipn=0;ipn<2;ipn++){mcutoff[ifv][ipn]=a.cf[ifv][ipn];mcutoffi[ifv][ipn]=a.cfi[ifv][ipn];}
     }
     bool tcut=(cut[0]&&cut[1]&&cut[2]&&cut[3]&&cut[4]);
//---
     rtigood=a.good;
     return tcut;
}

//---Particle
bool AMSAnalysis::Select_Particle(){

//---Load Status
   nparticle=pev->nParticle();
   nbeta=pev->nBeta();
   ncharge=pev->nCharge();
   ntrack=pev->nTrTrack();
   nshow=pev->nEcalShower();
   nrich=pev->nRichRing();
   nrichb=pev->nRichRingB();
   nrichh=pev->nRichHit();
//---
   ntrdtrack=pev->nTrdTrack();
   ntrdseg=pev->nTrdSegment();
   ntrdrawh=pev->nTrdRawHit();
   ntrdcl=pev->nTrdCluster();
   ntofclh=-1;
   anti_nhit=pev->nAntiCluster();

//---PreScale 
#if  defined (_PRPRESCALE_) || defined (_HEPRESCALE_) || defined (_IONPRESCALE_) ||  defined (_GOODTKTOFPAR_)
  if(pev->nTrTrack()<1)return false;//require Tk
#endif
//----TkIn Particle
  float tkiqm[2]={0};//L1orL9 +L1L9 
  bool l1hy=0;
  bool l9hy=0;
  for(int itr=0;itr<pev->nTrTrack();itr++){
    l1hy=((pev->pTrTrack(itr)->GetBitPatternJ()&(1<<0))>0);//L1Hit
    l9hy=((pev->pTrTrack(itr)->GetBitPatternJ()&(1<<8))>0);//L9Hit
    if(!l1hy&&!l9hy)continue;//Both
    float ntkq=pev->pTrTrack(itr)->GetInnerQ();
    if(ntkq>tkiqm[0]){//L1 or L9
         tkiqm[0]=ntkq;
     }
    if(l1hy&&l9hy&&ntkq>tkiqm[1]){//L1&&L9
         tkiqm[1]=ntkq;
    }
  }

  bool isintkbgpar=(tkiqm[0]>2.5);//No Helium+L1Y

#ifdef _HEPRESCALE_
   isintkbgpar=(tkiqm[0]>1.5);//He+Ion
#endif
#ifdef _HEPRESCALE2_
   isintkbgpar=(tkiqm[1]>1.5);//He+Full Span
#endif
#ifdef _PRPRESCALE_
   isintkbgpar=(tkiqm[1]>0.6); //P+He+Ion
#endif
#ifdef _PRHEPRESCALE3_
   isintkbgpar=(tkiqm[1]>0.6||tkiqm[0]>1.5);//L1InnerL9 Z>=1; L1Inner Z>=2
//   isintkbgpar=(tkiqm[1]>0.6);
#endif

//-----
#if defined (_PRPRESCALE_) || defined (_HEPRESCALE_) || defined (_IONPRESCALE_)
  if(!isintkbgpar&&!unbiasemctag)return false;
#else
  bool isl1l9bgpar=Select_L1L9Particle();
  bool isl1l9mcpar=Select_MCL1L9Particle(); 
  if(!isintkbgpar&&!isl1l9bgpar&&!isl1l9mcpar&&!unbiasemctag)return false;
#endif

//--TOFClusterH   
  if(ntofclh<0){
     TofRecH::BuildTofClusterH();
     ntofclh=pev->nTofClusterH();
   }
 
//--Paritcle
   TofRecH::BuildOpt=0;
   TofRecH::BuildBetaH();
   nbetah=pev->nBetaH();
  
//---Efficiency Mis-Match Continue to Search
  float tksq=0,tofsq=0;
  for(int ibh=0;ibh<nbetah;ibh++){
//---TOF sq
      BetaHR *betah=pev->pBetaH(ibh);
      float ntofsq=0;float ntksq=0;
      for(int ilay=0;ilay<4;ilay++){ 
         float tofql=betah->GetQL(ilay);
         if(tofql>0)ntofsq+=tofql;
      }
      if(betah->iTrTrack()>=0){ntksq=betah->pTrTrack()->GetQ();}
//---First BetaH
     if(ibetah<0){ibetah=ibh;tofsq=ntofsq;}
//---Fist Track
     if(itrtrack<0&&betah->iTrTrack()>=0){//First Track Order
        ibetah=ibh; itrtrack=betah->iTrTrack();
        tofsq=ntofsq;tksq=ntksq;
     }
//--If Second Tracker Max TkQ
     else if(itrtrack>=0&&betah->iTrTrack()>=0&&ntksq>tksq){
        ibetah=ibh; itrtrack=betah->iTrTrack();
        tofsq=ntofsq;tksq=ntksq;
     }
//---If NoTrack Max TofQ
     else if(itrtrack<0&&betah->iTrTrack()<0&&ntofsq>tofsq){//Then Eneryg> Raw
        ibetah=ibh; itrtrack=betah->iTrTrack();
        tofsq=ntofsq;tksq=ntksq;
     }
//---end try
  }

#ifdef _GOODTKTOFPAR_
  if(ibetah<0||itrtrack<0)return false;
#endif


//---2nd Particle
    Select_2ndParticle();

//--Link Particle
    Select_LinkParticle();


  return true;

}

bool  AMSAnalysis::Select_MCL1L9Particle(){

  if(isreal)return false; 
//--Hit Number
   bool mmlh[9]={0};
   for(int il=0;il<NTKL;il++){
      float coox=(tk_pz[il]-mevcoo[2])*mevdir[0]/mevdir[2]+mevcoo[0];
      float cooy=(tk_pz[il]-mevcoo[2])*mevdir[1]/mevdir[2]+mevcoo[1];
      float disr=coox*coox+cooy*cooy;
      if(il==9-1)mmlh[il]=(fabs(coox)<cirr[il]&&fabs(cooy)<ciry[il]);
      else       mmlh[il]=(sqrt(disr)<cirr[il]&&fabs(cooy)<ciry[il]);
   }
//---
   int mmnhitf=0;
   for(int il=0+1;il<9-1;il++){
      if(mmlh[il]){mmnhitf++;}
   }

   bool mcl1l9tkpar=(mmlh[0]&&mmlh[8]);//L1L9
#if defined (_HEPRESCALE2_) || defined (_PRHEPRESCALE3_)
   if(!mcl1l9tkpar&&(mch>2.5)){//MC Z>=2
       mcl1l9tkpar=(mmnhitf>=5&&mmlh[0]); //L1Inner
   }
#endif
  return mcl1l9tkpar;
}


bool AMSAnalysis::Select_L1L9Particle(){
 
 //--
   tk_l9qs=tk_l1qs=0;
   tk_l9q=tk_l1q=0;
   tk_l9qxy[0]=tk_l9qxy[1]=tk_l1qxy[0]=tk_l1qxy[1]=0;
   AMSPoint postr;AMSDir dirtr;

//---Pre Scale
   if(ntrdtrack<1&&ntrack<1){return false;}

//----InnerTk Interpolate L1 L9 Geometry
  float tkiqm[2]={0};
  for(int itr=0;itr<pev->nTrTrack();itr++){
    bool tkl1g=0,tkl9g=0;
    for(int ilay=0;ilay<NTKL;ilay++){//L1Pos+L9Pos Geometry
        if(ilay!=0&&ilay!=NTKL-1)continue;
        pev->pTrTrack(itr)->InterpolateLayerJ(ilay+1,postr,dirtr);//L1+L9
        float disr=sqrt(postr[0]*postr[0]+postr[1]*postr[1]);
        if(ilay==0)tkl1g=(disr          <cirr[ilay]&&fabs(postr[1])<ciry[ilay]);//L1
        else       tkl9g=(fabs(postr[0])<cirr[ilay]&&fabs(postr[1])<ciry[ilay]);//L9
     }
     if(!tkl1g&&!tkl9g)continue;
     float ntkq=pev->pTrTrack(itr)->GetInnerQ();
     if(ntkq>tkiqm[0])tkiqm[0]=ntkq;
     if(tkl1g&&tkl9g&&ntkq>tkiqm[1])tkiqm[1]=ntkq;
  }
  bool l1l9tkpar=(tkiqm[0]>2.5);//L1Inner
#if defined (_HEPRESCALE2_) || defined (_PRHEPRESCALE3_)
  l1l9tkpar=(tkiqm[1]>1.5); //L1InnerL9
#endif
  if(unbiasetag){
    l1l9tkpar=((tkiqm[0]>1.5)||(tkiqm[1]>0.6));//L1Inner He,L1InnerL9 Pr
  }

//----TrdInterpolate L1 L9 Geometry
  int l1l9trdpar=0; 
  for(int itr=0;itr<pev->nTrdTrack();itr++){
    bool trdl1g=0,trdl9g=0;
    for(int ilay=0;ilay<NTKL;ilay++){//L1Pos+L9Pos Geometry
        if(ilay!=0&&ilay!=NTKL-1)continue;
        pev->pTrdTrack(itr)->Interpolate(tk_pz[ilay],postr,dirtr);//L1+L9
        float disr=sqrt(postr[0]*postr[0]+postr[1]*postr[1]);
        if(ilay==0)trdl1g=(disr          <cirr[ilay]&&fabs(postr[1])<ciry[ilay]);//L1
        else       trdl9g=(fabs(postr[0])<cirr[ilay]&&fabs(postr[1])<ciry[ilay]);//L9
     }
     if(trdl1g&&trdl9g){l1l9trdpar=2;break;}//L1InnerL9
     else if(trdl1g)   {l1l9trdpar=1;}//L1Inner
  }

//---PreScale
   bool tktrdscal=1;
#if defined (_HEPRESCALE2_) || defined (_PRHEPRESCALE3_)
   tktrdscal=(l1l9tkpar||l1l9trdpar>=2);//Track|TRDL1InnerL9G
#endif
  if(unbiasetag)tktrdscal=(l1l9tkpar||l1l9trdpar>=1);//Track|TRDL1InnerG
  if(!tktrdscal)return false;

//---L1Hit
   float l1prob=0,l9prob=0;
   for(int itkh=0;itkh<pev->nTrRecHit();itkh++){
      TrRecHitR* tkhit=pev->pTrRecHit(itkh);
      if(tkhit->GetLayerJ()==1&&tkhit->GetYCluster()!=0){
        float nlq=tkhit->GetQ(1);//MaxY
        float nlp=tkhit->GetProb();
        if(nlq>tk_l1q)                  {l1i=itkh;tk_l1q=nlq;l1prob=nlp;}
        else if(nlq==tk_l1q&&nlp>l1prob){l1i=itkh;tk_l1q=nlq;l1prob=nlp;}
     }
      if(tkhit->GetLayerJ()==9&&tkhit->GetYCluster()!=0){
        float nlq=tkhit->GetQ(1);//MaxY
        float nlp=tkhit->GetProb();
        if(nlq>tk_l9q)                  {l9i=itkh;tk_l9q=nlq;l9prob=nlp;}
        else if(nlq==tk_l9q&&nlp>l9prob){l9i=itkh;tk_l9q=nlq;l9prob=nlp;}
      }
   }

//---L1 Scale Cut
  if(l1i>=0){
    TrRecHitR* tkhit=pev->pTrRecHit(l1i);
    tk_l1qs=tkhit->GetQStatus();
    tk_l1q     =tkhit->GetQ(2);
    tk_l1qxy[0]=tkhit->GetQ(0);
    tk_l1qxy[1]=tkhit->GetQ(1);  
  }
  if(l9i>=0){
    TrRecHitR* tkhit=pev->pTrRecHit(l9i);
    tk_l9qs=tkhit->GetQStatus();
    tk_l9q     =tkhit->GetQ(2);
    tk_l9qxy[0]=tkhit->GetQ(0);
    tk_l9qxy[1]=tkhit->GetQ(1);
  }

  bool tkl1scal=(l1i>=0&&tk_l1q>2.5);//L1 Large
#if defined (_HEPRESCALE2_) || defined (_PRHEPRESCALE3_)
  tkl1scal=(l1i>=0&&tk_l1q>1.5);
#endif
  if(unbiasetag){tkl1scal=((l1i>=0&&tk_l1q>1.5)||(l1i>=0&&tk_l1q>0.6&&l1l9trdpar>=2));}//(proton TRDL1InnerL9G)
  if(!tkl1scal)return l1l9tkpar;

//----Build TkTrdParticle
   TofRecH::BuildOpt=31000;
   TofRecH::ReBuild();

//-----BuildTofClusterH
   ntofclh=pev->nTofClusterH();
   int nbetahs=pev->nBetaH();

//--Select TrdBetaH
   float tofsqs=0;
   for(int ibh=0;ibh<nbetahs;ibh++){
      BetaHR *betah=pev->pBetaH(ibh);
      if(betah->iTrdTrack()<0)continue; 
      float ntofsq=0;
      for(int ilay=0;ilay<4;ilay++){
         float tofql=betah->GetQL(ilay);
         if(tofql>0)ntofsq+=tofql;
       }
       if(ntofsq>tofsqs){
        ibetahs=ibh;
        tofsqs=ntofsq;
      }
   }
     if(ibetahs<0){return l1l9tkpar;}
     BetaHR *betah=pev->pBetaH(ibetahs);
     itrdtracks=betah->iTrdTrack();
     tof_nhits=betah->GetSumHit();
     tof_chiscs=betah->GetNormChi2C();
     tof_chists=betah->GetNormChi2T();
     betahs=betah->GetBeta();
     for(int ilay=0;ilay<4;ilay++){
        tof_qls[ilay]=betah->GetQL(ilay);
     }

//---Ecal Match
     double ecalz=-143; double time;
     betah->TInterpolate(ecalz,postr,dirtr,time);
     for(int ic=0;ic<3;ic++){ecal_pos[ic]=postr[ic];}
     ecal_ens=-1000; ecal_dis=1000;
     for(int ish=0;ish<pev->nEcalShower();ish++){
        EcalShowerR *show=pev->pEcalShower(ish);
        betah->TInterpolate(show->Entry[2],postr,dirtr,time);
        double necal_dis=pow(show->Entry[0]-postr[0],2)+pow(show->Entry[1]-postr[1],2);
        necal_dis=sqrt(necal_dis);
        if(necal_dis<ecal_dis){
           ecal_dis=necal_dis;
           ecal_ens=show->EnergyD;
        }
     }

//--L1L9Hit X+Y Match + L1L9Q
    tk_l1mds=tk_l9mds=1000;
    tk_l1qvs=tk_l9qvs=0;
    for(int uexl=0;uexl<2;uexl++){
       int ilay=(uexl==0)?0:8; 
       int iexl=(uexl==0)?l1i:l9i;
       float qex=(uexl==0)?tk_l1q:tk_l9q;
       if(iexl<0)continue; 
       TrRecHitR* tkhit=pev->pTrRecHit(iexl);
       float ml1dis=1000;
       if(tkhit->GetXCluster()!=0&&tkhit->GetYCluster()!=0){//Only X+Y Cluster, Positon
          betah->TInterpolate(tk_pz[ilay],postr,dirtr,time);
          for(int imul=0;imul<tkhit->GetMultiplicity();imul++){
             AMSPoint ml1p=tkhit->GetCoord(imul);
             double disl1=(ml1p[0]-postr[0])*(ml1p[0]-postr[0])+(ml1p[1]-postr[1])*(ml1p[1]-postr[1]);
             disl1=sqrt(disl1);
             if(disl1<ml1dis){ml1dis=disl1;}
          }
       }
//---Check Proton
       float cutexdis=10;//10cm
       if(unbiasetag&&ml1dis>cutexdis&&qex<1.5){//Low amplitude Refind L1|L9 hit
         betah->TInterpolate(tk_pz[ilay],postr,dirtr,time);
         float qexn=0,exprobn=0;
         for(int itkh=0;itkh<pev->nTrRecHit();itkh++){
           TrRecHitR* tkhit1=pev->pTrRecHit(itkh);
           if(tkhit1->GetLayerJ()!=ilay+1||tkhit1->GetYCluster()==0||tkhit1->GetXCluster()==0||itkh==iexl)continue;
           float ml1disn=1000;
           for(int imul=0;imul<tkhit1->GetMultiplicity();imul++){
             AMSPoint ml1p=tkhit1->GetCoord(imul);
             double disl1=(ml1p[0]-postr[0])*(ml1p[0]-postr[0])+(ml1p[1]-postr[1])*(ml1p[1]-postr[1]);
             disl1=sqrt(disl1);
             if(disl1<ml1disn){ml1disn=disl1;}
           }
           if(ml1disn<cutexdis){
             float nlq=tkhit1->GetQ(1);//MaxY
             float nlp=tkhit1->GetProb();
             if     (nlq>qexn)              {tkhit=tkhit1;ml1dis=ml1disn;qexn=nlq;exprobn=nlp;}
             else if(nlq==qexn&&nlp>exprobn){tkhit=tkhit1;ml1dis=ml1disn;qexn=nlq;exprobn=nlp;}
           }
         }
       }
//---
       if(uexl==0){tk_l1mds=ml1dis;tk_l1qvs=tkhit->GetQ(2,betahs);}
       else       {tk_l9mds=ml1dis;tk_l9qvs=tkhit->GetQ(2,betahs);}
    }

//---Interpolate TkL
     int nhiti=0;
     bool trdl1l9g[2]={0};
     for(int ilay=0;ilay<NTKL;ilay++){
       betah->TInterpolate(tk_pz[ilay],postr,dirtr,time);
       tk_pos1s[ilay][0]=postr[0];tk_pos1s[ilay][1]=postr[1];tk_pos1s[ilay][2]=postr[2];
       float disr=sqrt(postr[0]*postr[0]+postr[1]*postr[1]);
       if(ilay!=0&&ilay!=NTKL-1){  
          if(disr<cirr[ilay]&&fabs(postr[1])<ciry[ilay])nhiti++;
       }
       else { 
          if(ilay==0)trdl1l9g[0]=(disr          <cirr[ilay]&&fabs(postr[1])<ciry[ilay]);//L1
          else       trdl1l9g[1]=(fabs(postr[0])<cirr[ilay]&&fabs(postr[1])<ciry[ilay]);//L9
       }
     }
     bool l1trdpar=(tk_l1mds<10&&(tof_qls[0]>2.5||tof_qls[1]>2.5));
#if defined (_HEPRESCALE2_) || defined (_PRHEPRESCALE3_)
     l1trdpar=(tk_l1mds<10&&(tof_qls[0]>1.5||tof_qls[1]>1.5)&&trdl1l9g[1]);//L1Dis && TOFHe && L9Geom
//     l1trdpar=(tk_l1mds<10&&(tof_qls[0]>1.5||tof_qls[1]>1.5));
#endif
//     bool l1trdpar=(nhiti>=5&&tk_l1mds<5&&tof_qls[0]>2.5);
     if(unbiasetag){
       bool l1trdparbz=(tk_l1mds<10&&(tof_qls[0]>1.5||tof_qls[1]>1.5));
       bool l1trdparpr=(tk_l1mds<10&&(tof_qls[0]>0.6||tof_qls[1]>0.6)&&(tof_qls[2]>0.6||tof_qls[3]>0.6)&&trdl1l9g[1]); //Up+Low TOF
       l1trdpar=(l1trdparbz||l1trdparpr);
     }
     if(!l1trdpar){return l1l9tkpar;}

     return true;
}

bool AMSAnalysis::Select_2ndParticle(){

//---Secondary TrTrack
      betah2r=0;
      betah2p=0;
      betah2q=0;
      betah2hb[0]=betah2hb[1]=0;
      betah2ch=10000;

//---1nd Not Exist
     if(ibetah<0||itrtrack<0||nbetah<=1)return false;
     BetaHR *betah=pev->pBetaH(ibetah);
     float sbeta=betah->GetBeta();

//---Require Tof+Tracker
     for(int ibh=0;ibh<nbetah;ibh++){
         if(ibh==ibetah)continue;//not selected
         BetaHR *betah2=pev->pBetaH(ibh);
         if(betah2->iTrTrack()<0||betah2->iTrTrack()==itrtrack)continue;//second Tracker
//---Non First BetaH Matched TofClusterH 
         int nclmatch=0;
         for(int ilay=0;ilay<NTOFL;ilay++){
           if(!betah2->TestExistHL(ilay))continue;
           if(betah2->GetClusterHL(ilay)==betah->GetClusterHL(ilay))continue;
           nclmatch++;
         }
//---Rigidity+HitB
         float nrig= betah2->pTrTrack()->GetRigidity();
         float nchis=betah2->pTrTrack()->GetChisq();
         float ntkq= betah2->pTrTrack()->GetQ();
         int ntkhb[2];
         ntkhb[0]=betah2->pTrTrack()->GetBitPatternXYJ();
         ntkhb[1]=betah2->pTrTrack()->GetBitPatternJ();
//----
         int nhit2i[2][2]={0};
         for(int ilay=0+1;ilay<9-1;ilay++){
             for(int ixy=0;ixy<2;ixy++){
               if((betah2hb[ixy]&(1<<ilay))>0)nhit2i[0][ixy]++;
               if((ntkhb[ixy]&(1<<ilay))>0)nhit2i[1][ixy]++;
             }
         }
         bool goodtkpr=(nhit2i[0][0]>=3&&nhit2i[0][1]>=5&&betah2r/sbeta>0);
         bool goodtknw=(nhit2i[1][0]>=3&&nhit2i[1][1]>=5&&nrig/sbeta>0);
//-------
         int fw=0;
//----Good TkNW
         if(goodtknw){
            if     (!goodtkpr)               {fw=1;}//tk-first now found
            else if(fabs(nrig)>fabs(betah2r)){fw=1;}//already found Largest Rig
         }
         else if(!goodtkpr){
            if     (nclmatch>betah2p)               {fw=1;}
            else if(nclmatch==betah2p&&ntkq>betah2q){fw=1;}
         }
         if(fw!=1)continue;
         betah2r=nrig;
         betah2p=nclmatch;
         betah2q=ntkq;
         betah2hb[0]=ntkhb[0]; betah2hb[1]=ntkhb[1];
         betah2ch=nchis;
//---
     }
     
    if(betah2q==0)return false;
    return true;

}


bool AMSAnalysis::Select_LinkParticle(){

//--Particle index    
   if(ibetah>=0&&itrtrack>=0){
     for(int ipar=0;ipar<nparticle;ipar++){
       if(pev->pParticle(ipar)->iTrTrack()==itrtrack){iparindex=ipar;}
     }
   }
  else if(ibetah<0&&itrtrack<0){
     float tksq=0;
     for(int itr=0;itr<ntrack;itr++){
        TrTrackR *trk=pev->pTrTrack(itr);
        float ntksq=trk->GetQ();
        if(ntksq>tksq){itrtrack=itr;tksq=ntksq;}
     }
   }

//-----index 
   if(iparindex>=0){
     btstat=pev->pParticle(iparindex)->BT_status;
     ibeta=pev->pParticle(iparindex)->iBeta();
     itrdtrack=pev->pParticle(iparindex)->iTrdTrack();
     irich=pev->pParticle(iparindex)->iRichRing();
     irichb=pev->pParticle(iparindex)->iRichRingB();
     icharge=pev->pParticle(iparindex)->iCharge();
//-----
     trd_pass=(pev->pParticle(iparindex)->IsInsideTRD())?1:0;
  }
  else if(itrtrack>=0){
    for(int ir=0;ir<nrich;ir++){
      if(pev->pRichRing(ir)->iTrTrack()==itrtrack)irich=ir;
    }
    for(int irb=0;irb<nrichb;irb++){
       if(pev->pRichRingB(irb)->iTrTrack()==itrtrack)irichb=irb;
    }
    for(int ib=0;ib<nbeta;ib++){
       if(pev->pBeta(ib)->iTrTrack()==itrtrack)ibeta=ib;
    }
    for(int ich=0;ich<ncharge;ich++){
      if(pev->pCharge(ich)->iBetaH()==ibetah){icharge=ich;}
    }
  }

//----Ecal Match 
  show_dis=1000;
  if(ibetah>=0){
    BetaHR *betah=pev->pBetaH(ibetah);
    double ecalz=-143; double time;
    AMSPoint postr;AMSDir dirtr;
    betah->TInterpolate(ecalz,postr,dirtr,time);
    for(int ic=0;ic<3;ic++){ec_pos[ic]=postr[ic];ec_dir[ic]=dirtr[ic];}
    for(int ish=0;ish<pev->nEcalShower();ish++){
        EcalShowerR *show=pev->pEcalShower(ish);
        betah->TInterpolate(show->Entry[2],postr,dirtr,time);
        double necal_dis=pow(show->Entry[0]-postr[0],2)+pow(show->Entry[1]-postr[1],2);
        necal_dis=sqrt(necal_dis);
        if(necal_dis<show_dis){
           show_dis=necal_dis;
           ishow=ish; 
        }
    }
 } 
 

  return true; 
 
}


//---Trk
bool AMSAnalysis::Select_Trk(int sel){
    int mfit;
    TrTrackR *trk=pev->pTrTrack(itrtrack);
    if(!trk)return false;

    float obeta=0; int fid=0;
    float frig=trk->GetRigidity(fid);
    if(ibetah<0)obeta=1;
    else        obeta=pev->pBetaH(ibetah)->GetBeta();

//---Charge
   if(sel==0){

//---New charge All 
     for(int isp=0;isp<2;isp++){
//--Float Z
        mean_t tkqa;
        if(isp==0)tkqa=trk->GetQ_all(obeta,fid);
        else      tkqa=trk->GetInnerQ_all(obeta,fid);
        tk_q[isp]   =tkqa.Mean;
        tk_qrms[isp]=tkqa.RMS;
        tk_qhit[isp]=tkqa.NPoints;
//---TkZ
       vector<like_t> likelihood;
       if(isp==0)trk->GetZ(likelihood,obeta,fid);
       else      trk->GetInnerZ(likelihood,obeta,fid);
       int nz=likelihood.size()>=2?2:likelihood.size();
       for(int ip=0;ip<nz;ip++){//Max + Second Max
        tk_rz[ip][isp]=likelihood[ip].Z;
        tk_rpz[ip][isp]=likelihood[ip].Prob;
      }
    }

//---New Charge Layer     
     for(int il=0;il<9;il++){
       tk_qls[il]=0;
       tk_ql2[il][0]=tk_ql2[il][1]=0;
       TrRecHitR* tkhit =trk->GetHitLJ(il+1);
       tk_ql[il]=trk->GetLayerJQ(il+1,obeta,fid);
       if(tkhit){
          tk_qls[il]=tkhit->GetQStatus();
          tk_ql2[il][0]=tkhit->GetQ(0,obeta,frig);//XQ
          tk_ql2[il][1]=tkhit->GetQ(1,obeta,frig);//YQ
        }
     }
    

//--Tk quality
//  if(trk->GetNhits()<=4||trk->GetNhitsXY()<=3)return false;
     tk_hitb[0]=trk->GetBitPatternXYJ(); tk_hitb[1]=trk->GetBitPatternJ();

//--Span All   
    if(isreal){
       if(tk_q[1]>2.5)TrClusterR::SetLinearityCorrection();  //Linear Correction
       else           TrClusterR::UnsetLinearityCorrection();//Helium Below
    }
    else      {
       TrExtAlignDB::SmearExtAlign();//MC Smear Ext-Layer
       TRCLFFKEY.UseSensorAlign = 0;
    }
     //--Span All    
    int algo=1;//// 1:Choutko  2:Alcaraz  (+10 no multiple scattering)
    int patt;/// 3:Inner only 5:w/ L1N 6:w/ L9 7:Full span
    int refit=3;
//---Algo
    for(int ialgo=0;ialgo<3;ialgo++){
      if     (ialgo==0)algo=1;//Choutko
      else if(ialgo==1)algo=2;//Alcaraz
      if(ialgo==2&&(tk_q[1]<=4||tk_q[1]>=9))continue;//Only For BC Chikanian
      else if(ialgo==2)algo=3;//ChikanianF
//---
      for(int ipgc=0;ipgc<3;ipgc++){
        if     (ipgc==0)refit=3;//Ex-PG
        else if(ipgc==1)refit=13;//Ex-CIEMAT
        else if(ipgc==2)refit=23;//PG+CIEMAT
//--
        for(int isp=0;isp<=5;isp++){
          if     (isp==0)patt=0;//Full Span
          else if(isp==1)patt=3;//Inner
          else if(isp==2)patt=5;//Inner+Layer1
          else if(isp==3)patt=6;//Inner+Layer9
          else if(isp==4)patt=1;//Inner Up
          else if(isp==5)patt=2;//Inner Down
//--Rigidity
          if(tk_q[1]>1.5)mfit=trk->iTrTrackPar(algo,patt,refit,TrFit::Mhelium,2);//Helium+Ion
          else           mfit=trk->iTrTrackPar(algo,patt,refit,TrFit::Mproton,1);//Proton
//--clear
          float nrig=9999;
          float enrig=9999;
          float nchis[3]={9999,9999,9999};
          float ntkres[9][3];
          float dpgc[9][3];//CIMAT-PG-Diff
          for(int il=0;il<9;il++)
            for(int ixy=0;ixy<3;ixy++){ntkres[il][ixy]=9999;dpgc[il][ixy]=9999;}
//--
          if(mfit>=0){
            nrig=trk->GetRigidity(mfit);
//---Add
            if(isreal){
              float bcor1=1,bcor2=1;
              int bret1= MagnetVarp::btempcor(bcor1, 0, 1);
              int bret2= MagnetVarp::btempcor(bcor2, 0, 2);
              if (bret1==0 && bret2==0){
                  nrig *= (bcor1+bcor2)/2;
//                  cout<<"corv="<<(bcor1+bcor2)/2<<endl;
              } 
              else if (bret1!=0 && bret2 == 0) nrig *= bcor2;
            }
//----
            enrig=trk->GetErrRinv(mfit);
            nchis[0]=trk->GetNormChisqX(mfit);
            nchis[1]=trk->GetNormChisqY(mfit);
            nchis[2]=trk->GetChisq(mfit);
            for(int il=0;il<NTKL;il++){
              AMSPoint pnt=trk->GetResidualJ(il+1,mfit);
              ntkres[il][0]=pnt[0];
              ntkres[il][1]=pnt[1];
              if(ipgc==2){for(int ixy=0;ixy<3;ixy++)dpgc[il][ixy]=(trk->GetPG_CIEMAT_diff(il+1))[ixy];}
             }
          }
//--Select Copy To Data
          tk_rigidity1[ialgo][ipgc][isp]=nrig;
          tk_erigidity1[ialgo][ipgc][isp]=enrig;
          for(int ichis=0;ichis<3;ichis++){tk_chis1[ialgo][ipgc][isp][ichis]=nchis[ichis];}//Algo+PG(CIEMAT+All)+Span
          if((ialgo==0)&&(ipgc==2)&&(isp<4)){//PG+CIEMAT
             for(int il=0;il<NTKL;il++){for(int ixy=0;ixy<2;ixy++)tk_res[isp][il][ixy]=ntkres[il][ixy];}
          }
          if((ialgo==0)&&(ipgc==2)&&(isp==0)){//PG+CIEMAT
            for(int il=0;il<NTKL;il++){for(int ixy=0;ixy<3;ixy++)tk_cdif[il][ixy]=dpgc[il][ixy];}
          }
//----
      }//end isp
     }//end ipgc
    }//end ialgo
//----
   }
  
  else {

//--Track Pos
    for(int ilay=0;ilay<NTKL;ilay++){
      AMSPoint postr;AMSDir dirtr;
      trk->InterpolateLayerJ(ilay+1,postr,dirtr);
      tk_pos[ilay][0]=postr[0];tk_pos[ilay][1]=postr[1];tk_pos[ilay][2]=postr[2];
//---
      if(ilay!=0&&ilay!=NTKL-1)continue;
//----
      int uexl=(ilay==0)?0:1;
      int iexl=(ilay==0)?l1i:l9i;
      tk_exql[uexl][0]=tk_exql[uexl][1]=tk_exql[uexl][2]=0;
      tk_exqls[uexl]=0;
      tk_exdis[uexl]=1000; 
      if(iexl<0)continue; 
      TrRecHitR* tkhit=pev->pTrRecHit(iexl);
      tk_exql[uexl][0]=tkhit->GetQ(0,obeta,frig);//XQ
      tk_exql[uexl][1]=tkhit->GetQ(1,obeta,frig);//YQ
      tk_exql[uexl][2]=tkhit->GetQ(2,obeta,frig);//Q
      tk_exqls[uexl]=tkhit->GetQStatus();
      float ml1dis=1000;
      for(int imul=0;imul<tkhit->GetMultiplicity();imul++){
         AMSPoint ml1p=tkhit->GetCoord(imul);
         double disl1=(ml1p[0]-postr[0])*(ml1p[0]-postr[0])+(ml1p[1]-postr[1])*(ml1p[1]-postr[1]);
         disl1=sqrt(disl1);
         if(disl1<ml1dis){ml1dis=disl1;}
      }
      tk_exdis[uexl]=ml1dis;
//---
   }


//---2nd Trk
   tk_oq[0]=tk_oq[1]=-1;
   for(int itr=0;itr<ntrack;itr++){
      if(itr==itrtrack)continue;
      float otkq=pev->pTrTrack(itr)->GetQ();
      if     (otkq>tk_oq[0]){tk_oq[1]=tk_oq[0];tk_oq[0]=otkq;}
      else if(otkq>tk_oq[1]){tk_oq[1]=otkq;}
   }
//--

  }//end else
   return true;
}

bool AMSAnalysis::Select_Trd(){

    TrdTrackR *trdtrack=pev->pTrdTrack(itrdtrack);
    if(trdtrack){
       nitrdseg=trdtrack->NTrdSegment();
//---TrdTrack Match
       AMSDir dir;AMSPoint pos;
       TrTrackR *trk=pev->pTrTrack(itrtrack);
       if(trk){
         trk->Interpolate(trdtrack->Coo[2],pos,dir);
         distrd[0]=trdtrack->Coo[0]-pos.x();
         distrd[1]=trdtrack->Coo[1]-pos.y();
         AMSDir trddir(trdtrack->Theta,trdtrack->Phi);
         distrd[2]=acos(fabs(trddir.prod(dir)))*180/3.1415926;
       }
    }
 
//-----TrdK
   TrTrackR *trk=pev->pTrTrack(itrtrack);
   if(!trk)return false;
   if(isreal==0)return false;
   if(tof_z[0]<=2)return false;
   int Track_Fitcode_Max=trk->iTrTrackPar(1,0,0); // Get any prefered fit code
   double LikelihoodRatio[3]={-1,-1,-1};  //To be filled with 3 LikelihoodRatio :  e/P, e/H, P/H
   int NHits=0;  //To be filled with number of hits taken into account in Likelihood Calculation
   float threshold=15;  //ADC above which will be taken into account in Likelihood Calculation,  15 ADC is the recommended value for the moment.
   TrdKCluster _trdcluster= TrdKCluster(pev,trk,Track_Fitcode_Max);
   // Get the status of the Calculation
   int IsReadAlignmentOK=_trdcluster.IsReadAlignmentOK;  // 0: Alignment not performed,  1: Static Alignment of Layer level,  2: Dynamic Alignment for entire TRD 
   int IsReadCalibOK=_trdcluster.IsReadCalibOK;  // 0: Gain Calibration not performed,  1: Gain Calibration Succeeded
  // Calculate Likelihood Ratio, Fill the LikelihoodRatio, NHits according to Track selection, and return validity 
   if(IsReadAlignmentOK==0||IsReadCalibOK==0){trd_statk=-200;} 
   else                                      {trd_statk=-100;}

  if(trd_statk==-100){
     trd_statk = _trdcluster.GetLikelihoodRatio_TrTrack(threshold,LikelihoodRatio,NHits);
     if(trd_statk!=1){
       trd_statk =_trdcluster.GetLikelihoodRatio_TRDRefit(threshold,LikelihoodRatio,NHits); 
       if(trd_statk==1)trd_statk+=10;
    }
   ///Get Number of surrounding fired tubes, excluding the ones crossed by the current TrTrack predic
    if(trd_statk%10==1){
      for(int i=0;i<3;i++)trd_lik[i]=LikelihoodRatio[i];
      trd_nhitk=NHits;
    }
    else {
      for(int i=0;i<3;i++)trd_lik[i]=-1;
      trd_nhitk=NHits;
   }
   ///TRD-Charge
     _trdcluster.CalculateTRDCharge(0,tof_betah);
     trd_qk[0]=      _trdcluster.GetTRDCharge();
     trd_qk[1]=      _trdcluster.GetTRDChargeUpper();
     trd_qk[2]=      _trdcluster.GetTRDChargeLower();
     trd_qrmsk[0]=   _trdcluster.GetTRDChargeError();
     trd_qnhk[0]= _trdcluster.GetQNHit();
     trd_qnhk[1]= _trdcluster.GetQNHitUpper();
     trd_qnhk[2]= _trdcluster.GetQNHitLower();
     trd_ipch=    _trdcluster.GetIPChi2();//impact Chis

///--End Fragmentation delta
     _trdcluster.CalculateTRDCharge(1,tof_betah);
     trd_qk[3]=      _trdcluster.GetTRDCharge();
     trd_qrmsk[1]=    _trdcluster.GetTRDChargeError();
      _trdcluster.CalculateTRDCharge(2,tof_betah);
     trd_qk[4]=      _trdcluster.GetTRDCharge();
     trd_qrmsk[2]=    _trdcluster.GetTRDChargeError();

 }//TrdK

//---Trd-Default-Charge
    ChargeR *charge=pev->pCharge(icharge);
    if(charge){
      ChargeSubDR *zTrd=charge->getSubD("AMSChargeTRD");
      if(zTrd){
        trd_rz = TMath::Max(Int_t(zTrd->ChargeI[0]),1);
        trd_rq = zTrd->Q;
        trd_rpz= zTrd->getProb();
      }
    }

 
    return true;
}

bool AMSAnalysis::Select_Tof(){

//---
     float rigidity[3]={0};
     if(itrtrack>=0){
       rigidity[0]=(tk_rigidity1[0][2][0]!=9999)?tk_rigidity1[0][2][0]:0;//Full Span
       rigidity[1]=(tk_rigidity1[0][2][1]!=9999)?tk_rigidity1[0][2][1]:0;//Inner
       rigidity[2]=(tk_rigidity1[0][2][2]!=9999)?tk_rigidity1[0][2][2]:0;//Inner+L1
     }
//--
     BetaR *beta=pev->pBeta(ibeta);
     if(beta)tof_beta=beta->Beta; 
//---
     BetaHR *betah=pev->pBetaH(ibetah);
      
     if(!betah)return false;
     tof_btype=betah->GetBuildType();

     tof_betah=betah->GetBeta();
     tof_chist=betah->GetChi2T();
     tof_chisc=betah->GetChi2C();
     tof_chisc_n=betah->GetNormChi2C();
     tof_chist_n=betah->GetNormChi2T();
     tof_hsumh=betah->GetSumHit();
     tof_hsumhu=betah->GetUseHit();

//---TOF Layer
     tof_pass=0;
     AMSPoint pnt; AMSDir dir; AMSPoint tofpnt;
     float disedge;double time;
     for(int ilay=0;ilay<NTOFL;ilay++){
//---Clear
        tof_barid[ilay]=tof_tl[ilay]=-1;
        tof_oq[ilay][0]=tof_oq[ilay][1]=0;
        tof_ob[ilay][0]=tof_ob[ilay][1]=-1;
//---All
        tof_nclhl[ilay]=betah->GetAllFireHL(ilay);
        betah->TInterpolate (TOFGeom::GetMeanZ(ilay),pnt,dir,time);
        int tk_pass=ParticleR::IsPassTOF(ilay,pnt,dir,tofpnt,disedge);
        if(tk_pass>=0){tof_pass+=int(pow(10.,3-ilay));}
//----TOFCL
        if(!betah->TestExistHL(ilay))continue;
        TofClusterHR *tofclh=betah->GetClusterHL(ilay);
        tof_barid[ilay]=tofclh->Bar;
        if(tofclh->IsGoodTime()){tof_tl[ilay]=tofclh->Time;}
     }

//---TOF ClusterH
    for(int iclh=0;iclh<ntofclh;iclh++){
       TofClusterHR *tofclh=pev->pTofClusterH(iclh);
       int ilay=tofclh->Layer;
       if(tofclh->Bar==tof_barid[ilay])continue;
       if(tofclh->GetQSignal()>tof_oq[ilay][0]){//>Max NonBetaH Cluster
          tof_oq[ilay][1]=tof_oq[ilay][0];
          tof_oq[ilay][0]=tofclh->GetQSignal();
          tof_ob[ilay][1]=tof_ob[ilay][0];
          tof_ob[ilay][0]=tofclh->Bar;
       }
       else if(tofclh->GetQSignal()>tof_oq[ilay][1]){//>Second Max NonBetaH Cluster
          tof_oq[ilay][1]=tofclh->GetQSignal();
          tof_ob[ilay][1]=tofclh->Bar;
       }
    }

//--TOFCharge All
   tof_qs=0;
   TofChargeHR *tofhc=betah->pTofChargeH();
   int nlay=0;
   tof_zr=tofhc->GetZ(nlay,tof_pzr);
   for(int uir=0;uir<3;uir++){
     tofhc->ReFit(0,TofChargeHR::DefaultQOptIonW,rigidity[uir]); 
     tof_q[uir][0]=tofhc->GetQ(nlay,tof_qrms[uir][0],1111);//All
     tof_q[uir][1]=tofhc->GetQ(nlay,tof_qrms[uir][1],-2);//Truncate 
///---Z All
     tof_z[uir]   =tofhc->GetZ(nlay,tof_pz[uir]);
     tof_zud[uir][0]=tofhc->GetZ(nlay,tof_pud[uir][0],0,1100);
     tof_zud[uir][1]=tofhc->GetZ(nlay,tof_pud[uir][1],0,11);

//---TOFQL
     for(int ilay=0;ilay<NTOFL;ilay++){
//---Clear
       for(int ity=0;ity<3;ity++){tof_qlr[uir][ity][ilay]=0;}
       tof_ql[uir][ilay]=tof_ql1[uir][ilay]=0;
//---BetaH Check
       tof_ql1[uir][ilay]=betah->GetQL(ilay,2,TofClusterHR::DefaultQOptIonW,111111,0,rigidity[uir]);
//---QL
       if(!tofhc->TestExistHL(ilay))continue;
       tof_ql[uir][ilay] =tofhc->GetQL(ilay,22);//New TOFL
       tof_qlr[uir][0][ilay]=tofhc->GetQL(ilay,12);
       tof_qlr[uir][1][ilay]=tofhc->GetQL(ilay,1);
       tof_qlr[uir][2][ilay]=tofhc->GetQL(ilay,0);
       if(uir!=0)continue;//only for uir==0
       if(tofhc->IsGoodQPathL(ilay)){tof_qs+=int(pow(10.,3-ilay));}
     }
   }

  return true; 
}

bool AMSAnalysis::Select_Rich(){
//---
    if(irich<0&&nrich>=0)irich=0;
    RichRingR *ring=pev->pRichRing(irich);
    if(ring){
      rich_beta[0]=ring->getBeta();
      rich_beta[1]=ring->Beta; 
      rich_beta[2]=ring->BetaRefit;
      rich_ebeta=ring->ErrorBeta;
      rich_ebeta1=ring->getBetaError();
      rich_pb=ring->Prob;
      rich_udis=ring->UDist;
//      rich_used=ring->Used;
      rich_hit=ring->getHits();// 
      rich_used=ring->getUsedHits();
//      rich_usedm=ring->UsedM;
      rich_usedm=ring->getReflectedHits();
      rich_q[0]=ring->getCharge2Estimate();
      rich_q[1]=ring->NpExp>0?sqrt(ring->NpCol/ring->NpExp):-1;
      rich_width=ring->getWidth();
      rich_npe[0]=ring->getPhotoelectrons();
      rich_npe[1]=ring->getExpectedPhotoElectrons();
      rich_npe[2]=RichHitR::getCollectedPhotoElectrons();
      rich_stat=ring->Status;
      rich_good=ring->IsGood();
      rich_clean=ring->IsClean();
      rich_NaF=ring->IsNaF();
      rich_KolProb=ring->getProb();
      rich_pmt=ring->getPMTs();
      rich_pmt1=RichHitR::getPMTs()-RichHitR::getPMTs(false);
      rich_PMTChargeConsistency=ring->getPMTChargeConsistency();
      rich_BetaConsistency=ring->getBetaConsistency();
      rich_x=ring->getTrackEmissionPoint()[0];
      rich_y=ring->getTrackEmissionPoint()[1];
      rich_tile=ring->getTileIndex();
      rich_distb=ring->DistanceTileBorder();
      rich_cstat=ring->PmtCorrectionsFailed();
    }
//-----
     if(irichb<0&&nrichb>=0)irichb=0; 
     RichRingBR *ringb=pev->pRichRingB(irichb);
     if(ringb){
       rich_betap=ringb->Beta;
       rich_likp= ringb->Likelihood;
       rich_pbp=  ringb->ProbKolm;
       rich_usedp=ringb->Used;
       rich_qp=   ringb->ChargeRec; 
       rich_statp=ringb->Status;
       for(int iz=0;iz<3;iz++){rich_pzp[iz]=ringb->ChargeProb[iz];}
     }
   
//---
   //---Trd-Default-Charge
   ChargeR *charge=pev->pCharge(icharge);
   if(charge){
     ChargeSubDR *zRing=charge->getSubD("AMSChargeRich");
     if(zRing){
       rich_rz = TMath::Max(Int_t(zRing->ChargeI[0]),1);
       rich_rq = zRing->Q;
       rich_rpz= zRing->getProb();
     }
   }
 
 
  return true;
}

bool AMSAnalysis::Select_Ecal(){
   
   EcalShowerR *show= pev->pEcalShower(ishow);
   ecal_q=0;
   for(int ilay=0;ilay<NECALL;ilay++){ecal_el[ilay]=ecal_eh[ilay]=ecal_nhit[ilay]=0;}
   if(show){
     ecal_en[0]=show->EnergyE;
     ecal_en[1]=show->EnergyA;
     ecal_en[2]=show->EnergyD;
     ecal_q=show->EcalChargeEstimator();
     for(int i2dcl=0;i2dcl<show->NEcal2DCluster();i2dcl++){//2dCl
       Ecal2DClusterR *e2dcl=show->pEcal2DCluster(i2dcl);
       for(int icl=0;icl<e2dcl->NEcalCluster();icl++){//1dCl
         EcalClusterR *ecl=e2dcl->pEcalCluster(icl);
         for(int ihit=0;ihit<ecl->NEcalHit();ihit++){//Nhit
           EcalHitR *ehit=ecl->pEcalHit(ihit);
           float edep=ehit->Edep;
           int ilay=ehit->Plane; 
           ecal_nhit[ilay]++;
           ecal_el[ilay]+=edep;
           if(edep>ecal_eh[ilay]){ecal_eh[ilay]=edep;}
         }
       }
     }
     /*ecal_bdt[0]=show->GetEcalBDT();
     ecal_bdt[1]=show->EcalStandaloneEstimatorV2();
     ecal_bdt[2]=show->GetEcalBDTCHI2(pev);*/
   }

/*   for(int i=0;i<pev->nEcalHit();i++){
        ecal_edep+=pev->pEcalHit(i)->Edep/1000.;
    }*/
   //if(sumedep>1)return 0;
   return true;
}

bool AMSAnalysis::Select_Anti(){
  return true;
}


void AMSAnalysis::InitEvent(AMSEventR *ev){
//---Header
   run=0;event=0;time[0]=0;time[1]=0;isbadrun=0;
//---Trigger
   nlevel1=-1;
//--Particle
   nparticle=nbetah=nbeta=ntrack=ntrdtrack=nrich=nrichb=nshow=ntofclh=ncharge=-1;
   tk_l1q=0;
   trd_pass=-1;
//---
   l1i=l9i=itrtrack=ibetah=ibeta=iparindex=itrdtrack=ishow=irich=irichb=icharge=-1;
//---
   ibetahs=itrdtracks=-1;
//---TOF
   tof_z[0]=0;
   tof_betah=tof_beta=-3;
//---Track
   tk_rz[0][0]=tk_rz[0][1]=tk_rz[1][0]=tk_rz[1][1]=-1;
//---Trd
   trd_nhitk=-1;
   trd_statk=-1000;
   trd_rq=trd_rz=-1;
//--Rich
   rich_beta[0]=rich_beta[1]=rich_betap=-3;
   rich_rz=rich_rq=-1;
//--Ecal
   ecal_en[0]=ecal_en[1]=ecal_en[2]=-3;
//--Anti
   anti_nhit=-1;

  pev=ev;
}

void AMSAnalysis::Fill(int it){
  if     (it==0)tout[0]->Fill();
  else if(it==1)tout[1]->Fill(); 
  else          tout[2]->Fill(); 
}

void AMSAnalysis::Save(){
    fout->cd();
    for(int it=0;it<3;it++)tout[it]->Write();
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
    int nrun=-1;
    cout<<"argc="<<argc<<endl;
    if(argc>=3){
      sprintf(ifile,"%s",argv[1]);
      sprintf(ofile,"%s",argv[2]);
      cout<<"ifile="<<ifile<<endl;
      cout<<"ofile="<<ofile<<endl;
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
//    char *otxt="./bac/";
    char *otxt=0;
    if(argc>=6){
       otxt=argv[5];
       cout<<"otxt="<<otxt<<endl;
    }

    char *ofilecp=0;
    if(argc>=7){
       ofilecp=argv[6];
       cout<<"ofilecp="<<ofilecp<<endl;
    }

    signal(SIGTERM, shandler);
    signal(SIGINT, shandler);

    AMSChain ams;
    int nfile=-1;
    if(rmode==1){
        ams.Add(ifile);
    }
    else{
       if(readfile(ifile,ams,nfile,0,otxt)!=0){
         printf("error files read:%s \n",ifile);
         return 2;
       }
    }//read file

//---
    AMSAnalysis *analysis =new AMSAnalysis();
    analysis->BookFile(ofile);
    analysis->LoadHeader();
///---Baichi
    Long64_t num2= ((num==-1)||(num>ams.GetEntries()))?ams.GetEntries():num;
    cout<<"num2="<<num2<<endl;
    int nevout=300000;
    static unsigned int prrun=0;
    static string bf="";//bad Tree
    static string pf="";//Pre Tree
    AMSEventR::fRunList.clear();//Clear AMSEventR:fRunList
    for(Long64_t entry=0; entry<num2; entry++){
//        AMSEventR *ev=(AMSEventR *)ams.GetEvent(entry);
        AMSEventR *ev=(AMSEventR *)ams.GetEvent();
        if(ev==NULL)continue;
        if(ev->Run()!=prrun){cout<<"ProcessInfo NewRun="<<ev->Run()<<endl;prrun=ev->Run();}
        if(entry%nevout==0)cout<<"ProcessInfo Entry="<<entry<<" Run="<<ev->Run()<<" Event="<<ev->Event()<<endl; 
        int records=ev->RecordRTIRun();//Run to AMSEventR:fRunList
//----Check Run in AMSEventR:fRunList
        TTree *evtree=ev->Tree();
        string nf=evtree->GetCurrentFile()->GetName();
        if (nf==bf)continue;//BadRunList
        if (nf!=pf&&records==2){//New Tree Exist in RunList
            cout<<"Error ExistFile in RunList="<<nf<<endl;
            pf=bf=nf;//Exist to  BadRunList
            continue;
        }
        else {pf=nf;}
//---Set Unbiase
       analysis->unbiasemctag=0;//MC
       analysis->unbiasetag=0;//ISS
//---Set MC Unbiase
       if(ev->nMCEventg()){//MC
          analysis->unbiasemctag=1;
#ifdef _MCPRESCALE_
 //         cout<<"current event="<<ev->Tree()->GetReadEvent()<<" "<<ev->Tree()->GetEntries()<<endl;
          if((evtree->GetReadEvent()!=0)&&(evtree->GetReadEvent()!=evtree->GetEntries()-1)){//no tree bein+end
            analysis->unbiasemctag=0;
          }
#endif
      }
       analysis->InitEvent(ev);
       int ksel=analysis->Select();
       if(ksel==1)analysis->Fill(1);
//----Set ISS|MC Unbiase
        if(ev->nMCEventg()){//MC
#ifdef _USEMCTKPr_
           if(entry%prunbiasmcpr==0)analysis->unbiasetag=1;
#else
           if(entry%prunbiasmc==0)analysis->unbiasetag=1;
#endif
        }
        else {
           if(entry%prunbias==0)analysis->unbiasetag=1;
        } 
        if(analysis->unbiasetag){
          analysis->InitEvent(ev);
          ksel=analysis->Select();
          if(ksel==1)analysis->Fill(2);
        } 
//---
        if(stop==1){cout<<"Force stop"<<endl;goto ENDList;}
    }

//--Write To List
  {
    map <string,unsigned int >runlistn;
    for(map<unsigned int,AMSSetupR::RunI>::iterator it=AMSEventR::fRunList.begin();it!=AMSEventR::fRunList.end();it++){//Get Run by Run
       AMSSetupR::RunI runi=it->second; //run information
       for(int k=0;k<runi.fname.size();k++)runlistn[runi.fname[k]]=runi.run;
    }
    nrun=WriteList(otxt,runlistn);
    AMSEventR::fRunList.clear();

//---CopyToEOS
     analysis->Save();
    if(nrun>0)cpofile(ofile,ofilecp,0);
  }

///--List Status
ENDList:
    const char *sname[11]={
      "Head  ", "Trig  ","RTI|MC","Part    ","Trk0    ","Tof     ","Trk1    ","Trd     ","Rich    ","Ecal    ","Anti    ",
    };
///---
    cout<<"<<---------Begin Static-------------->>"<<endl;
    cout<<"   Event("<<sstat[0]<<")"<<"   CPU"<<"   TreeSize"<<endl;
    for(int icut=0;icut<11;icut++){
      cout<<sname[icut]<<"   "<<sstat[icut+1]<<"   "<<(double(cput[icut]/CLOCKS_PER_SEC))<<"   "<<sstree[icut]<<endl;
     }
    cout<<"    Process Run="<<nrun<<endl;
    cout<<"<<---------End Static AMSJOB-------------->>"<<endl;

//-----
   int nstat=(nrun<=0)?2:0;
   return nstat;//nstat==2 no-run nstat=0 normal
   

}
