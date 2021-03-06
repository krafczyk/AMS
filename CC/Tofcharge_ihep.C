//  $Id$

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

float TofChargePar::GetQ(int pmtype){
  
  if     (pmtype==0)return QLD>0?sqrt(QLD):QLD;
  else if(pmtype==1)return QLA>0?sqrt(QLA):QLA;
  else if(pmtype==12||(pmtype==2&&DefaultQDAOpt==12)){//Then try default
    number QL; 
    TofPDFH::ChooseDA(Layer,Bar,6.,number(QLA),number(QLD),number(QLARaw),number(QLDRaw),QL);//Choose
    return QL>0?sqrt(QL):float(QL);
  }
  else {
      if(QLDRaw<0&&QLARaw<0)return -1;
      TofRecPar::IdCovert(Layer,Bar);
      double wd=TofRecH::GetWeightDA(TofRecPar::Idsoft,0,QLDRaw);
      double wa=TofRecH::GetWeightDA(TofRecPar::Idsoft,1,QLARaw);
      double qd=QLD>0?sqrt(QLD):QLD;
      double qa=QLA>0?sqrt(QLA):QLA;
      float qda=(wa*qa+wd*qd)/(wa+wd);
      return qda;
  }
}

///=======================================================
float TofChargePar::GetProbZ(float Z){

   float probz=TofPDFH::GetProbZ(Layer,Bar,number(Z),number(QLA), number(QLD),number(QLARaw),number(QLDRaw),number(Beta));//float double
   return probz;   
}

///=======================================================
void   TofChargePar::FillProbZ(int ZL,int ZH){

   for(int Z=ZL;Z<=ZH;Z++){
      ProbZ.push_back(make_pair(Z,GetProbZ(float(Z))));
   }
}

// **************************************************************
// Tof TofLikelihoodPar for Each Z
// **************************************************************
ClassImp(TofLikelihoodPar)

bool TofLikelihoodPar::IsUsedL (int iLayer){

  if(LPattern/int(pow(10., 3-iLayer))%10==1)return 1;
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

int TofChargeHR::DefaultQOpt=(TofRecH::kThetaCor|TofRecH::kBirkCor|TofRecH::kReAttCor|TofRecH::kBetaCor|TofRecH::kQ2Q);
int TofChargeHR::DefaultQOptIon=(TofRecH::kThetaCor|TofRecH::kBirkCor|TofRecH::kReAttCor|TofRecH::kBetaCor|TofRecH::kRigidityCor|TofRecH::kQ2Q);
int TofChargeHR::DefaultQOptIonW=(TofRecH::kThetaCor|TofRecH::kBirkCor|TofRecH::kReAttCor|TofRecH::kDAWeight|TofRecH::kBetaCor|TofRecH::kRigidityCor|TofRecH::kQ2Q);
///=======================================================

void TofChargeHR::Init(){
   cpar.clear();
   like.clear();
   likeQ.clear();
   Q.clear();
   RQ.clear();
   fTrTrack=-1;
   Rigidity=0;
}

///=======================================================
TofChargeHR::TofChargeHR(BetaHR *betah){

    TofPDFH::ReBuild(betah,(*this));
}

///=======================================================
void TofChargeHR::UpdateZ(int pattern,int opt){

/// First Check Whether Pattern exist
  bool upi=0,upf=0;
  if(opt%10==1){ //Integer
    map<int, vector<TofLikelihoodPar> >::iterator it;
    it=like.find(pattern);
    if(it==like.end()){upi=1;}//Not Found -- Request To Build Integer
  }
  if(opt/10%10==1){ //Float Likelihood
     map<int, float >::iterator it;
     it=likeQ.find(pattern);
     if(it==likeQ.end()){upf=1;}//Not Found --Request To Build Float
  }

//---Both Not Need To Update
  if(!upi&&!upf)return;

/// Charge Measument Par Of Select Good Layer 
  vector<TofChargePar> cparu;

///--Select Matched-TOF Layer
  TofPDFH::SelectM(pattern,cpar,fTrTrack,cparu);

  number MeanQ,RMSQ;
///--LikeLihood -Calculation
  if(upi){
    vector<TofLikelihoodPar> plike;
    TofPDFH::LikelihoodCal(cparu,plike); 
    like.insert(pair<int,vector<TofLikelihoodPar> >(pattern,plike));
    TofPDFH::GetLikelihoodQ(cparu,MeanQ,RMSQ,0);
    Q.insert(pair<int,float>(pattern,float(MeanQ)));
    RQ.insert(pair<int,float>(pattern,float(RMSQ)));
  }
  if(upf){
//---LikeLihood Q
    float plikeQ=TofPDFH::GetLikelihoodQ(cparu,MeanQ,RMSQ,1);
    likeQ.insert(pair<int,float >(pattern,plikeQ));
  }
 
}

///=======================================================
bool  TofChargeHR::TestExistHL(int ilay){
   
   return (GetTofChargePar(ilay))?1:0;
}

///=======================================================
TofChargePar  TofChargeHR::gTofChargePar(int ilay){
    
   for(unsigned int im=0;im<cpar.size();im++){
     if(cpar.at(im).Layer==ilay){return cpar.at(im);}
   }
   return TofChargePar(); 
}

///=======================================================
TofChargePar*  TofChargeHR::GetTofChargePar(int ilay){

    TofChargePar *chpar=0;
    for(unsigned int im=0;im<cpar.size();im++){
       if(cpar.at(im).Layer==ilay){return &(cpar.at(im));}
    }
    return chpar;
}

///=======================================================
bool  TofChargeHR::IsGoodQPathL(int ilay){
 
   TofChargePar* chpar=GetTofChargePar(ilay);
   if(chpar){return chpar->IsGoodPath;}
   else     {return 0;} 
}

///=======================================================
float TofChargeHR::GetQL(int ilay,int pmtype){

   TofChargePar* chpar=GetTofChargePar(ilay); 
   if(chpar){return chpar->GetQ(pmtype);}
   else     {return 0;}
}

///=======================================================
int TofChargeHR::GetNL(){

   return cpar.size();
}


///=======================================================
int TofChargeHR::GetNZ(int pattern){
  
/// First Update and Check
  UpdateZ(pattern);
/// Number of Most ProbZ
  return like[pattern].size();  
}

///=======================================================
int   TofChargeHR::GetZI(int Z,int pattern){

// First Update and Check
  UpdateZ(pattern);

/// Try To Find
  int indexz=-1;
  for(unsigned int iz=0;iz<like[pattern].size();iz++){
    if(like[pattern].at(iz).Z==Z){indexz=iz;break;}
  }
  return indexz;
}

///=======================================================
int TofChargeHR::GetZ(int &nlay,float &Prob,int IZ,int pattern){
 
/// First Update and Check
  UpdateZ(pattern);

  if(IZ>=int(like[pattern].size())||IZ<0)return -1;//Not Found

//---Get Z Par
  Prob=like[pattern].at(IZ).Prob;
  nlay=like[pattern].at(IZ).GetnLayer();
  return like[pattern].at(IZ).Z; 
}

///=======================================================
TofLikelihoodPar TofChargeHR::gTofLikelihoodPar(int IZ, int pattern){
  
/// First Update and Check
  UpdateZ(pattern);

  if(IZ>=int(like[pattern].size())||IZ<0)return TofLikelihoodPar();

  return like[pattern].at(IZ);
}

///=======================================================
TofLikelihoodPar * TofChargeHR::GetTofLikelihoodPar(int IZ, int pattern){
   
/// First Update and Check
  UpdateZ(pattern);

  TofLikelihoodPar *likepar=0;
  if(IZ>=int(like[pattern].size())||IZ<0)return likepar;
//---
  return &(like[pattern].at(IZ));
}


///=======================================================
float TofChargeHR::GetProbZ(int Z,int pattern){

/// Find Index
  int indexz=GetZI(Z,pattern);
 
/// Try To Find
  if(indexz<0){return 0;}
  else        {return like[pattern].at(indexz).Prob;}
}

///=======================================================
float  TofChargeHR::GetLkh(int Z,int &nlay,int pattern){

// First Update and Check
   UpdateZ(pattern);   
   nlay=like[pattern].at(0).GetnLayer();
/// Find Index
  int indexz=GetZI(Z,pattern);

  if(indexz<0){return  nlay*log(TofPDFPar::ProbLimit);}
  else        {return  like[pattern].at(indexz).Likelihood;}  
}

//=======================================================
float TofChargeHR::GetQ(int &nlay,float &qrms,int pattern){

/// First Update and Check
   UpdateZ(pattern);

/// Find
   nlay=like[pattern].at(0).GetnLayer();
   qrms=RQ[pattern];
   return Q[pattern]; 
}

//=======================================================
float TofChargeHR::GetLikeQ(int &nlay,int pattern){

/// First Update and Check For Both Integer and Float
  UpdateZ(pattern,11);

//---Get LikelihoodQ Par
  nlay=like[pattern].at(0).GetnLayer();
  return likeQ[pattern];
}

//=======================================================
int  TofChargeHR::ReFit(float fbeta,int opt,float frig){

  int kRigidity=(opt&TofRecH::kRigidityCor);
  double rig=0;
  if(fTrTrack>=0&&fbeta==0)rig=Rigidity;
  if(frig!=0)rig=frig;  
//----ReFit by new fbeta
   number QLA1,QLD1;
   for(unsigned int im=0;im<cpar.size();im++){
     number nbeta=(fbeta==0)?cpar.at(im).Beta:fbeta;
     TofRecPar::IdCovert(cpar.at(im).Layer,cpar.at(im).Bar);
     QLA1= TofRecH::GetQSignal(TofRecPar::Idsoft,1,(TofRecH::kBetaCor|kRigidity),cpar.at(im).QLARaw,0,1,nbeta,rig);
     QLD1= TofRecH::GetQSignal(TofRecPar::Idsoft,0,(TofRecH::kBetaCor|kRigidity),cpar.at(im).QLDRaw,0,1,nbeta,rig);
     cpar.at(im).QLA=QLA1; cpar.at(im).QLD=QLD1;
     cpar.at(im).Beta=nbeta;
     if(opt&TofRecH::kDAWeight)cpar.at(im).DefaultQDAOpt=22;
     else                      cpar.at(im).DefaultQDAOpt=12;
   }
   TofPDFH::FillProbV(cpar);

//---Clean Raw
   like.clear();
   likeQ.clear();
   Q.clear();
   RQ.clear();

//---Update Again
  UpdateZ(); 

  return 0;
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
   float Beta=betah->GetBeta(); 
   int  fTrTrack=betah->iTrTrack();
   float Rigidity=0;
#ifdef _PGTRACK_
   if(fTrTrack>=0)Rigidity=betah->pTrTrack()->GetRigidity();
#endif
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
//#endif
    }
  }

//--- Not Exist Layer Exist
  if(cpar.size()==0){tofch.UpdateZ();return -1;}

///--Fill Prob
  FillProbV(cpar);

  
//---Copy to TofChargeHR
  tofch.cpar=cpar;
  tofch.fTrTrack=fTrTrack;
  tofch.Rigidity=Rigidity;
  tofch.UpdateZ(); 

  return 0; 
}

//=======================================================
int TofPDFH::FillProbV(vector<TofChargePar> &cpar){

//---Find Limit and Clean
   int  MinZ=2*TofPDFPar::ZHLim,MaxZ=0,MinIZ=0,MaxIZ=0;
   float LProb=FLT_MAX,HProb=FLT_MAX;
   for(unsigned int im=0;im<cpar.size();im++){
//----Clear Prob
       cpar.at(im).ProbZ.clear(); 
//----GetQ
       float QL=cpar.at(im).GetQ();
//----Limit
       int QZ=int(QL+0.5)>=1?int(QL+0.5):1;
       if (QZ<MinZ||(QZ==MinZ&&cpar.at(im).GetProbZ(float(MinZ))<LProb)){
           MinZ=QZ;MinIZ=im; LProb=cpar.at(im).GetProbZ(float(MinZ));
       }
       if (QZ>MaxZ||(QZ==MaxZ&&cpar.at(im).GetProbZ(float(MaxZ))<HProb)){
           MaxZ=QZ;MaxIZ=im; HProb=cpar.at(im).GetProbZ(float(MaxZ));
        }
//#endif
   }


  //--- Most Prob Z push_back Prob
  const int ZEXTM=4;//MAX-STEP
  for(int Z=MinZ-1;Z>=1;Z--){
     if(Z==1){MinZ=1;break;}
     float ProbZ=cpar.at(MinIZ).GetProbZ(float(Z));
     if(ProbZ<=TofPDFPar::ProbLimit||MinZ-Z>=ZEXTM){MinZ=Z;break;}
  }
  for(int Z=MaxZ+1;Z<TofPDFPar::ZHLim;Z++){
     if(Z==TofPDFPar::ZHLim-1){MaxZ=TofPDFPar::ZHLim-1;break;}
     float ProbZ=cpar.at(MaxIZ).GetProbZ(float(Z));
     if(ProbZ<=TofPDFPar::ProbLimit||Z-MaxZ>=ZEXTM){MaxZ=Z;break;}
  }


//--Fill Prob for Z
  for(unsigned int im=0;im<cpar.size();im++){
     cpar.at(im).FillProbZ(MinZ,MaxZ);
  }

  return 0;

}

//=======================================================
void TofPDFH::LikelihoodCal(const vector<TofChargePar> &cpars,vector<TofLikelihoodPar> &like){

//---Cal Likehood
    like.clear();

/// Likelihood Use Layer Pattern
    int   lpattern=0;
    for(unsigned int im=0;im<cpars.size();im++){lpattern+=int(pow(10.,3-cpars.at(im).Layer));}

/// Protection
    if(cpars.size()==0){like.push_back(TofLikelihoodPar(lpattern,-1,0));return;}

/// For Each Z Calculate 
    float slike=0;int nhit=cpars.size();
    for(unsigned int iz=0;iz<cpars.at(0).ProbZ.size();iz++){
       float plike=GetLikelihood(iz,cpars);
       int   zvar=cpars.at(0).ProbZ.at(iz).first;
       like.push_back(TofLikelihoodPar(lpattern,zvar,plike));
       slike+=exp(plike/nhit);
    }

/// Sort Form MaxLikelihood To MinLikelihood
    sort(like.begin(),like.end());

/// Calculate Prob
    for(unsigned int ilk=0;ilk<like.size();ilk++){
       like.at(ilk).Prob=exp(like.at(ilk).Likelihood/nhit)/slike;
   }

}

//=======================================================
number TofPDFH::GetLikelihood(int IZ,const vector<TofChargePar> &cpars){

   number likelihood=0;
   for(unsigned int im=0;im<cpars.size();im++){
       number probv=cpars.at(im).ProbZ.at(IZ).second;
       likelihood+=log(probv);
   }
   return likelihood;
}


//=======================================================
number TofPDFH::GetLikelihoodQ(vector<TofChargePar> &cpars,number &MeanQ,number &RMSQ,int opt){

//---GetLimit And Imformation
    number DZ=0,LZ=2*TofPDFPar::ZHLim,HZ=0,QL;
    int nm=0;
    RMSQ=0;
    for(unsigned int im=0;im<cpars.size();im++){
       QL=cpars.at(im).GetQ();
       if(QL<=0){cout<<"<<----Error Q"<<endl;continue;}
       if(QL<LZ)LZ=QL;
       if(QL>HZ)HZ=QL;
       DZ+=QL;
       RMSQ+=QL*QL;
       nm++;
    }
    DZ/=nm;
    MeanQ=(nm==0)?-1:DZ;
    RMSQ=(nm==0)?-1:sqrt(RMSQ/nm-MeanQ*MeanQ);
    if(nm<=1){return MeanQ;}

///---Only Want to MeanQ
    if(opt==0){return MeanQ;}

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
   for(unsigned int im=0;im<chargepar.size();im++){
       number probv=chargepar.at(im).GetProbZ(par[0]);
       likelihood+=log(probv);
   }
   fval=-likelihood;
}



//=======================================================
int TofPDFH::SelectM(int pattern,const vector<TofChargePar> &cpar,int fTrTrack,vector<TofChargePar> &cpars){
   
//---Copy     
   cpars=cpar;

///---Erase Nagtive
    for(unsigned int im=0;im<cpars.size();im++){
      if(cpars.at(im).GetQ()<=0){
         cpars.erase(cpars.begin()+im);//0 Erase
         im--;
      }
    }


//--Use Select Pattern
    if(pattern>0){
      for(unsigned int im=0;im<cpars.size();im++){
         int ilay=cpars.at(im).Layer;
         if(pattern/int(pow(10.,3-ilay))%10==1)continue;//Select Countinue;
         cpars.erase(cpars.begin()+im);//0 Erase
         im--;
      }
    }

//---Pool PathLength ReMove // -2
    if(pattern<0&&(pattern>=-10)&&fTrTrack>=0&&cpars.size()>2){
       for(unsigned int im=0;im<cpars.size();im++){
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
       for(unsigned int iz=0;iz<cpars.at(0).ProbZ.size();iz++){//for All Z Find Max-Windows Layer
//---
         vector<int>WNL;vector<int>ONL;//Z in Windows
         for(unsigned int im=0;im<cpars.size();im++){
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
       for(unsigned int im=0;im<cpars.size();im++){
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
  if(QL<=0)return TofPDFPar::ProbLimit;

  number probv=0;
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
     if(QLDRaw>TofPDFPar::DAgate[ilay][ibar]&&(QLARaw>=TofPDFPar::DAgate[ilay][ibar]/1.5||QLARaw<=0)){isanode=0;}
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
 number wwl=0,wwh=0;
 if(ivl>=0){ //Interpolation
   wwh=fabs(betah-TofPDFPar::pdfvel[ZI][ivl]);
   wwl=fabs(TofPDFPar::pdfvel[ZI][ivh]-betah);
 }

//--Prepare PDF Function
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

//=======================================================
