// Simple version 9.06.1997 by E.Choumilov
// Last mod. 19.09.97 AH.

//
#include <commons.h> 
#include <event.h>
#include <trigger1D.h>
//
Triggerlvl1D::Triggerlvl1D(TriggerLVL1* p){
   _TrigMode    = p->_TriggerMode;
   _TofFlag     = p->_tofflag;
    for(int i=0;i<SCLRS;i++)_TofPatt[i]=p->_tofpatt[i];
   _AntiPatt    =p->_antipatt;
}
 void Triggerlvl1D::getTofPatt(integer* TofPatt) {UCOPY( &_TofPatt, TofPatt, sizeof(integer)*SCLRS/4);}



 
