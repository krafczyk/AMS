//
// 28.09.1999 E.Choumilov
//
#ifndef __AMSECALREC__
#define __AMSECALREC__
#include <typedefs.h>
#include <point.h>
#include <link.h>
#include <event.h>
#include <stdlib.h>
#include <time.h>
#include <ecaldbc.h>
//
//---------------------------------------
class AMSEcalRawEvent: public AMSlink{
private:
  integer _idsoft; //readout cell ID=SSPPC (SS->S-layer,PP->PMcell, C->SubCell in PMcell)
//integer _status; // status (0/1/... -> alive/dead/...) (It is really in AMSlink !!!)
  integer _padc;// Pulse hight (ADC-channels)
public:
  AMSEcalRawEvent(integer idsoft, integer status,  
        integer padc):AMSlink(status,0),_idsoft(idsoft),_padc(padc)
  {};
  ~AMSEcalRawEvent(){};
  AMSEcalRawEvent * next(){return (AMSEcalRawEvent*)_next;}
//
  integer operator < (AMSlink & o)const{
                return _idsoft<((AMSEcalRawEvent*)(&o))->_idsoft;}
//
  integer getid() const {return _idsoft;}
  integer getpadc(){return _padc;}
//
  static void mc_build(int &stat);
  static void validate(int &stat);
//
// interface with DAQ :
//
protected:
  void _printEl(ostream &stream){
    int i;
    stream <<"AMSEcalRawEvent: id="<<dec<<_idsoft<<endl;
    stream <<" Status="<<hex<<_status<<" Adc="<<_padc<<endl;
    stream <<dec<<endl<<endl;
  }
void _writeEl(){};
void _copyEl(){};
//
};
//-----------------------------------
class AMSEcalHit: public AMSlink{
private:
//integer _status; // status (0/1/... -> alive/dead/...) (It is really in AMSlink !!!)
  integer _proj;   //projection (0->X, 1->Y)
  integer _plane;  //continious numbering of planes through 2 projections(0,...)
  integer _cell;   // numbering in plane(0,...)
  number  _edep;
  number  _coot;   //transv.coord.
  number  _cool;   //long.coord.
  number  _cooz;
public:
  AMSEcalHit(integer status, integer proj, integer plane, integer cell,number edep,
         number coot, number cool, number cooz):AMSlink(status,0),_proj(proj),
	 _plane(plane),_cell(cell),_edep(edep),_coot(coot),_cool(cool),_cooz(cooz){};
  ~AMSEcalHit(){};
  AMSEcalHit * next(){return (AMSEcalHit*)_next;}
//
  integer getproj(){return _proj;}
  integer getplane(){return _plane;}
  integer getcell(){return _cell;}
  number getedep(){return _edep;}
  number getcoot(){return _coot;}
  number getcool(){return _cool;}
  number getcooz(){return _cooz;}
//
  static void build(int &stat);
//
protected:
  void _printEl(ostream &stream){
    int i;
    stream <<"AMSEcalHit: Proj/Plane/cell= "<<_proj<<" "<<_plane<<" "<<_cell<<endl;
    stream <<"Status="<<hex<<_status<<"  Edep="<<dec<<_edep<<endl; 
    stream <<"Coot/l/z= "<<_coot<<" "<<_cool<<" "<<_cooz<<endl;
    stream <<dec<<endl<<endl;
  }
void _writeEl(){};
void _copyEl(){};
//
};
//-----------------------------------
class AMSEcalCluster: public AMSlink{ // 1-D cluster
private:
//integer _status; // status (0/1/... -> alive/dead/...) (It is really in AMSlink !!!)
  integer _proj;   //projection (0->X, 1->Y)
  integer _plane;  //continious numbering of planes through 2 projections(0,...)
  number  _edep;
  AMSPoint _Coo;  // cluster center of gravity coordinates (cm)
  AMSPoint _ErrorCoo; // Error to coordinates
  integer _nmemb; // number of EcalHit members
  AMSEcalHit * _mptr[2*ECPMSMX]; // list of pointers to member
public:
  AMSEcalCluster(integer status, integer proj, integer pl, number edep, AMSPoint coo,
     AMSPoint ecoo, integer nm, AMSEcalHit *ptr[]): AMSlink(status,0),_proj(proj),
		        _plane(pl),_edep(edep),_Coo(coo),_ErrorCoo(ecoo),_nmemb(nm){
    for(int i=0;i<nm;i++)_mptr[i]=ptr[i];
  };
  ~AMSEcalCluster(){};
  AMSEcalCluster * next(){return (AMSEcalCluster*)_next;}
//
  integer getproj(){return _proj;}
  integer getplane(){return _plane;}
  number getedep(){return _edep;}
  AMSPoint getcoo()const {return _Coo;}
  AMSPoint getecoo()const {return _ErrorCoo;}
  integer getmemb(AMSEcalHit *ptr[]){
    for(int i=0;i<_nmemb;i++)ptr[i]=_mptr[i];
    return _nmemb;
  }
//
  static void build(int &stat);
  static integer Out(integer);
  static void init();
//
protected:
  void _printEl(ostream &stream){
    int i;
    stream <<"AMSEcalCluster: Proj= "<<_proj<<" plane="<<_plane<<endl;
    stream <<"Status="<<hex<<_status<<"  Edep="<<dec<<_edep<<endl; 
    stream <<"Coord="<<_Coo[0]<<" "<<_Coo[1]<<" "<<_Coo[2]<<"  ErrCoo="<<_ErrorCoo<<endl;
    stream <<"Nmembers="<<_nmemb<<endl;
    stream <<dec<<endl<<endl;
  }
void _writeEl();
void _copyEl();
//
};
#endif
