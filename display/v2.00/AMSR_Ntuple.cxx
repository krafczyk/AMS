
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_Ntuple                                                          //
// ===========                                                          //
//                                                                      //
// Class to handle ntuple and create link between it and ROOT Tree      //
// and interface to ROOT Tree                                           //
//                                                                      //
// SetTree(TTree*) used to create link between ntuple and ROOT Tree     //
//                                                                      //
// SetBranchStatus(char *varname, Bool_t status) simulates TTree to     //
// set ActiveBranches in order to fasten ntuple data accessing          //
//                                                                      //
// Usage:                                                               //
//                                                                      //
//   1. Create a class, e.g., AMSR_Ntuple *ntp = new AMSR_Ntuple();     //
//   Only one object of AMSR_Ntuple should be created !                 //
//                                                                      //
//   2. Initialize data file :                                          //
//      For ROOT file, ntp->SetTree(TTree *t) to set up address.        //
//      For ntuple file, ntp->OpenNtuple(ntpfile) and SetTree(0) to     //
//   open file and set link between ntuple and ROOT tree. The ntuple ID //
//   is 1 by default which can be changed via "ntp->SetNtupleID(id)".   //
//                                                                      //
//   3. Read event: ntp->GetEvent(event).                               //
//                                                                      //
//   4. Select variables/branches for reading                           //
//      ntp->SetBranchStatus(varname, status) to (de)activate a variable//
//   or branch for reading. When varname == "*", all variables/branches //
//   will be invoked. e.g. ntp->SetBranchStatus("*",1) to activate all  //
//   variables/branches.                                                //
//                                                                      //
//   5. To impose cuts on ntuple/tree, create object of TTreeFormula    //
//  to tree which can be get "GetTree()".                               //
//                                                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "AMSR_Ntuple.h"
#include <TTree.h>
//#include <TClonesArray.h>
//#include <TObjString.h>
#include <TFile.h>
#include <stdio.h>

PAWC_DEF PAWC;

EVENTH_DEF blkEventh;
BETA_DEF blkBeta;
CHARGE_DEF blkCharge;
PARTICLE_DEF blkParticle;
TOFCLUST_DEF blkTofclust;
TOFMCCLU_DEF blkTofmcclu;
TRCLUSTE_DEF blkTrcluste;
TRMCCLUS_DEF blkTrmcclus;
TRRECHIT_DEF blkTrrechit;
TRTRACK_DEF blkTrtrack;
MCEVENTG_DEF blkMceventg;
CTCCLUST_DEF blkCtcclust;
CTCMCCLU_DEF blkCtcmcclu;
ANTICLUS_DEF blkAnticlus;
ANTIMCCL_DEF blkAntimccl;
LVL3_DEF blkLvl3;
LVL1_DEF blkLvl1;
CTCHIT_DEF blkCtchit;
TRRAWCL_DEF blkTrrawcl;
ANTIRAWC_DEF blkAntirawc;
TOFRAWCL_DEF blkTofrawcl;

static char dir1[] = "//PHY1", *top1 = "PHY1";
static char dir2[] = "//PHY2", *top2 = "PHY2";

AMSR_Ntuple *gAMSR_Ntuple = 0;

ClassImp(AMSR_Ntuple)

//_____________________________________________________________________________
AMSR_Ntuple::AMSR_Ntuple(const char *name, const char *title)
     : TNamed(name,title)
{
   m_DataFileType = kUnknown;
   m_NVar       = 0;
   m_Entries    = 0;
   m_Lun        = 82;
   m_NtupleID   = 1;
   m_NextID     = m_NtupleID;
   m_MemID      = m_NtupleID;
   m_NtupleOpen = kFALSE;
   m_SameRead   = kFALSE;
   m_Tree       = 0;
   m_SampleTree = 0;
   
   int cols = sizeof(m_VarNames[0]);
   int rows = sizeof(m_VarNames)/cols;
   for (int i=0; i<rows; i++) {
      m_VarNames[i][cols-1] = 0;
   }
   
   m_BlkEventh   = &blkEventh;
   m_BlkBeta     = &blkBeta;
   m_BlkCharge   = &blkCharge;
   m_BlkParticle = &blkParticle;
   m_BlkTofclust = &blkTofclust;
   m_BlkTofmcclu = &blkTofmcclu;
   m_BlkTrcluste = &blkTrcluste;
   m_BlkTrmcclus = &blkTrmcclus;
   m_BlkTrrechit = &blkTrrechit;
   m_BlkTrtrack  = &blkTrtrack;
   m_BlkMceventg = &blkMceventg;
   m_BlkCtcclust = &blkCtcclust;
   m_BlkCtcmcclu = &blkCtcmcclu;
   m_BlkAnticlus = &blkAnticlus;
   m_BlkAntimccl = &blkAntimccl;
   m_BlkLvl3     = &blkLvl3;
   m_BlkLvl1     = &blkLvl1;
   m_BlkCtchit   = &blkCtchit;
   m_BlkTrrawcl  = &blkTrrawcl;
   m_BlkAntirawc = &blkAntirawc;
   m_BlkTofrawcl = &blkTofrawcl;
   
   if (gAMSR_Ntuple == 0) HLIMIT(NWPAW);

   CreateSampleTree();

   gAMSR_Ntuple = this;
}

//_____________________________________________________________________________
AMSR_Ntuple::~AMSR_Ntuple()
{
//   if (m_SampleTree != 0 && m_SampleTree->IsOpen()) m_SampleTree->Close();
   if (m_SampleTree != 0) delete m_SampleTree;
}

//_____________________________________________________________________________
void AMSR_Ntuple::CloseNtuple()
{
   if (m_NtupleOpen) {
      if (m_Lun == 81) {
	 HCDIR(dir1," ");
         HDELET(m_MemID);
         HREND(top1);
      } else {
	 HCDIR(dir2," ");
         HDELET(m_MemID);
         HREND(top2);
      }
      CLOSE(m_Lun);
      m_NtupleOpen = kFALSE;
   }
}

//_____________________________________________________________________________
void AMSR_Ntuple::CreateSampleTree()
{
//   //
//   //Get a sample of ROOT-TTree
//   //
//   if (m_SampleTree == 0 ) m_SampleTree = TFile::Open("NtupleSample.root");
//   m_Tree = (TTree*)m_SampleTree->Get("h1");
//
//   //
//   //Set Branch Addresses
//   //
//   SetTreeAddress();
   
   //
   //Check if already exist, if yes, get the pointer, otherwise, create one
   //
   if (m_SampleTree != 0) {
      m_Tree = m_SampleTree;
      return;
   }

   //
   //Create a new tree as a sample
   //
   m_SampleTree = new TTree("sample","a sample tree for ntuple");
   m_Tree = m_SampleTree;

   m_Tree->Branch("eventno", &m_BlkEventh->eventno, "eventno/i");
   m_Tree->Branch("run", &m_BlkEventh->run, "run/I");
   m_Tree->Branch("runtype", &m_BlkEventh->runtype, "runtype/I");
   m_Tree->Branch("time", m_BlkEventh->time, "time2]/I");
   m_Tree->Branch("Rawwords", &m_BlkEventh->Rawwords, "Rawwords/i");
   m_Tree->Branch("Rads", &m_BlkEventh->Rads, "Rads/F");
   m_Tree->Branch("Thetas", &m_BlkEventh->Thetas, "Thetas/F");
   m_Tree->Branch("Phis", &m_BlkEventh->Phis, "Phis/F");
   m_Tree->Branch("Yaws", &m_BlkEventh->Yaws, "Yaws/F");
   m_Tree->Branch("Pitchs", &m_BlkEventh->Pitchs, "Pitchs/F");
   m_Tree->Branch("Rolls", &m_BlkEventh->Rolls, "Rolls/F");
   m_Tree->Branch("Velocitys", &m_BlkEventh->Velocitys, "Velocitys/F");
   m_Tree->Branch("Veltheta", &m_BlkEventh->Veltheta, "Veltheta/F");
   m_Tree->Branch("Velphi", &m_BlkEventh->Velphi, "Velphi/F");
   m_Tree->Branch("Particles", &m_BlkEventh->Particles, "Particles/i");
   m_Tree->Branch("Tracks", &m_BlkEventh->Tracks, "Tracks/i");
   m_Tree->Branch("Betas", &m_BlkEventh->Betas, "Betas/i");
   m_Tree->Branch("Charges", &m_BlkEventh->Charges, "Charges/i");
   m_Tree->Branch("Trrechits", &m_BlkEventh->Trrechits, "Trrechits/i");
   m_Tree->Branch("Trclusters", &m_BlkEventh->Trclusters, "Trclusters/i");
   m_Tree->Branch("Trrawclusters", &m_BlkEventh->Trrawclusters, "Trrawclusters/i");
   m_Tree->Branch("Trmcclusters", &m_BlkEventh->Trmcclusters, "Trmcclusters/i");
   m_Tree->Branch("Tofclusters", &m_BlkEventh->Tofclusters, "Tofclusters/i");
   m_Tree->Branch("Tofmcclusters", &m_BlkEventh->Tofmcclusters, "Tofmcclusters/i");
   m_Tree->Branch("Ctcclusters", &m_BlkEventh->Ctcclusters, "Ctcclusters/i");
   m_Tree->Branch("Ctcmcclusters", &m_BlkEventh->Ctcmcclusters, "Ctcmcclusters/i");
   m_Tree->Branch("Antimcclusters", &m_BlkEventh->Antimcclusters, "Antimcclusters/i");
   m_Tree->Branch("Anticlusters", &m_BlkEventh->Anticlusters, "Anticlusters/i");
   m_Tree->Branch("Eventstatus", &m_BlkEventh->Eventstatus, "Eventstatus/I");
   m_Tree->Branch("nbeta", &m_BlkBeta->nbeta, "nbeta/I");
   m_Tree->Branch("betastatus", m_BlkBeta->betastatus, "betastatusnbeta]/I");
   m_Tree->Branch("betapattern", m_BlkBeta->betapattern, "betapatternnbeta]/i");
   m_Tree->Branch("beta", m_BlkBeta->beta, "betanbeta]/F");
   m_Tree->Branch("betaerror", m_BlkBeta->betaerror, "betaerrornbeta]/F");
   m_Tree->Branch("betachi2", m_BlkBeta->betachi2, "betachi2nbeta]/F");
   m_Tree->Branch("betachi2s", m_BlkBeta->betachi2s, "betachi2snbeta]/F");
   m_Tree->Branch("betantof", m_BlkBeta->betantof, "betantofnbeta]/i");
   m_Tree->Branch("betaptof", m_BlkBeta->betaptof, "betaptofnbeta][4]/I");
   m_Tree->Branch("betaptr", m_BlkBeta->betaptr, "betaptrnbeta]/I");
   m_Tree->Branch("ncharge", &m_BlkCharge->ncharge, "ncharge/I");
   m_Tree->Branch("chargestatus", m_BlkCharge->chargestatus, "chargestatusncharge]/I");
   m_Tree->Branch("chargebetap", m_BlkCharge->chargebetap, "chargebetapncharge]/I");
   m_Tree->Branch("chargetof", m_BlkCharge->chargetof, "chargetofncharge]/i");
   m_Tree->Branch("chargetracker", m_BlkCharge->chargetracker, "chargetrackerncharge]/i");
   m_Tree->Branch("probtof", m_BlkCharge->probtof, "probtofncharge][10]/F");
   m_Tree->Branch("probtracker", m_BlkCharge->probtracker, "probtrackerncharge][10]/F");
   m_Tree->Branch("truntof", m_BlkCharge->truntof, "truntofncharge]/F");
   m_Tree->Branch("truntracker", m_BlkCharge->truntracker, "truntrackerncharge]/F");
   m_Tree->Branch("npart", &m_BlkParticle->npart, "npart/I");
   m_Tree->Branch("pctcp", m_BlkParticle->pctcp, "pctcpnpart][2]/I");
   m_Tree->Branch("pbetap", m_BlkParticle->pbetap, "pbetapnpart]/i");
   m_Tree->Branch("pchargep", m_BlkParticle->pchargep, "pchargepnpart]/I");
   m_Tree->Branch("ptrackp", m_BlkParticle->ptrackp, "ptrackpnpart]/I");
   m_Tree->Branch("pid", m_BlkParticle->pid, "pidnpart]/i");
   m_Tree->Branch("pmass", m_BlkParticle->pmass, "pmassnpart]/F");
   m_Tree->Branch("perrmass", m_BlkParticle->perrmass, "perrmassnpart]/F");
   m_Tree->Branch("pmom", m_BlkParticle->pmom, "pmomnpart]/F");
   m_Tree->Branch("perrmom", m_BlkParticle->perrmom, "perrmomnpart]/F");
   m_Tree->Branch("pcharge", m_BlkParticle->pcharge, "pchargenpart]/F");
   m_Tree->Branch("ptheta", m_BlkParticle->ptheta, "pthetanpart]/F");
   m_Tree->Branch("pphi", m_BlkParticle->pphi, "pphinpart]/F");
   m_Tree->Branch("thetagl", m_BlkParticle->thetagl, "thetaglnpart]/F");
   m_Tree->Branch("phigl", m_BlkParticle->phigl, "phiglnpart]/F");
   m_Tree->Branch("pcoo", m_BlkParticle->pcoo, "pcoonpart][3]/F");
   m_Tree->Branch("signalctc", m_BlkParticle->signalctc, "signalctcnpart][2]/F");
   m_Tree->Branch("betactc", m_BlkParticle->betactc, "betactcnpart][2]/F");
   m_Tree->Branch("errorbetactc", m_BlkParticle->errorbetactc, "errorbetactcnpart][2]/F");
   m_Tree->Branch("cooctc", m_BlkParticle->cooctc, "cooctcnpart][2][3]/F");
   m_Tree->Branch("cootof", m_BlkParticle->cootof, "cootofnpart][4][3]/F");
   m_Tree->Branch("cooanti", m_BlkParticle->cooanti, "cooantinpart][2][3]/F");
   m_Tree->Branch("cootr", m_BlkParticle->cootr, "cootrnpart][6][3]/F");
   m_Tree->Branch("ntof", &m_BlkTofclust->ntof, "ntof/I");
   m_Tree->Branch("Tofstatus", m_BlkTofclust->Tofstatus, "Tofstatusntof]/I");
   m_Tree->Branch("plane", m_BlkTofclust->plane, "planentof]/i");
   m_Tree->Branch("bar", m_BlkTofclust->bar, "barntof]/i");
   m_Tree->Branch("Tofedep", m_BlkTofclust->Tofedep, "Tofedepntof]/F");
   m_Tree->Branch("Toftime", m_BlkTofclust->Toftime, "Toftimentof]/F");
   m_Tree->Branch("Tofetime", m_BlkTofclust->Tofetime, "Tofetimentof]/F");
   m_Tree->Branch("Tofcoo", m_BlkTofclust->Tofcoo, "Tofcoontof][3]/F");
   m_Tree->Branch("Tofercoo", m_BlkTofclust->Tofercoo, "Tofercoontof][3]/F");
   m_Tree->Branch("ntofmc", &m_BlkTofmcclu->ntofmc, "ntofmc/I");
   m_Tree->Branch("Tofmcidsoft", m_BlkTofmcclu->Tofmcidsoft, "Tofmcidsoftntofmc]/I");
   m_Tree->Branch("Tofmcxcoo", m_BlkTofmcclu->Tofmcxcoo, "Tofmcxcoontofmc][3]/F");
   m_Tree->Branch("Tofmctof", m_BlkTofmcclu->Tofmctof, "Tofmctofntofmc]/F");
   m_Tree->Branch("Tofmcedep", m_BlkTofmcclu->Tofmcedep, "Tofmcedepntofmc]/F");
   m_Tree->Branch("Ntrcl", &m_BlkTrcluste->Ntrcl, "Ntrcl/I");
   m_Tree->Branch("Idsoft", m_BlkTrcluste->Idsoft, "IdsoftNtrcl]/I");
   m_Tree->Branch("Statust", m_BlkTrcluste->Statust, "StatustNtrcl]/I");
   m_Tree->Branch("Neleml", m_BlkTrcluste->Neleml, "NelemlNtrcl]/I");
   m_Tree->Branch("Nelemr", m_BlkTrcluste->Nelemr, "NelemrNtrcl]/i");
   m_Tree->Branch("Sumt", m_BlkTrcluste->Sumt, "SumtNtrcl]/F");
   m_Tree->Branch("Sigmat", m_BlkTrcluste->Sigmat, "SigmatNtrcl]/F");
   m_Tree->Branch("Meant", m_BlkTrcluste->Meant, "MeantNtrcl]/F");
   m_Tree->Branch("Rmst", m_BlkTrcluste->Rmst, "RmstNtrcl]/F");
   m_Tree->Branch("Errormeant", m_BlkTrcluste->Errormeant, "ErrormeantNtrcl]/F");
   m_Tree->Branch("Amplitude", m_BlkTrcluste->Amplitude, "AmplitudeNtrcl][5]/F");
   m_Tree->Branch("ntrclmc", &m_BlkTrmcclus->ntrclmc, "ntrclmc/I");
   m_Tree->Branch("Idsoftmc", m_BlkTrmcclus->Idsoftmc, "Idsoftmcntrclmc]/I");
   m_Tree->Branch("Itra", m_BlkTrmcclus->Itra, "Itrantrclmc]/I");
   m_Tree->Branch("Left", m_BlkTrmcclus->Left, "Leftntrclmc][2]/I");
   m_Tree->Branch("Center", m_BlkTrmcclus->Center, "Centerntrclmc][2]/I");
   m_Tree->Branch("Right", m_BlkTrmcclus->Right, "Rightntrclmc][2]/I");
   m_Tree->Branch("ss", m_BlkTrmcclus->ss, "ssntrclmc][2][5]/F");
   m_Tree->Branch("xca", m_BlkTrmcclus->xca, "xcantrclmc][3]/F");
   m_Tree->Branch("xcb", m_BlkTrmcclus->xcb, "xcbntrclmc][3]/F");
   m_Tree->Branch("xgl", m_BlkTrmcclus->xgl, "xglntrclmc][3]/F");
   m_Tree->Branch("summc", m_BlkTrmcclus->summc, "summcntrclmc]/F");
   m_Tree->Branch("ntrrh", &m_BlkTrrechit->ntrrh, "ntrrh/I");
   m_Tree->Branch("px", m_BlkTrrechit->px, "pxntrrh]/I");
   m_Tree->Branch("py", m_BlkTrrechit->py, "pyntrrh]/I");
   m_Tree->Branch("statusr", m_BlkTrrechit->statusr, "statusrntrrh]/I");
   m_Tree->Branch("Layer", m_BlkTrrechit->Layer, "Layerntrrh]/i");
   m_Tree->Branch("hitr", m_BlkTrrechit->hitr, "hitrntrrh][3]/F");
   m_Tree->Branch("ehitr", m_BlkTrrechit->ehitr, "ehitrntrrh][3]/F");
   m_Tree->Branch("sumr", m_BlkTrrechit->sumr, "sumrntrrh]/F");
   m_Tree->Branch("difosum", m_BlkTrrechit->difosum, "difosumntrrh]/F");
   m_Tree->Branch("ntrtr", &m_BlkTrtrack->ntrtr, "ntrtr/I");
   m_Tree->Branch("trstatus", m_BlkTrtrack->trstatus, "trstatusntrtr]/I");
   m_Tree->Branch("pattern", m_BlkTrtrack->pattern, "patternntrtr]/i");
   m_Tree->Branch("nhits", m_BlkTrtrack->nhits, "nhitsntrtr]/i");
   m_Tree->Branch("phits", m_BlkTrtrack->phits, "phitsntrtr][6]/I");
   m_Tree->Branch("Fastfitdone", m_BlkTrtrack->Fastfitdone, "Fastfitdonentrtr]/i");
   m_Tree->Branch("Geanefitdone", m_BlkTrtrack->Geanefitdone, "Geanefitdonentrtr]/i");
   m_Tree->Branch("Advancedfitdone", m_BlkTrtrack->Advancedfitdone, "Advancedfitdonentrtr]/i");
   m_Tree->Branch("Chi2strline", m_BlkTrtrack->Chi2strline, "Chi2strlinentrtr]/F");
   m_Tree->Branch("Chi2circle", m_BlkTrtrack->Chi2circle, "Chi2circlentrtr]/F");
   m_Tree->Branch("Circleridgidity", m_BlkTrtrack->Circleridgidity, "Circleridgidityntrtr]/F");
   m_Tree->Branch("Chi2fastfit", m_BlkTrtrack->Chi2fastfit, "Chi2fastfitntrtr]/F");
   m_Tree->Branch("Ridgidity", m_BlkTrtrack->Ridgidity, "Ridgidityntrtr]/F");
   m_Tree->Branch("Errridgidity", m_BlkTrtrack->Errridgidity, "Errridgidityntrtr]/F");
   m_Tree->Branch("Theta", m_BlkTrtrack->Theta, "Thetantrtr]/F");
   m_Tree->Branch("phi", m_BlkTrtrack->phi, "phintrtr]/F");
   m_Tree->Branch("p0", m_BlkTrtrack->p0, "p0ntrtr][3]/F");
   m_Tree->Branch("gchi2", m_BlkTrtrack->gchi2, "gchi2ntrtr]/F");
   m_Tree->Branch("gridgidity", m_BlkTrtrack->gridgidity, "gridgidityntrtr]/F");
   m_Tree->Branch("gerrridgidity", m_BlkTrtrack->gerrridgidity, "gerrridgidityntrtr]/F");
   m_Tree->Branch("gtheta", m_BlkTrtrack->gtheta, "gthetantrtr]/F");
   m_Tree->Branch("gphi", m_BlkTrtrack->gphi, "gphintrtr]/F");
   m_Tree->Branch("gp0", m_BlkTrtrack->gp0, "gp0ntrtr][3]/F");
   m_Tree->Branch("hchi2", m_BlkTrtrack->hchi2, "hchi2ntrtr][2]/F");
   m_Tree->Branch("Hridgidity", m_BlkTrtrack->Hridgidity, "Hridgidityntrtr][2]/F");
   m_Tree->Branch("Herrridgidity", m_BlkTrtrack->Herrridgidity, "Herrridgidityntrtr][2]/F");
   m_Tree->Branch("htheta", m_BlkTrtrack->htheta, "hthetantrtr][2]/F");
   m_Tree->Branch("hphi", m_BlkTrtrack->hphi, "hphintrtr][2]/F");
   m_Tree->Branch("hp0", m_BlkTrtrack->hp0, "hp0ntrtr][2][3]/F");
   m_Tree->Branch("fchi2ms", m_BlkTrtrack->fchi2ms, "fchi2msntrtr]/F");
   m_Tree->Branch("gchi2ms", m_BlkTrtrack->gchi2ms, "gchi2msntrtr]/F");
   m_Tree->Branch("ridgidityms", m_BlkTrtrack->ridgidityms, "ridgiditymsntrtr]/F");
   m_Tree->Branch("gridgidityms", m_BlkTrtrack->gridgidityms, "gridgiditymsntrtr]/F");
   m_Tree->Branch("nmcg", &m_BlkMceventg->nmcg, "nmcg/I");
   m_Tree->Branch("nskip", m_BlkMceventg->nskip, "nskipnmcg]/I");
   m_Tree->Branch("Particle", m_BlkMceventg->Particle, "Particlenmcg]/I");
   m_Tree->Branch("coo", m_BlkMceventg->coo, "coonmcg][3]/F");
   m_Tree->Branch("dir", m_BlkMceventg->dir, "dirnmcg][3]/F");
   m_Tree->Branch("momentum", m_BlkMceventg->momentum, "momentumnmcg]/F");
   m_Tree->Branch("mass", m_BlkMceventg->mass, "massnmcg]/F");
   m_Tree->Branch("charge", m_BlkMceventg->charge, "chargenmcg]/F");
   m_Tree->Branch("nctccl", &m_BlkCtcclust->nctccl, "nctccl/I");
   m_Tree->Branch("Ctcstatus", m_BlkCtcclust->Ctcstatus, "Ctcstatusnctccl]/I");
   m_Tree->Branch("Ctclayer", m_BlkCtcclust->Ctclayer, "Ctclayernctccl]/i");
   m_Tree->Branch("ctccoo", m_BlkCtcclust->ctccoo, "ctccoonctccl][3]/F");
   m_Tree->Branch("ctcercoo", m_BlkCtcclust->ctcercoo, "ctcercoonctccl][3]/F");
   m_Tree->Branch("ctcrawsignal", m_BlkCtcclust->ctcrawsignal, "ctcrawsignalnctccl]/F");
   m_Tree->Branch("ctcsignal", m_BlkCtcclust->ctcsignal, "ctcsignalnctccl]/F");
   m_Tree->Branch("ctcesignal", m_BlkCtcclust->ctcesignal, "ctcesignalnctccl]/F");
   m_Tree->Branch("nctcclmc", &m_BlkCtcmcclu->nctcclmc, "nctcclmc/I");
   m_Tree->Branch("Ctcmcidsoft", m_BlkCtcmcclu->Ctcmcidsoft, "Ctcmcidsoftnctcclmc]/I");
   m_Tree->Branch("Ctcmcxcoo", m_BlkCtcmcclu->Ctcmcxcoo, "Ctcmcxcoonctcclmc][3]/F");
   m_Tree->Branch("Ctcmcxdir", m_BlkCtcmcclu->Ctcmcxdir, "Ctcmcxdirnctcclmc][3]/F");
   m_Tree->Branch("Ctcstep", m_BlkCtcmcclu->Ctcstep, "Ctcstepnctcclmc]/F");
   m_Tree->Branch("ctccharge", m_BlkCtcmcclu->ctccharge, "ctcchargenctcclmc]/F");
   m_Tree->Branch("ctcbeta", m_BlkCtcmcclu->ctcbeta, "ctcbetanctcclmc]/F");
   m_Tree->Branch("ctcedep", m_BlkCtcmcclu->ctcedep, "ctcedepnctcclmc]/F");
   m_Tree->Branch("nanti", &m_BlkAnticlus->nanti, "nanti/I");
   m_Tree->Branch("Antistatus", m_BlkAnticlus->Antistatus, "Antistatusnanti]/I");
   m_Tree->Branch("Antisector", m_BlkAnticlus->Antisector, "Antisectornanti]/i");
   m_Tree->Branch("Antiedep", m_BlkAnticlus->Antiedep, "Antiedepnanti]/F");
   m_Tree->Branch("Anticoo", m_BlkAnticlus->Anticoo, "Anticoonanti][3]/F");
   m_Tree->Branch("Antiercoo", m_BlkAnticlus->Antiercoo, "Antiercoonanti][3]/F");
   m_Tree->Branch("nantimc", &m_BlkAntimccl->nantimc, "nantimc/I");
   m_Tree->Branch("Antimcidsoft", m_BlkAntimccl->Antimcidsoft, "Antimcidsoftnantimc]/I");
   m_Tree->Branch("Antimcxcoo", m_BlkAntimccl->Antimcxcoo, "Antimcxcoonantimc][3]/F");
   m_Tree->Branch("Antimctof", m_BlkAntimccl->Antimctof, "Antimctofnantimc]/F");
   m_Tree->Branch("Antimcedep", m_BlkAntimccl->Antimcedep, "Antimcedepnantimc]/F");
   m_Tree->Branch("nlvl3", &m_BlkLvl3->nlvl3, "nlvl3/I");
   m_Tree->Branch("Lvl3toftr", m_BlkLvl3->Lvl3toftr, "Lvl3toftrnlvl3]/i");
   m_Tree->Branch("Lvl3antitr", m_BlkLvl3->Lvl3antitr, "Lvl3antitrnlvl3]/i");
   m_Tree->Branch("Lvl3trackertr", m_BlkLvl3->Lvl3trackertr, "Lvl3trackertrnlvl3]/I");
   m_Tree->Branch("Lvl3ntrhits", m_BlkLvl3->Lvl3ntrhits, "Lvl3ntrhitsnlvl3]/i");
   m_Tree->Branch("Lvl3npat", m_BlkLvl3->Lvl3npat, "Lvl3npatnlvl3]/i");
   m_Tree->Branch("Lvl3pattern", m_BlkLvl3->Lvl3pattern, "Lvl3patternnlvl3][2]/I");
   m_Tree->Branch("Lvl3residual", m_BlkLvl3->Lvl3residual, "Lvl3residualnlvl3][2]/F");
   m_Tree->Branch("Lvl3time", m_BlkLvl3->Lvl3time, "Lvl3timenlvl3]/F");
   m_Tree->Branch("Lvl3eloss", m_BlkLvl3->Lvl3eloss, "Lvl3elossnlvl3]/F");
   m_Tree->Branch("nlvl1", &m_BlkLvl1->nlvl1, "nlvl1/I");
   m_Tree->Branch("Lvl1lifetime", m_BlkLvl1->Lvl1lifetime, "Lvl1lifetimenlvl1]/i");
   m_Tree->Branch("Lvl1flag", m_BlkLvl1->Lvl1flag, "Lvl1flagnlvl1]/I");
   m_Tree->Branch("Lvl1tofpatt", m_BlkLvl1->Lvl1tofpatt, "Lvl1tofpattnlvl1][4]/I");
   m_Tree->Branch("Lvl1tofpatt1", m_BlkLvl1->Lvl1tofpatt1, "Lvl1tofpatt1nlvl1][4]/I");
   m_Tree->Branch("Lvl1antipatt", m_BlkLvl1->Lvl1antipatt, "Lvl1antipattnlvl1]/I");
   m_Tree->Branch("nctcht", &m_BlkCtchit->nctcht, "nctcht/I");
   m_Tree->Branch("Ctchitstatus", m_BlkCtchit->Ctchitstatus, "Ctchitstatusnctcht]/I");
   m_Tree->Branch("Ctchitlayer", m_BlkCtchit->Ctchitlayer, "Ctchitlayernctcht]/i");
   m_Tree->Branch("ctchitcolumn", m_BlkCtchit->ctchitcolumn, "ctchitcolumnnctcht]/i");
   m_Tree->Branch("ctchitrow", m_BlkCtchit->ctchitrow, "ctchitrownctcht]/i");
   m_Tree->Branch("ctchitsignal", m_BlkCtchit->ctchitsignal, "ctchitsignalnctcht]/F");
   m_Tree->Branch("ntrraw", &m_BlkTrrawcl->ntrraw, "ntrraw/I");
   m_Tree->Branch("rawaddress", m_BlkTrrawcl->rawaddress, "rawaddressntrraw]/I");
   m_Tree->Branch("rawlength", m_BlkTrrawcl->rawlength, "rawlengthntrraw]/I");
   m_Tree->Branch("s2n", m_BlkTrrawcl->s2n, "s2nntrraw]/F");
   m_Tree->Branch("nantiraw", &m_BlkAntirawc->nantiraw, "nantiraw/I");
   m_Tree->Branch("antirawstatus", m_BlkAntirawc->antirawstatus, "antirawstatusnantiraw]/I");
   m_Tree->Branch("antirawsector", m_BlkAntirawc->antirawsector, "antirawsectornantiraw]/i");
   m_Tree->Branch("antirawupdown", m_BlkAntirawc->antirawupdown, "antirawupdownnantiraw]/i");
   m_Tree->Branch("antirawsignal", m_BlkAntirawc->antirawsignal, "antirawsignalnantiraw]/F");
   m_Tree->Branch("ntofraw", &m_BlkTofrawcl->ntofraw, "ntofraw/I");
   m_Tree->Branch("tofrstatus", m_BlkTofrawcl->tofrstatus, "tofrstatusntofraw]/I");
   m_Tree->Branch("tofrplane", m_BlkTofrawcl->tofrplane, "tofrplanentofraw]/i");
   m_Tree->Branch("tofrbar", m_BlkTofrawcl->tofrbar, "tofrbarntofraw]/i");
   m_Tree->Branch("tofrtovta", m_BlkTofrawcl->tofrtovta, "tofrtovtantofraw][2]/F");
   m_Tree->Branch("tofrtovtd", m_BlkTofrawcl->tofrtovtd, "tofrtovtdntofraw][2]/F");
   m_Tree->Branch("tofrsdtm", m_BlkTofrawcl->tofrsdtm, "tofrsdtmntofraw][2]/F");
}

//_____________________________________________________________________________
void AMSR_Ntuple::GetEvent(Int_t event)
{
   //
   //Get one event for Ntuple or Tree according to its data type
   //
   if (event<0 || event>=m_Entries) return;
   
   if (m_DataFileType == kRootFile) m_Tree->GetEvent(event);

   else if (m_DataFileType == kNtupleFile) {

      event++;
      int ierr;
      if (m_SameRead) HGNTF(m_MemID, event, ierr);
      else {
         if (m_Tree->GetListOfActiveBranches() == 0)
            HGNT(m_MemID, event, ierr);
         else {
            SetVarNames();
            HGNTV(m_MemID, m_VarNames, m_NVar, event, ierr);
//	    for (int i=0; i<m_NVar; i++) 
//	      printf("m_VarNames[%d]=%s\n",i,m_VarNames[i]);
         }
      }
      m_SameRead = kTRUE;

   }

}

//_____________________________________________________________________________
void AMSR_Ntuple::GetRunTime(time_t* time)
{
  time[0] = m_BlkEventh->time[0];
  time[1] = m_BlkEventh->time[1];
}

//_____________________________________________________________________________
Int_t AMSR_Ntuple::OpenNtuple(char *ntpfile)
{
  //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  //
  //  Return values:
  //    0       successful
  //    1       can't open file
  //    2       can't get the specified ntuple
  //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
   
   int reclen = 0;
   int istat, ierr;
   int offset = 0;
   int lun, memID;
   char *top = 0;

   if (m_Lun == 81) {
      lun = 82;
      top = top2;
      offset = 2-m_NextID;
   } else {
      lun = 81;
      top = top1;
      offset = 1-m_NextID;
   }

   HROPEN(lun, top, ntpfile, " ", reclen, istat);
   if (istat != 0) {
      Error("AMSR_Ntuple::OpenNtuple",
            "Failure in opening ntuple file %s\n",ntpfile);
      CLOSE(lun);
      return 1;
   }

   if (top==top1) HCDIR(dir1, " ");
   else if (top==top2) HCDIR(dir2, " ");

   HRIN(m_NextID, 9999, offset);
   memID = m_NextID+offset;
   istat = EXIST(memID);
   if (istat != 0) {
      Error("AMSR_Ntuple::OpenNtuple",
            "No such ntuple ID =%d\n",m_NextID);
      HREND(top);
      CLOSE(lun);
      //
      //Fails, so resume the previous directory
      //
      if (m_NtupleOpen) {
         if (top==top1) HCDIR(dir2, " ");
         else if (top==top2) HCDIR(dir1, " ");
      }
      return 2;
   }

   //
   //then close the old one after successful opening of new ntuple file
   //
   if (m_NtupleOpen) CloseNtuple();

   //
   //Save current I/O unit, NtupleID, and set m_NtupleOpen
   //Change directory, etc.
   //
   if (top==top1) HCDIR(dir1, " ");
   else if (top==top2) HCDIR(dir2, " ");
   m_NtupleOpen = kTRUE;
   m_Lun        = lun;
   m_MemID      = memID;
   m_NtupleID   = m_NextID;

   //
   //Get entries in the ntuple
   //
   HNOENT(m_MemID, m_Entries);

//   printf("ntuple of %d(MemID) has %d events\n", m_MemID, m_Entries);
   
   //
   //Set block addresses for ntuple
   //
   HBNAME(m_MemID, " ", 0, "$CLEAR");

   HBNAME(m_MemID, "EVENTH", &(m_BlkEventh->eventno), "$SET");
   HBNAME(m_MemID, "BETA", &(m_BlkBeta->nbeta), "$SET");
   HBNAME(m_MemID, "CHARGE", &(m_BlkCharge->ncharge), "$SET");
   HBNAME(m_MemID, "PARTICLE", &(m_BlkParticle->npart), "$SET");
   HBNAME(m_MemID, "TOFCLUST", &(m_BlkTofclust->ntof), "$SET");
   HBNAME(m_MemID, "TOFMCCLU", &(m_BlkTofmcclu->ntofmc), "$SET");
   HBNAME(m_MemID, "TRCLUSTE", &(m_BlkTrcluste->Ntrcl), "$SET");
   HBNAME(m_MemID, "TRMCCLUS", &(m_BlkTrmcclus->ntrclmc), "$SET");
   HBNAME(m_MemID, "TRRECHIT", &(m_BlkTrrechit->ntrrh), "$SET");
   HBNAME(m_MemID, "TRTRACK", &(m_BlkTrtrack->ntrtr), "$SET");
   HBNAME(m_MemID, "MCEVENTG", &(m_BlkMceventg->nmcg), "$SET");
   HBNAME(m_MemID, "CTCCLUST", &(m_BlkCtcclust->nctccl), "$SET");
   HBNAME(m_MemID, "CTCMCCLU", &(m_BlkCtcmcclu->nctcclmc), "$SET");
   HBNAME(m_MemID, "ANTICLUS", &(m_BlkAnticlus->nanti), "$SET");
   HBNAME(m_MemID, "ANTIMCCL", &(m_BlkAntimccl->nantimc), "$SET");
   HBNAME(m_MemID, "LVL3", &(m_BlkLvl3->nlvl3), "$SET");
   HBNAME(m_MemID, "LVL1", &(m_BlkLvl1->nlvl1), "$SET");
   HBNAME(m_MemID, "CTCHIT", &(m_BlkCtchit->nctcht), "$SET");
   HBNAME(m_MemID, "TRRAWCL", &(m_BlkTrrawcl->ntrraw), "$SET");
   HBNAME(m_MemID, "ANTIRAWC", &(m_BlkAntirawc->nantiraw), "$SET");
   HBNAME(m_MemID, "TOFRAWCL", &(m_BlkTofrawcl->ntofraw), "$SET");


   //
   //Need to invoke HGNT to enable successful calling of HGNTV
   //
   HGNT(m_MemID, 1, ierr);
   m_SameRead   = kFALSE;

   SetTree(0);
   
   return 0;
}

//_____________________________________________________________________________
void AMSR_Ntuple::SetBranchStatus(char *varname, Bool_t status)
{
   m_Tree->SetBranchStatus(varname,status);
   if (m_DataFileType == kNtupleFile) m_SameRead = kFALSE;
}

//_____________________________________________________________________________
void AMSR_Ntuple::SetTree(TTree *t)
{
   //
   //Ntuple data file and Root Tree data file
   //

   if (t == 0) {      //Ntuple data file

//      if (m_DataFileType != kNtupleFile || m_Tree == 0) CreateSampleTree();
      m_Tree = m_SampleTree;
      m_DataFileType = kNtupleFile;

   } else {           //Root Tree data file

      m_Tree    = t;
      m_Entries = m_Tree->GetEntries();
      SetTreeAddress();
      m_DataFileType = kRootFile;

      //
      //Read the first event for all branches to ensure late selected read
      //
      m_Tree->GetEvent(0);
   }

}

//_____________________________________________________________________________
void AMSR_Ntuple::SetTreeAddress()
{
   //
   //Create association between ntuple and tree
   //

   if (m_Tree == 0) return;
   
   //
   //Set branch addresses
   //
   m_Tree->SetBranchAddress("eventno", &m_BlkEventh->eventno);
   m_Tree->SetBranchAddress("run", &m_BlkEventh->run);
   m_Tree->SetBranchAddress("runtype", &m_BlkEventh->runtype);
   m_Tree->SetBranchAddress("time", m_BlkEventh->time);
   m_Tree->SetBranchAddress("Rawwords", &m_BlkEventh->Rawwords);
   m_Tree->SetBranchAddress("Rads", &m_BlkEventh->Rads);
   m_Tree->SetBranchAddress("Thetas", &m_BlkEventh->Thetas);
   m_Tree->SetBranchAddress("Phis", &m_BlkEventh->Phis);
   m_Tree->SetBranchAddress("Yaws", &m_BlkEventh->Yaws);
   m_Tree->SetBranchAddress("Pitchs", &m_BlkEventh->Pitchs);
   m_Tree->SetBranchAddress("Rolls", &m_BlkEventh->Rolls);
   m_Tree->SetBranchAddress("Velocitys", &m_BlkEventh->Velocitys);
   m_Tree->SetBranchAddress("Veltheta", &m_BlkEventh->Veltheta);
   m_Tree->SetBranchAddress("Velphi", &m_BlkEventh->Velphi);
   m_Tree->SetBranchAddress("Particles", &m_BlkEventh->Particles);
   m_Tree->SetBranchAddress("Tracks", &m_BlkEventh->Tracks);
   m_Tree->SetBranchAddress("Betas", &m_BlkEventh->Betas);
   m_Tree->SetBranchAddress("Charges", &m_BlkEventh->Charges);
   m_Tree->SetBranchAddress("Trrechits", &m_BlkEventh->Trrechits);
   m_Tree->SetBranchAddress("Trclusters", &m_BlkEventh->Trclusters);
   m_Tree->SetBranchAddress("Trrawclusters", &m_BlkEventh->Trrawclusters);
   m_Tree->SetBranchAddress("Trmcclusters", &m_BlkEventh->Trmcclusters);
   m_Tree->SetBranchAddress("Tofclusters", &m_BlkEventh->Tofclusters);
   m_Tree->SetBranchAddress("Tofmcclusters", &m_BlkEventh->Tofmcclusters);
   m_Tree->SetBranchAddress("Ctcclusters", &m_BlkEventh->Ctcclusters);
   m_Tree->SetBranchAddress("Ctcmcclusters", &m_BlkEventh->Ctcmcclusters);
   m_Tree->SetBranchAddress("Antimcclusters", &m_BlkEventh->Antimcclusters);
   m_Tree->SetBranchAddress("Anticlusters", &m_BlkEventh->Anticlusters);
   m_Tree->SetBranchAddress("Eventstatus", &m_BlkEventh->Eventstatus);
   m_Tree->SetBranchAddress("nbeta", &m_BlkBeta->nbeta);
   m_Tree->SetBranchAddress("betastatus", m_BlkBeta->betastatus);
   m_Tree->SetBranchAddress("betapattern", m_BlkBeta->betapattern);
   m_Tree->SetBranchAddress("beta", m_BlkBeta->beta);
   m_Tree->SetBranchAddress("betaerror", m_BlkBeta->betaerror);
   m_Tree->SetBranchAddress("betachi2", m_BlkBeta->betachi2);
   m_Tree->SetBranchAddress("betachi2s", m_BlkBeta->betachi2s);
   m_Tree->SetBranchAddress("betantof", m_BlkBeta->betantof);
   m_Tree->SetBranchAddress("betaptof", m_BlkBeta->betaptof);
   m_Tree->SetBranchAddress("betaptr", m_BlkBeta->betaptr);
   m_Tree->SetBranchAddress("ncharge", &m_BlkCharge->ncharge);
   m_Tree->SetBranchAddress("chargestatus", m_BlkCharge->chargestatus);
   m_Tree->SetBranchAddress("chargebetap", m_BlkCharge->chargebetap);
   m_Tree->SetBranchAddress("chargetof", m_BlkCharge->chargetof);
   m_Tree->SetBranchAddress("chargetracker", m_BlkCharge->chargetracker);
   m_Tree->SetBranchAddress("probtof", m_BlkCharge->probtof);
   m_Tree->SetBranchAddress("probtracker", m_BlkCharge->probtracker);
   m_Tree->SetBranchAddress("truntof", m_BlkCharge->truntof);
   m_Tree->SetBranchAddress("truntracker", m_BlkCharge->truntracker);
   m_Tree->SetBranchAddress("npart", &m_BlkParticle->npart);
   m_Tree->SetBranchAddress("pctcp", m_BlkParticle->pctcp);
   m_Tree->SetBranchAddress("pbetap", m_BlkParticle->pbetap);
   m_Tree->SetBranchAddress("pchargep", m_BlkParticle->pchargep);
   m_Tree->SetBranchAddress("ptrackp", m_BlkParticle->ptrackp);
   m_Tree->SetBranchAddress("pid", m_BlkParticle->pid);
   m_Tree->SetBranchAddress("pmass", m_BlkParticle->pmass);
   m_Tree->SetBranchAddress("perrmass", m_BlkParticle->perrmass);
   m_Tree->SetBranchAddress("pmom", m_BlkParticle->pmom);
   m_Tree->SetBranchAddress("perrmom", m_BlkParticle->perrmom);
   m_Tree->SetBranchAddress("pcharge", m_BlkParticle->pcharge);
   m_Tree->SetBranchAddress("ptheta", m_BlkParticle->ptheta);
   m_Tree->SetBranchAddress("pphi", m_BlkParticle->pphi);
   m_Tree->SetBranchAddress("thetagl", m_BlkParticle->thetagl);
   m_Tree->SetBranchAddress("phigl", m_BlkParticle->phigl);
   m_Tree->SetBranchAddress("pcoo", m_BlkParticle->pcoo);
   m_Tree->SetBranchAddress("signalctc", m_BlkParticle->signalctc);
   m_Tree->SetBranchAddress("betactc", m_BlkParticle->betactc);
   m_Tree->SetBranchAddress("errorbetactc", m_BlkParticle->errorbetactc);
   m_Tree->SetBranchAddress("cooctc", m_BlkParticle->cooctc);
   m_Tree->SetBranchAddress("cootof", m_BlkParticle->cootof);
   m_Tree->SetBranchAddress("cooanti", m_BlkParticle->cooanti);
   m_Tree->SetBranchAddress("cootr", m_BlkParticle->cootr);
   m_Tree->SetBranchAddress("ntof", &m_BlkTofclust->ntof);
   m_Tree->SetBranchAddress("Tofstatus", m_BlkTofclust->Tofstatus);
   m_Tree->SetBranchAddress("plane", m_BlkTofclust->plane);
   m_Tree->SetBranchAddress("bar", m_BlkTofclust->bar);
   m_Tree->SetBranchAddress("Tofedep", m_BlkTofclust->Tofedep);
   m_Tree->SetBranchAddress("Toftime", m_BlkTofclust->Toftime);
   m_Tree->SetBranchAddress("Tofetime", m_BlkTofclust->Tofetime);
   m_Tree->SetBranchAddress("Tofcoo", m_BlkTofclust->Tofcoo);
   m_Tree->SetBranchAddress("Tofercoo", m_BlkTofclust->Tofercoo);
   m_Tree->SetBranchAddress("ntofmc", &m_BlkTofmcclu->ntofmc);
   m_Tree->SetBranchAddress("Tofmcidsoft", m_BlkTofmcclu->Tofmcidsoft);
   m_Tree->SetBranchAddress("Tofmcxcoo", m_BlkTofmcclu->Tofmcxcoo);
   m_Tree->SetBranchAddress("Tofmctof", m_BlkTofmcclu->Tofmctof);
   m_Tree->SetBranchAddress("Tofmcedep", m_BlkTofmcclu->Tofmcedep);
   m_Tree->SetBranchAddress("Ntrcl", &m_BlkTrcluste->Ntrcl);
   m_Tree->SetBranchAddress("Idsoft", m_BlkTrcluste->Idsoft);
   m_Tree->SetBranchAddress("Statust", m_BlkTrcluste->Statust);
   m_Tree->SetBranchAddress("Neleml", m_BlkTrcluste->Neleml);
   m_Tree->SetBranchAddress("Nelemr", m_BlkTrcluste->Nelemr);
   m_Tree->SetBranchAddress("Sumt", m_BlkTrcluste->Sumt);
   m_Tree->SetBranchAddress("Sigmat", m_BlkTrcluste->Sigmat);
   m_Tree->SetBranchAddress("Meant", m_BlkTrcluste->Meant);
   m_Tree->SetBranchAddress("Rmst", m_BlkTrcluste->Rmst);
   m_Tree->SetBranchAddress("Errormeant", m_BlkTrcluste->Errormeant);
   m_Tree->SetBranchAddress("Amplitude", m_BlkTrcluste->Amplitude);
   m_Tree->SetBranchAddress("ntrclmc", &m_BlkTrmcclus->ntrclmc);
   m_Tree->SetBranchAddress("Idsoftmc", m_BlkTrmcclus->Idsoftmc);
   m_Tree->SetBranchAddress("Itra", m_BlkTrmcclus->Itra);
   m_Tree->SetBranchAddress("Left", m_BlkTrmcclus->Left);
   m_Tree->SetBranchAddress("Center", m_BlkTrmcclus->Center);
   m_Tree->SetBranchAddress("Right", m_BlkTrmcclus->Right);
   m_Tree->SetBranchAddress("ss", m_BlkTrmcclus->ss);
   m_Tree->SetBranchAddress("xca", m_BlkTrmcclus->xca);
   m_Tree->SetBranchAddress("xcb", m_BlkTrmcclus->xcb);
   m_Tree->SetBranchAddress("xgl", m_BlkTrmcclus->xgl);
   m_Tree->SetBranchAddress("summc", m_BlkTrmcclus->summc);
   m_Tree->SetBranchAddress("ntrrh", &m_BlkTrrechit->ntrrh);
   m_Tree->SetBranchAddress("px", m_BlkTrrechit->px);
   m_Tree->SetBranchAddress("py", m_BlkTrrechit->py);
   m_Tree->SetBranchAddress("statusr", m_BlkTrrechit->statusr);
   m_Tree->SetBranchAddress("Layer", m_BlkTrrechit->Layer);
   m_Tree->SetBranchAddress("hitr", m_BlkTrrechit->hitr);
   m_Tree->SetBranchAddress("ehitr", m_BlkTrrechit->ehitr);
   m_Tree->SetBranchAddress("sumr", m_BlkTrrechit->sumr);
   m_Tree->SetBranchAddress("difosum", m_BlkTrrechit->difosum);
   m_Tree->SetBranchAddress("ntrtr", &m_BlkTrtrack->ntrtr);
   m_Tree->SetBranchAddress("trstatus", m_BlkTrtrack->trstatus);
   m_Tree->SetBranchAddress("pattern", m_BlkTrtrack->pattern);
   m_Tree->SetBranchAddress("nhits", m_BlkTrtrack->nhits);
   m_Tree->SetBranchAddress("phits", m_BlkTrtrack->phits);
   m_Tree->SetBranchAddress("Fastfitdone", m_BlkTrtrack->Fastfitdone);
   m_Tree->SetBranchAddress("Geanefitdone", m_BlkTrtrack->Geanefitdone);
   m_Tree->SetBranchAddress("Advancedfitdone", m_BlkTrtrack->Advancedfitdone);
   m_Tree->SetBranchAddress("Chi2strline", m_BlkTrtrack->Chi2strline);
   m_Tree->SetBranchAddress("Chi2circle", m_BlkTrtrack->Chi2circle);
   m_Tree->SetBranchAddress("Circleridgidity", m_BlkTrtrack->Circleridgidity);
   m_Tree->SetBranchAddress("Chi2fastfit", m_BlkTrtrack->Chi2fastfit);
   m_Tree->SetBranchAddress("Ridgidity", m_BlkTrtrack->Ridgidity);
   m_Tree->SetBranchAddress("Errridgidity", m_BlkTrtrack->Errridgidity);
   m_Tree->SetBranchAddress("Theta", m_BlkTrtrack->Theta);
   m_Tree->SetBranchAddress("phi", m_BlkTrtrack->phi);
   m_Tree->SetBranchAddress("p0", m_BlkTrtrack->p0);
   m_Tree->SetBranchAddress("gchi2", m_BlkTrtrack->gchi2);
   m_Tree->SetBranchAddress("gridgidity", m_BlkTrtrack->gridgidity);
   m_Tree->SetBranchAddress("gerrridgidity", m_BlkTrtrack->gerrridgidity);
   m_Tree->SetBranchAddress("gtheta", m_BlkTrtrack->gtheta);
   m_Tree->SetBranchAddress("gphi", m_BlkTrtrack->gphi);
   m_Tree->SetBranchAddress("gp0", m_BlkTrtrack->gp0);
   m_Tree->SetBranchAddress("hchi2", m_BlkTrtrack->hchi2);
   m_Tree->SetBranchAddress("Hridgidity", m_BlkTrtrack->Hridgidity);
   m_Tree->SetBranchAddress("Herrridgidity", m_BlkTrtrack->Herrridgidity);
   m_Tree->SetBranchAddress("htheta", m_BlkTrtrack->htheta);
   m_Tree->SetBranchAddress("hphi", m_BlkTrtrack->hphi);
   m_Tree->SetBranchAddress("hp0", m_BlkTrtrack->hp0);
   m_Tree->SetBranchAddress("fchi2ms", m_BlkTrtrack->fchi2ms);
   m_Tree->SetBranchAddress("gchi2ms", m_BlkTrtrack->gchi2ms);
   m_Tree->SetBranchAddress("ridgidityms", m_BlkTrtrack->ridgidityms);
   m_Tree->SetBranchAddress("gridgidityms", m_BlkTrtrack->gridgidityms);
   m_Tree->SetBranchAddress("nmcg", &m_BlkMceventg->nmcg);
   m_Tree->SetBranchAddress("nskip", m_BlkMceventg->nskip);
   m_Tree->SetBranchAddress("Particle", m_BlkMceventg->Particle);
   m_Tree->SetBranchAddress("coo", m_BlkMceventg->coo);
   m_Tree->SetBranchAddress("dir", m_BlkMceventg->dir);
   m_Tree->SetBranchAddress("momentum", m_BlkMceventg->momentum);
   m_Tree->SetBranchAddress("mass", m_BlkMceventg->mass);
   m_Tree->SetBranchAddress("charge", m_BlkMceventg->charge);
   m_Tree->SetBranchAddress("nctccl", &m_BlkCtcclust->nctccl);
   m_Tree->SetBranchAddress("Ctcstatus", m_BlkCtcclust->Ctcstatus);
   m_Tree->SetBranchAddress("Ctclayer", m_BlkCtcclust->Ctclayer);
   m_Tree->SetBranchAddress("ctccoo", m_BlkCtcclust->ctccoo);
   m_Tree->SetBranchAddress("ctcercoo", m_BlkCtcclust->ctcercoo);
   m_Tree->SetBranchAddress("ctcrawsignal", m_BlkCtcclust->ctcrawsignal);
   m_Tree->SetBranchAddress("ctcsignal", m_BlkCtcclust->ctcsignal);
   m_Tree->SetBranchAddress("ctcesignal", m_BlkCtcclust->ctcesignal);
   m_Tree->SetBranchAddress("nctcclmc", &m_BlkCtcmcclu->nctcclmc);
   m_Tree->SetBranchAddress("Ctcmcidsoft", m_BlkCtcmcclu->Ctcmcidsoft);
   m_Tree->SetBranchAddress("Ctcmcxcoo", m_BlkCtcmcclu->Ctcmcxcoo);
   m_Tree->SetBranchAddress("Ctcmcxdir", m_BlkCtcmcclu->Ctcmcxdir);
   m_Tree->SetBranchAddress("Ctcstep", m_BlkCtcmcclu->Ctcstep);
   m_Tree->SetBranchAddress("ctccharge", m_BlkCtcmcclu->ctccharge);
   m_Tree->SetBranchAddress("ctcbeta", m_BlkCtcmcclu->ctcbeta);
   m_Tree->SetBranchAddress("ctcedep", m_BlkCtcmcclu->ctcedep);
   m_Tree->SetBranchAddress("nanti", &m_BlkAnticlus->nanti);
   m_Tree->SetBranchAddress("Antistatus", m_BlkAnticlus->Antistatus);
   m_Tree->SetBranchAddress("Antisector", m_BlkAnticlus->Antisector);
   m_Tree->SetBranchAddress("Antiedep", m_BlkAnticlus->Antiedep);
   m_Tree->SetBranchAddress("Anticoo", m_BlkAnticlus->Anticoo);
   m_Tree->SetBranchAddress("Antiercoo", m_BlkAnticlus->Antiercoo);
   m_Tree->SetBranchAddress("nantimc", &m_BlkAntimccl->nantimc);
   m_Tree->SetBranchAddress("Antimcidsoft", m_BlkAntimccl->Antimcidsoft);
   m_Tree->SetBranchAddress("Antimcxcoo", m_BlkAntimccl->Antimcxcoo);
   m_Tree->SetBranchAddress("Antimctof", m_BlkAntimccl->Antimctof);
   m_Tree->SetBranchAddress("Antimcedep", m_BlkAntimccl->Antimcedep);
   m_Tree->SetBranchAddress("nlvl3", &m_BlkLvl3->nlvl3);
   m_Tree->SetBranchAddress("Lvl3toftr", m_BlkLvl3->Lvl3toftr);
   m_Tree->SetBranchAddress("Lvl3antitr", m_BlkLvl3->Lvl3antitr);
   m_Tree->SetBranchAddress("Lvl3trackertr", m_BlkLvl3->Lvl3trackertr);
   m_Tree->SetBranchAddress("Lvl3ntrhits", m_BlkLvl3->Lvl3ntrhits);
   m_Tree->SetBranchAddress("Lvl3npat", m_BlkLvl3->Lvl3npat);
   m_Tree->SetBranchAddress("Lvl3pattern", m_BlkLvl3->Lvl3pattern);
   m_Tree->SetBranchAddress("Lvl3residual", m_BlkLvl3->Lvl3residual);
   m_Tree->SetBranchAddress("Lvl3time", m_BlkLvl3->Lvl3time);
   m_Tree->SetBranchAddress("Lvl3eloss", m_BlkLvl3->Lvl3eloss);
   m_Tree->SetBranchAddress("nlvl1", &m_BlkLvl1->nlvl1);
   m_Tree->SetBranchAddress("Lvl1lifetime", m_BlkLvl1->Lvl1lifetime);
   m_Tree->SetBranchAddress("Lvl1flag", m_BlkLvl1->Lvl1flag);
   m_Tree->SetBranchAddress("Lvl1tofpatt", m_BlkLvl1->Lvl1tofpatt);
   m_Tree->SetBranchAddress("Lvl1tofpatt1", m_BlkLvl1->Lvl1tofpatt1);
   m_Tree->SetBranchAddress("Lvl1antipatt", m_BlkLvl1->Lvl1antipatt);
   m_Tree->SetBranchAddress("nctcht", &m_BlkCtchit->nctcht);
   m_Tree->SetBranchAddress("Ctchitstatus", m_BlkCtchit->Ctchitstatus);
   m_Tree->SetBranchAddress("Ctchitlayer", m_BlkCtchit->Ctchitlayer);
   m_Tree->SetBranchAddress("ctchitcolumn", m_BlkCtchit->ctchitcolumn);
   m_Tree->SetBranchAddress("ctchitrow", m_BlkCtchit->ctchitrow);
   m_Tree->SetBranchAddress("ctchitsignal", m_BlkCtchit->ctchitsignal);
   m_Tree->SetBranchAddress("ntrraw", &m_BlkTrrawcl->ntrraw);
   m_Tree->SetBranchAddress("rawaddress", m_BlkTrrawcl->rawaddress);
   m_Tree->SetBranchAddress("rawlength", m_BlkTrrawcl->rawlength);
   m_Tree->SetBranchAddress("s2n", m_BlkTrrawcl->s2n);
   m_Tree->SetBranchAddress("nantiraw", &m_BlkAntirawc->nantiraw);
   m_Tree->SetBranchAddress("antirawstatus", m_BlkAntirawc->antirawstatus);
   m_Tree->SetBranchAddress("antirawsector", m_BlkAntirawc->antirawsector);
   m_Tree->SetBranchAddress("antirawupdown", m_BlkAntirawc->antirawupdown);
   m_Tree->SetBranchAddress("antirawsignal", m_BlkAntirawc->antirawsignal);
   m_Tree->SetBranchAddress("ntofraw", &m_BlkTofrawcl->ntofraw);
   m_Tree->SetBranchAddress("tofrstatus", m_BlkTofrawcl->tofrstatus);
   m_Tree->SetBranchAddress("tofrplane", m_BlkTofrawcl->tofrplane);
   m_Tree->SetBranchAddress("tofrbar", m_BlkTofrawcl->tofrbar);
   m_Tree->SetBranchAddress("tofrtovta", m_BlkTofrawcl->tofrtovta);
   m_Tree->SetBranchAddress("tofrtovtd", m_BlkTofrawcl->tofrtovtd);
   m_Tree->SetBranchAddress("tofrsdtm", m_BlkTofrawcl->tofrsdtm);
}

//_____________________________________________________________________________
void AMSR_Ntuple::SetVarNames()
{
   //
   //Get variable names selected to read, which is used in HGNTV
   //

   TList *list = m_Tree->GetListOfActiveBranches();
   if (list == 0) return;

   int cols = sizeof(m_VarNames[0]);
   int rows = sizeof(m_VarNames)/cols;

   TBranch *branch;
   TObjLink *lnk = list->FirstLink();
   m_NVar = 0;

   while (lnk) {
      if (m_NVar >= rows) {
	 Error("AMSR_Ntuple","index %d out of boud [%d]\n",m_NVar+1,rows);
	 break;
      }
      branch = (TBranch*)lnk->GetObject();
      const char *name = branch->GetName();
      if (strlen(name) > cols) 
         Error("AMSR_Ntuple::SetVarNames","too long(>%d) name %s",cols,name);
      else strncpy(m_VarNames[m_NVar++], name, cols-1);
      lnk = lnk->Next();
   }

}

