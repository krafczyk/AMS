//Preselection routine for electron/positron analysis
#ifndef SELECT_H
#define SELECT_H

#include "TofTrack.h"
#include "Tofrec02_ihep.h"

//Subroutines
bool BadRunCheck(AMSEventR*);            //ISS only
bool IsScienceRun(AMSEventR*);           //ISS only
bool HasGoodLiveTime(Level1R*);          //ISS only
bool GoodHW(AMSEventR*);                 //ISS only
bool GoodHW_deeper(AMSEventR*);          //ISS only
bool GoodAccPattern(Level1R*);

bool GoodBeta(BetaHR*);
bool GoodBetaProtons(BetaHR*);

bool GoodTrkTrack(TrTrackR*);

bool GoodTrdTrack(TrdTrackR*);
bool InTrdAcceptance(TrTrackR*);

bool InEcalAcceptance(TrTrackR*);

bool IsInSolarArrays(AMSEventR*, int);

bool EcalTrkMatch(TrTrackR*, EcalShowerR*);
bool TrdTrkMatch(TrTrackR*,TrdTrackR*);
bool TofTrkMatch(AMSEventR*,TrTrackR*,int);
bool TofTrkMatchH(AMSEventR*,TrTrackR*,int);

bool EcalEnergyGreaterThan2(EcalShowerR*); //not used in Clean_Event

void SelectParticle(int kParticle=3);//electron, a la Geant

bool Clean_Event(AMSEventR* pev);
int Clean_Event_ChoosenParticle(AMSEventR* pev);
int Clean_Event_ChoosenParticle(AMSEventR* pev, int& Id_MaxSpan);
int Clean_Event_ChoosenParticle(AMSEventR* pev, int& Id_MaxSpan, int& Id_Inner);
int Clean_Event_ChoosenParticle(AMSEventR* pev, int& Id_MaxSpan, int& Id_Inner, TofTrack*& Tof_Track);

#endif
