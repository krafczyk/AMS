//  $Id: gamma.C,v 1.33 2003/03/25 09:05:19 choutko Exp $
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
  
  double newmid[8]={10000,10000,10000,10000,10000,10000,10000,10000};
  
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
      }
      //
      
      //
    }
  }// LA
  
  
  
  
  for(int i=0;i<TKDBc::nlay();i++){
    if (VARR <= 1.6){
      newmid[i]=(BF*(TKDBc::zposl(i)))+AF;
      if (FLPAT[i]==1 && middle[i] != 10000){
	CE[i]=middle[i];
	//     if (fabs(newmid[i]-middle[i]) < 0.6){
	CE[i]=newmid[i];
	//     }
      }
    }
  }
  //
  // One more alternative:
 /*

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
 */  
}

void AMSTrTrackGamma::_SingleCommonHit(integer FLPAT[], vector<double> H[]){
  
  

  // planes 1  2   3   4
  // FLPAT  0  1  1/2  2 -> 0 2 2 2

     if ((FLPAT[0] == 0 && FLPAT[1] == 1 && FLPAT[2] == 2 && (fabs(H[2][0]-H[2][1])<= 0.17)) ||
         (FLPAT[0] == 0 && FLPAT[1] == 1 && FLPAT[2] < 2 && FLPAT[3] == 2 && (fabs(H[3][0]-H[3][1])<= 0.5))){
       for (AMSTrRecHit * p=AMSTrRecHit::gethead(1); p!=NULL; p=p->next()){ 
	 if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;

	 if (p->checkstatus(AMSDBc::GAMMALEFT) || p->checkstatus(AMSDBc::GAMMARIGHT)){
             p->setstatus(AMSDBc::GAMMALEFT);
	     p->setstatus(AMSDBc::GAMMARIGHT);
	 }

       }
       if (FLPAT[2] == 1){
	 for (AMSTrRecHit * p=AMSTrRecHit::gethead(2); p!=NULL; p=p->next()){ 
	 if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;

	 if (p->checkstatus(AMSDBc::GAMMALEFT) || p->checkstatus(AMSDBc::GAMMARIGHT)){
             p->setstatus(AMSDBc::GAMMALEFT);
	     p->setstatus(AMSDBc::GAMMARIGHT);
	 }

       }
       }

     }

     if (FLPAT[0] == 1 && FLPAT[1] ==2 && FLPAT[2] == 2 && (fabs(H[1][0]-H[1][1])<= 0.44) &&  (fabs(H[2][0]-H[2][1])<= 0.63)){
       for (AMSTrRecHit * p=AMSTrRecHit::gethead(0); p!=NULL; p=p->next()){ 
	 if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;

	 if (p->checkstatus(AMSDBc::GAMMALEFT) || p->checkstatus(AMSDBc::GAMMARIGHT)){
             p->setstatus(AMSDBc::GAMMALEFT);
	     p->setstatus(AMSDBc::GAMMARIGHT);
	 }

       }
     }



  // planes 1  2  3  4
  // FLPAT  1  2  .. .. -> 2  2
  // FLPAT  1 1/0 2  .. -> 2 1/0 2

  //     if ((FLPAT[0] == 1 && FLPAT[1] == 2 && (fabs(H[1][0]-H[1][1])<= 0.19)) ||
     if ((FLPAT[0] == 1 && FLPAT[1] == 2 && (fabs(H[1][0]-H[1][1])<= 0.31)) ||
         (FLPAT[0] == 1 && FLPAT[1] <2 && FLPAT[2] == 2 && (fabs(H[2][0]-H[2][1])<= 0.17)) ||
         (FLPAT[0] == 1 && FLPAT[1] <2 && FLPAT[2] <2   && FLPAT[3] == 2 && (fabs(H[3][0]-H[3][1])<= 0.5))||

          (FLPAT[0] == 1 && FLPAT[1] <2 && FLPAT[2] <2 && FLPAT[3] <2 && FLPAT[4] == 2 && (fabs(H[4][0]-H[4][1])<= 0.95) ) ){
       for (AMSTrRecHit * p=AMSTrRecHit::gethead(0); p!=NULL; p=p->next()){ 
	 if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;

	 if (p->checkstatus(AMSDBc::GAMMALEFT) || p->checkstatus(AMSDBc::GAMMARIGHT)){
             p->setstatus(AMSDBc::GAMMALEFT);
	     p->setstatus(AMSDBc::GAMMARIGHT);
	 }

       }


  // planes 1  2  3  4
  // FLPAT  2  1  2  .. -> 2  2  2
       if (FLPAT[1] == 1){
	 for (AMSTrRecHit * p=AMSTrRecHit::gethead(1); p!=NULL; p=p->next()){ 
	   if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
	 if (p->checkstatus(AMSDBc::GAMMALEFT) || p->checkstatus(AMSDBc::GAMMARIGHT)){
	   p->setstatus(AMSDBc::GAMMALEFT);
	   p->setstatus(AMSDBc::GAMMARIGHT);
	 }

	 }
       }

       if (FLPAT[2] == 1){
	 for (AMSTrRecHit * p=AMSTrRecHit::gethead(2); p!=NULL; p=p->next()){ 
	   if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
	 if (p->checkstatus(AMSDBc::GAMMALEFT) || p->checkstatus(AMSDBc::GAMMARIGHT)){
	   p->setstatus(AMSDBc::GAMMALEFT);
	   p->setstatus(AMSDBc::GAMMARIGHT);
	 }

	 }
       }

       
     }     




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
  if (FLPAT[0] == 2 || FLPAT[0] == 1 ){
    int ja=-1;
    double yc[4];
    double zc[4];
    
    for(int j=1;j<5;j++){
      //  for(int j=1;j<7;j++){
      if (FLPAT[j]==2){
	ja++;
	yc[ja]=mid[j];
	zc[ja]=z_tkl[j];
      }
    }
    if ( ja == 0 ){
      // if ( ja <= 2){
      for(int j=5;j<8;j++){
	if (FLPAT[j]==2){
	  ja++;
	  yc[ja]=mid[j];
	  zc[ja]=z_tkl[j];
	}
      }
    }
    double thc1,A,B,VAR;
    double off1=10000;
    if (ja >= 1){
      dlinearme((ja+1),zc,yc,A,B,VAR);
      thc1=A+B*z_tkl[0];
      
      if (FLPAT[0] == 2){ //tk1
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
    
    // proviamo col tk1 single hit
    if (FLPAT[0] == 1){
      if (fabs(H[0][0]-thc1)>1 && ja >= 1 && VAR < 1){
	H[0][0]=10000;
	H[0].clear();
	H[0].resize(0);
	FLPAT[0]=0;
      }
    } //==1
  }
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
    
    double thc8,A,B,VAR;
    double off8=10000;
    if (ja >= 1){
      dlinearme((ja+1),zc,yc,A,B,VAR);
      thc8=A+B*z_tkl[7];
      for(int j=0;j<ja+1;j++){
	for(int ii=0;ii<TKDBc::nlay();ii++){
	  if (z_tkl[ii] == zc[ja]){
	    if ((6-ii) <= 3){
	      if ( re[7] > mare8[6-ii]*re[ii] ){//too large
		off8=(fabs(H[7][0]-thc8) > fabs(H[7][1]-thc8))?H[7][0]:H[7][1];
	      }
	      if ( off8 == 10000 && re[7] < 0.05 ){
		off8=H[7][0];
		if (fabs(thc8-((H[7][0]+H[7][1])/2)) > 2 ){
		  H[7][1]=10000; 
		}
	      }
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




///////////////////////////////

void AMSTrTrackGamma::SetBestResLR(integer FLPAT[], vector<double> H[], integer esc_1, integer esc_2,integer es_f1){
  // With this method we select the best set of Delta-y between two y hits
  // the same plane (or sub-level).
  // The estimator would be based on "lsqp2" 
  // In priciple it should be apply always because and the minimum number of 
  // Deltas must be 3

  double z_tkl[trconst::maxlay];
  for(int i=0;i<TKDBc::nlay();i++){
    z_tkl[i]=TKDBc::zposl(i);
  }
  const int maxsize=500;
  double res_LR[maxsize][trconst::maxlay];
  double mid_LR[maxsize][trconst::maxlay];
  
  double res_[trconst::maxlay];
  double z_fi[trconst::maxlay];
  double mid[trconst::maxlay];
 
  for(int i=0;i<TKDBc::nlay();i++){
    z_fi[i]=10000;
    res_[i]=10000;
    mid[i]=10000;
    for(int k=0;k<maxsize;k++){
      res_LR[k][i]=10000;
    }
  }
  int ja;
  ja = -1;


  // first if I have one plane with more than 6 hits I remove it from counting
  // or I check the centrality ....
  for(int i=1;i<TKDBc::nlay();i++){
    if (FLPAT[i]>=6){
      for(int jo=0;jo<FLPAT[i];jo++){
	//	H[i][jo]=10000;
      }  
    }
  }


  // i=1 means that I don't consider plane 1 for the moment ....
    for(int i=1;i<TKDBc::nlay();i++){
      // Let's consider occupancy=2 (FLPAT[i]==2)
      if (FLPAT[i]==2){
	ja++;  
	res_[ja]=fabs(H[i][0]-H[i][1]);
	z_fi[ja]=z_tkl[i];
	//
        mid[ja]=(H[i][0]+H[i][1])/2; 
      }
    } 

    // FIRST OF ALL: Is the res vs z decreasing ??????

 


     int intime=ja+1;
     for(int i=0;i<intime;i++){
      _CheckInvReswrtZ(res_,z_fi,mid,ja);
     }  

   

    int j1=-1;
  double res1_[trconst::maxlay];
  double z1_fi[trconst::maxlay];
  double mid1[trconst::maxlay];

  for(int i=0;i<TKDBc::nlay();i++){
    z1_fi[i]=10000;
    res1_[i]=10000;
    mid1[i]=10000;
  }






    if ((ja+1) < 3){ // we need an alternative
      j1=-1;
      // the candidate pair is compared to higher z plane in order to limit the mistake
      // in counting two LEFT hits or two RIGHT hits.
    if ((FLPAT[1]==2 || FLPAT[2]==2) && (FLPAT[3]==1 && FLPAT[4]==1)){
      //  X  2(2)  11  XX X
      if ((FLPAT[2]==2) &&
         (fabs(H[3][0]-H[4][0]) > fabs(H[2][0]-H[2][1]))){
	j1++;
       res1_[j1]=fabs(H[3][0]-H[4][0]);
       z1_fi[j1]=z_tkl[3]; // we assign the higher z coo
       mid1[j1]=(H[3][0]+H[4][0])/2;
      }
      if ((FLPAT[2]!=2 && FLPAT[1]==2) &&
         (fabs(H[3][0]-H[4][0]) > fabs(H[1][0]-H[1][1]))){
	j1++;
       res1_[j1]=fabs(H[3][0]-H[4][0]);
       z1_fi[j1]=z_tkl[3]; // we assign the higher z coo
       mid1[j1]=(H[3][0]+H[4][0])/2;
      }
    }
    // 
    if (FLPAT[5]==1 && FLPAT[6]==1){
     if (FLPAT[3]==2 || FLPAT[4]==2){
      //  X  XX  2(2)  11 X
      if ((FLPAT[4]==2) &&
         (fabs(H[5][0]-H[6][0]) > fabs(H[4][0]-H[4][1]))){
	j1++;
	res1_[j1]=fabs(H[5][0]-H[6][0]);
	z1_fi[j1]=z_tkl[5]; // we assign the higher z coo 
	mid1[j1]=(H[5][0]+H[6][0])/2;
      }
      if ((FLPAT[4]!=2 && FLPAT[3]==2) &&
         (fabs(H[5][0]-H[6][0]) > fabs(H[3][0]-H[3][1]))){
	j1++;
	res1_[j1]=fabs(H[5][0]-H[6][0]);
	z1_fi[j1]=z_tkl[5]; // we assign the higher z coo 
	mid1[j1]=(H[5][0]+H[6][0])/2;
      }
     }
    if (FLPAT[1]==2 || FLPAT[2]==2){
      //  X  2(2)  XX  11 X
      if ((FLPAT[2]==2) &&
         (fabs(H[5][0]-H[6][0]) > fabs(H[2][0]-H[2][1]))){
       	j1++;
	res1_[j1]=fabs(H[5][0]-H[6][0]);
	z1_fi[j1]=z_tkl[5]; // we assign the higher z coo 
	mid1[j1]=(H[5][0]+H[6][0])/2;
      }
      if ((FLPAT[2]!=2 && FLPAT[1]==2) &&
         (fabs(H[5][0]-H[6][0]) > fabs(H[1][0]-H[1][1]))){
	j1++;
	res1_[j1]=fabs(H[5][0]-H[6][0]);
	z1_fi[j1]=z_tkl[5]; // we assign the higher z coo 
	mid1[j1]=(H[5][0]+H[6][0])/2;
      }
    }
    }
    //
    }
    //


    // if double hits are not enough redo counting

 
    int ia;
  double res_lr[trconst::maxlay];
  double z_lr[trconst::maxlay];
  double mid_lr[trconst::maxlay];
  for(int i=0;i<TKDBc::nlay();i++){
    z_lr[i]=10000;
    res_lr[i]=10000;
    mid_lr[i]=10000;
  }
    ia=-1;
  // i=1 means plane 1 still excluded......
    for(int i=1;i<TKDBc::nlay();i++){
      //
      if (FLPAT[i]==2){
        for(int j=0;j<ja+1;j++){
          if (z_fi[j] == z_tkl[i]){
	    ia++;  
	    res_lr[ia]=res_[j];
	    z_lr[ia]=z_fi[j];
	    //	    mid_lr[ia]=mid[j];
            mid_lr[ia]=(H[i][0]+H[i][1])/2; 
	  }
	}
      }
      if (FLPAT[i]==1){
	for(int j=0;j<j1+1;j++){
	  if (z1_fi[j] == z_tkl[i]){
	    ia++;  
	    res_lr[ia]=res1_[j];
	    z_lr[ia]=z1_fi[j];
	    mid_lr[ia]=mid1[j];
	  }
	}
      }
      //
    } 

 
  // let's see the first plane:
  double minimo=10000;
  for(int i=0;i<ia+1;i++){
    if (res_lr[i]<minimo){
      minimo=res_lr[i];        
    }
  }
  if ((ia+1) == 2 && j1==-1){
    if (FLPAT[0]==2 && fabs(H[0][0]-H[0][1]) <= 0.07 && fabs(H[0][0]-H[0][1])< minimo){
      
      for(int i=0;i<TKDBc::nlay();i++){
	z_lr[i]=10000;
	res_lr[i]=10000;
	mid_lr[i]=10000;
      }
       

      res_lr[0]=fabs(H[0][0]-H[0][1]);
      z_lr[0]=z_tkl[0];
      mid_lr[0]=(H[0][0]+H[0][1])/2; 
      ia=0;
    for(int i=1;i<TKDBc::nlay();i++){
      //
      if (FLPAT[i]==2){
        for(int j=0;j<ja+1;j++){
          if (z_fi[j] == z_tkl[i]){
	    ia++;  
	    res_lr[ia]=res_[j];
	    z_lr[ia]=z_fi[j];
	    //	    mid_lr[ia]=mid[j];
	    mid_lr[ia]=mid[j];
	  }
	}
      }
    }


    }
  }
  //




  double mthl1[trconst::maxlay];
  double a0,a1,var;
  int flaoccup=0;
  var=10000;
  //
  if ((ia+1) == 2 && j1==-1){
    // let's check if there is an hybrid pair anyway, even if it 
    // is not comparable with one more upstream

dlinearme(ia+1,z_lr,mid_lr,a0,a1,var);
//
     
    j1=-1;
    for(int i=0;i<TKDBc::nlay();i++){
      z1_fi[i]=10000;
      res1_[i]=10000;
      mid1[i]=10000;
      mthl1[i]=10000;
    }

    if (FLPAT[3]==1 && FLPAT[4]==1){
      j1++;
      res1_[j1]=fabs(H[3][0]-H[4][0]);
      z1_fi[j1]=z_tkl[3];// we assign the higher z coo 
      mid1[j1]=(H[3][0]+H[4][0])/2;
      mthl1[j1]=a0+a1*z_tkl[3];
      
    }
    if (FLPAT[5]==1 && FLPAT[6]==1){
      j1++;
      res1_[j1]=fabs(H[5][0]-H[6][0]);
      z1_fi[j1]=z_tkl[5]; // we assign the higher z coo 
      mid1[j1]=(H[5][0]+H[6][0])/2;
      mthl1[j1]=a0+a1*z_tkl[5];
    }
    //
    if (j1 > -1){ // then it means I found some candidate
  // Then, we reexam the hybrid candidate in the case the 
  // center and the res would be compatible
      for(int i=0;i<j1+1;i++){   
        flaoccup=0;
	if (fabs(mid1[i]-mthl1[i]) < 2.2){ //2.2 cm
	  for(int j=0;j<ia+1;j++){ // we check it is in increasing order
	      if (z_lr[j] < z1_fi[i] && res_lr[j] > res1_[i]){
		flaoccup=1;
	      }
              if (z_lr[j] < z1_fi[i] && res_lr[j] < res1_[i]){
		flaoccup=0;
	      }
              if (z_lr[j] > z1_fi[i] && res_lr[j] < res1_[i]){
                flaoccup=1;
	      } 
             if (z_lr[j] > z1_fi[i] && res_lr[j] > res1_[i]){
                flaoccup=0;
	      } 
	     //
      //
	  }
	}

      if (flaoccup == 1){
        ia++;
        z_lr[ia]=z1_fi[i];
	res_lr[ia]=res1_[i];
	mid_lr[ia]=mthl1[i]; // NOTA BENE prendo  il teorico
      }

      }
      // this res "i" can be included

    }
 
    // 
  }

 if ((ia+1) == 2){
   if (esc_2 <= 4 && FLPAT[0] == 2){

       if (fabs(H[0][0]-H[0][1])<= 0.7){
	 ia++;
	 res_lr[ia]=fabs(H[0][0]-H[0][1]);
         z_lr[ia]=z_tkl[0];
         mid_lr[ia]=(H[0][0]+H[0][1])/2;
	 int iy1=ia;
	 int iy2=ia;
	 _CheckZOrder(res_lr,z_lr,mid_lr,iy2);
	 if (iy2 == iy1-1){
	   ia--;
	 }
       }
   }
 }


  ////////////////////////// the end //////////// the highest res_lr seeds have been found

    // NOW with or without hybrid res_, we must check:
    // 1) if they are at least 3, minimizing sdw (N.B.: It doesn't work if the pairs are less than 3)
    // 2) if they are just 2 .... alternatively centers.... 




  double CHI2[maxsize];
  double aa0,aa1,aa2,sdw,sdwmax,sdwini,varini,a0i,a1i;
  varini=10000;
  sdwini=10000;
  sdw=10000;
  sdwmax=10000;
  int l_[trconst::maxlay];
  int l1_[trconst::maxlay];
  int l2_[trconst::maxlay];
  for(int i=0;i<TKDBc::nlay();i++){
    l_[i]=10000;
    l1_[i]=10000;
    l2_[i]=10000;
  }
 double best_a0,best_a1,best_a2,best_sdw;
 int inc=1;
 	      best_a0=10000;
	      best_a1=10000;
	      best_a2=10000;
	      best_sdw=10000;

    //supponiamo di aver fatto del nostro meglio 
    int j;
if ((ia+1) >= 3){
  // let's look at tk-plane with more than 3 hits and let's choose the best residual
  //still excluding TK1
  /////
  // just in cae check first linera centers
dlinearme(ia+1,z_lr,mid_lr,a0i,a1i,varini);

dlsqp2me(ia+1,z_lr,res_lr,aa0,aa1,aa2,sdwini);

if (sdwini < 1){

   for(int i=1;i<TKDBc::nlay()-1;i++){
    if (FLPAT[i]>=3 && FLPAT[i]<6){
dlsqp2me(ia+1,z_lr,res_lr,aa0,aa1,aa2,sdwini);

 best_a0=aa0;
 best_a1=aa1;
 best_a2=aa2;
 best_sdw=sdwini;
 sdwmax=10000;
 //sdwmax=sdwini;
 ia++;  
 
 //
 j=-1;
 //
 for(int i1=0;i1<FLPAT[i]-1;i1++){
   for(int i2=i1+1;i2<FLPAT[i];i2++){
	  
     j++;
     if(j>maxsize)break;

     res_LR[j][i]=fabs(H[i][i1]-H[i][i2]);
     res_lr[ia]=res_LR[j][i];
     z_lr[ia]=z_tkl[i];
     mid_LR[j][i]=(H[i][i1]+H[i][i2])/2;
     mid_lr[ia]=mid_LR[j][i];
     
dlsqp2me(ia+1,z_lr,res_lr,aa0,aa1,aa2,sdw);
	  
           int iy1=ia;
           int iy2=ia;

	   _CheckZOrder(res_lr,z_lr,mid_lr,iy2);
	   if (iy2 == iy1){
	    CHI2[j]=sdw; 
	    dlinearme(ia+1,z_lr,mid_lr,a0,a1,var);
	    if (CHI2[j]<sdwmax){
	      if (fabs(mid_lr[ia]-(a0i+a1i*z_lr[ia]))<2){
	      l_[i]=j;
	      l1_[i]=i1;
	      l2_[i]=i2;

	      sdwmax=CHI2[j];
	      
	      best_a0=aa0;
	      best_a1=aa1;
	      best_a2=aa2;
	      best_sdw=sdwmax;
	      }
	    
	    }
	   }
	  
	  //      
   }
 }
 if (l_[i] != 10000){
   res_lr[ia]=res_LR[l_[i]][i];
   mid_lr[ia]=mid_LR[l_[i]][i];
   z_lr[ia]=z_tkl[i];
 }  

  //  if ((ia+1) > 4 &&  sdwini < 0.5 && sdwmax > 1){
  //    cout << " piano i = "<<i<<endl;
  //    for(int jo=0;jo<FLPAT[i];jo++){
   //     cout<<"H["<<i<<"]["<<jo<<"] = "<<H[i][jo]<<endl;
     //     H[i][jo]=10000;
   //     cout<<"H["<<i<<"]["<<jo<<"] = "<<H[i][jo]<<endl;  
   //   }
   //   ia--;
   // }

 if (l1_[i] == 10000 && l2_[i] == 10000 ) ia--;
    
    }
  }
}

if (FLPAT[0] >= 2 && FLPAT[0]<6){
 
dlinearme(ia+1,z_lr,mid_lr,a0i,a1i,varini);

dlsqp2me(ia+1,z_lr,res_lr,aa0,aa1,aa2,sdwini);

 if (sdwini < 1){
   sdwmax=10000;

   best_a0=aa0;
   best_a1=aa1;
   best_a2=aa2;
   best_sdw=sdwini;
   ia++;  
   
   //
   j=-1;
   //
   for(int i1=0;i1<FLPAT[0]-1;i1++){
     for(int i2=i1+1;i2<FLPAT[0];i2++){
       
       j++;
       if(j>maxsize)break;

       res_LR[j][0]=fabs(H[0][i1]-H[0][i2]);
       res_lr[ia]=res_LR[j][0];
       z_lr[ia]=z_tkl[0];
       mid_LR[j][0]=(H[0][i1]+H[0][i2])/2;
       mid_lr[ia]=mid_LR[j][0];

       dlsqp2me(ia+1,z_lr,res_lr,aa0,aa1,aa2,sdw);
       int iy1=ia;
       int iy2=ia;

       _CheckZOrder(res_lr,z_lr,mid_lr,iy2);
       if (iy2 == iy1){
	 CHI2[j]=sdw; 
	 dlinearme(ia+1,z_lr,mid_lr,a0,a1,var);
	    if (CHI2[j]<sdwmax){
	      if (FLPAT[0] > 2 || (FLPAT[0] == 2 && CHI2[j]< 0.5)){
		if (fabs(mid_lr[ia]-(a0i+a1i*z_lr[ia]))<1.2){
	      //
	      
	      l_[0]=j;
	      l1_[0]=i1;
	      l2_[0]=i2;

		  sdwmax=CHI2[j];
		  best_a0=aa0;
		  best_a1=aa1;
		  best_a2=aa2;
		  best_sdw=sdwmax;
		}
	      }
	    }
	    }
	  //      
   }
 }
	    if (l_[0] != 10000){
	      res_lr[ia]=res_LR[l_[0]][0];
	      mid_lr[ia]=mid_LR[l_[0]][0];
	      z_lr[ia]=z_tkl[0];
	    }

 // if ((ia+1) > 4 &&  sdwini < 0.5 && sdwmax > 1){
 //  cout << " piano 0 "<<endl;
 //  for(int jo=0;jo<FLPAT[0];jo++){
 //    cout<<"H[0]["<<jo<<"] = "<<H[0][jo]<<endl;
     //         H[0][jo]=10000;
 //    cout<<"H[0]["<<jo<<"] = "<<H[0][jo]<<endl;  
 // }
 // ia--;
 // }
 if (l1_[0] == 10000 && l2_[0] == 10000 ) ia--;

} //sdwini
  }// tk1 >3
//} // ia+1 >4

if (FLPAT[7] >= 3 && FLPAT[7]<6){

dlinearme(ia+1,z_lr,mid_lr,a0i,a1i,varini);
 /////
dlsqp2me(ia+1,z_lr,res_lr,aa0,aa1,aa2,sdwini);

 if (sdwini < 1){
   sdwmax=10000;

   best_a0=aa0;
   best_a1=aa1;
   best_a2=aa2;
   best_sdw=sdwini;
   ia++;  
   
   //
   j=-1;
   //
   for(int i1=0;i1<FLPAT[7]-1;i1++){
     for(int i2=i1+1;i2<FLPAT[7];i2++){
       j++;
       if(j>maxsize)break;

       res_LR[j][7]=fabs(H[7][i1]-H[7][i2]);
       res_lr[ia]=res_LR[j][7];
       z_lr[ia]=z_tkl[7];
       mid_LR[j][7]=(H[7][i1]+H[7][i2])/2;
       mid_lr[ia]=mid_LR[j][7];

       dlsqp2me(ia+1,z_lr,res_lr,aa0,aa1,aa2,sdw);

       int iy1=ia;
       int iy2=ia;
       

       _CheckZOrder(res_lr,z_lr,mid_lr,iy2);
       if (iy2 == iy1){
	 CHI2[j]=sdw; 

	 dlinearme(ia+1,z_lr,mid_lr,a0,a1,var);
	    if (CHI2[j]<sdwmax){
		if (fabs(mid_lr[ia]-(a0i+a1i*z_lr[ia]))<2){
	      //
	      
	      l_[7]=j;
	      l1_[7]=i1;
	      l2_[7]=i2;

	      sdwmax=CHI2[j];
	      
	      best_a0=aa0;
	      best_a1=aa1;
	      best_a2=aa2;
	      best_sdw=sdwmax;
	      }
	    }
	    	   }
	  //      
	}
 }
 if (l_[7] != 10000){
   res_lr[ia]=res_LR[l_[7]][7];
   mid_lr[ia]=mid_LR[l_[7]][7];
   z_lr[ia]=z_tkl[7];
 }
  //  if ((ia+1) > 4 &&  sdwini < 0.5 && sdwmax > 1){
  //    cout << " piano 7 "<<endl;
  //    for(int jo=0;jo<FLPAT[7];jo++){
   //     cout<<"H[7]["<<jo<<"] = "<<H[7][jo]<<endl;
     //     H[7][jo]=10000;
   //     cout<<"H[7]["<<jo<<"] = "<<H[7][jo]<<endl;  
   //   }
   //   ia--;
   // }

 if (l1_[7] == 10000 && l2_[7] == 10000 ) ia--;
    

 }
     } //tk8

} // ia+1 >=3
//






// If ia+1 is 3 means that the previous loop was not succesful
// High multiplicity no succesfully reduced at two
// -> BadTriplemore
// or no high multiplicity at all. In this case some:
// flpat=2 excluded -> BadDouble or
// flpat <=1
// Moreover if it was ia+1=2 the loop was not apply at all
// and in this case I think we have to exclude events with 
//too high multiplicity or 
//keeping single hits well separated from the expected, intepolated centers

 if ((ia+1) < 2 && esc_2 > 2){
     for(int i=1;i<TKDBc::nlay();i++){
       if (FLPAT[i]>2){
	 for(int jo=0;jo<FLPAT[i];jo++){
	   H[i][jo]=10000;
	 }
       }
     }
 }
 if ((ia+1) == 2){
      if (esc_2 >= 4 || es_f1 < 3 ){
     for(int i=1;i<TKDBc::nlay();i++){
       if (FLPAT[i]>2){
	 for(int jo=0;jo<FLPAT[i];jo++){
	   H[i][jo]=10000;
	 }
       }
       }  
     }
   //   if (esc_2 < 4 && es_f1 >=3 ){

   //   }
 }



  //


 
  // triple or more
  int Badtriplemore=1;

 double resexpeT,midexpeT,closeupT,closedoT,closestzT;
 int marindeT;

 double marginT[100];
 
 
 double maxmarginT;

  if ((ia+1) >=3 ){
    for(int i=0;i<TKDBc::nlay();i++){
      //
      for(int ii=0;ii<100;ii++){
	marginT[ii]=10000;
      }
      //
      if (FLPAT[i]>=3){
	Badtriplemore=1;
	for(int ii=0;ii<ia+1;ii++){
	  if (z_lr[ii] > (z_tkl[i]-0.5) && 
	      z_lr[ii] < (z_tkl[i]+0.5)){
	    Badtriplemore=0; 
	  }
	}
       //

       if (Badtriplemore){
	  // I have only one possibility: save one hit or exclude everybody
 	 dlinearme(ia+1,z_lr,mid_lr,a0,a1,var);
	 midexpeT=a0+a1*z_tkl[i];
        
	   resexpeT=best_a0+best_a1*z_tkl[i]+(best_a2*pow(z_tkl[i],2));
	 if (best_sdw < 0.6 && (ia+1) >= 4){   

	   for(int ii=0;ii<ia+1;ii++){
	     if (resexpeT>0.05){
	     if (z_tkl[i] < z_lr[ii] && resexpeT < res_lr[ii]){
	       // it doesn't respect the increasing order, the fitting is not properly done 
	       resexpeT=10000;
	     }
	     }
	   } 


	   if (resexpeT<0.05 || best_sdw < 0.01){
	     //
	     for(int jj=0;jj<FLPAT[i];jj++){
               marginT[jj]=fabs(H[i][jj]-midexpeT);
	     }
	     maxmarginT=10000;
             for(int jj=0;jj<FLPAT[i];jj++){
               if (marginT[jj] < maxmarginT){
		 maxmarginT=marginT[jj];
                 marindeT=jj;
	       }
	     } 
             if (maxmarginT <= 0.2){
	       for(int jj=0;jj<FLPAT[i];jj++){
		 if (jj != marindeT){
		   H[i][jj]=10000;

		 }
	       }
	     }
	     if (maxmarginT > 0.2){
	       for(int jj=0;jj<FLPAT[i];jj++){
	     	 H[i][jj]=10000;
	      }
	     }
	      //
	   }
	   
	    //----
	   if (resexpeT>0.05 && best_sdw > 0.01 && resexpeT != 10000){
            for(int jj=0;jj<FLPAT[i];jj++){
               marginT[jj]=fabs(H[i][jj]-(midexpeT+(resexpeT/2)));
               marginT[jj+FLPAT[i]]=fabs(H[i][jj]-(midexpeT-(resexpeT/2)));
               // 0        and   FLPAT[i]-1  margin  RIGHT
               // FLPAT[i] and 2*FLPAT[i]-1  margin  LEFT
	      }
              maxmarginT=10000;      
             for(int jj=0;jj<(FLPAT[i]*2);jj++){
               if (marginT[jj] < maxmarginT){
		 maxmarginT=marginT[jj];
                 marindeT=jj;
	       }
	     } 
	     // marinde  could be the candidate
             if (maxmarginT <= (resexpeT/1.2)){
               if (marindeT < FLPAT[i]){
		 //right
                 for(int jj=0;jj<FLPAT[i];jj++){
		   if (jj != marindeT){
		     H[i][jj]=10000;
		
		   }
		 }
               }
               if (marindeT >= FLPAT[i]){
		 //left
                for(int jj=0;jj<FLPAT[i];jj++){
		   if (jj != marindeT-FLPAT[i]){
		     H[i][jj]=10000;
		
		   }
		 }
               }
	     }             
             if (maxmarginT > (resexpeT/1.2) || maxmarginT > 0.35){
	       for(int jj=0;jj<FLPAT[i];jj++){
		 H[i][jj]=10000;
	       }
	     }
	    }
	 }
         if ((best_sdw > 0.6 || resexpeT == 10000 || (ia+1) == 3)){
           closestzT=10000;
           for(int ii=0;ii<ia+1;ii++){
	     if (fabs(z_tkl[i]-z_lr[ii])<closestzT){
	       closestzT=fabs(z_tkl[i]-z_lr[ii]);
               closeupT=10000;
               closedoT=10000;
               if (z_tkl[i]>z_lr[ii])closeupT=res_lr[ii];
               if (z_tkl[i]<z_lr[ii])closedoT=res_lr[ii];
	     }
	   }
	   if (closestzT != 10000 && closeupT != 10000){
             for(int jj=0;jj<FLPAT[i];jj++){
               marginT[jj]=fabs(H[i][jj]-(midexpeT+(closeupT/2)));
               marginT[jj+FLPAT[i]]=fabs(H[i][jj]-(midexpeT-(closeupT/2)));
               // 0        and   FLPAT[i]-1  margin  RIGHT
               // FLPAT[i] and 2*FLPAT[i]-1  margin  LEFT
	     }
	   }
	   if (closestzT != 10000 && closedoT != 10000){
             for(int jj=0;jj<FLPAT[i];jj++){
               marginT[jj]=fabs(H[i][jj]-(midexpeT+(closedoT/2)));
               marginT[jj+FLPAT[i]]=fabs(H[i][jj]-(midexpeT-(closedoT/2)));
               // 0        and   FLPAT[i]-1  margin  RIGHT
               // FLPAT[i] and 2*FLPAT[i]-1  margin  LEFT
	     }
	   }
	   //
	   maxmarginT=10000;
	   //
	   for(int jj=0;jj<(FLPAT[i]*2);jj++){
	     if (marginT[jj] < maxmarginT){
	       maxmarginT=marginT[jj];
	       marindeT=jj;
	     }
	   }
               if (closeupT != 10000 && maxmarginT <= (closeupT/2.) ){
               if (marindeT < FLPAT[i]){
		 //right
                 for(int jj=0;jj<FLPAT[i];jj++){
		   if (jj != marindeT){
		     H[i][jj]=10000;
		   }
		 }
               }
               if (marindeT >= FLPAT[i]){
		 //left
                for(int jj=0;jj<FLPAT[i];jj++){
		   if (jj != marindeT-FLPAT[i]){
		     H[i][jj]=10000;
		   }
		 }
               } 
	       
	       }
             if (closedoT != 10000 && maxmarginT <= (closedoT/2.2) ){
               if (marindeT < FLPAT[i]){
		 //right
                 for(int jj=0;jj<FLPAT[i];jj++){
		   if (jj != marindeT){
		     H[i][jj]=10000;
		   }
		 }
               }
               if (marindeT >= FLPAT[i]){
		 //left
                for(int jj=0;jj<FLPAT[i];jj++){
		   if (jj != marindeT-FLPAT[i]){
		     H[i][jj]=10000;
		   }
		 }
               } 
	     }
             if ((closedoT != 10000 && maxmarginT > (closedoT/2.2) ) || 
                 (closeupT != 10000 && maxmarginT > (closeupT/2.) ) ||
                  maxmarginT > 0.4 ){
	       for(int jj=0;jj<FLPAT[i];jj++){
		 H[i][jj]=10000;
	       }

	     }
	 }
       }
      }
    }
  }


 // first of all the double bad hits
  int BadDouble=1;

 double resexpe,midexpe,closeup,closedo,closestz;
 int marinde;
 double maxmargin,margin[4];
  if ((ia+1) >=3 ){
  for(int i=0;i<TKDBc::nlay();i++){
    if (FLPAT[i]==2){
       BadDouble=1;
       resexpe=10000;
       midexpe=10000;
       for(int ii=0;ii<ia+1;ii++){
         if (z_lr[ii] > (z_tkl[i]-0.5) && 
	     z_lr[ii] < (z_tkl[i]+0.5)){
           BadDouble=0; 
	 }
       }
       if (BadDouble){
	 // We have two possibility to remove only one or both hits
         // Trying to keep just one
	 // first of all let's check the linearity of the centers
	 dlinearme(ia+1,z_lr,mid_lr,a0,a1,var);
	 midexpe=a0+a1*z_tkl[i];
         

	    //---
	    resexpe=best_a0+best_a1*z_tkl[i]+(best_a2*pow(z_tkl[i],2));
	    if (ia+1 >= 4 && best_sdw < 0.6){
	      if (resexpe>0.05){
		for(int ii=0;ii<ia+1;ii++){
		  if (z_tkl[i] < z_lr[ii] && resexpe < res_lr[ii]){
		    // it doesn't respect the increasing order, the fitting is not properly done 
		    resexpe=10000;
		  }
		}
	      }  

            if (resexpe<0.05 || best_sdw < 0.01){
	      //
	      for(int jj=0;jj<2;jj++){
               margin[jj]=fabs(H[i][jj]-midexpe);
               // 0 and 1  margin
	      }
              maxmargin=10000;
             for(int jj=0;jj<2;jj++){
               if (margin[jj] < maxmargin){
		 maxmargin=margin[jj];
                 marinde=jj;
	       }
	     } 
             if (maxmargin <= 0.2){
               if (marinde == 0){
                 H[i][1]=10000;
	       }
               if (marinde == 1){
                 H[i][0]=10000;
               
	       }
	     }
             
             if (maxmargin > 0.2){
	       H[i][1]=10000;
	       H[i][0]=10000;
	     }
	      //
	    }

	    //----
            if (resexpe != 10000 && resexpe>0.05 && best_sdw > 0.01){
	      for(int jj=0;jj<2;jj++){
               margin[jj]=fabs(H[i][jj]-(midexpe+(resexpe/2)));
               margin[jj+2]=fabs(H[i][jj]-(midexpe-(resexpe/2)));
               // 0 and 1  margin RIGHT
               // 2 and 3 margin left
	      }
              maxmargin=10000;
           
             for(int jj=0;jj<4;jj++){
               if (margin[jj] < maxmargin){
		 maxmargin=margin[jj];
                 marinde=jj;
	       }
	     } 
             if (maxmargin <= (resexpe/1.2)){
               if (marinde == 0 || marinde == 2){
                 H[i][1]=10000;
	       }
               if (marinde == 1 || marinde == 3){
                 H[i][0]=10000;
	       }
	     }
             
             if (maxmargin > (resexpe/1.2) || maxmargin > 0.35){
	       H[i][1]=10000;
	       H[i][0]=10000;
	     }

	    }


            
          }
	  // ------------------------
         if (((ia+1) == 3 || best_sdw > 0.6 || resexpe == 10000)){
           closestz=10000;
           for(int ii=0;ii<ia+1;ii++){
	     if (fabs(z_tkl[i]-z_lr[ii])<closestz){
	       closestz=fabs(z_tkl[i]-z_lr[ii]);
               closeup=10000;
               closedo=10000;
               if (z_tkl[i]>z_lr[ii])closeup=res_lr[ii];
               if (z_tkl[i]<z_lr[ii])closedo=res_lr[ii];
	     }
	   }
	   if (closestz != 10000 && closeup != 10000){
             for(int jj=0;jj<2;jj++){
               margin[jj]=fabs(H[i][jj]-(midexpe+(closeup/2)));
               margin[jj+2]=fabs(H[i][jj]-(midexpe-(closeup/2)));
               // 0 and 1  margin RIGHT
               // 2 and 3 margin left
	     }
	   }
	   if (closestz != 10000 && closedo != 10000){
             for(int jj=0;jj<2;jj++){
               margin[jj]=fabs(H[i][jj]-(midexpe+(closedo/2)));
               margin[jj+2]=fabs(H[i][jj]-(midexpe-(closedo/2)));
	     }
	   }
	   maxmargin=10000;
           
             for(int jj=0;jj<4;jj++){
               if (margin[jj] < maxmargin){
		 maxmargin=margin[jj];
                 marinde=jj;
	       }
	     }
             if (closeup != 10000 && maxmargin <= (closeup/2.6) ){
               if (marinde == 0 || marinde == 2){
                 H[i][1]=10000;
	       }
	       if (marinde == 1 || marinde == 3){
                 H[i][0]=10000;
	       }
	     }
             if (closedo != 10000 && maxmargin <= (closedo/2.2) ){
               if (marinde == 0 || marinde == 2){
                 H[i][1]=10000;
	       }
	       if (marinde == 1 || marinde == 3){
                 H[i][0]=10000;
	       }
	     }
             if ((closedo != 10000 && maxmargin > (closedo/2.2) ) || 
                 (closeup != 10000 && maxmargin > (closeup/2.6) ) ||
                  maxmargin > 0.4 ){
	       H[i][1]=10000;
	       H[i][0]=10000;
	     }
	 }
       }

    }
  }

  } // ia+1 >=3


  for(int i=0;i<TKDBc::nlay();i++){
      const int maxflpat=100;
      double TH[maxflpat];
      int up=min(maxflpat,FLPAT[i]);
      int jn=-1;
      for(int ii=0;ii<maxflpat;ii++){
	TH[ii]=10000;
      }

    if (FLPAT[i]==2){
      for(int jo=0;jo<up;jo++){
	if (H[i][jo] != 10000){
	  jn++;
	  TH[jn]=H[i][jo];
	}
      }
      //
      H[i].clear();
      H[i].resize(0);
      
      for(int jo=0;jo<jn+1;jo++){   
	if (TH[jo] != 10000){
	  H[i].push_back(TH[jo]);
	}  
      }
      // 
    }
    if (FLPAT[i]>=3){
      //
      up=min(maxflpat,FLPAT[i]);
      //
      for(int ii=0;ii<maxflpat;ii++){
	TH[ii]=10000;
      }
      jn=-1;
      for(int jo=0;jo<up;jo++){
	if (l1_[i] == 10000 && l2_[i] == 10000 ){
	  if (H[i][jo] != 10000){
	    jn++;
	    TH[jn]=H[i][jo];
	  }
	}

	if (l1_[i] != 10000 && l2_[i] != 10000 ){
	  if (H[i][jo] != H[i][l1_[i]] && H[i][jo] != H[i][l2_[i]] ){
	    H[i][jo]=10000;
	  }
	  if (H[i][jo] != 10000){
	    jn++;
	    TH[jn]=H[i][jo];
	  }
	}
      }
   //
      H[i].clear();
      H[i].resize(0);

      for(int jo=0;jo<jn+1;jo++){   
	if (TH[jo] != 10000){
	  H[i].push_back(TH[jo]);
	}  
      }
      //

    }
  }
 //


}


//////////////////////////////////






void AMSTrTrackGamma::_LSQP2(integer FLPAT[], vector<double> H[], integer esc_1, integer esc_2){

  
  double z_tkl[trconst::maxlay];
  
  for(int i=0;i<TKDBc::nlay();i++){
    z_tkl[i]=TKDBc::zposl(i);
  }
  // 
  int ja;
  const int maxsize=500;
  double res_[trconst::maxlay];
  double mid[trconst::maxlay];
  double z_fi[trconst::maxlay];
  double res_LR[maxsize][trconst::maxlay];
  
  for(int i=0;i<TKDBc::nlay();i++){
    z_fi[i]=10000;
    res_[i]=10000;
    for(int k=0;k<maxsize;k++){
      res_LR[k][i]=10000;
    }
  }
  double CHI2[maxsize];
  double LCHI2[maxsize];
  double AA0;
  double AA1;
  double AA2;
  double SDW,sdwini;
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
  double besty0=10000;
  double th_mid0=10000;
  double minmid0=10000;

  // per eliminare il tk1 dal conteggio
  if ((esc_1 >= 2 && esc_2 != 0 && FLPAT[0] != 2) ||
      (esc_1 > 2 && esc_2 != 0 && FLPAT[0] == 2)){
    
    // if (esc_1 >= 2 && esc_2 != 0){
    ja=-1;

    for(int i=1;i<TKDBc::nlay();i++){
      if (FLPAT[i]==2){
	ja++;  
	res_[ja]=fabs(H[i][0]-H[i][1]);
	z_fi[ja]=z_tkl[i];
	// for the alternative let's do the centers in y
	mid[ja]=(H[i][0]+H[i][1])/2;
      }
    }
    //
    int j;
    double sdwmax,varmax;
    double zbef,rbef;
    for(int i=0;i<TKDBc::nlay();i++){
      if (FLPAT[i]>=3){
	// e se invece di incrementare ja considerassi sempre ja+1
	ja++;  
	z_fi[ja]=z_tkl[i];
	
	//
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
	    if(j>maxsize)break;
	    res_LR[j][i]=fabs(H[i][i1]-H[i][i2]);
	    res_[ja]=res_LR[j][i];
	    // for the alternative let's do the centers in y
	    mid[ja]=(H[i][i1]+H[i][i2])/2;
	    if (esc_1 >= 3){  
	      dlsqp2me(ja,z_fi,res_,AA0,AA1,AA2,sdwini);
	      dlsqp2me((ja+1),z_fi,res_,AA0,AA1,AA2,SDW);
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
	if (l_[i] == 10000) return;
	if (l_[i] != 10000){
	  res_[ja]=res_LR[l_[i]][i];
	}
	
	if (sdwini < 0.5 && sdwmax > 1.0){
	  for(int jo=0;jo<FLPAT[i];jo++){
	    H[i][jo]=10000;
	  }
	}
	// In realta` non ho controllato che SDW sia sufficientemente piccolo
	// SDW <= 0.32 per esempio ...??..!! 
	
	if (i == 0){ // first plane
	  dlinearme(ja,z_fi,mid,A0,A1,VAR);
	  //
	  if (VAR > 0.01 && sdwini > 1 && FLPAT[0] >= 3){
	    for(int jo=0;jo<FLPAT[0];jo++){
	      H[0][jo]=10000;
	    }
	  }
	  //
	  if (VAR <= 0.001 && FLPAT[0] >= 3){
	    th_mid0=A0+A1*z_tkl[0];
	    minmid0=10000;
	    for(int jo=0;jo<FLPAT[0];jo++){
	      if ((fabs(th_mid0-H[0][jo])) < minmid0){
		minmid0=fabs(th_mid0-H[0][jo]);
		besty0=H[0][jo];
	      }  
	    }
	  }
	}
	//
	if (z_fi[ja] == z_tkl[0] && res_[ja] > 1.1) {
	  //      dlinearme((ja+1),z_fi,mid,A0,A1,VAR);
	  dlinearme(ja,z_fi,mid,A0,A1,VAR);
	  mid[ja]=A0+A1*z_tkl[0];
	  bestdy1=min(fabs(mid[ja]-H[0][l1_[i]]),fabs(mid[ja]-H[0][l2_[i]]));
	  bestflag1=1;
	  if(bestdy1 == fabs(mid[ja]-H[0][l1_[i]])) besty1= H[0][l1_[i]];
	  if(bestdy1 == fabs(mid[ja]-H[0][l2_[i]])) besty1= H[0][l2_[i]];
	}  
	ja--;
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
	  if (th_mid0 != 10000){
	    if (besty0 != 10000 && H[0][jo] != besty0){
	      H[0][jo]=10000;
	    }
	  }
	  //
	  if (th_mid0 == 10000){
	    if (bestflag1 != 0 && H[0][jo] != besty1){
	      H[0][jo]=10000;
	    }
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
 
 

void AMSTrTrackGamma::_CheckZOrder(double re[], double ze[], double ce[], int& ja){

  double aboveZ=10000;
  double belowZ=-10000;
  double abovere=10000;
  double belowre=10000;
  int flagok=0;

  for(int i=0;i<ja;i++){  
    if (ze[i] > ze[ja]) {
      aboveZ=min(ze[i],aboveZ);
    }
    if (ze[i] < ze[ja]) {
      belowZ=max(ze[i],belowZ);
    }
  }

  if (aboveZ!=10000 && belowZ!=-10000){
    for(int i=0;i<ja;i++){
      if (ze[i] > (belowZ-0.5) && ze[i] < (belowZ+0.5)){
        belowre=re[i];
      }
      if (ze[i] > (aboveZ-0.5) && ze[i] < (aboveZ+0.5)){
        abovere=re[i];
      }
    }
  }
  if (re[ja] < belowre && re[ja] > abovere ){
    //OK !
    flagok=1;
  }

  // if it is first plane aboveZ=10000;
  if (aboveZ==10000 && belowZ!=-10000){
    // if (ze[ja] > (z_tkl[0]-0.5) && ze[ja] < (z_tkl[0]+0.5)){
    for(int i=0;i<ja;i++){
      if (ze[i] > (belowZ-0.5) && ze[i] < (belowZ+0.5)){
	if (re[i] > re[ja]){
	  //OK !
	  flagok=1;
	}
     }
   }   
 }
  // if it is last plane belowZ=-10000;
  //  if (ze[ja] > (z_tkl[7]-0.5) && ze[ja] < (z_tkl[7]+0.5)){
  if (belowZ==-10000 && aboveZ!=10000){
    for(int i=0;i<ja;i++){
     if (ze[i] > (aboveZ-0.5) && ze[i] < (aboveZ+0.5)){
	if (re[i] <= re[ja]){
	   //OK !
	  flagok=1;
 	}
     }
   }   
 }




  int iu=-1;
  if (flagok != 1){
    ze[ja]=10000;
    re[ja]=10000;
    ce[ja]=10000;
    ja--;
  }


  
}



void AMSTrTrackGamma::_CheckInvReswrtZ(double re[], double ze[], double ce[], int& ja){

  double res_3[trconst::maxlay];
  double z_fi3[trconst::maxlay];
  double mid3[trconst::maxlay];

    for(int i=0;i<TKDBc::nlay();i++){
      z_fi3[i]=10000;
      res_3[i]=10000;
      mid3[i]=10000;
    }


    //////
    int iy=-1;
    double res_4[trconst::maxlay];
    double z_fi4[trconst::maxlay];
    double mid4[trconst::maxlay];
    double z_tkl[trconst::maxlay];
    for(int i=0;i<TKDBc::nlay();i++){
      z_tkl[i]=TKDBc::zposl(i);
    }
    for(int i=0;i<TKDBc::nlay();i++){
      z_fi4[i]=10000;
      res_4[i]=10000;
      mid4[i]=10000;
    }
    for(int j=0;j<TKDBc::nlay();j++){
      for(int i=0;i<ja+1;i++){     
	if (ze[i] > (z_tkl[j]-0.5) && ze[i] < (z_tkl[j]+0.5)){
	  iy++;
	  z_fi4[iy]=ze[i];
	  res_4[iy]=re[i];
	  mid4[iy]=ce[i];
	}
      }
    }
    // questi sono ordinati secondo z decrescente


    /////
  //??$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  double ce2[8][2];
  double ce3[8][3];
  for(int i=0;i<8;i++){
    for(int j=0;j<2;j++){
      ce2[i][j]=10000;
    }
    for(int j=0;j<3;j++){
      ce3[i][j]=10000;
    }
  }

  int il2=-1;
  //first of all pairs of res
  for(int i=0;i<iy;i++){
    il2++;
    ce2[il2][0]=res_4[i];
    ce2[il2][1]=res_4[i+1];
  }
  
  
  
  int vecgb2[8]={1,1,1,1,1,1,1,1};
  int vecgb3[8]={1,1,1,1,1,1,1,1};
  int nbad2=0;
  int nbad3=0;
  for(int i=0;i<il2+1;i++){
    if (ce2[i][0]>ce2[i][1]){
      vecgb2[i]=0;
      nbad2++;

    }
  }
  ////////////// proviamo
  
  for(int i=1;i<il2;i++){
    if (vecgb2[i]==0 && vecgb2[i+1]==1 && vecgb2[i-1]==1){
      for(int j=0;j<(ja+1);j++){
	if (re[j] == ce2[i][1]){
          ze[j]=10000;
	  re[j]=10000;
	  ce[j]=10000;
	}
	} 
	
      }

    }
    


 double vecy[10],vecz[10];
 for(int i=0;i<10;i++){
   vecy[i]=10000;
   vecz[i]=10000;
 }
 double aa0,aa1,aa2,sdwa;
 double ab0,ab1,ab2,sdwb;
 double temp=10000;
 if (vecgb2[0]==0 && nbad2==1 && il2+1 >= 4 ){

   // first
   vecy[0]=ce2[0][0];
   vecz[0]=ze[0]; 
   for(int i=1;i<il2+1;i++){
     vecy[i]=ce2[i][0];
     vecz[i]=ze[i];
   }
   dlsqp2me(il2+1,vecz,vecy,aa0,aa1,aa2,sdwa);
   //second
   vecy[0]=ce2[0][1];
   vecz[0]=ze[1];
   for(int i=1;i<il2+1;i++){
     vecy[i]=ce2[i][1];
     vecz[i]=ze[i+1];    
   }
   dlsqp2me(il2+1,vecz,vecy,ab0,ab1,ab2,sdwb);
    if ((ab0+ab1*ze[0]+(ab2*pow(ze[0],2)))*(aa0+aa1*ze[1]+(aa2*pow(ze[1],2))) < 0){
      temp=(ab0+ab1*ze[0]+(ab2*pow(ze[0],2))<0)?ce2[0][1]:ce2[0][0];
    }
   if ((ab0+ab1*ze[0]+(ab2*pow(ze[0],2)))*(aa0+aa1*ze[1]+(aa2*pow(ze[1],2))) > 0){
    temp=(sdwa<sdwb)?ce2[0][1]:ce2[0][0];
   }
   if (temp != 10000){
      for(int j=0;j<(ja+1);j++){
	if (re[j] == temp){
          ze[j]=10000;
	  re[j]=10000;
	  ce[j]=10000;
	}
      }   
   }
 }
 for(int i=0;i<10;i++){
   vecy[i]=10000;
   vecz[i]=10000;
 }
temp=10000;

 if (nbad2==1 && vecgb2[il2]==0 && il2+1 >= 4){
   // first
   vecy[il2]=ce2[il2][0];
   vecz[il2]=ze[il2]; 
   for(int i=0;i<il2;i++){
     vecy[i]=re[i];
     vecz[i]=ze[i];
   }
   dlsqp2me(il2+1,vecz,vecy,aa0,aa1,aa2,sdwa);
   //second
   vecy[il2]=ce2[il2][1];
   vecz[il2]=ze[il2+1];
   for(int i=0;i<il2;i++){
     vecy[i]=re[i];
     vecz[i]=ze[i];    
   }
   dlsqp2me(il2+1,vecz,vecy,ab0,ab1,ab2,sdwb);
    if ((ab0+ab1*ze[il2]+(ab2*pow(ze[il2],2)))*(aa0+aa1*ze[il2+1]+(aa2*pow(ze[il2+1],2))) < 0){
      temp=(ab0+ab1*ze[il2]+(ab2*pow(ze[il2],2))<0)?ce2[il2][1]:ce2[il2][0];
    }
   if ((ab0+ab1*ze[il2]+(ab2*pow(ze[il2],2)))*(aa0+aa1*ze[il2+1]+(aa2*pow(ze[il2+1],2))) > 0){
    temp=(sdwa<sdwb)?ce2[il2][1]:ce2[il2][0];
   }
   if (temp != 10000){
      for(int j=0;j<(ja+1);j++){
	if (re[j] == temp){
          ze[j]=10000;
	  re[j]=10000;
	  ce[j]=10000;
	}
      }   
   }
 }

 


    
  int il3=-1;
  //first of all pairs of res
  for(int i=0;i<iy-1;i++){
    il3++;
    ce3[il3][0]=res_4[i];
    ce3[il3][1]=res_4[i+1];
    ce3[il3][2]=res_4[i+2];
  }
  
 //
  for(int i=0;i<TKDBc::nlay();i++){
    z_fi4[i]=10000;
    res_4[i]=10000;
    mid4[i]=10000;
  }
  
  iy=-1;
    for(int j=0;j<TKDBc::nlay();j++){
      for(int i=0;i<ja+1;i++){     
        if (re[i] != 10000){
	if (ze[i] > (z_tkl[j]-0.5) && ze[i] < (z_tkl[j]+0.5)){
	  iy++;
	  z_fi4[iy]=ze[i];
	  res_4[iy]=re[i];
	  mid4[iy]=ce[i];
	}
	}
      }
    }



  
  
 //??$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
  int iu=-1;
  //

  for(int i=0;i<iy;i++){
    if (res_4[i+1] >= res_4[i]){        
      iu++;
      z_fi3[iu]=z_fi4[i];
      res_3[iu]=res_4[i];
      mid3[iu]=mid4[i];
    }
  }

  for(int i=0;i<iy;i++){
    // changed by VC 24-mar-2003 
    if (iu>=0 && z_fi3[iu] == z_fi4[i]){
      if (res_4[i+1] >= res_4[i]){
	z_fi3[iu+1]=z_fi4[i+1];
	res_3[iu+1]=res_4[i+1];
        mid3[iu+1]=mid4[i+1];
      }
    }
  }
  iu++;
    ja = iu;
    for(int i=0;i<TKDBc::nlay();i++){
      ze[i]=10000;
      re[i]=10000;
      ce[i]=10000;
    }
    for(int i=0;i<(ja+1);i++){
	re[i]=res_3[i];
	ze[i]=z_fi3[i];
	ce[i]=mid3[i];
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
	 
	 xambig=0; 
	 for (AMSTrRecHit * pcha=AMSTrRecHit::gethead(inhi); pcha!=NULL; pcha=pcha->next()){
	   if(pcha->checkstatus(AMSDBc::FalseX) || pcha->checkstatus(AMSDBc::FalseTOFX))continue;
	   depos = pcha->getsum(); 
	   pc_hi = pcha->getHit();
	   if (pc_hi[1]==p_hi[1] && pc_hi[2]==p_hi[2]){ // ambigui solo in x e depos
	     if (depos == cha){ // same depos let's count x ambiguity 
	       xambig++;  
	     }
	     //
	   }
	 }
	 
	 if (CEN==10000){
	   pre->setstatus(AMSDBc::GAMMARIGHT);
	   pre->setstatus(AMSDBc::GAMMALEFT);
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
	     
	   } 
	   if (p_hi[1]<=CEN){
	     pre->clearstatus(AMSDBc::GAMMARIGHT);
	     pre->clearstatus(AMSDBc::GAMMALEFT);
	     pre->setstatus(AMSDBc::GAMMALEFT);
	     
	     if (xambig <= 2){ // too low ambiguty
	       pre->clearstatus(AMSDBc::GAMMALEFT);
	     } 
	     //        
	     
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
   
   //   AMSmceventg::PrintSeeds(cout);
   
   for(int i=0;i<TKDBc::nlay();i++){
     AMSTrRecHit::_markDouble(H[i],i);
     FLPAT[i]=H[i].size(); 
     if (i==0){
       if (FLPAT[i]>2){
	 H[i]=_TK1YRemoveTRD2TOF(H[i],FLPAT[i]);
	 FLPAT[i]=H[i].size();
	 
       }
     }
   }
   
   //   if (FLPAT[7] == 2 || FLPAT[0] == 2){
   //  _LookOneEight(FLPAT,H);
   //}
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
   if (esc_1 <= 2 && esc_0 >= 2 && es_f1 >= 3) return 0;
   if ((es_f1 + esc_0) >= 6) return 0;
   if (esc_10 >= 3)  return 0;
   if (esc_20 != 0)  return 0;
   
   
SetBestResLR(FLPAT,H,esc_1,esc_2,es_f1);
   
   //   _LSQP2(FLPAT,H,esc_1,esc_2);
   
 int rechecking=0;
 for(int i=0;i<TKDBc::nlay();i++){
   FLPAT[i]=H[i].size();
   if (FLPAT[i]>2) {
   rechecking=1;
   }
 }



 if (rechecking == 1){
 esc_1=0;
 esc_2=0;   for(int i=0;i<TKDBc::nlay();i++){
     if  (FLPAT[i]==2) esc_1++;
     if  (FLPAT[i]>2)  esc_2++;
   }
      _LSQP2(FLPAT,H,esc_1,esc_2);

   for(int i=0;i<TKDBc::nlay();i++){
     FLPAT[i]=H[i].size();
     if (FLPAT[i]>2) {
       return 0;
     }
     //
   }
 }

   //second time
   if (FLPAT[7] == 2 || FLPAT[0] == 2){
_LookOneEight(FLPAT,H);
   }
   
   for(int i=0;i<TKDBc::nlay();i++){
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
_LeftRight(H[i],i,CE[i]);
}
_SingleCommonHit(FLPAT,H);
   
XZLine_TOF RoadXZ(ii,neca,tm34,ecc);
   
   if (ii[0] == 0 && ii[1] == 0) return 0;
   
   double slo,inte,chit;
   
   if (ii[0] == 1 && ii[1] == 1 && ii[2] == 1 && ii[3] == 1) ideaflag=1;
   
   if (ideaflag==0){
     RoadXZ.TopSplash(tm34);
     RoadXZ.getTofMul(1,ii,tm34);
   }
   RoadXZ.Check_TRD_TK1(FLPAT[0],H[0],ii);
   RoadXZ.getTofMul(2,ii,tm34);
   //
   RoadXZ.makeEC_out(ecc,out);
   if (out==0){
     RoadXZ.Lines_Top_Bottom(out);
   }
   
   RoadXZ.Lines_Top_Bottom(out);
   if (out==0) return 0;
   if (out==1){
     RoadXZ.getParRoadXZ(fbotf,ftopf,x_starf,z_starf,SLOPEf,INTERf);
     RoadXZ.LastCheckTRDX(slo,inte,chit);
     if (chit <= 0.3 && chit > 0.001 ) {
       ftopf=1;
       fbotf=0;
       SLOPEf=slo;
       INTERf=inte;
     }
   }
   
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
   if (init_R==0 || init_L==0) return 0;
   if (firR == lasR || firL == lasL) return 0;
   
   
   
   int xsR=0;
   int xsL=0;
   int xsRL=0;
   for(int i=0;i<TKDBc::nlay();i++){
     xsRL+=FLPAT[i];
   }
   
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
   if (INDEXPL[i] < FLPAT[i]) recowrong++;
 }
 for(int i=0;i<TKDBc::nlay();i++){
   INDEXPL[i]=0;
 }
 
 if (slr != 10000 && qlr != 10000){
   
   for(int i=0;i<TKDBc::nlay();i++){
_LeftRight(H[i],i,CE[i]);
   } 
_SingleCommonHit(FLPAT,H);
   if (slr != 200 && qlr != 200){
     SLOPEf=slr;
     INTERf=qlr;
   }
   
   
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
	 for (AMSTrRecHit * p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
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
	 for (AMSTrRecHit * p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
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
       //
       if (nleft < 3 || nright < 3) return 0;
       if (nleft ==3 && nright ==3) return 0;
       //
       
       
       AMSTrTrackGamma * pntLR=   new AMSTrTrackGamma(nleft,nright,parrayL,parrayR,recowrong);
       
       int counting=0;
       int plusminus=0;
       if (nleft >=3 && nright >= 3){
	 pntLR->Fit(5,2);
	 counting=0;
	 plusminus=0;
	 pntLR->PAIR2GAMMA(counting,plusminus);
       }
       
       
       
       double res_d[trconst::maxlay];
       for(int i=0;i<TKDBc::nlay();i++){
	 res_d[i]=0;
       } 
       
       int flag=0;
       
       
       if (nleft >= 3){
	 NGammaFound++;
       }
       if (nright >= 3){
	 NGammaFound++;
       }
       if (nleft >=3 && nright >= 3){
	 pntLR->Fit(0,2);
	 pntLR->Fit(3,2);
	 pntLR->Fit(4,2);
       }
       else return 0;
       
       
       AMSEvent::gethead()->addnext(AMSID("AMSTrTrackGamma",0),pntLR);
       int done=0;
       pntLR->addtracks(done);
       
       // VC gamma
       
       
       {
	 AMSTrTrackGamma  *p= new AMSTrTrackGamma(*pntLR);
	 p->_ConstructGamma(0);
	 AMSEvent::gethead()->addnext(AMSID("AMSTrTrackGamma",0),p);
       }
       {
	 AMSTrTrackGamma  *p= new AMSTrTrackGamma(*pntLR);
	 p->_ConstructGamma(2);
	 AMSEvent::gethead()->addnext(AMSID("AMSTrTrackGamma",0),p);
       }
       
       
  }// (init_R!=0 && init_L!=0)
     
   } // xsRL
   // :=====================-
   
   return NGammaFound;
 }
 
 
 
 void AMSTrTrackGamma::getFFParam( number& Chi2, number& Rigi, number& Erigi, number& Thetaff,  number& Phiff,  AMSPoint& X0)const
   {Chi2=_Chi2FastFitL;Rigi=_RidgidityL;Erigi=_ErrRidgidityL;Thetaff=_ThetaL;Phiff=_PhiL;X0=_P0L;}
 
 
 
 void AMSTrTrackGamma::_copyEl(){
/*
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
*/
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


void AMSTrTrackGamma::RecoXRight(double X2P, double X3P, double DeltaRecoTop, double DeltaRecoBottom, int ifi, int ila, double RegStr0, int & lkfir, double & VAMAXR){


AMSTrRecHit * p;
AMSPoint p_hi;
double VA0MAX=10000;
AMSPoint phii[8];
AMSTrRecHit * pu[8];
double M_X,Q_X,M_Y,Q_Y,Str,Circ;  
double RegStr;
RegStr=0;
 double A0,B0,VA0;
 int lk=-1;
 lkfir=0;

 double ptryx[8]={10000,10000,10000,10000,10000,10000,10000,10000};
 double ptryy[8]={10000,10000,10000,10000,10000,10000,10000,10000};
 double ptryz[8]={10000,10000,10000,10000,10000,10000,10000,10000};
 double xamr[8]={10000,10000,10000,10000,10000,10000,10000,10000};
 double yamr[8]={10000,10000,10000,10000,10000,10000,10000,10000};
 double zamr[8]={10000,10000,10000,10000,10000,10000,10000,10000};


 for (pu[ifi]=AMSTrRecHit::gethead(ifi); pu[ifi]!=NULL; pu[ifi]=pu[ifi]->next()){
   if(pu[ifi]->checkstatus(AMSDBc::FalseX) || pu[ifi]->checkstatus(AMSDBc::FalseTOFX))continue;
   if (pu[ifi]->checkstatus(AMSDBc::GAMMARIGHT)){
     phii[ifi] = pu[ifi]->getHit(); 
     if (phii[ifi][0] <= X2P+DeltaRecoTop && phii[ifi][0] >= X2P-DeltaRecoTop){
	
         for (AMSTrRecHit * pcha=AMSTrRecHit::gethead(ifi); pcha!=NULL; pcha=pcha->next()){
	    pcha->clearstatus(AMSDBc::TOFFORGAMMA);
	  }

       pu[ifi]->setstatus(AMSDBc::TOFFORGAMMA);

       for (pu[ila]=AMSTrRecHit::gethead(ila); pu[ila]!=NULL; pu[ila]=pu[ila]->next()){
	 if(pu[ila]->checkstatus(AMSDBc::FalseX) || pu[ila]->checkstatus(AMSDBc::FalseTOFX))continue;
	 if (pu[ila]->checkstatus(AMSDBc::GAMMARIGHT)){
	   phii[ila] = pu[ila]->getHit(); 
	   if (phii[ila][0] <= X3P+DeltaRecoBottom && phii[ila][0] >= X3P-DeltaRecoBottom){


		    for (AMSTrRecHit * pcha=AMSTrRecHit::gethead(ila); pcha!=NULL; pcha=pcha->next()){
		      pcha->clearstatus(AMSDBc::TOFFORGAMMA);
		    }
		    pu[ila]->setstatus(AMSDBc::TOFFORGAMMA);

	     AMSTrTrackGamma::_intercept(phii[ifi][0],phii[ifi][1],phii[ifi][2],phii[ila][0],phii[ila][1],phii[ila][2],M_X,Q_X,M_Y,Q_Y); 
	     //
	     for (int i=ifi+1;i<ila;i++){
	       RegStr=RegStr0;
	       
	       for (pu[i]=AMSTrRecHit::gethead(i); pu[i]!=NULL; pu[i]=pu[i]->next()){
		 if(pu[i]->checkstatus(AMSDBc::FalseX) || pu[i]->checkstatus(AMSDBc::FalseTOFX))continue;
		 if (pu[i]->checkstatus(AMSDBc::GAMMARIGHT)){
		   
		   phii[i] = pu[i]->getHit(); 

		   AMSTrTrackGamma::_DISTANCE(phii[i][0],phii[i][1],phii[i][2],M_X,Q_X,M_Y,Q_Y,Str,Circ);
		   if (fabs(Str) <= RegStr){
		     RegStr=fabs(Str);
		     
		     for (AMSTrRecHit * pcha=AMSTrRecHit::gethead(i); pcha!=NULL; pcha=pcha->next()){
		       pcha->clearstatus(AMSDBc::TOFFORGAMMA);
		     }
		     pu[i]->setstatus(AMSDBc::TOFFORGAMMA);
		   }
		 }			    
	       }
	     }
	     lk=0;
	     for (int ii=0;ii<TKDBc::nlay();ii++){
	       for (p=AMSTrRecHit::gethead(ii); p!=NULL; p=p->next()){
		 if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
		 p_hi = p->getHit(); 
		 if (p->checkstatus(AMSDBc::TOFFORGAMMA)){
		   ptryx[lk]=p_hi[0];
		   ptryy[lk]=p_hi[1];
		   ptryz[lk]=p_hi[2];
		   lk++;
		 }  
	       }
	     }
	     if (lk < 3) continue;
	     dlinearme(lk,ptryz,ptryx,A0,B0,VA0);
	     if (VA0 < VA0MAX){
	       VA0MAX=VA0;
	       lkfir=lk; 
	       for (int ii=0;ii<lk;ii++){
		 zamr[ii]=ptryz[ii];
		 xamr[ii]=ptryx[ii];
		 yamr[ii]=ptryy[ii];  
		 
	       }
	     }
	     
	   }
	 }
       }
     }
   }
 }

 VAMAXR=VA0MAX;

  /////////////////////////////////////////////////

 double A,B,VA;
 double dxheo[8];
 double maxz,maxy,maxx;
 double prendo;
 double xxx2,zzz2,yyy2;
 double xmi2[8],zmi2[8],ymi2[8];
 double xmi3[8],zmi3[8],ymi3[8];
 for (int i=0;i<8;i++){
  xmi2[i]=10000;
  zmi2[i]=10000;
  ymi2[i]=10000;
  xmi3[i]=10000;
  zmi3[i]=10000;
  ymi3[i]=10000;
 }
 prendo=10000;
 xxx2=10000;
 zzz2=10000;
 yyy2=10000;



/////////////********************
 double maxdx=0;
 double mindx=10000; 
 double mi[8],zmi[8],xmi[8],zmin,xmin,xxx;
 double mii[8],zmii[8],xmii[8];
 double zmax,xmax;
 for (int i=0;i<8;i++){
   mi[i]=10000;
  xmi[i]=10000;
  zmi[i]=10000;
 }

 if (lkfir>2){
 dlinearme(lkfir,zamr,xamr,A,B,VA);         // linear fit

 for (int i=0;i<lkfir;i++){
   dxheo[i]=fabs(xamr[i]-(B*zamr[i])-A);
 }

 }

 if (VAMAXR > 0.5){
  if (lkfir > 4){

mindx=10000; 
 for (int i=0;i<lkfir;i++){
   if (dxheo[i] < mindx){
     mindx=dxheo[i];
     zmin=zamr[i];
     xmin=xamr[i];
   }
 }

 mi[0]=mindx;
 zmi[0]=zmin;
 xmi[0]=xmin;
 for (int j=1;j<4;j++){
   for (int i=0;i<lkfir;i++){
     if (dxheo[i] > mi[j-1] && dxheo[i] < mi[j]){
       mi[j]=dxheo[i]; 
      zmi[j]=zamr[i];
      xmi[j]=xamr[i];
     }
   }
 }

 dlinearme(4,zmi,xmi,A,B,VA);         // linear fit


   ///////////////////
 if (VA > 0.3){

    // togli il residuo piu` alto
  maxdx=0;
  for (int i=0;i<4;i++){
   if (mi[i] > maxdx){
     maxdx=mi[i];
     zmax=zmi[i];
     xmax=xmi[i];
   }
 }
  //annullo xzmii
 for (int i=0;i<8;i++){
   mii[i]=10000;
  xmii[i]=10000;
  zmii[i]=10000;
 }

 int jl=-1;
 for (int i=0;i<4;i++){
   if (mi[i] < maxdx){
     jl++;
       mii[jl]=mi[i]; 
      zmii[jl]=zmi[i];
      xmii[jl]=xmi[i];
     }
 }

 

 dlinearme(3,zmii,xmii,A,B,VA);         // linear fit

 }
  ///////////////////


  }//lkfir > 4

  /////////////////////
 if (lkfir == 4){
   // togli il residuo piu` alto
  maxdx=0;
  for (int i=0;i<lkfir;i++){
   if (dxheo[i] > maxdx){
     maxdx=dxheo[i];
     zmax=zamr[i];
     xmax=xamr[i];
   }
 }
 for (int j=0;j<3;j++){
   for (int i=0;i<lkfir;i++){
     if (dxheo[i] < maxdx){
       mi[j]=dxheo[i]; 
      zmi[j]=zamr[i];
      xmi[j]=xamr[i];
     }
   }
 }

 dlinearme(3,zmi,xmi,A,B,VA);         // linear fit

  }//lkfir == 4
 //////////////////
} // VAMAXR > 1
/////////////////////////////////////////
// for both: Looking for missing X-hit:  (VAMAXR < 1) and (VAMAXR >= 1)
// added on 10-febbraio
 if (lkfir>2){


 for (int i=0;i<TKDBc::nlay();i++){
   xxx=B*TKDBc::zposl(i)+A;  

 //
 prendo=10000;
 xxx2=10000;
 zzz2=10000;
 yyy2=10000;
 for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
   if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
   p_hi = p->getHit(); 
   for (int ii=0;ii<lkfir;ii++){
     if (p_hi[1]==yamr[ii]   && p_hi[2]==zamr[ii]){
       if (fabs(p_hi[0]-xxx) < prendo){
	 prendo=fabs(p_hi[0]-xxx);
	 xxx2=p_hi[0];
	 zzz2=zamr[ii];
         yyy2=p_hi[1];
       }
     }
   }
 }
 if (xxx2 == 10000 && zzz2 == 10000 && prendo == 10000){
   for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     p_hi = p->getHit(); 
     if (p->checkstatus(AMSDBc::GAMMARIGHT)){
       if (fabs(p_hi[0]-xxx) < prendo){
         if (fabs(p_hi[0]-xxx) < 0.9){
	 prendo=fabs(p_hi[0]-xxx);
	 xxx2=p_hi[0];
	 zzz2=p_hi[2];
         yyy2=p_hi[1];
	 }
       }
     }
   }
 }

 xmi2[i]=xxx2;
 zmi2[i]=zzz2;
 ymi2[i]=yyy2;
 }

 //



  ///////////////////////////////////////////////////


 int lc=-1;
 for (int i=0;i<TKDBc::nlay();i++){
  if (xmi2[i] != 10000 && ymi2[i] != 10000 && zmi2[i] != 10000){ 
    lc++;
    xmi3[lc]= xmi2[i];
    ymi3[lc]= ymi2[i];
    zmi3[lc]= zmi2[i];
  }
 }

 dlinearme(lc+1,zmi3,xmi3,A,B,VA);         // linear fit

 if ((VAMAXR < 1  && VA <= 1) ||
     (VAMAXR >= 1 && VA < VAMAXR)){

   VAMAXR=VA;
   lkfir=lc+1;
 for (int i=0;i<8;i++){
  xamr[i]=10000;
  zamr[i]=10000;
  yamr[i]=10000;
 }
   for (int i=0;i<lkfir;i++){
     xamr[i]=xmi3[i];
     yamr[i]=ymi3[i];
     zamr[i]=zmi3[i];
   }


} // VAMAXR 

 /////////////////////////////////////////////////

  for (int i=0;i<TKDBc::nlay();i++){
   for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
	if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
	p_hi = p->getHit(); 
        p->clearstatus(AMSDBc::TOFFORGAMMA); 
        for (int ii=0;ii<lkfir;ii++){
	  if (p_hi[0]==xamr[ii]  && p_hi[1]==yamr[ii]   && p_hi[2]==zamr[ii]){
            p->setstatus(AMSDBc::TOFFORGAMMA);
	  }
	}
   }
  }


 }//// lkfir >2



}

void AMSTrTrackGamma::RecoXLeft(double X2P, double X3P, double DeltaRecoTop, double DeltaRecoBottom, int ifi, int ila, double RegStr0, int & lkfil, double & VAMAXL){


AMSTrRecHit * p;
AMSPoint p_hi;
double VA0MAX=10000;
AMSPoint phii[8];
AMSTrRecHit * pu[8];
double M_X,Q_X,M_Y,Q_Y,Str,Circ;  
double RegStr;
RegStr=0;
 double A0,B0,VA0;
 int lk=-1;
 lkfil=0;
 double ptryx[8]={10000,10000,10000,10000,10000,10000,10000,10000};
 double ptryy[8]={10000,10000,10000,10000,10000,10000,10000,10000};
 double ptryz[8]={10000,10000,10000,10000,10000,10000,10000,10000};

 double xaml[8]={10000,10000,10000,10000,10000,10000,10000,10000};
 double yaml[8]={10000,10000,10000,10000,10000,10000,10000,10000};
 double zaml[8]={10000,10000,10000,10000,10000,10000,10000,10000};


 for (int i=0;i<8;i++){
  xaml[i]=10000;
  yaml[i]=10000;
  zaml[i]=10000;
 }

 for (pu[ifi]=AMSTrRecHit::gethead(ifi); pu[ifi]!=NULL; pu[ifi]=pu[ifi]->next()){
   if(pu[ifi]->checkstatus(AMSDBc::FalseX) || pu[ifi]->checkstatus(AMSDBc::FalseTOFX))continue;
   if (pu[ifi]->checkstatus(AMSDBc::GAMMALEFT)){
     phii[ifi] = pu[ifi]->getHit(); 
     if (phii[ifi][0] <= X2P+DeltaRecoTop && phii[ifi][0] >= X2P-DeltaRecoTop){
       
       for (AMSTrRecHit * pcha=AMSTrRecHit::gethead(ifi); pcha!=NULL; pcha=pcha->next()){
	 pcha->clearstatus(AMSDBc::TOFFORGAMMA);
       }
       pu[ifi]->setstatus(AMSDBc::TOFFORGAMMA);

	 for (pu[ila]=AMSTrRecHit::gethead(ila); pu[ila]!=NULL; pu[ila]=pu[ila]->next()){
	   if(pu[ila]->checkstatus(AMSDBc::FalseX) || pu[ila]->checkstatus(AMSDBc::FalseTOFX))continue;
	   if (pu[ila]->checkstatus(AMSDBc::GAMMALEFT)){
	     phii[ila] = pu[ila]->getHit(); 
	     if (phii[ila][0] <= X3P+DeltaRecoBottom && phii[ila][0] >= X3P-DeltaRecoBottom){
	       //
		       
		    for (AMSTrRecHit * pcha=AMSTrRecHit::gethead(ila); pcha!=NULL; pcha=pcha->next()){
		      pcha->clearstatus(AMSDBc::TOFFORGAMMA);
		    }
		    pu[ila]->setstatus(AMSDBc::TOFFORGAMMA);


	       AMSTrTrackGamma::_intercept(phii[ifi][0],phii[ifi][1],phii[ifi][2],phii[ila][0],phii[ila][1],phii[ila][2],M_X,Q_X,M_Y,Q_Y); 
	       //
	       for (int i=ifi+1;i<ila;i++){
		 RegStr=RegStr0;
		 for (pu[i]=AMSTrRecHit::gethead(i); pu[i]!=NULL; pu[i]=pu[i]->next()){
		   if(pu[i]->checkstatus(AMSDBc::FalseX) || pu[i]->checkstatus(AMSDBc::FalseTOFX))continue;
		   if (pu[i]->checkstatus(AMSDBc::GAMMALEFT)){

		     phii[i] = pu[i]->getHit(); 
		     AMSTrTrackGamma::_DISTANCE(phii[i][0],phii[i][1],phii[i][2],M_X,Q_X,M_Y,Q_Y,Str,Circ);

		     if (fabs(Str) <= RegStr){
		       RegStr=fabs(Str);       
		       for (AMSTrRecHit * pcha=AMSTrRecHit::gethead(i); pcha!=NULL; pcha=pcha->next()){
			 pcha->clearstatus(AMSDBc::TOFFORGAMMA);
		       }
		       pu[i]->setstatus(AMSDBc::TOFFORGAMMA);
		     }
		   }			    
		 }
	       }
	       lk=0;
	       for (int ii=0;ii<TKDBc::nlay();ii++){
		 for (p=AMSTrRecHit::gethead(ii); p!=NULL; p=p->next()){
		   if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
		   p_hi = p->getHit(); 
		   if (p->checkstatus(AMSDBc::TOFFORGAMMA)){
		     ptryx[lk]=p_hi[0];
		     ptryy[lk]=p_hi[1];
		     ptryz[lk]=p_hi[2];
		     lk++;
		   }  
		 }
	       }
	       if (lk < 3) continue;
	       dlinearme(lk,ptryz,ptryx,A0,B0,VA0);
	       if (VA0 < VA0MAX){
		 VA0MAX=VA0;
		 lkfil=lk; 
		 for (int ii=0;ii<lk;ii++){
		   zaml[ii]=ptryz[ii];
		   xaml[ii]=ptryx[ii];
		   yaml[ii]=ptryy[ii];  
		   
		 }
	       }
	     }   
	   }
	 }
       }
     }
   }


                      


 VAMAXL=VA0MAX;
  /////////////////////////////////////////////////

  //////////////


 double A,B,VA;

 double dxheo[8];
 double maxz,maxy,maxx;
 double prendo;
 double xxx2,zzz2,yyy2;
 double xmi2[8],zmi2[8],ymi2[8];
 double xmi3[8],zmi3[8],ymi3[8];
 for (int i=0;i<8;i++){
  xmi2[i]=10000;
  zmi2[i]=10000;
  ymi2[i]=10000;
  xmi3[i]=10000;
  zmi3[i]=10000;
  ymi3[i]=10000;
 }
 prendo=10000;
 xxx2=10000;
 zzz2=10000;
 yyy2=10000;


 double miii[8],zmiii[8],xmiii[8];
/////////////********************
 double maxdx=0;
 double mindx=10000; 
 double mi[8],zmi[8],xmi[8],zmin,xmin,xxx;
 double mii[8],zmii[8],xmii[8];
 double zmax,xmax;
 for (int i=0;i<8;i++){
   mi[i]=10000;
  xmi[i]=10000;
  zmi[i]=10000;
 }

 if (lkfil>2){
 dlinearme(lkfil,zaml,xaml,A,B,VA);         // linear fit
 for (int i=0;i<lkfil;i++){
   dxheo[i]=fabs(xaml[i]-(B*zaml[i])-A);
 }

}

 //


if (VAMAXL > 0.5){
  if (lkfil > 4){
    //   VA=10000;
mindx=10000; 
 for (int i=0;i<lkfil;i++){
   if (dxheo[i] < mindx){
     mindx=dxheo[i];
     zmin=zaml[i];
     xmin=xaml[i];
   }
 }

 mi[0]=mindx;
 zmi[0]=zmin;
 xmi[0]=xmin;
 for (int j=1;j<4;j++){
   for (int i=0;i<lkfil;i++){
     if (dxheo[i] > mi[j-1] && dxheo[i] < mi[j]){
       mi[j]=dxheo[i]; 
      zmi[j]=zaml[i];
      xmi[j]=xaml[i];
     }
   }
 }

 dlinearme(4,zmi,xmi,A,B,VA);         // linear fit

   ///////////////////
 if (VA > 0.3){

    // togli il residuo piu` alto
  maxdx=0;
  for (int i=0;i<4;i++){
   if (mi[i] > maxdx){
     maxdx=mi[i];
     zmax=zmi[i];
     xmax=xmi[i];
   }
 }
  //annullo xzmii
 for (int i=0;i<8;i++){
   mii[i]=10000;
  xmii[i]=10000;
  zmii[i]=10000;
 }

 int jl=-1;
 for (int i=0;i<4;i++){
   if (mi[i] < maxdx){
     jl++;
       mii[jl]=mi[i]; 
      zmii[jl]=zmi[i];
      xmii[jl]=xmi[i];
     }
 }

 dlinearme(3,zmii,xmii,A,B,VA);         // linear fit

 }


  ///////////////////

  }//lkfil > 4
  /////////////////////


  ///////////////////
 if (lkfil == 4){
   // togli il residuo piu` alto
  maxdx=0;
  for (int i=0;i<lkfil;i++){
   if (dxheo[i] > maxdx){
     maxdx=dxheo[i];
     zmax=zaml[i];
     xmax=xaml[i];
   }
 }
 for (int j=0;j<3;j++){
   for (int i=0;i<lkfil;i++){
     if (dxheo[i] < maxdx){
       mi[j]=dxheo[i]; 
      zmi[j]=zaml[i];
      xmi[j]=xaml[i];
     }
   }
 }

 dlinearme(3,zmi,xmi,A,B,VA);         // linear fit

 } // lkfil == 4
 //////////////////
} // VAMAXL > 1
/////////////////////////////////////////
// for both: Looking for missing X-hit:  (VAMAXL < 1) and (VAMAXL > 1)
// added on 10-febbraio
   if (lkfil > 2){

 for (int i=0;i<TKDBc::nlay();i++){
   xxx=B*TKDBc::zposl(i)+A;  
 //
 prendo=10000;
 xxx2=10000;
 zzz2=10000;
 yyy2=10000;
 for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
   if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
   p_hi = p->getHit(); 
   for (int ii=0;ii<lkfil;ii++){
     if (p_hi[1]==yaml[ii]   && p_hi[2]==zaml[ii]){
       if (fabs(p_hi[0]-xxx) < prendo){
	 prendo=fabs(p_hi[0]-xxx);
	 xxx2=p_hi[0];
	 zzz2=zaml[ii];
         yyy2=p_hi[1];
       }
     }
   }
 }
 if (xxx2 == 10000 && zzz2 == 10000 && prendo == 10000){
   for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     p_hi = p->getHit(); 
     if (p->checkstatus(AMSDBc::GAMMALEFT)){
       if (fabs(p_hi[0]-xxx) < prendo){
         if (fabs(p_hi[0]-xxx) < 0.9){
	 prendo=fabs(p_hi[0]-xxx);
	 xxx2=p_hi[0];
	 zzz2=p_hi[2];
         yyy2=p_hi[1];
         }
       }
     }
   }
 }

 xmi2[i]=xxx2;
 zmi2[i]=zzz2;
 ymi2[i]=yyy2;
 }
 
 //



  ///////////////////////////////////////////////////


 int lc=-1;
 for (int i=0;i<TKDBc::nlay();i++){
  if (xmi2[i] != 10000 && ymi2[i] != 10000 && zmi2[i] != 10000){ 
    lc++;
    xmi3[lc]= xmi2[i];
    ymi3[lc]= ymi2[i];
    zmi3[lc]= zmi2[i];
  }
 }

 dlinearme(lc+1,zmi3,xmi3,A,B,VA);         // linear fit

  /////////////////////////////////////////////////

 if ((VAMAXL < 1  && VA <= 1) ||
     (VAMAXL >= 1 && VA < VAMAXL)){
   //
  //
   VAMAXL=VA;
   lkfil=lc+1;
 for (int i=0;i<8;i++){
  xaml[i]=10000;
  zaml[i]=10000;
  yaml[i]=10000;
 }
   for (int i=0;i<lkfil;i++){
     xaml[i]=xmi3[i];
     yaml[i]=ymi3[i];
     zaml[i]=zmi3[i];
   }

} //VAMAXL
 
 /////////////////////////////////////////////////
 
 for (int i=0;i<TKDBc::nlay();i++){
   for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     p_hi = p->getHit(); 
     p->clearstatus(AMSDBc::TOFFORGAMMA); 
     for (int ii=0;ii<lkfil;ii++){
       if (p_hi[0]==xaml[ii]  && p_hi[1]==yaml[ii]   && p_hi[2]==zaml[ii]){
	 p->setstatus(AMSDBc::TOFFORGAMMA);
       }
     }
   }
 }
 
   } // lkfil >2


}


void AMSTrTrackGamma::RecoLeftRight(int& refitting, integer FLPAT[], double   SLOPEf, double  INTERf,double x_starf,
                                    double z_starf,int fbotf,int ftopf,
                                    double firR,double lasR,double firL,double lasL,
                                    int fir_planeR, int  fir_planeL,int las_planeR,int las_planeL,double & slr, double & qlr){




AMSTrRecHit * p;
AMSPoint p_hi;

 double X2P,X3P;
 double DeltaRecoTop=2.5;
 double DeltaRecoBottom=2.5;
 double MinDX2L,MinDX3L;
 double MinDX2R,MinDX3R;
 double valbackup;
 double varbackup;
 // NOW LEFT :


 if (fbotf == 1 && ftopf == 0){
   INTERf=x_starf-(SLOPEf*z_starf);
 }
   X2P=SLOPEf*firL+INTERf;
   X3P=SLOPEf*lasL+INTERf;
 
 MinDX2L=10000;
 MinDX3L=10000;
 double conL_w=0;
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

 //*****************

 int lhi=0;
 double x_d3[3],z_d3[3],y_d3[3];
 double x_d[8]={10000,10000,10000,10000,10000,10000,10000,10000};
 double y_d[8]={10000,10000,10000,10000,10000,10000,10000,10000};
 double z_d[8]={10000,10000,10000,10000,10000,10000,10000,10000};
 double M_X1,Q_X1,VARR;
 double CAMAX;
 double M_X,Q_X,M_Y,Q_Y,Str,Circ;  
 CAMAX=-10000;
 
AMSPoint phii[8];
number depo[8];
AMSTrRecHit * pu[8];

 double A0,B0,VA0;
 int lk=-1;
 int lkfil=0;
 int lo=-1;

 double AFL,BFL,VAMAXL;
 double AFR,BFR,VAMAXR;

 
 double VA0MAX=10000;


 double ptryx[8]={10000,10000,10000,10000,10000,10000,10000,10000};
 double ptryy[8]={10000,10000,10000,10000,10000,10000,10000,10000};
 double ptryz[8]={10000,10000,10000,10000,10000,10000,10000,10000};

 double xaml[8]={10000,10000,10000,10000,10000,10000,10000,10000};
 double yaml[8]={10000,10000,10000,10000,10000,10000,10000,10000};
 double zaml[8]={10000,10000,10000,10000,10000,10000,10000,10000};

 int ifi=0;
 int ila=0;
  for(int i=0;i<TKDBc::nlay();i++){
    if(firL > (TKDBc::zposl(i))-0.5 && firL < (TKDBc::zposl(i))+0.5){
      ifi=i;
    }
    if(lasL > (TKDBc::zposl(i))-0.5 && lasL < (TKDBc::zposl(i))+0.5){
      ila=i;
    }
  }

 double alf,blf,arf,brf;
 double nulf,nurf;
 double RegStr0;
 double RegStr;
 RegStr=0;
 RegStr0=2.5;
 if (refitting){
  RegStr0=2.; 
 }
   /////////000000000000000000//////////////////
 if (fbotf == 1 && ftopf == 0){
   DeltaRecoTop=3.5;
   DeltaRecoBottom=2.5;
 }
 if (fbotf == 0 && ftopf == 1){
 DeltaRecoTop=2.5;
 DeltaRecoBottom=3.5;
 }

 if(refitting == 0 || refitting == 500){

VA0MAX=10000;

////////////////

RecoXLeft(X2P,X3P,DeltaRecoTop,DeltaRecoBottom,ifi,ila,RegStr0,lkfil,VAMAXL);

   if (VAMAXL == 10000){
    if (fbotf == 1 && ftopf == 0){ 
RecoXLeft(X2P,X3P,5.5,1,ifi,ila,1.5,lkfil,VAMAXL); 
    }
    if (fbotf == 0 && ftopf == 1){ 
RecoXLeft(X2P,X3P,1,5.5,ifi,ila,1.5,lkfil,VAMAXL); 
    }
   }

   if (VAMAXL < 0.1 && VAMAXL > 0.05 && lkfil == 8){
RecoXLeft(0,0,10000,10000,ifi,ila,RegStr0,lkfil,VAMAXL);
   }




   if (VAMAXL > 0.08 || lkfil < 3 || (VAMAXL > 0.001 && lkfil == 3)){
RecoXLeft(X2P,X3P,5.5,5.5,ifi,ila,RegStr0,lkfil,VAMAXL);
 }

 if ((VAMAXL > 0.3) || (VAMAXL > 0.08 && lkfil <=4 )){
RecoXLeft(0,0,10000,10000,ifi,ila,RegStr0,lkfil,VAMAXL);

 }


 int ifla=0;
 int ifi2l,ila2l;
 ifi2l=ifi;
 ila2l=ila;
 //
 for (int i=0;i<TKDBc::nlay();i++){
   for (AMSTrRecHit * p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
      if (p->checkstatus(AMSDBc::GAMMALEFT)){
        if(ifla==0 && i > ifi){
	  ifi2l=i;
          ifla=1;
        }
        if(i < ila){
          ila2l=i;
	} 
      }
   }
 }


valbackup=VAMAXL;

 if (VAMAXL > 0.3 && lkfil >= 6){
   if (fbotf == 1 && ftopf == 0){ 
RecoXLeft(0,X3P,10000,DeltaRecoBottom,ifi2l,ila,4,lkfil,VAMAXL);
   if (VAMAXL > 0.3){
RecoXLeft(X2P,0,DeltaRecoTop,10000,ifi,ila,4,lkfil,VAMAXL);
   }
   }
   if (fbotf == 0 && ftopf == 1){ 
RecoXLeft(X2P,0,DeltaRecoTop,10000,ifi,ila2l,4,lkfil,VAMAXL);
   if (VAMAXL > 0.3){
RecoXLeft(0,X3P,10000,DeltaRecoBottom,ifi,ila,4,lkfil,VAMAXL);
   }
   }
 }

 if (VAMAXL > 0.3){
   if (fbotf == 1 && ftopf == 0){ 
RecoXLeft(0,X3P,10000,DeltaRecoBottom,ifi,ila,RegStr0,lkfil,VAMAXL);

   if (VAMAXL > 0.3){
RecoXLeft(X2P,0,DeltaRecoTop,10000,ifi,ila,RegStr0,lkfil,VAMAXL);

   }
   }
   if (fbotf == 0 && ftopf == 1){ 
RecoXLeft(X2P,0,DeltaRecoTop,10000,ifi,ila,RegStr0,lkfil,VAMAXL);

   if (VAMAXL > 0.3){
RecoXLeft(0,X3P,10000,DeltaRecoBottom,ifi,ila,RegStr0,lkfil,VAMAXL);

   }
   }
 }



 /// last fit to save A and B belonging to the VAMAXL

 double xa[8];
 double za[8];
 double ya[8];
 int laa=0;
 for (int i=0;i<TKDBc::nlay();i++){
   for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     p_hi = p->getHit(); 
     if (p->checkstatus(AMSDBc::TOFFORGAMMA)){
       za[laa]=p_hi[2];
       xa[laa]=p_hi[0];
       ya[laa]=p_hi[1];
       laa++;
      }
   }
  }

 double A11,B11,VA11;
 dlinearme(laa,za,xa,A11,B11,VA11);         // linear fit
 alf=A11;
 blf=B11;
 nulf=laa;
 }// refitting


 for (int i=0;i<TKDBc::nlay();i++){
   for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
      p->clearstatus(AMSDBc::GAMMALEFT);
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     if (p->checkstatus(AMSDBc::TOFFORGAMMA)){
       p->setstatus(AMSDBc::GAMMALEFT);
     }
   }
 }


 //////////////////////////////////////////


  //*****************
  
  for (int i=0;i<TKDBc::nlay();i++){
    for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
      if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
      p->clearstatus(AMSDBc::TOFFORGAMMA);
    }
  }
  
// NOW RIGHT :

   MinDX2R=10000;
   MinDX3R=10000;
 if (fbotf == 1 && ftopf == 0){
   INTERf=x_starf-(SLOPEf*z_starf);
 }
   X2P=SLOPEf*firR+INTERf;
   X3P=SLOPEf*lasR+INTERf;

   double conR_w=0;
   double firRX;
   double lasRX;
   int conR_f=-1;
   int conR_l=-1;
    double conRf=0;
     double conRl=0;
    deposfmin=-1; 
    deposlmin=-1; 

CAMAX=-10000;
 RegStr=0;
 RegStr0=2.5;
 if (refitting){
  RegStr0=2.;
 }

lhi=0;
 for (int i=0;i<TKDBc::nlay();i++){
 z_d[i]=10000;
 y_d[i]=10000;
 x_d[i]=10000;
 }
 lk=-1;
 int lkfir=0;

 lo=-1;
 VA0MAX=10000;

 ifi=0;
 ila=0;
 for (int i=0;i<8;i++){
  ptryx[i]=10000;
  ptryy[i]=10000;
  ptryz[i]=10000;
 }
 double xamr[8]={10000,10000,10000,10000,10000,10000,10000,10000};
 double yamr[8]={10000,10000,10000,10000,10000,10000,10000,10000};
 double zamr[8]={10000,10000,10000,10000,10000,10000,10000,10000};

 ifi=0;
 ila=0;
  for(int i=0;i<TKDBc::nlay();i++){
    if(firR > (TKDBc::zposl(i))-0.5 && firR < (TKDBc::zposl(i))+0.5){
      ifi=i;
    }
    if(lasR > (TKDBc::zposl(i))-0.5 && lasR < (TKDBc::zposl(i))+0.5){
      ila=i;
    }
  }
   /////////000000000000000000//////////////////
 if (fbotf == 1 && ftopf == 0){
 DeltaRecoTop=3.5;
 DeltaRecoBottom=2.5;
 }
 if (fbotf == 0 && ftopf == 1){
 DeltaRecoTop=2.5;
 DeltaRecoBottom=3.5;
 }


 if(refitting == 0 || refitting == 500){

VA0MAX=10000;


RecoXRight(X2P,X3P,DeltaRecoTop,DeltaRecoBottom,ifi,ila,RegStr0,lkfir,VAMAXR);

  if (VAMAXR ==10000){
    if (fbotf == 1 && ftopf == 0){ 
RecoXRight(X2P,X3P,5.5,1,ifi,ila,1.5,lkfir,VAMAXR); 
    }
    if (fbotf == 0 && ftopf == 1){ 
RecoXRight(X2P,X3P,1,5.5,ifi,ila,1.5,lkfir,VAMAXR); 
    }
   }



   if (VAMAXR < 0.1 && VAMAXR > 0.05 && lkfir == 8){
RecoXRight(0,0,10000,10000,ifi,ila,RegStr0,lkfir,VAMAXR);
   }

  if (VAMAXR > 0.08 || lkfir < 3 || (VAMAXR > 0.001 && lkfir == 3)){
RecoXRight(X2P,X3P,5.5,5.5,ifi,ila,RegStr0,lkfir,VAMAXR);
  }
 if ((VAMAXR > 0.3) || (VAMAXR > 0.08 && lkfir <=4 )){
RecoXRight(0,0,10000,10000,ifi,ila,RegStr0,lkfir,VAMAXR);
 }

 int ifla=0;
 int ifi2r,ila2r;
 ifi2r=ifi;
 ila2r=ila;

 for (int i=0;i<TKDBc::nlay();i++){
   for (AMSTrRecHit * p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
      if (p->checkstatus(AMSDBc::GAMMARIGHT)){
        if(ifla==0 && i > ifi){
	  ifi2r=i;
          ifla=1;
        }
        if(i < ila){
          ila2r=i;
	} 
      }
   }
 }


 varbackup=VAMAXR;

   if (VAMAXR > 0.3 && lkfir >= 6){
   if (fbotf == 1 && ftopf == 0){ 
RecoXRight(0,X3P,10000,DeltaRecoBottom,ifi2r,ila,4,lkfir,VAMAXR);
   if (VAMAXR > 0.3){
RecoXRight(X2P,0,DeltaRecoTop,10000,ifi,ila,4,lkfir,VAMAXR);
   }
   }
   if (fbotf == 0 && ftopf == 1){ 
RecoXRight(X2P,0,DeltaRecoTop,10000,ifi,ila2r,4,lkfir,VAMAXR);
   if (VAMAXR > 0.3){
RecoXRight(0,X3P,10000,DeltaRecoBottom,ifi,ila,4,lkfir,VAMAXR);
   }
   }

   }
   if (VAMAXR > 0.3){
   if (fbotf == 1 && ftopf == 0){ 
RecoXRight(0,X3P,10000,DeltaRecoBottom,ifi,ila,RegStr0,lkfir,VAMAXR);

   if (VAMAXR > 0.3){
RecoXRight(X2P,0,DeltaRecoTop,10000,ifi,ila,RegStr0,lkfir,VAMAXR);

   }
   }
   if (fbotf == 0 && ftopf == 1){ 
RecoXRight(X2P,0,DeltaRecoTop,10000,ifi,ila,RegStr0,lkfir,VAMAXR);

   if (VAMAXR > 0.3){
RecoXRight(0,X3P,10000,DeltaRecoBottom,ifi,ila,RegStr0,lkfir,VAMAXR);

   }
   }

   }



 double xa[8];
 double za[8];
 double ya[8];
 int laa=0;
 for (int i=0;i<TKDBc::nlay();i++){
   for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     p_hi = p->getHit(); 
     if (p->checkstatus(AMSDBc::TOFFORGAMMA)){
       za[laa]=p_hi[2];
       xa[laa]=p_hi[0];
       ya[laa]=p_hi[1];
       laa++;
      }
   }
  }

 double A11,B11,VA11;
 dlinearme(laa,za,xa,A11,B11,VA11);         // linear fit
 arf=A11;
 brf=B11;
 nurf=laa;

 }// refitting


 for (int i=0;i<TKDBc::nlay();i++){
   for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
      p->clearstatus(AMSDBc::GAMMARIGHT);
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     if (p->checkstatus(AMSDBc::TOFFORGAMMA)){
       p->setstatus(AMSDBc::GAMMARIGHT);
     }
   }
 }


 if (refitting == 0){
   slr=10000;
   qlr=10000; 
 }

 if (refitting == 0){

   if (nurf <= 3 && nulf > 5 && VAMAXR <= 0.01){
       slr=blf;
       qlr=alf; 
   }
   if (nulf <= 3 && nurf > 5 && VAMAXL <= 0.01){
       slr=brf;
       qlr=arf; 
   }
   


   if (min(VAMAXL,VAMAXR) <= 0.0005 && max(VAMAXL,VAMAXR) <=0.0005 && 
       max(VAMAXL,VAMAXR) < 2*min(VAMAXL,VAMAXR)) {
    if (nulf != nurf && min(nulf,nurf) >= 5){
      if (fabs(blf-brf) <= 0.0005 && fabs(alf-arf) > 3 ){
      slr=(nulf>nurf)?blf:brf;
      qlr=(nulf>nurf)?alf:arf;
      }
    }
   }


   if (min(nulf,nurf) >= 7){
    if (min(VAMAXL,VAMAXR) >= 0.3 && max(VAMAXL,VAMAXR) >= 0.3 ) {
      //slr=200;
      //qlr=200;
    }
   }
   //
   if (min(nulf,nurf) >= 5){
     if (min(VAMAXL,VAMAXR) > 0.05 && min(VAMAXL,VAMAXR) < 0.1){
       slr=(VAMAXL<VAMAXR)?blf:brf;
       qlr=(VAMAXL<VAMAXR)?alf:arf;
     }
   }

   if (min(VAMAXL,VAMAXR) <= 0.015 && min(VAMAXL,VAMAXR) >= 0.001 && max(VAMAXL,VAMAXR) >= 0.08 ) {
     if ((VAMAXL < VAMAXR && nulf > 3 ) ||
         (VAMAXR < VAMAXL && nurf > 3 )){
         slr=(VAMAXL<VAMAXR)?blf:brf;
         qlr=(VAMAXL<VAMAXR)?alf:arf;
     }
   }
   if (min(VAMAXL,VAMAXR) <= 0.001 && max(VAMAXL,VAMAXR) >= 0.1 ) {
     if ((VAMAXL < VAMAXR && nulf > 3 ) ||
         (VAMAXR < VAMAXL && nurf > 3 )){
         slr=(VAMAXL<VAMAXR)?blf:brf;
         qlr=(VAMAXL<VAMAXR)?alf:arf;
     }
   }  
   if (min(VAMAXL,VAMAXR) <= 0.04 && max(VAMAXL,VAMAXR) >= 0.2 ) {
    slr=(VAMAXL<VAMAXR)?blf:brf;
    qlr=(VAMAXL<VAMAXR)?alf:arf;
   }

   //   if (min(VAMAXL,VAMAXR) <= 0.015 && max(VAMAXL,VAMAXR) >= 0.08 ) {
   //  if ((VAMAXL < VAMAXR && nulf > 3) ||
   //      (VAMAXR < VAMAXL && nurf > 3)){
   //      slr=(VAMAXL<VAMAXR)?blf:brf;
   //      qlr=(VAMAXL<VAMAXR)?alf:arf;
   //  }
   //}  
   if (nulf == 8 && nurf == 8){
     if ((VAMAXL > 0.5 && VAMAXL < 0.1) && 
	 (VAMAXR > 0.5 && VAMAXR < 0.1)){
       //         slr=(VAMAXL<VAMAXR)?blf:brf;
	 //         qlr=(VAMAXL<VAMAXR)?alf:arf;
     }
   }
   //
 }


 int resetting=0;
  if ((min(VAMAXL,VAMAXR) <= 0.005 && max(VAMAXL,VAMAXR) >= 0.08 )||
      (min(VAMAXL,VAMAXR) <= 0.015 && max(VAMAXL,VAMAXR) >= 0.1 )) {
    if (VAMAXR == min(VAMAXL,VAMAXR)){
      resetting=200;
    } 
    if (VAMAXL == min(VAMAXL,VAMAXR)){
      resetting=100;  
    }    
  }


//*****************



 double temp=0;


//*****************


 for (int i=0;i<TKDBc::nlay();i++){
   for (p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     p->clearstatus(AMSDBc::TOFFORGAMMA);
   }
 }





  int INDECOM[trconst::maxlay]; 
 for(int i=0;i<TKDBc::nlay();i++){
   INDECOM[i]=0;
 }
 for (int i=0;i<TKDBc::nlay();i++){
   for (AMSTrRecHit * p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
    if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
     if (p->checkstatus(AMSDBc::GAMMARIGHT)){
       INDECOM[i]++;
     }
     if (p->checkstatus(AMSDBc::GAMMALEFT)){
       INDECOM[i]++;
     } 
   }
 }  

 double h[8][2];
 for (int i=0;i<TKDBc::nlay();i++){
   h[i][0]=10000;
   h[i][1]=10000;
 }
 for (int i=0;i<TKDBc::nlay();i++){
   for (AMSTrRecHit * p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
     if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue; 
     AMSPoint ph=p->getHit(); 
     if (p->checkstatus(AMSDBc::GAMMALEFT)){
       h[i][0]=ph[1];
     }
     if (p->checkstatus(AMSDBc::GAMMARIGHT)){
       h[i][1]=ph[1];
     }

   }
 }

 // 
  // planes   1  2   3   4
  // INDECOM  0  1  1/2  2 -> 0 2 2 2

     if ((INDECOM[0] == 0 && INDECOM[1] == 1 && INDECOM[2] == 2 && (fabs(h[2][0]-h[2][1])<= 0.17)) ||
         (INDECOM[0] == 0 && INDECOM[1] == 1 && INDECOM[2] < 2 && INDECOM[3] == 2 && (fabs(h[3][0]-h[3][1])<= 0.5))){
       for (AMSTrRecHit * p=AMSTrRecHit::gethead(1); p!=NULL; p=p->next()){ 
	 if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;

	 if (p->checkstatus(AMSDBc::GAMMALEFT) || p->checkstatus(AMSDBc::GAMMARIGHT)){
             p->setstatus(AMSDBc::GAMMALEFT);
	     p->setstatus(AMSDBc::GAMMARIGHT);
	 }

       }
       if (INDECOM[2] == 1){
	 for (AMSTrRecHit * p=AMSTrRecHit::gethead(2); p!=NULL; p=p->next()){ 
	 if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;

	 if (p->checkstatus(AMSDBc::GAMMALEFT) || p->checkstatus(AMSDBc::GAMMARIGHT)){
             p->setstatus(AMSDBc::GAMMALEFT);
	     p->setstatus(AMSDBc::GAMMARIGHT);
	 }

       }
       }

     }

  // planes 1  2  3  4
  // INDECOM  1  2  .. .. -> 2  2
  // INDECOM  1 1/0 2  .. -> 2 1/0 2

  //     if ((INDECOM[0] == 1 && INDECOM[1] == 2 && (fabs(h[1][0]-h[1][1])<= 0.19)) ||
     if ((INDECOM[0] == 1 && INDECOM[1] == 2 && (fabs(h[1][0]-h[1][1])<= 0.31)) ||
         (INDECOM[0] == 1 && INDECOM[1] <2 && INDECOM[2] == 2 && (fabs(h[2][0]-h[2][1])<= 0.37)) ||
         (INDECOM[0] == 1 && INDECOM[1] <2 && INDECOM[2] <2   && INDECOM[3] == 2 && (fabs(h[3][0]-h[3][1])<= 0.5)) ||
         (INDECOM[0] == 1 && INDECOM[1] <2 && INDECOM[2] <2 && INDECOM[3] <2 && INDECOM[4] == 2 && (fabs(h[4][0]-h[4][1])<= 0.95))){

       for (AMSTrRecHit * p=AMSTrRecHit::gethead(0); p!=NULL; p=p->next()){ 
	 if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;

	 if (p->checkstatus(AMSDBc::GAMMALEFT) || p->checkstatus(AMSDBc::GAMMARIGHT)){
             p->setstatus(AMSDBc::GAMMALEFT);
	     p->setstatus(AMSDBc::GAMMARIGHT);
	 }

       }

  // planes 1  2  3  4
  // FLPAT  2  1  2  .. -> 2  2  2
       if (INDECOM[1] == 1){
	 for (AMSTrRecHit * p=AMSTrRecHit::gethead(1); p!=NULL; p=p->next()){ 
	   if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
	 if (p->checkstatus(AMSDBc::GAMMALEFT) || p->checkstatus(AMSDBc::GAMMARIGHT)){
	   p->setstatus(AMSDBc::GAMMALEFT);
	   p->setstatus(AMSDBc::GAMMARIGHT);
	 }

	 }
       }

       if (INDECOM[2] == 1){
	 for (AMSTrRecHit * p=AMSTrRecHit::gethead(2); p!=NULL; p=p->next()){ 
	   if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
	 if (p->checkstatus(AMSDBc::GAMMALEFT) || p->checkstatus(AMSDBc::GAMMARIGHT)){
	   p->setstatus(AMSDBc::GAMMALEFT);
	   p->setstatus(AMSDBc::GAMMARIGHT);
	 }

	 }
       }


     }
       

 



ResetXhits(resetting,VAMAXL,VAMAXR);

 if (resetting > 1100){
   slr=10000;
   qlr=10000; 
 }

 

 //  



  ////////////////


  //..
}

void AMSTrTrackGamma::ResetXhits(int & resetting, double VAMAXL, double VAMAXR){

  double xamr[8]={10000,10000,10000,10000,10000,10000,10000,10000};
  double yamr[8]={10000,10000,10000,10000,10000,10000,10000,10000};
  double zamr[8]={10000,10000,10000,10000,10000,10000,10000,10000};
  double xaml[8]={10000,10000,10000,10000,10000,10000,10000,10000};
  double yaml[8]={10000,10000,10000,10000,10000,10000,10000,10000};
  double zaml[8]={10000,10000,10000,10000,10000,10000,10000,10000};
  int lr=0;
  int ll=0;
  //



  for (int i=0;i<TKDBc::nlay();i++){
    for (AMSTrRecHit * p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
      if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
      AMSPoint pli = p->getHit();
      if (p->checkstatus(AMSDBc::GAMMALEFT)){
	xaml[ll]=pli[0];
	yaml[ll]=pli[1];
	zaml[ll]=pli[2];
	ll++;
      }
      if (p->checkstatus(AMSDBc::GAMMARIGHT)){
	xamr[lr]=pli[0];
	yamr[lr]=pli[1];
	zamr[lr]=pli[2];
	lr++;
      }
    }
  }
  
  

  double aa,bb,vv,dmax,vvl,vvr;
  dmax=10000; 
double xamlb[8]={10000,10000,10000,10000,10000,10000,10000,10000};
double yamlb[8]={10000,10000,10000,10000,10000,10000,10000,10000};
double zamlb[8]={10000,10000,10000,10000,10000,10000,10000,10000};
  //
  if (resetting == 200){
dlinearme(lr,zamr,xamr,aa,bb,vv);         // linear fit

   for (int i=0;i<TKDBc::nlay();i++){
        dmax=10000;
    for (AMSTrRecHit * p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
      if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
      AMSPoint pli = p->getHit();
      for (int ii=0;ii<ll;ii++){
	if (pli[2]  > (zaml[ii]-0.5) && pli[2] < (zaml[ii]+0.5)){
	  if (pli[1] > (yaml[ii]-0.02) && pli[1] < (yaml[ii]+0.02)){
            if (fabs(pli[0]-aa-bb*zaml[ii]) <= dmax){
	      dmax=fabs(pli[0]-aa-bb*zaml[ii]);
              xamlb[ii]=pli[0];
            }
	  }
	}
      }
    }
   }

   for (int i=0;i<TKDBc::nlay();i++){
    for (AMSTrRecHit * p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
      if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
      AMSPoint pli = p->getHit();
      p->clearstatus(AMSDBc::GAMMALEFT);
    }
   }
  for (int i=0;i<TKDBc::nlay();i++){
    for (AMSTrRecHit * p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
      if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
      AMSPoint pli = p->getHit();
      for (int ii=0;ii<ll;ii++){
	if (pli[2]  > (zaml[ii]-0.5) && pli[2] < (zaml[ii]+0.5)){
	  if (pli[1] > (yaml[ii]-0.02) && pli[1] < (yaml[ii]+0.02)){
            if (pli[0] == xamlb[ii]){
	      p->setstatus(AMSDBc::GAMMALEFT);
		//		xamlb[ii]=pli[0];
		yamlb[ii]=pli[1];
		zamlb[ii]=pli[2];
	  }
	}          
      }
    }
    }
  }
    for (int ii=0;ii<ll;ii++){
      xaml[ii]=xamlb[ii];
      yaml[ii]=yamlb[ii];
      zaml[ii]=zamlb[ii];
    }

  }
  if (resetting == 100){
dlinearme(ll,zaml,xaml,aa,bb,vv);         // linear fit
   
   for (int i=0;i<TKDBc::nlay();i++){
        dmax=10000;
    for (AMSTrRecHit * p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
      if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
      AMSPoint pli = p->getHit();
      for (int ii=0;ii<lr;ii++){
	if (pli[2]  > (zamr[ii]-0.5) && pli[2] < (zamr[ii]+0.5)){
	  if (pli[1] > (yamr[ii]-0.02) && pli[1] < (yamr[ii]+0.02)){
            if (fabs(pli[0]-aa-bb*zamr[ii]) <= dmax){
	      dmax=fabs(pli[0]-aa-bb*zamr[ii]);
              xamlb[ii]=pli[0];
            }
	  }
	}
      }
    }
   }

   for (int i=0;i<TKDBc::nlay();i++){
    for (AMSTrRecHit * p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
      if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
      AMSPoint pli = p->getHit();
      p->clearstatus(AMSDBc::GAMMARIGHT);
    }
   }
  for (int i=0;i<TKDBc::nlay();i++){
    for (AMSTrRecHit * p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
      if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
      AMSPoint pli = p->getHit();
      for (int ii=0;ii<lr;ii++){
	if (pli[2]  > (zamr[ii]-0.5) && pli[2] < (zamr[ii]+0.5)){
	  if (pli[1] > (yamr[ii]-0.02) && pli[1] < (yamr[ii]+0.02)){
            if (pli[0] == xamlb[ii]){
	      p->setstatus(AMSDBc::GAMMARIGHT);
		//		xamlb[ii]=pli[0];
		yamlb[ii]=pli[1];
		zamlb[ii]=pli[2];
	  }
	}          
      }
    }
    }
  }
    for (int ii=0;ii<lr;ii++){
      xamr[ii]=xamlb[ii];
      yamr[ii]=yamlb[ii];
      zamr[ii]=zamlb[ii];
    }

  }
  //
dlinearme(lr,zamr,xamr,aa,bb,vvr);         // linear fit

dlinearme(ll,zaml,xaml,aa,bb,vvl);         // linear fit


    if (max(vvl,vvr) < 0.01 && min(ll,lr) >3){
      resetting=resetting+1000;
    } 
  //}
  
 

  
  double cetkylr[8]={10000,10000,10000,10000,10000,10000,10000,10000};  
  double tkxlr[8]={10000,10000,10000,10000,10000,10000,10000,10000};
  double maxtkx=10000;
  double xam[8]={10000,10000,10000,10000,10000,10000,10000,10000};
  double avexam[8]={10000,10000,10000,10000,10000,10000,10000,10000};
  double rexam[8]={10000,10000,10000,10000,10000,10000,10000,10000};
  double tkx[8]={10000,10000,10000,10000,10000,10000,10000,10000};
  
  double newxl[8]={10000,10000,10000,10000,10000,10000,10000,10000};
  double newxr[8]={10000,10000,10000,10000,10000,10000,10000,10000};

  for (int i1=0;i1<ll;i1++){
    for (int i2=0;i2<lr;i2++){
      
      if (zaml[i1] > (zamr[i2]-0.5) && zaml[i1] < (zamr[i2]+0.5)){
	for(int i3=0;i3<8;i3++){
          maxtkx=10000;
	  if (zaml[i1] > ((TKDBc::zposl(i3))-0.5) && zaml[i1] < ((TKDBc::zposl(i3))+0.5)){
	    //            
            cetkylr[i3]=(yaml[i1]+yamr[i2])*0.5;
	    //
	    tkxlr[i3]=fabs(xaml[i1]-xamr[i2]);

            if (tkxlr[i3] == 0){

	      for (AMSTrRecHit * po=AMSTrRecHit::gethead(i3); po!=NULL; po=po->next()){
		if(po->checkstatus(AMSDBc::FalseX) || po->checkstatus(AMSDBc::FalseTOFX))continue;
		AMSPoint pho = po->getHit();
		if (pho[1] == yaml[i1] || pho[1] == yamr[i2]){
		  if (pho[0] != xaml[i1]){
		    tkx[i3]=fabs(pho[0]-xaml[i1]);
		    if (tkx[i3] < maxtkx){
		      maxtkx=tkx[i3];
		      xam[i3]=pho[0];
		      rexam[i3]=maxtkx;
		      avexam[i3]=(pho[0]+xaml[i1])*0.5;		      
		      
		    } 
		  }
		}
	      }
	      // }
	      //
	    }
	  }
	}

	resetting++;
      }	  
    }
  }

  int lce=-1;
  double cen[8]={10000,10000,10000,10000,10000,10000,10000,10000};
  double AY,BY,VARY;
  double AX,BX,VARX;
  double AXr,BXr,VARXr;
  double AXl,BXl,VARXl;
  double zce[8]={10000,10000,10000,10000,10000,10000,10000,10000};
  for(int i3=0;i3<8;i3++){
    if (cetkylr[i3] != 10000){
      lce++;
      cen[lce]=cetkylr[i3];
      zce[lce]=TKDBc::zposl(i3); 
    }
  } 
  dlinearme(lce+1,zce,cen,AY,BY,VARY);         // linear fit
  // we got the Y slope
  dlinearme(lr,zamr,xamr,AXr,BXr,VARXr);         // linear fit

  dlinearme(ll,zaml,xaml,AXl,BXl,VARXl);         // linear fit


  BX=(VARXr >= VARXl)?BXr:BXl;
  if (fabs(BXr) < 0.01 && fabs(BXl) < 0.01){
    BX=(BX > 0)?BX:(-1*BX);
  }

  

  
  for(int i3=0;i3<8;i3++){
    if (xam[i3] != 10000 && avexam[i3] != 10000 && (rexam[i3] < 1 || rexam[i3] == 222)){

      if (xam[i3] > avexam[i3]){
         newxr[i3]=xam[i3];
         newxl[i3]=(2*avexam[i3])-xam[i3];  // (=xaml[i1])
      }
      if (xam[i3] < avexam[i3]){
         newxl[i3]=xam[i3];
         newxr[i3]=(2*avexam[i3])-xam[i3];  // (=xaml[i1])
      }
      // let's keep the average X IMPOSSIBLE
      for (int i1=0;i1<ll;i1++){
	if (zaml[i1] > ((TKDBc::zposl(i3))-0.5) && zaml[i1] < ((TKDBc::zposl(i3))+0.5)){
	  //          if ((BY*BX) <= 0){
          if ((BY*BX) <= -0.009){
	    xaml[i1]=newxl[i3];
          }
	  //          if ((BY*BX) > 0 ){
          if ((BY*BX) >= 0.009){
	    xaml[i1]=newxr[i3];
          }
	  // between 0.05 e -0.05 leave them as they are 

	}
      }
      for (int i2=0;i2<lr;i2++){
	if (zamr[i2] > ((TKDBc::zposl(i3))-0.5) && zamr[i2] < ((TKDBc::zposl(i3))+0.5)){

          //if ((BY*BX) <= 0){
            if ((BY*BX) <= -0.009){
	    xamr[i2]=newxr[i3];
	  }
	    //          if ((BY*BX) > 0){
	   if ((BY*BX) >= 0.009){
             xamr[i2]=newxl[i3];
	  }
// between 0.05 e -0.05 leave them as they are 

     	}
      }
      
      //
      

      
      
    }

  }


dlinearme(lr,zamr,xamr,aa,bb,vvr);         // linear fit

dlinearme(ll,zaml,xaml,aa,bb,vv);         // linear fit

 
  /// THAT's real resetting

 /////////////////////////////////////////////////
  
  for (int i=0;i<TKDBc::nlay();i++){
   for (AMSTrRecHit * p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
	if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
	AMSPoint p_hi = p->getHit(); 
        p->clearstatus(AMSDBc::GAMMALEFT); 
        p->clearstatus(AMSDBc::GAMMARIGHT); 
   }
  }


  
  for (int i=0;i<TKDBc::nlay();i++){
    for (AMSTrRecHit * p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
      if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
      AMSPoint p_hi = p->getHit(); 
      for (int ii=0;ii<ll;ii++){
//
	
	if (p_hi[2] > (zaml[ii]-0.5) && p_hi[2] < (zaml[ii]+0.5)){
	  if (p_hi[1] > (yaml[ii]-0.001) && p_hi[1] < (yaml[ii]+0.001)){
	    if (p_hi[0] > (xaml[ii]-0.001) && p_hi[0] < (xaml[ii]+0.001)){              
	      p->setstatus(AMSDBc::GAMMALEFT);
	    }
	  }
	}
      }
    }
  }
  
  
  for (int i=0;i<TKDBc::nlay();i++){
    for (AMSTrRecHit * p=AMSTrRecHit::gethead(i); p!=NULL; p=p->next()){
      if(p->checkstatus(AMSDBc::FalseX) || p->checkstatus(AMSDBc::FalseTOFX))continue;
      AMSPoint p_hi = p->getHit(); 
      for (int ii=0;ii<lr;ii++){
	//
	
	if (p_hi[2] > (zamr[ii]-0.5) && p_hi[2] < (zamr[ii]+0.5)){
	  if (p_hi[1] > (yamr[ii]-0.001) && p_hi[1] < (yamr[ii]+0.001)){
	    if (p_hi[0] > (xamr[ii]-0.001) && p_hi[0] < (xamr[ii]+0.001)){              
	      p->setstatus(AMSDBc::GAMMARIGHT);
	    }
	  }
	}
	
      }
    }
  }
  



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
    _Charge=0;
  }
  if (_GRidgidityMSR * _GRidgidityMSL > 0){
    plusminus=1;
    _Charge=2;
    pmeno=_GRidgidityMSR;
     ppiu= _GRidgidityMSL;
  } 
 if (_GRidgidityMSR * _GRidgidityMSL == 0){
    _Charge=2;
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
_GErrRidgidityR=outR[8];
_GThetaR=outR[3];
_GPhiR=outR[4];
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
_GErrRidgidityL=outL[8];
_GThetaL=outL[3];
_GPhiL=outL[4];
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
_Chi2MSL=outL[6];
if(outL[7] != 0)_Chi2MSL=FLT_MAX;
_RidgidityMSL=outL[5];
}
 else if(fit==5){ //JUAN !!!!!!!!
_GChi2MSR=outR[6];  
if(outR[7]!=0)_GChi2MSR=FLT_MAX;
_GRidgidityMSR=outR[5];
// me
_GThetaMSR=outR[3];
_GPhiMSR=outR[4];
_GP0MSR=AMSPoint(outR[0],outR[1],outR[2]);
//...
_GChi2MSR=outR[6];  
if(outR[7]!=0)_GChi2MSR=FLT_MAX;
_GRidgidityMSL=outL[5];
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
        
	//
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
  if(L-1>0){
  VAR=sum/(L-1);
  }
  else{
    VAR=10000;
  }
}

void MYlfit::make_a(){
  delta=L*sigma(x,x)-pow(sigma(x),2.0);
  if(delta){
   a=(1.0/delta)*(sigma(x,x)*sigma(y)-sigma(x)*sigma(x,y));
  }
  else{
    a=10000;
    //   cerr <<" this should be fixed by GL"<<endl;
  }
}

void MYlfit::make_b(){
  if(delta){
   b=(1.0/delta)*(L*sigma(x,y)-sigma(x)*sigma(y));
  }
  else{
    b=10000;
    //   cerr <<" this should be fixed by GL"<<endl;
  }
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
          (z2t[lb2]-z1t[lb1] !=0 && fabs(M1[lb1]-((x2t[lb2]-x1t[lb1])/(z2t[lb2]-z1t[lb1]))) > 0.25)){
        fbot=1;
        ftop=0;
        x_star=x1b[lb1];
        z_star=z1b[lb1];
        SLOPE=M1[lb1];
        INTER=Q1[lb1];
      }


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





void AMSTrTrackGamma::_ConstructGamma(int method){


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
         !pbeta->getptrack()->checkstatus(AMSDBc::TRDTRACK)  &&
         !pbeta->getptrack()->checkstatus(AMSDBc::FalseTOFX)){
        _pbeta=pbeta;
         break;
      }
       pbeta=pbeta->next();
    }
    if(_pbeta)break;
  }
  }
    _Charge=(_pntTrL->getrid()>0?1:-1)+(_pntTrR->getrid()>0?1:-1);
  if(_pbeta){
    _pbeta->getptrack()->AdvancedFit();
    if(_pbeta->getptrack()->getrid()*_pntTrL->getrid()>0)_pntTrL=_pbeta->getptrack();
    else if(_pbeta->getptrack()->getrid()*_pntTrR->getrid()>0)_pntTrR=_pbeta->getptrack();
  }

   if((_pntTrL->FastFitDone() && _pntTrR->FastFitDone()) && _pntTrL->getchi2()<FLT_MAX-1 && _pntTrR->getchi2()<FLT_MAX-1){
    number gers=0.03;
     _pntTrL->SimpleFit();
     _pntTrR->SimpleFit();

  // get angle between tracks;

  AMSDir dirL(_pntTrL->gettheta(method),_pntTrL->getphi(method));
  AMSDir dirR(_pntTrR->gettheta(method),_pntTrR->getphi(method));
  if (dirL[2]>0)dirL=dirL*(-1);
  if (dirR[2]>0)dirR=dirR*(-1);
  number ca=dirL.prod(dirR);
  if(ca>1)ca=1;


//  First  interpolate to zero order vertex guess  

   integer lay_l=min(_pntTrL->getphit(0)->getLayer(),_pntTrL->getphit(_pntTrL->getnhits()-1)->getLayer())-1;
   integer lay_r=min(_pntTrR->getphit(0)->getLayer(),_pntTrR->getphit(_pntTrR->getnhits()-1)->getLayer())-1;
   integer lay=min(lay_l,lay_r);

/*
  {
    _MGAM=sqrt(fabs(2*fabs(_pntTrR->getrid(method))*fabs(_pntTrL->getrid(method))*(1-ca)));
    cout <<"very  very initial ca "<<method<<" "<<180/3.1415926*acos(ca)<<" "<<_MGAM<<endl;

    AMSmceventg *pmcg=(AMSmceventg*)AMSEvent::gethead()->getheadC("AMSmceventg",0);
    if(pmcg){
     AMSPoint pge=dirR*fabs(_pntTrR->getrid(method))+dirL*fabs(_pntTrL->getrid(method));
     number cag=pmcg->getdir().prod(AMSDir(pge));
     cout <<" pmcg->getdir() "<<pmcg->getdir()<<endl;
     cout <<" AMSDir(pge) "<<AMSDir(pge)<<endl;
   if(cag>1)cag=1;
    cout <<" very initial ca2 "<<180/3.1415926*acos(cag)<<" "<<endl;
    }

  }     
*/

 AMSPoint p1L,p1R;
 number thetaL,phiL,local;
 number thetaR,phiR;




 if(_pntTrL->intercept(p1L,lay,thetaL,phiL,local,method) && _pntTrR->intercept(p1R,lay,thetaR,phiR,local,method)){
  
    

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
    _Vertex=(_P0L+_P0R)*0.5;
    _TrackDistance=(_P0L-_P0R).norm();
    

    AMSDir dir(0,0,-1);
    number length;
    _pntTrL->interpolate(_Vertex,dir,_P0L,thetaL,phiL,length,method);    
    _pntTrR->interpolate(_Vertex,dir,_P0R,thetaR,phiR,length,method);    
    AMSDir dirL1(thetaL,phiL); 
    AMSDir dirR1(thetaR,phiR); 
    if(1 && dirL1.prod(dirR1)>=ca){
     _pntTrL->SetPar(_pntTrL->getrid(method),thetaL,phiL,_P0L,method);
     _pntTrR->SetPar(_pntTrR->getrid(method),thetaR,phiR,_P0R,method);
     dirL=dirL1;
     dirR=dirR1;
     if (dirL[2]>0)dirL=dirL*(-1);
     if (dirR[2]>0)dirR=dirR*(-1);
    }


/*
{
  ca=dirL.prod(dirR);
  if(ca>1)ca=1;

    _MGAM=sqrt(fabs(2*fabs(_pntTrR->getrid(method))*fabs(_pntTrL->getrid(method))*(1-ca)));
    cout <<" final "<<180/3.1415926*acos(ca)<<" "<<_MGAM<<endl;

    AMSmceventg *pmcg=(AMSmceventg*)AMSEvent::gethead()->getheadC("AMSmceventg",0);
    if(pmcg){
     AMSPoint pge=dirR*fabs(_pntTrR->getrid(method))+dirL*fabs(_pntTrL->getrid(method));
     number cag=pmcg->getdir().prod(AMSDir(pge));
    if(cag>1)cag=1;
    cout <<" final2 "<<180/3.1415926*acos(cag)<<" "<<endl;
    }
}
*/





//  set gamma parameters  (assuming direction for top to bottom)

    _Charge=(_pntTrL->getrid(method)>0?1:-1)+(_pntTrR->getrid(method)>0?1:-1);
//    cout <<" final charge "<<_Charge<<endl;
    AMSPoint pge=dirR*fabs(_pntTrR->getrid(method))+dirL*fabs(_pntTrL->getrid(method));
    number err_1=pow(_pntTrL->geterid()*_pntTrL->getrid()*_pntTrL->getrid(),2)+pow(_pntTrL->geterid()*_pntTrL->getrid()*_pntTrL->getrid(),2);
   AMSPoint p1=dirR*fabs(_pntTrR->getrid(2))+dirL*fabs(_pntTrL->getrid(2));
   number err_2=p1.norm()-pge.norm();
    _PGAMM=(pge.norm()+p1.norm())/2;
    _ErrPGAMM=sqrt(err_2*err_2+err_1);

/*
{
  dirL=AMSDir(_pntTrL->gettheta(method),_pntTrL->getphi(method));
  dirR=AMSDir(_pntTrR->gettheta(method),_pntTrR->getphi(method));
  if (dirL[2]>0)dirL=dirL*(-1);
  if (dirR[2]>0)dirR=dirR*(-1);
     pge=dirR*fabs(_pntTrR->getrid(method))+dirL*fabs(_pntTrL->getrid(method));
     AMSTrTrack *ptr=lay_l<lay_r?_pntTrL:(lay_r<lay_l?_pntTrR:0);
     if(ptr  ){
      AMSPoint pge2=AMSDir(ptr->gettheta(method),ptr->getphi(method))*fabs(ptr->getrid(method));
      if(pge2[2]>0)pge2=pge2*(-1);
      pge=pge2;
     }

}    

*/
   _PhTheta=AMSDir(pge).gettheta();
    _PhPhi=AMSDir(pge).getphi();
    number ca=dirL.prod(dirR);
    _MGAM=sqrt(fabs(2*fabs(_pntTrR->getrid())*fabs(_pntTrL->getrid())*(1-ca)));
           
  return;
 } 
}
_MGAM=0;
_PGAMM=0;
_ErrPGAMM=0;
_PhTheta=0;
_PhPhi=0;
_Charge=0;
setstatus(AMSDBc::BAD);



}
