//  $Id: user.h,v 1.5 2009/01/27 16:10:41 choumilo Exp $
class AMSUser{
private:
  static uinteger _JobFirstRunN;
  static uinteger _PreviousRunN;
  static bool _NewRunStart;
public:
  static uinteger & JobFirstRunN(){return _JobFirstRunN;}
  static uinteger & PreviousRunN(){return _PreviousRunN;}
  static bool & NewRunStart(){return _NewRunStart;}
  void static InitJob();
  void static InitRun();
  void static InitEvent();
  void static Event();
  void static EndJob();
};
//
