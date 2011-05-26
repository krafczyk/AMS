#include "TrPdf.h"


ClassImp(TrPdf);


TrPdf::TrPdf(char* name, char* filename) { 
  Init(); 
  Name = name; 
  ReadTextFile(filename); 
}


TrPdf::TrPdf(char* name, TH1* histo, bool divide, bool normalize) { 
  Init(); 
  Name = name; 
  ReadTH1(histo, divide, normalize); 
}


TrPdf::TrPdf(char* name, TF1* fun, int nbins, double xlow, double xhig, bool normalize) {
  Init(); 
  Name = name; 
  ReadTF1(fun, nbins, xlow, xhig, normalize); 
}


void TrPdf::Init() {
  Name.Clear();
  N = 0;        
  X.reserve(10); 
  Y.reserve(10);
  IY.reserve(10);
  Graph = 0;
  LogLog = 0;
  SpeInd = 0;
}


void TrPdf::Copy(const TrPdf& orig) {
  Name = orig.Name;
  N = orig.N;
  X = orig.X;
  Y = orig.Y;
  IY = orig.IY;
  if (orig.Graph!=0)  Graph  = (TGraph*) orig.Graph->Clone(); // ?
  if (orig.LogLog!=0) LogLog = new TF1(*orig.LogLog);
  if (orig.SpeInd!=0) SpeInd = new TF1(*orig.SpeInd);
}


TrPdf& TrPdf::operator=(const TrPdf& that) {
  if (this!=&that) Copy(that); 
  return *this;
}


void TrPdf::Clear(Option_t* oo) {
  TObject::Clear(oo);
  Name.Clear();
  N = 0;
  X.clear();
  Y.clear();
  IY.clear();
  if (Graph!=0) Graph->Delete();
  Graph = 0;
  if (LogLog!=0) LogLog->Delete();
  LogLog = 0;
  if (SpeInd!=0) SpeInd->Delete();
  SpeInd = 0;
}


void TrPdf::Info(int verbosity) {
  printf("------- Info -------- \nName = %s\nNPoints = %2d \n",GetName().Data(),N);
  if (verbosity>0) {
    printf("More Info: The Point List (i,X,Y,DY)\n");
    if (N==0) printf("Empty!\n");
    for (int i=0; i<GetN(); i++) {
      printf("%3d %12.5g %12.5g\n",i,X.at(i),Y.at(i));
    }
  }
  if (verbosity>1) {
    printf("More Info: List of Fitting Parameters\n");
    printf("Log-Log Fitting:\n");
    if (GetLogLog()!=0) {
      for (int i=0; i<6; i++) {
        printf("parameter %1d = %10.5g\n",i,GetLogLog()->GetParameter(i));
      }
    } 
    else {
      printf("Empty!\n");
    }
    printf("Spectral Index Fitting:\n");
    if (GetSpeInd()!=0) {
      printf("normalization = %10.5g\n",GetSpeInd()->GetParameter(0));
      printf("spectral ind. = %10.5g\n",GetSpeInd()->GetParameter(1));
    }
    else { 	
      printf("Empty!\n");
    }
  }
  if (verbosity>2) {
    printf("More-and-more Info: The Integral List (i,X,Y)\n");
    if (int(IY.size())==0) printf("Empty!\n");
    for (int i=0; i<int(IY.size()); i++) {
      printf("%3d %12.5g %12.5g\n",i,X.at(i),IY.at(i));
    }
  }
  printf("------- End ---------\n"); 
}


void TrPdf::AddPoint(double x, double y) {
  for (int i=0; i<N; i++) {
    // insert if is in the middle
    if (x<X.at(i)) {
      X.insert(X.begin()+i,x);
      Y.insert(Y.begin()+i,y);
      N++;
      return;
    }
  }
  // if no greater than add at the end of the buffer 
  X.push_back(x);
  Y.push_back(y);
  N++;                   
}


void TrPdf::ScaleY(double scale) { 
  for (int i=0; i<N; i++) {
    SetY(i,GetY(i)*scale);
  }
}


double TrPdf::Eval(double x, bool logx, bool logy) {
  if (x<GetX(0)) {
    if (VERBOSE) printf("TrPdf::Eval-V requested x under lower bound, returning 0.\n");
    return 0.; 
  }
  if (x>GetX(GetN()-1)) { 
    if (VERBOSE) printf("TrPdf::Eval-V requested x over upper bound, returning 0.\n");
    return 0.;
  }
  double value = 0.;
  for (int i=0; i<GetN()-1; i++) {
    if ( (x>=X.at(i))&&(x<=X.at(i+1)) ) {
      double y1  = (logy) ? log10(GetY(i+1)) : GetY(i+1);
      double y0  = (logy) ? log10(GetY(i))   : GetY(i);
      double x1  = (logx) ? log10(GetX(i+1)) : GetX(i+1);
      double x0  = (logx) ? log10(GetX(i))   : GetX(i);
      double xx  = (logx) ? log10(x)         : x;
      value = (y1*(xx-x0) + y0*(x1-xx))/(x1-x0);
    }
  }
  return (logy) ? pow(10.,value) : value;
}


void TrPdf::Multiply(TrPdf* that, bool logx, bool logy) {
  for (int i=0; i<GetN(); i++) {
    double y0  = GetY(i);
    double y1  = that->Eval(GetX(i),logx,logy); 
    SetY(i,y0*y1);
  }
}
 

void TrPdf::Divide(TrPdf* that, bool logx, bool logy) {
  for (int i=0; i<GetN(); i++) {
    double y0  = GetY(i);
    double y1  = that->Eval(GetX(i),logx,logy); 
    SetY(i,y0/y1);
  }
}


void TrPdf::Sum(TrPdf* that, bool logx, bool logy) {
  for (int i=0; i<GetN(); i++) {
    double y0  = GetY(i);
    double y1  = that->Eval(GetX(i),logx,logy); 
    SetY(i,y0+y1);
  }
}


void TrPdf::Subtract(TrPdf* that, bool logx, bool logy) {
  for (int i=0; i<GetN(); i++) {
    double y0  = GetY(i);
    double y1  = that->Eval(GetX(i),logx,logy);
    SetY(i,y0-y1);
  }
}


void TrPdf::MultiplyPowerLaw(double index) {
  for (int i=0; i<GetN(); i++) {
    SetY(i,GetY(i)*pow(GetX(i),index));
  }
}


double TrPdf::Integral(int first, int last) {
  if (first<0) { 
    if (VERBOSE) printf("TrPdf::Integral-V first index out of bounds (%d, minimum is 0), using 0.\n",first); 
    first = 0; 
  }
  if (last>GetN()-1) { 
    if (VERBOSE) printf("TrPdf::Integral-V last index out of bounds (%d, maximum is N-1 = %d), using %d.\n",last,N-1,N-1); 
    last = GetN()-1; 
  }
  if (first>last) { 
    if (VERBOSE) printf("TrPdf::Integral-V first index greater than last (%d, %d), returning 0.\n",first,last); 
    return 0.; 
  } 
  if (first==last) return 0.;
  double sum = 0.;
  for (int i=first; i<last; i++) {
    double y1 = GetY(i+1);
    double y0 = GetY(i);
    double x1 = GetX(i+1);
    double x0 = GetX(i);
    sum += (y1+y0)*(x1-x0)/2.; 
  }
  return sum;
}


double TrPdf::Integral(double xlow, double xhig) {	
  if (xlow<GetX(0)) {
    if (VERBOSE) printf("TrPdf::Integral-V xlow out of bounds, using low edge.\n");
    xlow = GetX(0);
  }
  if (xhig>GetX(GetN()-1)) {
    if (VERBOSE) printf("TrPdf::Integral-V xhigh out of bounds, using upper edge.\n");
    xhig = GetX(GetN()-1);
  }
  if (xlow>=xhig) {
    if (VERBOSE) printf("TrPdf::Integral-V xlow greater-equal than xhig, returning 0.\n");
    return 0.;
  }
  double sum = 0.;
  double y1,y0,x1,x0;
  for (int i=0; i<GetN()-1; i++) {
    if      (xlow<=GetX(i))   { x0 = GetX(i);   y0 = GetY(i);    }
    else if (xlow>=GetX(i+1)) { x0 = GetX(i+1); y0 = GetY(i+1);  } 
    else                      { x0 = xlow;      y0 = Eval(xlow); }
    if      (xhig<=GetX(i))   { x1 = GetX(i);   y1 = GetY(i);    }
    else if (xhig>=GetX(i+1)) { x1 = GetX(i+1); y1 = GetY(i+1);  }
    else                      { x1 = xhig;      y1 = Eval(xhig); }
    sum  += (y1+y0)*(x1-x0)/2.;
  }
  return sum;
}


void TrPdf::CreateIntegral(int force) {
  if (force!=0) IY.clear();
  if (!IY.empty()) return;
  // create integral vector
  double sum = 0.;
  IY.push_back(sum);
  for (int i=1; i<N; i++) {
    double y1 = GetY(i);
    double y0 = GetY(i-1);
    double x1 = GetX(i);
    double x0 = GetX(i-1);
    sum += (y1+y0)*(x1-x0)/2.; 
    IY.push_back(sum);
  }
  return;
}


double TrPdf::GetInvNormCumulative(double y) {
  if (IY.empty()) CreateIntegral();
  if (y<0) return 0;
  if (y>1) return 1;	
  double value = 0.;
  for (int i=0; i<N-1; i++) {
    if ( (y>=GetNormCumulative(i))&&(y<GetNormCumulative(i+1)) ) {
      double y1 = GetX(i+1); 
      double y0 = GetX(i); 
      double x1 = GetNormCumulative(i+1);
      double x0 = GetNormCumulative(i);
      double xx = y;
      value = (y1*(xx-x0) + y0*(x1-xx))/(x1-x0);
    }
  }
  return value;
}


void TrPdf::Normalize() {
  float integral = Integral();
  if (integral>0) ScaleY(1./integral);
}


void TrPdf::ReadTextFile(char* filename, bool normalize) {
  // open file
  FILE* file = fopen(filename,"r");
  if (file==0) { printf("TrPdf::ReadTextFile-W no file named %s found, skipping!\n",filename); return; }	
  // filling with data
  while(!feof(file)) {
    float a = 0; 
    float b = 0;
    int val = fscanf(file,"%f%f",&a,&b);
    if (val==3) AddPoint(a,b);
  }
  // close file
  fclose(file);
  if (normalize) Normalize();
}


void TrPdf::ReadTH1(TH1* histo, bool divide, bool normalize) {
  if (histo==0) {
    printf("TrPdf::ReadTH1-W empty histogram, skipping!\n"); 
    return;
  }
  for (int i=1; i<=histo->GetXaxis()->GetNbins(); i++) {
	double x = histo->GetXaxis()->GetBinCenter(i); 
	double y = histo->GetBinContent(i);
	if (divide) y /= histo->GetXaxis()->GetBinWidth(i);
	AddPoint(x,y);
  }
  if (normalize) Normalize();
}

void TrPdf::ReadTF1(TF1* fun, int nbins, double xlow, double xhig, bool normalize) {
  if (fun==0) {
    printf("TrPdf::ReadTF1-W empty function, skipping!\n"); 
    return;
  }
  for (int i=0; i<nbins; i++) {
    double x = xlow + (i+0.5)*(xhig-xlow)/nbins; 
    double y = fun->Eval(x);
    AddPoint(x,y);
  }
  if (normalize) Normalize();		
}


TGraph* TrPdf::CreateGraph() {	
  if (Graph!=0) ClearGraph();
  float* x = new float[N];
  float* y = new float[N];
  for (int i=0; i<N; i++) {
    x[i] = X.at(i);
    y[i] = Y.at(i);
  }
  Graph = new TGraph(N,x,y);
  delete [] x;
  delete [] y;
  return Graph;
}


TGraph* TrPdf::GetGraph(int force) { 	
  if ( (Graph==0)||(force!=0) ) CreateGraph(); 
  return Graph; 
}


void TrPdf::FitLogLog(double min, double max) {
  TGraph* FluxLogLogTmp = new TGraph(GetGraph()->GetN());
  FluxLogLogTmp->SetName("spectrumloglogtmp");
  FluxLogLogTmp->SetTitle("spectrumloglogtmp");
  for (int ii=0; ii<Graph->GetN(); ii++) {
    double a,b;
    GetGraph()->GetPoint(ii,a,b);
    FluxLogLogTmp->SetPoint(ii,log10(a),log10(b));
  }
  TF1* LinFitTmp = new TF1("linfittmp","[0]+[1]*x+[2]*pow(x,2.)+[3]*pow(x,3.)+[4]*pow(x,4.)+[5]*pow(x,5.)",-2.,5.); 
  FluxLogLogTmp->Fit(LinFitTmp,"EQR","",log10(min),log10(max));
  LogLog = new TF1(Form("LogLog_%s",GetName().Data()),
    "pow(10.,[0]+[1]*log10(x)+[2]*pow(log10(x),2.)+[3]*pow(log10(x),3.)+[4]*pow(log10(x),4.)+[5]*pow(log10(x),5.))",1.e-2,1.e5);
  for (int i=0; i<6; i++) LogLog->SetParameter(i,LinFitTmp->GetParameter(i));
  delete LinFitTmp;
  delete FluxLogLogTmp;
}


void TrPdf::FitSpeInd(double min, double max)  {
  SpeInd = new TF1(Form("SpeInd_%s",GetName().Data()),"[0]*pow(x,[1])",1.e-2,1.e5);
  SpeInd->SetParameters(0.001,-2.7);
  GetGraph()->Fit(SpeInd,"EQR","",min,max);
}

