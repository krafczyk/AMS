//  $Id: Tofcharge_ihep.C,v 1.2 2012/11/09 00:37:45 qyan Exp $

// ------------------------------------------------------------
//      AMS TOF Charge and PDF Likelihood (BetaH Version)
// ------------------------------------------------------------
//      History
//        Created:       2012-Oct-01  Q.Yan (qyan@cern.ch) + V.Choutko
// -----------------------------------------------------------
#include "TMinuit.h"
#include "TMath.h"
#include "Tofrec02_ihep.h"
#include "Tofcharge_ihep.h"

// **************************************************************
// TofChargePar for Each Layer Q
// **************************************************************
ClassImp(TofChargePar)

float TofChargePar::GetQ(){
  number QL; 
  TofPDFH::ChooseDA(Layer,Bar,6.,number(QLA),number(QLD),number(QLARaw),number(QLDRaw),QL);//Choose
  return QL>0?sqrt(QL):float(QL);
}

///=======================================================
float TofChargePar::GetProbZ(float Z){

   float probz=TofPDFH::GetProbZ(Layer,Bar,number(Z),number(QLA), number(QLD),number(QLARaw),number(QLDRaw),number(Beta));//float double
   return probz;   
}

///=======================================================
void   TofChargePar::FillProbZ(int ZL,int ZH){

   for(int Z=ZL;Z<=ZH;Z++){
      ProbZ.push_back(make_pair<int, float>(Z,GetProbZ(float(Z))));
   }
}

// **************************************************************
// Tof TofLikelihoodPar for Each Z
// **************************************************************
ClassImp(TofLikelihoodPar)

bool TofLikelihoodPar::IsUsedL (int iLayer){

  if(LPattern/int(pow(10., 3-iLayer))==1)return 1;
  else   return 0;
}

///=======================================================
int TofLikelihoodPar::GetnLayer(){

  int nLayer=0;
  for(int ilay=0;ilay<4;ilay++){
    if(IsUsedL(ilay))nLayer++;
  }
  return nLayer;
}

// **************************************************************
// TofChargeHR for Each All-Z All Measurment
// **************************************************************
ClassImp(TofChargeHR)

void TofChargeHR::Init(){
   cpar.clear();
   like.clear();
   likeQ.clear();
   fTrTrack=-1;
}

///=======================================================
TofChargeHR::TofChargeHR(BetaHR *betah){

    TofPDFH::ReBuild(betah,(*this));
}

///=======================================================
void TofChargeHR::UpdateZ(int pattern){

/// First Check Whether Pattern exist
  map<int, vector<TofLikelihoodPar> >::iterator it;
  it=like.find(pattern);
  if(it!=like.end())return;

/// Charge Measument Par Of Select Good Layer 
  vector<TofChargePar> cparu;

///--Select Matched-TOF Layer
  int nsel=TofPDFH::SelectM(pattern,cpar,fTrTrack,cparu);

///--LikeLihood -Calculation
  vector<TofLikelihoodPar> plike;
  TofPDFH::LikelihoodCal(cparu,plike); 
  like.insert(pair<int,vector<TofLikelihoodPar> >(pattern,plike));

//---LikeLihood Q
  float plikeQ=TofPDFH::GetLikelihoodQ(cparu);
  likeQ.insert(pair<int,float >(pattern,plikeQ));
  
}

///=======================================================
int TofChargeHR::GetNZ(int pattern){
  
/// First Update and Check
  UpdateZ(pattern);
/// Number of Most ProbZ
  return like[pattern].size();  
}

///=======================================================
int TofChargeHR::GetZ(int &nlay,float &Prob,int IZ,int pattern){
 
/// First Update and Check
  UpdateZ(pattern);

//---Get Z Par
  Prob=like[pattern].at(IZ).Prob;
  nlay=like[pattern].at(IZ).GetnLayer();
  return like[pattern].at(IZ).Z; 
}

//=======================================================
float TofChargeHR::GetLikeQ(int &nlay,int pattern){
   
/// First Update and Check
  UpdateZ(pattern);

//---Get Z Par
  nlay=like[pattern].at(0).GetnLayer();
  return likeQ[pattern];

}


// **************************************************************
// TofPDFH TOF-Charge-Likelihood Reconstruction Engine
// **************************************************************
ClassImp(TofPDFH)

vector<TofChargePar> TofPDFH::chargepar;

int TofPDFH::ReBuild(BetaHR *betah,TofChargeHR &tofch){

//---TDV Init
    TofRecH::Init();

//--Clear
    tofch.Init();

//--Clear   
   vector<TofChargePar> cpar;

//---push_back of cpar
   int  MinZ=2*TofPDFPar::ZHLim,MaxZ=1,MinIZ=0,MaxIZ=0;
   float LProb=FLT_MAX,HProb=FLT_MAX;
   float Beta=betah->GetBeta(); 
   int  fTrTrack=betah->iTrTrack();
//---Beta
   for(int il=0;il<4;il++){
     if(betah->TestExistHL(il)){
//#ifdef __ROOTSHAREDLIBRARY__
       int   Bar=betah->GetClusterHL(il)->Bar;
       float QLA=betah->GetQL(il,1,TofClusterHR::DefaultQ2Opt);
       float QLD=betah->GetQL(il,0,TofClusterHR::DefaultQ2Opt);
       float QLARaw=betah->GetQL(il,1,(TofRecH::kThetaCor|TofRecH::kBirkCor|TofRecH::kReAttCor));
       float QLDRaw=betah->GetQL(il,0,(TofRecH::kThetaCor|TofRecH::kBirkCor|TofRecH::kReAttCor));
       bool  IsGoodPath=betah->IsGoodQPathL(il);
//----Set Par
       TofChargePar cparl(il,Bar,QLA,QLD,QLARaw,QLDRaw,Beta,IsGoodPath);
       float QL=cparl.GetQ();
       if(QL<=0||QL>TofPDFPar::ZHLim)continue;//OverFlow Or <=0 Not Used
//----Limit
       cpar.push_back(cparl);
       int QZ=int(QL+0.5)>=1?int(QL+0.5):1;
       if (QZ<MinZ||(QZ==MinZ&&cparl.GetProbZ(float(MinZ))<LProb)){
           MinZ=QZ;MinIZ=cpar.size()-1; LProb=cparl.GetProbZ(float(MinZ));
        } 
       if (QZ>MaxZ||(QZ==MaxZ&&cparl.GetProbZ(float(MaxZ))<HProb)){
           MaxZ=QZ;MaxIZ=cpar.size()-1; HProb=cparl.GetProbZ(float(MaxZ));
        }
//#endif
    }
  }

//--- Not Exist Layer Exist
  if(cpar.size()==0)return -1;

//--- Most Prob Z push_back Prob
  const int ZEXTM=4;//MAX-STEP
  for(int Z=MinZ-1;Z>=1;Z--){
     float ProbZ=cpar.at(MinIZ).GetProbZ(float(Z));
     if(ProbZ<=TofPDFPar::ProbLimit||MinZ-Z>=ZEXTM){MinZ=Z;break;}
  }
  for(int Z=MaxZ+1;Z<TofPDFPar::ZHLim;Z++){
     float ProbZ=cpar.at(MaxIZ).GetProbZ(float(Z));
     if(ProbZ<=TofPDFPar::ProbLimit||Z-MaxZ>=ZEXTM){MaxZ=Z;break;}
  }


//--Fill Prob for Z
  for(int im=0;im<cpar.size();im++){
     cpar.at(im).FillProbZ(MinZ,MaxZ);
  }

  
//---Copy to TofChargeHR
  tofch.cpar=cpar;
  tofch.fTrTrack=fTrTrack;
  tofch.UpdateZ(); 

  return 0; 
}

//=======================================================
void TofPDFH::LikelihoodCal(vector<TofChargePar> cpars,vector<TofLikelihoodPar> &like){

//---Cal Likehood
    like.clear();

/// Likelihood Use Layer Pattern
    int   lpattern=0;
    for(int im=0;im<cpars.size();im++){lpattern+=int(pow(10.,3-cpars.at(im).Layer));}

/// Protection
    if(cpars.size()==0){like.push_back(TofLikelihoodPar(lpattern,-1,0));return;}

/// For Each Z Calculate 
    float slike=0;int nhit=cpars.size();
    for(int iz=0;iz<cpars.at(0).ProbZ.size();iz++){
       float plike=GetLikelihood(iz,cpars);
       int   zvar=cpars.at(0).ProbZ.at(iz).first;
       like.push_back(TofLikelihoodPar(lpattern,zvar,plike));
       slike+=exp(plike/nhit);
    }

/// Sort Form MaxLikelihood To MinLikelihood
    sort(like.begin(),like.end());

/// Calculate Prob
    for(int ilk=0;ilk<like.size();ilk++){
       like.at(ilk).Prob=exp(like.at(ilk).Likelihood/nhit)/slike;
   }

}

//=======================================================
number TofPDFH::GetLikelihood(int IZ,vector<TofChargePar> cpars){

   number likelihood=0;
   for(int im=0;im<cpars.size();im++){
       number probv=cpars.at(im).ProbZ.at(IZ).second;
       likelihood+=log(probv);
   }
   return likelihood;
}


//=======================================================
number TofPDFH::GetLikelihoodQ(vector<TofChargePar> cpars){

//---GetLimit And Imformation
    number DZ=0,LZ=2*TofPDFPar::ZHLim,HZ=0,QL;
    int nm=0;
    for(int im=0;im<cpars.size();im++){
       QL=cpars.at(im).GetQ();
       if(QL<=0){cout<<"<<----Error Q"<<endl;continue;}
       if(QL<LZ)LZ=QL;
       if(QL>HZ)HZ=QL;
       DZ+=QL;
       nm++;
    }
    DZ/=nm;
    if     (nm==0)return -1;//Field
    else if(nm==1)return DZ;;

///---
    chargepar=cpars;

///---
    TVirtualFitter::SetDefaultFitter("Minuit");
    TVirtualFitter * minuit = TVirtualFitter::Fitter(0,1);
    minuit->SetFCN(GetLikelihoodF);

//-----begin    
   number arglist[100];
   arglist[0] = -1;
   // set print level
   minuit->ExecuteCommand("SET PRINT",arglist,2);
   minuit->ExecuteCommand("SET NOW",arglist,1);

   //--setpar
   const float ZEXT=3;
   LZ=(LZ-ZEXT)>0? LZ-ZEXT:0;
   HZ=HZ+ZEXT;
   minuit->SetParameter(0,"TOF_LikeQ",DZ,0.1,LZ,HZ);//--Like Q

   // minimize
   arglist[0] = 1000; // number of function calls
   arglist[1] = 0.01; // tolerance
   minuit->ExecuteCommand("MIGRAD",arglist,2);

//---
   number Q=minuit->GetParameter(0);

   return Q;
}


//=======================================================
void TofPDFH::GetLikelihoodF(Int_t & /*nPar*/, Double_t * /*grad*/ , Double_t &fval, Double_t *par, Int_t /*iflag */ ){
 
   number likelihood=0;
   for(int im=0;im<chargepar.size();im++){
       number probv=chargepar.at(im).GetProbZ(par[0]);
       likelihood+=log(probv);
   }
   fval=-likelihood;
}


//=======================================================
int TofPDFH::SelectM(int pattern,vector<TofChargePar> cpar,int fTrTrack,vector<TofChargePar> &cpars){
   
//---Copy     
   cpars=cpar;

///---Erase Nagtive
    for(int im=0;im<cpars.size();im++){
      if(cpars.at(im).GetQ()<=0){
         cpars.erase(cpars.begin()+im);//0 Erase
         im--;
      }
    }


//--Use Select Pattern
    if(pattern>0){
      for(int im=0;im<cpars.size();im++){
         int ilay=cpars.at(im).Layer;
         if(pattern/int(pow(10.,3-ilay))%10==1)continue;//Select Countinue;
         cpars.erase(cpars.begin()+im);//0 Erase
         im--;
      }
    }

//---Pool PathLength ReMove // -2
    if(pattern<0&&(pattern>=-10)&&fTrTrack>=0&&cpars.size()>2){
       for(int im=0;im<cpars.size();im++){
          if(cpars.at(im).IsGoodPath)continue;//Good Continue
          cpars.erase(cpars.begin()+im);//Bad Erase
          im--;
          if(cpars.size()<=2)break;
       }
    }

//-----   
    if(cpars.size()<=2||pattern>0||pattern==-10){
     }
//---ReMove Bad-Q
    else if(pattern%10==-1) {
       vector<int>WL;vector<int>OL;
       for(int iz=0;iz<cpars.at(0).ProbZ.size();iz++){//for All Z Find Max-Windows Layer
//---
         vector<int>WNL;vector<int>ONL;//Z in Windows
         for(int im=0;im<cpars.size();im++){
           if(cpars.at(im).ProbZ.at(iz).second>TofPDFPar::ProbLimit)WNL.push_back(im);
           else ONL.push_back(im);
         }
         if(WNL.size()>WL.size()){WL=WNL;OL=ONL;}
//----
      }
      for(int im=OL.size()-1;im>=0;im--){cpars.erase(cpars.begin()+OL.at(im));if(cpars.size()<=2)break;}
    }
///--ReMove Max-Q
    else if(pattern%10==-2){
       float  maxq=0; int imax=0;
       for(int im=0;im<cpars.size();im++){
         if(cpars.at(im).GetQ()>maxq){maxq=cpars.at(im).GetQ();imax=im;}
       }
       cpars.erase(cpars.begin()+imax);//0 Erase
    }
    

    return cpars.size();
}

//=======================================================
number TofPDFH::GetProbZ(int ilay,int ibar,number Z,number QLA,number QLD,number QLARaw,number QLDRaw,number betah){//before betacor

//--Protection
  if(Z<=0)return TofPDFPar::ProbLimit;
//--No Signal or OverFlow
  if((QLA<=0)&&(QLD<=0))return TofPDFPar::ProbLimit;

//---Choose Dynode Or Anode
  number QL;
  int isanode=ChooseDA(ilay,ibar,Z,QLA,QLD,QLARaw,QLDRaw,QL);

  number probv;
  for(int iz=0;iz<TofPDFPar::nPDFCh;iz++){
    if(Z==TofPDFPar::PDFCh[iz]){//Find Charge PDF
      probv= GetProbZI(ilay,ibar,iz,QL,betah,isanode);break;
    }
    else if(iz==TofPDFPar::nPDFCh-1||Z<TofPDFPar::PDFCh[0]){//Not Find (Lager Than maxZ =>Shift PDF||Small Than Z=1)
      QL=QL*(TofPDFPar::PDFCh[iz]*TofPDFPar::PDFCh[iz])/(Z*Z);//Much Small Assume Sigmal/Mean=Const
      probv=GetProbZI(ilay,ibar,iz,QL,betah,isanode)*((TofPDFPar::PDFCh[iz]*TofPDFPar::PDFCh[iz])/(Z*Z));//Scale Offset
      break;
    }
    else if(Z>TofPDFPar::PDFCh[iz]&&Z<TofPDFPar::PDFCh[iz+1]){//Middle Not Exist =>Shift PDF
//----
      number QLL=QL*(TofPDFPar::PDFCh[iz]*TofPDFPar::PDFCh[iz])/(Z*Z);//Much Small Assume Sigmal/Mean=Const
      number probvl=GetProbZI(ilay,ibar,iz,QLL,betah,isanode)*((TofPDFPar::PDFCh[iz]*TofPDFPar::PDFCh[iz])/(Z*Z));
      number QLH=QL*(TofPDFPar::PDFCh[iz+1]*TofPDFPar::PDFCh[iz+1])/(Z*Z);
      number probvh=GetProbZI(ilay,ibar,iz+1,QLH,betah,isanode)*((TofPDFPar::PDFCh[iz+1]*TofPDFPar::PDFCh[iz+1])/(Z*Z));
//----
      number wwl=fabs(TofPDFPar::PDFCh[iz+1]*TofPDFPar::PDFCh[iz+1]-Z*Z); 
      number wwh=fabs(Z*Z-TofPDFPar::PDFCh[iz]*TofPDFPar::PDFCh[iz]);
      probv=(wwl*probvl+wwh*probvh)/(wwl+wwh);
      break;
    }
  }

  if(probv<TofPDFPar::ProbLimit)probv=TofPDFPar::ProbLimit;
 
  return probv;
 
}

//=======================================================
int TofPDFH::ChooseDA(int ilay,int ibar,number Z,number QLA,number QLD,number QLARaw,number QLDRaw,number &QL){

//---Choose DA
  int isanode=1;//Default Anode 
  if(Z>=3){//Dynode Better Range
     if(Z>8){isanode=0;}//Dynode Range
     if(QLDRaw>TofPDFPar::DAgate[ilay][ibar]&&(QLARaw>=TofPDFPar::DAgate[ilay][ibar]/1.5||QLARaw<0)){isanode=0;}
  }

//--GetQ Var
  QL=(isanode==1)?QLA:QLD;
  
  return isanode;
}

//=======================================================
number TofPDFH::GetProbZI(int ilay,int ibar,int ZI,number QL,number betah,int isanode){//Index For BetaH
  
  betah=fabs(betah);
  
//-Search Beta Bin
  int ivh=-1,ivl=-1;
  if     (betah>=TofPDFPar::pdfvel[ZI][0]){ivh=0;}//High Vel Exist
  else {
     for(int iv=1;iv<TofPDFPar::nPDFVel;iv++){//Low Vel Exit
       if     (iv==TofPDFPar::nPDFVel-1)                              {ivh=iv;break;}//Find Last One
       else if(TofPDFPar::pdfvel[ZI][iv-1]<=TofPDFPar::pdfvel[ZI][iv]){ivh=iv-1;break;}//Found Decrease Signal
       else if((betah<TofPDFPar::pdfvel[ZI][iv-1])&&(betah>TofPDFPar::pdfvel[ZI][iv])){ivh=iv-1;ivl=iv;break;}//Has Found, Break
    }
 }

  if(ivh<0){cerr<<"<<----TofPDF: Error Find Beta Bin"<<endl;return 0;}
//--- 
 number wwl,wwh;
 if(ivl>=0){ //Interpolation
   wwh=fabs(betah-TofPDFPar::pdfvel[ZI][ivl]);
   wwl=fabs(TofPDFPar::pdfvel[ZI][ivh]-betah);
 }

//--Prepare PDF Function
 typedef Double_t (*pdffun)(Double_t *,Double_t *);
 pdffun  fun;
 if(TofPDFPar::PDFCh[ZI]<=TofPDFPar::ZPDFgate[0]){
    fun=TofPDFH::PDFPrHe;
 }
 else if(TofPDFPar::PDFCh[ZI]<=TofPDFPar::ZPDFgate[1]){//<16
    fun=TofPDFH::PDFBZ;
 }
 else  {
    fun=TofPDFH::PDFHBZ;
 }

//--Prepare and Choose PDF-Par  From Anode Or Dynode
  TofPDFPar *PPar=TofPDFPar::GetHead();
  Double_t *pdfpar[2]={0};//Low+Hihg
  pdfpar[0]=(isanode==1)?PPar->pdfpara[ZI][ivh][ilay][ibar]:PPar->pdfpard[ZI][ivh][ilay][ibar];
  if(ivl>=0)pdfpar[1]=(isanode==1)?PPar->pdfpara[ZI][ivl][ilay][ibar]:PPar->pdfpard[ZI][ivl][ilay][ibar];
//---Get Prob   
   number probv;
   if(ivl<0)     probv= (*fun)(&QL, pdfpar[0]);
   else          probv=((*fun)(&QL, pdfpar[0])*wwh+(*fun)(&QL, pdfpar[1])*wwl)/(wwh+wwl);

   return probv;
}


//=======================================================
Double_t TofPDFH::PDFPrHe(Double_t *x, Double_t *par){
     //  par[0]  -> prob of having exp
     //  par[1]  -> exp(-x/par[1]
     //  par[2]  -> gaus mean
     //  par[3]  -> gaus sigma
     //  par[4]  -> total area
     //  par[5]  -> 2nd exp prob
     //  par[6]  -> 2nd exp param 

     Double_t invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
     double summ[3]={0,0,0};

//--Gaus cov Expo(0)+ Gaus cov Expo(1)
   for(int iex=0;iex<2;iex++){
     double sc=5;
     double sum=0;
     double xmin=par[2]-sc*par[3];
     if(xmin<0)xmin=0;
     double xmax=par[2]+sc*par[3];
     if(xmin>x[0])xmin=x[0];
     if(xmax>x[0])xmax=x[0];
     double np=100;
       //cout << " xminmax "<<xmin<<" "<<xmax<<" "<<x[0]<<endl;

     double step=(xmax-xmin)/np;
     for(int i=1; i<=np; i++) {
         double xx = xmin + float(i-.5) * step;
         double t=x[0]-xx;
         if(iex==0)sum+=exp(-t/par[1])*TMath::Gaus(xx,par[2],par[3]);
         else      sum+=exp(-t/par[6])*TMath::Gaus(xx,par[2],par[3]);

     }
     if(iex==0)sum*=par[0]*(1-par[5])*step;
     else      sum*=par[5]*(1-par[0])*step;
     summ[iex]=sum;
  }

//---Gaus
     summ[2]=(1-par[0])*(1-par[5])*TMath::Gaus(x[0],par[2],par[3]);

//---Sum
      return (summ[0]+summ[1]+summ[2])*par[4];
}

//=======================================================
Double_t TofPDFH::PDFBZ(Double_t *x, Double_t *par){
    // par[0] -> prob of having exp
    // par[1] ->(exp(-x/par[1])
    // par[2] -> gaus mean
    // par[3] -> gaus sigma
    // par[4] -> total area

     Double_t invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)

//----Gaus cov Expo(0)
     Double_t np = 100.0;      // number of convolution steps
     double sc=5;
     double sum=0;
     double xmin=par[2]-sc*par[3];
     if(xmin<0)xmin=0;
     double xmax=par[2]+sc*par[3];
     if(xmin>x[0])xmin=x[0];
     if(xmax>x[0])xmax=x[0];

      //cout << " xminmax "<<xmin<<" "<<xmax<<" "<<x[0]<<endl;
      double step=(xmax-xmin)/np;
      for(int i=1; i<=np; i++) {
         double xx = xmin + float(i-.5) * step;
         double t=x[0]-xx;
         sum+=exp(-t/par[1])*TMath::Gaus(xx,par[2],par[3]);
      }

      sum*=invsq2pi*par[0]*step;

//----Gaus
      sum+=(1-par[0])*TMath::Gaus(x[0],par[2],par[3]);
      sum=sum*par[4]/par[3];
      return sum;
}

//=======================================================
Double_t TofPDFH::PDFHBZ(Double_t *x, Double_t *par){
 
  return TMath::Gaus(x[0],par[1],par[2],1);
}


// **************************************************************
// Tof PDF-TDV-Par
// **************************************************************
TofPDFPar* TofPDFPar::Head=0;

TofPDFPar* TofPDFPar::GetHead(){
  if(!Head)Head = new TofPDFPar();
  return Head;
}

//=======================================================
TofPDFPar::TofPDFPar(){
  TDVName="TofPDFAlign";
  TDVParN=0;
  TDVParN+=2*nPDFCh*nPDFVel*nPDFPar*TOFCSN::NBARN;//Anode+Dynode PDF Par
  TDVBlock=new float[TDVParN];
  TDVSize=TDVParN*sizeof(float);
//---Load
  for(int iblock=0;iblock<TDVParN;iblock++)TDVBlock[iblock++]=0;
  LoadOptPar(0);
  Isload=0;
};

//=======================================================
void   TofPDFPar::LoadOptPar(int opt){

   int iblock=0;
//--Anode PDF Par
   for (int ich=0;ich<nPDFCh;ich++){//Charge 
     for(int iv=0;iv<nPDFVel;iv++){//Vel
       for(int ipar=0;ipar<nPDFPar;ipar++){ //Par
        for(int ilay=0;ilay<TOFCSN::SCLRS;ilay++){
          for(int ibar=0;ibar<TOFCSN::NBAR[ilay];ibar++){//N+P
///----
           if(opt==0)pdfpara[ich][iv][ilay][ibar][ipar]=TDVBlock[iblock++];
           else      TDVBlock[iblock++]=pdfpara[ich][iv][ilay][ibar][ipar];
///----
         }
        }
      }
    }
  }
//--Dyndoe PDF Par
   for (int ich=0;ich<nPDFCh;ich++){//Charge 
     for(int iv=0;iv<nPDFVel;iv++){//Vel
       for(int ipar=0;ipar<nPDFPar;ipar++){ //Par
        for(int ilay=0;ilay<TOFCSN::SCLRS;ilay++){
          for(int ibar=0;ibar<TOFCSN::NBAR[ilay];ibar++){//N+P
            if(opt==0)pdfpard[ich][iv][ilay][ibar][ipar]=TDVBlock[iblock++];
            else      TDVBlock[iblock++]=pdfpard[ich][iv][ilay][ibar][ipar]; 
         }
        }
      }
    }
  }

  Isload=1;
}

//=======================================================
void   TofPDFPar::LoadTDVPar(){
   return LoadOptPar(0);
}

//=======================================================
int  TofPDFPar::LoadFromFile(const char *fpdf,int ida,int ichl,int ichh,int nv,int npar){//ida Dnode0 Anode1

//--PDFPar
   ifstream vlfile(fpdf,ios::in);
   if(!vlfile){
    cerr <<"<---- Error: missing "<<fpdf<<"--file !!: "<<endl;
    return -1;
   }

   for(int ich=ichl;ich<=ichh;ich++){
     for(int iv=0;iv<nv;iv++){
       for(int ipar=0;ipar<npar;ipar++){
         for(int ilay=0;ilay<TOFCSN::SCLRS;ilay++){
           for(int ibar=0;ibar<TOFCSN::NBAR[ilay];ibar++){
              if(ida==1)vlfile>>pdfpara[ich][iv][ilay][ibar][ipar];
              else      vlfile>>pdfpard[ich][iv][ilay][ibar][ipar];
           }
         }
       }
     }
   }
   vlfile.close();

   return 0;
}

//==========================================================
void TofPDFPar::PrintTDV(){
 cout<<"<<----Print TofPDFAlign"<<endl;
 for(int i=0;i<TDVParN;i++){cout<<TDVBlock[i]<<" ";}
 cout<<'\n';
 cout<<"<<----end of Print TofPDFAlign"<<endl;
}

//==========================================================
const int TofPDFPar::ZPDFgate[ZType]={2,14,1000};

//==========================================================
const int TofPDFPar::PDFCh[nPDFCh]={
 1,2,3,4,5,6,7,8,9,10,11,12,13,14,16,18,20,22,26,
};

//=======================================================
const float TofPDFPar::pdfvel[nPDFCh][nPDFVel]={
//Z=1
 0.975, 0.929, 0.878, 0.826, 0.753, 0.570,//>0.95 0.9~0.95 0.85~0.9 0.8~0.85 0.7~0.8 <0.7
//Z=2
 0.976, 0.928, 0.877, 0.827, 0.756, 0.615,
//Z=3
 0.975, 0.927, 0.876, 0.826, 0.756, 0.630,
//Z=4
 0.975, 0.927, 0.876, 0.826, 0.758, 0.633,
//Z=5
 0.975, 0.927, 0.876, 0.826, 0.756, 0.625,
//Z=6
 0.975, 0.927, 0.876, 0.826, 0.756, 0.625,
//Z=7
 0.973, 0.926, 0.876, 0.826, 0.754, 0.630,
//Z=8
 0.972, 0.926, 0.876, 0.825, 0.753, 0.633,
//Z=9 (One Bin)
  1., 2, 2, 2, 2, 2,
//Z=10
  0.972, 0.926, 0.876, 0.826, 0.730,  2., //>0.95 0.9~0.95 0.85~0.9 0.8~0.85 <0.8
//Z=11
  1,  2, 2, 2, 2, 2,
//Z=12 
  0.972, 0.926, 0.876, 0.826, 0.728, 2.,
//Z=13
  1,  2, 2, 2, 2, 2,
//Z=14
  0.972, 0.926, 0.854, 0.726, 2., 2, //>0.95 0.9~0.95 0.8~0.9 <0.8
//Z=16
  1,  2, 2, 2, 2, 2,
//Z=18
  1,  2, 2, 2, 2, 2,
//Z=20
  1,  2, 2, 2, 2, 2,
//Z=22
  1,  2, 2, 2, 2, 2,
//Z=26
  0.974, 0.927, 0.850, 2., 2, 2., //>0.95 0.9~0.95 <0.9
};

//==========================================================
const float TofPDFPar::DAgate[TOFCSN::SCLRS][TOFCSN::SCMXBR]={
//--Gaus
 36.1924508, 25.,        59.1081215,     16.,        36.2870294, 64.,        36.,       36.3128519,  0., 0.,
 15.3553412, 25.,        25.,            36.4819246, 40.9775920, 27.8336000, 25.,       25.,         0., 0.,
 36.,        25.,        25.,            20.1030702, 25.,        34.3711522, 33.,       30.,36.9542741, 38.2348669,
 36.,        25.,        22.,            25.,        25.,        22.,        25.,       25.,         0., 0.,
};

//==========================================================

