#include "VCon_gbatch.h"

#include "event.h"

VCon_gb::VCon_gb(AMSContainer * cc):VCon(){
  con=cc;
}


void  VCon_gb::removeEl(TrElem* prev, integer restore){
  if (!con)return;
  con->removeEl(dynamic_cast<AMSlink*> (prev),restore);
};

VCon* VCon_gb::GetCont(const char * name){
  AMSContainer *cc=AMSEvent::gethead()->getC(AMSID(name));
  if (cc){
    con=cc;
    return (VCon*)(this);
  }
  else return 0;

}

int VCon_gb::getnelem(){
  if(!con) return -1;
  else return con->getnelem();
}

void VCon_gb::eraseC(){
  if(!con) return ;
  else return con->eraseC();
}

TrElem* VCon_gb::getelem(int ii){
  if(!con) return 0;
  AMSlink* primo=con->gethead();
  for (int jj=0;jj<ii;jj++)
    primo=primo->next();
  return dynamic_cast<TrElem*> (primo);
}

void VCon_gb::addnext(TrElem* aa){
  if(!con) return;
  con->addnext(dynamic_cast<AMSlink*> (aa));
}

int  VCon_gb::getindex(TrElem* aa){
  if(!con) return 0;
  AMSlink* primo=con->gethead();
  for (int jj=0;jj<con->getnelem();jj++){
    if(primo==dynamic_cast<AMSlink*>(aa)) return jj;
    primo=primo->next();
  }
  return -1;
}

void VCon_gb::exchangeEl(TrElem* el1, TrElem* el2) {
  if(!con) return;
  con->exchangeEl(dynamic_cast<AMSlink*>(el1),dynamic_cast<AMSlink*>(el2));
}

