#include"richid.h"
#include"commons.h"


geant *AMSRICHIdGeom::_pmt_p[3]={0,0,0};
integer *AMSRICHIdGeom::_pmt_orientation=0;
integer *AMSRICHIdGeom::_pmt_crate=0;
integer *AMSRICHIdGeom::_npmts_crate=0;
integer *AMSRICHIdGeom::_crate_pmt=0;
integer AMSRICHIdGeom::_ncrates=12;
integer AMSRICHIdGeom::_npmts=1000;


void AMSRICHIdGeom::Init(){
    _ncrates=8;
    _npmts=680;


    // Alloc space for pmt position
    _pmt_p[0]=new geant[_npmts];
    _pmt_p[1]=new geant[_npmts];
    _pmt_p[2]=new geant[_npmts];

    _pmt_orientation=new integer[_npmts];
  
    // Alloc space for pmt->crate table
    _pmt_crate=new integer[_npmts];
  
    // Alloc space fot crate->first pmt table
    _crate_pmt=new integer[_ncrates];

    // Alloc scape for number of pmts per crate
    _npmts_crate=new integer[_ncrates];
 

    // Fill tables: manual filling


    // Crate positioning is according to next drawing
    //
    //          3| 2 |1
    //          -+---+-
    //          4|   |0
    //          -+---+-
    //          5| 6 |7


    _npmts_crate[0]=143;
    _npmts_crate[1]=27;
    _npmts_crate[2]=143;
    _npmts_crate[3]=27;
    _npmts_crate[4]=143;
    _npmts_crate[5]=27;
    _npmts_crate[6]=143;
    _npmts_crate[7]=27;

    // Setups the look-up table 
    _crate_pmt[0]=0;
    for(int i=1;i<8;i++) _crate_pmt[i]=_crate_pmt[i-1]+_npmts_crate[i-1];

    integer pmtnb=0;

    // Setup crates 0,2,4,6: It's a bit tricky due to the strange arrangement of pmts
    {
      const int nrows=17;
      const int pmts[nrows]={6,8,8,9,9,9,9,9,9,9,9,9,9,9,8,8,6};
      const int offset[nrows]={1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
      const int max_pmts=10;
      const int block_number=6; 
      
      geant xedge=RICHDB::hole_radius+pitch/2+RICpmtsupport;
      geant yedge=-RICHDB::hole_radius+pitch/2;  

      for(int i=0;i<nrows;i++){

	for(int j=0;j<pmts[i];j++){
         
          *(_pmt_p[0]+_crate_pmt[0]+pmtnb)=xedge+(j+offset[i])*pitch;
          *(_pmt_p[1]+_crate_pmt[0]+pmtnb)=yedge+i*pitch;
          *(_pmt_p[2]+_crate_pmt[0]+pmtnb)=-RICHDB::pmt_pos();

          *(_pmt_p[0]+_crate_pmt[2]+pmtnb)=yedge+i*pitch;
          *(_pmt_p[1]+_crate_pmt[2]+pmtnb)=xedge+(j+offset[i])*pitch;
          *(_pmt_p[2]+_crate_pmt[2]+pmtnb)=-RICHDB::pmt_pos();

          *(_pmt_p[0]+_crate_pmt[4]+pmtnb)=-(xedge+(j+offset[i])*pitch);
          *(_pmt_p[1]+_crate_pmt[4]+pmtnb)=-(yedge+i*pitch);
          *(_pmt_p[2]+_crate_pmt[4]+pmtnb)=-RICHDB::pmt_pos();

          *(_pmt_p[0]+_crate_pmt[6]+pmtnb)=-(yedge+i*pitch);
          *(_pmt_p[1]+_crate_pmt[6]+pmtnb)=-(xedge+(j+offset[i])*pitch);
          *(_pmt_p[2]+_crate_pmt[6]+pmtnb)=-RICHDB::pmt_pos();

	  pmtnb++;

        }
      }
    }

    pmtnb=0;


    { //Setup crates 1,3,5,7
      const int nrows=7;
      const int pmts[nrows]={6,6,5,4,3,2,1};
      const int offset[nrows]={1,0,0,0,0,0,0};
      const int max_pmts=6;
          
      geant xedge=RICHDB::hole_radius+pitch/2+RICpmtsupport;
      geant yedge=RICHDB::hole_radius+pitch/2+RICpmtsupport;

      for(int i=0;i<nrows;i++)
       for(int j=0;j<pmts[i];j++){

        *(_pmt_p[0]+_crate_pmt[1]+pmtnb)=xedge+(j+offset[i])*pitch;
        *(_pmt_p[1]+_crate_pmt[1]+pmtnb)=yedge+i*pitch;
        *(_pmt_p[2]+_crate_pmt[1]+pmtnb)=-RICHDB::pmt_pos();

        *(_pmt_p[0]+_crate_pmt[3]+pmtnb)=-(xedge+(j+offset[i])*pitch);
        *(_pmt_p[1]+_crate_pmt[3]+pmtnb)=yedge+i*pitch;       
        *(_pmt_p[2]+_crate_pmt[3]+pmtnb)=-RICHDB::pmt_pos();

        *(_pmt_p[0]+_crate_pmt[5]+pmtnb)=-(xedge+(j+offset[i])*pitch);
        *(_pmt_p[1]+_crate_pmt[5]+pmtnb)=-(yedge+i*pitch);       
        *(_pmt_p[2]+_crate_pmt[5]+pmtnb)=-RICHDB::pmt_pos();

        *(_pmt_p[0]+_crate_pmt[7]+pmtnb)=xedge+(j+offset[i])*pitch;
        *(_pmt_p[1]+_crate_pmt[7]+pmtnb)=-(yedge+i*pitch);
        *(_pmt_p[2]+_crate_pmt[7]+pmtnb)=-RICHDB::pmt_pos();

        pmtnb++;

       }

    }

    for(integer crate=0;crate<_ncrates;crate++)
      for(pmtnb=0;pmtnb<_npmts_crate[crate];pmtnb++)
        _pmt_crate[pmtnb+_crate_pmt[crate]]=crate;
        
    for(pmtnb=0;pmtnb<_npmts;pmtnb++)
      _pmt_orientation[pmtnb]=0;  // To be correctly filled in the future


    RICHDB::total=_npmts;

}




AMSRICHIdGeom::AMSRICHIdGeom(integer channel){
  channel=channel<0?0:channel%(_npmts*RICnwindows);

  _channel=channel;
  _pmt=channel/16;
  _pixel=channel%16;
  _crate=_pmt_crate[_pmt];
  _pmt_in_crate=_pmt-_crate_pmt[_crate];

}

AMSRICHIdGeom::AMSRICHIdGeom(integer pmtnb,geant x,geant y){
  // Better simulation of photocathode effective area


//  x-=AMSRICHIdGeom::pmt_pos(pmtnb,0)-RICcatolength/2;
//  y-=AMSRICHIdGeom::pmt_pos(pmtnb,1)-RICcatolength/2;
//  x/=RICcatolength/sqrt(RICnwindows);
//  y/=RICcatolength/sqrt(RICnwindows);


#ifdef __AMSDEBUG__
  //  cout <<"nx and ny accorgding to old routine: "<<integer(x2)<<" "<<integer(y2)<<endl;
#endif

  x-=AMSRICHIdGeom::pmt_pos(pmtnb,0);
  y-=AMSRICHIdGeom::pmt_pos(pmtnb,1);

  if(fabs(x)>RICcatolength/2 || fabs(y)>RICcatolength/2){_channel=-1;return;}  //error
  if(fabs(x)<RICcatogap/2. || fabs(y)<RICcatogap/2.){_channel=-1;return;}
  if(fabs(fabs(x)-(RICcatogap+cato_inner_pixel))<RICcatogap/2. ||
     fabs(fabs(y)-(RICcatogap+cato_inner_pixel))<RICcatogap/2.) {_channel=-1;return;}

  integer nx,ny;

  if(fabs(x)>RICcatogap+cato_inner_pixel){
    nx=x>0?3:0;
  }else{
    nx=x>0?2:1;
  }
  
  if(fabs(y)>RICcatogap+cato_inner_pixel){
    ny=y>0?3:0;
  }else{
    ny=y>0?2:1;
  }


#ifdef __AMSDEBUG__
  cout<<"nx and ny according to new routine "<<nx<<" "<<ny<<endl;
#endif

  integer channel=RICnwindows*pmtnb+integer(sqrt(RICnwindows))*
    ny+nx;
//    integer(y)+integer(x);
  
  _channel=channel;
  _pmt=channel/16;
  _pixel=channel%16;
  _crate=_pmt_crate[_pmt];
  _pmt_in_crate=_pmt-_crate_pmt[_crate];
}



// The rotation is not implemented yet

geant AMSRICHIdGeom::x(){

// WARNING This to be mapped to the new setup

  geant x=(2*(_pixel%integer(sqrt(RICnwindows)))-3)*
    RICHDB::lg_length/8.+*(_pmt_p[0]+_pmt);

#ifdef __AMSDEBUG__
  if(x>1e6){
    cout<<endl<<"--------"<<endl
	<<"PMT position in x "<<*(_pmt_p[0]+_pmt)<<endl
	<<"Factor "<<2*(_pixel%integer(sqrt(RICnwindows)))-3<<endl
	<<"--------"<<endl;
  }
#endif

  return x;
}

geant AMSRICHIdGeom::y(){
  geant y=(2*(_pixel/integer(sqrt(RICnwindows)))-3)*
    RICHDB::lg_length/8.+*(_pmt_p[1]+_pmt);

  return y;
}


geant AMSRICHIdGeom::z(){
  geant z=RICradpos+*(_pmt_p[2]+_pmt)+RICHDB::pmtb_height()/2.;

  return z;
}





