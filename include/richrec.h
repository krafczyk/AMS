#include <richdbc.h>


class AMSRichRawEvent: public AMSlink{
private:
  integer _idsoft; // (PMT number-1)*16+window number
  integer _status; // 0 means true, 1 means noise
  integer _padc;   // ADC counts
  geant _x;        // Reconstructed x for the hit
  geant _y;        // Reconstructed y for the hit
  AMSPoint _origin;   // Canonical origin for the hit
  AMSPoint _momentum; // Canonical momentum at the origin 
	 
public:
  AMSRichRawEvent(integer idsoft, integer status,  
		  integer padc,geant x,geant y,
		  AMSPoint origin,AMSPoint momentum):AMSlink(status,0),
    _idsoft(idsoft),_status(status),_padc(padc),_x(x),_y(y),
    _origin(origin),_momentum(momentum){};
  ~AMSRichRawEvent(){};
  AMSRichRawEvent * next(){return (AMSRichRawEvent*)_next;}

  integer getid() const {return _idsoft;}
  integer getstatus() {return _status;}
  integer getadc(){return _padc;}
  geant getx(){return _x;}
  geant gety(){return _y;}
  geant getorigin(integer i){return i>=0 && i<3 ? _origin[i]:0;}
  geant getmomentum(integer i){return i>=0 && i<3 ? _momentum[i]:0;}

  static void mc_build();

// interface with DAQ :

protected:
  void _printEl(ostream &stream){
    int i;
    stream <<"AMSRichRawEvent: id="<<_idsoft<<endl;
    stream <<" Adc="<<_padc<<endl;
  }
void _writeEl(){};
void _copyEl(){};
//
};
