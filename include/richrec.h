#include <richdbc.h>


class AMSRichRawEvent: public AMSlink{
private:
  integer _channel; // (PMT number-1)*16+window number
  integer _counts; // 0 means true, 1 means noise
	 
public:
  AMSRichRawEvent(integer channel,integer counts):AMSlink(),
    _channel(channel),_counts(counts){};
  ~AMSRichRawEvent(){};
  AMSRichRawEvent * next(){return (AMSRichRawEvent*)_next;}

  integer getchannel() const {return _channel;}
  integer getcounts() {return _counts;}

  static void mc_build();

// interface with DAQ :

protected:
  void _printEl(ostream &stream){
    int i;
    stream <<"AMSRichRawEvent: id="<<_channel<<endl;
    stream <<" Adc="<<_counts<<endl;
  }
void _writeEl();
void _copyEl(){};
//
};
