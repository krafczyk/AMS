// Original code by Choutko
// Last modification 19.09.97 AH.
#include <iostream.h>
#include <string.h>
#include <cern.h>

#include <typedefs.h>
#include <trigger3.h>
class Triggerlvl3D: public ooObj{
protected:

 integer _TOFTrigger;
 integer _AntiTrigger;
 integer _TrackerTrigger;   // 0  - initial state
                            // 1  - reject 
                            // 2  - Too many hits  ( no fit  done )
                            // 3  - No comb found
                            // 4  - >=2 comb found
                            // 5  - Reserved
                            // 6  - Reserved
                            // 7  - Accept 
 integer _NTrHits;      // Total number of "good" hits
 integer _NPatFound;    // Number of "tracks" found
 integer _Pattern[2];
 number  _Residual[2];      // Discriminators
 number  _Time;          // Time to process event

// Aux Tr

 integer _nhits[nl];
 integer _drp[nl][maxtrpl];
 geant _coo[nl][maxtrpl];
 geant _lowlimitY[nl];
 geant _upperlimitY[nl];
 geant _lowlimitX[nl];
 geant _upperlimitX[nl];
//
 static integer _TOFPattern[SCMXBR][SCMXBR];
 static integer _TOFStatus[SCLRS][SCMXBR];
 static integer _TrackerStatus[NTRHDRP2];
 static integer _TrackerAux[NTRHDRP][2];
 static integer _TOFAux[SCLRS][SCMXBR];
 static integer _NTOF[SCLRS];
 static geant _TOFCoo[SCLRS][SCMXBR][3];
 static geant _TrackerCoo[NTRHDRP][2][3];
 static geant _TrackerCooZ[nl];
 static integer _TrackerDRP2Layer[NTRHDRP][2];
 static const integer _patconf[LVL3NPAT][nl];
 static const integer _patpoints[LVL3NPAT];
 static const integer _patd[5]; 
 static number _stripsize;

public:
 Triggerlvl3D();
 Triggerlvl3D(TriggerLVL3* p);
 integer tofok();
 integer getTracTrig(){ return  _TrackerTrigger;}
 integer getTofTrig() { return  _TOFTrigger;}
 integer getAntiTrig(){ return  _AntiTrigger;}
 integer getNTrHits() {return _NTrHits;}
 integer getNPatFound(){return _NPatFound;}
 void    getPattern(integer* Pattern); 
 void    getResidual(number* Residual); 
 number  getTime()     {return  _Time;}
//
 void getnhits(integer* nhits);
 void getdrp  (integer* drp);
 void getcoo(geant* coo);
 void getlowlimitY(geant* llimitY); 
 void getupperlimitY(geant* ulimitY);
 void getlowlimitX(geant* llimitX); 
 void getupperlimitX(geant* ulimitX);
//
 void getTOFPattern(integer* nbuff0);
 void getTOFStatus(integer* nbuff1);
 void getTrackerStatus(integer* nbuff2);
 void getTrackerAux(integer* nbuff3);
 void getTOFAux(integer* nbuff4);
 void getNTOF(integer* nbuff5);
 void getTOFCoo(geant* nbuff6);
 void getTrackerCoo(geant* nbuff7);
 void getTrackerCooZ(geant* nbuff8);
 void getTrackerDRP2Layer(integer* nbuff9);
 number   getstripsize(){return _stripsize;}
 
};
