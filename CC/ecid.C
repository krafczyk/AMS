#include <ecid.h>
using namespace ecalconst;
 integer AMSECIdSoft::_GetGeo[ecalconst::ECRT][ecalconst::ECPMSL][2];   // slayer,pmtno
 integer AMSECIdSoft::_GetHard[ecalconst::ECSLMX][ecalconst::ECPMSMX][2];     // crate, hchan

AMSECIdSoft::AMSECIdSoft(int16 crate, int16 haddr, int16 channelh):_channelh(channelh),_crate(crate),_dead(0),_haddr(haddr){

_sl=_GetGeo[crate%ECRT][haddr%ECPMSMX][0];
_pmtno=_GetGeo[crate%ECRT][haddr%ECPMSMX][1];
if(_sl<0)_dead=1;
_channelh2s();
}

AMSECIdSoft::AMSECIdSoft(integer idsoft):_dead(0){
_sl=idsoft/1000-1;
_pmtno=(idsoft/10)%100-1;
_channel=idsoft%10-1;
_crate=_GetHard[_sl][_pmtno][0];
_haddr=_GetHard[_sl][_pmtno][1];
_channels2h();
if(_crate<0)_dead=1;
}

void AMSECIdSoft::_channelh2s(){
 if(_channelh==4){
   _channel=0;
 }
 else{
  if(_pmtno%2==0){
    switch (_channelh%4){
     case 0:
       _channel=2;
       break;
     case 1:
       _channel=3;
       break;
     case 2:
       _channel=1;
       break;
     case 3:
       _channel=0;
       break;
  }
 }
 else{
    switch (_channelh%4){
     case 0:
       _channel=3;
       break;
     case 1:
       _channel=2;
       break;
     case 2:
       _channel=0;
       break;
     case 3:
       _channel=1;
       break;
  }
 }
}
}

void AMSECIdSoft::_channels2h(){
 _channelh=-1;  //not yet implemented
}

void AMSECIdSoft::inittable(){
     for(int i=0;i<ECRT;i++){
       for( int j=0;j<ECPMSL;j++){
         for(int l=0;l<2;l++)_GetGeo[i][j][l]=-1;
        }
       }
     for(int i=0;i<ECSLMX;i++){
       for(int  j=0;j<ECPMSMX;j++){
          for(int l=0;l<2;l++)_GetHard[i][j][l]=-1;
         }
     }

    _GetHard[0][0][0]=0;
    _GetHard[0][0][1]=0;

    _GetHard[0][1][0]=1;
    _GetHard[0][1][1]=1;

    _GetHard[0][2][0]=0;
    _GetHard[0][2][1]=18;

    _GetHard[0][3][0]=1;
    _GetHard[0][3][1]=19;

    _GetHard[0][4][0]=0;
    _GetHard[0][4][1]=11;

    _GetHard[0][5][0]=1;
    _GetHard[0][5][1]=0;

    _GetHard[0][6][0]=0;
    _GetHard[0][6][1]=17;


    _GetHard[1][0][0]=1;
    _GetHard[1][0][1]=3;

    _GetHard[1][1][0]=0;
    _GetHard[1][1][1]=2;

    _GetHard[1][2][0]=1;
    _GetHard[1][2][1]=21;

    _GetHard[1][3][0]=0;
    _GetHard[1][3][1]=20;

    _GetHard[1][4][0]=1;
    _GetHard[1][4][1]=2;

    _GetHard[1][5][0]=0;
    _GetHard[1][5][1]=9;

    _GetHard[1][6][0]=0;
    _GetHard[1][6][1]=15;



    _GetHard[2][0][0]=0;
    _GetHard[2][0][1]=4;

    _GetHard[2][1][0]=1;
    _GetHard[2][1][1]=5;

    _GetHard[2][2][0]=0;
    _GetHard[2][2][1]=22;

    _GetHard[2][3][0]=1;
    _GetHard[2][3][1]=23;

    _GetHard[2][4][0]=0;
    _GetHard[2][4][1]=7;

    _GetHard[2][5][0]=1;
    _GetHard[2][5][1]=4;

    _GetHard[2][6][0]=0;
    _GetHard[2][6][1]=13;

    _GetHard[3][0][0]=1;
    _GetHard[3][0][1]=7;

    _GetHard[3][1][0]=0;
    _GetHard[3][1][1]=6;

    _GetHard[3][2][0]=1;
    _GetHard[3][2][1]=25;

    _GetHard[3][3][0]=0;
    _GetHard[3][3][1]=24;

    _GetHard[3][4][0]=1;
    _GetHard[3][4][1]=6;

    _GetHard[3][5][0]=0;
    _GetHard[3][5][1]=5;

    _GetHard[3][6][0]=0;
    _GetHard[3][6][1]=35;


    _GetHard[4][0][0]=0;
    _GetHard[4][0][1]=8;

    _GetHard[4][1][0]=1;
    _GetHard[4][1][1]=9;

    _GetHard[4][2][0]=0;
    _GetHard[4][2][1]=26;

    _GetHard[4][3][0]=1;
    _GetHard[4][3][1]=27;

    _GetHard[4][4][0]=0;
    _GetHard[4][4][1]=3;

    _GetHard[4][5][0]=1;
    _GetHard[4][5][1]=8;

    _GetHard[4][6][0]=0;
    _GetHard[4][6][1]=33;


    _GetHard[5][0][0]=1;
    _GetHard[5][0][1]=11;

    _GetHard[5][1][0]=0;
    _GetHard[5][1][1]=10;

    _GetHard[5][2][0]=1;
    _GetHard[5][2][1]=29;

    _GetHard[5][3][0]=0;
    _GetHard[5][3][1]=28;

    _GetHard[5][4][0]=1;
    _GetHard[5][4][1]=10;

    _GetHard[5][5][0]=0;
    _GetHard[5][5][1]=1;

    _GetHard[5][6][0]=0;
    _GetHard[5][6][1]=31;


    _GetHard[6][0][0]=0;
    _GetHard[6][0][1]=12;

    _GetHard[6][1][0]=1;
    _GetHard[6][1][1]=13;

    _GetHard[6][2][0]=0;
    _GetHard[6][2][1]=30;

    _GetHard[6][3][0]=1;
    _GetHard[6][3][1]=31;

    _GetHard[6][4][0]=0;
    _GetHard[6][4][1]=23;

    _GetHard[6][5][0]=1;
    _GetHard[6][5][1]=12;

    _GetHard[6][6][0]=0;
    _GetHard[6][6][1]=29;

    _GetHard[7][0][0]=1;
    _GetHard[7][0][1]=15;

    _GetHard[7][1][0]=0;
    _GetHard[7][1][1]=14;

    _GetHard[7][2][0]=1;
    _GetHard[7][2][1]=33;

    _GetHard[7][3][0]=0;
    _GetHard[7][3][1]=32;

    _GetHard[7][4][0]=1;
    _GetHard[7][4][1]=14;

    _GetHard[7][5][0]=0;
    _GetHard[7][5][1]=21;

    _GetHard[7][6][0]=0;
    _GetHard[7][6][1]=27;

    _GetHard[8][0][0]=0;
    _GetHard[8][0][1]=16;

    _GetHard[8][1][0]=1;
    _GetHard[8][1][1]=17;

    _GetHard[8][2][0]=0;
    _GetHard[8][2][1]=34;

    _GetHard[8][3][0]=1;
    _GetHard[8][3][1]=35;

    _GetHard[8][4][0]=0;
    _GetHard[8][4][1]=19;

    _GetHard[8][5][0]=1;
    _GetHard[8][5][1]=16;

    _GetHard[8][6][0]=0;
    _GetHard[8][6][1]=25;


     for(int i=0;i<ECSLMX;i++){
     for(int j=7;j<ECPMSMX;j++){
      _GetHard[i][j][0]=j%2;
      _GetHard[i][j][1]=36+i*ECPMSMX+(j-7)/2;
//       cout <<i<<" "<<j<<" "<<_GetHard[i][j][1]<<endl;
     }    
     }

  // perform duplicate check
     int tot=0;
     for(int i=0;i<ECSLMX;i++){
      for(int j=0;j<ECPMSMX;j++){
          int cr=_GetHard[i][j][0];
          int haddr=_GetHard[i][j][1];
          if(cr>=0){
            tot++;
            for(int i1=0;i1<ECSLMX;i1++){
             for(int j1=0;j1<ECPMSMX;j1++){
               if(j1==j && i1==i || (cr!=_GetHard[i1][j1][0]))continue;
               if(haddr == _GetHard[i1][j1][1]){
                 cerr <<"AMSECIdSoft::inittable-F-duplicate entry found "<<
                   haddr<<" i "<<i<<" j "<<j<<" i1 "<<i1<<
                   " j1 "<<j1<<endl;
                 abort();
               }
             }
            }           
           }
          }
          
        }
     
     cout <<" total "<<tot<<endl; 


   for(int i=0;i<ECRT;i++){
    for(int j=0;j<ECPMSL;j++){
      for(int i1=0;i1<ECSLMX;i1++){
        for(int j1=0;j1<ECPMSMX;j1++){
            if(_GetHard[i1][j1][0]==i && _GetHard[i1][j1][1]==j){
             _GetGeo[i][j][0]=i1;
             _GetGeo[i][j][1]=j1;
            }
          }
                                      
        }
      }      
  }


}
