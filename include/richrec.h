#include <richdbc.h>





class AMSRichRawEvent: public AMSlink{
private:
  integer _idsoft; //
  integer _padc;   // Pulse hight (ADC-channels)
  geant _x;
  geant _y;
	 
public:
  AMSRichRawEvent(integer idsoft, integer status,  
		  integer padc,geant x,geant y):AMSlink(status,0),
    _idsoft(idsoft),_padc(padc),_x(x),_y(y){};
  ~AMSRichRawEvent(){};
  AMSRichRawEvent * next(){return (AMSRichRawEvent*)_next;}

  integer getid() const {return _idsoft;}
  integer getpadc(){return _padc;}

  static void mc_build();

// interface with DAQ :

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
