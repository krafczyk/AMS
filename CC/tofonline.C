// Author D. Casadei 21 Feb 1998
// TOF Online histograms booking
#include <job.h>
#include <cern.h>
#include <typedefs.h>
#include <tofdbc.h>
#include <tofsim.h>
//#include <stdio.h>//DEBUG

extern TOFBrcal scbrcal[SCLRS][SCMXBR];// calibration data

void AMSJob::_retofonlineinitjob(){
//   Trigger (JL1 hits) histograms filleded in trigger1.C
//   (in TriggerLVL1::buildraw(...))
//   For each side:

  char string[64];

  cout<<"...TOF monitoring selected"<<endl;

  //  FILE *fd;                                     //DEBUG
  //  fd=fopen("online_booked_histograms.log","w"); //DEBUG

  HBOOK1(5013,"Triggers",1,0,1,0.); // for the normalization

#define JL1START 5000
  HBOOK1(5000,"Plane 1-P JL1 Triggers",14,.5,14.5,0.);
  HBOOK1(5001,"Plane 2-P JL1 Triggers",14,.5,14.5,0.);
  HBOOK1(5002,"Plane 3-P JL1 Triggers",14,.5,14.5,0.);
  HBOOK1(5003,"Plane 4-P JL1 Triggers",14,.5,14.5,0.);
  HBOOK1(5004,"Plane 1-N JL1 Triggers",14,.5,14.5,0.);
  HBOOK1(5005,"Plane 2-N JL1 Triggers",14,.5,14.5,0.);
  HBOOK1(5006,"Plane 3-N JL1 Triggers",14,.5,14.5,0.);
  HBOOK1(5007,"Plane 4-N JL1 Triggers",14,.5,14.5,0.);  
  // AND of sides:
  HBOOK1(5008,"Plane 1 JL1 Triggers",14,.5,14.5,0.);
  HBOOK1(5009,"Plane 2 JL1 Triggers",14,.5,14.5,0.);
  HBOOK1(5010,"Plane 3 JL1 Triggers",14,.5,14.5,0.);
  HBOOK1(5011,"Plane 4 JL1 Triggers",14,.5,14.5,0.);

  // Trigger (data) histograms filled in tofonlinefill1(...)
  // For each side:
#define HITSTART 5015
  for(int i=0;i<8;i++){
    int k=1,hisID;
    char c;
    hisID=HITSTART+i;
    if(hisID<HITSTART+4){
      c='P';
      k=i+1;
    }
    else{
      c='N';
      k=i-3;
    }
    sprintf(string,"Plane %1d-%1c Hits",k,c);
    HBOOK1(hisID,string,14,.5,14.5,0.);
    //    fprintf(fd,"hisID = %d\t%s\n",hisID,string);//DEBUG
  }
  // AND of sides
  // filled in AMSTOFRawCluster::build(...) (see tofrec.C)
  for(i=8;i<12;i++){
    int k=1,hisID;
    hisID=HITSTART+i;
    k=i-7;
    sprintf(string,"Plane %1d Hits",k);
    HBOOK1(hisID,string,14,.5,14.5,0.);
    //    fprintf(fd,"hisID = %d\t%s\n",hisID,string);//DEBUG
  }
  //  cerr<<"Booked HIT his"<<endl;//DEBUG

  // Edges histograms filled in tofonlinefill1(...)
  // History: 5030 - 5147
#define HISSTART 5030
  for(int start=HISSTART, inc=0;inc<4;inc++,start+=30){
    for(int i=0;i<28;i++){
      int k=1,hisID;
      char c;
      hisID=start+i;
      if(hisID<start+14){
	c='P';
	k=i+1;
      }
      else{
	c='N';
	k=i-13;
      }
      sprintf(string,"%1d.%02d%c - History Edges",(inc+1),k,c);
      HBOOK1(hisID,string,17,-.5,16.5,0.);
      //      fprintf(fd,"hisID = %d\t%s\n",hisID,string);//DEBUG
    }
  }
  //  cerr<<"Booked HIS his"<<endl;//DEBUG
  // Time Expansion: 5150 - 5267
#define TDCSTART 5150
  for(start=TDCSTART, inc=0;inc<4;inc++,start+=30){
    for(int i=0;i<28;i++){
      int k=1,hisID;
      char c;
      hisID=start+i;
      if(hisID<start+14){
	c='P';
	k=i+1;
      }
      else{
	c='N';
	k=i-13;
      }
      sprintf(string,"%1d.%02d%c - Time Expansion Edges",(inc+1),k,c);
      HBOOK1(hisID,string,17,-.5,16.5,0.);
      //      fprintf(fd,"hisID = %d\t%s\n",hisID,string);//DEBUG
    }
  }
  //  cerr<<"Booked TDC his"<<endl;//DEBUG
  // Anode: 5270 - 5487  
#define ANOSTART 5270
  for(start=ANOSTART, inc=0;inc<4;inc++,start+=30){
    for(int i=0;i<28;i++){
      int k=1,hisID;
      char c;
      hisID=start+i;
      if(hisID<start+14){
	c='P';
	k=i+1;
      }
      else{
	c='N';
	k=i-13;
      }
      sprintf(string,"%1d.%02d%c - Anode Edges",(inc+1),k,c);
      HBOOK1(hisID,string,17,-.5,16.5,0.);
      //      fprintf(fd,"hisID = %d\t%s\n",hisID,string);//DEBUG
    }
  }
  //  cerr<<"Booked ANO his"<<endl;//DEBUG
  // Dynode Edges: 5490 - 5607
#define DYNSTART 5490
  for(start=DYNSTART, inc=0;inc<4;inc++,start+=30){
    for(int i=0;i<28;i++){
      int k=1,hisID;
      char c;
      hisID=start+i;
      if(hisID<start+14){
	c='P';
	k=i+1;
      }
      else{
	c='N';
	k=i-13;
      }
      sprintf(string,"%1d.%02d%c - Dynode Edges",(inc+1),k,c);
      HBOOK1(hisID,string,17,-.5,16.5,0.);
      //      fprintf(fd,"hisID = %d\t%s\n",hisID,string);//DEBUG
    }
  }
  //  cerr<<"Booked DYN his"<<endl;//DEBUG

  // Charge spectra histograms filled in tofonlinefill1(...)
  // Anode: 5700 - 5827
#define ANODESTART 5700
  for(start=ANODESTART, inc=0;inc<4;inc++,start+=30){
    for(int i=0;i<28;i++){
      int k=1,hisID;
      char c;
      hisID=start+i;
      if(hisID<start+14){
	c='P';
	k=i+1;
      }
      else{
	c='N';
	k=i-13;
      }
      sprintf(string,"%1d.%02d%c - Anode (pC)",(inc+1),k,c);
      HBOOK1(hisID,string,100,0,600,0.);
      //      fprintf(fd,"hisID = %d\t%s\n",hisID,string);//DEBUG
    }
  }
  //  cerr<<"Booked ANODE his"<<endl;//DEBUG
   // Dynode: 5830 - 5947
#define DYNODESTART 5830
  for(start=DYNODESTART, inc=0;inc<4;inc++,start+=30){
    for(int i=0;i<28;i++){
      int k=1,hisID;
      char c;
      hisID=start+i;
      if(hisID<start+14){
	c='P';
	k=i+1;
      }
      else{
	c='N';
	k=i-13;
      }
      sprintf(string,"%1d.%02d%c - Dynode (pC)",(inc+1),k,c);
      HBOOK1(hisID,string,100,0,600,0.);
      //      fprintf(fd,"hisID = %d\t%s\n",hisID,string);//DEBUG
    }
  }
  //  cerr<<"Booked DYNODE his"<<endl;//DEBUG

  // Time Stretcher ratio histograms filled in tofonlinefill1(...)
  // ID's 6000 - 6117
#define STRSTART 6000
  for(start=STRSTART, inc=0;inc<4;inc++,start+=30){
    for(int i=0;i<28;i++){
      int k=1,hisID;
      char c;
      hisID=start+i;
      if(hisID<start+14){
	c='P';
	k=i+1;
      }
      else{
	c='N';
	k=i-13;
      }
      sprintf(string,"%1d.%02d%c - Stretcher ratio",(inc+1),k,c);
      HBOOK1(hisID,string,200,0,60,0.);
      //      fprintf(fd,"hisID = %d\t%s\n",hisID,string);//DEBUG
    }
  }
  //  cerr<<"Booked STR his"<<endl;//DEBUG

  //  fclose(fd);//DEBUG
}
//-----------------------------------------------------
void AMSTOFRawEvent::tofonlinefill1(integer ilay, integer ibar, integer isid,
					   int nedges[4], int num, int den,
					   number Atovt, number Dtovt){
// Histograms booking

//   cerr<<"ilay="<<ilay<<"\tibar="<<ibar<<"\tisid="<<isid<<endl;
//   cerr<<"nedges[]="<<nedges[0]<<" "<<nedges[1]<<" "<<nedges[2]<<" "<<nedges[3]<<endl;
//   cerr<<"num="<<num<<"\tden="<<den<<endl;
//   cerr<<"Atovt="<<Atovt<<"\tDtovt="<<Dtovt<<endl;

  geant str,off,strat[2][2];
  number q;
  unsigned int hisID,dir;

  static int nIDhis[1200];
  static int first=0;
  if(first==0)
    for(int k=0;k<1200;k++)nIDhis[k]=0;
  first++;

  if((isid==1 && (ilay==1||ilay==4)) || 
     (isid==2 && (ilay==2||ilay==3)))
    dir=0; // P
  else
    dir=1; // N
  
// his. ID = START + (ilay-1)*30 + (14-ibar) + dir*14
// where dir = 0 if (isid==1 && ilay==1||4) || (isid==2 && ilay==2||3)
//       dir = 1 if (isid==2 && ilay==1||4) || (isid==1 && ilay==2||3)

  // History edges
  hisID=HISSTART+(ilay-1)*30+(14-ibar)+dir*14;
  HFF1(hisID,nIDhis[hisID-5000],geant(nedges[0]),1.);

  // TDC edges
  hisID=TDCSTART+(ilay-1)*30+(14-ibar)+dir*14;
  HFF1(hisID,nIDhis[hisID-5000],geant(nedges[1]),1.);

  // Anode edges
  hisID=ANOSTART+(ilay-1)*30+(14-ibar)+dir*14;
  HFF1(hisID,nIDhis[hisID-5000],geant(nedges[2]),1.);

  // Dynode edges
  hisID=DYNSTART+(ilay-1)*30+(14-ibar)+dir*14;
  HFF1(hisID,nIDhis[hisID-5000],geant(nedges[3]),1.);

  // Good hits
  if(nedges[0]>=2&&nedges[1]>=4&&nedges[2]>=2){
    hisID=HITSTART+(ilay-1)+dir*4;
    HFF1(hisID,nIDhis[hisID-5000],geant(15-ibar),1.);
  }

  // Anode and dynode charge spectra (pC)
  scbrcal[ilay-1][ibar-1].q2t2q(1,int(isid-1),0,Atovt,q);
  hisID=ANODESTART+(ilay-1)*30+(14-ibar)+dir*14;
  if(q>FLT_MAX)q=FLT_MAX-1;
  else if(q<-FLT_MAX)q=-FLT_MAX+1;
  HFF1(hisID,nIDhis[hisID-5000],geant(q),1.);
  scbrcal[ilay-1][ibar-1].q2t2q(1,int(isid-1),1,Dtovt,q);
  if(q>FLT_MAX)q=FLT_MAX-1;
  else if(q<-FLT_MAX)q=-FLT_MAX+1;

  hisID=DYNODESTART+(ilay-1)*30+(14-ibar)+dir*14;
  HFF1(hisID,nIDhis[hisID-5000],geant(q),1.);

  // Time Expansion
  scbrcal[ilay-1][ibar-1].gtstrat(strat);
  off=strat[isid-1][1]; // offset of time str.
  str = den==0?FLT_MAX:(geant(num)-off)/geant(den); // Time Expansion
  hisID=STRSTART+(ilay-1)*30+(14-ibar)+dir*14;
  HFF1(hisID,nIDhis[hisID-5000],geant(str),1.);

}

