#ifndef __RICHCAL__
#define __RICHCAL__

#include <assert.h>
#include <string.h>
#include <math.h>

template <class T> class SH1{
  // Simple 1D histogram class

 private:
  int _bins;
  int _true_bins;
  float _minx;
  float _maxx;
  float _width_1;
  float _width;
  unsigned long _entries;
  T *_bin_content;
 public:
  SH1(int bins,float minx,float maxx):_bins(bins),_minx(minx),_maxx(maxx){
    assert(maxx>minx && bins>0);
    _true_bins=_bins+2;
    _bin_content=new T[_true_bins];
    _width_1=_bins/(_maxx-_minx);
    _width=(_maxx-_minx)/_bins;
    _entries=0;
  }

  SH1():_bins(0),_true_bins(0),_minx(0),_maxx(0),_bin_content(0){}

  ~SH1(){if(_bin_content) delete [] _bin_content;}

  int findbin(float x){
    int bin=int(floor((x-_minx)*_width_1));
    if(bin<0) bin=-1;
    if(bin>=_bins) bin=_bins;
    return bin;
  }

  float getx(int bin){
    if(bin<0) return _minx;
    if(bin>=_bins) return _maxx;
    
    return _minx+_width*(bin+0.5); 
  }

  float binmin(int bin){
    if(bin<0) return -INFINITY;
    if(bin>=_bins) return _maxx;
    return _minx+bin*_width;
  }
  
  float binmax(int bin){
    if(bin<0) return _minx;
    if(bin>=_bins) return INFINITY;
    return _minx+bin*_width+_width;
  }

  
  void reset(){
    memset(_bin_content,0,_true_bins*sizeof(T));
    _entries=0;
  }

  T& operator[](int i) {
    assert(i>=-1 && i<=_bins);
    return _bin_content[i+1];
  }

  T& operator[](float x) {
    int i=findbin(x);
    assert(i>=-1 && i<=_bins);
    return _bin_content[i+1];
  }

  T getbincontent(int i){
    assert(i>=-1 && i<=_bins);
    return _bin_content[i];
  } 
  
  void add(float x,T value){
    (operator[](x))+=value;
    _entries++;
  }
  
  
  void fill(float x){
    add(x,1);
  }

  int bins(){return _bins;}
  float minx(){return _minx;}
  float maxx(){return _maxx;}
  unsigned long entries(){return _entries;}
  T overflow(){return _bin_content[_true_bins-1];}
  T underflow(){return _bin_content[0];}


  void add(SH1<T> &histo){
    assert(_true_bins==histo._true_bins);
    assert(_maxx==histo._maxx && _minx==histo._minx);
    for(int i=0;i<_true_bins;i++) _bin_content[i]=_bin_content[i]+histo._bin_content[i];
    _entries+=histo._entries;
  }

};


#include "richid.h"
#include "trigger102.h"
#include "richrec.h"
#include "particle.h"

class AMSRichCalChannel{
 public:
  typedef SH1<unsigned int> Histo;

  // Class to calibrate a single channel
 private: 
  Histo *_monitor;         // Histogram for monitoring
  Histo *_calibrator;      // Histogram for calibration
  
  double _gain_reference;        // Reference values
  double _gain_sigma_reference;
  
  int _outsider_counter;
  unsigned int _status;

 public:
  AMSRichCalChannel(int bins,float minx,float maxx){_monitor=new Histo(bins,minx,maxx);_calibrator=new Histo(bins,minx,maxx);_outsider_counter=0;_status=0;};
  ~AMSRichCalChannel(){delete _monitor;delete _calibrator;}

  void SetGainReference(double gain){_gain_reference=gain;}
  double GetGainReference(){return _gain_reference;}
  void SetGainSigmaReference(double sigma){_gain_sigma_reference=sigma;}
  double GetGainSigmaReference(){return _gain_sigma_reference;}
  void add(float pe){_monitor->fill(pe);}
  Histo *monitor(){return _monitor;}
  Histo *calibrator(){return _calibrator;}
  int GetCounter(){return _outsider_counter;}
  void AddOrbit(){_outsider_counter++;_calibrator->add(*_monitor);_monitor->reset();}
  void ForgetOrbit(){_monitor->reset();if(_outsider_counter>0)_calibrator->reset();_outsider_counter=0;}
  int Calibrate(){return 0;}

  // Status getters and setters
  void FlagLowStat(){_status|=1<<2;}
  int LowStat(){return _status&(1<<2);} 

  void FlagCalibrated(){_status|=1;}
  int Calibrated(){return _status&1;}

  // Status constants
  static int calibrated_but_failed;
  static int calibrated;
  static int low_stat;
};


class AMSRichCal{
 public:
  static void init(int bins=1000,float minx=0,float maxx=5.);  // Number of bins per histogram
  static void finish();  // Save the calibration and free memory 

  static void process_event();  // Perform all the job
  static AMSRichRing *event_selection();


  // Monitoring and calibration parameters
  static unsigned int _events_per_orbit;
  static unsigned int _orbits_per_calibration;
  static double       _threshold;
  
  static float        _lower_threshold;
  static float        _upper_threshold;
  static float        _charge_lower_threshold;
  static float        _charge_upper_threshold;  
  static int          _low_stat_threshold;

 private:
  static AMSRichCalChannel* _channel[RICmaxpmts*RICnwindows];
  static int _histos;    // Number of histos=RICmaxpmts*RICnwindow
  static int _calibration;    // Number of histos=RICmaxpmts*RICnwindow

  static unsigned int _processed_events;
  static unsigned int reason;
  
 public:
  static int isCalibration(){return _calibration;}

};







#endif
