#include "TrdHCharge.h"
#include "TrdHCalib.h"
#include "amsdbc.h"
#include "root.h"
#include "timeid.h"

/*#ifndef _PGTRACK_
#include "commonsi.h"
AMSDATADIR_DEF amsdatadir_;
#endif*/

ClassImp(TrdHChargeR);
TrdHChargeR *TrdHChargeR::head=0;

Double_t pfun(Double_t x, Double_t *par) {
Double_t mpshift  = -0.22278298;
  if(x<par[1]){
    Double_t mean= par[1] - mpshift * par[2];
    return par[0]*TMath::Landau(x,par[1],par[2])*TMath::Exp(par[4]*(x-mean)+par[5]*(x-mean)*(x-mean)+par[6]*(x-mean)*(x-mean)*(x-mean));
  }
  else {
    Double_t mean= par[1] - mpshift * par[3];
    return par[0]*TMath::Landau(x,par[1],par[3])*TMath::Exp(par[4]*(x-mean)+par[5]*(x-mean)*(x-mean)+par[6]*(x-mean)*(x-mean)*(x-mean));
  }
}

double e_par(int p){
  if(p==0)return 0.533941;
  else if(p==1)return 2.04519;
  else if(p==2)return 0.90618;
  else if(p==3)return 0.177358;
  else if(p==4)return 13.3248;
  else if(p==5)return 4.80789;
  else if(p==6)return 0.312408;
  else if(p==7)return -0.0432751;
  else return 0.;
}


// get conv landau fit param depending on trtrack rigidity [GV] 
double p_par(int p){
  if(p==0)return 1.32391;
  else if(p==1)return 2.23676;
  else if(p==2)return 1.02281;
  else if(p==3)return 0.788797;
  else if(p==4)return -0.0594475;
  else if(p==5)return 0.00140946;
  else if(p==6)return -2.07114e-05;
  else return 0.;
}


int TrdHChargeR::GetCharge(TrdHTrackR *tr,float beta,int debug){
  if(debug)printf("Enter TrdHChargeR::GetCharge\n");
  if(pdfs.size()<2)return 1;
  if(beta<0.5)return 2;
  
  float betacorr=1.;
  if(beta<0.95)// beta correction to be done
    {
      betacorr=TrdHCalibR::gethead()->GetBetaCorr(beta);// approx mip beta (betagamma 3.04)
    }
  
  double ampcorr[20];
  for(int i=0;i<20;i++)ampcorr[i]=0.;
  
  for(int s=0;s<2;s++)
    for(int h=0;h<tr->pTrdHSegment(s)->nTrdRawHit();h++){
      TrdRawHitR* hit=tr->pTrdHSegment(s)->pTrdRawHit(h);
      if(hit->Amp<5)continue;
      ampcorr[hit->Layer]+=hit->Amp*betacorr*TrdHCalibR::gethead()->GetGainCorr(hit)*TrdHCalibR::gethead()->GetPathCorr(tr->TubePath(hit));
    }
  
  if(debug)
    for(int i=0;i<20;i++)
      printf(" L %i amp %.2f\n",i,ampcorr[i]);
  
  map<int,double> map_charge_prob;
  map<int,double>::iterator mit;

  int nhit=0;
  for(int chg=-1;chg<10;chg++){
    map<int,TrPdf*>::iterator it=pdfs.find(chg);
    if(it==pdfs.end())continue;

    int pcharge=0;
    if(chg<0)pcharge=-1;
    else pcharge=chg;
    
    for(int i=0;i<20;i++){
      if(ampcorr[i]<5)continue;

      if(chg<0){
	it=pdfs.find(-i-1);
	if(it==pdfs.end()){
	  cerr<<"electron pdf for layer "<<-i-1<<" not found"<<endl; 
	  continue;
	}
      }

      double prob=it->second->Eval(ampcorr[i]);
      if(prob<=0)continue;
      
      mit=map_charge_prob.find(pcharge);
      if(mit==map_charge_prob.end()){
	map_charge_prob.insert(pair<int,double>(pcharge,prob));
	mit=map_charge_prob.find(pcharge);
      }
      
      mit->second*=prob;
      
      if(debug>1)printf(" c %i prob %.2e accum %.2e\n",pcharge,prob,mit->second);
      nhit++;
    }
  }
    

  for(map<int,double>::iterator it=map_charge_prob.begin();it!=map_charge_prob.end();it++){
    it->second=pow((double)it->second,(double)(1./(double)nhit));
    if(debug)printf("normalized prob %i: %.4e\n",it->first,it->second);
  }
  
  double totalprob=0.;
  for(map<int,double>::iterator it=map_charge_prob.begin();it!=map_charge_prob.end();it++)
    totalprob+=it->second;
  
  for(map<int,double>::iterator it=map_charge_prob.begin();it!=map_charge_prob.end();it++)
    it->second/=totalprob;
  
  
  int toReturn=0;double maxprob=0.;
  charge_probabilities.clear();                                              
  for(map<int,double>::iterator it=map_charge_prob.begin();it!=map_charge_prob.end();it++){
    if(it->second>maxprob){
      maxprob=it->second;
      toReturn=it->first;
    }
    charge_probabilities.insert(pair<double,int>(it->second,it->first));
  }
  
  /*
    map<float,int>::const_iterator maxit=--charge_probabilities.end();
    if(charge_probabilities.size()==1)return charge_probabilities.begin()->second;
    
    map<float,int>::const_iterator upit=--charge_probabilities.end();
    map<float,int>::const_iterator lowit=--charge_probabilities.end();
    
    for(map<float,int>::const_iterator it=charge_probabilities.begin();it!=charge_probabilities.end();it++){
    if((maxit->second)-1==it->second)lowit=it;
    if((maxit->second)+1==it->second)upit=it;
    }
    
    float toReturn=0;
    if(lowit!=maxit&&upit!=maxit){
    toReturn= (lowit->first*lowit->second
    +maxit->first*maxit->second
    +upit->first*upit->second) 
    / (lowit->first+maxit->first+upit->first);
    }
    else if(lowit!=maxit){
    toReturn= (lowit->first*lowit->second
    +maxit->first*maxit->second)
    / (lowit->first+maxit->first);
    }
    else if(upit!=maxit){
    toReturn= (maxit->first*maxit->second
    +upit->first*upit->second) 
    / (maxit->first+upit->first);
    }
    if(debug){
    printf("max c %i prob %.2f - low c %i prob %.2f - up c %i prob %.2f\n",maxit->second,maxit->first,lowit->second,lowit->first,upit->second,upit->first);
    }*/

  return toReturn;
}

float TrdHChargeR::GetELikelihoodNEW(int debug){
  if(debug)cout<<"Entering TrdHChargeR::GetELikelihoodNEW"<<endl;

  double elik=0.;
  double sumlik=0.;
  for(map<double,int>::iterator it=charge_probabilities.begin();it!=charge_probabilities.end();it++){
    if(it->second==-1)elik=it->first;
    sumlik+=it->first;
  }
  if(debug)printf("elik %.2e sumlik%.2e - loglik %.2f\n",elik,sumlik,-log(elik/sumlik));
  return -log(elik/sumlik);
}

float TrdHChargeR::GetELikelihood(TrdHTrackR *tr,float beta, int opt,int debug){
  if(debug)printf("Enter TrdHChargeR::GetELikelihood\n");
  double elik=1., plik=1.;
  
  double pars[7];
  for(int i=0;i<7;i++)pars[i]=p_par(i);

  int l=0,n=0;
  for(l=0;l!=20;l++){
    float amp=tr->elayer[l]/adc2kev;
    
    if(amp<=0.)continue;
    n++;
    // multiply electron likelihoods per layer
    elik*=(e_par(0)*TMath::Landau(amp,e_par(1),e_par(2))+
	   e_par(3)*TMath::Landau(amp,e_par(4),e_par(5)))*exp(e_par(6)+e_par(7)*amp);
    
    if(debug)printf(" layer %i amp %.2f elik %.2e accum %.2e ",l,amp,e_par(0)*TMath::Landau(amp,e_par(1),e_par(2))+e_par(3)*TMath::Landau(amp,e_par(4),e_par(5)),elik);
    
    // multiply proton likelihoods per layer
    plik*=pfun(amp,pars);
    
    if(debug)printf(" plik %.2e accum %.2e\n",pfun(amp,pars),plik);
  }
  
  if(n==0||elik<=0.||plik<=0.){
    if(debug)printf("Error in AMSParticle::trd_Hlikelihood(): n %i elik %.2e plik %.2e \n",n,elik,plik);
    return 0.;
  }
  if(debug)printf("before norm: elik %.2e plik %.2e \n",elik,plik);
  
  // normalize probabilities to number of hits
  elik=pow(elik,(double)(1./(double)n));
  plik=pow(plik,(double)(1./(double)n));
  
  float loglik=-log(elik/(elik+plik));
  if(debug)printf("elik %.2f plik %.2f likelihood %.2f\n",elik,plik,loglik);

  if(opt==0)return loglik;
  else if(opt==1)return elik;
  else if(opt==2)return plik; 


  else return 0.; 
}

int TrdHChargeR::GetNCC(TrdHTrackR* tr,int debug){
  int n=0;
  for(int l=0;l!=20;l++){
    float amp=tr->elayer[l]/adc2kev;
    //#ifndef __ROOTSHAREDLIBRARY__
    //    if(amp>TRDRECFFKEY.CCAmpCut)n++;
    //#else
    if(amp>ccampcut)n++;
    //#endif
  }
  return n;
}


bool TrdHChargeR::closeTDV(){
  for(map<string,AMSTimeID*>::iterator it=tdvmap.begin();it!=tdvmap.end();it++){
    delete it->second;
  }
  tdvmap.clear();
  return 1;
}

bool TrdHChargeR::readTDV(unsigned int t, int debug){
  time_t thistime=(time_t)t;

  bool ok=true;
  for(map<string,AMSTimeID*>::iterator it=tdvmap.begin();it!=tdvmap.end();it++)
    if(!it->second->validate(thistime))ok=false;

  if(!FillPDFsFromTDV()) ok=false;
  
  return ok;
}

bool TrdHChargeR::InitTDV(unsigned int bgtime, unsigned int edtime, int type,char * tempdirname){
  time_t begintime = (time_t) bgtime;
  time_t endtime   = (time_t) edtime;
  
  //maxtube*trdconst::maxlad*trdconst::maxlay+maxtube*trdconst::maxlad+maxtube;
  int size=16*18*20+16*18+16;


  AMSTimeID::CType server=AMSTimeID::Standalone;
  tm begin = *gmtime(&begintime);
  tm end   = *gmtime(&endtime);
  
  char tdname[200];
  sprintf(tdname, "%s/DataBase/", tempdirname);
  if(!AMSDBc::amsdatabase)
    AMSDBc::amsdatabase=new char[200];
  
  strcpy(AMSDBc::amsdatabase,tdname);
  
  AMSTimeID *ptdv=new AMSTimeID(AMSID("TRDHPDF",type),
				begin,end,sizeof(charge_hist_array[0])*size,
				(void*)charge_hist_array,server);
  tdvmap.insert(pair<string,AMSTimeID*>("TRDHPDF",ptdv));

  if( ! readTDV(bgtime) ){
    cerr<<"Warning: readTDV inside InitTDV"<<endl;
    return false;
  }

  return true;
}

// write calibration to TDV
int TrdHChargeR::writeTDV(unsigned int bgtime, unsigned int edtime, int debug, char * tempdirname){
  if( !FillTDVFromPDFs() ){
    printf("Warning: FillTDVFromMedian inside writeTDV - low occupancy\n");
    //    return 2;
  }

  time_t begin,end,insert;

  bool ok=true;
  for(map<string,AMSTimeID*>::const_iterator it=tdvmap.begin();it!=tdvmap.end();it++){
    it->second->UpdateMe()=1;
    unsigned int crcold=it->second->getCRC();
    it->second->UpdCRC();
    
    if(crcold!=it->second->getCRC()){
      // valid for 10 days
      time(&insert);
      begin=(time_t)bgtime;
      end=(time_t)edtime;
      it->second->SetTime(insert,begin,end+864000);
      cout <<" Write time:" << endl;
      cout <<" Time Insert "<<ctime(&insert);
      cout <<" Time Begin "<<ctime(&begin);
      cout <<" Time End "<<ctime(&end);
      
      char tdname[200] = "";
      sprintf(tdname, "%s/DataBase/", tempdirname);
      
      if(!it->second->write(tdname)) {
	cerr <<"TrdHChargeR::writeTDV - problem to update tdvc"<<it->first<<endl;
	ok=false;
      }
    }
  }

  return ok;
};

bool TrdHChargeR::FillPDFsFromTDV(){

  int i=0;
  while(i<30000)
    {
      if(charge_hist_array[i]==0)break;
      
      int npnt=(int)charge_hist_array[i++];
      int c=(int)charge_hist_array[i++];
      
      char name[10];
      sprintf(name,"TRDHPDF%i",c);
      
      TrPdf *pdf=new TrPdf(name);

      int n=0;
      bool ok=true;
      while(n++<npnt){
	pdf->AddPoint(charge_hist_array[i++],charge_hist_array[i++]);
      }
      
      if(ok){
	//      const char *tst=name;
	//      TSpline5 spl(tst,gr);
	//      pdfs.insert(pair<int,TSpline5>(c,spl));
	
	//      TrPdf spl(tst,gr);
	pdfs.insert(pair<int,TrPdf*>(c,pdf));
      }
    }
  
  return true;
}

bool TrdHChargeR::FillTDVFromPDFs(){
  map<int,TrPdf*>::iterator it;

  for(int j=0;j<30000;j++)charge_hist_array[j]=0.;

  int i=0;
  for(int c=0;c<10;c++){
    //  map<int,TSpline5>::const_iterator it;
    it=pdfs.find(c);
    if(it==pdfs.end())continue;
    
    //  charge_hist_array[c][0]=it->second.GetNp();
    //  for(int p=0;p<it->second.GetNp();p++){

    charge_hist_array[i++]=it->second->GetN();

    if(c==0)charge_hist_array[i++]=-1.;
    else charge_hist_array[i++]=c;
    
    for(int p=0;p<it->second->GetN();p++){
      //    double x,y;
      //    it->second.GetKnot(p,x,y);
      charge_hist_array[i++]=it->second->GetX(p);
      charge_hist_array[i++]=it->second->GetY(p);
    }
  }

  return true;
}


int TrdHChargeR::AddEvent(TrdHTrackR *track,float beta,int charge, int debug){
  if(debug)cout<<"Entering TrdHChargeR::AddEvent"<<endl;
  if(beta<0.5)return 1;
  if(!track)return 2;


  float betacorr=1.;
  if(beta<0.95)
    {
      betacorr=TrdHCalibR::gethead()->GetBetaCorr(beta);
    }
  
  float ampcorr[20];
  for(int i=0;i<20;i++)ampcorr[i]=0.;
  
  for(int s=0;s<2;s++)
    for(int h=0;h<track->pTrdHSegment(s)->nTrdRawHit();h++){
      TrdRawHitR* hit=track->pTrdHSegment(s)->pTrdRawHit(h);
      if(hit->Amp<5)continue;
      ampcorr[hit->Layer]+=hit->Amp*betacorr*TrdHCalibR::gethead()->GetGainCorr(hit)*TrdHCalibR::gethead()->GetPathCorr(track->TubePath(hit));
    }


  if(debug)cout<<"beta "<<beta<<" charge "<<charge<<endl;
  
  if(charge>0){
    map<int,TH1F>::iterator it=spectra.find(charge);
    if(it==spectra.end()){
      char name[20] ;
      sprintf(name,"h_spectrum_%i",charge);
      printf("TrdHChargeR::AddEvent - adding %s\n",name);

      TH1F h(name,"",100,0,4000);
      spectra.insert(pair<int,TH1F>(charge,h));
      it=spectra.find(charge);
    }
    
    
    for(int i=0;i<20;i++)
      it->second.Fill(ampcorr[i]);
  }
  else{
    // storing electrons per layer as negative charges
    // numbering layer L1-L20 to avoid confusion
    for(int l=1;l<21;l++){
      map<int,TH1F>::iterator it=spectra.find(-l);
      if(it==spectra.end()){
	char name[20] ;
	sprintf(name,"h_spectrum_el_L%i",l);
	printf("TrdHChargeR::AddEvent - adding %s\n",name);
	
	TH1F h(name,"",400,0,4000);
	spectra.insert(pair<int,TH1F>(-l,h));
	it=spectra.find(-l);
      }
      
      it->second.Fill(ampcorr[l-1]);
    }
  }

  return 0;
}

int TrdHChargeR::CreatePDFs(int debug){
  if(debug)cout<<"Entering TrdHChargeR::CreatePDFs - spectra size "<<spectra.size()<<endl;
  pdfs.clear();
  char name[20];
  
  for(map<int,TH1F>::iterator it=spectra.begin();it!=spectra.end();it++){
    if(debug)printf("charge %i\n",it->first);
    if(it->first<0)
      sprintf(name,"TRDHPDF_EL_L%i",-1-it->first);
    else
      sprintf(name,"TRDHPDF%i",it->first);
    
    if(debug)printf("name %s\n",name);

    TrPdf *pdf=new TrPdf(name,&it->second,false,true);

    pdfs.insert(pair<int,TrPdf*>(it->first,pdf));
  }

  return 0;
}

