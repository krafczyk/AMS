#include "richradid.h"
#include "timeid.h"

/////////////////////////////////////////////////

geant RichRadiatorTile::LocalIndex(geant dx,geant dy){
  // In case no fine mesh is available for that tile
  if(number_of_nodes==0) return index;

  // Use the General RICH frame of reference
  dx+=position[0];
  dy+=position[1];
  float best_index=0;
  float best_distance=1e6;
  for(int i=0;i<number_of_nodes;i++){
    float d=fabs(dx-node_x[i])+fabs(dy-node_y[i]);
    if(d<best_distance){
      best_index=node_index[i];
      best_distance=d;
    }
  }

  return index+best_index;
}



/////////////////////////////////////////////////

bool    RichRadiatorTileManager::_IgnoreDB=false;
integer RichRadiatorTileManager::_number_of_rad_tiles=0;
RichRadiatorTile **RichRadiatorTileManager::_tiles=0;

void RichRadiatorTileManager::Init(){  // Default initialization
  if(_number_of_rad_tiles!=0) return; // Not necessary
//#pragma omp barrier 
//#pragma omp master
  {
    //  cout<<"RichRadiatorTileManager::Init-default radiator"<<endl;
    Init_Default();
    
    // Read in database entries if needed
    char filename[201]="";
#ifndef __ROOTSHAREDLIBRARY__
    UHTOC(RICRADSETUPFFKEY.tables_in,50,filename,200);
    
    for(int i=200;i>=0;i--){
      if(filename[i]!=' '){
	filename[i+1]=0;
	break;
      }
    } 
    if(filename[0]!='\0'){
      RichRadiatorTileManager::_IgnoreDB=true;
      ReadFromFile(filename);
    }else
#endif    
    {
      char name[801];
      //      sprintf(name,"%s/%s/RichDefaultAGLTables.dat",getenv("AMSDataDir"),AMSCommonsI::getversion());
      //      sprintf(name,"%s/%s/RichDefaultAGLTables.02.dat",getenv("AMSDataDir"),AMSCommonsI::getversion());
      //      sprintf(name,"%s/%s/RichDefaultAGLTables.03.dat",getenv("AMSDataDir"),AMSCommonsI::getversion());  // New very careful calibration
      sprintf(name,"%s/%s/RichDefaultAGLTables.04.dat",getenv("AMSDataDir"),AMSCommonsI::getversion());  // Refined calibration
      ReadFromFile(name);
    }
    
    // Read fine mesh data 
#ifndef __ROOTSHAREDLIBRARY__
    UHTOC(RICRADSETUPFFKEY.finemesh_in,50,filename,200);
    
    for(int i=200;i>=0;i--){
      if(filename[i]!=' '){
	filename[i+1]=0;
	break;
      }
    } 
    if((RICRADSETUPFFKEY.setup%10)==1)
#endif
    {
      if(filename[0]!='\0') ReadFineMeshFromFile(filename);
      else{
	char name[801];
	sprintf(name,"%s/%s/RichDefaultAGLFineMeshTables.dat",getenv("AMSDataDir"),AMSCommonsI::getversion());
	ReadFineMeshFromFile(name);
      }
    }
    
    
    // Compute tables
    _compute_tables();  

    // Fill TDV tables, just in case
    DumpToTDV();
  }
//#pragma omp barrier 
}


void RichRadiatorTileManager::Init_Default(){  // Default initialization
  const int agl_boxes_number=11;
  int agl_boxes[agl_boxes_number][agl_boxes_number]={
    0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1,
    1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1,
    1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0
  };

  const int naf_boxes_number=4;
  int naf_boxes[naf_boxes_number][naf_boxes_number]={
    1,1,1,1,
    1,1,1,1,
    1,1,1,1,
    1,1,1,1
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
 
  cout<<"RichRadiatorTileManager::Init_Default-number of radiator tiles: "<<_number_of_rad_tiles<<endl;

  _tiles=new RichRadiatorTile*[_number_of_rad_tiles];

  RICHDB::mat_init();  // Ensure that the agl tables are updated.

  //
  // Fill the information for agl
  //
  int current=0;
  for(int i=0;i<agl_boxes_number;i++)
    for(int j=0;j<agl_boxes_number;j++){
      if(agl_boxes[i][j]!=1) continue;
      double x=(i+0.5*(1-agl_boxes_number))*(RICHDB::rad_length+RICaethk);
      double y=(j+0.5*(1-agl_boxes_number))*(RICHDB::rad_length+RICaethk);
      _tiles[current]=new RichRadiatorTile;
      _tiles[current]->position[0]=x;
      _tiles[current]->position[1]=y;
      _tiles[current]->bounding_box[0][0]=-RICHDB::rad_length/2;
      _tiles[current]->bounding_box[0][1]=RICHDB::rad_length/2;
      _tiles[current]->bounding_box[1][0]=-RICHDB::rad_length/2;
      _tiles[current]->bounding_box[1][1]=RICHDB::rad_length/2;
      _tiles[current]->bounding_box[2][0]=-RICHDB::rad_agl_height/2;
      _tiles[current]->bounding_box[2][1]=RICHDB::rad_agl_height/2;
      _tiles[current]->kind=agl_kind;
      _tiles[current]->id=current;
      _tiles[current]->clarity=RICHDB::eff_rad_clarity;
      _tiles[current]->effective_scattering_probability=0;
      _tiles[current]->effective_scattering_angle=0;
      _tiles[current]->abs_length=RICHDB::abs_length;
      _tiles[current]->index=RICHDB::rad_index;
      current++;
    }

  //
  // Fill the information for naf
  //
  for(int i=0;i<naf_boxes_number;i++)
    for(int j=0;j<naf_boxes_number;j++){
      if(naf_boxes[i][j]!=1) continue;
      double x=(i+0.5*(1-naf_boxes_number))*(RICHDB::naf_length+RICaethk);
      double y=(j+0.5*(1-naf_boxes_number))*(RICHDB::naf_length+RICaethk);
      _tiles[current]=new RichRadiatorTile;
      _tiles[current]->position[0]=x;
      _tiles[current]->position[1]=y;
      _tiles[current]->bounding_box[0][0]=-RICHDB::naf_length/2;
      _tiles[current]->bounding_box[0][1]=RICHDB::naf_length/2;
      _tiles[current]->bounding_box[1][0]=-RICHDB::naf_length/2;
      _tiles[current]->bounding_box[1][1]=RICHDB::naf_length/2;
      _tiles[current]->bounding_box[2][0]=-RICHDB::naf_height/2;
      _tiles[current]->bounding_box[2][1]=RICHDB::naf_height/2;
      _tiles[current]->kind=naf_kind;
      _tiles[current]->id=current;
      _tiles[current]->clarity=0;
      _tiles[current]->effective_scattering_probability=0;
      _tiles[current]->effective_scattering_angle=0;
      _tiles[current]->abs_length=RICHDB::naf_abs_length;
      _tiles[current]->index=RICHDB::naf_index;

      current++;
    }



  //
  // Fill static tables
  //

  /*
  // Center position wrt default value
  _alignment[0]=0;
  _alignment[1]=0;
  _alignment[2]=0;
  // X axis
  _alignment[3]=1;
  _alignment[4]=0;
  _alignment[5]=0;
  // Y axis
  _alignment[6]=0;
  _alignment[7]=1;
  _alignment[8]=0;
  // Z axis
  _alignment[9]= 0;
  _alignment[10]=0;
  _alignment[11]=1;

  for(int i=0;i<_number_of_rad_tiles;i++){
    RichRadiatorTile &tile=*_tiles[i];
    int id=tile.id;
    _kind[id]=tile.kind;
    _position[id*5+0]=tile.position[0];
    _position[id*5+1]=tile.position[1];
    _position[id*5+2]=fabs(tile.bounding_box[0][1]);
    _position[id*5+3]=fabs(tile.bounding_box[1][1]);
    _position[id*5+4]=fabs(tile.bounding_box[2][1]);
    _optical_parameters[id*4+0]=tile.index;
    _optical_parameters[id*4+1]=tile.clarity;
    _optical_parameters[id*4+2]=tile.effective_scattering_probability;
    _optical_parameters[id*4+3]=tile.effective_scattering_angle;
  }
  */
}

void RichRadiatorTileManager::_compute_tables(){
  for(int current=0;current<_number_of_rad_tiles;current++)     recompute_tables(current,_tiles[current]->index);
}


#define max(x,y) ((x)<(y)?(y):(x))
#define min(x,y) ((x)<(y)?(x):(y))




void RichRadiatorTileManager::Finish_Default(){
  cout<<"RichTileManager finishing"<<endl;
#ifndef __ROOTSHAREDLIBRARY__
    char filename[201] = "";
    UHTOC(RICRADSETUPFFKEY.tables_out,50,filename,200);
    for(int i=200;i>=0;i--){
      if(filename[i]!=' '){
	filename[i+1]=0;
	break;
      }
    } 
    
    if(filename[0]=='\0') return;

#pragma omp single
    {    
      cout<<"RichRadiatorTileManager writing constants to file "<<filename<<endl;
      fstream data(filename,ios::out); // open  file for reading
      for(int i=0;i<_number_of_rad_tiles;i++){
	//	data<<i<<" "<<_tiles[i]->index<<" "<<_tiles[i]->clarity<<endl;

	data<<_tiles[i]->position[0]<<" "
	    <<_tiles[i]->position[1]<<" "
	    <<_tiles[i]->index<<" "	    
	    <<_tiles[i]->bounding_box[2][1]-_tiles[i]->bounding_box[2][0]<<" "
	    <<_tiles[i]->clarity<<endl;
	
      }
      data.close();
    }
#endif  
}


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
  AMSPoint pnt(0.,0.,RICHDB::RICradpos()-RICHDB::rad_height),point;
  AMSDir   dir(0.,0.,-1.);
  number theta,phi,length;
  
  // Transform RICH plane to AMS plane 
  pnt=RichAlignment::RichToAMS(pnt);
  dir=RichAlignment::RichToAMS(dir);

  // Get the track interpolation
  track->interpolate(pnt,dir,point,
		     theta,phi,length);


  // Transform the point to RICH frame again to get X and Y
  point=RichAlignment::AMSToRich(point);
  
  _current_tile=get_tile_number(point[0],point[1]);

  if(_current_tile<0){
    _p_direct=AMSPoint(0.,0.,0.);
    _p_reflected=AMSPoint(0.,0.,0.);
    _d_direct=AMSDir(0.,0.);
    _d_reflected=AMSDir(0.,0.);
    return;
  }

  // Use the mean position for the direct photons to estimate the
  // local mean index
  double dx=point[0]-_tiles[_current_tile]->position[0];
  double dy=point[1]-_tiles[_current_tile]->position[1];

  // Compute the distance to the tile border
  _distance2border=fmin(fabs(get_tile_boundingbox(_current_tile,0)-fabs(dx)),
			fabs(get_tile_boundingbox(_current_tile,1)-fabs(dy)));

  pnt.setp(0.,0.,RICHDB::RICradpos()-RICHDB::rad_height+getheight());

  // Transform to AMS
  pnt=RichAlignment::RichToAMS(pnt);

  track->interpolate(pnt,dir,point,
		     theta,phi,
		     length);

  // Transform the point to RICH frame again to get X and Y
  point=RichAlignment::AMSToRich(point);
  
  if(_current_tile!=get_tile_number(point[0],point[1])){
    _current_tile=-1;
    _p_direct=AMSPoint(0.,0.,0.);
    _p_reflected=AMSPoint(0.,0.,0.);
    _d_direct=AMSDir(0.,0.);
    _d_reflected=AMSDir(0.,0.);
    return;
  }
  
  _p_entrance=point;
  AMSDir d(theta,phi);
  _d_entrance=RichAlignment::AMSToRich(d);


  // Direct photons
  pnt.setp(0.,0.,RICHDB::RICradpos()-RICHDB::rad_height+_tiles[_current_tile]->mean_height);
  pnt=RichAlignment::RichToAMS(pnt);
  track->interpolate(pnt,dir,point,theta,phi,length);
  
  _p_direct=RichAlignment::AMSToRich(point);
  d=AMSDir(theta,phi);
  _d_direct=RichAlignment::AMSToRich(d);
  
  // Reflected photons
  pnt.setp(0.,0.,RICHDB::RICradpos()-RICHDB::rad_height+_tiles[_current_tile]->mean_height);
  pnt=RichAlignment::RichToAMS(pnt);
  track->interpolate(pnt,dir,point,theta,phi,length);
  
  _p_reflected=RichAlignment::AMSToRich(point);
  d=AMSDir(theta,phi);
  _d_reflected=RichAlignment::AMSToRich(d);
  

  if(_tiles[_current_tile]->kind==naf_kind) _local_index=_tiles[_current_tile]->mean_refractive_index;
  else _local_index=1+(_tiles[_current_tile]->mean_refractive_index-1)*
    (_tiles[_current_tile]->LocalIndex(dx,dy)-1)/
    (_tiles[_current_tile]->index-1);

  /*******************************************
#ifdef __G4AMS__
#warning Temporal patch for the output refractive index in the G4 MC
  if(AMSJob::gethead()->isSimulation() && _tiles[_current_tile]->kind!=naf_kind){
    // The tables for the different indexes for each tile is not yet implemented
    // in the G4 version, therefore we should return a single index, as set in gmat
    static int first=1;
    if(first){
      first=0;
      cout<<"RichRadiatorTileManager::RichRadiatorTileManager -- assuming a single refractive index for all AGL tiles for G4 MC reconstruction"<<endl;
    }
    _local_index=RICHDB::rad_index;
  }
#endif
  **************************************************/

  // Force the propagation direction to be downwards
  if(_d_direct[2]>0) _d_direct=_d_direct*(-1);
  if(_d_reflected[2]>0) _d_reflected=_d_reflected*(-1);


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
    cerr<<"RichRadiatorTileManager::_mean_height : Error"<<endl;
  }else{
    height=rheight-sum/densum;
    eff_index=sum_index/densum;
  }
} 


geant RichRadiatorTileManager::get_refractive_index(geant x,geant y,geant wavelength){
  // Correct the alignment
  AMSPoint org(x,y,0);
  AMSPoint local=RichAlignment::AMSToRich(org);
  x=org[0]; y=org[1];

  int tile_number=get_tile_number(x,y);

  if(tile_number<0){
#ifdef __AMSDEBUG__
    cout<<"-- WARNING: RichRadiatorTileManager::get_refractive_index: out of tile"<<endl;
#endif
    return 0.0;  
  }
  
  // For using the future parameterization
  double dx=x-_tiles[tile_number]->position[0];
  double dy=y-_tiles[tile_number]->position[1];
  

  // Get the wavelength bin using binary search
  int wl_bin=RICHDB::get_wavelength_bin(wavelength);

  // Use a linear interpolation to get the refractive index
  double ratio=(wavelength-RICHDB::wave_length[wl_bin])/(RICHDB::wave_length[wl_bin+1]-RICHDB::wave_length[wl_bin]);
  geant index=ratio*_tiles[tile_number]->index_table[wl_bin+1]+(1-ratio)*_tiles[tile_number]->index_table[wl_bin];

  // Correction taking into account the local refractive index
  geant local_index=_tiles[tile_number]->LocalIndex(dx,dy);
  geant final_index=_tiles[tile_number]->kind==naf_kind?index:1+(index-1)*(local_index-1)/(_tiles[tile_number]->index-1);

  // Return the computed index
  return final_index;
}


//geant   RichRadiatorTileManager::_alignment[9+3];                    // Alignment parameters (unused) position (3)+orientation(9)
//integer RichRadiatorTileManager::_kind[RICmaxtiles];                 // Kind of material
//geant   RichRadiatorTileManager::_position[RICmaxtiles*5];           // X and Y position + bounding box (XYZ Half length)
geant   RichRadiatorTileManager::_optical_parameters[RICmaxtiles*4]; // Mean index, clarity, scattering prob, scatterin angle



void RichRadiatorTileManager::DumpToTDV(){
  if(_number_of_rad_tiles>RICmaxtiles){
    cerr<<"RichRadiatorTileManager::DumpToTDV--too many tiles"<<endl;
    exit(1);
  }

  // Fill with zeroes
  const int n_=4;  // Number of stored parameters
  memset(_optical_parameters,0,sizeof(_optical_parameters[0])*RICmaxtiles*n_);
  
 
  // Fill the arrays 
  for(int i=0;i<_number_of_rad_tiles;i++){
    _optical_parameters[i*n_+0]=_tiles[i]->index;
    _optical_parameters[i*n_+1]=_tiles[i]->clarity;
  }


  // Update all DB
  //  UpdateDB(AMSID("RichRadTileParameters",AMSJob::gethead()->isRealData()));
}

time_t RichRadiatorTileManager::_parameters_begin,
        RichRadiatorTileManager::_parameters_insert,
        RichRadiatorTileManager::_parameters_end;


void RichRadiatorTileManager::GetFromTDV(){
  if(RichRadiatorTileManager::_IgnoreDB) return;
  // Go one by one with the current TDV, compare with current values used,
  // and if it is new, load and update
  //
  // Deal with parameters
  //

  const int n_=4;  // Number of stored parameters
  for(int i=0;i<_number_of_rad_tiles;i++){
    if(_tiles[i]->index==_optical_parameters[i*n_+0] &&
       _tiles[i]->clarity==_optical_parameters[i*n_+1]) continue;
    _tiles[i]->index=_optical_parameters[i*n_+0];
    _tiles[i]->clarity=_optical_parameters[i*n_+1];
    recompute_tables(i,_tiles[i]->index);
    // Update the information fo the NpExp computation 
#ifndef __ROOTSHAREDLIBRARY__
    AMSRichRing::_first_radiator_call[i]=1;
#endif
  }

}


void RichRadiatorTileManager::UpdateDB(AMSID my_id){
#ifndef __ROOTSHAREDLIBRARY__
  AMSTimeID *ptdv=AMSJob::gethead()->gettimestructure(my_id);
  ptdv->UpdateMe()=1;
#endif
}




/////////////////////////////////////////////////////////////////////////////////
// FORTRAN INTERFACE FOR USING NON UNIFORM REFRACTIVE INDEXES IN MC SIMULATION //
/////////////////////////////////////////////////////////////////////////////////


//
// Here we define a function to get the refractive index with a fortran call giving the position
//

// IMPORTANT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// We assume that in the MC we are not aligment 
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

 
extern "C" geant rrtmindex_(geant *x,geant *y,geant *wvl){
  return  RichRadiatorTileManager::get_refractive_index(*x,*y,*wvl);
}


//
// Get the number of Cherenkov photons. Prepare integration table
//

static double _cherenkov_integral[RICmaxentries];
static double _cherenkov_index[RICmaxentries];
static int _cherenkov_border;
#ifdef _OPENMP
#pragma omp threadprivate (_cherenkov_integral,_cherenkov_index, _cherenkov_border)
#endif

extern "C" geant getphotons_(geant *charge,geant *min_index,geant *vect,geant *step){
  const geant rfact=369.81E9;
  const geant cvpwl=1.2398E-6;  //m*eV=nm*GeV

  if(*charge==0) return 0;  // Simple test

  //
  // Get the refractive index in the xtremes
  //

  double mean=0;
  _cherenkov_integral[0]=0;
  _cherenkov_border=1;
  for(int i=1;i<RICmaxentries;i++){
    //
    // Get the momentum range
    //
    geant initial_momentum=cvpwl/(RICHDB::wave_length[i-1]);
    geant final_momentum=cvpwl/(RICHDB::wave_length[i]);

    //
    // Get the mean wavelength
    //
    double wavelength=0.5*(RICHDB::wave_length[i-1]+RICHDB::wave_length[i]);

    //
    // Get the refractive indexes in the middle of the step
    //
    geant index=RichRadiatorTileManager::get_refractive_index(vect[0]-0.5*(*step)*vect[3],vect[1]-0.5*(*step)*vect[4],wavelength);

    if(index==0.0) return 0;

    //
    // Store the current best limit
    // 
    
    _cherenkov_border=i+1;
    if(index<*min_index) break;

    
    //
    // Compute the first integral part
    //

    _cherenkov_integral[i]=_cherenkov_integral[i-1]+(final_momentum-initial_momentum)*
      (1-(*min_index)*(*min_index)/(index*index));

    mean=_cherenkov_integral[i];
  }

  if(mean==0) return 0; // If no photons, skip further computation

  //
  // Fill tables to sample from them
  //
  for(int i=0;i<RICmaxentries;i++){
    //
    // Prepare table of integrals
    //
    _cherenkov_integral[i]/=mean;

    //
    // Prepare table of indexes
    //
    double wavelength=RICHDB::wave_length[i];
    _cherenkov_index[i]=RichRadiatorTileManager::get_refractive_index(vect[0]-0.5*(*step)*vect[3],vect[1]-0.5*(*step)*vect[4],wavelength);
  }

  mean*=(*charge)*(*charge)*rfact*(*step); 
  return mean;
}


//
// Get the photon momentum and corresponding refractive index
//
extern "C" geant getmomentum_(geant *index){
  const geant cvpwl=1.2398E-6;  //m*eV=nm*GeV
  int dummy=0; 
#ifndef __ROOTSHAREDLIBRARY__
  geant integral=RNDM(dummy);
#else
  geant integral=0;
#endif
  
  //
  // Use binary search to find the photon
  int bin_max=_cherenkov_border-1;
  int bin_min=0;

  for(;;){
    int bin_med=(bin_max+bin_min)/2;
    if(integral<_cherenkov_integral[bin_med]) bin_max=bin_med; else bin_min=bin_med;
    if(bin_max-bin_min<=1) break;
  }

  //
  // Get the linear interpolation wavelength
  //
  geant ratio=(integral-_cherenkov_integral[bin_min])/
    (_cherenkov_integral[bin_max]-_cherenkov_integral[bin_min]);
  geant wl=RICHDB::wave_length[bin_min]*(1-ratio)+RICHDB::wave_length[bin_max]*ratio;

  geant momentum=cvpwl/wl;
  
  //
  // Get the linear interpolation index
  // 
  *index=_cherenkov_index[bin_min]*(1-ratio)+_cherenkov_index[bin_max]*ratio;

  // This a test to look for problems
  if(*index<1 || *index>2){
    cerr<<"getmomentum_-F-BIG PROBLEM AT getphoton_@richradid.C "<<bin_min<<" "<<bin_max<<" "<<ratio<< " "<<*index <<endl;
    cerr<<endl<<"DUMPING TABLE "<<endl;
    for(int i=0;i<_cherenkov_border-1;i++)
      cerr<<"WV "<<i<<" "<<integral<<" "<<RICHDB::wave_length[i]<<" INDEX "<<_cherenkov_index[i]<<" INTEGRAL "<<_cherenkov_integral[i]<<endl;
       abort();
      return 0;
  }

  return momentum;
}


//////////////////// END OF MC INTERFACE


//////// Read Radiator tile constants from file

void RichRadiatorTileManager::ReadFromFile(const char *filename){
  cout<<"RichRadiatorTileManager::ReadFromFile: reading constants from file"<<endl;
  fstream data(filename,ios::in); // open  file for reading
  
  if(data){
    cout<<"RichRadiatorTileManager::ReadFromFile: "<<filename<<" Opened"<<endl;
  }else{
    cerr <<"RichRadiatorTileManager::ReadFromFile: missing "<<filename<<endl;
    exit(1);
  }

  // Code to read all the stuff
  float x,y,index,thickness,clarity;
  while(!data.eof()){
    //    data>> x >> y >> index;
    data>>x>>y>>index>>thickness>>clarity;


    // Get the tile at the position x,y and 
    int tile_number=get_tile_number(x,y);

    if(tile_number<0){
      //      cout<<"RichRadiatorTileManager::ReadFromFile: Tile at "<<x<<" "<<y<<" is not defined"<<endl;
      continue;
    }
    
    float registered_index=_tiles[tile_number]->index;

    if(registered_index==0 || index==0 || fabs(1-index/registered_index)>1e-2){
      cout<<"RichRadiatorTileManager::ReadFromFile: Tile index at "<<x<<" "<<y<<" is not correct"<<endl;
      continue;
    }

#ifdef __AMSDEBUG__
    cout<<"   Filling "<<tile_number<<" with "<<index<<endl;
#endif

    _tiles[tile_number]->index=index;
    //    _tiles[tile_number]->bounding_box[2][0]=-thickness/2;
    //    _tiles[tile_number]->bounding_box[2][1]=thickness/2;
    _tiles[tile_number]->clarity=clarity;

  }


  data.close();
}


void RichRadiatorTileManager::ReadFineMeshFromFile(const char *filename){
  cout<<"RichRadiatorTileManager::ReadFineMeshFromFile: reading constants from file"<<endl;
  fstream data(filename,ios::in); // open  file for reading
  
  if(data){
    cout<<"RichRadiatorTileManager::ReadFineMeshFromFile: "<<filename<<" Opened"<<endl;
  }else{
    cerr <<"RichRadiatorTileManager::ReadFineMeshFromFile: missing "<<filename<<endl;
    exit(1);
  }

  // First count how many nodes we have
  float x,y,index;
  while(!data.eof()){
    data>> x >> y >> index;

    // Get the tile at the position x,y and 
    int tile_number=get_tile_number(x,y);

    if(tile_number<0){
      //      cout<<"RichRadiatorTileManager::ReadFineMeshFromFile: Tile at "<<x<<" "<<y<<" is not defined"<<endl;
      continue;
    }
    
    _tiles[tile_number]->number_of_nodes++;
  }

  // Reserve the required heap space 
  for(int i=0;i<_number_of_rad_tiles;i++){
    if(!_tiles[i]->number_of_nodes) continue;
    _tiles[i]->node_x=new float[_tiles[i]->number_of_nodes];
    _tiles[i]->node_y=new float[_tiles[i]->number_of_nodes];
    _tiles[i]->node_index=new float[_tiles[i]->number_of_nodes];
    _tiles[i]->number_of_nodes=0; // Reset it to use it as a counter
    _tiles[i]->index_bias=0;
  }

  data.close();

  fstream data_(filename,ios::in); // open  file for reading
  // Code to read all the stuff
  while(!data_.eof()){
    data_>> x >> y >> index;

    // Get the tile at the position x,y and 
    int tile_number=get_tile_number(x,y);

    if(tile_number<0){
      continue;
    }
    else if(tile_number>=_number_of_rad_tiles){
      cerr<<"  RichRadiatorTileManager::ReadFineMeshFromFile-E-WrongTileIndexIgnored "<<tile_number<<" "<<_number_of_rad_tiles<<endl;
      continue;
    }    
    _tiles[tile_number]->node_x[_tiles[tile_number]->number_of_nodes]=x;
    _tiles[tile_number]->node_y[_tiles[tile_number]->number_of_nodes]=y;
    _tiles[tile_number]->node_index[_tiles[tile_number]->number_of_nodes++]=index;
    _tiles[tile_number]->index_bias+=index;

#ifdef __AMSDEBUG__
    cout<<"Updating tile "<<tile_number<<" with "<< _tiles[tile_number]->number_of_nodes<<endl; 
#endif

  }


  data_.close();

  // Compute the bias for the npexp computation
  for(int i=0;i<_number_of_rad_tiles;i++){
    if(_tiles[i]->number_of_nodes==0) continue;
    _tiles[i]->index_bias/=_tiles[i]->number_of_nodes;
  }


#ifdef __AMSDEBUG__
  // Check tha the mean umbers are OK
  for(int i=0;i<_number_of_rad_tiles;i++){
    if(_tiles[i]->number_of_nodes==0){
      cout<<"Tile "<<i<<" had no nodes "<<endl;
    }else{
      if(!_tiles[i]->node_x){
	cout<<"Allocation error with tile "<<i<<endl;
      }else{
	double sum=0;
	for(int j=0;j<_tiles[i]->number_of_nodes;j++) sum+=_tiles[i]->node_index[j];
	cout<<"Tile "<<i<<" Mean index "<<sum/_tiles[i]->number_of_nodes<<" Coarse index "<<_tiles[i]->index<<endl;
	if(fabs(sum/_tiles[i]->number_of_nodes-_tiles[i]->index)>1e-3)
	  cout<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"<<endl<<endl;
      }
    }
  }
#endif

}




extern "C" void filltable_(geant *x,geant *y){
  RichRadiatorTileManager::UpdateOpticalParametersTable(*x,*y);  
}


void RichRadiatorTileManager::UpdateOpticalParametersTable(float x,float y){
  AMSPoint org(x,y,0);
  AMSPoint local=RichAlignment::AMSToRich(org);
  x=org[0]; y=org[1];

  int tile_number=get_tile_number(x,y);
  if(tile_number<0){
    // Fill the default values
    RICGTKOV.usrcla=RICHDB::rad_clarity;
  }else{  
    RICGTKOV.usrcla=_tiles[tile_number]->clarity;
  }
}


void RichRadiatorTileManager::recompute_tables(int current,double my_index){  // Recompute the index for a given tile, assuming a new refractive index 
  geant eff_index;
  geant eff_height;



  if(current<0 || current>=_number_of_rad_tiles) return;

  if(my_index<=1.0) return;

  double new_index=my_index+_tiles[current]->index_bias;

  for(int ii=0;ii<RICmaxentries;ii++){
    if(_tiles[current]->kind==agl_kind){
      _tiles[current]->index_table[ii]=1+(new_index-1)/(RICHDB::rad_index-1)*(RICHDB::index[ii]-1);
    }else if(_tiles[current]->kind==naf_kind){
      _tiles[current]->index_table[ii]=RICHDB::naf_index_table[ii];
    }
    
  }
  // Now we compute the effective height and effective index calling the dedicated routine
  
  _compute_mean_height(_tiles[current]->index_table,
		       _tiles[current]->clarity,
		       _tiles[current]->abs_length,
		       _tiles[current]->bounding_box[2][1]-_tiles[current]->bounding_box[2][0],
		       eff_index,
		       eff_height);
  
  if(_tiles[current]->kind==agl_kind){
    _tiles[current]->mean_refractive_index=eff_index;
    _tiles[current]->mean_height=eff_height;
  }else if(_tiles[current]->kind==naf_kind){
    _tiles[current]->mean_refractive_index=eff_index;
    _tiles[current]->mean_height=eff_height;
  }

  // Update the index
  _tiles[current]->index=my_index;
  
}


void RichRadiatorTileManager::Finish(){
  Finish_Default();
#ifndef __ROOTSHAREDLIBRARY__
  if((RICDBFFKEY.dump%10)==0) return;
  if(AMSFFKEY.Update==0) return;

  AMSTimeID *ptdv;
  time_t begin_time,end_time,insert_time;
  const int ntdv=1;
  const char* TDV2Update[ntdv]={"RichRadTilesParameters"};
  
  tm begin;
  tm end;
  begin.tm_isdst=0;
  end.tm_isdst=0;
  
  begin.tm_sec=RICDBFFKEY.sec[0];
  begin.tm_min=RICDBFFKEY.min[0];
  begin.tm_hour=RICDBFFKEY.hour[0];
  begin.tm_mday=RICDBFFKEY.day[0];
  begin.tm_mon=RICDBFFKEY.mon[0];
  begin.tm_year=RICDBFFKEY.year[0];
  
  
  end.tm_sec=RICDBFFKEY.sec[1];
  end.tm_min=RICDBFFKEY.min[1];
  end.tm_hour=RICDBFFKEY.hour[1];
  end.tm_mday=RICDBFFKEY.day[1];
  end.tm_mon=RICDBFFKEY.mon[1];
  end.tm_year=RICDBFFKEY.year[1];

  begin_time=mktime(&begin);
  end_time=mktime(&end);
  time(&insert_time);


  for (int i=0;i<ntdv;i++){
    ptdv = AMSJob::gethead()->gettimestructure(AMSID(TDV2Update[i],AMSJob::gethead()->isRealData()));
    ptdv->UpdateMe()=1;
    ptdv->UpdCRC();
    ptdv->SetTime(insert_time,begin_time,end_time);
    cout <<" RICH info has been updated for "<<*ptdv<<endl;
    ptdv->gettime(insert_time,begin_time,end_time);
    cout <<" Time Insert "<<ctime(&insert_time)<<endl;
    cout <<" Time Begin "<<ctime(&begin_time)<<endl;
    cout <<" Time End "<<ctime(&end_time)<<endl;
  }
#endif
}

