#include "root_RVSP.h"
#include "HistoMan.h"

/** 
 * This is an example of how to work with AMS Root Files. 
 *  \sa stlv
 */



///  This is an example of user class to process AMS Root Files 
/*!
  This class is derived from the AMSEventR class. \n
  
  The following member functions must be called by the user: \n
  void UBegin():       called everytime a loop on the tree starts,
  a convenient place to create your histograms. \n
  void UProcessFill(): called in the entry loop for all entries \n
  void UTerminate():   called at the end of a loop on the tree,
  a convenient place to draw/fit your histograms. \n
  
  The following member function is optional: \n
  bool UProcessCut(): called in the entry loop to accept/reject an event \n
  
  To use this file, try the following session on your Tree T: \n
  Root > T->Process("stlv.C+") -> compile library if not up to date \n
  Root > T->Process("stlv.C++") -> force always compilation \n
  Root > T->Process("stlv.C")  -> non-compiled (interpreted) mode \n
*/
static int runs[1000];
static int events[1000];
static int nruns=0;
class daqePG : public AMSEventR {
public :

  daqePG(TTree *tree=0){ };


  /// This is necessary if you want to run without compilation as:
  ///     chain.Process("stlv.C");
#ifdef __CINT__
#include <process_cint.h>
#endif

  /// User Function called before starting the event loop.
  /// Book Histos
  virtual void    UBegin();

  /// User Function called to preselect events (NOT MANDATORY).
  /// Called for all entries.
  virtual bool    UProcessCut();
  virtual bool    UProcessStatus(unsigned long long status);

  /// User Function called to analyze each event.
  /// Called for all entries.
  /// Fills histograms.
  virtual void    UProcessFill();

  /// Called at the end of a loop on the tree,
  /// a convenient place to draw/fit your histograms. \n
  virtual void    UTerminate();
  ClassDef(daqePG,1);
};


//create here pointers to histos and/or array of histos

ofstream ftxt;
ofstream ftxt1;


///****************************************************************************
void daqePG::UBegin()
///****************************************************************************
{
  cout << " Begin calling"<<endl;
  ftxt.open("selectaiee.txt");
  ftxt1.open("eventsaiee.txt");
  for(int i=0;i<sizeof(runs)/sizeof(runs[0]);i++)runs[i]=0;

  cout <<" Booking Histos"<<endl;
  hman.Add(new TH1F("nTrTrack","nTrTrack",10,0,20));
  hman.Enable();
  cout <<"  Histos Booked"<<endl;
  cout << " Begin called now"<<endl;
  
  

}


///****************************************************************************
bool daqePG::UProcessStatus(unsigned long long status)
///****************************************************************************
{
  if( !(status & 3) ||  !((status>>4)&1))return false;
  else return true;
}


///****************************************************************************
// UProcessCut() is not mandatory
bool daqePG::UProcessCut()
///****************************************************************************
{
  static char f[1024]="";
  const char * fname=_Tree->GetCurrentFile()->GetName();

  if(strcmp(f,fname)){
    cout <<" getfile "<<_Tree->GetCurrentFile()->GetName()<<endl;
    strcpy(f,fname); 
  }
  
  if (nParticle()==0  || nTrTrack()==0   ) return false;
  return true;
}


///****************************************************************************
void daqePG::UProcessFill()
///****************************************************************************
{
  try{
    hman.Fill("nTrTrack",nTrTrack());

    bool ecm=false;
    bool mc=false;
    float tofcoo[4][6];
    float clcoo[20][3];
    const int nbadl=3;
    int blay[3]={4,6,8};
    int blad[3]={7,12,7};
    int bhalf[3]={1,1,0};
    // User Function called for all entries .
    // Entry is the entry number in the current tree.
    // Fills histograms.
    //cout<<Run()<<" "<<Event()<<endl; 
    static int runo=0;
    //#pragma omp threadprivate (runo)
    if(Run()!=runo){
      cout <<"  tracks found run "<<Run()<<endl;
      runs[++nruns]=Run();
      events[nruns]=0;
      runo=Run();
    }   
    events[nruns]++;
    //if(events[nruns]>10000)return;
    // cout <<"calling "<<nTrTrack()<<endl;
    


  }
  catch (...)
    {
      static int i=0;
      if(i++<100);cout <<"  daqe exception catched "<<Run()<< " " <<Event()<<endl;
      //abort();
      return ;
    }
  
}


///****************************************************************************
///****************************************************************************
void daqePG::UTerminate()
///****************************************************************************
{
  ftxt.close();
  ftxt1.close();
  int evt=0;


  for(int i=0;i<sizeof(runs)/sizeof(runs[0]);i++){
    if(runs[i]){
      evt+=events[i];
      cout <<runs[i]<<" "<<events[i]<<" "<<evt<<endl;
    }
    else break;  
  }
  for(int i=0;i<sizeof(runs)/sizeof(runs[0]);i++){
    if(runs[i]){
      cout <<runs[i]<<",";
    }
    else break;

  }
  TH1F* hh=(TH1F*)hman.Get("nTrTrack");

  hh->Print();
  sleep(20);
  
  hman.Save();

  cout << " The end of UTerminate"<<endl;
  cout <<endl;
}

