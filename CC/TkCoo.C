/// $Id: TkCoo.C,v 1.20 2013/03/17 08:18:51 shaino Exp $ 

//////////////////////////////////////////////////////////////////////////
///
///\file  TkCoo.C
///\brief Source file of TkCoo class
///
///\date  2008/02/21 PZ  First version
///\date  2008/03/19 PZ  Add some features to TkSens
///\date  2008/04/10 AO  GetLocalCoo(float) of interstrip position 
///\date  2008/04/22 AO  Swiching back some methods  
///$Date: 2013/03/17 08:18:51 $
///
/// $Revision: 1.20 $
///
//////////////////////////////////////////////////////////////////////////
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>

#include "TkDBc.h"
#include "TkCoo.h"
#include "tkdcards.h"
#include "TrInnerDzDB.h"
#include "TrExtAlignDB.h"



void print_trace();
//--------------------------------------------------
AMSPoint TkCoo::GetGlobalN(int tkid,float X, float Y){
  // if(X<0||X > TkDBc::Head->_ssize_active[0]){
  //   printf("TkCoo::GetGlobalN Error X is outside the ladder size %f\n",X);
  //   return AMSPoint();
  // }
  if(Y<0 || Y>TkDBc::Head->_ssize_active[1]){
    printf("TkCoo::GetGlobalN Error Y is outside the ladder size %f\n",Y);
    return AMSPoint();
  }
  AMSPoint loc(X,Y,0.);
  return TkCoo::GetGlobalN(tkid, loc);
}


//--------------------------------------------------
AMSPoint TkCoo::GetGlobalN(int tkid, AMSPoint& loc){
  // if(loc[0]<0||loc[0] > TkDBc::Head->_ssize_active[0]){
  //   printf("TkCoo::GetGlobalN Error X is outside the ladder size %f\n",loc[0]);
  //   return AMSPoint();
  // }
  if(loc[1]<0 || loc[1] > TkDBc::Head->_ssize_active[1]){
    printf("TkCoo::GetGlobalN Error Y is outside the ladder size %f\n",loc[1]);
    return AMSPoint();
  }
  TkLadder* ll = TkDBc::Head->FindTkId(tkid);
  if(!ll){
    printf("GetGlobalN: ERROR cant find ladder %d into the database\n",tkid);
    return AMSPoint();
  }
  AMSPoint loc2(loc);
  // Set ladder local coo Z to zero
  loc2[2]=0;
  
  // Get the Global coo on the plane
  AMSPoint  oo = ll->GetRotMat()*loc2+ll->GetPos();
  
  // Get The Plane Pointer
  TkPlane*  pp = ll->GetPlane();

  // Convolute with the Plane pos in the space and Get the global Coo
  AMSPoint oo2 = pp->GetRotMat()*oo + pp->GetPos();

  return oo2;
}


//--------------------------------------------------
AMSPoint TkCoo::GetGlobalA(int tkid,float X, float Y){
  // if(X<0||X > TkDBc::Head->_ssize_active[0]){
  //   printf("TkCoo::GetGlobalN Error X is outside the ladder size %f\n",X);
  //   return AMSPoint();
  // }
 // if(Y<0 || Y>TkDBc::Head->_ssize_active[1]){
//    printf("TkCoo::GetGlobalA Error Y is outside the ladder size %f\n",Y);
//    return AMSPoint();
//  }
  AMSPoint loc(X,Y,0.);
  return TkCoo::GetGlobalA(tkid, loc);
}


//--------------------------------------------------
AMSPoint TkCoo::GetGlobalA(int tkid, AMSPoint& loc){
  // if(loc[0]<0||loc[0] > TkDBc::Head->_ssize_active[0]){
  //   printf("TkCoo::GetGlobalN Error X is outside the ladder size %f\n",loc[0]);
  //   return AMSPoint();
  // }
  if(loc[1]<0 || loc[1]>TkDBc::Head->_ssize_active[1]){
 // PZ FIXME!!!   printf("TkCoo::GetGlobalA Error Y is outside the ladder size %f\n",loc[1]);
    return AMSPoint();
  }
  TkLadder* ll=TkDBc::Head->FindTkId(tkid);
  if(!ll){
   //PZ FIXME!!!!  printf("GetGlobalA: ERROR cant find ladder %d into the database\n",tkid);
    return AMSPoint();
  }
  AMSPoint loc2(loc);
  // Set ladder local coo Z to zero
  loc2[2]=0;

  AMSRotMat RotG0  = ll->GetRotMatA();

  // Strip pitch correction
  if (TkLadder::version >= 3) {
    double aa, ab, ac;
    RotG0.GetRotAngles(aa, ab, ac); loc2[1] *= ac+1;
    RotG0.SetRotAngles(aa, ab, 0);
  }

  // Alignment corrected Ladder Rotation matrix
  AMSRotMat RotG  = RotG0*ll->GetRotMat();

  // Alignment corrected Ladder postion
  AMSPoint  PosG  = ll->GetPosA()+ll->GetPos();

  // Get the Global coo on the plane
  AMSPoint  oo    = RotG*loc2+PosG;

  // Get The Plane Pointer
  TkPlane*  pp    = ll->GetPlane();
  int layJ=ll->GetLayerJ();
  // Alignment corrected Plane Rotation matrix
  AMSRotMat PRotG0;
  if(layJ!=1&& layJ!=9) 
     PRotG0 = pp->GetRotMatA();
  else 
     PRotG0.SetRotAngles(TrExtAlignDB::GetDynCorr(layJ,3),
                         TrExtAlignDB::GetDynCorr(layJ,4),
                         TrExtAlignDB::GetDynCorr(layJ,5));

  AMSRotMat PRotG = PRotG0*pp->GetRotMat();

  int Layer=ll->GetLayer();
  float dzcorr=0.;
  if (Layer<=7) dzcorr=TrInnerDzDB::LDZA[Layer-1]; 
  AMSPoint LayerZCorrection(0,0,dzcorr);

  // Alignment corrected Plane postion
  AMSPoint planeA = pp->GetPosA();

  if(layJ==1|| layJ==9) 
   planeA=AMSPoint(TrExtAlignDB::GetDynCorr(layJ,0),
	            TrExtAlignDB::GetDynCorr(layJ,1),
	            TrExtAlignDB::GetDynCorr(layJ,2));
  
  AMSPoint  PPosG = planeA+pp->GetPos()+LayerZCorrection;

  // Covolute with the Plane pos in the space and Get the global Coo
  AMSPoint  oo2   = PRotG*oo + PPosG;

  // Sensor alignment correction
  double Ax= (TkDBc::Head->_ssize_inactive[0]-TkDBc::Head->_ssize_active[0])/2;
  int sens = (int)(abs(loc[0]+Ax)/TkDBc::Head->_SensorPitchK);
  if (TRCLFFKEY.UseSensorAlign==1 && 
      0 <= sens && sens < trconst::maxsen) {
    oo2[0] -= ll->_sensx[sens];
    oo2[1] -= ll->_sensy[sens];
    oo2[2] -= ll->_sensz[sens];
  }

  return oo2;
}


//--------------------------------------------------
int TkCoo::GetMaxMult(int tkid, float readchann){
  TkLadder* ll = TkDBc::Head->FindTkId(tkid);
  if(!ll){
    printf("GetMaxMult: ERROR cant find ladder %d into the database\n",tkid);
    return -1;
  }
  int max = 1;
  if(readchann<=639.) return 0;
  if( (ll->IsK7()) ) 
    max=(int) ceil((ll->GetNSensors()*TkDBc::Head->_NReadStripK7-readchann+640)/384.);
  else
    max=(int) ceil((ll->GetNSensors()*TkDBc::Head->_NReadStripK5-readchann+640)/384.);
  
  return (max-1);
}


//--------------------------------------------------
float TkCoo::GetLocalCoo(int tkid, int readchann,int mult){
  TkLadder* ll = TkDBc::Head->FindTkId(tkid);
  if(!ll){
    printf("GetLocalCoo: ERROR cant find ladder %d into the database\n",tkid);
    return -1;
  }
  int mmult=GetMaxMult(tkid,readchann);
  int plane=abs(tkid)/100;
  if(mult>mmult||mult<0) mult=mmult;
  // float out=0.;
  if(readchann<640) return GetLocalCooS(readchann);
  else{
    if(ll->IsK7()) return GetLocalCooK7(readchann,mult);
    else return GetLocalCooK5(readchann,mult);
  }
}

//--------------------------------------------------
float TkCoo::GetLocalCoo(int tkid, float readchann,int mult){
  TkLadder* ll = TkDBc::Head->FindTkId(tkid);
  if(!ll){
    printf("GetLocalCoo: ERROR cant find ladder %d into the database\n",tkid);
    return -1;
  } 
  int mmult=GetMaxMult(tkid,readchann);
  int plane=abs(tkid)/100;
  if(mult>mmult||mult<0) mult=mmult;
  // float out=0.;
  int channel = (int) round(readchann);
  if(readchann<640) {
    number cooY1,cooY2;  
    cooY1=TkCoo::GetLocalCooS(channel);
    if((readchann-channel)>=0)
      cooY2=TkCoo::GetLocalCooS(channel+1);
    else
      cooY2=TkCoo::GetLocalCooS(channel-1);
    return cooY1 + (readchann-channel)*abs(cooY1-cooY2);
  }
  else{
    if(ll->IsK7()) {
      number cooX1,cooX2;  
      cooX1=TkCoo::GetLocalCooK7(channel,mult);
      if((readchann-channel)>=0)
	cooX2=TkCoo::GetLocalCooK7(channel+1,mult);
      else
	cooX2=TkCoo::GetLocalCooK7(channel-1,mult);
      return cooX1 + (readchann-channel)*abs(cooX1-cooX2);
    }
    else {
      number cooX1,cooX2;  
      cooX1=TkCoo::GetLocalCooK5(channel,mult);
      if((readchann-channel)>=0)
	cooX2=TkCoo::GetLocalCooK5(channel+1,mult);
      else
	cooX2=TkCoo::GetLocalCooK5(channel-1,mult);
      return cooX1 + (readchann-channel)*abs(cooX1-cooX2);
    }
  }
}

//--------------------------------------------------
// The gaps between 0 and 1 and 638 to 639 are doubled
float TkCoo::GetLocalCooS(int readchann){
  if(readchann>639||readchann<0) return -1;
  float out= readchann*TkDBc::Head->_PitchS;
  if(readchann!=0)    out+=TkDBc::Head->_PitchS;
  if(readchann==639)  out+=TkDBc::Head->_PitchS;
  return out;
}


//--------------------------------------------------
// The gap between the 190 and 191 is one and half
float TkCoo::GetLocalCooK5(int readchann,int mult){
  if(readchann>1023||readchann<640) return -1;  
  int chann=readchann-640;
  int Sensor=((chann+mult*TkDBc::Head->_NReadoutChanK)/TkDBc::Head->_NReadStripK5);
  int chann2=chann;
  //  printf("Sensor %d  %d\n",Sensor,mult);
  if(chann2>191) chann2-=192;
  return TkDBc::Head->_SensorPitchK*Sensor+ chann2 * TkDBc::Head->_PitchK5
    + ((chann2 == 191) ? TkDBc::Head->_PitchK5*0.5 : 0);
}


//--------------------------------------------------
// The implantation of the strips in K7 is not uniform
// ImplantedFromReadK7 evaluate the implanted strips in terms of _ImplantPitchK unit
float TkCoo::GetLocalCooK7(int readchann,int mult){
  if(readchann>1023||readchann<640) return -1;  
  int chann=readchann-640;
  int Sensor=((chann+mult*TkDBc::Head->_NReadoutChanK)/TkDBc::Head->_NReadStripK7);
  int readstrip= (chann+mult*TkDBc::Head->_NReadoutChanK) - Sensor * TkDBc::Head->_NReadStripK7;
  return TkDBc::Head->_SensorPitchK*Sensor+ ImplantedFromReadK7(readstrip) * TkDBc::Head->_ImplantPitchK;    
}

//--------------------------------------------------
// - from   0 to  64: readout pitch is 2 1 2 1 2 1, ...
// - from  64 to 160: readout pitch is 2
// - from 160 to 224: readout pitch is 2 1 2 1 2 1, ... 
int TkCoo::ImplantedFromReadK7(int channel){
  // this is the conversion for sensor 1
  //NOTA: channel = 0 to 383..
  int pitch=0;
  int start=0;
  int strip=0;
  int diff=0;
  int startch=0;
  if (channel<64) { // subgroups 1 and 2, 64 channels
    pitch=3;
    startch=channel%2;
    start=2*startch;

    diff=(channel-startch)/2; // the difference is always even
    strip=start+diff*pitch;         
  } else if (channel<160) { // subgroups 3 to 5, 96 channels
    pitch=2;
    start=96;
    startch=64;
    diff=(channel-startch);
    strip=start+diff*pitch;
  } else if (channel<224) {
    pitch=3;
    startch=channel%2;
    start=48+2*startch;
    //    startch+=160;
    diff=(channel-startch)/2; // the difference is always even
    strip=start+diff*pitch; 
  }
  return strip;
}

//--------------------------------------------------
// Conversion (Readout Channel, Multiplicity) -> (Sensor, StripInSensor)
//             0, 1024          0, MaxMult      0, MaxSens   S:0,640 K5:0,192 K7:0,224
// Once more the readout scheme:
// S: No multiplicity     
// K5: NReadoutChannels = 384, NReadoutPerSensor = 192
//     Sens #0       Sens #1         Sens #2       Sens #3
//     | 0, ..., 191 |   0, ..., 191 | 0, ..., 191 |   0, ..., 191 | ...
//     | 0, ..., 191 | 192, ..., 383 | 0, ..., 191 | 192, ..., 383 | ...
//     | 0, ..., 191,  192, ..., 383 | 0, ..., 191,  192, ..., 383 | ... 
//     | 0, ...,            ..., 383 | 0, ...,            ..., 383 | ...
//     Mult #0                       Mult #1       
// K7: NReadoutChannels = 384, NReadoutPerSensor = 224
//     Sens #0       Sens#1                        Sens #2                                      Sens #3
//     | 0, ..., 223 |   0, ...,          ..., 223 |  0, ...,                          ..., 223 | ...
//     | 0, ..., 223 | 224, ..., 383,  0, ...,  63 | 64, ..., 287 | 288, ..., 383,  0, ..., 127 | ...
//     | 0, ..., 223,  224, ..., 383 | 0, ...,  63,  64, ..., 287,  288, ..., 383 | 0, ..., 127,  ...
//     | 0, ...,                 383 | 0, ...,                           ..., 383 | 0, ...,       ...
//     Mult #0                       Mult #1                                      Mult #2
// Sensor = int( (Channel + Multiplicity*NReadoutChannels)/NReadoutPerSensor) 
// Strip  = Channel + Multiplicity*NReadoutChannels - Sensor*NReadoutPerSensor
int TkCoo::GetSensorAddress(int tkid, int readchann, int mult, int& sens, int verbose) {
  // default for failure
  sens = -1;
  // if S nothing to do 
  if ((readchann>=0)&&(readchann<640)) { sens = 0; return readchann; }
  // K address between 0 and 383
  readchann -= 640;
  // take ladder to decide K5 or K7 
  TkLadder* ladder = TkDBc::GetHead()->FindTkId(tkid);
  if (!ladder) {  
    if (verbose>0) printf("TkCoo::GetSensorAddress-W cannot find ladder %d into the database. Return -1.\n",tkid); 
    sens = -1; 
    return -1; 
  }
  // check multiplicity
  int max_mult = GetMaxMult(tkid,readchann+640); // needed channel 0-1023
  if ( (mult<0)||(mult>max_mult) ) { 
    // this could actually happen for last sensors of K7 (max multiplicity is taken from first strip, not from seed strip), disable error message
    // if (verbose>0) printf("TkCoo::GetSensorAddress-W requested multiplicity %d out of range (0,%d), for channel %d on ladder %+04d. Return -2.\n",
    //   mult,max_mult,readchann+640,tkid);   
    sens = -2;
    return -2; 
  }
  // calculate sensor 
  int nread = TkDBc::GetHead()->_NReadoutChanK;
  int nread_per_sens = (ladder->IsK7()) ? TkDBc::GetHead()->_NReadStripK7 : TkDBc::GetHead()->_NReadStripK5;
  sens = int((readchann + mult*nread)/nread_per_sens);
  // check sensor_number
  int max_sens = ladder->GetNSensors();
  if ( (sens<0)||(sens>max_sens) ) { 
    if (verbose>0) printf("TkCoo::GetSensorAddress-W calculated sensor number %d out of range (0,%d). Return -3.\n",sens,max_sens); 
    sens = -3; 
    return -3; 
  }
  // calculate strip on sensor 
  int strip = readchann + mult*nread - sens*nread_per_sens;
  if ( (strip<0)||(strip>nread_per_sens) ) { 
    if (verbose>0) printf("TkCoo::GetSensorAddress-W calculated strip number %d out of range (0,%d). Return -4.\n",strip,nread_per_sens); 
    sens = -4; 
    return -4; 
  }
  return strip;
}

//--------------------------------------------------
double TkCoo::GetLadderLength(int tkid) {
  // Get ladder length along the X-coordinate in cm
  if(!TkDBc::Head) return -9999.;
  TkLadder* pp=TkDBc::Head->FindTkId(tkid);
  if(!pp) return -9999.;
  
  return (TkDBc::Head->_SensorPitchK*pp->GetNSensors()-0.004);
}


//--------------------------------------------------
AMSPoint TkCoo::GetLadderCenter(int tkid,int isMC) { 
// Get central position of the ladder in the X-coordinate in cm
  if(!TkDBc::Head) return AMSPoint(-9999.,-9999.,-9999.);
  TkLadder* pp=TkDBc::Head->FindTkId(tkid);
  if(!pp) return AMSPoint(-9999.,-9999.,-9999.);

  double hlen = GetLadderLength(tkid)/2-(TkDBc::Head->_ssize_inactive[0]-TkDBc::Head->_ssize_active[0])/2;
  double hwid = TkDBc::Head->_ssize_active[1]/2;
  if(isMC)  
    return TkCoo::GetGlobalT(tkid, hlen, hwid);
  else 
    return TkCoo::GetGlobalA(tkid, hlen, hwid);
}



AMSPoint TkCoo::GetGlobalNC(int tkid,float readchanK, float readchanS,int mult){
  TkLadder* ll = TkDBc::Head->FindTkId(tkid);
  if(!ll){
    printf("GetGlobalNC: ERROR cant find ladder %d into the database\n",tkid);
    return AMSPoint(-1,-1,-1);
  }
  int layer=abs(tkid)/100;
  if(ll->IsK7())
    return GetGlobalN(tkid,GetLocalCooK7((int)readchanK, mult),  GetLocalCooS((int)readchanS));
  else 
    return GetGlobalN(tkid,GetLocalCooK5((int)readchanK, mult),  GetLocalCooS((int)readchanS));
}

AMSPoint TkCoo::GetGlobalAC(int tkid,float readchanK, float readchanS,int mult){
  TkLadder* ll = TkDBc::Head->FindTkId(tkid);
  if(!ll){
    printf("GetGlobalAC: ERROR cant find ladder %d into the database\n",tkid);
    return AMSPoint(-1,-1,-1);
  }
  int layer=abs(tkid)/100;
  if(ll->IsK7())
    return GetGlobalA(tkid,GetLocalCooK7((int)readchanK, mult),  GetLocalCooS((int)readchanS));
  else 
    return GetGlobalA(tkid,GetLocalCooK5((int)readchanK, mult),  GetLocalCooS((int)readchanS));
}


/* Obtain a backtrace and print it to stdout. */
  void
print_trace (void)
{

  void *array[100];
  size_t size;
  char **strings;
  size_t i;

  size = backtrace (array, 10);
  strings = backtrace_symbols (array, size);
  printf ("Obtained %zd stack frames.\n", size);

  for (i = 0; i < size; i++)
    printf ("%s\n", strings[i]);

  free (strings);
}

AMSPoint TkCoo::GetGlobalTC(int tkid,float readchanK, float readchanS,int mult){
  TkLadder* ll = TkDBc::Head->FindTkId(tkid);
  if(!ll){
    printf("GetGlobalAC: ERROR cant find ladder %d into the database\n",tkid);
    return AMSPoint(-1,-1,-1);
  }
  int layer=abs(tkid)/100;
  if(ll->IsK7())
    return GetGlobalT(tkid,GetLocalCooK7((int)readchanK, mult),  GetLocalCooS((int)readchanS));
  else 
    return GetGlobalT(tkid,GetLocalCooK5((int)readchanK, mult),  GetLocalCooS((int)readchanS));
}

//--------------------------------------------------
AMSPoint TkCoo::GetGlobalT(int tkid,float X, float Y){
  // if(X<0||X > TkDBc::Head->_ssize_active[0]){
  //   printf("TkCoo::GetGlobalN Error X is outside the ladder size %f\n",X);
  //   return AMSPoint();
  // }
 // if(Y<0 || Y>TkDBc::Head->_ssize_active[1]){
//    printf("TkCoo::GetGlobalA Error Y is outside the ladder size %f\n",Y);
//    return AMSPoint();
//  }
  AMSPoint loc(X,Y,0.);
  return TkCoo::GetGlobalT(tkid, loc);
}


//--------------------------------------------------
AMSPoint TkCoo::GetGlobalT(int tkid, AMSPoint& loc){
  // if(loc[0]<0||loc[0] > TkDBc::Head->_ssize_active[0]){
  //   printf("TkCoo::GetGlobalN Error X is outside the ladder size %f\n",loc[0]);
  //   return AMSPoint();
  // }
  if(loc[1]<0 || loc[1]>TkDBc::Head->_ssize_active[1]){
 // PZ FIXME!!!   printf("TkCoo::GetGlobalT Error Y is outside the ladder size %f\n",loc[1]);
    return AMSPoint();
  }
  TkLadder* ll=TkDBc::Head->FindTkId(tkid);
  if(!ll){
   //PZ FIXME!!!!  printf("GetGlobalT: ERROR cant find ladder %d into the database\n",tkid);
    return AMSPoint();
  }
  AMSPoint loc2(loc);
  // Set ladder local coo Z to zero
  loc2[2]=0;

  // Alignment corrected Ladder Rotation matrix
  AMSRotMat RotG0  = ll->GetRotMatT();
  AMSRotMat RotG  = RotG0*ll->GetRotMat();

  // Alignment corrected Ladder postion
  AMSPoint  PosG  = ll->GetPosT()+ll->GetPos();

  // Get the Global coo on the plane
  AMSPoint  oo    = RotG*loc2+PosG;

  // Get The Plane Pointer
  TkPlane*  pp    = ll->GetPlane();

  // Alignment corrected Plane Rotation matrix
  AMSRotMat PRotG0 = pp->GetRotMatT();
  AMSRotMat PRotG = PRotG0*pp->GetRotMat();

  // Alignment corrected Plane postion
  AMSPoint  PPosG = pp->GetPosT()+pp->GetPos();

  // Covolute with the Plane pos in the space and Get the global Coo
  AMSPoint  oo2   = PRotG*oo + PPosG;

  
  return oo2;
}
