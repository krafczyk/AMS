//  $Id: user.h,v 1.7 2011/03/22 11:20:58 choumilo Exp $
class AMSUser{
private:
  static uinteger _JobFirstRunN;
  static uinteger _PreviousRunN;
  static time_t _RunFirstEventT;
  static time_t _JobFirstEventT;
public:
  static uinteger & JobFirstRunN(){return _JobFirstRunN;}
  static uinteger & PreviousRunN(){return _PreviousRunN;}
  static time_t & RunFirstEventT(){return _RunFirstEventT;}
  static time_t & JobFirstEventT(){return _JobFirstEventT;}
  void static InitJob();
  void static InitRun();
  void static InitEvent();
  void static Event();
  void static EndJob();
};
//
