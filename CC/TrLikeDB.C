#include "TrLikeDB.h"
#include "TrGainDB.h"


ClassImp(TrLikeDB);


TrLikeDB* TrLikeDB::fHead = 0;


double TrLikeDB::default_logprob = -8.;
int    TrLikeDB::fZmax = 0;


TrLikeDB* TrLikeDB::GetHead() {
  if (fHead==0) fHead = new TrLikeDB(); 
  return fHead;
}


void TrLikeDB::Clean() {
  fTrPdfMap.clear();
}


void TrLikeDB::Init() {
  if (LoadPdfVer0()) printf("TrLikeDB::Init()-V TrLikeDBPdfVer0 pdfs loaded, %d elements red.\n",(int)fTrPdfMap.size());
  else               printf("TrLikeDB::Init()-V TrLikeDBPdfVer0 NOT loaded.\n");
}


bool TrLikeDB::LoadPdfVer0(char* dirname) { 
  TFile* file = TFile::Open(Form("%s/v5.00/TrLikeDBver0/PDFDBv2.root",dirname),"read");
  if (!file) return false;
  int index = 0;
  int irig = 1;
  TH1* pdf = 0;
  fZmax = 28;
  for (int Z=1; Z<=fZmax; Z++) {
    for (int icat=0; icat<2; icat++) { 
      // 2D Xi_{X} vs Xi_{Y}
      index = CreateIndex(0,Z,icat,0,irig); 
      pdf = (TH1*) file->Get(Form("hLogXi2D_Z%d_Rig%d_Cat%d",Z,irig,icat+1));
      if (!pdf) {
        printf("TrLikeDB::LoadPdfVer0-E error in loading PDFs (hLogXi2D_Z%d_Rig%d_Cat%d). Tracker integer charge could have some problem!\n",Z,irig,icat+1);
        file->Close();
        break;
      } 
      pdf->SetDirectory(0);
      AddPdf(pdf,index);
      for (int iside=0; iside<2; iside++) { 
        // 1D Xi
        index = CreateIndex(1,Z,icat,iside,irig);   
        pdf = (TH1*) file->Get(Form("hLogXi1D_Z%d_Rig%d_Cat%d_Side%d",Z,irig,icat+1,iside));
        if (!pdf) {
          printf("TrLikeDB::LoadPdfVer0-E error in loading PDFs (hLogXi1D_Z%d_Rig%d_Cat%d_Side%d). Tracker integer charge could have some problem!\n",Z,irig,icat+1,iside);
          file->Close();
          break;
        }
        pdf->SetDirectory(0);
        AddPdf(pdf,index);
        // 1D Eta
        index = CreateIndex(2,Z,icat,iside,irig);
        pdf = (TH1*) file->Get(Form("hLogEtaS1D_Z%d_Rig%d_Cat%d_Side%d",Z,irig,icat+1,iside));
        if (!pdf) {
          printf("TrLikeDB::LoadPdfVer0-E error in loading PDFs (hLogEtaS1D_Z%d_Rig%d_Cat%d_Side%d). Tracker integer charge could have some problem!\n",Z,irig,icat+1,irig);
          file->Close();
          break;
        }
        pdf->SetDirectory(0);
        AddPdf(pdf,index);
      }   
    }
  }
  // create missing pdf here if needed ... not for now 
  file->Close();
  return true;
}


int TrLikeDB::CreateIndex(int type, int Z, int icat, int iside, int irig) {
  if ( (icat!=0)&&(icat!=1) ) return -1;
  if (Z>=fZmax) Z = fZmax; // use Z=fZmax for very high charges    
  return type + Z*10 + icat*10000 + iside*100000 + irig*1000000;
}


void TrLikeDB::AddPdf(TH1* pdf, int index) {
  if (index<0) return; // invalid index
  pair<map<int,TH1*>::iterator,bool> ret;
  ret = fTrPdfMap.insert(pair<int,TH1*>(index,pdf));
  if (ret.second==false) fTrPdfMap.find(index)->second = pdf; 
}


TH1* TrLikeDB::GetPdf(int index) {
  map<int,TH1*>::iterator it = fTrPdfMap.find(index);
  return (it!=fTrPdfMap.end()) ? it->second : 0;
}



double TrLikeDB::Interpolate(TH1* pdf, double x) {
  if (!pdf){
    static int bad=0;
    if(!bad++)cerr<<"TrLikeDB::Interpolate-S-NoPDF "<<endl;
    return fabs(default_logprob);
  }
  if ( (x<pdf->GetXaxis()->GetXmin())||(x>pdf->GetXaxis()->GetXmax()) ) 
    return fabs(default_logprob);
  return pdf->Interpolate(x);
}


double TrLikeDB::Interpolate(TH1* pdf, double x, double y) {
  if (!pdf) return fabs(default_logprob);
  if ( (x<pdf->GetXaxis()->GetXmin())||(x>pdf->GetXaxis()->GetXmax())||
       (y<pdf->GetYaxis()->GetXmin())||(y>pdf->GetYaxis()->GetXmax()) )  
    return fabs(default_logprob);
  return pdf->Interpolate(x,y);
}


void TrLikeDB::Print() {
  printf("TrLikeDB::Print()-V: \n");
  map<int,TH1*>::iterator it;
  int i = 0;
  for (it=fTrPdfMap.begin(); it!=fTrPdfMap.end(); it++) {
    printf("%4d %6d %40s\n",i,it->first,it->second->GetName());
    i++;
  }
}


