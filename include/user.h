//  $Id: user.h,v 1.4 2009/01/27 08:09:22 choumilo Exp $
class AMSUser{
public:
void static InitJob();
void static InitRun();
void static InitEvent();
void static Event();
void static EndJob();
};
//
//class AMSCalib{
//private:
//  static uinteger JobFirstRunN;
//  static bool NewRunStart;
//public:
//  static uinteger JobFirstRunN(){return JobFirstRunN;}
//  static bool NewRunStart(){return NewRunStart;}
//}
