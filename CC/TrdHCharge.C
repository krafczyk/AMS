#include "TrdHCharge.h"
#include "TrPdf.h"
#include "amsdbc.h"
#include "root.h"
#include "timeid.h"

ClassImp(TrdHChargeR);
TrdHChargeR *TrdHChargeR::head=0;

float TrdHChargeR::charge_hist_array[10][1000];
float TrdHChargeR::electron_hist_array[20000];

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


int TrdHChargeR::GetCharge(TrdHTrackR* track,float rig, int debug){
  if(debug)printf("Enter TrdHChargeR::GetCharge\n");
  if(pdfs.size()<2)return -1;
  
  map<int,double> map_charge_prob;
  map<int,double>::iterator mit;
  
  for(int pid=0;pid<7;pid++){
    float bg=0.;
    if(pid==1)bg=fabs(rig)/0.938;
    if(pid==2)bg=fabs(rig)*2./3.727;
    
    int c=pid;
    if(rig!=0.&&c){
      c+=10;
      track->UpdateLayerEdep(7,bg,pid);
    }
    else{
      track->UpdateLayerEdep(3,bg,pid);
    }
    
    map<int,TrPdf*>::iterator it=pdfs.find(c);
    if(it==pdfs.end()&&(c>0||!use_single_layer_pdfs))
      continue;
    
    int nhit=0;
    for(int i=0;i<20;i++){
      if(track->elayer[i]<15)continue;
      
      if(debug&&c==0&&i==0){
	if(use_single_layer_pdfs)cout<<"Using single layer electron pdfs"<<endl;
	else cout<<"Using all layer electron pdfs"<<endl;
      }
      if(c==0&&use_single_layer_pdfs){
	it=pdfs.find(-i-1);
	if(it==pdfs.end()){
	  cerr<<"electron pdf for layer "<<-i-1<<" not found"<<endl; 
	  continue;
	}
      }

      double prob=it->second->Eval(track->elayer[i]);
      if(prob<=1.e-8)prob=1.e-8;

      mit=map_charge_prob.find(c);
      if(mit==map_charge_prob.end()){
	map_charge_prob.insert(pair<int,double>(c,prob));
	mit=map_charge_prob.find(c);
      }
      else
	mit->second*=prob;
      nhit++;
      
      if(debug>1)printf(" c %i amp %.2f prob %.2e accum %.2e\n",c,track->elayer[i],prob,mit->second);
    }
    mit->second=pow((double)mit->second,(1./(double)nhit));
    if(debug)printf("normalized prob %i: %.4e\n",it->first,it->second);
  }
  
  double totalprob=0.;
  for(map<int,double>::iterator it=map_charge_prob.begin();it!=map_charge_prob.end();it++)
    totalprob+=it->second;
  
  for(map<int,double>::iterator it=map_charge_prob.begin();it!=map_charge_prob.end();it++)
    it->second/=totalprob;
  
  int toReturn=0;double maxprob=0.;
  track->charge_probabilities.clear();                                              
  for(map<int,double>::iterator it=map_charge_prob.begin();it!=map_charge_prob.end();it++){
    if(it->second>maxprob){
      maxprob=it->second;
      toReturn=it->first;
    }
    track->charge_probabilities.insert(pair<double,int>(it->second,it->first));
  }

  return toReturn%10;
}

float TrdHChargeR::GetELikelihood(TrdHTrackR *tr, int opt,int debug){
  if(debug)printf("Enter TrdHChargeR::GetELikelihood\n");
  double elik=1., plik=1.,loglik=0.;
  
  // old likelihood methods
  if(opt<0){
    double pars[7];
    for(int i=0;i<7;i++)pars[i]=p_par(i);
    
    int l=0,n=0;
    for(l=0;l!=20;l++){
      float amp=tr->elayer[l]*3./100.;
      
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
    
    loglik=-log(elik/(elik+plik));
    if(debug)printf("elik %.2f plik %.2f likelihood %.2f\n",elik,plik,loglik);
  }
  else{
    for(map<double,int>::iterator it=tr->charge_probabilities.begin();it!=tr->charge_probabilities.end();it++){
      if(it->second==0)elik=it->first;
      if(it->second==1)plik=it->first;
    }
    loglik=-log(elik/(elik+plik));
  }

  if(opt%10==0)return loglik;
  else if(opt%10==1)return elik;
  else if(opt%10==2)return plik; 
  else return 0.; 
}


bool TrdHChargeR::FillPDFsFromTDV(int debug){
  if(debug)printf("TrdHChargeR::FillPDFsFromTDV\n");
  for(int n=0;n<7;n++){
    if(debug)printf("n %i\n",n);
    
    int i=0;
    while(i<1000)
      {
	if(charge_hist_array[n][i+1]==0)break;
	
	int c=(int)charge_hist_array[n][i++];
	int npnt=(int)charge_hist_array[n][i++];
	if(debug)cout<<"c "<<c<<" npnt "<<npnt<<endl;
	TrPdf *pdf=new TrPdf((char*)GetStringForTDVEntry(c).c_str());

	int p=0;
	while(p++<npnt){
	  float x=charge_hist_array[n][i++];
	  float y=charge_hist_array[n][i++];
	  pdf->AddPoint(x,y);
	}
	
	if(debug)pdf->Info(0);
	pdfs.insert(pair<int,TrPdf*>(c,pdf));
      }
  }

  // electrons
  TrPdf *epdf[20];
  for(int l=0;l<20;l++)epdf[l]=0;

  int i=0;
  while(i<10000)
    {
      if(electron_hist_array[i]==0)break;
      
      int l=(int)electron_hist_array[i++];
      int arrl=-l-1;
      if(l>-1||l<-20)cerr<<"TrdHChargeR::FillPDFsFromTDV - l expected to be between -20 and -1 but it is "<<l <<endl;
      if(arrl<0||arrl>19)cerr<<"TrdHChargeR::FillPDFsFromTDV - arrl expected to be between 0 and 19 but it is"<< arrl<<endl;
      int npnt=(int)electron_hist_array[i++];
      if(debug)cout<<"l "<<l<< " npnt "<<npnt<<endl;
      if(!epdf[arrl])
	epdf[arrl]=new TrPdf((char*)GetStringForTDVEntry(l).c_str());

      float ysum=0.;
      int p=0;
      while(p++<npnt){
	float x=electron_hist_array[i++];
	float y=electron_hist_array[i++];
	ysum+=y;
	epdf[arrl]->AddPoint(x,y);
      }
      
      if(debug)epdf[arrl]->Info(0);
      if(ysum>0.)
	pdfs.insert(pair<int,TrPdf*>(l,epdf[arrl]));
    }

  cout<<"TrdHChargeR::FillPDFsFromTDV - PDFs loaded from TDV: "<<pdfs.size()<<endl;
  return true;
}

bool TrdHChargeR::FillTDVFromPDFs(int debug){
  map<int,TrPdf*>::iterator it;

  for(int n=0;n<10;n++)
    for(int j=0;j<1000;j++)charge_hist_array[n][j]=0.;
  
  for(int i=0;i<10000;i++)electron_hist_array[i]=0.;

  int eliter=0;
  int iter[10];
  for(int i=0;i<10;i++)iter[i]=0;

  for(map<int,TrPdf*>::const_iterator it=pdfs.begin();it!=pdfs.end();it++){
    if((int)it->second->GetN()==0)continue;
    int n=it->first;
    if(n>10)n-=10;
    
    it->second->Info(1);    
    if(n>=0){
      charge_hist_array[n][iter[n]++]=it->first;
      charge_hist_array[n][iter[n]++]=(int)it->second->GetN();
      
      if(debug)printf("TrdHChargeR::FillTDVFromPDFs - c %i ntdv %i pnt %i\n",it->first,n,it->second->GetN());
      for(int p=0;p<(int)it->second->GetN();p++){
	if(debug)printf(" p %i - %.2f %.2e\n",p,it->second->GetX(p),it->second->GetY(p));
	charge_hist_array[n][iter[n]++]=it->second->GetX(p);
	charge_hist_array[n][iter[n]++]=it->second->GetY(p);
      }
    }
    else{//electron
      electron_hist_array[eliter++]=it->first;
      electron_hist_array[eliter++]=(int)it->second->GetN();
      if(debug)printf("TrdHChargeR::FillTDVFromPDFs - L %i ntdv %i pnt %i\n",-it->first,n,it->second->GetN());
      for(int p=0;p<(int)it->second->GetN();p++){
	if(debug)printf(" p %i - %.2f %.2e\n",p,it->second->GetX(p),it->second->GetY(p));
	electron_hist_array[eliter++]=it->second->GetX(p);
	electron_hist_array[eliter++]=it->second->GetY(p);
      }
    }
  }
  return true;
}


int TrdHChargeR::AddEvent(TrdHTrackR *track,int pid, int debug){
  if(debug)cout<<"Entering TrdHChargeR::AddEvent"<<endl;
  if(!track)return 1;

  if(pid>=0){
    map<int,TH1F*>::iterator it=spectra.find(pid);
    if(it==spectra.end()){
      char name[20] ;
      sprintf(name,"h_spectrum_%i%s",pid,tag.c_str());
      if(debug)printf("TrdHChargeR::AddEvent - adding %s\n",name);
      
      TH1F *h=0;
      if(nlogbins>0)h=new TH1F(name,"",nlogbins-1,logbins);
      else h=new TH1F(name,"",399,10,4000);
      spectra.insert(pair<int,TH1F*>(pid,h));
      
      it=spectra.find(pid);
    }
  
    for(int i=0;i<20;i++)
      it->second->Fill(track->elayer[i]);
  }
  if(pid==0){
    // storing electrons per layer as negative charges
    // numbering layer L1-L20 to avoid confusion
    for(int l=1;l<21;l++){
      map<int,TH1F*>::iterator it=spectra.find(-l);
      if(it==spectra.end()){
	char name[20] ;
	sprintf(name,"h_spectrum_el_L%i%s",l,tag.c_str());
	
	TH1F *h=new TH1F(name,"",399,10,4000);
	if(nlogbins)h->SetBins(nlogbins-1,logbins);
	spectra.insert(pair<int,TH1F*>(-l,h));
	it=spectra.find(-l);
      }
      
      it->second->Fill(track->elayer[l-1]);
    }
  }
  
  return 0;
}

int TrdHChargeR::CreatePDFs(int debug){
  if(debug)cout<<"Entering TrdHChargeR::CreatePDFs - spectra size "<<spectra.size()<<endl;
  pdfs.clear();
  char name[20];
  
  for(map<int,TH1F*>::iterator it=spectra.begin();it!=spectra.end();it++){
    if(debug)printf("charge %i\n",it->first);
    if(it->first<0)
      sprintf(name,"TRDHPDF_EL_L%i",-it->first);
    else
      sprintf(name,"TRDHPDF%i",it->first);
    
    if(debug)printf("name %s\n",name);

    TrPdf *pdf=new TrPdf(name,it->second,true,true);

    pdfs.insert(pair<int,TrPdf*>(it->first,pdf));
  }

  cout<<"pdfs size "<<pdfs.size()<<endl;
  return 0;
}

string TrdHChargeR::GetStringForTDVEntry(int n){
  string name;
  char num[2];
  if (n<0){
    name="TRDElectronL";
    sprintf(num,"%i",-n);
    name.append(num);
  }
  if(n>=0){
    name="TRDCharge";
    sprintf(num,"%i",n);
    name.append(num);
  }
  return name;
}

int TrdHChargeR::GetTDVEntryForMapKey(int c){
  if(c<0)return 9-c;
  else return c-1;
}

int TrdHChargeR::CreateBins(int decades,int base, int binsperdecade){
  nlogbins=0;
  for(int d=base;d<base+decades;d++)
    for(int n=0;n<binsperdecade;n++){
      logbins[nlogbins]=pow(10,(double)d)*pow(10,(double)n/(double)binsperdecade);
      nlogbins++;
      if(logbins[nlogbins-1]>4096)break;
    }

  return 1;
}

bool TrdHChargeR::closeTDV(){
  for(map<string,AMSTimeID*>::iterator it=tdvmap.begin();it!=tdvmap.end();it++){
    delete it->second;
  }
  tdvmap.clear();
  return 1;
}

int TrdHChargeR::readAllTDV(unsigned int t, int debug){
  time_t thistime=(time_t)t;
  
  int error=0;
  if(debug)printf("map size %i\n",(int)tdvmap.size());
  for(map<string,AMSTimeID*>::iterator it=tdvmap.begin();it!=tdvmap.end();it++){
    if(debug){
      time_t insert,begin,end;
      it->second->gettime(insert,begin,end);
      printf("request %i insert %u begin %u end %u\n",t,insert,begin,end);
    }
    if(!it->second->validate(thistime)){
      cerr<<"TrdHChargeR::readAllTDV - "<<it->first<<" not validated"<<endl;
      error++;
    }
  }
  
  return error;
}

int TrdHChargeR::readSpecificTDV(string which,unsigned int t, int debug){
  time_t thistime=(time_t)t;
  
  int error=0;
  for(map<string,AMSTimeID*>::iterator it=tdvmap.begin();it!=tdvmap.end();it++)
    if(which==it->first)if(!it->second->validate(thistime))error++;

  return error;
}

int TrdHChargeR::initAllTDV(unsigned int bgtime, unsigned int edtime, int type,char * databasedirname){
  time_t begintime = (time_t) bgtime;
  time_t endtime   = (time_t) edtime;
  
  AMSTimeID::CType server=AMSTimeID::Standalone;
  tm begin = *gmtime(&begintime);
  tm end   = *gmtime(&endtime);
  
  if(strlen(databasedirname)){
    char tdname[200];
    sprintf(tdname, "%s/DataBase/", databasedirname);
    if(!AMSDBc::amsdatabase)
      AMSDBc::amsdatabase=new char[200];

    strcpy(AMSDBc::amsdatabase,tdname);
  }
  

  // all layer charges (0:e,1:p,2:He etc)
  //  for(int n=0;n<7;n++){
  for(int n=1;n<3;n++){
    AMSTimeID *ptdv=new AMSTimeID(AMSID(GetStringForTDVEntry(n).c_str(),type),
				  begin,end,sizeof(TrdHChargeR::charge_hist_array[n][0])*1000,
				  (void*)TrdHChargeR::charge_hist_array[n],server);
    tdvmap.insert(pair<string,AMSTimeID*>(GetStringForTDVEntry(n),ptdv));
  }

  // single layers electron
  {
    AMSTimeID *ptdv=new AMSTimeID(AMSID("TRDElectron",type),
				  begin,end,sizeof(TrdHChargeR::electron_hist_array[0])*10000,
				  (void*)TrdHChargeR::electron_hist_array,server);
    tdvmap.insert(pair<string,AMSTimeID*>("TRDElectron",ptdv));
  }
  
  if( readAllTDV(bgtime+5) ){
    cerr<<"TrdHChargeR::initAllTDV - readTDV error"<<endl;
    return 1;
  }
  

  return 0;
}

// write calibration to TDV
int TrdHChargeR::writeAllTDV(unsigned int bgtime, unsigned int edtime, int debug, char * databasedirname){
  if( !FillTDVFromPDFs() ){
    printf("Warning: FillTDVFromMedian inside writeTDV\n");
    return 1;
  }

  time_t begin,end,insert;

  bool ok=true;
  for(map<string,AMSTimeID*>::const_iterator it=tdvmap.begin();it!=tdvmap.end();it++){
    it->second->UpdateMe()=1;
    unsigned int crcold=it->second->getCRC();
    it->second->UpdCRC();
    
    printf("crcold %u new %u\n",crcold,it->second->getCRC());
    if(crcold!=it->second->getCRC()){
      // valid for 1 day
      time(&insert);
      begin=(time_t)bgtime;
      end=(time_t)edtime;
      it->second->SetTime(insert,begin+10,end+86400);
      cout <<" Write time:" << endl;
      cout <<" Time Insert "<<ctime(&insert);
      cout <<" Time Begin "<<ctime(&begin);
      cout <<" Time End "<<ctime(&end);
      
      char tdname[200] = "";
      if(strlen(databasedirname))sprintf(tdname, "%s/DataBase/", databasedirname);
      else if (AMSDBc::amsdatabase) sprintf(tdname, "%s/DataBase/", AMSDBc::amsdatabase);
      else{
	return 2;
      }
      
      if(!it->second->write(tdname)) {
	cerr <<"TrdHChargeR::writeAllTDV - problem to update tdv "<<it->first<<endl;
	ok=false;
      }
    }
  }
  
  return 0;
};

int TrdHChargeR::writeSpecificTDV(string which,unsigned int bgtime, unsigned int edtime, int debug, char * databasedirname){
  time_t begin,end,insert;
  
  bool ok=true;
  for(map<string,AMSTimeID*>::const_iterator it=tdvmap.begin();it!=tdvmap.end();it++){
    if(which!=it->first)continue;

    it->second->UpdateMe()=1;
    unsigned int crcold=it->second->getCRC();
    it->second->UpdCRC();
    
    if(crcold!=it->second->getCRC()){
      // valid for 1 day
      time(&insert);
      begin=(time_t)bgtime;
      end=(time_t)edtime;
      it->second->SetTime(insert,begin+10,end+86400);

      char tdname[200] = "";
      if(strlen(databasedirname))sprintf(tdname, "%s/DataBase/", databasedirname);
      else if (AMSDBc::amsdatabase) sprintf(tdname, "%s/DataBase/", AMSDBc::amsdatabase);
      else{
	return 2;
      }

      if(!it->second->write(tdname)) {
	cerr <<"TrdHChargeR::writeSpecificTDV - problem to update tdv "<<it->first<<endl;
	ok=false;
      }
    }
  }

  return ok;
};

int TrdHChargeR::Initialize(AMSEventR* pev, char *databasedir){
  if(lastrun!=pev->Run()){
    int bgntime=pev->Run();
    int endtime=pev->Run()+10;
    
    if(lastrun>0)closeTDV();
    else{
      initAllTDV(bgntime,endtime,int(pev->nMCEventg()==0),databasedir);
      FillPDFsFromTDV();
      lastrun=pev->Run();
    }
    return 1;
  }
  return 0;
}

