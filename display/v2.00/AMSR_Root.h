#ifndef AMSR_Root_H
#define AMSR_Root_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_Root                                                              //
//                                                                      //
// Main class to control the AMSR_Root program.                           //
//                                                                      //
// This class :                                                         //
//   - Initialises the run default parameters                           //
//   - Provides API to Set/Get run parameters                           //
//   - Creates the support lists (TClonesArrays) for the Event structure//
//   - Creates the physics objects makers                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TTree
#include <TTree.h>
#endif
#ifndef AMSR_HistBrowser_H
#include "AMSR_HistBrowser.h"
#endif
#ifndef AMSR_Maker_H
#include "AMSR_Maker.h"
#endif
#include <time.h>

class TBrowser;
//class AMSR_MCMaker;
class AMSR_ToFClusterReader;
class AMSR_TrackReader;
class AMSR_SiHitReader;
class AMSR_CTCClusterReader;
class AMSR_AntiClusterReader;
class AMSR_TrMCClusterReader;
class AMSR_ParticleReader;
class AMSR_MCParticleReader;
//class ATLFElectronMaker;
//class ATLFMuonMaker;
//class ATLFPhotonMaker;
//class ATLFJetMaker;
//class AMSR_TriggerMaker;
//class AMSR_MiscMaker;
class AMSR_VirtualDisplay;

class AMSR_Root : public TNamed {

private:
   char                *m_DataFileName;      //Data filename
   Int_t                m_Version;           //AMSR_Root version number
   Int_t                m_VersionDate;       //AMSR_Root version date
   Int_t                m_RunNum;            //Run number
   Int_t                m_EventNum;          //Event number
   Int_t                m_Mode;              //Run mode
   Int_t                m_NEvent;            //Total events in the tree
   Int_t		m_Event;             //Event counter in the input tree
   time_t               m_time[2];           //Time
   TTree               *m_Tree;              //Pointer to the Root tree
   TTree               *m_Input;             //Pointer to the input data tree
   TList               *m_Makers;            //List of Makers

//    pointers to standard Makers
   AMSR_ToFClusterReader *m_ToFClusterMaker;   //Pointer to ToFClusterMaker
   AMSR_TrackReader      *m_TrackMaker;        //Pointer to TrackMaker
   AMSR_SiHitReader      *m_SiHitMaker;        //Pointer to SiHitMaker
   AMSR_CTCClusterReader *m_CTCClusterMaker;   //Pointer to ToFClusterMaker
   AMSR_AntiClusterReader *m_AntiClusterMaker;   //Pointer to AntiClusterMaker
   AMSR_TrMCClusterReader *m_TrMCClusterMaker;   //Pointer to AntiClusterMaker
   AMSR_ParticleReader   *m_ParticleMaker;     //Pointer to ParticleMaker
   AMSR_MCParticleReader   *m_MCParticleMaker;     //Pointer to ParticleMaker

// AMSR_MCMaker          *m_MCMaker;           //Pointer to MCMaker
// AMSR_SiClusterMaker   *m_SiClusterMaker;    //Pointer to SiClusterMaker
// AMSR_CTCClusterMaker  *m_CTCClusterMaker;   //Pointer to CTCClusterMaker
// AMSR_ACCClusterMaker  *m_ACCClusterMaker;   //Pointer to ACCClusterMaker
// ATLFElectronMaker   *m_ElectronMaker;     //Pointer to ElectronMaker
// ATLFMuonMaker       *m_MuonMaker;         //Pointer to MuonMaker
// ATLFPhotonMaker     *m_PhotonMaker;       //Pointer to PhotonMaker
// ATLFJetMaker        *m_JetMaker;          //Pointer to JetMaker
// AMSR_TriggerMaker     *m_TriggerMaker;      //Pointer to TriggerMaker
// AMSR_MiscMaker        *m_MiscMaker;         //Pointer to MiscMaker
//    flags and switches
// Bool_t               m_Bfield;            //B-field (on=1,off=0)
// Int_t                m_TrackFinding;      //Track/finding on=1,off=0

   AMSR_HistBrowser       m_HistBrowser;       //Object to Browse Maker histograms
   AMSR_VirtualDisplay   *m_Display;         //!Pointer to Event display object

   void                 CommonConstruct();   //Common part of all constructors
//   Int_t                SelectedEvent;

public:
                      AMSR_Root();
                      AMSR_Root(const char *name, const char *title="The AMS Display with Root");
//   Int_t              SetSelectedEvent(const char * event);
           Int_t      IsGolden();
   virtual           ~AMSR_Root();
           Int_t      OpenDataFile(char * filename);
   virtual void       Browse(TBrowser *b);
   virtual Bool_t     GetEvent(Int_t event=1);      // *MENU*
   virtual Bool_t     GetEvent(Int_t run, Int_t event);      // *MENU*
//   virtual void       SelectEvent();      // *MENU*
   virtual void       Init(TTree * h1=0);           //Initialize to read from h1
   virtual void       Finish();                     //Finish a run
   AMSR_VirtualDisplay *Display() {return m_Display;}
   virtual void       Draw(Option_t *option="");    // *MENU*
   virtual void       Paint(Option_t *option="");
   virtual void       Clear(Option_t *option="");
   Bool_t             IsFolder() {return kTRUE;}
   virtual void       PrintInfo();

//
//    Getters
//
   Int_t          GetVersion()     {return m_Version;}
   Int_t          GetVersionDate() {return m_VersionDate;}
   Int_t          RunNum()         {return m_RunNum;}
   char*          GetTime()         {return ctime(m_time);}
   Int_t          EventNum()       {return m_EventNum;}
   Int_t          Event()          {return m_Event;}
   Int_t          NEvent()         {return m_NEvent;}
   Int_t          Mode()           {return m_Mode;}
   TTree         *Tree()           {return m_Tree;}
   TTree         *GetInput()       {return m_Input;}
   TList         *Makers()         {return m_Makers;}
   AMSR_Maker          *Maker(const char *name) {return (AMSR_Maker*)m_Makers->FindObject(name);}
   AMSR_ToFClusterReader *ToFClusterMaker() {return m_ToFClusterMaker;}
   AMSR_TrackReader      *TrackMaker()      {return m_TrackMaker;}
   AMSR_SiHitReader      *SiHitMaker()      {return m_SiHitMaker;}
   AMSR_AntiClusterReader *AntiClusterMaker() {return m_AntiClusterMaker;}
   AMSR_TrMCClusterReader *TrMCClusterMaker() {return m_TrMCClusterMaker;}
   AMSR_CTCClusterReader *CTCClusterMaker() {return m_CTCClusterMaker;}
   AMSR_ParticleReader   *ParticleMaker()   {return m_ParticleMaker;}
   AMSR_MCParticleReader   *MCParticleMaker()   {return m_MCParticleMaker;}
// AMSR_MCMaker         *MCMaker()         {return m_MCMaker;}
// AMSR_SiClusterMaker  *SiClusterMaker()  {return m_SiClusterMaker;}
// AMSR_CTCClusterMaker *CTCClusterMaker() {return m_CTCClusterMaker;}
// AMSR_ACCClusterMaker *ACCClusterMaker() {return m_ACCClusterMaker;}
// ATLFElectronMaker *ElectronMaker() {return m_ElectronMaker;}
// ATLFMuonMaker     *MuonMaker()     {return m_MuonMaker;}
// ATLFPhotonMaker   *PhotonMaker()   {return m_PhotonMaker;}
// ATLFJetMaker      *JetMaker()      {return m_JetMaker;}
//   AMSR_TrackMaker     *TrackMaker()    {return m_TrackMaker;}
//   AMSR_TriggerMaker   *TriggerMaker()  {return m_TriggerMaker;}
//   AMSR_MiscMaker      *MiscMaker()     {return m_MiscMaker;}

//   Bool_t            Bfield()       {return m_Bfield;}
//   Bool_t            TrackFinding() {return m_TrackFinding;}

//
//    Setters
//
   virtual void   SetDefaultParameters();
// virtual void   SetRunNum(Int_t run=1)     {m_RunNum=run;}
// virtual void   SetEventNum(Int_t event=1) {m_EventNum=event;}
   virtual void   SetEvent(Int_t event=1) {m_Event=event;}
   virtual void   SetMode(Int_t mode=0)   {m_Mode=mode;}
   void           SetDisplay(AMSR_VirtualDisplay *display) {m_Display=display;}
   virtual void   Make(Int_t i=0);
   void           MakeTree(const char* name="AMS Event", const char*title="AMSR_Root tree");
   void           FillTree();

//    Setters for flags and switches
//   void           SetBfield(Bool_t field=1)     {m_Bfield=field;}
//   void           SetTrackFinding(Bool_t val=0) {m_TrackFinding=val;}


   void           SortDown(Int_t n, Float_t *a, Int_t *index, Bool_t down=kTRUE);

   ClassDef(AMSR_Root, 1)   //AMSR_Root control class
};

EXTERN AMSR_Root *gAMSR_Root;

#endif
