#include "richradid.h"

/////////////////////////////////////////////////

integer RichRadiatorTileManager::_number_of_rad_tiles=0;
RichRadiatorTile **RichRadiatorTileManager::_tiles=0;


void RichRadiatorTileManager::Init(){  // Default initialization
  if(_number_of_rad_tiles!=0) return; // Not necessary

  cout<<"RichRadiatorTileManager::Init-default radiator"<<endl;
  Init_Default();
}

void RichRadiatorTileManager::Init_Default(){  // Default initialization
  if(RICRADSETUPFFKEY.setup!=1){
    cout<<"RichRadiatorTileManager::Init_Default RICRADSETUPFFKEY.setup!=0 is not longer supported -- forcing default RICRADSETUPFFKEY.setup=1"<<endl;
    RICRADSETUPFFKEY.setup=1;
  }

  const int agl_boxes_number=11;
  int agl_boxes[agl_boxes_number][agl_boxes_number]={
    0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
    0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1,
    1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1,
    1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0
  };

  const int naf_boxes_number=3;
  int naf_boxes[naf_boxes_number][naf_boxes_number]={
    1,1,1,
    1,1,1,
    1,1,1
  };


  // Count the number of tiles
  _number_of_rad_tiles=0;
  for(int i=0;i<agl_boxes_number;i++)
    for(int j=0;j<agl_boxes_number;j++){
      if(agl_boxes[i][j]==1) _number_of_rad_tiles++;
    }

  for(int i=0;i<naf_boxes_number;i++)
    for(int j=0;j<naf_boxes_number;j++){
      if(naf_boxes[i][j]==1) _number_of_rad_tiles++;
    }
 
  cout<<"   RichRadiatorTileManager::Init_Default-number of radiator tiles: "<<_number_of_rad_tiles<<endl;

  _tiles=new RichRadiatorTile*[_number_of_rad_tiles];

  RICHDB::mat_init();  // Ensure that the agl tables are updated.

  // Fill the information for agl
  int current=0;
  for(int i=0;i<agl_boxes_number;i++)
    for(int j=0;j<agl_boxes_number;j++){
      if(agl_boxes[i][j]!=1) continue;
      double x=(i+0.5*(1-agl_boxes_number))*RICHDB::rad_length;
      double y=(j+0.5*(1-agl_boxes_number))*RICHDB::rad_length;
      _tiles[current]=new RichRadiatorTile;
      _tiles[current]->position[0]=x;
      _tiles[current]->position[1]=y;
      _tiles[current]->bounding_box[0][0]=-RICHDB::rad_length/2+RICaethk/2.;
      _tiles[current]->bounding_box[0][1]=RICHDB::rad_length/2-RICaethk/2.;
      _tiles[current]->bounding_box[1][0]=-RICHDB::rad_length/2+RICaethk/2.;
      _tiles[current]->bounding_box[1][1]=RICHDB::rad_length/2-RICaethk/2.;
      _tiles[current]->bounding_box[2][0]=-RICHDB::rad_agl_height/2;
      _tiles[current]->bounding_box[2][1]=RICHDB::rad_agl_height/2;
      _tiles[current]->kind=agl_kind;
      _tiles[current]->id=current;
      _tiles[current]->clarity=RICHDB::eff_rad_clarity;
      _tiles[current]->effective_scattering_probability=0;
      _tiles[current]->effective_scattering_angle=0;
      _tiles[current]->abs_length=RICHDB::abs_length;
      _tiles[current]->index_table=RICHDB::index;
      current++;
    }

  // Now we compute the effective height and effective index calling the dedicated routine
  geant eff_index;
  geant eff_height;
  _compute_mean_height(_tiles[current-1]->index_table,
		       _tiles[current-1]->clarity,
		       _tiles[current-1]->abs_length,
		       _tiles[current-1]->bounding_box[2][1]-_tiles[current-1]->bounding_box[2][0],
		       eff_index,
		       eff_height);

  for(int i=0;i<current;i++){
    if(_tiles[i]->kind!=agl_kind) continue;
    _tiles[i]->mean_refractive_index=eff_index;
    _tiles[i]->mean_height=eff_height-0.1; // Corretion by hand
  }
  
  for(int i=0;i<naf_boxes_number;i++)
    for(int j=0;j<naf_boxes_number;j++){
      if(naf_boxes[i][j]!=1) continue;
      double x=(i+0.5*(1-naf_boxes_number))*RICHDB::rad_length;
      double y=(j+0.5*(1-naf_boxes_number))*RICHDB::rad_length;
      _tiles[current]=new RichRadiatorTile;
      _tiles[current]->position[0]=x;
      _tiles[current]->position[1]=y;
      _tiles[current]->bounding_box[0][0]=-RICHDB::rad_length/2+RICaethk/2.;
      _tiles[current]->bounding_box[0][1]=RICHDB::rad_length/2-RICaethk/2.;
      _tiles[current]->bounding_box[1][0]=-RICHDB::rad_length/2+RICaethk/2.;
      _tiles[current]->bounding_box[1][1]=RICHDB::rad_length/2-RICaethk/2.;
      _tiles[current]->bounding_box[2][0]=-RICHDB::naf_height/2;
      _tiles[current]->bounding_box[2][1]=RICHDB::naf_height/2;
      _tiles[current]->kind=naf_kind;
      _tiles[current]->id=current;
      _tiles[current]->clarity=0;
      _tiles[current]->effective_scattering_probability=0;
      _tiles[current]->effective_scattering_angle=0;
      _tiles[current]->abs_length=RICHDB::naf_abs_length;
      _tiles[current]->index_table=RICHDB::naf_index_table;
      current++;
    }

  _compute_mean_height(_tiles[current-1]->index_table,
		       _tiles[current-1]->clarity,
		       _tiles[current-1]->abs_length,
		       _tiles[current-1]->bounding_box[2][1]-_tiles[current-1]->bounding_box[2][0],
		       eff_index,
		       eff_height);
  
  for(int i=0;i<_number_of_rad_tiles;i++){
    if(_tiles[i]->kind!=naf_kind) continue;
    _tiles[i]->mean_refractive_index=eff_index;
    _tiles[i]->mean_height=eff_height-0.2; // Corretion by hand
  }
}

#define max(x,y) ((x)<(y)?(y):(x))
#define min(x,y) ((x)<(y)?(x):(y))

integer RichRadiatorTileManager::get_tile_number(geant x,geant y){
  for(int i=0;i<_number_of_rad_tiles;i++){
    double dx=x-_tiles[i]->position[0];
    double dy=y-_tiles[i]->position[1];

    if(dx<max(_tiles[i]->bounding_box[0][0],_tiles[i]->bounding_box[0][1]) &&
       dx>min(_tiles[i]->bounding_box[0][0],_tiles[i]->bounding_box[0][1]) &&
       dy<max(_tiles[i]->bounding_box[1][0],_tiles[i]->bounding_box[1][1]) &&
       dy>min(_tiles[i]->bounding_box[1][0],_tiles[i]->bounding_box[1][1])){
      return i;
    }
  }
  return -1;
}



RichRadiatorTileManager::RichRadiatorTileManager(AMSTrTrack *track){
  if(_number_of_rad_tiles==0){
    cerr<<"RichRadiatorTileManager::RichRadiatorTileManager -- tiles not initialized -- doing it"<<endl;
    Init();
  }

  // First decide wich kind of radiator is current
  AMSPoint pnt(0.,0.,RICHDB::RICradpos()-RICHDB::rad_height),
    point;
  AMSDir dir(0.,0.,-1.);
  
  number theta,phi,length;

  track->interpolate(pnt,dir,point,
		     theta,phi,length);

  _current_tile=get_tile_number(point[0],point[1]);
  
  if(_current_tile<0){
    _p_direct=AMSPoint(0.,0.,0.);
    _p_reflected=AMSPoint(0.,0.,0.);
    _d_direct=AMSDir(0.,0.);
    _d_reflected=AMSDir(0.,0.);
    return;
  }

  pnt.setp(0.,0.,RICHDB::RICradpos()-RICHDB::rad_height+getheight());
  track->interpolate(pnt,dir,point,
		     theta,phi,
		     length);
  
  if(getkind()!=get_tile_kind(get_tile_number(point[0],point[1]))){
    _current_tile=-1;
    _p_direct=AMSPoint(0.,0.,0.);
    _p_reflected=AMSPoint(0.,0.,0.);
    _d_direct=AMSDir(0.,0.);
    _d_reflected=AMSDir(0.,0.);
    return;
  }
  
  _p_entrance=point;
  _d_entrance=AMSDir(theta,phi);


  // Direct photons
  pnt.setp(0.,0.,RICHDB::RICradpos()-RICHDB::rad_height+_tiles[_current_tile]->mean_height);
  track->interpolate(pnt,dir,point,theta,phi,length);
  
  _p_direct=point;
  _d_direct=AMSDir(theta,phi);
  

  // Direct photons
  pnt.setp(0.,0.,RICHDB::RICradpos()-RICHDB::rad_height+_tiles[_current_tile]->mean_height);
  track->interpolate(pnt,dir,point,theta,phi,length);
  
  _p_reflected=point;
  _d_reflected=AMSDir(theta,phi);
} 




void RichRadiatorTileManager::_compute_mean_height(geant *index,
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
    cout<<"RichRadiatorTileManager::_mean_height : Error"<<endl;
  }else{
    height=rheight-sum/densum;

    eff_index=sum_index/densum;


  }
} 





