#include <ctcsim.h>
#include <point.h>
#include <event.h>
#include <amsgobj.h>
#include <commons.h>
#include <cern.h>
#include <mccluster.h>
#include <math.h>
#include <extC.h>
#include <tofdbc.h>
#include <ctcdbc.h>
#include <ntuple.h>
AMSID AMSCTCRawHit::crgid(integer imat){
      //      imat == 0   // agl
      //      imat == 1   // pmt
      //      imat == 2   // ptf
      integer y=(_row+1)/2;
      integer x=(_column+1)/2;
      integer iy=(_row+1)%2;
      integer ix=(_column+1)%2;      
      
     integer id=1000000*_layer+1000*y+100*x+2*iy+ix+1;
      if(imat==2){
       if(_layer==1)return AMSID("PTFU",id+10);     
       else if(x<6) return AMSID("PTFL",id+10);     
       else return AMSID("PTFE",id+10);     
      }
      else if(imat==0){
       if(_layer==1)return AMSID("AGLU",id+20);     
       else if(x<6) return AMSID("AGLL",id+20);     
       else return AMSID("AGLE",id+20);     
      }
      else if(imat==1){
       if(_layer==1)return AMSID("PMTU",id+30);     
       else if(x<6) return AMSID("PMTL",id+30);     
       else return AMSID("PMTE",id+30);     
      }
      else{
        cerr <<"AMSCTCRawHit::crgid-F-Invalid media par "<<imat<<endl;
        exit(1);
      }
}

//---------------------------------------------------------------------------
void AMSCTCRawHit::sictcdigi(){
  AMSCTCMCCluster * ptr;
  int icnt;
  for(icnt=0;icnt<CTCDBc::getnlay();icnt++){
  integer nrow=CTCDBc::getny()*2;
  integer ncol=CTCDBc::getnx(icnt+1)*2;
  geant * Tmp= (geant*)UPool.insert(sizeof(geant)*nrow*ncol);
  geant * TmpT= (geant*)UPool.insert(sizeof(geant)*nrow*ncol);
   ptr=(AMSCTCMCCluster*)AMSEvent::gethead()->
            getheadC("AMSCTCMCCluster",icnt,1); // last 1  to test sorted container
   VZERO(Tmp,sizeof(geant)*nrow*ncol/4);
   VZERO(TmpT,sizeof(geant)*nrow*ncol/4);
   geant value=0;     
   geant time=0;
   while(ptr){
    integer det=ptr->getdetno()-1;
#ifdef __AMSDEBUG__
    assert (det >=0 && det<2);
#endif
    number z=1.-1./pow(CTCMCFFKEY.Refraction[det],2.)/pow(ptr->getbeta(),2.);
    number Response;
    if(z>0)Response=z;
    else Response=0;
    value+=CTCMCFFKEY.Path2PhEl[det]*ptr->getstep()*Response*ptr->getcharge2();
    time+=ptr->gettime() * CTCMCFFKEY.Path2PhEl[det]*ptr->getstep()*Response*ptr->getcharge2();
    if( ptr->testlast()){
      integer ind=ptr->getcolno()-1+(ptr->getrowno()-1)*ncol;
#ifdef __AMSDEBUG__
   assert(ind >=0 && ind<nrow*ncol);
#endif
      Tmp[ind]+=value;
      TmpT[ind]+=value>0?time/value:time;
      value=0;
      time=0;
    }            

   ptr=ptr->next();  
   }
    for(int i=0;i<nrow*ncol;i++){
     integer ierr,ival;
     if(Tmp[i]>0){
      POISSN(Tmp[i],ival,ierr);
      AMSEvent::gethead()->addnext(AMSID("AMSCTCRawHit",icnt),
      new AMSCTCRawHit(0,i/ncol+1,icnt+1,i%ncol+1,ival,TmpT[i]));
     }      
    }
   UPool.udelete(Tmp); 
   UPool.udelete(TmpT); 
  }
}
//--------------------------------------------------------------------------------------

void AMSCTCRawHit::_writeEl(){

static integer init=0;
static CTCHitNtuple CTCCLN;
int i;

  if(init++==0){
    //book the ntuple block
  HBNAME(IOPA.ntuple,"CTCHit",CTCCLN.getaddress(),
  "CTCHit:I*4,CTCHitStatus:I*4, CTCHitLayer:I*4, CTCHitColumn:I*4,CTCHitRow:I*4,  CTCHitSignal:R*4");

  }
  CTCCLN.Event()=AMSEvent::gethead()->getid();
  CTCCLN.Status=_status;
  CTCCLN.Layer=_layer;
  CTCCLN.Column=_column;
  CTCCLN.Row=_row;
  CTCCLN.Signal=_signal;
  HFNTB(IOPA.ntuple,"CTCHit");





}


void AMSCTCRawHit::_copyEl(){
}


void AMSCTCRawHit::_printEl(ostream & stream){
  stream <<"AMSCTCRawHit row "<<_row<<" col  "<<_column<<" lay  "<<_layer
    <<" sig "<<_signal<<endl;
}
//-----------
// integer * AMSCTCRawHit::_pMap[4]={0,0,0,0};

integer AMSCTCRawHit::_nelem=0;

//
//void AMSCTCRawHit::init(){
//  int icnt;
//  for(icnt=0;icnt<CTCDBc::getnlay();icnt++){
//    integer nrow=CTCDBc::getny()*2;
//    integer ncol=CTCDBc::getnx(icnt+1)*2;
//    _nelem+=nrow*ncol;
//  }
//  for(icnt=0;icnt<4;icnt++){
//   _pMap[icnt]=new integer[_nelem];
//   assert(_pMap[icnt] != NULL);
//  }
//  int il,ir,ic,itot;
//  itot=0;
//  int ielec=0;    // Eugeni's id ( should be changed)
//  for(il=0;il<CTCDBc::getnlay();il++){
//    integer nrow=CTCDBc::getny()*2;
//    integer ncol=CTCDBc::getnx(il+1)*2;
//    for(ir=0;ir<nrow;ir++){
//      for(ic=0;ic<ncol;ic++){
//        _pMap[0][itot]=ic+1;
//        _pMap[1][itot]=ir+1;
//        _pMap[2][itot]=il+1;
//        if(il ==0 || ic<ncol-2){
//         if(ir%2 ==0)_pMap[3][itot]=ielec++;    
//         else if(ic%2)_pMap[3][itot]=_pMap[3][itot-ncol-1];
//         else _pMap[3][itot]=_pMap[3][itot-ncol+1];
//        }
//        else {
//         if(ir%2 ==0)_pMap[3][itot]=ielec++;
//         else _pMap[3][itot]=_pMap[3][itot-ncol];
//        }
//         
//        itot++;
//      }
//    }
//  }
//  // Sort (tricky)
//  //aux address array
//  integer ** aux=new integer*[_nelem];
//  for(int i=0;i<_nelem;i++)aux[i]=_pMap[3]+i;
//  AMSsortNAG(aux,_nelem);
//  // rebuild array;
//  integer tmp;
//  for( i=0;i<_nelem;i++){
//    for(int k=0;k<_nelem;k++){
//      if(_pMap[3]+k == aux[i]){
//        for(int l=0;l<3;l++){
//         tmp=_pMap[l][i];
//         _pMap[l][i]=_pMap[l][k];
//         _pMap[l][k]=tmp; 
//        }
//       }
//    }
//  }
//  AMSsortNAGa(_pMap[3],_nelem);
//  delete [] aux;
//
//}

//integer AMSCTCRawHit::_crsid(){
//
//  for(int i=0;i<_nelem;i++){
//   if(_pMap[2][i]==_layer && _pMap[0][i] == _column && _pMap[1][i]== _row){
//#ifdef __AMSDEBUG__
//   integer *col;
//   integer *row;
//   integer *lay;
//   int n=soft2geo(_pMap[3][i],col,row,lay);
//#endif
//    return _pMap[3][i];
//   }
//  }
//  cerr <<"AMSCTCRawHit::_crsid()-S- cannot find softid"<<endl;
//  _printEl(cerr);
//  return -1;
//}


//integer AMSCTCRawHit::soft2geo(integer softid, integer col[], integer row[], integer lay[]){
//integer ind=AMSbins(_pMap[3],softid,_nelem);
//if (ind <=0){
//  cerr << "AMSCTCRawHit::soft2geo-E-no geo id  "<<endl;
//  col=0;  
//  row=0;  
//  lay=0;  
//  return 0;
//}
//else{
// int m=1;
// col=_pMap[0]+ind-1;
// row=_pMap[1]+ind-1;
// lay=_pMap[2]+ind-1;
// for(int i=ind;i<_nelem;i++){
//  if(softid==_pMap[3][i])m++;
//  else return m;
// }
//return m;
//}
//}
