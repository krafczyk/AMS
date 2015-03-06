#include "VCon_gbatch.h"

#include "event.h"

VCon_gb::VCon_gb(AMSContainer * cc):VCon(),leli(-1),lel(0){
  con=cc;
}


void  VCon_gb::removeEl(TrElem* prev, integer restore){
  if (!con)return;
  con->removeEl(dynamic_cast<AMSlink*> (prev),restore);
  lel=0;
  leli=-1;
};

VCon* VCon_gb::GetCont(const char * name){
   lel=0;
   leli=-1;
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
   lel=0;
   leli=-1;
  if(!con) return ;
  else return con->eraseC();
}

TrElem* VCon_gb::getelem(int ii){
  if(!con) return 0;
  if(ii==leli+1 &&lel){
   leli=ii;
   lel=lel->next();
    return dynamic_cast<TrElem*>(lel);
  } 
 else if(ii==leli &&lel)return dynamic_cast<TrElem*>(lel);
AMSlink* primo=con->gethead();
  for (int jj=0;jj<ii;jj++)
    primo=primo->next();
    leli=ii;
    lel=primo;
  return dynamic_cast<TrElem*> (primo);
}

void VCon_gb::addnext(TrElem* aa){
  if(!con) return;
  con->addnext(dynamic_cast<AMSlink*> (aa));
  
}

int  VCon_gb::getindex(TrElem* aa){
  if(!con) return 0;
if(lel && lel==(AMSlink*)aa)return leli;
else if(lel && lel->next() ==(AMSlink*)aa){
lel=lel->next();
leli++;
return leli;
}
  AMSlink* primo=con->gethead();
  for (int jj=0;jj<con->getnelem();jj++){
    if(primo==dynamic_cast<AMSlink*>(aa)) return jj;
    primo=primo->next();
  }
  return -1;
}

void VCon_gb::exchangeEl(TrElem* el1, TrElem* el2) {
   lel=0;
   leli=-1;
  if(!con) return;
  con->exchangeEl(dynamic_cast<AMSlink*>(el1),dynamic_cast<AMSlink*>(el2));
}

