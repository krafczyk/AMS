#include <tkdbc.h>
#include <amsdbc.h>
#include <astring.h>
#include <job.h>
#include <commons.h>
TKDBc * TKDBc::_HeadSensor=0;
TKDBc * TKDBc::_HeadLayer=0;
TKDBc * TKDBc::_HeadLadder[2]={0,0};
TKDBc * TKDBc::_HeadMarker[2]={0,0};
integer TKDBc::_NumberSen=0;
integer TKDBc::_NumberMarkers=0;
integer TKDBc::_NumberLayer=0;
integer TKDBc::_NumberLadder=0;
integer TKDBc::_ReadOK=0;
void TKDBc::init(){
  // calculate sensor #
       int i,j,k;
       for ( i=0;i<AMSDBc::nlay();i++){
        for ( j=0;j<AMSDBc::nlad(i+1);j++){
         for ( k=0;k<AMSDBc::nsen(i+1,j+1);k++)_NumberSen++;
        }
       }

       cout <<"TKDBcI-I-Total of " <<_NumberSen<< "  sensors initialized."<<endl;
  // calculate ladder #
       for ( i=0;i<AMSDBc::nlay();i++){
        for ( j=0;j<AMSDBc::nlad(i+1);j++)_NumberLadder++;
       }

       cout <<"TKDBcI-I-Total of " <<_NumberLadder<< "  ladders initialized."<<endl;
       _NumberMarkers=AMSDBc::nlay()*4;
       _HeadSensor=new TKDBc[_NumberSen];
       _HeadLadder[0]=new TKDBc[_NumberLadder];
       _HeadLadder[1]=new TKDBc[_NumberLadder];
       _NumberLayer=AMSDBc::nlay();
       _HeadLayer=new TKDBc[_NumberLayer];
       _HeadMarker[0]= new TKDBc[_NumberMarkers]; 
       _HeadMarker[1]= new TKDBc[_NumberMarkers]; 
}

void TKDBc::read(){
if(!TKGEOMFFKEY.ReadGeomFromFile)return;

AString fnam(AMSDATADIR.amsdatadir);
fnam+="TKGeom_";
fnam+=AMSJob::gethead()->getsetup();
fnam+= AMSJob::gethead()->isRealData()?".1":".0";
ifstream iftxt((const char *)fnam,ios::in);
int active=0;
if(iftxt){
  // File exists...
 int i,j,k,l,idum,gid;
 char lay;
 for(i=0;i<_NumberLayer;i++){
   iftxt >> lay>>lay>>lay>>lay>>lay>>idum;
   iftxt >> gid>>_HeadLayer[idum]._status;
   for(j=0;j<3;j++)iftxt >> _HeadLayer[idum]._coo[j];
   for(j=0;j<3;j++){
     for(k=0;k<3;k++)iftxt >> _HeadLayer[idum]._nrm[j][k];
   }
 }
 for(l=0;l<2;l++){
 for(i=0;i<_NumberMarkers;i++){
   iftxt >> lay>>lay>>lay>>lay>>lay>>lay>>lay>>idum;
   iftxt >> gid>>_HeadMarker[l][idum]._status;
   for(j=0;j<3;j++)iftxt >> _HeadMarker[l][idum]._coo[j];
   for(j=0;j<3;j++){
     for(k=0;k<3;k++)iftxt >> _HeadMarker[l][idum]._nrm[j][k];
   }
 }
 }
 for(i=0;i<_NumberLadder;i++){
   iftxt >> lay>>lay>>lay>>lay>>lay>>lay>>lay>>idum;
   iftxt >> gid>>_HeadLadder[0][idum]._status;
   for(j=0;j<3;j++)iftxt >> _HeadLadder[0][idum]._coo[j];
   for(j=0;j<3;j++){
     for(k=0;k<3;k++)iftxt >> _HeadLadder[0][idum]._nrm[j][k];
   }
 }
 for(i=0;i<_NumberLadder;i++){
   iftxt >> lay>>lay>>lay>>lay>>lay>>lay>>lay>>idum;
   iftxt >> gid>>_HeadLadder[1][idum]._status;
   for(j=0;j<3;j++)iftxt >> _HeadLadder[1][idum]._coo[j];
   for(j=0;j<3;j++){
     for(k=0;k<3;k++)iftxt >> _HeadLadder[1][idum]._nrm[j][k];
   }
   if(lay=='P'){
    _HeadLadder[1][idum]._nrm[2][0]*=-1;
    _HeadLadder[1][idum]._nrm[2][1]*=-1;
    _HeadLadder[1][idum]._nrm[0][2]*=-1;
    _HeadLadder[1][idum]._nrm[1][2]*=-1;
   }
 }



 for(i=0;i<_NumberSen;i++){
   iftxt >> idum >> gid;
   iftxt>>_HeadSensor[idum]._status;
   for(j=0;j<3;j++)iftxt >> _HeadSensor[idum]._coo[j];
   for(j=0;j<3;j++){
     for(k=0;k<3;k++)iftxt >> _HeadSensor[idum]._nrm[j][k];
   }
   if(_HeadSensor[idum]._status!=0)active++;
 }
if(iftxt.eof() ){
  cerr<< "TKDBc::read-F-Unexpected EOF in file "<<fnam<<endl;
}
else {
 _ReadOK=1;
 cout <<"TKDBc::read-I-"<<active<<" active sensors have been read from "<<fnam<<endl;
  compileg();
}

}
else {
  cerr <<" TKDBc::read-E-File not exists "<<fnam <<endl;
  exit(1);
}


}

void TKDBc::compileg(){
if(!TKGEOMFFKEY.UpdateGeomFile)return;

   // modify some sensors
   (_HeadSensor[621])._status=0;
   (_HeadSensor[1288])._status=0;
   (_HeadSensor[1407])._status=0;
   (_HeadSensor[1122])._nrm[0][0]=cos(1.5e-3);
   (_HeadSensor[1122])._nrm[1][0]=sin(1.5e-3);
   (_HeadSensor[1122])._nrm[0][1]=-sin(1.5e-3);
   (_HeadSensor[1122])._nrm[1][1]=cos(1.5e-3);




  //read files and try to compile tracker geometry
 char files[nl][256]={"pl1_sf1.cer",
                   "pl2_sf4.gme",
                   "pl3_sf4.gme",
                   "pl4_sf3.gme",
                   "pl5_sf5.gme",
                   "pl6_sf2.cer"};

    int rsize=1024;
    TRLDGM_def TRLDGM;
    TRLYGM_def TRLYGM;
    int iostat;
    HROPEN(IOPA.hlun+1,"geometry","trgeometry","N",rsize,iostat);
    HBNT(IOPA.ntuple,"Tracker Geometry Ladders"," ");
    HBNAME(IOPA.ntuple,"trldgm",(int*)&(TRLDGM),"Layer:I,Ladder:I,Half:I,Coo(3),nrm(3,3):R,CooO(3),nrmO(3,3):R,Rx(10),Ry(10),Rz(10),status(10):I");
    HBNT(IOPA.ntuple+1,"Tracker Geometry Layers"," ");
    HBNAME(IOPA.ntuple+1,"trlygm",(int*)&(TRLYGM),"Layer:I,Coo(3),nrm(3,3):R,CooO(3),nrmO(3,3):R,Ra(4),Rx(4),Ry(4),Rz(4)");
     AMSPoint  CooMarkers[6][4];
  //ladders info outer planes only
   for(int il=0;il<6;il+=5){
    AString fnam(AMSDATADIR.amsdatadir);
    fnam+=files[il];
    ifstream fbin;
    fbin.open(fnam);
    if(!fbin){
       cerr<<"TKDBc::compileg-F-Couldnot open file"<<fnam<<endl;
       exit(1);
    }
     const int nls=17;
     integer StaLadders[nls][2];
     AMSPoint CooLadder[nls][2][10];
     integer CooStatus[nls][2][10];
     integer xPosLadder[nls][2][10];
     number  LadderNrm[nls][2][3][3];
     AMSPoint LadderCoo[nls][2];
     for(int j=0;j<nls;j++){
      if(j>6 && j<(il==0?11:10))for(int k=0;k<2;k++)StaLadders[j][k]=1;
      else for(int k=0;k<2;k++)StaLadders[j][k]=0;
      for(int l=0;l<10;l++)CooStatus[j][0][l]=1;
      for( l=0;l<10;l++)CooStatus[j][1][l]=1;
     }
        //read meas points
     char junk[256];
     for(j=0;j<80;j++){
       int ladder=10-j/20;
       if(il==5)ladder=6+j/20;
       int seg=(j%20)/5;
       
       int half= seg%2==0?1:0;
       if(il==5)half= seg%2==0?0:1;
       int meas;
       if(seg==0)meas=j%20;
       else if(seg==1)meas=j%20-5;
       else if(seg==2)meas=j%20-5;
       else meas=j%20-10;
       if(meas<0 || meas>9)cerr <<" wrong meas "<<meas<<endl;
       fbin>>junk>>junk;
       fbin>>CooLadder[ladder][half][meas][0]>>junk;
       fbin>>CooLadder[ladder][half][meas][1]>>junk;
       fbin>>CooLadder[ladder][half][meas][2];
       fbin.ignore(INT_MAX,'\n');
       //cout <<meas <<" "<<j<<" "<<half<<" "<<ladder<<" "<<il<<endl;
       //cout <<CooLadder[ladder][half][meas]<<endl;
       if(il==0){
        CooLadder[ladder][half][meas][0]=-CooLadder[ladder][half][meas][0];
        CooLadder[ladder][half][meas][1]=-CooLadder[ladder][half][meas][1];
       }
       CooLadder[ladder][half][meas][0]/=10.;
       CooLadder[ladder][half][meas][1]/=10.;
       CooLadder[ladder][half][meas][2]/=10.;
       CooLadder[ladder][half][meas][2]-=0.015; 
     }
    for(j=0;j<4;j++){
     fbin>>junk>>junk;
       int meas;
       if(il==0){
        if(j==0)meas=1;
        else if(j==1)meas=0;
        else if(j==2)meas=3;
        else meas=2;
       }
       else {
        if(j==0)meas=3;
        else if(j==1)meas=2;
        else if(j==2)meas=1;
        else meas=0;
       }
       fbin>>CooMarkers[il][meas][0]>>junk;
       fbin>>CooMarkers[il][meas][1]>>junk;
       fbin>>CooMarkers[il][meas][2]>>junk;
       fbin.ignore(INT_MAX,'\n');
       // change here as plane 6 was wrong positioned
       if(il==0){
        CooMarkers[il][meas][0]*=-1;
        CooMarkers[il][meas][1]*=-1;
       }
       CooMarkers[il][meas][0]/=10.;
       CooMarkers[il][meas][1]/=10.;
       CooMarkers[il][meas][2]=0; 
    }
       number cc[3];
       cc[0]=CooMarkers[il][0][0]+ CooMarkers[il][1][0]+ CooMarkers[il][2][0]+CooMarkers[il][3][0];
       cc[0]/=4; 
       cc[1]=CooMarkers[il][0][1]+ CooMarkers[il][1][1]+ CooMarkers[il][2][1]+CooMarkers[il][3][1];
       cc[1]/=4; 
       cc[2]=1.017;
     for(j=0;j<4;j++){
         for(int l=0;l<3;l++)CooMarkers[il][j][l]-=cc[l];
     }
     for(j=0;j<nls;j++){
      for(int k=0;k<2;k++){
       if(StaLadders[j][k]){
        for(int m=0;m<10;m++){
         for(int l=0;l<3;l++)CooLadder[j][k][m][l]-=cc[l];
        }
       }
      }
     }
     if(il==0){
      for(int k=6;k<12;k++){
       xPosLadder[k][0][0]=151;
       xPosLadder[k][0][1]=110;
       xPosLadder[k][0][2]=71;
       xPosLadder[k][0][3]=31;
       xPosLadder[k][0][4]=21;
       xPosLadder[k][0][5]=151;
       xPosLadder[k][0][6]=130;
       xPosLadder[k][0][7]=91;
       xPosLadder[k][0][8]=51;
       xPosLadder[k][0][9]=21;

       xPosLadder[k][1][0]=21;
       xPosLadder[k][1][1]=31;
       xPosLadder[k][1][2]=70;
       xPosLadder[k][1][3]=110;
       xPosLadder[k][1][4]=151;
       xPosLadder[k][1][5]=21;
       xPosLadder[k][1][6]=50;
       xPosLadder[k][1][7]=90;
       xPosLadder[k][1][8]=130;
       xPosLadder[k][1][9]=151;
      }
     }
     else{
      for(int k=6;k<12;k++){
       xPosLadder[k][1][0]=151;
       xPosLadder[k][1][1]=110;
       xPosLadder[k][1][2]=71;
       xPosLadder[k][1][3]=31;
       xPosLadder[k][1][4]=21;
       xPosLadder[k][1][5]=151;
       xPosLadder[k][1][6]=130;
       xPosLadder[k][1][7]=91;
       xPosLadder[k][1][8]=51;
       xPosLadder[k][1][9]=21;

       xPosLadder[k][0][0]=21;
       xPosLadder[k][0][1]=31;
       xPosLadder[k][0][2]=70;
       xPosLadder[k][0][3]=110;
       xPosLadder[k][0][4]=151;
       xPosLadder[k][0][5]=21;
       xPosLadder[k][0][6]=50;
       xPosLadder[k][0][7]=90;
       xPosLadder[k][0][8]=130;
       xPosLadder[k][0][9]=151;
      }
     }
      fitting(il,nls,10,StaLadders,CooLadder,CooStatus,LadderNrm, 
      TRLDGM, TRLYGM,xPosLadder,LadderCoo);

   }

    
  // ladders info, inners planes only
  
  for(il=1;il<5;il++){
    AString fnam(AMSDATADIR.amsdatadir);
    fnam+=files[il];
    ifstream fbin;
    fbin.open(fnam);
    if(!fbin){
       cerr<<"TKDBc::compileg-F-Couldnot open file"<<fnam<<endl;
       exit(1);
    }
     const int nls=17;
     integer CooStatus[nls][2][10];
     integer StaLadders[nls][2];
     AMSPoint CooLadder[nls][2][10];
     integer xPosLadder[nls][2][10];
     number  LadderNrm[nls][2][3][3];
     AMSPoint LadderCoo[nls][2];
     for(int j=0;j<nls;j++){
      if(j>3 && j<10)for(int k=0;k<2;k++)StaLadders[j][k]=1;
      else for(int k=0;k<2;k++)StaLadders[j][k]=0;
      for(int l=0;l<8;l++)CooStatus[j][0][l]=1;
      for( l=0;l<8;l++)CooStatus[j][1][l]=1;
     }
     if(il==1){
          CooStatus[4][0][6]=-1;
          CooStatus[9][0][5]=-1;
          CooStatus[9][0][6]=-1;
     }
     else if(il==2){
          CooStatus[4][0][3]=-1;
          CooStatus[4][0][4]=-1;
          CooStatus[4][0][6]=-1;
          CooStatus[9][0][5]=-1;
          CooStatus[9][0][6]=-1;
     }
     else if(il==3){
          CooStatus[4][0][6]=-1;
          CooStatus[9][0][5]=-1;
          CooStatus[7][0][0]=-1;
     }
     else if(il==4){
          CooStatus[9][0][4]=-1;
          CooStatus[9][0][6]=-1;
     }
     xPosLadder[9][0][0]=111;
     xPosLadder[9][0][1]=81;
     xPosLadder[9][0][2]=51;
     xPosLadder[9][0][3]=21;
     xPosLadder[9][0][4]=21;
     xPosLadder[9][0][5]=70;
     xPosLadder[9][0][6]=100;
     xPosLadder[9][0][7]=111;

     xPosLadder[8][0][0]=121;
     xPosLadder[8][0][1]=81;
     xPosLadder[8][0][2]=51;
     xPosLadder[8][0][+3]=21;
     xPosLadder[8][0][+4]=21;
     xPosLadder[8][0][+5]=71;
     xPosLadder[8][0][+6]=101;
     xPosLadder[8][0][+7]=121;

     xPosLadder[7][0][+0]=121;
     xPosLadder[7][0][+1]=81;
     xPosLadder[7][0][+2]=51;
     xPosLadder[7][0][+3]=21;
     xPosLadder[7][0][+4]=21;
     xPosLadder[7][0][+5]=71;
     xPosLadder[7][0][+6]=101;
     xPosLadder[7][0][+7]=121;

     xPosLadder[6][0][+0]=121;
     xPosLadder[6][0][+1]=81;
     xPosLadder[6][0][+2]=51;
     xPosLadder[6][0][+3]=21;
     xPosLadder[6][0][+4]=21;
     xPosLadder[6][0][+5]=71;
     xPosLadder[6][0][+6]=101;
     xPosLadder[6][0][+7]=121;

     xPosLadder[5][0][+0]=121;
     xPosLadder[5][0][+1]=81;
     xPosLadder[5][0][+2]=51;
     xPosLadder[5][0][+3]=21;
     xPosLadder[5][0][+4]=21;
     xPosLadder[5][0][+5]=71;
     xPosLadder[5][0][+6]=101;
     xPosLadder[5][0][+7]=121;

     xPosLadder[4][0][0]=111;
     xPosLadder[4][0][1]=81;
     xPosLadder[4][0][2]=51;
     xPosLadder[4][0][3]=21;
     xPosLadder[4][0][4]=21;
     xPosLadder[4][0][5]=70;
     xPosLadder[4][0][6]=100;
     xPosLadder[4][0][7]=111;

     xPosLadder[9][1][0]=121;
     xPosLadder[9][1][1]=101;
     xPosLadder[9][1][2]=71;
     xPosLadder[9][1][3]=21;
     xPosLadder[9][1][4]=21;
     xPosLadder[9][1][5]=51;
     xPosLadder[9][1][6]=81;
     xPosLadder[9][1][7]=121;

     xPosLadder[8][1][+0]=121;
     xPosLadder[8][1][+1]=101;
     xPosLadder[8][1][+2]=71;
     xPosLadder[8][1][+3]=21;
     xPosLadder[8][1][+4]=21;
     xPosLadder[8][1][+5]=51;
     xPosLadder[8][1][+6]=81;
     xPosLadder[8][1][+7]=121;

     xPosLadder[7][1][+0]=121;
     xPosLadder[7][1][+1]=101;
     xPosLadder[7][1][+2]=71;
     xPosLadder[7][1][+3]=21;
     xPosLadder[7][1][+4]=21;
     xPosLadder[7][1][+5]=51;
     xPosLadder[7][1][+6]=81;
     xPosLadder[7][1][+7]=121;

     xPosLadder[6][1][+0]=121;
     xPosLadder[6][1][+1]=101;
     xPosLadder[6][1][+2]=71;
     xPosLadder[6][1][+3]=21;
     xPosLadder[6][1][+4]=21;
     xPosLadder[6][1][+5]=51;
     xPosLadder[6][1][+6]=81;
     xPosLadder[6][1][+7]=121;

     xPosLadder[5][1][+0]=121;
     xPosLadder[5][1][+1]=101;
     xPosLadder[5][1][+2]=71;
     xPosLadder[5][1][+3]=21;
     xPosLadder[5][1][+4]=21;
     xPosLadder[5][1][+5]=51;
     xPosLadder[5][1][+6]=81;
     xPosLadder[5][1][+7]=121;



     xPosLadder[4][1][0]=121;
     xPosLadder[4][1][1]=101;
     xPosLadder[4][1][2]=71;
     xPosLadder[4][1][3]=21;
     xPosLadder[4][1][4]=21;
     xPosLadder[4][1][5]=51;
     xPosLadder[4][1][6]=81;
     xPosLadder[4][1][7]=121;

     char junk[256];
     for(j=0;j<96;j++){
      fbin>>junk>>junk>>junk>>junk>>junk;
      int half=j/48;
      int ladder = half==0 ? 9-(j%48)/8 : 4+(j%48)/8;
      int mea=(j%48)%8;
      fbin>>CooLadder[ladder][half][mea][0]>>CooLadder[ladder][half][mea][1]>>
      CooLadder[ladder][half][mea][2];
      for(int l=0;l<3;l++)CooLadder[ladder][half][mea][l]/=10;
      CooLadder[ladder][half][mea][2]-=0.017;
      fbin.ignore(INT_MAX,'\n');
     }
     for(j=0;j<4;j++){
      fbin>>junk>>junk>>junk>>junk;
      fbin>>CooMarkers[il][j][0]>>  CooMarkers[il][j][1]>>  CooMarkers[il][j][2];       
      CooMarkers[il][j][2]=0; 
      for(int l=0;l<3;l++)CooMarkers[il][j][l]/=10;
      fbin.ignore(INT_MAX,'\n');
     }
    // Now define Ladder coo system center:
       number cc[3];
       cc[0]=CooMarkers[il][0][0]+ CooMarkers[il][1][0]+ CooMarkers[il][2][0]+CooMarkers[il][3][0];
       cc[0]/=4; 
       cc[1]=CooMarkers[il][0][1]+ CooMarkers[il][1][1]+ CooMarkers[il][2][1]+CooMarkers[il][3][1];
       cc[1]/=4; 
       cc[2]=1.017;
     for(j=0;j<4;j++){
         for(int l=0;l<3;l++)CooMarkers[il][j][l]-=cc[l];
     }
     for(j=0;j<nls;j++){
      for(int k=0;k<2;k++){
       if(StaLadders[j][k]){
        for(int m=0;m<8;m++){
         for(int l=0;l<3;l++)CooLadder[j][k][m][l]-=cc[l];
        }
       }
      }
     }
     // now test for 1st ladder 
     // StaLadders[0][0]=1;
      CooLadder[0][0][0][0]=-1;
      CooLadder[0][0][1][0]=-3;
      CooLadder[0][0][2][0]=-7;
      CooLadder[0][0][3][0]=-9;
      CooLadder[0][0][4][0]=-9;
      CooLadder[0][0][5][0]=-6;
      CooLadder[0][0][6][0]=-4;
      CooLadder[0][0][7][0]=-1;
      for(int k=0;k<4;k++)CooLadder[0][0][k][1]=0;
      for(k=4;k<8;k++)CooLadder[0][0][k][1]=6;
      for(k=0;k<8;k++)CooLadder[0][0][k][2]=2;
      number xcl=-5.;
      number ycl=3;
      number zcl=2;
      geant d;
      number xc=RNDM(d);
      number yc=RNDM(d);
      number zc=RNDM(d);
      xc=xcl;
      yc=ycl;
      zc=zcl;
      number pitch=RNDM(d);
      number yaw=RNDM(d);
      number roll=RNDM(d);
      number cp=cos(pitch);
      number sp=sin(pitch);
      number cy=cos(yaw);
      number sy=sin(yaw);
      number cr=cos(roll);
      number sr=sin(roll);
      number l1=cy*cp;
      number m1=-sy;
      number n1=cy*sp;
      number l2=cr*sy*cp-sr*sp;
      number m2=cr*cy;
      number n2=cr*sy*sp+sr*cp;
      number l3=-sr*sy*cp-cr*sp;
      number m3=-sr*cy;
      number n3=-sr*sy*sp+cr*cp;
      number xcg=l1*(xcl-xc)+l2*(ycl-yc)+l3*(zcl-zc)+xc;
      number ycg=m1*(xcl-xc)+m2*(ycl-yc)+m3*(zcl-zc)+xc;
      number zcg=n1*(xcl-xc)+n2*(ycl-yc)+n3*(zcl-zc)+xc;
      for(k=0;k<8;k++){
       AMSPoint amsx;
       amsx[0]=l1*(CooLadder[0][0][k][0]-xc)+l2*(CooLadder[0][0][k][1]-yc)+l3*(CooLadder[0][0][k][2]-zc)+xc;
       amsx[1]=m1*(CooLadder[0][0][k][0]-xc)+m2*(CooLadder[0][0][k][1]-yc)+m3*(CooLadder[0][0][k][2]-zc)+yc;
       amsx[2]=n1*(CooLadder[0][0][k][0]-xc)+n2*(CooLadder[0][0][k][1]-yc)+n3*(CooLadder[0][0][k][2]-zc)+zc;
       CooLadder[0][0][k]=amsx;
      }
      fitting(il,nls,8,StaLadders,CooLadder,CooStatus,LadderNrm,
      TRLDGM, TRLYGM,xPosLadder,LadderCoo);
    }
    // now layers measurement
    ifstream fbin;
    AString fnam(AMSDATADIR.amsdatadir);
    fnam+="metr.tracker";
    fbin.open(fnam);
    if(!fbin){
       cerr<<"TKDBc::compileg-F-Couldnot open file"<<fnam<<endl;
       exit(1);
    }
    char junk[256];
    AMSPoint CooMarkersG[6][4];
    number LayerNrm[6][3][3];
    integer invert[6]={-1,1,1,-1,-1,1};
    number rzp[6][3];
    for(il=0;il<4;il++){
     fbin>>junk;
     for(int i=0;i<6;i++)fbin>>rzp[i][0];        
     fbin.ignore(INT_MAX,'\n');
     fbin>>junk;
     for(i=0;i<6;i++)fbin>>rzp[i][1];        
     fbin.ignore(INT_MAX,'\n');
     fbin>>junk;
     for(i=0;i<6;i++)fbin>>rzp[i][2];        
     fbin.ignore(INT_MAX,'\n');
     for(i=0;i<6;i++){
     int meas;
     if(invert[i] >0 ){   
      if(il==0)meas=2;
      else if(il==1)meas=0;
      else if(il==2)meas=3;
      else meas=1;
     }
     else{
      if(il==3)meas=2;
      else if(il==2)meas=0;
      else if(il==1)meas=3;
      else meas=1;
     }
      CooMarkersG[i][meas][0]=rzp[i][0]/10.*cos(rzp[i][2]/180.*AMSDBc::pi);    
      CooMarkersG[i][meas][1]=rzp[i][0]/10.*sin(rzp[i][2]/180.*AMSDBc::pi);    
      CooMarkersG[i][meas][2]=rzp[i][1]/10;
     }
    }
    fbin.close();
   // test plane
/*
{ 
      geant d;
      geant xc=3;
      geant yc=4;
      geant zc=5;
      cout <<xc<<" "<<yc<<" "<<zc<<endl;
      number pitch=RNDM(d);
       number yaw=RNDM(d);
       pitch=0.4;
       yaw=1.3;
      number roll=3.14;
      number cp=cos(pitch);
      number sp=sin(pitch);
      number cy=cos(yaw);
      number sy=sin(yaw);
      number cr=cos(roll);
      number sr=sin(roll);
      number l1=cy*cp;
      number m1=-sy;
      number n1=cy*sp;
      number l2=cr*sy*cp-sr*sp;
      number m2=cr*cy;
      number n2=cr*sy*sp+sr*cp;
      number l3=-sr*sy*cp-cr*sp;
      number m3=-sr*cy;
      number n3=-sr*sy*sp+cr*cp;
      cout <<l1<<" "<<l2<<" "<<l3<<endl;
      cout <<m1<<" "<<m2<<" "<<m3<<endl;
      cout <<n1<<" "<<n2<<" "<<n3<<endl;
      for(int k=0;k<4;k++){
//       cout <<"coomar "<<CooMarkers[1][k]<<endl; 
       CooMarkersG[0][k][0]=l1*CooMarkers[0][k][0]+l2*(CooMarkers[0][k][1])+l3*(CooMarkers[0][k][2])+xc;
       CooMarkersG[0][k][1]=m1*CooMarkers[0][k][0]+m2*(CooMarkers[0][k][1])+m3*(CooMarkers[0][k][2])+yc;
       CooMarkersG[0][k][2]=n1*CooMarkers[0][k][0]+n2*(CooMarkers[0][k][1])+n3*(CooMarkers[0][k][2])+zc;
      }
}
*/
      int ibeg =0;   
      for(il=ibeg;il<6;il++){
        number x(0),y(0),z(0),xy(0),xz(0),yz(0),x2(0),y2(0),z2(0);
        int nm=4;
        for(int k=0;k<nm;k++){
         //cout <<" CooMarkerG "<<il<<" "<<CooMarkersG[il][k]<<endl; 
         x+=CooMarkersG[il][k][0];
         y+=CooMarkersG[il][k][1];
         z+=CooMarkersG[il][k][2];
         xy+=CooMarkersG[il][k][0]*CooMarkersG[il][k][1];
         xz+=CooMarkersG[il][k][0]*CooMarkersG[il][k][2];
         yz+=CooMarkersG[il][k][1]*CooMarkersG[il][k][2];
         x2+=CooMarkersG[il][k][0]*CooMarkersG[il][k][0];
         y2+=CooMarkersG[il][k][1]*CooMarkersG[il][k][1];
         z2+=CooMarkersG[il][k][2]*CooMarkersG[il][k][2];
        }
         x/=nm;
         y/=nm;
         z/=nm;
         xz/=nm;
         yz/=nm;
         xy/=nm;
         x2/=nm;
         y2/=nm;
         z2/=nm;
         number dx2=x2-x*x;
         number dy2=y2-y*y;
         number dxy=xy-x*y;
         number dxz=xz-x*z;
         number dyz=yz-y*z;
         number b=(dx2*dyz-dxz*dxy)/(dx2*dy2-dxy*dxy);
         number a=(dxz*dy2-dyz*dxy)/(dx2*dy2-dxy*dxy);
         number c=z-a*x-b*y;
         //cout <<" c "<<c<<endl;
         AMSDir u,v(-a,-b,1),w; 
         number pitch=atan2(-v[0],v[2]);
         number roll=asin(-v[1]);
         cout <<"pitch "<<pitch<<" "<<roll<<endl;
         LayerNrm[il][0][0]=cos(pitch);
         LayerNrm[il][1][0]=0;
         LayerNrm[il][2][0]=sin(pitch);
         LayerNrm[il][0][1]=-sin(roll)*sin(pitch)*invert[il];            
         LayerNrm[il][1][1]=cos(roll)*invert[il];            
         LayerNrm[il][2][1]=sin(roll)*cos(pitch)*invert[il];
         LayerNrm[il][0][2]=-cos(roll)*sin(pitch)*invert[il];
         LayerNrm[il][1][2]=-sin(roll)*invert[il];
         LayerNrm[il][2][2]=cos(roll)*cos(pitch)*invert[il];
        
         u=AMSDir(LayerNrm[il][0][0],LayerNrm[il][1][0],LayerNrm[il][2][0]);
         v=AMSDir(LayerNrm[il][0][1],LayerNrm[il][1][1],LayerNrm[il][2][1]);
         w=AMSDir(LayerNrm[il][0][2],LayerNrm[il][1][2],LayerNrm[il][2][2]);
         AMSPoint tmp[4];
          
         for(k=0;k<nm;k++){
          AMSPoint amsx;
          amsx[0]=u.prod(CooMarkersG[il][k]);
          amsx[1]=v.prod(CooMarkersG[il][k]);
          amsx[2]=w.prod(CooMarkersG[il][k]);
          tmp[k]=amsx;
         }
          // now we have a x-y system only
         number angle[4];
         number angleg[4];
         for(k=0;k<nm;k++){
          int ik=(k+1)%4;
          angle[k]=atan2(CooMarkers[il][ik][1]-CooMarkers[il][k][1],
                        CooMarkers[il][ik][0]-CooMarkers[il][k][0]);
          angleg[k]=atan2(tmp[ik][1]-tmp[k][1],tmp[ik][0]-tmp[k][0]);
          //cout <<"t1 "<<tmp[ik][1]<< " "<<tmp[k][1]<<endl;
          //cout <<"t0 "<<tmp[ik][0]<< " "<<tmp[k][0]<<endl;
         }
         a=0;
         for(k=0;k<nm;k++){
          a+=asin(sin(angleg[k]-angle[k]));
          //cout <<"diff "<<angleg[k]-angle[k]<<" "<<fmod(angleg[k]-angle[k]+AMSDBc::twopi,AMSDBc::twopi)<<" "<<a/(k+1)<<endl;
         }
         a/=nm;
         //cout <<"a "<<a<<endl;
         for(k=0;k<nm;k++){
          TRLYGM.Ra[k]=angleg[k]-angle[k]-a;
         }
         AMSDir matrix[3];
         matrix[0]=AMSDir(cos(a),sin(a),0);         
         matrix[1]=AMSDir(-sin(a),cos(a),0);         
         matrix[2]=AMSDir(0,0,1);         
         number nrm[3][3];
         nrm[0][0]=matrix[0][0];
         nrm[1][0]=matrix[0][1];
         nrm[2][0]=matrix[0][2];
         nrm[0][1]=matrix[1][0];
         nrm[1][1]=matrix[1][1];
         nrm[2][1]=matrix[1][2];
         nrm[0][2]=matrix[2][0];
         nrm[1][2]=matrix[2][1];
         nrm[2][2]=matrix[2][2];
         mm3(LayerNrm[il],nrm,0);
         //cout <<nrm[0][0]<< " "<<nrm[0][1]<< " "<<nrm[0][2]<<endl;
         //cout <<nrm[1][0]<< " "<<nrm[1][1]<< " "<<nrm[1][2]<<endl;
         //cout <<nrm[2][0]<< " "<<nrm[2][1]<< " "<<nrm[2][2]<<endl;
         TRLYGM.Nrm[0][0]=nrm[0][0];
         TRLYGM.Nrm[0][1]=nrm[1][0];
         TRLYGM.Nrm[0][2]=nrm[2][0];
         TRLYGM.Nrm[1][0]=nrm[0][1];
         TRLYGM.Nrm[1][1]=nrm[1][1];
         TRLYGM.Nrm[1][2]=nrm[2][1];
         TRLYGM.Nrm[2][0]=nrm[0][2];
         TRLYGM.Nrm[2][1]=nrm[1][2];
         TRLYGM.Nrm[2][2]=nrm[2][2];
         UCOPY(nrm,LayerNrm[il],sizeof(nrm)/sizeof(integer));
         u=AMSDir(LayerNrm[il][0][0],LayerNrm[il][0][1],LayerNrm[il][0][2]);
         v=AMSDir(LayerNrm[il][1][0],LayerNrm[il][1][1],LayerNrm[il][1][2]);
         w=AMSDir(LayerNrm[il][2][0],LayerNrm[il][2][1],LayerNrm[il][2][2]);
         for(k=0;k<nm;k++){
          AMSPoint amsx;
          amsx[0]=u.prod(CooMarkers[il][k]);
          amsx[1]=v.prod(CooMarkers[il][k]);
          amsx[2]=w.prod(CooMarkers[il][k]);
          tmp[k]=amsx;
         }
         AMSPoint cl(0,0,0);
         for(k=0;k<nm;k++){
          for(int l=0;l<3;l++)cl[l]+=CooMarkersG[il][k][l]-tmp[k][l];
         }
         cl=cl/nm;         
         //cout <<" cl "<<cl<<endl;
         for(k=0;k<nm;k++){
          TRLYGM.Rx[k]=CooMarkersG[il][k][0]-tmp[k][0]-cl[0];
          TRLYGM.Ry[k]=CooMarkersG[il][k][1]-tmp[k][1]-cl[1];
          TRLYGM.Rz[k]=CooMarkersG[il][k][2]-tmp[k][2]-cl[2];
         }
         TRLYGM.Coo[0]=cl[0];
         TRLYGM.Coo[1]=cl[1];
         TRLYGM.Coo[2]=cl[2];
         TRLYGM.Layer=il+1;
         integer status;
         integer rgid;
         GetLayer(TRLYGM.Layer,status,TRLYGM.CooO,nrm,rgid);
         if(TKGEOMFFKEY.UpdateGeomFile==1 || TRLYGM.Layer!=1){
          SetLayer(TRLYGM.Layer,status,TRLYGM.Coo,LayerNrm[il],rgid);
         }
         TRLYGM.NrmO[0][0]=nrm[0][0];
         TRLYGM.NrmO[0][1]=nrm[1][0];
         TRLYGM.NrmO[0][2]=nrm[2][0];
         TRLYGM.NrmO[1][0]=nrm[0][1];
         TRLYGM.NrmO[1][1]=nrm[1][1];
         TRLYGM.NrmO[1][2]=nrm[2][1];
         TRLYGM.NrmO[2][0]=nrm[0][2];
         TRLYGM.NrmO[2][1]=nrm[1][2];
         TRLYGM.NrmO[2][2]=nrm[2][2];
        
        HFNT(IOPA.ntuple+1); 

      }
  char hpawc[256]="//PAWC";
  HCDIR (hpawc, " ");
  char houtput[]="//geometry";
  HCDIR (houtput, " ");
  integer ICYCL=0;
  HROUT (0, ICYCL, " ");
  HREND ("geometry");
  CLOSEF(IOPA.hlun+1);

   cout <<"TKDBc::update-I-GeomFileCompiled "<<endl;
   write(TKGEOMFFKEY.UpdateGeomFile);

}


void TKDBc::updatef(){
char dum;
number rdum;
if(!TKGEOMFFKEY.UpdateGeomFile)return;

// First planes

//   (_HeadLayer[0])._coo[2]= 50.949;
//   (_HeadLayer[1])._coo[2]=  29.2141;
//   (_HeadLayer[2])._coo[2]= 7.78935;
//   (_HeadLayer[3])._coo[2]= -7.78935;
//   (_HeadLayer[4])._coo[2]= -29.14585;
//   (_HeadLayer[5])._coo[2]=  -50.99;



{
   AString fnam="../../metro/metr.m.2";
   ifstream iftxt((const char *)fnam,ios::in);
      if(iftxt){
          for(int i=0;i<AMSDBc::nlay();i++){
         for(int l=0;l<3;l++)iftxt >>(_HeadLayer[i])._coo[l];
         for(int j=0;j<3;j++){
          for(int k=0;k<3;k++)iftxt >>(_HeadLayer[i])._nrm[k][j];
         }
    }
    iftxt.close();
      }
   else {
    cerr <<"TKDBc::update-E-CouldNotOpenFile "<<fnam<<endl;
   }
}  

// Temporary adjust to 1234 center
{
   number cp=0;
   for(int i=1;i<5;i++)cp+=_HeadLayer[i]._coo[2];
   cp=cp/4;
   for( i=0;i<AMSDBc::nlay();i++)_HeadLayer[i]._coo[2]+=-cp;
}
  

   //Now modify some sensors
   (_HeadSensor[621])._status=0;
   (_HeadSensor[1288])._status=0;
   (_HeadSensor[1407])._status=0;
   (_HeadSensor[1122])._nrm[0][0]=cos(1.5e-3);
   (_HeadSensor[1122])._nrm[1][0]=sin(1.5e-3);
   (_HeadSensor[1122])._nrm[0][1]=-sin(1.5e-3);
   (_HeadSensor[1122])._nrm[1][1]=cos(1.5e-3);


   AString fnam="../../metro/wjb/results.my";
   ifstream iftxt((const char *)fnam,ios::in);
   if(iftxt){


   // Now read Markers
   {
   iftxt >>dum>>dum>>dum>>dum>>dum>>dum>>dum;
   int i;
   for(i=0;i<AMSDBc::nlay();i++){
     for(int k=0;k<4;k++){
       for(int l=0;l<2;l++)iftxt>>(_HeadMarker[0][i*4+k])._coo[l];
       iftxt >> rdum ;   // Don't want z at the moment
     }
   }

   } 
   // now Ladders
   integer nladders=0;
   {
   integer ilay,ilad,ihalf,istat;
   for(;;){
     iftxt >> dum>>dum>>dum>>dum>>dum>>dum;
     iftxt >> ilay >> ilad >> ihalf>>istat;
     if(iftxt.eof())break;
     integer numl=getnumd(ilay-1,ilad-1);
     (_HeadLadder[ihalf][numl])._status=istat;
     for(int i=0;i<3;i++)iftxt >> (_HeadLadder[ihalf][numl])._coo[i];
     cout <<ilay<<" "<<ilad<<" "<<ihalf<<" "<<
     (_HeadLadder[ihalf][numl])._coo[0]<<" "<<
     (_HeadLadder[ihalf][numl])._coo[1]<<" "<<
     (_HeadLadder[ihalf][numl])._coo[2]<<endl;
     number xy,zx,zy;
     iftxt >> xy >> zx >> zy;
     number cx=sqrt(1.-(xy*xy+zx*zx));
     number cy=sqrt(1.-(xy*xy+zy*zy));
     number cz=sqrt(1.-(zx*zx+zy*zy));
     //     goto nahui;
     if(ihalf==0){ 
      (_HeadLadder[ihalf][numl])._nrm[0][0]=cx;
      (_HeadLadder[ihalf][numl])._nrm[1][0]=xy;
      (_HeadLadder[ihalf][numl])._nrm[2][0]=zx;
      (_HeadLadder[ihalf][numl])._nrm[0][1]=-xy;
      (_HeadLadder[ihalf][numl])._nrm[1][1]=cy;
      (_HeadLadder[ihalf][numl])._nrm[2][1]=zy;
      (_HeadLadder[ihalf][numl])._nrm[0][2]=-zx;
      (_HeadLadder[ihalf][numl])._nrm[1][2]=-zy;
      (_HeadLadder[ihalf][numl])._nrm[2][2]=cz;
     }
     else {
      (_HeadLadder[ihalf][numl])._nrm[0][0]=-cx;
      (_HeadLadder[ihalf][numl])._nrm[1][0]=-xy;
      (_HeadLadder[ihalf][numl])._nrm[2][0]=zx;
      (_HeadLadder[ihalf][numl])._nrm[0][1]=xy;
      (_HeadLadder[ihalf][numl])._nrm[1][1]=-cy;
      (_HeadLadder[ihalf][numl])._nrm[2][1]=zy;
      (_HeadLadder[ihalf][numl])._nrm[0][2]=zx;
      (_HeadLadder[ihalf][numl])._nrm[1][2]=zy;
      (_HeadLadder[ihalf][numl])._nrm[2][2]=cz;
     }
     //   nahui: 
    nladders++;
   }

   }
   cout <<"TKDBc::update-I-GeomFileUpdated for "<<nladders<<" ladders"<<endl;
   write(1);
}
else {
  cerr <<"TKDBc::update-E-CouldNotOpenFile "<<fnam<<endl;
}
}

void TKDBc::write(integer update){
if( (_ReadOK || !(TKGEOMFFKEY.WriteGeomToFile)) && !update)return;
AString fnam(AMSDATADIR.amsdatadir);
fnam+="TKGeom_";
fnam+=AMSJob::gethead()->getsetup();
if(update)fnam+=".update";
if(update==2)fnam+=".2";
else if (update==3)fnam+=".3";
ofstream iftxt;
if(update)iftxt.open((const char *)fnam,ios::out|ios::trunc);
else iftxt.open((const char *)fnam,ios::out|ios::noreplace);
if(iftxt){
  // Write file
  iftxt <<setprecision(12);
 int i,j,k,l,idum;
 for(i=0;i<_NumberLayer;i++){
   iftxt <<"Layer "<< i <<" "<< _HeadLayer[i]._gid<<" "<<_HeadLayer[i]._status<<endl;
   for(j=0;j<3;j++)iftxt << _HeadLayer[i]._coo[j]<<" ";
   iftxt <<endl;
   for(j=0;j<3;j++){
     for(k=0;k<3;k++)iftxt << _HeadLayer[i]._nrm[j][k]<<" ";
     iftxt <<endl;
   }
 }
 iftxt <<"      "<<endl;
 for(l=0;l<2;l++){
  for(i=0;i<_NumberMarkers;i++){
    iftxt <<"Marker N "<< i <<" "<< _HeadMarker[l][i]._gid<<" "<<_HeadMarker[l][i]._status<<endl;
    for(j=0;j<3;j++)iftxt << _HeadMarker[l][i]._coo[j]<<" ";
    iftxt <<endl;
    for(j=0;j<3;j++){
     for(k=0;k<3;k++)iftxt << _HeadMarker[l][i]._nrm[j][k]<<" ";
     iftxt <<endl;
    }
  }
 iftxt <<"      "<<endl;
 }

 for(i=0;i<_NumberLadder;i++){
   iftxt <<"Ladder L "<< i <<" "<< _HeadLadder[0][i]._gid<<" "<<_HeadLadder[0][i]._status<<endl;
   for(j=0;j<3;j++)iftxt << _HeadLadder[0][i]._coo[j]<<" ";
   iftxt <<endl;
   for(j=0;j<3;j++){
     for(k=0;k<3;k++)iftxt << _HeadLadder[0][i]._nrm[j][k]<<" ";
     iftxt <<endl;
   }
 }
 iftxt <<"      "<<endl;
 for(i=0;i<_NumberLadder;i++){
   iftxt <<"Ladder R "<< i <<" "<< _HeadLadder[1][i]._gid<<" "<<_HeadLadder[1][i]._status<<endl;
   for(j=0;j<3;j++)iftxt << _HeadLadder[1][i]._coo[j]<<" ";
   iftxt <<endl;
   for(j=0;j<3;j++){
     for(k=0;k<3;k++)iftxt << _HeadLadder[1][i]._nrm[j][k]<<" ";
     iftxt <<endl;
   }
 }
 iftxt <<"      "<<endl;
 for(i=0;i<_NumberSen;i++){
   iftxt << i <<" "<< _HeadSensor[i]._gid<<" "<<_HeadSensor[i]._status<<endl;
   for(j=0;j<3;j++)iftxt << _HeadSensor[i]._coo[j]<<" ";
   iftxt <<endl;
   for(j=0;j<3;j++){
     for(k=0;k<3;k++)iftxt << _HeadSensor[i]._nrm[j][k]<<" ";
     iftxt <<endl;
   }
 }
 iftxt << "           "<<endl;
 cout <<"TKDBC::write-I-GeometryWritten "<<fnam<<endl;
}
else cerr << " TKDBc::write-E-Error write "<<fnam <<endl;
}

void TKDBc::SetSensor(integer layer, integer ladder, integer sensor,
                      integer status, geant coo[], number nrm[3][3], integer gid){
        integer num = getnum(layer,ladder,sensor);
       #ifdef __AMSDEBUG__
          assert (num >=0 && num <_NumberSen);
       #endif
       int i,j;
       _HeadSensor[num]._status=status;
       _HeadSensor[num]._gid=gid;
       for(i=0;i<3;i++){
         _HeadSensor[num]._coo[i]=coo[i];
         for(j=0;j<3;j++)_HeadSensor[num]._nrm[i][j]=nrm[i][j];
       }
                
}

void TKDBc::SetLadder(integer layer, integer ladder,integer half,
      integer status, geant coo[], number nrm[3][3], integer gid){
        integer num = getnumd(layer,ladder);
       #ifdef __AMSDEBUG__
          assert (num >=0 && num <_NumberLadder);
       #endif
       int i,j;
       _HeadLadder[half][num]._status=status;
       _HeadLadder[half][num]._gid=gid;
       for(i=0;i<3;i++){
         _HeadLadder[half][num]._coo[i]=coo[i];
         for(j=0;j<3;j++)_HeadLadder[half][num]._nrm[i][j]=nrm[i][j];
       }
                
}

void TKDBc::SetLayer(integer layer, 
                      integer status, geant coo[], number nrm[3][3], integer gid){
       int i,j;
        integer num=layer-1;
       _HeadLayer[num]._status=status;
       _HeadLayer[num]._gid=gid;
       for(i=0;i<3;i++){
         _HeadLayer[num]._coo[i]=coo[i];
         for(j=0;j<3;j++)_HeadLayer[num]._nrm[i][j]=nrm[i][j];
       }
                
}

void TKDBc::GetSensor(integer layer, integer ladder, integer sensor, 
                      integer & status, geant coo[], number nrm[3][3]){
        integer num = getnum(layer,ladder,sensor);
       #ifdef __AMSDEBUG__
          assert (num >=0 && num <_NumberSen);
       #endif
       int i,j;
       status=_HeadSensor[num]._status;
      for(i=0;i<3;i++){
         coo[i]=_HeadSensor[num]._coo[i];
         for(j=0;j<3;j++)nrm[i][j]=(_HeadSensor[num])._nrm[i][j];
       }
                

}
void TKDBc::GetLadder(integer layer, integer ladder,  integer half,
                      integer & status, geant coo[], number nrm[3][3],
                      integer &gid){
        integer num = getnumd(layer,ladder);
       #ifdef __AMSDEBUG__
          assert (num >=0 && num <_NumberLadder);
       #endif
       int i,j;
       status=_HeadLadder[half][num]._status;
       gid=   _HeadLadder[half][num]._gid;
       for(i=0;i<3;i++){
         coo[i]=_HeadLadder[half][num]._coo[i];
         for(j=0;j<3;j++)nrm[i][j]=(_HeadLadder[half][num])._nrm[i][j];
       }
                

}
void TKDBc::GetLayer(integer layer,  
                      integer & status, geant coo[], number nrm[3][3],
                      integer &gid){
       int i,j;
       integer num=layer-1;
       status=_HeadLayer[num]._status;
       gid=_HeadLayer[num]._gid;
       for(i=0;i<3;i++){
         coo[i]=_HeadLayer[num]._coo[i];
         for(j=0;j<3;j++)nrm[i][j]=(_HeadLayer[num])._nrm[i][j];
       }
                

}



 void TKDBc::GetMarker(integer layer, integer markerno, integer markerpos,
                       integer & status, geant coo[], number nrm[3][3]){

       int i,j;
       integer num=(layer-1)*4+markerpos;
       status=_HeadMarker[markerno][num]._status;
       for(i=0;i<3;i++){
         coo[i]=_HeadMarker[markerno][num]._coo[i];
         for(j=0;j<3;j++)nrm[i][j]=(_HeadMarker[markerno][num])._nrm[i][j];
       }


 }
 void TKDBc::SetMarker(integer layer, integer markerno, integer markerpos,
                       integer & status, geant coo[], number nrm[3][3],integer gid){

       int i,j;
       integer num=(layer-1)*4+markerpos;
       _HeadMarker[markerno][num]._status=status;
       _HeadMarker[markerno][num]._gid=gid;
       for(i=0;i<3;i++){
         _HeadMarker[markerno][num]._coo[i]=coo[i];
         for(j=0;j<3;j++)(_HeadMarker[markerno][num])._nrm[i][j]=nrm[i][j];
       }


 }




integer TKDBc::getnum(integer layer, integer ladder, integer sensor){
  // calculate sensor #
  // layer, ladder , sensor from 0
       int num=0;
       int i,j,k;
       for ( i=0;i<layer;i++){
        for ( j=0;j<AMSDBc::nlad(i+1);j++){
         for ( k=0;k<AMSDBc::nsen(i+1,j+1);k++)num++;
        }
       }

        i=layer;
        for ( j=0;j<ladder;j++){
         for ( k=0;k<AMSDBc::nsen(i+1,j+1);k++)num++;
        }
        j=ladder;       
        for ( k=0;k<sensor;k++)num++;
        return num;
}

integer TKDBc::getnumd(integer layer, integer ladder){
  // calculate ladder #
  // layer, ladder from 0
       int num=0;
       int i,j,k;
       for ( i=0;i<layer;i++){
        for ( j=0;j<AMSDBc::nlad(i+1);j++)num++;
       }

        i=layer;
        for ( j=0;j<ladder;j++)num++;
        return num;
}


integer TKDBcI::_Count=0;

TKDBcI::TKDBcI(){
  if(_Count++ == 0){
    TKDBc::init(); 
  }
}

TKDBcI::~TKDBcI(){
  if(--_Count ==0){
    TKDBc::write();   
  }
}


void TKDBc::fitting(int il,int nls, int meas, integer StaLadders[17][2],
                   AMSPoint CooLadder[17][2][10], int CooStatus[17][2][10], 
           number LadderNrm[17][2][3][3], TRLDGM_def & TRLDGM,
           TRLYGM_def & TRLYGM, int xPosLadder[17][2][10], 
           AMSPoint LadderCoo[17][2]){
      // Now perform fitting 
      for(int i=0;i<nls;i++){
       for(int j=0;j<2;j++){
        if(StaLadders[i][j]){
        number x(0),y(0),z(0),xy(0),xz(0),yz(0),x2(0),y2(0),z2(0);
        int nm=meas;
        int ndm=0;
        for(int k=0;k<nm;k++){
         if(CooStatus[i][j][k]){
         x+=CooLadder[i][j][k][0];
         y+=CooLadder[i][j][k][1];
         z+=CooLadder[i][j][k][2];
         xy+=CooLadder[i][j][k][0]*CooLadder[i][j][k][1];
         xz+=CooLadder[i][j][k][0]*CooLadder[i][j][k][2];
         yz+=CooLadder[i][j][k][1]*CooLadder[i][j][k][2];
         x2+=CooLadder[i][j][k][0]*CooLadder[i][j][k][0];
         y2+=CooLadder[i][j][k][1]*CooLadder[i][j][k][1];
         z2+=CooLadder[i][j][k][2]*CooLadder[i][j][k][2];
         ndm++;
         }
        }
         x/=ndm;
         y/=ndm;
         z/=ndm;
         xz/=ndm;
         yz/=ndm;
         xy/=ndm;
         x2/=ndm;
         y2/=ndm;
         z2/=ndm;
         number dx2=x2-x*x;
         number dy2=y2-y*y;
         number dxy=xy-x*y;
         number dxz=xz-x*z;
         number dyz=yz-y*z;
         number b=(dx2*dyz-dxz*dxy)/(dx2*dy2-dxy*dxy);
         number a=(dxz*dy2-dyz*dxy)/(dx2*dy2-dxy*dxy);
         number c=z-a*x-b*y;
         AMSDir u,v(-a,-b,1),w; 
         number pitch=atan2(-v[0],v[2]);
         number roll=asin(-v[1]);
         LadderNrm[i][j][0][0]=cos(pitch);
         LadderNrm[i][j][1][0]=0;
         LadderNrm[i][j][2][0]=sin(pitch);
         LadderNrm[i][j][0][1]=-sin(roll)*sin(pitch);            
         LadderNrm[i][j][1][1]=cos(roll);            
         LadderNrm[i][j][2][1]=sin(roll)*cos(pitch);
         LadderNrm[i][j][0][2]=-cos(roll)*sin(pitch);
         LadderNrm[i][j][1][2]=-sin(roll);
         LadderNrm[i][j][2][2]=cos(roll)*cos(pitch);
        
         u=AMSDir(LadderNrm[i][j][0][0],LadderNrm[i][j][1][0],LadderNrm[i][j][2][0]);
         v=AMSDir(LadderNrm[i][j][0][1],LadderNrm[i][j][1][1],LadderNrm[i][j][2][1]);
         w=AMSDir(LadderNrm[i][j][0][2],LadderNrm[i][j][1][2],LadderNrm[i][j][2][2]);
         AMSPoint tmp[10];
         for(k=0;k<nm;k++){
          AMSPoint amsx;
          amsx[0]=u.prod(CooLadder[i][j][k]);
          amsx[1]=v.prod(CooLadder[i][j][k]);
          amsx[2]=w.prod(CooLadder[i][j][k]);
          tmp[k]=amsx;
         }
         number x1(0),y1(0),xx(0);
         x2=0;
         xy=0;
         y2=0;
         number xx2(0),xy2(0);
         int nm2=nm/2;
         int ndm1=0;
         int ndm2=0;
         for(k=0;k<nm2;k++){
         if(CooStatus[i][j][k]){
          x1+=tmp[k][0];
          y1+=tmp[k][1];
          xx+=tmp[k][0]*tmp[k][0];
          xy+=tmp[k][0]*tmp[k][1];
          ndm1++;
          }
         }
         for(k=nm2;k<nm;k++){
         if(CooStatus[i][j][k]){
          x2+=tmp[k][0];
          y2+=tmp[k][1];
          xx2+=tmp[k][0]*tmp[k][0];
          xy2+=tmp[k][0]*tmp[k][1];
          ndm2++;
         }
         }
         x1/=ndm1;         
         y1/=ndm1;         
         xx/=ndm1;         
         xy/=ndm1;
         x2/=ndm2;         
         y2/=ndm2;         
         xx2/=ndm2;         
         xy2/=ndm2;         
         a= (xy+xy2-x1*y1-x2*y2)/(xx+xx2-x1*x1-x2*x2);
         AMSDir matrix[3];
         matrix[0]=AMSDir(1,a,0);         
         matrix[1]=AMSDir(-a,1,0);         
         matrix[2]=AMSDir(0,0,1);         
         number nrm[3][3];
         nrm[0][0]=matrix[0][0];
         nrm[1][0]=matrix[0][1];
         nrm[2][0]=matrix[0][2];
         nrm[0][1]=matrix[1][0];
         nrm[1][1]=matrix[1][1];
         nrm[2][1]=matrix[1][2];
         nrm[0][2]=matrix[2][0];
         nrm[1][2]=matrix[2][1];
         nrm[2][2]=matrix[2][2];
         
         mm3(LadderNrm[i][j],nrm,0);
         if(j==1){
          // rotate x & by 180 degrees
           nrm[0][0]=-nrm[0][0];
           nrm[1][0]=-nrm[1][0];
           nrm[2][0]=-nrm[2][0];
           nrm[0][1]=-nrm[0][1];
           nrm[1][1]=-nrm[1][1];
           nrm[2][1]=-nrm[2][1];
         }
         TRLDGM.Nrm[0][0]=nrm[0][0];
         TRLDGM.Nrm[0][1]=nrm[1][0];
         TRLDGM.Nrm[0][2]=nrm[2][0];
         TRLDGM.Nrm[1][0]=nrm[0][1];
         TRLDGM.Nrm[1][1]=nrm[1][1];
         TRLDGM.Nrm[1][2]=nrm[2][1];
         TRLDGM.Nrm[2][0]=nrm[0][2];
         TRLDGM.Nrm[2][1]=nrm[1][2];
         TRLDGM.Nrm[2][2]=nrm[2][2];
         UCOPY(nrm,LadderNrm[i][j],sizeof(nrm)/sizeof(integer));
         u=AMSDir(LadderNrm[i][j][0][0],LadderNrm[i][j][1][0],LadderNrm[i][j][2][0]);
         v=AMSDir(LadderNrm[i][j][0][1],LadderNrm[i][j][1][1],LadderNrm[i][j][2][1]);
         w=AMSDir(LadderNrm[i][j][0][2],LadderNrm[i][j][1][2],LadderNrm[i][j][2][2]);
         for(k=0;k<nm;k++){
          AMSPoint amsx;
          amsx[0]=u.prod(CooLadder[i][j][k]);
          amsx[1]=v.prod(CooLadder[i][j][k]);
          amsx[2]=w.prod(CooLadder[i][j][k]);
          tmp[k]=amsx;
         }
          x=0;
          y=0;
          y1=0;
          y2=0;
          z=0;
          int nmd1=0;
          int nmd2=0;
          for(k=0;k<nm;k++){
          if(CooStatus[i][j][k]){
           if(k<nm/2){
            y1+=tmp[k][1];
            nmd1++;
           }
           else {
             y2+=tmp[k][1];
             nmd2++;
           }
           y+=tmp[k][1];
           z+=tmp[k][2];
           }
          }
          y1/=nmd1;
          y2/=nmd2;
          y/=nmd1+nmd2;
          z/=nmd1+nmd2;
          for(k=0;k<nm/2;k++){
           TRLDGM.Ry[k]=tmp[k][1]-y1;
           TRLDGM.Rz[k]=tmp[k][2]-z;
          }
          for(k=nm/2;k<nm;k++){
           TRLDGM.Ry[k]=tmp[k][1]-y2;
           TRLDGM.Rz[k]=tmp[k][2]-z;
          }
          // find the proper x
          x=0;
          int nmd=0;
          for(k=0;k<10;k++)TRLDGM.Status[k]=0;
          for(k=0;k<nm;k++){
          if(CooStatus[i][j][k]>0){
           TRLDGM.Status[k]=1;
           number dist=0.03;
           number xi;
           int is=xPosLadder[i][j][k]/10;
           int ilr=xPosLadder[i][j][k]%10;
           xi=AMSDBc::ssize_inactive(il,0)*(is-1+ilr)+dist*(ilr==0?1:-1);
           //cout <<il<<" "<<i<<" "<<j<<" "<<k<<" "<<tmp[k][0]<<" "<<xi<<" "<<tmp[k][0]-xi<<endl;
           x+=tmp[k][0]-xi;
           nmd++;
           } 
          }
          x/=nmd;
          for(k=0;k<nm;k++){
           number dist=0.03;
           number xi;
           int is=xPosLadder[i][j][k]/10;
           int ilr=xPosLadder[i][j][k]%10;
           xi=AMSDBc::ssize_inactive(il,0)*(is-1+ilr)+dist*(ilr==0?1:-1);
           TRLDGM.Rx[k]=tmp[k][0]-xi-x;
          }
          x+=0.5*AMSDBc::ssize_inactive(il,0)*(j==0?AMSDBc::nhalf(il+1,i+1):
          AMSDBc::nsen(il+1,i+1)-AMSDBc::nhalf(il+1,i+1));
          AMSPoint xp(x,y,z);
         u=AMSDir(LadderNrm[i][j][0][0],LadderNrm[i][j][0][1],LadderNrm[i][j][0][2]);
         v=AMSDir(LadderNrm[i][j][1][0],LadderNrm[i][j][1][1],LadderNrm[i][j][1][2]);
         w=AMSDir(LadderNrm[i][j][2][0],LadderNrm[i][j][2][1],LadderNrm[i][j][2][2]);

        LadderCoo[i][j][0]=u.prod(xp);          
        LadderCoo[i][j][1]=v.prod(xp);          
        LadderCoo[i][j][2]=w.prod(xp);          
        TRLDGM.Coo[0]=u.prod(xp); 
        TRLDGM.Coo[1]=v.prod(xp); 
        TRLDGM.Coo[2]=w.prod(xp); 
        TRLDGM.Layer=il+1;
        TRLDGM.Ladder=i+1;
        TRLDGM.Half=j;
        int rgid,status;
        GetLadder(TRLDGM.Layer-1,TRLDGM.Ladder-1,TRLDGM.Half,status,
         TRLDGM.CooO,nrm,rgid);        
         TRLDGM.NrmO[0][0]=nrm[0][0];
         TRLDGM.NrmO[0][1]=nrm[1][0];
         TRLDGM.NrmO[0][2]=nrm[2][0];
         TRLDGM.NrmO[1][0]=nrm[0][1];
         TRLDGM.NrmO[1][1]=nrm[1][1];
         TRLDGM.NrmO[1][2]=nrm[2][1];
         TRLDGM.NrmO[2][0]=nrm[0][2];
         TRLDGM.NrmO[2][1]=nrm[1][2];
         TRLDGM.NrmO[2][2]=nrm[2][2];
         SetLadder(TRLDGM.Layer-1,TRLDGM.Ladder-1,TRLDGM.Half,status,
         TRLDGM.Coo,LadderNrm[i][j],rgid);        
      
        HFNT(IOPA.ntuple); 
        }
       }
     }
}      
