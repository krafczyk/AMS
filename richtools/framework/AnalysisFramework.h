#ifndef __ANALYSIS_SKELETON__
#define __ANALYSIS_SKELETON__

/*****************************************************
This file defines the structure to be used in the analysis
 *****************************************************/
// Minimum set of definitions 
#define _PGTRACK_
#include "root_RVSP.h"

// Maps, string and vector (useful containers)
#include<map>
#include<string>
#include<vector>


// Simplification of part of the code
#include "simplify.h"

/******************************************************

Define the structure of the analysis here. Most definitions
should remain untouched.

*******************************************************/
using namespace std;

class AnalysisFramework : public AMSEventR {
   public :


   /////////////// Helper macros and definitions ///////////////

   /*******************************************************
        Tools for managing cuts 
   ********************************************************/

   map<string,unsigned int>   cuts_entries; // Entries for a given cut
   vector<string>             cuts_order;
   vector<bool>               cuts_isMask; 
   map<string,bool>           cuts_passed; // Cuts passed for a single event
   map<string,bool>::iterator cuts_it;      // Helper
   
   bool select(string name,bool condition,bool isMask=false){
     if(cuts_passed.find(name)==cuts_passed.end()){
       cuts_order.push_back(name);
       cuts_passed[name]=false;
       cuts_entries[name]=0;
       cuts_isMask.push_back(isMask);      
     }
     if(condition){cuts_passed[name]=true;cuts_entries[name]++;}
     return condition;
   }
   
   
   void reportCuts(){
     unsigned int all_entries=cuts_entries[cuts_order[0]];
     unsigned int prev_entries=all_entries;
     printf("\nCuts Efficiency report ------------------------------------------\n");
     for(int i=0;i<cuts_order.size();i++){
       string &name=cuts_order[i];
       unsigned int entries=cuts_entries[name];
       if(cuts_isMask[i]){
	 printf("Mask ---     %35s             Entries %8u   Efficiency %8.5g\n",
		name.c_str(),
		entries,
		double(entries)/double(prev_entries)
		);
       }else{
	 printf("Cut %35s    Entries %8u   Efficiency %8.5g   Total eff %8.5g\n",
		name.c_str(),
		entries,
		double(entries)/double(prev_entries),
		double(entries)/double(all_entries)
		);
	 prev_entries=entries;
       }
     }
     printf("\n");
   }
   
   
   void resetCuts(){
     cuts_order.clear();
     cuts_entries.clear();
     cuts_passed.clear();
   }

   void initCuts(){
     for(int i=0;i<cuts_order.size();i++) cuts_passed[cuts_order[i]]=false;
   }
   /******************************************************************************/

  // Constructor. Keep it here, although rarely used

  AnalysisFramework(TTree *tree=0){};


/// This is necessary if you want to run without compilation as:
///     chain.Process("stlv.C");
/// within root
#ifdef __CINT__
#include <process_cint.h>
#endif

   ///////////////////////////////// Further hooks for better analysis
  virtual void init(){};                // Called at the beginning of the loop: Init histograms and other stuff 
  virtual bool preProcess(){return true;};          // Called at the beginning of the event: put your cuts here
  virtual void process(){};             // Fill your histograms here   
  virtual void postProcess(){};         // Called after the event: perform any required cleanup here 
  virtual void finish(){};              // Called at the end of the loop


   /// User Function called before starting the event loop.
   /// In this function the user should book all the histograms 
  virtual void    UBegin(){CUTS_CLEAR;init();}

  /// User Function called to preselect events (NOT MANDATORY).
  /// Called for all entries at the very beginning. In principle
  /// intended to cut events
  virtual bool   UProcessCut(){CUTS_NEW_EVENT;return preProcess();}
  
  /// User Function called to analyze each event.
  /// Called for all entries.
  /// Fills histograms here.
  virtual void    UProcessFill(){process();postProcess();}
  
  /// Called at the end of a loop on the tree,
  /// a convenient place to draw/fit/write your histograms.
  virtual void    UTerminate(){finish();CUTS_REPORT;}

};

namespace Framework {};
using namespace Framework;

#endif
