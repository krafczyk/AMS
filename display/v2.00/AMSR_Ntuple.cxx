
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
#include <iostream.h>
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
   m_DataFileType = NtupleFile;
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

   SetSampleTree();

   gAMSR_Ntuple = this;
}

//_____________________________________________________________________________
AMSR_Ntuple::~AMSR_Ntuple()
{
   if (m_SampleTree != 0 && m_SampleTree->IsOpen()) m_SampleTree->Close();
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
void AMSR_Ntuple::GetEvent(Int_t event)
{
   //
   //Get one event for Ntuple or Tree according to its data type
   //
   if (event<0 || event>=m_Entries) return;
   
   if (m_DataFileType == RootFile) m_Tree->GetEvent(event);

   else if (m_DataFileType == NtupleFile) {

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
   
   return 0;
}

//_____________________________________________________________________________
void AMSR_Ntuple::SetBranchStatus(char *varname, Bool_t status)
{
   m_Tree->SetBranchStatus(varname,status);
   if (m_DataFileType == NtupleFile) m_SameRead = kFALSE;
}

//_____________________________________________________________________________
void AMSR_Ntuple::SetSampleTree()
{
   //
   //Get a sample of ROOT-TTree
   //
   if (m_SampleTree == 0 ) {
     char * Geo_dir=getenv("AMSGeoDir");
     if(!Geo_dir){
       cerr <<"AMSR_Display-F-AMSGeoDir Not Defined. Exit(1)"<<endl;
       exit(1);
     }
     char fname[256];
     strcpy(fname,Geo_dir);
     strcat(fname,"/NtupleSample.root");
     m_SampleTree = TFile::Open(fname);
   }
   m_Tree = (TTree*)m_SampleTree->Get("h1");

   //
   //Set Branch Addresses
   //
   SetTreeAddress();
   
}

//_____________________________________________________________________________
void AMSR_Ntuple::SetTree(TTree *t)
{
   //
   //Ntuple data file and Root Tree data file
   //

   if (t == 0) {      //Ntuple data file

      if (m_DataFileType != NtupleFile || m_Tree == 0) SetSampleTree();
      m_DataFileType = NtupleFile;

   } else {           //Root Tree data file

      m_Tree    = t;
      m_Entries = m_Tree->GetEntries();
      SetTreeAddress();
      m_DataFileType = RootFile;

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
   m_Tree->SetBranchAddress("eventno",&m_BlkEventh->eventno);
   m_Tree->SetBranchAddress("run",&m_BlkEventh->run);
   m_Tree->SetBranchAddress("runtype",&m_BlkEventh->runtype);
   m_Tree->SetBranchAddress("time",m_BlkEventh->time);
   m_Tree->SetBranchAddress("Rawwords",&m_BlkEventh->Rawwords);
   m_Tree->SetBranchAddress("Grmedphi",&m_BlkEventh->Grmedphi);
   m_Tree->SetBranchAddress("Rads",&m_BlkEventh->Rads);
   m_Tree->SetBranchAddress("Thetas",&m_BlkEventh->Thetas);
   m_Tree->SetBranchAddress("Phis",&m_BlkEventh->Phis);
   m_Tree->SetBranchAddress("Yaws",&m_BlkEventh->Yaws);
   m_Tree->SetBranchAddress("Pitchs",&m_BlkEventh->Pitchs);
   m_Tree->SetBranchAddress("Rolls",&m_BlkEventh->Rolls);
   m_Tree->SetBranchAddress("Velocitys",&m_BlkEventh->Velocitys);
   m_Tree->SetBranchAddress("Particles",&m_BlkEventh->Particles);
   m_Tree->SetBranchAddress("Tracks",&m_BlkEventh->Tracks);
   m_Tree->SetBranchAddress("Betas",&m_BlkEventh->Betas);
   m_Tree->SetBranchAddress("Charges",&m_BlkEventh->Charges);
   m_Tree->SetBranchAddress("Trrechits",&m_BlkEventh->Trrechits);
   m_Tree->SetBranchAddress("Trclusters",&m_BlkEventh->Trclusters);
   m_Tree->SetBranchAddress("Trrawclusters",&m_BlkEventh->Trrawclusters);
   m_Tree->SetBranchAddress("Trmcclusters",&m_BlkEventh->Trmcclusters);
   m_Tree->SetBranchAddress("Tofclusters",&m_BlkEventh->Tofclusters);
   m_Tree->SetBranchAddress("Tofmcclusters",&m_BlkEventh->Tofmcclusters);
   m_Tree->SetBranchAddress("Ctcclusters",&m_BlkEventh->Ctcclusters);
   m_Tree->SetBranchAddress("Ctcmcclusters",&m_BlkEventh->Ctcmcclusters);
   m_Tree->SetBranchAddress("Antimcclusters",&m_BlkEventh->Antimcclusters);
   m_Tree->SetBranchAddress("Anticlusters",&m_BlkEventh->Anticlusters);
   m_Tree->SetBranchAddress("nbeta",&m_BlkBeta->nbeta);
   m_Tree->SetBranchAddress("betastatus",m_BlkBeta->betastatus);
   m_Tree->SetBranchAddress("betapattern",m_BlkBeta->betapattern);
   m_Tree->SetBranchAddress("beta",m_BlkBeta->beta);
   m_Tree->SetBranchAddress("betaerror",m_BlkBeta->betaerror);
   m_Tree->SetBranchAddress("betachi2",m_BlkBeta->betachi2);
   m_Tree->SetBranchAddress("betachi2s",m_BlkBeta->betachi2s);
   m_Tree->SetBranchAddress("betantof",m_BlkBeta->betantof);
   m_Tree->SetBranchAddress("betaptof",m_BlkBeta->betaptof);
   m_Tree->SetBranchAddress("betaptr",m_BlkBeta->betaptr);
   m_Tree->SetBranchAddress("ncharge",&m_BlkCharge->ncharge);
   m_Tree->SetBranchAddress("chargestatus",m_BlkCharge->chargestatus);
   m_Tree->SetBranchAddress("chargebetap",m_BlkCharge->chargebetap);
   m_Tree->SetBranchAddress("chargetof",m_BlkCharge->chargetof);
   m_Tree->SetBranchAddress("chargetracker",m_BlkCharge->chargetracker);
   m_Tree->SetBranchAddress("probtof",m_BlkCharge->probtof);
   m_Tree->SetBranchAddress("probtracker",m_BlkCharge->probtracker);
   m_Tree->SetBranchAddress("npart",&m_BlkParticle->npart);
   m_Tree->SetBranchAddress("pctcp",m_BlkParticle->pctcp);
   m_Tree->SetBranchAddress("pbetap",m_BlkParticle->pbetap);
   m_Tree->SetBranchAddress("pchargep",m_BlkParticle->pchargep);
   m_Tree->SetBranchAddress("ptrackp",m_BlkParticle->ptrackp);
   m_Tree->SetBranchAddress("pid",m_BlkParticle->pid);
   m_Tree->SetBranchAddress("pmass",m_BlkParticle->pmass);
   m_Tree->SetBranchAddress("perrmass",m_BlkParticle->perrmass);
   m_Tree->SetBranchAddress("pmom",m_BlkParticle->pmom);
   m_Tree->SetBranchAddress("perrmom",m_BlkParticle->perrmom);
   m_Tree->SetBranchAddress("pcharge",m_BlkParticle->pcharge);
   m_Tree->SetBranchAddress("ptheta",m_BlkParticle->ptheta);
   m_Tree->SetBranchAddress("pphi",m_BlkParticle->pphi);
   m_Tree->SetBranchAddress("pcoo",m_BlkParticle->pcoo);
   m_Tree->SetBranchAddress("signalctc",m_BlkParticle->signalctc);
   m_Tree->SetBranchAddress("betactc",m_BlkParticle->betactc);
   m_Tree->SetBranchAddress("errorbetactc",m_BlkParticle->errorbetactc);
   m_Tree->SetBranchAddress("cooctc",m_BlkParticle->cooctc);
   m_Tree->SetBranchAddress("cootof",m_BlkParticle->cootof);
   m_Tree->SetBranchAddress("cooanti",m_BlkParticle->cooanti);
   m_Tree->SetBranchAddress("cootr",m_BlkParticle->cootr);
   m_Tree->SetBranchAddress("ntof",&m_BlkTofclust->ntof);
   m_Tree->SetBranchAddress("Tofstatus",m_BlkTofclust->Tofstatus);
   m_Tree->SetBranchAddress("plane",m_BlkTofclust->plane);
   m_Tree->SetBranchAddress("bar",m_BlkTofclust->bar);
   m_Tree->SetBranchAddress("Tofedep",m_BlkTofclust->Tofedep);
   m_Tree->SetBranchAddress("Toftime",m_BlkTofclust->Toftime);
   m_Tree->SetBranchAddress("Tofetime",m_BlkTofclust->Tofetime);
   m_Tree->SetBranchAddress("Tofcoo",m_BlkTofclust->Tofcoo);
   m_Tree->SetBranchAddress("Tofercoo",m_BlkTofclust->Tofercoo);
   m_Tree->SetBranchAddress("ntofmc",&m_BlkTofmcclu->ntofmc);
   m_Tree->SetBranchAddress("Tofmcidsoft",m_BlkTofmcclu->Tofmcidsoft);
   m_Tree->SetBranchAddress("Tofmcxcoo",m_BlkTofmcclu->Tofmcxcoo);
   m_Tree->SetBranchAddress("Tofmctof",m_BlkTofmcclu->Tofmctof);
   m_Tree->SetBranchAddress("Tofmcedep",m_BlkTofmcclu->Tofmcedep);
   m_Tree->SetBranchAddress("Ntrcl",&m_BlkTrcluste->Ntrcl);
   m_Tree->SetBranchAddress("Idsoft",m_BlkTrcluste->Idsoft);
   m_Tree->SetBranchAddress("Statust",m_BlkTrcluste->Statust);
   m_Tree->SetBranchAddress("Neleml",m_BlkTrcluste->Neleml);
   m_Tree->SetBranchAddress("Nelemr",m_BlkTrcluste->Nelemr);
   m_Tree->SetBranchAddress("Sumt",m_BlkTrcluste->Sumt);
   m_Tree->SetBranchAddress("Sigmat",m_BlkTrcluste->Sigmat);
   m_Tree->SetBranchAddress("Meant",m_BlkTrcluste->Meant);
   m_Tree->SetBranchAddress("Rmst",m_BlkTrcluste->Rmst);
   m_Tree->SetBranchAddress("Errormeant",m_BlkTrcluste->Errormeant);
   m_Tree->SetBranchAddress("Amplitude",m_BlkTrcluste->Amplitude);
   m_Tree->SetBranchAddress("ntrclmc",&m_BlkTrmcclus->ntrclmc);
   m_Tree->SetBranchAddress("Idsoftmc",m_BlkTrmcclus->Idsoftmc);
   m_Tree->SetBranchAddress("Itra",m_BlkTrmcclus->Itra);
   m_Tree->SetBranchAddress("Left",m_BlkTrmcclus->Left);
   m_Tree->SetBranchAddress("Center",m_BlkTrmcclus->Center);
   m_Tree->SetBranchAddress("Right",m_BlkTrmcclus->Right);
   m_Tree->SetBranchAddress("ss",m_BlkTrmcclus->ss);
   m_Tree->SetBranchAddress("xca",m_BlkTrmcclus->xca);
   m_Tree->SetBranchAddress("xcb",m_BlkTrmcclus->xcb);
   m_Tree->SetBranchAddress("xgl",m_BlkTrmcclus->xgl);
   m_Tree->SetBranchAddress("summc",m_BlkTrmcclus->summc);
   m_Tree->SetBranchAddress("ntrrh",&m_BlkTrrechit->ntrrh);
   m_Tree->SetBranchAddress("px",m_BlkTrrechit->px);
   m_Tree->SetBranchAddress("py",m_BlkTrrechit->py);
   m_Tree->SetBranchAddress("statusr",m_BlkTrrechit->statusr);
   m_Tree->SetBranchAddress("Layer",m_BlkTrrechit->Layer);
   m_Tree->SetBranchAddress("hitr",m_BlkTrrechit->hitr);
   m_Tree->SetBranchAddress("ehitr",m_BlkTrrechit->ehitr);
   m_Tree->SetBranchAddress("sumr",m_BlkTrrechit->sumr);
   m_Tree->SetBranchAddress("difosum",m_BlkTrrechit->difosum);
   m_Tree->SetBranchAddress("ntrtr",&m_BlkTrtrack->ntrtr);
   m_Tree->SetBranchAddress("trstatus",m_BlkTrtrack->trstatus);
   m_Tree->SetBranchAddress("pattern",m_BlkTrtrack->pattern);
   m_Tree->SetBranchAddress("nhits",m_BlkTrtrack->nhits);
   m_Tree->SetBranchAddress("phits",m_BlkTrtrack->phits);
   m_Tree->SetBranchAddress("Fastfitdone",m_BlkTrtrack->Fastfitdone);
   m_Tree->SetBranchAddress("Geanefitdone",m_BlkTrtrack->Geanefitdone);
   m_Tree->SetBranchAddress("Advancedfitdone",m_BlkTrtrack->Advancedfitdone);
   m_Tree->SetBranchAddress("Chi2strline",m_BlkTrtrack->Chi2strline);
   m_Tree->SetBranchAddress("Chi2circle",m_BlkTrtrack->Chi2circle);
   m_Tree->SetBranchAddress("Circleridgidity",m_BlkTrtrack->Circleridgidity);
   m_Tree->SetBranchAddress("Chi2fastfit",m_BlkTrtrack->Chi2fastfit);
   m_Tree->SetBranchAddress("Ridgidity",m_BlkTrtrack->Ridgidity);
   m_Tree->SetBranchAddress("Errridgidity",m_BlkTrtrack->Errridgidity);
   m_Tree->SetBranchAddress("Theta",m_BlkTrtrack->Theta);
   m_Tree->SetBranchAddress("phi",m_BlkTrtrack->phi);
   m_Tree->SetBranchAddress("p0",m_BlkTrtrack->p0);
   m_Tree->SetBranchAddress("gchi2",m_BlkTrtrack->gchi2);
   m_Tree->SetBranchAddress("gridgidity",m_BlkTrtrack->gridgidity);
   m_Tree->SetBranchAddress("gerrridgidity",m_BlkTrtrack->gerrridgidity);
   m_Tree->SetBranchAddress("gtheta",m_BlkTrtrack->gtheta);
   m_Tree->SetBranchAddress("gphi",m_BlkTrtrack->gphi);
   m_Tree->SetBranchAddress("gp0",m_BlkTrtrack->gp0);
   m_Tree->SetBranchAddress("hchi2",m_BlkTrtrack->hchi2);
   m_Tree->SetBranchAddress("Hridgidity",m_BlkTrtrack->Hridgidity);
   m_Tree->SetBranchAddress("Herrridgidity",m_BlkTrtrack->Herrridgidity);
   m_Tree->SetBranchAddress("htheta",m_BlkTrtrack->htheta);
   m_Tree->SetBranchAddress("hphi",m_BlkTrtrack->hphi);
   m_Tree->SetBranchAddress("hp0",m_BlkTrtrack->hp0);
   m_Tree->SetBranchAddress("fchi2ms",m_BlkTrtrack->fchi2ms);
   m_Tree->SetBranchAddress("gchi2ms",m_BlkTrtrack->gchi2ms);
   m_Tree->SetBranchAddress("ridgidityms",m_BlkTrtrack->ridgidityms);
   m_Tree->SetBranchAddress("gridgidityms",m_BlkTrtrack->gridgidityms);
   m_Tree->SetBranchAddress("nmcg",&m_BlkMceventg->nmcg);
   m_Tree->SetBranchAddress("nskip",m_BlkMceventg->nskip);
   m_Tree->SetBranchAddress("Particle",m_BlkMceventg->Particle);
   m_Tree->SetBranchAddress("coo",m_BlkMceventg->coo);
   m_Tree->SetBranchAddress("dir",m_BlkMceventg->dir);
   m_Tree->SetBranchAddress("momentum",m_BlkMceventg->momentum);
   m_Tree->SetBranchAddress("mass",m_BlkMceventg->mass);
   m_Tree->SetBranchAddress("charge",m_BlkMceventg->charge);
   m_Tree->SetBranchAddress("nctccl",&m_BlkCtcclust->nctccl);
   m_Tree->SetBranchAddress("Ctcstatus",m_BlkCtcclust->Ctcstatus);
   m_Tree->SetBranchAddress("Ctclayer",m_BlkCtcclust->Ctclayer);
   m_Tree->SetBranchAddress("ctccoo",m_BlkCtcclust->ctccoo);
   m_Tree->SetBranchAddress("ctcercoo",m_BlkCtcclust->ctcercoo);
   m_Tree->SetBranchAddress("ctcrawsignal",m_BlkCtcclust->ctcrawsignal);
   m_Tree->SetBranchAddress("ctcsignal",m_BlkCtcclust->ctcsignal);
   m_Tree->SetBranchAddress("ctcesignal",m_BlkCtcclust->ctcesignal);
   m_Tree->SetBranchAddress("nctcclmc",&m_BlkCtcmcclu->nctcclmc);
   m_Tree->SetBranchAddress("Ctcmcidsoft",m_BlkCtcmcclu->Ctcmcidsoft);
   m_Tree->SetBranchAddress("Ctcmcxcoo",m_BlkCtcmcclu->Ctcmcxcoo);
   m_Tree->SetBranchAddress("Ctcmcxdir",m_BlkCtcmcclu->Ctcmcxdir);
   m_Tree->SetBranchAddress("Ctcstep",m_BlkCtcmcclu->Ctcstep);
   m_Tree->SetBranchAddress("ctccharge",m_BlkCtcmcclu->ctccharge);
   m_Tree->SetBranchAddress("ctcbeta",m_BlkCtcmcclu->ctcbeta);
   m_Tree->SetBranchAddress("ctcedep",m_BlkCtcmcclu->ctcedep);
   m_Tree->SetBranchAddress("nanti",&m_BlkAnticlus->nanti);
   m_Tree->SetBranchAddress("Antistatus",m_BlkAnticlus->Antistatus);
   m_Tree->SetBranchAddress("Antisector",m_BlkAnticlus->Antisector);
   m_Tree->SetBranchAddress("Antiedep",m_BlkAnticlus->Antiedep);
   m_Tree->SetBranchAddress("Anticoo",m_BlkAnticlus->Anticoo);
   m_Tree->SetBranchAddress("Antiercoo",m_BlkAnticlus->Antiercoo);
   m_Tree->SetBranchAddress("nantimc",&m_BlkAntimccl->nantimc);
   m_Tree->SetBranchAddress("Antimcidsoft",m_BlkAntimccl->Antimcidsoft);
   m_Tree->SetBranchAddress("Antimcxcoo",m_BlkAntimccl->Antimcxcoo);
   m_Tree->SetBranchAddress("Antimctof",m_BlkAntimccl->Antimctof);
   m_Tree->SetBranchAddress("Antimcedep",m_BlkAntimccl->Antimcedep);
   m_Tree->SetBranchAddress("nlvl3",&m_BlkLvl3->nlvl3);
   m_Tree->SetBranchAddress("Lvl3toftr",m_BlkLvl3->Lvl3toftr);
   m_Tree->SetBranchAddress("Lvl3antitr",m_BlkLvl3->Lvl3antitr);
   m_Tree->SetBranchAddress("Lvl3trackertr",m_BlkLvl3->Lvl3trackertr);
   m_Tree->SetBranchAddress("Lvl3ntrhits",m_BlkLvl3->Lvl3ntrhits);
   m_Tree->SetBranchAddress("Lvl3npat",m_BlkLvl3->Lvl3npat);
   m_Tree->SetBranchAddress("Lvl3pattern",m_BlkLvl3->Lvl3pattern);
   m_Tree->SetBranchAddress("Lvl3residual",m_BlkLvl3->Lvl3residual);
   m_Tree->SetBranchAddress("Lvl3time",m_BlkLvl3->Lvl3time);
   m_Tree->SetBranchAddress("Lvl3eloss",m_BlkLvl3->Lvl3eloss);
   m_Tree->SetBranchAddress("nlvl1",&m_BlkLvl1->nlvl1);
   m_Tree->SetBranchAddress("Lvl1mode",m_BlkLvl1->Lvl1mode);
   m_Tree->SetBranchAddress("Lvl1flag",m_BlkLvl1->Lvl1flag);
   m_Tree->SetBranchAddress("Lvl1tofpatt",m_BlkLvl1->Lvl1tofpatt);
   m_Tree->SetBranchAddress("Lvl1tofpatt1",m_BlkLvl1->Lvl1tofpatt1);
   m_Tree->SetBranchAddress("Lvl1antipatt",m_BlkLvl1->Lvl1antipatt);
   m_Tree->SetBranchAddress("nctcht",&m_BlkCtchit->nctcht);
   m_Tree->SetBranchAddress("Ctchitstatus",m_BlkCtchit->Ctchitstatus);
   m_Tree->SetBranchAddress("Ctchitlayer",m_BlkCtchit->Ctchitlayer);
   m_Tree->SetBranchAddress("ctchitcolumn",m_BlkCtchit->ctchitcolumn);
   m_Tree->SetBranchAddress("ctchitrow",m_BlkCtchit->ctchitrow);
   m_Tree->SetBranchAddress("ctchitsignal",m_BlkCtchit->ctchitsignal);
   m_Tree->SetBranchAddress("ntrraw",&m_BlkTrrawcl->ntrraw);
   m_Tree->SetBranchAddress("rawaddress",m_BlkTrrawcl->rawaddress);
   m_Tree->SetBranchAddress("rawlength",m_BlkTrrawcl->rawlength);
   m_Tree->SetBranchAddress("s2n",m_BlkTrrawcl->s2n);
   m_Tree->SetBranchAddress("nantiraw",&m_BlkAntirawc->nantiraw);
   m_Tree->SetBranchAddress("antirawstatus",m_BlkAntirawc->antirawstatus);
   m_Tree->SetBranchAddress("antirawsector",m_BlkAntirawc->antirawsector);
   m_Tree->SetBranchAddress("antirawupdown",m_BlkAntirawc->antirawupdown);
   m_Tree->SetBranchAddress("antirawsignal",m_BlkAntirawc->antirawsignal);
   m_Tree->SetBranchAddress("ntofraw",&m_BlkTofrawcl->ntofraw);
   m_Tree->SetBranchAddress("tofrstatus",m_BlkTofrawcl->tofrstatus);
   m_Tree->SetBranchAddress("tofrplane",m_BlkTofrawcl->tofrplane);
   m_Tree->SetBranchAddress("tofrbar",m_BlkTofrawcl->tofrbar);
   m_Tree->SetBranchAddress("tofrtovta",m_BlkTofrawcl->tofrtovta);
   m_Tree->SetBranchAddress("tofrtovtd",m_BlkTofrawcl->tofrtovtd);
   m_Tree->SetBranchAddress("tofrsdtm",m_BlkTofrawcl->tofrsdtm);
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

