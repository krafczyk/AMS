#include "richradid.h"

/////////////////////////////////////////////////

integer RichRadiatorTile::_number_of_rad_tiles=0;
integer *RichRadiatorTile::_kind_of_tile=0;
geant RichRadiatorTile::_eff_indexes[radiator_kinds]={0,0};
geant RichRadiatorTile::_rad_heights[radiator_kinds]={0,0};
geant RichRadiatorTile::_clarities[radiator_kinds]={0,0};
geant *RichRadiatorTile::_abs_length[radiator_kinds]={0,0};
geant *RichRadiatorTile::_index_tables[radiator_kinds]={0,0};
geant RichRadiatorTile::_mean_height[radiator_kinds][2]={{0,0},{0,0}};


void RichRadiatorTile::Init(){

  // A really crude approximation to the circunference
  _number_of_rad_tiles=2*int((RICHDB::rad_radius/RICHDB::rad_length)+.5)+1;


  // Define te array
  _kind_of_tile=new integer[_number_of_rad_tiles*_number_of_rad_tiles];


  // Fill the array
  geant max_radius=_number_of_rad_tiles*RICHDB::rad_length/2.;

  // The algorithm is quite stupid: check if any of the tile borders is inside the radius.
  // If so fill it, otherwise let it empty. 

  for(int i=0;i<_number_of_rad_tiles;i++)
    for(int j=0;j<_number_of_rad_tiles;j++){

      geant x,y;

      x=i*RICHDB::rad_length-max_radius;
      y=j*RICHDB::rad_length-max_radius;

#define _rad_(x,y) (sqrt((x)*(x)+(y)*(y)))

      if(_rad_(x,y)<RICHDB::rad_radius || 
	 _rad_(x+RICHDB::rad_length,y)<RICHDB::rad_radius ||
	 _rad_(x,y+RICHDB::rad_length)<RICHDB::rad_radius ||
	 _rad_(x+RICHDB::rad_length,y+RICHDB::rad_length)<RICHDB::rad_radius ){

	_kind_of_tile[i*_number_of_rad_tiles+j]=agl_kind;


      }else _kind_of_tile[i*_number_of_rad_tiles+j]=empty_kind;
      
#undef _rad_
    }
 
  // In we have chosen Naf put a 3x3 array of Naf right in the center
  if(RICRADSETUPFFKEY.setup==1){

    for(int i=(_number_of_rad_tiles-1)/2-1;i<=(_number_of_rad_tiles-1)/2+1;i++)
      for(int j=(_number_of_rad_tiles-1)/2-1;j<=(_number_of_rad_tiles-1)/2+1;j++)
	_kind_of_tile[i*_number_of_rad_tiles+j]=naf_kind;

  }



#ifdef __AMSDEBUG__
  cout<<"Dumping radiador geometry"<<endl
      <<"-------------------------"<<endl;
  for(int j=0;j<_number_of_rad_tiles;j++){
    for(int i=0;i<_number_of_rad_tiles;i++)
      cout<<_kind_of_tile[i*_number_of_rad_tiles+j];
    cout<<endl;
  }
  
#endif
  

  // Here we should fill the effective index and the heights using the routine in RICHDB for both radiators 
  // Everything is filled by hand


  // Ensure that the index tables for agl are OK
  RICHDB::mat_init();

  //  _rad_heights[0]=RICHDB::rad_height;
  _rad_heights[0]=RICHDB::rad_agl_height; //Updated to new index
  _rad_heights[1]=RICHDB::naf_height;
  _abs_length[0]=RICHDB::abs_length;
  _abs_length[1]=RICHDB::naf_abs_length;
  //  _clarities[0]=RICHDB::rad_clarity;
  _clarities[0]=RICHDB::eff_rad_clarity;  // We use the effective one due to the scattering
  _clarities[1]=0.;
  _index_tables[0]=RICHDB::index;
  _index_tables[1]=RICHDB::naf_index_table;

  // Now we compute the effective height calling the 
  // magic routine

  for(int i=0;i<radiator_kinds;i++)
    _compute_mean_height(_index_tables[i],_clarities[i],_abs_length[i],_rad_heights[i],_eff_indexes[i],
  			 _mean_height[i][0]);

  _mean_height[0][1]=_mean_height[0][0]-.1;
  _mean_height[1][1]=_mean_height[1][0]-.2;



}




RichRadiatorTile::RichRadiatorTile(AMSTrTrack *track){

  // First decide wich kind of radiator is current

  AMSPoint pnt(0.,0.,RICHDB::RICradpos()-RICHDB::rad_height),
    point;
  AMSDir dir(0.,0.,-1.);
  
  number theta,phi,length;

  track->interpolate(pnt,dir,point,
		     theta,phi,length);

  _kind=get_tile_kind(get_tile_number(point[0],point[1]));

  if(_kind==empty_kind){
    _index=0;
    _height=0;
    _p_direct=AMSPoint(0.,0.,0.);
    _p_reflected=AMSPoint(0.,0.,0.);
    _d_direct=AMSDir(0.,0.);
    _d_reflected=AMSDir(0.,0.);
    return;
  }


  _index=_eff_indexes[_kind-1];
  _height=_rad_heights[_kind-1];

  pnt.setp(0.,0.,RICHDB::RICradpos()-RICHDB::rad_height+_height);
  track->interpolate(pnt,dir,point,
		     theta,phi,
		     length);
  
  if(_kind!=get_tile_kind(get_tile_number(point[0],point[1]))){
    _kind=empty_kind;
    _index=0;
    _height=0;
    _p_direct=AMSPoint(0.,0.,0.);
    _p_reflected=AMSPoint(0.,0.,0.);
    _d_direct=AMSDir(0.,0.);
    _d_reflected=AMSDir(0.,0.);
    return;
  }

  _p_entrance=point;
  _d_entrance=AMSDir(theta,phi);


  // Direct photons
  pnt.setp(0.,0.,RICHDB::RICradpos()-RICHDB::rad_height+_mean_height[_kind-1][0]);
  track->interpolate(pnt,dir,point,theta,phi,length);
  
  _p_direct=point;
  _d_direct=AMSDir(theta,phi);
  

  // Direct photons
  pnt.setp(0.,0.,RICHDB::RICradpos()-RICHDB::rad_height+_mean_height[_kind-1][1]);
  track->interpolate(pnt,dir,point,theta,phi,length);
  
  _p_reflected=point;
  _d_reflected=AMSDir(theta,phi);


} 




void RichRadiatorTile::_compute_mean_height(geant *index,
				    geant clarity,
				    geant *abs_len,
				    geant rheight,
				    geant &eff_index,
				    geant &height){
  // Computes the mean emission point inside the radiator
  // of the detected photons.
  // The credits go to .... Elisa Lanciotti

  const integer steps=100;    // Number of steps for the approximation
  geant lambda,qeff,n,dl,l_scat=0,l_abs_rad,l_abs_lg;
  geant sum=0,densum=0;
  geant sum_index=0;

  for(integer i=0;i<RICmaxentries-1;i++){ // Integration in wave length
    lambda=(RICHDB::wave_length[i]+RICHDB::wave_length[i+1])/2.;
    qeff=(RICHDB::eff[i]+RICHDB::eff[i+1])/2.;
    n=(index[i]+index[i+1])/2.;
    dl=RICHDB::wave_length[i]-RICHDB::wave_length[i+1];
    l_scat=clarity==0?1e6:(lambda/1000.)*(lambda/1000.)*(lambda/1000.)*(lambda/1000.)/
	clarity;
    l_abs_rad=(abs_len[i]+abs_len[i+1])/2.;
    l_abs_lg=(RICHDB::lg_abs[i]+RICHDB::lg_abs[i+1])/2.;


    for(integer j=0;j<steps;j++){ // Integration in radiador thicknes
      geant x=rheight*(geant(j)+0.5)/geant(steps);
      geant g=qeff/lambda/lambda/exp((rheight-x)*
				     (1/l_scat+1/l_abs_rad))/
	exp(RICHDB::lg_height/l_abs_lg);
      sum+=dl*g*x;
      densum+=dl*g;
      sum_index+=dl*g*n;
    }
  }
  if(!densum){
    cout<<"RichRadiatorTile::_mean_height : Error"<<endl;
  }else{
    height=rheight-sum/densum;

    eff_index=sum_index/densum;


  }
} 





