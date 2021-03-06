/********************************************************
* AMSR_Cint.cxx
********************************************************/
#include "AMSR_Cint.h"

#ifdef G__MEMTEST
#undef malloc
#endif


extern "C" void G__set_cpp_environmentAMSR_Cint() {
  G__add_compiledheader("TROOT.h");
  G__add_compiledheader("TMemberInspector.h");
  G__add_compiledheader("TTree.h");
  G__add_compiledheader("TFile.h");
  G__add_compiledheader("TH1.h");
  G__add_compiledheader("TRadioButton.h");
  G__add_compiledheader("TSwitch.h");
  G__add_compiledheader("AMSR_Axis.h");
  G__add_compiledheader("AMSR_3DCluster.h");
  G__add_compiledheader("AMSR_Maker.h");
  G__add_compiledheader("AMSR_HistBrowser.h");
  G__add_compiledheader("AMSR_Root.h");
  G__add_compiledheader("AMSR_ToFCluster.h");
  G__add_compiledheader("AMSR_ToFClusterReader.h");
  G__add_compiledheader("AMSR_AntiCluster.h");
  G__add_compiledheader("AMSR_AntiClusterReader.h");
  G__add_compiledheader("AMSR_TrMCCluster.h");
  G__add_compiledheader("AMSR_TrMCClusterReader.h");
  G__add_compiledheader("AMSR_Track.h");
  G__add_compiledheader("AMSR_TrackReader.h");
  G__add_compiledheader("AMSR_SiHit.h");
  G__add_compiledheader("AMSR_SiHitReader.h");
  G__add_compiledheader("AMSR_CTCCluster.h");
  G__add_compiledheader("AMSR_CTCClusterReader.h");
  G__add_compiledheader("AMSR_Particle.h");
  G__add_compiledheader("AMSR_ParticleReader.h");
  G__add_compiledheader("AMSR_MCParticle.h");
  G__add_compiledheader("AMSR_MCParticleReader.h");
  G__add_compiledheader("AMSR_Ntuple.h");
  G__add_compiledheader("AMSR_VirtualDisplay.h");
  G__add_compiledheader("Debugger.h");
  G__add_compiledheader("AMSR_Canvas.h");
  G__add_compiledheader("AMSR_GeometrySetter.h");
  G__add_compiledheader("AMSR_Display.h");
  G__add_compiledheader("TGRunEventDialog.h");
}
int G__cpp_dllrevAMSR_Cint() { return(51111); }

/*********************************************************
* Member function Interface Method
*********************************************************/

/* TRadioButton */
static int G__TRadioButton_TRadioButton_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   TRadioButton *p=NULL;
   if(G__getaryconstruct()) p=new TRadioButton[G__getaryconstruct()];
   else                    p=new TRadioButton;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_TRadioButton);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TRadioButton_TRadioButton_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   TRadioButton *p=NULL;
   switch(libp->paran) {
   case 7:
      p = new TRadioButton(
(Bool_t*)G__int(libp->para[0]),(char*)G__int(libp->para[1])
,(Coord_t)G__double(libp->para[2]),(Coord_t)G__double(libp->para[3])
,(Coord_t)G__double(libp->para[4]),(Coord_t)G__double(libp->para[5])
,(char*)G__int(libp->para[6]));
      break;
   case 6:
      p = new TRadioButton(
(Bool_t*)G__int(libp->para[0]),(char*)G__int(libp->para[1])
,(Coord_t)G__double(libp->para[2]),(Coord_t)G__double(libp->para[3])
,(Coord_t)G__double(libp->para[4]),(Coord_t)G__double(libp->para[5]));
      break;
   }
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_TRadioButton);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TRadioButton_ExecuteEvent_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((TRadioButton*)(G__getstructoffset()))->ExecuteEvent((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TRadioButton_DeclFileName_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((TRadioButton*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TRadioButton_DeclFileLine_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((TRadioButton*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TRadioButton_ImplFileName_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((TRadioButton*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TRadioButton_ImplFileLine_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((TRadioButton*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TRadioButton_Class_Version_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((TRadioButton*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TRadioButton_Class_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((TRadioButton*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TRadioButton_Dictionary_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((TRadioButton*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TRadioButton_IsA_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((TRadioButton*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TRadioButton_ShowMembers_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((TRadioButton*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TRadioButton_Streamer_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((TRadioButton*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__TRadioButton_wATRadioButton_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (TRadioButton *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((TRadioButton *)((G__getstructoffset())+sizeof(TRadioButton)*i))->~TRadioButton();
   else if(G__PVOID==G__getgvp()) delete (TRadioButton *)(G__getstructoffset());
   else ((TRadioButton *)(G__getstructoffset()))->~TRadioButton();
   return(1 || funcname || hash || result7 || libp) ;
}


/* TSwitch */
static int G__TSwitch_TSwitch_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   TSwitch *p=NULL;
   if(G__getaryconstruct()) p=new TSwitch[G__getaryconstruct()];
   else                    p=new TSwitch;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_TSwitch);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TSwitch_TSwitch_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   TSwitch *p=NULL;
   switch(libp->paran) {
   case 8:
      p = new TSwitch(
(char*)G__int(libp->para[0]),(Bool_t*)G__int(libp->para[1])
,(char*)G__int(libp->para[2]),(Coord_t)G__double(libp->para[3])
,(Coord_t)G__double(libp->para[4]),(Coord_t)G__double(libp->para[5])
,(Coord_t)G__double(libp->para[6]),(char*)G__int(libp->para[7]));
      break;
   case 7:
      p = new TSwitch(
(char*)G__int(libp->para[0]),(Bool_t*)G__int(libp->para[1])
,(char*)G__int(libp->para[2]),(Coord_t)G__double(libp->para[3])
,(Coord_t)G__double(libp->para[4]),(Coord_t)G__double(libp->para[5])
,(Coord_t)G__double(libp->para[6]));
      break;
   }
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_TSwitch);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TSwitch_ExecuteEvent_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((TSwitch*)(G__getstructoffset()))->ExecuteEvent((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TSwitch_GetButton_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((TSwitch*)(G__getstructoffset()))->GetButton());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TSwitch_DeclFileName_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((TSwitch*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TSwitch_DeclFileLine_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((TSwitch*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TSwitch_ImplFileName_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((TSwitch*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TSwitch_ImplFileLine_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((TSwitch*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TSwitch_Class_Version_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((TSwitch*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TSwitch_Class_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((TSwitch*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TSwitch_Dictionary_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((TSwitch*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TSwitch_IsA_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((TSwitch*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TSwitch_ShowMembers_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((TSwitch*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TSwitch_Streamer_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((TSwitch*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__TSwitch_wATSwitch_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (TSwitch *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((TSwitch *)((G__getstructoffset())+sizeof(TSwitch)*i))->~TSwitch();
   else if(G__PVOID==G__getgvp()) delete (TSwitch *)(G__getstructoffset());
   else ((TSwitch *)(G__getstructoffset()))->~TSwitch();
   return(1 || funcname || hash || result7 || libp) ;
}


/* AMSR_Axis */
static int G__AMSR_Axis_AMSR_Axis_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_Axis *p=NULL;
   if(G__getaryconstruct()) p=new AMSR_Axis[G__getaryconstruct()];
   else                    p=new AMSR_Axis;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Axis);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Axis_AMSR_Axis_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_Axis *p=NULL;
      p = new AMSR_Axis((Float_t*)G__int(libp->para[0]),(char*)G__int(libp->para[1]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Axis);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Axis_AMSR_Axis_2_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_Axis *p=NULL;
      p = new AMSR_Axis(
(Float_t)G__double(libp->para[0]),(Float_t)G__double(libp->para[1])
,(Float_t)G__double(libp->para[2]),(char*)G__int(libp->para[3]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Axis);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Axis_GetObjectInfo_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_Axis*)(G__getstructoffset()))->GetObjectInfo((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Axis_Paint_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_Axis*)(G__getstructoffset()))->Paint((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_Axis*)(G__getstructoffset()))->Paint();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Axis_DeclFileName_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_Axis*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Axis_DeclFileLine_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Axis*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Axis_ImplFileName_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_Axis*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Axis_ImplFileLine_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Axis*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Axis_Class_Version_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSR_Axis*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Axis_Class_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Axis*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Axis_Dictionary_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Axis*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Axis_IsA_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Axis*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Axis_ShowMembers_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Axis*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Axis_Streamer_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Axis*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic copy constructor
static int G__AMSR_Axis_AMSR_Axis_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSR_Axis *p;
   if(1!=libp->paran) ;
   p=new AMSR_Axis(*(AMSR_Axis*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Axis);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__AMSR_Axis_wAAMSR_Axis_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSR_Axis *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSR_Axis *)((G__getstructoffset())+sizeof(AMSR_Axis)*i))->~AMSR_Axis();
   else if(G__PVOID==G__getgvp()) delete (AMSR_Axis *)(G__getstructoffset());
   else ((AMSR_Axis *)(G__getstructoffset()))->~AMSR_Axis();
   return(1 || funcname || hash || result7 || libp) ;
}


/* AMSR_Display */
static int G__AMSR_Display_AMSR_Display_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_Display *p=NULL;
   if(G__getaryconstruct()) p=new AMSR_Display[G__getaryconstruct()];
   else                    p=new AMSR_Display;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Display);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_AMSR_Display_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_Display *p=NULL;
   switch(libp->paran) {
   case 4:
      p = new AMSR_Display(
(const char*)G__int(libp->para[0]),(TGeometry*)G__int(libp->para[1])
,(int)G__int(libp->para[2]),(int)G__int(libp->para[3]));
      break;
   case 3:
      p = new AMSR_Display(
(const char*)G__int(libp->para[0]),(TGeometry*)G__int(libp->para[1])
,(int)G__int(libp->para[2]));
      break;
   case 2:
      p = new AMSR_Display((const char*)G__int(libp->para[0]),(TGeometry*)G__int(libp->para[1]));
      break;
   case 1:
      p = new AMSR_Display((const char*)G__int(libp->para[0]));
      break;
   }
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Display);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_AllViews_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((AMSR_Display*)(G__getstructoffset()))->AllViews());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_GetView_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Display*)(G__getstructoffset()))->GetView());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_Clear_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_DisplayButtons_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->DisplayButtons();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_DistancetoPrimitive_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Display*)(G__getstructoffset()))->DistancetoPrimitive((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_Draw_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->Draw((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->Draw();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_DrawAllViews_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->DrawAllViews();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_DrawFrontAndSideViews_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->DrawFrontAndSideViews();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_DrawParticles_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((AMSR_Display*)(G__getstructoffset()))->DrawParticles());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_DrawTitle_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->DrawTitle((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->DrawTitle();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_DrawEventInfo_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->DrawEventInfo((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->DrawEventInfo();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_DrawCaption_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->DrawCaption((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->DrawCaption();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_DrawAxis_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 2:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->DrawAxis((Int_t)G__int(libp->para[0]),(Option_t*)G__int(libp->para[1]));
      break;
   case 1:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->DrawAxis((Int_t)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->DrawAxis();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_DrawView_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 3:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->DrawView((Float_t)G__double(libp->para[0]),(Float_t)G__double(libp->para[1])
,(Int_t)G__int(libp->para[2]));
      break;
   case 2:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->DrawView((Float_t)G__double(libp->para[0]),(Float_t)G__double(libp->para[1]));
      break;
   case 1:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->DrawView((Float_t)G__double(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->DrawView();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_DrawViewGL_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->DrawViewGL();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_DrawViewX3D_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->DrawViewX3D();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_AddParticleInfo_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->AddParticleInfo();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_ExecuteEvent_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->ExecuteEvent((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_GotoRunEvent_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((AMSR_Display*)(G__getstructoffset()))->GotoRunEvent());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_Pad_2_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Display*)(G__getstructoffset()))->Pad());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_GetCanvas_3_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Display*)(G__getstructoffset()))->GetCanvas());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_GetObjInfoPad_4_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Display*)(G__getstructoffset()))->GetObjInfoPad());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_GetPartInfoPad_5_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Display*)(G__getstructoffset()))->GetPartInfoPad());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_GetEventInfoPad_6_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Display*)(G__getstructoffset()))->GetEventInfoPad());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_GetTitlePad_7_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Display*)(G__getstructoffset()))->GetTitlePad());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_Paint_8_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->Paint((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->Paint();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_PaintParticles_9_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->PaintParticles((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->PaintParticles();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_PTcut_0_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letdouble(result7,102,(double)((AMSR_Display*)(G__getstructoffset()))->PTcut());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_PTcutEGMUNU_1_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letdouble(result7,102,(double)((AMSR_Display*)(G__getstructoffset()))->PTcutEGMUNU());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_GetObjectInfo_2_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_Display*)(G__getstructoffset()))->GetObjectInfo((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_SetDrawParticles_3_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->SetDrawParticles((Bool_t)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->SetDrawParticles();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_SetPTcut_4_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->SetPTcut((Float_t)G__double(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->SetPTcut();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_SetPTcutEGMUNU_5_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->SetPTcutEGMUNU((Float_t)G__double(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->SetPTcutEGMUNU();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_SetGeometry_6_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->SetGeometry((TGeometry*)G__int(libp->para[0]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_SetIdleTimer_7_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->SetIdleTimer((Long_t)G__int(libp->para[0]),(Text_t*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_IsIdleOn_8_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((AMSR_Display*)(G__getstructoffset()))->IsIdleOn());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_IdleSwitch_9_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->IdleSwitch((Int_t)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->IdleSwitch();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_IdleTime_0_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,108,(long)((AMSR_Display*)(G__getstructoffset()))->IdleTime());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_IdleCommand_1_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_Display*)(G__getstructoffset()))->IdleCommand());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_IdleTime_2_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->IdleTime((Long_t)G__int(libp->para[0]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_IdleCommand_3_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->IdleCommand((Text_t*)G__int(libp->para[0]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_Print_4_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->Print();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_SavePS_5_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->SavePS();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_SaveGIF_6_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->SaveGIF();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_SetView_7_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->SetView((Float_t)G__double(libp->para[0]),(Float_t)G__double(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_SetView_8_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->SetView((EAMSR_View)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->SetView();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_SetNextView_9_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->SetNextView((EAMSR_View)G__int(libp->para[0]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_ShowNextEvent_0_5(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->ShowNextEvent((Int_t)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->ShowNextEvent();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_DrawEvent_1_5(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->DrawEvent();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_SizeParticles_2_5(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->SizeParticles();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_GetGeometrySetter_3_5(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Display*)(G__getstructoffset()))->GetGeometrySetter());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_SetGeo_4_5(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->SetGeo();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_DeclFileName_5_5(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_Display*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_DeclFileLine_6_5(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Display*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_ImplFileName_7_5(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_Display*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_ImplFileLine_8_5(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Display*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_Class_Version_9_5(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSR_Display*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_Class_0_6(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Display*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_Dictionary_1_6(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_IsA_2_6(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Display*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_ShowMembers_3_6(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Display_Streamer_4_6(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Display*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic copy constructor
static int G__AMSR_Display_AMSR_Display_5_6(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSR_Display *p;
   if(1!=libp->paran) ;
   p=new AMSR_Display(*(AMSR_Display*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Display);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__AMSR_Display_wAAMSR_Display_6_6(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSR_Display *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSR_Display *)((G__getstructoffset())+sizeof(AMSR_Display)*i))->~AMSR_Display();
   else if(G__PVOID==G__getgvp()) delete (AMSR_Display *)(G__getstructoffset());
   else ((AMSR_Display *)(G__getstructoffset()))->~AMSR_Display();
   return(1 || funcname || hash || result7 || libp) ;
}


/* AMSR_3DCluster */
static int G__AMSR_3DCluster_AMSR_3DCluster_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_3DCluster *p=NULL;
   if(G__getaryconstruct()) p=new AMSR_3DCluster[G__getaryconstruct()];
   else                    p=new AMSR_3DCluster;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_3DCluster);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_3DCluster_AMSR_3DCluster_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_3DCluster *p=NULL;
   switch(libp->paran) {
   case 9:
      p = new AMSR_3DCluster(
(Float_t)G__double(libp->para[0]),(Float_t)G__double(libp->para[1])
,(Float_t)G__double(libp->para[2]),(Float_t)G__double(libp->para[3])
,(Float_t)G__double(libp->para[4]),(Float_t)G__double(libp->para[5])
,(Float_t)G__double(libp->para[6]),(Float_t)G__double(libp->para[7])
,(Int_t)G__int(libp->para[8]));
      break;
   case 8:
      p = new AMSR_3DCluster(
(Float_t)G__double(libp->para[0]),(Float_t)G__double(libp->para[1])
,(Float_t)G__double(libp->para[2]),(Float_t)G__double(libp->para[3])
,(Float_t)G__double(libp->para[4]),(Float_t)G__double(libp->para[5])
,(Float_t)G__double(libp->para[6]),(Float_t)G__double(libp->para[7]));
      break;
   }
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_3DCluster);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_3DCluster_AMSR_3DCluster_2_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_3DCluster *p=NULL;
   switch(libp->paran) {
   case 4:
      p = new AMSR_3DCluster(
(Float_t*)G__int(libp->para[0]),(Float_t*)G__int(libp->para[1])
,(Float_t*)G__int(libp->para[2]),(Int_t)G__int(libp->para[3]));
      break;
   case 3:
      p = new AMSR_3DCluster(
(Float_t*)G__int(libp->para[0]),(Float_t*)G__int(libp->para[1])
,(Float_t*)G__int(libp->para[2]));
      break;
   }
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_3DCluster);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_3DCluster_Delete_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_3DCluster*)(G__getstructoffset()))->Delete((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_3DCluster*)(G__getstructoffset()))->Delete();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_3DCluster_GetObjectInfo_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_3DCluster*)(G__getstructoffset()))->GetObjectInfo((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_3DCluster_GetPosition_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,70,(long)((AMSR_3DCluster*)(G__getstructoffset()))->GetPosition());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_3DCluster_GetSize_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,70,(long)((AMSR_3DCluster*)(G__getstructoffset()))->GetSize());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_3DCluster_DistancetoPrimitive_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_3DCluster*)(G__getstructoffset()))->DistancetoPrimitive((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_3DCluster_SetPoints_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_3DCluster*)(G__getstructoffset()))->SetPoints((Float_t*)G__int(libp->para[0]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_3DCluster_SetPosition_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_3DCluster*)(G__getstructoffset()))->SetPosition((Float_t)G__double(libp->para[0]),(Float_t)G__double(libp->para[1])
,(Float_t)G__double(libp->para[2]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_3DCluster_SetDirection_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_3DCluster*)(G__getstructoffset()))->SetDirection((Float_t)G__double(libp->para[0]),(Float_t)G__double(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_3DCluster_Sizeof3D_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_3DCluster*)(G__getstructoffset()))->Sizeof3D();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_3DCluster_DeclFileName_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_3DCluster*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_3DCluster_DeclFileLine_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_3DCluster*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_3DCluster_ImplFileName_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_3DCluster*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_3DCluster_ImplFileLine_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_3DCluster*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_3DCluster_Class_Version_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSR_3DCluster*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_3DCluster_Class_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_3DCluster*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_3DCluster_Dictionary_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_3DCluster*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_3DCluster_IsA_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_3DCluster*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_3DCluster_ShowMembers_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_3DCluster*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_3DCluster_Streamer_2_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_3DCluster*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic copy constructor
static int G__AMSR_3DCluster_AMSR_3DCluster_3_2(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSR_3DCluster *p;
   if(1!=libp->paran) ;
   p=new AMSR_3DCluster(*(AMSR_3DCluster*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_3DCluster);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__AMSR_3DCluster_wAAMSR_3DCluster_4_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSR_3DCluster *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSR_3DCluster *)((G__getstructoffset())+sizeof(AMSR_3DCluster)*i))->~AMSR_3DCluster();
   else if(G__PVOID==G__getgvp()) delete (AMSR_3DCluster *)(G__getstructoffset());
   else ((AMSR_3DCluster *)(G__getstructoffset()))->~AMSR_3DCluster();
   return(1 || funcname || hash || result7 || libp) ;
}


/* AMSR_Maker */
static int G__AMSR_Maker_Finish_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Maker*)(G__getstructoffset()))->Finish();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Maker_Browse_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Maker*)(G__getstructoffset()))->Browse((TBrowser*)G__int(libp->para[0]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Maker_Draw_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_Maker*)(G__getstructoffset()))->Draw((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_Maker*)(G__getstructoffset()))->Draw();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Maker_Clear_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_Maker*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_Maker*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Maker_IsFolder_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((AMSR_Maker*)(G__getstructoffset()))->IsFolder());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Maker_Histograms_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Maker*)(G__getstructoffset()))->Histograms());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Maker_Fruits_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Maker*)(G__getstructoffset()))->Fruits());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Maker_PrintInfo_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Maker*)(G__getstructoffset()))->PrintInfo();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Maker_Enabled_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((AMSR_Maker*)(G__getstructoffset()))->Enabled((TObject*)G__int(libp->para[0])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Maker_Save_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_Maker*)(G__getstructoffset()))->Save((Int_t)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_Maker*)(G__getstructoffset()))->Save();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Maker_MakeBranch_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Maker*)(G__getstructoffset()))->MakeBranch();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Maker_Make_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Maker*)(G__getstructoffset()))->Make();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Maker_DeclFileName_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_Maker*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Maker_DeclFileLine_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Maker*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Maker_ImplFileName_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_Maker*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Maker_ImplFileLine_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Maker*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Maker_Class_Version_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSR_Maker*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Maker_Class_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Maker*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Maker_Dictionary_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Maker*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Maker_IsA_2_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Maker*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Maker_ShowMembers_3_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Maker*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Maker_Streamer_4_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Maker*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__AMSR_Maker_wAAMSR_Maker_5_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSR_Maker *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSR_Maker *)((G__getstructoffset())+sizeof(AMSR_Maker)*i))->~AMSR_Maker();
   else if(G__PVOID==G__getgvp()) delete (AMSR_Maker *)(G__getstructoffset());
   else ((AMSR_Maker *)(G__getstructoffset()))->~AMSR_Maker();
   return(1 || funcname || hash || result7 || libp) ;
}


/* AMSR_HistBrowser */
static int G__AMSR_HistBrowser_AMSR_HistBrowser_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_HistBrowser *p=NULL;
   if(G__getaryconstruct()) p=new AMSR_HistBrowser[G__getaryconstruct()];
   else                    p=new AMSR_HistBrowser;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_HistBrowser);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_HistBrowser_Browse_2_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_HistBrowser*)(G__getstructoffset()))->Browse((TBrowser*)G__int(libp->para[0]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_HistBrowser_IsFolder_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((AMSR_HistBrowser*)(G__getstructoffset()))->IsFolder());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_HistBrowser_DeclFileName_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_HistBrowser*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_HistBrowser_DeclFileLine_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_HistBrowser*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_HistBrowser_ImplFileName_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_HistBrowser*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_HistBrowser_ImplFileLine_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_HistBrowser*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_HistBrowser_Class_Version_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSR_HistBrowser*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_HistBrowser_Class_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_HistBrowser*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_HistBrowser_Dictionary_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_HistBrowser*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_HistBrowser_IsA_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_HistBrowser*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_HistBrowser_ShowMembers_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_HistBrowser*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_HistBrowser_Streamer_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_HistBrowser*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic copy constructor
static int G__AMSR_HistBrowser_AMSR_HistBrowser_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSR_HistBrowser *p;
   if(1!=libp->paran) ;
   p=new AMSR_HistBrowser(*(AMSR_HistBrowser*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_HistBrowser);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__AMSR_HistBrowser_wAAMSR_HistBrowser_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSR_HistBrowser *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSR_HistBrowser *)((G__getstructoffset())+sizeof(AMSR_HistBrowser)*i))->~AMSR_HistBrowser();
   else if(G__PVOID==G__getgvp()) delete (AMSR_HistBrowser *)(G__getstructoffset());
   else ((AMSR_HistBrowser *)(G__getstructoffset()))->~AMSR_HistBrowser();
   return(1 || funcname || hash || result7 || libp) ;
}


/* AMSR_ToFClusterReader */
static int G__AMSR_ToFClusterReader_AMSR_ToFClusterReader_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_ToFClusterReader *p=NULL;
   if(G__getaryconstruct()) p=new AMSR_ToFClusterReader[G__getaryconstruct()];
   else                    p=new AMSR_ToFClusterReader;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ToFClusterReader);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFClusterReader_AMSR_ToFClusterReader_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_ToFClusterReader *p=NULL;
      p = new AMSR_ToFClusterReader((const char*)G__int(libp->para[0]),(const char*)G__int(libp->para[1]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ToFClusterReader);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFClusterReader_Finish_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_ToFClusterReader*)(G__getstructoffset()))->Finish();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFClusterReader_Make_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_ToFClusterReader*)(G__getstructoffset()))->Make();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFClusterReader_PrintInfo_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_ToFClusterReader*)(G__getstructoffset()))->PrintInfo();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFClusterReader_AddCluster_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 12:
      G__setnull(result7);
      ((AMSR_ToFClusterReader*)(G__getstructoffset()))->AddCluster(
(Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]),(Float_t)G__double(libp->para[3])
,(Float_t)G__double(libp->para[4]),(Float_t)G__double(libp->para[5])
,(Float_t*)G__int(libp->para[6]),(Float_t*)G__int(libp->para[7])
,(Int_t)G__int(libp->para[8]),(Int_t)G__int(libp->para[9])
,(Int_t)G__int(libp->para[10]),(TObjArray*)G__int(libp->para[11]));
      break;
   case 11:
      G__setnull(result7);
      ((AMSR_ToFClusterReader*)(G__getstructoffset()))->AddCluster(
(Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]),(Float_t)G__double(libp->para[3])
,(Float_t)G__double(libp->para[4]),(Float_t)G__double(libp->para[5])
,(Float_t*)G__int(libp->para[6]),(Float_t*)G__int(libp->para[7])
,(Int_t)G__int(libp->para[8]),(Int_t)G__int(libp->para[9])
,(Int_t)G__int(libp->para[10]));
      break;
   case 10:
      G__setnull(result7);
      ((AMSR_ToFClusterReader*)(G__getstructoffset()))->AddCluster(
(Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]),(Float_t)G__double(libp->para[3])
,(Float_t)G__double(libp->para[4]),(Float_t)G__double(libp->para[5])
,(Float_t*)G__int(libp->para[6]),(Float_t*)G__int(libp->para[7])
,(Int_t)G__int(libp->para[8]),(Int_t)G__int(libp->para[9]));
      break;
   case 9:
      G__setnull(result7);
      ((AMSR_ToFClusterReader*)(G__getstructoffset()))->AddCluster(
(Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]),(Float_t)G__double(libp->para[3])
,(Float_t)G__double(libp->para[4]),(Float_t)G__double(libp->para[5])
,(Float_t*)G__int(libp->para[6]),(Float_t*)G__int(libp->para[7])
,(Int_t)G__int(libp->para[8]));
      break;
   case 8:
      G__setnull(result7);
      ((AMSR_ToFClusterReader*)(G__getstructoffset()))->AddCluster(
(Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]),(Float_t)G__double(libp->para[3])
,(Float_t)G__double(libp->para[4]),(Float_t)G__double(libp->para[5])
,(Float_t*)G__int(libp->para[6]),(Float_t*)G__int(libp->para[7]));
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFClusterReader_RemoveCluster_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_ToFClusterReader*)(G__getstructoffset()))->RemoveCluster((const Int_t)G__int(libp->para[0]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFClusterReader_Clear_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_ToFClusterReader*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_ToFClusterReader*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFClusterReader_DeclFileName_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_ToFClusterReader*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFClusterReader_DeclFileLine_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_ToFClusterReader*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFClusterReader_ImplFileName_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_ToFClusterReader*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFClusterReader_ImplFileLine_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_ToFClusterReader*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFClusterReader_Class_Version_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSR_ToFClusterReader*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFClusterReader_Class_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_ToFClusterReader*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFClusterReader_Dictionary_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_ToFClusterReader*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFClusterReader_IsA_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_ToFClusterReader*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFClusterReader_ShowMembers_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_ToFClusterReader*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFClusterReader_Streamer_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_ToFClusterReader*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic copy constructor
static int G__AMSR_ToFClusterReader_AMSR_ToFClusterReader_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSR_ToFClusterReader *p;
   if(1!=libp->paran) ;
   p=new AMSR_ToFClusterReader(*(AMSR_ToFClusterReader*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ToFClusterReader);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__AMSR_ToFClusterReader_wAAMSR_ToFClusterReader_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSR_ToFClusterReader *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSR_ToFClusterReader *)((G__getstructoffset())+sizeof(AMSR_ToFClusterReader)*i))->~AMSR_ToFClusterReader();
   else if(G__PVOID==G__getgvp()) delete (AMSR_ToFClusterReader *)(G__getstructoffset());
   else ((AMSR_ToFClusterReader *)(G__getstructoffset()))->~AMSR_ToFClusterReader();
   return(1 || funcname || hash || result7 || libp) ;
}


/* AMSR_TrackReader */
static int G__AMSR_TrackReader_AMSR_TrackReader_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_TrackReader *p=NULL;
   if(G__getaryconstruct()) p=new AMSR_TrackReader[G__getaryconstruct()];
   else                    p=new AMSR_TrackReader;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrackReader);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrackReader_AMSR_TrackReader_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_TrackReader *p=NULL;
      p = new AMSR_TrackReader((const char*)G__int(libp->para[0]),(const char*)G__int(libp->para[1]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrackReader);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrackReader_AddTrack_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_TrackReader*)(G__getstructoffset()))->AddTrack((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrackReader_Clear_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_TrackReader*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_TrackReader*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrackReader_Finish_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_TrackReader*)(G__getstructoffset()))->Finish();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrackReader_Make_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_TrackReader*)(G__getstructoffset()))->Make();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrackReader_PrintInfo_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_TrackReader*)(G__getstructoffset()))->PrintInfo();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrackReader_Mult_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_TrackReader*)(G__getstructoffset()))->Mult());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrackReader_DeclFileName_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_TrackReader*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrackReader_DeclFileLine_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_TrackReader*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrackReader_ImplFileName_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_TrackReader*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrackReader_ImplFileLine_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_TrackReader*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrackReader_Class_Version_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSR_TrackReader*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrackReader_Class_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_TrackReader*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrackReader_Dictionary_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_TrackReader*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrackReader_IsA_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_TrackReader*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrackReader_ShowMembers_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_TrackReader*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrackReader_Streamer_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_TrackReader*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic copy constructor
static int G__AMSR_TrackReader_AMSR_TrackReader_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSR_TrackReader *p;
   if(1!=libp->paran) ;
   p=new AMSR_TrackReader(*(AMSR_TrackReader*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrackReader);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__AMSR_TrackReader_wAAMSR_TrackReader_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSR_TrackReader *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSR_TrackReader *)((G__getstructoffset())+sizeof(AMSR_TrackReader)*i))->~AMSR_TrackReader();
   else if(G__PVOID==G__getgvp()) delete (AMSR_TrackReader *)(G__getstructoffset());
   else ((AMSR_TrackReader *)(G__getstructoffset()))->~AMSR_TrackReader();
   return(1 || funcname || hash || result7 || libp) ;
}


/* AMSR_SiHitReader */
static int G__AMSR_SiHitReader_AMSR_SiHitReader_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_SiHitReader *p=NULL;
   if(G__getaryconstruct()) p=new AMSR_SiHitReader[G__getaryconstruct()];
   else                    p=new AMSR_SiHitReader;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_SiHitReader);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHitReader_AMSR_SiHitReader_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_SiHitReader *p=NULL;
      p = new AMSR_SiHitReader((const char*)G__int(libp->para[0]),(const char*)G__int(libp->para[1]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_SiHitReader);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHitReader_Finish_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_SiHitReader*)(G__getstructoffset()))->Finish();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHitReader_Make_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_SiHitReader*)(G__getstructoffset()))->Make();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHitReader_PrintInfo_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_SiHitReader*)(G__getstructoffset()))->PrintInfo();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHitReader_Enabled_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((AMSR_SiHitReader*)(G__getstructoffset()))->Enabled((TObject*)G__int(libp->para[0])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHitReader_AddCluster_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 9:
      G__setnull(result7);
      ((AMSR_SiHitReader*)(G__getstructoffset()))->AddCluster(
(Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]),(Int_t)G__int(libp->para[3])
,(Float_t*)G__int(libp->para[4]),(Float_t*)G__int(libp->para[5])
,(Float_t)G__double(libp->para[6]),(Float_t)G__double(libp->para[7])
,(TObjArray*)G__int(libp->para[8]));
      break;
   case 8:
      G__setnull(result7);
      ((AMSR_SiHitReader*)(G__getstructoffset()))->AddCluster(
(Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]),(Int_t)G__int(libp->para[3])
,(Float_t*)G__int(libp->para[4]),(Float_t*)G__int(libp->para[5])
,(Float_t)G__double(libp->para[6]),(Float_t)G__double(libp->para[7]));
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHitReader_RemoveCluster_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_SiHitReader*)(G__getstructoffset()))->RemoveCluster((const Int_t)G__int(libp->para[0]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHitReader_Clear_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_SiHitReader*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_SiHitReader*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHitReader_DeclFileName_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_SiHitReader*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHitReader_DeclFileLine_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_SiHitReader*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHitReader_ImplFileName_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_SiHitReader*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHitReader_ImplFileLine_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_SiHitReader*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHitReader_Class_Version_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSR_SiHitReader*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHitReader_Class_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_SiHitReader*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHitReader_Dictionary_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_SiHitReader*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHitReader_IsA_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_SiHitReader*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHitReader_ShowMembers_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_SiHitReader*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHitReader_Streamer_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_SiHitReader*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic copy constructor
static int G__AMSR_SiHitReader_AMSR_SiHitReader_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSR_SiHitReader *p;
   if(1!=libp->paran) ;
   p=new AMSR_SiHitReader(*(AMSR_SiHitReader*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_SiHitReader);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__AMSR_SiHitReader_wAAMSR_SiHitReader_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSR_SiHitReader *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSR_SiHitReader *)((G__getstructoffset())+sizeof(AMSR_SiHitReader)*i))->~AMSR_SiHitReader();
   else if(G__PVOID==G__getgvp()) delete (AMSR_SiHitReader *)(G__getstructoffset());
   else ((AMSR_SiHitReader *)(G__getstructoffset()))->~AMSR_SiHitReader();
   return(1 || funcname || hash || result7 || libp) ;
}


/* AMSR_CTCClusterReader */
static int G__AMSR_CTCClusterReader_AMSR_CTCClusterReader_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_CTCClusterReader *p=NULL;
   if(G__getaryconstruct()) p=new AMSR_CTCClusterReader[G__getaryconstruct()];
   else                    p=new AMSR_CTCClusterReader;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_CTCClusterReader);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCClusterReader_AMSR_CTCClusterReader_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_CTCClusterReader *p=NULL;
      p = new AMSR_CTCClusterReader((const char*)G__int(libp->para[0]),(const char*)G__int(libp->para[1]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_CTCClusterReader);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCClusterReader_Finish_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_CTCClusterReader*)(G__getstructoffset()))->Finish();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCClusterReader_Make_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_CTCClusterReader*)(G__getstructoffset()))->Make();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCClusterReader_PrintInfo_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_CTCClusterReader*)(G__getstructoffset()))->PrintInfo();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCClusterReader_AddCluster_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 9:
      G__setnull(result7);
      ((AMSR_CTCClusterReader*)(G__getstructoffset()))->AddCluster(
(Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Float_t)G__double(libp->para[2]),(Float_t)G__double(libp->para[3])
,(Float_t)G__double(libp->para[4]),(Float_t*)G__int(libp->para[5])
,(Float_t*)G__int(libp->para[6]),(Int_t)G__int(libp->para[7])
,(TObjArray*)G__int(libp->para[8]));
      break;
   case 8:
      G__setnull(result7);
      ((AMSR_CTCClusterReader*)(G__getstructoffset()))->AddCluster(
(Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Float_t)G__double(libp->para[2]),(Float_t)G__double(libp->para[3])
,(Float_t)G__double(libp->para[4]),(Float_t*)G__int(libp->para[5])
,(Float_t*)G__int(libp->para[6]),(Int_t)G__int(libp->para[7]));
      break;
   case 7:
      G__setnull(result7);
      ((AMSR_CTCClusterReader*)(G__getstructoffset()))->AddCluster(
(Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Float_t)G__double(libp->para[2]),(Float_t)G__double(libp->para[3])
,(Float_t)G__double(libp->para[4]),(Float_t*)G__int(libp->para[5])
,(Float_t*)G__int(libp->para[6]));
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCClusterReader_RemoveCluster_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_CTCClusterReader*)(G__getstructoffset()))->RemoveCluster((const Int_t)G__int(libp->para[0]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCClusterReader_Clear_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_CTCClusterReader*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_CTCClusterReader*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCClusterReader_DeclFileName_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_CTCClusterReader*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCClusterReader_DeclFileLine_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_CTCClusterReader*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCClusterReader_ImplFileName_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_CTCClusterReader*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCClusterReader_ImplFileLine_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_CTCClusterReader*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCClusterReader_Class_Version_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSR_CTCClusterReader*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCClusterReader_Class_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_CTCClusterReader*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCClusterReader_Dictionary_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_CTCClusterReader*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCClusterReader_IsA_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_CTCClusterReader*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCClusterReader_ShowMembers_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_CTCClusterReader*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCClusterReader_Streamer_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_CTCClusterReader*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic copy constructor
static int G__AMSR_CTCClusterReader_AMSR_CTCClusterReader_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSR_CTCClusterReader *p;
   if(1!=libp->paran) ;
   p=new AMSR_CTCClusterReader(*(AMSR_CTCClusterReader*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_CTCClusterReader);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__AMSR_CTCClusterReader_wAAMSR_CTCClusterReader_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSR_CTCClusterReader *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSR_CTCClusterReader *)((G__getstructoffset())+sizeof(AMSR_CTCClusterReader)*i))->~AMSR_CTCClusterReader();
   else if(G__PVOID==G__getgvp()) delete (AMSR_CTCClusterReader *)(G__getstructoffset());
   else ((AMSR_CTCClusterReader *)(G__getstructoffset()))->~AMSR_CTCClusterReader();
   return(1 || funcname || hash || result7 || libp) ;
}


/* AMSR_AntiClusterReader */
static int G__AMSR_AntiClusterReader_AMSR_AntiClusterReader_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_AntiClusterReader *p=NULL;
   if(G__getaryconstruct()) p=new AMSR_AntiClusterReader[G__getaryconstruct()];
   else                    p=new AMSR_AntiClusterReader;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_AntiClusterReader);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiClusterReader_AMSR_AntiClusterReader_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_AntiClusterReader *p=NULL;
      p = new AMSR_AntiClusterReader((const char*)G__int(libp->para[0]),(const char*)G__int(libp->para[1]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_AntiClusterReader);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiClusterReader_Finish_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_AntiClusterReader*)(G__getstructoffset()))->Finish();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiClusterReader_Make_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_AntiClusterReader*)(G__getstructoffset()))->Make();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiClusterReader_PrintInfo_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_AntiClusterReader*)(G__getstructoffset()))->PrintInfo();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiClusterReader_AddCluster_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 7:
      G__setnull(result7);
      ((AMSR_AntiClusterReader*)(G__getstructoffset()))->AddCluster(
(Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Float_t)G__double(libp->para[2]),(Float_t*)G__int(libp->para[3])
,(Float_t*)G__int(libp->para[4]),(Int_t)G__int(libp->para[5])
,(TObjArray*)G__int(libp->para[6]));
      break;
   case 6:
      G__setnull(result7);
      ((AMSR_AntiClusterReader*)(G__getstructoffset()))->AddCluster((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Float_t)G__double(libp->para[2]),(Float_t*)G__int(libp->para[3])
,(Float_t*)G__int(libp->para[4]),(Int_t)G__int(libp->para[5]));
      break;
   case 5:
      G__setnull(result7);
      ((AMSR_AntiClusterReader*)(G__getstructoffset()))->AddCluster((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Float_t)G__double(libp->para[2]),(Float_t*)G__int(libp->para[3])
,(Float_t*)G__int(libp->para[4]));
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiClusterReader_RemoveCluster_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_AntiClusterReader*)(G__getstructoffset()))->RemoveCluster((const Int_t)G__int(libp->para[0]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiClusterReader_Clear_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_AntiClusterReader*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_AntiClusterReader*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiClusterReader_DeclFileName_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_AntiClusterReader*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiClusterReader_DeclFileLine_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_AntiClusterReader*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiClusterReader_ImplFileName_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_AntiClusterReader*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiClusterReader_ImplFileLine_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_AntiClusterReader*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiClusterReader_Class_Version_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSR_AntiClusterReader*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiClusterReader_Class_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_AntiClusterReader*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiClusterReader_Dictionary_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_AntiClusterReader*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiClusterReader_IsA_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_AntiClusterReader*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiClusterReader_ShowMembers_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_AntiClusterReader*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiClusterReader_Streamer_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_AntiClusterReader*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic copy constructor
static int G__AMSR_AntiClusterReader_AMSR_AntiClusterReader_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSR_AntiClusterReader *p;
   if(1!=libp->paran) ;
   p=new AMSR_AntiClusterReader(*(AMSR_AntiClusterReader*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_AntiClusterReader);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__AMSR_AntiClusterReader_wAAMSR_AntiClusterReader_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSR_AntiClusterReader *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSR_AntiClusterReader *)((G__getstructoffset())+sizeof(AMSR_AntiClusterReader)*i))->~AMSR_AntiClusterReader();
   else if(G__PVOID==G__getgvp()) delete (AMSR_AntiClusterReader *)(G__getstructoffset());
   else ((AMSR_AntiClusterReader *)(G__getstructoffset()))->~AMSR_AntiClusterReader();
   return(1 || funcname || hash || result7 || libp) ;
}


/* AMSR_TrMCClusterReader */
static int G__AMSR_TrMCClusterReader_AMSR_TrMCClusterReader_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_TrMCClusterReader *p=NULL;
   if(G__getaryconstruct()) p=new AMSR_TrMCClusterReader[G__getaryconstruct()];
   else                    p=new AMSR_TrMCClusterReader;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrMCClusterReader);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCClusterReader_AMSR_TrMCClusterReader_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_TrMCClusterReader *p=NULL;
      p = new AMSR_TrMCClusterReader((const char*)G__int(libp->para[0]),(const char*)G__int(libp->para[1]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrMCClusterReader);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCClusterReader_Finish_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_TrMCClusterReader*)(G__getstructoffset()))->Finish();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCClusterReader_Make_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_TrMCClusterReader*)(G__getstructoffset()))->Make();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCClusterReader_PrintInfo_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_TrMCClusterReader*)(G__getstructoffset()))->PrintInfo();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCClusterReader_AddCluster_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 6:
      G__setnull(result7);
      ((AMSR_TrMCClusterReader*)(G__getstructoffset()))->AddCluster((Int_t)G__int(libp->para[0]),(Float_t)G__double(libp->para[1])
,(Float_t*)G__int(libp->para[2]),(Float_t*)G__int(libp->para[3])
,(Int_t)G__int(libp->para[4]),(TObjArray*)G__int(libp->para[5]));
      break;
   case 5:
      G__setnull(result7);
      ((AMSR_TrMCClusterReader*)(G__getstructoffset()))->AddCluster((Int_t)G__int(libp->para[0]),(Float_t)G__double(libp->para[1])
,(Float_t*)G__int(libp->para[2]),(Float_t*)G__int(libp->para[3])
,(Int_t)G__int(libp->para[4]));
      break;
   case 4:
      G__setnull(result7);
      ((AMSR_TrMCClusterReader*)(G__getstructoffset()))->AddCluster((Int_t)G__int(libp->para[0]),(Float_t)G__double(libp->para[1])
,(Float_t*)G__int(libp->para[2]),(Float_t*)G__int(libp->para[3]));
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCClusterReader_RemoveCluster_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_TrMCClusterReader*)(G__getstructoffset()))->RemoveCluster((const Int_t)G__int(libp->para[0]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCClusterReader_Clear_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_TrMCClusterReader*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_TrMCClusterReader*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCClusterReader_DeclFileName_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_TrMCClusterReader*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCClusterReader_DeclFileLine_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_TrMCClusterReader*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCClusterReader_ImplFileName_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_TrMCClusterReader*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCClusterReader_ImplFileLine_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_TrMCClusterReader*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCClusterReader_Class_Version_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSR_TrMCClusterReader*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCClusterReader_Class_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_TrMCClusterReader*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCClusterReader_Dictionary_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_TrMCClusterReader*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCClusterReader_IsA_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_TrMCClusterReader*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCClusterReader_ShowMembers_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_TrMCClusterReader*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCClusterReader_Streamer_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_TrMCClusterReader*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic copy constructor
static int G__AMSR_TrMCClusterReader_AMSR_TrMCClusterReader_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSR_TrMCClusterReader *p;
   if(1!=libp->paran) ;
   p=new AMSR_TrMCClusterReader(*(AMSR_TrMCClusterReader*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrMCClusterReader);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__AMSR_TrMCClusterReader_wAAMSR_TrMCClusterReader_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSR_TrMCClusterReader *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSR_TrMCClusterReader *)((G__getstructoffset())+sizeof(AMSR_TrMCClusterReader)*i))->~AMSR_TrMCClusterReader();
   else if(G__PVOID==G__getgvp()) delete (AMSR_TrMCClusterReader *)(G__getstructoffset());
   else ((AMSR_TrMCClusterReader *)(G__getstructoffset()))->~AMSR_TrMCClusterReader();
   return(1 || funcname || hash || result7 || libp) ;
}


/* AMSR_ParticleReader */
static int G__AMSR_ParticleReader_AMSR_ParticleReader_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_ParticleReader *p=NULL;
   if(G__getaryconstruct()) p=new AMSR_ParticleReader[G__getaryconstruct()];
   else                    p=new AMSR_ParticleReader;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ParticleReader);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ParticleReader_AMSR_ParticleReader_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_ParticleReader *p=NULL;
      p = new AMSR_ParticleReader((const char*)G__int(libp->para[0]),(const char*)G__int(libp->para[1]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ParticleReader);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ParticleReader_AddParticle_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_ParticleReader*)(G__getstructoffset()))->AddParticle((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ParticleReader_Clear_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_ParticleReader*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_ParticleReader*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ParticleReader_Finish_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_ParticleReader*)(G__getstructoffset()))->Finish();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ParticleReader_Make_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_ParticleReader*)(G__getstructoffset()))->Make();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ParticleReader_PrintInfo_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_ParticleReader*)(G__getstructoffset()))->PrintInfo();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ParticleReader_Mult_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_ParticleReader*)(G__getstructoffset()))->Mult());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ParticleReader_DeclFileName_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_ParticleReader*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ParticleReader_DeclFileLine_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_ParticleReader*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ParticleReader_ImplFileName_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_ParticleReader*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ParticleReader_ImplFileLine_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_ParticleReader*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ParticleReader_Class_Version_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSR_ParticleReader*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ParticleReader_Class_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_ParticleReader*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ParticleReader_Dictionary_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_ParticleReader*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ParticleReader_IsA_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_ParticleReader*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ParticleReader_ShowMembers_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_ParticleReader*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ParticleReader_Streamer_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_ParticleReader*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic copy constructor
static int G__AMSR_ParticleReader_AMSR_ParticleReader_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSR_ParticleReader *p;
   if(1!=libp->paran) ;
   p=new AMSR_ParticleReader(*(AMSR_ParticleReader*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ParticleReader);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__AMSR_ParticleReader_wAAMSR_ParticleReader_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSR_ParticleReader *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSR_ParticleReader *)((G__getstructoffset())+sizeof(AMSR_ParticleReader)*i))->~AMSR_ParticleReader();
   else if(G__PVOID==G__getgvp()) delete (AMSR_ParticleReader *)(G__getstructoffset());
   else ((AMSR_ParticleReader *)(G__getstructoffset()))->~AMSR_ParticleReader();
   return(1 || funcname || hash || result7 || libp) ;
}


/* AMSR_MCParticleReader */
static int G__AMSR_MCParticleReader_AMSR_MCParticleReader_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_MCParticleReader *p=NULL;
   if(G__getaryconstruct()) p=new AMSR_MCParticleReader[G__getaryconstruct()];
   else                    p=new AMSR_MCParticleReader;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_MCParticleReader);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_MCParticleReader_AMSR_MCParticleReader_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_MCParticleReader *p=NULL;
      p = new AMSR_MCParticleReader((const char*)G__int(libp->para[0]),(const char*)G__int(libp->para[1]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_MCParticleReader);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_MCParticleReader_AddParticle_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_MCParticleReader*)(G__getstructoffset()))->AddParticle((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_MCParticleReader_Clear_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_MCParticleReader*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_MCParticleReader*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_MCParticleReader_Finish_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_MCParticleReader*)(G__getstructoffset()))->Finish();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_MCParticleReader_Make_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_MCParticleReader*)(G__getstructoffset()))->Make();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_MCParticleReader_PrintInfo_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_MCParticleReader*)(G__getstructoffset()))->PrintInfo();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_MCParticleReader_DeclFileName_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_MCParticleReader*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_MCParticleReader_DeclFileLine_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_MCParticleReader*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_MCParticleReader_ImplFileName_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_MCParticleReader*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_MCParticleReader_ImplFileLine_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_MCParticleReader*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_MCParticleReader_Class_Version_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSR_MCParticleReader*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_MCParticleReader_Class_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_MCParticleReader*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_MCParticleReader_Dictionary_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_MCParticleReader*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_MCParticleReader_IsA_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_MCParticleReader*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_MCParticleReader_ShowMembers_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_MCParticleReader*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_MCParticleReader_Streamer_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_MCParticleReader*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic copy constructor
static int G__AMSR_MCParticleReader_AMSR_MCParticleReader_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSR_MCParticleReader *p;
   if(1!=libp->paran) ;
   p=new AMSR_MCParticleReader(*(AMSR_MCParticleReader*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_MCParticleReader);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__AMSR_MCParticleReader_wAAMSR_MCParticleReader_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSR_MCParticleReader *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSR_MCParticleReader *)((G__getstructoffset())+sizeof(AMSR_MCParticleReader)*i))->~AMSR_MCParticleReader();
   else if(G__PVOID==G__getgvp()) delete (AMSR_MCParticleReader *)(G__getstructoffset());
   else ((AMSR_MCParticleReader *)(G__getstructoffset()))->~AMSR_MCParticleReader();
   return(1 || funcname || hash || result7 || libp) ;
}


/* AMSR_Ntuple */
static int G__AMSR_Ntuple_AMSR_Ntuple_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_Ntuple *p=NULL;
   switch(libp->paran) {
   case 2:
      p = new AMSR_Ntuple((const char*)G__int(libp->para[0]),(const char*)G__int(libp->para[1]));
      break;
   case 1:
      p = new AMSR_Ntuple((const char*)G__int(libp->para[0]));
      break;
   case 0:
   if(G__getaryconstruct()) p=new AMSR_Ntuple[G__getaryconstruct()];
   else                    p=new AMSR_Ntuple;
      break;
   }
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Ntuple);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Ntuple_CloseNtuple_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Ntuple*)(G__getstructoffset()))->CloseNtuple();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Ntuple_GetEntries_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Ntuple*)(G__getstructoffset()))->GetEntries());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Ntuple_GetEvent_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Ntuple*)(G__getstructoffset()))->GetEvent((Int_t)G__int(libp->para[0]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Ntuple_GetEvtNum_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Ntuple*)(G__getstructoffset()))->GetEvtNum());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Ntuple_GetNextID_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Ntuple*)(G__getstructoffset()))->GetNextID());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Ntuple_GetLun_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Ntuple*)(G__getstructoffset()))->GetLun());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Ntuple_GetRunNum_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Ntuple*)(G__getstructoffset()))->GetRunNum());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Ntuple_GetRunTime_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Ntuple*)(G__getstructoffset()))->GetRunTime((time_t*)G__int(libp->para[0]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Ntuple_GetRunType_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Ntuple*)(G__getstructoffset()))->GetRunType());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Ntuple_GetNtupleID_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Ntuple*)(G__getstructoffset()))->GetNtupleID());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Ntuple_GetTree_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Ntuple*)(G__getstructoffset()))->GetTree());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Ntuple_IsNtupleOpen_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((AMSR_Ntuple*)(G__getstructoffset()))->IsNtupleOpen());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Ntuple_OpenNtuple_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Ntuple*)(G__getstructoffset()))->OpenNtuple((char*)G__int(libp->para[0])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Ntuple_SetBranchStatus_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Ntuple*)(G__getstructoffset()))->SetBranchStatus((char*)G__int(libp->para[0]),(Bool_t)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Ntuple_SetNtupleID_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Ntuple*)(G__getstructoffset()))->SetNtupleID((Int_t)G__int(libp->para[0]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Ntuple_SetTree_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_Ntuple*)(G__getstructoffset()))->SetTree((TTree*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_Ntuple*)(G__getstructoffset()))->SetTree();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Ntuple_DeclFileName_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_Ntuple*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Ntuple_DeclFileLine_2_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Ntuple*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Ntuple_ImplFileName_3_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_Ntuple*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Ntuple_ImplFileLine_4_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Ntuple*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Ntuple_Class_Version_5_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSR_Ntuple*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Ntuple_Class_6_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Ntuple*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Ntuple_Dictionary_7_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Ntuple*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Ntuple_IsA_8_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Ntuple*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Ntuple_ShowMembers_9_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Ntuple*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Ntuple_Streamer_0_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Ntuple*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic copy constructor
static int G__AMSR_Ntuple_AMSR_Ntuple_1_3(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSR_Ntuple *p;
   if(1!=libp->paran) ;
   p=new AMSR_Ntuple(*(AMSR_Ntuple*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Ntuple);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__AMSR_Ntuple_wAAMSR_Ntuple_2_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSR_Ntuple *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSR_Ntuple *)((G__getstructoffset())+sizeof(AMSR_Ntuple)*i))->~AMSR_Ntuple();
   else if(G__PVOID==G__getgvp()) delete (AMSR_Ntuple *)(G__getstructoffset());
   else ((AMSR_Ntuple *)(G__getstructoffset()))->~AMSR_Ntuple();
   return(1 || funcname || hash || result7 || libp) ;
}


/* AMSR_VirtualDisplay */
static int G__AMSR_VirtualDisplay_Clear_2_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_DisplayButtons_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->DisplayButtons();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_DistancetoPrimitive_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_VirtualDisplay*)(G__getstructoffset()))->DistancetoPrimitive((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_Draw_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->Draw((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->Draw();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_DrawAllViews_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->DrawAllViews();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_DrawFrontAndSideViews_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->DrawFrontAndSideViews();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_DrawParticles_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((AMSR_VirtualDisplay*)(G__getstructoffset()))->DrawParticles());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_DrawCaption_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->DrawCaption((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->DrawCaption();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_DrawView_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->DrawView((Float_t)G__double(libp->para[0]),(Float_t)G__double(libp->para[1])
,(Int_t)G__int(libp->para[2]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_DrawViewGL_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->DrawViewGL();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_DrawViewX3D_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->DrawViewX3D();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_ExecuteEvent_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->ExecuteEvent((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_GetCanvas_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_VirtualDisplay*)(G__getstructoffset()))->GetCanvas());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_Paint_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->Paint((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->Paint();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_PaintParticles_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->PaintParticles((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->PaintParticles();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_PTcut_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letdouble(result7,102,(double)((AMSR_VirtualDisplay*)(G__getstructoffset()))->PTcut());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_PTcutEGMUNU_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letdouble(result7,102,(double)((AMSR_VirtualDisplay*)(G__getstructoffset()))->PTcutEGMUNU());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_SetView_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->SetView((Float_t)G__double(libp->para[0]),(Float_t)G__double(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_SetView_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->SetView((EAMSR_View)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->SetView();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_IdleSwitch_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->IdleSwitch((Int_t)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->IdleSwitch();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_Print_2_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->Print();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_SavePS_3_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->SavePS();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_SaveGIF_4_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->SaveGIF();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_GotoRunEvent_5_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((AMSR_VirtualDisplay*)(G__getstructoffset()))->GotoRunEvent());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_ShowNextEvent_6_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->ShowNextEvent((Int_t)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->ShowNextEvent();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_SizeParticles_7_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->SizeParticles();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_DeclFileName_8_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_VirtualDisplay*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_DeclFileLine_9_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_VirtualDisplay*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_ImplFileName_0_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_VirtualDisplay*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_ImplFileLine_1_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_VirtualDisplay*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_Class_Version_2_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSR_VirtualDisplay*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_Class_3_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_VirtualDisplay*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_Dictionary_4_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_IsA_5_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_VirtualDisplay*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_ShowMembers_6_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_VirtualDisplay_Streamer_7_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_VirtualDisplay*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__AMSR_VirtualDisplay_wAAMSR_VirtualDisplay_8_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSR_VirtualDisplay *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSR_VirtualDisplay *)((G__getstructoffset())+sizeof(AMSR_VirtualDisplay)*i))->~AMSR_VirtualDisplay();
   else if(G__PVOID==G__getgvp()) delete (AMSR_VirtualDisplay *)(G__getstructoffset());
   else ((AMSR_VirtualDisplay *)(G__getstructoffset()))->~AMSR_VirtualDisplay();
   return(1 || funcname || hash || result7 || libp) ;
}


/* AMSR_Root */
static int G__AMSR_Root_AMSR_Root_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_Root *p=NULL;
   if(G__getaryconstruct()) p=new AMSR_Root[G__getaryconstruct()];
   else                    p=new AMSR_Root;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Root);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_AMSR_Root_2_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_Root *p=NULL;
   switch(libp->paran) {
   case 2:
      p = new AMSR_Root((const char*)G__int(libp->para[0]),(const char*)G__int(libp->para[1]));
      break;
   case 1:
      p = new AMSR_Root((const char*)G__int(libp->para[0]));
      break;
   }
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Root);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_IsGolden_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Root*)(G__getstructoffset()))->IsGolden());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_OpenDataFile_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Root*)(G__getstructoffset()))->OpenDataFile((char*)G__int(libp->para[0]),(EDataFileType)G__int(libp->para[1])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_Browse_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Root*)(G__getstructoffset()))->Browse((TBrowser*)G__int(libp->para[0]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_GetEvent_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__letint(result7,98,(long)((AMSR_Root*)(G__getstructoffset()))->GetEvent((Int_t)G__int(libp->para[0])));
      break;
   case 0:
      G__letint(result7,98,(long)((AMSR_Root*)(G__getstructoffset()))->GetEvent());
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_GetEvent_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((AMSR_Root*)(G__getstructoffset()))->GetEvent((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_Init_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_Root*)(G__getstructoffset()))->Init((TTree*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_Root*)(G__getstructoffset()))->Init();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_Finish_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Root*)(G__getstructoffset()))->Finish();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_Display_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Root*)(G__getstructoffset()))->Display());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_Draw_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_Root*)(G__getstructoffset()))->Draw((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_Root*)(G__getstructoffset()))->Draw();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_Paint_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_Root*)(G__getstructoffset()))->Paint((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_Root*)(G__getstructoffset()))->Paint();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_Clear_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_Root*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_Root*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_IsFolder_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((AMSR_Root*)(G__getstructoffset()))->IsFolder());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_PrintInfo_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Root*)(G__getstructoffset()))->PrintInfo();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_GetVersion_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Root*)(G__getstructoffset()))->GetVersion());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_GetVersionDate_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Root*)(G__getstructoffset()))->GetVersionDate());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_RunNum_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Root*)(G__getstructoffset()))->RunNum());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_GetTime_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_Root*)(G__getstructoffset()))->GetTime());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_EventNum_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Root*)(G__getstructoffset()))->EventNum());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_Event_2_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Root*)(G__getstructoffset()))->Event());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_NEvent_3_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Root*)(G__getstructoffset()))->NEvent());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_Mode_4_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Root*)(G__getstructoffset()))->Mode());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_Tree_5_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Root*)(G__getstructoffset()))->Tree());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_GetDataFileName_6_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_Root*)(G__getstructoffset()))->GetDataFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_GetDataFileType_7_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Root*)(G__getstructoffset()))->GetDataFileType());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_GetNtuple_8_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Root*)(G__getstructoffset()))->GetNtuple());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_Makers_9_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Root*)(G__getstructoffset()))->Makers());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_Maker_0_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Root*)(G__getstructoffset()))->Maker((const char*)G__int(libp->para[0])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_ToFClusterMaker_1_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Root*)(G__getstructoffset()))->ToFClusterMaker());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_TrackMaker_2_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Root*)(G__getstructoffset()))->TrackMaker());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_SiHitMaker_3_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Root*)(G__getstructoffset()))->SiHitMaker());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_AntiClusterMaker_4_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Root*)(G__getstructoffset()))->AntiClusterMaker());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_TrMCClusterMaker_5_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Root*)(G__getstructoffset()))->TrMCClusterMaker());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_CTCClusterMaker_6_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Root*)(G__getstructoffset()))->CTCClusterMaker());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_ParticleMaker_7_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Root*)(G__getstructoffset()))->ParticleMaker());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_MCParticleMaker_8_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Root*)(G__getstructoffset()))->MCParticleMaker());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_SetDefaultParameters_9_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Root*)(G__getstructoffset()))->SetDefaultParameters();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_SetEvent_0_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_Root*)(G__getstructoffset()))->SetEvent((Int_t)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_Root*)(G__getstructoffset()))->SetEvent();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_SetMode_1_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_Root*)(G__getstructoffset()))->SetMode((Int_t)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_Root*)(G__getstructoffset()))->SetMode();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_SetDisplay_2_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Root*)(G__getstructoffset()))->SetDisplay((AMSR_VirtualDisplay*)G__int(libp->para[0]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_Make_3_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_Root*)(G__getstructoffset()))->Make((Int_t)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_Root*)(G__getstructoffset()))->Make();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_MakeTree_4_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 2:
      G__setnull(result7);
      ((AMSR_Root*)(G__getstructoffset()))->MakeTree((const char*)G__int(libp->para[0]),(const char*)G__int(libp->para[1]));
      break;
   case 1:
      G__setnull(result7);
      ((AMSR_Root*)(G__getstructoffset()))->MakeTree((const char*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_Root*)(G__getstructoffset()))->MakeTree();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_FillTree_5_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Root*)(G__getstructoffset()))->FillTree();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_SortDown_6_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 4:
      G__setnull(result7);
      ((AMSR_Root*)(G__getstructoffset()))->SortDown((Int_t)G__int(libp->para[0]),(Float_t*)G__int(libp->para[1])
,(Int_t*)G__int(libp->para[2]),(Bool_t)G__int(libp->para[3]));
      break;
   case 3:
      G__setnull(result7);
      ((AMSR_Root*)(G__getstructoffset()))->SortDown((Int_t)G__int(libp->para[0]),(Float_t*)G__int(libp->para[1])
,(Int_t*)G__int(libp->para[2]));
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_DeclFileName_7_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_Root*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_DeclFileLine_8_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Root*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_ImplFileName_9_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_Root*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_ImplFileLine_0_5(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Root*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_Class_Version_1_5(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSR_Root*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_Class_2_5(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Root*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_Dictionary_3_5(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Root*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_IsA_4_5(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Root*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_ShowMembers_5_5(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Root*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Root_Streamer_6_5(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Root*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic copy constructor
static int G__AMSR_Root_AMSR_Root_7_5(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSR_Root *p;
   if(1!=libp->paran) ;
   p=new AMSR_Root(*(AMSR_Root*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Root);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__AMSR_Root_wAAMSR_Root_8_5(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSR_Root *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSR_Root *)((G__getstructoffset())+sizeof(AMSR_Root)*i))->~AMSR_Root();
   else if(G__PVOID==G__getgvp()) delete (AMSR_Root *)(G__getstructoffset());
   else ((AMSR_Root *)(G__getstructoffset()))->~AMSR_Root();
   return(1 || funcname || hash || result7 || libp) ;
}


/* AMSR_ToFCluster */
static int G__AMSR_ToFCluster_AMSR_ToFCluster_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_ToFCluster *p=NULL;
   if(G__getaryconstruct()) p=new AMSR_ToFCluster[G__getaryconstruct()];
   else                    p=new AMSR_ToFCluster;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ToFCluster);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFCluster_AMSR_ToFCluster_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_ToFCluster *p=NULL;
      p = new AMSR_ToFCluster(
(Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]),(Float_t)G__double(libp->para[3])
,(Float_t)G__double(libp->para[4]),(Float_t)G__double(libp->para[5])
,(Float_t*)G__int(libp->para[6]),(Float_t*)G__int(libp->para[7])
,(Int_t)G__int(libp->para[8]),(Int_t)G__int(libp->para[9])
,(Int_t)G__int(libp->para[10]),(TObjArray*)G__int(libp->para[11]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ToFCluster);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFCluster_GetObjectInfo_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_ToFCluster*)(G__getstructoffset()))->GetObjectInfo((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFCluster_Paint_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_ToFCluster*)(G__getstructoffset()))->Paint((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_ToFCluster*)(G__getstructoffset()))->Paint();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFCluster_Clear_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_ToFCluster*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_ToFCluster*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFCluster_GetStatus_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_ToFCluster*)(G__getstructoffset()))->GetStatus());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFCluster_GetPlane_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_ToFCluster*)(G__getstructoffset()))->GetPlane());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFCluster_GetBar_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_ToFCluster*)(G__getstructoffset()))->GetBar());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFCluster_GetEnergy_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letdouble(result7,102,(double)((AMSR_ToFCluster*)(G__getstructoffset()))->GetEnergy());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFCluster_GetTime_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letdouble(result7,102,(double)((AMSR_ToFCluster*)(G__getstructoffset()))->GetTime());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFCluster_GetNcells_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_ToFCluster*)(G__getstructoffset()))->GetNcells());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFCluster_GetNparticles_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_ToFCluster*)(G__getstructoffset()))->GetNparticles());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFCluster_GetNTracks_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_ToFCluster*)(G__getstructoffset()))->GetNTracks());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFCluster_GetTracks_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_ToFCluster*)(G__getstructoffset()))->GetTracks());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFCluster_Is3D_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((AMSR_ToFCluster*)(G__getstructoffset()))->Is3D());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFCluster_SetEnergy_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_ToFCluster*)(G__getstructoffset()))->SetEnergy((Float_t)G__double(libp->para[0]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFCluster_SetTime_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_ToFCluster*)(G__getstructoffset()))->SetTime((Float_t)G__double(libp->para[0]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFCluster_DeclFileName_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_ToFCluster*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFCluster_DeclFileLine_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_ToFCluster*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFCluster_ImplFileName_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_ToFCluster*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFCluster_ImplFileLine_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_ToFCluster*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFCluster_Class_Version_2_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSR_ToFCluster*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFCluster_Class_3_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_ToFCluster*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFCluster_Dictionary_4_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_ToFCluster*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFCluster_IsA_5_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_ToFCluster*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFCluster_ShowMembers_6_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_ToFCluster*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_ToFCluster_Streamer_7_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_ToFCluster*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic copy constructor
static int G__AMSR_ToFCluster_AMSR_ToFCluster_8_2(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSR_ToFCluster *p;
   if(1!=libp->paran) ;
   p=new AMSR_ToFCluster(*(AMSR_ToFCluster*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ToFCluster);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__AMSR_ToFCluster_wAAMSR_ToFCluster_9_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSR_ToFCluster *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSR_ToFCluster *)((G__getstructoffset())+sizeof(AMSR_ToFCluster)*i))->~AMSR_ToFCluster();
   else if(G__PVOID==G__getgvp()) delete (AMSR_ToFCluster *)(G__getstructoffset());
   else ((AMSR_ToFCluster *)(G__getstructoffset()))->~AMSR_ToFCluster();
   return(1 || funcname || hash || result7 || libp) ;
}


/* AMSR_AntiCluster */
static int G__AMSR_AntiCluster_AMSR_AntiCluster_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_AntiCluster *p=NULL;
   if(G__getaryconstruct()) p=new AMSR_AntiCluster[G__getaryconstruct()];
   else                    p=new AMSR_AntiCluster;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_AntiCluster);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiCluster_AMSR_AntiCluster_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_AntiCluster *p=NULL;
      p = new AMSR_AntiCluster(
(Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Float_t)G__double(libp->para[2]),(Float_t*)G__int(libp->para[3])
,(Float_t*)G__int(libp->para[4]),(Int_t)G__int(libp->para[5])
,(TObjArray*)G__int(libp->para[6]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_AntiCluster);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiCluster_GetObjectInfo_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_AntiCluster*)(G__getstructoffset()))->GetObjectInfo((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiCluster_Paint_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_AntiCluster*)(G__getstructoffset()))->Paint((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_AntiCluster*)(G__getstructoffset()))->Paint();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiCluster_Clear_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_AntiCluster*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_AntiCluster*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiCluster_GetStatus_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_AntiCluster*)(G__getstructoffset()))->GetStatus());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiCluster_GetSector_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_AntiCluster*)(G__getstructoffset()))->GetSector());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiCluster_GetSignal_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letdouble(result7,102,(double)((AMSR_AntiCluster*)(G__getstructoffset()))->GetSignal());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiCluster_GetTracks_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_AntiCluster*)(G__getstructoffset()))->GetTracks());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiCluster_Is3D_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((AMSR_AntiCluster*)(G__getstructoffset()))->Is3D());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiCluster_DeclFileName_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_AntiCluster*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiCluster_DeclFileLine_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_AntiCluster*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiCluster_ImplFileName_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_AntiCluster*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiCluster_ImplFileLine_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_AntiCluster*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiCluster_Class_Version_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSR_AntiCluster*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiCluster_Class_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_AntiCluster*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiCluster_Dictionary_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_AntiCluster*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiCluster_IsA_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_AntiCluster*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiCluster_ShowMembers_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_AntiCluster*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_AntiCluster_Streamer_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_AntiCluster*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic copy constructor
static int G__AMSR_AntiCluster_AMSR_AntiCluster_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSR_AntiCluster *p;
   if(1!=libp->paran) ;
   p=new AMSR_AntiCluster(*(AMSR_AntiCluster*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_AntiCluster);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__AMSR_AntiCluster_wAAMSR_AntiCluster_2_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSR_AntiCluster *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSR_AntiCluster *)((G__getstructoffset())+sizeof(AMSR_AntiCluster)*i))->~AMSR_AntiCluster();
   else if(G__PVOID==G__getgvp()) delete (AMSR_AntiCluster *)(G__getstructoffset());
   else ((AMSR_AntiCluster *)(G__getstructoffset()))->~AMSR_AntiCluster();
   return(1 || funcname || hash || result7 || libp) ;
}


/* AMSR_TrMCCluster */
static int G__AMSR_TrMCCluster_AMSR_TrMCCluster_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_TrMCCluster *p=NULL;
   if(G__getaryconstruct()) p=new AMSR_TrMCCluster[G__getaryconstruct()];
   else                    p=new AMSR_TrMCCluster;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrMCCluster);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCCluster_AMSR_TrMCCluster_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_TrMCCluster *p=NULL;
      p = new AMSR_TrMCCluster(
(Int_t)G__int(libp->para[0]),(Float_t)G__double(libp->para[1])
,(Float_t*)G__int(libp->para[2]),(Float_t*)G__int(libp->para[3])
,(Int_t)G__int(libp->para[4]),(TObjArray*)G__int(libp->para[5]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrMCCluster);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCCluster_GetObjectInfo_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_TrMCCluster*)(G__getstructoffset()))->GetObjectInfo((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCCluster_Paint_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_TrMCCluster*)(G__getstructoffset()))->Paint((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_TrMCCluster*)(G__getstructoffset()))->Paint();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCCluster_Clear_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_TrMCCluster*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_TrMCCluster*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCCluster_GetStatus_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_TrMCCluster*)(G__getstructoffset()))->GetStatus());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCCluster_GetSignal_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letdouble(result7,102,(double)((AMSR_TrMCCluster*)(G__getstructoffset()))->GetSignal());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCCluster_GetTracks_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_TrMCCluster*)(G__getstructoffset()))->GetTracks());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCCluster_Is3D_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((AMSR_TrMCCluster*)(G__getstructoffset()))->Is3D());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCCluster_DeclFileName_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_TrMCCluster*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCCluster_DeclFileLine_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_TrMCCluster*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCCluster_ImplFileName_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_TrMCCluster*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCCluster_ImplFileLine_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_TrMCCluster*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCCluster_Class_Version_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSR_TrMCCluster*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCCluster_Class_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_TrMCCluster*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCCluster_Dictionary_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_TrMCCluster*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCCluster_IsA_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_TrMCCluster*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCCluster_ShowMembers_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_TrMCCluster*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_TrMCCluster_Streamer_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_TrMCCluster*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic copy constructor
static int G__AMSR_TrMCCluster_AMSR_TrMCCluster_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSR_TrMCCluster *p;
   if(1!=libp->paran) ;
   p=new AMSR_TrMCCluster(*(AMSR_TrMCCluster*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrMCCluster);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__AMSR_TrMCCluster_wAAMSR_TrMCCluster_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSR_TrMCCluster *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSR_TrMCCluster *)((G__getstructoffset())+sizeof(AMSR_TrMCCluster)*i))->~AMSR_TrMCCluster();
   else if(G__PVOID==G__getgvp()) delete (AMSR_TrMCCluster *)(G__getstructoffset());
   else ((AMSR_TrMCCluster *)(G__getstructoffset()))->~AMSR_TrMCCluster();
   return(1 || funcname || hash || result7 || libp) ;
}


/* AMSR_Track */
static int G__AMSR_Track_AMSR_Track_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_Track *p=NULL;
   if(G__getaryconstruct()) p=new AMSR_Track[G__getaryconstruct()];
   else                    p=new AMSR_Track;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Track);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Track_AMSR_Track_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_Track *p=NULL;
      p = new AMSR_Track((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Track);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Track_GetObjectInfo_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_Track*)(G__getstructoffset()))->GetObjectInfo((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Track_SetHelix_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Track*)(G__getstructoffset()))->SetHelix();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Track_Paint_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_Track*)(G__getstructoffset()))->Paint((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_Track*)(G__getstructoffset()))->Paint();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Track_DeclFileName_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_Track*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Track_DeclFileLine_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Track*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Track_ImplFileName_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_Track*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Track_ImplFileLine_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Track*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Track_Class_Version_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSR_Track*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Track_Class_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Track*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Track_Dictionary_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Track*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Track_IsA_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Track*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Track_ShowMembers_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Track*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Track_Streamer_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Track*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic copy constructor
static int G__AMSR_Track_AMSR_Track_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSR_Track *p;
   if(1!=libp->paran) ;
   p=new AMSR_Track(*(AMSR_Track*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Track);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__AMSR_Track_wAAMSR_Track_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSR_Track *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSR_Track *)((G__getstructoffset())+sizeof(AMSR_Track)*i))->~AMSR_Track();
   else if(G__PVOID==G__getgvp()) delete (AMSR_Track *)(G__getstructoffset());
   else ((AMSR_Track *)(G__getstructoffset()))->~AMSR_Track();
   return(1 || funcname || hash || result7 || libp) ;
}


/* AMSR_SiHit */
static int G__AMSR_SiHit_AMSR_SiHit_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_SiHit *p=NULL;
   if(G__getaryconstruct()) p=new AMSR_SiHit[G__getaryconstruct()];
   else                    p=new AMSR_SiHit;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_SiHit);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHit_AMSR_SiHit_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_SiHit *p=NULL;
      p = new AMSR_SiHit(
(Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]),(Int_t)G__int(libp->para[3])
,(Float_t*)G__int(libp->para[4]),(Float_t*)G__int(libp->para[5])
,(Float_t)G__double(libp->para[6]),(Float_t)G__double(libp->para[7])
,(TObjArray*)G__int(libp->para[8]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_SiHit);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHit_GetObjectInfo_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_SiHit*)(G__getstructoffset()))->GetObjectInfo((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHit_Paint_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_SiHit*)(G__getstructoffset()))->Paint((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_SiHit*)(G__getstructoffset()))->Paint();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHit_Clear_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_SiHit*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_SiHit*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHit_GetStatus_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_SiHit*)(G__getstructoffset()))->GetStatus());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHit_GetPlane_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_SiHit*)(G__getstructoffset()))->GetPlane());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHit_GetX_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_SiHit*)(G__getstructoffset()))->GetX());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHit_GetY_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_SiHit*)(G__getstructoffset()))->GetY());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHit_GetAmplitude_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letdouble(result7,102,(double)((AMSR_SiHit*)(G__getstructoffset()))->GetAmplitude());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHit_GetAsymAmpl_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letdouble(result7,102,(double)((AMSR_SiHit*)(G__getstructoffset()))->GetAsymAmpl());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHit_GetTracks_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_SiHit*)(G__getstructoffset()))->GetTracks());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHit_Is3D_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((AMSR_SiHit*)(G__getstructoffset()))->Is3D());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHit_DeclFileName_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_SiHit*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHit_DeclFileLine_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_SiHit*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHit_ImplFileName_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_SiHit*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHit_ImplFileLine_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_SiHit*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHit_Class_Version_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSR_SiHit*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHit_Class_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_SiHit*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHit_Dictionary_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_SiHit*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHit_IsA_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_SiHit*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHit_ShowMembers_2_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_SiHit*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_SiHit_Streamer_3_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_SiHit*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic copy constructor
static int G__AMSR_SiHit_AMSR_SiHit_4_2(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSR_SiHit *p;
   if(1!=libp->paran) ;
   p=new AMSR_SiHit(*(AMSR_SiHit*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_SiHit);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__AMSR_SiHit_wAAMSR_SiHit_5_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSR_SiHit *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSR_SiHit *)((G__getstructoffset())+sizeof(AMSR_SiHit)*i))->~AMSR_SiHit();
   else if(G__PVOID==G__getgvp()) delete (AMSR_SiHit *)(G__getstructoffset());
   else ((AMSR_SiHit *)(G__getstructoffset()))->~AMSR_SiHit();
   return(1 || funcname || hash || result7 || libp) ;
}


/* AMSR_CTCCluster */
static int G__AMSR_CTCCluster_AMSR_CTCCluster_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_CTCCluster *p=NULL;
   if(G__getaryconstruct()) p=new AMSR_CTCCluster[G__getaryconstruct()];
   else                    p=new AMSR_CTCCluster;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_CTCCluster);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCCluster_AMSR_CTCCluster_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_CTCCluster *p=NULL;
      p = new AMSR_CTCCluster(
(Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]),(Float_t)G__double(libp->para[3])
,(Float_t)G__double(libp->para[4]),(Float_t*)G__int(libp->para[5])
,(Float_t*)G__int(libp->para[6]),(Int_t)G__int(libp->para[7])
,(TObjArray*)G__int(libp->para[8]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_CTCCluster);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCCluster_GetObjectInfo_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_CTCCluster*)(G__getstructoffset()))->GetObjectInfo((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCCluster_Paint_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_CTCCluster*)(G__getstructoffset()))->Paint((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_CTCCluster*)(G__getstructoffset()))->Paint();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCCluster_Clear_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_CTCCluster*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_CTCCluster*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCCluster_GetStatus_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_CTCCluster*)(G__getstructoffset()))->GetStatus());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCCluster_GetPlane_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_CTCCluster*)(G__getstructoffset()))->GetPlane());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCCluster_GetRawSignal_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letdouble(result7,102,(double)((AMSR_CTCCluster*)(G__getstructoffset()))->GetRawSignal());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCCluster_GetSignal_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letdouble(result7,102,(double)((AMSR_CTCCluster*)(G__getstructoffset()))->GetSignal());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCCluster_GetErrSignal_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letdouble(result7,102,(double)((AMSR_CTCCluster*)(G__getstructoffset()))->GetErrSignal());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCCluster_GetTracks_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_CTCCluster*)(G__getstructoffset()))->GetTracks());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCCluster_Is3D_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((AMSR_CTCCluster*)(G__getstructoffset()))->Is3D());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCCluster_DeclFileName_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_CTCCluster*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCCluster_DeclFileLine_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_CTCCluster*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCCluster_ImplFileName_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_CTCCluster*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCCluster_ImplFileLine_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_CTCCluster*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCCluster_Class_Version_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSR_CTCCluster*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCCluster_Class_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_CTCCluster*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCCluster_Dictionary_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_CTCCluster*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCCluster_IsA_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_CTCCluster*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCCluster_ShowMembers_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_CTCCluster*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_CTCCluster_Streamer_2_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_CTCCluster*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic copy constructor
static int G__AMSR_CTCCluster_AMSR_CTCCluster_3_2(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSR_CTCCluster *p;
   if(1!=libp->paran) ;
   p=new AMSR_CTCCluster(*(AMSR_CTCCluster*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_CTCCluster);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__AMSR_CTCCluster_wAAMSR_CTCCluster_4_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSR_CTCCluster *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSR_CTCCluster *)((G__getstructoffset())+sizeof(AMSR_CTCCluster)*i))->~AMSR_CTCCluster();
   else if(G__PVOID==G__getgvp()) delete (AMSR_CTCCluster *)(G__getstructoffset());
   else ((AMSR_CTCCluster *)(G__getstructoffset()))->~AMSR_CTCCluster();
   return(1 || funcname || hash || result7 || libp) ;
}


/* AMSR_Particle */
static int G__AMSR_Particle_AMSR_Particle_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_Particle *p=NULL;
   if(G__getaryconstruct()) p=new AMSR_Particle[G__getaryconstruct()];
   else                    p=new AMSR_Particle;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Particle);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Particle_GetObjectInfo_2_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_Particle*)(G__getstructoffset()))->GetObjectInfo((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Particle_SetHelix_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Particle*)(G__getstructoffset()))->SetHelix();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Particle_Paint_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_Particle*)(G__getstructoffset()))->Paint((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_Particle*)(G__getstructoffset()))->Paint();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Particle_DeclFileName_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_Particle*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Particle_DeclFileLine_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Particle*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Particle_ImplFileName_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_Particle*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Particle_ImplFileLine_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Particle*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Particle_Class_Version_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSR_Particle*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Particle_Class_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Particle*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Particle_Dictionary_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Particle*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Particle_IsA_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Particle*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Particle_ShowMembers_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Particle*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Particle_Streamer_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Particle*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic copy constructor
static int G__AMSR_Particle_AMSR_Particle_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSR_Particle *p;
   if(1!=libp->paran) ;
   p=new AMSR_Particle(*(AMSR_Particle*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Particle);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__AMSR_Particle_wAAMSR_Particle_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSR_Particle *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSR_Particle *)((G__getstructoffset())+sizeof(AMSR_Particle)*i))->~AMSR_Particle();
   else if(G__PVOID==G__getgvp()) delete (AMSR_Particle *)(G__getstructoffset());
   else ((AMSR_Particle *)(G__getstructoffset()))->~AMSR_Particle();
   return(1 || funcname || hash || result7 || libp) ;
}


/* AMSR_MCParticle */
static int G__AMSR_MCParticle_AMSR_MCParticle_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_MCParticle *p=NULL;
   if(G__getaryconstruct()) p=new AMSR_MCParticle[G__getaryconstruct()];
   else                    p=new AMSR_MCParticle;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_MCParticle);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_MCParticle_GetObjectInfo_2_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_MCParticle*)(G__getstructoffset()))->GetObjectInfo((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_MCParticle_SetHelix_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_MCParticle*)(G__getstructoffset()))->SetHelix();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_MCParticle_Paint_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSR_MCParticle*)(G__getstructoffset()))->Paint((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSR_MCParticle*)(G__getstructoffset()))->Paint();
      break;
   }
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_MCParticle_DeclFileName_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_MCParticle*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_MCParticle_DeclFileLine_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_MCParticle*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_MCParticle_ImplFileName_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_MCParticle*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_MCParticle_ImplFileLine_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_MCParticle*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_MCParticle_Class_Version_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSR_MCParticle*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_MCParticle_Class_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_MCParticle*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_MCParticle_Dictionary_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_MCParticle*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_MCParticle_IsA_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_MCParticle*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_MCParticle_ShowMembers_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_MCParticle*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_MCParticle_Streamer_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_MCParticle*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic copy constructor
static int G__AMSR_MCParticle_AMSR_MCParticle_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSR_MCParticle *p;
   if(1!=libp->paran) ;
   p=new AMSR_MCParticle(*(AMSR_MCParticle*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_MCParticle);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__AMSR_MCParticle_wAAMSR_MCParticle_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSR_MCParticle *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSR_MCParticle *)((G__getstructoffset())+sizeof(AMSR_MCParticle)*i))->~AMSR_MCParticle();
   else if(G__PVOID==G__getgvp()) delete (AMSR_MCParticle *)(G__getstructoffset());
   else ((AMSR_MCParticle *)(G__getstructoffset()))->~AMSR_MCParticle();
   return(1 || funcname || hash || result7 || libp) ;
}


/* AMSR_Canvas */
static int G__AMSR_Canvas_AddParticleInfo_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Canvas*)(G__getstructoffset()))->AddParticleInfo();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Canvas_AMSR_Canvas_2_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_Canvas *p=NULL;
   if(G__getaryconstruct()) p=new AMSR_Canvas[G__getaryconstruct()];
   else                    p=new AMSR_Canvas;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Canvas);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Canvas_AMSR_Canvas_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_Canvas *p=NULL;
   switch(libp->paran) {
   case 4:
      p = new AMSR_Canvas(
(Text_t*)G__int(libp->para[0]),(Text_t*)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]),(Int_t)G__int(libp->para[3]));
      break;
   case 3:
      p = new AMSR_Canvas(
(Text_t*)G__int(libp->para[0]),(Text_t*)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]));
      break;
   case 2:
      p = new AMSR_Canvas((Text_t*)G__int(libp->para[0]),(Text_t*)G__int(libp->para[1]));
      break;
   case 1:
      p = new AMSR_Canvas((Text_t*)G__int(libp->para[0]));
      break;
   }
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Canvas);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Canvas_Update_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Canvas*)(G__getstructoffset()))->Update();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Canvas_EditorBar_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Canvas*)(G__getstructoffset()))->EditorBar();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Canvas_HandleInput_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Canvas*)(G__getstructoffset()))->HandleInput((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Canvas_OpenFileCB_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Canvas*)(G__getstructoffset()))->OpenFileCB();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Canvas_SaveParticleCB_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Canvas*)(G__getstructoffset()))->SaveParticleCB();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Canvas_SaveParticleGIF_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Canvas*)(G__getstructoffset()))->SaveParticleGIF();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Canvas_PrintCB_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Canvas*)(G__getstructoffset()))->PrintCB();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Canvas_DeclFileName_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_Canvas*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Canvas_DeclFileLine_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Canvas*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Canvas_ImplFileName_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_Canvas*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Canvas_ImplFileLine_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_Canvas*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Canvas_Class_Version_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSR_Canvas*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Canvas_Class_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Canvas*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Canvas_Dictionary_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Canvas*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Canvas_IsA_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_Canvas*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Canvas_ShowMembers_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Canvas*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_Canvas_Streamer_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_Canvas*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__AMSR_Canvas_wAAMSR_Canvas_2_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSR_Canvas *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSR_Canvas *)((G__getstructoffset())+sizeof(AMSR_Canvas)*i))->~AMSR_Canvas();
   else if(G__PVOID==G__getgvp()) delete (AMSR_Canvas *)(G__getstructoffset());
   else ((AMSR_Canvas *)(G__getstructoffset()))->~AMSR_Canvas();
   return(1 || funcname || hash || result7 || libp) ;
}


/* Debugger */
static int G__Debugger_Debugger_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   Debugger *p=NULL;
   if(G__getaryconstruct()) p=new Debugger[G__getaryconstruct()];
   else                    p=new Debugger;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_Debugger);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__Debugger_On_2_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((Debugger*)(G__getstructoffset()))->On();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__Debugger_Off_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((Debugger*)(G__getstructoffset()))->Off();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__Debugger_Print_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((Debugger*)(G__getstructoffset()))->Print((char*)G__int(libp->para[0]),(int)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__Debugger_DeclFileName_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((Debugger*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__Debugger_DeclFileLine_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((Debugger*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__Debugger_ImplFileName_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((Debugger*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__Debugger_ImplFileLine_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((Debugger*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__Debugger_Class_Version_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((Debugger*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__Debugger_Class_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((Debugger*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__Debugger_Dictionary_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((Debugger*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__Debugger_IsA_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((Debugger*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__Debugger_ShowMembers_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((Debugger*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__Debugger_Streamer_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((Debugger*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic copy constructor
static int G__Debugger_Debugger_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   Debugger *p;
   if(1!=libp->paran) ;
   p=new Debugger(*(Debugger*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_Debugger);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__Debugger_wADebugger_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (Debugger *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((Debugger *)((G__getstructoffset())+sizeof(Debugger)*i))->~Debugger();
   else if(G__PVOID==G__getgvp()) delete (Debugger *)(G__getstructoffset());
   else ((Debugger *)(G__getstructoffset()))->~Debugger();
   return(1 || funcname || hash || result7 || libp) ;
}


/* AMSR_GeometrySetter */
static int G__AMSR_GeometrySetter_AMSR_GeometrySetter_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_GeometrySetter *p=NULL;
   if(G__getaryconstruct()) p=new AMSR_GeometrySetter[G__getaryconstruct()];
   else                    p=new AMSR_GeometrySetter;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_GeometrySetter);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_GeometrySetter_AMSR_GeometrySetter_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSR_GeometrySetter *p=NULL;
      p = new AMSR_GeometrySetter((TGeometry*)G__int(libp->para[0]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_GeometrySetter);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_GeometrySetter_TurnOn_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_GeometrySetter*)(G__getstructoffset()))->TurnOn((char*)G__int(libp->para[0]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_GeometrySetter_TurnOnWithSons_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_GeometrySetter*)(G__getstructoffset()))->TurnOnWithSons((char*)G__int(libp->para[0]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_GeometrySetter_TurnOff_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_GeometrySetter*)(G__getstructoffset()))->TurnOff((char*)G__int(libp->para[0]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_GeometrySetter_TurnOffWithSons_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_GeometrySetter*)(G__getstructoffset()))->TurnOffWithSons((char*)G__int(libp->para[0]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_GeometrySetter_CheckVisibility_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_GeometrySetter*)(G__getstructoffset()))->CheckVisibility((char*)G__int(libp->para[0]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_GeometrySetter_SetGeo_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_GeometrySetter*)(G__getstructoffset()))->SetGeo();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_GeometrySetter_DeclFileName_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_GeometrySetter*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_GeometrySetter_DeclFileLine_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_GeometrySetter*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_GeometrySetter_ImplFileName_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSR_GeometrySetter*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_GeometrySetter_ImplFileLine_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSR_GeometrySetter*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_GeometrySetter_Class_Version_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSR_GeometrySetter*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_GeometrySetter_Class_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_GeometrySetter*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_GeometrySetter_Dictionary_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_GeometrySetter*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_GeometrySetter_IsA_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSR_GeometrySetter*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_GeometrySetter_ShowMembers_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_GeometrySetter*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__AMSR_GeometrySetter_Streamer_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSR_GeometrySetter*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic copy constructor
static int G__AMSR_GeometrySetter_AMSR_GeometrySetter_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSR_GeometrySetter *p;
   if(1!=libp->paran) ;
   p=new AMSR_GeometrySetter(*(AMSR_GeometrySetter*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_GeometrySetter);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__AMSR_GeometrySetter_wAAMSR_GeometrySetter_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSR_GeometrySetter *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSR_GeometrySetter *)((G__getstructoffset())+sizeof(AMSR_GeometrySetter)*i))->~AMSR_GeometrySetter();
   else if(G__PVOID==G__getgvp()) delete (AMSR_GeometrySetter *)(G__getstructoffset());
   else ((AMSR_GeometrySetter *)(G__getstructoffset()))->~AMSR_GeometrySetter();
   return(1 || funcname || hash || result7 || libp) ;
}


/* TGRunEventDialog */
static int G__TGRunEventDialog_TGRunEventDialog_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   TGRunEventDialog *p=NULL;
      p = new TGRunEventDialog(
(const TGWindow*)G__int(libp->para[0]),(const TGWindow*)G__int(libp->para[1])
,(Int_t*)G__int(libp->para[2]),(Int_t*)G__int(libp->para[3]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_TGRunEventDialog);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TGRunEventDialog_ProcessMessage_2_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((TGRunEventDialog*)(G__getstructoffset()))->ProcessMessage((Long_t)G__int(libp->para[0]),(Long_t)G__int(libp->para[1])
,(Long_t)G__int(libp->para[2])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TGRunEventDialog_CloseWindow_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((TGRunEventDialog*)(G__getstructoffset()))->CloseWindow();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TGRunEventDialog_DeclFileName_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((TGRunEventDialog*)(G__getstructoffset()))->DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TGRunEventDialog_DeclFileLine_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((TGRunEventDialog*)(G__getstructoffset()))->DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TGRunEventDialog_ImplFileName_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((TGRunEventDialog*)(G__getstructoffset()))->ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TGRunEventDialog_ImplFileLine_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((TGRunEventDialog*)(G__getstructoffset()))->ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TGRunEventDialog_Class_Version_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((TGRunEventDialog*)(G__getstructoffset()))->Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TGRunEventDialog_Class_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((TGRunEventDialog*)(G__getstructoffset()))->Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TGRunEventDialog_Dictionary_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((TGRunEventDialog*)(G__getstructoffset()))->Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TGRunEventDialog_IsA_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((TGRunEventDialog*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TGRunEventDialog_ShowMembers_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((TGRunEventDialog*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__TGRunEventDialog_Streamer_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((TGRunEventDialog*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic copy constructor
static int G__TGRunEventDialog_TGRunEventDialog_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   TGRunEventDialog *p;
   if(1!=libp->paran) ;
   p=new TGRunEventDialog(*(TGRunEventDialog*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSR_CintLN_TGRunEventDialog);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
static int G__TGRunEventDialog_wATGRunEventDialog_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (TGRunEventDialog *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((TGRunEventDialog *)((G__getstructoffset())+sizeof(TGRunEventDialog)*i))->~TGRunEventDialog();
   else if(G__PVOID==G__getgvp()) delete (TGRunEventDialog *)(G__getstructoffset());
   else ((TGRunEventDialog *)(G__getstructoffset()))->~TGRunEventDialog();
   return(1 || funcname || hash || result7 || libp) ;
}


/* Setting up global function */

/*********************************************************
* Member function Stub
*********************************************************/

/* TRadioButton */

/* TSwitch */

/* AMSR_Axis */

/* AMSR_Display */

/* AMSR_3DCluster */

/* AMSR_Maker */

/* AMSR_HistBrowser */

/* AMSR_ToFClusterReader */

/* AMSR_TrackReader */

/* AMSR_SiHitReader */

/* AMSR_CTCClusterReader */

/* AMSR_AntiClusterReader */

/* AMSR_TrMCClusterReader */

/* AMSR_ParticleReader */

/* AMSR_MCParticleReader */

/* AMSR_Ntuple */

/* AMSR_VirtualDisplay */

/* AMSR_Root */

/* AMSR_ToFCluster */

/* AMSR_AntiCluster */

/* AMSR_TrMCCluster */

/* AMSR_Track */

/* AMSR_SiHit */

/* AMSR_CTCCluster */

/* AMSR_Particle */

/* AMSR_MCParticle */

/* AMSR_Canvas */

/* Debugger */

/* AMSR_GeometrySetter */

/* TGRunEventDialog */

/*********************************************************
* Global function Stub
*********************************************************/

/*********************************************************
* Get size of pointer to member function
*********************************************************/
class G__Sizep2memfuncAMSR_Cint {
 public:
  G__Sizep2memfuncAMSR_Cint() {p=&G__Sizep2memfuncAMSR_Cint::sizep2memfunc;}
    size_t sizep2memfunc() { return(sizeof(p)); }
  private:
    size_t (G__Sizep2memfuncAMSR_Cint::*p)();
};

size_t G__get_sizep2memfuncAMSR_Cint()
{
  G__Sizep2memfuncAMSR_Cint a;
  G__setsizep2memfunc((int)a.sizep2memfunc());
  return((size_t)a.sizep2memfunc());
}


/*********************************************************
* virtual base class offset calculation interface
*********************************************************/

   /* Setting up class inheritance */

/*********************************************************
* Inheritance information setup/
*********************************************************/
extern "C" void G__cpp_setup_inheritanceAMSR_Cint() {

   /* Setting up class inheritance */
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSR_CintLN_TRadioButton))) {
     TRadioButton *G__Lderived;
     G__Lderived=(TRadioButton*)0x1000;
     {
       TButton *G__Lpbase=(TButton*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TRadioButton),G__get_linked_tagnum(&G__AMSR_CintLN_TButton),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TPad *G__Lpbase=(TPad*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TRadioButton),G__get_linked_tagnum(&G__AMSR_CintLN_TPad),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TVirtualPad *G__Lpbase=(TVirtualPad*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TRadioButton),G__get_linked_tagnum(&G__AMSR_CintLN_TVirtualPad),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TWbox *G__Lpbase=(TWbox*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TRadioButton),G__get_linked_tagnum(&G__AMSR_CintLN_TWbox),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TBox *G__Lpbase=(TBox*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TRadioButton),G__get_linked_tagnum(&G__AMSR_CintLN_TBox),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TRadioButton),G__get_linked_tagnum(&G__AMSR_CintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttLine *G__Lpbase=(TAttLine*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TRadioButton),G__get_linked_tagnum(&G__AMSR_CintLN_TAttLine),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttFill *G__Lpbase=(TAttFill*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TRadioButton),G__get_linked_tagnum(&G__AMSR_CintLN_TAttFill),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttPad *G__Lpbase=(TAttPad*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TRadioButton),G__get_linked_tagnum(&G__AMSR_CintLN_TAttPad),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttText *G__Lpbase=(TAttText*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TRadioButton),G__get_linked_tagnum(&G__AMSR_CintLN_TAttText),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSR_CintLN_TSwitch))) {
     TSwitch *G__Lderived;
     G__Lderived=(TSwitch*)0x1000;
     {
       TPad *G__Lpbase=(TPad*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TSwitch),G__get_linked_tagnum(&G__AMSR_CintLN_TPad),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TVirtualPad *G__Lpbase=(TVirtualPad*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TSwitch),G__get_linked_tagnum(&G__AMSR_CintLN_TVirtualPad),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TWbox *G__Lpbase=(TWbox*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TSwitch),G__get_linked_tagnum(&G__AMSR_CintLN_TWbox),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TBox *G__Lpbase=(TBox*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TSwitch),G__get_linked_tagnum(&G__AMSR_CintLN_TBox),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TSwitch),G__get_linked_tagnum(&G__AMSR_CintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttLine *G__Lpbase=(TAttLine*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TSwitch),G__get_linked_tagnum(&G__AMSR_CintLN_TAttLine),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttFill *G__Lpbase=(TAttFill*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TSwitch),G__get_linked_tagnum(&G__AMSR_CintLN_TAttFill),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttPad *G__Lpbase=(TAttPad*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TSwitch),G__get_linked_tagnum(&G__AMSR_CintLN_TAttPad),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Axis))) {
     AMSR_Axis *G__Lderived;
     G__Lderived=(AMSR_Axis*)0x1000;
     {
       TPolyLine3D *G__Lpbase=(TPolyLine3D*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Axis),G__get_linked_tagnum(&G__AMSR_CintLN_TPolyLine3D),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Axis),G__get_linked_tagnum(&G__AMSR_CintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttLine *G__Lpbase=(TAttLine*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Axis),G__get_linked_tagnum(&G__AMSR_CintLN_TAttLine),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Display))) {
     AMSR_Display *G__Lderived;
     G__Lderived=(AMSR_Display*)0x1000;
     {
       AMSR_VirtualDisplay *G__Lpbase=(AMSR_VirtualDisplay*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Display),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_VirtualDisplay),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Display),G__get_linked_tagnum(&G__AMSR_CintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_3DCluster))) {
     AMSR_3DCluster *G__Lderived;
     G__Lderived=(AMSR_3DCluster*)0x1000;
     {
       TMarker3DBox *G__Lpbase=(TMarker3DBox*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_3DCluster),G__get_linked_tagnum(&G__AMSR_CintLN_TMarker3DBox),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_3DCluster),G__get_linked_tagnum(&G__AMSR_CintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttLine *G__Lpbase=(TAttLine*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_3DCluster),G__get_linked_tagnum(&G__AMSR_CintLN_TAttLine),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttFill *G__Lpbase=(TAttFill*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_3DCluster),G__get_linked_tagnum(&G__AMSR_CintLN_TAttFill),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Maker))) {
     AMSR_Maker *G__Lderived;
     G__Lderived=(AMSR_Maker*)0x1000;
     {
       TNamed *G__Lpbase=(TNamed*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Maker),G__get_linked_tagnum(&G__AMSR_CintLN_TNamed),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Maker),G__get_linked_tagnum(&G__AMSR_CintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_HistBrowser))) {
     AMSR_HistBrowser *G__Lderived;
     G__Lderived=(AMSR_HistBrowser*)0x1000;
     {
       TNamed *G__Lpbase=(TNamed*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_HistBrowser),G__get_linked_tagnum(&G__AMSR_CintLN_TNamed),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_HistBrowser),G__get_linked_tagnum(&G__AMSR_CintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ToFClusterReader))) {
     AMSR_ToFClusterReader *G__Lderived;
     G__Lderived=(AMSR_ToFClusterReader*)0x1000;
     {
       AMSR_Maker *G__Lpbase=(AMSR_Maker*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ToFClusterReader),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Maker),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TNamed *G__Lpbase=(TNamed*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ToFClusterReader),G__get_linked_tagnum(&G__AMSR_CintLN_TNamed),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ToFClusterReader),G__get_linked_tagnum(&G__AMSR_CintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrackReader))) {
     AMSR_TrackReader *G__Lderived;
     G__Lderived=(AMSR_TrackReader*)0x1000;
     {
       AMSR_Maker *G__Lpbase=(AMSR_Maker*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrackReader),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Maker),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TNamed *G__Lpbase=(TNamed*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrackReader),G__get_linked_tagnum(&G__AMSR_CintLN_TNamed),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrackReader),G__get_linked_tagnum(&G__AMSR_CintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_SiHitReader))) {
     AMSR_SiHitReader *G__Lderived;
     G__Lderived=(AMSR_SiHitReader*)0x1000;
     {
       AMSR_Maker *G__Lpbase=(AMSR_Maker*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_SiHitReader),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Maker),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TNamed *G__Lpbase=(TNamed*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_SiHitReader),G__get_linked_tagnum(&G__AMSR_CintLN_TNamed),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_SiHitReader),G__get_linked_tagnum(&G__AMSR_CintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_CTCClusterReader))) {
     AMSR_CTCClusterReader *G__Lderived;
     G__Lderived=(AMSR_CTCClusterReader*)0x1000;
     {
       AMSR_Maker *G__Lpbase=(AMSR_Maker*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_CTCClusterReader),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Maker),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TNamed *G__Lpbase=(TNamed*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_CTCClusterReader),G__get_linked_tagnum(&G__AMSR_CintLN_TNamed),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_CTCClusterReader),G__get_linked_tagnum(&G__AMSR_CintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_AntiClusterReader))) {
     AMSR_AntiClusterReader *G__Lderived;
     G__Lderived=(AMSR_AntiClusterReader*)0x1000;
     {
       AMSR_Maker *G__Lpbase=(AMSR_Maker*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_AntiClusterReader),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Maker),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TNamed *G__Lpbase=(TNamed*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_AntiClusterReader),G__get_linked_tagnum(&G__AMSR_CintLN_TNamed),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_AntiClusterReader),G__get_linked_tagnum(&G__AMSR_CintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrMCClusterReader))) {
     AMSR_TrMCClusterReader *G__Lderived;
     G__Lderived=(AMSR_TrMCClusterReader*)0x1000;
     {
       AMSR_Maker *G__Lpbase=(AMSR_Maker*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrMCClusterReader),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Maker),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TNamed *G__Lpbase=(TNamed*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrMCClusterReader),G__get_linked_tagnum(&G__AMSR_CintLN_TNamed),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrMCClusterReader),G__get_linked_tagnum(&G__AMSR_CintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ParticleReader))) {
     AMSR_ParticleReader *G__Lderived;
     G__Lderived=(AMSR_ParticleReader*)0x1000;
     {
       AMSR_Maker *G__Lpbase=(AMSR_Maker*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ParticleReader),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Maker),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TNamed *G__Lpbase=(TNamed*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ParticleReader),G__get_linked_tagnum(&G__AMSR_CintLN_TNamed),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ParticleReader),G__get_linked_tagnum(&G__AMSR_CintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_MCParticleReader))) {
     AMSR_MCParticleReader *G__Lderived;
     G__Lderived=(AMSR_MCParticleReader*)0x1000;
     {
       AMSR_Maker *G__Lpbase=(AMSR_Maker*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_MCParticleReader),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Maker),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TNamed *G__Lpbase=(TNamed*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_MCParticleReader),G__get_linked_tagnum(&G__AMSR_CintLN_TNamed),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_MCParticleReader),G__get_linked_tagnum(&G__AMSR_CintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Ntuple))) {
     AMSR_Ntuple *G__Lderived;
     G__Lderived=(AMSR_Ntuple*)0x1000;
     {
       TNamed *G__Lpbase=(TNamed*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Ntuple),G__get_linked_tagnum(&G__AMSR_CintLN_TNamed),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Ntuple),G__get_linked_tagnum(&G__AMSR_CintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_VirtualDisplay))) {
     AMSR_VirtualDisplay *G__Lderived;
     G__Lderived=(AMSR_VirtualDisplay*)0x1000;
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_VirtualDisplay),G__get_linked_tagnum(&G__AMSR_CintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Root))) {
     AMSR_Root *G__Lderived;
     G__Lderived=(AMSR_Root*)0x1000;
     {
       TNamed *G__Lpbase=(TNamed*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Root),G__get_linked_tagnum(&G__AMSR_CintLN_TNamed),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Root),G__get_linked_tagnum(&G__AMSR_CintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ToFCluster))) {
     AMSR_ToFCluster *G__Lderived;
     G__Lderived=(AMSR_ToFCluster*)0x1000;
     {
       AMSR_3DCluster *G__Lpbase=(AMSR_3DCluster*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ToFCluster),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_3DCluster),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TMarker3DBox *G__Lpbase=(TMarker3DBox*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ToFCluster),G__get_linked_tagnum(&G__AMSR_CintLN_TMarker3DBox),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ToFCluster),G__get_linked_tagnum(&G__AMSR_CintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttLine *G__Lpbase=(TAttLine*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ToFCluster),G__get_linked_tagnum(&G__AMSR_CintLN_TAttLine),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttFill *G__Lpbase=(TAttFill*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ToFCluster),G__get_linked_tagnum(&G__AMSR_CintLN_TAttFill),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_AntiCluster))) {
     AMSR_AntiCluster *G__Lderived;
     G__Lderived=(AMSR_AntiCluster*)0x1000;
     {
       AMSR_3DCluster *G__Lpbase=(AMSR_3DCluster*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_AntiCluster),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_3DCluster),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TMarker3DBox *G__Lpbase=(TMarker3DBox*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_AntiCluster),G__get_linked_tagnum(&G__AMSR_CintLN_TMarker3DBox),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_AntiCluster),G__get_linked_tagnum(&G__AMSR_CintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttLine *G__Lpbase=(TAttLine*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_AntiCluster),G__get_linked_tagnum(&G__AMSR_CintLN_TAttLine),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttFill *G__Lpbase=(TAttFill*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_AntiCluster),G__get_linked_tagnum(&G__AMSR_CintLN_TAttFill),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrMCCluster))) {
     AMSR_TrMCCluster *G__Lderived;
     G__Lderived=(AMSR_TrMCCluster*)0x1000;
     {
       AMSR_3DCluster *G__Lpbase=(AMSR_3DCluster*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrMCCluster),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_3DCluster),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TMarker3DBox *G__Lpbase=(TMarker3DBox*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrMCCluster),G__get_linked_tagnum(&G__AMSR_CintLN_TMarker3DBox),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrMCCluster),G__get_linked_tagnum(&G__AMSR_CintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttLine *G__Lpbase=(TAttLine*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrMCCluster),G__get_linked_tagnum(&G__AMSR_CintLN_TAttLine),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttFill *G__Lpbase=(TAttFill*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrMCCluster),G__get_linked_tagnum(&G__AMSR_CintLN_TAttFill),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Track))) {
     AMSR_Track *G__Lderived;
     G__Lderived=(AMSR_Track*)0x1000;
     {
       THelix *G__Lpbase=(THelix*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Track),G__get_linked_tagnum(&G__AMSR_CintLN_THelix),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TPolyLine3D *G__Lpbase=(TPolyLine3D*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Track),G__get_linked_tagnum(&G__AMSR_CintLN_TPolyLine3D),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Track),G__get_linked_tagnum(&G__AMSR_CintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttLine *G__Lpbase=(TAttLine*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Track),G__get_linked_tagnum(&G__AMSR_CintLN_TAttLine),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_SiHit))) {
     AMSR_SiHit *G__Lderived;
     G__Lderived=(AMSR_SiHit*)0x1000;
     {
       AMSR_3DCluster *G__Lpbase=(AMSR_3DCluster*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_SiHit),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_3DCluster),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TMarker3DBox *G__Lpbase=(TMarker3DBox*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_SiHit),G__get_linked_tagnum(&G__AMSR_CintLN_TMarker3DBox),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_SiHit),G__get_linked_tagnum(&G__AMSR_CintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttLine *G__Lpbase=(TAttLine*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_SiHit),G__get_linked_tagnum(&G__AMSR_CintLN_TAttLine),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttFill *G__Lpbase=(TAttFill*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_SiHit),G__get_linked_tagnum(&G__AMSR_CintLN_TAttFill),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_CTCCluster))) {
     AMSR_CTCCluster *G__Lderived;
     G__Lderived=(AMSR_CTCCluster*)0x1000;
     {
       AMSR_3DCluster *G__Lpbase=(AMSR_3DCluster*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_CTCCluster),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_3DCluster),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TMarker3DBox *G__Lpbase=(TMarker3DBox*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_CTCCluster),G__get_linked_tagnum(&G__AMSR_CintLN_TMarker3DBox),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_CTCCluster),G__get_linked_tagnum(&G__AMSR_CintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttLine *G__Lpbase=(TAttLine*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_CTCCluster),G__get_linked_tagnum(&G__AMSR_CintLN_TAttLine),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttFill *G__Lpbase=(TAttFill*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_CTCCluster),G__get_linked_tagnum(&G__AMSR_CintLN_TAttFill),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Particle))) {
     AMSR_Particle *G__Lderived;
     G__Lderived=(AMSR_Particle*)0x1000;
     {
       THelix *G__Lpbase=(THelix*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Particle),G__get_linked_tagnum(&G__AMSR_CintLN_THelix),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TPolyLine3D *G__Lpbase=(TPolyLine3D*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Particle),G__get_linked_tagnum(&G__AMSR_CintLN_TPolyLine3D),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Particle),G__get_linked_tagnum(&G__AMSR_CintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttLine *G__Lpbase=(TAttLine*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Particle),G__get_linked_tagnum(&G__AMSR_CintLN_TAttLine),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_MCParticle))) {
     AMSR_MCParticle *G__Lderived;
     G__Lderived=(AMSR_MCParticle*)0x1000;
     {
       THelix *G__Lpbase=(THelix*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_MCParticle),G__get_linked_tagnum(&G__AMSR_CintLN_THelix),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TPolyLine3D *G__Lpbase=(TPolyLine3D*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_MCParticle),G__get_linked_tagnum(&G__AMSR_CintLN_TPolyLine3D),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_MCParticle),G__get_linked_tagnum(&G__AMSR_CintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttLine *G__Lpbase=(TAttLine*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_MCParticle),G__get_linked_tagnum(&G__AMSR_CintLN_TAttLine),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Canvas))) {
     AMSR_Canvas *G__Lderived;
     G__Lderived=(AMSR_Canvas*)0x1000;
     {
       TCanvas *G__Lpbase=(TCanvas*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Canvas),G__get_linked_tagnum(&G__AMSR_CintLN_TCanvas),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TPad *G__Lpbase=(TPad*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Canvas),G__get_linked_tagnum(&G__AMSR_CintLN_TPad),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TVirtualPad *G__Lpbase=(TVirtualPad*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Canvas),G__get_linked_tagnum(&G__AMSR_CintLN_TVirtualPad),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TWbox *G__Lpbase=(TWbox*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Canvas),G__get_linked_tagnum(&G__AMSR_CintLN_TWbox),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TBox *G__Lpbase=(TBox*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Canvas),G__get_linked_tagnum(&G__AMSR_CintLN_TBox),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Canvas),G__get_linked_tagnum(&G__AMSR_CintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttLine *G__Lpbase=(TAttLine*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Canvas),G__get_linked_tagnum(&G__AMSR_CintLN_TAttLine),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttFill *G__Lpbase=(TAttFill*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Canvas),G__get_linked_tagnum(&G__AMSR_CintLN_TAttFill),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttPad *G__Lpbase=(TAttPad*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Canvas),G__get_linked_tagnum(&G__AMSR_CintLN_TAttPad),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSR_CintLN_Debugger))) {
     Debugger *G__Lderived;
     G__Lderived=(Debugger*)0x1000;
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_Debugger),G__get_linked_tagnum(&G__AMSR_CintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSR_CintLN_TGRunEventDialog))) {
     TGRunEventDialog *G__Lderived;
     G__Lderived=(TGRunEventDialog*)0x1000;
     {
       TGTransientFrame *G__Lpbase=(TGTransientFrame*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TGRunEventDialog),G__get_linked_tagnum(&G__AMSR_CintLN_TGTransientFrame),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TGMainFrame *G__Lpbase=(TGMainFrame*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TGRunEventDialog),G__get_linked_tagnum(&G__AMSR_CintLN_TGMainFrame),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TGCompositeFrame *G__Lpbase=(TGCompositeFrame*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TGRunEventDialog),G__get_linked_tagnum(&G__AMSR_CintLN_TGCompositeFrame),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TGFrame *G__Lpbase=(TGFrame*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TGRunEventDialog),G__get_linked_tagnum(&G__AMSR_CintLN_TGFrame),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TGWindow *G__Lpbase=(TGWindow*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TGRunEventDialog),G__get_linked_tagnum(&G__AMSR_CintLN_TGWindow),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TGObject *G__Lpbase=(TGObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TGRunEventDialog),G__get_linked_tagnum(&G__AMSR_CintLN_TGObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TGRunEventDialog),G__get_linked_tagnum(&G__AMSR_CintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
}

/*********************************************************
* typedef information setup/
*********************************************************/
extern "C" void G__cpp_setup_typetableAMSR_Cint() {

   /* Setting up typedef entry */
   G__search_typename2("Char_t",99,-1,0,
-1);
   G__setnewtype(-1,"Signed Character 1 byte",0);
   G__search_typename2("UChar_t",98,-1,0,
-1);
   G__setnewtype(-1,"Unsigned Character 1 byte",0);
   G__search_typename2("Short_t",115,-1,0,
-1);
   G__setnewtype(-1,"Signed Short integer 2 bytes",0);
   G__search_typename2("UShort_t",114,-1,0,
-1);
   G__setnewtype(-1,"Unsigned Short integer 2 bytes",0);
   G__search_typename2("Int_t",105,-1,0,
-1);
   G__setnewtype(-1,"Signed integer 4 bytes",0);
   G__search_typename2("UInt_t",104,-1,0,
-1);
   G__setnewtype(-1,"Unsigned integer 4 bytes",0);
   G__search_typename2("Seek_t",105,-1,0,
-1);
   G__setnewtype(-1,"File pointer",0);
   G__search_typename2("Long_t",108,-1,0,
-1);
   G__setnewtype(-1,"Signed long integer 8 bytes",0);
   G__search_typename2("ULong_t",107,-1,0,
-1);
   G__setnewtype(-1,"Unsigned long integer 8 bytes",0);
   G__search_typename2("Float_t",102,-1,0,
-1);
   G__setnewtype(-1,"Float 4 bytes",0);
   G__search_typename2("Double_t",100,-1,0,
-1);
   G__setnewtype(-1,"Float 8 bytes",0);
   G__search_typename2("Text_t",99,-1,0,
-1);
   G__setnewtype(-1,"General string",0);
   G__search_typename2("Bool_t",98,-1,0,
-1);
   G__setnewtype(-1,"Boolean (0=false, 1=true)",0);
   G__search_typename2("Byte_t",98,-1,0,
-1);
   G__setnewtype(-1,"Byte (8 bits)",0);
   G__search_typename2("Version_t",115,-1,0,
-1);
   G__setnewtype(-1,"Class version identifier",0);
   G__search_typename2("Option_t",99,-1,0,
-1);
   G__setnewtype(-1,"Option string",0);
   G__search_typename2("Ssiz_t",105,-1,0,
-1);
   G__setnewtype(-1,"String size",0);
   G__search_typename2("Real_t",102,-1,0,
-1);
   G__setnewtype(-1,"TVector and TMatrix element type",0);
   G__search_typename2("VoidFuncPtr_t",89,-1,0,
-1);
   G__setnewtype(-1,"pointer to void function",0);
   G__search_typename2("FreeHookFun_t",89,-1,0,
-1);
   G__setnewtype(-1,NULL,0);
   G__search_typename2("ReAllocFun_t",81,-1,2,
-1);
   G__setnewtype(-1,NULL,0);
   G__search_typename2("ReAllocCFun_t",81,-1,2,
-1);
   G__setnewtype(-1,NULL,0);
   G__search_typename2("Axis_t",102,-1,0,
-1);
   G__setnewtype(-1,"Axis values type",0);
   G__search_typename2("Stat_t",100,-1,0,
-1);
   G__setnewtype(-1,"Statistics type",0);
   G__search_typename2("Font_t",115,-1,0,
-1);
   G__setnewtype(-1,"Font number",0);
   G__search_typename2("Style_t",115,-1,0,
-1);
   G__setnewtype(-1,"Style number",0);
   G__search_typename2("Marker_t",115,-1,0,
-1);
   G__setnewtype(-1,"Marker number",0);
   G__search_typename2("Width_t",115,-1,0,
-1);
   G__setnewtype(-1,"Line width",0);
   G__search_typename2("Color_t",115,-1,0,
-1);
   G__setnewtype(-1,"Color number",0);
   G__search_typename2("SCoord_t",115,-1,0,
-1);
   G__setnewtype(-1,"Screen coordinates",0);
   G__search_typename2("Coord_t",102,-1,0,
-1);
   G__setnewtype(-1,"Pad world coordinates",0);
   G__search_typename2("Angle_t",102,-1,0,
-1);
   G__setnewtype(-1,"Graphics angle",0);
   G__search_typename2("Size_t",102,-1,0,
-1);
   G__setnewtype(-1,"Attribute size",0);
   G__search_typename2("Double_t (*)(Double_t*, Double_t*)",81,-1,0,
-1);
   G__setnewtype(-1,NULL,0);
   G__search_typename2("Handle_t",107,-1,0,
-1);
   G__setnewtype(-1,"Generic resource handle",0);
   G__search_typename2("Display_t",107,-1,0,
-1);
   G__setnewtype(-1,"Display handle",0);
   G__search_typename2("Window_t",107,-1,0,
-1);
   G__setnewtype(-1,"Window handle",0);
   G__search_typename2("Pixmap_t",107,-1,0,
-1);
   G__setnewtype(-1,"Pixmap handle",0);
   G__search_typename2("Drawable_t",107,-1,0,
-1);
   G__setnewtype(-1,"Drawable handle",0);
   G__search_typename2("Colormap_t",107,-1,0,
-1);
   G__setnewtype(-1,"Colormap handle",0);
   G__search_typename2("Cursor_t",107,-1,0,
-1);
   G__setnewtype(-1,"Cursor handle",0);
   G__search_typename2("FontH_t",107,-1,0,
-1);
   G__setnewtype(-1,"Font handle (as opposed to Font_t which is an index)",0);
   G__search_typename2("KeySym_t",107,-1,0,
-1);
   G__setnewtype(-1,"Key symbol handle",0);
   G__search_typename2("Atom_t",107,-1,0,
-1);
   G__setnewtype(-1,"WM token",0);
   G__search_typename2("GContext_t",107,-1,0,
-1);
   G__setnewtype(-1,"Graphics context handle (or pointer, needs to be long)",0);
   G__search_typename2("FontStruct_t",107,-1,0,
-1);
   G__setnewtype(-1,"Pointer to font structure",0);
   G__search_typename2("Mask_t",104,-1,0,
-1);
   G__setnewtype(-1,"Structure mask type",0);
   G__search_typename2("Time_t",107,-1,0,
-1);
   G__setnewtype(-1,"Event time",0);
   G__search_typename2("X3DBuffer",117,G__get_linked_tagnum(&G__AMSR_CintLN__x3d_data_),0,-1);
   G__setnewtype(-1,NULL,0);
   G__search_typename2("Size3D",117,G__get_linked_tagnum(&G__AMSR_CintLN__x3d_sizeof_),0,-1);
   G__setnewtype(-1,NULL,0);
   G__search_typename2("time_t",108,-1,0,
-1);
   G__setnewtype(-1,NULL,0);
}

/*********************************************************
* Data Member information setup/
*********************************************************/

   /* Setting up class,struct,union tag member variable */

   /* TRadioButton */
static void G__setup_memvarTRadioButton(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TRadioButton));
   { TRadioButton *p; p=(TRadioButton*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,66,0,0,-1,G__defined_typename("Bool_t"),-1,2,"fState=",0,"Point to externally supplied state word");
   G__memvar_setup((void*)NULL,67,0,0,-1,-1,-1,2,"fOffMethod=",0,"method to execute when toggled \"off\"");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* TSwitch */
static void G__setup_memvarTSwitch(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TSwitch));
   { TSwitch *p; p=(TSwitch*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TRadioButton),-1,-1,2,"fButton=",0,"the RadioButton displaying the logical state ");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TText),-1,-1,2,"fText=",0,"Text to be displayed");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSR_Axis */
static void G__setup_memvarAMSR_Axis(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Axis));
   { AMSR_Axis *p; p=(AMSR_Axis*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,67,0,0,-1,-1,-1,4,"m_Title=",0,"title of the axis (usually \"x\", \"y\", \"z\")");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSR_Display */
static void G__setup_memvarAMSR_Display(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Display));
   { AMSR_Display *p; p=(AMSR_Display*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,105,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_EAMSR_View),-1,-1,4,"m_View=",0,"view");
   G__memvar_setup((void*)NULL,108,0,0,-1,G__defined_typename("Long_t"),-1,4,"m_IdleTime=",0,"idle time in seconds");
   G__memvar_setup((void*)NULL,67,0,0,-1,G__defined_typename("Text_t"),-1,4,"m_IdleCommand=",0,"command after each idle time");
   G__memvar_setup((void*)NULL,98,0,0,-1,G__defined_typename("Bool_t"),-1,4,"m_IdleOn=",0,"IdleTimer is on?");
   G__memvar_setup((void*)NULL,98,0,0,-1,G__defined_typename("Bool_t"),-1,4,"m_DrawParticles=",0,"Flag True if particles to be drawn");
   G__memvar_setup((void*)NULL,98,0,0,-1,G__defined_typename("Bool_t"),-1,4,"m_DrawGeometry=",0,"Draw geometry?");
   G__memvar_setup((void*)NULL,98,0,0,-1,G__defined_typename("Bool_t"),-1,4,"m_DrawMoreGeometry=",0,"Draw geometry?");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TGeometry),-1,-1,4,"m_Geometry=",0,"Pointer to the geometry");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_GeometrySetter),-1,-1,4,"m_GeoSetter=",0,"Pointer to the geometry setter");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_PTcut=",0,"PT cut to display objects");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_PTcutEGMUNU=",0,"PT cut for Electrons, Gammas, MUons, Neutrinos");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_Theta=",0,"Viewing angle theta");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_Phi=",0,"Viewing angle phi");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Canvas),-1,-1,4,"m_Canvas=",0,"Pointer to the display canvas");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TPad),-1,-1,4,"m_UCPad=",0,"Pointer to the UC pad ");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TPad),-1,-1,4,"m_LogoPad=",0,"Pointer to the logo pad ");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TPad),-1,-1,4,"m_TrigPad=",0,"Pointer to the trigger pad ");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TPad),-1,-1,4,"m_ButtonPad=",0,"Pointer to the buttons pad");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TPad),-1,-1,4,"m_Pad=",0,"Pointer to the event display main pad");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TPad),-1,-1,4,"m_AxisPad[4]=",0,"Pointer to the axis pad");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TPad),-1,-1,4,"m_TitlePad=",0,"Pointer to title pad ");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TPad),-1,-1,4,"m_EventInfoPad=",0,"Pointer to event info pad ");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TPad),-1,-1,4,"m_ObjInfoPad=",0,"Pointer to object info pad ");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TPad),-1,-1,4,"m_PartInfoPad=",0,"Pointer to object info pad");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TRadioButton),-1,-1,4,"m_IdleSwitch=",0,"Pointer to idle TRadioButton");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TList),-1,-1,4,"m_ToBeDrawn=",0,"List of objects to be drawn");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSR_3DCluster */
static void G__setup_memvarAMSR_3DCluster(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_3DCluster));
   { AMSR_3DCluster *p; p=(AMSR_3DCluster*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,2,"fRange=",0,"Range that considered close on view pad");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSR_Maker */
static void G__setup_memvarAMSR_Maker(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Maker));
   { AMSR_Maker *p; p=(AMSR_Maker*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,2,"m_Save=",0,"= 1 if m-Maker to be saved in the Tree");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TObject),-1,-1,2,"m_Fruits=",0,"Pointer to maker fruits (result)");
   G__memvar_setup((void*)NULL,117,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TString),-1,-1,2,"m_BranchName=",0,"Name of branch (if any)");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TList),-1,-1,2,"m_Histograms=",0,"Pointer to list supporting Maker histograms");
   G__memvar_setup((void*)((long)(&p->DrawFruits)-(long)(p)),98,0,0,-1,G__defined_typename("Bool_t"),-1,1,"DrawFruits=",0,"Draw fruits?");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSR_HistBrowser */
static void G__setup_memvarAMSR_HistBrowser(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_HistBrowser));
   { AMSR_HistBrowser *p; p=(AMSR_HistBrowser*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSR_ToFClusterReader */
static void G__setup_memvarAMSR_ToFClusterReader(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ToFClusterReader));
   { AMSR_ToFClusterReader *p; p=(AMSR_ToFClusterReader*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Nclusters=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSR_TrackReader */
static void G__setup_memvarAMSR_TrackReader(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrackReader));
   { AMSR_TrackReader *p; p=(AMSR_TrackReader*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,2,"m_NTracks=",0,"Number of tracks");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TH1F),-1,-1,2,"m_Mult=",0,"tracks multiplicity");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSR_SiHitReader */
static void G__setup_memvarAMSR_SiHitReader(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_SiHitReader));
   { AMSR_SiHitReader *p; p=(AMSR_SiHitReader*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,2,"m_Nclusters=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->DrawUsedHitsOnly)-(long)(p)),98,0,0,-1,G__defined_typename("Bool_t"),-1,1,"DrawUsedHitsOnly=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSR_CTCClusterReader */
static void G__setup_memvarAMSR_CTCClusterReader(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_CTCClusterReader));
   { AMSR_CTCClusterReader *p; p=(AMSR_CTCClusterReader*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Nclusters=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSR_AntiClusterReader */
static void G__setup_memvarAMSR_AntiClusterReader(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_AntiClusterReader));
   { AMSR_AntiClusterReader *p; p=(AMSR_AntiClusterReader*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Nclusters=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSR_TrMCClusterReader */
static void G__setup_memvarAMSR_TrMCClusterReader(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrMCClusterReader));
   { AMSR_TrMCClusterReader *p; p=(AMSR_TrMCClusterReader*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Nclusters=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSR_ParticleReader */
static void G__setup_memvarAMSR_ParticleReader(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ParticleReader));
   { AMSR_ParticleReader *p; p=(AMSR_ParticleReader*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,2,"m_NParticles=",0,"Number of tracks");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TH1F),-1,-1,2,"m_Mult=",0,"tracks multiplicity");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSR_MCParticleReader */
static void G__setup_memvarAMSR_MCParticleReader(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_MCParticleReader));
   { AMSR_MCParticleReader *p; p=(AMSR_MCParticleReader*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,2,"m_NParticles=",0,"Number of tracks");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSR_Ntuple */
static void G__setup_memvarAMSR_Ntuple(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Ntuple));
   { AMSR_Ntuple *p; p=(AMSR_Ntuple*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,99,0,0,-1,G__defined_typename("Text_t"),-1,4,"m_VarNames[20][20]=",0,"Name list of m_NVar variables");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_NVar=",0,"Number of vairables in HGNTV");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TTree),-1,-1,4,"m_SampleTree=",0,"a sample of TTree to communicate with ntuple");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_MemID=",0,"Actual ntuple ID in memory");
   G__memvar_setup((void*)NULL,105,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_EDataFileType),-1,-1,2,"m_DataFileType=",0,"Data file type to access");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,2,"m_Entries=",0,"Entries in ntuple or Root-tree");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,2,"m_Lun=",0,"I/O unit for ntuple file");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,2,"m_NtupleID=",0,"Ntuple ID for current ntuple");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,2,"m_NextID=",0,"Ntuple ID for new ntuple file");
   G__memvar_setup((void*)NULL,98,0,0,-1,G__defined_typename("Bool_t"),-1,2,"m_NtupleOpen=",0,"Indicator whether already ntuple open");
   G__memvar_setup((void*)NULL,98,0,0,-1,G__defined_typename("Bool_t"),-1,2,"m_SameRead=",0,"Same variables reading for HGNTF");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TTree),-1,-1,2,"m_Tree=",0,"ROOT TTree associated with ntuple");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSR_VirtualDisplay */
static void G__setup_memvarAMSR_VirtualDisplay(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_VirtualDisplay));
   { AMSR_VirtualDisplay *p; p=(AMSR_VirtualDisplay*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSR_Root */
static void G__setup_memvarAMSR_Root(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Root));
   { AMSR_Root *p; p=(AMSR_Root*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,67,0,0,-1,-1,-1,4,"m_DataFileName=",0,"Data filename");
   G__memvar_setup((void*)NULL,105,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_EDataFileType),-1,-1,4,"m_DataFileType=",0,"Type of the data file");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Version=",0,"AMSR_Root version number");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_VersionDate=",0,"AMSR_Root version date");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_RunNum=",0,"Run number");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_EventNum=",0,"Event number");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Mode=",0,"Run mode");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_NEvent=",0,"Total events in the tree");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Event=",0,"Event counter in the input tree");
   G__memvar_setup((void*)NULL,108,0,0,-1,G__defined_typename("time_t"),-1,4,"m_Time[2]=",0,"Time");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TTree),-1,-1,4,"m_Tree=",0,"Pointer to the Root tree");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Ntuple),-1,-1,4,"m_Ntuple=",0,"Pointer to input ntuple/tree handler");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TList),-1,-1,4,"m_Makers=",0,"List of Makers");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ToFClusterReader),-1,-1,4,"m_ToFClusterMaker=",0,"Pointer to ToFClusterMaker");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrackReader),-1,-1,4,"m_TrackMaker=",0,"Pointer to TrackMaker");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_SiHitReader),-1,-1,4,"m_SiHitMaker=",0,"Pointer to SiHitMaker");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_CTCClusterReader),-1,-1,4,"m_CTCClusterMaker=",0,"Pointer to ToFClusterMaker");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_AntiClusterReader),-1,-1,4,"m_AntiClusterMaker=",0,"Pointer to AntiClusterMaker");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrMCClusterReader),-1,-1,4,"m_TrMCClusterMaker=",0,"Pointer to AntiClusterMaker");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ParticleReader),-1,-1,4,"m_ParticleMaker=",0,"Pointer to ParticleMaker");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_MCParticleReader),-1,-1,4,"m_MCParticleMaker=",0,"Pointer to ParticleMaker");
   G__memvar_setup((void*)NULL,117,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_HistBrowser),-1,-1,4,"m_HistBrowser=",0,"Object to Browse Maker histograms");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_VirtualDisplay),-1,-1,4,"m_Display=",0,"!Pointer to Event display object");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSR_ToFCluster */
static void G__setup_memvarAMSR_ToFCluster(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ToFCluster));
   { AMSR_ToFCluster *p; p=(AMSR_ToFCluster*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Status=",0,"Status word");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Plane=",0,"Plane number (1...4 : up ... down)");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Bar=",0,"Bar number (spec?)");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_Energy=",0,"Energy deposition in MeV");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_Time=",0,"time in seconds");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_ErTime=",0,"error of m_Time");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Ncells=",0,"Number of cells used for reconstructing ");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Nparticles=",0,"Number of particles passing thru");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_NTracks=",0,"number of tracks in m_Tracks;");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TObjArray),-1,-1,4,"m_Tracks=",0,"Tracks that pass this cluster");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSR_AntiCluster */
static void G__setup_memvarAMSR_AntiCluster(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_AntiCluster));
   { AMSR_AntiCluster *p; p=(AMSR_AntiCluster*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Status=",0,"Status word");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Sector=",0,"Plane number (1...4 : up ... down)");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_Signal=",0,"Edep in MeV");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_NTracks=",0,"number of tracks in m_Tracks;");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TObjArray),-1,-1,4,"m_Tracks=",0,"Tracks that pass this cluster");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSR_TrMCCluster */
static void G__setup_memvarAMSR_TrMCCluster(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrMCCluster));
   { AMSR_TrMCCluster *p; p=(AMSR_TrMCCluster*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Part=",0,"Geant Particle ID");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_Signal=",0,"Edep in MeV");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_NTracks=",0,"number of tracks in m_Tracks;");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TObjArray),-1,-1,4,"m_Tracks=",0,"Tracks that pass this cluster");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSR_Track */
static void G__setup_memvarAMSR_Track(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Track));
   { AMSR_Track *p; p=(AMSR_Track*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_ID=",0,"Track ID");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Status=",0,"Status word");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Pattern=",0,"Hit pattern, see datacards.doc");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_NHits=",0,"Number of hits");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_PHits[6]=",0,"Pointers to trrechit (object now?)");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_FastFitDone=",0,"non-zero if done");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_GeaneFitDone=",0,"non-zero if done");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_AdvancedFitDone=",0,"non-zero if done");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_Chi2StrLine=",0,"Chi2 sz fit (what is sz?)");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_Chi2Circle=",0,"Chi2 circular fit");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_CircleRigidity=",0,"Circular rigidity");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_FChi2=",0,"Chi2 fast nonlinear fit");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_FRigidity=",0,"Rigidity fast nonlinear fit");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_FErrRigidity=",0,"Error of Rigidity fast nonlinear fit");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_FTheta=",0,"Theta fast nonlinear fit");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_FPhi=",0,"Phi fast nonlinear fit");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_FP0[3]=",0,"Coordinates fast nonlinear fit");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_GChi2=",0,"Chi2 Geane fit");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_GRigidity=",0,"Rigidity Geane fit");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_GErrRigidity=",0,"Error of Rigidity Geane fit");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_GTheta=",0,"Theta Geane fit");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_GPhi=",0,"Phi Geane fit");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_GP0[3]=",0,"Coordinates Geane fit");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_HChi2[2]=",0,"Chi2 half-half fit");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_HRigidity[2]=",0,"Rigidity half-half fit");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_HErrRigidity[2]=",0,"Error of Rigidity half-half fit");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_HTheta[2]=",0,"Theta half-half fit");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_HPhi[2]=",0,"Phi half-half fit");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_HP0[2][3]=",0,"Coordinates half-half fit");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_FChi2MS=",0,"Fast chi2 mscat off");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_GChi2MS=",0,"Geane chi2 mscat off");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_RigidityMS=",0,"Fast rigidity mscat off");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_GRigidityMS=",0,"Geane rigidity mscat off");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSR_SiHit */
static void G__setup_memvarAMSR_SiHit(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_SiHit));
   { AMSR_SiHit *p; p=(AMSR_SiHit*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Status=",0,"Status word");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Plane=",0,"Layer number (1...6 : up ... down)");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_X=",0,"X track cluster");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Y=",0,"Y track cluster");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_ErrPosition[3]=",0,"Error of m_Position[]");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_Amplitude=",0,"Amplitude");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_AsymAmpl=",0,"Asymmetry of amplitudes in X vs Y side");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TObjArray),-1,-1,4,"m_Tracks=",0,"Tracks that pass this hit");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSR_CTCCluster */
static void G__setup_memvarAMSR_CTCCluster(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_CTCCluster));
   { AMSR_CTCCluster *p; p=(AMSR_CTCCluster*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Status=",0,"Status word");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Plane=",0,"Plane number (1...4 : up ... down)");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_RawSignal=",0,"Raw signal");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_Signal=",0,"Corrected signal");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_ErrSignal=",0,"Error of the above");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_NTracks=",0,"number of tracks in m_Tracks;");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TObjArray),-1,-1,4,"m_Tracks=",0,"Tracks that pass this cluster");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSR_Particle */
static void G__setup_memvarAMSR_Particle(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Particle));
   { AMSR_Particle *p; p=(AMSR_Particle*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_PCerenkov[2]=",0,"Pointers to matched Cerenkov clusters");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_PBeta=",0,"Pointer to its Beta properties");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_PCharge=",0,"Pointer to Charge properties");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_PTrack=",0,"Pointer to Track properties");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_GeantID=",0,"Particle ID a la Geant");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_Mass=",0,"Mass");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_ErrMass=",0,"Error of mass");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_Momentum=",0,"Momentum");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_ErrMomentum=",0,"Error of momentum");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_Charge=",0,"Charge");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_Theta=",0,"Track Theta (on entry)");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_Phi=",0,"Track Phi (on entry)");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_Position[3]=",0,"Track Position (on entry)");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_SignalCTC[2]=",0,"Signal in Cerenkov");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_BetaCTC[2]=",0,"Beta from Cerenkov");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_ErrBetaCTC[2]=",0,"Error of the above");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_PositionCTC[2][3]=",0,"Track extrapolated into Cerenkov");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_beta=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_errbeta=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_betapattern=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_pattern=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_trstatus=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSR_MCParticle */
static void G__setup_memvarAMSR_MCParticle(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_MCParticle));
   { AMSR_MCParticle *p; p=(AMSR_MCParticle*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_GeantID=",0,"Particle ID a la Geant");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_Mass=",0,"Mass");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_Momentum=",0,"Momentum");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_Charge=",0,"Charge");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_Position[3]=",0,"Track Position (on entry)");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_Dir[3]=",0,"Track Position (on entry)");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSR_Canvas */
static void G__setup_memvarAMSR_Canvas(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Canvas));
   { AMSR_Canvas *p; p=(AMSR_Canvas*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TGFileInfo),-1,-2,2,"m_FileInfo=",0,"File info container for OpenFileCB");
   G__memvar_setup((void*)(&AMSR_Canvas::fTheCanvas),85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TRootCanvas),-1,-2,1,"fTheCanvas=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* Debugger */
static void G__setup_memvarDebugger(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSR_CintLN_Debugger));
   { Debugger *p; p=(Debugger*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,98,0,0,-1,G__defined_typename("Bool_t"),-1,2,"isOn=",0,"Is the debugger on?");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSR_GeometrySetter */
static void G__setup_memvarAMSR_GeometrySetter(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_GeometrySetter));
   { AMSR_GeometrySetter *p; p=(AMSR_GeometrySetter*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,117,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_GeoToggle),-1,-2,2,"m_Toggle[-2147483648]=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TGeometry),-1,-1,2,"m_Geometry=",0,"pointer to the geometry");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,108,0,0,-1,-1,-1,4,"G__virtualinfo=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* TGRunEventDialog */
static void G__setup_memvarTGRunEventDialog(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TGRunEventDialog));
   { TGRunEventDialog *p; p=(TGRunEventDialog*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,73,0,0,-1,G__defined_typename("Int_t"),-1,2,"fRun=",0,"pointer to run number");
   G__memvar_setup((void*)NULL,73,0,0,-1,G__defined_typename("Int_t"),-1,2,"fEvent=",0,"pointer to event number");
   G__memvar_setup((void*)NULL,67,0,1,-1,-1,-2,2,"fLines=",0,"lines in TGTextFrame fText");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TGTextFrame),-1,-1,2,"fText=",0,"TGTextFrame to post a note");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TGLabel),-1,-1,2,"fLrun=",0,"\"Run\" label");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TGLabel),-1,-1,2,"fLevt=",0,"\"Event\" label");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TGTextBuffer),-1,-1,2,"fTbrun=",0,"text buffer for fTrun");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TGTextBuffer),-1,-1,2,"fTbevt=",0,"text buffer for fTevt");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TGTextEntry),-1,-1,2,"fTrun=",0,"text entry for run number");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TGTextEntry),-1,-1,2,"fTevt=",0,"text entry for event number");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TGTextButton),-1,-1,2,"fOk=",0,"button \"Ok\"");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TGTextButton),-1,-1,2,"fClear=",0,"button \"Clear\"");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TGTextButton),-1,-1,2,"fCancel=",0,"button \"Cancel\"");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TGVerticalFrame),-1,-1,2,"fVenter=",0,"frame to contain fHrun and fHevt");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TGHorizontalFrame),-1,-1,2,"fHrun=",0,"frame to contain fLrun and fTrun");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TGHorizontalFrame),-1,-1,2,"fHevt=",0,"frame to contain fLevt and fTevt");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TGHorizontalFrame),-1,-1,2,"fHb=",0,"frame to contain buttons");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TGLayoutHints),-1,-1,2,"fLmain=",0,"layout in main frame");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TGLayoutHints),-1,-1,2,"fLv=",0,"layout in fVenter");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TGLayoutHints),-1,-1,2,"fLhl=",0,"layout for textLabel of run and event     ");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TGLayoutHints),-1,-1,2,"fLht=",0,"layout for textEntry of run and event");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TGLayoutHints),-1,-1,2,"fLhb=",0,"layout for buttons in fHb");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}

extern "C" void G__cpp_setup_memvarAMSR_Cint() {
}
/***********************************************************
************************************************************
************************************************************
************************************************************
************************************************************
************************************************************
************************************************************
***********************************************************/

/*********************************************************
* Member function information setup for each class
*********************************************************/
static void G__setup_memfuncTRadioButton(void) {
   /* TRadioButton */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TRadioButton));
   G__memfunc_setup("TRadioButton",1215,G__TRadioButton_TRadioButton_0_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_TRadioButton),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("TRadioButton",1215,G__TRadioButton_TRadioButton_1_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_TRadioButton),-1,0,7,1,1,0,
"B - 'Bool_t' 0 - state C - - 0 - onMethod "
"f - 'Coord_t' 0 - x1 f - 'Coord_t' 0 - y1 "
"f - 'Coord_t' 0 - x2 f - 'Coord_t' 0 - y2 "
"C - - 0 0 offMethod",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ExecuteEvent",1237,G__TRadioButton_ExecuteEvent_3_0,121,-1,-1,0,3,1,1,0,
"i - 'Int_t' 0 - event i - 'Int_t' 0 - px "
"i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DeclFileName",1145,G__TRadioButton_DeclFileName_4_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__TRadioButton_DeclFileLine_5_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__TRadioButton_ImplFileName_6_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__TRadioButton_ImplFileLine_7_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__TRadioButton_Class_Version_8_0,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__TRadioButton_Class_9_0,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__TRadioButton_Dictionary_0_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__TRadioButton_IsA_1_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__TRadioButton_ShowMembers_2_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__TRadioButton_Streamer_3_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic destructor
   G__memfunc_setup("~TRadioButton",1341,G__TRadioButton_wATRadioButton_4_1,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncTSwitch(void) {
   /* TSwitch */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TSwitch));
   G__memfunc_setup("TSwitch",710,G__TSwitch_TSwitch_0_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_TSwitch),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("TSwitch",710,G__TSwitch_TSwitch_1_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_TSwitch),-1,0,8,1,1,0,
"C - - 0 - title B - 'Bool_t' 0 - state "
"C - - 0 - method f - 'Coord_t' 0 - x1 "
"f - 'Coord_t' 0 - y1 f - 'Coord_t' 0 - x2 "
"f - 'Coord_t' 0 - y2 C - - 0 0 offMethod",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ExecuteEvent",1237,G__TSwitch_ExecuteEvent_3_0,121,-1,-1,0,3,1,1,0,
"i - 'Int_t' 0 - event i - 'Int_t' 0 - px "
"i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("GetButton",924,G__TSwitch_GetButton_4_0,85,G__get_linked_tagnum(&G__AMSR_CintLN_TRadioButton),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileName",1145,G__TSwitch_DeclFileName_5_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__TSwitch_DeclFileLine_6_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__TSwitch_ImplFileName_7_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__TSwitch_ImplFileLine_8_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__TSwitch_Class_Version_9_0,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__TSwitch_Class_0_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__TSwitch_Dictionary_1_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__TSwitch_IsA_2_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__TSwitch_ShowMembers_3_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__TSwitch_Streamer_4_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic destructor
   G__memfunc_setup("~TSwitch",836,G__TSwitch_wATSwitch_5_1,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSR_Axis(void) {
   /* AMSR_Axis */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Axis));
   G__memfunc_setup("AMSR_Axis",807,G__AMSR_Axis_AMSR_Axis_0_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Axis),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSR_Axis",807,G__AMSR_Axis_AMSR_Axis_1_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Axis),-1,0,2,1,1,0,
"F - 'Float_t' 0 - end C - - 0 - title",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSR_Axis",807,G__AMSR_Axis_AMSR_Axis_2_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Axis),-1,0,4,1,1,0,
"f - 'Float_t' 0 - x f - 'Float_t' 0 - y "
"f - 'Float_t' 0 - z C - - 0 - title",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetObjectInfo",1283,G__AMSR_Axis_GetObjectInfo_4_0,67,-1,-1,0,2,1,1,0,
"i - 'Int_t' 0 - px i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Paint",508,G__AMSR_Axis_Paint_5_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DeclFileName",1145,G__AMSR_Axis_DeclFileName_6_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSR_Axis_DeclFileLine_7_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSR_Axis_ImplFileName_8_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSR_Axis_ImplFileLine_9_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSR_Axis_Class_Version_0_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSR_Axis_Class_1_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSR_Axis_Dictionary_2_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSR_Axis_IsA_3_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSR_Axis_ShowMembers_4_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSR_Axis_Streamer_5_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSR_Axis",807,G__AMSR_Axis_AMSR_Axis_6_1,(int)('i'),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Axis),-1,0,1,1,1,0,"u 'AMSR_Axis' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSR_Axis",933,G__AMSR_Axis_wAAMSR_Axis_7_1,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSR_Display(void) {
   /* AMSR_Display */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Display));
   G__memfunc_setup("AMSR_Display",1128,G__AMSR_Display_AMSR_Display_0_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Display),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSR_Display",1128,G__AMSR_Display_AMSR_Display_1_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Display),-1,0,4,1,1,0,
"C - - 0 - title U 'TGeometry' - 0 0 geo "
"i - - 0 1024 resx i - - 0 768 resy",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AllViews",807,G__AMSR_Display_AllViews_3_0,98,-1,G__defined_typename("Bool_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("GetView",699,G__AMSR_Display_GetView_4_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_EAMSR_View),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Clear",487,G__AMSR_Display_Clear_5_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DisplayButtons",1477,G__AMSR_Display_DisplayButtons_6_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DistancetoPrimitive",1991,G__AMSR_Display_DistancetoPrimitive_7_0,105,-1,G__defined_typename("Int_t"),0,2,1,1,0,
"i - 'Int_t' 0 - px i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Draw",398,G__AMSR_Display_Draw_8_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DrawAllViews",1205,G__AMSR_Display_DrawAllViews_9_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DrawFrontAndSideViews",2109,G__AMSR_Display_DrawFrontAndSideViews_0_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DrawParticles",1333,G__AMSR_Display_DrawParticles_1_1,98,-1,G__defined_typename("Bool_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DrawTitle",912,G__AMSR_Display_DrawTitle_2_1,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DrawEventInfo",1308,G__AMSR_Display_DrawEventInfo_3_1,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DrawCaption",1116,G__AMSR_Display_DrawCaption_4_1,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DrawAxis",803,G__AMSR_Display_DrawAxis_5_1,121,-1,-1,0,2,1,1,0,
"i - 'Int_t' 0 0 index C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DrawView",809,G__AMSR_Display_DrawView_6_1,121,-1,-1,0,3,1,1,0,
"f - 'Float_t' 0 9999 theta f - 'Float_t' 0 9999 phi "
"i - 'Int_t' 0 0 index",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DrawViewGL",956,G__AMSR_Display_DrawViewGL_7_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DrawViewX3D",1016,G__AMSR_Display_DrawViewX3D_8_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("AddParticleInfo",1481,G__AMSR_Display_AddParticleInfo_9_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ExecuteEvent",1237,G__AMSR_Display_ExecuteEvent_0_2,121,-1,-1,0,3,1,1,0,
"i - 'Int_t' 0 - event i - 'Int_t' 0 - px "
"i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("GotoRunEvent",1232,G__AMSR_Display_GotoRunEvent_1_2,98,-1,G__defined_typename("Bool_t"),0,0,1,1,0,"","*MENU*",(void*)NULL,1);
   G__memfunc_setup("Pad",277,G__AMSR_Display_Pad_2_2,85,G__get_linked_tagnum(&G__AMSR_CintLN_TPad),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetCanvas",892,G__AMSR_Display_GetCanvas_3_2,85,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Canvas),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("GetObjInfoPad",1244,G__AMSR_Display_GetObjInfoPad_4_2,85,G__get_linked_tagnum(&G__AMSR_CintLN_TPad),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetPartInfoPad",1368,G__AMSR_Display_GetPartInfoPad_5_2,85,G__get_linked_tagnum(&G__AMSR_CintLN_TPad),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetEventInfoPad",1475,G__AMSR_Display_GetEventInfoPad_6_2,85,G__get_linked_tagnum(&G__AMSR_CintLN_TPad),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetTitlePad",1079,G__AMSR_Display_GetTitlePad_7_2,85,G__get_linked_tagnum(&G__AMSR_CintLN_TPad),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Paint",508,G__AMSR_Display_Paint_8_2,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("PaintParticles",1443,G__AMSR_Display_PaintParticles_9_2,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("PTcut",496,G__AMSR_Display_PTcut_0_3,102,-1,G__defined_typename("Float_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("PTcutEGMUNU",961,G__AMSR_Display_PTcutEGMUNU_1_3,102,-1,G__defined_typename("Float_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("GetObjectInfo",1283,G__AMSR_Display_GetObjectInfo_2_3,67,-1,-1,0,2,1,1,0,
"i - 'Int_t' 0 - px i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SetDrawParticles",1633,G__AMSR_Display_SetDrawParticles_3_3,121,-1,-1,0,1,1,1,0,"b - 'Bool_t' 0 kTRUE draw","*MENU*",(void*)NULL,1);
   G__memfunc_setup("SetPTcut",796,G__AMSR_Display_SetPTcut_4_3,121,-1,-1,0,1,1,1,0,"f - 'Float_t' 0 0.4 ptcut","*MENU*",(void*)NULL,1);
   G__memfunc_setup("SetPTcutEGMUNU",1261,G__AMSR_Display_SetPTcutEGMUNU_5_3,121,-1,-1,0,1,1,1,0,"f - 'Float_t' 0 5 ptcut","*MENU*",(void*)NULL,1);
   G__memfunc_setup("SetGeometry",1144,G__AMSR_Display_SetGeometry_6_3,121,-1,-1,0,1,1,1,0,"U 'TGeometry' - 0 - geo","*--MENU*",(void*)NULL,1);
   G__memfunc_setup("SetIdleTimer",1195,G__AMSR_Display_SetIdleTimer_7_3,121,-1,-1,0,2,1,1,0,
"l - 'Long_t' 0 - seconds C - 'Text_t' 0 - command",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsIdleOn",759,G__AMSR_Display_IsIdleOn_8_3,98,-1,G__defined_typename("Bool_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IdleSwitch",1008,G__AMSR_Display_IdleSwitch_9_3,121,-1,-1,0,1,1,1,0,"i - 'Int_t' 0 0 flag",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("IdleTime",781,G__AMSR_Display_IdleTime_0_4,108,-1,G__defined_typename("Long_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IdleCommand",1085,G__AMSR_Display_IdleCommand_1_4,67,-1,G__defined_typename("Text_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IdleTime",781,G__AMSR_Display_IdleTime_2_4,121,-1,-1,0,1,1,1,0,"l - 'Long_t' 0 - seconds",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IdleCommand",1085,G__AMSR_Display_IdleCommand_3_4,121,-1,-1,0,1,1,1,0,"C - 'Text_t' 0 - command",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Print",525,G__AMSR_Display_Print_4_4,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SavePS",562,G__AMSR_Display_SavePS_5_4,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SaveGIF",613,G__AMSR_Display_SaveGIF_6_4,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SetView",711,G__AMSR_Display_SetView_7_4,121,-1,-1,0,2,1,1,0,
"f - 'Float_t' 0 - theta f - 'Float_t' 0 - phi",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SetView",711,G__AMSR_Display_SetView_8_4,121,-1,-1,0,1,1,1,0,"i 'EAMSR_View' - 0 kFrontView newView",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SetNextView",1126,G__AMSR_Display_SetNextView_9_4,121,-1,-1,0,1,1,1,0,"i 'EAMSR_View' - 0 - nextView",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ShowNextEvent",1346,G__AMSR_Display_ShowNextEvent_0_5,121,-1,-1,0,1,1,1,0,"i - 'Int_t' 0 1 delta",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DrawEvent",912,G__AMSR_Display_DrawEvent_1_5,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SizeParticles",1346,G__AMSR_Display_SizeParticles_2_5,121,-1,-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("GetGeometrySetter",1763,G__AMSR_Display_GetGeometrySetter_3_5,85,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_GeometrySetter),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("SetGeo",583,G__AMSR_Display_SetGeo_4_5,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileName",1145,G__AMSR_Display_DeclFileName_5_5,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSR_Display_DeclFileLine_6_5,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSR_Display_ImplFileName_7_5,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSR_Display_ImplFileLine_8_5,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSR_Display_Class_Version_9_5,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSR_Display_Class_0_6,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSR_Display_Dictionary_1_6,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSR_Display_IsA_2_6,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSR_Display_ShowMembers_3_6,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSR_Display_Streamer_4_6,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSR_Display",1128,G__AMSR_Display_AMSR_Display_5_6,(int)('i'),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Display),-1,0,1,1,1,0,"u 'AMSR_Display' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSR_Display",1254,G__AMSR_Display_wAAMSR_Display_6_6,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSR_3DCluster(void) {
   /* AMSR_3DCluster */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_3DCluster));
   G__memfunc_setup("AMSR_3DCluster",1259,G__AMSR_3DCluster_AMSR_3DCluster_0_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_3DCluster),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSR_3DCluster",1259,G__AMSR_3DCluster_AMSR_3DCluster_1_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_3DCluster),-1,0,9,1,1,0,
"f - 'Float_t' 0 - x f - 'Float_t' 0 - y "
"f - 'Float_t' 0 - z f - 'Float_t' 0 - dx "
"f - 'Float_t' 0 - dy f - 'Float_t' 0 - dz "
"f - 'Float_t' 0 - theta f - 'Float_t' 0 - phi "
"i - 'Int_t' 0 1 range",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSR_3DCluster",1259,G__AMSR_3DCluster_AMSR_3DCluster_2_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_3DCluster),-1,0,4,1,1,0,
"F - 'Float_t' 0 - xyz F - 'Float_t' 0 - dxyz "
"F - 'Float_t' 0 - cos i - 'Int_t' 0 5 range",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Delete",595,G__AMSR_3DCluster_Delete_4_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("GetObjectInfo",1283,G__AMSR_3DCluster_GetObjectInfo_5_0,67,-1,-1,0,2,1,1,0,
"i - 'Int_t' 0 - px i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("GetPosition",1141,G__AMSR_3DCluster_GetPosition_6_0,70,-1,G__defined_typename("Float_t"),0,0,1,1,4,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetSize",699,G__AMSR_3DCluster_GetSize_7_0,70,-1,G__defined_typename("Float_t"),0,0,1,1,4,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DistancetoPrimitive",1991,G__AMSR_3DCluster_DistancetoPrimitive_8_0,105,-1,G__defined_typename("Int_t"),0,2,1,1,0,
"i - 'Int_t' 0 - px i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SetPoints",937,G__AMSR_3DCluster_SetPoints_9_0,121,-1,-1,0,1,1,1,0,"F - 'Float_t' 0 - buff",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SetPosition",1153,G__AMSR_3DCluster_SetPosition_0_1,121,-1,-1,0,3,1,1,0,
"f - 'Float_t' 0 - x f - 'Float_t' 0 - y "
"f - 'Float_t' 0 - z",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SetDirection",1229,G__AMSR_3DCluster_SetDirection_1_1,121,-1,-1,0,2,1,1,0,
"f - 'Float_t' 0 - theta f - 'Float_t' 0 - phi",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Sizeof3D",743,G__AMSR_3DCluster_Sizeof3D_2_1,121,-1,-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DeclFileName",1145,G__AMSR_3DCluster_DeclFileName_3_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSR_3DCluster_DeclFileLine_4_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSR_3DCluster_ImplFileName_5_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSR_3DCluster_ImplFileLine_6_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSR_3DCluster_Class_Version_7_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSR_3DCluster_Class_8_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSR_3DCluster_Dictionary_9_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSR_3DCluster_IsA_0_2,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSR_3DCluster_ShowMembers_1_2,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSR_3DCluster_Streamer_2_2,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSR_3DCluster",1259,G__AMSR_3DCluster_AMSR_3DCluster_3_2,(int)('i'),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_3DCluster),-1,0,1,1,1,0,"u 'AMSR_3DCluster' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSR_3DCluster",1385,G__AMSR_3DCluster_wAAMSR_3DCluster_4_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSR_Maker(void) {
   /* AMSR_Maker */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Maker));
   G__memfunc_setup("Finish",609,G__AMSR_Maker_Finish_3_0,121,-1,-1,0,0,1,1,8,"","interface",(void*)NULL,3);
   G__memfunc_setup("Browse",626,G__AMSR_Maker_Browse_4_0,121,-1,-1,0,1,1,1,0,"U 'TBrowser' - 0 - b",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Draw",398,G__AMSR_Maker_Draw_5_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Clear",487,G__AMSR_Maker_Clear_6_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("IsFolder",792,G__AMSR_Maker_IsFolder_7_0,98,-1,G__defined_typename("Bool_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Histograms",1057,G__AMSR_Maker_Histograms_8_0,85,G__get_linked_tagnum(&G__AMSR_CintLN_TList),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Fruits",637,G__AMSR_Maker_Fruits_9_0,85,G__get_linked_tagnum(&G__AMSR_CintLN_TObject),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("PrintInfo",921,G__AMSR_Maker_PrintInfo_0_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Enabled",683,G__AMSR_Maker_Enabled_1_1,98,-1,G__defined_typename("Bool_t"),0,1,1,1,0,"U 'TObject' - 0 - obj","object enabled for drawing?",(void*)NULL,1);
   G__memfunc_setup("Save",399,G__AMSR_Maker_Save_2_1,121,-1,-1,0,1,1,1,0,"i - 'Int_t' 0 1 save",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("MakeBranch",972,G__AMSR_Maker_MakeBranch_3_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Make",382,G__AMSR_Maker_Make_4_1,121,-1,-1,0,0,1,1,8,"","interface",(void*)NULL,3);
   G__memfunc_setup("DeclFileName",1145,G__AMSR_Maker_DeclFileName_5_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSR_Maker_DeclFileLine_6_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSR_Maker_ImplFileName_7_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSR_Maker_ImplFileLine_8_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSR_Maker_Class_Version_9_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSR_Maker_Class_0_2,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSR_Maker_Dictionary_1_2,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSR_Maker_IsA_2_2,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSR_Maker_ShowMembers_3_2,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSR_Maker_Streamer_4_2,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic destructor
   G__memfunc_setup("~AMSR_Maker",1024,G__AMSR_Maker_wAAMSR_Maker_5_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSR_HistBrowser(void) {
   /* AMSR_HistBrowser */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_HistBrowser));
   G__memfunc_setup("AMSR_HistBrowser",1550,G__AMSR_HistBrowser_AMSR_HistBrowser_0_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_HistBrowser),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Browse",626,G__AMSR_HistBrowser_Browse_2_0,121,-1,-1,0,1,1,1,0,"U 'TBrowser' - 0 - b",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("IsFolder",792,G__AMSR_HistBrowser_IsFolder_3_0,98,-1,G__defined_typename("Bool_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DeclFileName",1145,G__AMSR_HistBrowser_DeclFileName_4_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSR_HistBrowser_DeclFileLine_5_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSR_HistBrowser_ImplFileName_6_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSR_HistBrowser_ImplFileLine_7_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSR_HistBrowser_Class_Version_8_0,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSR_HistBrowser_Class_9_0,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSR_HistBrowser_Dictionary_0_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSR_HistBrowser_IsA_1_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSR_HistBrowser_ShowMembers_2_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSR_HistBrowser_Streamer_3_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSR_HistBrowser",1550,G__AMSR_HistBrowser_AMSR_HistBrowser_4_1,(int)('i'),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_HistBrowser),-1,0,1,1,1,0,"u 'AMSR_HistBrowser' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSR_HistBrowser",1676,G__AMSR_HistBrowser_wAAMSR_HistBrowser_5_1,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSR_ToFClusterReader(void) {
   /* AMSR_ToFClusterReader */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ToFClusterReader));
   G__memfunc_setup("AMSR_ToFClusterReader",2000,G__AMSR_ToFClusterReader_AMSR_ToFClusterReader_0_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ToFClusterReader),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSR_ToFClusterReader",2000,G__AMSR_ToFClusterReader_AMSR_ToFClusterReader_1_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ToFClusterReader),-1,0,2,1,1,0,
"C - - 0 - name C - - 0 - title",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Finish",609,G__AMSR_ToFClusterReader_Finish_3_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Make",382,G__AMSR_ToFClusterReader_Make_4_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("PrintInfo",921,G__AMSR_ToFClusterReader_PrintInfo_5_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("AddCluster",1003,G__AMSR_ToFClusterReader_AddCluster_6_0,121,-1,-1,0,12,1,1,0,
"i - 'Int_t' 0 - status i - 'Int_t' 0 - plane "
"i - 'Int_t' 0 - bar f - 'Float_t' 0 - energy "
"f - 'Float_t' 0 - time f - 'Float_t' 0 - ertime "
"F - 'Float_t' 0 - coo F - 'Float_t' 0 - ercoo "
"i - 'Int_t' 0 0 ncells i - 'Int_t' 0 0 npart "
"i - 'Int_t' 0 0 ntracks U 'TObjArray' - 0 0 tracks",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("RemoveCluster",1360,G__AMSR_ToFClusterReader_RemoveCluster_7_0,121,-1,-1,0,1,1,1,0,"i - 'Int_t' 0 - cluster",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Clear",487,G__AMSR_ToFClusterReader_Clear_8_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DeclFileName",1145,G__AMSR_ToFClusterReader_DeclFileName_9_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSR_ToFClusterReader_DeclFileLine_0_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSR_ToFClusterReader_ImplFileName_1_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSR_ToFClusterReader_ImplFileLine_2_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSR_ToFClusterReader_Class_Version_3_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSR_ToFClusterReader_Class_4_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSR_ToFClusterReader_Dictionary_5_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSR_ToFClusterReader_IsA_6_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSR_ToFClusterReader_ShowMembers_7_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSR_ToFClusterReader_Streamer_8_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSR_ToFClusterReader",2000,G__AMSR_ToFClusterReader_AMSR_ToFClusterReader_9_1,(int)('i'),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ToFClusterReader),-1,0,1,1,1,0,"u 'AMSR_ToFClusterReader' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSR_ToFClusterReader",2126,G__AMSR_ToFClusterReader_wAAMSR_ToFClusterReader_0_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSR_TrackReader(void) {
   /* AMSR_TrackReader */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrackReader));
   G__memfunc_setup("AMSR_TrackReader",1498,G__AMSR_TrackReader_AMSR_TrackReader_0_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrackReader),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSR_TrackReader",1498,G__AMSR_TrackReader_AMSR_TrackReader_1_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrackReader),-1,0,2,1,1,0,
"C - - 0 - name C - - 0 - title",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AddTrack",766,G__AMSR_TrackReader_AddTrack_3_0,85,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Track),-1,0,2,1,1,0,
"i - 'Int_t' 0 - code i - 'Int_t' 0 - mcparticle",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Clear",487,G__AMSR_TrackReader_Clear_4_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Finish",609,G__AMSR_TrackReader_Finish_5_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Make",382,G__AMSR_TrackReader_Make_6_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("PrintInfo",921,G__AMSR_TrackReader_PrintInfo_7_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Mult",418,G__AMSR_TrackReader_Mult_8_0,85,G__get_linked_tagnum(&G__AMSR_CintLN_TH1F),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileName",1145,G__AMSR_TrackReader_DeclFileName_9_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSR_TrackReader_DeclFileLine_0_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSR_TrackReader_ImplFileName_1_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSR_TrackReader_ImplFileLine_2_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSR_TrackReader_Class_Version_3_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSR_TrackReader_Class_4_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSR_TrackReader_Dictionary_5_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSR_TrackReader_IsA_6_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSR_TrackReader_ShowMembers_7_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSR_TrackReader_Streamer_8_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSR_TrackReader",1498,G__AMSR_TrackReader_AMSR_TrackReader_9_1,(int)('i'),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrackReader),-1,0,1,1,1,0,"u 'AMSR_TrackReader' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSR_TrackReader",1624,G__AMSR_TrackReader_wAAMSR_TrackReader_0_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSR_SiHitReader(void) {
   /* AMSR_SiHitReader */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_SiHitReader));
   G__memfunc_setup("AMSR_SiHitReader",1478,G__AMSR_SiHitReader_AMSR_SiHitReader_0_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_SiHitReader),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSR_SiHitReader",1478,G__AMSR_SiHitReader_AMSR_SiHitReader_1_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_SiHitReader),-1,0,2,1,1,0,
"C - - 0 - name C - - 0 - title",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Finish",609,G__AMSR_SiHitReader_Finish_3_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Make",382,G__AMSR_SiHitReader_Make_4_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("PrintInfo",921,G__AMSR_SiHitReader_PrintInfo_5_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Enabled",683,G__AMSR_SiHitReader_Enabled_6_0,98,-1,G__defined_typename("Bool_t"),0,1,1,1,0,"U 'TObject' - 0 - obj",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("AddCluster",1003,G__AMSR_SiHitReader_AddCluster_7_0,121,-1,-1,0,9,1,1,0,
"i - 'Int_t' 0 - status i - 'Int_t' 0 - plane "
"i - 'Int_t' 0 - px i - 'Int_t' 0 - py "
"F - 'Float_t' 0 - hit F - 'Float_t' 0 - errhit "
"f - 'Float_t' 0 - ampl f - 'Float_t' 0 - asym "
"U 'TObjArray' - 0 0 tracks",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("RemoveCluster",1360,G__AMSR_SiHitReader_RemoveCluster_8_0,121,-1,-1,0,1,1,1,0,"i - 'Int_t' 0 - cluster",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Clear",487,G__AMSR_SiHitReader_Clear_9_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DeclFileName",1145,G__AMSR_SiHitReader_DeclFileName_0_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSR_SiHitReader_DeclFileLine_1_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSR_SiHitReader_ImplFileName_2_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSR_SiHitReader_ImplFileLine_3_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSR_SiHitReader_Class_Version_4_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSR_SiHitReader_Class_5_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSR_SiHitReader_Dictionary_6_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSR_SiHitReader_IsA_7_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSR_SiHitReader_ShowMembers_8_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSR_SiHitReader_Streamer_9_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSR_SiHitReader",1478,G__AMSR_SiHitReader_AMSR_SiHitReader_0_2,(int)('i'),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_SiHitReader),-1,0,1,1,1,0,"u 'AMSR_SiHitReader' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSR_SiHitReader",1604,G__AMSR_SiHitReader_wAAMSR_SiHitReader_1_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSR_CTCClusterReader(void) {
   /* AMSR_CTCClusterReader */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_CTCClusterReader));
   G__memfunc_setup("AMSR_CTCClusterReader",1953,G__AMSR_CTCClusterReader_AMSR_CTCClusterReader_0_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_CTCClusterReader),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSR_CTCClusterReader",1953,G__AMSR_CTCClusterReader_AMSR_CTCClusterReader_1_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_CTCClusterReader),-1,0,2,1,1,0,
"C - - 0 - name C - - 0 - title",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Finish",609,G__AMSR_CTCClusterReader_Finish_3_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Make",382,G__AMSR_CTCClusterReader_Make_4_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("PrintInfo",921,G__AMSR_CTCClusterReader_PrintInfo_5_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("AddCluster",1003,G__AMSR_CTCClusterReader_AddCluster_6_0,121,-1,-1,0,9,1,1,0,
"i - 'Int_t' 0 - status i - 'Int_t' 0 - plane "
"f - 'Float_t' 0 - rawsignal f - 'Float_t' 0 - signal "
"f - 'Float_t' 0 - ersig F - 'Float_t' 0 - coo "
"F - 'Float_t' 0 - ercoo i - 'Int_t' 0 0 ntracks "
"U 'TObjArray' - 0 0 tracks",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("RemoveCluster",1360,G__AMSR_CTCClusterReader_RemoveCluster_7_0,121,-1,-1,0,1,1,1,0,"i - 'Int_t' 0 - cluster",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Clear",487,G__AMSR_CTCClusterReader_Clear_8_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DeclFileName",1145,G__AMSR_CTCClusterReader_DeclFileName_9_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSR_CTCClusterReader_DeclFileLine_0_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSR_CTCClusterReader_ImplFileName_1_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSR_CTCClusterReader_ImplFileLine_2_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSR_CTCClusterReader_Class_Version_3_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSR_CTCClusterReader_Class_4_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSR_CTCClusterReader_Dictionary_5_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSR_CTCClusterReader_IsA_6_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSR_CTCClusterReader_ShowMembers_7_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSR_CTCClusterReader_Streamer_8_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSR_CTCClusterReader",1953,G__AMSR_CTCClusterReader_AMSR_CTCClusterReader_9_1,(int)('i'),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_CTCClusterReader),-1,0,1,1,1,0,"u 'AMSR_CTCClusterReader' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSR_CTCClusterReader",2079,G__AMSR_CTCClusterReader_wAAMSR_CTCClusterReader_0_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSR_AntiClusterReader(void) {
   /* AMSR_AntiClusterReader */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_AntiClusterReader));
   G__memfunc_setup("AMSR_AntiClusterReader",2131,G__AMSR_AntiClusterReader_AMSR_AntiClusterReader_0_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_AntiClusterReader),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSR_AntiClusterReader",2131,G__AMSR_AntiClusterReader_AMSR_AntiClusterReader_1_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_AntiClusterReader),-1,0,2,1,1,0,
"C - - 0 - name C - - 0 - title",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Finish",609,G__AMSR_AntiClusterReader_Finish_3_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Make",382,G__AMSR_AntiClusterReader_Make_4_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("PrintInfo",921,G__AMSR_AntiClusterReader_PrintInfo_5_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("AddCluster",1003,G__AMSR_AntiClusterReader_AddCluster_6_0,121,-1,-1,0,7,1,1,0,
"i - 'Int_t' 0 - status i - 'Int_t' 0 - sector "
"f - 'Float_t' 0 - signal F - 'Float_t' 0 - coo "
"F - 'Float_t' 0 - ercoo i - 'Int_t' 0 0 ntracks "
"U 'TObjArray' - 0 0 tracks",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("RemoveCluster",1360,G__AMSR_AntiClusterReader_RemoveCluster_7_0,121,-1,-1,0,1,1,1,0,"i - 'Int_t' 0 - cluster",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Clear",487,G__AMSR_AntiClusterReader_Clear_8_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DeclFileName",1145,G__AMSR_AntiClusterReader_DeclFileName_9_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSR_AntiClusterReader_DeclFileLine_0_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSR_AntiClusterReader_ImplFileName_1_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSR_AntiClusterReader_ImplFileLine_2_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSR_AntiClusterReader_Class_Version_3_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSR_AntiClusterReader_Class_4_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSR_AntiClusterReader_Dictionary_5_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSR_AntiClusterReader_IsA_6_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSR_AntiClusterReader_ShowMembers_7_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSR_AntiClusterReader_Streamer_8_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSR_AntiClusterReader",2131,G__AMSR_AntiClusterReader_AMSR_AntiClusterReader_9_1,(int)('i'),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_AntiClusterReader),-1,0,1,1,1,0,"u 'AMSR_AntiClusterReader' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSR_AntiClusterReader",2257,G__AMSR_AntiClusterReader_wAAMSR_AntiClusterReader_0_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSR_TrMCClusterReader(void) {
   /* AMSR_TrMCClusterReader */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrMCClusterReader));
   G__memfunc_setup("AMSR_TrMCClusterReader",2077,G__AMSR_TrMCClusterReader_AMSR_TrMCClusterReader_0_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrMCClusterReader),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSR_TrMCClusterReader",2077,G__AMSR_TrMCClusterReader_AMSR_TrMCClusterReader_1_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrMCClusterReader),-1,0,2,1,1,0,
"C - - 0 - name C - - 0 - title",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Finish",609,G__AMSR_TrMCClusterReader_Finish_3_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Make",382,G__AMSR_TrMCClusterReader_Make_4_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("PrintInfo",921,G__AMSR_TrMCClusterReader_PrintInfo_5_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("AddCluster",1003,G__AMSR_TrMCClusterReader_AddCluster_6_0,121,-1,-1,0,6,1,1,0,
"i - 'Int_t' 0 - part f - 'Float_t' 0 - signal "
"F - 'Float_t' 0 - coo F - 'Float_t' 0 - ercoo "
"i - 'Int_t' 0 0 ntracks U 'TObjArray' - 0 0 tracks",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("RemoveCluster",1360,G__AMSR_TrMCClusterReader_RemoveCluster_7_0,121,-1,-1,0,1,1,1,0,"i - 'Int_t' 0 - cluster",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Clear",487,G__AMSR_TrMCClusterReader_Clear_8_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DeclFileName",1145,G__AMSR_TrMCClusterReader_DeclFileName_9_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSR_TrMCClusterReader_DeclFileLine_0_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSR_TrMCClusterReader_ImplFileName_1_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSR_TrMCClusterReader_ImplFileLine_2_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSR_TrMCClusterReader_Class_Version_3_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSR_TrMCClusterReader_Class_4_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSR_TrMCClusterReader_Dictionary_5_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSR_TrMCClusterReader_IsA_6_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSR_TrMCClusterReader_ShowMembers_7_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSR_TrMCClusterReader_Streamer_8_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSR_TrMCClusterReader",2077,G__AMSR_TrMCClusterReader_AMSR_TrMCClusterReader_9_1,(int)('i'),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrMCClusterReader),-1,0,1,1,1,0,"u 'AMSR_TrMCClusterReader' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSR_TrMCClusterReader",2203,G__AMSR_TrMCClusterReader_wAAMSR_TrMCClusterReader_0_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSR_ParticleReader(void) {
   /* AMSR_ParticleReader */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ParticleReader));
   G__memfunc_setup("AMSR_ParticleReader",1817,G__AMSR_ParticleReader_AMSR_ParticleReader_0_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ParticleReader),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSR_ParticleReader",1817,G__AMSR_ParticleReader_AMSR_ParticleReader_1_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ParticleReader),-1,0,2,1,1,0,
"C - - 0 - name C - - 0 - title",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AddParticle",1085,G__AMSR_ParticleReader_AddParticle_3_0,85,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Particle),-1,0,2,1,1,0,
"i - 'Int_t' 0 - code i - 'Int_t' 0 - mcparticle",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Clear",487,G__AMSR_ParticleReader_Clear_4_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Finish",609,G__AMSR_ParticleReader_Finish_5_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Make",382,G__AMSR_ParticleReader_Make_6_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("PrintInfo",921,G__AMSR_ParticleReader_PrintInfo_7_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Mult",418,G__AMSR_ParticleReader_Mult_8_0,85,G__get_linked_tagnum(&G__AMSR_CintLN_TH1F),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileName",1145,G__AMSR_ParticleReader_DeclFileName_9_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSR_ParticleReader_DeclFileLine_0_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSR_ParticleReader_ImplFileName_1_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSR_ParticleReader_ImplFileLine_2_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSR_ParticleReader_Class_Version_3_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSR_ParticleReader_Class_4_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSR_ParticleReader_Dictionary_5_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSR_ParticleReader_IsA_6_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSR_ParticleReader_ShowMembers_7_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSR_ParticleReader_Streamer_8_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSR_ParticleReader",1817,G__AMSR_ParticleReader_AMSR_ParticleReader_9_1,(int)('i'),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ParticleReader),-1,0,1,1,1,0,"u 'AMSR_ParticleReader' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSR_ParticleReader",1943,G__AMSR_ParticleReader_wAAMSR_ParticleReader_0_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSR_MCParticleReader(void) {
   /* AMSR_MCParticleReader */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_MCParticleReader));
   G__memfunc_setup("AMSR_MCParticleReader",1961,G__AMSR_MCParticleReader_AMSR_MCParticleReader_0_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_MCParticleReader),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSR_MCParticleReader",1961,G__AMSR_MCParticleReader_AMSR_MCParticleReader_1_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_MCParticleReader),-1,0,2,1,1,0,
"C - - 0 - name C - - 0 - title",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AddParticle",1085,G__AMSR_MCParticleReader_AddParticle_3_0,85,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_MCParticle),-1,0,2,1,1,0,
"i - 'Int_t' 0 - code i - 'Int_t' 0 - mcparticle",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Clear",487,G__AMSR_MCParticleReader_Clear_4_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Finish",609,G__AMSR_MCParticleReader_Finish_5_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Make",382,G__AMSR_MCParticleReader_Make_6_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("PrintInfo",921,G__AMSR_MCParticleReader_PrintInfo_7_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DeclFileName",1145,G__AMSR_MCParticleReader_DeclFileName_8_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSR_MCParticleReader_DeclFileLine_9_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSR_MCParticleReader_ImplFileName_0_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSR_MCParticleReader_ImplFileLine_1_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSR_MCParticleReader_Class_Version_2_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSR_MCParticleReader_Class_3_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSR_MCParticleReader_Dictionary_4_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSR_MCParticleReader_IsA_5_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSR_MCParticleReader_ShowMembers_6_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSR_MCParticleReader_Streamer_7_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSR_MCParticleReader",1961,G__AMSR_MCParticleReader_AMSR_MCParticleReader_8_1,(int)('i'),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_MCParticleReader),-1,0,1,1,1,0,"u 'AMSR_MCParticleReader' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSR_MCParticleReader",2087,G__AMSR_MCParticleReader_wAAMSR_MCParticleReader_9_1,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSR_Ntuple(void) {
   /* AMSR_Ntuple */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Ntuple));
   G__memfunc_setup("SetTreeAddress",1410,(G__InterfaceMethod)NULL,121,-1,-1,0,0,1,2,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("CreateSampleTree",1606,(G__InterfaceMethod)NULL,121,-1,-1,0,0,1,2,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("SetVarNames",1097,(G__InterfaceMethod)NULL,121,-1,-1,0,0,1,2,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSR_Ntuple",1034,G__AMSR_Ntuple_AMSR_Ntuple_3_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Ntuple),-1,0,2,1,1,0,
"C - - 0 \"AMSR_Ntuple\" name C - - 0 \"The AMS Ntuple\" title",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("CloseNtuple",1134,G__AMSR_Ntuple_CloseNtuple_5_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetEntries",1018,G__AMSR_Ntuple_GetEntries_6_0,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetEvent",802,G__AMSR_Ntuple_GetEvent_7_0,121,-1,-1,0,1,1,1,0,"i - 'Int_t' 0 - event",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetEvtNum",895,G__AMSR_Ntuple_GetEvtNum_8_0,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetNextID",844,G__AMSR_Ntuple_GetNextID_9_0,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetLun",591,G__AMSR_Ntuple_GetLun_0_1,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetRunNum",901,G__AMSR_Ntuple_GetRunNum_1_1,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetRunTime",996,G__AMSR_Ntuple_GetRunTime_2_1,121,-1,-1,0,1,1,1,0,"L - 'time_t' 0 - time",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetRunType",1015,G__AMSR_Ntuple_GetRunType_3_1,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetNtupleID",1061,G__AMSR_Ntuple_GetNtupleID_4_1,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetTree",688,G__AMSR_Ntuple_GetTree_5_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TTree),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsNtupleOpen",1222,G__AMSR_Ntuple_IsNtupleOpen_6_1,98,-1,G__defined_typename("Bool_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("OpenNtuple",1034,G__AMSR_Ntuple_OpenNtuple_7_1,105,-1,G__defined_typename("Int_t"),0,1,1,1,0,"C - - 0 - ntpfile",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("SetBranchStatus",1534,G__AMSR_Ntuple_SetBranchStatus_8_1,121,-1,-1,0,2,1,1,0,
"C - - 0 - varname b - 'Bool_t' 0 - status",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("SetNtupleID",1073,G__AMSR_Ntuple_SetNtupleID_9_1,121,-1,-1,0,1,1,1,0,"i - 'Int_t' 0 - id",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("SetTree",700,G__AMSR_Ntuple_SetTree_0_2,121,-1,-1,0,1,1,1,0,"U 'TTree' - 0 0 t",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileName",1145,G__AMSR_Ntuple_DeclFileName_1_2,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSR_Ntuple_DeclFileLine_2_2,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSR_Ntuple_ImplFileName_3_2,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSR_Ntuple_ImplFileLine_4_2,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSR_Ntuple_Class_Version_5_2,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSR_Ntuple_Class_6_2,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSR_Ntuple_Dictionary_7_2,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSR_Ntuple_IsA_8_2,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSR_Ntuple_ShowMembers_9_2,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSR_Ntuple_Streamer_0_3,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSR_Ntuple",1034,G__AMSR_Ntuple_AMSR_Ntuple_1_3,(int)('i'),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Ntuple),-1,0,1,1,1,0,"u 'AMSR_Ntuple' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSR_Ntuple",1160,G__AMSR_Ntuple_wAAMSR_Ntuple_2_3,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSR_VirtualDisplay(void) {
   /* AMSR_VirtualDisplay */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_VirtualDisplay));
   G__memfunc_setup("Clear",487,G__AMSR_VirtualDisplay_Clear_2_0,121,-1,-1,0,1,1,1,8,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("DisplayButtons",1477,G__AMSR_VirtualDisplay_DisplayButtons_3_0,121,-1,-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("DistancetoPrimitive",1991,G__AMSR_VirtualDisplay_DistancetoPrimitive_4_0,105,-1,G__defined_typename("Int_t"),0,2,1,1,8,
"i - 'Int_t' 0 - px i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("Draw",398,G__AMSR_VirtualDisplay_Draw_5_0,121,-1,-1,0,1,1,1,8,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("DrawAllViews",1205,G__AMSR_VirtualDisplay_DrawAllViews_6_0,121,-1,-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("DrawFrontAndSideViews",2109,G__AMSR_VirtualDisplay_DrawFrontAndSideViews_7_0,121,-1,-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("DrawParticles",1333,G__AMSR_VirtualDisplay_DrawParticles_8_0,98,-1,G__defined_typename("Bool_t"),0,0,1,1,8,"",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("DrawCaption",1116,G__AMSR_VirtualDisplay_DrawCaption_9_0,121,-1,-1,0,1,1,1,8,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("DrawView",809,G__AMSR_VirtualDisplay_DrawView_0_1,121,-1,-1,0,3,1,1,8,
"f - 'Float_t' 0 - theta f - 'Float_t' 0 - phi "
"i - 'Int_t' 0 - index",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("DrawViewGL",956,G__AMSR_VirtualDisplay_DrawViewGL_1_1,121,-1,-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("DrawViewX3D",1016,G__AMSR_VirtualDisplay_DrawViewX3D_2_1,121,-1,-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("ExecuteEvent",1237,G__AMSR_VirtualDisplay_ExecuteEvent_3_1,121,-1,-1,0,3,1,1,8,
"i - 'Int_t' 0 - event i - 'Int_t' 0 - px "
"i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("GetCanvas",892,G__AMSR_VirtualDisplay_GetCanvas_4_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Canvas),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("Paint",508,G__AMSR_VirtualDisplay_Paint_5_1,121,-1,-1,0,1,1,1,8,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("PaintParticles",1443,G__AMSR_VirtualDisplay_PaintParticles_6_1,121,-1,-1,0,1,1,1,8,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("PTcut",496,G__AMSR_VirtualDisplay_PTcut_7_1,102,-1,G__defined_typename("Float_t"),0,0,1,1,8,"",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("PTcutEGMUNU",961,G__AMSR_VirtualDisplay_PTcutEGMUNU_8_1,102,-1,G__defined_typename("Float_t"),0,0,1,1,8,"",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("SetView",711,G__AMSR_VirtualDisplay_SetView_9_1,121,-1,-1,0,2,1,1,8,
"f - 'Float_t' 0 - theta f - 'Float_t' 0 - phi",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("SetView",711,G__AMSR_VirtualDisplay_SetView_0_2,121,-1,-1,0,1,1,1,8,"i 'EAMSR_View' - 0 kFrontView newView",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("IdleSwitch",1008,G__AMSR_VirtualDisplay_IdleSwitch_1_2,121,-1,-1,0,1,1,1,8,"i - 'Int_t' 0 0 flag",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("Print",525,G__AMSR_VirtualDisplay_Print_2_2,121,-1,-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("SavePS",562,G__AMSR_VirtualDisplay_SavePS_3_2,121,-1,-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("SaveGIF",613,G__AMSR_VirtualDisplay_SaveGIF_4_2,121,-1,-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("GotoRunEvent",1232,G__AMSR_VirtualDisplay_GotoRunEvent_5_2,98,-1,G__defined_typename("Bool_t"),0,0,1,1,8,"",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("ShowNextEvent",1346,G__AMSR_VirtualDisplay_ShowNextEvent_6_2,121,-1,-1,0,1,1,1,8,"i - 'Int_t' 0 1 delta",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("SizeParticles",1346,G__AMSR_VirtualDisplay_SizeParticles_7_2,121,-1,-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DeclFileName",1145,G__AMSR_VirtualDisplay_DeclFileName_8_2,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSR_VirtualDisplay_DeclFileLine_9_2,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSR_VirtualDisplay_ImplFileName_0_3,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSR_VirtualDisplay_ImplFileLine_1_3,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSR_VirtualDisplay_Class_Version_2_3,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSR_VirtualDisplay_Class_3_3,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSR_VirtualDisplay_Dictionary_4_3,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSR_VirtualDisplay_IsA_5_3,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSR_VirtualDisplay_ShowMembers_6_3,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSR_VirtualDisplay_Streamer_7_3,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic destructor
   G__memfunc_setup("~AMSR_VirtualDisplay",1997,G__AMSR_VirtualDisplay_wAAMSR_VirtualDisplay_8_3,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSR_Root(void) {
   /* AMSR_Root */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Root));
   G__memfunc_setup("CommonConstruct",1582,(G__InterfaceMethod)NULL,121,-1,-1,0,0,1,4,0,"","Common part of all constructors",(void*)NULL,0);
   G__memfunc_setup("AMSR_Root",822,G__AMSR_Root_AMSR_Root_1_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Root),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSR_Root",822,G__AMSR_Root_AMSR_Root_2_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Root),-1,0,2,1,1,0,
"C - - 0 - name C - - 0 \"The AMS Display with Root\" title",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsGolden",789,G__AMSR_Root_IsGolden_3_0,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("OpenDataFile",1164,G__AMSR_Root_OpenDataFile_5_0,105,-1,G__defined_typename("Int_t"),0,2,1,1,0,
"C - - 0 - filename i 'EDataFileType' - 0 - type",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Browse",626,G__AMSR_Root_Browse_6_0,121,-1,-1,0,1,1,1,0,"U 'TBrowser' - 0 - b",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("GetEvent",802,G__AMSR_Root_GetEvent_7_0,98,-1,G__defined_typename("Bool_t"),0,1,1,1,0,"i - 'Int_t' 0 1 event","*MENU*",(void*)NULL,1);
   G__memfunc_setup("GetEvent",802,G__AMSR_Root_GetEvent_8_0,98,-1,G__defined_typename("Bool_t"),0,2,1,1,0,
"i - 'Int_t' 0 - run i - 'Int_t' 0 - event","*MENU*",(void*)NULL,1);
   G__memfunc_setup("Init",404,G__AMSR_Root_Init_9_0,121,-1,-1,0,1,1,1,0,"U 'TTree' - 0 0 h1","Initialize to read from h1",(void*)NULL,1);
   G__memfunc_setup("Finish",609,G__AMSR_Root_Finish_0_1,121,-1,-1,0,0,1,1,0,"","Finish a run",(void*)NULL,1);
   G__memfunc_setup("Display",726,G__AMSR_Root_Display_1_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_VirtualDisplay),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Draw",398,G__AMSR_Root_Draw_2_1,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option","*MENU*",(void*)NULL,1);
   G__memfunc_setup("Paint",508,G__AMSR_Root_Paint_3_1,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Clear",487,G__AMSR_Root_Clear_4_1,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("IsFolder",792,G__AMSR_Root_IsFolder_5_1,98,-1,G__defined_typename("Bool_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("PrintInfo",921,G__AMSR_Root_PrintInfo_6_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("GetVersion",1030,G__AMSR_Root_GetVersion_7_1,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetVersionDate",1412,G__AMSR_Root_GetVersionDate_8_1,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("RunNum",613,G__AMSR_Root_RunNum_9_1,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetTime",687,G__AMSR_Root_GetTime_0_2,67,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("EventNum",818,G__AMSR_Root_EventNum_1_2,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Event",514,G__AMSR_Root_Event_2_2,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("NEvent",592,G__AMSR_Root_NEvent_3_2,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Mode",389,G__AMSR_Root_Mode_4_2,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Tree",400,G__AMSR_Root_Tree_5_2,85,G__get_linked_tagnum(&G__AMSR_CintLN_TTree),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetDataFileName",1435,G__AMSR_Root_GetDataFileName_6_2,67,-1,G__defined_typename("Text_t"),0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetDataFileType",1468,G__AMSR_Root_GetDataFileType_7_2,105,G__get_linked_tagnum(&G__AMSR_CintLN_EDataFileType),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetNtuple",920,G__AMSR_Root_GetNtuple_8_2,85,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Ntuple),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Makers",611,G__AMSR_Root_Makers_9_2,85,G__get_linked_tagnum(&G__AMSR_CintLN_TList),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Maker",496,G__AMSR_Root_Maker_0_3,85,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Maker),-1,0,1,1,1,0,"C - - 0 - name",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ToFClusterMaker",1499,G__AMSR_Root_ToFClusterMaker_1_3,85,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ToFClusterReader),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("TrackMaker",997,G__AMSR_Root_TrackMaker_2_3,85,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrackReader),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("SiHitMaker",977,G__AMSR_Root_SiHitMaker_3_3,85,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_SiHitReader),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AntiClusterMaker",1630,G__AMSR_Root_AntiClusterMaker_4_3,85,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_AntiClusterReader),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("TrMCClusterMaker",1576,G__AMSR_Root_TrMCClusterMaker_5_3,85,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrMCClusterReader),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("CTCClusterMaker",1452,G__AMSR_Root_CTCClusterMaker_6_3,85,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_CTCClusterReader),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ParticleMaker",1316,G__AMSR_Root_ParticleMaker_7_3,85,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ParticleReader),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("MCParticleMaker",1460,G__AMSR_Root_MCParticleMaker_8_3,85,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_MCParticleReader),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("SetDefaultParameters",2053,G__AMSR_Root_SetDefaultParameters_9_3,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SetEvent",814,G__AMSR_Root_SetEvent_0_4,121,-1,-1,0,1,1,1,0,"i - 'Int_t' 0 1 event",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SetMode",689,G__AMSR_Root_SetMode_1_4,121,-1,-1,0,1,1,1,0,"i - 'Int_t' 0 0 mode",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SetDisplay",1026,G__AMSR_Root_SetDisplay_2_4,121,-1,-1,0,1,1,1,0,"U 'AMSR_VirtualDisplay' - 0 - display",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Make",382,G__AMSR_Root_Make_3_4,121,-1,-1,0,1,1,1,0,"i - 'Int_t' 0 0 i",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("MakeTree",782,G__AMSR_Root_MakeTree_4_4,121,-1,-1,0,2,1,1,0,
"C - - 0 \"AMS Event\" name C - - 0 \"AMSR_Root tree\" title",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("FillTree",791,G__AMSR_Root_FillTree_5_4,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("SortDown",832,G__AMSR_Root_SortDown_6_4,121,-1,-1,0,4,1,1,0,
"i - 'Int_t' 0 - n F - 'Float_t' 0 - a "
"I - 'Int_t' 0 - index b - 'Bool_t' 0 kTRUE down",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileName",1145,G__AMSR_Root_DeclFileName_7_4,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSR_Root_DeclFileLine_8_4,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSR_Root_ImplFileName_9_4,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSR_Root_ImplFileLine_0_5,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSR_Root_Class_Version_1_5,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSR_Root_Class_2_5,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSR_Root_Dictionary_3_5,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSR_Root_IsA_4_5,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSR_Root_ShowMembers_5_5,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSR_Root_Streamer_6_5,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSR_Root",822,G__AMSR_Root_AMSR_Root_7_5,(int)('i'),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Root),-1,0,1,1,1,0,"u 'AMSR_Root' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSR_Root",948,G__AMSR_Root_wAAMSR_Root_8_5,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSR_ToFCluster(void) {
   /* AMSR_ToFCluster */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ToFCluster));
   G__memfunc_setup("AMSR_ToFCluster",1405,G__AMSR_ToFCluster_AMSR_ToFCluster_0_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ToFCluster),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSR_ToFCluster",1405,G__AMSR_ToFCluster_AMSR_ToFCluster_1_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ToFCluster),-1,0,12,1,1,0,
"i - 'Int_t' 0 - status i - 'Int_t' 0 - plane "
"i - 'Int_t' 0 - bar f - 'Float_t' 0 - energy "
"f - 'Float_t' 0 - time f - 'Float_t' 0 - ertime "
"F - 'Float_t' 0 - coo F - 'Float_t' 0 - ercoo "
"i - 'Int_t' 0 - ncells i - 'Int_t' 0 - npart "
"i - 'Int_t' 0 - ntracks U 'TObjArray' - 0 - tracks",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetObjectInfo",1283,G__AMSR_ToFCluster_GetObjectInfo_3_0,67,-1,-1,0,2,1,1,0,
"i - 'Int_t' 0 - px i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Paint",508,G__AMSR_ToFCluster_Paint_4_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Clear",487,G__AMSR_ToFCluster_Clear_5_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("GetStatus",932,G__AMSR_ToFCluster_GetStatus_6_0,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetPlane",784,G__AMSR_ToFCluster_GetPlane_7_0,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetBar",565,G__AMSR_ToFCluster_GetBar_8_0,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetEnergy",906,G__AMSR_ToFCluster_GetEnergy_9_0,102,-1,G__defined_typename("Float_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetTime",687,G__AMSR_ToFCluster_GetTime_0_1,102,-1,G__defined_typename("Float_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetNcells",897,G__AMSR_ToFCluster_GetNcells_1_1,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetNparticles",1333,G__AMSR_ToFCluster_GetNparticles_2_1,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetNTracks",982,G__AMSR_ToFCluster_GetNTracks_3_1,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetTracks",904,G__AMSR_ToFCluster_GetTracks_4_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TObjArray),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Is3D",307,G__AMSR_ToFCluster_Is3D_5_1,98,-1,G__defined_typename("Bool_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SetEnergy",918,G__AMSR_ToFCluster_SetEnergy_6_1,121,-1,-1,0,1,1,1,0,"f - 'Float_t' 0 - energy",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("SetTime",699,G__AMSR_ToFCluster_SetTime_7_1,121,-1,-1,0,1,1,1,0,"f - 'Float_t' 0 - time",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileName",1145,G__AMSR_ToFCluster_DeclFileName_8_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSR_ToFCluster_DeclFileLine_9_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSR_ToFCluster_ImplFileName_0_2,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSR_ToFCluster_ImplFileLine_1_2,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSR_ToFCluster_Class_Version_2_2,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSR_ToFCluster_Class_3_2,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSR_ToFCluster_Dictionary_4_2,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSR_ToFCluster_IsA_5_2,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSR_ToFCluster_ShowMembers_6_2,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSR_ToFCluster_Streamer_7_2,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSR_ToFCluster",1405,G__AMSR_ToFCluster_AMSR_ToFCluster_8_2,(int)('i'),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ToFCluster),-1,0,1,1,1,0,"u 'AMSR_ToFCluster' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSR_ToFCluster",1531,G__AMSR_ToFCluster_wAAMSR_ToFCluster_9_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSR_AntiCluster(void) {
   /* AMSR_AntiCluster */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_AntiCluster));
   G__memfunc_setup("AMSR_AntiCluster",1536,G__AMSR_AntiCluster_AMSR_AntiCluster_0_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_AntiCluster),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSR_AntiCluster",1536,G__AMSR_AntiCluster_AMSR_AntiCluster_1_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_AntiCluster),-1,0,7,1,1,0,
"i - 'Int_t' 0 - status i - 'Int_t' 0 - sector "
"f - 'Float_t' 0 - signal F - 'Float_t' 0 - coo "
"F - 'Float_t' 0 - ercoo i - 'Int_t' 0 - ntracks "
"U 'TObjArray' - 0 - tracks",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetObjectInfo",1283,G__AMSR_AntiCluster_GetObjectInfo_3_0,67,-1,-1,0,2,1,1,0,
"i - 'Int_t' 0 - px i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Paint",508,G__AMSR_AntiCluster_Paint_4_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Clear",487,G__AMSR_AntiCluster_Clear_5_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("GetStatus",932,G__AMSR_AntiCluster_GetStatus_6_0,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetSector",912,G__AMSR_AntiCluster_GetSector_7_0,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetSignal",894,G__AMSR_AntiCluster_GetSignal_8_0,102,-1,G__defined_typename("Float_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetTracks",904,G__AMSR_AntiCluster_GetTracks_9_0,85,G__get_linked_tagnum(&G__AMSR_CintLN_TObjArray),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Is3D",307,G__AMSR_AntiCluster_Is3D_0_1,98,-1,G__defined_typename("Bool_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DeclFileName",1145,G__AMSR_AntiCluster_DeclFileName_1_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSR_AntiCluster_DeclFileLine_2_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSR_AntiCluster_ImplFileName_3_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSR_AntiCluster_ImplFileLine_4_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSR_AntiCluster_Class_Version_5_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSR_AntiCluster_Class_6_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSR_AntiCluster_Dictionary_7_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSR_AntiCluster_IsA_8_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSR_AntiCluster_ShowMembers_9_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSR_AntiCluster_Streamer_0_2,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSR_AntiCluster",1536,G__AMSR_AntiCluster_AMSR_AntiCluster_1_2,(int)('i'),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_AntiCluster),-1,0,1,1,1,0,"u 'AMSR_AntiCluster' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSR_AntiCluster",1662,G__AMSR_AntiCluster_wAAMSR_AntiCluster_2_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSR_TrMCCluster(void) {
   /* AMSR_TrMCCluster */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrMCCluster));
   G__memfunc_setup("AMSR_TrMCCluster",1482,G__AMSR_TrMCCluster_AMSR_TrMCCluster_0_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrMCCluster),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSR_TrMCCluster",1482,G__AMSR_TrMCCluster_AMSR_TrMCCluster_1_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrMCCluster),-1,0,6,1,1,0,
"i - 'Int_t' 0 - part f - 'Float_t' 0 - signal "
"F - 'Float_t' 0 - coo F - 'Float_t' 0 - ercoo "
"i - 'Int_t' 0 - ntracks U 'TObjArray' - 0 - tracks",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetObjectInfo",1283,G__AMSR_TrMCCluster_GetObjectInfo_3_0,67,-1,-1,0,2,1,1,0,
"i - 'Int_t' 0 - px i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Paint",508,G__AMSR_TrMCCluster_Paint_4_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Clear",487,G__AMSR_TrMCCluster_Clear_5_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("GetStatus",932,G__AMSR_TrMCCluster_GetStatus_6_0,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetSignal",894,G__AMSR_TrMCCluster_GetSignal_7_0,102,-1,G__defined_typename("Float_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetTracks",904,G__AMSR_TrMCCluster_GetTracks_8_0,85,G__get_linked_tagnum(&G__AMSR_CintLN_TObjArray),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Is3D",307,G__AMSR_TrMCCluster_Is3D_9_0,98,-1,G__defined_typename("Bool_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DeclFileName",1145,G__AMSR_TrMCCluster_DeclFileName_0_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSR_TrMCCluster_DeclFileLine_1_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSR_TrMCCluster_ImplFileName_2_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSR_TrMCCluster_ImplFileLine_3_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSR_TrMCCluster_Class_Version_4_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSR_TrMCCluster_Class_5_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSR_TrMCCluster_Dictionary_6_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSR_TrMCCluster_IsA_7_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSR_TrMCCluster_ShowMembers_8_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSR_TrMCCluster_Streamer_9_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSR_TrMCCluster",1482,G__AMSR_TrMCCluster_AMSR_TrMCCluster_0_2,(int)('i'),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrMCCluster),-1,0,1,1,1,0,"u 'AMSR_TrMCCluster' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSR_TrMCCluster",1608,G__AMSR_TrMCCluster_wAAMSR_TrMCCluster_1_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSR_Track(void) {
   /* AMSR_Track */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Track));
   G__memfunc_setup("AMSR_Track",903,G__AMSR_Track_AMSR_Track_0_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Track),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSR_Track",903,G__AMSR_Track_AMSR_Track_1_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Track),-1,0,2,1,1,0,
"i - 'Int_t' 0 - status i - 'Int_t' 0 - pattern",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetObjectInfo",1283,G__AMSR_Track_GetObjectInfo_3_0,67,-1,-1,0,2,1,1,0,
"i - 'Int_t' 0 - px i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SetHelix",806,G__AMSR_Track_SetHelix_4_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Paint",508,G__AMSR_Track_Paint_5_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DeclFileName",1145,G__AMSR_Track_DeclFileName_6_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSR_Track_DeclFileLine_7_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSR_Track_ImplFileName_8_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSR_Track_ImplFileLine_9_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSR_Track_Class_Version_0_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSR_Track_Class_1_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSR_Track_Dictionary_2_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSR_Track_IsA_3_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSR_Track_ShowMembers_4_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSR_Track_Streamer_5_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSR_Track",903,G__AMSR_Track_AMSR_Track_6_1,(int)('i'),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Track),-1,0,1,1,1,0,"u 'AMSR_Track' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSR_Track",1029,G__AMSR_Track_wAAMSR_Track_7_1,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSR_SiHit(void) {
   /* AMSR_SiHit */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_SiHit));
   G__memfunc_setup("AMSR_SiHit",883,G__AMSR_SiHit_AMSR_SiHit_0_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_SiHit),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSR_SiHit",883,G__AMSR_SiHit_AMSR_SiHit_1_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_SiHit),-1,0,9,1,1,0,
"i - 'Int_t' 0 - status i - 'Int_t' 0 - plane "
"i - 'Int_t' 0 - px i - 'Int_t' 0 - py "
"F - 'Float_t' 0 - hit F - 'Float_t' 0 - errhit "
"f - 'Float_t' 0 - ampl f - 'Float_t' 0 - asym "
"U 'TObjArray' - 0 - tracks",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetObjectInfo",1283,G__AMSR_SiHit_GetObjectInfo_3_0,67,-1,-1,0,2,1,1,0,
"i - 'Int_t' 0 - px i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Paint",508,G__AMSR_SiHit_Paint_4_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Clear",487,G__AMSR_SiHit_Clear_5_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("GetStatus",932,G__AMSR_SiHit_GetStatus_6_0,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetPlane",784,G__AMSR_SiHit_GetPlane_7_0,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetX",376,G__AMSR_SiHit_GetX_8_0,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetY",377,G__AMSR_SiHit_GetY_9_0,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetAmplitude",1221,G__AMSR_SiHit_GetAmplitude_0_1,102,-1,G__defined_typename("Float_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetAsymAmpl",1092,G__AMSR_SiHit_GetAsymAmpl_1_1,102,-1,G__defined_typename("Float_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetTracks",904,G__AMSR_SiHit_GetTracks_2_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TObjArray),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Is3D",307,G__AMSR_SiHit_Is3D_3_1,98,-1,G__defined_typename("Bool_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DeclFileName",1145,G__AMSR_SiHit_DeclFileName_4_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSR_SiHit_DeclFileLine_5_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSR_SiHit_ImplFileName_6_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSR_SiHit_ImplFileLine_7_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSR_SiHit_Class_Version_8_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSR_SiHit_Class_9_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSR_SiHit_Dictionary_0_2,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSR_SiHit_IsA_1_2,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSR_SiHit_ShowMembers_2_2,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSR_SiHit_Streamer_3_2,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSR_SiHit",883,G__AMSR_SiHit_AMSR_SiHit_4_2,(int)('i'),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_SiHit),-1,0,1,1,1,0,"u 'AMSR_SiHit' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSR_SiHit",1009,G__AMSR_SiHit_wAAMSR_SiHit_5_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSR_CTCCluster(void) {
   /* AMSR_CTCCluster */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_CTCCluster));
   G__memfunc_setup("AMSR_CTCCluster",1358,G__AMSR_CTCCluster_AMSR_CTCCluster_0_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_CTCCluster),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSR_CTCCluster",1358,G__AMSR_CTCCluster_AMSR_CTCCluster_1_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_CTCCluster),-1,0,9,1,1,0,
"i - 'Int_t' 0 - status i - 'Int_t' 0 - plane "
"i - 'Int_t' 0 - rawsignal f - 'Float_t' 0 - signal "
"f - 'Float_t' 0 - errsignal F - 'Float_t' 0 - coo "
"F - 'Float_t' 0 - ercoo i - 'Int_t' 0 - ntracks "
"U 'TObjArray' - 0 - tracks",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetObjectInfo",1283,G__AMSR_CTCCluster_GetObjectInfo_3_0,67,-1,-1,0,2,1,1,0,
"i - 'Int_t' 0 - px i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Paint",508,G__AMSR_CTCCluster_Paint_4_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Clear",487,G__AMSR_CTCCluster_Clear_5_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("GetStatus",932,G__AMSR_CTCCluster_GetStatus_6_0,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetPlane",784,G__AMSR_CTCCluster_GetPlane_7_0,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetRawSignal",1192,G__AMSR_CTCCluster_GetRawSignal_8_0,102,-1,G__defined_typename("Float_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetSignal",894,G__AMSR_CTCCluster_GetSignal_9_0,102,-1,G__defined_typename("Float_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetErrSignal",1191,G__AMSR_CTCCluster_GetErrSignal_0_1,102,-1,G__defined_typename("Float_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetTracks",904,G__AMSR_CTCCluster_GetTracks_1_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TObjArray),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Is3D",307,G__AMSR_CTCCluster_Is3D_2_1,98,-1,G__defined_typename("Bool_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DeclFileName",1145,G__AMSR_CTCCluster_DeclFileName_3_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSR_CTCCluster_DeclFileLine_4_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSR_CTCCluster_ImplFileName_5_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSR_CTCCluster_ImplFileLine_6_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSR_CTCCluster_Class_Version_7_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSR_CTCCluster_Class_8_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSR_CTCCluster_Dictionary_9_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSR_CTCCluster_IsA_0_2,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSR_CTCCluster_ShowMembers_1_2,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSR_CTCCluster_Streamer_2_2,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSR_CTCCluster",1358,G__AMSR_CTCCluster_AMSR_CTCCluster_3_2,(int)('i'),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_CTCCluster),-1,0,1,1,1,0,"u 'AMSR_CTCCluster' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSR_CTCCluster",1484,G__AMSR_CTCCluster_wAAMSR_CTCCluster_4_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSR_Particle(void) {
   /* AMSR_Particle */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Particle));
   G__memfunc_setup("AMSR_Particle",1222,G__AMSR_Particle_AMSR_Particle_0_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Particle),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetObjectInfo",1283,G__AMSR_Particle_GetObjectInfo_2_0,67,-1,-1,0,2,1,1,0,
"i - 'Int_t' 0 - px i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SetHelix",806,G__AMSR_Particle_SetHelix_3_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Paint",508,G__AMSR_Particle_Paint_4_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DeclFileName",1145,G__AMSR_Particle_DeclFileName_5_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSR_Particle_DeclFileLine_6_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSR_Particle_ImplFileName_7_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSR_Particle_ImplFileLine_8_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSR_Particle_Class_Version_9_0,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSR_Particle_Class_0_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSR_Particle_Dictionary_1_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSR_Particle_IsA_2_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSR_Particle_ShowMembers_3_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSR_Particle_Streamer_4_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSR_Particle",1222,G__AMSR_Particle_AMSR_Particle_5_1,(int)('i'),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Particle),-1,0,1,1,1,0,"u 'AMSR_Particle' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSR_Particle",1348,G__AMSR_Particle_wAAMSR_Particle_6_1,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSR_MCParticle(void) {
   /* AMSR_MCParticle */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_MCParticle));
   G__memfunc_setup("AMSR_MCParticle",1366,G__AMSR_MCParticle_AMSR_MCParticle_0_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_MCParticle),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetObjectInfo",1283,G__AMSR_MCParticle_GetObjectInfo_2_0,67,-1,-1,0,2,1,1,0,
"i - 'Int_t' 0 - px i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SetHelix",806,G__AMSR_MCParticle_SetHelix_3_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Paint",508,G__AMSR_MCParticle_Paint_4_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DeclFileName",1145,G__AMSR_MCParticle_DeclFileName_5_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSR_MCParticle_DeclFileLine_6_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSR_MCParticle_ImplFileName_7_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSR_MCParticle_ImplFileLine_8_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSR_MCParticle_Class_Version_9_0,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSR_MCParticle_Class_0_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSR_MCParticle_Dictionary_1_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSR_MCParticle_IsA_2_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSR_MCParticle_ShowMembers_3_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSR_MCParticle_Streamer_4_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSR_MCParticle",1366,G__AMSR_MCParticle_AMSR_MCParticle_5_1,(int)('i'),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_MCParticle),-1,0,1,1,1,0,"u 'AMSR_MCParticle' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSR_MCParticle",1492,G__AMSR_MCParticle_wAAMSR_MCParticle_6_1,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSR_Canvas(void) {
   /* AMSR_Canvas */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Canvas));
   G__memfunc_setup("DrawEventStatus",1556,(G__InterfaceMethod)NULL,121,-1,-1,0,4,1,4,0,
"i - 'Int_t' 0 - event i - 'Int_t' 0 - px "
"i - 'Int_t' 0 - py U 'TObject' - 0 - selected",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AddParticleInfo",1481,G__AMSR_Canvas_AddParticleInfo_1_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSR_Canvas",1006,G__AMSR_Canvas_AMSR_Canvas_2_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Canvas),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSR_Canvas",1006,G__AMSR_Canvas_AMSR_Canvas_3_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Canvas),-1,0,4,1,1,0,
"C - 'Text_t' 0 - name C - 'Text_t' 0 \"The AMS Display with Root\" title "
"i - 'Int_t' 0 700 ww i - 'Int_t' 0 550 wh",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Update",611,G__AMSR_Canvas_Update_5_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("EditorBar",892,G__AMSR_Canvas_EditorBar_6_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("HandleInput",1116,G__AMSR_Canvas_HandleInput_7_0,121,-1,-1,0,3,1,1,0,
"i - 'Int_t' 0 - event i - 'Int_t' 0 - px "
"i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("OpenFileCB",919,G__AMSR_Canvas_OpenFileCB_8_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("SaveParticleCB",1352,G__AMSR_Canvas_SaveParticleCB_9_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("SaveParticleGIF",1433,G__AMSR_Canvas_SaveParticleGIF_0_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("PrintCB",658,G__AMSR_Canvas_PrintCB_1_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileName",1145,G__AMSR_Canvas_DeclFileName_2_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSR_Canvas_DeclFileLine_3_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSR_Canvas_ImplFileName_4_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSR_Canvas_ImplFileLine_5_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSR_Canvas_Class_Version_6_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSR_Canvas_Class_7_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSR_Canvas_Dictionary_8_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSR_Canvas_IsA_9_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSR_Canvas_ShowMembers_0_2,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSR_Canvas_Streamer_1_2,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic destructor
   G__memfunc_setup("~AMSR_Canvas",1132,G__AMSR_Canvas_wAAMSR_Canvas_2_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncDebugger(void) {
   /* Debugger */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSR_CintLN_Debugger));
   G__memfunc_setup("Debugger",805,G__Debugger_Debugger_0_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_Debugger),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("On",189,G__Debugger_On_2_0,121,-1,-1,0,0,1,1,0,"","Turn on the debugger",(void*)NULL,0);
   G__memfunc_setup("Off",283,G__Debugger_Off_3_0,121,-1,-1,0,0,1,1,0,"","Turn off the debugger",(void*)NULL,0);
   G__memfunc_setup("Print",525,G__Debugger_Print_4_0,121,-1,-1,0,2,2,1,0,
"C - - 0 - fmt i - - 0 - -","Print a debug message",(void*)NULL,0);
   G__memfunc_setup("DeclFileName",1145,G__Debugger_DeclFileName_5_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__Debugger_DeclFileLine_6_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__Debugger_ImplFileName_7_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__Debugger_ImplFileLine_8_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__Debugger_Class_Version_9_0,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__Debugger_Class_0_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__Debugger_Dictionary_1_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__Debugger_IsA_2_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__Debugger_ShowMembers_3_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__Debugger_Streamer_4_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("Debugger",805,G__Debugger_Debugger_5_1,(int)('i'),G__get_linked_tagnum(&G__AMSR_CintLN_Debugger),-1,0,1,1,1,0,"u 'Debugger' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~Debugger",931,G__Debugger_wADebugger_6_1,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSR_GeometrySetter(void) {
   /* AMSR_GeometrySetter */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_GeometrySetter));
   G__memfunc_setup("AMSR_GeometrySetter",1877,G__AMSR_GeometrySetter_AMSR_GeometrySetter_0_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_GeometrySetter),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSR_GeometrySetter",1877,G__AMSR_GeometrySetter_AMSR_GeometrySetter_1_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_GeometrySetter),-1,0,1,1,1,0,"U 'TGeometry' - 0 - geo",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("TurnOn",614,G__AMSR_GeometrySetter_TurnOn_3_0,121,-1,-1,0,1,1,1,0,"C - - 0 - name",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("TurnOnWithSons",1445,G__AMSR_GeometrySetter_TurnOnWithSons_4_0,121,-1,-1,0,1,1,1,0,"C - - 0 - name",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("TurnOff",708,G__AMSR_GeometrySetter_TurnOff_5_0,121,-1,-1,0,1,1,1,0,"C - - 0 - name",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("TurnOffWithSons",1539,G__AMSR_GeometrySetter_TurnOffWithSons_6_0,121,-1,-1,0,1,1,1,0,"C - - 0 - name",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("CheckVisibility",1542,G__AMSR_GeometrySetter_CheckVisibility_7_0,121,-1,-1,0,1,1,1,0,"C - - 0 - name",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("SetGeo",583,G__AMSR_GeometrySetter_SetGeo_8_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileName",1145,G__AMSR_GeometrySetter_DeclFileName_9_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSR_GeometrySetter_DeclFileLine_0_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSR_GeometrySetter_ImplFileName_1_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSR_GeometrySetter_ImplFileLine_2_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSR_GeometrySetter_Class_Version_3_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSR_GeometrySetter_Class_4_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSR_GeometrySetter_Dictionary_5_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSR_GeometrySetter_IsA_6_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSR_GeometrySetter_ShowMembers_7_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSR_GeometrySetter_Streamer_8_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSR_GeometrySetter",1877,G__AMSR_GeometrySetter_AMSR_GeometrySetter_9_1,(int)('i'),G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_GeometrySetter),-1,0,1,1,1,0,"u 'AMSR_GeometrySetter' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSR_GeometrySetter",2003,G__AMSR_GeometrySetter_wAAMSR_GeometrySetter_0_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncTGRunEventDialog(void) {
   /* TGRunEventDialog */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TGRunEventDialog));
   G__memfunc_setup("TGRunEventDialog",1570,G__TGRunEventDialog_TGRunEventDialog_0_0,105,G__get_linked_tagnum(&G__AMSR_CintLN_TGRunEventDialog),-1,0,4,1,1,0,
"U 'TGWindow' - 0 - p U 'TGWindow' - 0 - main "
"I - 'Int_t' 0 - pRun I - 'Int_t' 0 - pEvent",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ProcessMessage",1444,G__TGRunEventDialog_ProcessMessage_2_0,98,-1,G__defined_typename("Bool_t"),0,3,1,1,0,
"l - 'Long_t' 0 - msg l - 'Long_t' 0 - parm1 "
"l - 'Long_t' 0 - parm2",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("CloseWindow",1134,G__TGRunEventDialog_CloseWindow_3_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DeclFileName",1145,G__TGRunEventDialog_DeclFileName_4_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__TGRunEventDialog_DeclFileLine_5_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__TGRunEventDialog_ImplFileName_6_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__TGRunEventDialog_ImplFileLine_7_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__TGRunEventDialog_Class_Version_8_0,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__TGRunEventDialog_Class_9_0,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__TGRunEventDialog_Dictionary_0_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__TGRunEventDialog_IsA_1_1,85,G__get_linked_tagnum(&G__AMSR_CintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__TGRunEventDialog_ShowMembers_2_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__TGRunEventDialog_Streamer_3_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("TGRunEventDialog",1570,G__TGRunEventDialog_TGRunEventDialog_4_1,(int)('i'),G__get_linked_tagnum(&G__AMSR_CintLN_TGRunEventDialog),-1,0,1,1,1,0,"u 'TGRunEventDialog' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~TGRunEventDialog",1696,G__TGRunEventDialog_wATGRunEventDialog_5_1,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}


/*********************************************************
* Member function information setup
*********************************************************/
extern "C" void G__cpp_setup_memfuncAMSR_Cint() {
}

/*********************************************************
* Global variable information setup for each class
*********************************************************/
extern "C" void G__cpp_setup_globalAMSR_Cint() {

   /* Setting up global variables */
   G__resetplocal();

   G__memvar_setup((void*)(&gAMSR_Root),85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Root),-1,-1,1,"gAMSR_Root=",0,(char*)NULL);
   G__memvar_setup((void*)(&debugger),117,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_Debugger),-1,-1,1,"debugger=",0,(char*)NULL);
   G__memvar_setup((void*)(&gAMSR_Display),85,0,0,G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Display),-1,-1,1,"gAMSR_Display=",0,(char*)NULL);

   G__resetglobalenv();
}

/*********************************************************
* Global function information setup for each class
*********************************************************/
extern "C" void G__cpp_setup_funcAMSR_Cint() {
   G__lastifuncposition();


   G__resetifuncposition();
}

/*********************************************************
* Class,struct,union,enum tag information setup
*********************************************************/
/* Setup class/struct taginfo */
G__linked_taginfo G__AMSR_CintLN_TClass = { "TClass" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TList = { "TList" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TObjArray = { "TObjArray" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TObject = { "TObject" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TString = { "TString" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TNamed = { "TNamed" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TVirtualPad = { "TVirtualPad" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TGeometry = { "TGeometry" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TAttLine = { "TAttLine" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TAttFill = { "TAttFill" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TTree = { "TTree" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TH1F = { "TH1F" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TAttText = { "TAttText" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TAttPad = { "TAttPad" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TBox = { "TBox" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TWbox = { "TWbox" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TCanvas = { "TCanvas" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TPad = { "TPad" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TButton = { "TButton" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TText = { "TText" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TRadioButton = { "TRadioButton" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TSwitch = { "TSwitch" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN__x3d_data_ = { "_x3d_data_" , 115 , -1 };
G__linked_taginfo G__AMSR_CintLN__x3d_sizeof_ = { "_x3d_sizeof_" , 115 , -1 };
G__linked_taginfo G__AMSR_CintLN_TPolyLine3D = { "TPolyLine3D" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_AMSR_Axis = { "AMSR_Axis" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TMarker3DBox = { "TMarker3DBox" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_AMSR_Display = { "AMSR_Display" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_AMSR_3DCluster = { "AMSR_3DCluster" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_AMSR_Maker = { "AMSR_Maker" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_AMSR_HistBrowser = { "AMSR_HistBrowser" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_EDataFileType = { "EDataFileType" , 101 , -1 };
G__linked_taginfo G__AMSR_CintLN_AMSR_ToFClusterReader = { "AMSR_ToFClusterReader" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_AMSR_TrackReader = { "AMSR_TrackReader" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_AMSR_SiHitReader = { "AMSR_SiHitReader" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_AMSR_CTCClusterReader = { "AMSR_CTCClusterReader" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_AMSR_AntiClusterReader = { "AMSR_AntiClusterReader" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_AMSR_TrMCClusterReader = { "AMSR_TrMCClusterReader" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_AMSR_ParticleReader = { "AMSR_ParticleReader" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_AMSR_MCParticleReader = { "AMSR_MCParticleReader" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_AMSR_Ntuple = { "AMSR_Ntuple" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_AMSR_VirtualDisplay = { "AMSR_VirtualDisplay" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_AMSR_Root = { "AMSR_Root" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_AMSR_ToFCluster = { "AMSR_ToFCluster" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_AMSR_AntiCluster = { "AMSR_AntiCluster" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_AMSR_TrMCCluster = { "AMSR_TrMCCluster" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_THelix = { "THelix" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_AMSR_Track = { "AMSR_Track" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_AMSR_SiHit = { "AMSR_SiHit" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_AMSR_CTCCluster = { "AMSR_CTCCluster" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_AMSR_Particle = { "AMSR_Particle" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_AMSR_MCParticle = { "AMSR_MCParticle" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_AMSR_Canvas = { "AMSR_Canvas" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_EAMSR_View = { "EAMSR_View" , 101 , -1 };
G__linked_taginfo G__AMSR_CintLN_Debugger = { "Debugger" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TGObject = { "TGObject" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TGWindow = { "TGWindow" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TGFrame = { "TGFrame" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TGCompositeFrame = { "TGCompositeFrame" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TGLayoutHints = { "TGLayoutHints" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TGVerticalFrame = { "TGVerticalFrame" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TGHorizontalFrame = { "TGHorizontalFrame" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TGMainFrame = { "TGMainFrame" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TGTransientFrame = { "TGTransientFrame" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TRootCanvas = { "TRootCanvas" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TGLabel = { "TGLabel" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TGTextBuffer = { "TGTextBuffer" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TGTextEntry = { "TGTextEntry" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TGTextButton = { "TGTextButton" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TGFileInfo = { "TGFileInfo" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_AMSR_GeoToggle = { "AMSR_GeoToggle" , 115 , -1 };
G__linked_taginfo G__AMSR_CintLN_AMSR_GeometrySetter = { "AMSR_GeometrySetter" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TGTextFrame = { "TGTextFrame" , 99 , -1 };
G__linked_taginfo G__AMSR_CintLN_TGRunEventDialog = { "TGRunEventDialog" , 99 , -1 };

extern "C" void G__cpp_setup_tagtableAMSR_Cint() {

   /* Setting up class,struct,union tag entry */
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TRadioButton),sizeof(TRadioButton),-1,0,"Displayable toggle class",G__setup_memvarTRadioButton,G__setup_memfuncTRadioButton);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TSwitch),sizeof(TSwitch),-1,0,"Displayable Switch class",G__setup_memvarTSwitch,G__setup_memfuncTSwitch);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Axis),sizeof(AMSR_Axis),-1,0,"AMS axis class",G__setup_memvarAMSR_Axis,G__setup_memfuncAMSR_Axis);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Display),sizeof(AMSR_Display),-1,0,"Utility class to display AMSR_Root outline, tracks,...",G__setup_memvarAMSR_Display,G__setup_memfuncAMSR_Display);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_3DCluster),sizeof(AMSR_3DCluster),-1,0,"Cluster class",G__setup_memvarAMSR_3DCluster,G__setup_memfuncAMSR_3DCluster);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Maker),sizeof(AMSR_Maker),-1,2,"AMS virtual base class for Makers",G__setup_memvarAMSR_Maker,G__setup_memfuncAMSR_Maker);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_HistBrowser),sizeof(AMSR_HistBrowser),-1,0,"helper class to browse AMSR_Root Makers histograms",G__setup_memvarAMSR_HistBrowser,G__setup_memfuncAMSR_HistBrowser);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ToFClusterReader),sizeof(AMSR_ToFClusterReader),-1,0,"AMS Time of Flight Cluster Maker",G__setup_memvarAMSR_ToFClusterReader,G__setup_memfuncAMSR_ToFClusterReader);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrackReader),sizeof(AMSR_TrackReader),-1,0,"AMSR_Root TrackReader",G__setup_memvarAMSR_TrackReader,G__setup_memfuncAMSR_TrackReader);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_SiHitReader),sizeof(AMSR_SiHitReader),-1,0,"AMS Time of Flight Cluster Maker",G__setup_memvarAMSR_SiHitReader,G__setup_memfuncAMSR_SiHitReader);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_CTCClusterReader),sizeof(AMSR_CTCClusterReader),-1,0,"AMS Time of Flight Cluster Maker",G__setup_memvarAMSR_CTCClusterReader,G__setup_memfuncAMSR_CTCClusterReader);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_AntiClusterReader),sizeof(AMSR_AntiClusterReader),-1,0,"AMS Time of Flight Cluster Maker",G__setup_memvarAMSR_AntiClusterReader,G__setup_memfuncAMSR_AntiClusterReader);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrMCClusterReader),sizeof(AMSR_TrMCClusterReader),-1,0,"AMS Time of Flight Cluster Maker",G__setup_memvarAMSR_TrMCClusterReader,G__setup_memfuncAMSR_TrMCClusterReader);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ParticleReader),sizeof(AMSR_ParticleReader),-1,0,"AMSR_Root Particle Reader",G__setup_memvarAMSR_ParticleReader,G__setup_memfuncAMSR_ParticleReader);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_MCParticleReader),sizeof(AMSR_MCParticleReader),-1,0,"AMSR_Root Particle Reader",G__setup_memvarAMSR_MCParticleReader,G__setup_memfuncAMSR_MCParticleReader);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Ntuple),sizeof(AMSR_Ntuple),-1,0,"AMSR_Ntuple ntuple handling class",G__setup_memvarAMSR_Ntuple,G__setup_memfuncAMSR_Ntuple);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_VirtualDisplay),sizeof(AMSR_VirtualDisplay),-1,25,"Virtual base class for ATLFast event display",G__setup_memvarAMSR_VirtualDisplay,G__setup_memfuncAMSR_VirtualDisplay);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Root),sizeof(AMSR_Root),-1,0,"AMSR_Root control class",G__setup_memvarAMSR_Root,G__setup_memfuncAMSR_Root);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_ToFCluster),sizeof(AMSR_ToFCluster),-1,0,"AMS Time of Flight Cluster",G__setup_memvarAMSR_ToFCluster,G__setup_memfuncAMSR_ToFCluster);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_AntiCluster),sizeof(AMSR_AntiCluster),-1,0,"AMS Time of Flight Cluster",G__setup_memvarAMSR_AntiCluster,G__setup_memfuncAMSR_AntiCluster);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_TrMCCluster),sizeof(AMSR_TrMCCluster),-1,0,"AMS Time of Flight Cluster",G__setup_memvarAMSR_TrMCCluster,G__setup_memfuncAMSR_TrMCCluster);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Track),sizeof(AMSR_Track),-1,0,"AMSR_Root track class",G__setup_memvarAMSR_Track,G__setup_memfuncAMSR_Track);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_SiHit),sizeof(AMSR_SiHit),-1,0,"AMS Time of Flight Cluster",G__setup_memvarAMSR_SiHit,G__setup_memfuncAMSR_SiHit);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_CTCCluster),sizeof(AMSR_CTCCluster),-1,0,"AMS Time of Flight Cluster",G__setup_memvarAMSR_CTCCluster,G__setup_memfuncAMSR_CTCCluster);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Particle),sizeof(AMSR_Particle),-1,0,"AMSR_Root track class",G__setup_memvarAMSR_Particle,G__setup_memfuncAMSR_Particle);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_MCParticle),sizeof(AMSR_MCParticle),-1,0,"AMSR_Root track class",G__setup_memvarAMSR_MCParticle,G__setup_memfuncAMSR_MCParticle);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_Canvas),sizeof(AMSR_Canvas),-1,0,"AMS canvas control class",G__setup_memvarAMSR_Canvas,G__setup_memfuncAMSR_Canvas);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSR_CintLN_Debugger),sizeof(Debugger),-1,0,"Debug message printing class",G__setup_memvarDebugger,G__setup_memfuncDebugger);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSR_CintLN_AMSR_GeometrySetter),sizeof(AMSR_GeometrySetter),-1,0,"Utility class to display AMS geometry",G__setup_memvarAMSR_GeometrySetter,G__setup_memfuncAMSR_GeometrySetter);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSR_CintLN_TGRunEventDialog),sizeof(TGRunEventDialog),-1,0,"Event-changing dialog",G__setup_memvarTGRunEventDialog,G__setup_memfuncTGRunEventDialog);
}
extern "C" void G__cpp_setupAMSR_Cint() {
  G__check_setup_version(51111,"G__cpp_setupAMSR_Cint()");
  G__set_cpp_environmentAMSR_Cint();
  G__cpp_setup_tagtableAMSR_Cint();

  G__cpp_setup_inheritanceAMSR_Cint();

  G__cpp_setup_typetableAMSR_Cint();

  G__cpp_setup_memvarAMSR_Cint();

  G__cpp_setup_memfuncAMSR_Cint();
  G__cpp_setup_globalAMSR_Cint();
  G__cpp_setup_funcAMSR_Cint();

   if(0==G__getsizep2memfunc()) G__get_sizep2memfuncAMSR_Cint();
  return;
}
class G__cpp_setup_initAMSR_Cint {
  public:
    G__cpp_setup_initAMSR_Cint() { G__add_setup_func("AMSR_Cint",&G__cpp_setupAMSR_Cint); }
   ~G__cpp_setup_initAMSR_Cint() { G__remove_setup_func("AMSR_Cint"); }
};
G__cpp_setup_initAMSR_Cint G__cpp_setup_initializerAMSR_Cint;

//
// File generated by rootcint at Tue Oct  6 18:08:59 1998.
// Do NOT change. Changes will be lost next time file is generated
//

#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TError.h"

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, TRadioButton *&obj)
{
   // Read a pointer to an object of class TRadioButton.

   obj = (TRadioButton *) buf.ReadObject(TRadioButton::Class());
   return buf;
}

//______________________________________________________________________________
void TRadioButton::Streamer(TBuffer &R__b)
{
   // Stream an object of class TRadioButton.

   TButton::Streamer(R__b);
}

//______________________________________________________________________________
void TRadioButton::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class TRadioButton.

   TClass *R__cl  = TRadioButton::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   R__insp.Inspect(R__cl, R__parent, "*fState", &fState);
   R__insp.Inspect(R__cl, R__parent, "*fOffMethod", &fOffMethod);
   TButton::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, TSwitch *&obj)
{
   // Read a pointer to an object of class TSwitch.

   obj = (TSwitch *) buf.ReadObject(TSwitch::Class());
   return buf;
}

//______________________________________________________________________________
void TSwitch::Streamer(TBuffer &R__b)
{
   // Stream an object of class TSwitch.

   TPad::Streamer(R__b);
}

//______________________________________________________________________________
void TSwitch::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class TSwitch.

   TClass *R__cl  = TSwitch::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   R__insp.Inspect(R__cl, R__parent, "*fButton", &fButton);
   R__insp.Inspect(R__cl, R__parent, "*fText", &fText);
   TPad::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSR_Axis *&obj)
{
   // Read a pointer to an object of class AMSR_Axis.

   obj = (AMSR_Axis *) buf.ReadObject(AMSR_Axis::Class());
   return buf;
}

//______________________________________________________________________________
void AMSR_Axis::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSR_Axis.

   TPolyLine3D::Streamer(R__b);
}

//______________________________________________________________________________
void AMSR_Axis::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSR_Axis.

   TClass *R__cl  = AMSR_Axis::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   R__insp.Inspect(R__cl, R__parent, "*m_Title", &m_Title);
   TPolyLine3D::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSR_Display *&obj)
{
   // Read a pointer to an object of class AMSR_Display.

   obj = (AMSR_Display *) buf.ReadObject(AMSR_Display::Class());
   return buf;
}

//______________________________________________________________________________
void AMSR_Display::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSR_Display.

   AMSR_VirtualDisplay::Streamer(R__b);
}

//______________________________________________________________________________
void AMSR_Display::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSR_Display.

   TClass *R__cl  = AMSR_Display::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   R__insp.Inspect(R__cl, R__parent, "m_View", &m_View);
   R__insp.Inspect(R__cl, R__parent, "m_IdleTime", &m_IdleTime);
   R__insp.Inspect(R__cl, R__parent, "*m_IdleCommand", &m_IdleCommand);
   R__insp.Inspect(R__cl, R__parent, "m_IdleOn", &m_IdleOn);
   R__insp.Inspect(R__cl, R__parent, "m_DrawParticles", &m_DrawParticles);
   R__insp.Inspect(R__cl, R__parent, "m_DrawGeometry", &m_DrawGeometry);
   R__insp.Inspect(R__cl, R__parent, "m_DrawMoreGeometry", &m_DrawMoreGeometry);
   R__insp.Inspect(R__cl, R__parent, "*m_Geometry", &m_Geometry);
   R__insp.Inspect(R__cl, R__parent, "*m_GeoSetter", &m_GeoSetter);
   R__insp.Inspect(R__cl, R__parent, "m_PTcut", &m_PTcut);
   R__insp.Inspect(R__cl, R__parent, "m_PTcutEGMUNU", &m_PTcutEGMUNU);
   R__insp.Inspect(R__cl, R__parent, "m_Theta", &m_Theta);
   R__insp.Inspect(R__cl, R__parent, "m_Phi", &m_Phi);
   R__insp.Inspect(R__cl, R__parent, "*m_Canvas", &m_Canvas);
   R__insp.Inspect(R__cl, R__parent, "*m_UCPad", &m_UCPad);
   R__insp.Inspect(R__cl, R__parent, "*m_LogoPad", &m_LogoPad);
   R__insp.Inspect(R__cl, R__parent, "*m_TrigPad", &m_TrigPad);
   R__insp.Inspect(R__cl, R__parent, "*m_ButtonPad", &m_ButtonPad);
   R__insp.Inspect(R__cl, R__parent, "*m_Pad", &m_Pad);
   R__insp.Inspect(R__cl, R__parent, "*m_AxisPad[4]", &m_AxisPad);
   R__insp.Inspect(R__cl, R__parent, "*m_TitlePad", &m_TitlePad);
   R__insp.Inspect(R__cl, R__parent, "*m_EventInfoPad", &m_EventInfoPad);
   R__insp.Inspect(R__cl, R__parent, "*m_ObjInfoPad", &m_ObjInfoPad);
   R__insp.Inspect(R__cl, R__parent, "*m_PartInfoPad", &m_PartInfoPad);
   R__insp.Inspect(R__cl, R__parent, "*m_IdleSwitch", &m_IdleSwitch);
   R__insp.Inspect(R__cl, R__parent, "*m_ToBeDrawn", &m_ToBeDrawn);
   AMSR_VirtualDisplay::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSR_3DCluster *&obj)
{
   // Read a pointer to an object of class AMSR_3DCluster.

   obj = (AMSR_3DCluster *) buf.ReadObject(AMSR_3DCluster::Class());
   return buf;
}

//______________________________________________________________________________
void AMSR_3DCluster::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSR_3DCluster.

   TMarker3DBox::Streamer(R__b);
}

//______________________________________________________________________________
void AMSR_3DCluster::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSR_3DCluster.

   TClass *R__cl  = AMSR_3DCluster::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   R__insp.Inspect(R__cl, R__parent, "fRange", &fRange);
   TMarker3DBox::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSR_Maker *&obj)
{
   // Read a pointer to an object of class AMSR_Maker.

   obj = (AMSR_Maker *) buf.ReadObject(AMSR_Maker::Class());
   return buf;
}

//______________________________________________________________________________
void AMSR_Maker::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSR_Maker.

   TClass *R__cl  = AMSR_Maker::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   R__insp.Inspect(R__cl, R__parent, "m_Save", &m_Save);
   R__insp.Inspect(R__cl, R__parent, "*m_Fruits", &m_Fruits);
   m_BranchName.ShowMembers(R__insp, strcat(R__parent,"m_BranchName.")); R__parent[R__ncp] = 0;
   R__insp.Inspect(R__cl, R__parent, "*m_Histograms", &m_Histograms);
   R__insp.Inspect(R__cl, R__parent, "DrawFruits", &DrawFruits);
   TNamed::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSR_HistBrowser *&obj)
{
   // Read a pointer to an object of class AMSR_HistBrowser.

   obj = (AMSR_HistBrowser *) buf.ReadObject(AMSR_HistBrowser::Class());
   return buf;
}

//______________________________________________________________________________
void AMSR_HistBrowser::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSR_HistBrowser.

   TNamed::Streamer(R__b);
}

//______________________________________________________________________________
void AMSR_HistBrowser::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSR_HistBrowser.

   TClass *R__cl  = AMSR_HistBrowser::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   TNamed::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSR_ToFClusterReader *&obj)
{
   // Read a pointer to an object of class AMSR_ToFClusterReader.

   obj = (AMSR_ToFClusterReader *) buf.ReadObject(AMSR_ToFClusterReader::Class());
   return buf;
}

//______________________________________________________________________________
void AMSR_ToFClusterReader::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSR_ToFClusterReader.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      AMSR_Maker::Streamer(R__b);
      R__b >> m_Nclusters;
   } else {
      R__b.WriteVersion(AMSR_ToFClusterReader::IsA());
      AMSR_Maker::Streamer(R__b);
      R__b << m_Nclusters;
   }
}

//______________________________________________________________________________
void AMSR_ToFClusterReader::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSR_ToFClusterReader.

   TClass *R__cl  = AMSR_ToFClusterReader::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   R__insp.Inspect(R__cl, R__parent, "m_Nclusters", &m_Nclusters);
   AMSR_Maker::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSR_TrackReader *&obj)
{
   // Read a pointer to an object of class AMSR_TrackReader.

   obj = (AMSR_TrackReader *) buf.ReadObject(AMSR_TrackReader::Class());
   return buf;
}

//______________________________________________________________________________
void AMSR_TrackReader::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSR_TrackReader.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      AMSR_Maker::Streamer(R__b);
      R__b >> m_NTracks;
      R__b >> m_Mult;
   } else {
      R__b.WriteVersion(AMSR_TrackReader::IsA());
      AMSR_Maker::Streamer(R__b);
      R__b << m_NTracks;
      R__b << (TObject*)m_Mult;
   }
}

//______________________________________________________________________________
void AMSR_TrackReader::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSR_TrackReader.

   TClass *R__cl  = AMSR_TrackReader::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   R__insp.Inspect(R__cl, R__parent, "m_NTracks", &m_NTracks);
   R__insp.Inspect(R__cl, R__parent, "*m_Mult", &m_Mult);
   AMSR_Maker::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSR_SiHitReader *&obj)
{
   // Read a pointer to an object of class AMSR_SiHitReader.

   obj = (AMSR_SiHitReader *) buf.ReadObject(AMSR_SiHitReader::Class());
   return buf;
}

//______________________________________________________________________________
void AMSR_SiHitReader::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSR_SiHitReader.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      AMSR_Maker::Streamer(R__b);
      R__b >> m_Nclusters;
      R__b >> DrawUsedHitsOnly;
   } else {
      R__b.WriteVersion(AMSR_SiHitReader::IsA());
      AMSR_Maker::Streamer(R__b);
      R__b << m_Nclusters;
      R__b << DrawUsedHitsOnly;
   }
}

//______________________________________________________________________________
void AMSR_SiHitReader::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSR_SiHitReader.

   TClass *R__cl  = AMSR_SiHitReader::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   R__insp.Inspect(R__cl, R__parent, "m_Nclusters", &m_Nclusters);
   R__insp.Inspect(R__cl, R__parent, "DrawUsedHitsOnly", &DrawUsedHitsOnly);
   AMSR_Maker::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSR_CTCClusterReader *&obj)
{
   // Read a pointer to an object of class AMSR_CTCClusterReader.

   obj = (AMSR_CTCClusterReader *) buf.ReadObject(AMSR_CTCClusterReader::Class());
   return buf;
}

//______________________________________________________________________________
void AMSR_CTCClusterReader::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSR_CTCClusterReader.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      AMSR_Maker::Streamer(R__b);
      R__b >> m_Nclusters;
   } else {
      R__b.WriteVersion(AMSR_CTCClusterReader::IsA());
      AMSR_Maker::Streamer(R__b);
      R__b << m_Nclusters;
   }
}

//______________________________________________________________________________
void AMSR_CTCClusterReader::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSR_CTCClusterReader.

   TClass *R__cl  = AMSR_CTCClusterReader::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   R__insp.Inspect(R__cl, R__parent, "m_Nclusters", &m_Nclusters);
   AMSR_Maker::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSR_AntiClusterReader *&obj)
{
   // Read a pointer to an object of class AMSR_AntiClusterReader.

   obj = (AMSR_AntiClusterReader *) buf.ReadObject(AMSR_AntiClusterReader::Class());
   return buf;
}

//______________________________________________________________________________
void AMSR_AntiClusterReader::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSR_AntiClusterReader.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      AMSR_Maker::Streamer(R__b);
      R__b >> m_Nclusters;
   } else {
      R__b.WriteVersion(AMSR_AntiClusterReader::IsA());
      AMSR_Maker::Streamer(R__b);
      R__b << m_Nclusters;
   }
}

//______________________________________________________________________________
void AMSR_AntiClusterReader::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSR_AntiClusterReader.

   TClass *R__cl  = AMSR_AntiClusterReader::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   R__insp.Inspect(R__cl, R__parent, "m_Nclusters", &m_Nclusters);
   AMSR_Maker::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSR_TrMCClusterReader *&obj)
{
   // Read a pointer to an object of class AMSR_TrMCClusterReader.

   obj = (AMSR_TrMCClusterReader *) buf.ReadObject(AMSR_TrMCClusterReader::Class());
   return buf;
}

//______________________________________________________________________________
void AMSR_TrMCClusterReader::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSR_TrMCClusterReader.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      AMSR_Maker::Streamer(R__b);
      R__b >> m_Nclusters;
   } else {
      R__b.WriteVersion(AMSR_TrMCClusterReader::IsA());
      AMSR_Maker::Streamer(R__b);
      R__b << m_Nclusters;
   }
}

//______________________________________________________________________________
void AMSR_TrMCClusterReader::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSR_TrMCClusterReader.

   TClass *R__cl  = AMSR_TrMCClusterReader::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   R__insp.Inspect(R__cl, R__parent, "m_Nclusters", &m_Nclusters);
   AMSR_Maker::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSR_ParticleReader *&obj)
{
   // Read a pointer to an object of class AMSR_ParticleReader.

   obj = (AMSR_ParticleReader *) buf.ReadObject(AMSR_ParticleReader::Class());
   return buf;
}

//______________________________________________________________________________
void AMSR_ParticleReader::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSR_ParticleReader.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      AMSR_Maker::Streamer(R__b);
      R__b >> m_NParticles;
      R__b >> m_Mult;
   } else {
      R__b.WriteVersion(AMSR_ParticleReader::IsA());
      AMSR_Maker::Streamer(R__b);
      R__b << m_NParticles;
      R__b << (TObject*)m_Mult;
   }
}

//______________________________________________________________________________
void AMSR_ParticleReader::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSR_ParticleReader.

   TClass *R__cl  = AMSR_ParticleReader::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   R__insp.Inspect(R__cl, R__parent, "m_NParticles", &m_NParticles);
   R__insp.Inspect(R__cl, R__parent, "*m_Mult", &m_Mult);
   AMSR_Maker::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSR_MCParticleReader *&obj)
{
   // Read a pointer to an object of class AMSR_MCParticleReader.

   obj = (AMSR_MCParticleReader *) buf.ReadObject(AMSR_MCParticleReader::Class());
   return buf;
}

//______________________________________________________________________________
void AMSR_MCParticleReader::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSR_MCParticleReader.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      AMSR_Maker::Streamer(R__b);
      R__b >> m_NParticles;
   } else {
      R__b.WriteVersion(AMSR_MCParticleReader::IsA());
      AMSR_Maker::Streamer(R__b);
      R__b << m_NParticles;
   }
}

//______________________________________________________________________________
void AMSR_MCParticleReader::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSR_MCParticleReader.

   TClass *R__cl  = AMSR_MCParticleReader::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   R__insp.Inspect(R__cl, R__parent, "m_NParticles", &m_NParticles);
   AMSR_Maker::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSR_Ntuple *&obj)
{
   // Read a pointer to an object of class AMSR_Ntuple.

   obj = (AMSR_Ntuple *) buf.ReadObject(AMSR_Ntuple::Class());
   return buf;
}

//______________________________________________________________________________
void AMSR_Ntuple::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSR_Ntuple.

   TNamed::Streamer(R__b);
}

//______________________________________________________________________________
void AMSR_Ntuple::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSR_Ntuple.

   TClass *R__cl  = AMSR_Ntuple::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   R__insp.Inspect(R__cl, R__parent, "m_VarNames[20][20]", m_VarNames);
   R__insp.Inspect(R__cl, R__parent, "m_NVar", &m_NVar);
   R__insp.Inspect(R__cl, R__parent, "*m_SampleTree", &m_SampleTree);
   R__insp.Inspect(R__cl, R__parent, "m_MemID", &m_MemID);
   R__insp.Inspect(R__cl, R__parent, "m_DataFileType", &m_DataFileType);
   R__insp.Inspect(R__cl, R__parent, "m_Entries", &m_Entries);
   R__insp.Inspect(R__cl, R__parent, "m_Lun", &m_Lun);
   R__insp.Inspect(R__cl, R__parent, "m_NtupleID", &m_NtupleID);
   R__insp.Inspect(R__cl, R__parent, "m_NextID", &m_NextID);
   R__insp.Inspect(R__cl, R__parent, "m_NtupleOpen", &m_NtupleOpen);
   R__insp.Inspect(R__cl, R__parent, "m_SameRead", &m_SameRead);
   R__insp.Inspect(R__cl, R__parent, "*m_Tree", &m_Tree);
   TNamed::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSR_VirtualDisplay *&obj)
{
   // Read a pointer to an object of class AMSR_VirtualDisplay.

   obj = (AMSR_VirtualDisplay *) buf.ReadObject(AMSR_VirtualDisplay::Class());
   return buf;
}

//______________________________________________________________________________
void AMSR_VirtualDisplay::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSR_VirtualDisplay.

   TObject::Streamer(R__b);
}

//______________________________________________________________________________
void AMSR_VirtualDisplay::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSR_VirtualDisplay.

   TClass *R__cl  = AMSR_VirtualDisplay::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   TObject::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSR_Root *&obj)
{
   // Read a pointer to an object of class AMSR_Root.

   obj = (AMSR_Root *) buf.ReadObject(AMSR_Root::Class());
   return buf;
}

//______________________________________________________________________________
void AMSR_Root::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSR_Root.

   TClass *R__cl  = AMSR_Root::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   R__insp.Inspect(R__cl, R__parent, "*m_DataFileName", &m_DataFileName);
   R__insp.Inspect(R__cl, R__parent, "m_DataFileType", &m_DataFileType);
   R__insp.Inspect(R__cl, R__parent, "m_Version", &m_Version);
   R__insp.Inspect(R__cl, R__parent, "m_VersionDate", &m_VersionDate);
   R__insp.Inspect(R__cl, R__parent, "m_RunNum", &m_RunNum);
   R__insp.Inspect(R__cl, R__parent, "m_EventNum", &m_EventNum);
   R__insp.Inspect(R__cl, R__parent, "m_Mode", &m_Mode);
   R__insp.Inspect(R__cl, R__parent, "m_NEvent", &m_NEvent);
   R__insp.Inspect(R__cl, R__parent, "m_Event", &m_Event);
   R__insp.Inspect(R__cl, R__parent, "m_Time[2]", m_Time);
   R__insp.Inspect(R__cl, R__parent, "*m_Tree", &m_Tree);
   R__insp.Inspect(R__cl, R__parent, "*m_Ntuple", &m_Ntuple);
   R__insp.Inspect(R__cl, R__parent, "*m_Makers", &m_Makers);
   R__insp.Inspect(R__cl, R__parent, "*m_ToFClusterMaker", &m_ToFClusterMaker);
   R__insp.Inspect(R__cl, R__parent, "*m_TrackMaker", &m_TrackMaker);
   R__insp.Inspect(R__cl, R__parent, "*m_SiHitMaker", &m_SiHitMaker);
   R__insp.Inspect(R__cl, R__parent, "*m_CTCClusterMaker", &m_CTCClusterMaker);
   R__insp.Inspect(R__cl, R__parent, "*m_AntiClusterMaker", &m_AntiClusterMaker);
   R__insp.Inspect(R__cl, R__parent, "*m_TrMCClusterMaker", &m_TrMCClusterMaker);
   R__insp.Inspect(R__cl, R__parent, "*m_ParticleMaker", &m_ParticleMaker);
   R__insp.Inspect(R__cl, R__parent, "*m_MCParticleMaker", &m_MCParticleMaker);
   m_HistBrowser.ShowMembers(R__insp, strcat(R__parent,"m_HistBrowser.")); R__parent[R__ncp] = 0;
   R__insp.Inspect(R__cl, R__parent, "*m_Display", &m_Display);
   TNamed::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSR_ToFCluster *&obj)
{
   // Read a pointer to an object of class AMSR_ToFCluster.

   obj = (AMSR_ToFCluster *) buf.ReadObject(AMSR_ToFCluster::Class());
   return buf;
}

//______________________________________________________________________________
void AMSR_ToFCluster::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSR_ToFCluster.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      AMSR_3DCluster::Streamer(R__b);
      R__b >> m_Status;
      R__b >> m_Plane;
      R__b >> m_Bar;
      R__b >> m_Energy;
      R__b >> m_Time;
      R__b >> m_ErTime;
      R__b >> m_Ncells;
      R__b >> m_Nparticles;
      R__b >> m_NTracks;
      R__b >> m_Tracks;
   } else {
      R__b.WriteVersion(AMSR_ToFCluster::IsA());
      AMSR_3DCluster::Streamer(R__b);
      R__b << m_Status;
      R__b << m_Plane;
      R__b << m_Bar;
      R__b << m_Energy;
      R__b << m_Time;
      R__b << m_ErTime;
      R__b << m_Ncells;
      R__b << m_Nparticles;
      R__b << m_NTracks;
      R__b << m_Tracks;
   }
}

//______________________________________________________________________________
void AMSR_ToFCluster::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSR_ToFCluster.

   TClass *R__cl  = AMSR_ToFCluster::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   R__insp.Inspect(R__cl, R__parent, "m_Status", &m_Status);
   R__insp.Inspect(R__cl, R__parent, "m_Plane", &m_Plane);
   R__insp.Inspect(R__cl, R__parent, "m_Bar", &m_Bar);
   R__insp.Inspect(R__cl, R__parent, "m_Energy", &m_Energy);
   R__insp.Inspect(R__cl, R__parent, "m_Time", &m_Time);
   R__insp.Inspect(R__cl, R__parent, "m_ErTime", &m_ErTime);
   R__insp.Inspect(R__cl, R__parent, "m_Ncells", &m_Ncells);
   R__insp.Inspect(R__cl, R__parent, "m_Nparticles", &m_Nparticles);
   R__insp.Inspect(R__cl, R__parent, "m_NTracks", &m_NTracks);
   R__insp.Inspect(R__cl, R__parent, "*m_Tracks", &m_Tracks);
   AMSR_3DCluster::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSR_AntiCluster *&obj)
{
   // Read a pointer to an object of class AMSR_AntiCluster.

   obj = (AMSR_AntiCluster *) buf.ReadObject(AMSR_AntiCluster::Class());
   return buf;
}

//______________________________________________________________________________
void AMSR_AntiCluster::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSR_AntiCluster.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      AMSR_3DCluster::Streamer(R__b);
      R__b >> m_Status;
      R__b >> m_Sector;
      R__b >> m_Signal;
      R__b >> m_NTracks;
      R__b >> m_Tracks;
   } else {
      R__b.WriteVersion(AMSR_AntiCluster::IsA());
      AMSR_3DCluster::Streamer(R__b);
      R__b << m_Status;
      R__b << m_Sector;
      R__b << m_Signal;
      R__b << m_NTracks;
      R__b << m_Tracks;
   }
}

//______________________________________________________________________________
void AMSR_AntiCluster::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSR_AntiCluster.

   TClass *R__cl  = AMSR_AntiCluster::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   R__insp.Inspect(R__cl, R__parent, "m_Status", &m_Status);
   R__insp.Inspect(R__cl, R__parent, "m_Sector", &m_Sector);
   R__insp.Inspect(R__cl, R__parent, "m_Signal", &m_Signal);
   R__insp.Inspect(R__cl, R__parent, "m_NTracks", &m_NTracks);
   R__insp.Inspect(R__cl, R__parent, "*m_Tracks", &m_Tracks);
   AMSR_3DCluster::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSR_TrMCCluster *&obj)
{
   // Read a pointer to an object of class AMSR_TrMCCluster.

   obj = (AMSR_TrMCCluster *) buf.ReadObject(AMSR_TrMCCluster::Class());
   return buf;
}

//______________________________________________________________________________
void AMSR_TrMCCluster::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSR_TrMCCluster.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      AMSR_3DCluster::Streamer(R__b);
      R__b >> m_Part;
      R__b >> m_Signal;
      R__b >> m_NTracks;
      R__b >> m_Tracks;
   } else {
      R__b.WriteVersion(AMSR_TrMCCluster::IsA());
      AMSR_3DCluster::Streamer(R__b);
      R__b << m_Part;
      R__b << m_Signal;
      R__b << m_NTracks;
      R__b << m_Tracks;
   }
}

//______________________________________________________________________________
void AMSR_TrMCCluster::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSR_TrMCCluster.

   TClass *R__cl  = AMSR_TrMCCluster::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   R__insp.Inspect(R__cl, R__parent, "m_Part", &m_Part);
   R__insp.Inspect(R__cl, R__parent, "m_Signal", &m_Signal);
   R__insp.Inspect(R__cl, R__parent, "m_NTracks", &m_NTracks);
   R__insp.Inspect(R__cl, R__parent, "*m_Tracks", &m_Tracks);
   AMSR_3DCluster::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSR_Track *&obj)
{
   // Read a pointer to an object of class AMSR_Track.

   obj = (AMSR_Track *) buf.ReadObject(AMSR_Track::Class());
   return buf;
}

//______________________________________________________________________________
void AMSR_Track::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSR_Track.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      THelix::Streamer(R__b);
      R__b >> m_ID;
      R__b >> m_Status;
      R__b >> m_Pattern;
      R__b >> m_NHits;
      R__b.ReadStaticArray(m_PHits);
      R__b >> m_FastFitDone;
      R__b >> m_GeaneFitDone;
      R__b >> m_AdvancedFitDone;
      R__b >> m_Chi2StrLine;
      R__b >> m_Chi2Circle;
      R__b >> m_CircleRigidity;
      R__b >> m_FChi2;
      R__b >> m_FRigidity;
      R__b >> m_FErrRigidity;
      R__b >> m_FTheta;
      R__b >> m_FPhi;
      R__b.ReadStaticArray(m_FP0);
      R__b >> m_GChi2;
      R__b >> m_GRigidity;
      R__b >> m_GErrRigidity;
      R__b >> m_GTheta;
      R__b >> m_GPhi;
      R__b.ReadStaticArray(m_GP0);
      R__b.ReadStaticArray(m_HChi2);
      R__b.ReadStaticArray(m_HRigidity);
      R__b.ReadStaticArray(m_HErrRigidity);
      R__b.ReadStaticArray(m_HTheta);
      R__b.ReadStaticArray(m_HPhi);
      R__b.ReadStaticArray((float*)m_HP0);
      R__b >> m_FChi2MS;
      R__b >> m_GChi2MS;
      R__b >> m_RigidityMS;
      R__b >> m_GRigidityMS;
   } else {
      R__b.WriteVersion(AMSR_Track::IsA());
      THelix::Streamer(R__b);
      R__b << m_ID;
      R__b << m_Status;
      R__b << m_Pattern;
      R__b << m_NHits;
      R__b.WriteArray(m_PHits, 6);
      R__b << m_FastFitDone;
      R__b << m_GeaneFitDone;
      R__b << m_AdvancedFitDone;
      R__b << m_Chi2StrLine;
      R__b << m_Chi2Circle;
      R__b << m_CircleRigidity;
      R__b << m_FChi2;
      R__b << m_FRigidity;
      R__b << m_FErrRigidity;
      R__b << m_FTheta;
      R__b << m_FPhi;
      R__b.WriteArray(m_FP0, 3);
      R__b << m_GChi2;
      R__b << m_GRigidity;
      R__b << m_GErrRigidity;
      R__b << m_GTheta;
      R__b << m_GPhi;
      R__b.WriteArray(m_GP0, 3);
      R__b.WriteArray(m_HChi2, 2);
      R__b.WriteArray(m_HRigidity, 2);
      R__b.WriteArray(m_HErrRigidity, 2);
      R__b.WriteArray(m_HTheta, 2);
      R__b.WriteArray(m_HPhi, 2);
      R__b.WriteArray((float*)m_HP0, 6);
      R__b << m_FChi2MS;
      R__b << m_GChi2MS;
      R__b << m_RigidityMS;
      R__b << m_GRigidityMS;
   }
}

//______________________________________________________________________________
void AMSR_Track::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSR_Track.

   TClass *R__cl  = AMSR_Track::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   R__insp.Inspect(R__cl, R__parent, "m_ID", &m_ID);
   R__insp.Inspect(R__cl, R__parent, "m_Status", &m_Status);
   R__insp.Inspect(R__cl, R__parent, "m_Pattern", &m_Pattern);
   R__insp.Inspect(R__cl, R__parent, "m_NHits", &m_NHits);
   R__insp.Inspect(R__cl, R__parent, "m_PHits[6]", m_PHits);
   R__insp.Inspect(R__cl, R__parent, "m_FastFitDone", &m_FastFitDone);
   R__insp.Inspect(R__cl, R__parent, "m_GeaneFitDone", &m_GeaneFitDone);
   R__insp.Inspect(R__cl, R__parent, "m_AdvancedFitDone", &m_AdvancedFitDone);
   R__insp.Inspect(R__cl, R__parent, "m_Chi2StrLine", &m_Chi2StrLine);
   R__insp.Inspect(R__cl, R__parent, "m_Chi2Circle", &m_Chi2Circle);
   R__insp.Inspect(R__cl, R__parent, "m_CircleRigidity", &m_CircleRigidity);
   R__insp.Inspect(R__cl, R__parent, "m_FChi2", &m_FChi2);
   R__insp.Inspect(R__cl, R__parent, "m_FRigidity", &m_FRigidity);
   R__insp.Inspect(R__cl, R__parent, "m_FErrRigidity", &m_FErrRigidity);
   R__insp.Inspect(R__cl, R__parent, "m_FTheta", &m_FTheta);
   R__insp.Inspect(R__cl, R__parent, "m_FPhi", &m_FPhi);
   R__insp.Inspect(R__cl, R__parent, "m_FP0[3]", m_FP0);
   R__insp.Inspect(R__cl, R__parent, "m_GChi2", &m_GChi2);
   R__insp.Inspect(R__cl, R__parent, "m_GRigidity", &m_GRigidity);
   R__insp.Inspect(R__cl, R__parent, "m_GErrRigidity", &m_GErrRigidity);
   R__insp.Inspect(R__cl, R__parent, "m_GTheta", &m_GTheta);
   R__insp.Inspect(R__cl, R__parent, "m_GPhi", &m_GPhi);
   R__insp.Inspect(R__cl, R__parent, "m_GP0[3]", m_GP0);
   R__insp.Inspect(R__cl, R__parent, "m_HChi2[2]", m_HChi2);
   R__insp.Inspect(R__cl, R__parent, "m_HRigidity[2]", m_HRigidity);
   R__insp.Inspect(R__cl, R__parent, "m_HErrRigidity[2]", m_HErrRigidity);
   R__insp.Inspect(R__cl, R__parent, "m_HTheta[2]", m_HTheta);
   R__insp.Inspect(R__cl, R__parent, "m_HPhi[2]", m_HPhi);
   R__insp.Inspect(R__cl, R__parent, "m_HP0[2][3]", m_HP0);
   R__insp.Inspect(R__cl, R__parent, "m_FChi2MS", &m_FChi2MS);
   R__insp.Inspect(R__cl, R__parent, "m_GChi2MS", &m_GChi2MS);
   R__insp.Inspect(R__cl, R__parent, "m_RigidityMS", &m_RigidityMS);
   R__insp.Inspect(R__cl, R__parent, "m_GRigidityMS", &m_GRigidityMS);
   THelix::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSR_SiHit *&obj)
{
   // Read a pointer to an object of class AMSR_SiHit.

   obj = (AMSR_SiHit *) buf.ReadObject(AMSR_SiHit::Class());
   return buf;
}

//______________________________________________________________________________
void AMSR_SiHit::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSR_SiHit.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      AMSR_3DCluster::Streamer(R__b);
      R__b >> m_Status;
      R__b >> m_Plane;
      R__b >> m_X;
      R__b >> m_Y;
      R__b.ReadStaticArray(m_ErrPosition);
      R__b >> m_Amplitude;
      R__b >> m_AsymAmpl;
      R__b >> m_Tracks;
   } else {
      R__b.WriteVersion(AMSR_SiHit::IsA());
      AMSR_3DCluster::Streamer(R__b);
      R__b << m_Status;
      R__b << m_Plane;
      R__b << m_X;
      R__b << m_Y;
      R__b.WriteArray(m_ErrPosition, 3);
      R__b << m_Amplitude;
      R__b << m_AsymAmpl;
      R__b << m_Tracks;
   }
}

//______________________________________________________________________________
void AMSR_SiHit::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSR_SiHit.

   TClass *R__cl  = AMSR_SiHit::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   R__insp.Inspect(R__cl, R__parent, "m_Status", &m_Status);
   R__insp.Inspect(R__cl, R__parent, "m_Plane", &m_Plane);
   R__insp.Inspect(R__cl, R__parent, "m_X", &m_X);
   R__insp.Inspect(R__cl, R__parent, "m_Y", &m_Y);
   R__insp.Inspect(R__cl, R__parent, "m_ErrPosition[3]", m_ErrPosition);
   R__insp.Inspect(R__cl, R__parent, "m_Amplitude", &m_Amplitude);
   R__insp.Inspect(R__cl, R__parent, "m_AsymAmpl", &m_AsymAmpl);
   R__insp.Inspect(R__cl, R__parent, "*m_Tracks", &m_Tracks);
   AMSR_3DCluster::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSR_CTCCluster *&obj)
{
   // Read a pointer to an object of class AMSR_CTCCluster.

   obj = (AMSR_CTCCluster *) buf.ReadObject(AMSR_CTCCluster::Class());
   return buf;
}

//______________________________________________________________________________
void AMSR_CTCCluster::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSR_CTCCluster.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      AMSR_3DCluster::Streamer(R__b);
      R__b >> m_Status;
      R__b >> m_Plane;
      R__b >> m_RawSignal;
      R__b >> m_Signal;
      R__b >> m_ErrSignal;
      R__b >> m_NTracks;
      R__b >> m_Tracks;
   } else {
      R__b.WriteVersion(AMSR_CTCCluster::IsA());
      AMSR_3DCluster::Streamer(R__b);
      R__b << m_Status;
      R__b << m_Plane;
      R__b << m_RawSignal;
      R__b << m_Signal;
      R__b << m_ErrSignal;
      R__b << m_NTracks;
      R__b << m_Tracks;
   }
}

//______________________________________________________________________________
void AMSR_CTCCluster::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSR_CTCCluster.

   TClass *R__cl  = AMSR_CTCCluster::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   R__insp.Inspect(R__cl, R__parent, "m_Status", &m_Status);
   R__insp.Inspect(R__cl, R__parent, "m_Plane", &m_Plane);
   R__insp.Inspect(R__cl, R__parent, "m_RawSignal", &m_RawSignal);
   R__insp.Inspect(R__cl, R__parent, "m_Signal", &m_Signal);
   R__insp.Inspect(R__cl, R__parent, "m_ErrSignal", &m_ErrSignal);
   R__insp.Inspect(R__cl, R__parent, "m_NTracks", &m_NTracks);
   R__insp.Inspect(R__cl, R__parent, "*m_Tracks", &m_Tracks);
   AMSR_3DCluster::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSR_Particle *&obj)
{
   // Read a pointer to an object of class AMSR_Particle.

   obj = (AMSR_Particle *) buf.ReadObject(AMSR_Particle::Class());
   return buf;
}

//______________________________________________________________________________
void AMSR_Particle::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSR_Particle.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      THelix::Streamer(R__b);
      R__b.ReadStaticArray(m_PCerenkov);
      R__b >> m_PBeta;
      R__b >> m_PCharge;
      R__b >> m_PTrack;
      R__b >> m_GeantID;
      R__b >> m_Mass;
      R__b >> m_ErrMass;
      R__b >> m_Momentum;
      R__b >> m_ErrMomentum;
      R__b >> m_Charge;
      R__b >> m_Theta;
      R__b >> m_Phi;
      R__b.ReadStaticArray(m_Position);
      R__b.ReadStaticArray(m_SignalCTC);
      R__b.ReadStaticArray(m_BetaCTC);
      R__b.ReadStaticArray(m_ErrBetaCTC);
      R__b.ReadStaticArray((float*)m_PositionCTC);
      R__b >> m_beta;
      R__b >> m_errbeta;
      R__b >> m_betapattern;
      R__b >> m_pattern;
      R__b >> m_trstatus;
   } else {
      R__b.WriteVersion(AMSR_Particle::IsA());
      THelix::Streamer(R__b);
      R__b.WriteArray(m_PCerenkov, 2);
      R__b << m_PBeta;
      R__b << m_PCharge;
      R__b << m_PTrack;
      R__b << m_GeantID;
      R__b << m_Mass;
      R__b << m_ErrMass;
      R__b << m_Momentum;
      R__b << m_ErrMomentum;
      R__b << m_Charge;
      R__b << m_Theta;
      R__b << m_Phi;
      R__b.WriteArray(m_Position, 3);
      R__b.WriteArray(m_SignalCTC, 2);
      R__b.WriteArray(m_BetaCTC, 2);
      R__b.WriteArray(m_ErrBetaCTC, 2);
      R__b.WriteArray((float*)m_PositionCTC, 6);
      R__b << m_beta;
      R__b << m_errbeta;
      R__b << m_betapattern;
      R__b << m_pattern;
      R__b << m_trstatus;
   }
}

//______________________________________________________________________________
void AMSR_Particle::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSR_Particle.

   TClass *R__cl  = AMSR_Particle::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   R__insp.Inspect(R__cl, R__parent, "m_PCerenkov[2]", m_PCerenkov);
   R__insp.Inspect(R__cl, R__parent, "m_PBeta", &m_PBeta);
   R__insp.Inspect(R__cl, R__parent, "m_PCharge", &m_PCharge);
   R__insp.Inspect(R__cl, R__parent, "m_PTrack", &m_PTrack);
   R__insp.Inspect(R__cl, R__parent, "m_GeantID", &m_GeantID);
   R__insp.Inspect(R__cl, R__parent, "m_Mass", &m_Mass);
   R__insp.Inspect(R__cl, R__parent, "m_ErrMass", &m_ErrMass);
   R__insp.Inspect(R__cl, R__parent, "m_Momentum", &m_Momentum);
   R__insp.Inspect(R__cl, R__parent, "m_ErrMomentum", &m_ErrMomentum);
   R__insp.Inspect(R__cl, R__parent, "m_Charge", &m_Charge);
   R__insp.Inspect(R__cl, R__parent, "m_Theta", &m_Theta);
   R__insp.Inspect(R__cl, R__parent, "m_Phi", &m_Phi);
   R__insp.Inspect(R__cl, R__parent, "m_Position[3]", m_Position);
   R__insp.Inspect(R__cl, R__parent, "m_SignalCTC[2]", m_SignalCTC);
   R__insp.Inspect(R__cl, R__parent, "m_BetaCTC[2]", m_BetaCTC);
   R__insp.Inspect(R__cl, R__parent, "m_ErrBetaCTC[2]", m_ErrBetaCTC);
   R__insp.Inspect(R__cl, R__parent, "m_PositionCTC[2][3]", m_PositionCTC);
   R__insp.Inspect(R__cl, R__parent, "m_beta", &m_beta);
   R__insp.Inspect(R__cl, R__parent, "m_errbeta", &m_errbeta);
   R__insp.Inspect(R__cl, R__parent, "m_betapattern", &m_betapattern);
   R__insp.Inspect(R__cl, R__parent, "m_pattern", &m_pattern);
   R__insp.Inspect(R__cl, R__parent, "m_trstatus", &m_trstatus);
   THelix::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSR_MCParticle *&obj)
{
   // Read a pointer to an object of class AMSR_MCParticle.

   obj = (AMSR_MCParticle *) buf.ReadObject(AMSR_MCParticle::Class());
   return buf;
}

//______________________________________________________________________________
void AMSR_MCParticle::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSR_MCParticle.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      THelix::Streamer(R__b);
      R__b >> m_GeantID;
      R__b >> m_Mass;
      R__b >> m_Momentum;
      R__b >> m_Charge;
      R__b.ReadStaticArray(m_Position);
      R__b.ReadStaticArray(m_Dir);
   } else {
      R__b.WriteVersion(AMSR_MCParticle::IsA());
      THelix::Streamer(R__b);
      R__b << m_GeantID;
      R__b << m_Mass;
      R__b << m_Momentum;
      R__b << m_Charge;
      R__b.WriteArray(m_Position, 3);
      R__b.WriteArray(m_Dir, 3);
   }
}

//______________________________________________________________________________
void AMSR_MCParticle::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSR_MCParticle.

   TClass *R__cl  = AMSR_MCParticle::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   R__insp.Inspect(R__cl, R__parent, "m_GeantID", &m_GeantID);
   R__insp.Inspect(R__cl, R__parent, "m_Mass", &m_Mass);
   R__insp.Inspect(R__cl, R__parent, "m_Momentum", &m_Momentum);
   R__insp.Inspect(R__cl, R__parent, "m_Charge", &m_Charge);
   R__insp.Inspect(R__cl, R__parent, "m_Position[3]", m_Position);
   R__insp.Inspect(R__cl, R__parent, "m_Dir[3]", m_Dir);
   THelix::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSR_Canvas *&obj)
{
   // Read a pointer to an object of class AMSR_Canvas.

   obj = (AMSR_Canvas *) buf.ReadObject(AMSR_Canvas::Class());
   return buf;
}

//______________________________________________________________________________
void AMSR_Canvas::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSR_Canvas.

   TCanvas::Streamer(R__b);
}

//______________________________________________________________________________
void AMSR_Canvas::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSR_Canvas.

   TClass *R__cl  = AMSR_Canvas::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   TCanvas::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, Debugger *&obj)
{
   // Read a pointer to an object of class Debugger.

   obj = (Debugger *) buf.ReadObject(Debugger::Class());
   return buf;
}

//______________________________________________________________________________
void Debugger::Streamer(TBuffer &R__b)
{
   // Stream an object of class Debugger.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      TObject::Streamer(R__b);
      R__b >> isOn;
   } else {
      R__b.WriteVersion(Debugger::IsA());
      TObject::Streamer(R__b);
      R__b << isOn;
   }
}

//______________________________________________________________________________
void Debugger::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class Debugger.

   TClass *R__cl  = Debugger::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   R__insp.Inspect(R__cl, R__parent, "isOn", &isOn);
   TObject::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSR_GeometrySetter *&obj)
{
   // Read a pointer to an object of class AMSR_GeometrySetter.

   ::Error("AMSR_GeometrySetter::operator>>", "objects not inheriting from TObject need a specialized operator>> function"); if (obj) { }
   return buf;
}

//______________________________________________________________________________
void AMSR_GeometrySetter::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSR_GeometrySetter.

   ::Error("AMSR_GeometrySetter::Streamer", "version id <=0 in ClassDef, dummy Streamer() called"); if (R__b.IsReading()) { }
}

//______________________________________________________________________________
void AMSR_GeometrySetter::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSR_GeometrySetter.

   TClass *R__cl  = AMSR_GeometrySetter::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   R__insp.Inspect(R__cl, R__parent, "*m_Geometry", &m_Geometry);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, TGRunEventDialog *&obj)
{
   // Read a pointer to an object of class TGRunEventDialog.

   obj = (TGRunEventDialog *) buf.ReadObject(TGRunEventDialog::Class());
   return buf;
}

//______________________________________________________________________________
void TGRunEventDialog::Streamer(TBuffer &R__b)
{
   // Stream an object of class TGRunEventDialog.

   TGTransientFrame::Streamer(R__b);
}

//______________________________________________________________________________
void TGRunEventDialog::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class TGRunEventDialog.

   TClass *R__cl  = TGRunEventDialog::IsA();
   Int_t   R__ncp = strlen(R__parent);
   if (R__ncp || R__cl || R__insp.IsA()) { }
   R__insp.Inspect(R__cl, R__parent, "*fRun", &fRun);
   R__insp.Inspect(R__cl, R__parent, "*fEvent", &fEvent);
   R__insp.Inspect(R__cl, R__parent, "*fText", &fText);
   R__insp.Inspect(R__cl, R__parent, "*fLrun", &fLrun);
   R__insp.Inspect(R__cl, R__parent, "*fLevt", &fLevt);
   R__insp.Inspect(R__cl, R__parent, "*fTbrun", &fTbrun);
   R__insp.Inspect(R__cl, R__parent, "*fTbevt", &fTbevt);
   R__insp.Inspect(R__cl, R__parent, "*fTrun", &fTrun);
   R__insp.Inspect(R__cl, R__parent, "*fTevt", &fTevt);
   R__insp.Inspect(R__cl, R__parent, "*fOk", &fOk);
   R__insp.Inspect(R__cl, R__parent, "*fClear", &fClear);
   R__insp.Inspect(R__cl, R__parent, "*fCancel", &fCancel);
   R__insp.Inspect(R__cl, R__parent, "*fVenter", &fVenter);
   R__insp.Inspect(R__cl, R__parent, "*fHrun", &fHrun);
   R__insp.Inspect(R__cl, R__parent, "*fHevt", &fHevt);
   R__insp.Inspect(R__cl, R__parent, "*fHb", &fHb);
   R__insp.Inspect(R__cl, R__parent, "*fLmain", &fLmain);
   R__insp.Inspect(R__cl, R__parent, "*fLv", &fLv);
   R__insp.Inspect(R__cl, R__parent, "*fLhl", &fLhl);
   R__insp.Inspect(R__cl, R__parent, "*fLht", &fLht);
   R__insp.Inspect(R__cl, R__parent, "*fLhb", &fLhb);
   TGTransientFrame::ShowMembers(R__insp, R__parent);
}

