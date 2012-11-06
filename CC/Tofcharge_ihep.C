//  $Id: Tofcharge_ihep.C,v 1.1 2012/11/06 20:44:13 qyan Exp $

// ------------------------------------------------------------
//      AMS TOF Charge and PDF Likelihood (BetaH Version)
// ------------------------------------------------------------
//      History
//        Created:       2012-Oct-01  Q.Yan (qyan@cern.ch) + V.Choutko
// -----------------------------------------------------------
#include "TMinuit.h"
#include "Tofrec02_ihep.h"
#include "Tofcharge_ihep.h"

// **************************************************************
// TofChargePar for Each Layer Q
// **************************************************************
ClassImp(TofChargePar)

float TofChargePar::GetQ(){
  number QL; 
  TofPDFH::ChooseDA(Layer,Bar,100,number(QLARaw),number(QLDRaw),QL); 
  return float(QL);
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
  TofPDFH::SelectM(pattern,cpar,fTrTrack,cparu);

///--LikeLihood -Calculation
  vector<TofLikelihoodPar> plike;
  TofPDFH::LikelihoodCal(cparu,plike); 
  like.insert(pair<int,vector<TofLikelihoodPar> >(pattern,plike));

//---LikeLihood Q
  float plikeQ=TofPDFH::GetLikelihoodQ(cparu);
  likeQ.insert(pair<int,float >(pattern,plikeQ));
  
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

//--Clear
    tofch.Init();

//--Clear   
   vector<TofChargePar> cpar;

//---push_back of cpar
   int  MinZ=2*TofPDFPar::ZHLim,MaxZ=1,MinIZ=-1,MaxIZ=-1;
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
//----Limmit
       cpar.push_back(cparl);
       int QZ=int(QL+0.5)>=1?int(QL+0.5):1;
       if     (QZ<MinZ){MinZ=QZ;MinIZ=cpar.size()-1;}
       else if(QZ>MaxZ){MaxZ=QZ;MaxIZ=cpar.size()-1;}
//#endif
    }
  }

//--- Not Exist Layer Exist
  if(cpar.size()==0)return -1;

//--- Most Prob Z push_back Prob
  float LProb=cpar.at(MinIZ).GetProbZ(float(MinZ));
  for(int Z=MinZ-1;Z>=1;Z--){
     float ProbZ=cpar.at(MinIZ).GetProbZ(float(Z));
     if(ProbZ<=TofPDFPar::ProbLimit||ProbZ<=LProb*0.01){MinZ=Z;break;}
   }

  
 float HProb=cpar.at(MaxIZ).GetProbZ(float(MaxZ));
 for(int Z=MaxZ+1;Z<TofPDFPar::ZHLim;Z++){
    float ProbZ=cpar.at(MaxIZ).GetProbZ(float(Z));
    if(ProbZ<=TofPDFPar::ProbLimit||ProbZ<=HProb*0.01){MaxZ=Z;break;}
 }

 
//--Fill Prob for Z
  for(int im=0;im<cpar.size();im++){
     cpar.at(im).FillProbZ(MinZ,MaxZ);
  }
  
//---Copy to TofChargeHR
  tofch.cpar=cpar;
  tofch.UpdateZ(); 
  tofch.fTrTrack=fTrTrack; 

  return 0; 
}

//=======================================================
void TofPDFH::LikelihoodCal(vector<TofChargePar> cpars,vector<TofLikelihoodPar> &like){

//---Cal Likehood
    like.clear();
    float slike=0;
    int   lpattern=0;
    for(int im=0;im<cpars.size();im++){lpattern+=int(pow(10.,3-cpars.at(im).Layer));}
    for(int iz=0;iz<cpars.at(0).ProbZ.size();iz++){
       float plike=GetLikelihood(iz,cpars);
       int   zvar=cpars.at(0).ProbZ.at(iz).first;
       like.push_back(TofLikelihoodPar(lpattern,zvar,plike));
       slike+=exp(plike);
    }
    sort(like.begin(),like.end());
    for(int ilk=0;ilk<like.size();ilk++){
       like.at(ilk).Prob=exp(like.at(ilk).Likelihood)/slike;
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
    number DZ=0,LZ=2*TofPDFPar::ZHLim,HZ=0;
    
    for(int im=0;im<cpars.size();im++){
       if     (cpars.at(im).GetQ()<LZ)LZ =cpars.at(im).GetQ();
       else if(cpars.at(im).GetQ()>HZ)HZ= cpars.at(im).GetQ();
       DZ+=cpars.at(im).GetQ();
    }
    DZ/=cpars.size();

    chargepar=cpars;

///---
    TVirtualFitter::SetDefaultFitter("Minuit");
    TVirtualFitter * minuit = TVirtualFitter::Fitter(0,1);
    minuit->SetParameter(0,"TOF_LikeQ",DZ,0.1,LZ/1.2,HZ*1.2);//--Like Q
    minuit->SetFCN(GetLikelihoodF);

//-----begin    
   number arglist[100];
   arglist[0] = 0;
   // set print level
   minuit->ExecuteCommand("SET PRINT",arglist,2);

   // minimize
   arglist[0] = 100; // number of function calls
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

//--Use Select Pattern
    if(pattern>0){
      for(int im=0;im<cpars.size();im++){
         int ilay=cpar.at(im).Layer;
         if(pattern/int(pow(10.,3-ilay))%10==0)cpars.erase(cpars.begin()+im);
         im--;
      }
    }

//---Pool PathLength ReMove // -2
    if(pattern<0&&(pattern>=-10)&&fTrTrack>=0&&cpars.size()>2){
       for(int im=0;im<cpars.size();im++){
         if(!cpars.at(im).IsGoodPath)cpars.erase(cpars.begin()+im);
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

    return cpars.size();
}

//=======================================================
number TofPDFH::GetProbZ(int ilay,int ibar,number Z,number QLA,number QLD,number QLARaw,number QLDRaw,number betah){//before betacor

//--Protection
  if(Z<=0)return TofPDFPar::ProbLimit;
//--No Signal or OverFlow
  else if((QLA<=0)&&(QLD<=0))return TofPDFPar::ProbLimit;

//---Choose Dynode Or Anode
  number QL;
  int isanode=ChooseDA(ilay,ibar,Z,QLARaw,QLDRaw,QL);

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
int TofPDFH::ChooseDA(int ilay,int ibar,int Z, number QLARaw,number QLDRaw,number &QL){

//---Choose DA
  int isanode=1;//Default Anode 
  if(Z>2){//Dynode Better Range
     if(QLDRaw>TofPDFPar::DAgate[ilay][ibar]){
      isanode=0;      
    }
  }

//--GetQ Var
  QL=(isanode==1)?QLARaw:QLDRaw;
  
  return isanode;
}

//=======================================================
number TofPDFH::GetProbZI(int ilay,int ibar,int ZI,number QL,number betah,int isanode){//Index For BetaH
  
  betah=fabs(betah);
  
//-Search Beta Bin
  int ivl=-1, ivh=-1;
  if     (betah<=TofPDFPar::pdfvel[ZI][0]){ivh=0;}
  else {
     int iv=1;
     for(int iv=1;iv<TofPDFPar::nPDFVel;iv++){//Find Middle
       if(TofPDFPar::pdfvel[ZI][iv-1]>=TofPDFPar::pdfvel[ZI][iv]||iv==TofPDFPar::nPDFVel-1){//Found Decrease Signal or Last One, Stop
         ivl=iv-1;break;
       }
       else if((betah>TofPDFPar::pdfvel[ZI][iv-1])&&(betah<TofPDFPar::pdfvel[ZI][iv])){ivl=iv-1;ivh=iv;break;}//Has Found, Break
    }
 }

  if(ivl<0&&ivh<0){cerr<<"<<----TofPDF: Error Find Beta Bin"<<endl;return 0;}
//--- 
 number wwl,wwh;
 if(ivl>=0&&ivh>=0){ //Interpolation
   wwl=fabs(TofPDFPar::pdfvel[ZI][ivh]-betah);
   wwh=fabs(betah-TofPDFPar::pdfvel[ZI][ivl]);  
 }

//--Prepare PDF Function
 typedef Double_t (*pdffun)(Double_t *,Double_t *);
  pdffun  fun;
  if(TofPDFPar::PDFCh[ZI]<=2){
    fun=TofPDFH::PDFPrHe;
 }
 else {
    fun=TofPDFH::PDFBZ;
 }

//--Prepare and Choose PDF-Par  From Anode Or Dynode
  TofPDFPar *PPar=TofPDFPar::GetHead();
  Double_t *pdfpar[2];//Low+Hihg
  if(ivl>=0)pdfpar[0]=(isanode>0)?PPar->pdfpara[ZI][ivl][ilay][ibar]:PPar->pdfpard[ZI][ivl][ilay][ibar];
  if(ivh>=0)pdfpar[1]=(isanode>0)?PPar->pdfpara[ZI][ivh][ilay][ibar]:PPar->pdfpard[ZI][ivh][ilay][ibar];

//---Get Prob   
   number probv;
   if     (ivh<0)probv= (*fun)(&QL, pdfpar[0]);
   else if(ivl<0)probv= (*fun)(&QL, pdfpar[1]);
   else          probv=((*fun)(&QL, pdfpar[0])*wwl+(*fun)(&QL, pdfpar[1])*wwh)/(wwl+wwh);

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
  TDVName="TofPDFPar";
  TDVParN=0;
//  TDVParN+=nPDFCh*nPDFVel;
  TDVParN+=2*nPDFCh*nPDFVel*5*TOFCSN::NBARN;//Anode+Dynode PDF Par
};

//=======================================================
void   TofPDFPar::LoadTDVPar(){

   int iblock=0;
//--Anode PDF Par
   for (int ich=0;ich<nPDFCh;ich++){//Charge 
     for(int iv=0;iv<nPDFVel;iv++){//Vel
       for(int ipar=0;ipar<5;ipar){ //Par
        for(int ilay=0;ilay<TOFCSN::SCLRS;ilay++){
          for(int ibar=0;ibar<TOFCSN::NBAR[ilay];ibar++){//N+P
           int id=ilay*1000+ibar*100;
           pdfpara[ich][iv][ilay][ibar][ipar]=TDVBlock[iblock++];;
         }
        }
      }
    }
  }
//--Dyndoe PDF Par
   for (int ich=0;ich<nPDFCh;ich++){//Charge 
     for(int iv=0;iv<nPDFVel;iv++){//Vel
       for(int ipar=0;ipar<5;ipar){ //Par
        for(int ilay=0;ilay<TOFCSN::SCLRS;ilay++){
          for(int ibar=0;ibar<TOFCSN::NBAR[ilay];ibar++){//N+P
           int id=ilay*1000+ibar*100;
           pdfpard[ich][ipar][ilay][ibar][ipar]=TDVBlock[iblock++];;
         }
        }
      }
    }
  }

  Isload=1;
}

//=======================================================
int  TofPDFPar::LoadFromFile(char *fpdfa,char *fpdfd){

  int ib=0;
//--Anode PDFPar
   ifstream vlfile(fpdfa,ios::in);
   if(!vlfile){
    cerr <<"<---- Error: missing "<<fpdfa<<"--file !!: "<<endl;
    return -1;
   }
   int npar=nPDFCh*nPDFVel*5*TOFCSN::NBARN;
   for(int i=0;i<npar;i++){
     vlfile>>TDVBlock[ib++];
   }
   vlfile.close();

//---Dynode PDFPar 
   ifstream vlfile1(fpdfd,ios::in);
   if(!vlfile1){
    cerr <<"<---- Error: missing "<<fpdfd<<"--file !!: "<<endl;
    return -1;
   }
   for(int i=0;i<npar;i++){
     vlfile1>>TDVBlock[ib++];
   }
   vlfile1.close();
//---- 

   LoadTDVPar();
   return 0;
}

//==========================================================
void TofPDFPar::PrintTDV(){
 cout<<"<<----Print TofTAlignPar"<<endl;
 for(int i=0;i<TDVParN;i++){cout<<TDVBlock[i]<<" ";}
 cout<<'\n';
 cout<<"<<----end of Print TofTAlignPar"<<endl;
}

//==========================================================
const int TofPDFPar::PDFCh[nPDFCh]={
 1,2,3,4,5,6,7,8, 
};

//=======================================================
const float TofPDFPar::pdfvel[nPDFCh][nPDFVel]={
//->0.95 0.9~0.95 0.85~0.9 0.8~0.85 0.7~0.8 <0.7
//Z=1
 0.975, 0.927, 0.876, 0.826, 0.756, 0.630,//*
//Z=2
 0.975, 0.927, 0.876, 0.826, 0.758, 0.633,//*
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
//Z=9
// 1.,   0,  0, 0, 0, 0, 0, 0, 0,
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

