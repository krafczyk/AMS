#include "TrPdfDB.h"


ClassImp(TrPdfDB);


TrPdfDB* TrPdfDB::fHead = 0;


TrPdfDB* TrPdfDB::GetHead() {
  if (fHead==0) {
    printf("TrPdfDB::GetHead()-V istanciating TrPdfDB singleton.\n");
    fHead = new TrPdfDB(); 
  } 
  return fHead;
}


void TrPdfDB::AddPdf(TrPdf* pdf, int index) {
  pair<map<int,TrPdf*>::iterator,bool> ret;
  ret = fTrPdfMap.insert(pair<int,TrPdf*>(index,pdf));
  if (ret.second==false) fTrPdfMap.find(index)->second = pdf; 
}


TrPdf* TrPdfDB::GetPdf(int index) {
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
    printf("%3d of %3d   Index = %010d   Name = %20s   nPoints = %4d   xFirst = %7.3f   xLast = %7.3f\n",
           ii+1,GetEntries(),index,pdf->GetName().Data(),pdf->GetN(),pdf->GetX(0),pdf->GetX(pdf->GetN()-1));
  }
}


void TrPdfDB::Load(TFile* file) {
  TObject* obj = (TObject*) file->Get("TrPdfDB");
  if (obj==0) {
    printf("TrPdfDB::Load-W no TrPdfDB in file.\n");
    return;
  }
  if (fHead!=0) delete fHead;
  fHead = (TrPdfDB*) obj;
}


void TrPdfDB::Load(char* filename){
  TFile* file = TFile::Open(filename,"read");
  if (file==0) {
    printf("TrPdfDB::Load-W file %s empty or unreachable.\n",filename);
    return;
  }
  Load(file);
  file->Close();
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


void TrPdfDB::Clear(Option_t* option) {
  TObject::Clear(option);
  //! Complete deletion (who is the owner of TrPdf?)
  fTrPdfMap.clear();
}


void TrPdfDB::LoadDefaults() {
static int ld=0;
#ifdef __ROOTSHAREDLIBRARY__
#pragma omp threadprivate (ld)
#endif
if(ld==0){
  printf("TrPdfDB::LoadDefaults-V Loading default PDFs.\n");
  LoadPierrePdf01(Form("%s/v5.00/tkpdf001mc.root",getenv("AMSDataDir")));
  LoadPierrePdf02();
  // Info();
  ld=1; 
}
}


void TrPdfDB::LoadPierrePdf01(char* filename) {
  TFile* file = TFile::Open(filename,"read");
  if (file==0) printf("TrPdfDB::LoadPierrePdf01-W cannot open file %s. Loading only hard coded pdfs.\n",filename);
  LoadPierrePdf01(file);
  if (file!=0) file->Close();
}


void TrPdfDB::LoadPierrePdf01(TFile* file) {
  if (file!=0) { 
    // Create PDFs for single planes [sqrt(ADC) units]
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
      AddPdf(pdf,Z,0,kPdf01_SingleLayer);
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
      AddPdf(pdf,Z,1,kPdf01_SingleLayer);
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
      AddPdf(pdf,Z,2,kPdf01_SingleLayer);
    }
  }
  else {
    printf("TrPdfDB::LoadPierrePdf01-W no root file specified. No single plane PDF created.\n");
  }
 
  // Create PDFs for truncated mean evaluation [sqrt(ADC) units]
  float mu[8]    = {6.249,12.533,19.02,25.69,32.80,38.16,43,49.74};
  float sigma[8] = {0.726,1.23,1.57,2.1,3.1,2.50,5,3.30};
  for (int i=0; i<8; i++) {
    int Z = i+1;
    TrPdf* pdf = new TrPdf(Form("TM_X_Z%d",Z));
    for (int ix=0; ix<200; ix++) {
      float x = ix*0.50 + 0.25; 
      float y = TMath::Gaus(x,mu[i],sigma[i],kTRUE); 
      pdf->AddPoint(x,y);
    }
    AddPdf(pdf,Z,0,kPdf01_TruncatedMean);
  }
}


void TrPdfDB::LoadPierrePdf02() {
  // n-side
  // layer/charge/par
  double n_pars[9][8][5] = {
    {
      {   5.6822,  35.1983,    47058,   2.9345, 166.4023 },
      {   2.8323,  34.8385,  5004198,   3.8065,  74.6070 },
      {   2.4496,  38.7304,    22941,   3.5830,  68.2272 },
      {   1.7566,  41.1337,    10987,   3.4832, 497.3727 },
      {   1.9593,  42.1575,    29664,   3.0281, 100.4978 },
      {   0.9589,  42.9072,    94680,   3.6299,  80.0741 },
      {   3.1338,  43.3134,    30359,   1.5676,  62.4723 },
      {   0.8671,  42.4331,    65768,   3.4343,  62.7964 }
    },
    {
      {   5.2300,  38.0537,   458710,   4.1705, 139.5392 },
      {   3.1375,  38.0905, 15865250,   3.9333,  77.1850 },
      {   2.7542,  42.5525,    74709,   3.6964,  70.4416 },
      {   2.2673,  44.9276,    36274,   3.2996,  69.4135 },
      {   1.9005,  46.0992,    94658,   3.1792,  70.5721 },
      {   1.3423,  46.7390,   270799,   3.4061,  67.3113 },
      {   2.5024,  46.9960,    85562,   2.4120,  57.0670 },
      {   1.4465,  46.2449,   189313,   3.0984,  56.3366 }
    },
    {
      {   3.8253,  37.2161,   604875,   5.2833, 137.9804 },
      {   2.6478,  38.6455, 17515330,   4.2455,  75.4533 },
      {   2.0960,  43.0157,    82372,   4.0981,  70.0975 },
      {   1.4451,  45.6712,    39649,   3.8472,  71.1418 },
      {   1.0927,  46.8615,   100625,   3.8114,  74.4411 },
      {   0.2062,  48.4436,   280035,   4.3102,  78.9448 },
      {   1.6204,  46.8761,    86610,   2.9796,  68.8889 },
      {   0.3430,  46.9727,   185864,   3.9524,  63.8339 }
    },
    {
      {   4.2407,  36.9934,   637794,   4.7364, 134.5145 },
      {   2.8335,  38.3149, 18868660,   3.9829,  75.8718 },
      {   2.3059,  42.7089,    88915,   3.7979,  70.0348 },
      {   1.4704,  45.3249,    41626,   3.8615,  72.4719 },
      {   1.2560,  46.3805,   107084,   3.6566,  72.9829 },
      {   1.3937,  46.5441,   330483,   3.1895,  56.8098 },
      {   1.7418,  46.5026,    90919,   2.9211,  68.3969 },
      {   0.3306,  46.7823,   189609,   3.9840,  65.7883 }
    },
    {
      {   4.2442,  36.6655,   526645,   4.4398, 104.9560 },
      {   2.7023,  38.2866, 16788469,   4.0797,  74.7779 },
      {   2.2297,  42.6493,    78848,   3.7667,  68.4531 },
      {   1.4358,  45.4409,    37870,   3.7783,  69.8786 },
      {   1.1911,  46.6319,    96792,   3.4912,  72.7743 },
      {   0.5977,  47.4932,   270142,   3.7811,  68.1077 },
      {   1.7476,  46.8288,    81843,   2.7342,  68.7596 },
      {   0.8399,  46.3370,   179437,   3.4032,  56.0195 },
    },
    {
      {   4.0941,  37.2684,   392586,   5.0868, 113.0869 },
      {   2.3930,  38.5576, 13281435,   5.2508,  77.6521 },
      {   1.8383,  42.9488,    61585,   4.9201,  72.1904 },
      {   1.2563,  45.4513,    29717,   4.4645,  72.2804 },
      {   0.9960,  46.6024,    76611,   4.1483,  73.9652 },
      {   0.6178,  47.1858,   223108,   4.1583,  68.1710 },
      {   1.6219,  46.4960,    66775,   3.3424,  69.8942 },
      {   0.2729,  46.9276,   138727,   4.3396,  70.1674 },
    },
    {
      {   4.1377,  37.5732,   568048,   5.2028, 125.8618 },
      {   2.6411,  38.4589, 17986723,   4.2221,  75.6184 },
      {   2.0090,  42.6684,    83967,   3.9648,  69.7127 },
      {   1.3818,  45.1204,    40098,   3.7785,  70.2053 },
      {   1.0844,  46.1092,   101662,   3.5928,  72.6596 },
      {   0.4441,  47.1963,   286032,   3.9172,  69.7465 },
      {   1.5966,  46.1665,    86083,   2.8117,  68.1290 },
      {   0.3797,  46.2672,   178995,   3.7436,  63.5697 }
    },
    {
      {   4.3428,  38.4641,   499018,   5.2986, 137.1788 },
      {   2.7737,  38.8990, 17807129,   4.0373,  76.8327 },
      {   2.3233,  43.3360,    84000,   3.7919,  69.6057 },
      {   1.9134,  45.6695,    41129,   3.2790,  67.7165 },
      {   1.2440,  47.0946,   102393,   3.6359,  73.4402 },
      {   0.6903,  47.8076,   289098,   3.8108,  67.9475 },
      {   1.7169,  47.2448,    86543,   2.9580,  69.4998 },
      {   0.0752,  48.8511,   200630,   4.0887,  74.8251 }
    },
    {
      {   4.6347,  35.9548,    64152,   4.3580, 151.2744 },
      {   2.5466,  36.5793,  2295715,   4.8134,  80.2714 },
      {   2.1260,  40.6504,     9440,   4.0557,  67.7673 },
      {   0.9901,  43.5881,     4417,   4.7827, 432.7164 },
      {   1.6653,  43.4561,    12226,   3.1596,  52.6221 },
      {   1.1221,  44.0453,    35874,   3.4790,  65.0296 },
      {   2.2550,  43.8403,    10853,   2.6001,  64.4697 },
      {   1.3096,  43.0712,    24108,   3.4790,  58.9199 }
    } 
  };
  for (int layer=1; layer<=9; layer++) { 
    for (int z=1; z<=8; z++) {
      TrPdf* pdf = new TrPdf(Form("X_Z%02d_Layer%1d",z,layer));
      double scale[4] = {2,5,10,100};
      double offset = 0;
      for (int iy=0; iy<4; iy++) {
        for (int ix=0; ix<100; ix++) {
          double x = ix*scale[iy] + offset; 
          double par[5];
          for (int ipar=0; ipar<5; ipar++) par[ipar] = n_pars[layer-1][z-1][ipar];
          double y = newlangauexpfun(&x,par);
          pdf->AddPoint(x,y);
        }
        offset += 100*scale[iy];  
      }
      pdf->Normalize();
      AddPdf(pdf,z,0,kPdf02_SingleLayer,layer);
    }
  }

  // p-side
  // layer/charge/par
  double p_pars[9][3][5] = {
    {
      {   3.2872,  33.2309,    38274,   4.8219,  43.3303 },
      {   2.2833,  32.5905,  5031390,   3.9769,  65.5230 },
      {   0.8430,  33.7946,    21772,   4.8521, 128.1446 }
    },
    {
      {   2.3137,  35.4139,   359050,   5.7993,  45.5921 },
      {   2.5768,  35.2311, 15792231,   3.9343,  45.7535 },
      {   1.2319,  36.3082,    69822,   4.7776,  62.4857 }
    },
    {
      {   2.6996,  35.3827,   532225,   5.6470,  46.2198 },
      {   0.0099,  37.0131,329386229,   4.9908,  56.9095 },
      {   1.1985,  36.1283,    79502,   4.0760,  60.4400 },
    },
    {
      {   2.6509,  35.1346,   549425,   5.3997,  45.6203 },
      {   0.3062,  36.9254, 15388664,   5.6057,  45.2694 },
      {   1.0518,  35.6985,    84581,   4.9125,  62.1513 }
    },
    {
      {   2.4306,  35.4876,   447806,   5.5677,  45.9654 },
      {   2.4461,  36.5487, 17103519,   3.5455,  66.1752 },
      {   1.4200,  36.5692,    75922,   3.4801,  59.4433 }
    },
    {
      {   3.0058,  35.3752,   350098,   5.2633,  46.1580 },
      {   2.4930,  35.9500, 13778805,   3.7692,  65.4888 },
      {   1.3079,  36.3230,    60907,   4.3153,  60.0864 }
    },
    {
      {   3.1026,  34.9775,   515042,   5.0756,  45.7374 },
      {   0.0303,  36.6051, 69050467,   4.9634,  50.7363 },
      {   1.0192,  34.4787,    81070,   4.8741,  59.8886 }
    },
    {
      {   4.2382,  36.7689,   485418,   4.5238,  48.1435 },
      {   2.4468,  35.2572, 17952616,   3.9526,  65.5858 },
      {   1.2018,  35.5422,    79223,   4.6059,  60.0899 }
    },
    {
      {   2.9422,  32.8428,    55631,   5.0555,  43.2773 },
      {   1.9692,  32.8596,  2344222,   5.0543,  68.4274 },
      {   0.8083,  32.8192,     9477,   6.0952,  56.4747 }
    }
  };
  for (int layer=1; layer<=9; layer++) {
    for (int z=1; z<=3; z++) {
      TrPdf* pdf = new TrPdf(Form("Y_Z%02d_Layer%1d",z,layer));
      double scale[4] = {2,5,10,100};
      double offset = 0;
      for (int iy=0; iy<4; iy++) {
        for (int ix=0; ix<100; ix++) {
          double x = ix*scale[iy] + offset;
          double par[5];
          for (int ipar=0; ipar<5; ipar++) par[ipar] = p_pars[layer-1][z-1][ipar];
          double y = newlangauexpfun(&x,par);
          pdf->AddPoint(x,y);
        }
        offset += 100*scale[iy];
      }
      pdf->Normalize();
      AddPdf(pdf,z,1,kPdf02_SingleLayer,layer);
    }
  }

}
 
 


TH1D* TrPdfDB::GetSliceY(TH2D* th2, int ixbin) {
  if (th2==0) {
    printf("TrPdfDB::GetSliceY()-W no histogram to be sliced, return 0.\n");
    return 0;
  }
  // binning
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


double TrPdfDB::newlangauexpfun(double *x, double *par) {
  // par[0] = Width (scale) parameter of Landau density
  // par[1] = Most Probable (MP, location) parameter of Landau density
  // par[2] = Total area (integral -inf to inf, normalization constant)
  // par[3] = Width (sigma) of convoluted Gaussian function
  // par[4] = Separation point between langaus and exp (connected by derivative and continuity)
  // In the Landau distribution (represented by the CERNLIB approximation), 
  // the maximum is located at x=-0.22278298 with the location parameter=0.
  // This shift is corrected within this function, so that the actual
  // maximum is identical to the MP parameter.
  // The additional exponential function is connected imposing 2 condition
  // - continuity: LanGauss(sep) = k * exp(-xsep/slope)
  // - derivative continuity: LanGauss'(xsep) = -k/slope * exp(-xsep/slope)
  // slope = - LanGauss(xsep) / LanGauss'(xsep)
  // k     =   LanGauss(xsep) * exp(xsep/slope)
  if (x[0]<=par[4]) { //Landau and Gauss convolution, par[4] giving the separation point
    // Numeric constants
    double invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
    double mpshift  = -0.22278298;       // Landau maximum location
    // Control constants
    double np = 100.0;      // number of convolution steps
    double sc =   5.0;      // convolution extends to +-sc Gaussian sigmas
    // Variables
    double xx;
    double mpc;
    double fland;
    double sum = 0.0;
    double xlow,xupp;
    double step;
    double i;
    // MP shift correction
    mpc = par[1] - mpshift * par[0]; 
    // Range of convolution integral
    xlow = x[0] - sc * par[3];
    xupp = x[0] + sc * par[3];
    step = (xupp-xlow) / np;
    // Convolution integral of Landau and Gaussian by sum
    for (i=1.0; i<=np/2; i++) {
      xx = xlow + (i-.5) * step;
      fland = TMath::Landau(xx,mpc,par[0]) / par[0];
      sum += fland * TMath::Gaus(x[0],xx,par[3]);
      xx = xupp - (i-.5) * step;
      fland = TMath::Landau(xx,mpc,par[0]) / par[0];
      sum += fland * TMath::Gaus(x[0],xx,par[3]);
    }
    return (par[2] * step * sum * invsq2pi / par[3]);
  }
  // Exponential tail
  else { 
    // Control constants
    double dx = 10.;    // infinitesimal interval for derivative calculation 
                        // must be longer than the bin width (how implement this check?)
    // Variables   
    double xsep; 
    double xpre; 
    double fxsep;
    double fxpre;
    double deriv;
    double slope;
    double k;     
    // Derivative calculation
    xsep  = par[4];
    xpre  = par[4] - dx;
    fxsep = newlangauexpfun(&xsep,par);
    fxpre = newlangauexpfun(&xpre,par);
    deriv = (fxsep-fxpre)/dx;
    // check of existence
    if (deriv==0.) return -1e+06;  
    slope = -fxsep/deriv; 
    if (slope==0.) return -1e+06;  
    if ((xsep/slope)>709.) return -1e+06;
    k = fxsep * exp(xsep/slope);
    return k * exp(-x[0]/slope);
  }
}

