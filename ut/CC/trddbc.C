#include <trddbc.h>
#include <amsdbc.h>
#include <math.h>
char * TRDDBc::_OctagonMedia[maxo]={"TRDCarbonFiber", "TRDCarbonFiber",
"TRDCarbonFiber","TRDHC","TRDFoam","VACUUM","TRDHC"};
char * TRDDBc::_TubesMedia="TRDCapton";
char * TRDDBc::_ITubesMedia="TRDGas";
char * TRDDBc::_RadiatorMedia="TRDRadiator";
char * TRDDBc::_TubesBoxMedia="TRDFoam";

uinteger TRDDBc::_NoTRDOctagons[mtrdo]={5};
uinteger TRDDBc::_PrimaryOctagon[maxo]={0,1,2,0,1,1,2};
uinteger TRDDBc::_PrimaryOctagonNo=3;
uinteger TRDDBc::_TRDOctagonNo=1;
uinteger TRDDBc::_OctagonNo=maxo;
uinteger TRDDBc::_LayersNo[mtrdo]={maxlay};
uinteger TRDDBc::_LaddersNo[mtrdo][maxlay]={20,20,20,19,19,19,19,18,18,18,17,17,17,16,16,16,16,15,15,15};

uinteger TRDDBc::_TubesNo[mtrdo][maxlay][maxlad];

uinteger   TRDDBc::_NumberTubes=0;
uinteger   TRDDBc::_NumberLadders=0;


const number  TRDDBc::_TubeWallThickness=75e-4;
const number  TRDDBc::_TubeInnerDiameter=0.6;
const number  TRDDBc::_TubeBoxThickness=(0.62-TRDDBc::_TubeInnerDiameter-TRDDBc::_TubeWallThickness)/2.;
const number  TRDDBc::_LadderThickness=2.9;
const integer TRDDBc::_LadderOrientation[mtrdo][maxlay]={0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0};   // 0 -x 1 -y    
number TRDDBc::_OctagonDimensions[maxo][10]; 
number TRDDBc::_LaddersDimensions[mtrdo][maxlay][maxlad][3];
number TRDDBc::_TubesDimensions[mtrdo][maxlay][maxlad][3];    
number TRDDBc::_TubesBoxDimensions[mtrdo][maxlay][maxlad][10];    
number TRDDBc::_RadiatorDimensions[mtrdo][maxlay][maxlad][3];

  // Positions & Orientations
  
TRDDBc* TRDDBc::_HeadOctagonPos=0;
TRDDBc* TRDDBc::_HeadLadderPos=0;
TRDDBc* TRDDBc::_HeadRadiatorPos=0;
TRDDBc* TRDDBc::_HeadTubeBoxPos=0;
TRDDBc* TRDDBc::_HeadTubePos=0;

void TRDDBc::init(){

   // Quantities
    int i,j,k,l;
    for(i=0;i<TRDOctagonNo();i++){
     for(j=0;j<LayersNo(i);j++){
      for(k=0;k<LaddersNo(i,j);k++){
       _TubesNo[i][j][k]=16;
      }
     }
    }


  // calculate tubesno #
       for ( i=0;i<TRDOctagonNo();i++){
        for ( j=0;j<LayersNo(i);j++){
         for ( k=0;k<LaddersNo(i,j);k++){
          for(l=0;l<TubesNo(i,j,k);l++){
//           cout <<_NumberTubes<<" "<<i<<" "<<j<<" "<<k<<" "<<l<<endl;
           _NumberTubes++;
          }
         }
        }
       }

       cout <<"TRDDBcI-I-Total of " <<_NumberTubes<< "  tubes initialized."<<endl;
  // calculate ladder #
       for ( i=0;i<TRDOctagonNo();i++){
        for ( j=0;j<LayersNo(i);j++){
         for ( k=0;k<LaddersNo(i,j);k++)_NumberLadders++;
        }
       }
       cout <<"TRDDBcI-I-Total of " <<_NumberLadders<< "  ladders initialized."<<endl;



       _HeadLadderPos=new TRDDBc[_NumberLadders];
       _HeadRadiatorPos=new TRDDBc[_NumberLadders];
       _HeadTubeBoxPos=new TRDDBc[_NumberLadders];
       _HeadTubePos=new TRDDBc[_NumberTubes];
       _HeadOctagonPos= new TRDDBc[OctagonNo()]; 



   // HereStartsTheJob;

      number unitnrm[3][3]={1,0,0,0,1,0,0,0,1};

//    OctagonDimensions
   // Primary  
      OctagonDimensions(0,4)=-31./20;
      OctagonDimensions(0,5)=0;
      OctagonDimensions(0,6)=2450/20.;
      OctagonDimensions(0,8)=0;
      OctagonDimensions(0,9)=2450/20.;

      number ang=27.5/180.*AMSDBc::pi;
      OctagonDimensions(1,4)=-611./20-OctagonDimensions(0,4);
      OctagonDimensions(1,5)=0;
      OctagonDimensions(1,6)=1550/20.+30/10./cos(ang);
      OctagonDimensions(1,8)=0;
      OctagonDimensions(1,9)=OctagonDimensions(1,6)+2*fabs(OctagonDimensions(1,4))*tan(ang);

      OctagonDimensions(2,4)=-175./20;
      OctagonDimensions(2,5)=0;
      OctagonDimensions(2,6)=1698./20;
      OctagonDimensions(2,8)=0;
      OctagonDimensions(2,9)=OctagonDimensions(2,6)+2*fabs(OctagonDimensions(2,4))*tan(ang);

  //   Inner
      OctagonDimensions(3,4)=OctagonDimensions(0,4)+2/20.;
      OctagonDimensions(3,5)=0;
      OctagonDimensions(3,6)=OctagonDimensions(0,6)-4/20.;
      OctagonDimensions(3,8)=0;
      OctagonDimensions(3,9)=OctagonDimensions(0,9)-4/20.;

      
      OctagonDimensions(4,4)=OctagonDimensions(1,4);
      OctagonDimensions(4,5)=OctagonDimensions(1,6)-30/10./cos(ang);
      OctagonDimensions(4,6)=OctagonDimensions(1,6)-2/10./cos(ang);
      OctagonDimensions(4,8)=OctagonDimensions(1,9)-30/10./cos(ang);
      OctagonDimensions(4,9)=OctagonDimensions(1,9)-2/10./cos(ang);

      OctagonDimensions(5,4)=OctagonDimensions(1,4);
      OctagonDimensions(5,5)=0;
      OctagonDimensions(5,6)=OctagonDimensions(4,5)-2/10./cos(ang);
      OctagonDimensions(5,8)=0;
      OctagonDimensions(5,9)=OctagonDimensions(4,8)-2/10./cos(ang);

      OctagonDimensions(6,4)=OctagonDimensions(2,4)+4/20.;
      OctagonDimensions(6,5)=0;
      OctagonDimensions(6,6)=OctagonDimensions(2,6)-2/10./cos(ang)+2/10.*tan(ang);
      OctagonDimensions(6,8)=0;
      OctagonDimensions(6,9)=OctagonDimensions(2,9)-2/10./cos(ang)-2/10.*tan(ang);

      
      for(i=0;i<OctagonNo();i++){
       OctagonDimensions(i,0)=0-360/16.;
       OctagonDimensions(i,1)=360;
       OctagonDimensions(i,2)=8;
       OctagonDimensions(i,3)=2;
       OctagonDimensions(i,7)=-OctagonDimensions(i,4);
      }    

//    Octagon Pos

      uinteger status=1;
      uinteger gid=0;
      geant coo[3];
      for(i=0;i<PrimaryOctagonNo();i++){
       for(j=0;j<3;j++){
        coo[j]=0;
       }
       switch(i){
        case 2:
        coo[2]=839./10+OctagonDimensions(i,4);
        break;
        case 1:
        coo[2]=839./10-OctagonDimensions(i,4);
        break;
        case 0:
        coo[2]=1450./10+OctagonDimensions(i,4);
        break;
       }
       SetOctagon(i,status,coo,unitnrm,gid); 
      }

      for(i=PrimaryOctagonNo();i<OctagonNo();i++){
       for(j=0;j<3;j++){
        coo[j]=0;
       }
       SetOctagon(i,status,coo,unitnrm,gid); 
      }

     // LadderSizes
      for(i=0;i<TRDOctagonNo();i++){
       for(j=0;j<LayersNo(i);j++){
        for(k=0;k<LaddersNo(i,j);k++){
         coo[0]=TubeInnerDiameter()+2*TubeWallThickness()+2*TubeBoxThickness();
         coo[0]*=TubesNo(i,j,k);
         coo[1]=LadderThickness();
//       get octagon parameters at lowest point
         number rmin=OctagonDimensions(NoTRDOctagons(i),6);
         number rmax=OctagonDimensions(NoTRDOctagons(i),9);
         number zoct=OctagonDimensions(NoTRDOctagons(i),7)-OctagonDimensions(NoTRDOctagons(i),4);
         number tang=(rmax-rmin)/zoct;
         number r=rmax-(j+1)*coo[1]*tang;
         number a=2*r/(1.+sqrt(2.));
         integer leftright;
         if(k<(LaddersNo(i,j)+1)/2)leftright=-1;
         else leftright=1;
         number point=coo[0]/2.*(-1.*LaddersNo(i,j)+1.+2*k+leftright);
         number z=fabs(point)-a/2;
         coo[2]=z<0?r:r-z;
         coo[2]*=2;
         for(int l=0;l<3;l++)LaddersDimensions(i,j,k,l)=coo[l]/2;          
        }  
       }
      }
     number nrmxy[2][3][3]={0,0,1,1,0,0,0,1,0,
                            1,0,0,0,0,-1,0,1,0};
     number nrm[3][3];
    //Ladder Position & Orientation
      for(i=0;i<TRDOctagonNo();i++){
       for(j=0;j<LayersNo(i);j++){
        for(k=0;k<LaddersNo(i,j);k++){
         coo[LadderOrientation(i,j)]=0;
         coo[1-LadderOrientation(i,j)]=-LaddersDimensions(i,j,k,0)*(LaddersNo(i,j)-1.-2*k);

         coo[2]=LaddersDimensions(i,j,k,1)*(LayersNo(i)-1.-2*j+LadderShift(i,j,k)/2.);
         coo[2]=LaddersDimensions(i,j,k,1)*(LayersNo(i)-1.-2*j);
         SetLadder(k,j,i,status,coo,nrmxy[LadderOrientation(i,j)],gid);
        }
       }
      }
      
      // now radiators etc   
      for(i=0;i<TRDOctagonNo();i++){
       for(j=0;j<LayersNo(i);j++){
        for(k=0;k<LaddersNo(i,j);k++){
          RadiatorDimensions(i,j,k,0)=LaddersDimensions(i,j,k,0);
          RadiatorDimensions(i,j,k,1)=LaddersDimensions(i,j,k,1)-(TubeInnerDiameter()+2*TubeWallThickness()+2*TubeBoxThickness())/2;
          RadiatorDimensions(i,j,k,2)=LaddersDimensions(i,j,k,2);
          TubesDimensions(i,j,k,0)=TubeInnerDiameter()/2;          
          TubesDimensions(i,j,k,1)=(TubeInnerDiameter()+2*TubeWallThickness())/2;
          TubesDimensions(i,j,k,2)=LaddersDimensions(i,j,k,2);
        }
       }
      }
      for(i=0;i<TRDOctagonNo();i++){
       for(j=0;j<LayersNo(i);j++){
        for(k=0;k<LaddersNo(i,j);k++){
         coo[0]=coo[2]=0;
         coo[1]=LaddersDimensions(i,j,k,1)-RadiatorDimensions(i,j,k,1);
         SetRadiator(k,j,i,status,coo,unitnrm,gid);
         for(int l=0;l<TubesNo(i,j,k);l++){
           coo[0]=-LaddersDimensions(i,j,k,0)+(TubesDimensions(i,j,k,1)+TubeBoxThickness())*(1+2*l);
           coo[1]=-LaddersDimensions(i,j,k,1)+TubesDimensions(i,j,k,1)+TubeBoxThickness();
           coo[2]=0;
           SetTube(l,k,j,i,status,coo,unitnrm,gid);
         }
        }
       }
      }

}

void TRDDBc::read(){

init();

}

void TRDDBc::write(){
}

uinteger TRDDBc::getnumOctagon(uinteger oct){
#ifdef __AMSDEBUG__
assert(oct<OctagonNo());
#endif
return oct;
}

uinteger TRDDBc::getnumTube(uinteger tube,uinteger ladder, uinteger layer, uinteger oct){
       int num=0;
       int i,j,k,l;
       for ( i=0;i<oct;i++){
        for ( j=0;j<LayersNo(i);j++){
         for ( k=0;k<LaddersNo(i,j);k++){
          for(l=0;l<TubesNo(i,j,k);l++)num++;
         }
        }
       }

       for ( i=oct;i<oct+1;i++){
        for ( j=0;j<layer;j++){
         for ( k=0;k<LaddersNo(i,j);k++){
          for(l=0;l<TubesNo(i,j,k);l++)num++;
         }
        }
       }

       for ( i=oct;i<oct+1;i++){
        for ( j=layer;j<layer+1;j++){
         for ( k=0;k<ladder;k++){
          for(l=0;l<TubesNo(i,j,k);l++)num++;
         }
        }
       }

       for ( i=oct;i<oct+1;i++){
        for ( j=layer;j<layer+1;j++){
         for ( k=ladder;k<ladder+1;k++){
          for(l=0;l<tube;l++)num++;
         }
        }
       }
#ifdef __AMSDEBUG__
   assert(num<_NumberTubes);
#endif
     return num;
}

uinteger TRDDBc::getnumLadder(uinteger ladder, uinteger layer, uinteger oct){
       int num=0;
       int i,j,k,l;
       for ( i=0;i<oct;i++){
        for ( j=0;j<LayersNo(i);j++){
         for ( k=0;k<LaddersNo(i,j);k++)num++;
        }
       }

       for ( i=oct;i<oct+1;i++){
        for ( j=0;j<layer;j++){
         for ( k=0;k<LaddersNo(i,j);k++)num++;
        }
       }

       for ( i=oct;i<oct+1;i++){
        for ( j=layer;j<layer+1;j++){
         for ( k=0;k<ladder;k++)num++;
        }
       }

#ifdef __AMSDEBUG__
   assert(num<_NumberLadders);
#endif
     return num;
}

void TRDDBc::SetOctagon(uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3], uinteger &gid){
#ifdef __AMSDEBUG__
assert(oct<OctagonNo());
#endif
   _HeadOctagonPos[oct]._status=status;     
   _HeadOctagonPos[oct]._gid=gid;     
   int i,j;
   for(i=0;i<3;i++){
    _HeadOctagonPos[oct]._coo[i]=coo[i];
    for(j=0;j<3;j++){
     _HeadOctagonPos[oct]._nrm[i][j]=nrm[i][j];
    }
   }
}


void TRDDBc::GetOctagon(uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid){
#ifdef __AMSDEBUG__
assert(oct<OctagonNo());
#endif
   oct=getnumOctagon(oct);
   status=_HeadOctagonPos[oct]._status;     
   gid=_HeadOctagonPos[oct]._gid;     
   int i,j;
   for(i=0;i<3;i++){
    coo[i]=_HeadOctagonPos[oct]._coo[i];
    for(j=0;j<3;j++){
     nrm[i][j]=_HeadOctagonPos[oct]._nrm[i][j];
    }
   }

}

void TRDDBc::SetLadder(uinteger ladder, uinteger layer,uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3], uinteger gid){
#ifdef __AMSDEBUG__
   _check(oct,layer,ladder);
#endif
    oct=getnumLadder(ladder,layer,oct);    
   _HeadLadderPos[oct]._status=status;     
   _HeadLadderPos[oct]._gid=gid;     
   int i,j;
   for(i=0;i<3;i++){
    _HeadLadderPos[oct]._coo[i]=coo[i];
    for(j=0;j<3;j++){
     _HeadLadderPos[oct]._nrm[i][j]=nrm[i][j];
    }
   }
}


void TRDDBc::GetLadder(uinteger ladder, uinteger layer, uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid){
#ifdef __AMSDEBUG__
   _check(oct,layer,ladder);
#endif
   oct=getnumLadder(ladder,layer,oct);    
   status=_HeadLadderPos[oct]._status;     
   gid=_HeadLadderPos[oct]._gid;     
   int i,j;
   for(i=0;i<3;i++){
    coo[i]=_HeadLadderPos[oct]._coo[i];
    for(j=0;j<3;j++){
     nrm[i][j]=_HeadLadderPos[oct]._nrm[i][j];
    }
   }

}

void TRDDBc::SetRadiator(uinteger ladder, uinteger layer,uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3], uinteger gid){
#ifdef __AMSDEBUG__
   _check(oct,layer,ladder);
#endif
    oct=getnumLadder(ladder,layer,oct);    
   _HeadRadiatorPos[oct]._status=status;     
   _HeadRadiatorPos[oct]._gid=gid;     
   int i,j;
   for(i=0;i<3;i++){
    _HeadRadiatorPos[oct]._coo[i]=coo[i];
    for(j=0;j<3;j++){
     _HeadRadiatorPos[oct]._nrm[i][j]=nrm[i][j];
    }
   }
}


void TRDDBc::GetRadiator(uinteger ladder, uinteger layer, uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid){
#ifdef __AMSDEBUG__
   _check(oct,layer,ladder);
#endif
   oct=getnumLadder(ladder,layer,oct);    
   status=_HeadRadiatorPos[oct]._status;     
   gid=_HeadRadiatorPos[oct]._gid;     
   int i,j;
   for(i=0;i<3;i++){
    coo[i]=_HeadRadiatorPos[oct]._coo[i];
    for(j=0;j<3;j++){
     nrm[i][j]=_HeadRadiatorPos[oct]._nrm[i][j];
    }
   }

}

void TRDDBc::SetTubeBox(uinteger ladder, uinteger layer,uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3], uinteger gid){
#ifdef __AMSDEBUG__
   _check(oct,layer,ladder);
#endif
    oct=getnumLadder(ladder,layer,oct);    
   _HeadTubeBoxPos[oct]._status=status;     
   _HeadTubeBoxPos[oct]._gid=gid;     
   int i,j;
   for(i=0;i<3;i++){
    _HeadTubeBoxPos[oct]._coo[i]=coo[i];
    for(j=0;j<3;j++){
     _HeadTubeBoxPos[oct]._nrm[i][j]=nrm[i][j];
    }
   }
}


void TRDDBc::GetTubeBox(uinteger ladder, uinteger layer, uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid){
#ifdef __AMSDEBUG__
   _check(oct,layer,ladder);
#endif
   oct=getnumLadder(ladder,layer,oct);    
   status=_HeadTubeBoxPos[oct]._status;     
   gid=_HeadTubeBoxPos[oct]._gid;     
   int i,j;
   for(i=0;i<3;i++){
    coo[i]=_HeadTubeBoxPos[oct]._coo[i];
    for(j=0;j<3;j++){
     nrm[i][j]=_HeadTubeBoxPos[oct]._nrm[i][j];
    }
   }
}

void TRDDBc::SetTube(uinteger tube,uinteger ladder, uinteger layer,uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3], uinteger gid){
#ifdef __AMSDEBUG__
   _check(oct,layer,ladder,tube);
#endif
    oct=getnumTube(tube,ladder,layer,oct);    
   _HeadTubePos[oct]._status=status;     
   _HeadTubePos[oct]._gid=gid;     
   int i,j;
   for(i=0;i<3;i++){
    _HeadTubePos[oct]._coo[i]=coo[i];
    for(j=0;j<3;j++){
     _HeadTubePos[oct]._nrm[i][j]=nrm[i][j];
    }
   }
}


void TRDDBc::GetTube(uinteger tube,uinteger ladder, uinteger layer, uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid){
#ifdef __AMSDEBUG__
   _check(oct,layer,ladder,tube);
#endif
   oct=getnumTube(tube,ladder,layer,oct);    
   status=_HeadTubePos[oct]._status;     
   gid=_HeadTubePos[oct]._gid;     
   int i,j;
   for(i=0;i<3;i++){
    coo[i]=_HeadTubePos[oct]._coo[i];
    for(j=0;j<3;j++){
     nrm[i][j]=_HeadTubePos[oct]._nrm[i][j];
    }
   }

}

uinteger TRDDBc::_check(uinteger oct,uinteger layer,uinteger ladder, uinteger tube){
 if(oct>=TRDOctagonNo()){
  cerr<<"TRDDBc::_check-S-TRDOctagonNoOutOfOrder "<<oct<<endl;
  return 0;
 }
 if(layer>=LayersNo(oct)){
  cerr<<"TRDDBc::_check-S-TRDLayersNoOutOfOrder "<<layer<<endl;
  return 0;
 }
 if(ladder>=LaddersNo(oct,layer)){
  cerr<<"TRDDBc::_check-S-TRDLaddersNoOutOfOrder "<<ladder<<endl;
  return 0;
 }
 if(tube>=TubesNo(oct,layer,ladder)){
  cerr<<"TRDDBc::_check-S-TRDTubesNoOutOfOrder "<<tube<<endl;
  return 0;
 }
 return 1;
}

number & TRDDBc::RadiatorDimensions(uinteger toct, uinteger lay, uinteger lad,uinteger index){
#ifdef __AMSDEBUG__
_check(toct,lay,lad);
assert(index<sizeof(_RadiatorDimensions)/sizeof(_RadiatorDimensions[0][0][0][0])/mtrdo/maxlay/maxlad);
#endif
return _RadiatorDimensions[toct][lay][lad][index];
}

number & TRDDBc::TubesBoxDimensions(uinteger toct, uinteger lay, uinteger lad,uinteger index){
#ifdef __AMSDEBUG__
_check(toct,lay,lad);
assert(index<sizeof(_TubesBoxDimensions)/sizeof(_TubesBoxDimensions[0][0][0][0])/mtrdo/maxlay/maxlad);
#endif
return _TubesBoxDimensions[toct][lay][lad][index];
}

number  TRDDBc::ITubesDimensions(uinteger toct, uinteger lay, uinteger lad,uinteger index){
#ifdef __AMSDEBUG__
_check(toct,lay,lad);
assert(index<sizeof(_TubesDimensions)/sizeof(_TubesDimensions[0][0][0][0])/mtrdo/maxlay/maxlad);
#endif
switch(index){
case 2:
return _TubesDimensions[toct][lay][lad][index];
break;
case 1:
return _TubesDimensions[toct][lay][lad][0];
break;
case 0:
return 0;
break;
}
}

number & TRDDBc::TubesDimensions(uinteger toct, uinteger lay, uinteger lad,uinteger index){
#ifdef __AMSDEBUG__
_check(toct,lay,lad);
assert(index<sizeof(_TubesDimensions)/sizeof(_TubesDimensions[0][0][0][0])/mtrdo/maxlay/maxlad);
#endif
return _TubesDimensions[toct][lay][lad][index];
}

number & TRDDBc::LaddersDimensions(uinteger toct, uinteger lay, uinteger lad,uinteger index){
#ifdef __AMSDEBUG__
_check(toct,lay,lad);
assert(index<sizeof(_LaddersDimensions)/sizeof(_LaddersDimensions[0][0][0][0])/mtrdo/maxlay/maxlad);
#endif
return _LaddersDimensions[toct][lay][lad][index];
}

number & TRDDBc::OctagonDimensions(uinteger toct, uinteger index){
#ifdef __AMSDEBUG__
assert(toct<OctagonNo());
assert(index<sizeof(_OctagonDimensions)/sizeof(_OctagonDimensions[0][0])/maxo);
#endif
return _OctagonDimensions[toct][index];
}


char* TRDDBc::CodeLad(uinteger gid){
 static char output[3]={'\0','\0','\0'};
 static char code[]="QWERTYUIOPASFGHJKLZXCVBNM1234567890";
 integer size=sizeof(code)-1;
 if(gid<size*size){
  output[0]=code[gid%size]; 
  output[1]=code[gid/size]; 
 }
 else{
   cerr<<"TRDDBc::CodeLad-F-CouldNotCodeId "<<gid<<endl;
   exit(1);
 }
 return output;
}

integer TRDDBcI::_Count=0;

TRDDBcI::TRDDBcI(){
_Count++;
}

TRDDBcI::~TRDDBcI(){
  if(--_Count ==0){
    TRDDBc::write();   
  }
}
