#include "TrExtAlignDB.h"
#include "TkDBc.h"
#include "TFile.h"

ClassImp(TrExtAlignDB);
using namespace  std;
TrExtAlignDB* TrExtAlignDB::Head=0;

void TrExtAlignDB::Load(TFile * ff){
  if(!ff) return;
  if(Head) delete Head;
  Head= (TrExtAlignDB*) ff->Get("TrExtAlign");
  if(!Head)
    printf("TrExtAlignDB::Load-W- Cannot Load the external Layer AlignmentDB\n");
  return;
}


void TrExtAlignDB::UpdateTkDBc(uint time){
    TkPlane* PL8= TkDBc::Head->GetPlane(8);
    TkPlane* PL9= TkDBc::Head->GetPlane(9);
    map<uint, apar>::iterator l8,l9;
    l8=L8.lower_bound(time);
    if(l8!=L8.end()){
      PL8->setposA(l8->second.dpos[0],l8->second.dpos[1],l8->second.dpos[2]);
      PL8->SetRotAngles(l8->second.angles[0],l8->second.angles[1],l8->second.angles[2]);
    }
    l9=L9.lower_bound(time);

    if(l9!=L9.end()){
      PL9->setposA(l9->second.dpos[0],l9->second.dpos[1],l9->second.dpos[2]);
      PL9->SetRotAngles(l9->second.angles[0],l9->second.angles[1],l9->second.angles[2]);
    }
    return;

  }


