#ifndef __TKDBC__
#define __TKDBC__
#include <typedefs.h>

#include <iostream.h>
#include <fstream.h>
class TKDBc{
private:
 number _nrm[3][3];
 geant _coo[3];
 integer _status;
 integer _gid;
 static TKDBc * _Head;
 static TKDBc * _HeadLayer;
 static TKDBc * _HeadLadder[2];
 static integer _NumberSen;
 static integer _NumberLayer;
 static integer _NumberLadder;
 static integer _ReadOK; 
 
public:
  TKDBc(): _status(0){}
 static integer getnum(integer layer, integer ladder, integer sensor);
 static integer getnumd(integer layer, integer ladder);
 static void init();
 static void read();
 static void write();
 static integer GetStatus(integer layer, integer ladder, integer sensor){
                return _Head[getnum(layer,ladder,sensor)]._status;
 }
 static integer update(){return _ReadOK==0;}
 static void SetSensor(integer layer, integer ladder, integer sensor,
             integer  status, geant coo[],number nrm[3][3], integer gid);
 static void GetSensor(integer layer, integer ladder, integer sensor, 
                          integer & status, geant coo[],number nrm[3][3]);
 
 static void SetLayer(integer layer,
              integer  status, geant coo[],number nrm[3][3], integer gid);
 static void GetLayer(integer layer,  
                          integer & status, geant coo[],number nrm[3][3]);
 static void SetLadder(integer layer,integer ladder,integer half,
             integer  status, geant coo[],number nrm[3][3],integer gid);
 static void GetLadder(integer layer, integer ladder, integer half,
                          integer & status, geant coo[],number nrm[3][3]);
 

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
