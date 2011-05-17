#include "TrExtAlignDB.h"
#include "TkDBc.h"
#include "TFile.h"

ClassImp(TrExtAlignPar);
ClassImp(TrExtAlignDB);

using namespace  std;
TrExtAlignDB* TrExtAlignDB::Head=0;

void TrExtAlignPar::Print(Option_t *) const
{
  cout << "dpos= " << dpos[0] << " " << dpos[1] << " " << dpos[2];
  if (angles[0] != 0 || angles[1] != 0 || angles[2] != 0)
    cout << " angles= " << angles[0] << " " << angles[1] << " " << angles[2];
  cout << endl;
}

void TrExtAlignDB::Load(TFile * ff){
  if(!ff) return;
  if(Head) delete Head;
  Head= (TrExtAlignDB*) ff->Get("TrExtAlignDB");
  if(!Head)
    printf("TrExtAlignDB::Load-W- Cannot Load the external Layer AlignmentDB\n");
  return;
}


void TrExtAlignDB::UpdateTkDBc(uint time)
{
  for (int layer = 8; layer <= 9; layer++) {
    TkPlane* pl = TkDBc::Head->GetPlane(layer);
    if (!pl) continue;

    TrExtAlignPar par = Get(layer, time);
    AMSPoint  dpos(par.dpos[0], par.dpos[1], par.dpos[2]);
    AMSRotMat drot;
    drot.SetRotAngles(par.angles[0], par.angles[1], par.angles[2]);

    pl->posA = pl->posA + dpos;
    pl->rotA = pl->rotA * drot;
  }
}


