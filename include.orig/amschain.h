#ifndef _AMSCHAIN_H
#define _AMSCHAIN_H

#include <vector>
#include "TObject.h"
#include "TChain.h"

#include "root.h"
#include "AMSNtupleHelper.h"
//class AMSEventR;

//!  AMSChain class
/*!
  Utility class to uniformly access the AMS ROOT Trees both from 
  interective and from programs, it simplify the interactive 
  analysis on AMS data with  Cint/Python/Ruby interpreters. 
  Example of use with Cint:

  \include ana.root_cint.C
  \date 16/08/2009 P. Zuccon: Scorporated from root.C and integrated 
    with user selection function feature

  \author juan.alcaraz@cern.ch
  

*/

class AMSChain : public TChain {
private:
  int m_tree_entry;
  unsigned int fThreads;
  unsigned int fSize;
  AMSEventR* _EVENT;
  Int_t _ENTRY;
  const char* _NAME;
  Int_t _TREENUMBER;
  TFile* _FILE;
  TTree* amsnew; ///used for cloned tree
  TFile* fout;

  ///Get AMSEventR in entry number "entry", if kLocal is false the entry number is "global", if true the entry number is local w.r.t. curret tree
  AMSEventR* _getevent(Int_t entry, Bool_t kLocal=false);

public:
   Long64_t ntree;
   Long64_t nentries;
  int get_tree_entry()const {return m_tree_entry;}
  unsigned int get_run () const{return _EVENT?_EVENT->Run():0;}
  /// Default constructor (it builds automatically the AMSEventR object)
  AMSChain(const char* name="AMSRoot", unsigned int thr=1,unsigned int size=sizeof(AMSEventR))
    :TChain(name),fThreads(thr),fSize(size),_ENTRY(-1),m_tree_entry(-1),_NAME(name),_EVENT(NULL),_TREENUMBER(-1),_FILE(0),fout(0),amsnew(0){}
  char * getsetup();
  
  /// alternative constructor (It requires an AMSEventR object to be passed)
  AMSChain(AMSEventR* event ,const char* name="AMSRoot",unsigned int thr=1, unsigned int fSize=sizeof(AMSEventR)); 
  
  /// Destructor
  virtual ~AMSChain(){ if(fout) CloseOutputFile(); fout=0;_FILE=0;if (_EVENT) delete _EVENT; };
  
  ///Set event branch and links; called after reading of all trees; called automatically in GetEvent
  void Init(AMSEventR* event=0); 
  
  ///Get next AMSEventR object in the chain
  AMSEventR* GetEvent(); 
  
  ///Get AMSEventR in entry number "entry" (the entry number is "global", i.e. along the chain)
  AMSEventR* GetEvent(Int_t entry);
  AMSEventR* GetEventGlobal(Int_t entry);

  ///Get AMSEventR in entry number "entry" (the entry number is "local", i.e. along the curret tree)
  AMSEventR* GetEventLocal(Int_t entry);
  
  ///Get AMSEventR in entry number "entry" appling the user selection function (if defined)
  /// Return 1 if the entry number "entry" satisfies the  user selection function
  /// Return 0 otherwhise 
  /// Retrun -1 of eof
  Int_t ReadOneEvent(Int_t entry);
  
  ///Get AMSEventR with run number "run" and event number "ev"
  AMSEventR* GetEvent(UInt_t run, Int_t ev, Bool_t kDontRewind=false); 

  //  bool   getevent(unsigned int run, unsigned int event);
  
  ///Rewind the chain (go back before first entry)
  void Rewind() {_ENTRY=-1;_TREENUMBER=-1;}; 
  
  ///Get the current entry number
  Int_t Entry(){return _ENTRY;}
  
  ///Get the current event pointer
  AMSEventR* pEvent() {return _EVENT;} 
  
  ///Get the name of the tree
  const char* ChainName(){return _NAME;} 
  
  ///Load the user event selection function compiling the selected ile
  int  LoadUF(char* fname="AMSNtupleSelect.C");
  
  /// Generate a skeleton to be used as  user event selection function
  int GenUFSkel(char* fname="AMSNtupleSelect.C");

  /// Opens the file to output selected events
  void OpenOutputFile(const char* filename);
  /// Saves the current entry to the output file (if it is not open, it creates SelectedEvents.root)
  void SaveCurrentEvent();
  /// Saves the current entry to the output file without GetAllContents()
  void SaveCurrentEventCont();
  /// Properly closes the Output File for selected events
  void CloseOutputFile();

   using TTree::Process;
   Long64_t  Process(TSelector*pev, Option_t *option="", Long64_t nentries=1000000000000LL, Long64_t firstentry=0); // *MENU*
  
  ClassDef(AMSChain,5);       //AMSChain
#pragma omp threadprivate(fgIsA)
};
    
    
//!  AMSEventList class
/*!
  Utility class, to select a set of events and then having the possibility to write
  them into a new files, possibly with just a set of selected branches. 
  Example:
      
  \include select_entries.C
      
  \author juan.alcaraz@cern.ch
      
*/
    
class AMSEventList {
private:
  vector<int> _RUNs;
  vector<int> _EVENTs;
  
public:
  AMSEventList(); ///< Default Constructor
  AMSEventList(const char* filename); ///< Constructor with an already existing list
  virtual ~AMSEventList(){};

  void Add(int run, int event); ///<Add a (run,event) number to the list
  void Add(AMSEventR* pev); ///<Add the (run,event) number of this AMSEventR object to the list
  void Remove(int run, int event); ///<Remove a (run,event) number from the list
  void Remove(AMSEventR* pev); ///<Remove the (run,event) number of this AMSEventR object from the list
  bool Contains(int run, int event); ///<Returns true if the list contains this (run,event)
  bool Contains(AMSEventR* pev); ///<Returns true if the list contains the (run,event) of this AMSEventR object
  void Reset(); ///<Reset the array
  void Read(const char* filename); ///<Read "run event" list from ASCII file
  void Write(); ///<Write "run event" list to standard output
  void Write(const char* filename); ///<Write "run event" list to ASCII file
  void Write(AMSChain* chain, TFile* file); ///<Write/Add selected events from a chain into a ROOT file

  int GetEntries(); ///<Number of events in the list
  int GetRun(int i); ///<Retrieve run number for entry i
  int GetEvent(int i); ///<Retrieve event number for entry i

  ClassDef(AMSEventList,2)       //AMSEventList
#pragma omp threadprivate(fgIsA)
    };


#endif
