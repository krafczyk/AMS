#ifndef __TKDBC__
#define __TKDBC__
#include <typedefs.h>

#include <iostream.h>
#include <fstream.h>
class AMSPoint;
class TKDBc{
private:
 number _nrm[3][3];
 geant _coo[3];
 integer _status;
 integer _gid;
 static TKDBc * _HeadSensor;
 static TKDBc * _HeadLayer;
 static TKDBc * _HeadLadder[2];
 static TKDBc * _HeadMarker[2];
 static integer _NumberSen;
 static integer _NumberLayer;
 static integer _NumberLadder;
 static integer _NumberMarkers;
 static integer _ReadOK; 
 class TRLDGM_def{
 public:
   int Layer;
   int Ladder;
   int Half;
   geant Coo[3];
   geant Nrm[3][3];
   geant CooO[3];
   geant NrmO[3][3];
   geant Rx[10];
   geant Ry[10];
   geant Rz[10];
   int Status[10];
 };
 class TRLYGM_def{
 public:
   int Layer;
   geant Coo[3];
   geant Nrm[3][3];
   geant CooO[3];
   geant NrmO[3][3];
   geant Ra[4];
   geant Rx[4];
   geant Ry[4];
   geant Rz[4];
 };

public:
  TKDBc(): _status(0){}
 static integer getnum(integer layer, integer ladder, integer sensor);
 static integer getnumd(integer layer, integer ladder);
 static void init();
 static void read();
 static void write(integer update = 0);
 static void updatef();
 static void compileg();
 static void fitting(int il, int nls, int nmeas, integer StaLadders[17][2],
          AMSPoint CooLadder[17][2][10], int CooStatus[17][2][10],
         number LadderNrm[17][2][3][3], TRLDGM_def & TRLDGM,
         TRLYGM_def & TRLYGM, integer xPosLadder[17][2][10],
         AMSPoint LadderCoo[17][2]);
 static integer GetStatus(integer layer, integer ladder, integer sensor){
                return _HeadSensor[getnum(layer,ladder,sensor)]._status;
 }
 static integer update(){return _ReadOK==0;}
 static void SetSensor(integer layer, integer ladder, integer sensor,
             integer  status, geant coo[],number nrm[3][3], integer gid);
 static void GetSensor(integer layer, integer ladder, integer sensor, 
                          integer & status, geant coo[],number nrm[3][3]);
 
 static void SetLayer(integer layer,
              integer  status, geant coo[],number nrm[3][3], integer gid);
 static void GetLayer(integer layer,  
                          integer & status, geant coo[],number nrm[3][3],
                          integer &gid);
 static void SetLadder(integer layer,integer ladder,integer half,
             integer  status, geant coo[],number nrm[3][3],integer gid);
 static void GetLadder(integer layer, integer ladder, integer half,
                          integer & status, geant coo[],number nrm[3][3],
                          integer &gid);
 static void GetMarker(integer layer, integer markerno, integer markerpos,
                       integer & status, geant coo[], number nrm[3][3]);    
 static void SetMarker(integer layer, integer markerno, integer markerpos,
                       integer & status, geant coo[], number nrm[3][3], integer gid);    
 
#ifdef __DB__
   friend class TKDBcV;
#endif

};


class TKDBcI{
public:
TKDBcI();
~TKDBcI();
private:
 static integer _Count;
};
static TKDBcI tkI;



#endif
