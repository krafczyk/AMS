//  $Id$
#include "g4util.h"
#include "root.h"
AMSRandGeneral* AMSRandGeneral::_pstatic=0;
void AMSRandGeneral::book(geant parray[], uinteger larray, geant xmi, geant xma, integer hid){
  if(!_pstatic)_pstatic=new AMSRandGeneral();
  if(hid){
    AMSRandGeneral::service * p=new service(parray,larray,xmi,xma,hid);
    if(_pstatic->_first){
     AMSRandGeneral::service * go;
     for(go=_pstatic->_first;go->_next;go=go->_next){
       if(go->_id==hid){
        delete p;
        cerr<<"AMSRandGeneral::book-E-HistAlreadyExists "<<hid<<endl;
        break;
       }
     }
     go->_next=p;
    }   
    else _pstatic->_first=p;
 }
}

void AMSRandGeneral::book(integer hid){
 if(hid && AMSEventR::h1(hid)){
   int nx=AMSEventR::h1(hid)->GetNbinsX();
   geant* array=new geant[nx];
   geant xmi=   AMSEventR::h1(hid)->GetBinLowEdge(1);
   geant xma=   AMSEventR::h1(hid)->GetBinLowEdge(nx)+AMSEventR::h1(hid)->GetBinWidth(nx);
   for(int k=1;k<nx+1;k++){
    array[k-1]= AMSEventR::h1(hid)->GetBinContent(k);
   }
    book(array,nx,xmi,xma,hid);
    delete[] array;
 }
 else if (hid && HEXIST(hid)){
  char CHTITL[81];
  integer nx,ny,nwt,loc;
  geant xmi,xma,ymi,yma;
  HGIVE(hid,CHTITL,nx,xmi,xma,ny,ymi,yma,nwt,loc);
  if(!ny){
   geant* array=new geant[nx];
   HUNPAK(hid,array,"    ",0);
    book(array,nx,xmi,xma,hid);
    delete[] array;
  }
 }
}

number AMSRandGeneral::hrndm1(integer hid){
static integer hlast=0;
static AMSRandGeneral::service * plast=0;
#ifdef _OPENMP
#pragma omp threadprivate (hlast,plast)
#endif
if(_pstatic && hid){
 if(hid==hlast)return plast->_xmi+plast->_pgenerator->shoot()*plast->_range;
 else{
    for(AMSRandGeneral::service * go=_pstatic->_first;go;go=go->_next){
     if(go->_id == hid){
       plast=go;
       hlast=hid;
       return plast->_xmi+plast->_pgenerator->shoot()*plast->_range;
     }
    }
    cerr<<"AMSRandGeneral::hrndm1-E-NoSuchHistogram "<<hid<<endl;
    return 0;
 }
}
static int ip=0;
if(ip++<100)cerr<<"AMSRandGeneral::hrndm1-E-UnableToGenerate "<<hid<<" "<<_pstatic<<endl;
return 0;

}
