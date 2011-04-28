#include "TrPdfDB.h"


TrPdfDB* TrPdfDB::fHead=0;


TrPdfDB* TrPdfDB::GetHead(){
  if (fHead==0) {
    printf("TrPdfDB::GetHead()-V istanciating TrPdfDB singleton.\n");
    fHead = new TrPdfDB(); 
    // some default load
  } 
  return fHead;
}


void TrPdfDB::Add(TrPdf* pdf, int index) {
  pair<map<int,TrPdf*>::iterator,bool> ret;
  ret = fTrPdfMap.insert(pair<int,TrPdf*>(index,pdf));
  if (ret.second==false) fTrPdfMap.find(index)->second = pdf; 
}


TrPdf* TrPdfDB::Get(int index) {
  map<int,TrPdf*>::iterator it = fTrPdfMap.find(index);
  return (it!=fTrPdfMap.end()) ? it->second : 0;
}


void TrPdfDB::Info() {
  printf("TrPdfDB::Info() List of Pdf: ");
  if (GetEntries()==0) { 
    printf("Empty!\n"); 
    return; 
  }
  printf("\n");
  for (int ii=0; ii<GetEntries(); ii++) {
    TrPdf* pdf   = GetEntryPdf(ii);
    int    index = GetEntryIndex(ii); 
    printf("%3d of %3d   Index = %010d   Name = %10s   nPoints = %4d   xFirst = %7.3f   xLast = %7.3f\n",
           ii+1,GetEntries(),index,pdf->GetName(),pdf->GetN(),pdf->GetX(0),pdf->GetX(pdf->GetN()-1));
  }
}

 
TrPdf* TrPdfDB::GetEntryPdf(int ii){
  int count=0;
  for (map<int,TrPdf*>::iterator pp = fTrPdfMap.begin(); pp != fTrPdfMap.end(); pp++) {
    if (count==ii) return pp->second;
    count++;
  }
  return 0;
}


int TrPdfDB::GetEntryIndex(int ii){
  int count=0;
  for (map<int,TrPdf*>::iterator pp = fTrPdfMap.begin(); pp != fTrPdfMap.end(); pp++) {
    if (count==ii) return pp->first;
    count++;
  }
  return -1;
}


void TrPdfDB::Clear() {
  //! Complete deletion (who is the owner of TrPdf?)
  fTrPdfMap.clear();
}


void TrPdfDB::LoadPierrePdf(TFile* file) {
  TH2D* histo;
  // n-side
  histo = (TH2D*) file->FindObjectAny("AmpVsChargeSelected_all_N");
  for (int i=0; i<5; i++) {
    TH1D* th1d = GetSliceY(histo,i+1);
    if (th1d==0) continue;
    th1d->Rebin(50);
    int Z = i+2;
    TrPdf* pdf = new TrPdf(Form("Z%d_N",Z),th1d,true,true);
    delete th1d;
    Add(pdf,Z,0,0);
  }
  // p-side
  histo = (TH2D*) file->FindObjectAny("AmpVsChargeSelected_all_P");
  for (int i=0; i<5; i++) {
    TH1D* th1d = GetSliceY(histo,i+1);
    if (th1d==0) continue;
    th1d->Rebin(50);
    int Z = i+2;
    TrPdf* pdf = new TrPdf(Form("Z%d_P",Z),th1d,true,true);
    delete th1d;
    Add(pdf,Z,1,0);
  }
  // p/n combination
  histo = (TH2D*) file->FindObjectAny("AmpVsChargeSelected_PN_all");
  for (int i=0; i<5; i++) {
    TH1D* th1d = GetSliceY(histo,i+1);
    if (th1d==0) continue;
    th1d->Rebin(50);
    int Z = i+2;
    TrPdf* pdf = new TrPdf(Form("Z%d_PN",Z),th1d,true,true);
    delete th1d;
    Add(pdf,Z,2,0);
  }
  // info
  Info();
}


TH1D* TrPdfDB::GetSliceY(TH2D* th2, int ixbin) {
  if (th2==0) {
    printf("TrPdfDB::GetSliceY()-W no histogram to be sliced, return 0.\n");
    return 0;
  }
  // binning
  int   nxbins = th2->GetXaxis()->GetNbins();
  float   xmin = th2->GetXaxis()->GetXmin();
  float   xmax = th2->GetXaxis()->GetXmax();
  int   nybins = th2->GetYaxis()->GetNbins();
  float   ymin = th2->GetYaxis()->GetXmin();
  float   ymax = th2->GetYaxis()->GetXmax();
  // init
  char title[100];
  sprintf(title,"%s_slice%06d",th2->GetName(),ixbin);
  TH1D* slice = new TH1D(title,title,nybins,ymin,ymax);
  // loop
  for (int iy=0; iy<nybins; iy++) {
    slice->SetBinContent(iy+1,th2->GetBinContent(ixbin,iy+1));
  }
  return slice;
}

