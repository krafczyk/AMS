/********************************************************
* AMSCint.cxx
********************************************************/
#include "AMSCint.h"

#ifdef G__MEMTEST
#undef malloc
#endif


extern "C" void G__set_cpp_environmentAMSCint() {
  G__add_compiledheader("TROOT.h");
  G__add_compiledheader("TMemberInspector.h");
  G__add_compiledheader("TTree.h");
  G__add_compiledheader("TFile.h");
  G__add_compiledheader("TH1.h");
  G__add_compiledheader("THelix.h");
  G__add_compiledheader("TRadioButton.h");
  G__add_compiledheader("TSwitch.h");
  G__add_compiledheader("AMSAxis.h");
  G__add_compiledheader("AMS3DCluster.h");
  G__add_compiledheader("AMSMaker.h");
  G__add_compiledheader("AMSHistBrowser.h");
  G__add_compiledheader("AMSRoot.h");
  G__add_compiledheader("AMSToFCluster.h");
  G__add_compiledheader("AMSToFClusterReader.h");
  G__add_compiledheader("AMSAntiCluster.h");
  G__add_compiledheader("AMSAntiClusterReader.h");
  G__add_compiledheader("AMSTrack.h");
  G__add_compiledheader("AMSTrackReader.h");
  G__add_compiledheader("AMSSiHit.h");
  G__add_compiledheader("AMSSiHitReader.h");
  G__add_compiledheader("AMSCTCCluster.h");
  G__add_compiledheader("AMSCTCClusterReader.h");
  G__add_compiledheader("AMSParticle.h");
  G__add_compiledheader("AMSParticleReader.h");
  G__add_compiledheader("AMSVirtualDisplay.h");
  G__add_compiledheader("Debugger.h");
  G__add_compiledheader("AMSCanvas.h");
  G__add_compiledheader("AMSDisplay.h");
  G__add_compiledheader("AMSGeometrySetter.h");
}
int G__cpp_dllrevAMSCint() { return(51111); }

/*********************************************************
* Member function Interface Method
*********************************************************/

/* THelix */
static int G__THelix_THelix_2_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   THelix *p;
   if(G__getaryconstruct()) p=new THelix[G__getaryconstruct()];
   else                    p=new THelix;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_THelix);
   return(1);
}

static int G__THelix_THelix_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   THelix *p;
   switch(libp->paran) {
   case 14:
      p = new THelix(
(Double_t)G__double(libp->para[0]),(Double_t)G__double(libp->para[1])
,(Double_t)G__double(libp->para[2]),(Double_t)G__double(libp->para[3])
,(Double_t)G__double(libp->para[4]),(Double_t)G__double(libp->para[5])
,(Double_t)G__double(libp->para[6]),(Double_t)G__double(libp->para[7])
,(Double_t)G__double(libp->para[8]),(EHelixRangeType)G__int(libp->para[9])
,(Double_t)G__double(libp->para[10]),(Double_t)G__double(libp->para[11])
,(Double_t)G__double(libp->para[12]),(Option_t*)G__int(libp->para[13]));
      break;
   case 13:
      p = new THelix(
(Double_t)G__double(libp->para[0]),(Double_t)G__double(libp->para[1])
,(Double_t)G__double(libp->para[2]),(Double_t)G__double(libp->para[3])
,(Double_t)G__double(libp->para[4]),(Double_t)G__double(libp->para[5])
,(Double_t)G__double(libp->para[6]),(Double_t)G__double(libp->para[7])
,(Double_t)G__double(libp->para[8]),(EHelixRangeType)G__int(libp->para[9])
,(Double_t)G__double(libp->para[10]),(Double_t)G__double(libp->para[11])
,(Double_t)G__double(libp->para[12]));
      break;
   case 12:
      p = new THelix(
(Double_t)G__double(libp->para[0]),(Double_t)G__double(libp->para[1])
,(Double_t)G__double(libp->para[2]),(Double_t)G__double(libp->para[3])
,(Double_t)G__double(libp->para[4]),(Double_t)G__double(libp->para[5])
,(Double_t)G__double(libp->para[6]),(Double_t)G__double(libp->para[7])
,(Double_t)G__double(libp->para[8]),(EHelixRangeType)G__int(libp->para[9])
,(Double_t)G__double(libp->para[10]),(Double_t)G__double(libp->para[11]));
      break;
   case 11:
      p = new THelix(
(Double_t)G__double(libp->para[0]),(Double_t)G__double(libp->para[1])
,(Double_t)G__double(libp->para[2]),(Double_t)G__double(libp->para[3])
,(Double_t)G__double(libp->para[4]),(Double_t)G__double(libp->para[5])
,(Double_t)G__double(libp->para[6]),(Double_t)G__double(libp->para[7])
,(Double_t)G__double(libp->para[8]),(EHelixRangeType)G__int(libp->para[9])
,(Double_t)G__double(libp->para[10]));
      break;
   case 10:
      p = new THelix(
(Double_t)G__double(libp->para[0]),(Double_t)G__double(libp->para[1])
,(Double_t)G__double(libp->para[2]),(Double_t)G__double(libp->para[3])
,(Double_t)G__double(libp->para[4]),(Double_t)G__double(libp->para[5])
,(Double_t)G__double(libp->para[6]),(Double_t)G__double(libp->para[7])
,(Double_t)G__double(libp->para[8]),(EHelixRangeType)G__int(libp->para[9]));
      break;
   case 9:
      p = new THelix(
(Double_t)G__double(libp->para[0]),(Double_t)G__double(libp->para[1])
,(Double_t)G__double(libp->para[2]),(Double_t)G__double(libp->para[3])
,(Double_t)G__double(libp->para[4]),(Double_t)G__double(libp->para[5])
,(Double_t)G__double(libp->para[6]),(Double_t)G__double(libp->para[7])
,(Double_t)G__double(libp->para[8]));
      break;
   case 8:
      p = new THelix(
(Double_t)G__double(libp->para[0]),(Double_t)G__double(libp->para[1])
,(Double_t)G__double(libp->para[2]),(Double_t)G__double(libp->para[3])
,(Double_t)G__double(libp->para[4]),(Double_t)G__double(libp->para[5])
,(Double_t)G__double(libp->para[6]),(Double_t)G__double(libp->para[7]));
      break;
   case 7:
      p = new THelix(
(Double_t)G__double(libp->para[0]),(Double_t)G__double(libp->para[1])
,(Double_t)G__double(libp->para[2]),(Double_t)G__double(libp->para[3])
,(Double_t)G__double(libp->para[4]),(Double_t)G__double(libp->para[5])
,(Double_t)G__double(libp->para[6]));
      break;
   }
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_THelix);
   return(1);
}

static int G__THelix_THelix_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   THelix *p;
   switch(libp->paran) {
   case 7:
      p = new THelix(
(Double_t*)G__int(libp->para[0]),(Double_t*)G__int(libp->para[1])
,(Double_t)G__double(libp->para[2]),(Double_t*)G__int(libp->para[3])
,(EHelixRangeType)G__int(libp->para[4]),(Double_t*)G__int(libp->para[5])
,(Option_t*)G__int(libp->para[6]));
      break;
   case 6:
      p = new THelix(
(Double_t*)G__int(libp->para[0]),(Double_t*)G__int(libp->para[1])
,(Double_t)G__double(libp->para[2]),(Double_t*)G__int(libp->para[3])
,(EHelixRangeType)G__int(libp->para[4]),(Double_t*)G__int(libp->para[5]));
      break;
   case 5:
      p = new THelix(
(Double_t*)G__int(libp->para[0]),(Double_t*)G__int(libp->para[1])
,(Double_t)G__double(libp->para[2]),(Double_t*)G__int(libp->para[3])
,(EHelixRangeType)G__int(libp->para[4]));
      break;
   case 4:
      p = new THelix(
(Double_t*)G__int(libp->para[0]),(Double_t*)G__int(libp->para[1])
,(Double_t)G__double(libp->para[2]),(Double_t*)G__int(libp->para[3]));
      break;
   case 3:
      p = new THelix(
(Double_t*)G__int(libp->para[0]),(Double_t*)G__int(libp->para[1])
,(Double_t)G__double(libp->para[2]));
      break;
   }
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_THelix);
   return(1);
}

static int G__THelix_THelix_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   THelix *p;
      p = new THelix(*(THelix*)libp->para[0].ref);
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_THelix);
   return(1);
}

static int G__THelix_Copy_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((THelix*)(G__getstructoffset()))->Copy(*(TObject*)libp->para[0].ref);
   return(1);
}

static int G__THelix_Draw_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((THelix*)(G__getstructoffset()))->Draw((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((THelix*)(G__getstructoffset()))->Draw();
      break;
   }
   return(1);
}

static int G__THelix_GetOption_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((THelix*)(G__getstructoffset()))->GetOption());
   return(1);
}

static int G__THelix_Is3D_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((THelix*)(G__getstructoffset()))->Is3D());
   return(1);
}

static int G__THelix_Paint_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((THelix*)(G__getstructoffset()))->Paint((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((THelix*)(G__getstructoffset()))->Paint();
      break;
   }
   return(1);
}

static int G__THelix_Print_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((THelix*)(G__getstructoffset()))->Print((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((THelix*)(G__getstructoffset()))->Print();
      break;
   }
   return(1);
}

static int G__THelix_SavePrimitive_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((THelix*)(G__getstructoffset()))->SavePrimitive(*(ofstream*)libp->para[0].ref,(Option_t*)G__int(libp->para[1]));
   return(1);
}

static int G__THelix_SetOption_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((THelix*)(G__getstructoffset()))->SetOption((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((THelix*)(G__getstructoffset()))->SetOption();
      break;
   }
   return(1);
}

static int G__THelix_SetAxis_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((THelix*)(G__getstructoffset()))->SetAxis((Double_t*)G__int(libp->para[0]));
   return(1);
}

static int G__THelix_SetAxis_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((THelix*)(G__getstructoffset()))->SetAxis((Double_t)G__double(libp->para[0]),(Double_t)G__double(libp->para[1])
,(Double_t)G__double(libp->para[2]));
   return(1);
}

static int G__THelix_SetRange_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 2:
      G__setnull(result7);
      ((THelix*)(G__getstructoffset()))->SetRange((Double_t*)G__int(libp->para[0]),(EHelixRangeType)G__int(libp->para[1]));
      break;
   case 1:
      G__setnull(result7);
      ((THelix*)(G__getstructoffset()))->SetRange((Double_t*)G__int(libp->para[0]));
      break;
   }
   return(1);
}

static int G__THelix_SetRange_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 3:
      G__setnull(result7);
      ((THelix*)(G__getstructoffset()))->SetRange((Double_t)G__double(libp->para[0]),(Double_t)G__double(libp->para[1])
,(EHelixRangeType)G__int(libp->para[2]));
      break;
   case 2:
      G__setnull(result7);
      ((THelix*)(G__getstructoffset()))->SetRange((Double_t)G__double(libp->para[0]),(Double_t)G__double(libp->para[1]));
      break;
   }
   return(1);
}

static int G__THelix_SetHelix_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 13:
      G__setnull(result7);
      ((THelix*)(G__getstructoffset()))->SetHelix(
(Double_t)G__double(libp->para[0]),(Double_t)G__double(libp->para[1])
,(Double_t)G__double(libp->para[2]),(Double_t)G__double(libp->para[3])
,(Double_t)G__double(libp->para[4]),(Double_t)G__double(libp->para[5])
,(Double_t)G__double(libp->para[6]),(Double_t)G__double(libp->para[7])
,(Double_t)G__double(libp->para[8]),(EHelixRangeType)G__int(libp->para[9])
,(Double_t)G__double(libp->para[10]),(Double_t)G__double(libp->para[11])
,(Double_t)G__double(libp->para[12]));
      break;
   case 12:
      G__setnull(result7);
      ((THelix*)(G__getstructoffset()))->SetHelix(
(Double_t)G__double(libp->para[0]),(Double_t)G__double(libp->para[1])
,(Double_t)G__double(libp->para[2]),(Double_t)G__double(libp->para[3])
,(Double_t)G__double(libp->para[4]),(Double_t)G__double(libp->para[5])
,(Double_t)G__double(libp->para[6]),(Double_t)G__double(libp->para[7])
,(Double_t)G__double(libp->para[8]),(EHelixRangeType)G__int(libp->para[9])
,(Double_t)G__double(libp->para[10]),(Double_t)G__double(libp->para[11]));
      break;
   case 11:
      G__setnull(result7);
      ((THelix*)(G__getstructoffset()))->SetHelix(
(Double_t)G__double(libp->para[0]),(Double_t)G__double(libp->para[1])
,(Double_t)G__double(libp->para[2]),(Double_t)G__double(libp->para[3])
,(Double_t)G__double(libp->para[4]),(Double_t)G__double(libp->para[5])
,(Double_t)G__double(libp->para[6]),(Double_t)G__double(libp->para[7])
,(Double_t)G__double(libp->para[8]),(EHelixRangeType)G__int(libp->para[9])
,(Double_t)G__double(libp->para[10]));
      break;
   case 10:
      G__setnull(result7);
      ((THelix*)(G__getstructoffset()))->SetHelix(
(Double_t)G__double(libp->para[0]),(Double_t)G__double(libp->para[1])
,(Double_t)G__double(libp->para[2]),(Double_t)G__double(libp->para[3])
,(Double_t)G__double(libp->para[4]),(Double_t)G__double(libp->para[5])
,(Double_t)G__double(libp->para[6]),(Double_t)G__double(libp->para[7])
,(Double_t)G__double(libp->para[8]),(EHelixRangeType)G__int(libp->para[9]));
      break;
   case 9:
      G__setnull(result7);
      ((THelix*)(G__getstructoffset()))->SetHelix(
(Double_t)G__double(libp->para[0]),(Double_t)G__double(libp->para[1])
,(Double_t)G__double(libp->para[2]),(Double_t)G__double(libp->para[3])
,(Double_t)G__double(libp->para[4]),(Double_t)G__double(libp->para[5])
,(Double_t)G__double(libp->para[6]),(Double_t)G__double(libp->para[7])
,(Double_t)G__double(libp->para[8]));
      break;
   case 8:
      G__setnull(result7);
      ((THelix*)(G__getstructoffset()))->SetHelix(
(Double_t)G__double(libp->para[0]),(Double_t)G__double(libp->para[1])
,(Double_t)G__double(libp->para[2]),(Double_t)G__double(libp->para[3])
,(Double_t)G__double(libp->para[4]),(Double_t)G__double(libp->para[5])
,(Double_t)G__double(libp->para[6]),(Double_t)G__double(libp->para[7]));
      break;
   case 7:
      G__setnull(result7);
      ((THelix*)(G__getstructoffset()))->SetHelix(
(Double_t)G__double(libp->para[0]),(Double_t)G__double(libp->para[1])
,(Double_t)G__double(libp->para[2]),(Double_t)G__double(libp->para[3])
,(Double_t)G__double(libp->para[4]),(Double_t)G__double(libp->para[5])
,(Double_t)G__double(libp->para[6]));
      break;
   }
   return(1);
}

static int G__THelix_Sizeof3D_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((THelix*)(G__getstructoffset()))->Sizeof3D();
   return(1);
}

static int G__THelix_DeclFileName_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((THelix*)(G__getstructoffset()))->DeclFileName());
   return(1);
}

static int G__THelix_DeclFileLine_2_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((THelix*)(G__getstructoffset()))->DeclFileLine());
   return(1);
}

static int G__THelix_ImplFileName_3_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((THelix*)(G__getstructoffset()))->ImplFileName());
   return(1);
}

static int G__THelix_ImplFileLine_4_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((THelix*)(G__getstructoffset()))->ImplFileLine());
   return(1);
}

static int G__THelix_Class_Version_5_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((THelix*)(G__getstructoffset()))->Class_Version());
   return(1);
}

static int G__THelix_Class_6_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((THelix*)(G__getstructoffset()))->Class());
   return(1);
}

static int G__THelix_Dictionary_7_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((THelix*)(G__getstructoffset()))->Dictionary();
   return(1);
}

static int G__THelix_IsA_8_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((THelix*)(G__getstructoffset()))->IsA());
   return(1);
}

static int G__THelix_ShowMembers_9_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((THelix*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1);
}

static int G__THelix_Streamer_0_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((THelix*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1);
}

// automatic destructor
static int G__THelix_wATHelix_1_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (THelix *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((THelix *)((G__getstructoffset())+sizeof(THelix)*i))->~THelix();
   else if(G__PVOID==G__getgvp()) delete (THelix *)(G__getstructoffset());
   else ((THelix *)(G__getstructoffset()))->~THelix();
   return(1);
}


/* TRadioButton */
static int G__TRadioButton_TRadioButton_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   TRadioButton *p;
   if(G__getaryconstruct()) p=new TRadioButton[G__getaryconstruct()];
   else                    p=new TRadioButton;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_TRadioButton);
   return(1);
}

static int G__TRadioButton_TRadioButton_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   TRadioButton *p;
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
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_TRadioButton);
   return(1);
}

static int G__TRadioButton_ExecuteEvent_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((TRadioButton*)(G__getstructoffset()))->ExecuteEvent((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]));
   return(1);
}

static int G__TRadioButton_DeclFileName_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((TRadioButton*)(G__getstructoffset()))->DeclFileName());
   return(1);
}

static int G__TRadioButton_DeclFileLine_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((TRadioButton*)(G__getstructoffset()))->DeclFileLine());
   return(1);
}

static int G__TRadioButton_ImplFileName_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((TRadioButton*)(G__getstructoffset()))->ImplFileName());
   return(1);
}

static int G__TRadioButton_ImplFileLine_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((TRadioButton*)(G__getstructoffset()))->ImplFileLine());
   return(1);
}

static int G__TRadioButton_Class_Version_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((TRadioButton*)(G__getstructoffset()))->Class_Version());
   return(1);
}

static int G__TRadioButton_Class_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((TRadioButton*)(G__getstructoffset()))->Class());
   return(1);
}

static int G__TRadioButton_Dictionary_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((TRadioButton*)(G__getstructoffset()))->Dictionary();
   return(1);
}

static int G__TRadioButton_IsA_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((TRadioButton*)(G__getstructoffset()))->IsA());
   return(1);
}

static int G__TRadioButton_ShowMembers_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((TRadioButton*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1);
}

static int G__TRadioButton_Streamer_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((TRadioButton*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1);
}

// automatic copy constructor
static int G__TRadioButton_TRadioButton_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   TRadioButton *p;
   if(1!=libp->paran) ;
   p=new TRadioButton(*(TRadioButton*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_TRadioButton);
   return(1);
}

// automatic destructor
static int G__TRadioButton_wATRadioButton_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (TRadioButton *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((TRadioButton *)((G__getstructoffset())+sizeof(TRadioButton)*i))->~TRadioButton();
   else if(G__PVOID==G__getgvp()) delete (TRadioButton *)(G__getstructoffset());
   else ((TRadioButton *)(G__getstructoffset()))->~TRadioButton();
   return(1);
}


/* TSwitch */
static int G__TSwitch_TSwitch_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   TSwitch *p;
   if(G__getaryconstruct()) p=new TSwitch[G__getaryconstruct()];
   else                    p=new TSwitch;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_TSwitch);
   return(1);
}

static int G__TSwitch_TSwitch_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   TSwitch *p;
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
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_TSwitch);
   return(1);
}

static int G__TSwitch_ExecuteEvent_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((TSwitch*)(G__getstructoffset()))->ExecuteEvent((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]));
   return(1);
}

static int G__TSwitch_GetButton_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((TSwitch*)(G__getstructoffset()))->GetButton());
   return(1);
}

static int G__TSwitch_DeclFileName_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((TSwitch*)(G__getstructoffset()))->DeclFileName());
   return(1);
}

static int G__TSwitch_DeclFileLine_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((TSwitch*)(G__getstructoffset()))->DeclFileLine());
   return(1);
}

static int G__TSwitch_ImplFileName_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((TSwitch*)(G__getstructoffset()))->ImplFileName());
   return(1);
}

static int G__TSwitch_ImplFileLine_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((TSwitch*)(G__getstructoffset()))->ImplFileLine());
   return(1);
}

static int G__TSwitch_Class_Version_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((TSwitch*)(G__getstructoffset()))->Class_Version());
   return(1);
}

static int G__TSwitch_Class_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((TSwitch*)(G__getstructoffset()))->Class());
   return(1);
}

static int G__TSwitch_Dictionary_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((TSwitch*)(G__getstructoffset()))->Dictionary();
   return(1);
}

static int G__TSwitch_IsA_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((TSwitch*)(G__getstructoffset()))->IsA());
   return(1);
}

static int G__TSwitch_ShowMembers_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((TSwitch*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1);
}

static int G__TSwitch_Streamer_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((TSwitch*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1);
}

// automatic copy constructor
static int G__TSwitch_TSwitch_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   TSwitch *p;
   if(1!=libp->paran) ;
   p=new TSwitch(*(TSwitch*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_TSwitch);
   return(1);
}

// automatic destructor
static int G__TSwitch_wATSwitch_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (TSwitch *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((TSwitch *)((G__getstructoffset())+sizeof(TSwitch)*i))->~TSwitch();
   else if(G__PVOID==G__getgvp()) delete (TSwitch *)(G__getstructoffset());
   else ((TSwitch *)(G__getstructoffset()))->~TSwitch();
   return(1);
}


/* AMSAxis */
static int G__AMSAxis_AMSAxis_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSAxis *p;
   if(G__getaryconstruct()) p=new AMSAxis[G__getaryconstruct()];
   else                    p=new AMSAxis;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSAxis);
   return(1);
}

static int G__AMSAxis_AMSAxis_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSAxis *p;
      p = new AMSAxis((Float_t*)G__int(libp->para[0]),(char*)G__int(libp->para[1]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSAxis);
   return(1);
}

static int G__AMSAxis_AMSAxis_2_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSAxis *p;
      p = new AMSAxis(
(Float_t)G__double(libp->para[0]),(Float_t)G__double(libp->para[1])
,(Float_t)G__double(libp->para[2]),(char*)G__int(libp->para[3]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSAxis);
   return(1);
}

static int G__AMSAxis_GetObjectInfo_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSAxis*)(G__getstructoffset()))->GetObjectInfo((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1);
}

static int G__AMSAxis_Paint_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSAxis*)(G__getstructoffset()))->Paint((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSAxis*)(G__getstructoffset()))->Paint();
      break;
   }
   return(1);
}

static int G__AMSAxis_DeclFileName_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSAxis*)(G__getstructoffset()))->DeclFileName());
   return(1);
}

static int G__AMSAxis_DeclFileLine_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSAxis*)(G__getstructoffset()))->DeclFileLine());
   return(1);
}

static int G__AMSAxis_ImplFileName_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSAxis*)(G__getstructoffset()))->ImplFileName());
   return(1);
}

static int G__AMSAxis_ImplFileLine_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSAxis*)(G__getstructoffset()))->ImplFileLine());
   return(1);
}

static int G__AMSAxis_Class_Version_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSAxis*)(G__getstructoffset()))->Class_Version());
   return(1);
}

static int G__AMSAxis_Class_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSAxis*)(G__getstructoffset()))->Class());
   return(1);
}

static int G__AMSAxis_Dictionary_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSAxis*)(G__getstructoffset()))->Dictionary();
   return(1);
}

static int G__AMSAxis_IsA_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSAxis*)(G__getstructoffset()))->IsA());
   return(1);
}

static int G__AMSAxis_ShowMembers_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSAxis*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1);
}

static int G__AMSAxis_Streamer_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSAxis*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1);
}

// automatic copy constructor
static int G__AMSAxis_AMSAxis_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSAxis *p;
   if(1!=libp->paran) ;
   p=new AMSAxis(*(AMSAxis*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSAxis);
   return(1);
}

// automatic destructor
static int G__AMSAxis_wAAMSAxis_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSAxis *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSAxis *)((G__getstructoffset())+sizeof(AMSAxis)*i))->~AMSAxis();
   else if(G__PVOID==G__getgvp()) delete (AMSAxis *)(G__getstructoffset());
   else ((AMSAxis *)(G__getstructoffset()))->~AMSAxis();
   return(1);
}


/* AMSDisplay */
static int G__AMSDisplay_AMSDisplay_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSDisplay *p;
   if(G__getaryconstruct()) p=new AMSDisplay[G__getaryconstruct()];
   else                    p=new AMSDisplay;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSDisplay);
   return(1);
}

static int G__AMSDisplay_AMSDisplay_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSDisplay *p;
   switch(libp->paran) {
   case 2:
      p = new AMSDisplay((const char*)G__int(libp->para[0]),(TGeometry*)G__int(libp->para[1]));
      break;
   case 1:
      p = new AMSDisplay((const char*)G__int(libp->para[0]));
      break;
   }
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSDisplay);
   return(1);
}

static int G__AMSDisplay_AllViews_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((AMSDisplay*)(G__getstructoffset()))->AllViews());
   return(1);
}

static int G__AMSDisplay_GetView_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSDisplay*)(G__getstructoffset()))->GetView());
   return(1);
}

static int G__AMSDisplay_Clear_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1);
}

static int G__AMSDisplay_DisplayButtons_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->DisplayButtons();
   return(1);
}

static int G__AMSDisplay_DistancetoPrimitive_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSDisplay*)(G__getstructoffset()))->DistancetoPrimitive((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1);
}

static int G__AMSDisplay_Draw_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->Draw((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->Draw();
      break;
   }
   return(1);
}

static int G__AMSDisplay_DrawAllViews_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->DrawAllViews();
   return(1);
}

static int G__AMSDisplay_DrawFrontAndSideViews_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->DrawFrontAndSideViews();
   return(1);
}

static int G__AMSDisplay_DrawParticles_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((AMSDisplay*)(G__getstructoffset()))->DrawParticles());
   return(1);
}

static int G__AMSDisplay_DrawTitle_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->DrawTitle((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->DrawTitle();
      break;
   }
   return(1);
}

static int G__AMSDisplay_DrawEventInfo_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->DrawEventInfo((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->DrawEventInfo();
      break;
   }
   return(1);
}

static int G__AMSDisplay_DrawCaption_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->DrawCaption((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->DrawCaption();
      break;
   }
   return(1);
}

static int G__AMSDisplay_DrawAxis_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 2:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->DrawAxis((Int_t)G__int(libp->para[0]),(Option_t*)G__int(libp->para[1]));
      break;
   case 1:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->DrawAxis((Int_t)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->DrawAxis();
      break;
   }
   return(1);
}

static int G__AMSDisplay_DrawView_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 3:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->DrawView((Float_t)G__double(libp->para[0]),(Float_t)G__double(libp->para[1])
,(Int_t)G__int(libp->para[2]));
      break;
   case 2:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->DrawView((Float_t)G__double(libp->para[0]),(Float_t)G__double(libp->para[1]));
      break;
   case 1:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->DrawView((Float_t)G__double(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->DrawView();
      break;
   }
   return(1);
}

static int G__AMSDisplay_DrawViewGL_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->DrawViewGL();
   return(1);
}

static int G__AMSDisplay_DrawViewX3D_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->DrawViewX3D();
   return(1);
}

static int G__AMSDisplay_ExecuteEvent_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->ExecuteEvent((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]));
   return(1);
}

static int G__AMSDisplay_GetEvent_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->GetEvent((Int_t)G__int(libp->para[0]));
   return(1);
}

static int G__AMSDisplay_Pad_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSDisplay*)(G__getstructoffset()))->Pad());
   return(1);
}

static int G__AMSDisplay_GetCanvas_2_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSDisplay*)(G__getstructoffset()))->GetCanvas());
   return(1);
}

static int G__AMSDisplay_GetObjInfoPad_3_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSDisplay*)(G__getstructoffset()))->GetObjInfoPad());
   return(1);
}

static int G__AMSDisplay_GetEventInfoPad_4_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSDisplay*)(G__getstructoffset()))->GetEventInfoPad());
   return(1);
}

static int G__AMSDisplay_GetTitlePad_5_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSDisplay*)(G__getstructoffset()))->GetTitlePad());
   return(1);
}

static int G__AMSDisplay_Paint_6_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->Paint((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->Paint();
      break;
   }
   return(1);
}

static int G__AMSDisplay_PaintParticles_7_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->PaintParticles((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->PaintParticles();
      break;
   }
   return(1);
}

static int G__AMSDisplay_PTcut_8_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letdouble(result7,102,(double)((AMSDisplay*)(G__getstructoffset()))->PTcut());
   return(1);
}

static int G__AMSDisplay_PTcutEGMUNU_9_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letdouble(result7,102,(double)((AMSDisplay*)(G__getstructoffset()))->PTcutEGMUNU());
   return(1);
}

static int G__AMSDisplay_GetObjectInfo_0_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSDisplay*)(G__getstructoffset()))->GetObjectInfo((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1);
}

static int G__AMSDisplay_SetDrawParticles_1_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->SetDrawParticles((Bool_t)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->SetDrawParticles();
      break;
   }
   return(1);
}

static int G__AMSDisplay_SetPTcut_2_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->SetPTcut((Float_t)G__double(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->SetPTcut();
      break;
   }
   return(1);
}

static int G__AMSDisplay_SetPTcutEGMUNU_3_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->SetPTcutEGMUNU((Float_t)G__double(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->SetPTcutEGMUNU();
      break;
   }
   return(1);
}

static int G__AMSDisplay_SetGeometry_4_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->SetGeometry((TGeometry*)G__int(libp->para[0]));
   return(1);
}

static int G__AMSDisplay_SetView_5_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->SetView((Float_t)G__double(libp->para[0]),(Float_t)G__double(libp->para[1]));
   return(1);
}

static int G__AMSDisplay_SetView_6_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->SetView((EAMSView)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->SetView();
      break;
   }
   return(1);
}

static int G__AMSDisplay_ShowNextEvent_7_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->ShowNextEvent((Int_t)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->ShowNextEvent();
      break;
   }
   return(1);
}

static int G__AMSDisplay_SizeParticles_8_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->SizeParticles();
   return(1);
}

static int G__AMSDisplay_GetGeometrySetter_9_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSDisplay*)(G__getstructoffset()))->GetGeometrySetter());
   return(1);
}

static int G__AMSDisplay_SetGeo_0_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->SetGeo();
   return(1);
}

static int G__AMSDisplay_DeclFileName_1_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSDisplay*)(G__getstructoffset()))->DeclFileName());
   return(1);
}

static int G__AMSDisplay_DeclFileLine_2_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSDisplay*)(G__getstructoffset()))->DeclFileLine());
   return(1);
}

static int G__AMSDisplay_ImplFileName_3_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSDisplay*)(G__getstructoffset()))->ImplFileName());
   return(1);
}

static int G__AMSDisplay_ImplFileLine_4_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSDisplay*)(G__getstructoffset()))->ImplFileLine());
   return(1);
}

static int G__AMSDisplay_Class_Version_5_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSDisplay*)(G__getstructoffset()))->Class_Version());
   return(1);
}

static int G__AMSDisplay_Class_6_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSDisplay*)(G__getstructoffset()))->Class());
   return(1);
}

static int G__AMSDisplay_Dictionary_7_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->Dictionary();
   return(1);
}

static int G__AMSDisplay_IsA_8_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSDisplay*)(G__getstructoffset()))->IsA());
   return(1);
}

static int G__AMSDisplay_ShowMembers_9_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1);
}

static int G__AMSDisplay_Streamer_0_5(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSDisplay*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1);
}

// automatic copy constructor
static int G__AMSDisplay_AMSDisplay_1_5(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSDisplay *p;
   if(1!=libp->paran) ;
   p=new AMSDisplay(*(AMSDisplay*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSDisplay);
   return(1);
}

// automatic destructor
static int G__AMSDisplay_wAAMSDisplay_2_5(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSDisplay *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSDisplay *)((G__getstructoffset())+sizeof(AMSDisplay)*i))->~AMSDisplay();
   else if(G__PVOID==G__getgvp()) delete (AMSDisplay *)(G__getstructoffset());
   else ((AMSDisplay *)(G__getstructoffset()))->~AMSDisplay();
   return(1);
}


/* AMS3DCluster */
static int G__AMS3DCluster_AMS3DCluster_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMS3DCluster *p;
   if(G__getaryconstruct()) p=new AMS3DCluster[G__getaryconstruct()];
   else                    p=new AMS3DCluster;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMS3DCluster);
   return(1);
}

static int G__AMS3DCluster_AMS3DCluster_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMS3DCluster *p;
   switch(libp->paran) {
   case 9:
      p = new AMS3DCluster(
(Float_t)G__double(libp->para[0]),(Float_t)G__double(libp->para[1])
,(Float_t)G__double(libp->para[2]),(Float_t)G__double(libp->para[3])
,(Float_t)G__double(libp->para[4]),(Float_t)G__double(libp->para[5])
,(Float_t)G__double(libp->para[6]),(Float_t)G__double(libp->para[7])
,(Int_t)G__int(libp->para[8]));
      break;
   case 8:
      p = new AMS3DCluster(
(Float_t)G__double(libp->para[0]),(Float_t)G__double(libp->para[1])
,(Float_t)G__double(libp->para[2]),(Float_t)G__double(libp->para[3])
,(Float_t)G__double(libp->para[4]),(Float_t)G__double(libp->para[5])
,(Float_t)G__double(libp->para[6]),(Float_t)G__double(libp->para[7]));
      break;
   }
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMS3DCluster);
   return(1);
}

static int G__AMS3DCluster_AMS3DCluster_2_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMS3DCluster *p;
   switch(libp->paran) {
   case 4:
      p = new AMS3DCluster(
(Float_t*)G__int(libp->para[0]),(Float_t*)G__int(libp->para[1])
,(Float_t*)G__int(libp->para[2]),(Int_t)G__int(libp->para[3]));
      break;
   case 3:
      p = new AMS3DCluster(
(Float_t*)G__int(libp->para[0]),(Float_t*)G__int(libp->para[1])
,(Float_t*)G__int(libp->para[2]));
      break;
   }
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMS3DCluster);
   return(1);
}

static int G__AMS3DCluster_Delete_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMS3DCluster*)(G__getstructoffset()))->Delete((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMS3DCluster*)(G__getstructoffset()))->Delete();
      break;
   }
   return(1);
}

static int G__AMS3DCluster_GetObjectInfo_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMS3DCluster*)(G__getstructoffset()))->GetObjectInfo((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1);
}

static int G__AMS3DCluster_GetPosition_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,70,(long)((AMS3DCluster*)(G__getstructoffset()))->GetPosition());
   return(1);
}

static int G__AMS3DCluster_GetSize_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,70,(long)((AMS3DCluster*)(G__getstructoffset()))->GetSize());
   return(1);
}

static int G__AMS3DCluster_DistancetoPrimitive_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMS3DCluster*)(G__getstructoffset()))->DistancetoPrimitive((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1);
}

static int G__AMS3DCluster_SetPoints_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMS3DCluster*)(G__getstructoffset()))->SetPoints((Float_t*)G__int(libp->para[0]));
   return(1);
}

static int G__AMS3DCluster_SetPosition_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMS3DCluster*)(G__getstructoffset()))->SetPosition((Float_t)G__double(libp->para[0]),(Float_t)G__double(libp->para[1])
,(Float_t)G__double(libp->para[2]));
   return(1);
}

static int G__AMS3DCluster_SetDirection_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMS3DCluster*)(G__getstructoffset()))->SetDirection((Float_t)G__double(libp->para[0]),(Float_t)G__double(libp->para[1]));
   return(1);
}

static int G__AMS3DCluster_Sizeof3D_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMS3DCluster*)(G__getstructoffset()))->Sizeof3D();
   return(1);
}

static int G__AMS3DCluster_DeclFileName_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMS3DCluster*)(G__getstructoffset()))->DeclFileName());
   return(1);
}

static int G__AMS3DCluster_DeclFileLine_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMS3DCluster*)(G__getstructoffset()))->DeclFileLine());
   return(1);
}

static int G__AMS3DCluster_ImplFileName_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMS3DCluster*)(G__getstructoffset()))->ImplFileName());
   return(1);
}

static int G__AMS3DCluster_ImplFileLine_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMS3DCluster*)(G__getstructoffset()))->ImplFileLine());
   return(1);
}

static int G__AMS3DCluster_Class_Version_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMS3DCluster*)(G__getstructoffset()))->Class_Version());
   return(1);
}

static int G__AMS3DCluster_Class_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMS3DCluster*)(G__getstructoffset()))->Class());
   return(1);
}

static int G__AMS3DCluster_Dictionary_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMS3DCluster*)(G__getstructoffset()))->Dictionary();
   return(1);
}

static int G__AMS3DCluster_IsA_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMS3DCluster*)(G__getstructoffset()))->IsA());
   return(1);
}

static int G__AMS3DCluster_ShowMembers_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMS3DCluster*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1);
}

static int G__AMS3DCluster_Streamer_2_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMS3DCluster*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1);
}

// automatic copy constructor
static int G__AMS3DCluster_AMS3DCluster_3_2(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMS3DCluster *p;
   if(1!=libp->paran) ;
   p=new AMS3DCluster(*(AMS3DCluster*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMS3DCluster);
   return(1);
}

// automatic destructor
static int G__AMS3DCluster_wAAMS3DCluster_4_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMS3DCluster *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMS3DCluster *)((G__getstructoffset())+sizeof(AMS3DCluster)*i))->~AMS3DCluster();
   else if(G__PVOID==G__getgvp()) delete (AMS3DCluster *)(G__getstructoffset());
   else ((AMS3DCluster *)(G__getstructoffset()))->~AMS3DCluster();
   return(1);
}


/* AMSMaker */
static int G__AMSMaker_Init_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSMaker*)(G__getstructoffset()))->Init((TTree*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSMaker*)(G__getstructoffset()))->Init();
      break;
   }
   return(1);
}

static int G__AMSMaker_Finish_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSMaker*)(G__getstructoffset()))->Finish();
   return(1);
}

static int G__AMSMaker_Browse_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSMaker*)(G__getstructoffset()))->Browse((TBrowser*)G__int(libp->para[0]));
   return(1);
}

static int G__AMSMaker_Draw_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSMaker*)(G__getstructoffset()))->Draw((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSMaker*)(G__getstructoffset()))->Draw();
      break;
   }
   return(1);
}

static int G__AMSMaker_Clear_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSMaker*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSMaker*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1);
}

static int G__AMSMaker_IsFolder_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((AMSMaker*)(G__getstructoffset()))->IsFolder());
   return(1);
}

static int G__AMSMaker_Histograms_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSMaker*)(G__getstructoffset()))->Histograms());
   return(1);
}

static int G__AMSMaker_Fruits_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSMaker*)(G__getstructoffset()))->Fruits());
   return(1);
}

static int G__AMSMaker_PrintInfo_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSMaker*)(G__getstructoffset()))->PrintInfo();
   return(1);
}

static int G__AMSMaker_Enabled_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((AMSMaker*)(G__getstructoffset()))->Enabled((TObject*)G__int(libp->para[0])));
   return(1);
}

static int G__AMSMaker_Save_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSMaker*)(G__getstructoffset()))->Save((Int_t)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSMaker*)(G__getstructoffset()))->Save();
      break;
   }
   return(1);
}

static int G__AMSMaker_MakeBranch_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSMaker*)(G__getstructoffset()))->MakeBranch();
   return(1);
}

static int G__AMSMaker_Make_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSMaker*)(G__getstructoffset()))->Make();
   return(1);
}

static int G__AMSMaker_DeclFileName_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSMaker*)(G__getstructoffset()))->DeclFileName());
   return(1);
}

static int G__AMSMaker_DeclFileLine_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSMaker*)(G__getstructoffset()))->DeclFileLine());
   return(1);
}

static int G__AMSMaker_ImplFileName_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSMaker*)(G__getstructoffset()))->ImplFileName());
   return(1);
}

static int G__AMSMaker_ImplFileLine_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSMaker*)(G__getstructoffset()))->ImplFileLine());
   return(1);
}

static int G__AMSMaker_Class_Version_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSMaker*)(G__getstructoffset()))->Class_Version());
   return(1);
}

static int G__AMSMaker_Class_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSMaker*)(G__getstructoffset()))->Class());
   return(1);
}

static int G__AMSMaker_Dictionary_2_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSMaker*)(G__getstructoffset()))->Dictionary();
   return(1);
}

static int G__AMSMaker_IsA_3_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSMaker*)(G__getstructoffset()))->IsA());
   return(1);
}

static int G__AMSMaker_ShowMembers_4_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSMaker*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1);
}

static int G__AMSMaker_Streamer_5_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSMaker*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1);
}

// automatic destructor
static int G__AMSMaker_wAAMSMaker_6_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSMaker *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSMaker *)((G__getstructoffset())+sizeof(AMSMaker)*i))->~AMSMaker();
   else if(G__PVOID==G__getgvp()) delete (AMSMaker *)(G__getstructoffset());
   else ((AMSMaker *)(G__getstructoffset()))->~AMSMaker();
   return(1);
}


/* AMSHistBrowser */
static int G__AMSHistBrowser_AMSHistBrowser_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSHistBrowser *p;
   if(G__getaryconstruct()) p=new AMSHistBrowser[G__getaryconstruct()];
   else                    p=new AMSHistBrowser;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSHistBrowser);
   return(1);
}

static int G__AMSHistBrowser_Browse_2_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSHistBrowser*)(G__getstructoffset()))->Browse((TBrowser*)G__int(libp->para[0]));
   return(1);
}

static int G__AMSHistBrowser_IsFolder_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((AMSHistBrowser*)(G__getstructoffset()))->IsFolder());
   return(1);
}

static int G__AMSHistBrowser_DeclFileName_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSHistBrowser*)(G__getstructoffset()))->DeclFileName());
   return(1);
}

static int G__AMSHistBrowser_DeclFileLine_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSHistBrowser*)(G__getstructoffset()))->DeclFileLine());
   return(1);
}

static int G__AMSHistBrowser_ImplFileName_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSHistBrowser*)(G__getstructoffset()))->ImplFileName());
   return(1);
}

static int G__AMSHistBrowser_ImplFileLine_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSHistBrowser*)(G__getstructoffset()))->ImplFileLine());
   return(1);
}

static int G__AMSHistBrowser_Class_Version_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSHistBrowser*)(G__getstructoffset()))->Class_Version());
   return(1);
}

static int G__AMSHistBrowser_Class_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSHistBrowser*)(G__getstructoffset()))->Class());
   return(1);
}

static int G__AMSHistBrowser_Dictionary_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSHistBrowser*)(G__getstructoffset()))->Dictionary();
   return(1);
}

static int G__AMSHistBrowser_IsA_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSHistBrowser*)(G__getstructoffset()))->IsA());
   return(1);
}

static int G__AMSHistBrowser_ShowMembers_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSHistBrowser*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1);
}

static int G__AMSHistBrowser_Streamer_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSHistBrowser*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1);
}

// automatic copy constructor
static int G__AMSHistBrowser_AMSHistBrowser_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSHistBrowser *p;
   if(1!=libp->paran) ;
   p=new AMSHistBrowser(*(AMSHistBrowser*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSHistBrowser);
   return(1);
}

// automatic destructor
static int G__AMSHistBrowser_wAAMSHistBrowser_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSHistBrowser *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSHistBrowser *)((G__getstructoffset())+sizeof(AMSHistBrowser)*i))->~AMSHistBrowser();
   else if(G__PVOID==G__getgvp()) delete (AMSHistBrowser *)(G__getstructoffset());
   else ((AMSHistBrowser *)(G__getstructoffset()))->~AMSHistBrowser();
   return(1);
}


/* AMSToFClusterReader */
static int G__AMSToFClusterReader_AMSToFClusterReader_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSToFClusterReader *p;
   if(G__getaryconstruct()) p=new AMSToFClusterReader[G__getaryconstruct()];
   else                    p=new AMSToFClusterReader;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSToFClusterReader);
   return(1);
}

static int G__AMSToFClusterReader_AMSToFClusterReader_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSToFClusterReader *p;
      p = new AMSToFClusterReader((const char*)G__int(libp->para[0]),(const char*)G__int(libp->para[1]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSToFClusterReader);
   return(1);
}

static int G__AMSToFClusterReader_Init_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSToFClusterReader*)(G__getstructoffset()))->Init((TTree*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSToFClusterReader*)(G__getstructoffset()))->Init();
      break;
   }
   return(1);
}

static int G__AMSToFClusterReader_Finish_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSToFClusterReader*)(G__getstructoffset()))->Finish();
   return(1);
}

static int G__AMSToFClusterReader_Make_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSToFClusterReader*)(G__getstructoffset()))->Make();
   return(1);
}

static int G__AMSToFClusterReader_PrintInfo_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSToFClusterReader*)(G__getstructoffset()))->PrintInfo();
   return(1);
}

static int G__AMSToFClusterReader_AddCluster_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 12:
      G__setnull(result7);
      ((AMSToFClusterReader*)(G__getstructoffset()))->AddCluster(
(Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]),(Float_t)G__double(libp->para[3])
,(Float_t)G__double(libp->para[4]),(Float_t)G__double(libp->para[5])
,(Float_t*)G__int(libp->para[6]),(Float_t*)G__int(libp->para[7])
,(Int_t)G__int(libp->para[8]),(Int_t)G__int(libp->para[9])
,(Int_t)G__int(libp->para[10]),(TObjArray*)G__int(libp->para[11]));
      break;
   case 11:
      G__setnull(result7);
      ((AMSToFClusterReader*)(G__getstructoffset()))->AddCluster(
(Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]),(Float_t)G__double(libp->para[3])
,(Float_t)G__double(libp->para[4]),(Float_t)G__double(libp->para[5])
,(Float_t*)G__int(libp->para[6]),(Float_t*)G__int(libp->para[7])
,(Int_t)G__int(libp->para[8]),(Int_t)G__int(libp->para[9])
,(Int_t)G__int(libp->para[10]));
      break;
   case 10:
      G__setnull(result7);
      ((AMSToFClusterReader*)(G__getstructoffset()))->AddCluster(
(Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]),(Float_t)G__double(libp->para[3])
,(Float_t)G__double(libp->para[4]),(Float_t)G__double(libp->para[5])
,(Float_t*)G__int(libp->para[6]),(Float_t*)G__int(libp->para[7])
,(Int_t)G__int(libp->para[8]),(Int_t)G__int(libp->para[9]));
      break;
   case 9:
      G__setnull(result7);
      ((AMSToFClusterReader*)(G__getstructoffset()))->AddCluster(
(Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]),(Float_t)G__double(libp->para[3])
,(Float_t)G__double(libp->para[4]),(Float_t)G__double(libp->para[5])
,(Float_t*)G__int(libp->para[6]),(Float_t*)G__int(libp->para[7])
,(Int_t)G__int(libp->para[8]));
      break;
   case 8:
      G__setnull(result7);
      ((AMSToFClusterReader*)(G__getstructoffset()))->AddCluster(
(Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]),(Float_t)G__double(libp->para[3])
,(Float_t)G__double(libp->para[4]),(Float_t)G__double(libp->para[5])
,(Float_t*)G__int(libp->para[6]),(Float_t*)G__int(libp->para[7]));
      break;
   }
   return(1);
}

static int G__AMSToFClusterReader_RemoveCluster_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSToFClusterReader*)(G__getstructoffset()))->RemoveCluster((const Int_t)G__int(libp->para[0]));
   return(1);
}

static int G__AMSToFClusterReader_Clear_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSToFClusterReader*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSToFClusterReader*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1);
}

static int G__AMSToFClusterReader_DeclFileName_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSToFClusterReader*)(G__getstructoffset()))->DeclFileName());
   return(1);
}

static int G__AMSToFClusterReader_DeclFileLine_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSToFClusterReader*)(G__getstructoffset()))->DeclFileLine());
   return(1);
}

static int G__AMSToFClusterReader_ImplFileName_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSToFClusterReader*)(G__getstructoffset()))->ImplFileName());
   return(1);
}

static int G__AMSToFClusterReader_ImplFileLine_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSToFClusterReader*)(G__getstructoffset()))->ImplFileLine());
   return(1);
}

static int G__AMSToFClusterReader_Class_Version_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSToFClusterReader*)(G__getstructoffset()))->Class_Version());
   return(1);
}

static int G__AMSToFClusterReader_Class_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSToFClusterReader*)(G__getstructoffset()))->Class());
   return(1);
}

static int G__AMSToFClusterReader_Dictionary_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSToFClusterReader*)(G__getstructoffset()))->Dictionary();
   return(1);
}

static int G__AMSToFClusterReader_IsA_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSToFClusterReader*)(G__getstructoffset()))->IsA());
   return(1);
}

static int G__AMSToFClusterReader_ShowMembers_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSToFClusterReader*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1);
}

static int G__AMSToFClusterReader_Streamer_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSToFClusterReader*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1);
}

// automatic copy constructor
static int G__AMSToFClusterReader_AMSToFClusterReader_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSToFClusterReader *p;
   if(1!=libp->paran) ;
   p=new AMSToFClusterReader(*(AMSToFClusterReader*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSToFClusterReader);
   return(1);
}

// automatic destructor
static int G__AMSToFClusterReader_wAAMSToFClusterReader_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSToFClusterReader *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSToFClusterReader *)((G__getstructoffset())+sizeof(AMSToFClusterReader)*i))->~AMSToFClusterReader();
   else if(G__PVOID==G__getgvp()) delete (AMSToFClusterReader *)(G__getstructoffset());
   else ((AMSToFClusterReader *)(G__getstructoffset()))->~AMSToFClusterReader();
   return(1);
}


/* AMSTrackReader */
static int G__AMSTrackReader_AMSTrackReader_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSTrackReader *p;
   if(G__getaryconstruct()) p=new AMSTrackReader[G__getaryconstruct()];
   else                    p=new AMSTrackReader;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSTrackReader);
   return(1);
}

static int G__AMSTrackReader_AMSTrackReader_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSTrackReader *p;
      p = new AMSTrackReader((const char*)G__int(libp->para[0]),(const char*)G__int(libp->para[1]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSTrackReader);
   return(1);
}

static int G__AMSTrackReader_AddTrack_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSTrackReader*)(G__getstructoffset()))->AddTrack((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1);
}

static int G__AMSTrackReader_Clear_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSTrackReader*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSTrackReader*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1);
}

static int G__AMSTrackReader_Finish_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSTrackReader*)(G__getstructoffset()))->Finish();
   return(1);
}

static int G__AMSTrackReader_Init_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSTrackReader*)(G__getstructoffset()))->Init((TTree*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSTrackReader*)(G__getstructoffset()))->Init();
      break;
   }
   return(1);
}

static int G__AMSTrackReader_Make_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSTrackReader*)(G__getstructoffset()))->Make();
   return(1);
}

static int G__AMSTrackReader_PrintInfo_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSTrackReader*)(G__getstructoffset()))->PrintInfo();
   return(1);
}

static int G__AMSTrackReader_Mult_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSTrackReader*)(G__getstructoffset()))->Mult());
   return(1);
}

static int G__AMSTrackReader_DeclFileName_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSTrackReader*)(G__getstructoffset()))->DeclFileName());
   return(1);
}

static int G__AMSTrackReader_DeclFileLine_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSTrackReader*)(G__getstructoffset()))->DeclFileLine());
   return(1);
}

static int G__AMSTrackReader_ImplFileName_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSTrackReader*)(G__getstructoffset()))->ImplFileName());
   return(1);
}

static int G__AMSTrackReader_ImplFileLine_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSTrackReader*)(G__getstructoffset()))->ImplFileLine());
   return(1);
}

static int G__AMSTrackReader_Class_Version_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSTrackReader*)(G__getstructoffset()))->Class_Version());
   return(1);
}

static int G__AMSTrackReader_Class_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSTrackReader*)(G__getstructoffset()))->Class());
   return(1);
}

static int G__AMSTrackReader_Dictionary_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSTrackReader*)(G__getstructoffset()))->Dictionary();
   return(1);
}

static int G__AMSTrackReader_IsA_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSTrackReader*)(G__getstructoffset()))->IsA());
   return(1);
}

static int G__AMSTrackReader_ShowMembers_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSTrackReader*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1);
}

static int G__AMSTrackReader_Streamer_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSTrackReader*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1);
}

// automatic copy constructor
static int G__AMSTrackReader_AMSTrackReader_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSTrackReader *p;
   if(1!=libp->paran) ;
   p=new AMSTrackReader(*(AMSTrackReader*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSTrackReader);
   return(1);
}

// automatic destructor
static int G__AMSTrackReader_wAAMSTrackReader_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSTrackReader *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSTrackReader *)((G__getstructoffset())+sizeof(AMSTrackReader)*i))->~AMSTrackReader();
   else if(G__PVOID==G__getgvp()) delete (AMSTrackReader *)(G__getstructoffset());
   else ((AMSTrackReader *)(G__getstructoffset()))->~AMSTrackReader();
   return(1);
}


/* AMSSiHitReader */
static int G__AMSSiHitReader_AMSSiHitReader_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSSiHitReader *p;
   if(G__getaryconstruct()) p=new AMSSiHitReader[G__getaryconstruct()];
   else                    p=new AMSSiHitReader;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSSiHitReader);
   return(1);
}

static int G__AMSSiHitReader_AMSSiHitReader_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSSiHitReader *p;
      p = new AMSSiHitReader((const char*)G__int(libp->para[0]),(const char*)G__int(libp->para[1]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSSiHitReader);
   return(1);
}

static int G__AMSSiHitReader_Init_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSSiHitReader*)(G__getstructoffset()))->Init((TTree*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSSiHitReader*)(G__getstructoffset()))->Init();
      break;
   }
   return(1);
}

static int G__AMSSiHitReader_Finish_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSSiHitReader*)(G__getstructoffset()))->Finish();
   return(1);
}

static int G__AMSSiHitReader_Make_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSSiHitReader*)(G__getstructoffset()))->Make();
   return(1);
}

static int G__AMSSiHitReader_PrintInfo_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSSiHitReader*)(G__getstructoffset()))->PrintInfo();
   return(1);
}

static int G__AMSSiHitReader_Enabled_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((AMSSiHitReader*)(G__getstructoffset()))->Enabled((TObject*)G__int(libp->para[0])));
   return(1);
}

static int G__AMSSiHitReader_AddCluster_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 9:
      G__setnull(result7);
      ((AMSSiHitReader*)(G__getstructoffset()))->AddCluster(
(Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]),(Int_t)G__int(libp->para[3])
,(Float_t*)G__int(libp->para[4]),(Float_t*)G__int(libp->para[5])
,(Float_t)G__double(libp->para[6]),(Float_t)G__double(libp->para[7])
,(TObjArray*)G__int(libp->para[8]));
      break;
   case 8:
      G__setnull(result7);
      ((AMSSiHitReader*)(G__getstructoffset()))->AddCluster(
(Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]),(Int_t)G__int(libp->para[3])
,(Float_t*)G__int(libp->para[4]),(Float_t*)G__int(libp->para[5])
,(Float_t)G__double(libp->para[6]),(Float_t)G__double(libp->para[7]));
      break;
   }
   return(1);
}

static int G__AMSSiHitReader_RemoveCluster_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSSiHitReader*)(G__getstructoffset()))->RemoveCluster((const Int_t)G__int(libp->para[0]));
   return(1);
}

static int G__AMSSiHitReader_Clear_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSSiHitReader*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSSiHitReader*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1);
}

static int G__AMSSiHitReader_DeclFileName_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSSiHitReader*)(G__getstructoffset()))->DeclFileName());
   return(1);
}

static int G__AMSSiHitReader_DeclFileLine_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSSiHitReader*)(G__getstructoffset()))->DeclFileLine());
   return(1);
}

static int G__AMSSiHitReader_ImplFileName_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSSiHitReader*)(G__getstructoffset()))->ImplFileName());
   return(1);
}

static int G__AMSSiHitReader_ImplFileLine_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSSiHitReader*)(G__getstructoffset()))->ImplFileLine());
   return(1);
}

static int G__AMSSiHitReader_Class_Version_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSSiHitReader*)(G__getstructoffset()))->Class_Version());
   return(1);
}

static int G__AMSSiHitReader_Class_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSSiHitReader*)(G__getstructoffset()))->Class());
   return(1);
}

static int G__AMSSiHitReader_Dictionary_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSSiHitReader*)(G__getstructoffset()))->Dictionary();
   return(1);
}

static int G__AMSSiHitReader_IsA_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSSiHitReader*)(G__getstructoffset()))->IsA());
   return(1);
}

static int G__AMSSiHitReader_ShowMembers_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSSiHitReader*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1);
}

static int G__AMSSiHitReader_Streamer_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSSiHitReader*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1);
}

// automatic copy constructor
static int G__AMSSiHitReader_AMSSiHitReader_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSSiHitReader *p;
   if(1!=libp->paran) ;
   p=new AMSSiHitReader(*(AMSSiHitReader*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSSiHitReader);
   return(1);
}

// automatic destructor
static int G__AMSSiHitReader_wAAMSSiHitReader_2_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSSiHitReader *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSSiHitReader *)((G__getstructoffset())+sizeof(AMSSiHitReader)*i))->~AMSSiHitReader();
   else if(G__PVOID==G__getgvp()) delete (AMSSiHitReader *)(G__getstructoffset());
   else ((AMSSiHitReader *)(G__getstructoffset()))->~AMSSiHitReader();
   return(1);
}


/* AMSCTCClusterReader */
static int G__AMSCTCClusterReader_AMSCTCClusterReader_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSCTCClusterReader *p;
   if(G__getaryconstruct()) p=new AMSCTCClusterReader[G__getaryconstruct()];
   else                    p=new AMSCTCClusterReader;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSCTCClusterReader);
   return(1);
}

static int G__AMSCTCClusterReader_AMSCTCClusterReader_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSCTCClusterReader *p;
      p = new AMSCTCClusterReader((const char*)G__int(libp->para[0]),(const char*)G__int(libp->para[1]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSCTCClusterReader);
   return(1);
}

static int G__AMSCTCClusterReader_Init_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSCTCClusterReader*)(G__getstructoffset()))->Init((TTree*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSCTCClusterReader*)(G__getstructoffset()))->Init();
      break;
   }
   return(1);
}

static int G__AMSCTCClusterReader_Finish_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSCTCClusterReader*)(G__getstructoffset()))->Finish();
   return(1);
}

static int G__AMSCTCClusterReader_Make_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSCTCClusterReader*)(G__getstructoffset()))->Make();
   return(1);
}

static int G__AMSCTCClusterReader_PrintInfo_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSCTCClusterReader*)(G__getstructoffset()))->PrintInfo();
   return(1);
}

static int G__AMSCTCClusterReader_AddCluster_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 9:
      G__setnull(result7);
      ((AMSCTCClusterReader*)(G__getstructoffset()))->AddCluster(
(Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Float_t)G__double(libp->para[2]),(Float_t)G__double(libp->para[3])
,(Float_t)G__double(libp->para[4]),(Float_t*)G__int(libp->para[5])
,(Float_t*)G__int(libp->para[6]),(Int_t)G__int(libp->para[7])
,(TObjArray*)G__int(libp->para[8]));
      break;
   case 8:
      G__setnull(result7);
      ((AMSCTCClusterReader*)(G__getstructoffset()))->AddCluster(
(Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Float_t)G__double(libp->para[2]),(Float_t)G__double(libp->para[3])
,(Float_t)G__double(libp->para[4]),(Float_t*)G__int(libp->para[5])
,(Float_t*)G__int(libp->para[6]),(Int_t)G__int(libp->para[7]));
      break;
   case 7:
      G__setnull(result7);
      ((AMSCTCClusterReader*)(G__getstructoffset()))->AddCluster(
(Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Float_t)G__double(libp->para[2]),(Float_t)G__double(libp->para[3])
,(Float_t)G__double(libp->para[4]),(Float_t*)G__int(libp->para[5])
,(Float_t*)G__int(libp->para[6]));
      break;
   }
   return(1);
}

static int G__AMSCTCClusterReader_RemoveCluster_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSCTCClusterReader*)(G__getstructoffset()))->RemoveCluster((const Int_t)G__int(libp->para[0]));
   return(1);
}

static int G__AMSCTCClusterReader_Clear_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSCTCClusterReader*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSCTCClusterReader*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1);
}

static int G__AMSCTCClusterReader_DeclFileName_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSCTCClusterReader*)(G__getstructoffset()))->DeclFileName());
   return(1);
}

static int G__AMSCTCClusterReader_DeclFileLine_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSCTCClusterReader*)(G__getstructoffset()))->DeclFileLine());
   return(1);
}

static int G__AMSCTCClusterReader_ImplFileName_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSCTCClusterReader*)(G__getstructoffset()))->ImplFileName());
   return(1);
}

static int G__AMSCTCClusterReader_ImplFileLine_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSCTCClusterReader*)(G__getstructoffset()))->ImplFileLine());
   return(1);
}

static int G__AMSCTCClusterReader_Class_Version_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSCTCClusterReader*)(G__getstructoffset()))->Class_Version());
   return(1);
}

static int G__AMSCTCClusterReader_Class_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSCTCClusterReader*)(G__getstructoffset()))->Class());
   return(1);
}

static int G__AMSCTCClusterReader_Dictionary_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSCTCClusterReader*)(G__getstructoffset()))->Dictionary();
   return(1);
}

static int G__AMSCTCClusterReader_IsA_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSCTCClusterReader*)(G__getstructoffset()))->IsA());
   return(1);
}

static int G__AMSCTCClusterReader_ShowMembers_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSCTCClusterReader*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1);
}

static int G__AMSCTCClusterReader_Streamer_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSCTCClusterReader*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1);
}

// automatic copy constructor
static int G__AMSCTCClusterReader_AMSCTCClusterReader_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSCTCClusterReader *p;
   if(1!=libp->paran) ;
   p=new AMSCTCClusterReader(*(AMSCTCClusterReader*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSCTCClusterReader);
   return(1);
}

// automatic destructor
static int G__AMSCTCClusterReader_wAAMSCTCClusterReader_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSCTCClusterReader *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSCTCClusterReader *)((G__getstructoffset())+sizeof(AMSCTCClusterReader)*i))->~AMSCTCClusterReader();
   else if(G__PVOID==G__getgvp()) delete (AMSCTCClusterReader *)(G__getstructoffset());
   else ((AMSCTCClusterReader *)(G__getstructoffset()))->~AMSCTCClusterReader();
   return(1);
}


/* AMSAntiClusterReader */
static int G__AMSAntiClusterReader_AMSAntiClusterReader_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSAntiClusterReader *p;
   if(G__getaryconstruct()) p=new AMSAntiClusterReader[G__getaryconstruct()];
   else                    p=new AMSAntiClusterReader;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSAntiClusterReader);
   return(1);
}

static int G__AMSAntiClusterReader_AMSAntiClusterReader_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSAntiClusterReader *p;
      p = new AMSAntiClusterReader((const char*)G__int(libp->para[0]),(const char*)G__int(libp->para[1]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSAntiClusterReader);
   return(1);
}

static int G__AMSAntiClusterReader_Init_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSAntiClusterReader*)(G__getstructoffset()))->Init((TTree*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSAntiClusterReader*)(G__getstructoffset()))->Init();
      break;
   }
   return(1);
}

static int G__AMSAntiClusterReader_Finish_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSAntiClusterReader*)(G__getstructoffset()))->Finish();
   return(1);
}

static int G__AMSAntiClusterReader_Make_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSAntiClusterReader*)(G__getstructoffset()))->Make();
   return(1);
}

static int G__AMSAntiClusterReader_PrintInfo_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSAntiClusterReader*)(G__getstructoffset()))->PrintInfo();
   return(1);
}

static int G__AMSAntiClusterReader_AddCluster_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 7:
      G__setnull(result7);
      ((AMSAntiClusterReader*)(G__getstructoffset()))->AddCluster(
(Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Float_t)G__double(libp->para[2]),(Float_t*)G__int(libp->para[3])
,(Float_t*)G__int(libp->para[4]),(Int_t)G__int(libp->para[5])
,(TObjArray*)G__int(libp->para[6]));
      break;
   case 6:
      G__setnull(result7);
      ((AMSAntiClusterReader*)(G__getstructoffset()))->AddCluster((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Float_t)G__double(libp->para[2]),(Float_t*)G__int(libp->para[3])
,(Float_t*)G__int(libp->para[4]),(Int_t)G__int(libp->para[5]));
      break;
   case 5:
      G__setnull(result7);
      ((AMSAntiClusterReader*)(G__getstructoffset()))->AddCluster((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Float_t)G__double(libp->para[2]),(Float_t*)G__int(libp->para[3])
,(Float_t*)G__int(libp->para[4]));
      break;
   }
   return(1);
}

static int G__AMSAntiClusterReader_RemoveCluster_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSAntiClusterReader*)(G__getstructoffset()))->RemoveCluster((const Int_t)G__int(libp->para[0]));
   return(1);
}

static int G__AMSAntiClusterReader_Clear_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSAntiClusterReader*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSAntiClusterReader*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1);
}

static int G__AMSAntiClusterReader_DeclFileName_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSAntiClusterReader*)(G__getstructoffset()))->DeclFileName());
   return(1);
}

static int G__AMSAntiClusterReader_DeclFileLine_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSAntiClusterReader*)(G__getstructoffset()))->DeclFileLine());
   return(1);
}

static int G__AMSAntiClusterReader_ImplFileName_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSAntiClusterReader*)(G__getstructoffset()))->ImplFileName());
   return(1);
}

static int G__AMSAntiClusterReader_ImplFileLine_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSAntiClusterReader*)(G__getstructoffset()))->ImplFileLine());
   return(1);
}

static int G__AMSAntiClusterReader_Class_Version_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSAntiClusterReader*)(G__getstructoffset()))->Class_Version());
   return(1);
}

static int G__AMSAntiClusterReader_Class_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSAntiClusterReader*)(G__getstructoffset()))->Class());
   return(1);
}

static int G__AMSAntiClusterReader_Dictionary_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSAntiClusterReader*)(G__getstructoffset()))->Dictionary();
   return(1);
}

static int G__AMSAntiClusterReader_IsA_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSAntiClusterReader*)(G__getstructoffset()))->IsA());
   return(1);
}

static int G__AMSAntiClusterReader_ShowMembers_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSAntiClusterReader*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1);
}

static int G__AMSAntiClusterReader_Streamer_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSAntiClusterReader*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1);
}

// automatic copy constructor
static int G__AMSAntiClusterReader_AMSAntiClusterReader_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSAntiClusterReader *p;
   if(1!=libp->paran) ;
   p=new AMSAntiClusterReader(*(AMSAntiClusterReader*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSAntiClusterReader);
   return(1);
}

// automatic destructor
static int G__AMSAntiClusterReader_wAAMSAntiClusterReader_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSAntiClusterReader *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSAntiClusterReader *)((G__getstructoffset())+sizeof(AMSAntiClusterReader)*i))->~AMSAntiClusterReader();
   else if(G__PVOID==G__getgvp()) delete (AMSAntiClusterReader *)(G__getstructoffset());
   else ((AMSAntiClusterReader *)(G__getstructoffset()))->~AMSAntiClusterReader();
   return(1);
}


/* AMSParticleReader */
static int G__AMSParticleReader_AMSParticleReader_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSParticleReader *p;
   if(G__getaryconstruct()) p=new AMSParticleReader[G__getaryconstruct()];
   else                    p=new AMSParticleReader;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSParticleReader);
   return(1);
}

static int G__AMSParticleReader_AMSParticleReader_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSParticleReader *p;
      p = new AMSParticleReader((const char*)G__int(libp->para[0]),(const char*)G__int(libp->para[1]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSParticleReader);
   return(1);
}

static int G__AMSParticleReader_AddParticle_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSParticleReader*)(G__getstructoffset()))->AddParticle((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1);
}

static int G__AMSParticleReader_Clear_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSParticleReader*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSParticleReader*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1);
}

static int G__AMSParticleReader_Finish_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSParticleReader*)(G__getstructoffset()))->Finish();
   return(1);
}

static int G__AMSParticleReader_Init_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSParticleReader*)(G__getstructoffset()))->Init((TTree*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSParticleReader*)(G__getstructoffset()))->Init();
      break;
   }
   return(1);
}

static int G__AMSParticleReader_Make_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSParticleReader*)(G__getstructoffset()))->Make();
   return(1);
}

static int G__AMSParticleReader_PrintInfo_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSParticleReader*)(G__getstructoffset()))->PrintInfo();
   return(1);
}

static int G__AMSParticleReader_Mult_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSParticleReader*)(G__getstructoffset()))->Mult());
   return(1);
}

static int G__AMSParticleReader_DeclFileName_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSParticleReader*)(G__getstructoffset()))->DeclFileName());
   return(1);
}

static int G__AMSParticleReader_DeclFileLine_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSParticleReader*)(G__getstructoffset()))->DeclFileLine());
   return(1);
}

static int G__AMSParticleReader_ImplFileName_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSParticleReader*)(G__getstructoffset()))->ImplFileName());
   return(1);
}

static int G__AMSParticleReader_ImplFileLine_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSParticleReader*)(G__getstructoffset()))->ImplFileLine());
   return(1);
}

static int G__AMSParticleReader_Class_Version_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSParticleReader*)(G__getstructoffset()))->Class_Version());
   return(1);
}

static int G__AMSParticleReader_Class_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSParticleReader*)(G__getstructoffset()))->Class());
   return(1);
}

static int G__AMSParticleReader_Dictionary_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSParticleReader*)(G__getstructoffset()))->Dictionary();
   return(1);
}

static int G__AMSParticleReader_IsA_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSParticleReader*)(G__getstructoffset()))->IsA());
   return(1);
}

static int G__AMSParticleReader_ShowMembers_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSParticleReader*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1);
}

static int G__AMSParticleReader_Streamer_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSParticleReader*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1);
}

// automatic copy constructor
static int G__AMSParticleReader_AMSParticleReader_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSParticleReader *p;
   if(1!=libp->paran) ;
   p=new AMSParticleReader(*(AMSParticleReader*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSParticleReader);
   return(1);
}

// automatic destructor
static int G__AMSParticleReader_wAAMSParticleReader_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSParticleReader *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSParticleReader *)((G__getstructoffset())+sizeof(AMSParticleReader)*i))->~AMSParticleReader();
   else if(G__PVOID==G__getgvp()) delete (AMSParticleReader *)(G__getstructoffset());
   else ((AMSParticleReader *)(G__getstructoffset()))->~AMSParticleReader();
   return(1);
}


/* AMSVirtualDisplay */
static int G__AMSVirtualDisplay_Clear_2_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSVirtualDisplay*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSVirtualDisplay*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1);
}

static int G__AMSVirtualDisplay_DisplayButtons_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSVirtualDisplay*)(G__getstructoffset()))->DisplayButtons();
   return(1);
}

static int G__AMSVirtualDisplay_DistancetoPrimitive_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSVirtualDisplay*)(G__getstructoffset()))->DistancetoPrimitive((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1);
}

static int G__AMSVirtualDisplay_Draw_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSVirtualDisplay*)(G__getstructoffset()))->Draw((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSVirtualDisplay*)(G__getstructoffset()))->Draw();
      break;
   }
   return(1);
}

static int G__AMSVirtualDisplay_DrawAllViews_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSVirtualDisplay*)(G__getstructoffset()))->DrawAllViews();
   return(1);
}

static int G__AMSVirtualDisplay_DrawFrontAndSideViews_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSVirtualDisplay*)(G__getstructoffset()))->DrawFrontAndSideViews();
   return(1);
}

static int G__AMSVirtualDisplay_DrawParticles_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((AMSVirtualDisplay*)(G__getstructoffset()))->DrawParticles());
   return(1);
}

static int G__AMSVirtualDisplay_DrawCaption_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSVirtualDisplay*)(G__getstructoffset()))->DrawCaption((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSVirtualDisplay*)(G__getstructoffset()))->DrawCaption();
      break;
   }
   return(1);
}

static int G__AMSVirtualDisplay_DrawView_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSVirtualDisplay*)(G__getstructoffset()))->DrawView((Float_t)G__double(libp->para[0]),(Float_t)G__double(libp->para[1])
,(Int_t)G__int(libp->para[2]));
   return(1);
}

static int G__AMSVirtualDisplay_DrawViewGL_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSVirtualDisplay*)(G__getstructoffset()))->DrawViewGL();
   return(1);
}

static int G__AMSVirtualDisplay_DrawViewX3D_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSVirtualDisplay*)(G__getstructoffset()))->DrawViewX3D();
   return(1);
}

static int G__AMSVirtualDisplay_ExecuteEvent_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSVirtualDisplay*)(G__getstructoffset()))->ExecuteEvent((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]));
   return(1);
}

static int G__AMSVirtualDisplay_Paint_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSVirtualDisplay*)(G__getstructoffset()))->Paint((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSVirtualDisplay*)(G__getstructoffset()))->Paint();
      break;
   }
   return(1);
}

static int G__AMSVirtualDisplay_PaintParticles_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSVirtualDisplay*)(G__getstructoffset()))->PaintParticles((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSVirtualDisplay*)(G__getstructoffset()))->PaintParticles();
      break;
   }
   return(1);
}

static int G__AMSVirtualDisplay_PTcut_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letdouble(result7,102,(double)((AMSVirtualDisplay*)(G__getstructoffset()))->PTcut());
   return(1);
}

static int G__AMSVirtualDisplay_PTcutEGMUNU_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letdouble(result7,102,(double)((AMSVirtualDisplay*)(G__getstructoffset()))->PTcutEGMUNU());
   return(1);
}

static int G__AMSVirtualDisplay_SetView_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSVirtualDisplay*)(G__getstructoffset()))->SetView((Float_t)G__double(libp->para[0]),(Float_t)G__double(libp->para[1]));
   return(1);
}

static int G__AMSVirtualDisplay_SetView_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSVirtualDisplay*)(G__getstructoffset()))->SetView((EAMSView)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSVirtualDisplay*)(G__getstructoffset()))->SetView();
      break;
   }
   return(1);
}

static int G__AMSVirtualDisplay_ShowNextEvent_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSVirtualDisplay*)(G__getstructoffset()))->ShowNextEvent((Int_t)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSVirtualDisplay*)(G__getstructoffset()))->ShowNextEvent();
      break;
   }
   return(1);
}

static int G__AMSVirtualDisplay_SizeParticles_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSVirtualDisplay*)(G__getstructoffset()))->SizeParticles();
   return(1);
}

static int G__AMSVirtualDisplay_DeclFileName_2_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSVirtualDisplay*)(G__getstructoffset()))->DeclFileName());
   return(1);
}

static int G__AMSVirtualDisplay_DeclFileLine_3_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSVirtualDisplay*)(G__getstructoffset()))->DeclFileLine());
   return(1);
}

static int G__AMSVirtualDisplay_ImplFileName_4_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSVirtualDisplay*)(G__getstructoffset()))->ImplFileName());
   return(1);
}

static int G__AMSVirtualDisplay_ImplFileLine_5_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSVirtualDisplay*)(G__getstructoffset()))->ImplFileLine());
   return(1);
}

static int G__AMSVirtualDisplay_Class_Version_6_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSVirtualDisplay*)(G__getstructoffset()))->Class_Version());
   return(1);
}

static int G__AMSVirtualDisplay_Class_7_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSVirtualDisplay*)(G__getstructoffset()))->Class());
   return(1);
}

static int G__AMSVirtualDisplay_Dictionary_8_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSVirtualDisplay*)(G__getstructoffset()))->Dictionary();
   return(1);
}

static int G__AMSVirtualDisplay_IsA_9_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSVirtualDisplay*)(G__getstructoffset()))->IsA());
   return(1);
}

static int G__AMSVirtualDisplay_ShowMembers_0_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSVirtualDisplay*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1);
}

static int G__AMSVirtualDisplay_Streamer_1_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSVirtualDisplay*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1);
}

// automatic destructor
static int G__AMSVirtualDisplay_wAAMSVirtualDisplay_2_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSVirtualDisplay *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSVirtualDisplay *)((G__getstructoffset())+sizeof(AMSVirtualDisplay)*i))->~AMSVirtualDisplay();
   else if(G__PVOID==G__getgvp()) delete (AMSVirtualDisplay *)(G__getstructoffset());
   else ((AMSVirtualDisplay *)(G__getstructoffset()))->~AMSVirtualDisplay();
   return(1);
}


/* AMSRoot */
static int G__AMSRoot_AMSRoot_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSRoot *p;
   if(G__getaryconstruct()) p=new AMSRoot[G__getaryconstruct()];
   else                    p=new AMSRoot;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSRoot);
   return(1);
}

static int G__AMSRoot_AMSRoot_2_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSRoot *p;
   switch(libp->paran) {
   case 2:
      p = new AMSRoot((const char*)G__int(libp->para[0]),(const char*)G__int(libp->para[1]));
      break;
   case 1:
      p = new AMSRoot((const char*)G__int(libp->para[0]));
      break;
   }
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSRoot);
   return(1);
}

static int G__AMSRoot_Browse_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->Browse((TBrowser*)G__int(libp->para[0]));
   return(1);
}

static int G__AMSRoot_GetEvent_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->GetEvent((Int_t)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->GetEvent();
      break;
   }
   return(1);
}

static int G__AMSRoot_SelectEvent_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->SelectEvent();
   return(1);
}

static int G__AMSRoot_Init_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->Init((TTree*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->Init();
      break;
   }
   return(1);
}

static int G__AMSRoot_Finish_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->Finish();
   return(1);
}

static int G__AMSRoot_Display_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSRoot*)(G__getstructoffset()))->Display());
   return(1);
}

static int G__AMSRoot_Draw_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->Draw((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->Draw();
      break;
   }
   return(1);
}

static int G__AMSRoot_Paint_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->Paint((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->Paint();
      break;
   }
   return(1);
}

static int G__AMSRoot_Clear_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1);
}

static int G__AMSRoot_IsFolder_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((AMSRoot*)(G__getstructoffset()))->IsFolder());
   return(1);
}

static int G__AMSRoot_PrintInfo_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->PrintInfo();
   return(1);
}

static int G__AMSRoot_GetVersion_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSRoot*)(G__getstructoffset()))->GetVersion());
   return(1);
}

static int G__AMSRoot_GetVersionDate_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSRoot*)(G__getstructoffset()))->GetVersionDate());
   return(1);
}

static int G__AMSRoot_RunNum_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSRoot*)(G__getstructoffset()))->RunNum());
   return(1);
}

static int G__AMSRoot_EventNum_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSRoot*)(G__getstructoffset()))->EventNum());
   return(1);
}

static int G__AMSRoot_Event_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSRoot*)(G__getstructoffset()))->Event());
   return(1);
}

static int G__AMSRoot_Mode_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSRoot*)(G__getstructoffset()))->Mode());
   return(1);
}

static int G__AMSRoot_Tree_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSRoot*)(G__getstructoffset()))->Tree());
   return(1);
}

static int G__AMSRoot_GetInput_2_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSRoot*)(G__getstructoffset()))->GetInput());
   return(1);
}

static int G__AMSRoot_Makers_3_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSRoot*)(G__getstructoffset()))->Makers());
   return(1);
}

static int G__AMSRoot_Maker_4_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSRoot*)(G__getstructoffset()))->Maker((const char*)G__int(libp->para[0])));
   return(1);
}

static int G__AMSRoot_ToFClusterMaker_5_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSRoot*)(G__getstructoffset()))->ToFClusterMaker());
   return(1);
}

static int G__AMSRoot_TrackMaker_6_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSRoot*)(G__getstructoffset()))->TrackMaker());
   return(1);
}

static int G__AMSRoot_SiHitMaker_7_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSRoot*)(G__getstructoffset()))->SiHitMaker());
   return(1);
}

static int G__AMSRoot_AntiClusterMaker_8_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSRoot*)(G__getstructoffset()))->AntiClusterMaker());
   return(1);
}

static int G__AMSRoot_CTCClusterMaker_9_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSRoot*)(G__getstructoffset()))->CTCClusterMaker());
   return(1);
}

static int G__AMSRoot_ParticleMaker_0_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSRoot*)(G__getstructoffset()))->ParticleMaker());
   return(1);
}

static int G__AMSRoot_SetDefaultParameters_1_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->SetDefaultParameters();
   return(1);
}

static int G__AMSRoot_SetEvent_2_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->SetEvent((Int_t)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->SetEvent();
      break;
   }
   return(1);
}

static int G__AMSRoot_SetMode_3_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->SetMode((Int_t)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->SetMode();
      break;
   }
   return(1);
}

static int G__AMSRoot_SetDisplay_4_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->SetDisplay((AMSVirtualDisplay*)G__int(libp->para[0]));
   return(1);
}

static int G__AMSRoot_Make_5_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->Make((Int_t)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->Make();
      break;
   }
   return(1);
}

static int G__AMSRoot_MakeTree_6_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 2:
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->MakeTree((const char*)G__int(libp->para[0]),(const char*)G__int(libp->para[1]));
      break;
   case 1:
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->MakeTree((const char*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->MakeTree();
      break;
   }
   return(1);
}

static int G__AMSRoot_FillTree_7_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->FillTree();
   return(1);
}

static int G__AMSRoot_SortDown_8_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 4:
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->SortDown((Int_t)G__int(libp->para[0]),(Float_t*)G__int(libp->para[1])
,(Int_t*)G__int(libp->para[2]),(Bool_t)G__int(libp->para[3]));
      break;
   case 3:
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->SortDown((Int_t)G__int(libp->para[0]),(Float_t*)G__int(libp->para[1])
,(Int_t*)G__int(libp->para[2]));
      break;
   }
   return(1);
}

static int G__AMSRoot_DeclFileName_9_3(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSRoot*)(G__getstructoffset()))->DeclFileName());
   return(1);
}

static int G__AMSRoot_DeclFileLine_0_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSRoot*)(G__getstructoffset()))->DeclFileLine());
   return(1);
}

static int G__AMSRoot_ImplFileName_1_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSRoot*)(G__getstructoffset()))->ImplFileName());
   return(1);
}

static int G__AMSRoot_ImplFileLine_2_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSRoot*)(G__getstructoffset()))->ImplFileLine());
   return(1);
}

static int G__AMSRoot_Class_Version_3_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSRoot*)(G__getstructoffset()))->Class_Version());
   return(1);
}

static int G__AMSRoot_Class_4_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSRoot*)(G__getstructoffset()))->Class());
   return(1);
}

static int G__AMSRoot_Dictionary_5_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->Dictionary();
   return(1);
}

static int G__AMSRoot_IsA_6_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSRoot*)(G__getstructoffset()))->IsA());
   return(1);
}

static int G__AMSRoot_ShowMembers_7_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1);
}

static int G__AMSRoot_Streamer_8_4(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSRoot*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1);
}

// automatic copy constructor
static int G__AMSRoot_AMSRoot_9_4(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSRoot *p;
   if(1!=libp->paran) ;
   p=new AMSRoot(*(AMSRoot*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSRoot);
   return(1);
}

// automatic destructor
static int G__AMSRoot_wAAMSRoot_0_5(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSRoot *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSRoot *)((G__getstructoffset())+sizeof(AMSRoot)*i))->~AMSRoot();
   else if(G__PVOID==G__getgvp()) delete (AMSRoot *)(G__getstructoffset());
   else ((AMSRoot *)(G__getstructoffset()))->~AMSRoot();
   return(1);
}


/* AMSToFCluster */
static int G__AMSToFCluster_AMSToFCluster_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSToFCluster *p;
   if(G__getaryconstruct()) p=new AMSToFCluster[G__getaryconstruct()];
   else                    p=new AMSToFCluster;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSToFCluster);
   return(1);
}

static int G__AMSToFCluster_AMSToFCluster_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSToFCluster *p;
      p = new AMSToFCluster(
(Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]),(Float_t)G__double(libp->para[3])
,(Float_t)G__double(libp->para[4]),(Float_t)G__double(libp->para[5])
,(Float_t*)G__int(libp->para[6]),(Float_t*)G__int(libp->para[7])
,(Int_t)G__int(libp->para[8]),(Int_t)G__int(libp->para[9])
,(Int_t)G__int(libp->para[10]),(TObjArray*)G__int(libp->para[11]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSToFCluster);
   return(1);
}

static int G__AMSToFCluster_GetObjectInfo_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSToFCluster*)(G__getstructoffset()))->GetObjectInfo((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1);
}

static int G__AMSToFCluster_Paint_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSToFCluster*)(G__getstructoffset()))->Paint((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSToFCluster*)(G__getstructoffset()))->Paint();
      break;
   }
   return(1);
}

static int G__AMSToFCluster_Clear_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSToFCluster*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSToFCluster*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1);
}

static int G__AMSToFCluster_GetStatus_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSToFCluster*)(G__getstructoffset()))->GetStatus());
   return(1);
}

static int G__AMSToFCluster_GetPlane_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSToFCluster*)(G__getstructoffset()))->GetPlane());
   return(1);
}

static int G__AMSToFCluster_GetBar_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSToFCluster*)(G__getstructoffset()))->GetBar());
   return(1);
}

static int G__AMSToFCluster_GetEnergy_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letdouble(result7,102,(double)((AMSToFCluster*)(G__getstructoffset()))->GetEnergy());
   return(1);
}

static int G__AMSToFCluster_GetTime_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letdouble(result7,102,(double)((AMSToFCluster*)(G__getstructoffset()))->GetTime());
   return(1);
}

static int G__AMSToFCluster_GetNcells_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSToFCluster*)(G__getstructoffset()))->GetNcells());
   return(1);
}

static int G__AMSToFCluster_GetNparticles_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSToFCluster*)(G__getstructoffset()))->GetNparticles());
   return(1);
}

static int G__AMSToFCluster_GetNTracks_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSToFCluster*)(G__getstructoffset()))->GetNTracks());
   return(1);
}

static int G__AMSToFCluster_GetTracks_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSToFCluster*)(G__getstructoffset()))->GetTracks());
   return(1);
}

static int G__AMSToFCluster_Is3D_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((AMSToFCluster*)(G__getstructoffset()))->Is3D());
   return(1);
}

static int G__AMSToFCluster_SetEnergy_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSToFCluster*)(G__getstructoffset()))->SetEnergy((Float_t)G__double(libp->para[0]));
   return(1);
}

static int G__AMSToFCluster_SetTime_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSToFCluster*)(G__getstructoffset()))->SetTime((Float_t)G__double(libp->para[0]));
   return(1);
}

static int G__AMSToFCluster_DeclFileName_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSToFCluster*)(G__getstructoffset()))->DeclFileName());
   return(1);
}

static int G__AMSToFCluster_DeclFileLine_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSToFCluster*)(G__getstructoffset()))->DeclFileLine());
   return(1);
}

static int G__AMSToFCluster_ImplFileName_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSToFCluster*)(G__getstructoffset()))->ImplFileName());
   return(1);
}

static int G__AMSToFCluster_ImplFileLine_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSToFCluster*)(G__getstructoffset()))->ImplFileLine());
   return(1);
}

static int G__AMSToFCluster_Class_Version_2_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSToFCluster*)(G__getstructoffset()))->Class_Version());
   return(1);
}

static int G__AMSToFCluster_Class_3_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSToFCluster*)(G__getstructoffset()))->Class());
   return(1);
}

static int G__AMSToFCluster_Dictionary_4_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSToFCluster*)(G__getstructoffset()))->Dictionary();
   return(1);
}

static int G__AMSToFCluster_IsA_5_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSToFCluster*)(G__getstructoffset()))->IsA());
   return(1);
}

static int G__AMSToFCluster_ShowMembers_6_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSToFCluster*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1);
}

static int G__AMSToFCluster_Streamer_7_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSToFCluster*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1);
}

// automatic copy constructor
static int G__AMSToFCluster_AMSToFCluster_8_2(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSToFCluster *p;
   if(1!=libp->paran) ;
   p=new AMSToFCluster(*(AMSToFCluster*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSToFCluster);
   return(1);
}

// automatic destructor
static int G__AMSToFCluster_wAAMSToFCluster_9_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSToFCluster *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSToFCluster *)((G__getstructoffset())+sizeof(AMSToFCluster)*i))->~AMSToFCluster();
   else if(G__PVOID==G__getgvp()) delete (AMSToFCluster *)(G__getstructoffset());
   else ((AMSToFCluster *)(G__getstructoffset()))->~AMSToFCluster();
   return(1);
}


/* AMSAntiCluster */
static int G__AMSAntiCluster_AMSAntiCluster_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSAntiCluster *p;
   if(G__getaryconstruct()) p=new AMSAntiCluster[G__getaryconstruct()];
   else                    p=new AMSAntiCluster;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSAntiCluster);
   return(1);
}

static int G__AMSAntiCluster_AMSAntiCluster_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSAntiCluster *p;
      p = new AMSAntiCluster(
(Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Float_t)G__double(libp->para[2]),(Float_t*)G__int(libp->para[3])
,(Float_t*)G__int(libp->para[4]),(Int_t)G__int(libp->para[5])
,(TObjArray*)G__int(libp->para[6]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSAntiCluster);
   return(1);
}

static int G__AMSAntiCluster_GetObjectInfo_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSAntiCluster*)(G__getstructoffset()))->GetObjectInfo((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1);
}

static int G__AMSAntiCluster_Paint_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSAntiCluster*)(G__getstructoffset()))->Paint((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSAntiCluster*)(G__getstructoffset()))->Paint();
      break;
   }
   return(1);
}

static int G__AMSAntiCluster_Clear_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSAntiCluster*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSAntiCluster*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1);
}

static int G__AMSAntiCluster_GetStatus_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSAntiCluster*)(G__getstructoffset()))->GetStatus());
   return(1);
}

static int G__AMSAntiCluster_GetSector_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSAntiCluster*)(G__getstructoffset()))->GetSector());
   return(1);
}

static int G__AMSAntiCluster_GetSignal_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letdouble(result7,102,(double)((AMSAntiCluster*)(G__getstructoffset()))->GetSignal());
   return(1);
}

static int G__AMSAntiCluster_GetTracks_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSAntiCluster*)(G__getstructoffset()))->GetTracks());
   return(1);
}

static int G__AMSAntiCluster_Is3D_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((AMSAntiCluster*)(G__getstructoffset()))->Is3D());
   return(1);
}

static int G__AMSAntiCluster_DeclFileName_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSAntiCluster*)(G__getstructoffset()))->DeclFileName());
   return(1);
}

static int G__AMSAntiCluster_DeclFileLine_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSAntiCluster*)(G__getstructoffset()))->DeclFileLine());
   return(1);
}

static int G__AMSAntiCluster_ImplFileName_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSAntiCluster*)(G__getstructoffset()))->ImplFileName());
   return(1);
}

static int G__AMSAntiCluster_ImplFileLine_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSAntiCluster*)(G__getstructoffset()))->ImplFileLine());
   return(1);
}

static int G__AMSAntiCluster_Class_Version_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSAntiCluster*)(G__getstructoffset()))->Class_Version());
   return(1);
}

static int G__AMSAntiCluster_Class_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSAntiCluster*)(G__getstructoffset()))->Class());
   return(1);
}

static int G__AMSAntiCluster_Dictionary_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSAntiCluster*)(G__getstructoffset()))->Dictionary();
   return(1);
}

static int G__AMSAntiCluster_IsA_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSAntiCluster*)(G__getstructoffset()))->IsA());
   return(1);
}

static int G__AMSAntiCluster_ShowMembers_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSAntiCluster*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1);
}

static int G__AMSAntiCluster_Streamer_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSAntiCluster*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1);
}

// automatic copy constructor
static int G__AMSAntiCluster_AMSAntiCluster_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSAntiCluster *p;
   if(1!=libp->paran) ;
   p=new AMSAntiCluster(*(AMSAntiCluster*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSAntiCluster);
   return(1);
}

// automatic destructor
static int G__AMSAntiCluster_wAAMSAntiCluster_2_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSAntiCluster *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSAntiCluster *)((G__getstructoffset())+sizeof(AMSAntiCluster)*i))->~AMSAntiCluster();
   else if(G__PVOID==G__getgvp()) delete (AMSAntiCluster *)(G__getstructoffset());
   else ((AMSAntiCluster *)(G__getstructoffset()))->~AMSAntiCluster();
   return(1);
}


/* AMSTrack */
static int G__AMSTrack_AMSTrack_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSTrack *p;
   if(G__getaryconstruct()) p=new AMSTrack[G__getaryconstruct()];
   else                    p=new AMSTrack;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSTrack);
   return(1);
}

static int G__AMSTrack_AMSTrack_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSTrack *p;
      p = new AMSTrack((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSTrack);
   return(1);
}

static int G__AMSTrack_GetObjectInfo_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSTrack*)(G__getstructoffset()))->GetObjectInfo((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1);
}

static int G__AMSTrack_SetHelix_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSTrack*)(G__getstructoffset()))->SetHelix();
   return(1);
}

static int G__AMSTrack_Paint_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSTrack*)(G__getstructoffset()))->Paint((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSTrack*)(G__getstructoffset()))->Paint();
      break;
   }
   return(1);
}

static int G__AMSTrack_DeclFileName_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSTrack*)(G__getstructoffset()))->DeclFileName());
   return(1);
}

static int G__AMSTrack_DeclFileLine_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSTrack*)(G__getstructoffset()))->DeclFileLine());
   return(1);
}

static int G__AMSTrack_ImplFileName_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSTrack*)(G__getstructoffset()))->ImplFileName());
   return(1);
}

static int G__AMSTrack_ImplFileLine_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSTrack*)(G__getstructoffset()))->ImplFileLine());
   return(1);
}

static int G__AMSTrack_Class_Version_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSTrack*)(G__getstructoffset()))->Class_Version());
   return(1);
}

static int G__AMSTrack_Class_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSTrack*)(G__getstructoffset()))->Class());
   return(1);
}

static int G__AMSTrack_Dictionary_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSTrack*)(G__getstructoffset()))->Dictionary();
   return(1);
}

static int G__AMSTrack_IsA_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSTrack*)(G__getstructoffset()))->IsA());
   return(1);
}

static int G__AMSTrack_ShowMembers_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSTrack*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1);
}

static int G__AMSTrack_Streamer_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSTrack*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1);
}

// automatic copy constructor
static int G__AMSTrack_AMSTrack_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSTrack *p;
   if(1!=libp->paran) ;
   p=new AMSTrack(*(AMSTrack*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSTrack);
   return(1);
}

// automatic destructor
static int G__AMSTrack_wAAMSTrack_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSTrack *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSTrack *)((G__getstructoffset())+sizeof(AMSTrack)*i))->~AMSTrack();
   else if(G__PVOID==G__getgvp()) delete (AMSTrack *)(G__getstructoffset());
   else ((AMSTrack *)(G__getstructoffset()))->~AMSTrack();
   return(1);
}


/* AMSSiHit */
static int G__AMSSiHit_AMSSiHit_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSSiHit *p;
   if(G__getaryconstruct()) p=new AMSSiHit[G__getaryconstruct()];
   else                    p=new AMSSiHit;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSSiHit);
   return(1);
}

static int G__AMSSiHit_AMSSiHit_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSSiHit *p;
      p = new AMSSiHit(
(Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]),(Int_t)G__int(libp->para[3])
,(Float_t*)G__int(libp->para[4]),(Float_t*)G__int(libp->para[5])
,(Float_t)G__double(libp->para[6]),(Float_t)G__double(libp->para[7])
,(TObjArray*)G__int(libp->para[8]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSSiHit);
   return(1);
}

static int G__AMSSiHit_GetObjectInfo_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSSiHit*)(G__getstructoffset()))->GetObjectInfo((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1);
}

static int G__AMSSiHit_Paint_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSSiHit*)(G__getstructoffset()))->Paint((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSSiHit*)(G__getstructoffset()))->Paint();
      break;
   }
   return(1);
}

static int G__AMSSiHit_Clear_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSSiHit*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSSiHit*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1);
}

static int G__AMSSiHit_GetStatus_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSSiHit*)(G__getstructoffset()))->GetStatus());
   return(1);
}

static int G__AMSSiHit_GetPlane_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSSiHit*)(G__getstructoffset()))->GetPlane());
   return(1);
}

static int G__AMSSiHit_GetX_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSSiHit*)(G__getstructoffset()))->GetX());
   return(1);
}

static int G__AMSSiHit_GetY_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSSiHit*)(G__getstructoffset()))->GetY());
   return(1);
}

static int G__AMSSiHit_GetAmplitude_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letdouble(result7,102,(double)((AMSSiHit*)(G__getstructoffset()))->GetAmplitude());
   return(1);
}

static int G__AMSSiHit_GetAsymAmpl_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letdouble(result7,102,(double)((AMSSiHit*)(G__getstructoffset()))->GetAsymAmpl());
   return(1);
}

static int G__AMSSiHit_GetTracks_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSSiHit*)(G__getstructoffset()))->GetTracks());
   return(1);
}

static int G__AMSSiHit_Is3D_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((AMSSiHit*)(G__getstructoffset()))->Is3D());
   return(1);
}

static int G__AMSSiHit_DeclFileName_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSSiHit*)(G__getstructoffset()))->DeclFileName());
   return(1);
}

static int G__AMSSiHit_DeclFileLine_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSSiHit*)(G__getstructoffset()))->DeclFileLine());
   return(1);
}

static int G__AMSSiHit_ImplFileName_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSSiHit*)(G__getstructoffset()))->ImplFileName());
   return(1);
}

static int G__AMSSiHit_ImplFileLine_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSSiHit*)(G__getstructoffset()))->ImplFileLine());
   return(1);
}

static int G__AMSSiHit_Class_Version_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSSiHit*)(G__getstructoffset()))->Class_Version());
   return(1);
}

static int G__AMSSiHit_Class_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSSiHit*)(G__getstructoffset()))->Class());
   return(1);
}

static int G__AMSSiHit_Dictionary_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSSiHit*)(G__getstructoffset()))->Dictionary();
   return(1);
}

static int G__AMSSiHit_IsA_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSSiHit*)(G__getstructoffset()))->IsA());
   return(1);
}

static int G__AMSSiHit_ShowMembers_2_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSSiHit*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1);
}

static int G__AMSSiHit_Streamer_3_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSSiHit*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1);
}

// automatic copy constructor
static int G__AMSSiHit_AMSSiHit_4_2(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSSiHit *p;
   if(1!=libp->paran) ;
   p=new AMSSiHit(*(AMSSiHit*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSSiHit);
   return(1);
}

// automatic destructor
static int G__AMSSiHit_wAAMSSiHit_5_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSSiHit *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSSiHit *)((G__getstructoffset())+sizeof(AMSSiHit)*i))->~AMSSiHit();
   else if(G__PVOID==G__getgvp()) delete (AMSSiHit *)(G__getstructoffset());
   else ((AMSSiHit *)(G__getstructoffset()))->~AMSSiHit();
   return(1);
}


/* AMSCTCCluster */
static int G__AMSCTCCluster_AMSCTCCluster_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSCTCCluster *p;
   if(G__getaryconstruct()) p=new AMSCTCCluster[G__getaryconstruct()];
   else                    p=new AMSCTCCluster;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSCTCCluster);
   return(1);
}

static int G__AMSCTCCluster_AMSCTCCluster_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSCTCCluster *p;
      p = new AMSCTCCluster(
(Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]),(Float_t)G__double(libp->para[3])
,(Float_t)G__double(libp->para[4]),(Float_t*)G__int(libp->para[5])
,(Float_t*)G__int(libp->para[6]),(Int_t)G__int(libp->para[7])
,(TObjArray*)G__int(libp->para[8]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSCTCCluster);
   return(1);
}

static int G__AMSCTCCluster_GetObjectInfo_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSCTCCluster*)(G__getstructoffset()))->GetObjectInfo((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1);
}

static int G__AMSCTCCluster_Paint_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSCTCCluster*)(G__getstructoffset()))->Paint((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSCTCCluster*)(G__getstructoffset()))->Paint();
      break;
   }
   return(1);
}

static int G__AMSCTCCluster_Clear_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSCTCCluster*)(G__getstructoffset()))->Clear((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSCTCCluster*)(G__getstructoffset()))->Clear();
      break;
   }
   return(1);
}

static int G__AMSCTCCluster_GetStatus_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSCTCCluster*)(G__getstructoffset()))->GetStatus());
   return(1);
}

static int G__AMSCTCCluster_GetPlane_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSCTCCluster*)(G__getstructoffset()))->GetPlane());
   return(1);
}

static int G__AMSCTCCluster_GetRawSignal_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letdouble(result7,102,(double)((AMSCTCCluster*)(G__getstructoffset()))->GetRawSignal());
   return(1);
}

static int G__AMSCTCCluster_GetSignal_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letdouble(result7,102,(double)((AMSCTCCluster*)(G__getstructoffset()))->GetSignal());
   return(1);
}

static int G__AMSCTCCluster_GetErrSignal_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letdouble(result7,102,(double)((AMSCTCCluster*)(G__getstructoffset()))->GetErrSignal());
   return(1);
}

static int G__AMSCTCCluster_GetTracks_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSCTCCluster*)(G__getstructoffset()))->GetTracks());
   return(1);
}

static int G__AMSCTCCluster_Is3D_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,98,(long)((AMSCTCCluster*)(G__getstructoffset()))->Is3D());
   return(1);
}

static int G__AMSCTCCluster_DeclFileName_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSCTCCluster*)(G__getstructoffset()))->DeclFileName());
   return(1);
}

static int G__AMSCTCCluster_DeclFileLine_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSCTCCluster*)(G__getstructoffset()))->DeclFileLine());
   return(1);
}

static int G__AMSCTCCluster_ImplFileName_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSCTCCluster*)(G__getstructoffset()))->ImplFileName());
   return(1);
}

static int G__AMSCTCCluster_ImplFileLine_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSCTCCluster*)(G__getstructoffset()))->ImplFileLine());
   return(1);
}

static int G__AMSCTCCluster_Class_Version_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSCTCCluster*)(G__getstructoffset()))->Class_Version());
   return(1);
}

static int G__AMSCTCCluster_Class_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSCTCCluster*)(G__getstructoffset()))->Class());
   return(1);
}

static int G__AMSCTCCluster_Dictionary_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSCTCCluster*)(G__getstructoffset()))->Dictionary();
   return(1);
}

static int G__AMSCTCCluster_IsA_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSCTCCluster*)(G__getstructoffset()))->IsA());
   return(1);
}

static int G__AMSCTCCluster_ShowMembers_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSCTCCluster*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1);
}

static int G__AMSCTCCluster_Streamer_2_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSCTCCluster*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1);
}

// automatic copy constructor
static int G__AMSCTCCluster_AMSCTCCluster_3_2(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSCTCCluster *p;
   if(1!=libp->paran) ;
   p=new AMSCTCCluster(*(AMSCTCCluster*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSCTCCluster);
   return(1);
}

// automatic destructor
static int G__AMSCTCCluster_wAAMSCTCCluster_4_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSCTCCluster *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSCTCCluster *)((G__getstructoffset())+sizeof(AMSCTCCluster)*i))->~AMSCTCCluster();
   else if(G__PVOID==G__getgvp()) delete (AMSCTCCluster *)(G__getstructoffset());
   else ((AMSCTCCluster *)(G__getstructoffset()))->~AMSCTCCluster();
   return(1);
}


/* AMSParticle */
static int G__AMSParticle_AMSParticle_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSParticle *p;
   if(G__getaryconstruct()) p=new AMSParticle[G__getaryconstruct()];
   else                    p=new AMSParticle;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSParticle);
   return(1);
}

static int G__AMSParticle_GetObjectInfo_2_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSParticle*)(G__getstructoffset()))->GetObjectInfo((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])));
   return(1);
}

static int G__AMSParticle_SetHelix_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSParticle*)(G__getstructoffset()))->SetHelix();
   return(1);
}

static int G__AMSParticle_Paint_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   switch(libp->paran) {
   case 1:
      G__setnull(result7);
      ((AMSParticle*)(G__getstructoffset()))->Paint((Option_t*)G__int(libp->para[0]));
      break;
   case 0:
      G__setnull(result7);
      ((AMSParticle*)(G__getstructoffset()))->Paint();
      break;
   }
   return(1);
}

static int G__AMSParticle_DeclFileName_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSParticle*)(G__getstructoffset()))->DeclFileName());
   return(1);
}

static int G__AMSParticle_DeclFileLine_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSParticle*)(G__getstructoffset()))->DeclFileLine());
   return(1);
}

static int G__AMSParticle_ImplFileName_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSParticle*)(G__getstructoffset()))->ImplFileName());
   return(1);
}

static int G__AMSParticle_ImplFileLine_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSParticle*)(G__getstructoffset()))->ImplFileLine());
   return(1);
}

static int G__AMSParticle_Class_Version_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSParticle*)(G__getstructoffset()))->Class_Version());
   return(1);
}

static int G__AMSParticle_Class_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSParticle*)(G__getstructoffset()))->Class());
   return(1);
}

static int G__AMSParticle_Dictionary_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSParticle*)(G__getstructoffset()))->Dictionary();
   return(1);
}

static int G__AMSParticle_IsA_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSParticle*)(G__getstructoffset()))->IsA());
   return(1);
}

static int G__AMSParticle_ShowMembers_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSParticle*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1);
}

static int G__AMSParticle_Streamer_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSParticle*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1);
}

// automatic copy constructor
static int G__AMSParticle_AMSParticle_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSParticle *p;
   if(1!=libp->paran) ;
   p=new AMSParticle(*(AMSParticle*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSParticle);
   return(1);
}

// automatic destructor
static int G__AMSParticle_wAAMSParticle_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSParticle *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSParticle *)((G__getstructoffset())+sizeof(AMSParticle)*i))->~AMSParticle();
   else if(G__PVOID==G__getgvp()) delete (AMSParticle *)(G__getstructoffset());
   else ((AMSParticle *)(G__getstructoffset()))->~AMSParticle();
   return(1);
}


/* Debugger */
static int G__Debugger_Debugger_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   Debugger *p;
   if(G__getaryconstruct()) p=new Debugger[G__getaryconstruct()];
   else                    p=new Debugger;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_Debugger);
   return(1);
}

static int G__Debugger_On_2_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((Debugger*)(G__getstructoffset()))->On();
   return(1);
}

static int G__Debugger_Off_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((Debugger*)(G__getstructoffset()))->Off();
   return(1);
}

static int G__Debugger_Print_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((Debugger*)(G__getstructoffset()))->Print((char*)G__int(libp->para[0]),(int)G__int(libp->para[1]));
   return(1);
}

static int G__Debugger_DeclFileName_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((Debugger*)(G__getstructoffset()))->DeclFileName());
   return(1);
}

static int G__Debugger_DeclFileLine_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((Debugger*)(G__getstructoffset()))->DeclFileLine());
   return(1);
}

static int G__Debugger_ImplFileName_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((Debugger*)(G__getstructoffset()))->ImplFileName());
   return(1);
}

static int G__Debugger_ImplFileLine_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((Debugger*)(G__getstructoffset()))->ImplFileLine());
   return(1);
}

static int G__Debugger_Class_Version_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((Debugger*)(G__getstructoffset()))->Class_Version());
   return(1);
}

static int G__Debugger_Class_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((Debugger*)(G__getstructoffset()))->Class());
   return(1);
}

static int G__Debugger_Dictionary_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((Debugger*)(G__getstructoffset()))->Dictionary();
   return(1);
}

static int G__Debugger_IsA_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((Debugger*)(G__getstructoffset()))->IsA());
   return(1);
}

static int G__Debugger_ShowMembers_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((Debugger*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1);
}

static int G__Debugger_Streamer_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((Debugger*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1);
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
   result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_Debugger);
   return(1);
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
   return(1);
}


/* AMSCanvas */
static int G__AMSCanvas_AMSCanvas_2_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSCanvas *p;
   if(G__getaryconstruct()) p=new AMSCanvas[G__getaryconstruct()];
   else                    p=new AMSCanvas;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSCanvas);
   return(1);
}

static int G__AMSCanvas_AMSCanvas_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSCanvas *p;
   switch(libp->paran) {
   case 4:
      p = new AMSCanvas(
(Text_t*)G__int(libp->para[0]),(Text_t*)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]),(Int_t)G__int(libp->para[3]));
      break;
   case 3:
      p = new AMSCanvas(
(Text_t*)G__int(libp->para[0]),(Text_t*)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]));
      break;
   case 2:
      p = new AMSCanvas((Text_t*)G__int(libp->para[0]),(Text_t*)G__int(libp->para[1]));
      break;
   case 1:
      p = new AMSCanvas((Text_t*)G__int(libp->para[0]));
      break;
   }
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSCanvas);
   return(1);
}

static int G__AMSCanvas_Update_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSCanvas*)(G__getstructoffset()))->Update();
   return(1);
}

static int G__AMSCanvas_EditorBar_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSCanvas*)(G__getstructoffset()))->EditorBar();
   return(1);
}

static int G__AMSCanvas_HandleInput_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSCanvas*)(G__getstructoffset()))->HandleInput((Int_t)G__int(libp->para[0]),(Int_t)G__int(libp->para[1])
,(Int_t)G__int(libp->para[2]));
   return(1);
}

static int G__AMSCanvas_OpenFileCB_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSCanvas*)(G__getstructoffset()))->OpenFileCB((Widget)G__int(libp->para[0]),(XtPointer)G__int(libp->para[1])
,(XtPointer)G__int(libp->para[2]));
   return(1);
}

static int G__AMSCanvas_SaveParticleCB_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSCanvas*)(G__getstructoffset()))->SaveParticleCB((Widget)G__int(libp->para[0]),(XtPointer)G__int(libp->para[1])
,(XtPointer)G__int(libp->para[2]));
   return(1);
}

static int G__AMSCanvas_PrintCB_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSCanvas*)(G__getstructoffset()))->PrintCB((Widget)G__int(libp->para[0]),(XtPointer)G__int(libp->para[1])
,(XtPointer)G__int(libp->para[2]));
   return(1);
}

static int G__AMSCanvas_DeclFileName_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSCanvas*)(G__getstructoffset()))->DeclFileName());
   return(1);
}

static int G__AMSCanvas_DeclFileLine_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSCanvas*)(G__getstructoffset()))->DeclFileLine());
   return(1);
}

static int G__AMSCanvas_ImplFileName_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSCanvas*)(G__getstructoffset()))->ImplFileName());
   return(1);
}

static int G__AMSCanvas_ImplFileLine_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSCanvas*)(G__getstructoffset()))->ImplFileLine());
   return(1);
}

static int G__AMSCanvas_Class_Version_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSCanvas*)(G__getstructoffset()))->Class_Version());
   return(1);
}

static int G__AMSCanvas_Class_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSCanvas*)(G__getstructoffset()))->Class());
   return(1);
}

static int G__AMSCanvas_Dictionary_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSCanvas*)(G__getstructoffset()))->Dictionary();
   return(1);
}

static int G__AMSCanvas_IsA_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSCanvas*)(G__getstructoffset()))->IsA());
   return(1);
}

static int G__AMSCanvas_ShowMembers_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSCanvas*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1);
}

static int G__AMSCanvas_Streamer_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSCanvas*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1);
}

// automatic copy constructor
static int G__AMSCanvas_AMSCanvas_1_2(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSCanvas *p;
   if(1!=libp->paran) ;
   p=new AMSCanvas(*(AMSCanvas*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSCanvas);
   return(1);
}

// automatic destructor
static int G__AMSCanvas_wAAMSCanvas_2_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSCanvas *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSCanvas *)((G__getstructoffset())+sizeof(AMSCanvas)*i))->~AMSCanvas();
   else if(G__PVOID==G__getgvp()) delete (AMSCanvas *)(G__getstructoffset());
   else ((AMSCanvas *)(G__getstructoffset()))->~AMSCanvas();
   return(1);
}


/* AMSGeometrySetter */
static int G__AMSGeometrySetter_AMSGeometrySetter_0_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSGeometrySetter *p;
   if(G__getaryconstruct()) p=new AMSGeometrySetter[G__getaryconstruct()];
   else                    p=new AMSGeometrySetter;
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSGeometrySetter);
   return(1);
}

static int G__AMSGeometrySetter_AMSGeometrySetter_1_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   AMSGeometrySetter *p;
      p = new AMSGeometrySetter((TGeometry*)G__int(libp->para[0]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSGeometrySetter);
   return(1);
}

static int G__AMSGeometrySetter_TurnOn_3_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSGeometrySetter*)(G__getstructoffset()))->TurnOn((char*)G__int(libp->para[0]));
   return(1);
}

static int G__AMSGeometrySetter_TurnOnWithSons_4_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSGeometrySetter*)(G__getstructoffset()))->TurnOnWithSons((char*)G__int(libp->para[0]));
   return(1);
}

static int G__AMSGeometrySetter_TurnOff_5_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSGeometrySetter*)(G__getstructoffset()))->TurnOff((char*)G__int(libp->para[0]));
   return(1);
}

static int G__AMSGeometrySetter_TurnOffWithSons_6_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSGeometrySetter*)(G__getstructoffset()))->TurnOffWithSons((char*)G__int(libp->para[0]));
   return(1);
}

static int G__AMSGeometrySetter_CheckVisibility_7_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSGeometrySetter*)(G__getstructoffset()))->CheckVisibility((char*)G__int(libp->para[0]));
   return(1);
}

static int G__AMSGeometrySetter_SetGeo_8_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSGeometrySetter*)(G__getstructoffset()))->SetGeo();
   return(1);
}

static int G__AMSGeometrySetter_DeclFileName_9_0(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSGeometrySetter*)(G__getstructoffset()))->DeclFileName());
   return(1);
}

static int G__AMSGeometrySetter_DeclFileLine_0_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSGeometrySetter*)(G__getstructoffset()))->DeclFileLine());
   return(1);
}

static int G__AMSGeometrySetter_ImplFileName_1_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,67,(long)((AMSGeometrySetter*)(G__getstructoffset()))->ImplFileName());
   return(1);
}

static int G__AMSGeometrySetter_ImplFileLine_2_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)((AMSGeometrySetter*)(G__getstructoffset()))->ImplFileLine());
   return(1);
}

static int G__AMSGeometrySetter_Class_Version_3_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)((AMSGeometrySetter*)(G__getstructoffset()))->Class_Version());
   return(1);
}

static int G__AMSGeometrySetter_Class_4_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSGeometrySetter*)(G__getstructoffset()))->Class());
   return(1);
}

static int G__AMSGeometrySetter_Dictionary_5_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSGeometrySetter*)(G__getstructoffset()))->Dictionary();
   return(1);
}

static int G__AMSGeometrySetter_IsA_6_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,85,(long)((AMSGeometrySetter*)(G__getstructoffset()))->IsA());
   return(1);
}

static int G__AMSGeometrySetter_ShowMembers_7_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSGeometrySetter*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1);
}

static int G__AMSGeometrySetter_Streamer_8_1(G__value *result7,char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((AMSGeometrySetter*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1);
}

// automatic copy constructor
static int G__AMSGeometrySetter_AMSGeometrySetter_9_1(G__value *result7,char *funcname,struct G__param *libp,int hash)
{
   AMSGeometrySetter *p;
   if(1!=libp->paran) ;
   p=new AMSGeometrySetter(*(AMSGeometrySetter*)G__int(libp->para[0]));
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__AMSCintLN_AMSGeometrySetter);
   return(1);
}

// automatic destructor
static int G__AMSGeometrySetter_wAAMSGeometrySetter_0_2(G__value *result7,char *funcname,struct G__param *libp,int hash) {
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (AMSGeometrySetter *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         ((AMSGeometrySetter *)((G__getstructoffset())+sizeof(AMSGeometrySetter)*i))->~AMSGeometrySetter();
   else if(G__PVOID==G__getgvp()) delete (AMSGeometrySetter *)(G__getstructoffset());
   else ((AMSGeometrySetter *)(G__getstructoffset()))->~AMSGeometrySetter();
   return(1);
}


/* Setting up global function */

/*********************************************************
* Member function Stub
*********************************************************/

/* Setting up global function stub */

/*********************************************************
* Get size of pointer to member function
*********************************************************/
class G__Sizep2memfuncAMSCint {
 public:
  G__Sizep2memfuncAMSCint() {p=&G__Sizep2memfuncAMSCint::sizep2memfunc;}
    size_t sizep2memfunc() { return(sizeof(p)); }
  private:
    size_t (G__Sizep2memfuncAMSCint::*p)();
};

size_t G__get_sizep2memfuncAMSCint()
{
  G__Sizep2memfuncAMSCint a;
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
extern "C" void G__cpp_setup_inheritanceAMSCint() {

   /* Setting up class inheritance */
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSCintLN_THelix))) {
     THelix *G__Lderived;
     G__Lderived=(THelix*)0x1000;
     {
       TPolyLine3D *G__Lpbase=(TPolyLine3D*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_THelix),G__get_linked_tagnum(&G__AMSCintLN_TPolyLine3D),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_THelix),G__get_linked_tagnum(&G__AMSCintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttLine *G__Lpbase=(TAttLine*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_THelix),G__get_linked_tagnum(&G__AMSCintLN_TAttLine),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSCintLN_TRadioButton))) {
     TRadioButton *G__Lderived;
     G__Lderived=(TRadioButton*)0x1000;
     {
       TButton *G__Lpbase=(TButton*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_TRadioButton),G__get_linked_tagnum(&G__AMSCintLN_TButton),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TPad *G__Lpbase=(TPad*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_TRadioButton),G__get_linked_tagnum(&G__AMSCintLN_TPad),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TVirtualPad *G__Lpbase=(TVirtualPad*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_TRadioButton),G__get_linked_tagnum(&G__AMSCintLN_TVirtualPad),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TWbox *G__Lpbase=(TWbox*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_TRadioButton),G__get_linked_tagnum(&G__AMSCintLN_TWbox),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TBox *G__Lpbase=(TBox*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_TRadioButton),G__get_linked_tagnum(&G__AMSCintLN_TBox),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_TRadioButton),G__get_linked_tagnum(&G__AMSCintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttLine *G__Lpbase=(TAttLine*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_TRadioButton),G__get_linked_tagnum(&G__AMSCintLN_TAttLine),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttFill *G__Lpbase=(TAttFill*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_TRadioButton),G__get_linked_tagnum(&G__AMSCintLN_TAttFill),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttPad *G__Lpbase=(TAttPad*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_TRadioButton),G__get_linked_tagnum(&G__AMSCintLN_TAttPad),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttText *G__Lpbase=(TAttText*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_TRadioButton),G__get_linked_tagnum(&G__AMSCintLN_TAttText),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSCintLN_TSwitch))) {
     TSwitch *G__Lderived;
     G__Lderived=(TSwitch*)0x1000;
     {
       TPad *G__Lpbase=(TPad*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_TSwitch),G__get_linked_tagnum(&G__AMSCintLN_TPad),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TVirtualPad *G__Lpbase=(TVirtualPad*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_TSwitch),G__get_linked_tagnum(&G__AMSCintLN_TVirtualPad),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TWbox *G__Lpbase=(TWbox*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_TSwitch),G__get_linked_tagnum(&G__AMSCintLN_TWbox),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TBox *G__Lpbase=(TBox*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_TSwitch),G__get_linked_tagnum(&G__AMSCintLN_TBox),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_TSwitch),G__get_linked_tagnum(&G__AMSCintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttLine *G__Lpbase=(TAttLine*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_TSwitch),G__get_linked_tagnum(&G__AMSCintLN_TAttLine),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttFill *G__Lpbase=(TAttFill*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_TSwitch),G__get_linked_tagnum(&G__AMSCintLN_TAttFill),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttPad *G__Lpbase=(TAttPad*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_TSwitch),G__get_linked_tagnum(&G__AMSCintLN_TAttPad),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSCintLN_AMSAxis))) {
     AMSAxis *G__Lderived;
     G__Lderived=(AMSAxis*)0x1000;
     {
       TPolyLine3D *G__Lpbase=(TPolyLine3D*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSAxis),G__get_linked_tagnum(&G__AMSCintLN_TPolyLine3D),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSAxis),G__get_linked_tagnum(&G__AMSCintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttLine *G__Lpbase=(TAttLine*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSAxis),G__get_linked_tagnum(&G__AMSCintLN_TAttLine),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSCintLN_AMSDisplay))) {
     AMSDisplay *G__Lderived;
     G__Lderived=(AMSDisplay*)0x1000;
     {
       AMSVirtualDisplay *G__Lpbase=(AMSVirtualDisplay*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSDisplay),G__get_linked_tagnum(&G__AMSCintLN_AMSVirtualDisplay),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSDisplay),G__get_linked_tagnum(&G__AMSCintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSCintLN_AMS3DCluster))) {
     AMS3DCluster *G__Lderived;
     G__Lderived=(AMS3DCluster*)0x1000;
     {
       TMarker3DBox *G__Lpbase=(TMarker3DBox*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMS3DCluster),G__get_linked_tagnum(&G__AMSCintLN_TMarker3DBox),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMS3DCluster),G__get_linked_tagnum(&G__AMSCintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttLine *G__Lpbase=(TAttLine*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMS3DCluster),G__get_linked_tagnum(&G__AMSCintLN_TAttLine),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttFill *G__Lpbase=(TAttFill*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMS3DCluster),G__get_linked_tagnum(&G__AMSCintLN_TAttFill),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSCintLN_AMSMaker))) {
     AMSMaker *G__Lderived;
     G__Lderived=(AMSMaker*)0x1000;
     {
       TNamed *G__Lpbase=(TNamed*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSMaker),G__get_linked_tagnum(&G__AMSCintLN_TNamed),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSMaker),G__get_linked_tagnum(&G__AMSCintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSCintLN_AMSHistBrowser))) {
     AMSHistBrowser *G__Lderived;
     G__Lderived=(AMSHistBrowser*)0x1000;
     {
       TNamed *G__Lpbase=(TNamed*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSHistBrowser),G__get_linked_tagnum(&G__AMSCintLN_TNamed),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSHistBrowser),G__get_linked_tagnum(&G__AMSCintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSCintLN_AMSToFClusterReader))) {
     AMSToFClusterReader *G__Lderived;
     G__Lderived=(AMSToFClusterReader*)0x1000;
     {
       AMSMaker *G__Lpbase=(AMSMaker*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSToFClusterReader),G__get_linked_tagnum(&G__AMSCintLN_AMSMaker),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TNamed *G__Lpbase=(TNamed*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSToFClusterReader),G__get_linked_tagnum(&G__AMSCintLN_TNamed),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSToFClusterReader),G__get_linked_tagnum(&G__AMSCintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSCintLN_AMSTrackReader))) {
     AMSTrackReader *G__Lderived;
     G__Lderived=(AMSTrackReader*)0x1000;
     {
       AMSMaker *G__Lpbase=(AMSMaker*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSTrackReader),G__get_linked_tagnum(&G__AMSCintLN_AMSMaker),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TNamed *G__Lpbase=(TNamed*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSTrackReader),G__get_linked_tagnum(&G__AMSCintLN_TNamed),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSTrackReader),G__get_linked_tagnum(&G__AMSCintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSCintLN_AMSSiHitReader))) {
     AMSSiHitReader *G__Lderived;
     G__Lderived=(AMSSiHitReader*)0x1000;
     {
       AMSMaker *G__Lpbase=(AMSMaker*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSSiHitReader),G__get_linked_tagnum(&G__AMSCintLN_AMSMaker),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TNamed *G__Lpbase=(TNamed*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSSiHitReader),G__get_linked_tagnum(&G__AMSCintLN_TNamed),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSSiHitReader),G__get_linked_tagnum(&G__AMSCintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSCintLN_AMSCTCClusterReader))) {
     AMSCTCClusterReader *G__Lderived;
     G__Lderived=(AMSCTCClusterReader*)0x1000;
     {
       AMSMaker *G__Lpbase=(AMSMaker*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSCTCClusterReader),G__get_linked_tagnum(&G__AMSCintLN_AMSMaker),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TNamed *G__Lpbase=(TNamed*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSCTCClusterReader),G__get_linked_tagnum(&G__AMSCintLN_TNamed),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSCTCClusterReader),G__get_linked_tagnum(&G__AMSCintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSCintLN_AMSAntiClusterReader))) {
     AMSAntiClusterReader *G__Lderived;
     G__Lderived=(AMSAntiClusterReader*)0x1000;
     {
       AMSMaker *G__Lpbase=(AMSMaker*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSAntiClusterReader),G__get_linked_tagnum(&G__AMSCintLN_AMSMaker),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TNamed *G__Lpbase=(TNamed*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSAntiClusterReader),G__get_linked_tagnum(&G__AMSCintLN_TNamed),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSAntiClusterReader),G__get_linked_tagnum(&G__AMSCintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSCintLN_AMSParticleReader))) {
     AMSParticleReader *G__Lderived;
     G__Lderived=(AMSParticleReader*)0x1000;
     {
       AMSMaker *G__Lpbase=(AMSMaker*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSParticleReader),G__get_linked_tagnum(&G__AMSCintLN_AMSMaker),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TNamed *G__Lpbase=(TNamed*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSParticleReader),G__get_linked_tagnum(&G__AMSCintLN_TNamed),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSParticleReader),G__get_linked_tagnum(&G__AMSCintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSCintLN_AMSVirtualDisplay))) {
     AMSVirtualDisplay *G__Lderived;
     G__Lderived=(AMSVirtualDisplay*)0x1000;
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSVirtualDisplay),G__get_linked_tagnum(&G__AMSCintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSCintLN_AMSRoot))) {
     AMSRoot *G__Lderived;
     G__Lderived=(AMSRoot*)0x1000;
     {
       TNamed *G__Lpbase=(TNamed*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSRoot),G__get_linked_tagnum(&G__AMSCintLN_TNamed),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSRoot),G__get_linked_tagnum(&G__AMSCintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSCintLN_AMSToFCluster))) {
     AMSToFCluster *G__Lderived;
     G__Lderived=(AMSToFCluster*)0x1000;
     {
       AMS3DCluster *G__Lpbase=(AMS3DCluster*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSToFCluster),G__get_linked_tagnum(&G__AMSCintLN_AMS3DCluster),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TMarker3DBox *G__Lpbase=(TMarker3DBox*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSToFCluster),G__get_linked_tagnum(&G__AMSCintLN_TMarker3DBox),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSToFCluster),G__get_linked_tagnum(&G__AMSCintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttLine *G__Lpbase=(TAttLine*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSToFCluster),G__get_linked_tagnum(&G__AMSCintLN_TAttLine),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttFill *G__Lpbase=(TAttFill*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSToFCluster),G__get_linked_tagnum(&G__AMSCintLN_TAttFill),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSCintLN_AMSAntiCluster))) {
     AMSAntiCluster *G__Lderived;
     G__Lderived=(AMSAntiCluster*)0x1000;
     {
       AMS3DCluster *G__Lpbase=(AMS3DCluster*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSAntiCluster),G__get_linked_tagnum(&G__AMSCintLN_AMS3DCluster),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TMarker3DBox *G__Lpbase=(TMarker3DBox*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSAntiCluster),G__get_linked_tagnum(&G__AMSCintLN_TMarker3DBox),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSAntiCluster),G__get_linked_tagnum(&G__AMSCintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttLine *G__Lpbase=(TAttLine*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSAntiCluster),G__get_linked_tagnum(&G__AMSCintLN_TAttLine),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttFill *G__Lpbase=(TAttFill*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSAntiCluster),G__get_linked_tagnum(&G__AMSCintLN_TAttFill),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSCintLN_AMSTrack))) {
     AMSTrack *G__Lderived;
     G__Lderived=(AMSTrack*)0x1000;
     {
       THelix *G__Lpbase=(THelix*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSTrack),G__get_linked_tagnum(&G__AMSCintLN_THelix),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TPolyLine3D *G__Lpbase=(TPolyLine3D*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSTrack),G__get_linked_tagnum(&G__AMSCintLN_TPolyLine3D),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSTrack),G__get_linked_tagnum(&G__AMSCintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttLine *G__Lpbase=(TAttLine*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSTrack),G__get_linked_tagnum(&G__AMSCintLN_TAttLine),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSCintLN_AMSSiHit))) {
     AMSSiHit *G__Lderived;
     G__Lderived=(AMSSiHit*)0x1000;
     {
       AMS3DCluster *G__Lpbase=(AMS3DCluster*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSSiHit),G__get_linked_tagnum(&G__AMSCintLN_AMS3DCluster),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TMarker3DBox *G__Lpbase=(TMarker3DBox*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSSiHit),G__get_linked_tagnum(&G__AMSCintLN_TMarker3DBox),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSSiHit),G__get_linked_tagnum(&G__AMSCintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttLine *G__Lpbase=(TAttLine*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSSiHit),G__get_linked_tagnum(&G__AMSCintLN_TAttLine),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttFill *G__Lpbase=(TAttFill*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSSiHit),G__get_linked_tagnum(&G__AMSCintLN_TAttFill),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSCintLN_AMSCTCCluster))) {
     AMSCTCCluster *G__Lderived;
     G__Lderived=(AMSCTCCluster*)0x1000;
     {
       AMS3DCluster *G__Lpbase=(AMS3DCluster*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSCTCCluster),G__get_linked_tagnum(&G__AMSCintLN_AMS3DCluster),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TMarker3DBox *G__Lpbase=(TMarker3DBox*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSCTCCluster),G__get_linked_tagnum(&G__AMSCintLN_TMarker3DBox),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSCTCCluster),G__get_linked_tagnum(&G__AMSCintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttLine *G__Lpbase=(TAttLine*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSCTCCluster),G__get_linked_tagnum(&G__AMSCintLN_TAttLine),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttFill *G__Lpbase=(TAttFill*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSCTCCluster),G__get_linked_tagnum(&G__AMSCintLN_TAttFill),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSCintLN_AMSParticle))) {
     AMSParticle *G__Lderived;
     G__Lderived=(AMSParticle*)0x1000;
     {
       THelix *G__Lpbase=(THelix*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSParticle),G__get_linked_tagnum(&G__AMSCintLN_THelix),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TPolyLine3D *G__Lpbase=(TPolyLine3D*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSParticle),G__get_linked_tagnum(&G__AMSCintLN_TPolyLine3D),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSParticle),G__get_linked_tagnum(&G__AMSCintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttLine *G__Lpbase=(TAttLine*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSParticle),G__get_linked_tagnum(&G__AMSCintLN_TAttLine),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSCintLN_Debugger))) {
     Debugger *G__Lderived;
     G__Lderived=(Debugger*)0x1000;
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_Debugger),G__get_linked_tagnum(&G__AMSCintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
   }
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__AMSCintLN_AMSCanvas))) {
     AMSCanvas *G__Lderived;
     G__Lderived=(AMSCanvas*)0x1000;
     {
       TCanvas *G__Lpbase=(TCanvas*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSCanvas),G__get_linked_tagnum(&G__AMSCintLN_TCanvas),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TPad *G__Lpbase=(TPad*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSCanvas),G__get_linked_tagnum(&G__AMSCintLN_TPad),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TVirtualPad *G__Lpbase=(TVirtualPad*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSCanvas),G__get_linked_tagnum(&G__AMSCintLN_TVirtualPad),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TWbox *G__Lpbase=(TWbox*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSCanvas),G__get_linked_tagnum(&G__AMSCintLN_TWbox),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TBox *G__Lpbase=(TBox*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSCanvas),G__get_linked_tagnum(&G__AMSCintLN_TBox),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSCanvas),G__get_linked_tagnum(&G__AMSCintLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttLine *G__Lpbase=(TAttLine*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSCanvas),G__get_linked_tagnum(&G__AMSCintLN_TAttLine),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttFill *G__Lpbase=(TAttFill*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSCanvas),G__get_linked_tagnum(&G__AMSCintLN_TAttFill),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TAttPad *G__Lpbase=(TAttPad*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSCanvas),G__get_linked_tagnum(&G__AMSCintLN_TAttPad),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
}

/*********************************************************
* typedef information setup/
*********************************************************/
extern "C" void G__cpp_setup_typetableAMSCint() {

   /* Setting up typedef entry */
   G__search_typename("Char_t",99,-1,0);
   G__setnewtype(-1,"Signed Character 1 byte",0);
   G__search_typename("UChar_t",98,-1,0);
   G__setnewtype(-1,"Unsigned Character 1 byte",0);
   G__search_typename("Short_t",115,-1,0);
   G__setnewtype(-1,"Signed Short integer 2 bytes",0);
   G__search_typename("UShort_t",114,-1,0);
   G__setnewtype(-1,"Unsigned Short integer 2 bytes",0);
   G__search_typename("Int_t",105,-1,0);
   G__setnewtype(-1,"Signed integer 4 bytes",0);
   G__search_typename("UInt_t",104,-1,0);
   G__setnewtype(-1,"Unsigned integer 4 bytes",0);
   G__search_typename("Seek_t",105,-1,0);
   G__setnewtype(-1,"File pointer",0);
   G__search_typename("Long_t",108,-1,0);
   G__setnewtype(-1,"Signed long integer 8 bytes",0);
   G__search_typename("ULong_t",107,-1,0);
   G__setnewtype(-1,"Unsigned long integer 8 bytes",0);
   G__search_typename("Float_t",102,-1,0);
   G__setnewtype(-1,"Float 4 bytes",0);
   G__search_typename("Double_t",100,-1,0);
   G__setnewtype(-1,"Float 8 bytes",0);
   G__search_typename("Text_t",99,-1,0);
   G__setnewtype(-1,"General string",0);
   G__search_typename("Bool_t",98,-1,0);
   G__setnewtype(-1,"Boolean (0=false, 1=true)",0);
   G__search_typename("Byte_t",98,-1,0);
   G__setnewtype(-1,"Byte (8 bits)",0);
   G__search_typename("Version_t",115,-1,0);
   G__setnewtype(-1,"Class version identifier",0);
   G__search_typename("Option_t",99,-1,0);
   G__setnewtype(-1,"Option string",0);
   G__search_typename("Ssiz_t",105,-1,0);
   G__setnewtype(-1,"String size",0);
   G__search_typename("Real_t",102,-1,0);
   G__setnewtype(-1,"TVector and TMatrix element type",0);
   G__search_typename("VoidFuncPtr_t",89,-1,0);
   G__setnewtype(-1,"pointer to void function",0);
   G__search_typename("FreeHookFun_t",89,-1,0);
   G__setnewtype(-1,NULL,0);
   G__search_typename("ReAllocFun_t",89,-1,2);
   G__setnewtype(-1,NULL,0);
   G__search_typename("ReAllocCFun_t",89,-1,2);
   G__setnewtype(-1,NULL,0);
   G__search_typename("Axis_t",102,-1,0);
   G__setnewtype(-1,"Axis values type",0);
   G__search_typename("Stat_t",100,-1,0);
   G__setnewtype(-1,"Statistics type",0);
   G__search_typename("Font_t",115,-1,0);
   G__setnewtype(-1,"Font number",0);
   G__search_typename("Style_t",115,-1,0);
   G__setnewtype(-1,"Style number",0);
   G__search_typename("Marker_t",115,-1,0);
   G__setnewtype(-1,"Marker number",0);
   G__search_typename("Width_t",115,-1,0);
   G__setnewtype(-1,"Line width",0);
   G__search_typename("Color_t",115,-1,0);
   G__setnewtype(-1,"Color number",0);
   G__search_typename("SCoord_t",115,-1,0);
   G__setnewtype(-1,"Screen coordinates",0);
   G__search_typename("Coord_t",102,-1,0);
   G__setnewtype(-1,"Pad world coordinates",0);
   G__search_typename("Angle_t",102,-1,0);
   G__setnewtype(-1,"Graphics angle",0);
   G__search_typename("Size_t",102,-1,0);
   G__setnewtype(-1,"Attribute size",0);
   G__search_typename("DrawFaceFunc_t",97,-1,0);
   G__setnewtype(-1,NULL,0);
   G__search_typename("LegoFunc_t",97,-1,0);
   G__setnewtype(-1,NULL,0);
   G__search_typename("SurfaceFunc_t",97,-1,0);
   G__setnewtype(-1,NULL,0);
   G__search_typename("Double_t (*)(Double_t *, Double_t *)",81,-1,0);
   G__setnewtype(-1,NULL,0);
   G__search_typename("X3DBuffer",117,G__get_linked_tagnum(&G__AMSCintLN__x3d_data_),0);
   G__setnewtype(-1,NULL,0);
   G__search_typename("Size3D",117,G__get_linked_tagnum(&G__AMSCintLN__x3d_sizeof_),0);
   G__setnewtype(-1,NULL,0);
   G__search_typename("Widget",85,G__get_linked_tagnum(&G__AMSCintLN__WidgetRec),0);
   G__setnewtype(-1,NULL,0);
   G__search_typename("XtPointer",89,-1,0);
   G__setnewtype(-1,NULL,0);
}

/*********************************************************
* Data Member information setup/
*********************************************************/

   /* Setting up class,struct,union tag member variable */

   /* THelix */
static void G__setup_memvarTHelix(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSCintLN_THelix));
   { THelix *p; p=(THelix*)0x1000;
   G__memvar_setup((void*)NULL,100,0,0,-1,G__defined_typename("Double_t"),-1,2,"fX0=",0,"Initial X position");
   G__memvar_setup((void*)NULL,100,0,0,-1,G__defined_typename("Double_t"),-1,2,"fY0=",0,"Initial Y position");
   G__memvar_setup((void*)NULL,100,0,0,-1,G__defined_typename("Double_t"),-1,2,"fZ0=",0,"Initial Z position");
   G__memvar_setup((void*)NULL,100,0,0,-1,G__defined_typename("Double_t"),-1,2,"fVt=",0,"Transverse velocity (constant of motion)");
   G__memvar_setup((void*)NULL,100,0,0,-1,G__defined_typename("Double_t"),-1,2,"fPhi0=",0,"Initial phase, so vx0 = fVt*cos(fPhi0)");
   G__memvar_setup((void*)NULL,100,0,0,-1,G__defined_typename("Double_t"),-1,2,"fVz=",0,"Z velocity (constant of motion)");
   G__memvar_setup((void*)NULL,100,0,0,-1,G__defined_typename("Double_t"),-1,2,"fW=",0,"Angular frequency");
   G__memvar_setup((void*)NULL,100,0,0,-1,G__defined_typename("Double_t"),-1,2,"fAxis[3]=",0,"Direction unit vector of the helix axis");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TRotMatrix),-1,-1,2,"fRotMat=",0,"Rotation matrix: axis // z  -->  axis // fAxis");
   G__memvar_setup((void*)NULL,100,0,0,-1,G__defined_typename("Double_t"),-1,2,"fRange[2]=",0,"Range of helix parameter t");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-2,2,"minNSeg=",0,"minimal number of segments in polyline");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* TRadioButton */
static void G__setup_memvarTRadioButton(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSCintLN_TRadioButton));
   { TRadioButton *p; p=(TRadioButton*)0x1000;
   G__memvar_setup((void*)NULL,66,0,0,-1,G__defined_typename("Bool_t"),-1,2,"fState=",0,"Point to externally supplied state word");
   G__memvar_setup((void*)NULL,67,0,0,-1,-1,-1,2,"fOffMethod=",0,"method to execute when toggled \"off\"");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* TSwitch */
static void G__setup_memvarTSwitch(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSCintLN_TSwitch));
   { TSwitch *p; p=(TSwitch*)0x1000;
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TRadioButton),-1,-1,2,"fButton=",0,"the RadioButton displaying the logical state ");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TText),-1,-1,2,"fText=",0,"Text to be displayed");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSAxis */
static void G__setup_memvarAMSAxis(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSAxis));
   { AMSAxis *p; p=(AMSAxis*)0x1000;
   G__memvar_setup((void*)NULL,67,0,0,-1,-1,-1,4,"m_Title=",0,"title of the axis (usually \"x\", \"y\", \"z\")");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSDisplay */
static void G__setup_memvarAMSDisplay(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSDisplay));
   { AMSDisplay *p; p=(AMSDisplay*)0x1000;
   G__memvar_setup((void*)NULL,105,0,0,G__get_linked_tagnum(&G__AMSCintLN_EAMSView),-1,-1,4,"m_View=",0,"view");
   G__memvar_setup((void*)NULL,98,0,0,-1,G__defined_typename("Bool_t"),-1,4,"m_DrawParticles=",0,"Flag True if particles to be drawn");
   G__memvar_setup((void*)NULL,98,0,0,-1,G__defined_typename("Bool_t"),-1,4,"m_DrawGeometry=",0,"Draw geometry?");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TGeometry),-1,-1,4,"m_Geometry=",0,"Pointer to the geometry");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_AMSGeometrySetter),-1,-1,4,"m_GeoSetter=",0,"Pointer to the geometry setter");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_PTcut=",0,"PT cut to display objects");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_PTcutEGMUNU=",0,"PT cut for Electrons, Gammas, MUons, Neutrinos");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_Theta=",0,"Viewing angle theta");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_Phi=",0,"Viewing angle phi");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_AMSCanvas),-1,-1,4,"m_Canvas=",0,"Pointer to the display canvas");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TPad),-1,-1,4,"m_UCPad=",0,"Pointer to the UC pad ");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TPad),-1,-1,4,"m_LogoPad=",0,"Pointer to the logo pad ");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TPad),-1,-1,4,"m_TrigPad=",0,"Pointer to the trigger pad ");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TPad),-1,-1,4,"m_ButtonPad=",0,"Pointer to the buttons pad");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TPad),-1,-1,4,"m_Pad=",0,"Pointer to the event display main pad");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TPad),-1,-1,4,"m_AxisPad[4]=",0,"Pointer to the axis pad");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TPad),-1,-1,4,"m_TitlePad=",0,"Pointer to title pad ");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TPad),-1,-1,4,"m_EventInfoPad=",0,"Pointer to event info pad ");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TPad),-1,-1,4,"m_ObjInfoPad=",0,"Pointer to object info pad ");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TArc),-1,-1,4,"m_EM1=",0,"Pointer to arc showing ON/OFF trigger EM1");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TArc),-1,-1,4,"m_PH1=",0,"Pointer to arc showing ON/OFF trigger PH1");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TArc),-1,-1,4,"m_EM2=",0,"Pointer to arc showing ON/OFF trigger EM2");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TArc),-1,-1,4,"m_MU1=",0,"Pointer to arc showing ON/OFF trigger MU1");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TArc),-1,-1,4,"m_MU2=",0,"Pointer to arc showing ON/OFF trigger MU2");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TArc),-1,-1,4,"m_EMU=",0,"Pointer to arc showing ON/OFF trigger EMU");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TArc),-1,-1,4,"m_JT1=",0,"Pointer to arc showing ON/OFF trigger JT1");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TArc),-1,-1,4,"m_JT3=",0,"Pointer to arc showing ON/OFF trigger JT3");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TArc),-1,-1,4,"m_JT4=",0,"Pointer to arc showing ON/OFF trigger JT4");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TArc),-1,-1,4,"m_ALL=",0,"Pointer to arc showing ON/OFF trigger ALL");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TList),-1,-1,4,"m_ToBeDrawn=",0,"List of objects to be drawn");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMS3DCluster */
static void G__setup_memvarAMS3DCluster(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSCintLN_AMS3DCluster));
   { AMS3DCluster *p; p=(AMS3DCluster*)0x1000;
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,2,"fX=",0,"X0 (interface for versions before v1.03/08)");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,2,"fY=",0,"Y0 (interface for versions before v1.03/08)");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,2,"fZ=",0,"Z0 (interface for versions before v1.03/08)");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,2,"fRange=",0,"Range that considered close on view pad");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSMaker */
static void G__setup_memvarAMSMaker(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSMaker));
   { AMSMaker *p; p=(AMSMaker*)0x1000;
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,2,"m_Save=",0,"= 1 if m-Maker to be saved in the Tree");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TObject),-1,-1,2,"m_Fruits=",0,"Pointer to maker fruits (result)");
   G__memvar_setup((void*)NULL,117,0,0,G__get_linked_tagnum(&G__AMSCintLN_TString),-1,-1,2,"m_BranchName=",0,"Name of branch (if any)");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TList),-1,-1,2,"m_Histograms=",0,"Pointer to list supporting Maker histograms");
   G__memvar_setup((void*)((long)(&p->DrawFruits)-(long)(p)),98,0,0,-1,G__defined_typename("Bool_t"),-1,1,"DrawFruits=",0,"Draw fruits?");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSHistBrowser */
static void G__setup_memvarAMSHistBrowser(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSHistBrowser));
   { AMSHistBrowser *p; p=(AMSHistBrowser*)0x1000;
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSToFClusterReader */
static void G__setup_memvarAMSToFClusterReader(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSToFClusterReader));
   { AMSToFClusterReader *p; p=(AMSToFClusterReader*)0x1000;
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Nclusters=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSTrackReader */
static void G__setup_memvarAMSTrackReader(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSTrackReader));
   { AMSTrackReader *p; p=(AMSTrackReader*)0x1000;
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,2,"m_NTracks=",0,"Number of tracks");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TH1F),-1,-1,2,"m_Mult=",0,"tracks multiplicity");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSSiHitReader */
static void G__setup_memvarAMSSiHitReader(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSSiHitReader));
   { AMSSiHitReader *p; p=(AMSSiHitReader*)0x1000;
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,2,"m_Nclusters=",0,(char*)NULL);
   G__memvar_setup((void*)((long)(&p->DrawUsedHitsOnly)-(long)(p)),98,0,0,-1,G__defined_typename("Bool_t"),-1,1,"DrawUsedHitsOnly=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSCTCClusterReader */
static void G__setup_memvarAMSCTCClusterReader(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSCTCClusterReader));
   { AMSCTCClusterReader *p; p=(AMSCTCClusterReader*)0x1000;
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Nclusters=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSAntiClusterReader */
static void G__setup_memvarAMSAntiClusterReader(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSAntiClusterReader));
   { AMSAntiClusterReader *p; p=(AMSAntiClusterReader*)0x1000;
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Nclusters=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSParticleReader */
static void G__setup_memvarAMSParticleReader(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSParticleReader));
   { AMSParticleReader *p; p=(AMSParticleReader*)0x1000;
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,2,"m_NParticles=",0,"Number of tracks");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TH1F),-1,-1,2,"m_Mult=",0,"tracks multiplicity");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSVirtualDisplay */
static void G__setup_memvarAMSVirtualDisplay(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSVirtualDisplay));
   { AMSVirtualDisplay *p; p=(AMSVirtualDisplay*)0x1000;
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSRoot */
static void G__setup_memvarAMSRoot(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSRoot));
   { AMSRoot *p; p=(AMSRoot*)0x1000;
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Version=",0,"AMSRoot version number");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_VersionDate=",0,"AMSRoot version date");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_RunNum=",0,"Run number");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_EventNum=",0,"Event number");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Mode=",0,"Run mode");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Event=",0,"Event counter in the input tree");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TTree),-1,-1,4,"m_Tree=",0,"Pointer to the Root tree");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TTree),-1,-1,4,"m_Input=",0,"Pointer to the input data tree");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TList),-1,-1,4,"m_Makers=",0,"List of Makers");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_AMSToFClusterReader),-1,-1,4,"m_ToFClusterMaker=",0,"Pointer to ToFClusterMaker");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_AMSTrackReader),-1,-1,4,"m_TrackMaker=",0,"Pointer to TrackMaker");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_AMSSiHitReader),-1,-1,4,"m_SiHitMaker=",0,"Pointer to SiHitMaker");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_AMSCTCClusterReader),-1,-1,4,"m_CTCClusterMaker=",0,"Pointer to ToFClusterMaker");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_AMSAntiClusterReader),-1,-1,4,"m_AntiClusterMaker=",0,"Pointer to AntiClusterMaker");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_AMSParticleReader),-1,-1,4,"m_ParticleMaker=",0,"Pointer to ParticleMaker");
   G__memvar_setup((void*)NULL,117,0,0,G__get_linked_tagnum(&G__AMSCintLN_AMSHistBrowser),-1,-1,4,"m_HistBrowser=",0,"Object to Browse Maker histograms");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_AMSVirtualDisplay),-1,-1,4,"m_Display=",0,"!Pointer to Event display object");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSToFCluster */
static void G__setup_memvarAMSToFCluster(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSToFCluster));
   { AMSToFCluster *p; p=(AMSToFCluster*)0x1000;
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Status=",0,"Status word");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Plane=",0,"Plane number (1...4 : up ... down)");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Bar=",0,"Bar number (spec?)");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_Energy=",0,"Energy deposition in MeV");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_Time=",0,"time in seconds");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_ErTime=",0,"error of m_Time");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Ncells=",0,"Number of cells used for reconstructing ");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Nparticles=",0,"Number of particles passing thru");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_NTracks=",0,"number of tracks in m_Tracks;");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TObjArray),-1,-1,4,"m_Tracks=",0,"Tracks that pass this cluster");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSAntiCluster */
static void G__setup_memvarAMSAntiCluster(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSAntiCluster));
   { AMSAntiCluster *p; p=(AMSAntiCluster*)0x1000;
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Status=",0,"Status word");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Sector=",0,"Plane number (1...4 : up ... down)");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_Signal=",0,"Edep in MeV");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_NTracks=",0,"number of tracks in m_Tracks;");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TObjArray),-1,-1,4,"m_Tracks=",0,"Tracks that pass this cluster");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSTrack */
static void G__setup_memvarAMSTrack(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSTrack));
   { AMSTrack *p; p=(AMSTrack*)0x1000;
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
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSSiHit */
static void G__setup_memvarAMSSiHit(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSSiHit));
   { AMSSiHit *p; p=(AMSSiHit*)0x1000;
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Status=",0,"Status word");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Plane=",0,"Layer number (1...6 : up ... down)");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_X=",0,"X track cluster");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Y=",0,"Y track cluster");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_ErrPosition[3]=",0,"Error of m_Position[]");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_Amplitude=",0,"Amplitude");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_AsymAmpl=",0,"Asymmetry of amplitudes in X vs Y side");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TObjArray),-1,-1,4,"m_Tracks=",0,"Tracks that pass this hit");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSCTCCluster */
static void G__setup_memvarAMSCTCCluster(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSCTCCluster));
   { AMSCTCCluster *p; p=(AMSCTCCluster*)0x1000;
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Status=",0,"Status word");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_Plane=",0,"Plane number (1...4 : up ... down)");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_RawSignal=",0,"Raw signal");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_Signal=",0,"Corrected signal");
   G__memvar_setup((void*)NULL,102,0,0,-1,G__defined_typename("Float_t"),-1,4,"m_ErrSignal=",0,"Error of the above");
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,4,"m_NTracks=",0,"number of tracks in m_Tracks;");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TObjArray),-1,-1,4,"m_Tracks=",0,"Tracks that pass this cluster");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSParticle */
static void G__setup_memvarAMSParticle(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSParticle));
   { AMSParticle *p; p=(AMSParticle*)0x1000;
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
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* Debugger */
static void G__setup_memvarDebugger(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSCintLN_Debugger));
   { Debugger *p; p=(Debugger*)0x1000;
   G__memvar_setup((void*)NULL,98,0,0,-1,G__defined_typename("Bool_t"),-1,2,"isOn=",0,"Is the debugger on?");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSCanvas */
static void G__setup_memvarAMSCanvas(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSCanvas));
   { AMSCanvas *p; p=(AMSCanvas*)0x1000;
   G__memvar_setup((void*)NULL,117,0,0,G__get_linked_tagnum(&G__AMSCintLN_MenuDesc_t),-1,-2,2,"fgAMSFilePane[-2147483648]=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TMotifCanvas),-1,-2,2,"fTheCanvas=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}


   /* AMSGeometrySetter */
static void G__setup_memvarAMSGeometrySetter(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSGeometrySetter));
   { AMSGeometrySetter *p; p=(AMSGeometrySetter*)0x1000;
   G__memvar_setup((void*)NULL,117,0,0,G__get_linked_tagnum(&G__AMSCintLN_AMSGeoToggle),-1,-2,2,"m_Toggle[-2147483648]=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TGeometry),-1,-1,2,"m_Geometry=",0,"pointer to the geometry");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,108,0,0,-1,-1,-1,4,"G__virtualinfo=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}

extern "C" void G__cpp_setup_memvarAMSCint() {
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
static void G__setup_memfuncTHelix(void) {
   /* THelix */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSCintLN_THelix));
   G__memfunc_setup("SetRotMatrix",1238,(G__InterfaceMethod)NULL,121,-1,-1,0,0,1,2,0,"","Set rotation matrix ",(void*)NULL,0);
   G__memfunc_setup("FindClosestPhase",1615,(G__InterfaceMethod)NULL,100,-1,G__defined_typename("Double_t"),0,2,1,2,0,
"d - 'Double_t' 0 - phi0 d - 'Double_t' 0 - cosine",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("THelix",590,G__THelix_THelix_2_0,105,G__get_linked_tagnum(&G__AMSCintLN_THelix),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("THelix",590,G__THelix_THelix_3_0,105,G__get_linked_tagnum(&G__AMSCintLN_THelix),-1,0,14,1,1,0,
"d - 'Double_t' 0 - x d - 'Double_t' 0 - y "
"d - 'Double_t' 0 - z d - 'Double_t' 0 - vx "
"d - 'Double_t' 0 - vy d - 'Double_t' 0 - vz "
"d - 'Double_t' 0 - w d - 'Double_t' 0 0 r1 "
"d - 'Double_t' 0 1 r2 i 'EHelixRangeType' - 0 kHelixZ rtype "
"d - 'Double_t' 0 0 hx d - 'Double_t' 0 0 hy "
"d - 'Double_t' 0 1 hz C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("THelix",590,G__THelix_THelix_4_0,105,G__get_linked_tagnum(&G__AMSCintLN_THelix),-1,0,7,1,1,0,
"D - 'Double_t' 0 - xyz D - 'Double_t' 0 - v "
"d - 'Double_t' 0 - w D - 'Double_t' 0 0 range "
"i 'EHelixRangeType' - 0 kHelixZ rtype D - 'Double_t' 0 0 axis "
"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("THelix",590,G__THelix_THelix_5_0,105,G__get_linked_tagnum(&G__AMSCintLN_THelix),-1,0,1,1,1,0,"u 'THelix' - 1 - helix",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Copy",411,G__THelix_Copy_7_0,121,-1,-1,0,1,1,1,0,"u 'TObject' - 1 - helix",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Draw",398,G__THelix_Draw_8_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("GetOption",921,G__THelix_GetOption_9_0,67,-1,G__defined_typename("Option_t"),0,0,1,1,9,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Is3D",307,G__THelix_Is3D_0_1,98,-1,G__defined_typename("Bool_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Paint",508,G__THelix_Paint_1_1,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Print",525,G__THelix_Print_2_1,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SavePrimitive",1352,G__THelix_SavePrimitive_3_1,121,-1,-1,0,2,1,1,0,
"u 'ofstream' - 1 - out C - 'Option_t' 0 - option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SetOption",933,G__THelix_SetOption_4_1,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SetAxis",705,G__THelix_SetAxis_5_1,121,-1,-1,0,1,1,1,0,"D - 'Double_t' 0 - axis","Define new axis",(void*)NULL,1);
   G__memfunc_setup("SetAxis",705,G__THelix_SetAxis_6_1,121,-1,-1,0,3,1,1,0,
"d - 'Double_t' 0 - x d - 'Double_t' 0 - y "
"d - 'Double_t' 0 - z",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SetRange",793,G__THelix_SetRange_7_1,121,-1,-1,0,2,1,1,0,
"D - 'Double_t' 0 - range i 'EHelixRangeType' - 0 kHelixZ rtype",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SetRange",793,G__THelix_SetRange_8_1,121,-1,-1,0,3,1,1,0,
"d - 'Double_t' 0 - r1 d - 'Double_t' 0 - r2 "
"i 'EHelixRangeType' - 0 kHelixZ rtype",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SetHelix",806,G__THelix_SetHelix_9_1,121,-1,-1,0,13,1,1,0,
"d - 'Double_t' 0 - x d - 'Double_t' 0 - y "
"d - 'Double_t' 0 - z d - 'Double_t' 0 - vx "
"d - 'Double_t' 0 - vy d - 'Double_t' 0 - vz "
"d - 'Double_t' 0 - w d - 'Double_t' 0 0 r1 "
"d - 'Double_t' 0 0 r2 i 'EHelixRangeType' - 0 kUnchanged type "
"d - 'Double_t' 0 0 hx d - 'Double_t' 0 0 hy "
"d - 'Double_t' 0 1 hz",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Sizeof3D",743,G__THelix_Sizeof3D_0_2,121,-1,-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DeclFileName",1145,G__THelix_DeclFileName_1_2,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__THelix_DeclFileLine_2_2,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__THelix_ImplFileName_3_2,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__THelix_ImplFileLine_4_2,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__THelix_Class_Version_5_2,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__THelix_Class_6_2,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__THelix_Dictionary_7_2,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__THelix_IsA_8_2,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__THelix_ShowMembers_9_2,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__THelix_Streamer_0_3,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic destructor
   G__memfunc_setup("~THelix",716,G__THelix_wATHelix_1_3,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncTRadioButton(void) {
   /* TRadioButton */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSCintLN_TRadioButton));
   G__memfunc_setup("TRadioButton",1215,G__TRadioButton_TRadioButton_0_0,105,G__get_linked_tagnum(&G__AMSCintLN_TRadioButton),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("TRadioButton",1215,G__TRadioButton_TRadioButton_1_0,105,G__get_linked_tagnum(&G__AMSCintLN_TRadioButton),-1,0,7,1,1,0,
"B - 'Bool_t' 0 - state C - - 0 - onMethod "
"f - 'Coord_t' 0 - x1 f - 'Coord_t' 0 - y1 "
"f - 'Coord_t' 0 - x2 f - 'Coord_t' 0 - y2 "
"C - - 0 0 offMethod",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ExecuteEvent",1237,G__TRadioButton_ExecuteEvent_3_0,121,-1,-1,0,3,1,1,0,
"i - 'Int_t' 0 - event i - 'Int_t' 0 - px "
"i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileName",1145,G__TRadioButton_DeclFileName_4_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__TRadioButton_DeclFileLine_5_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__TRadioButton_ImplFileName_6_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__TRadioButton_ImplFileLine_7_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__TRadioButton_Class_Version_8_0,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__TRadioButton_Class_9_0,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__TRadioButton_Dictionary_0_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__TRadioButton_IsA_1_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__TRadioButton_ShowMembers_2_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__TRadioButton_Streamer_3_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("TRadioButton",1215,G__TRadioButton_TRadioButton_4_1,(int)('i'),G__get_linked_tagnum(&G__AMSCintLN_TRadioButton),-1,0,1,1,1,0,"u 'TRadioButton' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~TRadioButton",1341,G__TRadioButton_wATRadioButton_5_1,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncTSwitch(void) {
   /* TSwitch */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSCintLN_TSwitch));
   G__memfunc_setup("TSwitch",710,G__TSwitch_TSwitch_0_0,105,G__get_linked_tagnum(&G__AMSCintLN_TSwitch),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("TSwitch",710,G__TSwitch_TSwitch_1_0,105,G__get_linked_tagnum(&G__AMSCintLN_TSwitch),-1,0,8,1,1,0,
"C - - 0 - title B - 'Bool_t' 0 - state "
"C - - 0 - method f - 'Coord_t' 0 - x1 "
"f - 'Coord_t' 0 - y1 f - 'Coord_t' 0 - x2 "
"f - 'Coord_t' 0 - y2 C - - 0 0 offMethod",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ExecuteEvent",1237,G__TSwitch_ExecuteEvent_3_0,121,-1,-1,0,3,1,1,0,
"i - 'Int_t' 0 - event i - 'Int_t' 0 - px "
"i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetButton",924,G__TSwitch_GetButton_4_0,85,G__get_linked_tagnum(&G__AMSCintLN_TRadioButton),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileName",1145,G__TSwitch_DeclFileName_5_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__TSwitch_DeclFileLine_6_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__TSwitch_ImplFileName_7_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__TSwitch_ImplFileLine_8_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__TSwitch_Class_Version_9_0,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__TSwitch_Class_0_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__TSwitch_Dictionary_1_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__TSwitch_IsA_2_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__TSwitch_ShowMembers_3_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__TSwitch_Streamer_4_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("TSwitch",710,G__TSwitch_TSwitch_5_1,(int)('i'),G__get_linked_tagnum(&G__AMSCintLN_TSwitch),-1,0,1,1,1,0,"u 'TSwitch' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~TSwitch",836,G__TSwitch_wATSwitch_6_1,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSAxis(void) {
   /* AMSAxis */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSAxis));
   G__memfunc_setup("AMSAxis",630,G__AMSAxis_AMSAxis_0_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSAxis),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSAxis",630,G__AMSAxis_AMSAxis_1_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSAxis),-1,0,2,1,1,0,
"F - 'Float_t' 0 - end C - - 0 - title",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSAxis",630,G__AMSAxis_AMSAxis_2_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSAxis),-1,0,4,1,1,0,
"f - 'Float_t' 0 - x f - 'Float_t' 0 - y "
"f - 'Float_t' 0 - z C - - 0 - title",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetObjectInfo",1283,G__AMSAxis_GetObjectInfo_4_0,67,-1,-1,0,2,1,1,0,
"i - 'Int_t' 0 - px i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Paint",508,G__AMSAxis_Paint_5_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DeclFileName",1145,G__AMSAxis_DeclFileName_6_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSAxis_DeclFileLine_7_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSAxis_ImplFileName_8_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSAxis_ImplFileLine_9_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSAxis_Class_Version_0_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSAxis_Class_1_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSAxis_Dictionary_2_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSAxis_IsA_3_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSAxis_ShowMembers_4_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSAxis_Streamer_5_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSAxis",630,G__AMSAxis_AMSAxis_6_1,(int)('i'),G__get_linked_tagnum(&G__AMSCintLN_AMSAxis),-1,0,1,1,1,0,"u 'AMSAxis' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSAxis",756,G__AMSAxis_wAAMSAxis_7_1,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSDisplay(void) {
   /* AMSDisplay */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSDisplay));
   G__memfunc_setup("AMSDisplay",951,G__AMSDisplay_AMSDisplay_0_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSDisplay),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSDisplay",951,G__AMSDisplay_AMSDisplay_1_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSDisplay),-1,0,2,1,1,0,
"C - - 0 - title U 'TGeometry' - 0 0 geo",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AllViews",807,G__AMSDisplay_AllViews_3_0,98,-1,G__defined_typename("Bool_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("GetView",699,G__AMSDisplay_GetView_4_0,105,G__get_linked_tagnum(&G__AMSCintLN_EAMSView),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Clear",487,G__AMSDisplay_Clear_5_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DisplayButtons",1477,G__AMSDisplay_DisplayButtons_6_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DistancetoPrimitive",1991,G__AMSDisplay_DistancetoPrimitive_7_0,105,-1,G__defined_typename("Int_t"),0,2,1,1,0,
"i - 'Int_t' 0 - px i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Draw",398,G__AMSDisplay_Draw_8_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DrawAllViews",1205,G__AMSDisplay_DrawAllViews_9_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DrawFrontAndSideViews",2109,G__AMSDisplay_DrawFrontAndSideViews_0_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DrawParticles",1333,G__AMSDisplay_DrawParticles_1_1,98,-1,G__defined_typename("Bool_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DrawTitle",912,G__AMSDisplay_DrawTitle_2_1,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DrawEventInfo",1308,G__AMSDisplay_DrawEventInfo_3_1,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DrawCaption",1116,G__AMSDisplay_DrawCaption_4_1,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DrawAxis",803,G__AMSDisplay_DrawAxis_5_1,121,-1,-1,0,2,1,1,0,
"i - 'Int_t' 0 0 index C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DrawView",809,G__AMSDisplay_DrawView_6_1,121,-1,-1,0,3,1,1,0,
"f - 'Float_t' 0 9999 theta f - 'Float_t' 0 9999 phi "
"i - 'Int_t' 0 0 index",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DrawViewGL",956,G__AMSDisplay_DrawViewGL_7_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DrawViewX3D",1016,G__AMSDisplay_DrawViewX3D_8_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ExecuteEvent",1237,G__AMSDisplay_ExecuteEvent_9_1,121,-1,-1,0,3,1,1,0,
"i - 'Int_t' 0 - event i - 'Int_t' 0 - px "
"i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("GetEvent",802,G__AMSDisplay_GetEvent_0_2,121,-1,-1,0,1,1,1,0,"i - 'Int_t' 0 - event","*MENU*",(void*)NULL,1);
   G__memfunc_setup("Pad",277,G__AMSDisplay_Pad_1_2,85,G__get_linked_tagnum(&G__AMSCintLN_TPad),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetCanvas",892,G__AMSDisplay_GetCanvas_2_2,85,G__get_linked_tagnum(&G__AMSCintLN_AMSCanvas),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetObjInfoPad",1244,G__AMSDisplay_GetObjInfoPad_3_2,85,G__get_linked_tagnum(&G__AMSCintLN_TPad),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetEventInfoPad",1475,G__AMSDisplay_GetEventInfoPad_4_2,85,G__get_linked_tagnum(&G__AMSCintLN_TPad),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetTitlePad",1079,G__AMSDisplay_GetTitlePad_5_2,85,G__get_linked_tagnum(&G__AMSCintLN_TPad),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Paint",508,G__AMSDisplay_Paint_6_2,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("PaintParticles",1443,G__AMSDisplay_PaintParticles_7_2,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("PTcut",496,G__AMSDisplay_PTcut_8_2,102,-1,G__defined_typename("Float_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("PTcutEGMUNU",961,G__AMSDisplay_PTcutEGMUNU_9_2,102,-1,G__defined_typename("Float_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("GetObjectInfo",1283,G__AMSDisplay_GetObjectInfo_0_3,67,-1,-1,0,2,1,1,0,
"i - 'Int_t' 0 - px i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SetDrawParticles",1633,G__AMSDisplay_SetDrawParticles_1_3,121,-1,-1,0,1,1,1,0,"b - 'Bool_t' 0 kTRUE draw","*MENU*",(void*)NULL,1);
   G__memfunc_setup("SetPTcut",796,G__AMSDisplay_SetPTcut_2_3,121,-1,-1,0,1,1,1,0,"f - 'Float_t' 0 0.4 ptcut","*MENU*",(void*)NULL,1);
   G__memfunc_setup("SetPTcutEGMUNU",1261,G__AMSDisplay_SetPTcutEGMUNU_3_3,121,-1,-1,0,1,1,1,0,"f - 'Float_t' 0 5 ptcut","*MENU*",(void*)NULL,1);
   G__memfunc_setup("SetGeometry",1144,G__AMSDisplay_SetGeometry_4_3,121,-1,-1,0,1,1,1,0,"U 'TGeometry' - 0 - geo","*--MENU*",(void*)NULL,1);
   G__memfunc_setup("SetView",711,G__AMSDisplay_SetView_5_3,121,-1,-1,0,2,1,1,0,
"f - 'Float_t' 0 - theta f - 'Float_t' 0 - phi",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SetView",711,G__AMSDisplay_SetView_6_3,121,-1,-1,0,1,1,1,0,"i 'EAMSView' - 0 kFrontView newView",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowNextEvent",1346,G__AMSDisplay_ShowNextEvent_7_3,121,-1,-1,0,1,1,1,0,"i - 'Int_t' 0 1 delta",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SizeParticles",1346,G__AMSDisplay_SizeParticles_8_3,121,-1,-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("GetGeometrySetter",1763,G__AMSDisplay_GetGeometrySetter_9_3,85,G__get_linked_tagnum(&G__AMSCintLN_AMSGeometrySetter),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("SetGeo",583,G__AMSDisplay_SetGeo_0_4,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileName",1145,G__AMSDisplay_DeclFileName_1_4,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSDisplay_DeclFileLine_2_4,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSDisplay_ImplFileName_3_4,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSDisplay_ImplFileLine_4_4,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSDisplay_Class_Version_5_4,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSDisplay_Class_6_4,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSDisplay_Dictionary_7_4,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSDisplay_IsA_8_4,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSDisplay_ShowMembers_9_4,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSDisplay_Streamer_0_5,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSDisplay",951,G__AMSDisplay_AMSDisplay_1_5,(int)('i'),G__get_linked_tagnum(&G__AMSCintLN_AMSDisplay),-1,0,1,1,1,0,"u 'AMSDisplay' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSDisplay",1077,G__AMSDisplay_wAAMSDisplay_2_5,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMS3DCluster(void) {
   /* AMS3DCluster */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSCintLN_AMS3DCluster));
   G__memfunc_setup("AMS3DCluster",1082,G__AMS3DCluster_AMS3DCluster_0_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMS3DCluster),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMS3DCluster",1082,G__AMS3DCluster_AMS3DCluster_1_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMS3DCluster),-1,0,9,1,1,0,
"f - 'Float_t' 0 - x f - 'Float_t' 0 - y "
"f - 'Float_t' 0 - z f - 'Float_t' 0 - dx "
"f - 'Float_t' 0 - dy f - 'Float_t' 0 - dz "
"f - 'Float_t' 0 - theta f - 'Float_t' 0 - phi "
"i - 'Int_t' 0 1 range",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMS3DCluster",1082,G__AMS3DCluster_AMS3DCluster_2_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMS3DCluster),-1,0,4,1,1,0,
"F - 'Float_t' 0 - xyz F - 'Float_t' 0 - dxyz "
"F - 'Float_t' 0 - cos i - 'Int_t' 0 5 range",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Delete",595,G__AMS3DCluster_Delete_4_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("GetObjectInfo",1283,G__AMS3DCluster_GetObjectInfo_5_0,67,-1,-1,0,2,1,1,0,
"i - 'Int_t' 0 - px i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("GetPosition",1141,G__AMS3DCluster_GetPosition_6_0,70,-1,G__defined_typename("Float_t"),0,0,1,1,4,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetSize",699,G__AMS3DCluster_GetSize_7_0,70,-1,G__defined_typename("Float_t"),0,0,1,1,4,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DistancetoPrimitive",1991,G__AMS3DCluster_DistancetoPrimitive_8_0,105,-1,G__defined_typename("Int_t"),0,2,1,1,0,
"i - 'Int_t' 0 - px i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("SetPoints",937,G__AMS3DCluster_SetPoints_9_0,121,-1,-1,0,1,1,1,0,"F - 'Float_t' 0 - buff",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("SetPosition",1153,G__AMS3DCluster_SetPosition_0_1,121,-1,-1,0,3,1,1,0,
"f - 'Float_t' 0 - x f - 'Float_t' 0 - y "
"f - 'Float_t' 0 - z",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("SetDirection",1229,G__AMS3DCluster_SetDirection_1_1,121,-1,-1,0,2,1,1,0,
"f - 'Float_t' 0 - theta f - 'Float_t' 0 - phi",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Sizeof3D",743,G__AMS3DCluster_Sizeof3D_2_1,121,-1,-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileName",1145,G__AMS3DCluster_DeclFileName_3_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMS3DCluster_DeclFileLine_4_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMS3DCluster_ImplFileName_5_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMS3DCluster_ImplFileLine_6_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMS3DCluster_Class_Version_7_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMS3DCluster_Class_8_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMS3DCluster_Dictionary_9_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMS3DCluster_IsA_0_2,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMS3DCluster_ShowMembers_1_2,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMS3DCluster_Streamer_2_2,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMS3DCluster",1082,G__AMS3DCluster_AMS3DCluster_3_2,(int)('i'),G__get_linked_tagnum(&G__AMSCintLN_AMS3DCluster),-1,0,1,1,1,0,"u 'AMS3DCluster' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMS3DCluster",1208,G__AMS3DCluster_wAAMS3DCluster_4_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSMaker(void) {
   /* AMSMaker */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSMaker));
   G__memfunc_setup("Init",404,G__AMSMaker_Init_3_0,121,-1,-1,0,1,1,1,8,"U 'TTree' - 0 0 t","interface",(void*)NULL,3);
   G__memfunc_setup("Finish",609,G__AMSMaker_Finish_4_0,121,-1,-1,0,0,1,1,8,"","interface",(void*)NULL,3);
   G__memfunc_setup("Browse",626,G__AMSMaker_Browse_5_0,121,-1,-1,0,1,1,1,0,"U 'TBrowser' - 0 - b",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Draw",398,G__AMSMaker_Draw_6_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Clear",487,G__AMSMaker_Clear_7_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("IsFolder",792,G__AMSMaker_IsFolder_8_0,98,-1,G__defined_typename("Bool_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Histograms",1057,G__AMSMaker_Histograms_9_0,85,G__get_linked_tagnum(&G__AMSCintLN_TList),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Fruits",637,G__AMSMaker_Fruits_0_1,85,G__get_linked_tagnum(&G__AMSCintLN_TObject),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("PrintInfo",921,G__AMSMaker_PrintInfo_1_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Enabled",683,G__AMSMaker_Enabled_2_1,98,-1,G__defined_typename("Bool_t"),0,1,1,1,0,"U 'TObject' - 0 - obj","object enabled for drawing?",(void*)NULL,1);
   G__memfunc_setup("Save",399,G__AMSMaker_Save_3_1,121,-1,-1,0,1,1,1,0,"i - 'Int_t' 0 1 save",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("MakeBranch",972,G__AMSMaker_MakeBranch_4_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Make",382,G__AMSMaker_Make_5_1,121,-1,-1,0,0,1,1,8,"","interface",(void*)NULL,3);
   G__memfunc_setup("DeclFileName",1145,G__AMSMaker_DeclFileName_6_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSMaker_DeclFileLine_7_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSMaker_ImplFileName_8_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSMaker_ImplFileLine_9_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSMaker_Class_Version_0_2,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSMaker_Class_1_2,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSMaker_Dictionary_2_2,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSMaker_IsA_3_2,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSMaker_ShowMembers_4_2,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSMaker_Streamer_5_2,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic destructor
   G__memfunc_setup("~AMSMaker",847,G__AMSMaker_wAAMSMaker_6_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSHistBrowser(void) {
   /* AMSHistBrowser */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSHistBrowser));
   G__memfunc_setup("AMSHistBrowser",1373,G__AMSHistBrowser_AMSHistBrowser_0_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSHistBrowser),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Browse",626,G__AMSHistBrowser_Browse_2_0,121,-1,-1,0,1,1,1,0,"U 'TBrowser' - 0 - b",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("IsFolder",792,G__AMSHistBrowser_IsFolder_3_0,98,-1,G__defined_typename("Bool_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileName",1145,G__AMSHistBrowser_DeclFileName_4_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSHistBrowser_DeclFileLine_5_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSHistBrowser_ImplFileName_6_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSHistBrowser_ImplFileLine_7_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSHistBrowser_Class_Version_8_0,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSHistBrowser_Class_9_0,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSHistBrowser_Dictionary_0_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSHistBrowser_IsA_1_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSHistBrowser_ShowMembers_2_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSHistBrowser_Streamer_3_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSHistBrowser",1373,G__AMSHistBrowser_AMSHistBrowser_4_1,(int)('i'),G__get_linked_tagnum(&G__AMSCintLN_AMSHistBrowser),-1,0,1,1,1,0,"u 'AMSHistBrowser' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSHistBrowser",1499,G__AMSHistBrowser_wAAMSHistBrowser_5_1,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSToFClusterReader(void) {
   /* AMSToFClusterReader */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSToFClusterReader));
   G__memfunc_setup("AMSToFClusterReader",1823,G__AMSToFClusterReader_AMSToFClusterReader_0_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSToFClusterReader),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSToFClusterReader",1823,G__AMSToFClusterReader_AMSToFClusterReader_1_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSToFClusterReader),-1,0,2,1,1,0,
"C - - 0 - name C - - 0 - title",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Init",404,G__AMSToFClusterReader_Init_3_0,121,-1,-1,0,1,1,1,0,"U 'TTree' - 0 0 tree",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Finish",609,G__AMSToFClusterReader_Finish_4_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Make",382,G__AMSToFClusterReader_Make_5_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("PrintInfo",921,G__AMSToFClusterReader_PrintInfo_6_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("AddCluster",1003,G__AMSToFClusterReader_AddCluster_7_0,121,-1,-1,0,12,1,1,0,
"i - 'Int_t' 0 - status i - 'Int_t' 0 - plane "
"i - 'Int_t' 0 - bar f - 'Float_t' 0 - energy "
"f - 'Float_t' 0 - time f - 'Float_t' 0 - ertime "
"F - 'Float_t' 0 - coo F - 'Float_t' 0 - ercoo "
"i - 'Int_t' 0 0 ncells i - 'Int_t' 0 0 npart "
"i - 'Int_t' 0 0 ntracks U 'TObjArray' - 0 0 tracks",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("RemoveCluster",1360,G__AMSToFClusterReader_RemoveCluster_8_0,121,-1,-1,0,1,1,1,0,"i - 'Int_t' 0 - cluster",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Clear",487,G__AMSToFClusterReader_Clear_9_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileName",1145,G__AMSToFClusterReader_DeclFileName_0_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSToFClusterReader_DeclFileLine_1_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSToFClusterReader_ImplFileName_2_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSToFClusterReader_ImplFileLine_3_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSToFClusterReader_Class_Version_4_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSToFClusterReader_Class_5_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSToFClusterReader_Dictionary_6_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSToFClusterReader_IsA_7_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSToFClusterReader_ShowMembers_8_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSToFClusterReader_Streamer_9_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSToFClusterReader",1823,G__AMSToFClusterReader_AMSToFClusterReader_0_2,(int)('i'),G__get_linked_tagnum(&G__AMSCintLN_AMSToFClusterReader),-1,0,1,1,1,0,"u 'AMSToFClusterReader' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSToFClusterReader",1949,G__AMSToFClusterReader_wAAMSToFClusterReader_1_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSTrackReader(void) {
   /* AMSTrackReader */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSTrackReader));
   G__memfunc_setup("AMSTrackReader",1321,G__AMSTrackReader_AMSTrackReader_0_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSTrackReader),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSTrackReader",1321,G__AMSTrackReader_AMSTrackReader_1_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSTrackReader),-1,0,2,1,1,0,
"C - - 0 - name C - - 0 - title",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AddTrack",766,G__AMSTrackReader_AddTrack_3_0,85,G__get_linked_tagnum(&G__AMSCintLN_AMSTrack),-1,0,2,1,1,0,
"i - 'Int_t' 0 - code i - 'Int_t' 0 - mcparticle",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Clear",487,G__AMSTrackReader_Clear_4_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Finish",609,G__AMSTrackReader_Finish_5_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Init",404,G__AMSTrackReader_Init_6_0,121,-1,-1,0,1,1,1,0,"U 'TTree' - 0 0 h1",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Make",382,G__AMSTrackReader_Make_7_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("PrintInfo",921,G__AMSTrackReader_PrintInfo_8_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Mult",418,G__AMSTrackReader_Mult_9_0,85,G__get_linked_tagnum(&G__AMSCintLN_TH1F),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileName",1145,G__AMSTrackReader_DeclFileName_0_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSTrackReader_DeclFileLine_1_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSTrackReader_ImplFileName_2_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSTrackReader_ImplFileLine_3_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSTrackReader_Class_Version_4_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSTrackReader_Class_5_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSTrackReader_Dictionary_6_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSTrackReader_IsA_7_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSTrackReader_ShowMembers_8_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSTrackReader_Streamer_9_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSTrackReader",1321,G__AMSTrackReader_AMSTrackReader_0_2,(int)('i'),G__get_linked_tagnum(&G__AMSCintLN_AMSTrackReader),-1,0,1,1,1,0,"u 'AMSTrackReader' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSTrackReader",1447,G__AMSTrackReader_wAAMSTrackReader_1_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSSiHitReader(void) {
   /* AMSSiHitReader */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSSiHitReader));
   G__memfunc_setup("AMSSiHitReader",1301,G__AMSSiHitReader_AMSSiHitReader_0_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSSiHitReader),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSSiHitReader",1301,G__AMSSiHitReader_AMSSiHitReader_1_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSSiHitReader),-1,0,2,1,1,0,
"C - - 0 - name C - - 0 - title",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Init",404,G__AMSSiHitReader_Init_3_0,121,-1,-1,0,1,1,1,0,"U 'TTree' - 0 0 tree",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Finish",609,G__AMSSiHitReader_Finish_4_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Make",382,G__AMSSiHitReader_Make_5_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("PrintInfo",921,G__AMSSiHitReader_PrintInfo_6_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Enabled",683,G__AMSSiHitReader_Enabled_7_0,98,-1,G__defined_typename("Bool_t"),0,1,1,1,0,"U 'TObject' - 0 - obj",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("AddCluster",1003,G__AMSSiHitReader_AddCluster_8_0,121,-1,-1,0,9,1,1,0,
"i - 'Int_t' 0 - status i - 'Int_t' 0 - plane "
"i - 'Int_t' 0 - px i - 'Int_t' 0 - py "
"F - 'Float_t' 0 - hit F - 'Float_t' 0 - errhit "
"f - 'Float_t' 0 - ampl f - 'Float_t' 0 - asym "
"U 'TObjArray' - 0 0 tracks",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("RemoveCluster",1360,G__AMSSiHitReader_RemoveCluster_9_0,121,-1,-1,0,1,1,1,0,"i - 'Int_t' 0 - cluster",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Clear",487,G__AMSSiHitReader_Clear_0_1,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileName",1145,G__AMSSiHitReader_DeclFileName_1_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSSiHitReader_DeclFileLine_2_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSSiHitReader_ImplFileName_3_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSSiHitReader_ImplFileLine_4_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSSiHitReader_Class_Version_5_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSSiHitReader_Class_6_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSSiHitReader_Dictionary_7_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSSiHitReader_IsA_8_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSSiHitReader_ShowMembers_9_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSSiHitReader_Streamer_0_2,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSSiHitReader",1301,G__AMSSiHitReader_AMSSiHitReader_1_2,(int)('i'),G__get_linked_tagnum(&G__AMSCintLN_AMSSiHitReader),-1,0,1,1,1,0,"u 'AMSSiHitReader' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSSiHitReader",1427,G__AMSSiHitReader_wAAMSSiHitReader_2_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSCTCClusterReader(void) {
   /* AMSCTCClusterReader */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSCTCClusterReader));
   G__memfunc_setup("AMSCTCClusterReader",1776,G__AMSCTCClusterReader_AMSCTCClusterReader_0_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSCTCClusterReader),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSCTCClusterReader",1776,G__AMSCTCClusterReader_AMSCTCClusterReader_1_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSCTCClusterReader),-1,0,2,1,1,0,
"C - - 0 - name C - - 0 - title",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Init",404,G__AMSCTCClusterReader_Init_3_0,121,-1,-1,0,1,1,1,0,"U 'TTree' - 0 0 tree",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Finish",609,G__AMSCTCClusterReader_Finish_4_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Make",382,G__AMSCTCClusterReader_Make_5_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("PrintInfo",921,G__AMSCTCClusterReader_PrintInfo_6_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("AddCluster",1003,G__AMSCTCClusterReader_AddCluster_7_0,121,-1,-1,0,9,1,1,0,
"i - 'Int_t' 0 - status i - 'Int_t' 0 - plane "
"f - 'Float_t' 0 - rawsignal f - 'Float_t' 0 - signal "
"f - 'Float_t' 0 - ersig F - 'Float_t' 0 - coo "
"F - 'Float_t' 0 - ercoo i - 'Int_t' 0 0 ntracks "
"U 'TObjArray' - 0 0 tracks",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("RemoveCluster",1360,G__AMSCTCClusterReader_RemoveCluster_8_0,121,-1,-1,0,1,1,1,0,"i - 'Int_t' 0 - cluster",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Clear",487,G__AMSCTCClusterReader_Clear_9_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileName",1145,G__AMSCTCClusterReader_DeclFileName_0_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSCTCClusterReader_DeclFileLine_1_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSCTCClusterReader_ImplFileName_2_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSCTCClusterReader_ImplFileLine_3_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSCTCClusterReader_Class_Version_4_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSCTCClusterReader_Class_5_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSCTCClusterReader_Dictionary_6_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSCTCClusterReader_IsA_7_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSCTCClusterReader_ShowMembers_8_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSCTCClusterReader_Streamer_9_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSCTCClusterReader",1776,G__AMSCTCClusterReader_AMSCTCClusterReader_0_2,(int)('i'),G__get_linked_tagnum(&G__AMSCintLN_AMSCTCClusterReader),-1,0,1,1,1,0,"u 'AMSCTCClusterReader' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSCTCClusterReader",1902,G__AMSCTCClusterReader_wAAMSCTCClusterReader_1_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSAntiClusterReader(void) {
   /* AMSAntiClusterReader */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSAntiClusterReader));
   G__memfunc_setup("AMSAntiClusterReader",1954,G__AMSAntiClusterReader_AMSAntiClusterReader_0_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSAntiClusterReader),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSAntiClusterReader",1954,G__AMSAntiClusterReader_AMSAntiClusterReader_1_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSAntiClusterReader),-1,0,2,1,1,0,
"C - - 0 - name C - - 0 - title",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Init",404,G__AMSAntiClusterReader_Init_3_0,121,-1,-1,0,1,1,1,0,"U 'TTree' - 0 0 tree",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Finish",609,G__AMSAntiClusterReader_Finish_4_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Make",382,G__AMSAntiClusterReader_Make_5_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("PrintInfo",921,G__AMSAntiClusterReader_PrintInfo_6_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("AddCluster",1003,G__AMSAntiClusterReader_AddCluster_7_0,121,-1,-1,0,7,1,1,0,
"i - 'Int_t' 0 - status i - 'Int_t' 0 - sector "
"f - 'Float_t' 0 - signal F - 'Float_t' 0 - coo "
"F - 'Float_t' 0 - ercoo i - 'Int_t' 0 0 ntracks "
"U 'TObjArray' - 0 0 tracks",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("RemoveCluster",1360,G__AMSAntiClusterReader_RemoveCluster_8_0,121,-1,-1,0,1,1,1,0,"i - 'Int_t' 0 - cluster",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Clear",487,G__AMSAntiClusterReader_Clear_9_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileName",1145,G__AMSAntiClusterReader_DeclFileName_0_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSAntiClusterReader_DeclFileLine_1_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSAntiClusterReader_ImplFileName_2_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSAntiClusterReader_ImplFileLine_3_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSAntiClusterReader_Class_Version_4_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSAntiClusterReader_Class_5_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSAntiClusterReader_Dictionary_6_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSAntiClusterReader_IsA_7_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSAntiClusterReader_ShowMembers_8_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSAntiClusterReader_Streamer_9_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSAntiClusterReader",1954,G__AMSAntiClusterReader_AMSAntiClusterReader_0_2,(int)('i'),G__get_linked_tagnum(&G__AMSCintLN_AMSAntiClusterReader),-1,0,1,1,1,0,"u 'AMSAntiClusterReader' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSAntiClusterReader",2080,G__AMSAntiClusterReader_wAAMSAntiClusterReader_1_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSParticleReader(void) {
   /* AMSParticleReader */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSParticleReader));
   G__memfunc_setup("AMSParticleReader",1640,G__AMSParticleReader_AMSParticleReader_0_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSParticleReader),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSParticleReader",1640,G__AMSParticleReader_AMSParticleReader_1_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSParticleReader),-1,0,2,1,1,0,
"C - - 0 - name C - - 0 - title",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AddParticle",1085,G__AMSParticleReader_AddParticle_3_0,85,G__get_linked_tagnum(&G__AMSCintLN_AMSParticle),-1,0,2,1,1,0,
"i - 'Int_t' 0 - code i - 'Int_t' 0 - mcparticle",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Clear",487,G__AMSParticleReader_Clear_4_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Finish",609,G__AMSParticleReader_Finish_5_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Init",404,G__AMSParticleReader_Init_6_0,121,-1,-1,0,1,1,1,0,"U 'TTree' - 0 0 h1",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Make",382,G__AMSParticleReader_Make_7_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("PrintInfo",921,G__AMSParticleReader_PrintInfo_8_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Mult",418,G__AMSParticleReader_Mult_9_0,85,G__get_linked_tagnum(&G__AMSCintLN_TH1F),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileName",1145,G__AMSParticleReader_DeclFileName_0_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSParticleReader_DeclFileLine_1_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSParticleReader_ImplFileName_2_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSParticleReader_ImplFileLine_3_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSParticleReader_Class_Version_4_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSParticleReader_Class_5_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSParticleReader_Dictionary_6_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSParticleReader_IsA_7_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSParticleReader_ShowMembers_8_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSParticleReader_Streamer_9_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSParticleReader",1640,G__AMSParticleReader_AMSParticleReader_0_2,(int)('i'),G__get_linked_tagnum(&G__AMSCintLN_AMSParticleReader),-1,0,1,1,1,0,"u 'AMSParticleReader' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSParticleReader",1766,G__AMSParticleReader_wAAMSParticleReader_1_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSVirtualDisplay(void) {
   /* AMSVirtualDisplay */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSVirtualDisplay));
   G__memfunc_setup("Clear",487,G__AMSVirtualDisplay_Clear_2_0,121,-1,-1,0,1,1,1,8,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("DisplayButtons",1477,G__AMSVirtualDisplay_DisplayButtons_3_0,121,-1,-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("DistancetoPrimitive",1991,G__AMSVirtualDisplay_DistancetoPrimitive_4_0,105,-1,G__defined_typename("Int_t"),0,2,1,1,8,
"i - 'Int_t' 0 - px i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("Draw",398,G__AMSVirtualDisplay_Draw_5_0,121,-1,-1,0,1,1,1,8,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("DrawAllViews",1205,G__AMSVirtualDisplay_DrawAllViews_6_0,121,-1,-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("DrawFrontAndSideViews",2109,G__AMSVirtualDisplay_DrawFrontAndSideViews_7_0,121,-1,-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("DrawParticles",1333,G__AMSVirtualDisplay_DrawParticles_8_0,98,-1,G__defined_typename("Bool_t"),0,0,1,1,8,"",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("DrawCaption",1116,G__AMSVirtualDisplay_DrawCaption_9_0,121,-1,-1,0,1,1,1,8,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("DrawView",809,G__AMSVirtualDisplay_DrawView_0_1,121,-1,-1,0,3,1,1,8,
"f - 'Float_t' 0 - theta f - 'Float_t' 0 - phi "
"i - 'Int_t' 0 - index",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("DrawViewGL",956,G__AMSVirtualDisplay_DrawViewGL_1_1,121,-1,-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("DrawViewX3D",1016,G__AMSVirtualDisplay_DrawViewX3D_2_1,121,-1,-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("ExecuteEvent",1237,G__AMSVirtualDisplay_ExecuteEvent_3_1,121,-1,-1,0,3,1,1,8,
"i - 'Int_t' 0 - event i - 'Int_t' 0 - px "
"i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("Paint",508,G__AMSVirtualDisplay_Paint_4_1,121,-1,-1,0,1,1,1,8,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("PaintParticles",1443,G__AMSVirtualDisplay_PaintParticles_5_1,121,-1,-1,0,1,1,1,8,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("PTcut",496,G__AMSVirtualDisplay_PTcut_6_1,102,-1,G__defined_typename("Float_t"),0,0,1,1,8,"",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("PTcutEGMUNU",961,G__AMSVirtualDisplay_PTcutEGMUNU_7_1,102,-1,G__defined_typename("Float_t"),0,0,1,1,8,"",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("SetView",711,G__AMSVirtualDisplay_SetView_8_1,121,-1,-1,0,2,1,1,8,
"f - 'Float_t' 0 - theta f - 'Float_t' 0 - phi",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("SetView",711,G__AMSVirtualDisplay_SetView_9_1,121,-1,-1,0,1,1,1,8,"i 'EAMSView' - 0 kFrontView newView",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("ShowNextEvent",1346,G__AMSVirtualDisplay_ShowNextEvent_0_2,121,-1,-1,0,1,1,1,8,"i - 'Int_t' 0 1 delta",(char*)NULL,(void*)NULL,3);
   G__memfunc_setup("SizeParticles",1346,G__AMSVirtualDisplay_SizeParticles_1_2,121,-1,-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DeclFileName",1145,G__AMSVirtualDisplay_DeclFileName_2_2,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSVirtualDisplay_DeclFileLine_3_2,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSVirtualDisplay_ImplFileName_4_2,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSVirtualDisplay_ImplFileLine_5_2,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSVirtualDisplay_Class_Version_6_2,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSVirtualDisplay_Class_7_2,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSVirtualDisplay_Dictionary_8_2,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSVirtualDisplay_IsA_9_2,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSVirtualDisplay_ShowMembers_0_3,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSVirtualDisplay_Streamer_1_3,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic destructor
   G__memfunc_setup("~AMSVirtualDisplay",1820,G__AMSVirtualDisplay_wAAMSVirtualDisplay_2_3,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSRoot(void) {
   /* AMSRoot */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSRoot));
   G__memfunc_setup("CommonConstruct",1582,(G__InterfaceMethod)NULL,121,-1,-1,0,0,1,4,0,"","Common part of all constructors",(void*)NULL,0);
   G__memfunc_setup("AMSRoot",645,G__AMSRoot_AMSRoot_1_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSRoot),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSRoot",645,G__AMSRoot_AMSRoot_2_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSRoot),-1,0,2,1,1,0,
"C - - 0 - name C - - 0 \"The AMS Display with Root\" title",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Browse",626,G__AMSRoot_Browse_4_0,121,-1,-1,0,1,1,1,0,"U 'TBrowser' - 0 - b",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("GetEvent",802,G__AMSRoot_GetEvent_5_0,121,-1,-1,0,1,1,1,0,"i - 'Int_t' 0 1 event","*MENU*",(void*)NULL,1);
   G__memfunc_setup("SelectEvent",1122,G__AMSRoot_SelectEvent_6_0,121,-1,-1,0,0,1,1,0,"","*MENU*",(void*)NULL,1);
   G__memfunc_setup("Init",404,G__AMSRoot_Init_7_0,121,-1,-1,0,1,1,1,0,"U 'TTree' - 0 0 h1","Initialize to read from h1",(void*)NULL,1);
   G__memfunc_setup("Finish",609,G__AMSRoot_Finish_8_0,121,-1,-1,0,0,1,1,0,"","Finish a run",(void*)NULL,1);
   G__memfunc_setup("Display",726,G__AMSRoot_Display_9_0,85,G__get_linked_tagnum(&G__AMSCintLN_AMSVirtualDisplay),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Draw",398,G__AMSRoot_Draw_0_1,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option","*MENU*",(void*)NULL,1);
   G__memfunc_setup("Paint",508,G__AMSRoot_Paint_1_1,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Clear",487,G__AMSRoot_Clear_2_1,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("IsFolder",792,G__AMSRoot_IsFolder_3_1,98,-1,G__defined_typename("Bool_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("PrintInfo",921,G__AMSRoot_PrintInfo_4_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("GetVersion",1030,G__AMSRoot_GetVersion_5_1,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetVersionDate",1412,G__AMSRoot_GetVersionDate_6_1,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("RunNum",613,G__AMSRoot_RunNum_7_1,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("EventNum",818,G__AMSRoot_EventNum_8_1,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Event",514,G__AMSRoot_Event_9_1,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Mode",389,G__AMSRoot_Mode_0_2,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Tree",400,G__AMSRoot_Tree_1_2,85,G__get_linked_tagnum(&G__AMSCintLN_TTree),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetInput",816,G__AMSRoot_GetInput_2_2,85,G__get_linked_tagnum(&G__AMSCintLN_TTree),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Makers",611,G__AMSRoot_Makers_3_2,85,G__get_linked_tagnum(&G__AMSCintLN_TList),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Maker",496,G__AMSRoot_Maker_4_2,85,G__get_linked_tagnum(&G__AMSCintLN_AMSMaker),-1,0,1,1,1,0,"C - - 0 - name",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ToFClusterMaker",1499,G__AMSRoot_ToFClusterMaker_5_2,85,G__get_linked_tagnum(&G__AMSCintLN_AMSToFClusterReader),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("TrackMaker",997,G__AMSRoot_TrackMaker_6_2,85,G__get_linked_tagnum(&G__AMSCintLN_AMSTrackReader),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("SiHitMaker",977,G__AMSRoot_SiHitMaker_7_2,85,G__get_linked_tagnum(&G__AMSCintLN_AMSSiHitReader),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AntiClusterMaker",1630,G__AMSRoot_AntiClusterMaker_8_2,85,G__get_linked_tagnum(&G__AMSCintLN_AMSAntiClusterReader),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("CTCClusterMaker",1452,G__AMSRoot_CTCClusterMaker_9_2,85,G__get_linked_tagnum(&G__AMSCintLN_AMSCTCClusterReader),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ParticleMaker",1316,G__AMSRoot_ParticleMaker_0_3,85,G__get_linked_tagnum(&G__AMSCintLN_AMSParticleReader),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("SetDefaultParameters",2053,G__AMSRoot_SetDefaultParameters_1_3,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SetEvent",814,G__AMSRoot_SetEvent_2_3,121,-1,-1,0,1,1,1,0,"i - 'Int_t' 0 1 event",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SetMode",689,G__AMSRoot_SetMode_3_3,121,-1,-1,0,1,1,1,0,"i - 'Int_t' 0 0 mode",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SetDisplay",1026,G__AMSRoot_SetDisplay_4_3,121,-1,-1,0,1,1,1,0,"U 'AMSVirtualDisplay' - 0 - display",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Make",382,G__AMSRoot_Make_5_3,121,-1,-1,0,1,1,1,0,"i - 'Int_t' 0 0 i",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("MakeTree",782,G__AMSRoot_MakeTree_6_3,121,-1,-1,0,2,1,1,0,
"C - - 0 \"AMS Event\" name C - - 0 \"AMSRoot tree\" title",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("FillTree",791,G__AMSRoot_FillTree_7_3,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("SortDown",832,G__AMSRoot_SortDown_8_3,121,-1,-1,0,4,1,1,0,
"i - 'Int_t' 0 - n F - 'Float_t' 0 - a "
"I - 'Int_t' 0 - index b - 'Bool_t' 0 kTRUE down",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileName",1145,G__AMSRoot_DeclFileName_9_3,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSRoot_DeclFileLine_0_4,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSRoot_ImplFileName_1_4,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSRoot_ImplFileLine_2_4,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSRoot_Class_Version_3_4,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSRoot_Class_4_4,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSRoot_Dictionary_5_4,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSRoot_IsA_6_4,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSRoot_ShowMembers_7_4,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSRoot_Streamer_8_4,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSRoot",645,G__AMSRoot_AMSRoot_9_4,(int)('i'),G__get_linked_tagnum(&G__AMSCintLN_AMSRoot),-1,0,1,1,1,0,"u 'AMSRoot' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSRoot",771,G__AMSRoot_wAAMSRoot_0_5,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSToFCluster(void) {
   /* AMSToFCluster */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSToFCluster));
   G__memfunc_setup("AMSToFCluster",1228,G__AMSToFCluster_AMSToFCluster_0_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSToFCluster),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSToFCluster",1228,G__AMSToFCluster_AMSToFCluster_1_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSToFCluster),-1,0,12,1,1,0,
"i - 'Int_t' 0 - status i - 'Int_t' 0 - plane "
"i - 'Int_t' 0 - bar f - 'Float_t' 0 - energy "
"f - 'Float_t' 0 - time f - 'Float_t' 0 - ertime "
"F - 'Float_t' 0 - coo F - 'Float_t' 0 - ercoo "
"i - 'Int_t' 0 - ncells i - 'Int_t' 0 - npart "
"i - 'Int_t' 0 - ntracks U 'TObjArray' - 0 - tracks",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetObjectInfo",1283,G__AMSToFCluster_GetObjectInfo_3_0,67,-1,-1,0,2,1,1,0,
"i - 'Int_t' 0 - px i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Paint",508,G__AMSToFCluster_Paint_4_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Clear",487,G__AMSToFCluster_Clear_5_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("GetStatus",932,G__AMSToFCluster_GetStatus_6_0,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetPlane",784,G__AMSToFCluster_GetPlane_7_0,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetBar",565,G__AMSToFCluster_GetBar_8_0,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetEnergy",906,G__AMSToFCluster_GetEnergy_9_0,102,-1,G__defined_typename("Float_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetTime",687,G__AMSToFCluster_GetTime_0_1,102,-1,G__defined_typename("Float_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetNcells",897,G__AMSToFCluster_GetNcells_1_1,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetNparticles",1333,G__AMSToFCluster_GetNparticles_2_1,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetNTracks",982,G__AMSToFCluster_GetNTracks_3_1,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetTracks",904,G__AMSToFCluster_GetTracks_4_1,85,G__get_linked_tagnum(&G__AMSCintLN_TObjArray),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Is3D",307,G__AMSToFCluster_Is3D_5_1,98,-1,G__defined_typename("Bool_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SetEnergy",918,G__AMSToFCluster_SetEnergy_6_1,121,-1,-1,0,1,1,1,0,"f - 'Float_t' 0 - energy",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("SetTime",699,G__AMSToFCluster_SetTime_7_1,121,-1,-1,0,1,1,1,0,"f - 'Float_t' 0 - time",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileName",1145,G__AMSToFCluster_DeclFileName_8_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSToFCluster_DeclFileLine_9_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSToFCluster_ImplFileName_0_2,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSToFCluster_ImplFileLine_1_2,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSToFCluster_Class_Version_2_2,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSToFCluster_Class_3_2,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSToFCluster_Dictionary_4_2,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSToFCluster_IsA_5_2,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSToFCluster_ShowMembers_6_2,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSToFCluster_Streamer_7_2,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSToFCluster",1228,G__AMSToFCluster_AMSToFCluster_8_2,(int)('i'),G__get_linked_tagnum(&G__AMSCintLN_AMSToFCluster),-1,0,1,1,1,0,"u 'AMSToFCluster' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSToFCluster",1354,G__AMSToFCluster_wAAMSToFCluster_9_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSAntiCluster(void) {
   /* AMSAntiCluster */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSAntiCluster));
   G__memfunc_setup("AMSAntiCluster",1359,G__AMSAntiCluster_AMSAntiCluster_0_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSAntiCluster),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSAntiCluster",1359,G__AMSAntiCluster_AMSAntiCluster_1_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSAntiCluster),-1,0,7,1,1,0,
"i - 'Int_t' 0 - status i - 'Int_t' 0 - sector "
"f - 'Float_t' 0 - signal F - 'Float_t' 0 - coo "
"F - 'Float_t' 0 - ercoo i - 'Int_t' 0 - ntracks "
"U 'TObjArray' - 0 - tracks",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetObjectInfo",1283,G__AMSAntiCluster_GetObjectInfo_3_0,67,-1,-1,0,2,1,1,0,
"i - 'Int_t' 0 - px i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Paint",508,G__AMSAntiCluster_Paint_4_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Clear",487,G__AMSAntiCluster_Clear_5_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("GetStatus",932,G__AMSAntiCluster_GetStatus_6_0,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetSector",912,G__AMSAntiCluster_GetSector_7_0,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetSignal",894,G__AMSAntiCluster_GetSignal_8_0,102,-1,G__defined_typename("Float_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetTracks",904,G__AMSAntiCluster_GetTracks_9_0,85,G__get_linked_tagnum(&G__AMSCintLN_TObjArray),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Is3D",307,G__AMSAntiCluster_Is3D_0_1,98,-1,G__defined_typename("Bool_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DeclFileName",1145,G__AMSAntiCluster_DeclFileName_1_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSAntiCluster_DeclFileLine_2_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSAntiCluster_ImplFileName_3_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSAntiCluster_ImplFileLine_4_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSAntiCluster_Class_Version_5_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSAntiCluster_Class_6_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSAntiCluster_Dictionary_7_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSAntiCluster_IsA_8_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSAntiCluster_ShowMembers_9_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSAntiCluster_Streamer_0_2,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSAntiCluster",1359,G__AMSAntiCluster_AMSAntiCluster_1_2,(int)('i'),G__get_linked_tagnum(&G__AMSCintLN_AMSAntiCluster),-1,0,1,1,1,0,"u 'AMSAntiCluster' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSAntiCluster",1485,G__AMSAntiCluster_wAAMSAntiCluster_2_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSTrack(void) {
   /* AMSTrack */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSTrack));
   G__memfunc_setup("AMSTrack",726,G__AMSTrack_AMSTrack_0_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSTrack),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSTrack",726,G__AMSTrack_AMSTrack_1_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSTrack),-1,0,2,1,1,0,
"i - 'Int_t' 0 - status i - 'Int_t' 0 - pattern",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetObjectInfo",1283,G__AMSTrack_GetObjectInfo_3_0,67,-1,-1,0,2,1,1,0,
"i - 'Int_t' 0 - px i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SetHelix",806,G__AMSTrack_SetHelix_4_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Paint",508,G__AMSTrack_Paint_5_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DeclFileName",1145,G__AMSTrack_DeclFileName_6_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSTrack_DeclFileLine_7_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSTrack_ImplFileName_8_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSTrack_ImplFileLine_9_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSTrack_Class_Version_0_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSTrack_Class_1_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSTrack_Dictionary_2_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSTrack_IsA_3_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSTrack_ShowMembers_4_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSTrack_Streamer_5_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSTrack",726,G__AMSTrack_AMSTrack_6_1,(int)('i'),G__get_linked_tagnum(&G__AMSCintLN_AMSTrack),-1,0,1,1,1,0,"u 'AMSTrack' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSTrack",852,G__AMSTrack_wAAMSTrack_7_1,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSSiHit(void) {
   /* AMSSiHit */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSSiHit));
   G__memfunc_setup("AMSSiHit",706,G__AMSSiHit_AMSSiHit_0_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSSiHit),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSSiHit",706,G__AMSSiHit_AMSSiHit_1_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSSiHit),-1,0,9,1,1,0,
"i - 'Int_t' 0 - status i - 'Int_t' 0 - plane "
"i - 'Int_t' 0 - px i - 'Int_t' 0 - py "
"F - 'Float_t' 0 - hit F - 'Float_t' 0 - errhit "
"f - 'Float_t' 0 - ampl f - 'Float_t' 0 - asym "
"U 'TObjArray' - 0 - tracks",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetObjectInfo",1283,G__AMSSiHit_GetObjectInfo_3_0,67,-1,-1,0,2,1,1,0,
"i - 'Int_t' 0 - px i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Paint",508,G__AMSSiHit_Paint_4_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Clear",487,G__AMSSiHit_Clear_5_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("GetStatus",932,G__AMSSiHit_GetStatus_6_0,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetPlane",784,G__AMSSiHit_GetPlane_7_0,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetX",376,G__AMSSiHit_GetX_8_0,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetY",377,G__AMSSiHit_GetY_9_0,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetAmplitude",1221,G__AMSSiHit_GetAmplitude_0_1,102,-1,G__defined_typename("Float_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetAsymAmpl",1092,G__AMSSiHit_GetAsymAmpl_1_1,102,-1,G__defined_typename("Float_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetTracks",904,G__AMSSiHit_GetTracks_2_1,85,G__get_linked_tagnum(&G__AMSCintLN_TObjArray),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Is3D",307,G__AMSSiHit_Is3D_3_1,98,-1,G__defined_typename("Bool_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DeclFileName",1145,G__AMSSiHit_DeclFileName_4_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSSiHit_DeclFileLine_5_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSSiHit_ImplFileName_6_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSSiHit_ImplFileLine_7_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSSiHit_Class_Version_8_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSSiHit_Class_9_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSSiHit_Dictionary_0_2,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSSiHit_IsA_1_2,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSSiHit_ShowMembers_2_2,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSSiHit_Streamer_3_2,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSSiHit",706,G__AMSSiHit_AMSSiHit_4_2,(int)('i'),G__get_linked_tagnum(&G__AMSCintLN_AMSSiHit),-1,0,1,1,1,0,"u 'AMSSiHit' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSSiHit",832,G__AMSSiHit_wAAMSSiHit_5_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSCTCCluster(void) {
   /* AMSCTCCluster */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSCTCCluster));
   G__memfunc_setup("AMSCTCCluster",1181,G__AMSCTCCluster_AMSCTCCluster_0_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSCTCCluster),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSCTCCluster",1181,G__AMSCTCCluster_AMSCTCCluster_1_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSCTCCluster),-1,0,9,1,1,0,
"i - 'Int_t' 0 - status i - 'Int_t' 0 - plane "
"i - 'Int_t' 0 - rawsignal f - 'Float_t' 0 - signal "
"f - 'Float_t' 0 - errsignal F - 'Float_t' 0 - coo "
"F - 'Float_t' 0 - ercoo i - 'Int_t' 0 - ntracks "
"U 'TObjArray' - 0 - tracks",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetObjectInfo",1283,G__AMSCTCCluster_GetObjectInfo_3_0,67,-1,-1,0,2,1,1,0,
"i - 'Int_t' 0 - px i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Paint",508,G__AMSCTCCluster_Paint_4_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Clear",487,G__AMSCTCCluster_Clear_5_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("GetStatus",932,G__AMSCTCCluster_GetStatus_6_0,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetPlane",784,G__AMSCTCCluster_GetPlane_7_0,105,-1,G__defined_typename("Int_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetRawSignal",1192,G__AMSCTCCluster_GetRawSignal_8_0,102,-1,G__defined_typename("Float_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetSignal",894,G__AMSCTCCluster_GetSignal_9_0,102,-1,G__defined_typename("Float_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetErrSignal",1191,G__AMSCTCCluster_GetErrSignal_0_1,102,-1,G__defined_typename("Float_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetTracks",904,G__AMSCTCCluster_GetTracks_1_1,85,G__get_linked_tagnum(&G__AMSCintLN_TObjArray),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Is3D",307,G__AMSCTCCluster_Is3D_2_1,98,-1,G__defined_typename("Bool_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DeclFileName",1145,G__AMSCTCCluster_DeclFileName_3_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSCTCCluster_DeclFileLine_4_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSCTCCluster_ImplFileName_5_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSCTCCluster_ImplFileLine_6_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSCTCCluster_Class_Version_7_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSCTCCluster_Class_8_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSCTCCluster_Dictionary_9_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSCTCCluster_IsA_0_2,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSCTCCluster_ShowMembers_1_2,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSCTCCluster_Streamer_2_2,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSCTCCluster",1181,G__AMSCTCCluster_AMSCTCCluster_3_2,(int)('i'),G__get_linked_tagnum(&G__AMSCintLN_AMSCTCCluster),-1,0,1,1,1,0,"u 'AMSCTCCluster' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSCTCCluster",1307,G__AMSCTCCluster_wAAMSCTCCluster_4_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSParticle(void) {
   /* AMSParticle */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSParticle));
   G__memfunc_setup("AMSParticle",1045,G__AMSParticle_AMSParticle_0_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSParticle),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetObjectInfo",1283,G__AMSParticle_GetObjectInfo_2_0,67,-1,-1,0,2,1,1,0,
"i - 'Int_t' 0 - px i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("SetHelix",806,G__AMSParticle_SetHelix_3_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Paint",508,G__AMSParticle_Paint_4_0,121,-1,-1,0,1,1,1,0,"C - 'Option_t' 0 \"\" option",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("DeclFileName",1145,G__AMSParticle_DeclFileName_5_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSParticle_DeclFileLine_6_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSParticle_ImplFileName_7_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSParticle_ImplFileLine_8_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSParticle_Class_Version_9_0,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSParticle_Class_0_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSParticle_Dictionary_1_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSParticle_IsA_2_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSParticle_ShowMembers_3_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSParticle_Streamer_4_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSParticle",1045,G__AMSParticle_AMSParticle_5_1,(int)('i'),G__get_linked_tagnum(&G__AMSCintLN_AMSParticle),-1,0,1,1,1,0,"u 'AMSParticle' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSParticle",1171,G__AMSParticle_wAAMSParticle_6_1,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncDebugger(void) {
   /* Debugger */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSCintLN_Debugger));
   G__memfunc_setup("Debugger",805,G__Debugger_Debugger_0_0,105,G__get_linked_tagnum(&G__AMSCintLN_Debugger),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("On",189,G__Debugger_On_2_0,121,-1,-1,0,0,1,1,0,"","Turn on the debugger",(void*)NULL,0);
   G__memfunc_setup("Off",283,G__Debugger_Off_3_0,121,-1,-1,0,0,1,1,0,"","Turn off the debugger",(void*)NULL,0);
   G__memfunc_setup("Print",525,G__Debugger_Print_4_0,121,-1,-1,0,2,1,1,0,
"C - - 0 - fmt i - - 0 - -","Print a debug message",(void*)NULL,0);
   G__memfunc_setup("DeclFileName",1145,G__Debugger_DeclFileName_5_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__Debugger_DeclFileLine_6_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__Debugger_ImplFileName_7_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__Debugger_ImplFileLine_8_0,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__Debugger_Class_Version_9_0,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__Debugger_Class_0_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__Debugger_Dictionary_1_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__Debugger_IsA_2_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__Debugger_ShowMembers_3_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__Debugger_Streamer_4_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("Debugger",805,G__Debugger_Debugger_5_1,(int)('i'),G__get_linked_tagnum(&G__AMSCintLN_Debugger),-1,0,1,1,1,0,"u 'Debugger' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~Debugger",931,G__Debugger_wADebugger_6_1,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSCanvas(void) {
   /* AMSCanvas */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSCanvas));
   G__memfunc_setup("DrawEventStatus",1556,(G__InterfaceMethod)NULL,121,-1,-1,0,4,1,4,0,
"i - 'Int_t' 0 - event i - 'Int_t' 0 - px "
"i - 'Int_t' 0 - py U 'TObject' - 0 - selected",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AddParticleInfo",1481,(G__InterfaceMethod)NULL,121,-1,-1,0,0,1,2,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSCanvas",829,G__AMSCanvas_AMSCanvas_2_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSCanvas),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSCanvas",829,G__AMSCanvas_AMSCanvas_3_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSCanvas),-1,0,4,1,1,0,
"C - 'Text_t' 0 - name C - 'Text_t' 0 \"The AMS Display with Root\" title "
"i - 'Int_t' 0 700 ww i - 'Int_t' 0 550 wh",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Update",611,G__AMSCanvas_Update_5_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("EditorBar",892,G__AMSCanvas_EditorBar_6_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("HandleInput",1116,G__AMSCanvas_HandleInput_7_0,121,-1,-1,0,3,1,1,0,
"i - 'Int_t' 0 - event i - 'Int_t' 0 - px "
"i - 'Int_t' 0 - py",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("OpenFileCB",919,G__AMSCanvas_OpenFileCB_8_0,121,-1,-1,0,3,1,1,0,
"U '_WidgetRec' 'Widget' 0 - - Y - 'XtPointer' 0 - - "
"Y - 'XtPointer' 0 - -",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("SaveParticleCB",1352,G__AMSCanvas_SaveParticleCB_9_0,121,-1,-1,0,3,1,1,0,
"U '_WidgetRec' 'Widget' 0 - - Y - 'XtPointer' 0 - - "
"Y - 'XtPointer' 0 - -",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("PrintCB",658,G__AMSCanvas_PrintCB_0_1,121,-1,-1,0,3,1,1,0,
"U '_WidgetRec' 'Widget' 0 - - Y - 'XtPointer' 0 - - "
"Y - 'XtPointer' 0 - -",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileName",1145,G__AMSCanvas_DeclFileName_1_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSCanvas_DeclFileLine_2_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSCanvas_ImplFileName_3_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSCanvas_ImplFileLine_4_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSCanvas_Class_Version_5_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSCanvas_Class_6_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSCanvas_Dictionary_7_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSCanvas_IsA_8_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSCanvas_ShowMembers_9_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSCanvas_Streamer_0_2,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSCanvas",829,G__AMSCanvas_AMSCanvas_1_2,(int)('i'),G__get_linked_tagnum(&G__AMSCintLN_AMSCanvas),-1,0,1,1,1,0,"u 'AMSCanvas' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSCanvas",955,G__AMSCanvas_wAAMSCanvas_2_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}

static void G__setup_memfuncAMSGeometrySetter(void) {
   /* AMSGeometrySetter */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSGeometrySetter));
   G__memfunc_setup("AMSGeometrySetter",1700,G__AMSGeometrySetter_AMSGeometrySetter_0_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSGeometrySetter),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("AMSGeometrySetter",1700,G__AMSGeometrySetter_AMSGeometrySetter_1_0,105,G__get_linked_tagnum(&G__AMSCintLN_AMSGeometrySetter),-1,0,1,1,1,0,"U 'TGeometry' - 0 - geo",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("TurnOn",614,G__AMSGeometrySetter_TurnOn_3_0,121,-1,-1,0,1,1,1,0,"C - - 0 - name",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("TurnOnWithSons",1445,G__AMSGeometrySetter_TurnOnWithSons_4_0,121,-1,-1,0,1,1,1,0,"C - - 0 - name",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("TurnOff",708,G__AMSGeometrySetter_TurnOff_5_0,121,-1,-1,0,1,1,1,0,"C - - 0 - name",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("TurnOffWithSons",1539,G__AMSGeometrySetter_TurnOffWithSons_6_0,121,-1,-1,0,1,1,1,0,"C - - 0 - name",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("CheckVisibility",1542,G__AMSGeometrySetter_CheckVisibility_7_0,121,-1,-1,0,1,1,1,0,"C - - 0 - name",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("SetGeo",583,G__AMSGeometrySetter_SetGeo_8_0,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileName",1145,G__AMSGeometrySetter_DeclFileName_9_0,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileLine",1152,G__AMSGeometrySetter_DeclFileLine_0_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileName",1171,G__AMSGeometrySetter_ImplFileName_1_1,67,-1,-1,0,0,1,1,1,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ImplFileLine",1178,G__AMSGeometrySetter_ImplFileLine_2_1,105,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class_Version",1339,G__AMSGeometrySetter_Class_Version_3_1,115,-1,G__defined_typename("Version_t"),0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Class",502,G__AMSGeometrySetter_Class_4_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Dictionary",1046,G__AMSGeometrySetter_Dictionary_5_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("IsA",253,G__AMSGeometrySetter_IsA_6_1,85,G__get_linked_tagnum(&G__AMSCintLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__AMSGeometrySetter_ShowMembers_7_1,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__AMSGeometrySetter_Streamer_8_1,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   // automatic copy constructor
   G__memfunc_setup("AMSGeometrySetter",1700,G__AMSGeometrySetter_AMSGeometrySetter_9_1,(int)('i'),G__get_linked_tagnum(&G__AMSCintLN_AMSGeometrySetter),-1,0,1,1,1,0,"u 'AMSGeometrySetter' - 1 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~AMSGeometrySetter",1826,G__AMSGeometrySetter_wAAMSGeometrySetter_0_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}


/*********************************************************
* Member function information setup
*********************************************************/
extern "C" void G__cpp_setup_memfuncAMSCint() {
}

/*********************************************************
* Global variable information setup for each class
*********************************************************/
extern "C" void G__cpp_setup_globalAMSCint() {

   /* Setting up global variables */
   G__resetplocal();

   G__memvar_setup((void*)(&gAMSRoot),85,0,0,G__get_linked_tagnum(&G__AMSCintLN_AMSRoot),-1,-1,1,"gAMSRoot=",0,(char*)NULL);
   G__memvar_setup((void*)(&debugger),117,0,0,G__get_linked_tagnum(&G__AMSCintLN_Debugger),-1,-1,1,"debugger=",0,(char*)NULL);

   G__resetglobalenv();
}

/*********************************************************
* Global function information setup for each class
*********************************************************/
extern "C" void G__cpp_setup_funcAMSCint() {
   G__lastifuncposition();


   G__resetifuncposition();
}

/*********************************************************
* Class,struct,union,enum tag information setup
*********************************************************/
/* Setup class/struct taginfo */
G__linked_taginfo G__AMSCintLN_TClass = { "TClass" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_TList = { "TList" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_TObjArray = { "TObjArray" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_TObject = { "TObject" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_TString = { "TString" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_TNamed = { "TNamed" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_TVirtualPad = { "TVirtualPad" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_TGeometry = { "TGeometry" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_TAttLine = { "TAttLine" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_TAttFill = { "TAttFill" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_TTree = { "TTree" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_TH1F = { "TH1F" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_TAttText = { "TAttText" , 99 , -1 };
G__linked_taginfo G__AMSCintLN__x3d_data_ = { "_x3d_data_" , 115 , -1 };
G__linked_taginfo G__AMSCintLN__x3d_sizeof_ = { "_x3d_sizeof_" , 115 , -1 };
G__linked_taginfo G__AMSCintLN_TPolyLine3D = { "TPolyLine3D" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_TRotMatrix = { "TRotMatrix" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_THelix = { "THelix" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_TAttPad = { "TAttPad" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_TBox = { "TBox" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_TWbox = { "TWbox" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_TCanvas = { "TCanvas" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_TPad = { "TPad" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_TButton = { "TButton" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_TText = { "TText" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_TRadioButton = { "TRadioButton" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_TSwitch = { "TSwitch" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_AMSAxis = { "AMSAxis" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_TMarker3DBox = { "TMarker3DBox" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_AMSDisplay = { "AMSDisplay" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_AMS3DCluster = { "AMS3DCluster" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_AMSMaker = { "AMSMaker" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_AMSHistBrowser = { "AMSHistBrowser" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_AMSToFClusterReader = { "AMSToFClusterReader" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_AMSTrackReader = { "AMSTrackReader" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_AMSSiHitReader = { "AMSSiHitReader" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_AMSCTCClusterReader = { "AMSCTCClusterReader" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_AMSAntiClusterReader = { "AMSAntiClusterReader" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_AMSParticleReader = { "AMSParticleReader" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_AMSVirtualDisplay = { "AMSVirtualDisplay" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_AMSRoot = { "AMSRoot" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_AMSToFCluster = { "AMSToFCluster" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_AMSAntiCluster = { "AMSAntiCluster" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_AMSTrack = { "AMSTrack" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_AMSSiHit = { "AMSSiHit" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_AMSCTCCluster = { "AMSCTCCluster" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_AMSParticle = { "AMSParticle" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_EAMSView = { "EAMSView" , 101 , -1 };
G__linked_taginfo G__AMSCintLN_Debugger = { "Debugger" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_TMotifCanvas = { "TMotifCanvas" , 99 , -1 };
G__linked_taginfo G__AMSCintLN__WidgetRec = { "_WidgetRec" , 115 , -1 };
G__linked_taginfo G__AMSCintLN_MenuDesc_t = { "MenuDesc_t" , 115 , -1 };
G__linked_taginfo G__AMSCintLN_AMSCanvas = { "AMSCanvas" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_AMSGeoToggle = { "AMSGeoToggle" , 115 , -1 };
G__linked_taginfo G__AMSCintLN_AMSGeometrySetter = { "AMSGeometrySetter" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_TArc = { "TArc" , 99 , -1 };
G__linked_taginfo G__AMSCintLN_AMSColorManager = { "AMSColorManager" , 0 , -1 };
G__linked_taginfo G__AMSCintLN_MySelectionDialog = { "MySelectionDialog" , 0 , -1 };

extern "C" void G__cpp_setup_tagtableAMSCint() {

   /* Setting up class,struct,union tag entry */
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSCintLN_THelix),sizeof(THelix),-1,0,"A Helix drawn as a PolyLine3D",G__setup_memvarTHelix,G__setup_memfuncTHelix);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSCintLN_TRadioButton),sizeof(TRadioButton),-1,0,"Displayable toggle class",G__setup_memvarTRadioButton,G__setup_memfuncTRadioButton);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSCintLN_TSwitch),sizeof(TSwitch),-1,0,"Displayable Switch class",G__setup_memvarTSwitch,G__setup_memfuncTSwitch);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSAxis),sizeof(AMSAxis),-1,0,"AMS axis class",G__setup_memvarAMSAxis,G__setup_memfuncAMSAxis);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSDisplay),sizeof(AMSDisplay),-1,0,"Utility class to display AMSRoot outline, tracks,...",G__setup_memvarAMSDisplay,G__setup_memfuncAMSDisplay);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSCintLN_AMS3DCluster),sizeof(AMS3DCluster),-1,0,"Cluster class",G__setup_memvarAMS3DCluster,G__setup_memfuncAMS3DCluster);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSMaker),sizeof(AMSMaker),-1,3,"AMS virtual base class for Makers",G__setup_memvarAMSMaker,G__setup_memfuncAMSMaker);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSHistBrowser),sizeof(AMSHistBrowser),-1,0,"helper class to browse AMSRoot Makers histograms",G__setup_memvarAMSHistBrowser,G__setup_memfuncAMSHistBrowser);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSToFClusterReader),sizeof(AMSToFClusterReader),-1,0,"AMS Time of Flight Cluster Maker",G__setup_memvarAMSToFClusterReader,G__setup_memfuncAMSToFClusterReader);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSTrackReader),sizeof(AMSTrackReader),-1,0,"AMSRoot TrackReader",G__setup_memvarAMSTrackReader,G__setup_memfuncAMSTrackReader);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSSiHitReader),sizeof(AMSSiHitReader),-1,0,"AMS Time of Flight Cluster Maker",G__setup_memvarAMSSiHitReader,G__setup_memfuncAMSSiHitReader);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSCTCClusterReader),sizeof(AMSCTCClusterReader),-1,0,"AMS Time of Flight Cluster Maker",G__setup_memvarAMSCTCClusterReader,G__setup_memfuncAMSCTCClusterReader);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSAntiClusterReader),sizeof(AMSAntiClusterReader),-1,0,"AMS Time of Flight Cluster Maker",G__setup_memvarAMSAntiClusterReader,G__setup_memfuncAMSAntiClusterReader);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSParticleReader),sizeof(AMSParticleReader),-1,0,"AMSRoot Particle Reader",G__setup_memvarAMSParticleReader,G__setup_memfuncAMSParticleReader);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSVirtualDisplay),sizeof(AMSVirtualDisplay),-1,19,"Virtual base class for ATLFast event display",G__setup_memvarAMSVirtualDisplay,G__setup_memfuncAMSVirtualDisplay);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSRoot),sizeof(AMSRoot),-1,0,"AMSRoot control class",G__setup_memvarAMSRoot,G__setup_memfuncAMSRoot);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSToFCluster),sizeof(AMSToFCluster),-1,0,"AMS Time of Flight Cluster",G__setup_memvarAMSToFCluster,G__setup_memfuncAMSToFCluster);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSAntiCluster),sizeof(AMSAntiCluster),-1,0,"AMS Time of Flight Cluster",G__setup_memvarAMSAntiCluster,G__setup_memfuncAMSAntiCluster);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSTrack),sizeof(AMSTrack),-1,0,"AMSRoot track class",G__setup_memvarAMSTrack,G__setup_memfuncAMSTrack);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSSiHit),sizeof(AMSSiHit),-1,0,"AMS Time of Flight Cluster",G__setup_memvarAMSSiHit,G__setup_memfuncAMSSiHit);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSCTCCluster),sizeof(AMSCTCCluster),-1,0,"AMS Time of Flight Cluster",G__setup_memvarAMSCTCCluster,G__setup_memfuncAMSCTCCluster);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSParticle),sizeof(AMSParticle),-1,0,"AMSRoot track class",G__setup_memvarAMSParticle,G__setup_memfuncAMSParticle);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSCintLN_Debugger),sizeof(Debugger),-1,0,"Debug message printing class",G__setup_memvarDebugger,G__setup_memfuncDebugger);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSCanvas),sizeof(AMSCanvas),-1,0,"AMS canvas control class",G__setup_memvarAMSCanvas,G__setup_memfuncAMSCanvas);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSGeometrySetter),sizeof(AMSGeometrySetter),-1,0,"Utility class to display AMS geometry",G__setup_memvarAMSGeometrySetter,G__setup_memfuncAMSGeometrySetter);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSCintLN_AMSColorManager),0,-1,0,(char*)NULL,NULL,NULL);
   G__tagtable_setup(G__get_linked_tagnum(&G__AMSCintLN_MySelectionDialog),0,-1,0,(char*)NULL,NULL,NULL);
}
extern "C" void G__cpp_setupAMSCint() {
  G__check_setup_version(51111,"G__cpp_setupAMSCint()");
  G__set_cpp_environmentAMSCint();
  G__cpp_setup_tagtableAMSCint();

  G__cpp_setup_inheritanceAMSCint();

  G__cpp_setup_typetableAMSCint();

  G__cpp_setup_memvarAMSCint();

  G__cpp_setup_memfuncAMSCint();
  G__cpp_setup_globalAMSCint();
  G__cpp_setup_funcAMSCint();

   if(0==G__getsizep2memfunc()) G__get_sizep2memfuncAMSCint();
  return;
}
class G__cpp_setup_initAMSCint {
  public:
    G__cpp_setup_initAMSCint() { G__add_setup_func("AMSCint",&G__cpp_setupAMSCint); }
   ~G__cpp_setup_initAMSCint() { G__remove_setup_func("AMSCint"); }
};
G__cpp_setup_initAMSCint G__cpp_setup_initializerAMSCint;

//
// File generated by rootcint at Mon Jan 12 18:55:25 1998.
// Do NOT change. Changes will be lost next time file is generated
//

#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TError.h"

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, THelix *&obj)
{
   // Read a pointer to an object of class THelix.

   obj = (THelix *) buf.ReadObject(THelix::Class());
   return buf;
}

//______________________________________________________________________________
void THelix::Streamer(TBuffer &R__b)
{
   // Stream an object of class THelix.

   TPolyLine3D::Streamer(R__b);
}

//______________________________________________________________________________
void THelix::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class THelix.

   TClass *R__cl  = THelix::IsA();
   Int_t   R__ncp = strlen(R__parent);
   R__insp.Inspect(R__cl, R__parent, "fX0", &fX0);
   R__insp.Inspect(R__cl, R__parent, "fY0", &fY0);
   R__insp.Inspect(R__cl, R__parent, "fZ0", &fZ0);
   R__insp.Inspect(R__cl, R__parent, "fVt", &fVt);
   R__insp.Inspect(R__cl, R__parent, "fPhi0", &fPhi0);
   R__insp.Inspect(R__cl, R__parent, "fVz", &fVz);
   R__insp.Inspect(R__cl, R__parent, "fW", &fW);
   R__insp.Inspect(R__cl, R__parent, "fAxis[3]", fAxis);
   R__insp.Inspect(R__cl, R__parent, "*fRotMat", &fRotMat);
   R__insp.Inspect(R__cl, R__parent, "fRange[2]", fRange);
   TPolyLine3D::ShowMembers(R__insp, R__parent);
}

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
   R__insp.Inspect(R__cl, R__parent, "*fButton", &fButton);
   R__insp.Inspect(R__cl, R__parent, "*fText", &fText);
   TPad::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSAxis *&obj)
{
   // Read a pointer to an object of class AMSAxis.

   obj = (AMSAxis *) buf.ReadObject(AMSAxis::Class());
   return buf;
}

//______________________________________________________________________________
void AMSAxis::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSAxis.

   TPolyLine3D::Streamer(R__b);
}

//______________________________________________________________________________
void AMSAxis::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSAxis.

   TClass *R__cl  = AMSAxis::IsA();
   Int_t   R__ncp = strlen(R__parent);
   R__insp.Inspect(R__cl, R__parent, "*m_Title", &m_Title);
   TPolyLine3D::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSDisplay *&obj)
{
   // Read a pointer to an object of class AMSDisplay.

   obj = (AMSDisplay *) buf.ReadObject(AMSDisplay::Class());
   return buf;
}

//______________________________________________________________________________
void AMSDisplay::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSDisplay.

   AMSVirtualDisplay::Streamer(R__b);
}

//______________________________________________________________________________
void AMSDisplay::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSDisplay.

   TClass *R__cl  = AMSDisplay::IsA();
   Int_t   R__ncp = strlen(R__parent);
   R__insp.Inspect(R__cl, R__parent, "m_View", &m_View);
   R__insp.Inspect(R__cl, R__parent, "m_DrawParticles", &m_DrawParticles);
   R__insp.Inspect(R__cl, R__parent, "m_DrawGeometry", &m_DrawGeometry);
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
   R__insp.Inspect(R__cl, R__parent, "*m_EM1", &m_EM1);
   R__insp.Inspect(R__cl, R__parent, "*m_PH1", &m_PH1);
   R__insp.Inspect(R__cl, R__parent, "*m_EM2", &m_EM2);
   R__insp.Inspect(R__cl, R__parent, "*m_MU1", &m_MU1);
   R__insp.Inspect(R__cl, R__parent, "*m_MU2", &m_MU2);
   R__insp.Inspect(R__cl, R__parent, "*m_EMU", &m_EMU);
   R__insp.Inspect(R__cl, R__parent, "*m_JT1", &m_JT1);
   R__insp.Inspect(R__cl, R__parent, "*m_JT3", &m_JT3);
   R__insp.Inspect(R__cl, R__parent, "*m_JT4", &m_JT4);
   R__insp.Inspect(R__cl, R__parent, "*m_ALL", &m_ALL);
   R__insp.Inspect(R__cl, R__parent, "*m_ToBeDrawn", &m_ToBeDrawn);
   AMSVirtualDisplay::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMS3DCluster *&obj)
{
   // Read a pointer to an object of class AMS3DCluster.

   obj = (AMS3DCluster *) buf.ReadObject(AMS3DCluster::Class());
   return buf;
}

//______________________________________________________________________________
void AMS3DCluster::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMS3DCluster.

   TMarker3DBox::Streamer(R__b);
}

//______________________________________________________________________________
void AMS3DCluster::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMS3DCluster.

   TClass *R__cl  = AMS3DCluster::IsA();
   Int_t   R__ncp = strlen(R__parent);
   R__insp.Inspect(R__cl, R__parent, "fX", &fX);
   R__insp.Inspect(R__cl, R__parent, "fY", &fY);
   R__insp.Inspect(R__cl, R__parent, "fZ", &fZ);
   R__insp.Inspect(R__cl, R__parent, "fRange", &fRange);
   TMarker3DBox::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSMaker *&obj)
{
   // Read a pointer to an object of class AMSMaker.

   obj = (AMSMaker *) buf.ReadObject(AMSMaker::Class());
   return buf;
}

//______________________________________________________________________________
void AMSMaker::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSMaker.

   TClass *R__cl  = AMSMaker::IsA();
   Int_t   R__ncp = strlen(R__parent);
   R__insp.Inspect(R__cl, R__parent, "m_Save", &m_Save);
   R__insp.Inspect(R__cl, R__parent, "*m_Fruits", &m_Fruits);
   m_BranchName.ShowMembers(R__insp, strcat(R__parent,"m_BranchName.")); R__parent[R__ncp] = 0;
   R__insp.Inspect(R__cl, R__parent, "*m_Histograms", &m_Histograms);
   R__insp.Inspect(R__cl, R__parent, "DrawFruits", &DrawFruits);
   TNamed::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSHistBrowser *&obj)
{
   // Read a pointer to an object of class AMSHistBrowser.

   obj = (AMSHistBrowser *) buf.ReadObject(AMSHistBrowser::Class());
   return buf;
}

//______________________________________________________________________________
void AMSHistBrowser::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSHistBrowser.

   TNamed::Streamer(R__b);
}

//______________________________________________________________________________
void AMSHistBrowser::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSHistBrowser.

   TClass *R__cl  = AMSHistBrowser::IsA();
   Int_t   R__ncp = strlen(R__parent);
   TNamed::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSToFClusterReader *&obj)
{
   // Read a pointer to an object of class AMSToFClusterReader.

   obj = (AMSToFClusterReader *) buf.ReadObject(AMSToFClusterReader::Class());
   return buf;
}

//______________________________________________________________________________
void AMSToFClusterReader::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSToFClusterReader.

   AMSMaker::Streamer(R__b);
}

//______________________________________________________________________________
void AMSToFClusterReader::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSToFClusterReader.

   TClass *R__cl  = AMSToFClusterReader::IsA();
   Int_t   R__ncp = strlen(R__parent);
   R__insp.Inspect(R__cl, R__parent, "m_Nclusters", &m_Nclusters);
   AMSMaker::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSTrackReader *&obj)
{
   // Read a pointer to an object of class AMSTrackReader.

   obj = (AMSTrackReader *) buf.ReadObject(AMSTrackReader::Class());
   return buf;
}

//______________________________________________________________________________
void AMSTrackReader::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSTrackReader.

   AMSMaker::Streamer(R__b);
}

//______________________________________________________________________________
void AMSTrackReader::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSTrackReader.

   TClass *R__cl  = AMSTrackReader::IsA();
   Int_t   R__ncp = strlen(R__parent);
   R__insp.Inspect(R__cl, R__parent, "m_NTracks", &m_NTracks);
   R__insp.Inspect(R__cl, R__parent, "*m_Mult", &m_Mult);
   AMSMaker::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSSiHitReader *&obj)
{
   // Read a pointer to an object of class AMSSiHitReader.

   obj = (AMSSiHitReader *) buf.ReadObject(AMSSiHitReader::Class());
   return buf;
}

//______________________________________________________________________________
void AMSSiHitReader::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSSiHitReader.

   AMSMaker::Streamer(R__b);
}

//______________________________________________________________________________
void AMSSiHitReader::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSSiHitReader.

   TClass *R__cl  = AMSSiHitReader::IsA();
   Int_t   R__ncp = strlen(R__parent);
   R__insp.Inspect(R__cl, R__parent, "m_Nclusters", &m_Nclusters);
   R__insp.Inspect(R__cl, R__parent, "DrawUsedHitsOnly", &DrawUsedHitsOnly);
   AMSMaker::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSCTCClusterReader *&obj)
{
   // Read a pointer to an object of class AMSCTCClusterReader.

   obj = (AMSCTCClusterReader *) buf.ReadObject(AMSCTCClusterReader::Class());
   return buf;
}

//______________________________________________________________________________
void AMSCTCClusterReader::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSCTCClusterReader.

   AMSMaker::Streamer(R__b);
}

//______________________________________________________________________________
void AMSCTCClusterReader::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSCTCClusterReader.

   TClass *R__cl  = AMSCTCClusterReader::IsA();
   Int_t   R__ncp = strlen(R__parent);
   R__insp.Inspect(R__cl, R__parent, "m_Nclusters", &m_Nclusters);
   AMSMaker::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSAntiClusterReader *&obj)
{
   // Read a pointer to an object of class AMSAntiClusterReader.

   obj = (AMSAntiClusterReader *) buf.ReadObject(AMSAntiClusterReader::Class());
   return buf;
}

//______________________________________________________________________________
void AMSAntiClusterReader::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSAntiClusterReader.

   AMSMaker::Streamer(R__b);
}

//______________________________________________________________________________
void AMSAntiClusterReader::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSAntiClusterReader.

   TClass *R__cl  = AMSAntiClusterReader::IsA();
   Int_t   R__ncp = strlen(R__parent);
   R__insp.Inspect(R__cl, R__parent, "m_Nclusters", &m_Nclusters);
   AMSMaker::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSParticleReader *&obj)
{
   // Read a pointer to an object of class AMSParticleReader.

   obj = (AMSParticleReader *) buf.ReadObject(AMSParticleReader::Class());
   return buf;
}

//______________________________________________________________________________
void AMSParticleReader::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSParticleReader.

   AMSMaker::Streamer(R__b);
}

//______________________________________________________________________________
void AMSParticleReader::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSParticleReader.

   TClass *R__cl  = AMSParticleReader::IsA();
   Int_t   R__ncp = strlen(R__parent);
   R__insp.Inspect(R__cl, R__parent, "m_NParticles", &m_NParticles);
   R__insp.Inspect(R__cl, R__parent, "*m_Mult", &m_Mult);
   AMSMaker::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSVirtualDisplay *&obj)
{
   // Read a pointer to an object of class AMSVirtualDisplay.

   obj = (AMSVirtualDisplay *) buf.ReadObject(AMSVirtualDisplay::Class());
   return buf;
}

//______________________________________________________________________________
void AMSVirtualDisplay::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSVirtualDisplay.

   TObject::Streamer(R__b);
}

//______________________________________________________________________________
void AMSVirtualDisplay::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSVirtualDisplay.

   TClass *R__cl  = AMSVirtualDisplay::IsA();
   Int_t   R__ncp = strlen(R__parent);
   TObject::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSRoot *&obj)
{
   // Read a pointer to an object of class AMSRoot.

   obj = (AMSRoot *) buf.ReadObject(AMSRoot::Class());
   return buf;
}

//______________________________________________________________________________
void AMSRoot::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSRoot.

   TClass *R__cl  = AMSRoot::IsA();
   Int_t   R__ncp = strlen(R__parent);
   R__insp.Inspect(R__cl, R__parent, "m_Version", &m_Version);
   R__insp.Inspect(R__cl, R__parent, "m_VersionDate", &m_VersionDate);
   R__insp.Inspect(R__cl, R__parent, "m_RunNum", &m_RunNum);
   R__insp.Inspect(R__cl, R__parent, "m_EventNum", &m_EventNum);
   R__insp.Inspect(R__cl, R__parent, "m_Mode", &m_Mode);
   R__insp.Inspect(R__cl, R__parent, "m_Event", &m_Event);
   R__insp.Inspect(R__cl, R__parent, "*m_Tree", &m_Tree);
   R__insp.Inspect(R__cl, R__parent, "*m_Input", &m_Input);
   R__insp.Inspect(R__cl, R__parent, "*m_Makers", &m_Makers);
   R__insp.Inspect(R__cl, R__parent, "*m_ToFClusterMaker", &m_ToFClusterMaker);
   R__insp.Inspect(R__cl, R__parent, "*m_TrackMaker", &m_TrackMaker);
   R__insp.Inspect(R__cl, R__parent, "*m_SiHitMaker", &m_SiHitMaker);
   R__insp.Inspect(R__cl, R__parent, "*m_CTCClusterMaker", &m_CTCClusterMaker);
   R__insp.Inspect(R__cl, R__parent, "*m_AntiClusterMaker", &m_AntiClusterMaker);
   R__insp.Inspect(R__cl, R__parent, "*m_ParticleMaker", &m_ParticleMaker);
   m_HistBrowser.ShowMembers(R__insp, strcat(R__parent,"m_HistBrowser.")); R__parent[R__ncp] = 0;
   R__insp.Inspect(R__cl, R__parent, "*m_Display", &m_Display);
   TNamed::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSToFCluster *&obj)
{
   // Read a pointer to an object of class AMSToFCluster.

   obj = (AMSToFCluster *) buf.ReadObject(AMSToFCluster::Class());
   return buf;
}

//______________________________________________________________________________
void AMSToFCluster::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSToFCluster.

   AMS3DCluster::Streamer(R__b);
}

//______________________________________________________________________________
void AMSToFCluster::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSToFCluster.

   TClass *R__cl  = AMSToFCluster::IsA();
   Int_t   R__ncp = strlen(R__parent);
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
   AMS3DCluster::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSAntiCluster *&obj)
{
   // Read a pointer to an object of class AMSAntiCluster.

   obj = (AMSAntiCluster *) buf.ReadObject(AMSAntiCluster::Class());
   return buf;
}

//______________________________________________________________________________
void AMSAntiCluster::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSAntiCluster.

   AMS3DCluster::Streamer(R__b);
}

//______________________________________________________________________________
void AMSAntiCluster::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSAntiCluster.

   TClass *R__cl  = AMSAntiCluster::IsA();
   Int_t   R__ncp = strlen(R__parent);
   R__insp.Inspect(R__cl, R__parent, "m_Status", &m_Status);
   R__insp.Inspect(R__cl, R__parent, "m_Sector", &m_Sector);
   R__insp.Inspect(R__cl, R__parent, "m_Signal", &m_Signal);
   R__insp.Inspect(R__cl, R__parent, "m_NTracks", &m_NTracks);
   R__insp.Inspect(R__cl, R__parent, "*m_Tracks", &m_Tracks);
   AMS3DCluster::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSTrack *&obj)
{
   // Read a pointer to an object of class AMSTrack.

   obj = (AMSTrack *) buf.ReadObject(AMSTrack::Class());
   return buf;
}

//______________________________________________________________________________
void AMSTrack::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSTrack.

   THelix::Streamer(R__b);
}

//______________________________________________________________________________
void AMSTrack::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSTrack.

   TClass *R__cl  = AMSTrack::IsA();
   Int_t   R__ncp = strlen(R__parent);
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
TBuffer &operator>>(TBuffer &buf, AMSSiHit *&obj)
{
   // Read a pointer to an object of class AMSSiHit.

   obj = (AMSSiHit *) buf.ReadObject(AMSSiHit::Class());
   return buf;
}

//______________________________________________________________________________
void AMSSiHit::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSSiHit.

   AMS3DCluster::Streamer(R__b);
}

//______________________________________________________________________________
void AMSSiHit::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSSiHit.

   TClass *R__cl  = AMSSiHit::IsA();
   Int_t   R__ncp = strlen(R__parent);
   R__insp.Inspect(R__cl, R__parent, "m_Status", &m_Status);
   R__insp.Inspect(R__cl, R__parent, "m_Plane", &m_Plane);
   R__insp.Inspect(R__cl, R__parent, "m_X", &m_X);
   R__insp.Inspect(R__cl, R__parent, "m_Y", &m_Y);
   R__insp.Inspect(R__cl, R__parent, "m_ErrPosition[3]", m_ErrPosition);
   R__insp.Inspect(R__cl, R__parent, "m_Amplitude", &m_Amplitude);
   R__insp.Inspect(R__cl, R__parent, "m_AsymAmpl", &m_AsymAmpl);
   R__insp.Inspect(R__cl, R__parent, "*m_Tracks", &m_Tracks);
   AMS3DCluster::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSCTCCluster *&obj)
{
   // Read a pointer to an object of class AMSCTCCluster.

   obj = (AMSCTCCluster *) buf.ReadObject(AMSCTCCluster::Class());
   return buf;
}

//______________________________________________________________________________
void AMSCTCCluster::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSCTCCluster.

   AMS3DCluster::Streamer(R__b);
}

//______________________________________________________________________________
void AMSCTCCluster::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSCTCCluster.

   TClass *R__cl  = AMSCTCCluster::IsA();
   Int_t   R__ncp = strlen(R__parent);
   R__insp.Inspect(R__cl, R__parent, "m_Status", &m_Status);
   R__insp.Inspect(R__cl, R__parent, "m_Plane", &m_Plane);
   R__insp.Inspect(R__cl, R__parent, "m_RawSignal", &m_RawSignal);
   R__insp.Inspect(R__cl, R__parent, "m_Signal", &m_Signal);
   R__insp.Inspect(R__cl, R__parent, "m_ErrSignal", &m_ErrSignal);
   R__insp.Inspect(R__cl, R__parent, "m_NTracks", &m_NTracks);
   R__insp.Inspect(R__cl, R__parent, "*m_Tracks", &m_Tracks);
   AMS3DCluster::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSParticle *&obj)
{
   // Read a pointer to an object of class AMSParticle.

   obj = (AMSParticle *) buf.ReadObject(AMSParticle::Class());
   return buf;
}

//______________________________________________________________________________
void AMSParticle::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSParticle.

   THelix::Streamer(R__b);
}

//______________________________________________________________________________
void AMSParticle::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSParticle.

   TClass *R__cl  = AMSParticle::IsA();
   Int_t   R__ncp = strlen(R__parent);
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
   THelix::ShowMembers(R__insp, R__parent);
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

   TObject::Streamer(R__b);
}

//______________________________________________________________________________
void Debugger::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class Debugger.

   TClass *R__cl  = Debugger::IsA();
   Int_t   R__ncp = strlen(R__parent);
   R__insp.Inspect(R__cl, R__parent, "isOn", &isOn);
   TObject::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSCanvas *&obj)
{
   // Read a pointer to an object of class AMSCanvas.

   obj = (AMSCanvas *) buf.ReadObject(AMSCanvas::Class());
   return buf;
}

//______________________________________________________________________________
void AMSCanvas::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSCanvas.

   TCanvas::Streamer(R__b);
}

//______________________________________________________________________________
void AMSCanvas::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSCanvas.

   TClass *R__cl  = AMSCanvas::IsA();
   Int_t   R__ncp = strlen(R__parent);
   TCanvas::ShowMembers(R__insp, R__parent);
}

//______________________________________________________________________________
TBuffer &operator>>(TBuffer &buf, AMSGeometrySetter *&obj)
{
   // Read a pointer to an object of class AMSGeometrySetter.

   ::Error("AMSGeometrySetter::operator>>", "objects not inheriting from TObject need a specialized operator>> function");
   return buf;
}

//______________________________________________________________________________
void AMSGeometrySetter::Streamer(TBuffer &R__b)
{
   // Stream an object of class AMSGeometrySetter.

   ::Error("AMSGeometrySetter::Streamer", "version id <=0 in ClassDef, dummy Streamer() called");
}

//______________________________________________________________________________
void AMSGeometrySetter::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
   // Inspect the data members of an object of class AMSGeometrySetter.

   TClass *R__cl  = AMSGeometrySetter::IsA();
   Int_t   R__ncp = strlen(R__parent);
   R__insp.Inspect(R__cl, R__parent, "*m_Geometry", &m_Geometry);
}

