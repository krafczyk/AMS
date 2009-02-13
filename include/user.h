//  $Id: user.h,v 1.6 2009/02/13 16:30:49 choumilo Exp $
class AMSUser{
private:
  static uinteger _JobFirstRunN;
  static uinteger _PreviousRunN;
  static time_t _RunFirstEventT;
public:
  static uinteger & JobFirstRunN(){return _JobFirstRunN;}
  static uinteger & PreviousRunN(){return _PreviousRunN;}
  static time_t & RunFirstEventT(){return _RunFirstEventT;}
  void static InitJob();
  void static InitRun();
  void static InitEvent();
  void static Event();
  void static EndJob();
};
//
