#ifndef AMSR_Ntuple_H
#define AMSR_Ntuple_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_Ntuple                                                          //
//                                                                      //
// Class to handle ntuple and make association between ntuple and Root  //
//  tree and acts as an interface to ROOT tree when data is ROOT tree   //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <TNamed.h>
#ifndef __CINT__
 #include "AMSR_NtupleCommons.h"
#endif
#ifndef AMSR_Types_H
#include "AMSR_Types.h"
#endif
#include <time.h>

class TTree;
class TFile;

//enum EDataFileType { RootFile=0, ObjectivityFile=1, NtupleFile=2 };

class AMSR_Ntuple : public TNamed {

 private:
   Text_t        m_VarNames[20][20];    //Name list of m_NVar variables
   Int_t         m_NVar;        //Number of vairables in HGNTV
   TFile        *m_SampleTree;  //File storing the TTree sample
   Int_t         m_MemID;       //Actual ntuple ID in memory

 protected:
   EDataFileType m_DataFileType;//Data file type to access
   Int_t         m_Entries;     //Entries in ntuple or Root-tree
   Int_t         m_Lun;         //I/O unit for ntuple file
   Int_t         m_NtupleID;    //Ntuple ID for current ntuple
   Int_t         m_NextID;      //Ntuple ID for new ntuple file
   Bool_t        m_NtupleOpen;  //Indicator whether already ntuple open
   Bool_t        m_SameRead;    //Same variables reading for HGNTF
   TTree        *m_Tree;        //ROOT TTree associated with ntuple
#ifndef __CINT__
   EVENTH_DEF   *m_BlkEventh;   //Pointers to common blocks used by CWN ntuple
   BETA_DEF     *m_BlkBeta;     // according to CWN block name
   CHARGE_DEF   *m_BlkCharge;
   PARTICLE_DEF *m_BlkParticle;
   TOFCLUST_DEF *m_BlkTofclust;
   TOFMCCLU_DEF *m_BlkTofmcclu;
   TRCLUSTE_DEF *m_BlkTrcluste;
   TRMCCLUS_DEF *m_BlkTrmcclus;
   TRRECHIT_DEF *m_BlkTrrechit;
   TRTRACK_DEF  *m_BlkTrtrack;
   MCEVENTG_DEF *m_BlkMceventg;
   CTCCLUST_DEF *m_BlkCtcclust;
   CTCMCCLU_DEF *m_BlkCtcmcclu;
   ANTICLUS_DEF *m_BlkAnticlus;
   ANTIMCCL_DEF *m_BlkAntimccl;
   LVL3_DEF     *m_BlkLvl3;
   LVL1_DEF     *m_BlkLvl1;
   CTCHIT_DEF   *m_BlkCtchit;
   TRRAWCL_DEF  *m_BlkTrrawcl;
   ANTIRAWC_DEF *m_BlkAntirawc;
   TOFRAWCL_DEF *m_BlkTofrawcl;
#endif
   
 protected:
   void          SetTreeAddress();
   void          SetSampleTree();
   void          SetVarNames();
   
 public:
//                 AMSR_Ntuple();
                 AMSR_Ntuple(const char *name="AMSR_Ntuple",
			     const char *title="The AMS Ntuple");
                ~AMSR_Ntuple();
   void          CloseNtuple();
   Int_t         GetEntries()    { return m_Entries;}
   void          GetEvent(Int_t event);
   Int_t         GetEvtNum()     { return m_BlkEventh->eventno;}
   Int_t         GetNextID()     { return m_NextID;}
   Int_t         GetLun()        { return m_Lun;}
   Int_t         GetRunNum()     { return m_BlkEventh->run;}
   void          GetRunTime(time_t* time);
   Int_t         GetRunType()    { return m_BlkEventh->runtype;}
   Int_t         GetNtupleID()   { return m_NtupleID;}
   TTree        *GetTree()       { return m_Tree;}
   Bool_t        IsNtupleOpen()  { return m_NtupleOpen;}
   Int_t         OpenNtuple(char *ntpfile);
   void          SetBranchStatus(char *varname, Bool_t status);
   void          SetNtupleID(Int_t id) { m_NextID = id;}
   void          SetTree(TTree *t=0);

   friend   class    AMSR_AntiClusterReader;
   friend   class    AMSR_CTCClusterReader;
   friend   class    AMSR_MCParticleReader;
   friend   class    AMSR_ParticleReader;
   friend   class    AMSR_SiHitReader;
   friend   class    AMSR_ToFClusterReader;
   friend   class    AMSR_TrMCClusterReader;
   friend   class    AMSR_TrackReader;

   ClassDef(AMSR_Ntuple, 0)   //AMSR_Ntuple ntuple handling class
};

#endif
