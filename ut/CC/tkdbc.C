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
ifstream iftxt((const char *)fnam,ios::in);
int active=0;
if(iftxt){
  // File exists...
 int i,j,k,l,idum,gid;
 char lay;
 for(i=0;i<_NumberLayer;i++){
   iftxt >> lay>>lay>>lay>>lay>>lay>>idum >> gid>>_HeadLayer[idum]._status;
   for(j=0;j<3;j++)iftxt >> _HeadLayer[idum]._coo[j];
   for(j=0;j<3;j++){
     for(k=0;k<3;k++)iftxt >> _HeadLayer[idum]._nrm[j][k];
   }
 }
 for(l=0;l<2;l++){
 for(i=0;i<_NumberMarkers;i++){
   iftxt >> lay>>lay>>lay>>lay>>lay>>lay>>lay>>idum >> gid>>_HeadMarker[l][idum]._status;
   for(j=0;j<3;j++)iftxt >> _HeadMarker[l][idum]._coo[j];
   for(j=0;j<3;j++){
     for(k=0;k<3;k++)iftxt >> _HeadMarker[l][idum]._nrm[j][k];
   }
 }
 }
 for(i=0;i<_NumberLadder;i++){
   iftxt >> lay>>lay>>lay>>lay>>lay>>lay>>lay>>idum >> gid>>_HeadLadder[0][idum]._status;
   for(j=0;j<3;j++)iftxt >> _HeadLadder[0][idum]._coo[j];
   for(j=0;j<3;j++){
     for(k=0;k<3;k++)iftxt >> _HeadLadder[0][idum]._nrm[j][k];
   }
 }
 for(i=0;i<_NumberLadder;i++){
   iftxt >> lay>>lay>>lay>>lay>>lay>>lay>>lay>>idum >> gid>>_HeadLadder[1][idum]._status;
   for(j=0;j<3;j++)iftxt >> _HeadLadder[1][idum]._coo[j];
   for(j=0;j<3;j++){
     for(k=0;k<3;k++)iftxt >> _HeadLadder[1][idum]._nrm[j][k];
   }
 }



 for(i=0;i<_NumberSen;i++){
   iftxt >> idum >> gid>>_HeadSensor[idum]._status;
   for(j=0;j<3;j++)iftxt >> _HeadSensor[idum]._coo[j];
   for(j=0;j<3;j++){
     for(k=0;k<3;k++)iftxt >> _HeadSensor[idum]._nrm[j][k];
   }
   if(_HeadSensor[idum]._status)active++;
 }
if(iftxt.eof() ){
  cerr<< "TKDBc::read-F-Unexpected EOF in file "<<fnam<<endl;
}
else {
 _ReadOK=1;
 cout <<"TKDBc::read-I-"<<active<<" active sensors have been read"<<endl;
 updatef();
}
}
else {
  cerr <<" TKDBc::read-E-File not exists "<<fnam <<endl;
}


}


void TKDBc::updatef(){
char dum;
number rdum;
if(!TKGEOMFFKEY.UpdateGeomFile)return;

// First planes
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
  
//   (_HeadLayer[0])._coo[2]= 50.949;
//   (_HeadLayer[1])._coo[2]=  29.2141;
//   (_HeadLayer[2])._coo[2]= 7.78935;
//   (_HeadLayer[3])._coo[2]= -7.78935;
//   (_HeadLayer[4])._coo[2]= -29.14585;
//   (_HeadLayer[5])._coo[2]=  -50.99;

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
     integer numl=getnumd(ilay-1,ilad-1);
     (_HeadLadder[ihalf][numl])._status=istat;
     for(int i=0;i<3;i++)iftxt >> (_HeadLadder[ihalf][numl])._coo[i];
     number xy,zx,zy;
     iftxt >> xy >> zx >> zy;
     number cx=sqrt(1.-(xy*xy+zx*zx));
     number cy=sqrt(1.-(xy*xy+zy*zy));
     number cz=sqrt(1.-(zx*zx+zy*zy));
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
      (_HeadLadder[ihalf][numl])._nrm[0][2]=-zx;
      (_HeadLadder[ihalf][numl])._nrm[1][2]=-zy;
      (_HeadLadder[ihalf][numl])._nrm[2][2]=cz;
     } 
    nladders++;
     if(iftxt.eof())break;
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
   iftxt <<"Ladder N "<< i <<" "<< _HeadLadder[0][i]._gid<<" "<<_HeadLadder[0][i]._status<<endl;
   for(j=0;j<3;j++)iftxt << _HeadLadder[0][i]._coo[j]<<" ";
   iftxt <<endl;
   for(j=0;j<3;j++){
     for(k=0;k<3;k++)iftxt << _HeadLadder[0][i]._nrm[j][k]<<" ";
     iftxt <<endl;
   }
 }
 iftxt <<"      "<<endl;
 for(i=0;i<_NumberLadder;i++){
   iftxt <<"Ladder P "<< i <<" "<< _HeadLadder[1][i]._gid<<" "<<_HeadLadder[1][i]._status<<endl;
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
                      integer & status, geant coo[], number nrm[3][3]){
        integer num = getnumd(layer,ladder);
       #ifdef __AMSDEBUG__
          assert (num >=0 && num <_NumberLadder);
       #endif
       int i,j;
       status=_HeadLadder[half][num]._status;
       for(i=0;i<3;i++){
         coo[i]=_HeadLadder[half][num]._coo[i];
         for(j=0;j<3;j++)nrm[i][j]=(_HeadLadder[half][num])._nrm[i][j];
       }
                

}
void TKDBc::GetLayer(integer layer,  
                      integer & status, geant coo[], number nrm[3][3]){
       int i,j;
       integer num=layer-1;
       status=_HeadLayer[num]._status;
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
