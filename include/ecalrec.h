//
// 28.09.1999 E.Choumilov
//
#ifndef __AMSECALREC__
#define __AMSECALREC__
#include <typedefs.h>
#include <point.h>
#include <link.h>
#include <event.h>
#include <ecaldbc.h>
//
//---------------------------------------
class AMSEcalRawEvent: public AMSlink{
private:
  static integer trpatt; // 
  int16u idsoft; // SSCC (SS->S-layer, CC->Cell)
  int16u status; // status (0/1/... -> alive/dead/...)
  int16u padc;// Pulse hight (ADC-channels)
public:
  AMSEcalRawEvent(int16u _idsoft, int16u _status,
        int16u _padc):idsoft(_idsoft),status(_status),padc(_padc)
  {};
  ~AMSEcalRawEvent(){};
  AMSEcalRawEvent * next(){return (AMSEcalRawEvent*)_next;}
//
  integer operator < (AMSlink & o)const{
                return idsoft<((AMSEcalRawEvent*)(&o))->idsoft;}
//
  int16u getid() const {return idsoft;}
  int16u getstat() const {return status;}
  int16u getpadc(){return padc;}
//
  static void setpatt(integer patt){
    trpatt=patt;
  }
  static integer getpatt(){
    return trpatt;
  }
  static void mc_build(int &stat);
  static void validate(int &stat);
//
// interface with DAQ :
//
protected:
  void _printEl(ostream &stream){
    int i;
    stream <<"AMSEcalRawEvent: id="<<dec<<idsoft<<endl;
    stream <<hex<<padc<<endl;
    stream <<dec<<endl<<endl;
  }
void _writeEl(){};
void _copyEl(){};
//
};
#endif
