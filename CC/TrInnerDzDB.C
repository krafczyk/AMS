//
//  TrInnerDzDB.C
//  TrInnerDzDB
//
//  Created by Zuccon Paolo on 19/04/12.
//  Copyright (c) 2012 INFN Perugia. All rights reserved.
//

#include <iostream>
#include "TrInnerDzDB.h"
#include "timeid.h"
#include "TFile.h"

ClassImp (TrInnerDzDB);
ClassImp (TrInnerDzDB::DzElem);
TrInnerDzDB* TrInnerDzDB::Head=0;
float TrInnerDzDB::TDVSwap[2*(1+kLaynum)];

int TrInnerDzDB::ForceFromTDV=0;

float TrInnerDzDB::LDZA[kLaynum]={0.,0.,0.,0.,0.,0.,0.};
uint  TrInnerDzDB::UTIME=0;

int TrInnerDzDB::UpdateTkDBc(uint Timeid){
  //  TkDBc* db=TkDBc::GetHead();
  float Dz[kLaynum];
  int ret=GetEntry(Timeid,Dz,1);
  if( ret >=0) {
    UTIME=Timeid;
    for (int ii=0;ii<kLaynum;ii++)
      LDZA[ii]=Dz[ii];
    return 0;
  }
  else if(ret ==-2 || ret==-3){ //Fall back to TDV
    int retTDV=GetFromTDV(Timeid);
    if(retTDV<0) return -2;
    int ret2=GetEntry(Timeid,Dz,1);
    if( ret2 >=0) {
      UTIME=Timeid;
      for (int ii=0;ii<kLaynum;ii++)
	LDZA[ii]=Dz[ii];
      return 0;
    }
    else return -3;    
  }
  return -4;
}

int TrInnerDzDB::GetEntry(uint Timeid, float* Dz, int kind){
  mapit it0,it1,itC;
  if(!Dz) return -1;
  it1=pos.upper_bound(Timeid);
  if (it1==pos.begin()) return -2;
  if (it1==pos.end())   return -3;
  it0=it1; it0--;
  uint T0=it0->first;
  uint T1=it1->first;
  int DT0=Timeid-T0;
  int DT1=T1-Timeid;
  if(DT0 < 0 || DT1 <0 ) return -5;
  if (kind ==0) { // return the closer
    itC=it1;
    if(DT0<DT1) itC=it0;
    for(int ii=0;ii<kLaynum;ii++)
      Dz[ii]=itC->second.dz[ii];
  }else if (kind ==1) { //linear interpolation
    for(int ii=0;ii<kLaynum;ii++)
      Dz[ii]=(it0->second.dz[ii]*DT1+it1->second.dz[ii]*DT0)/(DT0+DT1);
  }else
    return -4;
  return 0;
}


int TrInnerDzDB::GetFromTDV( uint Timeid, bool force) {
  time_t starttime=0;
  time_t endtime=0; 
  tm begin;
  tm end;
  gmtime_r(&starttime,&begin);
  gmtime_r(&endtime,&end);
    
  static AMSTimeID *tid=0;
#pragma omp threadprivate (tid)
  if (tid && force) { delete tid; tid = 0; }

  if(!tid){
    tid= new AMSTimeID(
		       AMSID("TrInnerDzAlign",1),
		       begin,
		       end,
		       TrInnerDzDB::GetTDVSwapSize(),
		       TrInnerDzDB::TDVSwap,
		       AMSTimeID::Standalone,
		       1,
		       TrInnerLin2DB);
  }
  memset(TDVSwap,0,GetTDVSwapSize());
  time_t tt=Timeid;
  int ret=tid->validate(tt);
  if(ret<0) return -1;
  else {
    cout << "TrInnerDzDB::GetFromTDV-I-GetFromTDV" << endl;
    Print();
    return 0;
  }
}

int TrInnerDzDB::DB2TDV(){
  for (mapit it= pos.begin();it!=pos.end();it++)
    TrInnerDB2Lin2TDV(it);
  return 0;
}


int TrInnerDzDB::TrInnerDB2Lin2TDV(mapit it0){
  mapit it1(it0); it1++;
  if (it0 == pos.end()|| it1==pos.end()) return -1;
    

  TDVSwap[0]         = *( (float*) &(it0->first) );
  TDVSwap[kLaynum+1] = *( (float*) &(it1->first) );

  for (int ii=0;ii<kLaynum;ii++){
    TDVSwap[ii+1]         = it0->second.dz[ii];
    TDVSwap[ii+kLaynum+2] = it1->second.dz[ii];
  }

  cout<<"Updating: "<<it0->first<<" "<<it1->first<<endl;
    
  time_t starttime=it0->first+3600;
  time_t endtime=it1->first+3600;
  tm begin;
  tm end;
  gmtime_r(&starttime,&begin);
  gmtime_r(&endtime,&end);
    
  AMSTimeID tid(
		AMSID("TrInnerDzAlign",1),
		begin,
		end,
		TrInnerDzDB::GetTDVSwapSize(),
		TrInnerDzDB::TDVSwap,
		AMSTimeID::Standalone,
		1);
  tid.UpdateMe();
  char database[200];
  char *ADD=getenv("AMSDataDir");
  if(!ADD) {printf("TrInnerDzDB::TrInnerDB2Lin2TDV-E- Cannot find the Enviromnet variable AMSDataDir!\n I give up in updating TDV!\n");
    return -2;
  }
  sprintf(database,"%s/DataBase/",ADD);

  int ret2=tid.write(database);

  if (ret2<0) return -3;
  return 0;
}

void TrInnerLin2DB(){
  float dz0[TrInnerDzDB::kLaynum],dz1[TrInnerDzDB::kLaynum];
  memset(dz0,0,TrInnerDzDB::kLaynum*sizeof(float));
  memset(dz1,0,TrInnerDzDB::kLaynum*sizeof(float));
  uint T0 = *(   (uint*) &(TrInnerDzDB::TDVSwap[0]) );
  uint T1 = *(   (uint*) &(TrInnerDzDB::TDVSwap[8]) );
    
   for (int ii=0;ii<TrInnerDzDB::kLaynum;ii++){
     dz0[ii]=TrInnerDzDB::TDVSwap[ii+1];
     dz1[ii]=TrInnerDzDB::TDVSwap[ii+TrInnerDzDB::kLaynum+2];
   }
   TrInnerDzDB * db=TrInnerDzDB::GetHead();
   db->AddEntry(T0,dz0);
   db->AddEntry(T1,dz1);
  return;
}



int TrInnerDzDB::TDV2DB(){
  vector<uint>times;

  time_t starttime=0;
  time_t endtime=0;
  tm begin;
  tm end;
  gmtime_r(&starttime,&begin);
  gmtime_r(&endtime,&end);
    
  AMSTimeID tid(
		AMSID("TrInnerDzAlign",1),
		begin,
		end,
		TrInnerDzDB::GetTDVSwapSize(),
		TrInnerDzDB::TDVSwap,
		AMSTimeID::Standalone,
		1,
		TrInnerLin2DB);
  int dbsize= tid.GetDataBaseSize();
  for (int ii=0;ii<dbsize;ii++){
    uint ltimes[5] = {0};
    tid.GetDBRecordTimes(ii,ltimes);
    times.push_back(ltimes[2]+1);
  }
  for (unsigned int jj=0;jj<times.size();jj++){
    time_t tt= (time_t)times[jj];
    tid.validate(tt);
  }
  return 0;
    
}

int TrInnerDzDB::ReadFromFile(char* filename){
  FILE * ff=fopen(filename,"r");
  if(!ff) return -1;

  while(1){
    uint t;
    float a,b,c,d,e,f,g,dz[kLaynum];
    fscanf(ff," %u  %e  %e  %e  %e  %e  %e  %e",&t,
	   &a,&b,&c,&d,&e,&f,&g);
    if(feof(ff)) break;
    if(ferror(ff)) return -2;
    dz[0]=a;dz[1]=b;dz[2]=c;
    dz[3]=d;dz[4]=e;dz[5]=f;dz[6]=g;
    AddEntry(t,dz);
  }
  return 0;
}
 
int TrInnerDzDB::WriteToFile(char* filename, int append){
 FILE * ff;
  if(append) 
    ff=fopen(filename,"a");
  else
    ff=fopen(filename,"w");
  if(!ff) return -1;
  for (mapit it=pos.begin();it!=pos.end();it++){

    fprintf(ff,"%9u ",it->first);
    if(ferror(ff)) return -2;
    for (int ii=0;ii<kLaynum;ii++){
      fprintf(ff,"%+12.5e ",it->second.dz[ii]);
      if(ferror(ff)) return -2;
    }
    fprintf(ff,"\n");
    if(ferror(ff)) return -2;
  }
  fclose(ff);
  return 0;
}

void TrInnerDzDB::Print(Option_t *) const
{
  typedef std::map<uint,TrInnerDzDB::DzElem>::const_iterator mapitc;
  for (mapitc it = pos.begin(); it != pos.end(); it++) {
    printf("%9u", it->first);
    for (int ii = 0; ii < kLaynum; ii++)
      printf(" %5.1f", it->second.dz[ii]*1e4);
    printf("\n");
  }
}


void TrInnerDzDB::Load(TFile * ff){
  if(!ff) return;

  if (Head) delete Head;
  Head = (TrInnerDzDB*) ff->Get("TrInnerDzDB");
  if(!Head)
    cout << "TrInnerDzDB::Load-W- Cannot Load the InnerDz Layer AlignmentDB"
	 << endl;
  else
    cout << "TrInnerDzDB::Load-I- Loaded from: " << ff->GetName() 
	 << " Size= " << Head->pos.size() << endl;
  return;
}
