#include <trdsim.h>
#include <event.h>
#include <extC.h>
#include <tofsim02.h>
#include <ntuple.h>

void AMSTRDRawHit::lvl3CompatibilityAddress(int16u ptr,uinteger& udr, uinteger & ufe,uinteger& ute,uinteger& chan){
 chan = ptr & 15;
 ute = (ptr>>4) & 3;
 ufe = (ptr>>6) & 7;
 udr = (ptr>>9) & 7;
}

bool AMSTRDRawHit::_HardWareCompatibilityMode=false;
void AMSTRDRawHit::sitrddigi(){

    AMSTRDMCCluster * ptr=
    (AMSTRDMCCluster*)AMSEvent::gethead()->getheadC("AMSTRDMCCluster",0,1);    
    geant edep=0;
    while(ptr){
     edep+=ptr->getedep()*TRDMCFFKEY.GeV2ADC;
     if(ptr->testlast()){
           AMSTRDIdSoft idsoft(ptr->getid());
           number amp=(edep*idsoft.getgain()+idsoft.getped()+idsoft.getsig()*rnormx());
           if (amp>idsoft.overflow())amp=idsoft.overflow();
           if(amp-idsoft.getped()>TRDMCFFKEY.Thr1R*idsoft.getsig())
        AMSEvent::gethead()->addnext(AMSID("AMSTRDRawHit",idsoft.getcrate()),
        new AMSTRDRawHit(idsoft,(amp-idsoft.getped())*TRDMCFFKEY.f2i));
        edep=0;       
     }

     ptr=ptr->next();
    }
    int cl=AMSEvent::gethead()->getC("AMSTRDRawHit",0)->getnelem();
//    cout <<"  ntrdclusters "<<AMSEvent::gethead()->getC("AMSTRDMCCluster",0)->getnelem()<<" "<<cl<<endl;
  if(TRDMCFFKEY.NoiseOn &&TOF2RawEvent::gettrfl())AMSTRDRawHit::sitrdnoise();
//    cout <<"  noise "<<AMSEvent::gethead()->getC("AMSTRDRawHit",0)->getnelem()-cl<<endl;


}


void AMSTRDRawHit::sitrdnoise(){
//   brute force now
//   if necessary should be changed a-la sitknoise

for ( int i=0;i<TRDDBc::TRDOctagonNo();i++){
      for (int j=0;j<TRDDBc::LayersNo(i);j++){
	  for(int k=0;k<TRDDBc::LaddersNo(i,j);k++){
            for (int l=0;l<TRDDBc::TubesNo(i,j,k);l++){
             float amp=rnormx();
             if(amp>TRDMCFFKEY.Thr1R){
              AMSTRDIdGeom id(j,k,l,i);
              AMSTRDIdSoft idsoft(id);
              AMSTRDRawHit *p =new AMSTRDRawHit(idsoft,amp*idsoft.getsig()*TRDMCFFKEY.f2i);
              AMSTRDRawHit *ph= (AMSTRDRawHit *)AMSEvent::gethead()->getheadC(AMSID("AMSTRDRawHit",idsoft.getcrate()));
              while(ph){
                if(ph<p){
                  AMSEvent::gethead()->addnext(AMSID("AMSTRDRawHit",idsoft.getcrate()),p);
                  break;
                }
                else if(!(p<ph)){
                  delete p;
                  break;
                }
                ph=ph->next();
              }
             }
            }
           }
          }
}

}





void AMSTRDRawHit::builddaq(int n, int16u* p){

}

void AMSTRDRawHit::buildraw(int i, int n, int16u*p){
}


void AMSTRDRawHit::_writeEl(){
  TRDRawHitNtuple* TrN = AMSJob::gethead()->getntuple()->Get_trdht();
  if (TrN->Ntrdht>=MAXTRDRHT) return;
  integer flag =    (IOPA.WriteAll%10==1)
                 || ( checkstatus(AMSDBc::USED));
  if(AMSTRDRawHit::Out( flag  )){
    TrN->Layer[TrN->Ntrdht]=_id.getlayer();
    TrN->Ladder[TrN->Ntrdht]=_id.getladder();
    TrN->Tube[TrN->Ntrdht]=_id.gettube();
    TrN->Amp[TrN->Ntrdht]=Amp();
    TrN->Ntrdht++;
  }

}

void AMSTRDRawHit::_copyEl(){
}

void AMSTRDRawHit::_printEl(ostream & o){
o<<_id<<" "<<_Amp<<endl;
}



integer AMSTRDRawHit::Out(integer status){
static integer init=0;
static integer WriteAll=0;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
  for(int n=0;n<ntrig;n++){
    if(strcmp("AMSTRDRawHit",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
    }
  }
}
return (WriteAll || status);
}






integer AMSTRDRawHit::lvl3format(int16 * adc, integer nmax){
  //
  // convert my stupid format to lvl3 one for ams02 flight 
  // (pure fantasy, not yet defined
  //  word 1:   tube no in address format
  //  word 2:   tube amp  (-ped)


  if (nmax < 2)return 0;
  adc[0]=_id.gethaddr();
  adc[1]=_Amp;
  return 2;
}



