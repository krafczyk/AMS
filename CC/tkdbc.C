#include <tkdbc.h>
#include <amsdbc.h>
#include <astring.h>
#include <job.h>
#include <commons.h>
TKDBc * TKDBc::_Head=0;
integer TKDBc::_NumberSen=0;
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
       _Head=new TKDBc[_NumberSen];
}

void TKDBc::read(){
AString fnam(AMSDATADIR.amsdatadir);
fnam+="TKGeom_";
fnam+=AMSJob::gethead()->getsetup();
ifstream iftxt((const char *)fnam,ios::in);
int active=0;
if(iftxt){
  // File exists...
 int i,j,k,idum;
 for(i=0;i<_NumberSen;i++){
   iftxt >> idum >> _Head[idum]._status;
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
if( _ReadOK)return;
AString fnam(AMSDATADIR.amsdatadir);
fnam+="TKGeom_";
fnam+=AMSJob::gethead()->getsetup();
ofstream iftxt((const char *)fnam,ios::out|ios::noreplace);
if(iftxt){
  // Write file
 int i,j,k,idum;
 for(i=0;i<_NumberSen;i++){
   iftxt << i <<" "<< _Head[i]._status<<endl;
   for(j=0;j<3;j++)iftxt << _Head[i]._coo[j]<<" ";
   iftxt <<endl;
   for(j=0;j<3;j++){
     for(k=0;k<3;k++)iftxt << _Head[i]._nrm[j][k]<<" ";
     iftxt <<endl;
   }
 }
 iftxt << "           "<<endl;
}
else cerr << " TKDBc::write-E-File "<<fnam <<" already exists."<<endl;
}

void TKDBc::SetSensor(integer layer, integer ladder, integer sensor,
                      integer status, geant coo[], number nrm[3][3]){
        integer num = getnum(layer,ladder,sensor);
       #ifdef __AMSDEBUG__
          assert (num >=0 && num <_NumberSen);
       #endif
       int i,j;
       _Head[num]._status=status;
       for(i=0;i<3;i++){
         _Head[num]._coo[i]=coo[i];
         for(j=0;j<3;j++)_Head[num]._nrm[i][j]=nrm[i][j];
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
