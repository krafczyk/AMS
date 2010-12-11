#include "root_setup.h"
#include "root.h"
AMSSetupR* AMSSetupR::_Head=0;
void AMSSetupR::CreateBranch(TTree *tree, int branchSplit){
  if(tree){
    _Head=this;
    tree->Branch("run.","AMSSetupR",&_Head,12800000,branchSplit);
    TBranch * branch=tree->GetBranch("run.");
    branch->SetCompressionLevel(6);
    cout <<" SetupR CompressionLevel "<<branch->GetCompressionLevel()<<" "<<branch->GetSplitLevel()<<endl;
  }
}

void AMSSetupR::UpdateHeader(AMSEventR *head){
if(!head)return;
if(!fHeader.Run){
cerr<<"AMSSetupR::UpdateHeader-W-RunNotSet.SettingTo "<<head->Run()<<endl;
fHeader.Run=head->Run();
//add tsql attempt
}
if(fHeader.LEvent<head->Event()){
 fHeader.LEvent=head->Event();
 fHeader.LEventTime=head->UTime();
 //fBValues.insert(make_pair(head->UTime(),BValues()));
}
if(fHeader.FEvent==0){
 fHeader.FEvent=head->Event();
 fHeader.FEventTime=head->UTime();
}

}
bool AMSSetupR::UpdateVersion(uinteger run,uinteger os,uinteger buildno,uinteger buildtime){
fHeader.Run=run;
fHeader.OS=os;
fHeader.BuildNo=buildno;
fHeader.BuildTime=buildtime;
if(run){
//add tsql attempt
}
return true;
}
void AMSSetupR::Reset(){
fGPSTime.clear();
fTTCS.clear();
fBValues.clear();
fISSData.clear();
fScalers.clear();
fLVL1Setup.clear();
Header a;
fHeader=a;
}

AMSSetupR::AMSSetupR(){
Reset();
}
  ClassImp(AMSSetupR::Header)
  ClassImp(AMSSetupR)
  ClassImp(AMSSetupR::BValues)

 void AMSSetupR::Init(TTree *tree){
  //   Set branch addresses
  if (tree == 0) return;
  AMSSetupR *dummy=this;
  tree->SetBranchAddress("run.",&dummy);
   
}
