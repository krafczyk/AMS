//  $Id: gamma.C,v 1.15 2002/11/29 17:56:58 glamanna Exp $
// Author G.LAMANNA 13-Sept-2002
//
// See gamma.h for the Class AMSTrTrackGamma initialization.
//
//
#include <trrec.h>
#include <commons.h>
#include <math.h>
#include <limits.h>
#include <job.h>
#include <amsstl.h>
#include <trrawcluster.h>
#include <extC.h>
#include <upool.h>
#include <string.h>
#include <tofrec02.h>
#include <ntuple.h>
#include <cont.h>
#include <tkdbc.h>
#include <trigger302.h>
#include <tralig.h>
#include <mccluster.h>
#include <trdrec.h>
#include <iomanip>
#include <string>
#include <vector>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <algorithm>
#include <gamma.h>
#include <ecalrec.h>
#include <event.h>
#include <link.h>
#include <ecaldbc.h>
#include <ecalcalib.h>
#include <cern.h>
#include <point.h>
using namespace std;
using namespace ecalconst;



void AMSTrTrackGamma::_SingleHit(integer FLPAT[], double CE[], int LA){
  double z_fi2[trconst::maxlay];
  double z_tkl[trconst::maxlay];
  double middle[trconst::maxlay];
  double z_ref=10000;
  double di_zz2[trconst::maxlay][trconst::maxlay];
  double d_cres;
  double di_max;
  int L=2;
  if (LA>=3) L=3;
  double y_ce[3];
  double z_ce[3];
  int l_z[3][trconst::maxlay];
  double AF;
  double BF;
  double VARR;

 for(int i=0;i<TKDBc::nlay();i++){
#ifdef __AMSDEBUG__
   cout << "In SingleHIt  FLPAT["<<i<<"] ="<<FLPAT[i]<<endl;
   cout << "In SingleHIt     CE["<<i<<"]= "<< CE[i]<<endl;
#endif
   z_tkl[i]=TKDBc::zposl(i);
   z_fi2[i]=10000;
   middle[i]=10000;
   if (FLPAT[i]==2){
     z_fi2[i]=z_tkl[i]; 
   }
   for(int k=0;k<3;k++){
    l_z[k][i]=0;
   }
 }
 // I step
 for(int i=0;i<TKDBc::nlay();i++){
   if (FLPAT[i]==1){
     //
    for(int j=0;j<TKDBc::nlay();j++){
     di_zz2[j][i]=10000;
    }
    //
    if (i == 2 || i == 4 || i == 6){
     if (FLPAT[i-1]==2){
       middle[i]=CE[i-1];
     }
    }
    if (i == 1 || i == 3 || i == 5){
     if (FLPAT[i+1]==2){
       middle[i]=CE[i+1];
     }
    }
    //
    z_ref=z_tkl[i];
    //
    for(int j=0;j<TKDBc::nlay();j++){
     di_zz2[j][i]=fabs(z_ref-z_fi2[j]);
    }
    //
   }
 }
 if (LA >= 2){
 // II step, in case middle was not found
 for(int i=0;i<TKDBc::nlay();i++){
   if (FLPAT[i]==1 && middle[i]==10000){
    z_ref=z_tkl[i];
    d_cres=0;
    for(int k=0;k<L;k++){
      di_max=10000;
      for(int j=0;j<TKDBc::nlay();j++){
	if (di_zz2[j][i] < di_max && di_zz2[j][i] > d_cres){       
         l_z[k][i]=j;
         di_max=di_zz2[j][i];
	}
      }
     d_cres=di_max; 
    } //k=1,L

    for(int k=0;k<L;k++){
    y_ce[k]=CE[l_z[k][i]];
    z_ce[k]=z_fi2[l_z[k][i]];
    }

    dlinearme(L,z_ce,y_ce,AF,BF,VARR);         // linear fit
    middle[i]=(BF*z_ref)+AF;
#ifdef __AMSDEBUG__
    cout<< "AF,BF,z_ref  "<< AF<<" "<<BF<<" "<<z_ref<<endl; 
    cout<< "****into single LFIT middle["<<i<<"]= "<< middle[i]<<endl;
#endif   
   }
 }
 }// LA

 for(int i=0;i<TKDBc::nlay();i++){
   if (FLPAT[i]==1 && middle[i] != 10000){
     CE[i]=middle[i];
   }
#ifdef __AMSDEBUG__
cout<< "****into single CE["<<i<<"]= "<< CE[i]<<endl;
#endif
 }
 //
 // One more alternative:
 int cou1a=0;    
 int cou1b=0;    
 for(int i=0;i<5;i++){
  if (FLPAT[i]==1){
    cou1a++; // first half TK 1, 2,3, 4,5
  }
 }

 if (cou1a <= 2 ){
   int jw=-1;
   double y_v[5];
   double z_v[5];
   for(int i=0;i<5;i++){
     if (FLPAT[i]==2){
       jw++;
       y_v[jw]=CE[i];
       z_v[jw]=z_tkl[i];
     }
   }
   dlinearme((jw+1),z_v,y_v,AF,BF,VARR);         // linear fit
   double mid[5];
   for(int i=0;i<5;i++){
     if (FLPAT[i]==1){
       mid[i]=(BF*z_tkl[i])+AF;
       if (fabs(CE[i]-mid[i]) > 0.4) CE[i]=mid[i];    
     }
   }
 }
   //   
 for(int i=3;i<8;i++){
  if (FLPAT[i]==1){
    cou1b++; // first half TK 4,5, 6,7, 8
  }
 }
 if (cou1b <= 2 ){
   int jw=-1;
   double y_v[5];
   double z_v[5];
   for(int i=3;i<8;i++){
     if (FLPAT[i]==2){
       jw++;
       y_v[jw]=CE[i];
       z_v[jw]=z_tkl[i];
     }
   }
   dlinearme((jw+1),z_v,y_v,AF,BF,VARR);         // linear fit
  double mid[5];
   for(int i=3;i<8;i++){
      if (FLPAT[i]==1){
       mid[i]=(BF*z_tkl[i])+AF;
       if (fabs(CE[i]-mid[i]) > 0.4) CE[i]=mid[i];    
     }
   }
 }
 //
#ifdef __AMSDEBUG__
 for(int i=0;i<TKDBc::nlay();i++){
   cout<< "******into SingleHIt CE["<<i<<"]= "<< CE[i]<<endl;
 }
#endif
// ..alter

}

void AMSTrTrackGamma::_LookOneEight(integer FLPAT[], vector<double> H[]){

  double mire8[4]={1.5,1.6,3,3.2};
  double mare8[4]={2.3,2.5,7.5,13};
  double mare1[4]={0.55,0.5,0.25,0.2};
  double re[trconst::maxlay];
  double z_tkl[trconst::maxlay];
  double mid[trconst::maxlay];

  for(int i=0;i<TKDBc::nlay();i++){
    z_tkl[i]=TKDBc::zposl(i);
      if (FLPAT[i]==2){
	re[i]=fabs(H[i][0]-H[i][1]);
       mid[i]=(H[i][0]+H[i][1])/2;
      }
  }
  ////////
  // TK1
  if (FLPAT[0] == 2){
    int ja=-1;
  double yc[4];
  double zc[4];
  
  for(int j=1;j<5;j++){
    if (FLPAT[j]==2){
      ja++;
      yc[ja]=mid[j];
      zc[ja]=z_tkl[j];
    }
  }
  if ( ja == 0 ){
    for(int j=5;j<8;j++){
      if (FLPAT[j]==2){
	ja++;
	yc[ja]=mid[j];
	zc[ja]=z_tkl[j];
      }
    }
  }
  //  AMSTrTrackGamma * punto;
  double thc1,A,B,VAR;
  double off1=10000;
  if (ja >= 1){
    dlinearme((ja+1),zc,yc,A,B,VAR);
    thc1=A+B*z_tkl[0];
    for(int j=0;j<ja+1;j++){
      for(int ii=0;ii<TKDBc::nlay();ii++){
	if (z_tkl[ii] == zc[ja]){
	  if ( re[0] > mare1[ii-1]*re[ii] ){//too large
	    off1=(fabs(H[0][0]-thc1) > fabs(H[0][1]-thc1))?H[0][0]:H[0][1];
	  }
	}
      } 
    }
  }

  if (off1 != 10000){
    double th[2];
    for(int jo=0;jo<2;jo++){
      if (H[0][jo] == off1)  H[0][jo]=10000;
      th[jo]=H[0][jo]; 
    } 
    //
    H[0].clear();
    H[0].resize(0);
    for(int jo=0;jo<2;jo++){
      if (th[jo] != 10000){
	H[0].push_back(th[jo]);
      }  
    }  
   FLPAT[0]=H[0].size();
  }
 }// TK1
  ////////////
  // TK8
  if (FLPAT[7] == 2){

  int ja=-1;
  double yc[4];
  double zc[4];
  
  for(int j=3;j<7;j++){
    if (FLPAT[j]==2){
      ja++;
      yc[ja]=mid[j];
      zc[ja]=z_tkl[j];
    }
  }

  if ( ja == 0 ){
    for(int j=0;j<3;j++){
      if (FLPAT[j]==2){
	ja++;
	yc[ja]=mid[j];
	zc[ja]=z_tkl[j];
      }
    }
  }
  
  //  AMSTrTrackGamma * punto;
  double thc8,A,B,VAR;
  double off8=10000;
  if (ja >= 1){
    dlinearme((ja+1),zc,yc,A,B,VAR);
    thc8=A+B*z_tkl[7];
    for(int j=0;j<ja+1;j++){
      for(int ii=0;ii<TKDBc::nlay();ii++){
	if (z_tkl[ii] == zc[ja]){
	  if ( re[7] > mare8[6-ii]*re[ii] ){//too large
	    off8=(fabs(H[7][0]-thc8) > fabs(H[7][1]-thc8))?H[7][0]:H[7][1];
	  }
	}
      } 
    }
  }


  if (off8 != 10000){
    double th[2];
    for(int jo=0;jo<2;jo++){
      if (H[7][jo] == off8)  H[7][jo]=10000;
      th[jo]=H[7][jo]; 
    } 
    //
    H[7].clear();
    H[7].resize(0);
    for(int jo=0;jo<2;jo++){
      if (th[jo] != 10000){
	H[7].push_back(th[jo]);
      }  
    }  
   FLPAT[7]=H[7].size();
  }
 } // TK8
  //..
}

void AMSTrTrackGamma::_LSQP2(integer FLPAT[], vector<double> H[], integer esc_1, integer esc_2){

  
  double z_tkl[trconst::maxlay];

  for(int i=0;i<TKDBc::nlay();i++){
    z_tkl[i]=TKDBc::zposl(i);
#ifdef __AMSDEBUG__  
  cout<<"esc_1 esc_2 "<<esc_1<<" "<<esc_2<<endl;
    cout << "z_tkl["<<i<<"] ="<<z_tkl[i]<<endl;
    cout << "FLPAT["<<i<<"] ="<<FLPAT[i]<<endl;
    AMSPoint p_hi;
    AMSTrRecHit* pre;
    for (pre=AMSTrRecHit::gethead(i); pre!=NULL; pre=pre->next()){
     if(pre->checkstatus(AMSDBc::FalseX) || pre->checkstatus(AMSDBc::FalseTOFX))continue;
      p_hi = pre->getHit(); 
      number cha = pre->getsum();
      cout<< "* OOOOOOOOO cha & p_hi[1 .. 3] = "<< cha<< " "<<p_hi[0]<<" "<<p_hi[1]<<" "<<p_hi[2]<<endl; 
    }
    for(int j=0;j<FLPAT[i];j++){
      cout << "AOOOOOOOOOOOOO *** H["<<i<<"]["<<j<<"]=  "<< H[i][j] <<endl;
    }
#endif
  }
  // 
  int ja;
  double res_[trconst::maxlay];
  double mid[trconst::maxlay];
  double z_fi[trconst::maxlay];
  double res_LR[500][trconst::maxlay];
  
  for(int i=0;i<TKDBc::nlay();i++){
    z_fi[i]=10000;
    res_[i]=10000;
    for(int k=0;k<500;k++){
      res_LR[k][i]=10000;
    }
  }
  double CHI2[500];
  double LCHI2[500];
  double AA0;
  double AA1;
  double AA2;
  double SDW;
  int IFAIL;
  double A0;
  double A1;
  double VAR;
  int l_[trconst::maxlay];
  int l1_[trconst::maxlay];
  int l2_[trconst::maxlay];
  for(int i=0;i<TKDBc::nlay();i++){
    l_[i]=10000;
    l1_[i]=10000;
    l2_[i]=10000;
  }

  int bestflag1=0;
  double bestdy1=10000;
  double besty1=10000;




 if (esc_1 >= 2 && esc_2 != 0){
    ja=-1;
    for(int i=0;i<TKDBc::nlay();i++){
      if (FLPAT[i]==2){
	ja++;  
	res_[ja]=fabs(H[i][0]-H[i][1]);
	z_fi[ja]=z_tkl[i];
	// for the alternative let's do the centers in y
         mid[ja]=(H[i][0]+H[i][1])/2;
        //
      }
    }
    //
  int j;
  double sdwmax,varmax;
  double zbef,rbef;
  for(int i=0;i<TKDBc::nlay();i++){
    if (FLPAT[i]>=3){
      ja++;  
      z_fi[ja]=z_tkl[i];
      sdwmax=10000;
      varmax=10000;
      j=-1;
      //
      
      for (int k=0;k<ja;k++){
	if (z_fi[k] > z_fi[ja]){
	  zbef=z_fi[k];
	  rbef=res_[k];
	}
      }
      //
      for(int i1=0;i1<FLPAT[i]-1;i1++){
	for(int i2=i1+1;i2<FLPAT[i];i2++){
	  j++;
	  res_LR[j][i]=fabs(H[i][i1]-H[i][i2]);
	  res_[ja]=res_LR[j][i];
	  // for the alternative let's do the centers in y
	  mid[ja]=(H[i][i1]+H[i][i2])/2;
	  //
	  if (esc_1 >= 3){  
	    dlsqp2me((ja+1),z_fi,res_,AA0,AA1,AA2,SDW);
#ifdef __AMSDEBUG__
	    cout << "AA0,AA1,AA2,SDW"<<AA0<<" "<<AA1<<" "<<AA2<<" "<<SDW<<endl;
	    cout << "res_["<<ja<<"] = "<<res_[ja]<<endl;
#endif
	    if (AA0 == 0 && AA1 == 0 && AA2 == 0 && SDW  == 10000){
	      dlinearme((ja+1),z_fi,mid,A0,A1,VAR);
#ifdef __AMSDEBUG__
	      cout << "POLY 2 DOES NOT WORK, WE USE AN ALTERNATIVE"<<endl;
	      cout <<"rbef= "<<rbef<<endl;
	      cout << "res_["<<ja<<"] "<<res_[ja]<<endl;
	      cout << "varmax = "<<varmax<<endl;
#endif
           if (res_[ja] > rbef){
            LCHI2[j]=VAR;
            if (LCHI2[j]<varmax){
	    l_[i]=j;
	    l1_[i]=i1;
	    l2_[i]=i2;
	    varmax=LCHI2[j];
            }
	    }
	  } //AA0 == 0 ALTERNATIVE END
	  //
          //
	  CHI2[j]=SDW;
	  if (CHI2[j]<sdwmax){
	  l_[i]=j;
          l1_[i]=i1;
	  l2_[i]=i2;
	  sdwmax=CHI2[j];
	 }
	} // esc_1 >= 3
	// //
        // // When the double hit planes are only 2
        if (esc_1 == 2){
	  // WHEN the increasing order of residuals w.r.t. z is respected:
	  if (((z_fi[ja] < z_fi[0] && res_[ja] >= res_[0]) || 
	       (z_fi[ja] > z_fi[0] && res_[ja] <= res_[0])) && 
	      ((z_fi[ja] < z_fi[1] && res_[ja] >= res_[1]) || 
	       (z_fi[ja] > z_fi[1] && res_[ja] <= res_[1]))){
	    
	    dlsqp2me((ja+1),z_fi,res_,AA0,AA1,AA2,SDW);
#ifdef __AMSDEBUG__
	    cout << "AA0,AA1,AA2,SDW"<<AA0<<" "<<AA1<<" "<<AA2<<" "<<SDW<<endl;
#endif
          if (AA0 == 0 && AA1 == 0 && AA2 == 0 && SDW  == 10000){
            dlinearme((ja+1),z_fi,mid,A0,A1,VAR);
            if (res_[ja] > rbef){
            LCHI2[j]=VAR;
            if (LCHI2[j]<varmax){
	    l_[i]=j;
	    l1_[i]=i1;
	    l2_[i]=i2;
	    varmax=LCHI2[j];
	    }
	    }
	  } //AA0 == 0 ALTERNATIVE END
	  //
	    CHI2[j]=SDW; 
	    if (CHI2[j]<sdwmax){
	      l_[i]=j;
	      l1_[i]=i1;
	      l2_[i]=i2;
	      sdwmax=CHI2[j];
	    }
          } 
	} // esc_1 == 2
	//
      }
    }
#ifdef __AMSDEBUG__
      cout << "FINALLY  res_["<<ja<<"] "<<res_[ja]<<endl;
      cout << "FINALLY  l_["<<i<<"] "<<l_[i]<<endl;
#endif
      if (l_[i] == 10000) return;
      if (l_[i] != 10000){
        res_[ja]=res_LR[l_[i]][i];
      }
#ifdef __AMSDEBUG__
      cout << "FINALLY  res_["<<ja<<"] "<<res_[ja]<<endl;
#endif
    //    if (i == 0){ // first plane
     if (z_fi[ja] == z_tkl[0] && res_[ja] > 1.8) {
      dlinearme((ja+1),z_fi,mid,A0,A1,VAR);
      bestdy1=min(fabs(mid[ja]-H[0][l1_[i]]),fabs(mid[ja]-H[0][l2_[i]]));
      //cout << "bestdy1 = "<<bestdy1<<endl;
      bestflag1=1;
      if(bestdy1 == fabs(mid[ja]-H[0][l1_[i]])) besty1= H[0][l1_[i]];
      if(bestdy1 == fabs(mid[ja]-H[0][l2_[i]])) besty1= H[0][l2_[i]];
     }  

    }
  }
 //
  for(int i=0;i<TKDBc::nlay();i++){
    if (FLPAT[i]>=3){
      //
      const int maxflpat=100;
      double TH[maxflpat];
      int up=min(maxflpat,FLPAT[i]);
      //
      for(int jo=0;jo<up;jo++){
	if (H[i][jo] != H[i][l1_[i]] && H[i][jo] != H[i][l2_[i]] ){
	  H[i][jo]=10000;
	}
        if (bestflag1 != 0 && H[0][jo] != besty1){
	  H[0][jo]=10000;
	}
	TH[jo]=H[i][jo];
      }
   //
      H[i].clear();
      H[i].resize(0);
      for(int jo=0;jo<FLPAT[i];jo++){   
	if (TH[jo] != 10000){
	  H[i].push_back(TH[jo]);
	}  
      }
      //
    }
  }
 //
  } 
}

vector<double> AMSTrTrackGamma::_TK1YRemoveTRD2TOF(vector<double> HH, integer Num){
  double temp;
  //DIGITAL ALPHA
  //  double H1[Num];
  const int maxH1=300;
  double H1[maxH1];
  double VEC,V_LI;
  int Y1Out=0;
  //TOF
  int mto=0;
  int mpl=0;
  int pl=0;
  double x,y,z;
  AMSTOFCluster * ploop;  
  //TRD
  number P0[3];
  number TKP1[3];
  number MinTRDTOF=3.5;
  number SearchReg(4);
  int trdf=0;
  number theta,phi,sleng;
  AMSContainer *pc=AMSEvent::gethead()->getC("AMSTRDTrack",0); 
  
  // LOOKING FOR TOFHITS ON FIRST TOF PLANE

  int j=1;
  for(int i=0;i<4;i++){
   mpl=0;
   for (ploop = AMSTOFCluster::gethead(i); ploop ; ploop=ploop->next() ){
     mpl++;
   }
   if (i==0 && mpl==0) {
    j++;
   }
  }



  for (ploop = AMSTOFCluster::gethead(j); ploop ; ploop=ploop->next() ){
      x = ploop->getcoo()[0];
      y = ploop->getcoo()[1];
      z = ploop->getcoo()[2];
        pl = ploop->getplane();
     mto++;
     P0[2]=z;

 /////TRD

    if(pc){
      AMSTRDTrack * ptrd = (AMSTRDTrack*)pc->gethead();
      while(ptrd){
       trdf++;
       AMSDir s(ptrd->gettheta(),ptrd->getphi());
       if(s[2]!=0){
	  P0[0]=ptrd->getCooStr()[0]+s[0]/s[2]*(P0[2]-ptrd->getCooStr()[2]);
          P0[1]=ptrd->getCooStr()[1]+s[1]/s[2]*(P0[2]-ptrd->getCooStr()[2]);
	  if (fabs(y-P0[1])<=MinTRDTOF) {
	    MinTRDTOF=fabs(y-P0[1]);
	    TKP1[2]=TKDBc::zposl(0);
	    TKP1[1]=ptrd->getCooStr()[1]+s[1]/s[2]*(TKP1[2]-ptrd->getCooStr()[2]);             
	    Y1Out=1;
            
          } 
       }
       ptrd=ptrd->next();
      }
    }
    } //tof

  if (Y1Out){
    VEC=TKP1[1];
    V_LI=1.5*MinTRDTOF;
    for(int j=0;j<Num;j++){
      H1[j]=HH[j];
    }
    HH.clear();
    HH.resize(0);
    for(int j=0;j<Num;j++){ 
      temp=H1[j];
      if (fabs(temp-VEC)<=V_LI){
	HH.push_back(temp);  
      }
    }
  }
  return HH;
}

void AMSTrTrackGamma::_Averes(number& AVER, number& SAVER, vector<double> HH, integer Num, integer Com){

 double SUM=0.0;
 double SI=0.0;
 
 AVER=0.0;
 SAVER=0.0;
 if (Num<=1) {
  AVER=0.0;
 SAVER=0.0;
 }
 if (Num==2 && Com==1){
   AVER+=fabs(HH[0]-HH[1]);
   SAVER=0.0; 
 }
 if (Com>1){
  for(int i=0;i<(Num-1);i++){
   for(int j=(i+1);j<Num;j++){
   AVER+=fabs(HH[i]-HH[j]);  
   }
  }
  AVER=AVER/Com;
  for(int i=0;i<(Num-1);i++){
    for(int j=(i+1);j<Num;j++){
      SUM+=pow((fabs(HH[i]-HH[j])-AVER),2);
    }
  }    
  SI=sqrt(SUM/(Com-1));
  SAVER=SI/(sqrt(number(Com)));
 }
}

void AMSTrTrackGamma::_Cente(number& CEN, number& SCEN, vector<double> HH, integer Num, integer Com, number AVER, number SAVER){

  int nde=0;
  double DSUM=0.0;
  double SI=0.0;
  CEN=0.0;
  SCEN=0.0;
 if (Num==1) {
  CEN=HH[0];
  SCEN=0.0; 
 }
 if (Num==2 && Com==1){
   CEN=(HH[0]+HH[1])/2.;
   SCEN=0.0;
 }
 
 
 if (Com>1){
 for(int i=0;i<(Num-1);i++){
   for(int j=(i+1);j<Num;j++){
     if((fabs(HH[i]-HH[j])>=(AVER-(3.*SAVER))) && (fabs(HH[i]-HH[j])<=(AVER+(3.*SAVER)))){
        CEN+=((HH[i]+HH[j])/2.);
        nde++;  
    }
   }
  }
 CEN=CEN/nde; 
    
  if (nde>1){
  for(int i=0;i<(Num-1);i++){
   for(int j=(i+1);j<Num;j++){
     if(fabs(HH[i]-HH[j])>=(AVER-(3.*SAVER)) &&  fabs(HH[i]-HH[j])<=(AVER+(3.*SAVER))){
        DSUM+=pow((((HH[i]+HH[j])/2.)-CEN),2);
	}
   }
  }
  SI=sqrt(DSUM/(nde-1));
  SCEN=SI/(sqrt(double(nde)));
  }
 }
}
 


void AMSTrTrackGamma::_Combi(integer& NC, integer Nele, integer Ngrp){
 int nf=1;
 int rf=1;
 int NmRf=1;
 if(Nele > 1){
  for(int i=1;i<(Nele+1);i++){
   nf=nf*i;
  }
 }
 if(Ngrp > 1){
  for(int i=1;i<(Ngrp+1);i++){
   rf=rf*i;
  }
 }

 if (Nele==Ngrp) NC=1;
 if (Nele<Ngrp) NC=0;
 if (Ngrp > 1){
  if (Nele>Ngrp) {
   for(int i=1;i<(Nele-Ngrp+1);i++){
    NmRf=NmRf*i;
   }
  NC=nf/(rf*NmRf);
  }
 }

} 



void AMSTrTrackGamma::_LeftRight(vector<double> HH, integer inhi, number CEN){
 AMSPoint p_hi;
 AMSPoint pc_hi;
 AMSTrRecHit* pre;
 int xambig=0;

 number depos;
    for (pre=AMSTrRecHit::gethead(inhi); pre!=NULL; pre=pre->next()){
     if(pre->checkstatus(AMSDBc::FalseX) || pre->checkstatus(AMSDBc::FalseTOFX))continue;
      p_hi = pre->getHit(); 
      number cha = pre->getsum(); 
      number depomin=10000;
      //first of all
            pre->clearstatus(AMSDBc::GAMMARIGHT);
	    pre->clearstatus(AMSDBc::GAMMALEFT);
      //Single Hit:
       if (HH.size()==1){
         if (p_hi[1]==HH[0]){
           // CEN was not found
	   //..........
            // B)
	     // Charge quantity : let's keep the two highest (one for left one for right) even if it is a single hit
             // but with ambiguity otherwise let's keep all
	     //NOW the charge descrimination
             int cmax1=0;
             int cmax2=0;
             number deposmax1=-1;
             number deposmax2=-1;
             for (AMSTrRecHit * pcha=AMSTrRecHit::gethead(inhi); pcha!=NULL; pcha=pcha->next()){
                if(pcha->checkstatus(AMSDBc::FalseX) || pcha->checkstatus(AMSDBc::FalseTOFX))continue;
	         depos = pcha->getsum(); 
                 pc_hi = pcha->getHit();
                 if (pc_hi[1]==p_hi[1] && pc_hi[2]==p_hi[2]){
                   if (depos >= deposmax1){
                     if (deposmax1 == -1 || cmax1>0){
		     if (depos != deposmax1) cmax1=0;
		     deposmax1=depos;
                     cmax1++;//ambiguity
		     }
		   }
		 }
	     }
	     
             for (AMSTrRecHit * pcha=AMSTrRecHit::gethead(inhi); pcha!=NULL; pcha=pcha->next()){
                if(pcha->checkstatus(AMSDBc::FalseX) || pcha->checkstatus(AMSDBc::FalseTOFX))continue;
	         depos = pcha->getsum(); 
                 pc_hi = pcha->getHit();
                 if (pc_hi[1]==p_hi[1] && pc_hi[2]==p_hi[2]){
                   if (depos >= deposmax2 && depos < deposmax1){
                     if (deposmax2 == -1 || cmax2>0){
		     if (depos != deposmax2) cmax2=0;
		     deposmax2=depos;
                     cmax2++; //ambiguity
		     }
		   }
		 }
	     }

	     // A)
	     //                   cout <<" begin: cha "<<cha<<" p_hi[0] "<<p_hi[0]<<" p_hi[1] "<<p_hi[1]<<" p_hi[2] "<<p_hi[2]<<endl;
             //let's eliminate hit with no ambiguity
                 xambig=0; 
		 //                   cout <<" begin xambig "<<xambig<<" cha "<<cha<<" p_hi[1] "<<p_hi[1]<<" p_hi[2] "<<p_hi[2]<<endl;
             for (AMSTrRecHit * pcha=AMSTrRecHit::gethead(inhi); pcha!=NULL; pcha=pcha->next()){
                if(pcha->checkstatus(AMSDBc::FalseX) || pcha->checkstatus(AMSDBc::FalseTOFX))continue;
	       // cout <<"xambig "<<xambig<<endl;
	         depos = pcha->getsum(); 
                 pc_hi = pcha->getHit();
                 if (pc_hi[1]==p_hi[1] && pc_hi[2]==p_hi[2]){ // ambigui solo in x e depos
                   if (depos == cha){ // same depos let's count x ambiguity 
                     xambig++;  
		     //    cout <<" xambig "<<xambig<<" cha "<<cha<<" pc_hi[0] "<<pc_hi[0]<<" pc_hi[1] "<<pc_hi[1]<<" pc_hi[2] "<<pc_hi[2]<<endl;
		   }
		   //
		 }
	     }
	     //	     cout <<" end xambig "<<xambig<<" cha "<<cha<<" p_hi[1] "<<p_hi[1]<<" p_hi[2] "<<p_hi[2]<<endl;
	     //.......
 
  	   if (CEN==10000){
            pre->setstatus(AMSDBc::GAMMARIGHT);
            pre->setstatus(AMSDBc::GAMMALEFT);
#ifdef __AMSDEBUG__
	     cout<< "Single Hit****RIGHT cha & p_hi[1 .. 3] = "<< cha<< " "<<p_hi[0]<<" "<<p_hi[1]<<" "<<p_hi[2]<<endl; 
	     cout<< "Single Hit****LEFT cha &  p_hi[1 .. 3] = "<< cha<< " "<<p_hi[0]<<" "<<p_hi[1]<<" "<<p_hi[2]<<endl;
#endif
	     
             if (xambig <= 2){ // too low ambiguty
            pre->clearstatus(AMSDBc::GAMMARIGHT);
	    pre->clearstatus(AMSDBc::GAMMALEFT);
	     }
	     // eliminated hit with too low ambiguity
	     //
	     if (cmax1 > 2 && cmax2 > 2){
                if (cha != deposmax1 && cha != deposmax2){
                 pre->clearstatus(AMSDBc::GAMMARIGHT);
	         pre->clearstatus(AMSDBc::GAMMALEFT);
	       }
	     }

	   }
	   //the CEN has been found in Single Hit, then:
          if (CEN != 10000 && p_hi[1]>CEN){
	    pre->clearstatus(AMSDBc::GAMMARIGHT);
	    pre->clearstatus(AMSDBc::GAMMALEFT);
            pre->setstatus(AMSDBc::GAMMARIGHT);
#ifdef __AMSDEBUG__
	    //	     cout<< "single in RIGHT cha & p_hi[1 .. 3] = "<< cha<< " "<<p_hi[0]<<" "<<p_hi[1]<<" "<<p_hi[2]<<endl; 
#endif
	    //             cout<< "ambiguity =" <<xambig<<endl;
             if (xambig <= 2){ // too low ambiguty
              pre->clearstatus(AMSDBc::GAMMARIGHT);
	     } 
            if (cmax1 > 2){
                if (cha != deposmax1){
                 pre->clearstatus(AMSDBc::GAMMARIGHT);
	       }
	    }

	  }
          if (CEN != 10000 && p_hi[1]<=CEN){
	    pre->clearstatus(AMSDBc::GAMMARIGHT);
	    pre->clearstatus(AMSDBc::GAMMALEFT);
            pre->setstatus(AMSDBc::GAMMALEFT);

#ifdef __AMSDEBUG__
	    cout<< "single in LEFT  cha & p_hi[1 .. 3] = "<< cha<< " "<<p_hi[0]<<" "<<p_hi[1]<<" "<<p_hi[2]<<endl; 
#endif

             if (xambig <= 2){ // too low ambiguty
              pre->clearstatus(AMSDBc::GAMMALEFT);
	     } 
            if (cmax1 > 2){
                if (cha != deposmax1){
                 pre->clearstatus(AMSDBc::GAMMALEFT);
	       }
	    }

	  }    
         }
       } // single
       // now double hit but we have to redo the calculation onto the charges
       if (HH.size()>1){
        for (int k=0;k<(HH.size());k++){
         if (p_hi[1]==HH[k]){

	     //NOW the charge descrimination
	     // A)
             //let's eliminate hit with no ambiguity
             xambig=0; 
             for (AMSTrRecHit * pcha=AMSTrRecHit::gethead(inhi); pcha!=NULL; pcha=pcha->next()){
                if(pcha->checkstatus(AMSDBc::FalseX) || pcha->checkstatus(AMSDBc::FalseTOFX))continue;
	         depos = pcha->getsum(); 
                 pc_hi = pcha->getHit();
                 if (pc_hi[1]==p_hi[1] && pc_hi[2]==p_hi[2]){ // ambigui solo in x e depos
                   if (depos == cha){ // same depos let's count x ambiguity
                     xambig++;  
		   }
		 }
	     }



	     //.......

          if (p_hi[1]>CEN){
            
            pre->clearstatus(AMSDBc::GAMMARIGHT);
	    pre->clearstatus(AMSDBc::GAMMALEFT); 
            pre->setstatus(AMSDBc::GAMMARIGHT);

             if (xambig <= 2){ // too low ambiguty
              pre->clearstatus(AMSDBc::GAMMARIGHT);
	     } 

#ifdef __AMSDEBUG__
            if (pre->checkstatus(AMSDBc::GAMMARIGHT)){ 
	    cout<< "*********RIGHT cha & p_hi[1 .. 3] = "<<cha <<" " <<p_hi[0]<<" "<<p_hi[1]<<" "<<p_hi[2]<<endl;
	    }
             if (pre->checkstatus(AMSDBc::GAMMALEFT)){ 
             cout<< "THERE IS A LEFT IN RIGHT cha & p_hi[1 .. 3] = "<<cha <<" " <<p_hi[0]<<" "<<p_hi[1]<<" "<<p_hi[2]<<endl;
	     }
#endif 
	  } 
          if (p_hi[1]<=CEN){
            pre->clearstatus(AMSDBc::GAMMARIGHT);
	    pre->clearstatus(AMSDBc::GAMMALEFT);
            pre->setstatus(AMSDBc::GAMMALEFT);

	    if (xambig <= 2){ // too low ambiguty
              pre->clearstatus(AMSDBc::GAMMALEFT);
	     } 
	    //        
#ifdef __AMSDEBUG__

             if (pre->checkstatus(AMSDBc::GAMMALEFT)){ 
	     cout<< "*********LEFT cha & p_hi[1 .. 3] = "<<cha <<" " <<p_hi[0]<<" "<<p_hi[1]<<" "<<p_hi[2]<<endl;
	     }
             if (pre->checkstatus(AMSDBc::GAMMARIGHT)){
             cout<< "THERE IS A RIGHT IN LEFT cha & p_hi[1 .. 3] = "<<cha <<" " <<p_hi[0]<<" "<<p_hi[1]<<" "<<p_hi[2]<<endl;
	     }
#endif 
          }
         } 
        }
       }
    }
} 



integer AMSTrTrackGamma::build(integer refit){

  int ideaflag=0;
  double tm34=0;
  number ecc=0;
  int ii[5]={0,0,0,0,0};
  int neca[3]={0,0,0};
  int fbotf=0;
  int ftopf=0;
  double x_starf=10000;  
  double z_starf=10000;
  double SLOPEf=10000;  
  double INTERf=10000;
  int out=0;
  int nl=0;

  int NCO[trconst::maxlay];
  vector<double> H[trconst::maxlay];
  int FLPAT[trconst::maxlay];
  AMSPoint p_hi;
  AMSTrRecHit * p;
  int Nuser=0;
  int Nlayu[trconst::maxlay];
  number RES[trconst::maxlay];
  number SRES[trconst::maxlay];
  number CE[trconst::maxlay];
  number SCE[trconst::maxlay];
  
  for(int i=0;i<TKDBc::nlay();i++){
    Nlayu[i]=0;
    RES[i]=0;
    SRES[i]=0;
    CE[i]=0;
    SCE[i]=0;
    NCO[i]=0;
  }
  //---------------------------------------
  
 integer NGammaFound=-1;

 if(refit){
    // Cleanup all track containers
    int i;
    for(i=0;;i++){
      AMSContainer *pctr=AMSEvent::gethead()->getC("AMSTrTrackGamma",i);
      if(pctr)pctr->eraseC();
      else break ;
    }
  } 
 //---------------------------------------
  for(int i=0;i<TKDBc::nlay();i++){
   Nuser+=(AMSEvent::gethead()->getC("AMSTrRecHit",i))->getnelem();
   Nlayu[i]+=(AMSEvent::gethead()->getC("AMSTrRecHit",i))->getnelem();
  }
  uinteger evn=AMSEvent::gethead()->getEvent();
  uinteger Run=AMSEvent::gethead()->getrun();

  //#ifdef __AMSDEBUG__
  cout << "  #####   #####  #####  Run "<<Run<< " "<< "Event " <<evn <<endl;
  AMSmceventg::PrintSeeds(cout);
  //  cout << " ##### Nuser which should correspond to ntrrh =   " << Nuser <<endl;
  //#endif
  for(int i=0;i<TKDBc::nlay();i++){
#ifdef __AMSDEBUG__
   cout << " #####     layer "<<i<< "    hits "<<Nlayu[i]<<endl;
#endif
   AMSTrRecHit::_markDouble(H[i],i);
   FLPAT[i]=H[i].size(); 
   if (i==0){
    if (FLPAT[i]>2){
      H[i]=_TK1YRemoveTRD2TOF(H[i],FLPAT[i]);
      FLPAT[i]=H[i].size();
    }
   }
  }

  if (FLPAT[7] == 2 || FLPAT[0] == 2){
 _LookOneEight(FLPAT,H);
#ifdef __AMSDEBUG__
  for(int i=0;i<TKDBc::nlay();i++){
   cout << "AFTER Look FLPAT["<<i<<"] ="<<FLPAT[i]<<endl;
   for(int j=0;j<FLPAT[i];j++){
       cout << "After LOOK *** H["<<i<<"]["<<j<<"]=  "<< H[i][j] <<endl;     
   }
  }
#endif
  }
  // let's define multiplicity counters to veto or break the reconstruction
 int esc_0=0;
 int esc_1=0;
 int esc_2=0;
 int esc_22=0;
 int esc_4=0;
 int esc_10=0;
 int esc_20=0;
 int es_f1=0;
 int pl23_0=0;
 int pl45_0=0;
 int pl67_0=0;
 int esc_all=0;

  for(int i=0;i<TKDBc::nlay();i++){
#ifdef __AMSDEBUG__
      for(int j=0;j<FLPAT[i];j++){
      cout << "before********* H["<<i<<"]["<<j<<"]=  "<< H[i][j] <<endl;       
      }
#endif
    if  (FLPAT[i]!=0) esc_all++; 
    if  (FLPAT[i]==2) esc_1++;
    if  (FLPAT[i]>2)  esc_2++;
    if  (FLPAT[i]>3)  esc_4++;
    if  (FLPAT[i]==0) esc_0++;
    if  (FLPAT[i]>10) esc_10++;
    if  (FLPAT[i]>20) esc_20++;
    if  (FLPAT[i]==1) es_f1++;
  }
  if  (FLPAT[1]!=0 || FLPAT[2]!=0) pl23_0=FLPAT[1]+FLPAT[2];
  if  (FLPAT[3]!=0 || FLPAT[4]!=0) pl45_0=FLPAT[3]+FLPAT[4];
  if  (FLPAT[5]!=0 || FLPAT[6]!=0) pl67_0=FLPAT[5]+FLPAT[6];
  
  if (((pl23_0 == 0 && pl45_0 == 0) || (pl67_0 == 0 && pl45_0 == 0) || (pl67_0 == 0 && pl23_0 == 0)) && (FLPAT[0] > 2 || FLPAT[7] > 2))  return 0;
    
  if (((pl23_0 == 0 && pl45_0 == 0 && FLPAT[5]>2 && FLPAT[6]>2) || (pl67_0 == 0 && pl45_0 == 0 && FLPAT[1]>2 && FLPAT[2]>2) || (pl67_0 == 0 && pl23_0 == 0 && FLPAT[3]>2 && FLPAT[4]>2)) )  return 0;
  
  if (pl23_0 == 0 || pl45_0 == 0 || pl67_0 ==0 && ( (FLPAT[0] == 0 && FLPAT[7] <=1) || (FLPAT[0] <= 1 && FLPAT[7] == 0) ))  return 0; 


  
  if (esc_all <= 3) return 0;
  if (esc_all ==4 && (FLPAT[0] ==0 || FLPAT[7] ==0)) return 0;
  if (esc_all <6 && (FLPAT[0] ==0 && FLPAT[7] ==0)) return 0;
  if (esc_1 == 1 && esc_2 >= 4) return 0;
  if (esc_2 >= 5) return 0;
  if (esc_4 >= 4) return 0;
  if (esc_4 >= 2 && esc_2 >= 3) return 0;
  if (es_f1 >= 3 && esc_2 >= 3) return 0;
  if (esc_0 >= 2 && esc_2 >= 3) return 0;
  if (esc_0 >= 2 && esc_2 >= 2 && es_f1 >= 2) return 0;
  if (esc_1 <= 2 && esc_2 >= 4) return 0;
  if (esc_10 >= 3)  return 0;
  if (esc_20 != 0)  return 0;

_LSQP2(FLPAT,H,esc_1,esc_2);


 for(int i=0;i<TKDBc::nlay();i++){
   FLPAT[i]=H[i].size();
//   cout << "FLPAT["<<i<<"] ="<<FLPAT[i]<<endl;
   if (FLPAT[i]>2) {
//     cout << "_LSQP2 didn't work ABANDON" <<endl;
     return 0;
   }
   //
#ifdef __AMSDEBUG__
   for(int j=0;j<FLPAT[i];j++){
     cout << "After *** H["<<i<<"]["<<j<<"]=  "<< H[i][j] <<endl;       
   }
#endif
   if (FLPAT[i]>1){
     _Combi(NCO[i],FLPAT[i],2);
     _Averes(RES[i],SRES[i],H[i],FLPAT[i],NCO[i]);
     _Cente(CE[i],SCE[i],H[i],FLPAT[i],NCO[i],RES[i],SRES[i]);
     if  (FLPAT[i]==2)  esc_22++;
   }
   //
   if (((NCO[1]+NCO[2]) > 10 || (NCO[3]+NCO[4]) > 10 || (NCO[5]+NCO[6]) > 10) && esc_0 >= 3)  return 0;
 }

_SingleHit(FLPAT,CE,esc_22);

 for(int i=0;i<TKDBc::nlay();i++){
#ifdef __AMSDEBUG__
 cout<< "******1 in piu` CE["<<i<<"]= "<< CE[i]<<endl;
#endif
_LeftRight(H[i],i,CE[i]);
 }
XZLine_TOF RoadXZ(ii,neca,tm34,ecc);
#ifdef __AMSDEBUG__
 cout << "ii[0] "<<ii[0]<<" ii[1] "<<ii[1]<<" ii[2] "<<ii[2]<< endl;
 cout << "ii[3] "<<ii[3]<<" ii[4] "<<ii[4]<<endl;
 cout << "tm34  "<<tm34<<endl;
 cout << "neca[0] "<<neca[0]<<" neca[1] "<<neca[1]<<" neca[2] "<<neca[2]<< endl;
#endif
 if (ii[0] == 0 && ii[1] == 0) return 0;


 double slo,inte,chit;

 if (ii[0] == 1 && ii[1] == 1 && ii[2] == 1 && ii[3] == 1) ideaflag=1;
 // cout << "ideaflag = "<<ideaflag <<endl;

 if (ideaflag==0){
RoadXZ.TopSplash(tm34);
RoadXZ.getTofMul(1,ii,tm34);
 }
RoadXZ.Check_TRD_TK1(FLPAT[0],H[0],ii);
RoadXZ.getTofMul(2,ii,tm34);
//
RoadXZ.makeEC_out(ecc,out);
RoadXZ.Lines_Top_Bottom(out);

#ifdef __AMSDEBUG__
 cout << "AAA ii[0] "<<ii[0]<<" ii[1] "<<ii[1]<<" ii[2] "<<ii[2]<< endl;
 cout << "ii[3] "<<ii[3]<<" ii[4] "<<ii[4]<<endl;
#endif
RoadXZ.Lines_Top_Bottom(out);
 if (out==0) return 0;
 if (out==1){
RoadXZ.getParRoadXZ(fbotf,ftopf,x_starf,z_starf,SLOPEf,INTERf);
RoadXZ.LastCheckTRDX(slo,inte,chit);
 if (chit <= 0.1 && chit > 0.001 ) {
   ftopf=1;
   fbotf=0;
   SLOPEf=slo;
   INTERf=inte;
 }
 }
#ifdef __AMSDEBUG__
 cout << "AGAIN FINE: fbotf = "<<fbotf<<" ftopf = "<<ftopf<<endl;
 cout << "x_starf z_starf= "<<x_starf<<" "<<z_starf<<endl;
 cout << "SLOPEf INTERf = "<<SLOPEf<<" "<<INTERf<<endl;
#endif


 //------------------------------------------------------- FINALLY
 double slr=10000;
 double qlr=10000;
 int init_R=0;
 int init_L=0;
 double firR;
 double lasR;
 double firL,lasL;
 int fir_planeR,fir_planeL;
 int las_planeR,las_planeL;
 
 for (int i=0;i<TKDBc::nlay();i++){
   for (AMSTrRecHit * p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     p->clearstatus(AMSDBc::TOFFORGAMMA); 
     p_hi = p->getHit(); 
     //
     if (p->checkstatus(AMSDBc::GAMMARIGHT)){
       if(init_R==0){
	  init_R=1;
          firR=p_hi[2];
          fir_planeR=i;
	}
          lasR=p_hi[2];
          las_planeR=i;
      }
      if (p->checkstatus(AMSDBc::GAMMALEFT)){
	 if(init_L==0){
	  init_L=1;
          firL=p_hi[2];
          fir_planeL=i;
	}
          lasL=p_hi[2];
          las_planeL=i;
      }
      //
     }
    }
    if (init_R==0){
      firR=0;
      lasR=0;
    }
    if (init_L==0){
      firL=0;
      lasL=0;
    }
#ifdef __AMSDEBUG__
    if (init_R==0 || init_L==0){
      cout<<"NO SUFFICIENT HITS FOR DOUBLE TRACK RECOGNITION "<<endl;
    }
 cout<<"firR lasR............................"<<firR<<" "<<lasR<<endl;
 cout<<"firL lasL............................"<<firL<<" "<<lasL<<endl;
#endif

   int xsR=0;
   int xsL=0;
   int xsRL=0;
 for(int i=0;i<TKDBc::nlay();i++){
   xsRL+=FLPAT[i];
 }
#ifdef __AMSDEBUG__
   cout <<" xsR xsL          =   "<<xsR<<" "<<xsL<<endl; 
   cout <<"                 xsRL          =   "<<xsRL<<endl; 
#endif

   if(xsRL > 6) {
     int refitting=0;
     if (init_R!=0 && init_L!=0){

RecoLeftRight(refitting,FLPAT,SLOPEf,INTERf,x_starf,z_starf,fbotf,ftopf,
                     firR,lasR,firL,lasL,fir_planeR,fir_planeL,
                     las_planeR,las_planeL,slr,qlr);

 int INDEXPL[trconst::maxlay]; 
 int esc_co1=0;
 for(int i=0;i<TKDBc::nlay();i++){
   if  (FLPAT[i]==2) esc_co1++;
   INDEXPL[i]=0;
 }
 
 for (int i=0;i<TKDBc::nlay();i++){
   for (AMSTrRecHit * p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     if (p->checkstatus(AMSDBc::GAMMARIGHT)){
       INDEXPL[i]++;
     }
     if (p->checkstatus(AMSDBc::GAMMALEFT)){
       INDEXPL[i]++;
     } 
   }
 }  
 
 int recowrong=0;
 for(int i=0;i<TKDBc::nlay();i++){
#ifdef __AMSDEBUG__
   cout<<" Let's check that I have maximum 2 hits per plane: INDEXPL["<<i<<"]= "<<INDEXPL[i]<<endl;
#endif
   if (INDEXPL[i] < FLPAT[i]) recowrong++;
   if (FLPAT[i] == 2 && INDEXPL[i] == 0) refitting++;
  }
#ifdef __AMSDEBUG__
 cout << "             BE CAREFUL : recowrong = "<<recowrong<<endl;
#endif
 for(int i=0;i<TKDBc::nlay();i++){
   INDEXPL[i]=0;
 }

//
 if (refitting != 0){
   for(int i=0;i<TKDBc::nlay();i++){
_LeftRight(H[i],i,CE[i]);
   } 
   
RecoLeftRight(refitting,FLPAT,SLOPEf,INTERf,x_starf,z_starf,fbotf,ftopf,
                      firR,lasR,firL,lasL,fir_planeR,fir_planeL,
                     las_planeR,las_planeL,slr,qlr);
 }
#ifdef __AMSDEBUG__
 for(int i=0;i<TKDBc::nlay();i++){
   cout<< "*$$        INDEXPL["<<i<<"]= "<<INDEXPL[i]<<endl; 
 }
#endif
 AMSTrRecHit * parrayR[trconst::maxlay];
 int nright=-1; 
  for (int i=0;i<TKDBc::nlay();i++){
   for (AMSTrRecHit * p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     if (p->checkstatus(AMSDBc::GAMMARIGHT)){
      AMSPoint p_hi = p->getHit(); 
#ifdef __AMSDEBUG__
cout<< "*$$$$$$$ RIGHT p_hi[1 .. 3] = "<<p_hi[0]<<" "<<p_hi[1]<<" "<<p_hi[2]<<endl; 
#endif
          nright++;
          INDEXPL[i]++;
          parrayR[nright]=p;
   }
   }
  }

  AMSTrRecHit * parrayL[trconst::maxlay]; 
  int nleft=-1;
  for (int i=0;i<TKDBc::nlay();i++){
   for (AMSTrRecHit * p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     if (p->checkstatus(AMSDBc::GAMMALEFT)){
      AMSPoint p_hi = p->getHit(); 
#ifdef __AMSDEBUG__
cout<< "*$$$$$$$ LEFT p_hi[1 .. 3] = "<<p_hi[0]<<" "<<p_hi[1]<<" "<<p_hi[2]<<endl; 
#endif
         nleft++;
         INDEXPL[i]++;
         parrayL[nleft]=p;
     }
   }
  }
#ifdef __AMSDEBUG__
 for(int i=0;i<TKDBc::nlay();i++){
   cout<<" AGAIN Let's check that I have maximum 2 hits per plane: INDEXPL["<<i<<"]= "<<INDEXPL[i]<<endl;
 }
#endif
  nleft=nleft+1;
  nright=nright+1;
#ifdef __AMSDEBUG__
  cout <<"1   nleft nright"<<nleft<<" "<< nright<<endl;
#endif
  //
  if (nleft < 3 || nright < 3) return 0;
  if (nleft ==3 && nright ==3) return 0;
  //
   

    AMSTrTrackGamma * pntLR=   new AMSTrTrackGamma(nleft,nright,parrayL,parrayR,recowrong);

    int counting=0;
    int plusminus=0;
    if (nleft >=3 && nright >= 3){
    pntLR->Fit(5,2);
#ifdef __AMSDEBUG__
    cout << "_GRidgidityMSR          " <<pntLR->_GRidgidityMSR<< endl; 
    cout << "_GRidgidityMSL          " <<pntLR->_GRidgidityMSL<< endl; 
    cout << "Now we are after  Fit(5,2)"<< endl;
 #endif
    counting=0;
    plusminus=0;
    pntLR->PAIR2GAMMA(counting,plusminus);
      
      if (pntLR->_VE1[2] <= 52 || pntLR->_VE2[2] <= 52  ){
	refitting+=100;
      }
    }


    // before proceeding check refitting:
    if (refitting >= 100){
      delete pntLR; 
      for(int i=0;i<TKDBc::nlay();i++){
_LeftRight(H[i],i,CE[i]);
      } 


RecoLeftRight(refitting,FLPAT,SLOPEf,INTERf,x_starf,z_starf,fbotf,ftopf,
                     firR,lasR,firL,lasL,fir_planeR,fir_planeL,
                     las_planeR,las_planeL,slr,qlr);

 if (refitting >=10000){
   for(int i=0;i<TKDBc::nlay();i++){
     _LeftRight(H[i],i,CE[i]);
   } 
   SLOPEf=slr;
   INTERf=qlr;
   //cout << "SLOPEf INTERf = "<<SLOPEf<<" "<<INTERf<<endl;
   //cout <<" 5 refitting  "<<refitting<<endl;
 refitting=0;
RecoLeftRight(refitting,FLPAT,SLOPEf,INTERf,x_starf,z_starf,fbotf,ftopf,
                     firR,lasR,firL,lasL,fir_planeR,fir_planeL,
                     las_planeR,las_planeL,slr,qlr);
 }
 for(int i=0;i<TKDBc::nlay();i++){
   INDEXPL[i]=0;
 }
 
 AMSTrRecHit * parrayR[trconst::maxlay];
 int nright=-1; 
 for (int i=0;i<TKDBc::nlay();i++){
   for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     if (p->checkstatus(AMSDBc::GAMMARIGHT)){
       AMSPoint p_hi = p->getHit(); 
       nright++;
       INDEXPL[i]++;
       parrayR[nright]=p;
     }
   }
 }
 
 AMSTrRecHit * parrayL[trconst::maxlay]; 
 int nleft=-1;
 for (int i=0;i<TKDBc::nlay();i++){
   for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     if (p->checkstatus(AMSDBc::GAMMALEFT)){
       AMSPoint p_hi = p->getHit(); 
       nleft++;
       INDEXPL[i]++;
       parrayL[nleft]=p;
     }
   }
 }
 nleft=nleft+1;
 nright=nright+1;

 AMSTrTrackGamma * pntLR=   new AMSTrTrackGamma(nleft,nright,parrayL,parrayR,recowrong);
 int counting=0;
 int plusminus=0;
 if (nleft >=3 && nright >= 3){
   pntLR->Fit(5,2);
   counting=0;
   plusminus=0;
   pntLR->PAIR2GAMMA(counting,plusminus);
 }
 }//refitting >=100
    //


 double res_d[trconst::maxlay];
 for(int i=0;i<TKDBc::nlay();i++){
   res_d[i]=0;
  } 

 int flag=0;
 pntLR->LR_RES_STUDY3(INDEXPL,res_d,flag);

 if (flag != 0){
   delete pntLR; 
   nright=-1;  
   nleft=-1;
   for (int i=0;i<TKDBc::nlay();i++){
     for (AMSTrRecHit * p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
       if (p->checkstatus(AMSDBc::GAMMARIGHT)){
	 nright++;
	 parrayR[nright]=p;
	 AMSPoint p_hi = p->getHit(); 
       }
       if (p->checkstatus(AMSDBc::GAMMALEFT)){
	 nleft++;
	 parrayL[nleft]=p;
	 AMSPoint p_hi = p->getHit(); 
       }
     }
   }
   nleft=nleft+1;
   nright=nright+1;

   AMSTrTrackGamma *pntLR=   new AMSTrTrackGamma(nleft,nright,parrayL,parrayR,recowrong);
   if (nleft >=3 && nright >= 3){
     pntLR->Fit(5,2);
     counting=0;
     plusminus=0;
     pntLR->PAIR2GAMMA(counting,plusminus);
   }


AMSDir DIRi(0,0,1.);
number tetL,fifL,sileL;
number tetR,fifR,sileR;
AMSPoint P_0L2;
AMSPoint P_0R2;
double RES_REF;
double z_tkl[trconst::maxlay];
 for(int i=0;i<TKDBc::nlay();i++){
   z_tkl[i]=TKDBc::zposl(i);
 }

   if (flag < 500){
     //LoopMissingPlane
     for(int j=0;j<TKDBc::nlay();j++){
       int go=0;
       if (res_d[j] == 0){
	 //
	 for(int i=0;i<nleft;i++){
	   if (pntLR->_HLEFT[i][2] == z_tkl[j]){
	     go++;
	   }
	 }
	 for(int i=0;i<nright;i++){
	   if (pntLR->_HRIGH[i][2] == z_tkl[j]){
	     go++;
	   }
	 }
	 //
	 if (go == 0){
	   for(int jh=0;jh<j;jh++){
	     RES_REF=res_d[jh]; // we take the residual which comes immediately before
	   }
AMSPoint ZEL(pntLR->_GP0MSL[0],pntLR->_GP0MSL[1],z_tkl[j]);
AMSPoint ZER(pntLR->_GP0MSR[0],pntLR->_GP0MSR[1],z_tkl[j]);
#ifdef __AMSDEBUG__
cout<< "    INTERPOLING PLANE  "<<j+1<<endl;
cout<< "AMSPoint ZEL =  "<<ZEL[0]<<" "<<ZEL[1]<<" "<<ZEL[2]<<endl;
cout<< "AMSPoint ZER =  "<<ZER[0]<<" "<<ZER[1]<<" "<<ZER[2]<<endl;
#endif
pntLR->interpolate(1,ZEL,DIRi,P_0L2,tetL,fifL,sileL);
pntLR->interpolate(0,ZER,DIRi,P_0R2,tetR,fifR,sileR);
pntLR->HITRESEARCH(j,RES_REF,P_0L2,P_0R2);
delete pntLR; 
  nright=-1;  
  nleft=-1;
  for (int i=0;i<TKDBc::nlay();i++){
    for (AMSTrRecHit * p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
      if (p->checkstatus(AMSDBc::GAMMARIGHT)){
	nright++;
	parrayR[nright]=p;
      }
      if (p->checkstatus(AMSDBc::GAMMALEFT)){
	nleft++;
	parrayL[nleft]=p;
      }
    }
  }
  nleft=nleft+1;
  nright=nright+1;

pntLR= new AMSTrTrackGamma(nleft,nright,parrayL,parrayR,recowrong);
if (nleft >=3 && nright >= 3){
pntLR->Fit(5,2);
counting=0;
plusminus=0;
pntLR->PAIR2GAMMA(counting,plusminus);
}
	 } //go  
       }
     }
   }// flag < 500
   

if (flag >= 500){
   // Let's defined the expected residual "RES_REF " in the empty plane  "pla".
   int pla=flag-500;
   RES_REF=0;
   if (pla != 0){
     RES_REF=0.03;
      for(int jh=0;jh<pla;jh++){
        RES_REF=res_d[jh]; // we take the residual which comes immediately before
      }     
   }


AMSPoint ZEL(pntLR->_GP0MSL[0],pntLR->_GP0MSL[1],z_tkl[pla]);
AMSPoint ZER(pntLR->_GP0MSR[0],pntLR->_GP0MSR[1],z_tkl[pla]);
pntLR->interpolate(1,ZEL,DIRi,P_0L2,tetL,fifL,sileL);
pntLR->interpolate(0,ZER,DIRi,P_0R2,tetR,fifR,sileR);
pntLR->HITRESEARCH(pla,RES_REF,P_0L2,P_0R2);
delete pntLR; 
  nright=-1;  
  nleft=-1;
  for (int i=0;i<TKDBc::nlay();i++){
   for (AMSTrRecHit * p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     if (p->checkstatus(AMSDBc::GAMMARIGHT)){
       nright++;
       parrayR[nright]=p;
   }
     if (p->checkstatus(AMSDBc::GAMMALEFT)){
       nleft++;
       parrayL[nleft]=p;
     }
   }
  }
  nleft=nleft+1;
  nright=nright+1;
pntLR= new AMSTrTrackGamma(nleft,nright,parrayL,parrayR,recowrong);
if (nleft >=3 && nright >= 3){
pntLR->Fit(5,2);
counting=0;
plusminus=0;
pntLR->PAIR2GAMMA(counting,plusminus);
}
 } // flag >= 500

 }//  flag != 0



  if (nleft >= 3){
   NGammaFound++;
  }
  if (nright >= 3){
   NGammaFound++;
  }
#ifdef __AMSDEBUG__
cout << " FINALLY 8-------------- PGAMM "<<pntLR->_PGAMM<<endl;
#endif
if (nleft >=3 && nright >= 3){
pntLR->Fit(0,2);
pntLR->Fit(3,2);
pntLR->Fit(4,2);
}
else return 0;


    AMSEvent::gethead()->addnext(AMSID("AMSTrTrackGamma",0),pntLR);
    int done=0;
    pntLR->addtracks(done);
//    pntLR->_ConstructGamma();

// VC gamma

    AMSTrTrackGamma  *p= new AMSTrTrackGamma(*pntLR);
     p->_ConstructGamma();
    AMSEvent::gethead()->addnext(AMSID("AMSTrTrackGamma",0),p);

//    cout<<" done ------- = "<< done<< endl;


  }// (init_R!=0 && init_L!=0)

 } // xsRL
 // :=====================-

return NGammaFound;
}



void AMSTrTrackGamma::getFFParam( number& Chi2, number& Rigi, number& Erigi, number& Thetaff,  number& Phiff,  AMSPoint& X0)const
{Chi2=_Chi2FastFitL;Rigi=_RidgidityL;Erigi=_ErrRidgidityL;Thetaff=_ThetaL;Phiff=_PhiL;X0=_P0L;}



void AMSTrTrackGamma::_copyEl(){
#ifdef __WRITEROOTCLONES__
  TrGammaRoot02 *ptr = (TrGammaRoot02*)_ptr;
  if (ptr) {
    // //    if (_ptrack)  ptr->fTrack =_ptrack ->GetClonePointer();
    //    for (int i=0; i<_NHits; i++) {
    //
    //  if (_Pthit[i]) ptr->fTrRecHit->Add(_Pthit[i]->GetClonePointer());
    //    }
} else {
  cout<<"AMSTrTrack::_copyEl -I-  AMSTrTrackGamma::TrGammaRoot02 *ptr is NULL "<<endl;
}

#endif
}

void AMSTrTrackGamma::_writeEl(){
if(AMSTrTrackGamma::Out(1)){
   int i;
#ifdef __WRITEROOTCLONES__
//    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
 TrGamma* TrTN = AMSJob::gethead()->getntuple()->Get_tpai02();
  if (TrTN->Ngam>=MAXPAIR02) return; //const int MAXPAIR02    =   2; see ntuple.h
// Fill the ntuple
 


TrTN->Pgam[TrTN->Ngam]=_PGAMM;
TrTN->ErrPgam[TrTN->Ngam]=_ErrPGAMM>FLT_MAX?FLT_MAX:_ErrPGAMM;
TrTN->Massgam[TrTN->Ngam]=_MGAM;
TrTN->Thetagam[TrTN->Ngam]=_PhTheta;
TrTN->Phigam[TrTN->Ngam]=_PhPhi;

for(int i=0;i<3;i++)TrTN->Vert[TrTN->Ngam][i]=_Vertex[i];
TrTN->Distance[TrTN->Ngam]=_TrackDistance;
TrTN->Charge[TrTN->Ngam]=_Charge;

TrTN->GammaStatus[TrTN->Ngam]=_status;
//

TrTN->PtrLeft[TrTN->Ngam]=_pntTrL->getpos();
TrTN->PtrRight[TrTN->Ngam]=_pntTrR->getpos();


TrTN->Jthetal[TrTN->Ngam]=(geant)_GThetaMSL;
TrTN->Jphil[TrTN->Ngam]=(geant)_GPhiMSL;
TrTN->Jthetar[TrTN->Ngam]=(geant)_GThetaMSR;
TrTN->Jphir[TrTN->Ngam]=(geant)_GPhiMSR;
for(i=0;i<3;i++)TrTN->Jp0l[TrTN->Ngam][i]=(geant)_GP0MSL[i];
for(i=0;i<3;i++)TrTN->Jp0r[TrTN->Ngam][i]=(geant)_GP0MSR[i];

TrTN->Ngam++;

}
}
               
AMSTrTrackGamma::AMSTrTrackGamma(integer nhitL, integer nhitR, AMSTrRecHit * phLeft[], AMSTrRecHit * phRight[], int state):AMSlink(state),_PGAMM(0),_ErrPGAMM(0),_MGAM(0),_VE1(0,0,0),_VE2(0,0,0),_PhTheta(0),_PhPhi(0),_Gacosd(0),_Vertex(0,0,0),_TrackDistance(0),_Charge(0){
  int i;
//  _GammaStatus=state;
  //RIGHT
  _NhRight=nhitR;
  for( i=0;i<_NhRight;i++)_PRight[i]=phRight[i];
  for(i=_NhRight;i<trconst::maxlay;i++)_PRight[i]=0;
  
  //LEFT
  _NhLeft=nhitL;
  for( i=0;i<_NhLeft;i++)_PLeft[i]=phLeft[i];
  for(i=_NhLeft;i<trconst::maxlay;i++)_PLeft[i]=0;

}


integer AMSTrTrackGamma::Out(integer status){
static integer init=0;
static integer WriteAll=0;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
  for(int n=0;n<ntrig;n++){
    if(strcmp("AMSTrTrackGamma",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
    }
  }
}
return (WriteAll || status);
}




void AMSTrTrackGamma::LR_RES_STUDY3(integer INDEXPL[], double res_d[], int& flag){

  double z_tkl[trconst::maxlay];
  for(int i=0;i<TKDBc::nlay();i++){
    z_tkl[i]=TKDBc::zposl(i);
  }
  AMSTrRecHit * pre;
  AMSPoint hitr;

  //**************************************************************************
  
  double res_LR[trconst::maxlay];
  double res_[trconst::maxlay];
  double z_fi[trconst::maxlay];
  double res_3[trconst::maxlay];
  double z_fi3[trconst::maxlay];
  double z_fi0[trconst::maxlay];
  int ja=-1;
  //INITIALIZING
  
  for(int i=0;i<TKDBc::nlay();i++){
    z_fi[i]=10000;
    res_[i]=10000;
    z_fi3[i]=10000;
    res_3[i]=10000;
    z_fi0[i]=10000;
    res_LR[i]=10000;
  }
  
  // we select those plane where both LEFT and RIGHT hits exist and the DY in between
  // has been calculated, 
  // ja= number of couples
  // z_fi=z_fi0 Z position of the plane
  // res_= DeltaY LR
  
  
  for(int i1=0;i1<_NhLeft;i1++){
    //
#ifdef __AMSDEBUG__
    cout << "_HLEFT[i1][0 ... 2] = "<<_HLEFT[i1][0]<< " "<< _HLEFT[i1][1]<< " "<<  _HLEFT[i1][2]<<endl;
#endif
    for(int i2=0;i2<_NhRight;i2++){
      if (_HLEFT[i1][2] > (_HRIGH[i2][2]-0.5) && 
	  _HLEFT[i1][2] < (_HRIGH[i2][2]+0.5)){
	//
	if (_HLEFT[i1][2] > (z_tkl[0]-0.5) && 
	    _HLEFT[i1][2] < (z_tkl[0]+0.5)){
	  res_LR[0]=fabs(_HLEFT[i1][1]-_HRIGH[i2][1]);
	  if (res_LR[0] < 0.7){
	    ja++;
	    z_fi[ja]=z_tkl[0];
	    res_[ja]=res_LR[0];
	  }
	}
	for(int jp=1;jp<TKDBc::nlay();jp++){
	  if (_HLEFT[i1][2] > (z_tkl[jp]-0.5) && 
	      _HLEFT[i1][2] < (z_tkl[jp]+0.5)){
	    res_LR[jp]=fabs(_HLEFT[i1][1]-_HRIGH[i2][1]);
	    ja++;
	    z_fi[ja]=z_tkl[jp];
	    res_[ja]=res_LR[jp];
	  }
	} 
      }
    }
  }
  // we storage z_fi in z_fi0 to get memory of the starting situation    
  for(int i=0;i<TKDBc::nlay();i++){
    z_fi0[i]=z_fi[i];
  }
  //**************************************************************************
#ifdef __AMSDEBUG__
  cout << "LR_RES_STUDY3------before while (SDW >= 0.32)"<< endl;
  for(int i=0;i<TKDBc::nlay();i++){
    cout <<  "z_fi[i] res_[i] = "<<z_fi[i]<< " "<<  res_[i]<<endl;
  }
#endif
  //**************************************************************************
  // FIRST OF ALL Is the res vs z increasing ??????

  int iu=-1;
  for(int i=0;i<ja;i++){
    if (res_[i+1] >= res_[i]) {
      iu++;
      z_fi3[iu]=z_fi[i];
      res_3[iu]=res_[i];
    }
  }
  for(int i=0;i<ja;i++){
    if (z_fi3[iu] == z_fi[i]){
      if (res_[i+1] >= res_[i]) {
	z_fi3[iu+1]=z_fi[i+1];
	res_3[iu+1]=res_[i+1];
      }
    }
  }
  iu++;
  flag=0; // loop times counter and flag for poly2 accomplishment

#ifdef __AMSDEBUG__
  for(int i=0;i<(iu+1);i++){
    cout <<  "z_fi3[i] res_3["<<i<<"] = "<<z_fi3[i]<< " "<<  res_3[i]<<endl;
  }
#endif
  if (iu != ja){
    flag++;
    ja = iu;
    for(int i=0;i<TKDBc::nlay();i++){
      z_fi[i]=10000;
      res_[i]=10000;
    }
    for(int i=0;i<(ja+1);i++){
	res_[i]=res_3[i];
	z_fi[i]=z_fi3[i];
    }   
  }
//  for(int i=0;i<TKDBc::nlay();i++){
//    cout <<  "AFTER res_3..... z_fi[i] res_["<<i<<"] = "<<z_fi[i]<< " "<<  res_[i]<<endl;
//  }


  // defining parameters of the poly2 fit
  double AA0;
  double AA1;
  double AA2;
  double SDW=10000;
  // INITIALIZATION
  int ju,jd;
  double ex_cur[trconst::maxlay];
  double res_cur[trconst::maxlay];
  double res_2[trconst::maxlay];
  double z_fi2[trconst::maxlay];
  double res_max;
  double cur_max;
  double z_max;


    // INITIALIZING AT EVERY LOOP the local arrays containg output temporary
    ju=0;
    jd=0;
    for(int i=0;i<TKDBc::nlay();i++){
      z_fi2[i]=10000;
      res_2[i]=10000;
      ex_cur[i]=10000;
      res_cur[i]=10000;
    }
    res_max=10000;
    cur_max=10000;
    z_max=0;


  // defining total number of pairs LR = N
  int N=ja+1;
  if (N > 2){
    //poly2 fit   
    dlsqp2me(N,z_fi,res_,AA0,AA1,AA2,SDW);
    for(int i=0;i<N;i++){
      ex_cur[i]=AA0+(AA1*z_fi[i])+(AA2*pow(z_fi[i],2));
      res_cur[i]=fabs(res_[i]-ex_cur[i]); 
    }
    // // // // // // 

  // Entering loop of poly2 chi2 minimization below 0.32
    //  while (SDW >= 0.32 && N >2 ){
  while (SDW >= 0.32){
    if (N <= 2) SDW =0;
    if (N <= 2) break;
//    cout <<  "N = "<< N << endl;
    flag++;
    cur_max=_vdmax(res_cur,N);

    for(int ki=0;ki<N;ki++){
      if (res_cur[ki] == cur_max) z_max=z_fi[ki];
    }

    // Now we fill up the II arrays res_2 excluding the z_max plane which has the farest residual from the fitted one
    // i2 is the new counter for the II arrays 
    int i2=-1;
    for(int ki=0;ki<N;ki++){
      if (res_cur[ki] != cur_max) {
	i2++;
	res_2[i2]=res_[ki];
	z_fi2[i2]=z_fi[ki];
      }
    }
    // Let's reduce N of one unity since we excluded one res_
    N--;
    // Now before starting eventually a new loop or coming out we update z_fi ans res_
    for(int i=0;i<TKDBc::nlay();i++){
      z_fi[i]=z_fi2[i];
      res_[i]=res_2[i];
    }
    // AGAIN 
    for(int i=0;i<TKDBc::nlay();i++){
      z_fi2[i]=10000;
      res_2[i]=10000;
      ex_cur[i]=10000;
      res_cur[i]=10000;
    }
    //poly2 fit   
    dlsqp2me(N,z_fi,res_,AA0,AA1,AA2,SDW);
    for(int i=0;i<N;i++){
      ex_cur[i]=AA0+(AA1*z_fi[i])+(AA2*pow(z_fi[i],2));
      res_cur[i]=fabs(res_[i]-ex_cur[i]); 
    }    
  } // END WHILE LOOP
  } //N>2
  //**************************************************************************
  // finally we are out which means eventually: (SDW.lt.0.32) CONTINUE
  // in case SDW ==0, when too few LR or N<=2 or break with SDW=10000, NB ==1 means
  // no sdw minimization obtained otherwise NB = 0
  int  NB=0;
  if (SDW == 0 || SDW == 10000) NB=1; 
  //
  // REINIZIALIZING res_d 
  for(int ku=0;ku<TKDBc::nlay();ku++){
    res_d[ku]=0;
  }
  // Then we put the best res_ in res_d 
  for(int ku=0;ku<TKDBc::nlay();ku++){
    for(int jh=0;jh<TKDBc::nlay();jh++){
      if (z_fi[jh] > (z_tkl[ku]-0.5) && 
	  z_fi[jh] < (z_tkl[ku]+0.5)){
	res_d[ku]=res_[jh];  
      }
    }
  }
  //*********************** We want to make fit of the centers of each pair LR 
  //                        to make a sort of interpolation afterwards
  double centro[trconst::maxlay];
  double centro2[trconst::maxlay];
  double z_gi[trconst::maxlay];
  double z_gi0[trconst::maxlay];
  double z_gi2[trconst::maxlay];
  double A;
  double B;
  double VAR;
  double BB0,BB1,BB2,SDW1;

  double cen_fi[trconst::maxlay];
  double re_fi[trconst::maxlay];
  double re_cemax;
  double re_ce[trconst::maxlay];

  int lflag[trconst::maxlay];
  int lfleg[trconst::maxlay];

  double XL_CA[trconst::maxlay];
  double XR_CA[trconst::maxlay];
  int NTRsize=0;
  for(int i=0;i<trconst::maxlay;i++){
   NTRsize+=(AMSEvent::gethead()->getC("AMSTrRecHit",i))->getnelem();
   // NTRsize corresponds to ntrrh in the ntuple which is the total number of reconstructed hits
  }
  const int maxcasize=5000;
  double LEY_CA[maxcasize],RIY_CA[maxcasize];
  int CAsize=min(maxcasize,NTRsize);

  double ca_lmax,ca_rmax;
  double ri_fi[trconst::maxlay];
  double le_fi[trconst::maxlay];
  double KLEFT[3][trconst::maxlay];
  double KRIGHT[3][trconst::maxlay];

  for(int i=0;i<CAsize;i++){
   LEY_CA[i]=10000;
   RIY_CA[i]=10000;
  }
  for(int i=0;i<TKDBc::nlay();i++){
   for(int ii=0;ii<3;ii++){
     KLEFT[ii][i]=10000;
     KRIGHT[ii][i]=10000;
   }
  }
   // initializing at -10000
    for(int jp=0;jp<TKDBc::nlay();jp++){
      re_ce[jp]=-10000;
      XL_CA[jp]=10000;
      XR_CA[jp]=10000;
       
    }

  if (NB != 1 && fabs(_PGAMM) <= 10){
    
    for(int jp=0;jp<TKDBc::nlay();jp++){
      centro[jp]=10000;
      centro2[jp]=10000;
      z_gi[jp]=10000;
      z_gi2[jp]=10000;
      z_gi0[jp]=10000; 
    }
    ja=-1;
    for(int i1=0;i1<_NhLeft;i1++){
      for(int i2=0;i2<_NhRight;i2++){
	if (_HLEFT[i1][2] > (_HRIGH[i2][2]-0.5) && 
	    _HLEFT[i1][2] < (_HRIGH[i2][2]+0.5)){
	  for(int jp=0;jp<TKDBc::nlay();jp++){
	    if (_HLEFT[i1][2] > (z_tkl[jp]-0.5) && 
		_HLEFT[i1][2] < (z_tkl[jp]+0.5)){
              
	      //
              if (res_d[jp] != 0){
	      //
	      ja++;
	      centro[ja]=(_HLEFT[i1][1]+_HRIGH[i2][1])/2;
	      z_gi[ja]=z_tkl[jp];
	      z_gi0[ja]=z_tkl[jp];
	      } // res_d
	      //
	    }
	  }
	}
      }
    }
    //  *********** And now with the Y_centers between L and R:.....
    // let's perform both : poly and linear fit of Y_centers vs Z.

    dlsqp2me((ja+1),z_gi,centro,BB0,BB1,BB2,SDW1); // poly2 fit
    dlinearme((ja+1),z_gi,centro,A,B,VAR);         // linear fit

    //*** this is the first fit I perform and it is time to investigate
    //*** for the moment the treatment of flig .ne. 0 is postponed
    int flig=0;
    if (SDW1 <= (2.4-pow((0.82+(0.1*VAR)),-4)) ){
      if (VAR > 0.2) { // forse 0.4 ....
	if (SDW1 > 0.4){
	  if (VAR <= 1.5) flig=1; // SDW1 to be minimized ... below 0.4
	  if (VAR > 1.5) flig=2; // VAR to be minimized ... below 1.5
	} //! sdw1
      } // VAR
    } // FIT 
    //
  //***********************************************
    //NOW a new while loop on the lsq CHI2 = VAR
   
    while(VAR >= 0.2){
//      cout << "                    inside while of var "<<endl;
      for(int i=0;i<(ja+1);i++){
	//linear residuals of Y_centers
	re_ce[i]=fabs(centro[i]-(B*z_gi[i]+A));
      }
      flag++; // we increment the same flag even if it has nothing to do with 
      re_cemax=_vdmax(re_ce,(ja+1));
      int ja2=-1;
      for(int i=0;i<(ja+1);i++){
	if (re_ce[i] != re_cemax){
	  ja2++;
	  z_gi2[ja2]=z_gi[i];
	  centro2[ja2]=centro[i];
	  //in such a way I exclude the largest residual
	}
      }
      //
      for(int jp=0;jp<TKDBc::nlay();jp++){
	//resetting vectors
	centro[jp]=10000;
	z_gi[jp]=10000;
	re_ce[jp]=-10000;
      }
      // new counts
      ja=ja2;
      for(int i=0;i<(ja+1);i++){
	// new centers
	centro[i]=centro2[i];
	z_gi[i]=z_gi2[i];
      }
	     //...and again...
dlsqp2me((ja+1),z_gi,centro,BB0,BB1,BB2,SDW1);
dlinearme((ja+1),z_gi,centro,A,B,VAR); 
 
    }// END OF WHILE

    
    for(int jp=0;jp<TKDBc::nlay();jp++){
      // interpolated centers and residuals for the 8 planes
      cen_fi[jp]=(B*z_tkl[jp]+A);                        // by lin
      re_fi[jp]=AA0+AA1*z_tkl[jp]+AA2*pow(z_tkl[jp],2); // by poly
    }
    for(int jp=0;jp<TKDBc::nlay();jp++){
      lflag[jp]=0;
      lfleg[jp]=0;
    }
    
    // lfleg=0 when there is a single hit then
    // lfleg=1 when has been lost afret sdw
    // lfleg=2 when is still there 
    for(int jp=0;jp<TKDBc::nlay();jp++){
      for(int jj=0;jj<TKDBc::nlay();jj++){
	if (z_fi0[jj] == z_tkl[jp]) { // those which were there before the sdw cycling
	  lfleg[jp]=1;
	}
      }
      for(int jj=0;jj<TKDBc::nlay();jj++){
	if (z_fi[jj] == z_tkl[jp]) { // those which are still there
	  lfleg[jp]=2; // where is two means that the pair of hits survived at the SDW minimization
	}
      }
    }

    // lflag=0 when there is a single hit then
    // lflag=1 when has been lost after VAR on the centers in Y between L and R
    // lflag=2 when is still there 
    for(int jp=0;jp<TKDBc::nlay();jp++){
      for(int jj=0;jj<TKDBc::nlay();jj++){
	if (z_gi0[jj] == z_tkl[jp]) {
	  lflag[jp]=1;
	}
      }
      for(int jj=0;jj<TKDBc::nlay();jj++){
	if (z_gi[jj] == z_tkl[jp]) {
	  lflag[jp]=2; // where is two means that the pair of hits survived at the SDW1 minimization
	}
      }  
    }
    // 	    
    for(int jp=0;jp<TKDBc::nlay();jp++){
      for(int i1=0;i1<_NhLeft;i1++){
	if (_HLEFT[i1][2] > (z_tkl[jp]-0.5) && 
	    _HLEFT[i1][2] < (z_tkl[jp]+0.5)){
	  XL_CA[jp]=_HLEFT[i1][0]; // now we collect X coo of left fits  
	}
      }
    }
    
    for(int jp=0;jp<TKDBc::nlay();jp++){
      for(int i2=0;i2<_NhRight;i2++){
	if (_HRIGH[i2][2] > (z_tkl[jp]-0.5) && 
	    _HRIGH[i2][2] < (z_tkl[jp]+0.5)){
	  XR_CA[jp]=_HRIGH[i2][0]; // now we collect X coo of right hits
	}
      }
    }
    //
    int lr;
    for(int jp=0;jp<TKDBc::nlay();jp++){
      lr=-1;
      // initializing
      for(int i=0;i<CAsize;i++){
	LEY_CA[i]=10000;
	RIY_CA[i]=10000;
      }
      //
      if (lflag[jp] == 1 || lfleg[jp] == 1){ // when the hits were lost
	for(int i=0;i<TKDBc::nlay();i++){
	  for (pre=AMSTrRecHit::gethead(i); pre!=NULL; pre=pre->next()){
	    if(pre->checkstatus(AMSDBc::FalseX) || pre->checkstatus(AMSDBc::FalseTOFX))continue;
	    hitr = pre->getHit();
	    if (hitr[2] > (z_tkl[jp]-0.5) && 
		hitr[2]< (z_tkl[jp]+0.5)){
	      lr++;
	      LEY_CA[lr]=fabs(hitr[1]-(cen_fi[jp]-(re_fi[jp]/2)));
	      RIY_CA[lr]=fabs(hitr[1]-(cen_fi[jp]+(re_fi[jp]/2)));
	    }
	  }
	}
	// For every plane we find the minimal distance to the theoretical Y
	le_fi[jp]=_vdmin(LEY_CA,lr+1);
	ri_fi[jp]=_vdmin(RIY_CA,lr+1);
	ca_lmax=10000;
	ca_rmax=10000;
	for(int i=0;i<TKDBc::nlay();i++){
	  for (pre=AMSTrRecHit::gethead(i); pre!=NULL; pre=pre->next()){
	    if(pre->checkstatus(AMSDBc::FalseX) || pre->checkstatus(AMSDBc::FalseTOFX))continue;
	    hitr = pre->getHit();
	    if (hitr[2] > (z_tkl[jp]-0.5) && hitr[2]< (z_tkl[jp]+0.5)){
	      if ((fabs(hitr[1]-(cen_fi[jp]-(re_fi[jp]/2))) == le_fi[jp]) && 
		  (le_fi[jp] <= sqrt(re_fi[jp]*2))){
                  for(int i1=0;i1<_NhLeft;i1++){
                    if(hitr[2] ==  _HLEFT[i1][2] && hitr[0] !=  _HLEFT[i1][0] &&  hitr[1] !=  _HLEFT[i1][1]){
		KLEFT[2][jp]=hitr[2];
		KLEFT[1][jp]=hitr[1]; 
		if (fabs(hitr[0]-XL_CA[jp]) < ca_lmax){
		  KLEFT[0][jp]=hitr[0];
		  ca_lmax=fabs(hitr[0]-XL_CA[jp]); 
		}
		    }
		  }
	      }
	      if ((fabs(hitr[1]-(cen_fi[jp]+(re_fi[jp]/2))) ==  ri_fi[jp]) && 
		  (ri_fi[jp] <= sqrt(re_fi[jp]*2))) {
                  for(int i2=0;i2<_NhRight;i2++){
                    if(hitr[2] ==  _HRIGH[i2][2] && hitr[0] !=  _HRIGH[i2][0] &&  hitr[1] !=  _HRIGH[i2][1]){
		KRIGHT[2][jp]=hitr[2];
		KRIGHT[1][jp]=hitr[1];
		if (fabs(hitr[0]-XR_CA[jp]) < ca_rmax) {
		  KRIGHT[0][jp]=hitr[0];
		  ca_rmax=fabs(hitr[0]-XR_CA[jp]); 
		}
		    }
		  }
	      }

	    }
	  }
	}
      } // flag o fleg ==1
    } //jp 1 8

  }//
  if (fabs(_PGAMM) > 10 || NB == 1) {
    for(int i=0;i<TKDBc::nlay();i++){
      z_gi[i]=z_fi[i];
      lflag[i]=0;
      lfleg[i]=0;
      if (flag != 0) flag=500+i;
    }
  }

  // RESETTING !!!!!
  for(int jp=0;jp<TKDBc::nlay();jp++){
    for (pre=AMSTrRecHit::gethead(jp); pre!=NULL; pre=pre->next()){
      pre->clearstatus(AMSDBc::GAMMARIGHT);
      pre->clearstatus(AMSDBc::GAMMALEFT);      
    }
  }
  //* SINGLE HIT
  for(int jp=0;jp<TKDBc::nlay();jp++){
    if (INDEXPL[jp] == 1) {
      // 
      for (pre=AMSTrRecHit::gethead(jp); pre!=NULL; pre=pre->next()){
	    if(pre->checkstatus(AMSDBc::FalseX) || pre->checkstatus(AMSDBc::FalseTOFX))continue;
	hitr = pre->getHit();
	for(int i=0;i<_NhLeft;i++){
	  if (hitr[0] == _HLEFT[i][0] && hitr[1] == _HLEFT[i][1] && hitr[2] == _HLEFT[i][2]){
	    pre->setstatus(AMSDBc::GAMMALEFT);
	  }
	}
	for(int i=0;i<_NhRight;i++){
	  if (hitr[0] == _HRIGH[i][0] && hitr[1] == _HRIGH[i][1] && hitr[2] == _HRIGH[i][2]){
	    pre->setstatus(AMSDBc::GAMMARIGHT);
	  }
	}
      }
    } // ==1  
  } //jp
  for(int jp=0;jp<TKDBc::nlay();jp++){
    if (INDEXPL[jp] == 2) {
      for(int jj=0;jj<TKDBc::nlay();jj++){
	if ((z_fi[jj] < 55) && (z_fi[jj] > (z_tkl[jp]-0.5) && z_fi[jj] < (z_tkl[jp]+0.5))){
	  for(int i=0;i<TKDBc::nlay();i++){
	    if (z_fi[jj] == z_gi[i]){
	      for (pre=AMSTrRecHit::gethead(jp); pre!=NULL; pre=pre->next()){
	    if(pre->checkstatus(AMSDBc::FalseX) || pre->checkstatus(AMSDBc::FalseTOFX))continue;
		hitr = pre->getHit();
		for(int ii=0;ii<_NhLeft;ii++){
		  if (hitr[0] == _HLEFT[ii][0] && hitr[1] == _HLEFT[ii][1] && hitr[2] == _HLEFT[ii][2]){
		    pre->setstatus(AMSDBc::GAMMALEFT);
		  }
		}
		for(int ii=0;ii<_NhRight;ii++){
		  if (hitr[0] == _HRIGH[ii][0] && hitr[1] == _HRIGH[ii][1] && hitr[2] == _HRIGH[ii][2]){
		    pre->setstatus(AMSDBc::GAMMARIGHT);
		  }
		}
	      }
	      //
	    }
	  }
	}
      }
    } // ==2
    //
    //finally the modified ones , it means separately the z_fi lost w.r.t. z_fi0
    // ................................               and z_gi lost w.r.t  z_gi0
    if (lflag[jp] == 1 || lfleg[jp] == 1){
      for (pre=AMSTrRecHit::gethead(jp); pre!=NULL; pre=pre->next()){
	    if(pre->checkstatus(AMSDBc::FalseX) || pre->checkstatus(AMSDBc::FalseTOFX))continue;
	hitr = pre->getHit();
	if (KLEFT[2][jp] != 10000){ 
	  if (hitr[0] == KLEFT[0][jp] && hitr[1] == KLEFT[1][jp] && hitr[2] == KLEFT[2][jp]){
	    pre->setstatus(AMSDBc::GAMMALEFT);
	  }
	}
	if (KRIGHT[2][jp] != 10000){
	  if (hitr[0] == KRIGHT[0][jp] && hitr[1] == KRIGHT[1][jp] && hitr[2] == KRIGHT[2][jp]){
	    pre->setstatus(AMSDBc::GAMMARIGHT);
	  }
	}
      }
      //
    } // lflag lfleg
  }
  // Be CArefull because if lflag lfleg ==1 and no KRIGHT neither KLEFT were recosntructed the planes is kept empty and we have to use interpolation....
}


void AMSTrTrackGamma::interpolate(int which, AMSPoint  pntplane, AMSDir dirplane,
                                  AMSPoint & P1,number & theta, number & phi, number & length){

  // interpolates track to plane (pntplane, dirplane)
  // and calculates the track parameters(P1,theta,phi) and total track length

  geant out[7];
  static number m55[5][5];
  geant init[7];
  geant point[6];
  geant charge=1;
    AMSDir s(_GThetaMSR,_GPhiMSR);
    AMSDir sl(_GThetaMSL,_GPhiMSL);
  if (which == 1){ //LEFT
  AMSDir dir(sin(_GThetaMSL)*cos(_GPhiMSL),
             sin(_GThetaMSL)*sin(_GPhiMSL),
             cos(_GThetaMSL));
   init[0]=_GP0MSL[0];
   init[1]=_GP0MSL[1];
   init[2]=_GP0MSL[2];
   init[6]=_GRidgidityMSL;
   init[3]=dir[0];
   init[4]=dir[1];
   init[5]=dir[2];
  }
  if (which == 0){ //RIGHT
  AMSDir dir(sin(_GThetaMSR)*cos(_GPhiMSR),
             sin(_GThetaMSR)*sin(_GPhiMSR),
             cos(_GThetaMSR)); 
   init[0]=_GP0MSR[0];
   init[1]=_GP0MSR[1];
   init[2]=_GP0MSR[2];
   init[6]=_GRidgidityMSR;
   init[3]=dir[0];
   init[4]=dir[1];
   init[5]=dir[2];
  }

  point[0]=pntplane[0];
  point[1]=pntplane[1];
  point[2]=pntplane[2];
  point[3]=dirplane[0];
  point[4]=dirplane[1];
  point[5]=dirplane[2];
  geant slength;
  TKFITPAR(init, charge,  point,  out,  m55, slength);
  P1[0]=out[0];
  P1[1]=out[1];
  P1[2]=out[2];
  theta=acos(out[5]);
if(theta<AMSDBc::pi/2)theta=AMSDBc::pi-theta;
  phi=atan2(out[4],out[3]);
  length=slength;  
}


void AMSTrTrackGamma::RecoLeftRight(int& refitting, integer FLPAT[], double   SLOPEf, double  INTERf,double x_starf,
                                    double z_starf,int fbotf,int ftopf,
                                    double firR,double lasR,double firL,double lasL,
                                    int fir_planeR, int  fir_planeL,int las_planeR,int las_planeL,double & slr, double & qlr){

#ifdef __AMSDEBUG__
 cout << "INTO RecoLeftRight: fbotf = "<<fbotf<<" ftopf = "<<ftopf<<endl;
 cout << "x_starf z_starf= "<<x_starf<<" "<<z_starf<<endl;
 cout << "SLOPEf INTERf = "<<SLOPEf<<" "<<INTERf<<endl;
 cout << " firR lasR firL,lasL=  "<< firR<<" "<< lasR<<" "<< firL <<" "<<lasL<<endl;
 cout << " fir_planeR,fir_planeL =  "<<fir_planeR<<" "<< fir_planeL<<endl;
 cout << " las_planeR,las_planeL =  "<<las_planeR<<" "<< las_planeL<<endl;
#endif

 int FIXNUr=0;
 int FIXNUl=0;
 double FIXVAr=10000;
 double FIXVAl=10000;
 double FIXMl=10000;
 double FIXQl=10000;
 double FIXMr=10000;
 double FIXQr=10000;


AMSTrRecHit * p;
AMSPoint p_hi;

 double X2P,X3P;
 double DeltaRecoTop=2.5;
 double DeltaRecoBottom=2.5;
 double MinDX2L,MinDX3L;
 double MinDX2R,MinDX3R;

 // NOW LEFT :


 if (fbotf == 1 && ftopf == 0){
   INTERf=x_starf-(SLOPEf*z_starf);
 }
   X2P=SLOPEf*firL+INTERf;
   X3P=SLOPEf*lasL+INTERf;

#ifdef __AMSDEBUG__
 cout << "LEFT X2P = "<<X2P<< "X3P = "<<X3P<<endl;
 cout << "x2l = "<<DeltaRecoTop<< " x3l = "<<DeltaRecoBottom<<endl;
#endif
 
 MinDX2L=10000;
 MinDX3L=10000;
 int conL_w=-1;
 double firLX;
 double lasLX;
 int conL_f=-1;
 int conL_l=-1;
 double conLf=0;
 double conLl=0;

 number deposf,deposl;
 number deposfmin=-1; 
 number deposlmin=-1; 
 //
 while (MinDX2L == 10000){
   conL_f++;
   conLf=0.5*conL_f;
   if (MinDX2L == 10000) DeltaRecoTop+=conL_f;
   //
   for (int i=0;i<TKDBc::nlay();i++){
     for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
       p_hi = p->getHit(); 
       deposf = p->getsum();
       if (p->checkstatus(AMSDBc::GAMMALEFT)){
	 //
	 if (p_hi[2] == firL && (p_hi[0] <= X2P+DeltaRecoTop && p_hi[0] >= X2P-DeltaRecoTop)){
	   if (fabs(X2P-p_hi[0]) <= MinDX2L ){
	     //
 	     if (deposf >= deposfmin){ // in this way I take akways the greatest charge at first hit
 	       deposfmin=deposf;
	       //
	       for (AMSTrRecHit * pcha=AMSTrRecHit::gethead(i); pcha!=NULL; pcha=pcha->next()){
		 pcha->clearstatus(AMSDBc::TOFFORGAMMA);
	       }
	       p->setstatus(AMSDBc::TOFFORGAMMA);
	       MinDX2L=fabs(X2P-p_hi[0]);
	       firLX=p_hi[0];
	       //
	      }
	     //
	   }
	 }
          //
   	 }
      }
    }
     //
    }

	 //
   while (MinDX3L == 10000){
   conL_l++;
   conLl=0.5*conL_l;
    if (MinDX3L == 10000) DeltaRecoBottom+=conLl;
   for (int i=0;i<TKDBc::nlay();i++){
     for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
	if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
       p_hi = p->getHit();
       deposl = p->getsum(); 
       if (p->checkstatus(AMSDBc::GAMMALEFT)){
	 if (p_hi[2] == lasL && (p_hi[0] <= X3P+DeltaRecoBottom && p_hi[0] >= X3P-DeltaRecoBottom)){
	   if (fabs(X3P-p_hi[0]) <= MinDX3L){
             
     
	     for (AMSTrRecHit * pcha=AMSTrRecHit::gethead(i); pcha!=NULL; pcha=pcha->next()){
	       pcha->clearstatus(AMSDBc::TOFFORGAMMA);
	     }
	     p->setstatus(AMSDBc::TOFFORGAMMA);
	     MinDX3L=fabs(X3P-p_hi[0]);
	     lasLX=p_hi[0];
	   }
	 }
	 //
    }
      }
         }
     }
   conL_w=max(conLl,conLf);
   //   cout <<" conL_l "<<conL_l<<" conL_f "<<conL_f<<" conL_w "<<conL_w<<endl;

 //*****************

AMSTrRecHit * pa;
AMSTrRecHit * bo;
AMSPoint pa_hi;
AMSPoint bo_hi;

 double RegStr=3.5;
 if (refitting){
 double RegStr=3.5;
 }


 int lhi=0;
 double x_d3[3],z_d3[3],y_d3[3];
 double x_d[8]={10000,10000,10000,10000,10000,10000,10000,10000};
 double y_d[8]={10000,10000,10000,10000,10000,10000,10000,10000};
 double z_d[8]={10000,10000,10000,10000,10000,10000,10000,10000};
 double M_X1,Q_X1,VARR;
 double CAMAX;
 double M_X,Q_X,M_Y,Q_Y,Str,Circ;  
 CAMAX=-10000;
 
 for (int i=0;i<TKDBc::nlay();i++){
   for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
	if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     p_hi = p->getHit(); 
     if (p->checkstatus(AMSDBc::GAMMALEFT)){
       if (p_hi[2] == firL && p->checkstatus(AMSDBc::TOFFORGAMMA)){
	 //
         for (int ii=0;ii<TKDBc::nlay();ii++){
	   for (pa=AMSTrRecHit::gethead(ii); pa!=NULL; pa=pa->next()){
	if(pa->checkstatus(AMSDBc::FalseX) || pa->checkstatus(AMSDBc::FalseTOFX))continue;
	     pa_hi = pa->getHit(); 
	     if (pa->checkstatus(AMSDBc::GAMMALEFT)){
	       if (pa_hi[2] == lasL && pa->checkstatus(AMSDBc::TOFFORGAMMA)){
//                
		 AMSTrTrackGamma::_intercept(p_hi[0],p_hi[1],p_hi[2],pa_hi[0],pa_hi[1],pa_hi[2],M_X,Q_X,M_Y,Q_Y);   
//
	         //ALTERNATIVE
                 if (refitting){	 
		 x_d[0]= p_hi[0];
		 y_d[0]= p_hi[1];
                 z_d[0]= p_hi[2];
                 }
		 //

		 for (int j=(i+1);j<ii;j++){
		   RegStr=3.5+conL_w;
 
		   //ALTERNATIVE
                   if (refitting){
                     RegStr=3.5+conL_w;                   	
                   if (z_d[lhi] != 10000){
		    lhi++;
		    if (lhi > 1){ 
                     if (lhi == 2) {
		       for (int im=0;im<lhi;im++){
		       }
		 AMSTrTrackGamma::_intercept(x_d[lhi-2],y_d[lhi-2],z_d[lhi-2],x_d[lhi-1],y_d[lhi-1],z_d[lhi-1],M_X,Q_X,M_Y,Q_Y);   
                     }
                     if (lhi > 2){
                       for (int jk=0;jk<3;jk++){
                         x_d3[jk]=x_d[lhi+jk-3];
                         y_d3[jk]=y_d[lhi+jk-3];  
                         z_d3[jk]=z_d[lhi+jk-3];  
                       }
		 AMSTrTrackGamma::_intercept(x_d[lhi-2],y_d[lhi-2],z_d[lhi-2],x_d[lhi-1],y_d[lhi-1],z_d[lhi-1],M_X,Q_X,M_Y,Q_Y);   
                     }
		    }		
		   }   
                   }
		   number bochamin=-1; 
		   for (bo=AMSTrRecHit::gethead(j); bo!=NULL; bo=bo->next()){
		     if(bo->checkstatus(AMSDBc::FalseX) || bo->checkstatus(AMSDBc::FalseTOFX))continue;
		     bo_hi = bo->getHit();
                     number bocha  = bo->getsum(); 
		     if (bo->checkstatus(AMSDBc::GAMMALEFT)){
		       AMSTrTrackGamma::_DISTANCE(bo_hi[0],bo_hi[1],bo_hi[2],M_X,Q_X,M_Y,Q_Y,Str,Circ);
		       if (FLPAT[j] == 1){

			 if (fabs(Str) <= RegStr && fabs(Circ) < 5){
			   RegStr=fabs(Str);
                           
                           if (bocha >= bochamin){ // I take akways the greatest charge single hit
 	                      bochamin=bocha;

			   for (AMSTrRecHit * bo2=AMSTrRecHit::gethead(j); bo2!=NULL; bo2=bo2->next()){
			     bo2->clearstatus(AMSDBc::TOFFORGAMMA);
			   }
			   bo->setstatus(AMSDBc::TOFFORGAMMA);
                                         
			   //ALTERNATIVE
                           if (refitting){	
			   x_d[lhi]=bo_hi[0];
                           y_d[lhi]=bo_hi[1];
			   z_d[lhi]=bo_hi[2];
			   }
                          //  
			   
			   }
			 }
		       }
		       if (FLPAT[j] != 1){
			 if (fabs(Str) <= RegStr){
			   RegStr=fabs(Str);
			   for (AMSTrRecHit * bo2=AMSTrRecHit::gethead(j); bo2!=NULL; bo2=bo2->next()){
			     bo2->clearstatus(AMSDBc::TOFFORGAMMA);
			   }
			   bo->setstatus(AMSDBc::TOFFORGAMMA);

			   //ALTERNATIVE
                           if (refitting){	
      			   x_d[lhi]=bo_hi[0];
                           y_d[lhi]=bo_hi[1];
			   z_d[lhi]=bo_hi[2];
			   }
			   //  
			   
			 }
		       }
		       //
		     }
		   }
		 }
		 //
	       }
	     }
	   }
	 }
       }
     }
   }
 }
 //*****************
 //////////////////////////////////////////
 ///
 double xa[8];
 double za[8];
 double ya[8];
 int laa=0;
  //Now in order to be free from the status(AMSDBc::TOFFORGAMMA):
  for (int i=0;i<TKDBc::nlay();i++){
   for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
	if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     p_hi = p->getHit(); 
      if (p->checkstatus(AMSDBc::TOFFORGAMMA)){
       za[laa]=p_hi[2];
       xa[laa]=p_hi[0];
       ya[laa]=p_hi[1];
       laa++;
	//             cout<< "*after best_last_____ LEFT p_hi[1 .. 3] = "<<p_hi[0]<<" "<<p_hi[1]<<" "<<p_hi[2]<<endl; 
      }
      p->clearstatus(AMSDBc::GAMMALEFT);
   }
  }

 double A11,B11,VA11;
 dlinearme(laa,za,xa,A11,B11,VA11);         // linear fit


  if (refitting >=100 && VA11 >1){



 // in order to reasset the linearity in XZ:
double z_tkl[trconst::maxlay];
 for(int i=0;i<TKDBc::nlay();i++){
   z_tkl[i]=TKDBc::zposl(i);
 }
AMSPoint pe_hi;
 double diffm;
 double xnew[8]={10000,10000,10000,10000,10000,10000,10000,10000};
 AMSTrRecHit * pe;
 // cout <<" fbotf "<<fbotf<<" ftopf "<<ftopf<<endl;
 M_X=SLOPEf;
 Q_X=INTERf;
 if (fbotf == 0 && ftopf == 1){
   int manyhit=0;
  for (int i=0;i<TKDBc::nlay();i++){
   for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
	if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     p_hi = p->getHit(); 
      if (p->checkstatus(AMSDBc::TOFFORGAMMA)){
          diffm=10000;
          number pechamin=-1;
          for (pe=AMSTrRecHit::gethead(i); pe!=NULL; pe=pe->next()){
	    if(pe->checkstatus(AMSDBc::FalseX) || pe->checkstatus(AMSDBc::FalseTOFX))continue;
            pe_hi = pe->getHit();
            number pecha  = pe->getsum();
            if (pe_hi[1] == p_hi[1]){
              if (fabs(pe_hi[0]-(SLOPEf*p_hi[2]+INTERf)) <= diffm){
                if (pecha >= pechamin){
                  pechamin=pecha;
		xnew[i]=pe_hi[0];
                diffm=fabs(pe_hi[0]-(SLOPEf*p_hi[2]+INTERf));
		}
              }
	    }
	  }
      //
      if (xnew[i] != 10000 && xnew[i] != p_hi[0]){
           for (AMSTrRecHit * paa=AMSTrRecHit::gethead(i); paa!=NULL; paa=paa->next()){
	     if(paa->checkstatus(AMSDBc::FalseX) || paa->checkstatus(AMSDBc::FalseTOFX))continue;
	     AMSPoint paa_hi = paa->getHit();
             paa->clearstatus(AMSDBc::TOFFORGAMMA);
             if (paa_hi[0] == xnew[i] && paa_hi[1] == p_hi[1] && paa_hi[2] == p_hi[2]){
              paa->setstatus(AMSDBc::TOFFORGAMMA); 
	     }
	   }
      }

      //
      if (xnew[i] != 10000) manyhit++;
      if (manyhit >= 2){
	int ifla=0;
            for (int kk=i-1; kk!=-1; --kk){
               if (xnew[kk] != 10000 && ifla==0){
	       ifla=1;
	       // slope in XZ : MX
	       M_X = (xnew[kk]-xnew[i])/(z_tkl[kk]-z_tkl[i]); 
	       // intercept in XZ : QX 
	       Q_X = xnew[i] - (z_tkl[i]*(xnew[kk]-xnew[i]))/(z_tkl[kk]-z_tkl[i]);
	     }
	    }
      }
      //

      }
   }
  }
 }
 if (fbotf == 1 && ftopf == 0){
   int manyhit=0;
  for (int i=TKDBc::nlay()-1; i!=-1 ; --i){
      for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
	if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     p_hi = p->getHit(); 
      if (p->checkstatus(AMSDBc::TOFFORGAMMA)){
          diffm=10000;
          number pechamin=-1;
          for (pe=AMSTrRecHit::gethead(i); pe!=NULL; pe=pe->next()){
	    if(pe->checkstatus(AMSDBc::FalseX) || pe->checkstatus(AMSDBc::FalseTOFX))continue;
            pe_hi = pe->getHit();
            number pecha  = pe->getsum();
            if (pe_hi[1] == p_hi[1]){
              if (fabs(pe_hi[0]-(M_X*p_hi[2]+Q_X)) <= diffm){
                if (pecha >= pechamin){
                  pechamin=pecha;
		xnew[i]=pe_hi[0];
                diffm=fabs(pe_hi[0]-(M_X*p_hi[2]+Q_X));
		}
              }
	    }
	  }
      //
      if (xnew[i] != 10000 && xnew[i] != p_hi[0]){
           for (AMSTrRecHit * paa=AMSTrRecHit::gethead(i); paa!=NULL; paa=paa->next()){
	     if(paa->checkstatus(AMSDBc::FalseX) || paa->checkstatus(AMSDBc::FalseTOFX))continue;
	     AMSPoint paa_hi = paa->getHit();
             paa->clearstatus(AMSDBc::TOFFORGAMMA);
             if (paa_hi[0] == xnew[i] && paa_hi[1] == p_hi[1] && paa_hi[2] == p_hi[2]){
              paa->setstatus(AMSDBc::TOFFORGAMMA); 
	     }
	   }
      }

      //
      if (xnew[i] != 10000) manyhit++;
      if (manyhit >= 2){
	int ifla=0;
           for (int kk=i+1; kk!=TKDBc::nlay(); kk++){
	     if (xnew[kk] != 10000 && ifla==0){
	       ifla=1;
	       // slope in XZ : MX
	       M_X = (xnew[kk]-xnew[i])/(z_tkl[kk]-z_tkl[i]); 
	       // intercept in XZ : QX 
	       Q_X = xnew[i] - (z_tkl[i]*(xnew[kk]-xnew[i]))/(z_tkl[kk]-z_tkl[i]);
	     }
	   }
      }
      //

      }
      }
  }
 }



 // let's check the linearity in X and remove the highest residual :
 double xx[8];
 double zz[8];
 double yy[8];
 int li=0;
 for (int i=0;i<TKDBc::nlay();i++){
   for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
	if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     if (p->checkstatus(AMSDBc::TOFFORGAMMA)){
       AMSPoint pli = p->getHit();
       zz[li]=pli[2];
       xx[li]=pli[0];
       yy[li]=pli[1];
       li++;
     }
   }
 }



 double dxheo[8];
 double A,B,VA;
 dlinearme(li,zz,xx,A,B,VA);         // linear fit
 // cout<< "LEFT$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  A,B,VA  "<< A<<" "<<B<<" "<<VA<<endl; 
 int higcou=0;
 double maxdxheo=0.4;
 double maxz,maxy,maxx;
 int higcou2=0;
 for (int i=0;i<li;i++){
  dxheo[i]=fabs(xx[i]-(B*zz[i])-A);
  if (dxheo[i] > maxdxheo){
  higcou2++;
  }
 }
 for (int i=0;i<li;i++){
   dxheo[i]=fabs(xx[i]-(B*zz[i])-A); 
   //   cout<< "LEFT$$$$$$$$$ zz and residual "<<i<<"]= "<< zz[i]<<" "<<dxheo[i]<<endl; 
   if (dxheo[i] > maxdxheo){
     higcou++;
     maxdxheo=dxheo[i];
     maxz=zz[i];
     maxy=yy[i];
     maxx=xx[i];
   }
 }
 double xx2[8];
 double zz2[8];
 int li2=0;
double xx3,xx3v; 

if (higcou != 0 && (li-higcou2) >= 3 && li > 3){
   for (int i=0;i<li;i++){
     if (dxheo[i]<maxdxheo){
       zz2[li2]=zz[i];
       xx2[li2]=xx[i];
       li2++;
     }
   }     

 
 dlinearme(li2,zz2,xx2,A,B,VA);         // linear fit
 // cout<< "LEFT   2   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  A,B,VA  "<< A<<" "<<B<<" "<<VA<<endl; 
 for (int i=0;i<li2;i++){
   xx3=B*maxz+A;  
 }
   double closest=10000;
   for (int i=0;i<TKDBc::nlay();i++){
     for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
	if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
       AMSPoint pli = p->getHit();
       if (pli[2] == maxz && pli[1] == maxy){
	 if (pli[0] == maxx)p->clearstatus(AMSDBc::TOFFORGAMMA);
         if (fabs(pli[0] - xx3) < closest){
	   closest=fabs(pli[0] - xx3);
           xx3v=pli[0];        
	 } 
       }
     }
   }
   for (int i=0;i<TKDBc::nlay();i++){
     for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
	if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
       AMSPoint pli = p->getHit();
       if (pli[2] == maxz && pli[1] == maxy && pli[0] == xx3v){
          p->setstatus(AMSDBc::TOFFORGAMMA);
       }
     }
   }
} 
   FIXMl=B;
   FIXQl=A;
   FIXVAl=VA;
   //   cout <<" FIXVAl= "<<FIXVAl<<endl;
   //cout <<" FIXMl, FIXQl "<<FIXMl<<" "<<FIXQl<<endl;
   
  } // refitting >=100
//*****************

  for (int i=0;i<TKDBc::nlay();i++){
   for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
	if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     if (p->checkstatus(AMSDBc::TOFFORGAMMA)){
      p->setstatus(AMSDBc::GAMMALEFT);
     }
   }
  }
  for (int i=0;i<TKDBc::nlay();i++){
   for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
	if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     p->clearstatus(AMSDBc::TOFFORGAMMA);
     p_hi = p->getHit(); 
     if (p->checkstatus(AMSDBc::GAMMALEFT) ){
       //    cout<< "*+++++ LEFT p_hi[1 .. 3] = "<<p_hi[0]<<" "<<p_hi[1]<<" "<<p_hi[2]<<endl;
      FIXNUl++;
     }
   }
  }
  
// NOW RIGHT :

   DeltaRecoTop=2.5;
   DeltaRecoBottom=2.5;
   MinDX2R=10000;
   MinDX3R=10000;

 if (fbotf == 1 && ftopf == 0){
   INTERf=x_starf-(SLOPEf*z_starf);
 }
   X2P=SLOPEf*firR+INTERf;
   X3P=SLOPEf*lasR+INTERf;
#ifdef __AMSDEBUG__
 cout << "RIGHT X2P = "<<X2P<< "X3P = "<<X3P<<endl;
  cout << "x2l = "<<DeltaRecoTop<< " x3l = "<<DeltaRecoBottom<<endl;
#endif

   int conR_w=-1;
   double firRX;
   double lasRX;
   int conR_f=-1;
   int conR_l=-1;
    double conRf=0;
     double conRl=0;
    deposfmin=-1; 
    deposlmin=-1; 
   /// /// /// /// //// /
   while (MinDX2R == 10000){
     conR_f++;
     conRf=0.5*conR_f;
     if (MinDX2R == 10000) DeltaRecoTop+=conRf;

     //
     for (int i=0;i<TKDBc::nlay();i++){
       for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
	if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
	 p_hi = p->getHit();  
         deposf = p->getsum();
   	 if (p->checkstatus(AMSDBc::GAMMARIGHT)){
   	   if (p_hi[2] == firR && (p_hi[0] <= X2P+DeltaRecoTop && p_hi[0] >= X2P-DeltaRecoTop)){
   	     if (fabs(X2P-p_hi[0]) <= MinDX2R){
	     //
 	     if (deposf >= deposfmin){
 	       deposfmin=deposf;
	       //
	       for (AMSTrRecHit * pcha=AMSTrRecHit::gethead(i); pcha!=NULL; pcha=pcha->next()){
		 pcha->clearstatus(AMSDBc::TOFFORGAMMA);
	       }
	       p->setstatus(AMSDBc::TOFFORGAMMA);
   	       MinDX2R=fabs(X2P-p_hi[0]);
   	       firRX=p_hi[0];
	       //
	      }
             //
	     }
	   }
	   //
   	 }
       }
     }
     //
   }
   /////////
       while (MinDX3R == 10000){
   	conR_l++;
        conRl=0.5*conR_l;
  	if (MinDX3R == 10000) DeltaRecoBottom+=conRl;
	for (int i=0;i<TKDBc::nlay();i++){
	  for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
	if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
	    p_hi = p->getHit(); 
	    if (p->checkstatus(AMSDBc::GAMMARIGHT)){
	      if (p_hi[2] == lasR && (p_hi[0] <= X3P+DeltaRecoBottom && p_hi[0] >= X3P-DeltaRecoBottom)){
		if (fabs(X3P-p_hi[0]) < MinDX3R){
		  for (AMSTrRecHit * pcha=AMSTrRecHit::gethead(i); pcha!=NULL; pcha=pcha->next()){
		    pcha->clearstatus(AMSDBc::TOFFORGAMMA);
		  }
		  p->setstatus(AMSDBc::TOFFORGAMMA);
		  MinDX3R=fabs(X3P-p_hi[0]);
		  lasRX=p_hi[0];
		}
	      }
	      //
	    }
	  }
	  	}
	   }
      
      conR_w=max(conRl,conRf);


#ifdef __AMSDEBUG__
   cout <<" conR_l "<<conR_l<<" conR_f "<<conR_f<<" conR_w "<<conR_w<<endl;
   if((MinDX2R < 2*MinDX3R || MinDX3R < 2*MinDX2R) || (MinDX2R > 2 || MinDX3R >2)){
     cout << "MinDX2R  MinDX3R= "<<MinDX2R<<" "<<MinDX3R<<endl;
     cout <<" SOmething is wrong in the X road path "<<endl;
   } //the end
#endif

 //*****************
 // NOW RIGHT :

CAMAX=-10000;
 RegStr=3.5;
 if (refitting){
  RegStr=3.5;
 }

lhi=0;
 for (int i=0;i<TKDBc::nlay();i++){
 z_d[i]=10000;
 }

 for (int i=0;i<TKDBc::nlay();i++){
   for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
	if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     p_hi = p->getHit(); 
     if (p->checkstatus(AMSDBc::GAMMARIGHT)){
       if (p_hi[2] == firR && p->checkstatus(AMSDBc::TOFFORGAMMA)){
         for (int ii=0;ii<TKDBc::nlay();ii++){
	   for (pa=AMSTrRecHit::gethead(ii); pa!=NULL; pa=pa->next()){
	     if(pa->checkstatus(AMSDBc::FalseX) || pa->checkstatus(AMSDBc::FalseTOFX))continue;
	     pa_hi = pa->getHit(); 
	     if (pa->checkstatus(AMSDBc::GAMMARIGHT)){
	       if (pa_hi[2] == lasR && pa->checkstatus(AMSDBc::TOFFORGAMMA)){

		 AMSTrTrackGamma::_intercept(p_hi[0],p_hi[1],p_hi[2],pa_hi[0],pa_hi[1],pa_hi[2],M_X,Q_X,M_Y,Q_Y);
		 //
	         //ALTERNATIVE	 
		 if (refitting){	
		 x_d[0]= p_hi[0];
                 y_d[0]= p_hi[1];
		 z_d[0]= p_hi[2];
                 }
		 //
		 for (int j=(i+1);j<ii;j++){
		   RegStr=3.5+conR_w;

		   //ALTERNATIVE
                  if (refitting){
                    RegStr=3.5+conR_w;	
                   if (z_d[lhi] != 10000){
		    lhi++;
		    if (lhi > 1){ 
                     if (lhi == 2) {
		 AMSTrTrackGamma::_intercept(x_d[lhi-2],y_d[lhi-2],z_d[lhi-2],x_d[lhi-1],y_d[lhi-1],z_d[lhi-1],M_X,Q_X,M_Y,Q_Y);   
                     }
                     if (lhi > 2){
                       for (int jk=0;jk<3;jk++){
                         x_d3[jk]=x_d[lhi+jk-3];
                         y_d3[jk]=y_d[lhi+jk-3];
                         z_d3[jk]=z_d[lhi+jk-3];
                       }
		 AMSTrTrackGamma::_intercept(x_d[lhi-2],y_d[lhi-2],z_d[lhi-2],x_d[lhi-1],y_d[lhi-1],z_d[lhi-1],M_X,Q_X,M_Y,Q_Y);   
                     }
		    }		
		   }
                  }   
		   //
		   	
		   number bochamin=-1; 	   
		   for (bo=AMSTrRecHit::gethead(j); bo!=NULL; bo=bo->next()){
		     if(bo->checkstatus(AMSDBc::FalseX) || bo->checkstatus(AMSDBc::FalseTOFX))continue;
		     bo_hi = bo->getHit(); 
                     number bocha  = bo->getsum(); 
		     if (bo->checkstatus(AMSDBc::GAMMARIGHT)){
		       AMSTrTrackGamma::_DISTANCE(bo_hi[0],bo_hi[1],bo_hi[2],M_X,Q_X,M_Y,Q_Y,Str,Circ);  
		       //
		       if (FLPAT[j] == 1){

			 if (fabs(Str) <= RegStr && fabs(Circ) < 5){
			   RegStr=fabs(Str);
                           
                           if (bocha >= bochamin){ // I take akways the greatest charge single hit
 	                      bochamin=bocha;

			   for (AMSTrRecHit * bo2=AMSTrRecHit::gethead(j); bo2!=NULL; bo2=bo2->next()){
			     bo2->clearstatus(AMSDBc::TOFFORGAMMA);
			   }
			   bo->setstatus(AMSDBc::TOFFORGAMMA);
			   //ALTERNATIVE
                           if (refitting){	
			   x_d[lhi]=bo_hi[0];
                           y_d[lhi]=bo_hi[1];
			   z_d[lhi]=bo_hi[2];
                           }
			   //
			   }
			 }
		       }
		       if (FLPAT[j] != 1){
			 if (fabs(Str) <= RegStr){

			   RegStr=fabs(Str);
			   for (AMSTrRecHit * bo2=AMSTrRecHit::gethead(j); bo2!=NULL; bo2=bo2->next()){
			     bo2->clearstatus(AMSDBc::TOFFORGAMMA);
			   }
			   bo->setstatus(AMSDBc::TOFFORGAMMA);
			   //ALTERNATIVE
                           if (refitting){	
			   x_d[lhi]=bo_hi[0];
                           y_d[lhi]=bo_hi[1];
			   z_d[lhi]=bo_hi[2];
                           }
			   //
			 }
		       }
		       //
		     }
		   }
		 }
		 //
	       }
	     }
	   }
	 }
       }
     }
   }
 }
 //*****************
 //////////////////////////////////////////////////
 laa=0;
  //Now in order to be free from the status(AMSDBc::TOFFORGAMMA):
  for (int i=0;i<TKDBc::nlay();i++){
   for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
	if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     p_hi = p->getHit(); 
      if (p->checkstatus(AMSDBc::TOFFORGAMMA)){
       za[laa]=p_hi[2];
       xa[laa]=p_hi[0];
       ya[laa]=p_hi[1];
       laa++;
	//       cout<< "*after best_last_____ RIGHT p_hi[1 .. 3] = "<<p_hi[0]<<" "<<p_hi[1]<<" "<<p_hi[2]<<endl; 
      }
      p->clearstatus(AMSDBc::GAMMARIGHT);
   }
  }
 //*****************

 dlinearme(laa,za,xa,A11,B11,VA11);         // linear fit
 if (refitting >=100  && VA11 >1){

double z_tkl[trconst::maxlay];
 for(int i=0;i<TKDBc::nlay();i++){
   z_tkl[i]=TKDBc::zposl(i);
 }
 // in order to reasset the linearity in XZ:
AMSPoint pe_hi;
 double diffm=10000;
 double xnew[8]={10000,10000,10000,10000,10000,10000,10000,10000};
 AMSTrRecHit * pe;


 M_X=SLOPEf;
 Q_X=INTERf;
 if (fbotf == 0 && ftopf == 1){
   int manyhit=0;
  for (int i=0;i<TKDBc::nlay();i++){
   for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
	if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     p_hi = p->getHit(); 
      if (p->checkstatus(AMSDBc::TOFFORGAMMA)){
          diffm=10000;
          number pechamin=-1;
          for (pe=AMSTrRecHit::gethead(i); pe!=NULL; pe=pe->next()){
	if(pe->checkstatus(AMSDBc::FalseX) || pe->checkstatus(AMSDBc::FalseTOFX))continue;
            pe_hi = pe->getHit();
            number pecha  = pe->getsum(); 
            if (pe_hi[1] == p_hi[1]){
              if (fabs(pe_hi[0]-(SLOPEf*p_hi[2]+INTERf)) <= diffm){
                if (pecha >= pechamin){
                  pechamin=pecha;
		xnew[i]=pe_hi[0];
                diffm=fabs(pe_hi[0]-(SLOPEf*p_hi[2]+INTERf));
		}
              }
	    }
	  }
      //
      if (xnew[i] != 10000 && xnew[i] != p_hi[0]){
           for (AMSTrRecHit * paa=AMSTrRecHit::gethead(i); paa!=NULL; paa=paa->next()){
	     if(paa->checkstatus(AMSDBc::FalseX) || paa->checkstatus(AMSDBc::FalseTOFX))continue;
	     AMSPoint paa_hi = paa->getHit();
             paa->clearstatus(AMSDBc::TOFFORGAMMA);
             if (paa_hi[0] == xnew[i] && paa_hi[1] == p_hi[1] && paa_hi[2] == p_hi[2]){
              paa->setstatus(AMSDBc::TOFFORGAMMA); 
	     }
	   }

      }

      //
      if (xnew[i] != 10000) manyhit++;
      if (manyhit >= 2){
	int ifla=0;
            for (int kk=i-1; kk!=-1; --kk){
               if (xnew[kk] != 10000 && ifla==0){
	       ifla=1;
	       // slope in XZ : MX
	       M_X = (xnew[kk]-xnew[i])/(z_tkl[kk]-z_tkl[i]); 
	       // intercept in XZ : QX 
	       Q_X = xnew[i] - (z_tkl[i]*(xnew[kk]-xnew[i]))/(z_tkl[kk]-z_tkl[i]);
	     }
	    }
      }
      //

      }
   }
  }
 }
 if (fbotf == 1 && ftopf == 0){
   int manyhit=0;
  for (int i=TKDBc::nlay()-1; i!=-1 ; --i){
   for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
	if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     p_hi = p->getHit(); 
      if (p->checkstatus(AMSDBc::TOFFORGAMMA)){
          diffm=10000;
          number pechamin=-1;
          for (pe=AMSTrRecHit::gethead(i); pe!=NULL; pe=pe->next()){
	    if(pe->checkstatus(AMSDBc::FalseX) || pe->checkstatus(AMSDBc::FalseTOFX))continue;
            pe_hi = pe->getHit();
            number pecha  = pe->getsum();
            if (pe_hi[1] == p_hi[1]){
              if (fabs(pe_hi[0]-(SLOPEf*p_hi[2]+INTERf)) <= diffm){
                if (pecha >= pechamin){
                  pechamin=pecha;
		xnew[i]=pe_hi[0];
                diffm=fabs(pe_hi[0]-(SLOPEf*p_hi[2]+INTERf));
		}
              }
	    }
	  }

      if (xnew[i] != 10000 && xnew[i] != p_hi[0]){
           for (AMSTrRecHit * paa=AMSTrRecHit::gethead(i); paa!=NULL; paa=paa->next()){
	     if(paa->checkstatus(AMSDBc::FalseX) || paa->checkstatus(AMSDBc::FalseTOFX))continue;
	     AMSPoint paa_hi = paa->getHit();
             paa->clearstatus(AMSDBc::TOFFORGAMMA);
             if (paa_hi[0] == xnew[i] && paa_hi[1] == p_hi[1] && paa_hi[2] == p_hi[2]){
              paa->setstatus(AMSDBc::TOFFORGAMMA); 
	     }
	   }
      }

      //
      if (xnew[i] != 10000) manyhit++;
      if (manyhit >= 2){
	int ifla=0;
           for (int kk=i+1; kk!=TKDBc::nlay(); kk++){
	     if (xnew[kk] != 10000 && ifla==0){
	       ifla=1;
	       // slope in XZ : MX
	       M_X = (xnew[kk]-xnew[i])/(z_tkl[kk]-z_tkl[i]); 
	       // intercept in XZ : QX 
	       Q_X = xnew[i] - (z_tkl[i]*(xnew[kk]-xnew[i]))/(z_tkl[kk]-z_tkl[i]);
	     }
	   }
      }
      //

      }
   }
  }
 }

 // let's check the linearity in X and remove the highest residual :

 double xx[8];
 double zz[8];
 double yy[8];
 int li=0;
 for (int i=0;i<TKDBc::nlay();i++){
   for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     if (p->checkstatus(AMSDBc::TOFFORGAMMA)){
       AMSPoint pli = p->getHit();
       zz[li]=pli[2];
       xx[li]=pli[0];
       yy[li]=pli[1];
       li++;
     }
   }
 }
 double dxheo[8];
 double A,B,VA;
 dlinearme(li,zz,xx,A,B,VA);         // linear fit
 // cout<< "RIGHT$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  A,B,VA  "<< A<<" "<<B<<" "<<VA<<endl; 
 int higcou=0;

 double maxdxheo=0.4;
 double maxz,maxy,maxx;
 int higcou2=0;
 for (int i=0;i<li;i++){
  dxheo[i]=fabs(xx[i]-(B*zz[i])-A);
  if (dxheo[i] > maxdxheo){
  higcou2++;
  }
 }

 for (int i=0;i<li;i++){
   dxheo[i]=fabs(xx[i]-(B*zz[i])-A);  
   //   cout<< "RIGHT$$$$$$$$$ zz and residual "<<i<<"]= "<< zz[i]<<" "<<dxheo[i]<<endl;
   if (dxheo[i] > maxdxheo){
     higcou++;
     maxdxheo=dxheo[i];
     maxz=zz[i];
     maxy=yy[i];
     maxx=xx[i];
   }
 }
 double xx2[8];
 double zz2[8];
 int li2=0;
 double xx3,xx3v;
 if (higcou != 0 && (li-higcou2) >= 3 && li > 3){
   for (int i=0;i<li;i++){
     if (dxheo[i]<maxdxheo){
       zz2[li2]=zz[i];
       xx2[li2]=xx[i];
       li2++;
     }
   }     


 dlinearme(li2,zz2,xx2,A,B,VA);         // linear fit
 // cout<< "RIGHT   2   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  A,B,VA  "<< A<<" "<<B<<" "<<VA<<endl; 
 for (int i=0;i<li2;i++){
   xx3=B*maxz+A;  
 }
   double closest=10000;
   for (int i=0;i<TKDBc::nlay();i++){
     for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
       AMSPoint pli = p->getHit();
       if (pli[2] == maxz && pli[1] == maxy){
	 if (pli[0] == maxx)p->clearstatus(AMSDBc::TOFFORGAMMA);
         if (fabs(pli[0] - xx3) < closest){
	   closest=fabs(pli[0] - xx3);
           xx3v=pli[0];        
	 } 
       }
     }
   }
   for (int i=0;i<TKDBc::nlay();i++){
     for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
       AMSPoint pli = p->getHit();
       if (pli[2] == maxz && pli[1] == maxy && pli[0] == xx3v){
          p->setstatus(AMSDBc::TOFFORGAMMA);
       }
     }
   }
 }
   FIXMr=B;
   FIXQr=A;
   FIXVAr=VA;
   //   cout <<" FIXVAr= "<<FIXVAr<<endl;
   //   cout <<" FIXMr, FIXQr "<<FIXMr<<" "<<FIXQr<<endl;

  } //refitting >=100


//*****************
  for (int i=0;i<TKDBc::nlay();i++){
   for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     if (p->checkstatus(AMSDBc::TOFFORGAMMA)){
      p->setstatus(AMSDBc::GAMMARIGHT);
      }
   }
  }
  for (int i=0;i<TKDBc::nlay();i++){
   for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     p->clearstatus(AMSDBc::TOFFORGAMMA);
      p_hi = p->getHit(); 
     if (p->checkstatus(AMSDBc::GAMMARIGHT) ){
       cout<< "*+++++ RIGHT p_hi[1 .. 3] = "<<p_hi[0]<<" "<<p_hi[1]<<" "<<p_hi[2]<<endl;
       FIXNUr++;
     }
   }
  }


  if ((FIXMl != 10000 && FIXQl !=10000) || (FIXMr != 10000 && FIXQr !=10000)){  

     double bestvar=min(FIXVAr,FIXVAl);
     double lesshit=min(FIXNUr,FIXNUl);
     // we require sufficient number of hits
     //     if ((FIXVAr > 1 || FIXVAl > 1) && (lesshit <= 4)){
     if ((FIXVAr > 1 || FIXVAl > 1)){
     if ((bestvar == FIXVAr && FIXNUr > 4) ||  (bestvar == FIXVAl && FIXNUl > 4)){
       if (bestvar < 1){
	 if (bestvar == FIXVAr){
	   //              cout <<" out FIXMr, FIXQr "<<FIXMr<<" "<<FIXQr<<endl;
          slr=FIXMr;
          qlr=FIXQr;
	 }
         if (bestvar == FIXVAl){
	   //  cout <<" out FIXMl, FIXQl "<<FIXMl<<" "<<FIXQl<<endl;
          slr=FIXMl;
          qlr=FIXQl;
	 }

         refitting+=10000;
       }
     }
     }
  }

 // tk1 SINGLE HIT
 double h1r[3]={10000,10000,10000};
 double h2r[3]={10000,10000,10000};
 double h1l[3]={10000,10000,10000};
 double h2l[3]={10000,10000,10000};
 double h3l[3]={10000,10000,10000};
 double h3r[3]={10000,10000,10000};

   for (p=AMSTrRecHit::gethead(0); p!=NULL; p=p->next()){ 
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     AMSPoint ph=p->getHit();
     if (p->checkstatus(AMSDBc::GAMMARIGHT)){
       h1r[0]= ph[0];
       h1r[1]= ph[1];
       h1r[2]= ph[2];
     }
     if (p->checkstatus(AMSDBc::GAMMALEFT)){
       h1l[0]= ph[0];
       h1l[1]= ph[1];
       h1l[2]= ph[2];
     }
   }
   for (p=AMSTrRecHit::gethead(1); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue; 
     AMSPoint ph=p->getHit();
     if (p->checkstatus(AMSDBc::GAMMARIGHT)){
       h2r[0]= ph[0];
       h2r[1]= ph[1];
       h2r[2]= ph[2];
     }
     if (p->checkstatus(AMSDBc::GAMMALEFT)){
       h2l[0]= ph[0];
       h2l[1]= ph[1];
       h2l[2]= ph[2];
     }
   }

   for (p=AMSTrRecHit::gethead(2); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue; 
     AMSPoint ph=p->getHit();
     if (p->checkstatus(AMSDBc::GAMMARIGHT)){
       h3r[0]= ph[0];
       h3r[1]= ph[1];
       h3r[2]= ph[2];
     }
     if (p->checkstatus(AMSDBc::GAMMALEFT)){
       h3l[0]= ph[0];
       h3l[1]= ph[1];
       h3l[2]= ph[2];
     }
   }


   double rewy[3]={10000,10000,10000};
   if (h2r[2] != 10000 && h2l[2] != 10000){
     rewy[1]=fabs(h2r[1]-h2l[1]);
   }
   if (h3r[2] != 10000 && h3l[2] != 10000){
     rewy[2]=fabs(h3r[1]-h3l[1]);
   }

   if ((h1r[2] != 10000 && h1l[2] == 10000) && 
       ((h2r[2] != 10000 && h2l[2] != 10000) ||  (h3r[2] != 10000 && h3l[2] != 10000))){
     if (rewy[1] <= 0.09 || rewy[2] <= 0.12 ){
       for (p=AMSTrRecHit::gethead(0); p!=NULL; p=p->next()){ 
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
	 AMSPoint ph=p->getHit();
	 if (p->checkstatus(AMSDBc::GAMMARIGHT)){
	   if (ph[0] == h1r[0] && ph[1] == h1r[1] && ph[2] == h1r[2]){
	     p->setstatus(AMSDBc::GAMMALEFT);
	     p->setstatus(AMSDBc::GAMMARIGHT);
	   }
	 }
       }
     }
   }
   if ((h1r[2] == 10000 && h1l[2] != 10000) && 
       ((h2r[2] != 10000 && h2l[2] != 10000) ||  (h3r[2] != 10000 && h3l[2] != 10000))){
     if (rewy[1] <= 0.09 || rewy[2] <= 0.12 ){
       for (p=AMSTrRecHit::gethead(0); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue; 
	 AMSPoint ph=p->getHit();
	 if (p->checkstatus(AMSDBc::GAMMALEFT)){
	   if (ph[0] == h1l[0] && ph[1] == h1l[1] && ph[2] == h1l[2]){
	     p->setstatus(AMSDBc::GAMMALEFT);
	     p->setstatus(AMSDBc::GAMMARIGHT);
	   }
	 }
       }
     }
   }


 /////


  //..
}

void AMSTrTrackGamma::HITRESEARCH(int pla, double RES_REF, AMSPoint P_0L2, AMSPoint P_0R2){

 AMSPoint hitr;
 double d_yl=10000000;
 double d_yr=10000000;
 double d_xl=10000000;
 double d_xr=10000000;
 double tey_L,tey_R;
double tex_L,tex_R;
 double KLEFTY,KRIGHTY,KY2;
 AMSTrRecHit * p;

   // Now we found the theoretical hits pair on plane "pla" and we look for the closest reconstructed hits
   for (p=AMSTrRecHit::gethead(pla); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     hitr = p->getHit();
     tey_L=fabs(hitr[1]-P_0L2[1]);
     if (tey_L < d_yl){
        d_yl=tey_L;
        KLEFTY= hitr[1];
     }
     tey_R=fabs(hitr[1]-P_0R2[1]);
     if (tey_R < d_yr){
         d_yr=tey_R;  
         KRIGHTY= hitr[1];
     }
   }

   //////
   if (KLEFTY == KRIGHTY){
     d_yl=10000000;
     tey_L=0;
     for (p=AMSTrRecHit::gethead(pla); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
       hitr = p->getHit();
       if (fabs(hitr[1]-KLEFTY) >= RES_REF){
	 tey_L=fabs(hitr[1]-KLEFTY);
	 if (tey_L < d_yl) d_yl=tey_L;
	 KY2=hitr[1];
       }
     }
     
     if (KY2 >= KLEFTY){
       KRIGHTY=KY2;
     }
     if (KY2 < KLEFTY){
       KRIGHTY=KLEFTY;
       KLEFTY=KY2;
     }
   }
   ///////
   
   for (p=AMSTrRecHit::gethead(pla); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     hitr = p->getHit();
     if (hitr[1] == KLEFTY){
       tex_L=fabs(hitr[0]-P_0L2[0]);
       if (tex_L < d_xl) d_xl=tex_L;
     }
     if (hitr[1] == KRIGHTY){
       tex_R=fabs(hitr[0]-P_0R2[0]);
       if (tex_R < d_xr) d_xr=tex_R;
     }
   }
   // !DONE

   // now just to be sure
   for (p=AMSTrRecHit::gethead(pla); p!=NULL; p=p->next()){
     p->clearstatus(AMSDBc::GAMMARIGHT);
     p->clearstatus(AMSDBc::GAMMALEFT);
   }
   for (p=AMSTrRecHit::gethead(pla); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
    hitr = p->getHit();
      if ((hitr[1] == KLEFTY) &&
        fabs(hitr[0]-P_0L2[0]) == d_xl){
        p->setstatus(AMSDBc::GAMMALEFT);  
    }
    //
     if ((hitr[1] == KRIGHTY) &&
        fabs(hitr[0]-P_0R2[0]) == d_xr){
        p->setstatus(AMSDBc::GAMMARIGHT);
     }
   }
}

void AMSTrTrackGamma::addtracks(int & done){

_pntTrL =  new AMSTrTrack(_NhLeft, _PLeft, _FastFitDoneL, _GeaneFitDoneL, _Chi2FastFitL, _RidgidityL,  _ErrRidgidityL,  _ThetaL,  _PhiL,  _P0L, _GChi2L,  _GRidgidityL,  _GErrRidgidityL, _GThetaMSL, _GPhiMSL, _GP0MSL,_Chi2MSL,  _GChi2MSL,  _RidgidityMSL,  _GRidgidityMSL);

      _pntTrL->clearstatus(AMSDBc::GAMMARIGHT);
      _pntTrL->setstatus(AMSDBc::GAMMALEFT);
 // permanently add;
  AMSEvent::gethead()->addnext(AMSID("AMSTrTrack",0),_pntTrL);
  done++;

_pntTrR =  new AMSTrTrack(_NhRight, _PRight, _FastFitDoneR, _GeaneFitDoneR, _Chi2FastFitR, _RidgidityR,  _ErrRidgidityR,  _ThetaR,  _PhiR,  _P0R, _GChi2R,  _GRidgidityR,  _GErrRidgidityR, _GThetaMSR, _GPhiMSR, _GP0MSR,_Chi2MSR,  _GChi2MSR,  _RidgidityMSR,  _GRidgidityMSR);

    _pntTrR->clearstatus(AMSDBc::GAMMALEFT);
    _pntTrR->setstatus(AMSDBc::GAMMARIGHT);
 // permanently add;
  AMSEvent::gethead()->addnext(AMSID("AMSTrTrack",0),_pntTrR);
  done++;

}

void AMSTrTrackGamma::PAIR2GAMMA(int & counting, int & plusminus){
  // first of all the Juan fitting was called, then...
  counting++;
  double pmeno;
  double ppiu;
  double n_L[3];
  double n_R[3];
  double p3g[3];
  if (_GRidgidityMSR * _GRidgidityMSL < 0){
    pmeno=(_GRidgidityMSR<0)?_GRidgidityMSR:_GRidgidityMSL;
    ppiu=(_GRidgidityMSR<0)?_GRidgidityMSL:_GRidgidityMSR;
    plusminus=0;
  }
  if (_GRidgidityMSR * _GRidgidityMSL > 0){
    plusminus=1;
    pmeno=_GRidgidityMSR;
     ppiu= _GRidgidityMSL;
  } 
 if (_GRidgidityMSR * _GRidgidityMSL == 0){
    plusminus=2;
    pmeno=_GRidgidityMSR;
     ppiu= _GRidgidityMSL;
   }
  

 n_L[0]=sin(_GThetaMSL)*cos(_GPhiMSL);
 n_L[1]=sin(_GThetaMSL)*sin(_GPhiMSL);
 n_L[2]=-sqrt(1-pow(n_L[0],2)-pow(n_L[1],2));

 n_R[0]=sin(_GThetaMSR)*cos(_GPhiMSR);
 n_R[1]=sin(_GThetaMSR)*sin(_GPhiMSR);
 n_R[2]=-sqrt(1-pow(n_R[0],2)-pow(n_R[1],2));

 p3g[0]=(fabs(_GRidgidityMSR)*((sin(_GThetaMSR)*cos(_GPhiMSR)))) + (fabs(_GRidgidityMSL)*((sin(_GThetaMSL)*cos(_GPhiMSL))));       // =p1x+p2x
 p3g[1]=(fabs(_GRidgidityMSR)*((sin(_GThetaMSR)*sin(_GPhiMSR)))) + (fabs(_GRidgidityMSL)*((sin(_GThetaMSL)*sin(_GPhiMSL))));       // =p1y+p2y
 p3g[2]=fabs(_GRidgidityMSR)*n_R[2] + fabs(_GRidgidityMSL)*n_L[2]; // =p1z+p2z
_PGAMM=sqrt(pow(p3g[0],2) + pow(p3g[1],2) + pow(p3g[2],2));
_PhTheta=acos(p3g[2]/_PGAMM);
if(_PhTheta<AMSDBc::pi/2)_PhTheta=AMSDBc::pi-_PhTheta;
_PhPhi=atan2(p3g[1]/_PGAMM,p3g[0]/_PGAMM);
// Get opening angle of converting photons through the scalar product
 number cosd=0;
  cosd=n_R[0]*n_L[0]+n_R[1]*n_L[1]+n_R[2]*n_L[2]; 
  if(fabs(cosd) <= 1){
_Gacosd=acos(cosd);
  }
 // INVARIANT MASS        
_MGAM=sqrt(fabs(2*fabs(_GRidgidityMSR)*fabs(_GRidgidityMSL)*(1-cosd)));



//  changed by VC

  AMSDir RDir(_GThetaMSR,_GPhiMSR);
  AMSDir LDir(_GThetaMSL,_GPhiMSL);
  AMSPoint p3=RDir*fabs(_GRidgidityMSR)+LDir*fabs(_GRidgidityMSL);
 
  //    _PGAMM=p3.norm();
  //  _ErrPGAMM=sqrt(pow(_GErrRidgidityL*_GRidgidityMSL*_GRidgidityMSL,2)+pow(_GErrRidgidityR*_GRidgidityMSR*_GRidgidityMSR,2));
    //_PhTheta=AMSDir(p3).gettheta();
    //if(_PhTheta<AMSDBc::pi/2)_PhTheta=AMSDBc::pi-_PhTheta;
    //_PhPhi=AMSDir(p3).getphi();
    //number ca=RDir.prod(LDir);
    // _MGAM=sqrt(fabs(2*fabs(_GRidgidityMSR)*fabs(_GRidgidityMSL)*(1-ca)));
//cout << "phphi "<<_PhPhi<<" "<<_MGAM<<" "<<_PGAMM<<" "<<_ErrPGAMM<<endl;


// Get opening angle of converting photons through the scalar product
// number cosd=0;
 // cosd=n_R[0]*n_L[0]+n_R[1]*n_L[1]+n_R[2]*n_L[2]; 
 // if(fabs(cosd) <= 1){
 //_Gacosd=acos(cosd);
 // }
// _Gacosd=0;


// finally the vertex:
_MyVertex(n_L,n_R);


 if (_VE1[2] >= _VE2[2]) _Vertex=_VE1;
 else _Vertex=_VE2;


 //----
#ifdef __AMSDEBUG__
 cout << "_PGAMM = "<<_PGAMM<<endl;
 cout << "_PhTheta _PhPhi= "<<_PhTheta<<"  "<<_PhPhi<<endl;
 cout << "_Gacosd= "<<_Gacosd<<endl;
 cout << "_MGAM= "<<_MGAM<<endl;
#endif
}


void AMSTrTrackGamma::_MyVertex(double n_L[],double n_R[]){
  double P1[3];
  double P2[3];
  double t_1;
  double t_2;
  double DELTA[3];
  double BD2;
  double AD1;
  double C12;
  for(int i=0;i<3;i++){
    P1[i]=_GP0MSL[i];    
    P2[i]=_GP0MSR[i];    
 DELTA[i]=P1[i]-P2[i];
 }
  C12=(n_L[0]*n_R[0])+(n_L[1]*n_R[1])+(n_L[2]*n_R[2]);
  BD2=(DELTA[0]*n_R[0])+(DELTA[1]*n_R[1])+(DELTA[2]*n_R[2]);
  AD1=(DELTA[0]*n_L[0])+(DELTA[1]*n_L[1])+(DELTA[2]*n_L[2]);

  t_2=(BD2-(AD1*C12))/(1-pow(C12,2));
  t_1=t_2*C12-AD1;

  _VE1[0]=P1[0]+(t_1*n_L[0]);
  _VE1[1]=P1[1]+(t_1*n_L[1]);
  _VE1[2]=P1[2]+(t_1*n_L[2]);

  _VE2[0]=P2[0]+(t_2*n_R[0]);
  _VE2[1]=P2[1]+(t_2*n_R[1]);
  _VE2[2]=P2[2]+(t_2*n_R[2]);
 
}

double AMSTrTrackGamma::_vdmax(double v[], int size){
  double dmax=v[0];
  for (int i=0; i<size; i++){
    if (v[i]>dmax){
       dmax=v[i];
    }
  }
  return dmax;
}

double AMSTrTrackGamma::_vdmin(double v[], int size){
  double dmin=v[0];
  for (int i=0; i<size; i++){
    if (v[i]<dmin){
       dmin=v[i];
    }
  }
  return dmin;
}


void AMSTrTrackGamma::_crHitLR(){
  // A simplified version of crHit
  // alignment still missing
  for(int i=0;i<_NhRight;i++){
    for(int j=0;j<3;j++){
      _HRIGH[i][j]=_PRight[i]->getHit()[j];
     _EHRIGH[i][j]=_PRight[i]->getEHit()[j];
    }
  }
  for(int i=0;i<_NhLeft;i++){
    for(int j=0;j<3;j++){
       _HLEFT[i][j]=_PLeft[i]->getHit()[j];
       _EHLEFT[i][j]=_PLeft[i]->getEHit()[j];
    }
   }
}

number AMSTrTrackGamma::Fit(integer fits, integer ipart){
//  cout << "WE are INSIDE  Fit("<<fits<<","<<ipart<<") "<< endl;
  // fit =0  fit pattern
  //NO // fit =1  fit 1st part if pat=0,1,2,3, ... etc  
  //NO // fit =2  fit 2nd half if pat=0,1,2,3  ... etc + interpolate to beg of 1st
  // fit =3  Geanefit pattern
  // fit =4  fast fit with ims=0

// ! Changed
  // fit =5  Juans fit with ims=1


   // Create Proper Hit/Ehit things
    _crHitLR();

  // Protection from too low mass
    integer ipartR;
    integer ipartL;
    if(ipart==2){
     ipartL=ipart;
     ipartR=3;
    }
    if(ipart==3){
     ipartR=ipart;
     ipartL=2;
    }
    int npt[2];
    npt[0]=_NhRight;
    npt[1]=_NhLeft;
  const integer maxhits=10;
  assert(npt[0] < maxhits);
  assert(npt[1] < maxhits);
  static geant hitsR[maxhits][3];
  static geant sigmaR[maxhits][3];
  static geant normalR[maxhits][3];
  static integer layerR[maxhits];
  static geant hitsL[maxhits][3];
  static geant sigmaL[maxhits][3];
  static geant normalL[maxhits][3];
  static integer layerL[maxhits];
  integer ialgo=1; // ialgo ==1 oppure 3 se fit ==3 oppure 4 se fit ==5
  geant outR[9];
  geant outL[9];
  int i;
  int fit =abs(fits);
    if(fit==3 )ialgo=3;      
    else if(fit==5)ialgo=4; // ma fit==5 e` Juan !!!!!!!
    for(i=0;i<_NhRight;i++){
     normalR[i][0]=0;
     normalR[i][1]=0;
     normalR[i][2]=fits<0?1:-1;
    }
    for(i=0;i<_NhLeft;i++){
     normalL[i][0]=0;
     normalL[i][1]=0;
     normalL[i][2]=fits<0?1:-1;
    }

  if(fit == 0 || fit==3 || fit==4 || fit==5){
    //  NOW the default initialization for those variables which
    /*  will not be changed 
_AddressLR=0;
_PatternLR=0;
_AdvancedFitDoneLR=0;
_Chi2StrLineLR=0;
_Chi2CircleLR=0;
_CircleRidgidityLR=0;
 for(int j=0;j<2;j++){   
_DbaseLR[j]=0;
    // since the fit == 2 and fit == 1 is not used:
_HChi2LR[j]=-1;
_HRidgidityLR[j]=0;
_HErrRidgidityLR[j]=0;
_HThetaLR[j]=0;
_HPhiLR[j]=0;
    
 }
  for(int k=0;k<3;k++)_HP0LR[0][k]=0;
  for(int k=0;k<3;k++)_HP0LR[1][k]=0;
    */
    //
    //RIGHT
    for(i=0;i<_NhRight;i++){
     layerR[i]=_PRight[i]->getLayer();
     for(int j=0;j<3;j++){
      if (_PRight[i]) {
       hitsR[i][j]=getHright(i,fits<0)[j];
       sigmaR[i][j]=getEHright(i)[j];
      } else {
        cout <<"AMSTrTrack::Fit -W- _PRight["<<i<<"] = NULL, j"<<j<<endl;
      }
     }
    }
    //LEFT
    for(i=0;i<_NhLeft;i++){
     layerL[i]=_PLeft[i]->getLayer();
     for(int j=0;j<3;j++){
      if (_PLeft[i]) {
       hitsL[i][j]=getHleft(i,fits<0)[j];
       sigmaL[i][j]=getEHleft(i)[j];
      } else {
        cout <<"AMSTrTrack::Fit -W- _PLeft["<<i<<"] = NULL, j"<<j<<endl;
      }
     }
    }
    //
  }//0345
  
integer ims; 
if(fit==4)ims=0;
else ims=1;
// ipartR=3; //e-
TKFITG(npt[0],hitsR,sigmaR,normalR,ipartR,ialgo,ims,layerR,outR);
// ipartL=2; //e+
TKFITG(npt[1],hitsL,sigmaL,normalL,ipartL,ialgo,ims,layerL,outL);
if(fit==0){
_FastFitDoneL=1;
_Chi2FastFitL=outL[6];
if(outL[7] != 0)_Chi2FastFitL=FLT_MAX;
_RidgidityL=outL[5];
_ErrRidgidityL=outL[8];
_ThetaL=outL[3];
_PhiL=outL[4];
_P0L=AMSPoint(outL[0],outL[1],outL[2]);
//...
_FastFitDoneR=1;
_Chi2FastFitR=outR[6];
if(outR[7] != 0)_Chi2FastFitR=FLT_MAX;
_RidgidityR=outR[5];
_ErrRidgidityR=outR[8];
_ThetaR=outR[3];
_PhiR=outR[4];
_P0R=AMSPoint(outR[0],outR[1],outR[2]);
}
else if(fit==3){
_GeaneFitDoneR=ipartR;
_GChi2R=outR[6];
_GRidgidityR=outR[5];
// cout<< "Fit 3 ROUTINE outR[5] = "<<outR[5]<<endl;
_GErrRidgidityR=outR[8];
_GThetaR=outR[3];
_GPhiR=outR[4];
// cout << "Fit 3 ROUTINE theta and phi RIGHT= "<<outR[3]<< " " <<outR[4]<<endl;
_GP0R=AMSPoint(outR[0],outR[1],outR[2]);
if(outR[7] !=0 || _GChi2R>TRFITFFKEY.Chi2FastFit){
  _GeaneFitDoneR=0;
  if(outR[7]){
    _GChi2R=FLT_MAX;
    _GErrRidgidityR=FLT_MAX;
    _GRidgidityR=FLT_MAX;
  }
}  
//...
_GeaneFitDoneL=ipartL;
_GChi2L=outL[6];
_GRidgidityL=outL[5];
// cout<< "Fit 3 ROUTINE outL[5] = "<<outL[5]<<endl;
_GErrRidgidityL=outL[8];
_GThetaL=outL[3];
_GPhiL=outL[4];
// cout << "Fit 3 ROUTINE theta and phi LEFT= "<<outR[3]<< " " <<outR[4]<<endl;
_GP0L=AMSPoint(outL[0],outL[1],outL[2]);
if(outL[7] !=0 || _GChi2L>TRFITFFKEY.Chi2FastFit){
  _GeaneFitDoneL=0;
  if(outL[7]){
    _GChi2L=FLT_MAX;
    _GErrRidgidityL=FLT_MAX;
    _GRidgidityL=FLT_MAX;
  }
}  

}//fit 3
else if(fit==4){
_Chi2MSR=outR[6];
if(outR[7] != 0)_Chi2MSR=FLT_MAX;
_RidgidityMSR=outR[5];
// cout<< "Fit 4 ROUTINE outR[5] = "<<outR[5]<<endl;
// cout << "Fit 4 ROUTINE theta and phi RIGHT= "<<outR[3]<< " " <<outR[4]<<endl;
//...
_Chi2MSL=outL[6];
if(outL[7] != 0)_Chi2MSL=FLT_MAX;
_RidgidityMSL=outL[5];
// cout<< "Fit 4 ROUTINE outL[5] = "<<outL[5]<<endl;
// cout << "Fit 4 ROUTINE theta and phi LEFT= "<<outR[3]<< " " <<outR[4]<<endl;
}
 else if(fit==5){ //JUAN !!!!!!!!
_GChi2MSR=outR[6];  
if(outR[7]!=0)_GChi2MSR=FLT_MAX;
_GRidgidityMSR=outR[5];
// cout<< "JUAN ROUTINE outR[5] = "<<outR[5]<<endl;
// me
_GThetaMSR=outR[3];
_GPhiMSR=outR[4];
_GP0MSR=AMSPoint(outR[0],outR[1],outR[2]);
//...
_GChi2MSR=outR[6];  
if(outR[7]!=0)_GChi2MSR=FLT_MAX;
_GRidgidityMSL=outL[5];
// cout<< "JUAN ROUTINE outL[5] = "<<outL[5]<<endl;
// me
_GThetaMSL=outL[3];
_GPhiMSL=outL[4];
_GP0MSL=AMSPoint(outL[0],outL[1],outL[2]);
}

 if (_Chi2FastFitL == FLT_MAX || _Chi2FastFitR == FLT_MAX){
   _Chi2FastFitLR= FLT_MAX;
 }
 if (_Chi2FastFitL != FLT_MAX && _Chi2FastFitR != FLT_MAX){
   _Chi2FastFitLR= (_Chi2FastFitL + _Chi2FastFitR)/2;
 }
 
return _Chi2FastFitLR;
    //-
}









void AMSTrRecHit::_markDouble(vector<double>& vec_one, integer inhi){

 AMSTrRecHit * pre;
 AMSPoint p_hi;
 int Nuser=0;
 double temp,tao;
 int flua=0;
 int flub=0;
 int i=0;
  for (pre=AMSTrRecHit::gethead(inhi); pre!=NULL; pre=pre->next()){
     if(pre->checkstatus(AMSDBc::FalseX) || pre->checkstatus(AMSDBc::FalseTOFX))continue;
        p_hi = pre->getHit();
            if (flua==0){
	    temp=p_hi[1];
	    vec_one.push_back(temp);
            flua=1;
          }
          if (flua!=0){
            flub=0;
            for(int j=0;j<vec_one.size();j++){
               if (p_hi[1]==vec_one[j]){
		 flub=1;
		  }
               }               
             }
           if (flua!=0 && flub==0){
             temp=p_hi[1];
             vec_one.push_back(temp); 
          }  
  }
}



void AMSTrTrackGamma::dlinearme(int M, double v[], double w[], double & a0, double & a1, double & chi){

  //for DIGITAL ALPHAS
  //  double xx[M];
  //  double yy[M];
  double xx[100];
  double yy[100];
  
  for (int i=0; i<M; i++){
    xx[i]=v[i];
    yy[i]=w[i];
  }

  MYlfit lsf(M,xx,yy);
  lsf.make_a();
  lsf.make_b();
  lsf.make_VAR();
  a0=lsf._getA(); //Intercept
  a1=lsf._getB();//SLOPE
  chi=lsf._getVAR();//Goodness 

}

void AMSTrTrackGamma::dlsqp2me(int M, double v[], double w[], double & a0, double & a1, double & a2, double & chi){

  //for DIGITAL ALPHAS
  //  double xx[M];
  //  double yy[M];
  double xx[100];
  double yy[100];

  
  for (int i=0; i<M; i++){
    xx[i]=v[i];
    yy[i]=w[i];
  }
    MYlsqp2 pl2(M,xx,yy);
    pl2.make_param();
    pl2.make_c();
    pl2.make_b();
    pl2.make_a();
    pl2.make_SDW();
    a0=pl2._getA(); 
    a1=pl2._getB();
    a2=pl2._getC();
    chi=pl2._getSDW();

    if (fabs(a2) > 10000000){
      a0=0;
      a1=0;
      a2=0;
     chi=10000;
    }
}


double MYlsqp2::sigma(double* r){
  double sum1=0.0;
  for(int i=0;i<L;++i){
    sum1 += r[i];
  }
  return(sum1);
}

double MYlsqp2::sigma(double* r, double* s){
  double sumxy=0.0;
  for(int i=0;i<L;++i){
    sumxy += r[i]*s[i];
  }
  return(sumxy);
}

double MYlsqp2::sigma(double* r, double* s, double* t){
  double sum3=0.0;
  for(int i=0;i<L;++i){
    sum3 += r[i]*s[i]*t[i];
  }
  return(sum3);
}
double MYlsqp2::sigma(double* r, double* s, double* t, double* u){
  double sum4=0.0;
  for(int i=0;i<L;++i){
    sum4 += r[i]*s[i]*t[i]*u[i];
  }
  return(sum4);
}




MYlsqp2::MYlsqp2(int M, double* xp, double* yp){
  L=M;
  x=xp;
  y=yp;
}

void MYlsqp2::make_param(){
//  cout<< "Make_nij L "<< L <<endl;  
  n1[0]=L;
  n1[1]=sigma(x);
  n1[2]=sigma(x,x);
  n2[0]=sigma(x);
  n2[1]=sigma(x,x);
  n2[2]=sigma(x,x,x);
  n3[0]=sigma(x,x);
  n3[1]=sigma(x,x,x);
  n3[2]=sigma(x,x,x,x);
  n4[0]=sigma(y);
  n4[1]=sigma(x,y);
  n4[2]=sigma(x,x,y);
  //
  al=n1[1]*n1[2];
  be=n1[0]*n1[2];
  ga=n1[0]*n1[1];
  //
  eps=-2*al*n3[0]+be*n3[1]+ga*n3[2];
  bi=-2*al*n2[0]+be*n2[1]+ga*n2[2];
  rho=-2*al*n4[0]+be*n4[1]+ga*n4[2];
  //
}




void MYlsqp2::make_c(){
  double nc,dc;
  nc=n4[1]-(n4[0]*n1[1]/n1[0])+((rho*n2[0]*n1[1])/(bi*n1[0]))-(rho*n2[1]/bi);
  dc=((eps*n2[0]*n1[1])/(bi*n1[0]))-(n3[0]*n1[1]/n1[0])+n3[1]-(eps*n2[1]/bi);
  c=nc/dc;
}

void MYlsqp2::make_b(){
  b=(rho-c*eps)/bi;
}

void MYlsqp2::make_a(){
  a=(n4[0]-b*n2[0]-c*n3[0])/n1[0];
}

void MYlsqp2::make_SDW(){
  double sum=0.0;
  for(int i=0;i<L;++i){
    sum += pow((y[i]-a-(b*x[i])-(c*x[i]*x[i])),2);
  }
  if (L  > 3) sdw=sqrt(sum/(L-3));
  if (L == 3) sdw=sqrt(sum/(L-1));
  if (L == 2) sdw=sqrt(sum/(L-1));
  if (L < 2)  sdw=0;

}


double MYlsqp2::_getA(){
  return(a);
};

double MYlsqp2::_getB(){
  return(b);
};

double MYlsqp2::_getC(){
  return(c);
};

double MYlsqp2::_getSDW(){
  return(sdw);
};

////////////////////////*

double MYlfit::sigma(double* r){
  double sum=0.0;
  for(int i=0;i<L;++i){
    sum += r[i];
  }
  return(sum);
}

double MYlfit::sigma(double* r, double* s){
  double sum=0.0;
  for(int i=0;i<L;++i){
    sum += r[i]*s[i];
  }
  return(sum);
}


void MYlfit::make_VAR(){
  double sum=0.0;
  for(int i=0;i<L;++i){
    sum += pow((y[i]-a-(b*x[i])),2);
  }
  VAR=sum/(L-1);
}

void MYlfit::make_a(){
  delta=L*sigma(x,x)-pow(sigma(x),2.0);
  a=(1.0/delta)*(sigma(x,x)*sigma(y)-sigma(x)*sigma(x,y));
}

void MYlfit::make_b(){
  b=(1.0/delta)*(L*sigma(x,y)-sigma(x)*sigma(y));
}

void MYlfit::display(){
  cout << "Slope is " << b<<endl;
  cout << "Y-intercept is "<< a<< endl;
}

double MYlfit::_getA(){
  return(a);
};

double MYlfit::_getB(){
  return(b);
};

double MYlfit::_getVAR(){
  return(VAR);
};

MYlfit::MYlfit(int M, double* xp, double* yp){
  L=M;
  x=xp;
  y=yp;
#ifdef __AMSDEBUG__
   cout<< "MYlfit L "<< L <<endl; 
   for (int ig=0;ig<L;ig++){
     cout<< "MYlfit y["<<ig<<"] "<<y[ig]<<endl; 
     cout<< "MYlfit x["<<ig<<"] "<<x[ig]<<endl; 
   }
#endif
}

XZLine_TOF::XZLine_TOF(int* mul, int* nm, double& tm34, number& ecc){ 
 
  double tm;
  number ec;

getTofMul(0,mul,tm);
getEcalMul(0,nm,ec);
 
    NN=nm;
    LL=mul;
    tm34=tm;
    ecc=ec;
#ifdef __AMSDEBUG__
    cout<<  "XZLine_TOF     NN    "<<NN[0]<<" "<<NN[1]<<" "<<NN[2]  <<endl;
    cout << "XZLine_TOF     LL[0] "<<LL[0]<<" LL[1] "<<LL[1]<<" LL[2] "<<LL[2]<< endl;
    cout << "               LL[3] "<<LL[3]<<" LL[4] "<<LL[4]<<endl; 
    cout << "tm34  "<<tm34<<endl;
    cout << "ecc  "<<ecc<<endl;
#endif
}

void XZLine_TOF::LastCheckTRDX(double& slotrd, double& intertrd, double& chitrd){

  double AF,BF,VARR;
  int ntrdj=0;
 AMSPoint cotr;
 int L=-1;
 double ztrd[20];
 double xtrd[20];
 double ytrd[20];
 int cltrd[20]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,};
for(int i=0;i<trdconst::maxlay;i++){
  ntrdj+=(AMSEvent::gethead()->getC("AMSTRDCluster",i))->getnelem();
  for (AMSTRDCluster * ptrd=AMSTRDCluster::gethead(i); ptrd!=NULL; ptrd=ptrd->next()){
    cltrd[i]++;
    cotr=ptrd->getCoo();
  }
}

for(int i=0;i<trdconst::maxlay;i++){
  for (AMSTRDCluster * ptrd=AMSTRDCluster::gethead(i); ptrd!=NULL; ptrd=ptrd->next()){
    cotr=ptrd->getCoo();    
    if (cotr[2] >= 96.875 && cotr[2]<=128.775){
      if (cltrd[i] <= 1){
	L++;
	if (L < 20){
	  ztrd[L]=cotr[2];
	  xtrd[L]=cotr[0];
	}
      }
    }
  }
}

//for(int i=0;i<20;i++){
// cout <<" cltrd["<<i<<"]" <<cltrd[i]<<endl;
//}
// cout <<" in trd L= " <<L<<endl;
 L=L+1;
 if (L<=20){
  AMSTrTrackGamma::dlinearme(L,ztrd,xtrd,AF,BF,VARR);         // linear fit
 slotrd=BF;
 intertrd=AF;
 chitrd=VARR;
 }

 if (L <= 3) chitrd=10000;

}




void XZLine_TOF::Lines_Top_Bottom(int& out){

  double DM34,DM14,DM24,DM12;
  int lb13,lb14,lb11,lb23,lb24,lb1,lb2;
  double DI44,DI24,DI12,DI34;
  
  double MX2,QX2,MY2,QY2;
  double MX1,QX1,MY1,QY1;

  AMSTOFCluster * ploop;
  AMSTOFCluster * ploop3;
  AMSTOFCluster * ploop4;
  int state;
  number p1[3];
  number p2[3];
  number p3[3];
  number p4[3];
  vector<double> M1,M13,M14;
  vector<double> Q1,Q13,Q14;
  vector<double> x1t,x1b,x13t,x14t,x13b,x14b;
  vector<double> z1t,z1b,z13t,z14t,z13b,z14b;
  vector<double> M2,M23,M24;
  vector<double> Q2,Q23,Q24;
  vector<double> x2b,x2t,x23t,x24t,x23b,x24b;
  vector<double> z2b,z2t,z23t,z24t,z23b,z24b;

  if (LL[0] >= 1){
    for (ploop = AMSTOFCluster::gethead(0); ploop ; ploop=ploop->next() ){
      state=ploop->checkstatus(AMSDBc::TOFFORGAMMA); 
      if (state){
	p1[0] = ploop->getcoo()[0];
	p1[1] = ploop->getcoo()[1];
	p1[2] = ploop->getcoo()[2];         
	//1st plane vs 3rd plane
	if (LL[2] >= 1){
	  for (ploop3 = AMSTOFCluster::gethead(2); ploop3 ; ploop3=ploop3->next() ){
	    state=ploop3->checkstatus(AMSDBc::TOFFORGAMMA); 
	    if (state){
	      p3[0] = ploop3->getcoo()[0];
	      p3[1] = ploop3->getcoo()[1];
	      p3[2] = ploop3->getcoo()[2];         
	      intercept(p1[0],p1[1],p1[2],p3[0],p3[1],p3[2],MX1,QX1,MY1,QY1);
	      M1.push_back(MX1); 
	      Q1.push_back(QX1);
	      x1t.push_back(p1[0]);
	      z1t.push_back(p1[2]);
	      x1b.push_back(p3[0]);
	      z1b.push_back(p3[2]);
	      //
	      M13.push_back(MX1); 
	      Q13.push_back(QX1);
	      x13t.push_back(p1[0]);
	      z13t.push_back(p1[2]);
	      x13b.push_back(p3[0]);
	      z13b.push_back(p3[2]);
              //
	    }
	  }
	}
	// 1st plane vs 4th plane
	if (LL[3] >= 1){
	  for (ploop4 = AMSTOFCluster::gethead(3); ploop4 ; ploop4=ploop4->next() ){
	    state=ploop4->checkstatus(AMSDBc::TOFFORGAMMA); 
	    if (state){
	      p4[0] = ploop4->getcoo()[0];
	      p4[1] = ploop4->getcoo()[1];
	      p4[2] = ploop4->getcoo()[2];         
	      intercept(p1[0],p1[1],p1[2],p4[0],p4[1],p4[2],MX1,QX1,MY1,QY1);
	      M1.push_back(MX1); 
	      Q1.push_back(QX1);
	      x1t.push_back(p1[0]);
	      z1t.push_back(p1[2]);
	      x1b.push_back(p4[0]);
	      z1b.push_back(p4[2]);
	      //
	      M14.push_back(MX1); 
	      Q14.push_back(QX1);
	      x14t.push_back(p1[0]);
	      z14t.push_back(p1[2]);
	      x14b.push_back(p4[0]);
	      z14b.push_back(p4[2]);
              //
	    }
	  }
	}
	//
      }
    }
  }
  if (LL[1] >= 1){
    for (ploop = AMSTOFCluster::gethead(1); ploop ; ploop=ploop->next() ){
      state=ploop->checkstatus(AMSDBc::TOFFORGAMMA); 
      if (state){
	p2[0] = ploop->getcoo()[0];
	p2[1] = ploop->getcoo()[1];
	p2[2] = ploop->getcoo()[2];         
	//2nd plane vs 3rd plane
	if (LL[2] >= 1){
	  for (ploop3 = AMSTOFCluster::gethead(2); ploop3 ; ploop3=ploop3->next() ){
	    state=ploop3->checkstatus(AMSDBc::TOFFORGAMMA); 
	    if (state){
	      p3[0] = ploop3->getcoo()[0];
	      p3[1] = ploop3->getcoo()[1];
	      p3[2] = ploop3->getcoo()[2];         
	      intercept(p2[0],p2[1],p2[2],p3[0],p3[1],p3[2],MX2,QX2,MY2,QY2);
	      M2.push_back(MX2); 
	      Q2.push_back(QX2);
	      x2t.push_back(p2[0]);
	      z2t.push_back(p2[2]);
	      x2b.push_back(p3[0]);
	      z2b.push_back(p3[2]);
	      //
	      M23.push_back(MX2); 
	      Q23.push_back(QX2);
	      x23t.push_back(p2[0]);
	      z23t.push_back(p2[2]);
	      x23b.push_back(p3[0]);
	      z23b.push_back(p3[2]);
	      //
	    }
	  }
	}
	// 2nd plane vs 4th plane
	if (LL[3] >= 1){
	  for (ploop4 = AMSTOFCluster::gethead(3); ploop4 ; ploop4=ploop4->next() ){
	    state=ploop4->checkstatus(AMSDBc::TOFFORGAMMA); 
	    if (state){
	      p4[0] = ploop4->getcoo()[0];
	      p4[1] = ploop4->getcoo()[1];
	      p4[2] = ploop4->getcoo()[2];         
	      intercept(p2[0],p2[1],p2[2],p4[0],p4[1],p4[2],MX2,QX2,MY2,QY2);
	      M2.push_back(MX2); 
	      Q2.push_back(QX2);
	      x2t.push_back(p2[0]);
	      z2t.push_back(p2[2]);
	      x2b.push_back(p4[0]);
	      z2b.push_back(p4[2]);
	      //
	      M24.push_back(MX2); 
	      Q24.push_back(QX2);
	      x24t.push_back(p2[0]);
	      z24t.push_back(p2[2]);
	      x24b.push_back(p4[0]);
	      z24b.push_back(p4[2]);
	      //	
           }
	  }
	}
	//
      }
    }
  }

  ///////////
  if (LL[0] >= 1 && LL[1] >= 1 ){
  lb1=0;
  lb2=0;
  DM12=10000;
  for (int i=0;i<M1.size();i++){
   for (int j=0;j<M2.size();j++){
     DI12=fabs((M1[i])-(M2[j]));
     if (DI12 < DM12){
       DM12=DI12;
       lb1=i;
       lb2=j;
     }
   }
  }
   out=1;
      fbot=0;
      ftop=1;
      x_star=x1t[lb1];
      z_star=z1t[lb1];
      SLOPE=M1[lb1];
      INTER=Q1[lb1];
      if (fabs(x2t[lb2]-(M1[lb1]*z2t[lb2]+Q1[lb1])) > 3 || 
          fabs(M1[lb1]-((x2t[lb2]-x1t[lb1])/(z2t[lb2]-z1t[lb1]))) > 0.25){
        fbot=1;
        ftop=0;
        x_star=x1b[lb1];
        z_star=z1b[lb1];
        SLOPE=M1[lb1];
        INTER=Q1[lb1];
      }


#ifdef __AMSDEBUG__
 cout << "Lines_Top_Bottom: fbot = "<<fbot <<" ftop = "<<ftop<<endl;
 cout << "x_star z_star= "<<x_star<<" "<<z_star<<endl;
 cout << "SLOPE INTER = "<<SLOPE<<" "<<INTER<<endl;
#endif
  }
  //-----  
  //Now OPTION 2
  if (LL[1] ==0 && LL[0] != 0){
   if (LL[0] == 1 && LL[3] !=0 ){
     //*
     if (LL[2] !=0){
     DM34=10000;
     lb13=0;
     lb14=0;
     for (int i=0;i<M13.size();i++){
       for (int j=0;j<M14.size();j++){
        DI34=fabs((M13[i])-(M14[j]));
        if (DI34 < DM34){
          DM34=DI34;
          lb13=i;
          lb14=j;
	}
       }
     }
     } //LL[2] !=0
     //**
     if (LL[2] ==1){
   out=1;
      fbot=0;
      ftop=1;
      x_star=x14t[lb14];
      z_star=z14t[lb14];
      SLOPE=M14[lb14];
      INTER=Q14[lb14];
      if (fabs(x14b[lb14] - (M14[lb14]*z14t[lb14]+Q14[lb14])) > 3){
      fbot=1;
      ftop=0;
      x_star=x14b[lb14];
      z_star=z14b[lb14];
      }
     }
     //**
    if (LL[2] >1 || LL[2] ==0){
      if (LL[3] ==1){
    out=1;
      fbot=0;
      ftop=1;
      // Because of only 1 hit on plane 1 and only one hit ion plane 4 the vectors have dimension 1
      x_star=x14t[0];
      z_star=z14t[0];
      SLOPE=M14[0];
      INTER=Q14[0]; 
      }
      if (LL[3] >1){
        DM14=10000;
        lb14=0;
        lb11=0;
     for (int i=0;i<(M14.size()-1);i++){
       for (int j=(i+1);j<M14.size();j++){
        DI44=fabs((M14[i])-(M14[j]));
        if (DI44 < DM14){
          DM14=DI44;
          lb11=i;
          lb14=j;
	}
       }
     }
    out=1;
      fbot=0;
      ftop=1;
      x_star=x14t[0];
      z_star=z14t[0];
     if (fabs(x14b[lb11] - (M14[lb11]*z14t[lb11]+Q14[lb11])) <=
         fabs(x14b[lb14] - (M14[lb14]*z14t[lb14]+Q14[lb14])) ){
        SLOPE=M14[lb11];
        INTER=Q14[lb11]; 
     }
     if (fabs(x14b[lb11] - (M14[lb11]*z14t[lb11]+Q14[lb11])) >
         fabs(x14b[lb14] - (M14[lb14]*z14t[lb14]+Q14[lb14])) ){
        SLOPE=M14[lb14];
        INTER=Q14[lb14]; 
     }

      } //if (LL[3] >1){
    } //if (LL[2] >1 || LL[2] ==0){
     //**
   }
  }
  //_________________------------------__________________--------------___
  //Now OPTION 3
 if (LL[0] ==0 && LL[1] != 0){
   if (LL[1] == 1 && LL[3] !=0){
     //*
     if (LL[2] !=0){
       DM34=10000;
       lb23=0;
       lb24=0;
     for (int i=0;i<M23.size();i++){
       for (int j=0;j<M24.size();j++){
        DI24=fabs((M23[i])-(M24[j]));
        if (DI24 < DM34){
          DM34=DI24;
          lb23=i;
          lb24=j;
	}
       }
     }
     } //LL[2] !=0
     //**
     if (LL[2] ==1){
   out=1;
      fbot=0;
      ftop=1;
      x_star=x24t[lb24];
      z_star=z24t[lb24];
      SLOPE=M24[lb24];
      INTER=Q24[lb24];
      if (fabs(x24b[lb24] - (M24[lb24]*z24t[lb24]+Q24[lb24])) > 3){
      fbot=1;
      ftop=0;
      x_star=x24b[lb24];
      z_star=z24b[lb24];
      }
     }
     //**
    if (LL[2] >1 || LL[2] ==0){
      if (LL[3] ==1){
    out=1;
      fbot=0;
      ftop=1;
      // Because of only 1 hit on plane 2 and only one hit ion plane 4 the vectors have dimension 1
      x_star=x24t[0];
      z_star=z24t[0];
      SLOPE=M24[0];
      INTER=Q24[0]; 
      }
      if (LL[3] >1){
        DM24=10000;
        lb14=0;
        lb11=0;
     for (int i=0;i<(M24.size()-1);i++){
       for (int j=(i+1);j<M24.size();j++){
        DI44=fabs((M24[i])-(M24[j]));
        if (DI44 < DM24){
          DM24=DI44;
          lb11=i;
          lb14=j;
	}
       }
     }
    out=1;
      fbot=0;
      ftop=1;
      x_star=x24t[0];
      z_star=z24t[0];
     if (fabs(x24b[lb11] - (M24[lb11]*z24t[lb11]+Q24[lb11])) <=
         fabs(x24b[lb14] - (M24[lb14]*z24t[lb14]+Q24[lb14])) ){
        SLOPE=M24[lb11];
        INTER=Q24[lb11]; 
     }
     if (fabs(x24b[lb11] - (M24[lb11]*z24t[lb11]+Q24[lb11])) >
         fabs(x24b[lb14] - (M24[lb14]*z24t[lb14]+Q24[lb14])) ){
        SLOPE=M24[lb14];
        INTER=Q24[lb14]; 
     }

      } //if (LL[3] >1){
    } //if (LL[2] >1 || LL[2] ==0){
   //**
   } //if (LL[0] ==0 && LL[1] != 0 && LL[2] !=0
 }//if (LL[1] == 1 && LL[3] !=0)


}



void XZLine_TOF::getParRoadXZ(int& bott, int& topp, double& x_ss, double& z_ss, double& SLL, double& INTT)const
{bott=fbot;topp=ftop;x_ss=x_star;z_ss=z_star;SLL=SLOPE;INTT=INTER;}

void XZLine_TOF::makeEC_out(number& ecc, int& out){
  double MX,QX,MY,QY;
  int iflag=1;
  //ECAL
  double xen,yen,zen;
  double xex,yex,zex;
  int stau;
  int sta;
  

  EcalShower * psh=(EcalShower*)AMSEvent::gethead()->getheadC("EcalShower",0,2); 
  while(psh){
    xen=psh->getEntryPoint()[0];
    yen=psh->getEntryPoint()[1];
    zen=psh->getEntryPoint()[2];
    xex=psh->getExitPoint()[0];
    yex=psh->getExitPoint()[1];
    zex=psh->getExitPoint()[2];

    stau=psh->getstatus();
    sta=(stau/33554432)-(((stau/33554432)/2)*2);

    if (NN[0] != 0) {
    if (sta != 0 ) iflag=0;
    if (fabs(xex) >= 32 || fabs(xen) >= 30 || zen > -140 || ecc <= 40) iflag=0;
    if ((fabs(yex) >= 30 || fabs(yen) >= 30 ) &&  ecc <= 80) iflag=0;
    if ((fabs(xex) < 32 && fabs(xen) < 30) && (fabs(yex) >= 30 || fabs(yen) >= 30) && ecc > 80) iflag=1;
    if (iflag == 1){
       
      //
intercept(xen,yen,zen,xex,yex,zex,MX,QX,MY,QY);
//
      out=1;
      fbot=1;
      ftop=0;
      x_star=MX*64.5+QX;
      z_star=64.5;
      SLOPE=MX;
      INTER=QX;
    }
    }
    psh=psh->next();
  }

}



void XZLine_TOF::getEcalMul(int times,int nn[],number& ecc){

  const integer Maxrow=ecalconst::ECSLMX*2;

  for(int i=0;i<Maxrow;i++){
    AMSEcalHit * ptr=(AMSEcalHit*)AMSEvent::gethead()->getheadC("AMSEcalHit",i);
    while(ptr){
      nn[0]++;
      ptr=ptr->next();
    }
  }

  Ecal2DCluster * p2=(Ecal2DCluster*)AMSEvent::gethead()->getheadC("Ecal2DCluster",0,2);
  while(p2){
    nn[1]++;
    if (times == 0){
    number ec2ene=p2->getEnergy();
    ecc=ecc+ec2ene;
    }
    p2=p2->next();
  }
  if (times == 0 && nn[1] != 0){
    ecc=ecc/1000;    
  }


  EcalShower * psh=(EcalShower*)AMSEvent::gethead()->getheadC("EcalShower",0,2); 
   while(psh){
    nn[2]++;			     
    psh=psh->next();
   }

}

void XZLine_TOF::getTofMul(int times,int mal[],double& tao){
   int state;
   AMSTOFCluster * ploop;  
   double temp=10000;

   for(int il=0;il<5;il++){
    mal[il]=0;
   }

   for(int il=0;il<4;il++){    
    for (ploop = AMSTOFCluster::gethead(il); ploop ; ploop=ploop->next() ){
      if (times == 0){
        ploop->clearstatus(AMSDBc::TOFFORGAMMA);
        ploop->setstatus(AMSDBc::TOFFORGAMMA); // Let's start putting all of them at 1

      }

      state=ploop->checkstatus(AMSDBc::TOFFORGAMMA); 
      if (state)mal[il]++;
    }
   }
   for(int il=2;il<4;il++){
    for (ploop = AMSTOFCluster::gethead(il); ploop ; ploop=ploop->next() ){
    if (times == 0){
        ploop->clearstatus(AMSDBc::TOFFORGAMMA);
        ploop->setstatus(AMSDBc::TOFFORGAMMA); // Let's start putting all of them at 1
      }
      state=ploop->checkstatus(AMSDBc::TOFFORGAMMA); 
      if (state)mal[4]++;
      if (fabs(ploop->gettime()) < fabs(temp)) temp=ploop->gettime();

    }
   }
   tao=temp;

 }




void XZLine_TOF::TopSplash(double& tm34){

 int state,sta;
 double t_top;
 double DE_tb;
 double TB_const=0.06E-07;
 double E34_const=1.4;

  AMSTOFCluster * ploop;
  AMSTOFCluster * ploop2;
  for (int il=0;il<2;il++){
   if (LL[il] > 1 && NN[0] !=0){
    for (ploop = AMSTOFCluster::gethead(il); ploop ; ploop=ploop->next() ){
      if (fabs(ploop->gettime()) < fabs(tm34)){
        ploop->clearstatus(AMSDBc::TOFFORGAMMA);
      }
      if (fabs(ploop->gettime()) >= fabs(tm34)){
        ploop->setstatus(AMSDBc::TOFFORGAMMA);
      }  
    }
   }
  }
 //
  
  for (int il=0;il<2;il++){
    for (ploop = AMSTOFCluster::gethead(il); ploop ; ploop=ploop->next() ){
      state=ploop->checkstatus(AMSDBc::TOFFORGAMMA);
      t_top=ploop->gettime();   
      if (state){
	for (int jl=2;jl<4;jl++){
	  for (ploop2 = AMSTOFCluster::gethead(jl); ploop2 ; ploop2=ploop2->next() ){
	    DE_tb=ploop2->gettime();
	    if (DE_tb > TB_const){
              ploop2->clearstatus(AMSDBc::TOFFORGAMMA);
	    }
	    if (DE_tb <= TB_const && DE_tb > 0){
               ploop2->setstatus(AMSDBc::TOFFORGAMMA);

	    }
	  }
	}
      }
    }
  }
//plane 4
  for (ploop2 = AMSTOFCluster::gethead(3); ploop2 ; ploop2=ploop2->next() ){
   if (LL[2] !=0 && LL[3] > 2 && NN[0] !=0){
     if (ploop2->getedep() < E34_const){
      ploop2->clearstatus(AMSDBc::TOFFORGAMMA);
     }
      if (ploop2->getedep() >= E34_const){
      ploop2->setstatus(AMSDBc::TOFFORGAMMA);
      }
   }
  }
  //
}
  
  

   

void XZLine_TOF::Check_TRD_TK1(int Num, vector<double> HH, int jj[]){
  int ntrdj=0;

  //TRD segment on last three tubes
AMSPoint cotr;
 int cltrd[20]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,};
for(int i=0;i<trdconst::maxlay;i++){
 ntrdj+=(AMSEvent::gethead()->getC("AMSTRDCluster",i))->getnelem();
  for (AMSTRDCluster * ptrd=AMSTRDCluster::gethead(i); ptrd!=NULL; ptrd=ptrd->next()){
    cltrd[i]++;
  }
}

 double yf[3],zf[3];

if ( cltrd[0] == 0 && cltrd[1] == 0 && cltrd[2] == 0){
  for(int i=0;i<3;i++){
  for (AMSTRDCluster * ptrd=AMSTRDCluster::gethead(i); ptrd!=NULL; ptrd=ptrd->next()){
    cotr=ptrd->getCoo();
    yf[i]=cotr[1];  
    zf[i]=cotr[2];
  }
}

double A,B,VAR; 
AMSTrTrackGamma::dlinearme(3,zf,yf,A,B,VAR);         // linear fit
// NOW we remove the TOF hits too far from the trd prediction:
 double de=10000;
 double vtotd[2]={10000,10000};
 double can[2]; 
 number x,y,z;
 AMSTOFCluster * ploop;

 for (int j=0;j<2;j++){ //first and second plane
   if (jj[j] > 1){ // more then 1 single tof hit pre plane
    for (ploop = AMSTOFCluster::gethead(j); ploop ; ploop=ploop->next() ){
      x = ploop->getcoo()[0];
      y = ploop->getcoo()[1];
      z = ploop->getcoo()[2];
      vtotd[j]=fabs(y-(z*B-A));
      if (vtotd[j] != 10000 && vtotd[j] < de){
	de=vtotd[j];
        can[j]=y;
      }
    }
  }
 }
 for (int j=0;j<2;j++){ //first and second plane
   if (jj[j] > 1){ // more then 1 single tof hit pre plane
     if (can[j] != 10000){
       for (ploop = AMSTOFCluster::gethead(j); ploop ; ploop=ploop->next() ){
         x = ploop->getcoo()[0];
         y = ploop->getcoo()[1];
         z = ploop->getcoo()[2];
         if (fabs(y-can[j]) >= 1){
               ploop->clearstatus(AMSDBc::TOFFORGAMMA);
          }
          if (fabs(y-can[j]) < 1){
               ploop->setstatus(AMSDBc::TOFFORGAMMA);
          }
       }
     }
   }
 }
}
 //////////////////

 AMSTOFCluster * ploop;
  int m=10000;
  double dF_KT;
  double dFm=10000;
  double y;
 
  if (jj[0] > 1 && jj[1] == 0) m=0;
  if (jj[1] > 1 && jj[0] == 0) m=1;
 
 //
    if (m == 1 || m == 0) {   
     for (ploop = AMSTOFCluster::gethead(m); ploop ; ploop=ploop->next() ){
      y = ploop->getcoo()[1];
       if (Num > 1){
        for (int l=0;l<Num;l++){
	  dF_KT=fabs(y-HH[l]); 
          if (dF_KT < dFm){
	    dFm=dF_KT;
	  }
        }
       }
       if (Num == 1){
         dF_KT=fabs(y-HH[0]);
         if (dF_KT < dFm){
          dFm=dF_KT;
         }
       }
     }

     for (ploop = AMSTOFCluster::gethead(m); ploop ; ploop=ploop->next() ){
      y = ploop->getcoo()[1];
       if (Num >= 1){
        for (int l=0;l<Num;l++){
          if (fabs(y-HH[l]) == dFm){
          ploop->setstatus(AMSDBc::TOFFORGAMMA);
          }
          if (fabs(y-HH[l]) != dFm){
          ploop->clearstatus(AMSDBc::TOFFORGAMMA);
          }
         }
         }
     }
    }


 //


}





void AMSTrTrackGamma::_ConstructGamma(){


//  Find if there are some tracks already fitted

  
    AMSBeta *_pbeta=0;
  for(int patb=0; patb<npatb; patb++){
    AMSBeta *pbeta=(AMSBeta*)AMSEvent::gethead()->getheadC("AMSBeta",patb);
    while(pbeta){
      if(pbeta->getptrack()->checkstatus(AMSDBc::GAMMALEFT)){
        _pbeta=pbeta;
        break;
      }
      else if(pbeta->getptrack()->checkstatus(AMSDBc::GAMMARIGHT)){
        _pbeta=pbeta;
        break;
      }
      pbeta=pbeta->next();
    }
    if(_pbeta)break;
  }
  if(!_pbeta){
  for(int patb=0; patb<npatb; patb++){
    AMSBeta *pbeta=(AMSBeta*)AMSEvent::gethead()->getheadC("AMSBeta",patb);
    while(pbeta){
      if(!pbeta->getptrack()->checkstatus(AMSDBc::NOTRACK) &&
         !pbeta->getptrack()->checkstatus(AMSDBc::ECALTRACK) &&
         !pbeta->getptrack()->checkstatus(AMSDBc::TRDTRACK)){
        _pbeta=pbeta;
         break;
      }
       pbeta=pbeta->next();
    }
    if(_pbeta)break;
  }
  }
    _Charge=(_pntTrL->getrid()>0?1:-1)+(_pntTrR->getrid()>0?1:-1);
    cout <<" init charge "<<_Charge<<endl;
  if(_pbeta){
    _pbeta->getptrack()->AdvancedFit();
    if(_pbeta->getptrack()->getrid()*_pntTrL->getrid()>0)_pntTrL=_pbeta->getptrack();
    else if(_pbeta->getptrack()->getrid()*_pntTrR->getrid()>0)_pntTrR=_pbeta->getptrack();
  }

   if((_pntTrL->FastFitDone() && _pntTrR->FastFitDone()) ){

//  First  interpolate to zero order vertex guess  

   integer lay_l=min(_pntTrL->getphit(0)->getLayer(),_pntTrL->getphit(_pntTrL->getnhits()-1)->getLayer())-1;
   integer lay_r=min(_pntTrR->getphit(0)->getLayer(),_pntTrR->getphit(_pntTrR->getnhits()-1)->getLayer())-1;
   integer lay=min(lay_l,lay_r);

{
  AMSDir dirL(_pntTrL->gettheta(2),_pntTrL->getphi(2));
  AMSDir dirR(_pntTrR->gettheta(2),_pntTrR->getphi(2));
  if (dirL[2]>0)dirL=dirL*(-1);
  if (dirR[2]>0)dirR=dirR*(-1);
    number ca=dirL.prod(dirR);
    _MGAM=sqrt(fabs(2*fabs(_pntTrR->getpirid())*fabs(_pntTrL->getpirid())*(1-ca)));
    if(ca>1)ca=1;
    cout <<"very very initial ca "<<180/3.1415926*acos(ca)<<" "<<_MGAM<<endl;

    AMSmceventg *pmcg=(AMSmceventg*)AMSEvent::gethead()->getheadC("AMSmceventg",0);
    if(pmcg){
     AMSPoint pge=dirR*fabs(_pntTrR->getpirid())+dirL*fabs(_pntTrL->getpirid());
     AMSTrTrack *ptr=lay_l<lay_r?_pntTrL:(lay_r<lay_l?_pntTrR:0);
     if(ptr  ){
      AMSPoint pge2=AMSDir(ptr->gettheta(),ptr->getphi())*fabs(ptr->getrid());
      if(pge2[2]>0)pge2=pge2*(-1);
      pge=pge2;
     }
     ca=pmcg->getdir().prod(AMSDir(pge));
     cout <<" pmcg->getdir() "<<pmcg->getdir()<<endl;
     cout <<" AMSDir(pge) "<<AMSDir(pge)<<endl;
   if(ca>1)ca=1;
    cout <<"very very initial ca2 "<<180/3.1415926*acos(ca)<<" "<<endl;
    }

}    


 AMSPoint p1L,p1R;
 number thetaL,phiL,local;
 number thetaR,phiR;
{

{
  AMSDir dirL(_pntTrL->gettheta(2),_pntTrL->getphi(2));
  AMSDir dirR(_pntTrR->gettheta(2),_pntTrR->getphi(2));
  if (dirL[2]>0)dirL=dirL*(-1);
  if (dirR[2]>0)dirR=dirR*(-1);
    number ca=dirL.prod(dirR);
    _MGAM=sqrt(fabs(2*fabs(_pntTrR->getpirid())*fabs(_pntTrL->getpirid())*(1-ca)));
    if(ca>1)ca=1;
    cout <<"very very initial ca "<<180/3.1415926*acos(ca)<<" "<<_MGAM<<endl;

    AMSmceventg *pmcg=(AMSmceventg*)AMSEvent::gethead()->getheadC("AMSmceventg",0);
    if(pmcg){
     AMSPoint pge=dirR*fabs(_pntTrR->getpirid())+dirL*fabs(_pntTrL->getpirid());
     ca=pmcg->getdir().prod(AMSDir(pge));
     cout <<" pmcg->getdir() "<<pmcg->getdir()<<endl;
     cout <<" AMSDir(pge) "<<AMSDir(pge)<<endl;
   if(ca>1)ca=1;
    cout <<"very very initial ca2 "<<180/3.1415926*acos(ca)<<" "<<endl;
    }

}    


{
  AMSDir dirL(_pntTrL->gettheta(),_pntTrL->getphi());
  AMSDir dirR(_pntTrR->gettheta(),_pntTrR->getphi());
  if (dirL[2]>0)dirL=dirL*(-1);
  if (dirR[2]>0)dirR=dirR*(-1);
    number ca=dirL.prod(dirR);
    _MGAM=sqrt(fabs(2*fabs(_pntTrR->getrid())*fabs(_pntTrL->getrid())*(1-ca)));
    if(ca>1)ca=1;
    cout <<"very initial ca "<<180/3.1415926*acos(ca)<<" "<<_MGAM<<endl;

    AMSmceventg *pmcg=(AMSmceventg*)AMSEvent::gethead()->getheadC("AMSmceventg",0);
    if(pmcg){
     AMSPoint pge=dirR*fabs(_pntTrR->getrid())+dirL*fabs(_pntTrL->getrid());
     ca=pmcg->getdir().prod(AMSDir(pge));
     cout <<" pmcg->getdir() "<<pmcg->getdir()<<endl;
     cout <<" AMSDir(pge) "<<AMSDir(pge)<<endl;
   if(ca>1)ca=1;
    cout <<"very initial ca2 "<<180/3.1415926*acos(ca)<<" "<<endl;
    }

}    
}
 if(_pntTrL->intercept(p1L,lay,thetaL,phiL,local,_pntTrL->getpattern()<0?1:0) && _pntTrR->intercept(p1R,lay,thetaR,phiR,local,_pntTrR->getpattern()<0?1:0)){
  
  AMSDir dirL(thetaL,phiL);
  AMSDir dirR(thetaR,phiR);
  if (dirL[2]>0)dirL=dirL*(-1);
  if (dirR[2]>0)dirR=dirR*(-1);


{
    number ca=dirL.prod(dirR);
    _MGAM=sqrt(fabs(2*fabs(_pntTrR->getrid())*fabs(_pntTrL->getrid())*(1-ca)));
    if(ca>1)ca=1;
    cout <<" initial ca "<<180/3.1415926*acos(ca)<<" "<<_MGAM<<endl;

    AMSmceventg *pmcg=(AMSmceventg*)AMSEvent::gethead()->getheadC("AMSmceventg",0);
    if(pmcg){
     AMSPoint pge=dirR*fabs(_pntTrR->getgrid())+dirL*fabs(_pntTrL->getgrid());
     ca=pmcg->getdir().prod(AMSDir(pge));
    if(ca>1)ca=1;
    cout <<" initial ca2 "<<180/3.1415926*acos(ca)<<" "<<endl;
    }
}
    

//  Now find the two str lines interception   


   
    AMSPoint alpha=dirL.crossp(dirR);
    AMSPoint beta=dirL.crossp(p1L-p1R);
    number t=alpha.prod(beta)/alpha.prod(alpha);
//    if(t>0)t=0;   // min vertex 
//     t=0;
    _P0R=p1R+dirR*t;
    _P0L=p1L+dirL*t;
    AMSPoint dc=p1L-_P0R;
    number dist=dirL.prod(dc);
    _TrackDistance=sqrt(dc.prod(dc)-dist*dist);
    _Vertex=(_P0L+_P0R)*0.5;
//    _TrackDistance=(_P0L-_P0R).norm();
    

    AMSDir dir(0,0,-1);
    number length;
    _pntTrL->interpolate(_Vertex,dir,_P0L,thetaL,phiL,length,_pntTrL->getpattern()<0?1:0);    
    _pntTrR->interpolate(_Vertex,dir,_P0R,thetaR,phiR,length,_pntTrR->getpattern()<0?1:0);    
    if(_pntTrL->getpattern()<0)_pntTrL->SetPar(_pntTrL->getgrid(),thetaL,phiL,_P0L,1);
    else _pntTrL->SetPar(_pntTrL->getrid(),thetaL,phiL,_P0L);
    if(_pntTrR->getpattern()<0)_pntTrR->SetPar(_pntTrR->getgrid(),thetaR,phiR,_P0R,1);
    else _pntTrR->SetPar(_pntTrR->getrid(),thetaR,phiR,_P0R);
    dirL=AMSDir(thetaL,phiL);
    dirR=AMSDir(thetaR,phiR);
    if (dirL[2]>0)dirL=dirL*(-1);
    if (dirR[2]>0)dirR=dirR*(-1);




{
    number ca=dirL.prod(dirR);
    _MGAM=sqrt(fabs(2*fabs(_pntTrR->getrid())*fabs(_pntTrL->getrid())*(1-ca)));
    if(ca>1)ca=1;
    cout <<" final "<<180/3.1415926*acos(ca)<<" "<<_MGAM<<endl;

    AMSmceventg *pmcg=(AMSmceventg*)AMSEvent::gethead()->getheadC("AMSmceventg",0);
    if(pmcg){
     AMSPoint pge=dirR*fabs(_pntTrR->getgrid())+dirL*fabs(_pntTrL->getgrid());
     ca=pmcg->getdir().prod(AMSDir(pge));
    if(ca>1)ca=1;
    cout <<" final2 "<<180/3.1415926*acos(ca)<<" "<<endl;
    }
}






//  set gamma parameters  (assuming direction for top to bottom)
    _Charge=(_pntTrL->getrid()>0?1:-1)+(_pntTrR->getrid()>0?1:-1);
    cout <<" final charge "<<_Charge<<endl;
    AMSPoint pge=dirR*fabs(_pntTrR->getrid())+dirL*fabs(_pntTrL->getrid());
    number err_1=pow(_pntTrL->geterid()*_pntTrL->getrid()*_pntTrL->getrid(),2)+pow(_pntTrL->geterid()*_pntTrL->getrid()*_pntTrL->getrid(),2);
   AMSPoint p1=dirR*fabs(_pntTrR->getgrid())+dirL*fabs(_pntTrL->getgrid());
   number err_2=p1.norm()-pge.norm();
    _PGAMM=(pge.norm()+p1.norm())/2;
    _ErrPGAMM=sqrt(err_2*err_2+err_1);

{
  dirL=AMSDir(_pntTrL->gettheta(),_pntTrL->getphi());
  dirR=AMSDir(_pntTrR->gettheta(),_pntTrR->getphi());
  if (dirL[2]>0)dirL=dirL*(-1);
  if (dirR[2]>0)dirR=dirR*(-1);
     pge=dirR*fabs(_pntTrR->getrid())+dirL*fabs(_pntTrL->getrid());
     AMSTrTrack *ptr=lay_l<lay_r?_pntTrL:(lay_r<lay_l?_pntTrR:0);
     if(ptr  ){
      AMSPoint pge2=AMSDir(ptr->gettheta(),ptr->getphi())*fabs(ptr->getrid());
      if(pge2[2]>0)pge2=pge2*(-1);
      pge=pge2;
     }

}    


   _PhTheta=AMSDir(pge).gettheta();
    _PhPhi=AMSDir(pge).getphi();
    number ca=dirL.prod(dirR);
    _MGAM=sqrt(fabs(2*fabs(_pntTrR->getrid())*fabs(_pntTrL->getrid())*(1-ca)));
           
  return;
 } 
}
_PGAMM=0;
_ErrPGAMM=0;
_PhTheta=0;
_PhPhi=0;
setstatus(AMSDBc::BAD);



}
