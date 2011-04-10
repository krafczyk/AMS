/*
  This program is and edit of the original ROOT hadd routine.
*/

#include "TrHistoMan.h"

#include "RConfig.h"
#include <string>
#include "TChain.h"
#include "TFile.h"
#include "THashList.h"
#include "TH1.h"
#include "TKey.h"
#include "TObjString.h"
#include "Riostream.h"
#include "TClass.h"
#include "TSystem.h"
#include <stdlib.h>

TList *FileList;
TFile *Target, *Source;
Bool_t noTrees;
Bool_t fastMethod;

int AddFile(TList* sourcelist, std::string entry, int newcomp) ;
int MergeRootfile( TDirectory *target, TList *sourcelist);
int CompressTimeNtuple();

//___________________________________________________________________________
int main( int argc, char **argv ) 
{

   if ( argc < 3 || "-h" == string(argv[1]) || "--help" == string(argv[1]) ) {
      cout << "Usage: " << argv[0] << " [-f] [-T] targetfile source1 [source2 source3 ...]" << endl;
      cout << "   This program will add monitoring files" << endl;
      cout << "   If -f (\"force\") is given the outputfile is overridden." << endl;
      cout << "   If the first argument is -T, Trees are not merged" <<endl;
      cout << "   If  only one source file is indicated it will extract in the target file only the relevant monitoring infos." << endl; 
      return 1;
   }
   FileList = new TList();

   Bool_t force = (!strcmp(argv[1],"-f") || !strcmp(argv[2],"-f"));
   noTrees = (!strcmp(argv[1],"-T") || !strcmp(argv[2],"-T"));
   Int_t newcomp = 1;
   char ft[4];
   for (int j=0;j<=9;j++) {
      sprintf(ft,"-f%d",j);
      if (!strcmp(argv[1],ft) || !strcmp(argv[2],ft)) {
         force = kTRUE;
         newcomp = j;
         break;
      }
   }
 
   gSystem->Load("libTreePlayer");
 
   int ffirst = 2;
   if (force) ffirst++;
   if (noTrees) ffirst++;

   cout << "Target file: " << argv[ffirst-1] << endl;
   
   Target = TFile::Open( argv[ffirst-1], (force?"RECREATE":"CREATE") );
   if (!Target || Target->IsZombie()) {
      cerr << "Error opening target file (does " << argv[ffirst-1] << " exist?)." << endl;
      cerr << "Pass \"-f\" argument to force re-creation of output file." << endl;
      exit(1);
   }
   Target->SetCompressionLevel(newcomp);
  
   // by default hadd can merge Trees in a file that can go up to 100 Gbytes
   Long64_t maxsize = 100000000; //100GB
   maxsize *= 100;  //to bypass some compiler limitations with big constants
   TTree::SetMaxTreeSize(maxsize);
  
   fastMethod = kTRUE;
   for ( int i = ffirst; i < argc; i++ ) {
      if( AddFile(FileList, argv[i], newcomp) !=0 ) return 1;
   }
   if (!fastMethod) {
      cout <<"Sources and Target have different compression levels"<<endl;
      cout <<"Merging will be slower"<<endl;
   }

   int status = MergeRootfile( Target, FileList);

   // must delete Target to avoid a problem with dictionaries in~ TROOT
   delete Target;

   /* DISABLED (WRITTEN ONCE, READ MANY ...)
   Target = TFile::Open(argv[ffirst-1],"UPDATE");
   // as last step reduce the timentuple size! 
   CompressTimeNtuple(); 
   delete Target; 
   */

   return status;
}

//___________________________________________________________________________
int AddFile(TList* sourcelist, std::string entry, int newcomp) 
{
   // add a new file to the list of files
   static int count(0);
   if( entry.empty() ) return 0;
   size_t j =entry.find_first_not_of(' ');
   if( j==std::string::npos ) return 0;
   entry = entry.substr(j);
   if( entry.substr(0,1)=="@"){
      std::ifstream indirect_file(entry.substr(1).c_str() );
      if( ! indirect_file.is_open() ) {
         std::cerr<< "Could not open indirect file " << entry.substr(1) << std::endl;
         return 1;
      }
      while( indirect_file ){
         std::string line;
         std::getline(indirect_file, line);
         if( AddFile(sourcelist, line, newcomp)!=0 )return 1;;
      }
      return 0;
   }
   cout << "Source file " << (++count) << ": " << entry << endl;

   TFile* source = TFile::Open(entry.c_str());
   if (source==0) {
     cout << "No file named " << entry.c_str() << " skipping." << endl;
     return 0; 
  }
   if (source->TestBit(TFile::kRecovered)) {
     cout << "The file " << entry.c_str() << " was not closed, skipping." << endl;
     return 0;
   }
   sourcelist->Add(source);
   if (newcomp != source->GetCompressionLevel())  fastMethod = kFALSE;
   return 0;
}


//___________________________________________________________________________
int MergeRootfile( TDirectory *target, TList *sourcelist) 
{
   // Merge all objects in a directory
   int status = 0;
   cout << "Target path: " << target->GetPath() << endl;
   TString path( (char*)strstr( target->GetPath(), ":" ) );
   path.Remove( 0, 2 );

   TDirectory *first_source = (TDirectory*)sourcelist->First();
   Int_t nguess = sourcelist->GetSize()+1000;
   THashList allNames(nguess);
   ((THashList*)target->GetList())->Rehash(nguess);
   ((THashList*)target->GetListOfKeys())->Rehash(nguess);
   TList listH;
   TString listHargs;
   listHargs.Form("((TCollection*)0x%lx)",&listH);
   while(first_source) {
      TDirectory *current_sourcedir = first_source->GetDirectory(path);
      if (!current_sourcedir) {
         first_source = (TDirectory*)sourcelist->After(first_source);
         continue;
      }

      // loop over all keys in this directory
      TChain *globChain = 0;
      TIter nextkey( current_sourcedir->GetListOfKeys() );
      TKey *key, *oldkey=0;
      //gain time, do not add the objects in the list in memory
      TH1::AddDirectory(kFALSE);
  
      while ( (key = (TKey*)nextkey())) {
         if (current_sourcedir == target) break;
         //keep only the highest cycle number for each key
         if (oldkey && !strcmp(oldkey->GetName(),key->GetName())) continue;
         if (!strcmp(key->GetClassName(),"TProcessID")) {key->ReadObj(); continue;}
         if (allNames.FindObject(key->GetName())) continue;
         TClass *cl = TClass::GetClass(key->GetClassName());
         if (!cl || !cl->InheritsFrom(TObject::Class())) {
            cout << "Cannot merge object type, name: " 
                 << key->GetName() << " title: " << key->GetTitle() << endl;
            continue;
         }
         allNames.Add(new TObjString(key->GetName()));
         // read object from first source file
         //current_sourcedir->cd();
         TObject *obj = key->ReadObj();
         //printf("keyname=%s, obj=%x\n",key->GetName(),obj);
      
         /* AO begin */ 
         if ( obj->IsA()->InheritsFrom("TObjString") ) continue;
         if ( strcmp(obj->GetName(),"AMSRoot")==0) continue;
         if ( strcmp(obj->GetName(),"AMSRootSetup")==0) continue;  
         if ( strcmp(obj->GetName(),"datacards")==0) continue;
         if ( strcmp(obj->GetName(),"AMS02Geometry")==0) continue;
         if ( strcmp(obj->GetName(),"TkDBc")==0) continue;
         if ( strcmp(obj->GetName(),"TrParDB")==0) continue;
         if ( strcmp(obj->GetName(),"TrReconPar")==0) continue;  
         /* AO end */

         if (obj->IsA()->InheritsFrom("TTree")!=0) {
            // loop over all source files create a chain of Trees "globChain"
            if (!noTrees) {
               TString obj_name;
               if (path.Length()) {
                  obj_name = path + "/" + obj->GetName();
               } else {
                  obj_name = obj->GetName();
               }
               globChain = new TChain(obj_name);
               globChain->Add(first_source->GetName());
               TFile *nextsource = (TFile*)sourcelist->After( first_source );
               while ( nextsource ) {     	  
                  //do not add to the list a file that does not contain this Tree
                  TFile *curf = TFile::Open(nextsource->GetName());
                  if (curf) {
                     Bool_t mustAdd = kFALSE;
                     if (curf->FindKey(obj_name)) {
                        mustAdd = kTRUE;
                     } else {
                        //we could be more clever here. No need to import the object
                        //we are missing a function in TDirectory
                        TObject *aobj = curf->Get(obj_name);
                        if (aobj) { mustAdd = kTRUE; delete aobj;}
                     }
                     if (mustAdd) {
                        globChain->Add(nextsource->GetName());
                        cout << "NextSourceName: " << nextsource->GetName() << endl;
                     }
                  }
                  delete curf;
                  nextsource = (TFile*)sourcelist->After( nextsource );
               }
            }
         } else if ( obj->IsA()->InheritsFrom( "TDirectory" ) ) {
            // it's a subdirectory

            cout << "Found subdirectory " << obj->GetName() << endl;
            // create a new subdir of same name and title in the target file
            target->cd();
            TDirectory *newdir = target->mkdir( obj->GetName(), obj->GetTitle() );

            // newdir is now the starting point of another round of merging
            // newdir still knows its depth within the target file via
            // GetPath(), so we can still figure out where we are in the recursion
            status = MergeRootfile( newdir, sourcelist);
            if (status) return status;

         } else if ( obj->InheritsFrom(TObject::Class())
              && obj->IsA()->GetMethodWithPrototype("Merge", "TCollection*") ) {
            // object implements Merge(TCollection*)

            // loop over all source files and merge same-name object
            TFile *nextsource = (TFile*)sourcelist->After( first_source );
            while ( nextsource ) {
               // make sure we are at the correct directory level by cd'ing to path
               TDirectory *ndir = nextsource->GetDirectory(path);
               if (ndir) {
                  ndir->cd();
                  TKey *key2 = (TKey*)gDirectory->GetListOfKeys()->FindObject(key->GetName());
                  if (key2) {
                     TObject *hobj = key2->ReadObj();
                     hobj->ResetBit(kMustCleanup);
                     listH.Add(hobj);
                     Int_t error = 0;
                     obj->Execute("Merge", listHargs.Data(), &error);
                     if (error) {
                        cerr << "Error calling Merge() on " << obj->GetName()
                             << " with the corresponding object in " << nextsource->GetName() << endl;
                     }
                     listH.Delete();
                  }
               }
               nextsource = (TFile*)sourcelist->After( nextsource );
            }
         } else {
            // object is of no type that we can merge 
            cout << "Cannot merge object type, name: " 
                 << obj->GetName() << " title: " << obj->GetTitle() << endl;

            // loop over all source files and write similar objects directly to the output file
            TFile *nextsource = (TFile*)sourcelist->After( first_source );
            while ( nextsource ) {
               // make sure we are at the correct directory level by cd'ing to path
               TDirectory *ndir = nextsource->GetDirectory(path);
               if (ndir) {
                  ndir->cd();
                  TKey *key2 = (TKey*)gDirectory->GetListOfKeys()->FindObject(key->GetName());
                  if (key2) {
                     TObject *nobj = key2->ReadObj();
                     nobj->ResetBit(kMustCleanup);
                     int nbytes1 = target->WriteTObject(nobj, key2->GetName(), "SingleKey" );
                     if (nbytes1 <= 0) status = -1;
                     delete nobj;
                  }
               }
               nextsource = (TFile*)sourcelist->After( nextsource );
            }
         }

         // now write the merged histogram (which is "in" obj) to the target file
         // note that this will just store obj in the current directory level,
         // which is not persistent until the complete directory itself is stored
         // by "target->Write()" below
         if ( obj ) {
            target->cd();
       
            //!!if the object is a tree, it is stored in globChain...
            if(obj->IsA()->InheritsFrom( "TDirectory" )) {
               //printf("cas d'une directory\n");
            } else if(obj->IsA()->InheritsFrom( "TTree" )) {
               if (!noTrees) {
                  globChain->ls();
                  if (fastMethod) globChain->Merge(target->GetFile(),0,"keep fast");
                  else            globChain->Merge(target->GetFile(),0,"keep");
                  delete globChain;
               }
            } else {
               int nbytes2 = obj->Write( key->GetName(), TObject::kSingleKey );
               if (nbytes2 <= 0) status = -1;
            }
         }
         oldkey = key;
         delete obj;
      } // while ( ( TKey *key = (TKey*)nextkey() ) )
      first_source = (TDirectory*)sourcelist->After(first_source);
   }
   // save modifications to target file
   target->SaveSelf(kTRUE);
   return status;
}

//___________________________________________________________________________
int CompressTimeNtuple() {
  // no trees?
  if (noTrees) return 0;
  // old tree
  TTree* ntuple = (TTree*) Target->FindObjectAny("timentuple");
  if (ntuple==0) return 0;
  TIME_EVENT time_event;
  ntuple->SetBranchAddress("timebranch",&time_event);
  int nentries = ntuple->GetEntries();
  int step = 1 + nentries/100;
  // new tree
  TTree* ntuple2 = (TTree*) ntuple->CloneTree(0);
  for (int i=0; i<nentries; i++) { 
    if ((i%step)==0) { 
      cout << i << endl;
      ntuple->GetEntry(i);
      ntuple2->Fill();
    }   
  }
  Target->cd("TrOnlineMon");
  Target->Delete("timentuple;*");
  ntuple2->Write();
  return 0;
}
