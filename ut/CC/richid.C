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
  if(RICCONTROL.setup==0){  // The official setup
    _ncrates=12;
    _npmts=1000;

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
    //               4  3 2  1          Y
    //                 -----            ^
    //               5 |   | 0          |
    //                 |   |            |
    //               6 |   | 11         |
    //                 -----            |
    //               7  8 9  10         -------> X

    _npmts_crate[0]=104;
    _npmts_crate[1]=42;
    _npmts_crate[2]=104;
    _npmts_crate[3]=104;
    _npmts_crate[4]=42;
    _npmts_crate[5]=104;
    _npmts_crate[6]=104;
    _npmts_crate[7]=42;
    _npmts_crate[8]=104;
    _npmts_crate[9]=104;
    _npmts_crate[10]=42;
    _npmts_crate[11]=104;

    _crate_pmt[0]=0;
    for(int i=1;i<12;i++) _crate_pmt[i]=_crate_pmt[i-1]+_npmts_crate[i-1];
  


    // Setup crates 0,2,3,5,6,8,9,11



    integer pmtnb=0;

    geant xedge=RICGEOM.hole_radius+RICGEOM.light_guides_length/2+RICpmtsupport;
    geant yedge=RICGEOM.hole_radius+
        (1-2*RICHDB::n_rows[0])*RICGEOM.light_guides_length/2;

  
    for(int i=0;i<RICHDB::n_rows[0];i++)
      for(int j=0;j<RICHDB::n_pmts[i][0];j++){

        geant x=xedge+(j+RICHDB::offset[i][0])*RICGEOM.light_guides_length;
        geant y=yedge+i*RICGEOM.light_guides_length;
        geant z=RICHDB::pmt_pos();

        *(_pmt_p[0]+_crate_pmt[0]+pmtnb)=x;
        *(_pmt_p[1]+_crate_pmt[0]+pmtnb)=y;
        *(_pmt_p[2]+_crate_pmt[0]+pmtnb)=z;

        *(_pmt_p[0]+_crate_pmt[2]+pmtnb)=y;
        *(_pmt_p[1]+_crate_pmt[2]+pmtnb)=x;
        *(_pmt_p[2]+_crate_pmt[2]+pmtnb)=z;

        *(_pmt_p[0]+_crate_pmt[3]+pmtnb)=-y;
        *(_pmt_p[1]+_crate_pmt[3]+pmtnb)=x;
        *(_pmt_p[2]+_crate_pmt[3]+pmtnb)=z;

        *(_pmt_p[0]+_crate_pmt[5]+pmtnb)=-x;
        *(_pmt_p[1]+_crate_pmt[5]+pmtnb)=y;
        *(_pmt_p[2]+_crate_pmt[5]+pmtnb)=z;

        *(_pmt_p[0]+_crate_pmt[6]+pmtnb)=-x;
        *(_pmt_p[1]+_crate_pmt[6]+pmtnb)=-y;
        *(_pmt_p[2]+_crate_pmt[6]+pmtnb)=z;
      
        *(_pmt_p[0]+_crate_pmt[8]+pmtnb)=-y;
        *(_pmt_p[1]+_crate_pmt[8]+pmtnb)=-x;
        *(_pmt_p[2]+_crate_pmt[8]+pmtnb)=z;

        *(_pmt_p[0]+_crate_pmt[9]+pmtnb)=y;
        *(_pmt_p[1]+_crate_pmt[9]+pmtnb)=-x;
        *(_pmt_p[2]+_crate_pmt[9]+pmtnb)=z;

        *(_pmt_p[0]+_crate_pmt[11]+pmtnb)=x;
        *(_pmt_p[1]+_crate_pmt[11]+pmtnb)=-y;
        *(_pmt_p[2]+_crate_pmt[11]+pmtnb)=z;

        pmtnb++;
      }

    pmtnb=0;
    xedge=RICGEOM.hole_radius+RICGEOM.light_guides_length/2+RICpmtsupport;
    yedge=RICGEOM.hole_radius+RICGEOM.light_guides_length/2+RICpmtsupport;

    for(int i=0;i<RICHDB::n_rows[1];i++)
      for(int j=0;j<RICHDB::n_pmts[i][1];j++){
        geant x=xedge+(j+RICHDB::offset[i][1])*RICGEOM.light_guides_length;
        geant y=yedge+i*RICGEOM.light_guides_length;
        geant z=RICHDB::pmt_pos();

        *(_pmt_p[0]+_crate_pmt[1]+pmtnb)=x;
        *(_pmt_p[1]+_crate_pmt[1]+pmtnb)=y;
        *(_pmt_p[2]+_crate_pmt[1]+pmtnb)=z;
  
        *(_pmt_p[0]+_crate_pmt[4]+pmtnb)=-x;
        *(_pmt_p[1]+_crate_pmt[4]+pmtnb)=y;
        *(_pmt_p[2]+_crate_pmt[4]+pmtnb)=z;

        *(_pmt_p[0]+_crate_pmt[7]+pmtnb)=-x;
        *(_pmt_p[1]+_crate_pmt[7]+pmtnb)=-y;
        *(_pmt_p[2]+_crate_pmt[7]+pmtnb)=z;

        *(_pmt_p[0]+_crate_pmt[10]+pmtnb)=x;
        *(_pmt_p[1]+_crate_pmt[10]+pmtnb)=-y;
        *(_pmt_p[2]+_crate_pmt[10]+pmtnb)=z;
      
      
        pmtnb++;
      }


    for(integer crate=0;crate<_ncrates;crate++)
      for(pmtnb=0;pmtnb<_npmts_crate[crate];pmtnb++)
        _pmt_crate[pmtnb+_crate_pmt[crate]]=crate;

    for(pmtnb=0;pmtnb<_npmts;pmtnb++)
      _pmt_orientation[pmtnb]=0;  // To be correctly filled in the future


#ifdef __AMSDEBUG__
  for(pmtnb=0;pmtnb<_npmts;pmtnb++)
    cout <<"  PMT #"<<pmtnb<<" position "<<*(_pmt_p[0]+pmtnb)<<" "<<
      *(_pmt_p[1]+pmtnb)<<" "<<*(_pmt_p[2]+pmtnb)<<"  crate "<<
      _pmt_crate[pmtnb]<<endl;
#endif


    // Fill some numbers for backward compatiblity... this to be removed soon

    RICHDB::total=_npmts;
    for(pmtnb=0;pmtnb<_npmts;pmtnb++){
      RICHDB::pmt_p[pmtnb][0]=*(_pmt_p[0]+pmtnb);
      RICHDB::pmt_p[pmtnb][1]=*(_pmt_p[1]+pmtnb);
    //    RICHDB::pmt_p[pmtnb][2]=*(_pmt_p[2]+pmtnb);
    }
  }else if(RICCONTROL.setup==1){  // Unofficial geometry as well as an example of how to define a new one

    cout<<">>>>>>>>>>>>> Unofficial RICH setup chosen <<<<<<<<<<<<<<"<<endl;

    _ncrates=8;
    _npmts=732;


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


    _npmts_crate[0]=152;
    _npmts_crate[1]=31;
    _npmts_crate[2]=152;
    _npmts_crate[3]=31;
    _npmts_crate[4]=152;
    _npmts_crate[5]=31;
    _npmts_crate[6]=152;
    _npmts_crate[7]=31;

    // Setups the look-up table 
    _crate_pmt[0]=0;
    for(int i=1;i<8;i++) _crate_pmt[i]=_crate_pmt[i-1]+_npmts_crate[i-1];

    integer pmtnb=0;
    const geant pitch=3.7;   // Pitch between PMTs

    // Setup crates 0,2,4,6: It's a bit tricky due to the strange arrangement of pmts
    {
      const int nrows=17;
      const int pmts[nrows]={7,8,9,9,9,9,10,10,10,10,10,9,9,9,9,8,7};
      const int offset[nrows]={1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
      const int max_pmts=10;
      const int block_number=6; 
      
      geant xedge=RICHDB::hole_radius+pitch/2+RICpmtsupport;
      geant yedge=-RICHDB::hole_radius+pitch/2;
      geant jump=0;
  

      for(int i=0;i<nrows;i++){

	if(i==block_number) jump=RICHDB::hole_radius-nrows*pitch/2;
	if(i==nrows-block_number) jump=2*RICHDB::hole_radius-nrows*pitch;

	for(int j=0;j<pmts[i];j++){
         
          *(_pmt_p[0]+_crate_pmt[0]+pmtnb)=xedge+(j+offset[i])*pitch;
          *(_pmt_p[1]+_crate_pmt[0]+pmtnb)=yedge+jump+i*pitch;
          *(_pmt_p[2]+_crate_pmt[0]+pmtnb)=RICHDB::pmt_pos();

          *(_pmt_p[0]+_crate_pmt[2]+pmtnb)=yedge+jump+i*pitch;
          *(_pmt_p[1]+_crate_pmt[2]+pmtnb)=xedge+(j+offset[i])*pitch;
          *(_pmt_p[2]+_crate_pmt[2]+pmtnb)=RICHDB::pmt_pos();

          *(_pmt_p[0]+_crate_pmt[4]+pmtnb)=-(xedge+(j+offset[i])*pitch);
          *(_pmt_p[1]+_crate_pmt[4]+pmtnb)=-(yedge+jump+i*pitch);
          *(_pmt_p[2]+_crate_pmt[4]+pmtnb)=RICHDB::pmt_pos();

          *(_pmt_p[0]+_crate_pmt[6]+pmtnb)=-(yedge+jump+i*pitch);
          *(_pmt_p[1]+_crate_pmt[6]+pmtnb)=-(xedge+(j+offset[i])*pitch);
          *(_pmt_p[2]+_crate_pmt[6]+pmtnb)=RICHDB::pmt_pos();

	  pmtnb++;

        }
      }
    }

    pmtnb=0;


    { //Setup crates 1,3,5,7
      const int nrows=7;
      const int pmts[nrows]={6,6,6,5,4,3,1};
      const int offset[nrows]={1,0,0,0,0,0,0};
      const int max_pmts=6;
          
      geant xedge=RICHDB::hole_radius+pitch/2+RICpmtsupport;
      geant yedge=RICHDB::hole_radius+pitch/2+RICpmtsupport;

      for(int i=0;i<nrows;i++)
       for(int j=0;j<pmts[i];j++){

        *(_pmt_p[0]+_crate_pmt[1]+pmtnb)=xedge+(j+offset[i])*pitch;
        *(_pmt_p[1]+_crate_pmt[1]+pmtnb)=yedge+i*pitch;
        *(_pmt_p[2]+_crate_pmt[1]+pmtnb)=RICHDB::pmt_pos();

        *(_pmt_p[0]+_crate_pmt[3]+pmtnb)=-(xedge+(j+offset[i])*pitch);
        *(_pmt_p[1]+_crate_pmt[3]+pmtnb)=yedge+i*pitch;       
        *(_pmt_p[2]+_crate_pmt[3]+pmtnb)=RICHDB::pmt_pos();

        *(_pmt_p[0]+_crate_pmt[5]+pmtnb)=-(xedge+(j+offset[i])*pitch);
        *(_pmt_p[1]+_crate_pmt[5]+pmtnb)=-(yedge+i*pitch);       
        *(_pmt_p[2]+_crate_pmt[5]+pmtnb)=RICHDB::pmt_pos();

        *(_pmt_p[0]+_crate_pmt[7]+pmtnb)=xedge+(j+offset[i])*pitch;
        *(_pmt_p[1]+_crate_pmt[7]+pmtnb)=-(yedge+i*pitch);
        *(_pmt_p[2]+_crate_pmt[7]+pmtnb)=RICHDB::pmt_pos();

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
  // No tiene en cuenta la rotacion del pmt
  x-=AMSRICHIdGeom::pmt_pos(pmtnb,0)-RICcatolength/2;
  y-=AMSRICHIdGeom::pmt_pos(pmtnb,1)-RICcatolength/2;
  x/=RICcatolength/sqrt(RICnwindows);
  y/=RICcatolength/sqrt(RICnwindows);
  integer channel=RICnwindows*pmtnb+integer(sqrt(RICnwindows))*
    integer(y)+integer(x);
  
  _channel=channel;
  _pmt=channel/16;
  _pixel=channel%16;
  _crate=_pmt_crate[_pmt];
  _pmt_in_crate=_pmt-_crate_pmt[_crate];
}



// The rotation is not implemented yet

geant AMSRICHIdGeom::x(){
  geant x=(2*(_pixel%integer(sqrt(RICnwindows)))-3)*
    RICHDB::lg_tile_size/8.+*(_pmt_p[0]+_pmt);

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
    RICHDB::lg_tile_size/8.+*(_pmt_p[1]+_pmt);

  return y;
}


geant AMSRICHIdGeom::z(){
  geant z=*(_pmt_p[2]+_pmt);

  return z;
}





