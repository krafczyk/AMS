#include <tkdbc.h>
#include <amsdbc.h>
#include <astring.h>
#include <job.h>
#include <commons.h>
TKDBc * TKDBc::_Head=0;
TKDBc * TKDBc::_HeadLayer=0;
TKDBc * TKDBc::_HeadLadder[2]={0,0};
integer TKDBc::_NumberSen=0;
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
       _Head=new TKDBc[_NumberSen];
       _HeadLadder[0]=new TKDBc[_NumberLadder];
       _HeadLadder[1]=new TKDBc[_NumberLadder];
       _NumberLayer=AMSDBc::nlay();
       _HeadLayer=new TKDBc[_NumberLayer];
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
 int i,j,k,idum,gid;
 char lay;
 for(i=0;i<_NumberLayer;i++){
   iftxt >> lay>>lay>>lay>>lay>>lay>>idum >> gid>>_HeadLayer[idum]._status;
   for(j=0;j<3;j++)iftxt >> _HeadLayer[idum]._coo[j];
   for(j=0;j<3;j++){
     for(k=0;k<3;k++)iftxt >> _HeadLayer[idum]._nrm[j][k];
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
   iftxt >> idum >> gid>>_Head[idum]._status;
   for(j=0;j<3;j++)iftxt >> _Head[idum]._coo[j];
   for(j=0;j<3;j++){
     for(k=0;k<3;k++)iftxt >> _Head[idum]._nrm[j][k];
   }
   if(_Head[idum]._status)active++;
 }
if(iftxt.eof() ){
  cerr<< "TKDBc::read-F-Unexpected EOF in file "<<fnam<<endl;
}
else {
 _ReadOK=1;
 cout <<"TKDBc::read-I-"<<active<<" active sensors have been read"<<endl;
}
}
else {
  cerr <<" TKDBc::read-E-File not exists "<<fnam <<endl;
}
}


void TKDBc::write(){
if( _ReadOK || !TKGEOMFFKEY.WriteGeomToFile)return;
AString fnam(AMSDATADIR.amsdatadir);
fnam+="TKGeom_";
fnam+=AMSJob::gethead()->getsetup();
ofstream iftxt((const char *)fnam,ios::out|ios::noreplace);
if(iftxt){
  // Write file
 int i,j,k,idum;
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
   iftxt << i <<" "<< _Head[i]._gid<<" "<<_Head[i]._status<<endl;
   for(j=0;j<3;j++)iftxt << _Head[i]._coo[j]<<" ";
   iftxt <<endl;
   for(j=0;j<3;j++){
     for(k=0;k<3;k++)iftxt << _Head[i]._nrm[j][k]<<" ";
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
       _Head[num]._status=status;
       _Head[num]._gid=gid;
       for(i=0;i<3;i++){
         _Head[num]._coo[i]=coo[i];
         for(j=0;j<3;j++)_Head[num]._nrm[i][j]=nrm[i][j];
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
       status=_Head[num]._status;
       for(i=0;i<3;i++){
         coo[i]=_Head[num]._coo[i];
         for(j=0;j<3;j++)nrm[i][j]=(_Head[num])._nrm[i][j];
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
