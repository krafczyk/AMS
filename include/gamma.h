// Author G.LAMANNA 13-Sept-2002

//#include <ecalrec.h>
#include <typedefs.h>
#include <iostream.h>
#include <math.h>




class XZLine_TOF{
protected:
  int fbot,ftop;
  double x_star,z_star;
  double SLOPE,INTER;
  int* LL;
  int* NN;
public:
 XZLine_TOF(int* mul,int* nm, double&, number&);
 inline void intercept(double X1,double Y1,double Z1,double X2,double Y2,double Z2,double& MX,double& QX,double& MY,double& QY){
  // slope in XZ : MX
    MX = (X2-X1)/(Z2-Z1);
  // intercept in XZ : QX 
    QX = X1- (Z1*(X2-X1))/(Z2-Z1); 
  // slope in YZ : MY
   MY = (Y2-Y1)/(Z2-Z1);
  // intercept in YZ : QY 
    QY = Y1- (Z1*(Y2-Y1))/(Z2-Z1);}
 //void intercept(double, double, double, double, double, double, double& , double& , double& , double& );
 void _getTofMul(int, int mul[], double&);
 void _getEcalMul(int, int nn[], number&);
 void TopSplash(double&);
 void Check_TRD_TK1(int,vector<double>,int jj[]);
 void makeEC_out(number&, int&);
 // void makeTOF_out(int&);
 void Lines_Top_Bottom(int&);
 //
 void getParRoadXZ(int& bott, int& topp, double& x_ss, double& z_ss, double& SLL, double& INTT)const;
};

class MYlfit{
 protected:
  double a,b,delta,VAR;
  double* x;
  double* y;
  int L;
  double sigma(double* r);
  double sigma(double* r, double* s);
 public:
  MYlfit(int M, double* xp, double* yp);
  void make_a();
  void make_b();
  void make_VAR();
  void display();
  double _getA();
  double _getB(); 
  double _getVAR(); 
};


class MYlsqp2{
 protected:
  double a,b,c,sdw;
  double n1[3],n2[3],n3[3],n4[3];
  double al,be,ga,rho,eps,bi;
  int L;
  double* x;
  double* y;
  double sigma(double* r);
  double sigma(double* r, double* s);
  double sigma(double* r, double* s, double* t);
  double sigma(double* r, double* s, double* t, double* u);
 public:
  MYlsqp2( int M, double* xp, double* yp);
  void make_param();
  void make_a();
  void make_b();
  void make_c();
  void make_SDW();
  double _getA();
  double _getB();
  double _getC();
  double _getSDW(); 
};
