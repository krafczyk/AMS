#include "TrdTFit.h"
#include <fstream>
#include "root.h"

//using namespace TRDTFIT;
ClassImp(TrdTFit);
TrdTFit *TrdTFit::head=0;

// method needed to separate TRD dallas sensor names into A-/B-tag
vector<string> splitSensorName(string str){
  size_t found;

  vector<string> toReturn;

  found=str.find_first_of(" ");
  if (found!=string::npos)
    {
      toReturn.push_back(str.substr(0,found));
    }
  found=str.find_last_of(" ");
  if (found!=string::npos)
    {
      toReturn.push_back(str.substr(found+1));
    }

  return toReturn;
}

int TrdTFit::DoFit(int time){
  if(!SensPosAmpVec)return 0;
  if((int)SensPosAmpVec->size()<150){
    SensPosAmpVec->clear();return -2;
  }
  
  if(!fitfunc)
    fitfunc=new Tfunc(20.,0.,0.,0.,0.,0.,0.,0.);  
  
  tm *t = gmtime((time_t*)&time);
  char updtxt[120];
  sprintf(updtxt,"%02d/%02d %02d:%02d:%02d",t->tm_mday,t->tm_mon+1,t->tm_hour, t->tm_min, t->tm_sec);

  if(debug)
    cout<<"TrdTFit::DoFit-I-starting fit for "<<SensPosAmpVec->size()<<" values - time "<<updtxt<<endl;;

  int fitted=GlobalTFit(fitfunc, SensPosAmpVec);

  // possible goodness-of-fit-test
  chi2=0;
  if(fitted){
    for(vector<double>::iterator it=fitfunc->getResiduals().begin(); it!=fitfunc->getResiduals().end();it++)chi2+=*it/0.1;
    if(debug){
      fitfunc->Info();
      if(debug)cout<<"TrdTFit::DoFit-I-chi2 "<<chi2<<endl;
    }
    // reset 
    fitfunc->clearResiduals();
  }
  SensPosAmpVec->clear();

  return fitted;
}

// read and fit TRD temperatures for given event
int TrdTFit::ReadAndFitTemperatures(AMSEventR *pev){
  int time=pev->fHeader.Time[0];
  if(SensPosAmpVec)SensPosAmpVec->clear();

  AMSSetupR::SlowControlR *cr=&pev->getsetup()->fSlowControl;
  if(!cr){
    cerr<<"TrdTFit::DoFit-E-SlowControlR pointer not found"<<endl;
    exit(1);
  }
  for(AMSSetupR::SlowControlR::etable_i it=cr->fETable.begin();it!=cr->fETable.end();it++){
    
    if(strstr(it->second.NodeName.Data(),"USCM_UG")&&it->second.datatype==25&&it->second.subtype<202){
      vector<float> values;
      int stat=pev->getsetup()->fSlowControl.GetData(it->first.c_str(),time, 100.,  values);//,it->second.datatype,it->second.subtype);
      if(stat!=0){
	cout<<"TrdTFit::DoFit-I-stat "<<stat<<" - begin "<<cr->fBegin<<" end "<<cr->fEnd<<" time "<<time<<endl;
	continue;
      }
      
      vector<string> strvec=splitSensorName(it->first);
      if(strvec[1].compare(0,1,"P")!=0)
	fill(strvec[1].c_str(),values[0]);
    }
  }
  int stat=-1;
  if(SensPosAmpVec){
    if(debug)
      cout<<"TrdTFit::DoFit-I-vector of sensors size "<<SensPosAmpVec->size()<<endl;
    stat=DoFit(time);
  }
  return stat;
}

  double TrdTFit::MYRANDOM(int &myidum){
    const int IA=16807,IM=2147483647,IQ=127773,IR=2836,NTAB=32;
    const int NDIV=(1+(IM-1)/NTAB);
    const double EPS=3.0e-16,AM=1.0/IM,RNMX=(1.0-EPS);
    static int iy=0;
    static int iv[NTAB];
    int j,k;
    double temp;
    
    if (myidum <= 0 || !iy) {
      if (-myidum < 1) myidum=1;
      else myidum = -myidum;
      for (j=NTAB+7;j>=0;j--) {
	k=myidum/IQ;
	myidum=IA*(myidum-k*IQ)-IR*k;
	if (myidum < 0) myidum += IM;
	if (j < NTAB) iv[j] = myidum;
      } 
      
      iy=iv[0];
    }
    k=myidum/IQ;
    myidum=IA*(myidum-k*IQ)-IR*k;
    if (myidum < 0) myidum += IM;
    j=iy/NDIV;
    iy=iv[j];
    iv[j] = myidum;
    if ((temp=AM*iy) > RNMX){
      return RNMX;
    }
    else{
      return temp;
    }
  }

double TrdTFit::toBeMinimised(double *x /*parameters*/)
  {
    //value of loglikelihood function
    double f = 0; 
    
    fitfunc->modify(x);

    vector<double> residuals;
    // evaluation of likelihood function
    f = logLIK(fitfunc, begin, end, 0,residuals,false);

    ++ncount;

    //return loglikelihood
    return f;
  }


void TrdTFit::SA(int& N, double* X, double& RT, double& EPS, int& NS, int& NT, 
		 int& NEPS, int& MAXEVL, double* LB, double* UB, double* C, 
		 int& IPRINT, double& T, double* VM, double* XOPT, double& FOPT, 
		 int& NACC, int& NFCNEV, int& NOBDS, int& IER, double* FSTAR, 
		 double* XP, int* NACP /*, TH2F * h2DSpace*/)
{    
  double F, FP, P, PP, RATIO;
  int NUP, NDOWN, NREJ, NNEW, LNOBDS;
  bool QUIT;
  NACC = 0;
  NOBDS = 0;
  NFCNEV = 0;
  IER = 99;
  for(int i=0; i<N; i++){
      XOPT[i]=X[i];
      NACP[i] = 0;
    }
    for(int i=0; i<NEPS; i++){
      FSTAR[i] = 1.0E20;
    }
    if(T <= 0.0){
      std::cout << "TrdTFit::SA-E-T < 0!" << std::endl;
      IER = 3;
      return;
    }
    for(int i=0;i<N;i++){
      if((X[i] > UB[i]) || (X[i] < LB[i])){
	IER = 2;
	return;
      }
    } 
    F = toBeMinimised(X);
    F = -F;
    NFCNEV++;
    FOPT = F;
    FSTAR[0] = F;
    for(;;){
      NUP = 0;
      NREJ = 0;
      NNEW = 0;
      NDOWN = 0;
      LNOBDS = 0;
      for(int M=0; M<NT; M++){
	for(int J=0; J<NS; J++){
	  for(int H=0; H<N; H++){
	    for(int I = 0; I<N; I++){
	      if(I == H){
		XP[I] = X[I] + 
		  (MYRANDOM(myidum) *2. -1.) * VM[I];
	      }
	      else{
		XP[I] = X[I];
	      }
	      if((XP[I] < LB[I]) || (XP[I] > UB[I])){
		XP[I] = LB[I] + 
		  (UB[I] - LB[I])*MYRANDOM(myidum);
		LNOBDS++;
		NOBDS++;
	      }  
	    }
	    FP = toBeMinimised(XP);
	    FP = -FP;
	    NFCNEV++;
	    if(NFCNEV >= MAXEVL){
	      FOPT = -FOPT;
	      IER = 1;
	      return;
	    }
	    if(FP >= F){
	      // step is accepted normally
	      for (int I=0; I<N; I++){
		X[I] = XP[I];
	      }
	      //h2DSpace -> Fill( X[3],X[4] );
	      F = FP;
	      NACC++;
	      NACP[H]++;
	      NUP++;
	      if(FP > FOPT){
		if(IPRINT >= 4){
		  std::cout << "TrdTFit::SA-I-NEW OPTIMUM!" << std::endl;
		  std::cout << "TrdTFit::SA-I-FP: " << FP << std::endl;
		}
		for(int I=0; I<N; I++){
		  XOPT[I] = XP[I];
		}
		FOPT = FP;
		NNEW++;
	      }
	    }
	    else{
	      P = exp((FP-F)/T);
	      PP = MYRANDOM(myidum);
	      if(PP < P){
		// step is accepted by chance
		for(int i=0; i<N;i++){
		  X[i] = XP[i];
		}
		//h2DSpace -> Fill( X[3],X[4] );
		F = FP;
		NACC++;
		NACP[H]++;
		NDOWN++;
	      }
	      else{
		NREJ++;
	      } 
	    }
	  }
	}
	for(int I=0;I<N;I++){
	  RATIO = static_cast<double>(NACP[I])/static_cast<double>(NS);
	  if(RATIO > 0.6){
	    VM[I] = VM[I]*(1.+C[I]*(RATIO -0.6)/0.4);
	  }
	  else{
	    if(RATIO < 0.4){
	      VM[I] = VM[I]/(1.0 + C[I]*((0.4 -RATIO)/0.4));
	    }
	  }
	  if(VM[I] > (UB[I]-LB[I])){
	    VM[I] = UB[I] - LB[I];
	  }
	}	
	for(int i=0; i<N; i++){
	  NACP[i] = 0;
	}
      }      
      QUIT = false;
      FSTAR[0] = F;
      if((FOPT - FSTAR[0]) <= EPS)
	QUIT = true;
      for(int i=0; i<NEPS; i++){
	if(fabs(F-FSTAR[i]) > EPS)
	  QUIT = false;
      }
      if(QUIT){
	for(int i=0;i<N;i++){
	  X[i] = XOPT[i];
	}
	IER = 0;
	FOPT = -FOPT;
	return;
      }
      T = RT * T;
      for(int i=NEPS-1; i>0; i--){
	FSTAR[i] = FSTAR[i-1];
      }
      F = FOPT;
      for(int i=0; i<N; i++){
	X[i] = XOPT[i];
      }
    }
  }

  inline bool TrdTFit::GlobalTFit(Tfunc* fitfunc, ownvector *posTvec){
  {
  
    begin=posTvec->begin();
    end=posTvec->end();
    
    //number of fit degrees
    int m = posTvec->size();

    //array of Parameters to be fitted
    int N=7;
    double x[N];
    
    TStopwatch timer;
    timer.Start();
    
    x[0]=fitfunc->getT0();
    x[1]=fitfunc->getPx();
    x[2]=fitfunc->getPx2();
    x[3]=fitfunc->getPy();
    x[4]=fitfunc->getPy2();
    x[5]=fitfunc->getPz();
    x[6]=fitfunc->getPz2();
  


    int NEPS=4; 

    int NS, NT, NFCNEV, IER, MAXEVL, IPRINT, NACC, NOBDS;
    double * LB = new double[N];
    double * UB = new double[N];
    double * XOPT = new double[N];
    double * C = new double[N]; 
    double * VM = new double[N];
    double * FSTAR = new double[NEPS];
    double * XP = new double[N];
    double T, EPS, RT, FOPT; 
    int * NACP = new int[N];
    // internal parameters

    /*** DO NOT CHANGE FOLLOWING PARAMETERS ***/

    NS = 20;

    NT = 5;

    for(int i=0; i<N; i++){
      C[i] = 2.0;
    }

    for (int i=0; i<N;i++){
      VM[i] = 1.0;
    }


    /*** Following parameters may be changed carefully  ***/

    EPS = 1.0E-5;
    //relative precision of the likelihood value

    RT = 0.85;
    //Annealing-Faktor: 0<RT<1; the higher the value the higher the probability to leave local minima and the higher the fit time
    //recommended: between 0.85 and 0.5

    MAXEVL = 500000;
    //Catch: maximum number of iterations; then abort even if EPS precision not reached; correlated to T and RT

    IPRINT = 3;
    //printout level;

    //definition of parameter space
    //lower bound LB
    //upper bound UB
    //0,1,2: T0,px,py,pz
    LB[0] = -40;
    UB[0] = 60;
    LB[1] = -10;
    UB[1] = 10;
    LB[2] = -10;
    UB[2] = 10;
    LB[3] = -10; 
    UB[3] = 10;
    LB[4] = -10;
    UB[4] = 10;
    LB[5] = -10;
    UB[5] = 10;
    LB[6] = -10; 
    UB[6] = 10;


    T = 100000.0;
    //start value for annealing. T>0. the higher the value the higher the probability to leave local minima and the higher the fit time
    
    //global parameter: iteration steps
    ncount = 0; // set back to zero

    SA(N,x,RT,EPS,NS,NT,NEPS,MAXEVL,LB,UB,C,IPRINT,T,VM,XOPT,FOPT,
       NACC,NFCNEV,NOBDS,IER,FSTAR,XP,NACP); 

    double t_cpu = timer.CpuTime();
    
    std::cout << "TrdTFit::SA-I-cpu time spend = " 
    	      << t_cpu << " seconds" << std::endl << std::flush; 

    fitfunc->modify(x);
    // store fit results
    // Tmean, px, py, pz

    //store residuals
    vector<double> residuals;
    res = 1;
    double logLik = logLIK(fitfunc,begin,end,res,residuals,false);
    fitfunc->setResiduals(residuals);

    //store loglik of final fit
    fitfunc->setLik(logLik);
   
    delete[] LB;
    delete[] UB;
    delete[] XOPT;
    delete[] C; 
    delete[] VM;
    delete[] FSTAR;
    delete[] XP;
    delete[] NACP;    

	
    return true;
  }
  }

  //PDF (simple pdf to test minimisation algorithm)
  double TrdTFit::simple_pdf(double& Tpos,double& Texp){
    const double sigma = 1.0;
    
    const double diff =  Tpos - Texp;
    double T_pdf = 1.0 / sqrt( pow(sigma,2) * 2.0 * M_PI ) * 
      exp( - pow(diff, 2) / (2.0 * pow(sigma,2)));
    
    if (T_pdf < 0.0000000005) return 0.00000000005;
    return T_pdf;
  }

double TrdTFit::logLIK(Tfunc* fitfunc, ownvector::iterator begin_, ownvector::iterator end_, const int& res, vector<double>& residuals, const bool& debug) 
{
  double lik = 0.0;
  double T_pos=-1.e6;
  double T_expected=-1.e6;
  
  for(ownvector::iterator it=begin_; it!=end_;it++){
    T_pos=it->second;
    T_expected=fitfunc->getTempAtPos(it->first);
    
    if(res==1){
      residuals.push_back(TMath::Abs(T_pos-T_expected));
    }
    
    lik += log( simple_pdf (T_pos,T_expected));
  }
  
  return -lik;
}
  
// create map of sensor name and position from external file
void TrdTFit::makeSensorMap(char* mapnam){
  ifstream maps;
  maps.open(mapnam, ifstream::in);
  
  if(!maps.is_open())cerr<<"TrdTFit::makeSensorMap-E-file "<<mapnam<<endl;
  if(!sensMap)sensMap=new map<string, AMSPoint>;
  
  sensMap->clear();
  
  char chars[17];
  string ID_;
  float x_,y_,z_;
  AMSPoint sensPos;

  while(!maps.eof()){
    if(!maps.good()) break;
    maps >> chars;
    maps >> x_ >> y_ >> z_;
    maps.ignore(256,'\n');
    ID_=chars;
    sensPos.setp(x_,y_,z_);
    sensMap->insert(pair<string, AMSPoint>(ID_,sensPos));
  }

  maps.close();
  return;
}

// show TRD DTS map 
void TrdTFit::showSensorMap() {
  map<string, AMSPoint>::iterator itr;
  int i = 0;

  cout<<"TrdTFit::showSensorMap-I-Number of sensors in vector "<<sensMap->size()<<endl;
  for(itr=sensMap->begin(); itr != sensMap->end(); ++itr) {
    cout << " I= " << i  << " "<< itr->first << ": " 
	 << "X=" <<itr->second.x() << " " << "Y=" <<itr->second.y() << " " << "Z=" << itr->second.z() << endl;
    i++;
  }
  cout << endl;
}

// Fill temperature sensor informations into map
int TrdTFit::fill(const char *name,float temp){
  map<string, AMSPoint>::iterator sit;
  sit=sensMap->find( (string)name );

  if(sit==sensMap->end()){
    cerr<<"TrdTFit::fill-E-sensor "<<name<<" not found"<<endl;
    return 1;
  }
  else if(temp>-100&&temp<100){
    if(!SensPosAmpVec)SensPosAmpVec=new ownvector;
    int got=0;
    for(int i=0;i<(int)SensPosAmpVec->size();i++){
      if(SensPosAmpVec->at(i).first.x()==sit->second.x()&&
	 SensPosAmpVec->at(i).first.y()==sit->second.y()&&
	 SensPosAmpVec->at(i).first.z()==sit->second.z()){
	SensPosAmpVec->at(i).second=temp;got=1;break;
      }
    }
    
    if(!got)
      SensPosAmpVec->push_back(make_pair<AMSPoint,float>(sit->second,temp));
  }
  return 0;
}

