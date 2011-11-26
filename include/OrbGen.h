//P.Zuccon 2011
#ifndef  OrbGen_h
#define  OrbGen_h
class MTF1;
class TProfile2D;

typedef unsigned int uint;

class time_rep{
public:
  uint Time_s;
  uint Time_ns;
  short sign;
  time_rep():Time_s(0),Time_ns(0),sign(1){}
  time_rep(uint sec,uint nsec):Time_s(sec),Time_ns(nsec),sign(1){}

  virtual ~time_rep(){}
  time_rep& operator+(const time_rep o);
  time_rep& operator-(const time_rep o);

};


class OrbGen{
private:
  MTF1* proton;
  MTF1* mexp;
  TProfile2D* Rate;
  TProfile2D* LT;
  static OrbGen* Head;
  OrbGen();
public:
  double Lat;
  double Lon;
  double CLat;
  double CLon;
  double Rigidity;
  double Coo[3];
  double Dir[3];
  double cutoff;
  time_rep  Time;
  time_rep  TimeN;
  int Pid;
  static OrbGen* GetOrbGen(){
    if(Head) return Head;
    else{
      Head=new OrbGen();
      return Head;
    }
  }
  static void DeleteOrbGen(){
    if(Head) delete Head;
    Head=0;
  }

  virtual ~OrbGen();
  void SetTime(uint sec,uint nsec=0){ Time.Time_s=sec; Time.Time_ns=nsec;TimeN=Time;}
  void GenDir();
  void NextTime();
  void Print();
  
private:
  void TimePos();
  static void GetPos(uint t, double & Lat, double& Lon);
  double StormerCutoff(double ThetaM,double alt=400);
};

#endif

