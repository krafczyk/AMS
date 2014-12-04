#include <iostream>
#include "TBranch.h"
#include "root.h"
#include "amschain.h"
#include <stdio.h>
#include "TObjString.h"
#include <fstream>
#include <TStreamerInfo.h>
#include <TRFIOFile.h>
#include <TCastorFile.h>
#include <TXNetFile.h>
#include <TRegexp.h>
#include <TEnv.h>
#include "TChainElement.h"

//    TRFIOFile f("");
//     TXNetFile g("");
//     TCastorFile h("");
int rootread(char * fname, int nev, int iver,  int & lastevent,bool jou){
//TStreamerInfo::fgInfoFactory=new TStreamerInfo();
 gEnv->SetValue("TFile.Recover", 0);


 int LastEvent=lastevent;
 int diff=0;
 lastevent=0;
int firstevent=-1;
 if(!iver){
  fclose(stderr);
  fclose(stdout);
  cout.setstate(ios_base::failbit);
  cerr.setstate(ios_base::failbit);
 }
 else{
  cout.clear();
  cerr.clear();
 }
 //TFile * rfile= new TFile(fname,"READ");
 //TFile *rfile=TFile::Open(fname,"READ");
         TRegexp d("^-root:",false);
        TRegexp e("^rfio:",false);
        TRegexp c("/castor/",false);
 TFile *rfile=0;
TString name(fname);
if(name.Contains(c)){
       string rn=fname;
       int pos=rn.find("/castor/");
       string stager_get="/usr/bin/stager_get -M ";
       stager_get+=(fname+pos);
       stager_get+=" 1>/dev/null 2>&1 &";
       system(stager_get.c_str());
       sleep(2);
       bool staged=false;
       bool stagein=false; 
       int again=0;
{
again2:
           stager_get="/usr/bin/stager_qry -M ";
           stager_get+=(fname+pos);
           stager_get+=" | grep -c STAGED 2>&1";
           FILE *fp=popen(stager_get.c_str(),"r");
           char path[1024];
           if(fp==NULL){
             staged=false;
           }
           else if(fgets(path, sizeof(path), fp) != NULL && strstr(path,"1")){
             staged=true;
           }
           else staged=false;
           pclose(fp);

}
if(!staged && name.Contains(e)){
           stager_get="/usr/bin/stager_qry -M ";
           stager_get+=(fname+pos);
           stager_get+=" | grep -c CANBEMIGR 2>&1";
           FILE *fp=popen(stager_get.c_str(),"r");
           char path[1024];
           if(fp==NULL){
             staged=false;
           }
           else if(fgets(path, sizeof(path), fp) != NULL && strstr(path,"1")){
             staged=true;
           }
           else staged=false;
           pclose(fp);
}
if(!staged){
           stager_get="stager_qry -M ";
           stager_get+=(fname+pos);
           stager_get+=" | grep -c 'No such file or directory' 2>&1";
           FILE *fp=popen(stager_get.c_str(),"r");
           char path[1024];
           if(fp==NULL){
             stagein=false;
           }
           else if(fgets(path, sizeof(path), fp) != NULL && strstr(path,"0")){
              if(iver>0)cout<<" stagein again "<<again<<endl;
             if(again<10){
                again++;
                pclose(fp);
                goto again2;
             }
             stagein=true;
           }
           else stagein=false;
           pclose(fp);
}
if(!stagein && !staged){
           stager_get="stager_qry -M ";
           stager_get+=(fname+pos);
           stager_get+=" | grep -c 'not on this service class' 2>&1";
           FILE *fp=popen(stager_get.c_str(),"r");
           char path[1024];
           if(fp==NULL){
             stagein=false;
           }
           else if(fgets(path, sizeof(path), fp) != NULL && strstr(path,"1")){
              if(iver>0)cout<<" stagein again "<<again<<endl;
             if(again<10){
                again++;
                pclose(fp);
                goto again2;
             }
             stagein=true;
           }
           else stagein=false;
           pclose(fp);
}
if(iver){
cout <<" STAGEIN STAGED "<<stagein<<" "<<staged<<endl;
}
if(!staged && !stagein){
//return -1;
}
else if(stagein)return -6; 
}

        if(name.Contains(d))rfile=new TXNetFile(fname,"READ");
        else if(name.Contains(e)){
       rfile=new TRFIOFile(fname,"READ");
       //rfile=new TCastorFile(fname,"READ");
       }
  else {
     AMSChain chain;
     chain.Add(fname);
           TObjArray* arr=chain.GetListOfFiles();
	  TIter next(arr);
	  TChainElement* el=(TChainElement*) next();
	  if(el){
           rfile=TFile::Open(el->GetTitle(),"READ");
          }
          else TFile::Open(fname,"READ");
  }
if(!rfile){
        if(iver>0)cout <<"problem to open file "<<fname<<" "<<endl;
	return -1;
 }
 TTree * tree=dynamic_cast<TTree*>(rfile->Get("AMSRoot"));
 if(!tree){
        if(iver>0)cout <<"problem to find tree AMSRoot "<<endl;
	return -1;
 }

 if(!rfile->Get("datacards/TKGEOMFFKEY_DEF"))return -1;
if(! rfile->Get("datacards/TRMCFFKEY_DEF"))return -1;
if(! rfile->Get("datacards/TRMCFFKEY_DEF"))return -1;
if(! rfile->Get("datacards/TRCALIB_DEF"))return -1;
if(! rfile->Get("datacards/TRCALIB_DEF"))return -1;
if(! rfile->Get("datacards/TRCLFFKEY_DEF"))return -1;
if(! rfile->Get("datacards/TRFITFFKEY_DEF"))return -1;
if(! rfile->Get("datacards/TRCHAFFKEY_DEF"))return -1;


 if(jou){
   TObjString s("");
   s.Read("DataCards");
   if(s.String().Length()>1){
     TString fnam(fname);
     fnam+=".jou";
     ofstream fout;
     fout.open((const char*)fnam);
     if(fout){
       fout <<s.String();
       fout.close();
     }
     else if(iver){
      cerr<<" Unable to open jou file "<<(const char*)fnam<<endl; 
     }  
   }
   else if(iver){
    cerr<<" Unable to find datacards in "<<fname<<endl;
   }
   
 } 
 int nevents=tree->GetEntries();
int ver;
 if(iver)cout <<"  entries "<<nevents<<" "<<nev<<endl;
 if(nevents<=0)return -2;
 else if(nev>nevents)return -3;
 else if(nev<nevents)return -4;
  AMSEventR *pev = new AMSEventR();
//  tree->SetMakeClass(1);
  pev->Init(tree);
  pev->GetBranch(tree);
  bool fast=true;
   bool castor=name.Contains(c);
again:
  int nbadev=0;
  int nevread=0;
  for (int i=0;i<nevents;i++){
   if(i==0){
      if(pev->ReadHeader(i))ver=pev->Version();
  }
   if(castor){
      if(!pev->ReadHeader(i))break;
      else return 0;
    }
   if(fast && nevents>1001 && i>1 && i<nevents-1000){
    nevread++;
    continue; 
   }
   else if(i==nevents-1001 &&  (!pev->ReadHeader(i))){
     fast=false;
     goto again;
   }
   else if(!pev->ReadHeader(i))break;
   if(iver && i%100==0)cout <<" i "<<i<<" "<<nevents<<endl;
   if((pev->fStatus/1073741824)%2)nbadev++;
   if(firstevent<0)firstevent=pev->fHeader.Event;
   if(lastevent && abs(lastevent-int(pev->fHeader.Event))>diff){
     diff=std::abs(lastevent-int(pev->fHeader.Event));
   }
   lastevent=pev->fHeader.Event;
   nevread++;
  }
  rfile->Close();
  if(nevread!=nev)return -2;
  if(nev>2 && LastEvent>0){
   float rate=(lastevent+1-firstevent)/nevread;
   if(iver)cout <<"last event "<<LastEvent <<" " <<lastevent<<" "<<rate<<" "<<diff<<endl; 
   if(iver)cout <<" LastEvent,,,"<<lastevent << ",,,"<<firstevent<<",,,"<<ver<<",,,"<<nevread<<endl;
   if(0 && abs(LastEvent-lastevent)>15*rate && abs(LastEvent-lastevent)>3*diff){
     return -5;
   } 
  }
  float rrr=100*float(nbadev)/float(nevread);
  return int(rrr); 
}


