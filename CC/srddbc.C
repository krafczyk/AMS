//  $Id: srddbc.C,v 1.2 2001/01/22 17:32:21 choutko Exp $
#include <srddbc.h>
#include <amsdbc.h>
#include <math.h>
char * SRDDBc::_VolumeMedia[maxo]={"VACUUM", "SRDWLS","VACUUM",
"TRDCarbonFiber","TRDHC"};
char * SRDDBc::_XtallsMedia="SRDXtall";
char * SRDDBc::_PMTMedia="SRDPMT";

uinteger SRDDBc::_VolumesNo=maxo;

uinteger SRDDBc::_XtallColNo=200/2.5;
uinteger SRDDBc::_XtallRowNo=200/2.5;
uinteger SRDDBc::_XtallNo=_XtallColNo*_XtallRowNo;
uinteger SRDDBc::_PMTColNo=_XtallColNo/2;
uinteger SRDDBc::_PMTRowNo=_XtallRowNo/2;
uinteger SRDDBc::_PMTNo=_PMTColNo*_PMTRowNo;



const number  SRDDBc::_XtallSize[]={2.5,2.5,0.1,2.5,2.5};
const number  SRDDBc::_PMTSize[]={0,3.35,0.45,5.,5. };
const number  SRDDBc::_VolumeSize[maxo][3]={260.,260.,0.1,
                                  260.,260.,0.2,
                                  260.,260.,1.77,
                                  260.,260.,0.43,
                                  260.,260.,12.};

  // Positions & Orientations
  
SRDDBc* SRDDBc::_HeadVolumePos=0;
SRDDBc* SRDDBc::_HeadXtallPos=0;
SRDDBc* SRDDBc::_HeadPMTPos=0;

void SRDDBc::init(){




  cout <<"SRDDBcI-I-Total of " <<_XtallNo<< "  xtalls initialized."<<endl;



       _HeadXtallPos=new SRDDBc[_XtallNo];
       _HeadPMTPos=new SRDDBc[_PMTNo];
       _HeadVolumePos=new SRDDBc[_VolumesNo];



   // HereStartsTheJob;

      number unitnrm[3][3]={1,0,0,0,1,0,0,0,1};


//    Volume Pos

      uinteger status=1;
      uinteger gid=0;
      int i;
      geant coo[3];
      for(i=0;i<VolumesNo();i++){
       for(int j=0;j<3;j++){
        coo[j]=0;
       }
       coo[2]=1450./10+VolumeDimensions(i,2);
       for(int k=VolumesNo()-1;k>i;k--){
        coo[2]+=2*VolumeDimensions(k,2);
       }
       SetVolume(i,status,coo,unitnrm,gid); 
      }

// Xtalls, PMT


      for(i=0;i<XtallColNo();i++){
       for(int j=0;j<XtallRowNo();j++){
        coo[0]=-1.*(XtallColNo()-2.*i)*XtallDimensions(0,0,3)+XtallDimensions(0,0,0);
        coo[1]=-1.*(XtallRowNo()-2.*j)*XtallDimensions(0,0,4)+XtallDimensions(0,0,1);
        coo[2]=VolumeDimensions(0,2)-XtallDimensions(0,0,2);
        SetXtall(i,j,status,coo,unitnrm,gid); 
      }
     }
      for(i=0;i<PMTColNo();i++){
       for(int j=0;j<PMTRowNo();j++){
        coo[0]=-1.*(PMTColNo()-2.*i)*PMTDimensions(0,0,3)+PMTDimensions(0,0,1);
        coo[1]=-1.*(PMTRowNo()-2.*j)*PMTDimensions(0,0,4)+PMTDimensions(0,0,1);
        coo[2]=VolumeDimensions(2,2)-PMTDimensions(0,0,2);
        SetPMT(i,j,status,coo,unitnrm,gid); 
      }
     }

}

void SRDDBc::read(){

init();

}

void SRDDBc::write(){
}

uinteger SRDDBc::getnumVolume(uinteger oct){
#ifdef __AMSDEBUG__
assert(oct<VolumesNo());
#endif
return oct;
}

uinteger SRDDBc::getnumXtall(uinteger col,uinteger row){

     uinteger num=col*_XtallRowNo+row;
#ifdef __AMSDEBUG__
   assert(num<_XtallNo);
#endif
     return num;
}

uinteger SRDDBc::getnumPMT(uinteger col,uinteger row){

     uinteger num=col*_PMTRowNo+row;
#ifdef __AMSDEBUG__
   assert(num<_PMTNo);
#endif
     return num;
}


void SRDDBc::SetVolume(uinteger oct,
             uinteger  status, geant coo[],number nrm[3][3], uinteger gid){
#ifdef __AMSDEBUG__
assert(oct<VolumesNo());
#endif
   _HeadVolumePos[oct]._status=status;     
   _HeadVolumePos[oct]._gid=gid;     
   int i,j;
   for(i=0;i<3;i++){
    _HeadVolumePos[oct]._coo[i]=coo[i];
    for(j=0;j<3;j++){
     _HeadVolumePos[oct]._nrm[i][j]=nrm[i][j];
    }
   }
}


void SRDDBc::GetVolume(uinteger oct,
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid){
#ifdef __AMSDEBUG__
assert(oct<VolumesNo());
#endif
   oct=getnumVolume(oct);
   status=_HeadVolumePos[oct]._status;     
   gid=_HeadVolumePos[oct]._gid;     
   int i,j;
   for(i=0;i<3;i++){
    coo[i]=_HeadVolumePos[oct]._coo[i];
    for(j=0;j<3;j++){
     nrm[i][j]=_HeadVolumePos[oct]._nrm[i][j];
    }
   }

}

void SRDDBc::SetXtall(uinteger col, uinteger row,
             uinteger  status, geant coo[],number nrm[3][3], uinteger gid){
    uinteger oct=getnumXtall(col,row);    
   _HeadXtallPos[oct]._status=status;     
   _HeadXtallPos[oct]._gid=gid;     
   int i,j;
   for(i=0;i<3;i++){
    _HeadXtallPos[oct]._coo[i]=coo[i];
    for(j=0;j<3;j++){
     _HeadXtallPos[oct]._nrm[i][j]=nrm[i][j];
    }
   }
}


void SRDDBc::GetXtall(uinteger col, uinteger row, 
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid){
   uinteger oct=getnumXtall(col,row);    
   status=_HeadXtallPos[oct]._status;     
   gid=_HeadXtallPos[oct]._gid;     
   int i,j;
   for(i=0;i<3;i++){
    coo[i]=_HeadXtallPos[oct]._coo[i];
    for(j=0;j<3;j++){
     nrm[i][j]=_HeadXtallPos[oct]._nrm[i][j];
    }
   }

}
void SRDDBc::SetPMT(uinteger col, uinteger row,
             uinteger  status, geant coo[],number nrm[3][3], uinteger gid){
    uinteger oct=getnumPMT(col,row);    
   _HeadPMTPos[oct]._status=status;     
   _HeadPMTPos[oct]._gid=gid;     
   int i,j;
   for(i=0;i<3;i++){
    _HeadPMTPos[oct]._coo[i]=coo[i];
    for(j=0;j<3;j++){
     _HeadPMTPos[oct]._nrm[i][j]=nrm[i][j];
    }
   }
}


void SRDDBc::GetPMT(uinteger col, uinteger row, 
                          uinteger & status, geant coo[],number nrm[3][3],
                          uinteger &gid){
   uinteger oct=getnumPMT(col,row);    
   status=_HeadPMTPos[oct]._status;     
   gid=_HeadPMTPos[oct]._gid;     
   int i,j;
   for(i=0;i<3;i++){
    coo[i]=_HeadPMTPos[oct]._coo[i];
    for(j=0;j<3;j++){
     nrm[i][j]=_HeadPMTPos[oct]._nrm[i][j];
    }
   }

}

number  SRDDBc::VolumeDimensions(uinteger vol, uinteger index){
#ifdef __AMSDEBUG__
 assert(index<sizeof(_VolumeSize)/sizeof(_VolumeSize[0][0])/maxo);
 assert(vol<maxo);
#endif
return _VolumeSize[vol][index]/2;
}

integer SRDDBcI::_Count=0;

SRDDBcI::SRDDBcI(){
_Count++;
}

SRDDBcI::~SRDDBcI(){
  if(--_Count ==0){
    SRDDBc::write();   
  }
}
