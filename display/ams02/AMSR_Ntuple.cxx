//  $Id: AMSR_Ntuple.cxx,v 1.18 2001/08/20 18:55:02 kscholbe Exp $

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
#include "hbook.h"
#include <TTree.h>
//#include <TClonesArray.h>
//#include <TObjString.h>
#include "Debugger.h"
#include <TFile.h>
#include <stdio.h>
#include <iostream.h>

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
TRDCLMC_DEF blkTrdclmc;
TRDCL_DEF blkTrdcl;
TRTRACK_DEF blkTrtrack;
TRDTRK_DEF blkTrdtrk;
ECALSHOWER_DEF blkecsh;
MCEVENTG_DEF blkMceventg;
ANTICLUS_DEF blkAnticlus;
ANTIMCCL_DEF blkAntimccl;
LVL3_DEF blkLvl3;
LVL1_DEF blkLvl1;
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
   m_BlkTrdclmc  = &blkTrdclmc;
   m_BlkTrdcl    = &blkTrdcl;
   m_BlkTrdtrk   = &blkTrdtrk;
   m_BlkTrtrack  = &blkTrtrack;
   m_Blkecsh  =    &blkecsh;
   m_BlkMceventg = &blkMceventg;
   m_BlkAnticlus = &blkAnticlus;
   m_BlkAntimccl = &blkAntimccl;
   m_BlkLvl3     = &blkLvl3;
   m_BlkLvl1     = &blkLvl1;
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

   m_Tree->Branch("eventno", &m_BlkEventh->eventno, "eventno/I");
   m_Tree->Branch("run", &m_BlkEventh->run, "run/I");
   m_Tree->Branch("runtype", &m_BlkEventh->runtype, "runtype/I");
   m_Tree->Branch("time", m_BlkEventh->time, "time[2]/I");
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
   m_Tree->Branch("Thetam", &m_BlkEventh->Thetam, "Thetam/F");
   m_Tree->Branch("Phim", &m_BlkEventh->Phim, "Phim/F");
   m_Tree->Branch("Tracks", &m_BlkEventh->Tracks, "Tracks/i");
   m_Tree->Branch("Trrechits", &m_BlkEventh->Trrechits, "Trrechits/i");
   m_Tree->Branch("Trclusters", &m_BlkEventh->Trclusters, "Trclusters/i");
   m_Tree->Branch("Trrawclusters", &m_BlkEventh->Trrawclusters, "Trrawclusters/i");
   m_Tree->Branch("Trmcclusters", &m_BlkEventh->Trmcclusters, "Trmcclusters/i");
   m_Tree->Branch("Tofclusters", &m_BlkEventh->Tofclusters, "Tofclusters/i");
   m_Tree->Branch("Tofmcclusters", &m_BlkEventh->Tofmcclusters, "Tofmcclusters/i");
   m_Tree->Branch("Antimcclusters", &m_BlkEventh->Antimcclusters, "Antimcclusters/i");
   m_Tree->Branch("Trdmcclusters", &m_BlkEventh->Trdmcclusters, "Trdmcclusters/i");
   m_Tree->Branch("Anticlusters", &m_BlkEventh->Anticlusters, "Anticlusters/i");
   m_Tree->Branch("Ecalclusters", &m_BlkEventh->Ecalclusters, "Ecalclusters/i");
   m_Tree->Branch("Ecalhits", &m_BlkEventh->Ecalhits, "Ecalhits/i");
   m_Tree->Branch("Richmcclusters", &m_BlkEventh->Richmcclusters, "Richmcclusters/i");
   m_Tree->Branch("Richits", &m_BlkEventh->Richits, "Richits/i");
   m_Tree->Branch("TRDRawHits", &m_BlkEventh->TRDRawHits, "TRDRawHits/i");
   m_Tree->Branch("TRDClusters", &m_BlkEventh->TRDClusters, "TRDClusters/i");
   m_Tree->Branch("TRDSegments", &m_BlkEventh->TRDSegments, "TRDSegments/i");
   m_Tree->Branch("TRDTracks", &m_BlkEventh->TRDTracks, "TRDTracks/i");
   m_Tree->Branch("Eventstatus", &m_BlkEventh->Eventstatus, "Eventstatus[2]/i");


   m_Tree->Branch("nbeta", &m_BlkBeta->nbeta, "nbeta/I");
   m_Tree->Branch("betastatus", m_BlkBeta->betastatus, "betastatus[nbeta]/I");
   m_Tree->Branch("betapattern", m_BlkBeta->betapattern, "betapattern[nbeta]/i");
   m_Tree->Branch("beta", m_BlkBeta->beta, "beta[nbeta]/F");
   m_Tree->Branch("betac", m_BlkBeta->betac, "betac[nbeta]/F");
   m_Tree->Branch("betaerror", m_BlkBeta->betaerror, "betaerror[nbeta]/F");
   m_Tree->Branch("betaerrorc", m_BlkBeta->betaerrorc, "betaerrorc[nbeta]/F");
   m_Tree->Branch("betachi2", m_BlkBeta->betachi2, "betachi2[nbeta]/F");
   m_Tree->Branch("betachi2s", m_BlkBeta->betachi2s, "betachi2s[nbeta]/F");
   m_Tree->Branch("betantof", m_BlkBeta->betantof, "betantof[nbeta]/i");
   m_Tree->Branch("betaptof", m_BlkBeta->betaptof, "betaptof[nbeta][4]/I");
   m_Tree->Branch("betaptr", m_BlkBeta->betaptr, "betaptr[nbeta]/I");

   m_Tree->Branch("ncharge", &m_BlkCharge->ncharge, "ncharge/I");
   m_Tree->Branch("chargestatus", m_BlkCharge->chargestatus, "chargestatus[ncharge]/I");
   m_Tree->Branch("chargebetap", m_BlkCharge->chargebetap, "chargebetap[ncharge]/I");
   m_Tree->Branch("chargetof", m_BlkCharge->chargetof, "chargetof[ncharge]/i");
   m_Tree->Branch("chargetracker", m_BlkCharge->chargetracker, "chargetracker[ncharge]/i");
   m_Tree->Branch("probtof", m_BlkCharge->probtof, "probtof[ncharge][4]/F");
   m_Tree->Branch("chintof", m_BlkCharge->chintof, "chintof[ncharge][4]/i");
   m_Tree->Branch("probtracker", m_BlkCharge->probtracker, "probtracker[ncharge][4]/F");
   m_Tree->Branch("chintracker", m_BlkCharge->chintracker, "chintracker[ncharge][4]/i");
   m_Tree->Branch("proballtracker", m_BlkCharge->proballtracker, "proballtracker[ncharge]/F");
   m_Tree->Branch("truntof", m_BlkCharge->truntof, "truntof[ncharge]/F");
   m_Tree->Branch("truntofd", m_BlkCharge->truntofd, "truntofd[ncharge]/F");
   m_Tree->Branch("truntracker", m_BlkCharge->truntracker, "truntracker[ncharge]/F");

   m_Tree->Branch("npart", &m_BlkParticle->npart, "npart/I");
   m_Tree->Branch("pbetap", m_BlkParticle->pbetap, "pbetap[npart]/i");
   m_Tree->Branch("pchargep", m_BlkParticle->pchargep, "pchargep[npart]/I");
   m_Tree->Branch("ptrackp", m_BlkParticle->ptrackp, "ptrackp[npart]/I");
   m_Tree->Branch("ptrdp", m_BlkParticle->ptrd, "ptrdp[npart]/I");
   m_Tree->Branch("prichp", m_BlkParticle->prich, "prichp[npart]/I");
   m_Tree->Branch("pecalp", m_BlkParticle->pecal, "pecalp[npart]/I");
   m_Tree->Branch("pid", m_BlkParticle->pid, "pid[npart]/i");
   m_Tree->Branch("pidvice", m_BlkParticle->pidvice, "pidvice[npart]/i");
   m_Tree->Branch("probpid", m_BlkParticle->probpid, "probpid[npart][2]/F");
   m_Tree->Branch("fitmom", m_BlkParticle->fitmom, "fitmom[npart]/F");
   m_Tree->Branch("pmass", m_BlkParticle->pmass, "pmass[npart]/F");
   m_Tree->Branch("perrmass", m_BlkParticle->perrmass, "perrmass[npart]/F");
   m_Tree->Branch("pmom", m_BlkParticle->pmom, "pmom[npart]/F");
   m_Tree->Branch("perrmom", m_BlkParticle->perrmom, "perrmom[npart]/F");
   m_Tree->Branch("pcharge", m_BlkParticle->pcharge, "pcharge[npart]/F");
   m_Tree->Branch("ptheta", m_BlkParticle->ptheta, "ptheta[npart]/F");
   m_Tree->Branch("pphi", m_BlkParticle->pphi, "pphi[npart]/F");
   m_Tree->Branch("thetagl", m_BlkParticle->thetagl, "thetagl[npart]/F");
   m_Tree->Branch("phigl", m_BlkParticle->phigl, "phigl[npart]/F");
   m_Tree->Branch("pcoo", m_BlkParticle->pcoo, "pcoo[npart][3]/F");
   m_Tree->Branch("cutoff", m_BlkParticle->cutoff, "cutoff[npart]/F");
   m_Tree->Branch("cootof", m_BlkParticle->cootof, "cootof[npart][4][3]/F");
   m_Tree->Branch("cooanti", m_BlkParticle->cooanti, "cooanti[npart][2][3]/F");
   m_Tree->Branch("cooecal", m_BlkParticle->cooecal, "cooecal[npart][3][3]/F");
   m_Tree->Branch("cootr", m_BlkParticle->cootr, "cootr[npart][8][3]/F");
   m_Tree->Branch("cootrd", m_BlkParticle->cootrd, "cootrd[npart][3]/F");

   m_Tree->Branch("ntof", &m_BlkTofclust->ntof, "ntof/I");
   m_Tree->Branch("Tofstatus", m_BlkTofclust->Tofstatus, "Tofstatus[ntof]/I");
   m_Tree->Branch("plane", m_BlkTofclust->plane, "plane[ntof]/i");
   m_Tree->Branch("bar", m_BlkTofclust->bar, "bar[ntof]/i");
   m_Tree->Branch("nmemb", m_BlkTofclust->nmemb, "nmemb[ntof]/i");
   m_Tree->Branch("Tofedep", m_BlkTofclust->Tofedep, "Tofedep[ntof]/F");
   m_Tree->Branch("Tofedepd", m_BlkTofclust->Tofedepd, "Tofedepd[ntof]/F");
   m_Tree->Branch("Toftime", m_BlkTofclust->Toftime, "Toftime[ntof]/F");
   m_Tree->Branch("Tofetime", m_BlkTofclust->Tofetime, "Tofetime[ntof]/F");
   m_Tree->Branch("Tofcoo", m_BlkTofclust->Tofcoo, "Tofcoo[ntof][3]/F");
   m_Tree->Branch("Tofercoo", m_BlkTofclust->Tofercoo, "Tofercoo[ntof][3]/F");

   m_Tree->Branch("ntrdcl", &m_BlkTrdcl->ntrdcl, "ntrdcl/I");
   m_Tree->Branch("Trdclstatus", m_BlkTrdcl->status, "trdclstatus[ntrdcl]/I");
   m_Tree->Branch("Trdclcoo", m_BlkTrdcl->coo, "trdclcoo[3][ntrdcl]/F");
   m_Tree->Branch("Trdcllayer", m_BlkTrdcl->layer, "trdcllayer[ntrdcl]/I");
   m_Tree->Branch("Trdcldir", m_BlkTrdcl->coodir, "trddir[3][ntrdcl]/F");
   m_Tree->Branch("Trdmultip", m_BlkTrdcl->multip, "trdmul[ntrdcl]/I");
   m_Tree->Branch("Trdhmultip", m_BlkTrdcl->hmultip, "trdhmul[ntrdcl]/I");
   m_Tree->Branch("Trdedep", m_BlkTrdcl->edep, "trdedep[ntrdcl]/F");
   m_Tree->Branch("Trdprawhit", m_BlkTrdcl->prawhit, "ptrdrht[ntrdcl]/I");

   m_Tree->Branch("ntrdtrk", &m_BlkTrdtrk->ntrdtrk, "ntrdtrk/I");
   m_Tree->Branch("Trdtrkstatus", m_BlkTrdtrk->status, "trdtrkstatus[ntrdtrk]/I");
   m_Tree->Branch("Trdtrkcoo", m_BlkTrdtrk->coo, "trdtrkcoo[3][ntrdtrk]/F");
   m_Tree->Branch("Trdtrkercoo", m_BlkTrdtrk->ercoo, "trdtrkercoo[3][ntrdtrk]/F");
   m_Tree->Branch("Trdtrkphi", m_BlkTrdtrk->phi, "trdtrkphi[ntrdtrk]/F");
   m_Tree->Branch("Trdtrktheta", m_BlkTrdtrk->theta, "trdtrktheta[ntrdtrk]/F");
   m_Tree->Branch("Trdtrkchi2", m_BlkTrdtrk->chi2, "trdtrkchi2[ntrdtrk]/F");
   m_Tree->Branch("Trdtrkns", m_BlkTrdtrk->ns, "trdtrkns[ntrdtrk]/I");
   m_Tree->Branch("Trdtrkpat", m_BlkTrdtrk->pat, "trdtrkpat[ntrdtrk]/I");
   m_Tree->Branch("Trdtrkps", m_BlkTrdtrk->ps, "trdtrkps[5][ntrdtrk]/I");


   m_Tree->Branch("ntofmc", &m_BlkTofmcclu->ntofmc, "ntofmc/I");
   m_Tree->Branch("Tofmcidsoft", m_BlkTofmcclu->Tofmcidsoft, "Tofmcidsoft[ntofmc]/I");
   m_Tree->Branch("Tofmcxcoo", m_BlkTofmcclu->Tofmcxcoo, "Tofmcxcoo[ntofmc][3]/F");
   m_Tree->Branch("Tofmctof", m_BlkTofmcclu->Tofmctof, "Tofmctof[ntofmc]/F");
   m_Tree->Branch("Tofmcedep", m_BlkTofmcclu->Tofmcedep, "Tofmcedep[ntofmc]/F");

   m_Tree->Branch("Ntrcl", &m_BlkTrcluste->Ntrcl, "Ntrcl/I");
   m_Tree->Branch("Idsoft", m_BlkTrcluste->Idsoft, "Idsoft[Ntrcl]/I");
   m_Tree->Branch("Statust", m_BlkTrcluste->Statust, "Statust[Ntrcl]/I");
   m_Tree->Branch("Neleml", m_BlkTrcluste->Neleml, "Neleml[Ntrcl]/I");
   m_Tree->Branch("Nelemr", m_BlkTrcluste->Nelemr, "Nelemr[Ntrcl]/i");
   m_Tree->Branch("Sumt", m_BlkTrcluste->Sumt, "Sumt[Ntrcl]/F");
   m_Tree->Branch("Sigmat", m_BlkTrcluste->Sigmat, "Sigmat[Ntrcl]/F");
   m_Tree->Branch("Meant", m_BlkTrcluste->Meant, "Meant[Ntrcl]/F");
   m_Tree->Branch("Rmst", m_BlkTrcluste->Rmst, "Rmst[Ntrcl]/F");
   m_Tree->Branch("Errormeant", m_BlkTrcluste->Errormeant, "Errormeant[Ntrcl]/F");
   m_Tree->Branch("Amplitude", m_BlkTrcluste->Amplitude, "Amplitude[Ntrcl][5]/F");

   m_Tree->Branch("ntrclmc", &m_BlkTrmcclus->ntrclmc, "ntrclmc/I");
   m_Tree->Branch("Idsoftmc", m_BlkTrmcclus->Idsoftmc, "Idsoftmc[ntrclmc]/I");
   m_Tree->Branch("Itra", m_BlkTrmcclus->Itra, "Itra[ntrclmc]/I");
   m_Tree->Branch("Left", m_BlkTrmcclus->Left, "Left[ntrclmc][2]/I");
   m_Tree->Branch("Center", m_BlkTrmcclus->Center, "Center[ntrclmc][2]/I");
   m_Tree->Branch("Right", m_BlkTrmcclus->Right, "Right[ntrclmc][2]/I");
   m_Tree->Branch("ss", m_BlkTrmcclus->ss, "ss[ntrclmc][2][5]/F");
   m_Tree->Branch("xca", m_BlkTrmcclus->xca, "xca[ntrclmc][3]/F");
   m_Tree->Branch("xcb", m_BlkTrmcclus->xcb, "xcb[ntrclmc][3]/F");
   m_Tree->Branch("xgl", m_BlkTrmcclus->xgl, "xgl[ntrclmc][3]/F");
   m_Tree->Branch("summc", m_BlkTrmcclus->summc, "summc[ntrclmc]/F");

   m_Tree->Branch("ntrrh", &m_BlkTrrechit->ntrrh, "ntrrh/I");
   m_Tree->Branch("px", m_BlkTrrechit->px, "px[ntrrh]/I");
   m_Tree->Branch("py", m_BlkTrrechit->py, "py[ntrrh]/I");
   m_Tree->Branch("statusr", m_BlkTrrechit->statusr, "statusr[ntrrh]/I");
   m_Tree->Branch("Layer", m_BlkTrrechit->Layer, "Layer[ntrrh]/i");
   m_Tree->Branch("hitr", m_BlkTrrechit->hitr, "hitr[ntrrh][3]/F");
   m_Tree->Branch("ehitr", m_BlkTrrechit->ehitr, "ehitr[ntrrh][3]/F");
   m_Tree->Branch("sumr", m_BlkTrrechit->sumr, "sumr[ntrrh]/F");
   m_Tree->Branch("difosum", m_BlkTrrechit->difosum, "difosum[ntrrh]/F");
   m_Tree->Branch("cofgx", m_BlkTrrechit->cofgx, "cofgx[ntrrh]/F");
   m_Tree->Branch("cofgy", m_BlkTrrechit->cofgy, "cofgy[ntrrh]/F");

   m_Tree->Branch("ntrtr", &m_BlkTrtrack->ntrtr, "ntrtr/I");
   m_Tree->Branch("trstatus", m_BlkTrtrack->trstatus, "trstatus[ntrtr]/I");
   m_Tree->Branch("pattern", m_BlkTrtrack->pattern, "pattern[ntrtr]/i");
   m_Tree->Branch("address", m_BlkTrtrack->address, "address[ntrtr]/I");
   m_Tree->Branch("nhits", m_BlkTrtrack->nhits, "nhits[ntrtr]/i");
   m_Tree->Branch("phits", m_BlkTrtrack->phits, "phits[ntrtr][8]/I");
   m_Tree->Branch("Locdbaver", m_BlkTrtrack->Locdbaver, "Locdbaver[ntrtr]/F");
   m_Tree->Branch("Geanefitdone", m_BlkTrtrack->Geanefitdone, "Geanefitdone[ntrtr]/i");
   m_Tree->Branch("Advancedfitdone", m_BlkTrtrack->Advancedfitdone, "Advancedfitdone[ntrtr]/i");
   m_Tree->Branch("Chi2strline", m_BlkTrtrack->Chi2strline, "Chi2strline[ntrtr]/F");
   m_Tree->Branch("Chi2circle", m_BlkTrtrack->Chi2circle, "Chi2circle[ntrtr]/F");
   m_Tree->Branch("Circleridgidity", m_BlkTrtrack->Circleridgidity, "Circleridgidity[ntrtr]/F");
   m_Tree->Branch("Chi2fastfit", m_BlkTrtrack->Chi2fastfit, "Chi2fastfit[ntrtr]/F");
   m_Tree->Branch("Ridgidity", m_BlkTrtrack->Ridgidity, "Ridgidity[ntrtr]/F");
   m_Tree->Branch("Errridgidity", m_BlkTrtrack->Errridgidity, "Errridgidity[ntrtr]/F");
   m_Tree->Branch("Theta", m_BlkTrtrack->Theta, "Theta[ntrtr]/F");
   m_Tree->Branch("phi", m_BlkTrtrack->phi, "phi[ntrtr]/F");
   m_Tree->Branch("p0", m_BlkTrtrack->p0, "p0[ntrtr][3]/F");
   m_Tree->Branch("gchi2", m_BlkTrtrack->gchi2, "gchi2[ntrtr]/F");
   m_Tree->Branch("gridgidity", m_BlkTrtrack->gridgidity, "gridgidity[ntrtr]/F");
   m_Tree->Branch("gerrridgidity", m_BlkTrtrack->gerrridgidity, "gerrridgidity[ntrtr]/F");
//   m_Tree->Branch("gtheta", m_BlkTrtrack->gtheta, "gtheta[ntrtr]/F");
//   m_Tree->Branch("gphi", m_BlkTrtrack->gphi, "gphi[ntrtr]/F");
//   m_Tree->Branch("gp0", m_BlkTrtrack->gp0, "gp0[ntrtr][3]/F");
   m_Tree->Branch("hchi2", m_BlkTrtrack->hchi2, "hchi2[ntrtr][2]/F");
   m_Tree->Branch("Hridgidity", m_BlkTrtrack->Hridgidity, "Hridgidity[ntrtr][2]/F");
   m_Tree->Branch("Herrridgidity", m_BlkTrtrack->Herrridgidity, "Herrridgidity[ntrtr][2]/F");
   m_Tree->Branch("htheta", m_BlkTrtrack->htheta, "htheta[ntrtr][2]/F");
   m_Tree->Branch("hphi", m_BlkTrtrack->hphi, "hphi[ntrtr][2]/F");
   m_Tree->Branch("hp0", m_BlkTrtrack->hp0, "hp0[ntrtr][2][3]/F");
   m_Tree->Branch("fchi2ms", m_BlkTrtrack->fchi2ms, "fchi2ms[ntrtr]/F");
   m_Tree->Branch("pierrrig", m_BlkTrtrack->pierrrig, "pierrrig[ntrtr]/F");
   m_Tree->Branch("ridgidityms", m_BlkTrtrack->ridgidityms, "ridgidityms[ntrtr]/F");
   m_Tree->Branch("piridgidity", m_BlkTrtrack->piridgidity, "piridgidity[ntrtr]/F");

   m_Tree->Branch("nmcg", &m_BlkMceventg->nmcg, "nmcg/I");
   m_Tree->Branch("nskip", m_BlkMceventg->nskip, "nskip[nmcg]/I");
   m_Tree->Branch("Particle", m_BlkMceventg->Particle, "Particle[nmcg]/I");
   m_Tree->Branch("coo", m_BlkMceventg->coo, "coo[nmcg][3]/F");
   m_Tree->Branch("dir", m_BlkMceventg->dir, "dir[nmcg][3]/F");
   m_Tree->Branch("momentum", m_BlkMceventg->momentum, "momentum[nmcg]/F");
   m_Tree->Branch("mass", m_BlkMceventg->mass, "mass[nmcg]/F");
   m_Tree->Branch("charge", m_BlkMceventg->charge, "charge[nmcg]/F");

   m_Tree->Branch("nanti", &m_BlkAnticlus->nanti, "nanti/I");
   m_Tree->Branch("Antistatus", m_BlkAnticlus->Antistatus, "Antistatus[nanti]/I");
   m_Tree->Branch("Antisector", m_BlkAnticlus->Antisector, "Antisector[nanti]/i");
   m_Tree->Branch("Antiedep", m_BlkAnticlus->Antiedep, "Antiedep[nanti]/F");
   m_Tree->Branch("Anticoo", m_BlkAnticlus->Anticoo, "Anticoo[nanti][3]/F");
   m_Tree->Branch("Antiercoo", m_BlkAnticlus->Antiercoo, "Antiercoo[nanti][3]/F");

   m_Tree->Branch("nantimc", &m_BlkAntimccl->nantimc, "nantimc/I");
   m_Tree->Branch("Antimcidsoft", m_BlkAntimccl->Antimcidsoft, "Antimcidsoft[nantimc]/I");
   m_Tree->Branch("Antimcxcoo", m_BlkAntimccl->Antimcxcoo, "Antimcxcoo[nantimc][3]/F");
   m_Tree->Branch("Antimctof", m_BlkAntimccl->Antimctof, "Antimctof[nantimc]/F");
   m_Tree->Branch("Antimcedep", m_BlkAntimccl->Antimcedep, "Antimcedep[nantimc]/F");

   m_Tree->Branch("nlvl3", &m_BlkLvl3->nlvl3, "nlvl3/I");
   m_Tree->Branch("Lvl3toftr", m_BlkLvl3->Lvl3toftr, "Lvl3toftr[nlvl3]/i");
   m_Tree->Branch("Lvl3antitr", m_BlkLvl3->Lvl3antitr, "Lvl3antitr[nlvl3]/i");
   m_Tree->Branch("Lvl3trackertr", m_BlkLvl3->Lvl3trackertr, "Lvl3trackertr[nlvl3]/I");
   m_Tree->Branch("Lvl3ntrhits", m_BlkLvl3->Lvl3ntrhits, "Lvl3ntrhits[nlvl3]/i");
   m_Tree->Branch("Lvl3npat", m_BlkLvl3->Lvl3npat, "Lvl3npat[nlvl3]/i");
   m_Tree->Branch("Lvl3pattern", m_BlkLvl3->Lvl3pattern, "Lvl3pattern[nlvl3][2]/I");
   m_Tree->Branch("Lvl3residual", m_BlkLvl3->Lvl3residual, "Lvl3residual[nlvl3][2]/F");
   m_Tree->Branch("Lvl3time", m_BlkLvl3->Lvl3time, "Lvl3time[nlvl3]/F");
   m_Tree->Branch("Lvl3eloss", m_BlkLvl3->Lvl3eloss, "Lvl3eloss[nlvl3]/F");

   m_Tree->Branch("nlvl1", &m_BlkLvl1->nlvl1, "nlvl1/I");
   m_Tree->Branch("Lvl1lifetime", m_BlkLvl1->mode, "mode[nlvl1]/I");

   m_Tree->Branch("Lvl1tofflag", m_BlkLvl1->Lvl1tofflag, "Lvl1flag[nlvl1]/I");
   m_Tree->Branch("Lvl1tofpatt", m_BlkLvl1->Lvl1tofpatt, "Lvl1tofpatt[nlvl1][4]/I");
   m_Tree->Branch("Lvl1tofpatt1", m_BlkLvl1->Lvl1tofpatt1, "Lvl1tofpatt1[nlvl1][4]/I");
   m_Tree->Branch("Lvl1antipatt", m_BlkLvl1->Lvl1antipatt, "Lvl1antipatt[nlvl1]/I");
   m_Tree->Branch("ecalflag", m_BlkLvl1->ecalflag, "Lvl1ecalflag[nlvl1]/I");

   m_Tree->Branch("ntrraw", &m_BlkTrrawcl->ntrraw, "ntrraw/I");
   m_Tree->Branch("rawaddress", m_BlkTrrawcl->rawaddress, "rawaddress[ntrraw]/I");
   m_Tree->Branch("rawlength", m_BlkTrrawcl->rawlength, "rawlength[ntrraw]/I");
   m_Tree->Branch("s2n", m_BlkTrrawcl->s2n, "s2n[ntrraw]/F");

   m_Tree->Branch("nantiraw", &m_BlkAntirawc->nantiraw, "nantiraw/I");
   m_Tree->Branch("antirawstatus", m_BlkAntirawc->antirawstatus, "antirawstatus[nantiraw]/I");
   m_Tree->Branch("antirawsector", m_BlkAntirawc->antirawsector, "antirawsector[nantiraw]/i");
   m_Tree->Branch("antirawupdown", m_BlkAntirawc->antirawupdown, "antirawupdown[nantiraw]/i");
   m_Tree->Branch("antirawsignal", m_BlkAntirawc->antirawsignal, "antirawsignal[nantiraw]/F");


   m_Tree->Branch("ntofraw", &m_BlkTofrawcl->ntofraw, "ntofraw/I");
   m_Tree->Branch("tofrstatus", m_BlkTofrawcl->tofrstatus, "tofrstatus[ntofraw]/I");
   m_Tree->Branch("tofrplane", m_BlkTofrawcl->tofrplane, "tofrplane[ntofraw]/i");
   m_Tree->Branch("tofrbar", m_BlkTofrawcl->tofrbar, "tofrbar[ntofraw]/i");
   m_Tree->Branch("tofrtovta", m_BlkTofrawcl->tofrtovta, "tofrtovta[ntofraw][2]/F");
   m_Tree->Branch("tofrtovtd", m_BlkTofrawcl->tofrtovtd, "tofrtovtd[ntofraw][2]/F");
   m_Tree->Branch("tofrsdtm", m_BlkTofrawcl->tofrsdtm, "tofrsdtm[ntofraw][2]/F");
   m_Tree->Branch("tofreda", m_BlkTofrawcl->tofreda, "tofreda[ntofraw]/F");
   m_Tree->Branch("tofredd", m_BlkTofrawcl->tofredd, "tofredd[ntofraw]/F");
   m_Tree->Branch("tofrtm", m_BlkTofrawcl->tofrtm, "tofrtm[ntofraw]/F");
   m_Tree->Branch("tofrcoo", m_BlkTofrawcl->tofrcoo, "tofrcoo[ntofraw]/F");

}

//_____________________________________________________________________________
void AMSR_Ntuple::GetEvent(Int_t event)
{
   //
   //Get one event for Ntuple or Tree according to its data type
   //
   if (event<0 || event>=m_Entries) return;
   
   if (m_DataFileType == kRootFile) 
	{
	  debugger.Print("AMSR_Ntuple::GetEvent:getting event %d\n",event);

	  m_Tree->GetEvent(event);
	}

   else if (m_DataFileType == kNtupleFile) {

      event++;
      int ierr;
      if (m_SameRead) 
	{ 	
	HGNT(m_MemID, event, ierr);
	}

      else {
            HGNT(m_MemID, event, ierr);
/*
         if (m_Tree->GetListOfBranches() == 0)
	{
            HGNT(m_MemID, event, ierr);
	}	
         else {
            SetVarNames();
            HGNTV(m_MemID, m_VarNames, m_NVar, event, ierr);
	    for (int i=0; i<m_NVar; i++) 
	      printf("m_VarNames[%d]=%s\n",i,m_VarNames[i]);
         }
*/
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

   HROPEN(lun, top, ntpfile, "XP", reclen, istat);
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
   HBNAME(m_MemID, "TRDMCCL", &(m_BlkTrdclmc->ntrdclmc), "$SET");
   HBNAME(m_MemID, "TRDClu", &(m_BlkTrdcl->ntrdcl), "$SET");
   HBNAME(m_MemID, "PARTICLE", &(m_BlkParticle->npart), "$SET");
   HBNAME(m_MemID, "TOFCLUST", &(m_BlkTofclust->ntof), "$SET");
   HBNAME(m_MemID, "TOFMCCLU", &(m_BlkTofmcclu->ntofmc), "$SET");
   HBNAME(m_MemID, "TRCLUSTE", &(m_BlkTrcluste->Ntrcl), "$SET");
   HBNAME(m_MemID, "TRMCCLUS", &(m_BlkTrmcclus->ntrclmc), "$SET");
   HBNAME(m_MemID, "TRRECHIT", &(m_BlkTrrechit->ntrrh), "$SET");
   HBNAME(m_MemID, "TRTRACK", &(m_BlkTrtrack->ntrtr), "$SET");
   HBNAME(m_MemID, "TRDTRK", &(m_BlkTrdtrk->ntrdtrk), "$SET");
   HBNAME(m_MemID, "ECALSHOW", &(m_Blkecsh->Necsh), "$SET");
   HBNAME(m_MemID, "MCEVENTG", &(m_BlkMceventg->nmcg), "$SET");
   HBNAME(m_MemID, "ANTICLUS", &(m_BlkAnticlus->nanti), "$SET");
   HBNAME(m_MemID, "ANTIMCCL", &(m_BlkAntimccl->nantimc), "$SET");
   HBNAME(m_MemID, "LVL3", &(m_BlkLvl3->nlvl3), "$SET");
   HBNAME(m_MemID, "LVL1", &(m_BlkLvl1->nlvl1), "$SET");
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
	   debugger.Print("AMSR_Ntuple::SetTree(): no of entries %d\n",
          m_Entries);


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


   m_Tree->SetBranchAddress("event02.Eventno", &m_BlkEventh->eventno);
   m_Tree->SetBranchAddress("event02.Run", &m_BlkEventh->run);
   m_Tree->SetBranchAddress("event02.RunType", &m_BlkEventh->runtype);
   m_Tree->SetBranchAddress("event02.Time[2]", m_BlkEventh->time);
   m_Tree->SetBranchAddress("event02.RawWords", &m_BlkEventh->Rawwords);
   m_Tree->SetBranchAddress("event02.RadS", &m_BlkEventh->Rads);
   m_Tree->SetBranchAddress("event02.ThetaS", &m_BlkEventh->Thetas);
   m_Tree->SetBranchAddress("event02.PhiS", &m_BlkEventh->Phis);
   m_Tree->SetBranchAddress("event02.Yaw", &m_BlkEventh->Yaws);
   m_Tree->SetBranchAddress("event02.Pitch", &m_BlkEventh->Pitchs);
   m_Tree->SetBranchAddress("event02.Roll", &m_BlkEventh->Rolls);
   m_Tree->SetBranchAddress("event02.VelocityS", &m_BlkEventh->Velocitys);
   m_Tree->SetBranchAddress("event02.VelTheta", &m_BlkEventh->Veltheta);
   m_Tree->SetBranchAddress("event02.VelPhi", &m_BlkEventh->Velphi);
   m_Tree->SetBranchAddress("event02.ThetaM", &m_BlkEventh->Thetam);
   m_Tree->SetBranchAddress("event02.PhiM", &m_BlkEventh->Phim);
   m_Tree->SetBranchAddress("event02.Tracks", &m_BlkEventh->Tracks);
   m_Tree->SetBranchAddress("event02.TrRecHits", &m_BlkEventh->Trrechits);
   m_Tree->SetBranchAddress("event02.TrClusters", &m_BlkEventh->Trclusters);
   m_Tree->SetBranchAddress("event02.TrRawClusters", &m_BlkEventh->Trrawclusters);
   m_Tree->SetBranchAddress("event02.TrMCClusters", &m_BlkEventh->Trmcclusters);
   m_Tree->SetBranchAddress("event02.TOFClusters", &m_BlkEventh->Tofclusters);
   m_Tree->SetBranchAddress("event02.TOFMCClusters", &m_BlkEventh->Tofmcclusters);
   m_Tree->SetBranchAddress("event02.AntiMCClusters", &m_BlkEventh->Antimcclusters);
   m_Tree->SetBranchAddress("event02.TRDMCClusters", &m_BlkEventh->Trdmcclusters);
   m_Tree->SetBranchAddress("event02.AntiClusters", &m_BlkEventh->Anticlusters);
   m_Tree->SetBranchAddress("event02.EcalClusters", &m_BlkEventh->Ecalclusters);
   m_Tree->SetBranchAddress("event02.EcalHits", &m_BlkEventh->Ecalhits);
   m_Tree->SetBranchAddress("event02.RICMCClusters", &m_BlkEventh->Richmcclusters);
   m_Tree->SetBranchAddress("event02.RICHits", &m_BlkEventh->Richits);
   m_Tree->SetBranchAddress("event02.TRDRawHits", &m_BlkEventh->TRDRawHits);
   m_Tree->SetBranchAddress("event02.TRDClusters", &m_BlkEventh->TRDClusters);
   m_Tree->SetBranchAddress("event02.TRDSegments", &m_BlkEventh->TRDSegments);
   m_Tree->SetBranchAddress("event02.TRDTracks", &m_BlkEventh->TRDTracks);
   m_Tree->SetBranchAddress("event02.EventStatus[2]", m_BlkEventh->Eventstatus);  

/* 
   m_Tree->SetBranchAddress("beta02.Nbeta", &m_BlkBeta->nbeta);
   m_Tree->SetBranchAddress("beta02.Status[50]", m_BlkBeta->betastatus);
   m_Tree->SetBranchAddress("beta02.Pattern[50]", m_BlkBeta->betapattern);
   m_Tree->SetBranchAddress("beta02.Beta[50]", m_BlkBeta->beta);
   m_Tree->SetBranchAddress("beta02.BetaC[50]", m_BlkBeta->betac);
   m_Tree->SetBranchAddress("beta02.Error[50]", m_BlkBeta->betaerror);
   m_Tree->SetBranchAddress("beta02.ErrorC[50]", m_BlkBeta->betaerrorc);
   m_Tree->SetBranchAddress("beta02.Chi2[50]", m_BlkBeta->betachi2);
   m_Tree->SetBranchAddress("beta02.Chi2S[50]", m_BlkBeta->betachi2s);
   m_Tree->SetBranchAddress("beta02.NTOF[50]", m_BlkBeta->betantof);
   m_Tree->SetBranchAddress("beta02.pTOF[50][4]", m_BlkBeta->betaptof);
   m_Tree->SetBranchAddress("beta02.pTr[50]", m_BlkBeta->betaptr);


   m_Tree->SetBranchAddress("charge02.Ncharge", &m_BlkCharge->ncharge);
   m_Tree->SetBranchAddress("charge02.Status[30]", m_BlkCharge->chargestatus);
   m_Tree->SetBranchAddress("charge02.BetaP[30]", m_BlkCharge->chargebetap);
   m_Tree->SetBranchAddress("charge02.ChargeTOF[30]", m_BlkCharge->chargetof);
   m_Tree->SetBranchAddress("charge02.ChargeTracker[30]", m_BlkCharge->chargetracker);
   m_Tree->SetBranchAddress("charge02.ProbTOF[30][4]", m_BlkCharge->probtof);
   m_Tree->SetBranchAddress("charge02.ChInTOF[30][4]", m_BlkCharge->chintof);
   m_Tree->SetBranchAddress("charge02.ProbTracker[30][4]", m_BlkCharge->probtracker);
   m_Tree->SetBranchAddress("charge02.ChInTracker[30][4]", m_BlkCharge->chintracker);
   m_Tree->SetBranchAddress("charge02.ProbAllTracker[30]", m_BlkCharge->proballtracker);
   m_Tree->SetBranchAddress("charge02.TrunTOF[30]", m_BlkCharge->truntof);
   m_Tree->SetBranchAddress("charge02.TrunTOFD[30]", m_BlkCharge->truntofd);
   m_Tree->SetBranchAddress("charge02.TrunTracker[30]", m_BlkCharge->truntracker);

   m_Tree->SetBranchAddress("part02.Npart", &m_BlkParticle->npart);
   m_Tree->SetBranchAddress("part02.BetaP[20]", m_BlkParticle->pbetap);
   m_Tree->SetBranchAddress("part02.ChargeP[20]", m_BlkParticle->pchargep);
   m_Tree->SetBranchAddress("part02.TrackP[20]", m_BlkParticle->ptrackp);
   m_Tree->SetBranchAddress("part02.TRDP[20]", m_BlkParticle->ptrd);
   m_Tree->SetBranchAddress("part02.RICHP[20]", m_BlkParticle->prich);
   m_Tree->SetBranchAddress("part02.EcalP[20]", m_BlkParticle->pecal);
   m_Tree->SetBranchAddress("part02.Particle[20]", m_BlkParticle->pid);
   m_Tree->SetBranchAddress("part02.ParticleVice[20]", m_BlkParticle->pidvice);
   m_Tree->SetBranchAddress("part02.Prob[20][2]", m_BlkParticle->probpid);
   m_Tree->SetBranchAddress("part02.FitMom[20]", m_BlkParticle->fitmom);
   m_Tree->SetBranchAddress("part02.Mass[20]", m_BlkParticle->pmass);
   m_Tree->SetBranchAddress("part02.ErrMass[20]", m_BlkParticle->perrmass);
   m_Tree->SetBranchAddress("part02.Momentum[20]", m_BlkParticle->pmom);
   m_Tree->SetBranchAddress("part02.ErrMomentum[20]", m_BlkParticle->perrmom);
   m_Tree->SetBranchAddress("part02.Beta[20]", m_BlkParticle->pbeta);
   m_Tree->SetBranchAddress("part02.ErrBeta[20]", m_BlkParticle->perrbeta);
   m_Tree->SetBranchAddress("part02.Charge[20]", m_BlkParticle->pcharge);
   m_Tree->SetBranchAddress("part02.Theta[20]", m_BlkParticle->ptheta);
   m_Tree->SetBranchAddress("part02.Phi[20]", m_BlkParticle->pphi);
   m_Tree->SetBranchAddress("part02.ThetaGl[20]", m_BlkParticle->thetagl);
   m_Tree->SetBranchAddress("part02.PhiGl[20]", m_BlkParticle->phigl);
   m_Tree->SetBranchAddress("part02.Coo[20][3]", m_BlkParticle->pcoo);
   m_Tree->SetBranchAddress("part02.Cutoff[20]", m_BlkParticle->cutoff);
   m_Tree->SetBranchAddress("part02.TOFCoo[20][4][3]", m_BlkParticle->cootof);
   m_Tree->SetBranchAddress("part02.AntiCoo[20][2][3]", m_BlkParticle->cooanti);
   m_Tree->SetBranchAddress("part02.EcalCoo[20][3][3]", m_BlkParticle->cooecal);
   m_Tree->SetBranchAddress("part02.TrCoo[20][8][3]", m_BlkParticle->cootr);
   m_Tree->SetBranchAddress("part02.TRDCoo[20][3]", m_BlkParticle->cootrd);



   m_Tree->SetBranchAddress("tofcl.Ntof", &m_BlkTofclust->ntof);
   m_Tree->SetBranchAddress("tofcl.Status[20]", m_BlkTofclust->Tofstatus);
   m_Tree->SetBranchAddress("tofcl.Layer[20]", m_BlkTofclust->plane);
   m_Tree->SetBranchAddress("tofcl.Bar[20]", m_BlkTofclust->bar);
   m_Tree->SetBranchAddress("tofcl.Nmemb[20]", m_BlkTofclust->nmemb);
   m_Tree->SetBranchAddress("tofcl.Edep[20]", m_BlkTofclust->Tofedep);
   m_Tree->SetBranchAddress("tofcl.Edepd[20]", m_BlkTofclust->Tofedepd);
   m_Tree->SetBranchAddress("tofcl.Time[20]", m_BlkTofclust->Toftime);
   m_Tree->SetBranchAddress("tofcl.ErrTime[20]", m_BlkTofclust->Tofetime);
   m_Tree->SetBranchAddress("tofcl.Coo[20][3]", m_BlkTofclust->Tofcoo);
   m_Tree->SetBranchAddress("tofcl.ErrorCoo[20][3]", m_BlkTofclust->Tofercoo);


   m_Tree->SetBranchAddress("tofmccl.tofmc", &m_BlkTofmcclu->ntofmc);
   m_Tree->SetBranchAddress("tofmccl.Idsoft[200]", m_BlkTofmcclu->Tofmcidsoft);
   m_Tree->SetBranchAddress("tofmccl.Coo[200][3]", m_BlkTofmcclu->Tofmcxcoo);
   m_Tree->SetBranchAddress("tofmccl.TOF[200]", m_BlkTofmcclu->Tofmctof);
   m_Tree->SetBranchAddress("tofmccl.Edep[200]", m_BlkTofmcclu->Tofmcedep);

   m_Tree->SetBranchAddress("trcl.Ntrcl", &m_BlkTrcluste->Ntrcl);
   m_Tree->SetBranchAddress("trcl.Idsoft[200]", m_BlkTrcluste->Idsoft);
   m_Tree->SetBranchAddress("trcl.Status[200]", m_BlkTrcluste->Statust);
   m_Tree->SetBranchAddress("trcl.NelemL[200]", m_BlkTrcluste->Neleml);
   m_Tree->SetBranchAddress("trcl.NelemR[200]", m_BlkTrcluste->Nelemr);
   m_Tree->SetBranchAddress("trcl.Sum[200]", m_BlkTrcluste->Sumt);
   m_Tree->SetBranchAddress("trcl.Sigma[200]", m_BlkTrcluste->Sigmat);
   m_Tree->SetBranchAddress("trcl.Mean[200]", m_BlkTrcluste->Meant);
   m_Tree->SetBranchAddress("trcl.RMS[200]", m_BlkTrcluste->Rmst);
   m_Tree->SetBranchAddress("trcl.ErrorMean[200]", m_BlkTrcluste->Errormeant);
   m_Tree->SetBranchAddress("trcl.Amplitude[200][5]", m_BlkTrcluste->Amplitude);


   m_Tree->SetBranchAddress("trclmc.Ntrclmc", &m_BlkTrmcclus->ntrclmc);
   m_Tree->SetBranchAddress("trclmc.Idsoft[200]", m_BlkTrmcclus->Idsoftmc);
   m_Tree->SetBranchAddress("trclmc.TrackNo[200]", m_BlkTrmcclus->Itra);
   m_Tree->SetBranchAddress("trclmc.Left[200][2]", m_BlkTrmcclus->Left);
   m_Tree->SetBranchAddress("trclmc.Center[200][2]", m_BlkTrmcclus->Center);
   m_Tree->SetBranchAddress("trclmc.Right[200][2]", m_BlkTrmcclus->Right);
   m_Tree->SetBranchAddress("trclmc.SS[200][2][5]", m_BlkTrmcclus->ss);
   m_Tree->SetBranchAddress("trclmc.Xca[200][3]", m_BlkTrmcclus->xca);
   m_Tree->SetBranchAddress("trclmc.Xcb[200][3]", m_BlkTrmcclus->xcb);
   m_Tree->SetBranchAddress("trclmc.Xgl[200][3]", m_BlkTrmcclus->xgl);
   m_Tree->SetBranchAddress("trclmc.Sum[200]", m_BlkTrmcclus->summc);

   m_Tree->SetBranchAddress("trrh02.Ntrrh", &m_BlkTrrechit->ntrrh);
   m_Tree->SetBranchAddress("trrh02.pX[666]", m_BlkTrrechit->px);
   m_Tree->SetBranchAddress("trrh02.pY[666]", m_BlkTrrechit->py);
   m_Tree->SetBranchAddress("trrh02.Status[666]", m_BlkTrrechit->statusr);
   m_Tree->SetBranchAddress("trrh02.Layer[666]", m_BlkTrrechit->Layer);
   m_Tree->SetBranchAddress("trrh02.Hit[666][3]", m_BlkTrrechit->hitr);
   m_Tree->SetBranchAddress("trrh02.EHit[666][3]", m_BlkTrrechit->ehitr);
   m_Tree->SetBranchAddress("trrh02.Sum[666]", m_BlkTrrechit->sumr);
   m_Tree->SetBranchAddress("trrh02.DifoSum[666]", m_BlkTrrechit->difosum);
   m_Tree->SetBranchAddress("trrh02.CofgX[666]", m_BlkTrrechit->cofgx);
   m_Tree->SetBranchAddress("trrh02.CofgY[666]", m_BlkTrrechit->cofgy);

   m_Tree->SetBranchAddress("trtr02.Ntrtr", &m_BlkTrtrack->ntrtr);
   m_Tree->SetBranchAddress("trtr02.Status[100]", m_BlkTrtrack->trstatus);
   m_Tree->SetBranchAddress("trtr02.Pattern[100]", m_BlkTrtrack->pattern);
   m_Tree->SetBranchAddress("trtr02.Address[100]", m_BlkTrtrack->address);
   m_Tree->SetBranchAddress("trtr02.NHits[100]", m_BlkTrtrack->nhits);
   m_Tree->SetBranchAddress("trtr02.pHits[100][8]", m_BlkTrtrack->phits);
   m_Tree->SetBranchAddress("trtr02.LocDBAver[100]", m_BlkTrtrack->Locdbaver);
   m_Tree->SetBranchAddress("trtr02.GeaneFitDone[100]", m_BlkTrtrack->Geanefitdone);
   m_Tree->SetBranchAddress("trtr02.AdvancedFitDone[100]", m_BlkTrtrack->Advancedfitdone);
   m_Tree->SetBranchAddress("trtr02.Chi2StrLine[100]", m_BlkTrtrack->Chi2strline);
   m_Tree->SetBranchAddress("trtr02.Chi2Circle[100]", m_BlkTrtrack->Chi2circle);
   m_Tree->SetBranchAddress("trtr02.CircleRidgidity[100]", m_BlkTrtrack->Circleridgidity);
   m_Tree->SetBranchAddress("trtr02.Chi2FastFit[100]", m_BlkTrtrack->Chi2fastfit);
   m_Tree->SetBranchAddress("trtr02.Ridgidity[100]", m_BlkTrtrack->Ridgidity);
   m_Tree->SetBranchAddress("trtr02.ErrRidgidity[100]", m_BlkTrtrack->Errridgidity);
   m_Tree->SetBranchAddress("trtr02.Theta[100]", m_BlkTrtrack->Theta);
   m_Tree->SetBranchAddress("trtr02.Phi[100]", m_BlkTrtrack->phi);
   m_Tree->SetBranchAddress("trtr02.P0[100][3]", m_BlkTrtrack->p0);
   m_Tree->SetBranchAddress("trtr02.GChi2[100]", m_BlkTrtrack->gchi2);
   m_Tree->SetBranchAddress("trtr02.GRidgidity[100]", m_BlkTrtrack->gridgidity);
   m_Tree->SetBranchAddress("trtr02.GErrRidgidity[100]", m_BlkTrtrack->gerrridgidity);
//   m_Tree->SetBranchAddress("trtr02.GTheta[100]", m_BlkTrtrack->gtheta);
//   m_Tree->SetBranchAddress("trtr02.GPhi[100]", m_BlkTrtrack->gphi);
//   m_Tree->SetBranchAddress("trtr02.GP0[100][3]", m_BlkTrtrack->gp0);
   m_Tree->SetBranchAddress("trtr02.HChi2[100][2]", m_BlkTrtrack->hchi2);
   m_Tree->SetBranchAddress("trtr02.HRidgidity[100][2]", m_BlkTrtrack->Hridgidity);
   m_Tree->SetBranchAddress("trtr02.HErrRidgidity[100][2]", m_BlkTrtrack->Herrridgidity);
   m_Tree->SetBranchAddress("trtr02.HTheta[100][2]", m_BlkTrtrack->htheta);
   m_Tree->SetBranchAddress("trtr02.HPhi[100][2]", m_BlkTrtrack->hphi);
   m_Tree->SetBranchAddress("trtr02.HP0[100][2][3]", m_BlkTrtrack->hp0);
   m_Tree->SetBranchAddress("trtr02.FChi2MS[100]", m_BlkTrtrack->fchi2ms);
   m_Tree->SetBranchAddress("trtr02.PiErrRig[100]", m_BlkTrtrack->pierrrig);
   m_Tree->SetBranchAddress("trtr02.RidgidityMS[100]", m_BlkTrtrack->ridgidityms);
   m_Tree->SetBranchAddress("trtr02.PiRigidity[100]", m_BlkTrtrack->piridgidity);


   m_Tree->SetBranchAddress("trdcl.Ntrdcl", &m_BlkTrdcl->ntrdcl);
   m_Tree->SetBranchAddress("trdcl.Status[100]", m_BlkTrdcl->status);
   m_Tree->SetBranchAddress("trdcl.Coo[100][3]", m_BlkTrdcl->coo);
   m_Tree->SetBranchAddress("trdcl.Layer[100]", m_BlkTrdcl->layer);
   m_Tree->SetBranchAddress("trdcl.CooDir[100][3]", m_BlkTrdcl->coodir);
   m_Tree->SetBranchAddress("trdcl.Multip[100]", m_BlkTrdcl->multip);
   m_Tree->SetBranchAddress("trdcl.HMultip[100]", m_BlkTrdcl->hmultip);
   m_Tree->SetBranchAddress("trdcl.EDep[100]", m_BlkTrdcl->edep);
   m_Tree->SetBranchAddress("trdcl.pRawHit[100]", m_BlkTrdcl->prawhit);

   m_Tree->SetBranchAddress("trdtrk.Ntrdtrk", &m_BlkTrdtrk->ntrdtrk);
   m_Tree->SetBranchAddress("trdtrk.Status[40]", m_BlkTrdtrk->status);
   m_Tree->SetBranchAddress("trdtrk.Coo[40][3]", m_BlkTrdtrk->coo);
   m_Tree->SetBranchAddress("trdtrk.ErCoo[40][3]", m_BlkTrdtrk->ercoo);
   m_Tree->SetBranchAddress("trdtrk.Phi[40]", m_BlkTrdtrk->phi);
   m_Tree->SetBranchAddress("trdtrk.Theta[40]", m_BlkTrdtrk->theta);
   m_Tree->SetBranchAddress("trdtrk.Chi2[40]", m_BlkTrdtrk->chi2);
   m_Tree->SetBranchAddress("trdtrk.NSeg[40]", m_BlkTrdtrk->ns);
   m_Tree->SetBranchAddress("trdtrk.Pattern[40]", m_BlkTrdtrk->pat);
   m_Tree->SetBranchAddress("trdtrk.pSeg[40][5]", m_BlkTrdtrk->ps);


   m_Tree->SetBranchAddress("mcg02.Nmcg", &m_BlkMceventg->nmcg);
   m_Tree->SetBranchAddress("mcg02.Nskip[100]", m_BlkMceventg->nskip);
   m_Tree->SetBranchAddress("mcg02.Particle[100]", m_BlkMceventg->Particle);
   m_Tree->SetBranchAddress("mcg02.Coo[100][3]", m_BlkMceventg->coo);
   m_Tree->SetBranchAddress("mcg02.Dir[100][3]", m_BlkMceventg->dir);
   m_Tree->SetBranchAddress("mcg02.Momentum[100]", m_BlkMceventg->momentum);
   m_Tree->SetBranchAddress("mcg02.Mass[100]", m_BlkMceventg->mass);
   m_Tree->SetBranchAddress("mcg02.Charge[100]", m_BlkMceventg->charge);

   m_Tree->SetBranchAddress("anti.Nanti", &m_BlkAnticlus->nanti);
   m_Tree->SetBranchAddress("anti.Status[16]", m_BlkAnticlus->Antistatus);
   m_Tree->SetBranchAddress("anti.Sector[16]", m_BlkAnticlus->Antisector);
   m_Tree->SetBranchAddress("anti.Edep[16]", m_BlkAnticlus->Antiedep);
   m_Tree->SetBranchAddress("anti.Coo[16][3]", m_BlkAnticlus->Anticoo);
   m_Tree->SetBranchAddress("anti.ErrorCoo[16][3]", m_BlkAnticlus->Antiercoo);

   m_Tree->SetBranchAddress("antimc.Nantimc", &m_BlkAntimccl->nantimc);
   m_Tree->SetBranchAddress("antimc.Idsoft[200]", m_BlkAntimccl->Antimcidsoft);
   m_Tree->SetBranchAddress("antimc.Coo[200][3]", m_BlkAntimccl->Antimcxcoo);
   m_Tree->SetBranchAddress("antimc.TOF[200]", m_BlkAntimccl->Antimctof);
   m_Tree->SetBranchAddress("antimc.Edep[200]", m_BlkAntimccl->Antimcedep);

   m_Tree->SetBranchAddress("lvl3.Nlvl3", &m_BlkLvl3->nlvl3);
   m_Tree->SetBranchAddress("lvl3.TOFTr[2]", m_BlkLvl3->Lvl3toftr);
   m_Tree->SetBranchAddress("lvl3.AntiTr[2]", m_BlkLvl3->Lvl3antitr);
   m_Tree->SetBranchAddress("lvl3.TrackerTr[2]", m_BlkLvl3->Lvl3trackertr);
   m_Tree->SetBranchAddress("lvl3.NTrHits[2]", m_BlkLvl3->Lvl3ntrhits);
   m_Tree->SetBranchAddress("lvl3.NPatFound[2]", m_BlkLvl3->Lvl3npat);
   m_Tree->SetBranchAddress("lvl3.Pattern[2][2]", m_BlkLvl3->Lvl3pattern);
   m_Tree->SetBranchAddress("lvl3.Residual[2][2]", m_BlkLvl3->Lvl3residual);
   m_Tree->SetBranchAddress("lvl3.Time[2]", m_BlkLvl3->Lvl3time);
   m_Tree->SetBranchAddress("lvl3.ELoss[2]", m_BlkLvl3->Lvl3eloss);

   m_Tree->SetBranchAddress("lvl102.Nlvl1", &m_BlkLvl1->nlvl1);
   m_Tree->SetBranchAddress("lvl102.Mode[2]", m_BlkLvl1->mode);
   m_Tree->SetBranchAddress("lvl102.TOFlag[2]", m_BlkLvl1->Lvl1tofflag);
   m_Tree->SetBranchAddress("lvl102.TOFPatt[2][4]", m_BlkLvl1->Lvl1tofpatt);
   m_Tree->SetBranchAddress("lvl102.TOFPatt1[2][4]", m_BlkLvl1->Lvl1tofpatt1);
   m_Tree->SetBranchAddress("lvl102.AntiPatt[2]", m_BlkLvl1->Lvl1antipatt);
   m_Tree->SetBranchAddress("lvl102.ECALflag[2]", m_BlkLvl1->ecalflag);  */


/* These not written to Root file at the moment


   m_Tree->SetBranchAddress("Ntrraw", &m_BlkTrrawcl->ntrraw);
   m_Tree->SetBranchAddress("address[300]", m_BlkTrrawcl->rawaddress);
   m_Tree->SetBranchAddress("nelem[300]", m_BlkTrrawcl->rawlength);
   m_Tree->SetBranchAddress("s2n[300]", m_BlkTrrawcl->s2n);

   m_Tree->SetBranchAddress("Nantiraw", &m_BlkAntirawc->nantiraw);
   m_Tree->SetBranchAddress("Status[32]", m_BlkAntirawc->antirawstatus);
   m_Tree->SetBranchAddress("Sector[32]", m_BlkAntirawc->antirawsector);
   m_Tree->SetBranchAddress("UpDown[32]", m_BlkAntirawc->antirawupdown);
   m_Tree->SetBranchAddress("Signal[32]", m_BlkAntirawc->antirawsignal); 

   m_Tree->SetBranchAddress("Ntofraw", &m_BlkTofrawcl->ntofraw);
   m_Tree->SetBranchAddress("Status[20]", m_BlkTofrawcl->tofrstatus);
   m_Tree->SetBranchAddress("Layer[20]", m_BlkTofrawcl->tofrplane);
   m_Tree->SetBranchAddress("Bar[20]", m_BlkTofrawcl->tofrbar);
   m_Tree->SetBranchAddress("tovta[20][2]", m_BlkTofrawcl->tofrtovta);
   m_Tree->SetBranchAddress("tovtd[20][2]", m_BlkTofrawcl->tofrtovtd);
   m_Tree->SetBranchAddress("sdtm[20][2]", m_BlkTofrawcl->tofrsdtm);
   m_Tree->SetBranchAddress("edepa[20]", m_BlkTofrawcl->tofreda);
   m_Tree->SetBranchAddress("edepd[20]", m_BlkTofrawcl->tofredd);
   m_Tree->SetBranchAddress("time[20]", m_BlkTofrawcl->tofrtm);
   m_Tree->SetBranchAddress("cool[20]", m_BlkTofrawcl->tofrcoo);   */

}

//_____________________________________________________________________________
void AMSR_Ntuple::SetVarNames()
{
   //
   //Get variable names selected to read, which is used in HGNTV
   //

   TObjArray *objarray = m_Tree->GetListOfBranches();
   if (objarray == 0) return;

   int cols = sizeof(m_VarNames[0]);
   int rows = sizeof(m_VarNames)/cols;

   TBranch *branch;
   TObject* obj = objarray->First();
   m_NVar = 0;

   while (obj) {
      if (m_NVar >= rows) {
	 Error("AMSR_Ntuple","index %d out of bounds [%d]\n",m_NVar+1,rows);
	 break;
      }
      branch = (TBranch*)obj;
      const char *name = branch->GetName();
      if (strlen(name) > cols) 
         Error("AMSR_Ntuple::SetVarNames","too long(>%d) name %s",cols,name);
      else strncpy(m_VarNames[m_NVar++], name, cols-1);
	obj = objarray->After(obj);
   }

}

