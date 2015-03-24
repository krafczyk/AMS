#include "TrkCC.h"
#include <stdlib.h>

#ifdef _PGTRACK_

#ifndef _NOGBATCH_
AMSEventR* TrkCC::_HeadE=0;
#endif //_NOGBATCH_ 

//! Maximum number of printout of warnings/errors
#define ERRMAX 99

void PrintError(TString err, bool exempt){

  static int _errs=0;
  
  if (_errs<ERRMAX || exempt) {   
    cerr << err.Data() << endl;
    _errs++; 
  }
  
  return;
}

//----------------------------------------------

TrkCCVar* TrkCCVar::_head=NULL;
TrkCCNozzoli* TrkCCNozzoli::_head[nReg][nVer];
TrkCCPizzolotto* TrkCCPizzolotto::_head[nBDT][nVer];

#ifndef _NOGBATCH_
ClassImp(TrkCC)
ClassImp(TrkCCVar)
ClassImp(TrkCCAlgorithm)
ClassImp(TrkCCNozzoli)
ClassImp(TrkCCPizzolotto)
ClassImp(TMVAxmlReader)
#endif

TrkCC::TrkCC(TString algo, TString particle, int version, int gbatchPRODversion, int gbatchCODEversion, int runtype, bool istolerant, int verbose){
  
  // input:
  _verbose=verbose;
  _istolerant=istolerant;
  _runtype=runtype;
  _gbatchCODEversion=gbatchCODEversion;
  _gbatchPRODversion=gbatchPRODversion;
  _version=version;
  _particle=particle;
  _algo=algo;

  bool notok=true;
  if (_algo.Contains("Nozzoli") || _algo.Contains("Pizzolotto")) {
    if (_particle=="electron" || _particle=="Electron" || _particle=="ELECTRON") _normRIG = 0;
    else if (_particle=="protonMip" || _particle=="ProtonMip" || _particle=="PROTONMIP") _normRIG = 1;
    else if (_particle=="antiprotonMip" || _particle=="AntiProtonMip" || _particle=="ANTIPROTONMIP") _normRIG = -1;
    else if (_particle=="protonShower" || _particle=="ProtonMip" || _particle=="PROTONMIP") _normRIG = 2;
    else if (_particle=="antiprotonShower" || _particle=="AntiProtonShower" || _particle=="ANTIPROTONSHOWER") _normRIG = -2;
    TString subid;
    TString Letter[10]={"A", "B", "C", "D", "E", "F", "G", "H", "I", "J"};
    for (int ii=0; ii<nReg; ii++) {
      if (_algo=="NozzoliRegression"+Letter[ii]) subid=Letter[ii];
    }
    for (int ii=0; ii<nBDT; ii++) {
      if (_algo=="PizzolottoBDT"+Letter[ii] || _algo=="PizzolottoRarity"+Letter[ii]) subid=Letter[ii];
    }
    if (_algo.Contains("Nozzoli")) {
      algCC = TrkCCNozzoli::gethead(subid, version, _verbose);
    }
    else if (_algo.Contains("Pizzolotto")) {
      algCC = TrkCCPizzolotto::gethead(subid, version, _verbose);
      if (_algo.Contains("Rarity")) {
	((TrkCCPizzolotto*)algCC)->SetRarity();
      }
    }
    notok=false;
  }
  else {
    notok=true;
  }
  if (notok) {
    cerr << "TrkCC::ctor-E-_algo/particle not recognized: " << _algo.Data() << "/" << _particle.Data() << "\n";
  }

  if (_verbose>2) printf("TrkCC::ctor-I-TrkCCAlgorithm instanciated\n");
  
  _nvars = algCC->GetNVars();
  
  _gr = new TGraph***[nTrkCCpatts];
  for (int ii=0; ii<nTrkCCpatts; ii++) {
    _gr[ii] = new TGraph**[_nvars];
    for (int jj=0; jj<_nvars; jj++) {
      _gr[ii][jj] = new TGraph*[5];
    }
  }
  
  
  varCC = TrkCCVar::gethead();
  if (_verbose>2) printf("TrkCC::ctor-I-TrkCCVar instanciated\n");

  if (_runtype!=0) _LoadFiles();

  if (_verbose>2) printf("TrkCC::ctor-I-exiting from ctor\n");
}

TrkCC::~TrkCC(){
  Clear();
  
  for (int ii=0; ii<nTrkCCpatts; ii++) {
    for (int jj=0; jj<_nvars; jj++) {
      delete [] _gr[ii][jj];
    }
    delete [] _gr[ii];
  }
  delete [] _gr;  
};

void TrkCC::Clear(Option_t* option){
  
  if( _verbose>=2)  cout<<"TrkCC::Clear-I- calling clear "<<endl;

  if (algCC) delete algCC;
  algCC = NULL;

  for (int i=0;i<6;i++){
    if (_infile[i]) {
      _infile[i]->Close();
      delete _infile[i];
    }
  }
  
  return;
}

void TrkCC::_LoadFiles(){
  if (_verbose>=2) cout<<"TrkCC::_LoadFiles()-I-start loading files"<<endl;
  if( _runtype==0) {
    cout<<"TrkCC::_LoadFiles-I-Autodetection of RUN/MC still not performed"<<endl;
  }  else {
    // Load files
    TDirectory* curr = gDirectory;
    TString dbPath;
    TString dbdir;
    dbPath =Form("%s", getenv("TrkCCDataDir"));
    if (dbPath==NULL) {
      dbdir = Form("%s", getenv("AMSDataDir"));
      if (dbdir!=NULL) {
	dbPath = Form("%s/v5.00/TrkCC", dbdir.Data());
      }
      else dbPath=Form("/afs/cern.ch/exp/ams/Offline/AMSDataDir/v5.00/TrkCC");
    }
    if(_verbose>=2) cout <<"TrkCC::_LoadFiles-I-dbPath= "<<dbPath.Data()<< endl;
    // in a macro use #define TRKCCDATADIR "/storage/gpfs_ams/ams/users/fnozzoli/TrkCC/"
#ifdef TRKCCDATADIR
    //    dbPath=Form("/storage/gpfs_ams/ams/users/fnozzoli/TrkCC/");  //TESTING       
    dbPath=Form(TRKCCDATADIR);
    cout<<"TrkCC::_LoadFiles-I-DB PATH USER HARDCODED DEFINED "<<dbPath.Data()<<endl;
#endif
    
    int ll = _LoadWeights(dbPath);  
    if( ll!= 0 ) cerr<<"TrkCC::_LoadFiles-E-an error occourred in TrkCC::LoadWeights "<<endl;
    int lln = _LoadNormalisation(dbPath);
    if( lln!= 0 ) cerr<<"TrkCC::_LoadFiles-E-an error occourred in TrkCC::LoadNormalisation "<<endl;
    if (lln!=0 || ll !=0) {
      cerr<<"THE program will exit(1) / CRASH NOW" << endl;
      exit(1);
      TH1F* h=NULL; // Force crashing                                                               
      h->Fill(10);
    }
    if (curr) curr->cd();
  }
  return;
}

//-------------------------------------------------------------------------
#ifndef _NOGBATCH_  
int TrkCC::FindRunType(){
  // return = 1 DATA                                                            
  // return = 2 pre-launch BT or Muons (pre 1305331200)                         
  // return = 99  MC                                                            
  int run_type;
  //  cout << " in FindRunType" << endl;                                          
  AMSEventR* evt = AMSEventRHead()?AMSEventRHead():AMSEventR::Head();
  MCEventgR *pmc = evt->pMCEventg(0);
  if (!pmc) { 
    run_type = 1;  // is DATA                                      
    if (evt->Run()<1305331200) {// is pre-launch                              
      run_type = 2;
      //cerr<<"TrkCC::_FindRunType-I-Run autoselected as pre-ISS DATA (BT or Muons)" << endl;   
    }
    //else  cerr<<"TrkCC::_FindRunType-I-Run autoselected as DATA" << endl;      
  }
  else { // is MC              
    run_type = 99;
    //cerr<<"TrkCC::_FindRunType-I-Run autoselected as MC" << endl;      
  }
  return run_type;
}
#endif // _NOGBATCH_  

void TrkCC::_FirstLoad(){
  
#ifndef _NOGBATCH_
  if(!AMSEventR::Head()) {
    PrintError("TrkCC::_FirstLoad-E-AMSEventR still not existing");
  } 
  else {
    _runtype = FindRunType();  //autodetection of run type
  }
#endif   // _NOGBATCH_
  
  if (_istolerant==1 &&  _runtype==0) {
    cerr<<"TrkCC::_ProcessEvent-I-Failed to autodetect run-type, TOLERANT FORCING TO DATA run-type" << endl;
    _runtype=1;
  }
  if (_istolerant==0 &&  _runtype==0) {
    cerr<<"TrkCC::_ProcessEvent-E-Failed to autodetect run-type, THE program will exit(1) or CRASH NOW" << endl;
    exit(1);
    TH1F* h=NULL; // Force crashing                                                               
    h->Fill(10);
  }

  if(_runtype!=0) _LoadFiles();   //load for the first time the external files
  return;
}

// use if you already have an external filled TrkCCVar
int TrkCC::ProcessEvent(TrkCCVar* extTrkCCVar, int maskingnumber, float energyGuess){
  if (_runtype==0) _FirstLoad();
  if (!extTrkCCVar) return -2;
  varCC = extTrkCCVar;// since is a singleton actually pass this is useless but is more clear for the user
  algCC->_evenergy = varCC->energy;
  if (abs(_normRIG)==1) algCC->_evenergy=fabs(varCC->rigidity_ch);
  if (abs(_normRIG)==2) algCC->_evenergy=fabs(varCC->rigidity_ch);
  if (algCC->_evenergy==0) algCC->_evenergy=fabs(varCC->rigidity_ch);
  if (energyGuess > 0) algCC->_evenergy=energyGuess;  // use external guess
  if (algCC->_evenergy==0 && energyGuess == 0) energyGuess=100; // savatage default
  if (algCC->_evenergy==0) algCC->_evenergy=energyGuess;
  algCC->_recommendmask=_FillVarList(maskingnumber);
  _NormalizeList(); 
  return 1;
}

// use if you have an external vector already filled
int TrkCC::ProcessEvent(float varlistfull[nTrkCCVar],int maskingnumber, float energyGuess){
  if (_runtype==0) _FirstLoad();
  varCC = TrkCCVar::gethead();
  varCC->FillFromVector(varlistfull);//if there are more TrkCC this is useless since TrkCCVar is a singleton but this guarantees that if the event changed the TrkCC is filled correctly, paying not too much in terms of CPU time
  algCC->_evenergy = varCC->energy;
  if (abs(_normRIG)==1) algCC->_evenergy=fabs(varCC->rigidity_ch);
  if (abs(_normRIG)==2) algCC->_evenergy=fabs(varCC->rigidity_ch);
  if (algCC->_evenergy==0) algCC->_evenergy=fabs(varCC->rigidity_ch);
  if (energyGuess > 0) algCC->_evenergy=energyGuess;  // use external guess
  if (algCC->_evenergy==0 && energyGuess == 0) energyGuess=100; // savatage default
  if (algCC->_evenergy==0) algCC->_evenergy=energyGuess;
  algCC->_recommendmask=_FillVarList(maskingnumber);
  _NormalizeList(); 
  return 1;
}

// use if you already have an external varlist vector
// 
int TrkCC::ProcessEvent(float veclist[], float energyGuess, int pattern, int isnorm, unsigned long long recommendmask){
  if (_runtype==0) _FirstLoad();
  algCC->_evenergy=energyGuess;
  algCC->_evpatt=pattern;
  algCC->_recommendmask=recommendmask;
  if (isnorm==0){
    for(int ivv=0;ivv<_nvars;ivv++){
      algCC->_varlist[ivv]=veclist[ivv];
    }
    _NormalizeList();
  }
  if (isnorm==1){
    //  cout << "normalized?" << isnorm << endl;
    for(int ivv=0;ivv<_nvars;ivv++){
      algCC->_varnorm[ivv]=veclist[ivv];
    }
  }
  return 1;
}

#ifndef _NOGBATCH_
int TrkCC::ProcessEvent(int ipart, int rebuildRefit, float energyGuess, bool fullProcess){
  if (_runtype==0) _FirstLoad();
  algCC->_evenergy=0;
  static int nm=varCC->nmsk;
  AMSEventR* evt=AMSEventR::Head();
  if (varCC->run!=evt->Run() || varCC->evtno!=evt->Event() || varCC->ppno!=ipart) {//since TrkCCVar is a singleton, this avoid the reprossing of it if already done for that Run/Event/Particle
    varCC->run=evt->Run();
    varCC->evtno=evt->Event();
    varCC->ppno=ipart;
    ParticleR *part = evt->pParticle(ipart);
    nm = varCC->FillFromGbatch(part, rebuildRefit, _gbatchPRODversion, fullProcess);
    if (nm<1) {
      PrintError(Form("TrkCC::ProcessEvent-E-Problem in FillFromGbatch code = %d", nm));
      if (nm==-2) PrintError("AMS event does not exists");
      else if (nm==-1) PrintError("AMS particle does not exists");
      else if (nm==0) PrintError("tracker trk does not exists");
      if (nm<=0) return nm;
    }
  }
  algCC->_recommendmask = _FillVarList(nm);
  if (energyGuess > 0) algCC->_evenergy=energyGuess;  // use external guess
  if (algCC->_evenergy==0 && energyGuess == 0) energyGuess=100; // savatage default
  if (algCC->_evenergy==0) algCC->_evenergy=energyGuess;
  _NormalizeList(); 
  return nm;
}

int TrkCC::ProcessEvent(ParticleR* part, int rebuildRefit, bool forceProcess, float energyGuess, bool fullProcess){
  if (_runtype==0) _FirstLoad();
  algCC->_evenergy=0;
  static int nm=varCC->nmsk;
  AMSEventR* evt=AMSEventR::Head();
  if ((varCC->run!=evt->Run() || varCC->evtno!=evt->Event() || varCC->ppno!=-1* (long) part) && (!forceProcess)) {//since TrkCCVar is a singleton, this avoid the reprossing of it if already done for that Run/Event/Particle
    varCC->run=evt->Run();
    varCC->evtno=evt->Event();
    varCC->ppno=-1* (long) part;
    nm = varCC->FillFromGbatch(part, rebuildRefit, _gbatchPRODversion, fullProcess);
    if (nm<1) {
      PrintError(Form("TrkCC::ProcessEvent-E-Problem in FillFromGbatch code = %d", nm));
      if (nm==-2) PrintError("AMS event does not exists");
      else if (nm==-1) PrintError("AMS particle does not exists");
      else if (nm==0) PrintError("tracker trk does not exists");
      if (nm<=0) return nm;
    }
  }
  algCC->_recommendmask = _FillVarList(nm);
  if (energyGuess > 0) algCC->_evenergy=energyGuess;  // use external guess
  if (algCC->_evenergy==0 && energyGuess == 0) energyGuess=100; // savatage default
  if (algCC->_evenergy==0) algCC->_evenergy=energyGuess;
  _NormalizeList(); 
  return nm;
}
#endif // _NOGBATCH_

//-------------------------------------------------------------------------
int TrkCC::_NormalizeList(){
  if (_verbose>=9) cout<<"TrkCC::NormalizeList-I- begin"<<endl;
  int pattern= algCC->_evpatt;
  double logenergy=log10(fabs(algCC->_evenergy));
  bool curetails= true;//<-- should be an input parameter?

  double normvalue;
  double value;
  // Normalize variables:
  for( int iv=0; iv<_nvars; iv++){
    normvalue=0.;
    value = algCC->_varlist[iv];

    if (iv<_nvarf){
    if( _gr[pattern][iv][2]!=NULL){ //median is defined:
      normvalue=_NormBozNoz(value, logenergy, 
			   _gr[pattern][iv][1],_gr[pattern][iv][3],_gr[pattern][iv][2],_gr[pattern][iv][0],_gr[pattern][iv][4],
			   1, curetails);
    } else {  //when median is NOT defined use linear normalization:
      normvalue=_NormBozNoz(value, logenergy, 
			   _gr[pattern][iv][1],_gr[pattern][iv][3],(TGraph*)NULL, (TGraph*)NULL, (TGraph*)NULL,
			   0, curetails);    
    }}
    else 
      {normvalue = value;}
    if( !(fabs(normvalue)<1.e37)){ // Avoid NAN
	if(normvalue>1.e36)  normvalue=1.e36; //clamp + inf
	if(normvalue<-1.e36) normvalue=-1.e36;//clamp - inf
	if( !(fabs(normvalue)<1.e37)) normvalue = 0; // correct NAN
    }
    algCC->_varnorm[iv]=normvalue;
  }
   if (_verbose>=9) cout<<"TrkCC::NormalizeList-I- end"<<endl;
  return 0;
}

//-------------------------------------------------------------------------
int TrkCC::FillTH1(TH1* histo, int regtype, unsigned long long masktype){
  if (masktype==0) algCC->_usedmask=algCC->_recommendmask;
  else if (masktype==1) algCC->_usedmask=0xFFFFFFFFFFFFFULL;
  else algCC->_usedmask=(masktype & algCC->_recommendmask);
  int ifill = 0;
  for (int ivv=0;ivv<_nvars;ivv++){
    if (!(fabs(algCC->_varnorm[ivv]) >=0)) continue;
    double a = 1.;
    if (_algo.Contains("Nozzoli")){
      if (ivv>=((TrkCCNozzoli*)algCC)->_rmaxvar[regtype]) {a = 0;}
      else
	{a = ((TrkCCNozzoli*)algCC)->_rweight[regtype][algCC->_evpatt][0][0][ivv];}  // will give the sign of variable fixed by the one at energy bin 0
    }
    if(fabs(a)<1.e-30) continue;
    if( (algCC->_usedmask & ((unsigned long long) 1 << ivv)) == 0) continue;
    ifill++;
    double varfill = algCC->_varnorm[ivv]*a/fabs(a);
    histo->Fill(varfill);
  }
return ifill;
}

//-------------------------------------------------------------------------
//TrkCC::_LoadNormalisation Open files with energy normalisation parameters
int TrkCC::_LoadNormalisation(TString dbPath) {
  
  int iret = 0;
  if (_verbose>=2) cout<<"TrkCC::LoadNormalisation-I- begin"<<endl;
  if( _runtype==0) return (-9);
  
  // const char* type;
  char type[20];
  if( _runtype==1)  sprintf(type, "ISS");
  else if( _runtype==2)  sprintf(type, "BT");
  else if( _runtype==99) sprintf(type, "MC");
  
  TString dirname = Form("%s/version%d/normalizations_E/%s_B%d_codB%d/", dbPath.Data(), _version, type, _gbatchPRODversion, _gbatchCODEversion);
  if (_normRIG==1) dirname = Form("%s/version%d/normalizations_R/%s_B%d_codB%d/", dbPath.Data(), _version, type, _gbatchPRODversion, _gbatchCODEversion); 
  if (_normRIG==-1) dirname = Form("%s/version%d/normalizations_RN/%s_B%d_codB%d/", dbPath.Data(), _version, type, _gbatchPRODversion, _gbatchCODEversion); 
  if (_normRIG==2) dirname = Form("%s/version%d/normalizations_RS/%s_B%d_codB%d/", dbPath.Data(), _version, type, _gbatchPRODversion, _gbatchCODEversion); 
  if (_normRIG==-2) dirname = Form("%s/version%d/normalizations_RNS/%s_B%d_codB%d/", dbPath.Data(), _version, type, _gbatchPRODversion, _gbatchCODEversion); 
  if(_verbose>=2) cout<<"TrkCC::_LoadNormalization-I- looking in dir for norm files"<<dirname<<endl;

  for(int patt=0; patt<nTrkCCpatts; patt++){  
    _infile[patt]=NULL;  //init
  } 

  for(int patt=0; patt<nTrkCCpatts; patt++){  
    // Open file:
    TString filename;
    filename = Form("%s/graph%d.root", dirname.Data(), patt);
    if(_verbose>=2) cout<<"TrkCC::_LoadNormalization-I-  pattern="<<patt<<"  opening file name "<<filename<<endl;
    _infile[patt] = new TFile(filename);
    
    // check if file exists
    if (!_infile[patt] || _infile[patt]->IsZombie()) {
      PrintError(Form("TrkCC::_LoadNormalization-E-%s not found!", filename.Data()));
      if (_istolerant==0) {  // program will crash
	cerr << "TrkCC::_LoadNormalization-E-" << "program is exit(1) or chrashing now, " << filename << " not found!" << endl;
	exit(1);
	TH1F* h=NULL; // Force crashing
	h->Fill(10);
      }
      else {
	// Remember to put link of version B0_codB0 to last recommended version
	if (_normRIG==0) filename= Form("%s/version0/normalizations_E/%s_B0_codB0/graph%d.root", dbPath.Data(), type, patt);
	if (_normRIG==1) filename= Form("%s/version0/normalizations_R/%s_B0_codB0/graph%d.root", dbPath.Data(), type, patt);
	if (_normRIG==-1) filename= Form("%s/version0/normalizations_RN/%s_B0_codB0/graph%d.root", dbPath.Data(), type, patt);
	if (_normRIG==2) filename= Form("%s/version0/normalizations_RS/%s_B0_codB0/graph%d.root", dbPath.Data(), type, patt);
	if (_normRIG==-2) filename= Form("%s/version0/normalizations_RNS/%s_B0_codB0/graph%d.root", dbPath.Data(), type, patt);
	cerr<<"TrkCC::_LoadNormalization-I-  ( Required infile was not found && istolerant==1 ) => open default file: " << filename << endl;
	
	_infile[patt] = new TFile(filename);
	if (!_infile[patt] || _infile[patt]->IsZombie()) {
	  PrintError(Form("TrkCC::_LoadNormalization-E-Default file %s not found! exit(1)", filename.Data()));
	  exit(1);
	  TH1F* h=NULL; // Force Crashing 
	  h->Fill(10);
	}
      }
    }
    if (_verbose>0) {  
      cout << "TrkCC::_LoadNormalization-I- Loaded file:" << filename << endl;
    }
    if(patt==0) {
      _list=NULL;
      _list = (TTree*) _infile[0]->Get("vlist"); //hypothesis: all pattern contain the same list!
      iret = _LoadListaVars();  
    }
    // Get graphs from files
    for( int iv=0; iv<_nvars; iv++){
      //cout<<" Pointers "<<_gr[patt][iv][0]=NULL; 
      _gr[patt][iv][0]=NULL; 
      _gr[patt][iv][1]=NULL;  
      _gr[patt][iv][2]=NULL;  
      _gr[patt][iv][3]=NULL;  
      _gr[patt][iv][4]=NULL;
      _gr[patt][iv][0]= (TGraph*) _infile[patt]->Get(Form("graphTail_%d",iv*2));   //lower quantile, 10%
      //  cout << _gr[patt][iv][0]->Eval(120.) << " prova0 " << patt << " " << iv << endl;
      _gr[patt][iv][1]= (TGraph*) _infile[patt]->Get(Form("graph_%d",iv*2));       //lower quantile, 25.8%
      //      cout << Form("graph_%d",iv*2) << " NOME PROVA" << endl;
      //cout << _gr[patt][iv][1]->Eval(120.) << " prova1 " << patt << " " << iv << endl;
      _gr[patt][iv][2]= (TGraph*) _infile[patt]->Get(Form("graphMedian_%d",iv*2)); // median, 50%
      //cout << _gr[patt][iv][2]->Eval(120.) << " prova2 " << patt << " " << iv << Form(" name = graphMedian_%d",iv*2) << endl;
      _gr[patt][iv][3]= (TGraph*) _infile[patt]->Get(Form("graph_%d",iv*2+1));     // upper quantile, 74.2%
      //cout << _gr[patt][iv][3]->Eval(120.) << " prova3 " << patt << " " << iv << endl;
      _gr[patt][iv][4]= (TGraph*) _infile[patt]->Get(Form("graphTail_%d",iv*2+1)); //upper quantile, 90%
      //cout << _gr[patt][iv][4]->Eval(120.) << " prova4 " << patt << " " << iv << endl;
      //
    } 
    // cout<<" F pattern "<<patt<<" type? "<<type<<endl;  
  }
  
  return iret;
}



//-------------------------------------------------------------------------
//TrkCC::_LoadListaVars load the file with info about used variables and normalization
int TrkCC::_LoadListaVars() {

  if( _list->GetEntries()!=_nvars) {
    PrintError(Form("TrkCC::_LoadListaVars-E- Number of variables loaded from normalisation file (=%lld) is not the same as the coded number of variables (=%d)", _list->GetEntries(), _nvars));
    return -1;
  }
  Int_t vpos, islog;
  Char_t vname[100];
  Double_t fisflat, fnbins, flinint;
  Double_t hlow, hhigh,hloglow,hloghigh;
  _list->SetBranchAddress("vpos",&vpos);
  _list->SetBranchAddress("vname",&vname);
  _list->SetBranchAddress("islog",&islog);
  _list->SetBranchAddress("isflat",&fisflat);
  _list->SetBranchAddress("hlow",&hlow);
  _list->SetBranchAddress("hhigh",&hhigh);
  _list->SetBranchAddress("nbins",&fnbins);
  _list->SetBranchAddress("hloglow",&hloglow);
  _list->SetBranchAddress("hloghigh",&hloghigh);
  _list->SetBranchAddress("linint",&flinint);

  _nvarf=_list->GetEntries("islog<10");
  _nvari=_list->GetEntries("islog>=10");

  for( Int_t iv=0; iv<_nvarf+_nvari; iv++){
    _list->GetEntry(iv);
    algCC->_listamatr[iv][0]= vpos;
    algCC->_listamatr[iv][1]= islog;
    algCC->_listamatr[iv][2]= fisflat;
    algCC->_listamatr[iv][3]= hlow;
    algCC->_listamatr[iv][4]= hhigh;
    algCC->_listamatr[iv][5]= fnbins;
    algCC->_listamatr[iv][6]= hloglow;
    algCC->_listamatr[iv][7]= hloghigh;
    algCC->_listamatr[iv][8]= flinint;
  }
  return 0;
}

//-------------------------------------------------------------------------
//TrkCC::_LoadWeights Open files with weights for regression and BDT
int TrkCC::_LoadWeights(TString dbPath) {
  if (_verbose>=2) cout<<"TrkCC::_LoadWeights-I- begin"<<endl;

  int iret=0;

  // Define path
  TString weightsPath = Form("%s/version%d/weights/", dbPath.Data(), _version);
  if(_verbose>=2) cout<<"TrkCC::_LoadWeights-I- looking in dir for norm files"<<weightsPath.Data()<<endl;

  //Open weights
  iret = iret + algCC->LoadWeights(weightsPath);
  
  return iret;
}

//pay attention if return 0
float TrkCC::GetNormalizedVar(int varnumber, int isnormalized) {
  if (varnumber>=_nvars) {
    PrintError(Form("TrkCC::GetNormalizedVar()-E- ERROR! varnumber %d is larger of _nvars=%d", varnumber, _nvars));
    return 0;
  }
  if (isnormalized == 0) return algCC->_varlist[varnumber];
  return algCC->_varnorm[varnumber];
}

unsigned long long TrkCC::GetRecoMask(int maskingnumber){
  // return recommendmask bitwise recommended mask based on maskingnumber
  unsigned long long recommendmask=0xFFFFFFFFFFFFFULL;
  int errfull[nTrkCCVar];
  for (int ivvar=0; ivvar<nTrkCCVar; ivvar++){
      errfull[ivvar]=0;}

  if ((maskingnumber % 2)==0) { // no ecal
    errfull[16]=1;  // no axischi2
    errfull[66]=1;  // no energy
    errfull[25]=1;  // no ecal-trk_cofgx
    errfull[26]=1;  // no ecal-trk_dirx
    errfull[34]=1;  // no ecal-trk_cofgy
    errfull[35]=1;  // no ecal-trk_diry
  }

  if ((maskingnumber % 3)==0) { // no tof
    errfull[18]=1;  // no tofqd
    errfull[19]=1;  // no tofqmax
    errfull[22]=1;  // no late backsplash
    errfull[27]=1;  // no tof_chic
    errfull[28]=1;  // no tof_chit
    errfull[29]=1;  // no backsplash
    errfull[32]=1;  // no nbeta
    errfull[37]=1;  // no tof qu
    errfull[38]=1;  // no tof qrms
    errfull[39]=1;  // no tof nused
    errfull[40]=1;  // no beta
    errfull[57]=1;  // no num interactions
  }

  if ((maskingnumber % 5)==0) { // no trd
    errfull[36]=1;  // no trk-trd dir
    for (int iitd=44;iitd<=53;iitd++)     errfull[iitd]=1;  // no trd var
  }

  if ((maskingnumber % 7)==0) { // no rich
    errfull[20]=1;  // no rich unused
    errfull[23]=1;  // no rich ratio
    errfull[24]=1;  // no rich res
    errfull[33]=1;  // no rich nrings
    errfull[59]=1;  // no rich beta
    errfull[60]=1;  // no rich rms
  }

  // Transform variables when needed using option in tree list
  Int_t vpos, islog;
  for( int iv=0; iv<_nvars; iv++){
    vpos    = int(algCC->_listamatr[iv][0]);
    islog   = int(algCC->_listamatr[iv][1]);
    
    if (errfull[vpos]==1) {
      recommendmask &= ~((unsigned long long) 1 << iv); // unset the iv bit
      //cout << iv << " masked1" << endl;
    }
    if( islog==2 && vpos==0) { //log(E/P)  (iv=21)
      if (errfull[66]==1 || errfull[5]==1) {recommendmask &= ~((unsigned long long) 1 << iv); // unset the iv bit
	//cout << iv << " masked2" << endl;
      }
    }
    if( islog==2 && vpos==1) {//inner half Rigidity check (iv=2)
      if (errfull[13]==1 || errfull[14]==1) {recommendmask &= ~((unsigned long long) 1 << iv); // unset the iv bit
	//cout << iv << " masked3 " <<errfull[13]<<errfull[14]<< endl;
      }
    }
    if( islog==2 && vpos==2) {// top-bottom Rigidity check (iv=27) (only fullspan)
      if (errfull[11]==1 || errfull[12]==1 || algCC->_evpatt!=5) {recommendmask &= ~((unsigned long long) 1 << iv); // unset the iv bit
	//cout << iv << " masked4" << endl;
      }
    }
    if( islog==2 && vpos==3) {// inner-maxspan Rigidity check (iv=5) (no inner pattern)
      if (errfull[10]==1 || errfull[5]==1 || algCC->_evpatt<2) {recommendmask &= ~((unsigned long long) 1 << iv); // unset the iv bit
	//cout << iv << " masked5" << endl;
      }
    }
    if( islog==2 && vpos==4) { // choutko vs alcaratz Rigidity check (iv=28)
      if (errfull[5]==1 || errfull[6]==1) {recommendmask &= ~((unsigned long long) 1 << iv); // unset the iv bit
	//cout << iv << " masked6 " << errfull[5]<<errfull[6]<<endl;
      }
    }
    if( islog==2 && vpos==5) {//intcompl quasifolat from int (iv=38)           
      if (errfull[44]==1 || errfull[31]==1 || errfull[32]==1 || errfull[33]==1 || errfull[57]==1 || errfull[65]==1 || errfull[61]==1) { recommendmask &= ~((unsigned long long) 1 << iv); // unset the iv bit
	//cout << iv << " masked7" << endl;
      }
    }
    if( islog==2 && vpos==10) {// |1-E/P| (iv=22)
      if (errfull[66]==1 || errfull[5]==1) {recommendmask &= ~((unsigned long long) 1 << iv); // unset the iv bit
	//      cout << iv << " masked8" << endl;
      }
    }
  }  
  return recommendmask; 
}


unsigned long long TrkCC::_FillVarList(int maskingnumber){
  // fill the TrkCC data structure and normalize the variables
  // starting from the pre-filled human readable data structure TrkCCVar
  // also fill algCC->_evenergy with energy if energy>0 or with |Rigidity|
  // also fill algCC->_evpatt with event trk pattern
  // also return recommendmask bitwise recommended mask based on maskingnumber
  float varlistfull[nTrkCCVar];
  
  unsigned long long recommendmask=0xFFFFFFFFFFFFFULL;
  int errfull[nTrkCCVar];
  for (int ivvar=0; ivvar<nTrkCCVar; ivvar++){
    int ick = varCC->CheckVar(ivvar);
    if (_verbose>=1 && ick<0) cout << "TrkCC::_FillVarList variable cured" << ivvar << endl;
    if (ick<0) {varCC->Init(ivvar); // cure the problematic values
      errfull[ivvar]=1;}
    else {errfull[ivvar]=0;}
  } 

  if ((maskingnumber % 2)==0) { // no ecal
    errfull[16]=1;  // no axischi2
    errfull[66]=1;  // no energy
    errfull[25]=1;  // no ecal-trk_cofgx
    errfull[26]=1;  // no ecal-trk_dirx
    errfull[34]=1;  // no ecal-trk_cofgy
    errfull[35]=1;  // no ecal-trk_diry
  }

  if ((maskingnumber % 3)==0) { // no tof
    errfull[18]=1;  // no tofqd
    errfull[19]=1;  // no tofqmax
    errfull[22]=1;  // no late backsplash
    errfull[27]=1;  // no tof_chic
    errfull[28]=1;  // no tof_chit
    errfull[29]=1;  // no backsplash
    errfull[32]=1;  // no nbeta
    errfull[37]=1;  // no tof qu
    errfull[38]=1;  // no tof qrms
    errfull[39]=1;  // no tof nused
    errfull[40]=1;  // no beta
    errfull[57]=1;  // no num interactions
  }

  if ((maskingnumber % 5)==0) { // no trd
    errfull[36]=1;  // no trk-trd dir
    for (int iitd=44;iitd<=53;iitd++)     errfull[iitd]=1;  // no trd var
  }

  if ((maskingnumber % 7)==0) { // no rich
    errfull[20]=1;  // no rich unused
    errfull[23]=1;  // no rich ratio
    errfull[24]=1;  // no rich res
    errfull[33]=1;  // no rich nrings
    errfull[59]=1;  // no rich beta
    errfull[60]=1;  // no rich rms
  }

  varlistfull[0]=varCC->nmsk;
  varlistfull[1]=varCC->ly1;
  varlistfull[2]=varCC->ly2;
  varlistfull[3]=varCC->ly9;
  varlistfull[4]=varCC->pat;
  varlistfull[5]=varCC->rigidity_ch;
  varlistfull[6]=varCC->rigidity_al;
  varlistfull[7]=varCC->x0rad;
  varlistfull[8]=varCC->chix_ch;
  varlistfull[9]=varCC->chiy_ch;
  varlistfull[10]=varCC->rigidity_in;
  varlistfull[11]=varCC->rigidity_bt;
  varlistfull[12]=varCC->rigidity_tp;
  varlistfull[13]=varCC->rigidity_ibt;
  varlistfull[14]=varCC->rigidity_itp;
  varlistfull[15]=varCC->err_ch;
  varlistfull[16]=varCC->axischi2;
  varlistfull[17]=varCC->trk_ql9;
  varlistfull[18]=varCC->tof_qd;
  varlistfull[19]=varCC->tof_qmax;
  varlistfull[20]=varCC->rich_nunused;
  varlistfull[21]=varCC->pMDR;
  varlistfull[22]=varCC->nlbks; // late backsplash
  varlistfull[23]=varCC->rich_ratio;
  varlistfull[24]=varCC->rich_res;
  varlistfull[25]=varCC->trk_cofgx;
  varlistfull[26]=varCC->trk_dirx;
  varlistfull[27]=varCC->tof_chi2c;
  varlistfull[28]=varCC->tof_chi2t;
  varlistfull[29]=varCC->nbks;
  varlistfull[30]=varCC->ntrkcl9;
  varlistfull[31]=varCC->ntrk;
  varlistfull[32]=varCC->nbeta;
  varlistfull[33]=varCC->nrings;
  varlistfull[34]=varCC->trk_cofgy;
  varlistfull[35]=varCC->trk_diry;
  varlistfull[36]=varCC->trd_diry;
  varlistfull[37]=varCC->tof_qu;
  varlistfull[38]=varCC->tof_qrms;
  varlistfull[39]=varCC->tof_nused;
  varlistfull[40]=varCC->tof_beta;
  varlistfull[41]=varCC->trk_q;
  varlistfull[42]=varCC->trk_yrms;
  varlistfull[43]=varCC->trk_ql18;
  varlistfull[44]=varCC->trd_ntrk;
  varlistfull[45]=varCC->trd_nhit;
  varlistfull[46]=varCC->trd_ratio;
  varlistfull[47]=varCC->trd_ratioff;
  varlistfull[48]=varCC->trd_sumv;
  varlistfull[49]=varCC->trd_noff;
  varlistfull[50]=varCC->trd_path;
  varlistfull[51]=varCC->trd_mindist;
  varlistfull[52]=varCC->trd_q;   // charge of trdkcalib
  varlistfull[53]=varCC->trd_chi2;
  varlistfull[54]=varCC->trk_qrms;
  varlistfull[55]=varCC->trk_qmax;
  varlistfull[56]=varCC->ntrky5cl18;
  varlistfull[57]=varCC->nint;
  varlistfull[58]=varCC->dtrkavgy;
  varlistfull[59]=varCC->rich_beta;
  varlistfull[60]=varCC->rich_rms;
  varlistfull[61]=varCC->minoise1;
  varlistfull[62]=varCC->minoise2;
  varlistfull[63]=varCC->minoise3;
  varlistfull[64]=varCC->minoise4;
  varlistfull[65]=varCC->nlayer;
  varlistfull[66]=varCC->energy;
  varlistfull[67]=varCC->maxresx0;
  varlistfull[68]=varCC->maxresy0;
  varlistfull[69]=varCC->maxresx1;
  varlistfull[70]=varCC->maxresy1;
  varlistfull[71]=varCC->minresx;
  varlistfull[72]=varCC->minresy;

  // now set the algCC->_evenergy and algCC->_evpatt
  algCC->_evenergy=varCC->energy;
  if (abs(_normRIG)==1) algCC->_evenergy=fabs(varCC->rigidity_ch);
  if (abs(_normRIG)==2) algCC->_evenergy=fabs(varCC->rigidity_ch);
  if (algCC->_evenergy==0) algCC->_evenergy=fabs(varCC->rigidity_ch);
  algCC->_evpatt=varCC->pat;

  // Transform variables when needed using option in tree list
  Int_t vpos, islog;
  Double_t fisflat, fnbins, flinint;
  Double_t hlow, hhigh,hloglow,hloghigh;
  for( int iv=0; iv<_nvars; iv++){
    vpos    = int(algCC->_listamatr[iv][0]);
    islog   = int(algCC->_listamatr[iv][1]);
    fisflat = algCC->_listamatr[iv][2];
    hlow    = algCC->_listamatr[iv][3];
    hhigh   = algCC->_listamatr[iv][4];
    fnbins  = algCC->_listamatr[iv][5];
    hloglow = algCC->_listamatr[iv][6];
    hloghigh= algCC->_listamatr[iv][7];
    flinint = algCC->_listamatr[iv][8];

    //    cout << "listamatr" << iv << endl;
    //cout << algCC->_listamatr[iv][0] << " " << algCC->_listamatr[iv][1] << " " << algCC->_listamatr[iv][2] << " " << algCC->_listamatr[iv][3] << " " << algCC->_listamatr[iv][4] << " " << algCC->_listamatr[iv][5] << " " << algCC->_listamatr[iv][6] << " " << algCC->_listamatr[iv][7] << " " << algCC->_listamatr[iv][8] << endl;

    algCC->_varlist[iv]=fabs(varlistfull[vpos]);
    if (errfull[vpos]==1) {
      recommendmask &= ~((unsigned long long) 1 << iv); // unset the iv bit
      //cout << iv << " masked1" << endl;
}
    if( islog==1) algCC->_varlist[iv] = log10(fabs(varlistfull[vpos])+1.e-09);
    if( islog==2 && vpos==0) { //log(E/P)  (iv=21)
      algCC->_varlist[iv] =log10(fabs(varlistfull[66]/varlistfull[5]));
      if (errfull[66]==1 || errfull[5]==1) {recommendmask &= ~((unsigned long long) 1 << iv); // unset the iv bit
	//cout << iv << " masked2" << endl;
      }
    }
    if( islog==2 && vpos==1) {//inner half Rigidity check (iv=2)
      algCC->_varlist[iv] = log10(fabs((varlistfull[13]-varlistfull[14]+1.e-6)/(1.e-6+varlistfull[13]+varlistfull[14])));
      if (errfull[13]==1 || errfull[14]==1) {recommendmask &= ~((unsigned long long) 1 << iv); // unset the iv bit
	//cout << iv << " masked3 " <<errfull[13]<<errfull[14]<< endl;
      }
    }
    if( islog==2 && vpos==2) {// top-bottom Rigidity check (iv=27) (only fullspan)
      algCC->_varlist[iv] = log10(fabs((varlistfull[11]-varlistfull[12]+1.e-6)/(1.e-6+varlistfull[11]+varlistfull[12])));
      if (errfull[11]==1 || errfull[12]==1 || algCC->_evpatt!=5) {recommendmask &= ~((unsigned long long) 1 << iv); // unset the iv bit
	//cout << iv << " masked4" << endl;
      }
    }
    if( islog==2 && vpos==3) {// inner-maxspan Rigidity check (iv=5) (no inner pattern)
      algCC->_varlist[iv] = log10(fabs((varlistfull[10]-varlistfull[5]+1.e-6)/(1.e-6+varlistfull[10]+varlistfull[5])));
      if (errfull[10]==1 || errfull[5]==1 || algCC->_evpatt<2) {recommendmask &= ~((unsigned long long) 1 << iv); // unset the iv bit
	//cout << iv << " masked5" << endl;
      }
    }
    if( islog==2 && vpos==4) { // choutko vs alcaratz Rigidity check (iv=28)
      algCC->_varlist[iv] = log10(fabs((varlistfull[5]-varlistfull[6]+1.e-6)/(1.e-6+varlistfull[5]+varlistfull[6])));
      if (errfull[5]==1 || errfull[6]==1) {recommendmask &= ~((unsigned long long) 1 << iv); // unset the iv bit
	//cout << iv << " masked6 " << errfull[5]<<errfull[6]<<endl;
      }
    }
    if( islog==2 && vpos==5) {//intcompl quasifolat from int (iv=38)                                 
      algCC->_varlist[iv] = (varlistfull[44]+varlistfull[31]+varlistfull[32]+varlistfull[33]+varlistfull[57]-varlistfull[65]-varlistfull[61]);
      if (errfull[44]==1 || errfull[31]==1 || errfull[32]==1 || errfull[33]==1 || errfull[57]==1 || errfull[65]==1 || errfull[61]==1) { recommendmask &= ~((unsigned long long) 1 << iv); // unset the iv bit
	//cout << iv << " masked7" << endl;
      }
    }
    if( islog==2 && vpos==10) {// |1-E/P| (iv=22)
      algCC->_varlist[iv] =fabs(1.-fabs(varlistfull[66]/varlistfull[5]));
      if (errfull[66]==1 || errfull[5]==1) {recommendmask &= ~((unsigned long long) 1 << iv); // unset the iv bit
	//      cout << iv << " masked8" << endl;
      }
    }
    Int_t intint=0;
    if( islog==10) intint = int(varlistfull[vpos]);
    if( islog==11 && log10(fabs(varlistfull[vpos]))>fisflat) intint++;
    if( islog==11 && log10(fabs(varlistfull[vpos]))>hlow)    intint++;
    if( islog==11 && log10(fabs(varlistfull[vpos]))>hhigh)   intint++;
    if( islog==11 && log10(fabs(varlistfull[vpos]))>fnbins)  intint++;
    if( islog==11 && log10(fabs(varlistfull[vpos]))>hloglow) intint++;
    if( islog==11 && log10(fabs(varlistfull[vpos]))>hloghigh) intint++;
    if( islog==11 && log10(fabs(varlistfull[vpos]))>flinint) intint++;
    if (islog>=10) algCC->_varlist[iv]=(float)intint;
  }
   if(_verbose>=1 && recommendmask != 0xFFFFFFFFFFFFFULL) cout << maskingnumber << " masking | recommendmask unset " << std::hex << recommendmask << std::dec << endl;
  
  return recommendmask; 
}

//---------------------------------------------------------------------------------
// pay attention if return 0 
Double_t TrkCC::_NormBozNoz(Double_t x, Double_t loge, 
			   TGraph* glow1, TGraph* gupp1, TGraph *gmedian, TGraph* glow2, TGraph* gupp2, 
			   Int_t ilin, Bool_t cureTails)
{
  if (_verbose>=9) cout<<"TrkCC::_NormBozNoz()-I- begin"<<endl;

  Double_t norm=0.;
  if (ilin == 0){
    if (_verbose>=1 && glow1==NULL ) cerr<<"TrkCC::_NormBozNoz()-E-ERROR low graph is null "<<endl;
    if (_verbose>=1 && gupp1==NULL ) cerr<<"TrkCC::_NormBozNoz()-E-ERROR upp graph is null "<<endl;                                      
    Double_t lv = glow1->Eval( loge);
    Double_t uv = gupp1->Eval( loge);
    Double_t cv = (lv+uv)/2.;
    Double_t dlt = fabs(lv-uv)/2.;  // the sigma width
    if (dlt == 0) dlt = 1;
    norm = (x-cv)/dlt;  // number of sigma from center
  }
  else if (ilin == 1){
    if (_verbose>=1) {
      if (glow1==NULL) cerr<<"TrkCC::_normBozNoz()-E-ERROR low1 graph is null "<<endl;
      if (gupp1==NULL) cerr<<"TrkCC::_normBozNoz()-E-ERROR upp1 graph is null "<<endl;                                      
      if (gmedian==NULL) cerr<<"TrkCC::_normBozNoz()-E-ERROR median graph is null "<<endl;                                      
      if (glow2==NULL) cerr<<"TrkCC::_normBozNoz()-E-ERROR low2 graph is null "<<endl;
      if (gupp2==NULL) cerr<<"TrkCC::_normBozNoz()-E-ERROR upp2 graph is null "<<endl;
    }
    Double_t median  = gmedian->Eval( loge);
    Double_t xminus1 = glow1->Eval( loge);
    Double_t xplus1  = gupp1->Eval( loge);
    Double_t xminus2 = glow2->Eval( loge);
    Double_t xplus2  = gupp2->Eval( loge);
    Double_t delta;
    Double_t eta;
    Double_t lambda;
    
    // check parameters and cure if necessary
    _CureIfSlopeCrossing(loge, xminus2,xminus1,median,xplus1,xplus2, glow2,glow1,gmedian,gupp1,gupp2);
    Int_t ret = FindBozNozPar(delta,eta,lambda, xminus2,xminus1,median,xplus1,xplus2, 0.65,1.25,2);
   
    if(ret<0){
      norm = 0.; //was: -199.;
      PrintError(Form("TrkCC::normBozNoz()-E-ERROR! FindBozNoz returns a negative value ret=%d in pars:= %f %f %f %f %f, logE=%f, x=%f, %s", ret, xminus2, xminus1, median, xplus1, xplus2, loge, x, glow1->GetTitle()));  
    }
    else {
      if( cureTails==true && x<xminus2 ){
	Double_t epsilon=(fabs(xminus2-xminus1)/100.);
	norm = _ApplyTailLinearisation(delta,eta,lambda,median,x, xminus2, epsilon);
      } else if( cureTails==true && x>xplus2 ) {
	Double_t epsilon=fabs(xplus2-xplus1)/100.  ;
	norm = _ApplyTailLinearisation(delta,eta,lambda,median,x, xplus2 , epsilon);
      } else {
	norm= ApplyBozNoz(delta,eta,lambda,median , x);
      }
    }
  }
  if (_verbose>=9) cout<<"TrkCC::_NormBozNoz()-I- end"<<endl;
  return norm;
}

//-------------------------------------------------------------------------------------------
// TrkCC::_CureIfSlopeCrossing
// when loge>maxloge and extrapolation leads to wrong parameter ranking. Cure it here!
// Solution: use a mean slope of all graphs or the slope of median. We verified that the median alone behaves
// better than the mean of the slopes (one very large slope can lead to bad behaviours).
int TrkCC::_CureIfSlopeCrossing(Double_t loge, Double_t& xminus2,Double_t& xminus1, Double_t& median,
			 Double_t& xplus1,Double_t& xplus2,
			 TGraph* glow2, TGraph* glow1,TGraph* gmedian,TGraph* gupp1,TGraph* gupp2)
{
  // if (_verbose>=9) cout<<"TrkCC::_CureIfSlopeCrossing()-I- begin"<<endl;
  Int_t ret=0;
  if (xplus2<=xplus1)   ret= -3;
  if (xplus1<=median)   ret= -4;
  if (xminus1>=median)  ret= -5;
  if (xminus2>=xminus1) ret= -6;
  if (ret==0) return 0; //everything OK                                                                                                                              
  Int_t N = glow1->GetN();
  Double_t minloge,maxloge, yy;
  glow1->GetPoint(N-1,maxloge,yy);
  glow1->GetPoint(0,minloge,yy);

  if( loge<maxloge && loge>minloge ){
    //printf("[cureIfSlopeCrossing] ERROR in graphs! Order xminus2<xminus1<median<xlow1<xlow2 not satisfied. Loge=%f  Graphs %s\n,
    //       loge,glow1->GetTitle());
    return -99;
  }

  if (loge>=maxloge) {
    Double_t slopemedian =( gmedian->Eval(loge) - gmedian->Eval(maxloge) )/(loge-maxloge);
    Double_t meanSlope=slopemedian;
    // redefine values using mean slope
    xminus2 = (loge-maxloge)*meanSlope + glow2->Eval(maxloge);
    xminus1 = (loge-maxloge)*meanSlope + glow1->Eval(maxloge);
    median  = (loge-maxloge)*meanSlope + gmedian->Eval(maxloge);
    xplus1  = (loge-maxloge)*meanSlope + gupp1->Eval(maxloge);
    xplus2  = (loge-maxloge)*meanSlope + gupp2->Eval(maxloge);
  }

  if (loge<=minloge) {
    Double_t slopemedian =( gmedian->Eval(loge) - gmedian->Eval(minloge) )/(loge-minloge);
    Double_t meanSlope=slopemedian;
    // redefine values using mean slope                                                                                          
    xminus2 = (loge-minloge)*meanSlope + glow2->Eval(minloge);
    xminus1 = (loge-minloge)*meanSlope + glow1->Eval(minloge);
    median  = (loge-minloge)*meanSlope + gmedian->Eval(minloge);
    xplus1  = (loge-minloge)*meanSlope + gupp1->Eval(minloge);
    xplus2  = (loge-minloge)*meanSlope + gupp2->Eval(minloge);
  }

  // if (_verbose>=9) cout<<"TrkCC::_CureIfSlopeCrossing()-I- cured end"<<endl;
  return 0;
}

int TrkCC::FindBozNozPar(Double_t& delta, Double_t& eta, Double_t& lambda,
			  Double_t xminus2, Double_t xminus1, Double_t mediana, Double_t xplus1, Double_t xplus2,
			  Double_t pval1, Double_t pval2, Int_t whichsymmetrize12){
  if (_verbose>=9) cout << " [FindBozNozPar] begin" << endl;
  Int_t ret=0;
  if (pval1 <= 0 || pval2 <=0) return -1;
  if (pval1>=pval2)     return -2;
  if (xplus2<=xplus1)   return -3;
  if (xplus1<=mediana)  return -4;
  if (xminus1>=mediana) return -5;
  if (xminus2>=xminus1) return -6;
  if (whichsymmetrize12 != 2 && whichsymmetrize12!= 1) whichsymmetrize12 = 2;
  if (_verbose>=99) cout << " [FindBozNozPar] qui ci arrivo" << endl;
  Double_t mealyplus = xplus2;
  Double_t mealyminus = xminus2;
  Double_t nozplus = xplus1;
  Double_t nozminus = xminus1;
  Double_t pvalN = pval1;
  Double_t pvalM = pval2;
  if (whichsymmetrize12 == 1){
    mealyplus = xplus1;
    mealyminus = xminus1;
    nozplus = xplus2;
    nozminus = xminus2;
    pvalN = pval2;
    pvalM = pval1;
  }
  ret = 0;
  ret = ret + _ManlyPar(lambda, mealyminus, mealyplus, mediana);
  Double_t qvalM = _Manly(lambda,mealyplus-mediana);
  Double_t qvalN = 0.5*(_Manly(lambda,nozplus-mediana)- _Manly(lambda,nozminus-mediana));
  ret = ret + _NozPar(eta,qvalN,qvalM,pvalN,pvalM);
  Double_t mmm = _Manly(lambda,mealyplus-mediana);
  Double_t nnn = _Nozly(eta,mmm);
  delta = pvalM/nnn;
  if (_verbose>=9) cout << " [FindBozNozPar] end" << endl;
  return ret;
}

Double_t TrkCC::_Manly(Double_t lambda, Double_t x){
  if (lambda == 0) return x;
  Double_t T = (exp(lambda*x)-1.)/lambda;
  return T;
}

Double_t  TrkCC::_Findc(Double_t a,Double_t b,Double_t& ex){
  // found the root of exp(a/x)+exp(-b/x)-2=0  (b>a>0)             
  // ex is the target relative error on x determination            

  if (b<0 || a<0 || a>b) {ex=1000.; return 0.;}
  Double_t xright;
  Double_t xleft;
  Double_t y=1;
  Double_t x = a;
  while (y>0){
    xleft = x;
    x = 2.*x;
    y = exp(a/x)+exp(-b/x)-2.;
  }
  //  if (_verbose>=99) cout << " [FindC] qua ok" << x << "=x | a= " << a << " b= " << b << endl;
  xright = x;
  Double_t goal = fabs(ex);   
  if (goal >0.1) goal = 0.1;
  ex = fabs((xleft-xright)/(xleft+xright));
  int n=0;
  while (ex>goal && y != 0){               
    n++;
    if (_verbose>=99) cout << n << " [Findc]" << xleft << " | " << xright << " y= " << y << " ex = " << ex << "goal" << goal<<endl;
    x = (xleft+xright)/2.;
    y = exp(a/x)+exp(-b/x)-2.;
    if(y<=0) {xright=x;}
    else { xleft=x;}
    ex = fabs((xleft-xright)/(xleft+xright));
  }
  return x;
}

Int_t  TrkCC::_ManlyPar(Double_t& lambda, Double_t xminus, Double_t xplus, Double_t mediana){
  if (_verbose>=9) cout << " [ManlyPar] begin" << endl;
  // return LAMBDA of a mealy transformation that symmetrize xminus and xplus
  // respect to the median
  // return 0 = OK
  // return <0 = problems
  lambda = 0;
  Double_t x1 = xplus - mediana;
  Double_t x2 = -(xminus - mediana);
  if (x1<=0 || x2<=0) return -1;
  if (x1 == x2) return 0;
  Double_t a = x1;
  Double_t b = x2;
  Double_t csign = 1;
  if (a>b) {
    b = x1;
    a = x2;
    csign = -1;
  }
  Double_t ey = 1.e-9;
  if (_verbose>=99) cout << " [ManlyPar] prefindc" << endl;
  Double_t c = _Findc(a,b,ey);
  if (_verbose>=99) cout << " [ManlyPar] postfindc" << endl;
  if (c<=0) return -2;
  lambda = csign/c;
  if (_verbose>=9) cout << " [ManlyPar] end" << endl;
  return 0;
}

Int_t TrkCC::_NozPar(Double_t& eta, Double_t qval2, Double_t qval1, Double_t pval2, Double_t pval1){
  // find eta putting noztrasf(qval2)/noztrasf(qval1) = pval2/pval1
  // return 0 OK
  // return <0 error
  if (_verbose>=9) cout << " [NozPar] begin" << endl;
  Double_t ex = 1.e-6; // relative error on eta
  eta = 0;
  if (pval1 <= 0 || pval2 <= 0 || qval1 <= 0 || qval2 <= 0) return -1;
  if (pval1==pval2 || qval1==qval2) return -2;
  if (pval1<pval2 && qval1>qval2) return -3;
  if (pval1>pval2 && qval1<qval2) return -3;
  
  if (pval1/pval2 == qval1/qval2) return 0;  // flat trasf
  
  Double_t pmax = pval2;
  Double_t pmin = pval1;
  Double_t qmax = qval2;
  Double_t qmin = qval1;
  if (pmax < pmin){
    pmax = pval1;
    pmin = pval2;
    qmax = qval1;
    qmin = qval2;
  }  
  if (qmax/qmin < pmax/pmin) 
    {
      // relatively simple transformation
      Double_t xright;
      Double_t xleft;
      Double_t y=-1;
      Double_t x = 0;
      Double_t cost = pmax/pmin;
      while (y<0){
	xleft = x;
	x = 2.*x+1.;
	//	y = (exp(x*qmax)-exp(-x*qmax))/(exp(x*qmin)-exp(-x*qmin))-cost; //results in  NAN if X is large
	y = (exp(x*(qmax-qmin))-exp(-x*(qmax+qmin)))/(1.-exp(-2.*x*qmin))-cost;
      }
      xright = x;
      Double_t goal = fabs(ex);
      if (goal >0.1) goal = 0.1;
      ex = fabs((xleft-xright)/(xleft+xright));
      while (ex>goal && y!=0){
	x = (xleft+xright)/2.;
	// y = (exp(x*qmax)-exp(-x*qmax))/(exp(x*qmin)-exp(-x*qmin))-cost; // results in NAN for X large
	y = (exp(x*(qmax-qmin))-exp(-x*(qmax+qmin)))/(1.-exp(-2.*x*qmin))-cost;
	if(y>=0) {xright=x;}
	else { xleft=x;}
	ex = fabs((xleft-xright)/(xleft+xright));
      }
      eta =  x;
    } 
  else 
    { // logarithmic case
      Double_t xright;
      Double_t xleft;
      Double_t y=1;
      Double_t x = 0;
      Double_t cost = pmax/pmin;
      while (y>0){
	xleft = x;
	x = 2.*x+1.;
	y = (log(x*qmax+1)/log(x*qmin+1))-cost;
      }
      xright = x;
      Double_t goal = fabs(ex);
      if (goal >0.1) goal = 0.1;
      ex = fabs((xleft-xright)/(xleft+xright));
      while (ex>goal && y!=0){
	x = (xleft+xright)/2.;
	//    cout << xright << " " << xleft << " ym = " << y << endl;
	y = (log(x*qmax+1)/log(x*qmin+1))-cost;
	if(y<=0) {xright=x;}
	else { xleft=x;}
	ex = fabs((xleft-xright)/(xleft+xright));
      }
      eta =  -x;
    }      
  if (_verbose>=9) cout << " [NozPar] end" << endl;
  return 0;
}

Double_t TrkCC::_Nozly(Double_t eta, Double_t x)
{
  if (eta == 0) return x;
  if (x == 0) return x;
  Double_t T ;
  if (eta >0){
    T = (exp(eta*x)-exp(-eta*x))/(2.*eta);
  }  else {
    T = (x/fabs(eta*x))*log(fabs(eta*x)+1); 
  }
  return T;
}


// pay attention if return 0 
Double_t TrkCC::_ApplyTailLinearisation(Double_t delta, Double_t eta, Double_t lambda, Double_t mediana, Double_t x, Double_t xtreme, Double_t epsilon){
  // ( xtreme=xplus2 if x>xplus2 ) || (xtreme=xminus2) if x<xminus2 )
  // epsilon (suggestion) use a funxtion of xplus1-xplus2 or xminus1-xminus2
  Double_t norm=0.;
  if( x<xtreme || x>xtreme ){   
    Double_t vn1 = ApplyBozNoz(delta,eta,lambda,mediana, xtreme-epsilon ); 
    Double_t vn2 = ApplyBozNoz(delta,eta,lambda,mediana, xtreme+epsilon );
    Double_t slope =  (vn2-vn1)/(2*epsilon);
    norm = (x-xtreme)*slope + ApplyBozNoz(delta,eta,lambda,mediana, xtreme);
  }
  else {
    PrintError("TrkCC::_ApplyTailLinearisation()-E- some not allowed parameters. Should not happen.");
  }
  return norm;
}

// Returns x after a Boz-Noz transformation usings parameters delta,eta,lambda,median
Double_t TrkCC::ApplyBozNoz(Double_t delta, Double_t eta, Double_t lambda, Double_t mediana, Double_t x) {
  return delta*_Nozly(eta,_Manly(lambda,x-mediana)); 
}

double TrkCC::GetEvaluator() {
  return algCC->GetEvaluator();
}

TrkCCVar* TrkCCVar::gethead() {
  if (!_head) {
    printf("TrkCCVar::gethead()-M-CreatingObjectTrkCCVarAsSingleton\n");
    _head = new TrkCCVar();
  }
  return _head;
}

TrkCCVar::TrkCCVar(){
  run = 0;
  evtno = 0;
  ppno = 0;
  Init();
}

TrkCCVar::~TrkCCVar(){
  _head=0;
}

void TrkCCVar::Init(int varnumber){
  if(varnumber<0 || varnumber==0) nmsk=0;
  if(varnumber<0 || varnumber==1) ly1=9999;
  if(varnumber<0 || varnumber==2) ly2=9999;
  if(varnumber<0 || varnumber==3) ly9=9999;
  if(varnumber<0 || varnumber==4) pat=9999;
  if(varnumber<0 || varnumber==5) rigidity_ch=0;
  if(varnumber<0 || varnumber==6) rigidity_al=0;
  if(varnumber<0 || varnumber==7) x0rad=0;
  if(varnumber<0 || varnumber==8) chix_ch=0;
  if(varnumber<0 || varnumber==9) chiy_ch=0;
  if(varnumber<0 || varnumber==10) rigidity_in=0;
  if(varnumber<0 || varnumber==11) rigidity_bt=0;
  if(varnumber<0 || varnumber==12) rigidity_tp=0;
  if(varnumber<0 || varnumber==13) rigidity_ibt=0;
  if(varnumber<0 || varnumber==14) rigidity_itp=0;
  if(varnumber<0 || varnumber==15) err_ch=0;
  if(varnumber<0 || varnumber==16) axischi2=0;
  if(varnumber<0 || varnumber==17) trk_ql9=1;
  if(varnumber<0 || varnumber==18) tof_qd=1;
  if(varnumber<0 || varnumber==19) tof_qmax=1;
  if(varnumber<0 || varnumber==20) rich_nunused=0;
  if(varnumber<0 || varnumber==21) pMDR=9999;
  if(varnumber<0 || varnumber==22) nlbks=0; 
  if(varnumber<0 || varnumber==23) rich_ratio=1;
  if(varnumber<0 || varnumber==24) rich_res=0;
  if(varnumber<0 || varnumber==25) trk_cofgx=0;
  if(varnumber<0 || varnumber==26) trk_dirx=0;
  if(varnumber<0 || varnumber==27) tof_chi2c=0;
  if(varnumber<0 || varnumber==28) tof_chi2t=0;
  if(varnumber<0 || varnumber==29) nbks=0;
  if(varnumber<0 || varnumber==30) ntrkcl9=1;
  if(varnumber<0 || varnumber==31) ntrk=1;
  if(varnumber<0 || varnumber==32) nbeta=1;
  if(varnumber<0 || varnumber==33) nrings=1;
  if(varnumber<0 || varnumber==34) trk_cofgy=0;
  if(varnumber<0 || varnumber==35) trk_diry=0;
  if(varnumber<0 || varnumber==36) trd_diry=0;
  if(varnumber<0 || varnumber==37) tof_qu=1;
  if(varnumber<0 || varnumber==38) tof_qrms=0;
  if(varnumber<0 || varnumber==39) tof_nused=0;
  if(varnumber<0 || varnumber==40) tof_beta=1;
  if(varnumber<0 || varnumber==41) trk_q=1;
  if(varnumber<0 || varnumber==42) trk_yrms=0;
  if(varnumber<0 || varnumber==43) trk_ql18=1;
  if(varnumber<0 || varnumber==44) trd_ntrk=1;
  if(varnumber<0 || varnumber==45) trd_nhit=20;
  if(varnumber<0 || varnumber==46) trd_ratio=1;
  if(varnumber<0 || varnumber==47) trd_ratioff=1;
  if(varnumber<0 || varnumber==48) trd_sumv=0;
  if(varnumber<0 || varnumber==49) trd_noff=0;
  if(varnumber<0 || varnumber==50) trd_path=0;
  if(varnumber<0 || varnumber==51) trd_mindist=0;
  if(varnumber<0 || varnumber==52) trd_q=1;   
  if(varnumber<0 || varnumber==53) trd_chi2=0;
  if(varnumber<0 || varnumber==54) trk_qrms=0;
  if(varnumber<0 || varnumber==55) trk_qmax=1;
  if(varnumber<0 || varnumber==56) ntrky5cl18=1;
  if(varnumber<0 || varnumber==57) nint=0;
  if(varnumber<0 || varnumber==58) dtrkavgy=0;
  if(varnumber<0 || varnumber==59) rich_beta=1;
  if(varnumber<0 || varnumber==60) rich_rms=0;
  if(varnumber<0 || varnumber==61) minoise1=0;
  if(varnumber<0 || varnumber==62) minoise2=0;
  if(varnumber<0 || varnumber==63) minoise3=0;
  if(varnumber<0 || varnumber==64) minoise4=0;
  if(varnumber<0 || varnumber==65) nlayer=7;
  if(varnumber<0 || varnumber==66) energy=0;
  if(varnumber<0 || varnumber==67) maxresx0=0;
  if(varnumber<0 || varnumber==68) maxresy0=0;
  if(varnumber<0 || varnumber==69) maxresx1=0;
  if(varnumber<0 || varnumber==70) maxresy1=0;
  if(varnumber<0 || varnumber==71) minresx=0;
  if(varnumber<0 || varnumber==72) minresy=0;
  return;
}

// return 0 is ok  return -1 problem;
int TrkCCVar::CheckVar(int varnumber){
  if(varnumber==0 && !(fabs(nmsk)<1.e20)) return -1;
  if(varnumber==1 && !(fabs(ly1)<1.e20)) return -1;
  if(varnumber==2 && !(fabs(ly2)<1.e20)) return -1;
  if(varnumber==3 && !(fabs(ly9)<1.e20)) return -1;
  if(varnumber==4 && !(fabs(pat)<1.e20)) return -1;
  if(varnumber==5 && !(fabs(rigidity_ch)<1.e20)) return -1;
  if(varnumber==6 && !(fabs(rigidity_al)<1.e20)) return -1;
  if(varnumber==7 && !(fabs(x0rad)<1.e20)) return -1;
  if(varnumber==8 && !(fabs(chix_ch)<1.e20)) return -1;
  if(varnumber==9 && !(fabs(chiy_ch)<1.e20)) return -1;
  if(varnumber==10 && !(fabs(rigidity_in)<1.e20)) return -1;
  if(varnumber==11 && !(fabs(rigidity_bt)<1.e20)) return -1;
  if(varnumber==12 && !(fabs(rigidity_tp)<1.e20)) return -1;
  if(varnumber==13 && !(fabs(rigidity_ibt)<1.e20)) return -1;
  if(varnumber==14 && !(fabs(rigidity_itp)<1.e20)) return -1;
  if(varnumber==15 && !(fabs(err_ch)<1.e20)) return -1;
  if(varnumber==16 && !(fabs(axischi2)<1.e20)) return -1;
  if(varnumber==17 && !(fabs(trk_ql9)<1.e20)) return -1;
  if(varnumber==18 && !(fabs(tof_qd)<1.e20)) return -1;
  if(varnumber==19 && !(fabs(tof_qmax)<1.e20)) return -1;
  if(varnumber==20 && !(fabs(rich_nunused)<1.e20)) return -1;
  if(varnumber==21 && !(fabs(pMDR)<1.e20)) return -1;
  if(varnumber==22 && !(fabs(nlbks)<1.e20)) return -1;
  if(varnumber==23 && !(fabs(rich_ratio)<1.e20)) return -1;
  if(varnumber==24 && !(fabs(rich_res)<1.e20)) return -1;
  if(varnumber==25 && !(fabs(trk_cofgx)<1.e20)) return -1;
  if(varnumber==26 && !(fabs(trk_dirx)<1.e20)) return -1;
  if(varnumber==27 && !(fabs(tof_chi2c)<1.e20)) return -1;
  if(varnumber==28 && !(fabs(tof_chi2t)<1.e20)) return -1;
  if(varnumber==29 && !(fabs(nbks)<1.e20)) return -1;
  if(varnumber==30 && !(fabs(ntrkcl9)<1.e20)) return -1;
  if(varnumber==31 && !(fabs(ntrk)<1.e20)) return -1;
  if(varnumber==32 && !(fabs(nbeta)<1.e20)) return -1;
  if(varnumber==33 && !(fabs(nrings)<1.e20)) return -1;
  if(varnumber==34 && !(fabs(trk_cofgy)<1.e20)) return -1;
  if(varnumber==35 && !(fabs(trk_diry)<1.e20)) return -1;
  if(varnumber==36 && !(fabs(trd_diry)<1.e20)) return -1;
  if(varnumber==37 && !(fabs(tof_qu)<1.e20)) return -1;
  if(varnumber==38 && !(fabs(tof_qrms)<1.e20)) return -1;
  if(varnumber==39 && !(fabs(tof_nused)<1.e20)) return -1;
  if(varnumber==40 && !(fabs(tof_beta)<1.e20)) return -1;
  if(varnumber==41 && !(fabs(trk_q)<1.e20)) return -1;
  if(varnumber==42 && !(fabs(trk_yrms)<1.e20)) return -1;
  if(varnumber==43 && !(fabs(trk_ql18)<1.e20)) return -1;
  if(varnumber==44 && !(fabs(trd_ntrk)<1.e20)) return -1;
  if(varnumber==45 && !(fabs(trd_nhit)<1.e20)) return -1;
  if(varnumber==46 && !(fabs(trd_ratio)<1.e20)) return -1;
  if(varnumber==47 && !(fabs(trd_ratioff)<1.e20)) return -1;
  if(varnumber==48 && !(fabs(trd_sumv)<1.e20)) return -1;
  if(varnumber==49 && !(fabs(trd_noff)<1.e20)) return -1;
  if(varnumber==50 && !(fabs(trd_path)<1.e20)) return -1;
  if(varnumber==51 && !(fabs(trd_mindist)<1.e20)) return -1;
  if(varnumber==52 && !(fabs(trd_q)<1.e20)) return -1;
  if(varnumber==53 && !(fabs(trd_chi2)<1.e20)) return -1;
  if(varnumber==54 && !(fabs(trk_qrms)<1.e20)) return -1;
  if(varnumber==55 && !(fabs(trk_qmax)<1.e20)) return -1;
  if(varnumber==56 && !(fabs(ntrky5cl18)<1.e20)) return -1;
  if(varnumber==57 && !(fabs(nint)<1.e20)) return -1;
  if(varnumber==58 && !(fabs(dtrkavgy)<1.e20)) return -1;
  if(varnumber==59 && !(fabs(rich_beta)<1.e20)) return -1;
  if(varnumber==60 && !(fabs(rich_rms)<1.e20)) return -1;
  if(varnumber==61 && !(fabs(minoise1)<1.e20)) return -1;
  if(varnumber==62 && !(fabs(minoise2)<1.e20)) return -1;
  if(varnumber==63 && !(fabs(minoise3)<1.e20)) return -1;
  if(varnumber==64 && !(fabs(minoise4)<1.e20)) return -1;
  if(varnumber==65 && !(fabs(nlayer)<1.e20)) return -1;
  if(varnumber==66 && !(fabs(energy)<1.e20)) return -1;
  if(varnumber==67 && !(fabs(maxresx0)<1.e20)) return -1;
  if(varnumber==68 && !(fabs(maxresy0)<1.e20)) return -1;
  if(varnumber==69 && !(fabs(maxresx1)<1.e20)) return -1;
  if(varnumber==70 && !(fabs(maxresy1)<1.e20)) return -1;
  if(varnumber==71 && !(fabs(minresx)<1.e20)) return -1;
  if(varnumber==72 && !(fabs(minresy)<1.e20))  return -1; 
  return 0;
}


void TrkCCVar::FillFromVector(float varlistfull[nTrkCCVar]){
  nmsk= int(varlistfull[0]);
  ly1=  int(varlistfull[1]);
  ly2=  int(varlistfull[2]);
  ly9=  int(varlistfull[3]);
  pat=  int(varlistfull[4]);
  rigidity_ch= varlistfull[5];
  rigidity_al=  varlistfull[6];
  x0rad=    varlistfull[7];
  chix_ch=  varlistfull[8];
  chiy_ch=  varlistfull[9];
  rigidity_in=  varlistfull[10];
  rigidity_bt=  varlistfull[11];
  rigidity_tp=  varlistfull[12];
  rigidity_ibt=  varlistfull[13];
  rigidity_itp=  varlistfull[14];
  err_ch=  varlistfull[15];
  axischi2= varlistfull[16];
  trk_ql9= varlistfull[17];
  tof_qd=  varlistfull[18];
  tof_qmax=  varlistfull[19];
  rich_nunused=  int(varlistfull[20]);
  pMDR=  varlistfull[21];
  nlbks= int(varlistfull[22]);
  rich_ratio=  varlistfull[23];
  rich_res=  varlistfull[24];
  trk_cofgx=  varlistfull[25];
  trk_dirx=  varlistfull[26];
  tof_chi2c=  varlistfull[27];
  tof_chi2t=  varlistfull[28];
  nbks=  int(varlistfull[29]);
  ntrkcl9=  int(varlistfull[30]);
  ntrk=  int(varlistfull[31]);
  nbeta=  int(varlistfull[32]);
  nrings=  int(varlistfull[33]);
  trk_cofgy=   varlistfull[34];
  trk_diry= varlistfull[35];
  trd_diry=  varlistfull[36];
  tof_qu=  varlistfull[37];
  tof_qrms=  varlistfull[38];
  tof_nused=  int(varlistfull[39]);
  tof_beta=  varlistfull[40];
  trk_q=  varlistfull[41];
  trk_yrms=  varlistfull[42];
  trk_ql18=  varlistfull[43];
  trd_ntrk=  int(varlistfull[44]);
  trd_nhit=  int(varlistfull[45]);
  trd_ratio=  varlistfull[46];
  trd_ratioff=  varlistfull[47];
  trd_sumv=  varlistfull[48];
  trd_noff=  int(varlistfull[49]);
  trd_path=  varlistfull[50];
  trd_mindist=  varlistfull[51];
  trd_q=  varlistfull[52];
  trd_chi2=  varlistfull[53];
  trk_qrms=  varlistfull[54];
  trk_qmax=  varlistfull[55];
  ntrky5cl18=  int(varlistfull[56]);
  nint=  int(varlistfull[57]);
  dtrkavgy=  varlistfull[58];
  rich_beta=  varlistfull[59];
  rich_rms=  varlistfull[60];
  minoise1=  varlistfull[61];
  minoise2=  varlistfull[62];
  minoise3=  varlistfull[63];
  minoise4=  varlistfull[64];
  nlayer=  int(varlistfull[65]);
  energy=  varlistfull[66];
  maxresx0=  varlistfull[67];
  maxresy0=  varlistfull[68];
  maxresx1=  varlistfull[69];
  maxresy1=  varlistfull[70];
  minresx=  varlistfull[71];
  minresy=  varlistfull[72];  
  return;
}

float TrkCCVar::GetFullVectorElement(int varnumber){
  if( varnumber==0) return (float) nmsk;
  if( varnumber==1) return (float) ly1;
  if( varnumber==2) return (float) ly2;
  if( varnumber==3) return (float) ly9;
  if( varnumber==4) return (float) pat;
  if( varnumber==5) return (float) rigidity_ch;
  if( varnumber==6) return (float) rigidity_al;
  if( varnumber==7) return (float) x0rad;
  if( varnumber==8) return (float) chix_ch;
  if( varnumber==9) return (float) chiy_ch;
  if( varnumber==10) return (float) rigidity_in;
  if( varnumber==11) return (float) rigidity_bt;
  if( varnumber==12) return (float) rigidity_tp;
  if( varnumber==13) return (float) rigidity_ibt;
  if( varnumber==14) return (float) rigidity_itp;
  if( varnumber==15) return (float) err_ch;
  if( varnumber==16) return (float) axischi2;
  if( varnumber==17) return (float) trk_ql9;
  if( varnumber==18) return (float) tof_qd;
  if( varnumber==19) return (float) tof_qmax;
  if( varnumber==20) return (float) rich_nunused;
  if( varnumber==21) return (float) pMDR;
  if( varnumber==22) return (float) nlbks;
  if( varnumber==23) return (float) rich_ratio;
  if( varnumber==24) return (float) rich_res;
  if( varnumber==25) return (float) trk_cofgx;
  if( varnumber==26) return (float) trk_dirx;
  if( varnumber==27) return (float) tof_chi2c;
  if( varnumber==28) return (float) tof_chi2t;
  if( varnumber==29) return (float) nbks;
  if( varnumber==30) return (float) ntrkcl9;
  if( varnumber==31) return (float) ntrk;
  if( varnumber==32) return (float) nbeta;
  if( varnumber==33) return (float) nrings;
  if( varnumber==34) return (float) trk_cofgy;
  if( varnumber==35) return (float) trk_diry;
  if( varnumber==36) return (float) trd_diry;
  if( varnumber==37) return (float) tof_qu;
  if( varnumber==38) return (float) tof_qrms;
  if( varnumber==39) return (float) tof_nused;
  if( varnumber==40) return (float) tof_beta;
  if( varnumber==41) return (float) trk_q;
  if( varnumber==42) return (float) trk_yrms;
  if( varnumber==43) return (float) trk_ql18;
  if( varnumber==44) return (float) trd_ntrk;
  if( varnumber==45) return (float) trd_nhit;
  if( varnumber==46) return (float) trd_ratio;
  if( varnumber==47) return (float) trd_ratioff;
  if( varnumber==48) return (float) trd_sumv;
  if( varnumber==49) return (float) trd_noff;
  if( varnumber==50) return (float) trd_path;
  if( varnumber==51) return (float) trd_mindist;
  if( varnumber==52) return (float) trd_q;
  if( varnumber==53) return (float) trd_chi2;
  if( varnumber==54) return (float) trk_qrms;
  if( varnumber==55) return (float) trk_qmax;
  if( varnumber==56) return (float) ntrky5cl18;
  if( varnumber==57) return (float) nint;
  if( varnumber==58) return (float) dtrkavgy;
  if( varnumber==59) return (float) rich_beta;
  if( varnumber==60) return (float) rich_rms;
  if( varnumber==61) return (float) minoise1;
  if( varnumber==62) return (float) minoise2;
  if( varnumber==63) return (float) minoise3;
  if( varnumber==64) return (float) minoise4;
  if( varnumber==65) return (float) nlayer;
  if( varnumber==66) return (float) energy;
  if( varnumber==67) return (float) maxresx0;
  if( varnumber==68) return (float) maxresy0;
  if( varnumber==69) return (float) maxresx1;
  if( varnumber==70) return (float) maxresy1;
  if( varnumber==71) return (float) minresx;
  if( varnumber==72) return (float) minresy;
  cerr << "TrkCCVar::GetFullVectorElement value out of range " << varnumber << endl;
  return 0;
}

#ifndef _NOGBATCH_
int TrkCCVar::FillFromGbatch(ParticleR* part, int rebuildRefit, int gbatchPRODversion, bool fullProcess){
  // fill the TrkCCVar data structure from gbatch  
  // part is the pre-selected ParticleR pointer  
  // abs(rebuildRefit)=iTrTrackPar 3rd parameter (0 = stored, 1 refit if necessary, 2 force trk refit, 3 fit and rebuild also Coo)
  //  rebuildRefit <0 force rebuild of betah and anticluster
  //  gbatchPRODversion if >0 and <900 force the Bfield Temperature correction
  // fullProcess=false (only partial filling, fast)  fullprocess=true (slow)
  // return -2 = event problem
  // return -1 = particle problem
  // return 0 = trk problem                    
  // return 1 = ok                             
  // return 2*n = ecal problem                 
  // return 3*n = tof problem            
  // return 5*n = trd problem            
  // return 7*n = rich problem
  
  // cout<<  "start FillFromGbatch" <<endl;            

  AMSEventR* evt=AMSEventR::Head();
  int iret = 1;
  nmsk = -2;
  if (!evt) return -2;
  ntrk = (evt->nTrTrack());
  //  ParticleR *part = evt->pParticle(nptc);
  nmsk = -1;
  if (!part) return -1;
  //  printf("%p\n", part);//only for debug
  TrTrackR *trk = part->pTrTrack();
  nmsk = 0;
  if (!trk) return 0;

  // B-Field Correction
  double btcor = 1;
  MCEventgR *pmc = evt->pMCEventg(0);

  int ifit = abs(rebuildRefit);
  int ifitspecial = ifit;
  if (gbatchPRODversion==950 && !pmc) ifitspecial = 21; //sada recipe for pass6 fit span!=0,3,5,6,7


  if (!pmc && gbatchPRODversion!=0 && gbatchPRODversion<900) { // is already inserted in pass6
    float bcor1, bcor2;
    int bret1= MagnetVarp::btempcor(bcor1, 0, 1);
    int bret2= MagnetVarp::btempcor(bcor2, 0, 2);
    if (bret1==0 && bret2==0) btcor = (bcor1+bcor2)/2;                                                        
    else if (bret1!=0 && bret2 == 0) btcor = bcor2;                                                           
  }


  int btp = trk->GetBitPatternJ();
  int idd = trk->iTrTrackPar(1, 0, ifit);
  if( idd < 0) return 0;
  rigidity_ch = btcor*trk->GetRigidity(idd);
  chix_ch   = trk->GetNormChisqX(idd);
  chiy_ch   = trk->GetNormChisqY(idd);
  
  // radiation length @ Z=0 computation
  AMSPoint trk0;
  AMSDir dtrk0;
  trk->Interpolate(0., trk0, dtrk0, idd);
  x0rad=evt->GetRadiationLength(trk0,dtrk0,rigidity_ch/sqrt(fabs(btcor)),200.,0.); //BTCOR truth is in the middle?    
                                                           
  err_ch = 0.;
  int  iddc = trk->iTrTrackPar(21,0,ifit); // error same weight            
  if(fullProcess) {if (iddc>=0) err_ch = trk->GetErrRinv(iddc);} // error same weight     
  rigidity_al = 0;
  iddc = trk->iTrTrackPar(2,0,ifit);
  if (iddc>=0) rigidity_al = btcor*trk->GetRigidity(iddc);
  rigidity_ck = 0;
  iddc = trk->iTrTrackPar(3,0,ifit);
  if (iddc>=0) rigidity_ck = btcor*trk->GetRigidity(iddc);
  rigidity_tp = 0;
  iddc = trk->iTrTrackPar(1,5,ifit);
  if (iddc>=0) rigidity_tp = btcor*trk->GetRigidity(iddc);
  rigidity_bt = 0;
  iddc = trk->iTrTrackPar(1,6,ifit);
  if (iddc>=0) rigidity_bt = btcor*trk->GetRigidity(iddc);
  rigidity_itp = 0;
  iddc = trk->iTrTrackPar(1,1,ifitspecial);
  if (iddc>=0) rigidity_itp = btcor*trk->GetRigidity(iddc);
  rigidity_ibt = 0;
  iddc = trk->iTrTrackPar(1,2,ifitspecial);
  if (iddc>=0) rigidity_ibt = btcor*trk->GetRigidity(iddc);
  rigidity_in = 0;
  iddc = trk->iTrTrackPar(1,3,ifit);
  if (iddc>=0) rigidity_in = btcor*trk->GetRigidity(iddc);


  // search nearest noise hit for each layer
  int trk_layer_pattern[9] = {0,0,0,0,0,0,0,0,0};
  Double_t noise_y[9] = {1.e9,1.e9,1.e9,1.e9,1.e9,1.e9,1.e9,1.e9,1.e9};
  int laypa = 1;
  for (int layer=1; layer<=9; layer++) {
    int bitexam = int(pow(2.,layer-1));
    bitexam = (bitexam & btp);
    if (bitexam>0)    trk_layer_pattern[layer-1]  = 1;  // flags hitted layers
    AMSPoint pnoise = TrTrackSelection::GetMinDist(trk, laypa, 1);
    noise_y[layer-1]=fabs(pnoise.y());
    laypa=laypa*10;
  }
  nlayer = 0;
  minoise1 = 7.;
  int lminoise1 = -1;
  for (int layer=1; layer<=9; layer++) {
    if(trk_layer_pattern[layer-1] ==1) nlayer=nlayer+1;  // found number hitted layer
    if (noise_y[layer-1]<minoise1 && noise_y[layer-1] >0.001 && trk_layer_pattern[layer-1] ==1)
      {minoise1 = noise_y[layer-1]; lminoise1=layer;}}

  if (fullProcess) {
    minoise2 = 7.;
    int lminoise2 = -1;
    for (int layer=1; layer<=9; layer++) {
      if (noise_y[layer-1]<minoise2 && noise_y[layer-1] >0.001 && trk_layer_pattern[layer-1] ==1 && layer != lminoise1)
	{minoise2 = noise_y[layer-1]; lminoise2=layer;}}
    minoise3=7.;
    int lminoise3 = -1;
    for (int layer=1; layer<=9; layer++) {
      if (noise_y[layer-1]<minoise3 && noise_y[layer-1] >0.001 && trk_layer_pattern[layer-1] ==1 && layer != lminoise1 && layer != lminoise2)
	{minoise3 = noise_y[layer-1]; lminoise3=layer;}}
    minoise4=7.;
    int lminoise4 = -1;
    for (int layer=1; layer<=9; layer++) {
      if (noise_y[layer-1]<minoise4 && noise_y[layer-1] >0.001 && trk_layer_pattern[layer-1] ==1 && layer != lminoise1 && layer != lminoise2 && layer != lminoise3)
	{minoise4 = noise_y[layer-1]; lminoise4=layer;}}
  }
  // SEARCH FOR MAXIMUM RESIDUAL(0) NEXT OF MAXIMUM (1) MINIMUM (2)
  maxresx0=0;
  maxresy0=0;
  maxresx1=0;
  maxresy1=0;
  minresx=1.e9;
  minresy=1.e9;
  double resappox[9];
  double resappoy[9];
  AMSPoint pres;
  for (int layer=1; layer<=9; layer++) {
    resappox[layer-1]=0;
    resappoy[layer-1]=0;
    if (trk_layer_pattern[layer-1] ==1){
      trk->GetResidualKindJ(layer,pres,0,idd);
      resappox[layer-1]=fabs(pres[0]);
      resappoy[layer-1]=fabs(pres[1]);
      if (maxresx0 < fabs(pres[0])) maxresx0 = fabs(pres[0]); 
      if (maxresy0 < fabs(pres[1])) maxresy0 = fabs(pres[1]); 
    }
  }

  for (int layer=1; layer<=9; layer++) {
    if ((maxresx1 < resappox[layer-1]) && (resappox[layer-1]<maxresx0)) maxresx1 = resappox[layer-1];
    if ((maxresy1 < resappoy[layer-1]) && (resappoy[layer-1]<maxresy0)) maxresy1 = resappoy[layer-1];
    if ((minresx > resappox[layer-1]) && (resappox[layer-1]>0)) minresx = resappox[layer-1];
    if ((minresy > resappoy[layer-1]) && (resappoy[layer-1]>0)) minresy = resappoy[layer-1];
  }

  // TRK PARAMETER EVALUATION                                                                       
  // pattern                                                                                                           
  pat = -1;
  ly1 = trk_layer_pattern[0];
  //  ly1 = btp%2;  // to check
  ly9 = trk_layer_pattern[8];
  /*
    ly9 = 0;
    if (btp > 255) ly9 = 1; // to check
    ly2 = 0;
    if(btp & (1 << 1)) ly2=1;
  */
  ly2 = trk_layer_pattern[1];
  if (ly1==0 && ly9==0 && ly2==0) pat=0;  // select 000                                                               
  if (ly1==0 && ly9==0 && ly2==1) pat=1;  // select 001                                                               
  if (ly1==0 && ly9==1 && ly2==0) pat=2;  // select 010                                                               
  if (ly1==0 && ly9==1 && ly2==1) pat=3;  // select 011                                                               
  if (ly1==1 && ly9==0               ) pat=4;  // select 10x                                                          
  if (ly1==1 && ly9==1               ) pat=5;  // select 11x                                                          
  // track position and direction
  dtrkavgy=0;
  double ytrk[9]={0,0,0,0,0,0,0,0,0};
  for (int layer=1; layer<=9; layer++) {
    AMSPoint pntj;
    AMSDir dirj;
    trk->InterpolateLayerJ(layer,pntj,dirj,idd);
    ytrk[layer-1]=pntj[1];
    dtrkavgy=dtrkavgy+dirj[1];
  }
  dtrkavgy=dtrkavgy/9.;
  
  // trk cluster for each layer                                                
  int ntrcl[9]={0,0,0,0,0,0,0,0,0};
  int nytrcl[9]={0,0,0,0,0,0,0,0,0};
  int nytrcl5[9]={0,0,0,0,0,0,0,0,0};
  double qlaymaxx[9]={0,0,0,0,0,0,0,0,0};
  double qlaymaxy[9]={0,0,0,0,0,0,0,0,0};
  double ytrcl[9]={0,0,0,0,0,0,0,0,0};
  double yytrcl[9]={0,0,0,0,0,0,0,0,0};
  trk_ql18=0;
  int ntrclu = evt->NTrCluster();
  for (int icl = 0; icl<ntrclu; icl++){ //scan trk cluster 
    TrClusterR* trcl = evt->pTrCluster(icl);
    if (!trcl) continue;
    int layer = trcl->GetLayer(); // old numbering                             
    if (layer==8) layer=0;
    if (layer==9) layer=8;
    float qapp = trcl->GetQ();
    if (trcl->GetSide()==1 && qapp>qlaymaxy[layer]) qlaymaxy[layer]=qapp;
    if (trcl->GetSide()!=1 && qapp>qlaymaxx[layer]) qlaymaxx[layer]=qapp;
    if (layer<8 && qlaymaxx[layer]>trk_ql18) trk_ql18 = qlaymaxx[layer]; //not used
    if (layer<8 && qlaymaxy[layer]>trk_ql18) trk_ql18 = qlaymaxy[layer];
    if (qapp<0.75) continue; // noise hit                                                                              
    ntrcl[layer]++;
    if(trcl->GetSide()==1){nytrcl[layer]++;
      ytrcl[layer]=ytrcl[layer]+trcl->GetGCoord(0);
      if(fabs(trcl->GetGCoord(0)-ytrk[layer])<5.) nytrcl5[layer]++;
      yytrcl[layer]=yytrcl[layer]+(trcl->GetGCoord(0)*trcl->GetGCoord(0));
    }}
  for (int ill=0;ill<=8;ill++){
    if(nytrcl[ill]>0) ytrcl[ill]=ytrcl[ill]/(1.*nytrcl[ill]);  // ymedio                                               
    if(nytrcl[ill]>0) yytrcl[ill]=yytrcl[ill]/(1.*nytrcl[ill]);  // yquadro medio                                      
  }

  // weighted average of rms of yclusters in each layer
  trk_yrms=0;
  for (int ill=0;ill<=8;ill++){
    trk_yrms=trk_yrms+nytrcl[ill]*(yytrcl[ill]-ytrcl[ill]*ytrcl[ill]);}
  trk_yrms=  trk_yrms/(nytrcl[0]+nytrcl[1]+nytrcl[2]+nytrcl[3]+nytrcl[4]+nytrcl[5]+nytrcl[6]+nytrcl[7]+nytrcl[8]);

  trk_ql9 = qlaymaxy[8];  // not used usually
  if(trk_ql9<qlaymaxx[8]) trk_ql9=qlaymaxx[8];

  ntrkcl9 = ntrcl[8];
  ntrky5cl18 = nytrcl5[0]+nytrcl5[1]+nytrcl5[2]+nytrcl5[3]+nytrcl5[4]+nytrcl5[5]+nytrcl5[6]+nytrcl5[7];
  

  // BETA/TOf RELATED
  if (rebuildRefit<0) TofRecH::ReBuild();
  nbeta = evt->nBetaH();
  double betaq = 1;
  BetaHR *betah=part->pBetaH();
  int bartof[4] = {-10,-10,-10,-10};
  double ttof[2] = {0,0};  // avg time 0 = upper tof  
  tof_qrms=0;
  tof_chi2c=0;
  tof_chi2t=0;
  tof_beta =1;
  tof_nused=0;
  tof_qu = 0;
  tof_qd = 0;
  if (! betah || nbeta<1) {iret = iret*3;}
  else {
    float qtofrms=0;
    int ntofh=0;
    //    double qtoft=betah->GetQ(ntofh,qtofrms);
    betah->GetQ(ntofh,qtofrms);
    tof_qrms=qtofrms;
    tof_chi2c=betah->GetNormChi2C();
    tof_chi2t=betah->GetNormChi2T();
    betaq = betah->GetBetaS();
    tof_beta = betaq;
    if (betaq>1) betaq=0.999999;
    if ((betah->NTofClusterH())<1) iret = iret*3;

    double tusedtof[4] ={0,0,0,0};
    double qtof[4] ={0,0,0,0};
    for(int it=0; it<betah->NTofClusterH(); it++){
      TofClusterHR* ptof = betah->pTofClusterH(it);
      if( !ptof ) continue;
      if( ptof->IsGoodTime()==false) continue; //true have a signal in both sides                             
      tof_nused++;  // not used
      int tlayer = ptof->Layer;
      bartof[tlayer]=ptof->Bar;
      tusedtof[tlayer]= ptof->Time;
      qtof[tlayer]=betah->GetQL(tlayer);
    }

    ttof[0] = ttof[0]+tusedtof[0];
    ttof[0] = ttof[0]+tusedtof[1];
    tof_qu = tof_qu +qtof[0]; 
    tof_qu = tof_qu +qtof[1]; 
    if (bartof[0]>=0 && bartof[1]>=0) {
      ttof[0]=0.5 * ttof[0];
      tof_qu = 0.5 * tof_qu;}

    ttof[1] = ttof[1]+tusedtof[2];
    ttof[1] = ttof[1]+tusedtof[3];
    tof_qd = tof_qd +qtof[2]; 
    tof_qd = tof_qd +qtof[3]; 
    if (bartof[2]>=0 && bartof[3]>=0) {
      ttof[1]=0.5 * ttof[1];
      tof_qd = 0.5 * tof_qd;}
  }
  
  // track charge and max charge in the track
  trk_q=trk->GetQ(betaq);
  trk_qrms=trk->GetQ_RMS(betaq);
  trk_qmax=0;
  for (int layer=1; layer<=9; layer++) {
    double qapp = trk->GetLayerJQ(layer,betaq);
    if (qapp>trk_qmax) trk_qmax=qapp;}
  // TOF stdalone activity:                                                                                 
  double bkstr[2] = {10,3.55}; // threshold for upper-lower baksplash                                       
  double upbkstr[2] = {30.,16.}; // upper threshold to reject Ghost baksplash                                       
  int ntoflbks[2] ={0,0};
  int ntofbks[2] ={0,0};
  int ntofint[2] ={0,0};
  tof_qmax=0;
  int ntofclu=evt->NTofClusterH();
  for(int it=0; it<ntofclu; it++){
    TofClusterHR* ptof = evt->pTofClusterH(it);
    if( !ptof ) continue;
    if( ptof->IsGoodTime()==false) continue; //true have a signal in both sides                             
    int tlayer = ptof->Layer;
    int bar = ptof->Bar;
    double qt = ptof->GetQSignal();
    if(qt>tof_qmax) {tof_qmax = qt;}  // store maximum charge dep 
    if (bar == bartof[tlayer]) continue; // cluster used in particle                                        
    double tctime = ptof->Time;
    int iup = 0; // upper tof                                                                               
    if (tlayer>1) iup=1;
    if (tctime > ttof[iup]+bkstr[iup] && tctime < ttof[iup]+upbkstr[iup]) {ntofbks[iup]++; // is backsplash                                    
    }else  // not backspash                                                                                 
      { if (tctime < ttof[iup]+bkstr[iup] && tctime < ttof[iup]-10.) ntofint[iup]++; // is interaction                                           
	if (tctime > ttof[iup]+upbkstr[iup] && tctime < ttof[iup]+100.) ntoflbks[iup]++; // is late backsplash                                                                                           
      }
  }

  // ACC-TOF TIMING MATCHING                                                                             
  if (rebuildRefit<0) evt->RebuildAntiClusters();
  int nantiint=0;
  int nantibks=0;
  int nantilbks=0;
  double qaccmax=0;
  double tanti[8]={0,0,0,0,0,0,0,0};
  double qanti[8]={0,0,0,0,0,0,0,0};
  double zanti[8]={0,0,0,0,0,0,0,0};
  for (int icl = 0; icl<evt->nAntiCluster();icl++){ //scan acc                                           
    AntiClusterR* pcl = evt->pAntiCluster(icl);
    if (!pcl) continue;
    if(pcl->adc[0]==0 && pcl->adc[1]==0) continue; // no both adc                                        
    if(pcl->chi>30) continue; // high chisquare                                                          
    if(pcl->Npairs<=0) continue; // not paired                                                           
    int bar = pcl->Sector-1;
    tanti[bar]=pcl->time;
    zanti[bar]=pcl->unfzeta;
    qanti[bar]=pcl->rawq;
    if (qanti[bar]>qaccmax) qaccmax=qanti[bar];
    if (tanti[bar]>ttof[1]+4. && tanti[bar]<ttof[1]+20.) {nantibks++;  // anti is a backsplash                                   
    }
    else {if (tanti[bar]<ttof[1]+3. && tanti[bar]>ttof[1]-10.) nantiint++;  // anti is from interaction                                                     
      if (tanti[bar]>ttof[1]+20. && tanti[bar]<ttof[1]+100.) nantilbks++;  // anti is from late backsplash                                                     
    }
  }


  nint=ntofint[0]+ntofint[1]+nantiint;
  nbks=ntofbks[0]+ntofbks[1]+nantibks;
  nlbks=ntoflbks[0]+ntoflbks[1]+nantilbks;

  //  cout << iret << " return point 1" << endl;
  // ECAL RELATED
  energy = 0;
  axischi2  =0;
  trk_cofgx=0;
  trk_cofgy=0;
  trk_dirx=0;
  trk_diry=0;
  EcalShowerR *ecal = part->pEcalShower();
  if (!ecal) {iret = iret*2;}
  else {
    energy = ecal->EnergyE;
    //    cout << " internal energy " << energy << endl;
    static EcalAxis *ecalaxis = new EcalAxis();//ATTENTION: THIS STATIC IS SHARED BETWEEN VARIOUS INSTANCES!!!
    double vv = ecalaxis->process(ecal,2,fabs(rigidity_ch)/rigidity_ch);//process ecalaxis using lateral fit method               
    if(vv>0){
      axischi2  =ecalaxis->ecalchi2->get_chi2() ;
      AMSPoint trkcofg;
      AMSDir dtrkcofg;
      AMSPoint acofg;
      AMSDir dax;
      ecalaxis->interpolate(ecal->CofG[2],acofg,dax,2);
      trk->Interpolate(ecal->CofG[2], trkcofg, dtrkcofg, idd);
      trk_cofgx=acofg[0]-trkcofg[0];
      trk_cofgy=acofg[1]-trkcofg[1];
      trk_dirx=dax[0]-dtrkcofg[0];
      trk_diry=dax[1]-dtrkcofg[1];
    }
    else {iret = iret*2;}
  }
  //  cout << iret << " return point 2" << endl;
  // TRD related
  AMSPoint Ptrd;
  AMSDir Dtrd;
  trk->Interpolate(113.5, Ptrd, Dtrd, idd);
  trd_ntrk=evt->nTrdTrack();
  if(trd_ntrk==0) {iret = iret*5;}

  trd_chi2=0;
  trd_q=1;
  trd_diry=0;
  int ntrd=0;
  double mindist=10000.;
  AMSPoint dtrdm;
  dtrdm.setp(0,0,0);
  int ntrdtrk=evt->NTrdTrack();
  for(int i=0; i<ntrdtrk; i++){
    TrdTrackR* trdtrk=evt->pTrdTrack(i);
    if( !trdtrk ) continue;
    ntrd++;
    double trdtheta=trdtrk->Theta;
    double trdphi=trdtrk->Phi;
    double trdx=trdtrk->Coo[0];  
    double trdy=trdtrk->Coo[1];
    double trdz=trdtrk->Coo[2];
    //double qtrdtrk=trdtrk->Q;
    AMSPoint Dhtrd; // da trd                                                                            
    AMSPoint PDtrd; // da trk    
    // inverted direction                                                                        
    Dhtrd.setp(-sin(trdtheta)*cos(trdphi),-sin(trdtheta)*sin(trdphi),-cos(trdtheta));
    PDtrd.setp(Dtrd[0],Dtrd[1],Dtrd[2]);
    //      double cosapp = fabs(Dhtrd.prod(PDtrd));
    AMSPoint Dcrs = Dhtrd.crossp(PDtrd);
    double norm = Dcrs.norm();
    AMSPoint Phtrd;
    Phtrd.setp(Ptrd[0]-trdx,Ptrd[1]-trdy,Ptrd[2]-trdz);
    double minapp = fabs((Phtrd.prod(Dcrs))/norm);  // componente perpendicolare distanza                 
    if(minapp<mindist) {mindist=minapp; //trd_q=qtrdtrk;
      trd_diry=Dhtrd[1]-Dtrd[1]; //trd-trk
      trd_chi2=trdtrk->Chi2;  // (Thanks to Nikolay Nikonov)
    }  
  }
  if (mindist>9000) mindist=0;
  trd_mindist=mindist;

  //  cout << iret << " return point 3" << endl;
  trd_nhit=0;
  trd_sumv=0;
  trd_path=0;
  trd_ratio=0;
  trd_ratioff=0;

  //    double kLike[3]={-1,-1,-1}; //To be filled with 3 LikelihoodRatio : e/P, e/H, P/H                      
  //    double knotratio[3]={-1,-1,-1}; //To be filled with 3 Likelihood not ratio                              
  //int NHits=0;                //To be filled with number of hits taken into account in Likelihood Calculation
  //float threshold=15;         //min ADC taken into account in Likelihood Calculation,  15 ADC is the recommended value for the moment. 
  if( trk && idd>=0 && evt->NTrdRawHit()>0 ){
    TrdKCluster trdkcluster(evt, trk, idd );
    int QTRDStatus = trdkcluster.CalculateTRDCharge(0, betaq);
    if (QTRDStatus>=0) trd_q = trdkcluster.GetTRDCharge();
    if (! (fabs(trd_q)<1.e20)) trd_q = 1.; // avoid nan
    /* no more necessary
    // Get the status of the Calculation
    int trdKIsReadAlignmentOK=trdkcluster.IsReadAlignmentOK;
    int trdKIsReadCalibOK=trdkcluster.IsReadCalibOK;
    int isvalid;
    if (energy>0) {isvalid = trdkcluster.GetLikelihoodRatio_TrTrack(threshold,kLike,NHits,energy,knotratio);}
    else {isvalid = trdkcluster.GetLikelihoodRatio_TrTrack(threshold,kLike,NHits);}
    if( NHits>0 && idd>0 && trdKIsReadAlignmentOK>0 && trdKIsReadCalibOK>0 && isvalid )
    {
    
    goodTrdKCalib=true;
    Likep = kLike[0];
    LikeHe = kLike[1];
    LikHep = kLike[2];
    nrLik1 = knotratio[0];
    nrLik2 = knotratio[1];
    nrLik3 = knotratio[2];
    int QTRDStatus = trdkcluster.CalculateTRDCharge(0, betaq);
    if (QTRDStatus>=0) trd_q = trdkcluster.GetTRDCharge();
    TRDCleanliness=trdkcluster.GetCleanliness();
    TRDAsyNormE = trdkcluster.GetAsyNormE();
    TRDAsyD = trdkcluster.GetAsyD();
    TRDIPChi2 = trdkcluster.GetIPChi2();
    
    } 
    */
    
    int nhitted = 0;
    int noff = 0;
    double sum=0;
    double sumv=0;
    double sumvoff=0;
    double ple=0;
    for(int i=0;i<trdkcluster.NHits();i++){
      TrdKHit *hit=trdkcluster.GetHit(i);
      float path_length=hit->Tube_Track_3DLength(&Ptrd,&Dtrd);
      float Amp=hit->TRDHit_Amp;
      //double zhit = (hit->TRDTube_Center_Raw[2]);
      if(Amp>0 && path_length>0){
	nhitted++;
	ple+=path_length;
	sum+=Amp/path_length;
	sumv+=Amp;
      }
      else{
	if(Amp>0){
	  noff++;
	  sumvoff+=Amp;
   	  sumv+=Amp;
	}}}
    trd_nhit=nhitted; // not used
    trd_noff=noff;  // not used
    trd_sumv=sumv;  // not used
    trd_path=ple;
    if(nhitted!=0) trd_ratio=sum/(1.*nhitted);  // not used
    if(noff!=0) trd_ratioff=sumvoff/(1.*noff); // not used
  }
  else {iret = iret*5;}
  
  // RICH 
  //      cout << iret << " return point 4" << endl;
  nrings=evt->nRichRing();

  double betarich=1;
  double qqrich=1;
  double wrich=1;
  double nringexph=0;
  double nringph=0;
  RichRingR *rich=part->pRichRing();
  if(!rich || nrings==0 || evt->NRichHit()==0) {betarich=1; iret=iret*7;}
  else{
    betarich=rich->getBeta();
    nringph=rich->getPhotoelectrons();
    nringexph=rich->getExpectedPhotoelectrons();
    qqrich=rich->getCharge2Estimate();
    wrich=rich->getWidth();
  }
  
  rich_beta=betarich; // not used
  rich_w=wrich;

  rich_ratio = 1;
  if(nringexph!=0) rich_ratio=nringph/nringexph;

  double yrich=0;
  double yyrich=0;
  double xrich=0;
  double xxrich=0;
  int nunusedrh=0;
  double zrich=0;
  int nri = evt->NRichHit();
  for (int irh = 0; irh<nri;irh++){ //scan rich hits                                              
    RichHitR* rhit = evt->pRichHit(irh);
    if (!rhit) continue;
    int sw = rhit->Status;
    int used = 0;
    for (int iu=0;iu<11;iu++){
      if((sw & 1<<iu)) used++;}
    if (used==0){
      xrich = xrich+rhit->Coo[0];
      yrich = yrich+rhit->Coo[1];
      zrich = zrich+rhit->Coo[2];
      xxrich = xxrich+rhit->Coo[0]*rhit->Coo[0];
      yyrich = yyrich+rhit->Coo[1]*rhit->Coo[1];
      nunusedrh++;
    }
  }
  if(nunusedrh!=0) {
    xrich=xrich/(1.*nunusedrh);
    yrich=yrich/(1.*nunusedrh);
    zrich=zrich/(1.*nunusedrh);
    xxrich=xxrich/(1.*nunusedrh);
    yyrich=yyrich/(1.*nunusedrh);
  }
  //  cout << iret << " return point 5" << endl;
  AMSPoint trkrh;
  AMSDir dtrkrh;
  trk->Interpolate(zrich, trkrh, dtrkrh, idd);
  
  rich_nunused=nunusedrh;
  rich_res=fabs(xrich-trkrh[0])+fabs(yrich-trkrh[1]);
  rich_rms=0.;
  if(nunusedrh>1) rich_rms=sqrt(yyrich-yrich*yrich+xxrich-xrich*xrich);

  pMDR = 0;
  if (fullProcess) {
    Double_t Bmd,SenThetaX, SenThetaY, brLev, pMDR, pMDR_known;
    MagField* magfield = MagField::GetPtr();  
    PathOnB(trk, idd, magfield, Bmd, SenThetaX, SenThetaY, brLev, pMDR, pMDR_known, 0.1, false);
    pMDR = pMDR_known;
  }
  nmsk = iret;
  return iret;
}
#endif

//------------------------------------------------------------------
#ifndef _NOGBATCH_
// AUTHOR M.DURANTI INFN-PG
void TrkCCVar::PathOnB(TrTrackR* tr, int kDef, MagField* magfield, Double_t& B, 
		    Double_t& SenThetaX, Double_t& SenThetaY, Double_t& L, Double_t& pMDR, 
		    Double_t& pMDR_known, Double_t step, bool modulus){

  // Z of tracker layers
  static Double_t zlay[9]={53.060000, 29.228000, 25.212000, 1.698000, -2.318000, -25.212000, -29.228000, 158.920000, -135.882000};
  //layer esterni alla fine così se looppo solo su 7 piani automaticamente è l'inner
  
  B=0;
  SenThetaX=0;
  SenThetaY=0;
  L=0;
  pMDR=0;
  pMDR_known=0;

  if (!tr) {
    printf("PathOnB:: You passed an empty TrTrackR pointer... Returning 0's!");
    return;
  }
  
  static int ntrhits;
  ntrhits = tr->GetNhits();
  
  static Double_t start;
  start=-999999;

  static Double_t stop;
  stop=999999;
  
  for (int ii=0; ii<ntrhits; ii++) {
    static TrRecHitR* rh;
    rh=tr->pTrRecHit(ii);
    if (rh) {
      if (rh->GetYCluster()) {//only if there's a measurement of Y
	static int lay;
	lay=rh->GetLayer();
	static Double_t zed;
	zed=zlay[lay-1];
	//	printf("%d) layer = %d, zed = %f\n", ii, lay, zed);//only for debug
	if (zed>start) start=zed;//the greatest
	if (zed<stop) stop=zed;//the smallest
      }
    }
  }
  //  printf("Start = %f, Stop = %f\n", start, stop);  
  
  static float x[3]={0, 0, 0};
  static float _B[3]={0, 0, 0};
  
#define ONLYY 

#ifdef ONLYY
  int howmany=0;
  static AMSPoint inmiddleout[3];
  static Double_t zinmiddleout[3];
  zinmiddleout[0]=start;
  zinmiddleout[1]=(start+stop)/2;//a Z/2
  zinmiddleout[2]=stop;
  tr->Interpolate(3, zinmiddleout, inmiddleout, 0, 0, kDef);
  static Double_t DZ;
  static Double_t DY;
  static Double_t DX;
  static Double_t D_ZY;
  static Double_t D_ZX;
  DZ=fabs(inmiddleout[2].z()-inmiddleout[0].z());
  DY=fabs(inmiddleout[2].y()-inmiddleout[0].y());
  DX=fabs(inmiddleout[2].x()-inmiddleout[0].x());
  D_ZY=sqrt(DZ*DZ+DY*DY);
  D_ZX=sqrt(DZ*DZ+DX*DX);
  L=D_ZY;
  SenThetaX=fabs(DZ/D_ZX);//vertical means ThetaX=90 -> SenThetaX=1
  SenThetaY=fabs(DZ/D_ZY);//vertical means ThetaY=90 -> SenThetaY=1
  static Double_t y1;
  static Double_t y2;
  static Double_t y3;
  y1=inmiddleout[0].y();
  y2=inmiddleout[1].y();
  y3=inmiddleout[2].y();
  static Double_t SigmaY;
  static TF1* sigmay = new TF1("sigmay", "13.75-3.437*(x-1.0)+31.86*((x-1.0)*(x-1.0))", 0.0, 1.0);
  static Double_t sigmay0=sigmay->Eval(1.0)/1000000.0;//in m
  SigmaY=sigmay->Eval(SenThetaY)/1000000.0;//in m
  //  printf("SigmaY = %f\n", SigmaY);//only for debug
  static Double_t SigmaS;
  static TF1* sigmas = new TF1("sigmas", "18.003-139.973*x+599.883*x*x-1496.24*x*x*x+2235.34*x*x*x*x-1976.75*x*x*x*x*x+954.921*x*x*x*x*x*x-194.295*x*x*x*x*x*x*x", 0.0, 1.0);
  static Double_t sigmas0=sigmas->Eval(1.0);
  static Double_t kost=sqrt(3.0/2.0);//from Gluckstern simple model
  SigmaS=kost*SigmaY*sigmas->Eval(SenThetaY)/sigmas0;
  //  printf("SigmaS = %f\n", SigmaS);//only for debug
#endif

  static AMSPoint pvec[10000];
  static Double_t zpl[10000];
  static int nz;
  nz=0;
  for (Double_t ii=start; ii>=(stop-1); ii-=step) {
    zpl[nz]=ii;
    nz++;
  }
  tr->Interpolate(nz, zpl, pvec, 0, 0, kDef);
  static int index;
  index=0;

  for (Double_t ii=start+step; ii>=stop; ii-=step) {
    static AMSPoint global;
    static AMSPoint global_old;
    index++;
    global=pvec[index];
    global_old=pvec[index-1];
#ifdef ONLYY
    x[0]=global.x();
    x[1]=global.y();
    x[2]=global.z();
    if (magfield) magfield->GuFld(x, _B);
    if (modulus) B+=fabs(_B[0]);
    else B+=_B[0];//va bene con il segno perchè se ci fosse una zona con campo nell'altro verso questo "diminuirebbe" la curvatura
    howmany++;
#else
    x[0]=global.x();
    x[1]=global.y();
    x[2]=global.z();
    static float dx[3]={0, 0, 0};
    dx[0]=x[0]-global_old.x();
    dx[1]=x[1]-global_old.y();
    dx[2]=x[2]-global_old.z();
    if (magfield) magfield->GuFld(x, _B);
    static Double_t vec[3];
    //---------------------------sbagliato----------------------------
    // if (_B[0]) {
    //   vec[1]=-dx[2];
    //   vec[2]=dx[1];
    // }
    // if (_B[1]) {
    // vec[0]=dx[2];
    // vec[2]-=dx[0];
    // }
    // if (_B[2]) {
    // vec[0]-=dx[1];
    // vec[1]+=dx[0];
    // }
    // B+=vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2];
    //---------------------------sbagliato-----------------------------
    vec[0]=_B[1]*dx[2]*dx[2] - _B[2]*dx[1]*dx[1];
    vec[1]=_B[2]*dx[0]*dx[0] - _B[0]*dx[2]*dx[2];
    vec[2]=_B[0]*dx[1]*dx[1] - _B[1]*dx[0]*dx[0];
    BL2+=sqrt(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]);
#endif
#ifndef ONLYY
    //    printf("%f) %f %f %f, %f %f %f -> %f %f\n", ii, x[0], x[1], x[2], B[0], B[1], B[2], intB, intBL2);//only for debug
#else
    //    printf("%f) %f %f %f, %f\n", ii, x[0], x[1], x[2], _B[0]);//only for debug
#endif
  }
#ifdef ONLYY
  //  printf("B total = %f 0.1*T\n", B);
  B/=howmany;
  //  printf("B mean = %f 0.1*T\n", B);
  pMDR=0.3*B*L*L/(8.0*SenThetaX*SigmaS);
  //pMDR = L^2qB / 8 sigmas sinthetax sin^2thetay.
  //0.3 is the factor conversion from Kg m/s / C*T to GeV/c / T
  //L here is DZ/SenThetaY and so the sinthetay is already included.
  pMDR_known=0.3*B*DZ*DZ/(8.0*SenThetaX*sigmay0*sqrt(3.0/2.0));
  //pMDR = L^2qB / 8 sigmas sinthetax sin^2thetay.
  //0.3 is the factor conversion from Kg m/s / C*T to GeV/c / T
  //sigmay0 is sigmay at sinthetay=1.0
  //sigmas=sqrt(3/2)*sigmay from Gluckstern simple model
#endif

  B/=10;//to have in T instead of 0.1*T
  //  printf("B (T) = %f\n", B);
  L/=100;//to have in m instead of cm
  pMDR/=(10*100*100);//to have in m^2*T instead of 0.1*cm^2*T
  pMDR_known/=(10*100*100);//to have in m^2*T instead of 0.1*cm^2*T

  L/=(2*sqrt(SigmaS*10000.0));
  //  printf("L = %f\n", L);//only for debug

  //  sleep(3);

  return;
}
#endif
//---------------------------------------------------------------------------//

TMVAxmlReader::TMVAxmlReader(){
  //nothing needed 
}

TMVAxmlReader::~TMVAxmlReader(){
}

string TMVAxmlReader::_GetField(string line,string searchkey){
  int i_pos= line.find(searchkey);
  string expression = line.substr(i_pos);
  int f_pos  = expression.find(" "); // includes " in the expression
  expression = expression.substr(0 , f_pos-1);
  expression = expression.substr(searchkey.length());
  return expression;
}

Int_t TMVAxmlReader::_GetVariables(TString varhead,  TString key, FILE *file,vector <TString> &bdtvars){
  varhead.ReplaceAll(Form("<%s=\"",key.Data()),"");
  varhead.ReplaceAll("\">","");
  Int_t nofvars = atoi( varhead.Data());
   
  char buf[2000];
  for( int i=0; i<nofvars; i++){
    if(fgets(buf,sizeof(buf),file)!=0){
      string expression = _GetField((string)buf,"Expression=\"");
      bdtvars.push_back((TString)expression);
    }
  }
  return nofvars;
}

void TMVAxmlReader::Get(vector <TString> &bdtvars,TString filename, Bool_t debug ){
  _debug = debug;

  FILE *fi = fopen( filename.Data(),"r");
  int nofVars=0;
  int nofSpects=0;

  char buf[2000];
  Int_t done=0;
  while(fgets(buf,sizeof(buf),fi)!= 0 || done<2){ 

    if( strstr(buf,"Variables NVar")!=0){
      nofVars = _GetVariables(buf,"Variables NVar",fi, bdtvars);      
      done = done+1;
    }
    if( strstr(buf,"Spectators NSpec")!=0){
      nofSpects = _GetVariables(buf,"Spectators NSpec",fi, bdtvars);
      done = done+1;
    }
  }  
  bdtvars.push_back(Form("%d",nofVars));   // last two elements are nofVars
  bdtvars.push_back(Form("%d",nofSpects)); // and nofSpects

  if(_debug){
    cout<<" [TMVAxmlReader::get] -- debug="<<_debug<<"  The following variable list was obtained from xml file:  "<<filename.Data()<<endl;
    cout<<" [TMVAxmlReader::get] nofVars "<<nofVars<<"  nofSpects "<<nofSpects<<endl;
    cout<<" [TMVAxmlReader::get] Total "<<bdtvars.size()<<endl;
    for( int i=0; i<(int)bdtvars.size() ; i++){
      cout<<"[TMVAxmlReader::get]  i="<<i<<"  String: "<<bdtvars.at(i).Data()<<endl;
    }
    cout<<"[TMVAxmlReader::get] ---- "<<endl;
  }
  
  return;
}

TrkCCAlgorithm::TrkCCAlgorithm(){
  _recommendmask=0xFFFFFFFFFFFFFULL;
  _usedmask=0xFFFFFFFFFFFFFULL;
}

TrkCCNozzoli::TrkCCNozzoli(TString subid, int version, int verbose){
  
  _subid=subid;
  
  if (version==1) _nvars = nVarsTrkCCNozzoliVersion1;
  else if (version==0) {//default
    version=1;
    _nvars = nVarsTrkCCNozzoliVersion1;
  }

  _verbose=verbose;

  _varlist = new float[_nvars];
  _varnorm = new float[_nvars];
  _listamatr = new float*[_nvars];
  for (int ii=0; ii<_nvars; ii++) {
    _listamatr[ii] = new float[9];
  }

  _evenergy=0;
  
  for (int ii=0; ii<nReg; ii++) {
    for (int jj=0; jj<nTrkCCpatts; jj++) {
      for (int kk=0; kk<3; kk++) {
	for (int ll=0; ll<rnEne; ll++) {
	  _rweight[ii][jj][kk][ll] = new float[_nvars];
	}
      }
    }
  }

};

TrkCCNozzoli::~TrkCCNozzoli(){
  _head[MakeSubIdInt(_subid)][_version]=NULL;

  delete [] _varlist;
  delete [] _varnorm;
  for (int ii=0; ii<_nvars; ii++) {
    delete [] _listamatr[ii];
  }
  delete [] _listamatr;

};

TrkCCNozzoli* TrkCCNozzoli::gethead(TString subid, int version, int verbose) {
  
  static bool firstever = true;
  if (firstever) {
    for (int ii=0; ii<nReg; ii++) {
      for (int jj=0; jj<nVer; jj++) {
	_head[ii][jj]=NULL;
      }
    }
  }
  
  if (!_head[MakeSubIdInt(subid)][version]) {
    printf("TrkCCNozzoli::gethead()-M-Creating object TrkCCNozzoli%s_%d as singleton\n", subid.Data(), version);
    _head[MakeSubIdInt(subid)][version] = new TrkCCNozzoli(subid, version, verbose);
  }
  
  return _head[MakeSubIdInt(subid)][version];
}

double TrkCCNozzoli::GetEvaluator() {
  return GetRegression(MakeSubIdInt(_subid));
}

//-------------------------------------------------------------------------
int TrkCCNozzoli::_EvaluateRegression(float varnor[], float a[], float b[], float ps[], float& vreg){
  float* reg = new float[_nvars];
  int debbugg = _verbose;
  unsigned long long masskk = _usedmask;
  float reg0 = vreg;
  int iret =  EvaluateRegression(varnor, a, b, ps, reg, _nvars, reg0, debbugg, masskk);
  vreg = reg0;
  delete [] reg;
  return iret;
}

//-------------------------------------------------------------------------
int TrkCCNozzoli::EvaluateRegression(float varnor[], float a[], float b[], float ps[], float reg[], int nvr, float& vreg, int debbugg, unsigned long long masskk){
  // return 0 OK                                                                                       
  // return <0 problem (-2 problem at second variable that is varnor[1])                                                                 
  float* pss = new float[nvr];
  double ptot = 0;
  int nanflag = -1;
  for (int ivv=0;ivv<nvr;ivv++){  //single variable regression
    double vappo = varnor[ivv];
    if (!(fabs(varnor[ivv]) >=0)) nanflag=ivv;
    if (!(fabs(varnor[ivv]) >=0)) vappo=0;
    if (ps[ivv]<0) vappo = ((log10(fabs(vappo))+fabs(vappo))-0.523158)/1.03787; //normalized fabs distribution
    if(vappo>7) vappo=7.;
    if(vappo<-7) vappo=-7.;
    reg[ivv]=0;
    if(fabs(a[ivv])>1.e-30) {reg[ivv]=(vappo-b[ivv])/a[ivv];}
    else{ reg[ivv]=0; nanflag=ivv;}
    //WARNING
    //    if (ivv==1 || ivv==36 || ivv == 8 || ivv == 19) {reg[ivv]=0;ps[ivv]=0;} //test no cofg no trkq
    pss[ivv]=ps[ivv];
    //cout << std::hex << (_usedmask &  ((unsigned long long) 1  << ivv)) << std::dec << " " << ivv << endl;
    if( (masskk & ((unsigned long long) 1 << ivv)) == 0) {
      pss[ivv] = 0.;
      //cout << ivv << " variable masked"<<endl; 
    }
    ptot = fabs(pss[ivv])+ptot;
  }
  if (debbugg>0 && nanflag >= 0) cout << "cured problem at variable " << nanflag << " " << varnor[nanflag]<<endl;
  double reg0=0;
  for (int ivv=0;ivv<nvr;ivv++){  //total regression                           
    pss[ivv]=pss[ivv]/ptot;
    //  if (_verbose>=9) cout << ivv << " ps=" << pss[ivv] << " reg=" << reg[ivv]<< " a="<<a[ivv]<<endl;
    reg0 = reg0+fabs(pss[ivv])*reg[ivv];
  }
  vreg = reg0;
  //cout << vreg << " raw regression " << endl;
  int iret = -(nanflag+1);
  delete [] pss;
  return iret;
}
  
// pay attention if return 0
// example of usage 
float TrkCCNozzoli::GetRegression(int regtype, unsigned long long masktype, int fixbin){
  //cout << " getregression " << endl;
  if (regtype>=_lastreg){   
    PrintError(Form("TrkCC::GetRegression %d not existing -> used 0", regtype));
    regtype=0;
  }

  if (masktype==0) _usedmask=_recommendmask;
  else if (masktype==1) _usedmask=0xFFFFFFFFFFFFFULL;
  else _usedmask=masktype & _recommendmask;

  float reg=0;
  float regnorm=0;
  float* revarnorm = new float[_nvars];
  // revarnorm IS AN HARD-CODED redefinition of varnorm
  // I am sorry but this is necessary since some varnorm variables 
  //have the same mean but different RMS for CC/NOCC samples
  // for this motivation you found use of fabs()
  // revarnorm[8] is a temptative to create a quasi-float variable from int variables.
  //    float revarnorm[_nvarf+_nvari]; // this is not allowed in cint
  for (int ivv = 0; ivv<_nvars; ivv++){
    revarnorm[ivv]=_varnorm[ivv];
    if(regtype==0 || regtype==2) {
      if (ivv==5 || ivv == 21 || ivv == 26 || ivv == 27) revarnorm[ivv]=0.6+2.*log10(fabs(_varnorm[ivv]));
      if(ivv==5 || ivv == 21 || ivv == 26 || ivv == 27) revarnorm[ivv]=revarnorm[ivv]*(exp(0.1*revarnorm[ivv])) - 1.;
      if(ivv==5 && _evpatt<2) revarnorm[ivv]=revarnorm[ivv]-16.;
      if(ivv==27 && _evpatt!=5) revarnorm[ivv]=0.;
      if (ivv == 8) {
        revarnorm[8]=0.05*_varnorm[8]+0.0005*_varnorm[17]+0.01*_varnorm[25]+0.05*_varnorm[28]-0.001*_varnorm[32];
        revarnorm[8]=revarnorm[8]*10.;
      }
    }
  }

  if (fixbin>=0) { // forced the training energy bin (for calibration purposes)
      _EvaluateRegression(revarnorm,_rweight[regtype][_evpatt][0][fixbin],_rweight[regtype][_evpatt][1][fixbin],_rweight[regtype][_evpatt][2][fixbin],reg);
      regnorm = (reg-_mddd[regtype][fixbin][_evpatt])/_rmss[regtype][fixbin][_evpatt];
      delete [] revarnorm;
      return regnorm;
  }

  // here fixbin <0 (default) use bin interpolation
  if (_evenergy < _reneth[regtype][0])  // below first bin  
    {
      _EvaluateRegression(revarnorm,_rweight[regtype][_evpatt][0][0],_rweight[regtype][_evpatt][1][0],_rweight[regtype][_evpatt][2][0],reg);
      regnorm = (reg-_mddd[regtype][0][_evpatt])/_rmss[regtype][0][_evpatt];
      delete [] revarnorm;
      return regnorm;
    }
  
  if (_evenergy >= _reneth[regtype][_nene[regtype]-1]) // over last bin                                          
    {
      _EvaluateRegression(revarnorm,_rweight[regtype][_evpatt][0][_nene[regtype]-1],_rweight[regtype][_evpatt][1][_nene[regtype]-1],_rweight[regtype][_evpatt][2][_nene[regtype]-1],reg);
      regnorm = (reg-_mddd[regtype][_nene[regtype]-1][_evpatt])/_rmss[regtype][_nene[regtype]-1][_evpatt];
      //cout << " exit2 " << regnorm<< endl;
      delete [] revarnorm;
      return regnorm;
    }
  for (int ieg = 1; ieg<_nene[regtype];ieg++){
    if (_evenergy>=_reneth[regtype][ieg-1] && _evenergy <_reneth[regtype][ieg]){
      double x = (_evenergy-_reneth[regtype][ieg-1])/(_reneth[regtype][ieg]-_reneth[regtype][ieg-1]);
      _EvaluateRegression(revarnorm,_rweight[regtype][_evpatt][0][ieg-1],_rweight[regtype][_evpatt][1][ieg-1],_rweight[regtype][_evpatt][2][ieg-1],reg);
      float regnorm_dw = (reg-_mddd[regtype][ieg-1][_evpatt])/_rmss[regtype][ieg-1][_evpatt];
      _EvaluateRegression(revarnorm,_rweight[regtype][_evpatt][0][ieg],_rweight[regtype][_evpatt][1][ieg],_rweight[regtype][_evpatt][2][ieg],reg);
      float regnorm_up = (reg-_mddd[regtype][ieg][_evpatt])/_rmss[regtype][ieg][_evpatt];
      regnorm = regnorm_dw*(1.-x) + x*regnorm_up;
      delete [] revarnorm;
      return regnorm;
    }
  }

  delete [] revarnorm;
    
  return 0;
}

//---------------------------------------------------------------

TrkCCPizzolotto::TrkCCPizzolotto(TString subid, int version, int verbose){

  _subid=subid;
  
  if (version==1) _nvars = nVarsTrkCCPizzolottoVersion1;
  else if (version==0) {//default
    version=1;
    _nvars = nVarsTrkCCPizzolottoVersion1;
  }
  
  for( int ib=0; ib<nBDT; ib++){
    _reader[ib]=NULL;
  }

  _verbose=verbose;

  _varlist = new float[_nvars];
  _varnorm = new float[_nvars];
  _listamatr = new float*[_nvars];
  for (int ii=0; ii<_nvars; ii++) {
    _listamatr[ii] = new float[9];
  }

  _evenergy=0;
  
  _rarity=false;
  
};

void TrkCCPizzolotto::SetRarity(bool on) {
  if (on) _rarity=true;
  else _rarity=false;
};

TrkCCPizzolotto::~TrkCCPizzolotto(){

  _head[MakeSubIdInt(_subid)][_version]=0;

  delete [] _varlist;
  delete [] _varnorm;
  for (int ii=0; ii<_nvars; ii++) {
    delete [] _listamatr[ii];
  }
  delete [] _listamatr;
  
  for( int ib=0; ib<nBDT; ib++){
    if(_reader[ib]!=NULL) delete _reader[ib];
  }
};

TrkCCPizzolotto* TrkCCPizzolotto::gethead(TString subid, int version, int verbose) {
  
  static bool firstever = true;
  if (firstever) {
    for (int ii=0; ii<nBDT; ii++) {
      for (int jj=0; jj<nVer; jj++) {
	_head[ii][jj]=NULL;
      }
    }
  }
  
  if (!_head[MakeSubIdInt(subid)][version]) {
    printf("TrkCCPizzolotto::gethead()-M-Creating object TrkCCPizzolotto%s_%d as singleton\n", subid.Data(), version);
    _head[MakeSubIdInt(subid)][version] = new TrkCCPizzolotto(subid, version, verbose);
  }
  
  return _head[MakeSubIdInt(subid)][version];
}

double TrkCCPizzolotto::GetEvaluator() {
  
  if (_rarity) return GetRarity(MakeSubIdInt(_subid));
  else return GetBDT(MakeSubIdInt(_subid));
}

float TrkCCPizzolotto::GetBDT(int ib){
  if (ib>=nBDT) {
    PrintError(Form("TrkCCPizzolotto::GetBDT()-E- Required Bdt (int=%d) does not exist. There are only %d available", ib, nBDT));
    return 99.;
  }
  float bdt=-99.;
  if( _reader[ib]==NULL) return bdt;
  if( _verbose>=9 ) cout<<"TrkCCPizzolotto::GetBDT(int)-I- Getting BDT for method "<<Form("bdt%dPatt%d",ib,_evpatt)<<endl;
  bdt = _reader[ib]->EvaluateMVA( Form("bdt%dPatt%d",ib,_evpatt) );   
  return bdt;
}

float TrkCCPizzolotto::GetRarity(int ib){
  if (ib>=nBDT) { 
    PrintError(Form("TrkCCPizzolotto::GetRarity()-E- Required Bdt/rar (int=%d) does not exist. There are only %d readers defined", ib, nBDT));
    return 99.;
  }
  float rar=-99.;
  if( _reader[ib]==NULL) return rar;
  if( _verbose>=9 ) cout<<"TrkCCPizzolotto::GetRarity(int)-I- Getting Rarity for method "<<Form("bdt%dPatt%d",ib,_evpatt)<<endl;
  rar = _reader[ib]->GetRarity(Form("bdt%dPatt%d",ib,_evpatt));   
  return rar;
}

 //-------------------------------------------------------------------------
int TrkCCPizzolotto::_LoadBDTWeights(TString weightsPath){
  if (_verbose>=2) cout<<"TrkCCPizzolotto::_LoadBDTWeights-I- begin"<<endl;

  int ret = 0;
  TString path;

  path = Form("%s/bdt/bdtCC/",weightsPath.Data());
  _reader[0] = _InitBDTReader(path,"bdt0"); // bdt0
  if(_reader[0]==NULL) ret = 100;

  path = Form("%s/bdt/bdtCCorth1/",weightsPath.Data());
  _reader[1] = _InitBDTReader(path,"bdt1"); // bdt1
  if(_reader[1]==NULL) ret = ret + 200;

  path = Form("%s/bdt/bdtCCorth2/",weightsPath.Data());
  _reader[2] = _InitBDTReader(path,"bdt2"); // bdt2
  if(_reader[2]==NULL) ret = ret + 400;
  
  return ret;
}

//-------------------------------------------------------------------------
int TrkCCNozzoli::NormWeights(float ps[], int nvars, int imatr, float nval){
  //normalize the vector ps
  // when 0<=imatr<_nvars and 0<nval<1 
  // then ps is normalized keeping fix ps[nvars]=nval
  int iret = 0; // 0=normal, 1= error
  int nvv=nvars;
  double npes=nval;
  if (npes<0 || npes >1) {npes = 0; imatr=-1; iret=1;}
  if (imatr>=nvv) {iret = 1; npes=ps[0]; imatr=-1;}
  if(npes >=1 && imatr>=0){
    for(int ivv=0;ivv<nvv;ivv++){
      ps[ivv] = 0.;
    }
    ps[imatr]=1;}
  if(npes >=1 && imatr>=0) return iret;
  double ptot = 0;
  double ppar=0;
  for(int ivv=0;ivv<nvv;ivv++){
    //    ps[ivv]=ps[ivv]);
    if (ivv==imatr) continue;
    ppar = ppar+fabs(ps[ivv]);
  }
  ptot = fabs(ppar/(1.-npes));
  for(int ivv=0;ivv<nvv;ivv++){
    ps[ivv]=ps[ivv]/ptot;
    if (ivv==imatr) ps[ivv]=npes;}
  return iret;
}

//-------------------------------------------------------------------------
int TrkCCNozzoli::_LoadRegressionWeights(TString weightsPath){
  if (_verbose>=9) cout<<"TrkCCNozzoli::LoadRegressionWeights()-I- begin"<<endl;
  //return 0  NO ERRORS
  _lastreg = 0;

  ifstream fin(Form("%s/regressions/list_regressions.dat",weightsPath.Data()));
  if (!fin || !(fin.good())) {
    PrintError(Form("TrkCCNozzoli::_LoadRegressionWeights-E- Problem in opening file %s/regressions/list_regressions.dat", weightsPath.Data()));
    return -1;
  }
  
  while (!fin.eof()) {
    TString test = fin.peek();
    if (_verbose>999) cout << "read " << test.Data() << endl;
    if (test == "E") break;
    if (test == "#" || test == " " || test == '\n') {
      if (_verbose>999) cout << "ignoring lines _verbose=" << _verbose << endl;
      fin.ignore(100, '\n');}
    else {
      int ireg,nenebin,rmaxvar;
      fin >> ireg >> nenebin >> rmaxvar;
      if (_verbose>999) cout << ireg << nenebin << rmaxvar << endl;
      _lastreg++;
      if (ireg>=nReg){
	PrintError(Form("TrkCCNozzoli::_LoadRegressionWeights-E- Problem in file %s/regressions/list_regressions.dat\nthe regression: %d, larger than max allowed: %d", weightsPath.Data(), ireg, nReg-1));
	return -1;
      }
      if (nenebin>=rnEne){
	PrintError(Form("TrkCCNozzoli::_LoadRegressionWeights-E- Problem in file %s/regressions/list_regressions.dat\nregression %d number of energy bin: %d is larger than max allowed: %d", weightsPath.Data(), ireg, nenebin, rnEne-1));
	return -1;
      }
      if (rmaxvar>=_nvars) rmaxvar = _nvars-1;
      _rmaxvar[ireg] = rmaxvar;
      _nene[ireg] = nenebin;
      for (int iene = 0; iene<=nenebin; iene++){
	fin >> _reneth[ireg][iene];
	if (_verbose>999) cout << _reneth[ireg][iene] << " " ;
      }
	if (_verbose>999) cout << endl ;
      for (int ipt = 0; ipt<nTrkCCpatts; ipt++){
	int ptt;
	fin >> ptt;
	if (ptt!=ipt){
	  PrintError(Form("TrkCCNozzoli::_LoadRegressionWeights-E- Problem in file %s/regressions/list_regressions.dat\nfile malformed, expected pattern %d found pattern: %d", weightsPath.Data(), ipt, ptt));
	  return -1;
	}
	for (int iene = 0; iene<nenebin; iene++){
	  fin >> _mddd[ireg][iene][ipt];
	if (_verbose>999) cout <<  _mddd[ireg][iene][ipt] << " " ;
	}
	if (_verbose>999) cout << endl ;
      }
      for (int ipt = 0; ipt<nTrkCCpatts; ipt++){
	int ptt;
	fin >> ptt;
	if (ptt!=ipt){
	  PrintError(Form("TrkCCNozzoli::_LoadRegressionWeights-E- Problem in file %s/regressions/list_regressions.dat\nfile malformed, expected pattern %d found pattern: %d", weightsPath.Data(), ipt, ptt));
	  return -1;
	}
	for (int iene = 0; iene<nenebin; iene++){
	  fin >> _rmss[ireg][iene][ipt];
	if (_verbose>999) cout << _rmss[ireg][iene][ipt] << " " ;
	}
	if (_verbose>999) cout << endl ;
      }
    }
  }
  fin.close();
  if(_verbose>0) cout << Form("TrkCCNozzoli::_LoadRegressionWeights-I- Good reading of file %s/regressions/list_regressions.dat", weightsPath.Data())<<endl;

  for (int ireg=0;ireg<_lastreg;ireg++){
    for (int ipt=0;ipt<nTrkCCpatts;ipt++){
      for (int ien=0;ien<_nene[ireg];ien++){
	int emin = _reneth[ireg][ien];
	int emax = _reneth[ireg][1];
	if (ien!= 0 && ien==_nene[ireg]-1) emax = 5000; // last bin
	if (ien!= 0 && ien!=_nene[ireg]-1) emax = _reneth[ireg][ien+2]; //for intermediate bin there is chaining of windows 
	TString nomefile = Form("%s/regressions/out%d_%d_%d_%d.dat",weightsPath.Data(),ireg,ipt,emin,emax);
	ifstream fin2(Form("%s",nomefile.Data()));
	if (!fin2 || !(fin2.good())) {
	  PrintError(Form("TrkCCNozzoli::_LoadRegressionWeights-E-Problem in opening file %s", nomefile.Data()));
	  return -1;
	}
	if (_verbose>0) cout << Form("TrkCCNozzoli::_LoadRegressionWeights-I-opening file %s", nomefile.Data())<<endl;
	for (int ipar=1;ipar<=3;ipar++){
	  int para,ptt;
	  fin2 >> para;
	  fin2 >> ptt;
	  int femin,femax;
	  fin2 >> femin;
	  fin2 >> femax;
	  if (para!=ipar){
	    PrintError(Form("TrkCCNozzoli::_LoadRegressionWeights-E- Problem in file %s\nfile malformed, expected parameter %d found parameter: %d", nomefile.Data(), ipar, para));
	    return -1;
	  }
	  if (ptt!=ipt){
	    PrintError(Form("TrkCCCNozzoli::_LoadRegressionWeights-E- Problem in file %s\nfile malformed, expected pattern %d found pattern: %d", nomefile.Data(), ipt, ptt));
	    return -1;
	  }
	  if (femin!=emin){
	    PrintError(Form("TrkCCCNozzoli::_LoadRegressionWeights-E- Problem in file %s\nfile malformed, expected emin %d found %d", nomefile.Data(), emin, femin));
	    return -1;
	  }
	  
	  if (femax!=emax){
	    PrintError(Form("TrkCCCNozzoli::_LoadRegressionWeights-E- Problem in file %s\nfile malformed, expected emax %d found %d", nomefile.Data(), emax, femax));
	    return -1;
	  }
	  float* appo = new float[_nvars];
          for (int ivar=0;ivar<_nvars;ivar++){
            fin2 >> appo[ivar];
            if (_verbose>999) cout << appo[ivar] << " " ;
          }
          if (_verbose>999) cout << endl ;
          if (ipar==3) NormWeights(appo, _nvars);
          for (int ivar=0;ivar<_nvars;ivar++){
            _rweight[ireg][ipt][ipar-1][ien][ivar] = appo[ivar];
          }
	  if (appo) delete [] appo;
	}      
	fin2.close();
      }
    }
  }
  return 0;
}


TMVA::Reader* TrkCCPizzolotto::_InitBDTReader( TString bdtWeightsPath, TString bdtName){
  if( _verbose>=2) cout<<"TrkCCPizzolotto::_InitBDTReader-I- initialization "<<endl;

  TMVA::Reader* reader;
  TString roption = "!Color:Silent";
  if( _verbose>=9) roption = "!Color:!Silent";
  reader = new TMVA::Reader( roption.Data() );

  // Hypothesis: all patterns use the same list of variables!
  // Initialise reader with used variables
  TString weightfile = Form("%s/classification_patt5_BDT.weights.xml",bdtWeightsPath.Data()) ;
  float spectvar[10];// note: at the moment spectators are decleared because BDT needs it. but they are ignored! 
  vector <TString> bdtvars;
  TMVAxmlReader* xmlgetter = new TMVAxmlReader();
  Bool_t debug = false;
  if( _verbose>=1)  debug = true;
  xmlgetter->Get(bdtvars,weightfile, debug);
  Int_t size = bdtvars.size() ;
  Int_t nvars=0, nspect=0;
  if(bdtvars.at(size-2).IsDigit()==true)   nvars = atoi( bdtvars.at(size-2));
  if(bdtvars.at(size-1).IsDigit()==true)  nspect = atoi( bdtvars.at(size-1));
  if (nspect==-1) cout << "TrkCCPizzolotto::_InitBdtReader-E-nspect"<< endl;
  for(int vv=0; vv<nvars; vv++){

    // get index from name
    TString name = bdtvars.at(vv);
    name.ReplaceAll("fnorm","");
    name.ReplaceAll("[","");
    name.ReplaceAll("]","");
    Int_t iv = -1;
    if( name.IsDigit()==true ){
      iv = atoi( name.Data() );
    }

    if( iv>=0 ){
      reader->AddVariable( Form("fnorm[%d]",iv) , &_varnorm[iv]);
    }
  }
  for(int vv=nvars; vv<(size-2); vv++){
    reader->AddSpectator( bdtvars.at(vv), &spectvar[vv-nvars]);
  }
  
  
  // BookMVA
  for( int patt=0; patt<nTrkCCpatts; patt++){
    TString method = Form("%sPatt%d",bdtName.Data(),patt) ;
    TString weight = Form("%s/classification_patt%d_BDT.weights.xml",bdtWeightsPath.Data(),patt) ;
    if(_verbose>=2) cout<<"TrkCCPizzolotto::_InitBdtReader-I- begin MethodName= "<<method.Data()<<"   file= "<<weight.Data()<<endl;
    reader->BookMVA(method, weight);
  }
  return reader;
}
 
//---------------------------
 
int MakeSubIdInt(TString subid) {
  
  if (subid=="A") return 0;
  else if (subid=="B") return 1;
  else if (subid=="C") return 2;
  else if (subid=="D") return 3;
  else if (subid=="E") return 4;
  else if (subid=="F") return 5;
  else if (subid=="G") return 6;
  else if (subid=="H") return 7;
  else if (subid=="I") return 8;
  else if (subid=="J") return 9;
  else {
    printf("Sub-id %s is not a valid sub-id...\n", subid.Data());
    return -99;
  }
}

#endif // _PGTRACK_
