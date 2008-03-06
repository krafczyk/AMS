#include "typedefs.h"
#include "node.h"
#include "snode.h"
#include "amsdbc.h"
#include "gmat.h"
#include "extC.h"
#include <stdlib.h>
#include "gvolume.h"
#include "amsgobj.h"

#include "commons.h"
#include "tkdbc.h"
extern "C" void mtx_(geant nrm[][3],geant vect[]);
extern "C" void mtx2_(number nrm[][3],geant  xnrm[][3]);
#define MTX mtx_
#define MTX2 mtx2_
namespace amsgeom{
extern void tkgeom02(AMSgvolume &);
extern void tkgeom02d(AMSgvolume &);
extern void magnetgeom(AMSgvolume &);
extern void magnetgeom02(AMSgvolume &);
extern void ext1structure02(AMSgvolume &);
extern void magnetgeom02o(AMSgvolume &);
extern void magnetgeom02Test(AMSgvolume &);
extern void tofgeom02(AMSgvolume &);
extern void antigeom02(AMSgvolume &);
extern void antigeom002(AMSgvolume &);
extern void ext2structure(AMSgvolume &);
#ifdef __G4AMS__
extern void antigeom02g4(AMSgvolume &);
extern void testg4geom(AMSgvolume &);
#endif
extern void richgeom02(AMSgvolume &,float zshift=0);
extern void ecalgeom02(AMSgvolume &);
extern void trdgeom02(AMSgvolume &, float zshift=0);
//extern void srdgeom02(AMSgvolume &);
extern void Put_rad(AMSgvolume *,integer);
extern void Put_pmt(AMSgvolume *,integer);
};
using namespace amsgeom;

void amsgeom::tkgeom02(AMSgvolume & mother){
  TKDBc::read();





  AMSID amsid;
  geant xx[3]={0.9,0.2,0.2};
  geant  xnrm[3][3];
  geant par[6]={0.,0.,0.,0.,0.,0.};
  number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
  number inrm[3][3];
  char name[5];
  geant coo[3]={0.,0.,0.};

  integer gid=0;
  integer nrot=501; // Temporary arbitary assignment
  //static AMSgvolume mother(0,0,AMSDBc::ams_name,"BOX",par,3,coo,nrm,"ONLY",
  //                         0,gid);  // temporary a dummy volume
  AMSNode * cur;
  AMSgvolume * dau;
  int i;
  int nhalfL=0; 
  for ( i=0;i<TKDBc::nlay();i++){
    ostrstream ost(name,sizeof(name));
    ost << "STK"<<i+1<<ends;
    coo[0]=TKDBc::xposl(i);
    coo[1]=TKDBc::yposl(i);
    coo[2]=TKDBc::zposl(i);
    nrm[0][0]=TKDBc::nrml(0,0,i);
    nrm[0][1]=TKDBc::nrml(0,1,i); 
    nrm[0][2]=TKDBc::nrml(0,2,i); 
    nrm[1][0]=TKDBc::nrml(1,0,i); 
    nrm[1][1]=TKDBc::nrml(1,1,i); 
    nrm[1][2]=TKDBc::nrml(1,2,i); 
    nrm[2][0]=TKDBc::nrml(2,0,i); 
    nrm[2][1]=TKDBc::nrml(2,1,i); 
    nrm[2][2]=TKDBc::nrml(2,2,i); 
    int ii;
    gid=i+1;
    integer status=1;
    integer rgid;
    if(TKDBc::update())TKDBc::SetLayer(i+1,status,coo,nrm,gid);
    else               TKDBc::GetLayer(i+1,status,coo,nrm,rgid);
#ifdef __AMSDEBUG__
    if(i==2 && AMSgvolume::debug){
      MTX(xnrm,xx);
      int a1,a2;
      for( a1=0;a1<3;a1++)for( a2=0;a2<3;a2++)nrm[a2][a1]=xnrm[a1][a2];
    }
#endif
    int npar=3;
    if(npar==5){
      for ( ii=0;ii<npar;ii++)par[ii]=TKDBc::layd(i,ii);
      dau=(AMSgvolume*)mother.add(new AMSgvolume(
						 "VACUUM",nrot++,name,"CONE",par,npar,coo,nrm, "ONLY",0,gid,1));
      //      cout <<" layer "<<i<<" "<<nrot<<endl;
    }
    else{
      par[0]=TKDBc::layd(i,1);
      par[1]=TKDBc::layd(i,2);
      par[2]=TKDBc::layd(i,0);
      dau=(AMSgvolume*)mother.add(new AMSgvolume(
						 "VACUUM",nrot++,name,"TUBE",par,npar,coo,nrm, "ONLY",0,gid,1));
      //      cout <<" layer "<<i<<" "<<nrot<<endl;
    }
    int j;
    for (j=0;j<TKDBc::nlad(i+1);j++){
      int k;
      AMSgvolume * lad[2];
      for(k=0;k<2;k++){
	ost.clear();
	ost.seekp(0);
	ost << "L" << i+1<<(j+1)*2+k<<ends;
	double pairx=0;
	if(k==0){
	  par[0]=TKDBc::ssize_inactive(i,0)*TKDBc::nhalf(i+1,j+1)/2.;
	  pairx=TKDBc::ssize_inactive(i,0)*
	    (TKDBc::nsen(i+1,j+1)-TKDBc::nhalf(i+1,j+1))/2.;
        }
	else{
	  par[0]=TKDBc::ssize_inactive(i,0)*
	    (TKDBc::nsen(i+1,j+1)-TKDBc::nhalf(i+1,j+1))/2.;
	  pairx=TKDBc::ssize_inactive(i,0)*TKDBc::nhalf(i+1,j+1)/2.;
	}
	par[1]=TKDBc::ssize_inactive(i,1)/2;
	par[2]=TKDBc::silicon_z(i)/2;
	coo[0]=(2*k-1)*(TKDBc::ssize_inactive(i,0)*TKDBc::nsen(i+1,j+1)/2+
			TKDBc::halfldist(i)-par[0]);
	if(pairx==0){
	  //coo[0]+=-(2*k-1)*TKDBc::halfldist(i);
	  //cout <<"  Coo[0] was changed "<<" "<<i<<" "<<coo[0]<<endl;         
	}
	coo[1]=(TKDBc::nlad(i+1)-j)*TKDBc::c2c(i)-
	  (TKDBc::nlad(i+1)+1)*TKDBc::c2c(i)/2.;
	//coo[1]+=(2*k-1)*0.008;
	coo[2]=TKDBc::zpos(i);
	VZERO(nrm,9*sizeof(nrm[0][0])/4);
	if(k==0){
          nrm[0][0]=1;
          nrm[1][1]=1;
          nrm[2][2]=1;
	}
	else{
          nrm[0][0]=-1;
          nrm[1][1]=-1;
          nrm[2][2]= 1;
	}
#ifdef __AMSDEBUG__
	if(TKDBc::nsen(i+1,j+1)==TKDBc::nhalf(i+1,j+1) ||
	   TKDBc::nhalf(i+1,j+1)==0){
          if(par[0]!=0){
	    cout << i+1<<" "<<j+1<<" "<<coo[0]<<" "<<coo[1]<<" "<<coo[2]<<" "<<nrm[0][0]<<endl;
          }
	}
#endif
	//
	//  Ladder 
	//
        gid=i+1+10*(j+1)+100000;
        integer status=1;
        int rgid;
        if(!par[0]>0){
          status=0;
        }
        //cout <<"Lad "<<i<<" "<<j<<" "<<k<<" "<< coo[0]<<" "<<coo[1]<<" "<<coo[2]<<" "<<par[0]<<" "<<par[1]<<" "<<par[2]<<endl;
        if(TKDBc::update())TKDBc::SetLadder(i,j,k,status,coo,nrm,gid);
        else               TKDBc::GetLadder(i,j,k,status,coo,nrm,rgid);
        if(TKDBc::update()){
	  cout <<"Lad "<<i<<" "<<j<<" "<<k<<" "<< coo[0]<<" "<<coo[1]<<" "<<coo[2]<<" "<<par[0]<<" "<<par[1]<<" "<<par[2]<<endl;
        }
        //        if(i==5){
        //          cout <<"Lad "<<j<<" "<<k<<" "<< coo[0]<<" "<<coo[1]<<" "<<coo[2]<<endl;
        //        }
        if(par[0]>0)lad[k]=(AMSgvolume*)dau->add(new AMSgvolume(
								"NONACTIVE_SILICON",nrot++,name,"BOX",par,3,coo,nrm,"ONLY",1,gid,1));
        else lad[k]=0; 
        if(lad[k]){
	  ost.clear();
	  ost.seekp(0);
	  ost << (k==0?"ELL":"ELR") << i+1<<ends;
	  par[0]=TKDBc::zelec(i,1)/2.;
	  par[1]=TKDBc::c2c(i)/2.;
	  //        par[2]=TKDBc::zelec(i,0)/2;
	  par[2]=(dau->getpar(0)- TKDBc::zelec(i,2))/2;
	  coo[0]=lad[k]->getcoo(0)+(2*k-1)*(lad[k]->getpar(0)+par[0]);
	  coo[1]=(TKDBc::nlad(i+1)-j)*TKDBc::c2c(i)-
	    (TKDBc::nlad(i+1)+1)*TKDBc::c2c(i)/2.;
	  coo[2]=TKDBc::zelec(i,2)+par[2];
	  VZERO(nrm,9*sizeof(nrm[0][0])/4);
	  nrm[0][0]=1;
	  nrm[1][1]=1;
	  nrm[2][2]=1;
	  gid=i+1+10*(j+1);
	  cur=dau->add(new AMSgvolume(
				      "ELECTRONICS",nrot++,name,"BOX",par,3,coo,nrm,"ONLY",1,gid,1));
        }



      }

    
      // Now Sensors

      for ( k=0;k<TKDBc::nhalf(i+1,j+1);k++){
	ost.clear();
	ost.seekp(0);
	ost << "S" << i+1<<(j+1)*2<<ends;
	par[0]=TKDBc::ssize_active(i,0)/2;
	par[1]=TKDBc::ssize_active(i,1)/2;
	par[2]=TKDBc::silicon_z(i)/2;
	coo[0]=-TKDBc::ssize_inactive(i,0)/2.+
	  (2*k+2-TKDBc::nhalf(i+1,j+1))*TKDBc::ssize_inactive(i,0)/2.;
	coo[1]=0;
	coo[2]=0;
	VZERO(nrm,9*sizeof(nrm[0][0])/4);
	nrm[0][0]=1;
	nrm[1][1]=1;
	nrm[2][2]=1;
#ifdef __AMSDEBUG__
	if(i==2 && AMSgvolume::debug){
          MTX(xnrm,xx);
          int a1,a2;
          for( a1=0;a1<3;a1++)for( a2=0;a2<3;a2++)nrm[a2][a1]=xnrm[a1][a2];
	}
#endif
	gid=i+1+10*(j+1)+1000*(k+1);
	if(TKDBc::activeladdshuttle(i+1,j+1,0)){  
          if(k==0)nhalfL++;
          integer status=1;
          if(TKDBc::update())TKDBc::SetSensor(i,j,k,status,coo,nrm,gid);
          else               TKDBc::GetSensor(i,j,k,status,coo,nrm);
          if(status){
	    cur=lad[0]->add(new AMSgvolume(
					   "ACTIVE_SILICON",nrot++,name,"BOX",par,3,coo,nrm,"ONLY",1,gid,1));
	    //          cout <<"add volume "<<name<<" "<<gid<<endl;
	    //          cout <<" to "<<*(lad[0]);
          }
	}
      }

      for ( k=TKDBc::nhalf(i+1,j+1);k<TKDBc::nsen(i+1,j+1);k++){
	ost.clear();
        ost.seekp(0);
        ost << "S" << i+1<<(j+1)*2+1<<ends;
        par[0]=TKDBc::ssize_active(i,0)/2;
        par[1]=TKDBc::ssize_active(i,1)/2;
        par[2]=TKDBc::silicon_z(i)/2;
        coo[0]=-TKDBc::ssize_inactive(i,0)/2.+
	  (2*(k-TKDBc::nhalf(i+1,j+1))+2+
	   TKDBc::nhalf(i+1,j+1)-TKDBc::nsen(i+1,j+1))*
	  TKDBc::ssize_inactive(i,0)/2.;
        coo[1]=0;
        coo[2]=0;
        VZERO(nrm,9*sizeof(nrm[0][0])/4);
        nrm[0][0]=1;
        nrm[1][1]=1;
        nrm[2][2]=1;
#ifdef __AMSDEBUG__
	if(i==2 && AMSgvolume::debug){
          MTX(xnrm,xx);
          int a1,a2;
          for( a1=0;a1<3;a1++)for( a2=0;a2<3;a2++)nrm[a2][a1]=xnrm[a1][a2];
	}
#endif
        gid=i+1+10*(j+1)+1000*(k+1);
        if(TKDBc::activeladdshuttle(i+1,j+1,1)){  
	  if(k==TKDBc::nhalf(i+1,j+1))nhalfL++;
	  integer status=1;
	  if(TKDBc::update())TKDBc::SetSensor(i,j,k,status,coo,nrm,gid);
	  else               TKDBc::GetSensor(i,j,k,status,coo,nrm);
	  if(status){
	    cur=lad[1]->add(new AMSgvolume(
					   "ACTIVE_SILICON",nrot++,name,"BOX",par,3,coo,nrm,"ONLY",1,gid,1));
	    //          cout <<"add volume "<<name<<" "<<gid<<endl;
	    //          cout <<" to "<<*(lad[1]);
	  }
        }
      }


    }
    // Now Support foam
 
    for ( j=0;j<TKDBc::nlad(i+1);j++){
      ost.clear();
      ost.seekp(0);
      ost << "FOA" << i+1<<ends;
      par[0]=TKDBc::ssize_inactive(i,0)*TKDBc::nsen(i+1,j+1)/2.;
      par[1]=TKDBc::c2c(i)/2;
      par[2]=(TKDBc::support_foam_w(i))/2;    
      coo[0]=0;
      coo[1]=(TKDBc::nlad(i+1)-j)*TKDBc::c2c(i)-(TKDBc::nlad(i+1)+1)*TKDBc::c2c(i)/2.;
      coo[2]=TKDBc::zpos(i)-TKDBc::silicon_z(i)/2.-par[2]-TKDBc::support_foam_tol(i);
      VZERO(nrm,9*sizeof(nrm[0][0])/4);
      nrm[0][0]=1;
      nrm[1][1]=1;
      nrm[2][2]=1;
      gid=i+1+10*(j+1);
      cur=dau->add(new AMSgvolume(
				  "Tr_Foam",nrot++,name,"BOX",par,3,coo,nrm,"ONLY",1,gid,1));

    }

    // Now Honecomb Planes

    ost.clear();
    ost.seekp(0);
    ost << "PLA" << i+1<<ends;
    par[0]=0;
    par[1]=TKDBc::support_hc_r(i);
    par[2]=TKDBc::support_hc_w(i)/2;
    coo[0]=0;
    coo[1]=0;
    coo[2]=TKDBc::zpos(i)-TKDBc::silicon_z(i)/2.+TKDBc::support_hc_z(i);
    VZERO(nrm,9*sizeof(nrm[0][0])/4);
    nrm[0][0]=1;
    nrm[1][1]=1;
    nrm[2][2]=1;
    gid=i+1;
    cur=dau->add(new AMSgvolume(
				"Tr_Honeycomb",nrot++,name,"TUBE",par,3,coo,nrm,"ONLY",1,gid,1));



  }
  cout <<"<---- TKGeom-I-"<<nhalfL<<" Active halfladders initialized"<<endl<<endl;

}
