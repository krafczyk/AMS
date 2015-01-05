//
#include "TrdMTrack.h"

ClassImp(TrdMTrack);


TrdKCalib *TrdMTrack::trdk_db = NULL;
TrdKPDF *TrdMTrack::Epdf=NULL;
TrdKPDF *TrdMTrack::Ppdf=NULL;

int TrdMTrack::nbins=30;
float TrdMTrack::xbins[31]={1., 1.24, 1.53, 1.90, 2.35, 2.90, 3.59, 4.45, 5.51, 6.81, 8.43, 10.44, 12.92, 15.99, 19.79, 24.49, 30.32, 37.52,  46.44, 57.48, 71.14, 88.05, 108.97, 134.87, 166.92, 206.60, 255.70, 316.47, 391.69, 484.78, 600.};

/////////////////////////////////////////////////////////////////////

TrdMTrack::TrdMTrack(){

  TrdGainType=1;
  TrdPdfType=1;
  TrdAlignType=2;

  cout << "TrdMTrack constructor: TrdGainType " << TrdGainType << " TrdAlignType " << TrdAlignType << " TrdPdfType " << TrdPdfType <<  " is being used!" << endl;
 
   
  Init_Base();
  
  isprocessed=false;
  
    return;
};

/////////////////////////////////////////////////////////////////////

TrdMTrack::TrdMTrack(int gaintype, int aligntype, int pdftype){

  if(gaintype<-1 || gaintype>1)  {cout << "TrdMTrack constructor: Invalid GainType option" << endl; return;}
  if(pdftype<0 || pdftype>1) {cout << "TrdMTrack constructor: Invalid PDFType option" << endl; return;}
  if(aligntype<-1 || aligntype>2) {cout << "TrdMTrack constructor: Invalid AlignType option" << endl; return;}

  TrdGainType=gaintype;
  TrdPdfType=pdftype;
  TrdAlignType=aligntype;

  cout << "TrdMTrack constructor: TrdGainType " << TrdGainType << " TrdAlignType " << TrdAlignType << " TrdPdfType " << TrdPdfType <<  " is being used!" << endl;

  
  Init_Base();
 
  isprocessed=false;
  
    return;
};

/////////////////////////////////////////////////////////////////////

TrdMTrack::~TrdMTrack(){
  
  phitlen.clear();
  TrdMHits.clear(); 
  hitres.clear(); 
  xetime.clear();
  xe.clear();
  //xenon->Clear();
  //delete xenon;
  
  for(int i=0; i<328; i++){
    mod_time[i].clear();
    gaintime[i].clear();
    gain[i].clear();
    for(int j=0; j<4; j++) mod_corr[i][j].clear();
  }
  
  for(int i=0; i<6; i++){
    for( int j=0; j<20; j++) {ElPDF[i][j]->Clear(); delete ElPDF[i][j]; ElPDF[i][j]=0;}
    for( int j=0; j<30; j++) {ProPDF[i][j]->Clear(); delete ProPDF[i][j]; ProPDF[i][j]=0;}
  }
  
  delete _fit;
  if(trdk_db) delete trdk_db;
  delete Epdf;
  delete Ppdf;

};

/////////////////////////////////////////////////////////////////////

float TrdMTrack::GetLogLikelihood(AMSEventR *_evt, ParticleR *par){
 
  phitlen.clear();
  TrdMHits.clear();
  _fit->Clear();

  IsValid=false;

  evt=_evt;
  _Time=evt->UTime();

  if(TrdAlignType==2 && ((unsigned int)_Time> _calibtime+10000000 ||(unsigned int)_Time< _calibtime) ) update_alignment_vectors((unsigned long)_Time);
  // cout << "Utime: " << evt->UTime() <<endl ;

  if(_Time>=int(FirstRun) && _Time<=int(LastRun)) IsValid=true;
  else { TrdGainType=-1; TrdAlignType=-1; IsValid=true;}

  n_mhX=0;
  n_mhY=0;
 
  TrTrackR *_track=par->pTrTrack();
  if(!_track) return -1;
  _id=_track->iTrTrackPar(1,0,1);
  if(_id<0) return -1;
  _rig=_track->GetRigidity(_id);

  TrdTrackR *_trd=par->pTrdTrack();
  //if(!_trd) return -1;
  
  SetAlignment();
  if(!_trd) SetupMHTrack(_track);
  else SetupMHTrack(_track, _trd);
  MakeHitSelection();
  float lik=CalcLogLikelihood();
 
  isprocessed=true;
  _Time_previous=_Time;

  return lik;
  
}
//---------------------------------------------------------------------------- 

void TrdMTrack::SetupMHTrack(TrTrackR *track, TrdTrackR *trd){

  vector<AMSPoint> trhit;
  vector<AMSPoint> tr_err;
  vector<AMSPoint> trdtk_pntvec;
  vector<int>dirvec;
  vector<int>gaptype;
        
  int trd_coo[20][3];
  int tr_coo[20][3];
  for( int i=0; i<3; i++){
    for( int j=0; j<20; j++) {
      trd_coo[j][i]=0;
      tr_coo[j][i]=0;
    }
  }
  
  for(int i=0; i<trd->NTrdSegment(); i++){
    TrdSegmentR *seg= trd->pTrdSegment(i);
    if(seg==0) continue;
    for(int j=0; j<seg->NTrdCluster(); j++){
      TrdClusterR *clu=seg->pTrdCluster(j);
       if(clu==0) continue;
       TrdRawHitR *hit=clu->pTrdRawHit();
       if(hit==0) continue;
      
       if(hit->Amp > trd_coo[hit->Layer][0]){
	 trd_coo[hit->Layer][0]= hit->Amp;
	 trd_coo[hit->Layer][1]=hit->Ladder;
	 trd_coo[hit->Layer][2]=hit->Tube;
       }
    }
  }

  for(int i=0; i<evt->nTrdRawHit(); i++){
    TrdRawHitR* hit=evt->pTrdRawHit(i);
    
    if(!hit) continue;
    AMSPoint pnt;
    AMSDir dir;
    
    TRDHitRZD rzd(*hit);   
       
    int tubenumber=hit->Layer*224+hit->Ladder*16+hit->Tube;
    if(hit->Layer>4) tubenumber+=(hit->Layer-4)*32;
    if(hit->Layer>12)  tubenumber+=(hit->Layer-12)*32;
    int modnumber=hit->Layer*14+hit->Ladder;
    if(hit->Layer>4) modnumber+=(hit->Layer-4)*2;
    if(hit->Layer>12)  modnumber+=(hit->Layer-12)*2;		  
    
    AMSPoint TRDTube_Center_check;
    if(hit->Layer <4 || hit->Layer>15) TRDTube_Center_check.setp(xycorr[modnumber][0], xycorr[modnumber][2]+rzd.r, xycorr[modnumber][4]+rzd.z);
    else TRDTube_Center_check.setp(xycorr[modnumber][0]+rzd.r, xycorr[modnumber][2], xycorr[modnumber][4]+rzd.z);
    AMSDir TRDTube_Dir_check;
    if(hit->Layer <4 || hit->Layer>15) TRDTube_Dir_check.setd(1+xycorr[modnumber][1], xycorr[modnumber][3] ,xycorr[modnumber][5]);
    else TRDTube_Dir_check.setd(xycorr[modnumber][1],1+xycorr[modnumber][3],xycorr[modnumber][5]);
     
    track->Interpolate(TRDTube_Center_check.z(), pnt, dir, _id);
        
    AMSPoint check=pnt-TRDTube_Center_check;
    AMSPoint check2=dir.crossp(TRDTube_Dir_check);
    float abstand = (check.prod(check2))/check2.norm();
    
    if(fabs(abstand)<0.303 ) {
      tr_coo[hit->Layer][0]=hit->Amp;
      tr_coo[hit->Layer][1]=hit->Ladder;
      tr_coo[hit->Layer][2]=hit->Tube;
    }
  }
  
  AMSPoint trdtk_pnt;
  AMSDir trdtk_dir;
  
  for( int i=0; i<20; i++){
    if(trd_coo[i][0]==0 && tr_coo[i][0]==0 && i>0 && i<19 ){
      if((trd_coo[i-1][0]>0 || tr_coo[i-1][0]>0) && (trd_coo[i+1][0]>0 || tr_coo[i+1][0]>0)){
	
	float z=0.12+get_trd_rz(i-1,trd_coo[i-1][1] , 0, 1)+(get_trd_rz(i+1,trd_coo[i+1][1] , 0, 1)-get_trd_rz(i-1,trd_coo[i-1][1] , 0, 1))/2;
	
	interpolate_trd_track(trd,  trdtk_pnt, trdtk_dir, z);
	 
	if(i>3 && i<16) {
	  
	  int ladder = which_tube(i,trdtk_pnt.x(), 0);
	  int tube = which_tube(i,trdtk_pnt.x(), 1);
	  
	  int modnumber=i*14+ladder;
	  if(i>4) modnumber+=(i-4)*2;
	  if(i>12)  modnumber+=(i-12)*2;		  
	  
	  if(tube!=15){
	    
	    //	 printf("defining tube\n");
	    AMSPoint TRDTube_Center_1(get_trd_rz(i, ladder, tube,0),xycorr[modnumber][2],get_trd_rz(i, ladder, tube,1));
	    AMSDir TRDTube_Dir;
	    TRDTube_Dir.setd(xycorr[modnumber][1], 1+xycorr[modnumber][3] ,xycorr[modnumber][5]);
	    AMSPoint TRDTube_Center_2(get_trd_rz(i, ladder, tube+1,0),xycorr[modnumber][2],get_trd_rz(i, ladder, tube+1,1));
	    AMSPoint diff_0=TRDTube_Center_2-TRDTube_Center_1;
	    
	    AMSPoint TRDGap_0 = TRDTube_Center_1+(diff_0*0.5);
	    
	    track->Interpolate(TRDGap_0.z(), trdtk_pnt, trdtk_dir, _id);
	    
	    AMSPoint w0=trdtk_pnt-TRDGap_0;
	    
	    float a=1;
	    float b=trdtk_dir.prod(TRDTube_Dir);
	    float c=1;
	    float d=trdtk_dir.prod(w0);
	    float e=TRDTube_Dir.prod(w0);

	    float denorm=a*c-b*b;
	    if(denorm==0)cout<<"ERROR: Track Tube in Parrarel orientation"<<endl;
	    float tc=(a*e-b*d)/denorm;
	    
	    AMSPoint TubePos=TRDGap_0+TRDTube_Dir*tc;
	    
	    track->Interpolate(TubePos.z(), trdtk_pnt, trdtk_dir, _id);
	     
	    AMSPoint check=trdtk_pnt-TRDGap_0;
	    AMSPoint check2=trdtk_dir.crossp(TRDTube_Dir);
	    //	    float z_res=abstand*check2.z()/check2.norm();
	    
	    AMSDir zdir(0,0,1);
	    AMSDir anti=zdir.crossp(TRDTube_Dir);;
	    
	    if( trdtk_pnt.x()-TubePos.x()> 0.303 && tube!=14) {TubePos=TubePos+anti*(get_trd_rz(i, ladder, tube,0)-get_trd_rz(i, ladder, tube+1,0));tube+=1;}
	    else if(trdtk_pnt.x()-TubePos.x()<-0.303 && tube!=0){ TubePos=TubePos+anti*(get_trd_rz(i, ladder, tube,0)-get_trd_rz(i, ladder, tube-1,0));tube-=1;}
	    
	    if(fabs(trdtk_pnt.x()-TubePos.x())<0.303){
	      trdtk_pntvec.push_back(TubePos);
	      dirvec.push_back(i);
	      if(tube==0 || tube==3 || tube==6 || tube==8 || tube==11 || tube==14) gaptype.push_back(1);
	      else gaptype.push_back(0);
	      misshit=true;
	      n_mhX++;
	    }
	  }
	}

	else{
	  
	  int ladder = which_tube(i,trdtk_pnt.y(), 0);
	  int tube = which_tube(i,trdtk_pnt.y(), 1);
	  
	  int modnumber=i*14+ladder;
	  if(i>4) modnumber+=(i-4)*2;
	  if(i>12)  modnumber+=(i-12)*2;		  
	  
	  if(tube!=15){
	    
	    //	 printf("defining tube\n");
	    AMSPoint TRDTube_Center_1(xycorr[modnumber][0],get_trd_rz(i, ladder, tube,0),get_trd_rz(i, ladder, tube,1));
	    AMSDir TRDTube_Dir;
	    TRDTube_Dir.setd(1+xycorr[modnumber][1], xycorr[modnumber][3],xycorr[modnumber][5]);
	    AMSPoint TRDTube_Center_2(xycorr[modnumber][0], get_trd_rz(i, ladder, tube+1,0),get_trd_rz(i, ladder, tube+1,1));
	    AMSPoint diff_0=TRDTube_Center_2-TRDTube_Center_1;
	    
	    AMSPoint TRDGap_0 = TRDTube_Center_1+(diff_0*0.5);
	    
	    track->Interpolate(TRDGap_0.z(), trdtk_pnt, trdtk_dir, _id);
	    
	    AMSPoint w0=trdtk_pnt-TRDGap_0;
	    
	    float a=1;
	    float b=trdtk_dir.prod(TRDTube_Dir);
	    float c=1;
	    float d=trdtk_dir.prod(w0);
	    float e=TRDTube_Dir.prod(w0);
 
	    float denorm=a*c-b*b;
	    if(denorm==0)cout<<"ERROR: Track Tube in Parrarel orientation"<<endl;
        float tc=(a*e-b*d)/denorm;

	    AMSPoint TubePos=TRDGap_0+TRDTube_Dir*tc;
	    	     
	    track->Interpolate(TubePos.z(), trdtk_pnt, trdtk_dir, _id);
	    
	    AMSPoint check=trdtk_pnt-TRDGap_0;
	    AMSPoint check2=trdtk_dir.crossp(TRDTube_Dir);
	    // float z_res=abstand*check2.z()/check2.norm();
	    
	    AMSDir zdir(0,0,1);
	    AMSDir anti=zdir.crossp(TRDTube_Dir);
	    
	    if( trdtk_pnt.y()-TubePos.y()> 0.303 && tube!=14) {TubePos=TubePos+anti*(get_trd_rz(i, ladder, tube,0)-get_trd_rz(i, ladder, tube+1,0));tube+=1;}
	    else if(trdtk_pnt.y()-TubePos.y()<-0.303 && tube!=0) {TubePos=TubePos+anti*(get_trd_rz(i, ladder, tube,0)-get_trd_rz(i, ladder, tube-1,0));tube-=1;}
	    
	    if( fabs(trdtk_pnt.y()-TubePos.y())<0.303){
	      trdtk_pntvec.push_back(TubePos);
	      dirvec.push_back(i);
	      if(tube==0 || tube==3 || tube==6 || tube==8 || tube==11 || tube==14) gaptype.push_back(1);
	      else gaptype.push_back(0);
	      misshit=true;
	      n_mhY++;
	    }
	  }
	}
      }
    }

    else if( trd_coo[i][0]==0 && ((i==0 && (trd_coo[1][0]>0 || tr_coo[1][0]>0)) || (i==19 && (trd_coo[18][0]>0 || tr_coo[18][0]>0)))){
      float z;
      if(i==0) z=get_trd_rz(i+1,trd_coo[i+1][1] , 0, 1)-2.9;
      else z=get_trd_rz(i-1,trd_coo[i-1][1] , 0, 1)+2.9;
      
      interpolate_trd_track(trd,  trdtk_pnt, trdtk_dir, z);
      
      int ladder = which_tube(i,trdtk_pnt.y(), 0);
      int tube = which_tube(i,trdtk_pnt.y(), 1);
      
      int modnumber=i*14+ladder;
      if(i>4) modnumber+=(i-4)*2;
      if(i>12)  modnumber+=(i-12)*2;		  
     
      if(tube!=15){
	
	//	 printf("defining tube\n");
	AMSPoint TRDTube_Center_1(xycorr[modnumber][0],get_trd_rz(i, ladder, tube,0),get_trd_rz(i, ladder, tube,1));
	AMSDir TRDTube_Dir;
	TRDTube_Dir.setd(1+xycorr[modnumber][1], xycorr[modnumber][3],xycorr[modnumber][5]);
	AMSPoint TRDTube_Center_2(xycorr[modnumber][0], get_trd_rz(i, ladder, tube+1,0),get_trd_rz(i, ladder, tube+1,1));
	AMSPoint diff_0=TRDTube_Center_2-TRDTube_Center_1;
	
	AMSPoint TRDGap_0 = TRDTube_Center_1+(diff_0*0.5);
	
	track->Interpolate(TRDGap_0.z(), trdtk_pnt, trdtk_dir, _id);
	
	AMSPoint w0=trdtk_pnt-TRDGap_0;
	
	float a=1;
	float b=trdtk_dir.prod(TRDTube_Dir);
	float c=1;
	float d=trdtk_dir.prod(w0);
	float e=TRDTube_Dir.prod(w0);
	
	float denorm=a*c-b*b;
	if(denorm==0)cout<<"ERROR: Track Tube in Parrarel orientation"<<endl;
	float tc=(a*e-b*d)/denorm;
	
	AMSPoint TubePos=TRDGap_0+TRDTube_Dir*tc;
	
	track->Interpolate(TubePos.z(), trdtk_pnt, trdtk_dir, _id);
	
	AMSPoint check=trdtk_pnt-TRDGap_0;
	AMSPoint check2=trdtk_dir.crossp(TRDTube_Dir);
	//	float z_res=abstand*check2.z()/check2.norm();
	 
	AMSDir zdir(0,0,1);
	AMSDir anti=zdir.crossp(TRDTube_Dir);
	
	if( trdtk_pnt.y()-TubePos.y()> 0.303 && tube!=14) {TubePos=TubePos+anti*(get_trd_rz(i, ladder, tube,0)-get_trd_rz(i, ladder, tube+1,0));tube+=1;}
	else if(trdtk_pnt.y()-TubePos.y()<-0.303 && tube!=0) {TubePos=TubePos+anti*(get_trd_rz(i, ladder, tube,0)-get_trd_rz(i, ladder, tube-1,0));tube-=1;}
	
	if( fabs(trdtk_pnt.y()-TubePos.y())<0.303){
	  trdtk_pntvec.push_back(TubePos);
	  dirvec.push_back(i);
	  if(tube==0 || tube==3 || tube==6 || tube==8 || tube==11 || tube==14) gaptype.push_back(1);
	  else gaptype.push_back(0);
	  misshit=true;
	  n_mhY++;
	}
      }
    } 
  }

  

  if((int)trdtk_pntvec.size()>0){
  
    int tr_num=0;
    
     bool l1=false;	 
     AMSPoint trerr;
     
     for(int l=1; l<9; l++){
       trerr.setp(0.0020, 0.0013, 0.0200); 
       
       TrRecHitR *tr_hit=track->GetHitLJ(l); 
       
       if(tr_hit>0){
	 AMSPoint thit=tr_hit->GetCoord();
	 
	 if(!track->TestHitLayerJHasXY(l)) {
	   trerr.setp(0., 0.0013, 0.0200);
	   
	 }
	 //  else addhit=thit;;
	 if(l==1) l1=true;
	 if(l==2) trerr.setp(0.0100, 0.0013, 0.0200);
	 tr_num++;
	 trhit.push_back(thit);
	 tr_err.push_back(trerr); 
       }
     }
     float serr=0.0020, berr=0.0030;
     if(l1)  _fit->Add(trhit[0], tr_err[0]);
     //  printf("mh: pl1 hit added");
     for(unsigned int p=0; p<trdtk_pntvec.size(); p++){
       if(tr_num+p<20){
	 if(dirvec[(int)dirvec.size()-1-p]<4 || dirvec[(int)dirvec.size()-1-p]>15 ) {
	   AMSPoint test(0, trdtk_pntvec[(int)trdtk_pntvec.size()-1-p].y(), trdtk_pntvec[(int)trdtk_pntvec.size()-1-p].z() ); 
	   if(gaptype[(int)gaptype.size()-1-p]==0) _fit->Add(test, 0., serr, 0.020);
	   else if(gaptype[(int)gaptype.size()-1-p]==1) _fit->Add(test, 0., berr, 0.020);
	   // printf("mh: MH hit added");
	 }
	 else{
	   AMSPoint test(trdtk_pntvec[(int)trdtk_pntvec.size()-1-p].x(), 0, trdtk_pntvec[(int)trdtk_pntvec.size()-1-p].z()); 
	   if(gaptype[(int)gaptype.size()-1-p]==0) _fit->Add(test, serr, 0., 0.020);
	   else if(gaptype[(int)gaptype.size()-1-p]==1) _fit->Add(test, berr, 0., 0.020);
	   //  printf("mh: MH hit added");
	 }
       }	 
     }
     

     for(unsigned int l=0; l<trhit.size(); l++){
       if(!l1 && l==0) _fit->Add(trhit[l], tr_err[l]);
       if( l>0) _fit->Add(trhit[l], tr_err[l]);
       //  printf("mh: tracker hit added");
   
     }
     _fit->DoFit(4, 0, 0);

  }
  else{

    AMSPoint trerr;
     
    for(int l=1; l<9; l++){
      trerr.setp(0.0020, 0.0013, 0.0200); 
      
      TrRecHitR *tr_hit=track->GetHitLJ(l); 
      
      if(tr_hit>0){
	if(!track->TestHitLayerJHasXY(l)) {
	  trerr.setp(0.0, 0.0013, 0.0200);
	}
	
	trhit.push_back(tr_hit->GetCoord());
	tr_err.push_back(trerr); 
      }
    }
    for(unsigned int l=0; l<trhit.size(); l++){
      _fit->Add(trhit[l], tr_err[l]);
      // printf("no mh: tracker hit added");
       }
    
    _fit->DoFit(4, 0, 0);

  }
  gaptype.clear();
  dirvec.clear();
  trdtk_pntvec.clear();
  tr_err.clear();
  trhit.clear();

  return;
}
//---------------------------------------------------------------------------- 

void TrdMTrack::SetupMHTrack(TrTrackR *track){

  vector<AMSPoint> trhit;
  vector<AMSPoint> tr_err;

  AMSPoint trerr;
     
  for(int l=1; l<9; l++){
    trerr.setp(0.0020, 0.0013, 0.0200); 
    
    TrRecHitR *tr_hit=track->GetHitLJ(l); 
    
    if(tr_hit>0){
      if(!track->TestHitLayerJHasXY(l)) {
	trerr.setp(0.0, 0.0013, 0.0200);
      }
      
      trhit.push_back(tr_hit->GetCoord());
      tr_err.push_back(trerr); 
    }
  }
  for(unsigned int l=0; l<trhit.size(); l++){
    _fit->Add(trhit[l], tr_err[l]);
    // printf("no mh: tracker hit added");
    
  }
  
  _fit->DoFit(4, 0, 0);

  tr_err.clear();
  trhit.clear();
  
  return;
}

/////////////////////////////////////////////////////////////////////

void TrdMTrack::MakeHitSelection(){

  for(int i=0; i<evt->nTrdRawHit(); i++){
    TrdRawHitR* hit=evt->pTrdRawHit(i);
    if(!hit) continue;
    AMSPoint pnt;
    AMSDir dir;
    
    TRDHitRZD rzd(*hit);   
    
    int tubenumber=hit->Layer*224+hit->Ladder*16+hit->Tube;
    if(hit->Layer>4) tubenumber+=(hit->Layer-4)*32;
    if(hit->Layer>12)  tubenumber+=(hit->Layer-12)*32;
    int modnumber=hit->Layer*14+hit->Ladder;
    if(hit->Layer>4) modnumber+=(hit->Layer-4)*2;
    if(hit->Layer>12)  modnumber+=(hit->Layer-12)*2;		  
    
    AMSPoint TRDTube_Center_check;
    if(hit->Layer <4 || hit->Layer>15) TRDTube_Center_check.setp(xycorr[modnumber][0], xycorr[modnumber][2]+rzd.r, rzd.z+xycorr[modnumber][4]);
    else TRDTube_Center_check.setp(xycorr[modnumber][0]+rzd.r, xycorr[modnumber][2], rzd.z+xycorr[modnumber][4]);
    AMSDir TRDTube_Dir_check;
    if(hit->Layer <4 || hit->Layer>15) TRDTube_Dir_check.setd(1+xycorr[modnumber][1],xycorr[modnumber][3] ,xycorr[modnumber][5]);
    else TRDTube_Dir_check.setd(xycorr[modnumber][1],1+xycorr[modnumber][3] ,xycorr[modnumber][5]);
    
    dir.setd(0, 0,   1);
    pnt.setp(0, 0, TRDTube_Center_check.z());
    
    _fit->Interpolate(pnt, dir);
    
    AMSPoint check=pnt-TRDTube_Center_check;
    AMSPoint check2=dir.crossp(TRDTube_Dir_check);
    
    float abstand;
    abstand = (check.prod(check2))/check2.norm();
    AMSPoint res=check2/check2.norm()*abstand;

    if(fabs(abstand)<0.5) {
      
      float pl=GetPathLength3D(TRDTube_Center_check, TRDTube_Dir_check, pnt, dir);
      
      if(pl>0.1){
	TrdMHits.push_back(hit);
	phitlen.push_back(pl);
	hitres.push_back(res);
      }
      
    }
  }
  
  return;
}

/////////////////////////////////////////////////////////////////////

int TrdMTrack::NHits(){

  return (int)TrdMHits.size();
}
/////////////////////////////////////////////////////////////////////

int TrdMTrack::nMissHit_usedX(){

  return n_mhX;
}
/////////////////////////////////////////////////////////////////////

int TrdMTrack::nMissHit_usedY(){

  return n_mhY;
}

/////////////////////////////////////////////////////////////////////

float TrdMTrack::CalcLogLikelihood(){

  float like=-1;

  int nhits=0;
  vector<double> Elike, Plike;
  float xe= GetXePressure();
  int xeindex=(int)((xe-700)/50);
  if(xeindex<0) xeindex=0; if(xeindex>5) xeindex=5;

  int pindex=0;
  for(int i=nbins-1; i>=0; i--) if(fabs(_rig)> xbins[i] && pindex==0) pindex=i;

  for(int i=0; i<(int)TrdMHits.size(); i++){
    
    int lay= TrdMHits[i]->Layer;
    int lad= TrdMHits[i]->Ladder;
    int tub= TrdMHits[i]->Tube;
        
    float gaincorr=GetGainCorrection(lay, lad, tub);
    // printf("lay %i lad %i tube %i  gaincorr: %f\n",lay, lad, tub, gaincorr);

    float pl=phitlen[i];
    float ampcorr=TrdMHits[i]->Amp*gaincorr;
    // if(ampcorr>4047.) ampcorr=4047.;
    // if(ampcorr<20.) ampcorr=20.;
    
    double elik=1., plik=1.;
    if(TrdPdfType==0){
      elik=GetElProb(xeindex, lay, pl, ampcorr);
      
      plik=GetProProb(xeindex, pindex, pl, ampcorr);

    }
    else if(TrdPdfType==1){
      elik=GetElProb(ampcorr, _rig, pl, lay, xe);
      plik=GetProProb(ampcorr, _rig, pl, lay, xe);
    }
    if(elik!=0 && plik!=0){
      Elike.push_back(elik);
      Plike.push_back(plik);
      // printf("ampcorr: %f _rig: %f pl: %f lay:%i xe: %f   elik: %f plike: %f\n", ampcorr, _rig, pl, lay, xe, elik, plik);
      nhits++;
    }
  }
  
  if(nhits==0) {
      return -1;
    }
  
  double elike=1., plike=1.; 
  for(int i=0; i<nhits; i++){
    elike*=pow(Elike[i], (double)(1./(double)nhits));
    plike*=pow(Plike[i], (double)(1./(double)nhits));
  }

  if(elike!=plike) like=-log(elike/(elike+plike));
  // printf("Elike: %f Plike: %f loglike: %f\n", elike,plike, like);

  return like;
}

/////////////////////////////////////////////////////////////////////

void TrdMTrack::interpolate_trd_track(TrdTrackR* trdtrack, AMSPoint &pnt, AMSDir &dir, int z)
{
  TVector3 track_pos, track_dir;

  track_dir.SetMagThetaPhi(1.,trdtrack->Theta,trdtrack->Phi);
  track_pos.SetXYZ(trdtrack->Coo[0],trdtrack->Coo[1],trdtrack->Coo[2]);
      

  TVector3 pos0=track_pos+(z-track_pos.z())/track_dir.z()*track_dir;

  pnt.setp(pos0.x(), pos0.y(), pos0.z());
  dir.setd(track_dir.x(), track_dir.y(), track_dir.z());

  return;
}

/////////////////////////////////////////////////////////////////////

int TrdMTrack::which_tube(int lay, float loc, int mod){

  int here=-1;
  int ladder=-1;
  int tube=-1;
  
 
  for( int i=0; i<18; i++){
    for( int j=0; j<16; j++){
      if(loc>=get_trd_rz(lay,i,j,0)) ladder = i;
      if(loc>=get_trd_rz(lay,i,j,0)) tube = j;
    }
  }
  
  
  if(mod==0) here=ladder;
  else if(mod==1) here=tube;
  
  return here;
  
}

/////////////////////////////////////////////////////////////////////

float TrdMTrack::get_trd_rz(int layer, int ladder, int tube, int mod){
  
  int d=-1;
  int module=layer*14;
  if(layer>4) module+=2*(layer-4);
  if(layer>12) module+=2*(layer-12);

  if(layer<12) ladder++;
  if(layer< 4) ladder++;
  
  if((layer>=16)||(layer<=3)) d = 1;
  else                        d = 0;
  
  float z = 85.275 + 2.9*(float)layer;
  if(ladder%2==0) z += 1.45;
  
  float r = 10.1*(float)(ladder-9);
  
  if((d==1) && (ladder>=12)) r+=0.78;
  if((d==1) && (ladder<= 5)) r-=0.78;
  
  r += 0.31 + 0.62*(float)tube;
  if(tube >=  1) r+=0.03;
  if(tube >=  4) r+=0.03;
  if(tube >=  7) r+=0.03;
  if(tube >=  9) r+=0.03;
  if(tube >= 12) r+=0.03;
  if(tube >= 15) r+=0.03;
  
  if( mod==0) return r+xycorr[module][0+(2*d)];
  else if(mod==1) return z+xycorr[module][4];
  
  return -1;
}

/////////////////////////////////////////////////////////////////////

float TrdMTrack::GetPathLength3D(AMSPoint trdpnt, AMSDir trddir, AMSPoint tpnt, AMSDir tdir ){

  float r=0.303;

  AMSDir dir=trddir/trddir.norm();
  AMSDir tkdir=tdir/tdir.norm();

  float a=dir.prod(tpnt);
  float b=dir.prod(trdpnt);
  float c=dir.prod(tkdir);
  float d=dir.prod(dir);

  float p=(2*c/d*dir.prod(trdpnt))-(2*trdpnt.prod(tkdir))+(2*c*a/(d*d)*dir.norm()*dir.norm())-(2*b*c/(d*d)*dir.norm()*dir.norm())-(2*(a/d-b/d)*dir.prod(tkdir))-(2*c/d*tpnt.prod(dir))+(2*tkdir.prod(tpnt));
  float q=(trdpnt.norm()*trdpnt.norm())+(tpnt.norm()*tpnt.norm())+(2*(a-b)/d*trdpnt.prod(dir))-(2*trdpnt.prod(tpnt))+(((a*a+b*b)/(d*d)-2*a*b/(d*d))*dir.norm()*dir.norm())-(2*(a/d-b/d)*tpnt.prod(dir))-(r*r);
  float norm=(tkdir.norm()*tkdir.norm())-(2*c/d*dir.prod(tkdir))+(c*c/(d*d)*dir.norm()*dir.norm());

  float pnorm=p/norm;
  float qnorm=q/norm;

  if(pnorm*pnorm/4-qnorm<0) return 0;

  float m1=0-pnorm/2+sqrt(pnorm*pnorm/4-qnorm);
  float m2=0-pnorm/2-sqrt(pnorm*pnorm/4-qnorm);

  AMSPoint x1=tpnt+tkdir*m1;
  AMSPoint x2=tpnt+tkdir*m2;

  float pl=(x2-x1).norm();
 
  return pl;
}

/////////////////////////////////////////////////////////////////////

void TrdMTrack::Init_Base(){
  
  cout<<"TrdMTrack::Init_Base: Initialising TrdMTrack... " <<endl;
 
  char fname[200];
  char description[100];

  const char *amsdatadir=getenv("AMSDataDir");
  char local[]="/afs/cern.ch/ams/Offline/AMSDataDir";
  if(!(amsdatadir && strlen(amsdatadir))){
    amsdatadir=local;
  }
   
  sprintf(fname, "%s/v5.00/TRD/MTrack_lookup_files.txt", amsdatadir);

  FILE* file=fopen(fname, "r");
  
  if(!file) {cout<<"TrdMTrack:: Could not read lookup files! Using time independent corrections only! "<< fname <<endl; FirstRun=0; LastRun=0; return;}
  
  fscanf(file, "%[^\n]\n", description);
  fscanf(file, "%s %lu \n", description, &first_alignment);
  fscanf(file, "%s %lu \n", description, &last_alignment);
  fscanf(file, "%s %s \n", description, xenonfile);
  fscanf(file, "%s %s \n", description, qt_gainfile);
  fscanf(file, "%s %s \n", description, qt_alignmentfile);
  fscanf(file, "%s %s \n", description, trd_shimmingfile);

  fclose(file);
  
  FirstRun=first_alignment;

  Init_Xe();
  Init_GainCorrection();
  Init_Alignment();
  Init_PDFs();
  generate_modul_matrix();
  _fit = new TrFit();

  _Time_previous=0;
  IsValid=true;
 
  return;
}

/////////////////////////////////////////////////////////////////////

void TrdMTrack::Init_Xe(){
  
  const char *amsdatadir=getenv("AMSDataDir");
  char local[]="/afs/cern.ch/ams/Offline/AMSDataDir";
  if(!(amsdatadir && strlen(amsdatadir))){
    amsdatadir=local;
  }
   
  TString name=xenonfile;
  cout<<"TrdMTrack::Init_Xe:Read TRD Pressure from: "<<name<<endl;
  TFile* f=new TFile(name);
  
  if(!f) {cout<<"TrdMTrack::Init_Xe: Could not read Xenon Pressure!  Using time independent corrections only!"<<name<<endl;  FirstRun=0; LastRun=0; return;}
  
  // get tree
  TTree* tree = (TTree*)f->Get("trd_online");
  if (!tree) {
    cout << "ERROR reading tree 'trd_online' " << endl;
    return;
  }
  // set branches
  unsigned long long t; // should be of type time_t, but doesn't work well with CINT
  double value;
  tree->SetBranchAddress("time", &t);
  tree->SetBranchAddress("xe", &value);

  unsigned int nEntries = tree->GetEntries();

  for (unsigned int i = 0; i < nEntries; i++) {
    tree->GetEntry(i);
    xetime.push_back((unsigned int)t);
    xe.push_back((float)value);
  }

  f->Close();
  delete f;
 
  if((unsigned int)(xetime[0])>FirstRun) FirstRun=(unsigned int)(xetime[0]);
  if((unsigned int)(xetime[(int)xetime.size()-1])<LastRun) LastRun=(unsigned int)(xetime[(int)xetime.size()-1]);
  
  cout<<"TrdMTrack::Init_Xe: Pressure read..."<<endl;
  
  return;
  
}

/////////////////////////////////////////////////////////////////////

void TrdMTrack::Init_GainCorrection(){

  if(TrdGainType==-1){
    cout<<"No gain correction applied!" <<endl;
  }

  if(TrdGainType==0){
  
    TString name=qt_gainfile;
    cout<<"TrdMTrack::Init_GainCorrection: Read TRD Gain from: "<<name<<endl;
    TFile* f=new TFile(name);
    
    if(!f) {cout<<"TrdMTrack::Init_GainCorrection: Could not read Gains! Using time independent corrections only!"<<name<<endl; FirstRun=0; LastRun=0; return;}
    
    char gname[80];
    int test;
    for(int j=0; j<328; j++){
      sprintf(gname, "fitMpvGraphGoodFit_%i", j);
      TGraph *g;
      g=(TGraph*)f->Get(gname);
      
      double time;
      double ga;
      for(int i=0; i<g->GetN(); i++){
	test=g->GetPoint(i, time, ga);
	if(test!=-1) {
	  gaintime[j].push_back((unsigned int)time);
	  gain[j].push_back((float)ga);
	}
      }
      g->Clear();
      delete g;
      g=0;
    }
    f->Close();
    delete f;
  
    if((unsigned int)(gaintime[6][0])>FirstRun) FirstRun=(unsigned int)(gaintime[6][0]);
    if((unsigned int)(gaintime[6][(int)gaintime[6].size()-1])<LastRun) LastRun=(unsigned int)(gaintime[6][(int)gaintime[6].size()-1]);
    
  }
  else if(TrdGainType==1){
    if(TrdKCluster::_DB_instance) trdk_db=TrdKCluster::_DB_instance;
    else{
      if(!trdk_db) trdk_db = new TrdKCalib();
    }

  }

  if(TrdGainType==1 || TrdGainType==0) cout<<"TrdMTrack::Init_GainCorrection: Gain factors read..."<<endl;

  return;

}
/////////////////////////////////////////////////////////////////////

void TrdMTrack::Init_Alignment(){
  
  if(TrdAlignType==-1){
    
    TString zname=trd_shimmingfile;
    cout<<"TrdMTrack::Init_Alignment: Read TRD Shimming from: "<< zname <<endl;
    TFile* fi=new TFile(zname);
    
    if( !fi) {cout<<"TrdMTrack::Init_Alignment: Could not read Alignment! "<<zname<<endl; return;}

    double z;
    double time;
    
    TGraph *zg=(TGraph*)fi->Get("grModul_zmeasurement");
    for(int j=0; j<328; j++){
      zg->GetPoint(j, time, z);
      z_corr[j][0]=(float)time;
      z_corr[j][1]=(float)z;
    }
    zg->Clear();
    delete zg;
    zg=0;

    fi->Close();
    delete fi;
  }

  else if(TrdAlignType==2 ){
    
    if(FirstRun!=0){
      update_alignment_vectors(FirstRun);
  
      if(FirstRun<first_alignment) FirstRun=first_alignment;
      if(LastRun>last_alignment) LastRun=last_alignment;
    
    }
  }

  else if(TrdAlignType==0){
   
    TString name=qt_alignmentfile;
    TFile* f=new TFile(name);
   
    TString zname=trd_shimmingfile;
    cout<<"TrdMTrack::Init_Alignment: Read TRD Alignment from: "<< name <<endl;
    TFile* fi=new TFile(zname);
    
    if(!f || !fi) {cout<<"TrdMTrack::Init_Alignment: Could not read Alignment! Using time independent corrections only!"<<name<<endl; FirstRun=0; LastRun=0; return;}
   
    char gname[80];
    double z;
    double time;
    int test;
    
    for(int j=0; j<328; j++){
      sprintf(gname, "fitMeanGraphGoodFit_%i", j);
      TGraph *g;
      g=(TGraph*)f->Get(gname);
      for(int i=0; i<g->GetN(); i++){
	test=g->GetPoint(i, time, z);
	if(test!=-1) { 
	  mod_time[j].push_back((unsigned long)time);
	  mod_corr[j][0].push_back((float)z);
	}
      }
      g->Clear();
      delete g;
      g=0;
    }
    f->Close();
    delete f;

    TGraph *zg=(TGraph*)fi->Get("grModul_zmeasurement");
    for(int j=0; j<328; j++){
      test=zg->GetPoint(j, time, z);
      z_corr[j][0]=(float)time;
      z_corr[j][1]=(float)z;
    }
    zg->Clear();
    delete zg;
    zg=0;

    fi->Close();
    delete fi;

    for( int i=0; i<328; i++){
      index[i]=0;
    }

  }
  else if(TrdAlignType==1){
   
    if(!trdk_db) {
      if(TrdKCluster::_DB_instance) trdk_db=TrdKCluster::_DB_instance;
      else    trdk_db = new TrdKCalib();
    }
  }

  if(TrdAlignType==0 || TrdAlignType==2){
    if((unsigned int)(mod_time[6][0])>FirstRun) FirstRun=(unsigned int)(mod_time[6][0]);
    if((unsigned int)(mod_time[6][(int)mod_time[6].size()-1])<LastRun) LastRun=(unsigned int)(mod_time[6][(int)mod_time[6].size()-1]);
  }

  cout<<"TrdMTrack::Init_Alignment: Alignment read..."<<endl;

  return;

}

/////////////////////////////////////////////////////////////////////

void TrdMTrack::update_alignment_vectors(unsigned long evttime){

  const char *amsdatadirm=getenv("AMSDataDirM");
  const char *amsdatadir=getenv("AMSDataDir");
  char local[]="/afs/cern.ch/exp/ams/Offline/AMSDataDir";
  if(!(amsdatadirm && strlen(amsdatadirm))){
    amsdatadirm=amsdatadir;
  }
  if(!(amsdatadirm && strlen(amsdatadirm))){
    amsdatadirm=local;
  }
   
  char number[10];
  sprintf(number, "%i", (int)(evttime/10000000));

  TString name;
  name=TString(amsdatadirm)+"/v5.00/TRD/Modul_alignment_"+TString(number)+".root";
   
    cout<<"TrdMTrack::Init_Alignment: Read TRD Alignment from: "<< name <<endl;
    TFile* f=new TFile(name);
    
    if(!f) {cout<<"TrdMTrack::Init_Alignment: Could not read Alignment! "<<name<<endl; return;}
  
    char gname[80];
    double z;
    double time;
    int test = -1;

    for(int j=0; j<328; j++){
      mod_time[j].clear();
      for(int i=0; i<4; i++){
	mod_corr[j][i].clear();
      }
    }

    
    for(int j=0; j<328; j++){
      sprintf(gname, "grModul_rres_%i", j);
      TGraph *g1;
      g1=(TGraph*)f->Get(gname);
      for(int i=0; i<g1->GetN(); i++){
	test=g1->GetPoint(i, time, z);
	if(test!=-1) { 
	  mod_time[j].push_back((unsigned int)time);
	  mod_corr[j][0].push_back((float)z);
	}
      }
      g1->Clear();
      delete g1;
      g1=0;
      TGraph *g2;
      sprintf(gname, "grModul_rinc_%i", j);
      g2=(TGraph*)f->Get(gname);
      for(int i=0; i<g2->GetN(); i++){
	g2->GetPoint(i, time, z);
	if(test!=-1) mod_corr[j][1].push_back((float)z);
      }
      g2->Clear();
      delete g2;
      g2=0;
      TGraph *g3;
      sprintf(gname, "grModul_zres_%i", j);
      g3=(TGraph*)f->Get(gname);
      for(int i=0; i<g3->GetN(); i++){
  	g3->GetPoint(i, time, z);
	if(test!=-1) mod_corr[j][2].push_back((float)z);
      }
      g3->Clear();
      delete g3;
      g3=0;
      TGraph *g4;
      sprintf(gname, "grModul_zinc_%i", j);
      g4=(TGraph*)f->Get(gname);
      for(int i=0; i<g4->GetN(); i++){
	  g4->GetPoint(i, time, z);
	if(test!=-1) mod_corr[j][3].push_back((float)z);
      }
      g4->Clear();
      delete g4;
      g4=0;
    }

    f->Close();
    delete f;
    
    _calibtime=(int)(evttime/10000000)*10000000;

    for( int i=0; i<328; i++){
      index[i]=0;
    }
  
    cout<<"TrdMTrack::Init_Alignment: new alignment files read..."<<endl;

  return;
}

/////////////////////////////////////////////////////////////////////

void TrdMTrack::Init_PDFs(){

  if(TrdPdfType==0){
    const char *amsdatadir=getenv("AMSDataDir");
    char local[]="/afs/cern.ch/ams/Offline/AMSDataDir";
    if(!(amsdatadir && strlen(amsdatadir))){
      amsdatadir=local;
    }
    
    // TString name=TString(amsdatadir)+"/v5.00/TRD/TrdScalibPdfs_v23.root";
    TString name=TString(amsdatadir)+"/v5.00/TRD/acroot/data/TrdQt_pdf_v11.root";
    cout<<"TrdMTrack::Init_PDFs: Read TRD PDFs from: "<<name<<endl;
    TFile* f=new TFile(name);
    
    if(!f) {cout<<"TrdMTrack::Init_PDFs: Could not read PDFs! "<<name<<endl; return;}
    
    char gname[80];
    for(int j=0; j<6; j++){
      for(int i=0; i<30; i++){
	//	sprintf(gname, "grTrdS_Prot_%i_%i", j, i);
	sprintf(gname, "grTrdQt_PDF_ADCVsRigidity_Proton_%i_%i", j, i);
	ProPDF[j][i]=(TGraph*)f->Get(gname);
      }
      for(int i=0; i<20; i++){
	//	sprintf(gname, "grTrdS_Elec_%i_%i", j, i);
	sprintf(gname, "grTrdQt_PDF_ADCVsLayer_Electron_%i_%i", j, i);
	ElPDF[j][i]=(TGraph*)f->Get(gname);
      }
    }
    f->Close();
    delete f;
  }
  else if(TrdPdfType==1){

    if(!Ppdf) Ppdf = new TrdKPDF("Proton");
    if(!Epdf) Epdf = new TrdKPDF("Electron");

  }

  cout<<"TrdMTrack::Init_PDFs: PDF's read..."<<endl;
  
  return;
  
}

/////////////////////////////////////////////////////////////////////

void TrdMTrack::generate_modul_matrix(){

 
  int iLay0 = 0;
  for (int iLay = iLay0; iLay<iLay0+4; iLay++) {
    int n = iLay0 + iLay;
    for (int iLadr=0; iLadr<14; iLadr++) {
      if (iLadr>=0 && iLadr<14 && iLay>=0 && iLay<20) 
        mStraw[iLadr][iLay] =   n;
      n += 4;
    }
  }

  iLay0 = 4;
  for (int iLay = iLay0; iLay<iLay0+4; iLay++) {
    int n = (56-iLay0) + iLay;
    for (int iLadr=0; iLadr<16; iLadr++) {
      if (iLadr>=0 && iLadr<16 && iLay>=0 && iLay<20) 
        mStraw[iLadr][iLay] =   n;
      n += 4;
    }
  }

  iLay0 = 8;
  for (int iLay = iLay0; iLay<iLay0+4; iLay++) {
    int n = (120-iLay0) + iLay;
    for (int iLadr=0; iLadr<16; iLadr++) {
      if (iLadr>=0 && iLadr<16 && iLay>=0 && iLay<20) 
        mStraw[iLadr][iLay] =   n;
      n += 4;
  }
  }
    
  iLay0 = 12;
  for (int iLay = iLay0; iLay<iLay0+4; iLay++) {
    int n = (184-iLay0) + iLay;
    for (int iLadr=0; iLadr<18; iLadr++) {
      if (iLadr>=0 && iLadr<18 && iLay>=0 && iLay<20) 
        mStraw[iLadr][iLay] =   n;
      n += 4;
    }
  }
  
  iLay0 = 16;
  for (int iLay = iLay0; iLay<iLay0+4; iLay++) {
    int n = (256-iLay0) + iLay;
    for (int iLadr=0; iLadr<18; iLadr++) {
      if (iLadr>=0 && iLadr<18 && iLay>=0 && iLay<20) 
        mStraw[iLadr][iLay] =   n;
      n += 4;
    }
  }
  cout<<"TrdMTrack::generate_modul_matrix: TRD Straw Matrix generated..."<<endl;

  return;
}


/////////////////////////////////////////////////////////////////////

float TrdMTrack::GetGainCorrection(int layer, int ladder, int tube){
   
  int module = mStraw[ladder][layer];
  if(module<0 || module>327) return 0;

  float gaincorr, fix=100., gcorr;
 
  if(TrdGainType==-1) gaincorr=1;


  if(TrdGainType==0){
    int index=0;
    unsigned long time=(unsigned long)(_Time);
    while(time>gaintime[module][index+1] && index<((int)gaintime[module].size()-1)) index++;

    float inter = (float)(time-gaintime[module][index])/(float)(gaintime[module][index+1]-gaintime[module][index]);

   
    gcorr=gain[module][index]+inter*(gain[module][index+1]-gain[module][index]);
    gaincorr=fix/gcorr;
    // printf("Time: %li lower: %li upper: %li inter: %f gain: %f \n", time, gaintime[module][index],gaintime[module][index+1],  inter, gaincorr);
  
  }
  else if(TrdGainType==1){
    int read= trdk_db->readDB_Calibration(_Time);
    if(read==0){
      cout<<"~~~~~~~WARNING, TrdKCluster~~~~~~~Can Not Read DBs for Gain Calibration, Returning 0"<<endl;
      return 0;
    }
   
    int tubeid=module*16+tube;
    gaincorr= trdk_db->GetGainCorrectionFactorTube(tubeid,_Time);
    // printf("Time: %li gain: %f\n", (unsigned long)_Time, gaincorr);
  }
  
  if(gaincorr==0){ printf("TrdMTrack::GetGainCorrection: No calibration available...\n"); return -1;}

  return gaincorr;

}

/////////////////////////////////////////////////////////////////////

float TrdMTrack::GetXePressure(){
   
  int index=0;
  float xe_pressure;
  unsigned int time=(unsigned int)(_Time);
  while(time>xetime[index+1] && index<((int)xetime.size()-1)) index++;
  float inter = (time-xetime[index])/(xetime[index+1]-xetime[index]);
  xe_pressure=xe[index]+(xe[index+1]-xe[index])*inter;
  //xe_pressure=xenon->Eval(time);
  
  if(xe_pressure==0){ printf("TrdMTrack::GetXePressure: No Xe pressure available...\n"); return -1;}

  return xe_pressure;
}

/////////////////////////////////////////////////////////////////////

void TrdMTrack::SetAlignment(){

  for(int i=0; i<328; i++){
    for(int j=0; j<6; j++){
      xycorr[i][j]=0;
    }
  }

  if(_Time<_Time_previous) {
    for(int i=0; i<328; i++){
      index[i]=0;
    }
  }

  if(TrdAlignType==-1){
    for( int i=0; i<328; i++){
     
      xycorr[i][0]=0;
      xycorr[i][1]=0;
      xycorr[i][2]=0;
      xycorr[i][3]=0;
      xycorr[i][4]=z_corr[i][0]/pow(10.,4)+0.1;
      xycorr[i][5]=z_corr[i][1]/pow(10.,6);
     
    }
  }
  else if(TrdAlignType==2 ){
    // int index=0;
    unsigned long time=(unsigned long)(_Time);
    
 
    for( int i=0; i<328; i++){
      if((int)mod_time[0].size()>1){
	while(time>mod_time[i][index[i]+1] && index[i]<((int)mod_time[i].size()-2)) index[i]++;
	if(i>55 && i<256){
	  xycorr[i][0]=mod_corr[i][0][index[i]];
	  xycorr[i][1]=mod_corr[i][1][index[i]];
	  xycorr[i][2]=0;
	  xycorr[i][3]=0;
	  xycorr[i][4]=mod_corr[i][2][index[i]];
	  xycorr[i][5]=mod_corr[i][3][index[i]];
	}
	else {
	  xycorr[i][0]=0;
	  xycorr[i][1]=0;
	  xycorr[i][2]=mod_corr[i][0][index[i]];
	  xycorr[i][3]=mod_corr[i][1][index[i]];
	  xycorr[i][4]=mod_corr[i][2][index[i]];
	  xycorr[i][5]=mod_corr[i][3][index[i]];
	}
      }
      else {
      	xycorr[i][0]=0;
	xycorr[i][1]=0;
	xycorr[i][2]=0;
	xycorr[i][3]=0;
	xycorr[i][4]=z_corr[i][0]/pow(10.,4)+0.1;
	xycorr[i][5]=z_corr[i][1]/pow(10.,6);
      }
    }
  }
  else if(TrdAlignType==0){
    
    unsigned long time=(unsigned long)(_Time);
    
    for( int i=0; i<328; i++){
      // int index=0;
      int lay=0, lad=0;
      int l=i;
      for(int j=0; j<4; j++)  if(l-j*14>=0) { lay = j; lad=l-j*14;}
      for(int j=0; j<8; j++)  if(l-(j*16+56)>=0) {lay = j+4; lad=l-(j*16+56);}
      for(int j=0; j<8; j++)  if(l-(j*18+184)>=0) {lay = j+12; lad=l-(j*18+184);}
      int module=mStraw[lad][lay];
      // printf("%i -> lad %i lay %i mod %i\n", i, lad, lay, module);

      while(time>mod_time[module][index[i]+1] && index[i]<((int)mod_time[module].size()-2)) index[i]++;
      float inter = (float)(time-mod_time[module][index[i]])/(float)(mod_time[module][index[i]+1]-mod_time[module][index[i]]);
     
      if(i>55 && i<256){
	xycorr[i][0]=0-(mod_corr[module][0][index[i]]+inter*(mod_corr[module][0][index[i]+1]-mod_corr[module][0][index[i]]));
	xycorr[i][1]=0;
	xycorr[i][2]=0;
	xycorr[i][3]=0;
	xycorr[i][4]=z_corr[i][0]/pow(10.,4)+0.1;
	xycorr[i][5]=z_corr[i][1]/pow(10.,6);
      }
      else {
	xycorr[i][0]=0;
	xycorr[i][1]=0;
	xycorr[i][2]=0-(mod_corr[module][0][index[i]]+(mod_corr[module][0][index[i]+1]-mod_corr[module][0][index[i]])*inter);
	xycorr[i][3]=0;
	xycorr[i][4]=z_corr[i][0]/pow(10.,4)+0.1;
	xycorr[i][5]=z_corr[i][1]/pow(10.,6);
      }
    }
  }
  else if(TrdAlignType==1){
    
    AMSPoint T[20];
    AMSRotMat R[20];
    AMSPoint Center[20];
    
    int read = trdk_db->readDB_Alignment(evt->Run());
    if(read==0){
      cout<<"~~~~~~~WARNING, TrdKCluster~~~~~~~Can Not Read DBs for Alignment, Returning"<<endl;
      return;
    }
    if(read!=0){

      for(int d=0; d<20; d++){

	TRDAlignmentPar param=trdk_db->GetAlignmentPar(d,evt->Run());
       	TRDAlignmentPar* para=&param;
	T[d].setp(para->dX,para->dY,para->dZ);
	R[d].SetRotAngles(-1*para->alpha,-1*para->beta,-1*para->gamma);
	Center[d].setp(para->RotationCenter_X,para->RotationCenter_Y,para->RotationCenter_Z);
	//	delete para;
      }
      for( int i=0; i<328; i++){
	int lay=0, lad=0;
	int l=i;
	for(int j=0; j<4; j++)  if(l-j*14>=0) { lay = j; lad=l-j*14;}
	for(int j=0; j<8; j++)  if(l-(j*16+56)>=0) {lay =j+4; lad=l-(j*16+56);}
	for(int j=0; j<8; j++)  if(l-(j*18+184)>=0) {lay = j+12; lad=l-(j*18+184);}

	if(i>55 && i<256){
	  AMSPoint tube(get_trd_rz(lay, lad, 8, 0), 0, get_trd_rz(lay, lad, 8, 1));
	  AMSDir tdir(0, 1, 0);
	  AMSPoint ttube= R[lay]*(tube-Center[lay])+T[lay]+Center[lay];
	  tdir=R[lay]*tdir;
	  xycorr[i][0]=ttube.x()-get_trd_rz(lay, lad, 8, 0);
	  xycorr[i][1]=tdir.x();
	  xycorr[i][2]=ttube.y();
	  xycorr[i][3]=tdir.y()-1.;
	  xycorr[i][4]=ttube.z()-get_trd_rz(lay, lad, 8, 1);
	  xycorr[i][5]=tdir.z();
	}
	else {
	  AMSPoint tube(0, get_trd_rz(lay, lad, 8, 0), get_trd_rz(lay, lad, 8, 1));
	  AMSDir tdir(1, 0, 0);
	  AMSPoint ttube= R[lay]*(tube-Center[lay])+T[lay]+Center[lay];
	  xycorr[i][0]=ttube.x();
	  xycorr[i][1]=tdir.x()-1.;
	  xycorr[i][2]=ttube.y()-get_trd_rz(lay, lad, 8, 0);
	  xycorr[i][3]=tdir.y();
	  xycorr[i][4]=ttube.z()-get_trd_rz(lay, lad, 8, 1);
	  xycorr[i][5]=tdir.z();
	}
      }
    }
  }

  return;
}

/////////////////////////////////////////////////////////////////////

double TrdMTrack::GetElProb(int xeindex, int sindex, float pl, float amp){
  
  double  elprob=ElPDF[xeindex][sindex]->Eval(amp/pl);

  if(elprob==0 && amp/pl>3000.) {
	int j=1;
	while(elprob==0) {elprob=ElPDF[xeindex][sindex]->Eval(amp/pl-j); j++;}
      }
  else if(elprob==0 && amp/pl<50.){
    int j=1;
    while(elprob==0) {elprob=ElPDF[xeindex][sindex]->Eval(amp/pl+j); j++;}
  }
  
  if(elprob==0){ printf("TrdMTrack::GetElProb: No Probability calculated...\n"); return -1;}
  
  return elprob;
}

double TrdMTrack::GetElProb(float amp, float rig, float pl, int lay, float xe ){

  double elprob=Epdf->GetLikelihood(amp, fabs(rig), pl, lay, xe/1000);

  if(elprob==0){ printf("TrdMTrack::GetElProb: No Probability calculated...\n"); return -1;}
  
  return elprob;
}

/////////////////////////////////////////////////////////////////////

double TrdMTrack::GetProProb(int xeindex, int sindex, float pl, float amp){
   
  double proprob=ProPDF[xeindex][sindex]->Eval(amp/pl);
 
  if(proprob==0 && amp/pl>3000.) {
	int j=1;
	while(proprob==0) {proprob=ProPDF[xeindex][sindex]->Eval(amp/pl-j); j++;}
      }
  else if(proprob==0 && amp/pl<50.){
    int j=1;
    while(proprob==0) {proprob=ProPDF[xeindex][sindex]->Eval(amp/pl+j); j++;}
  }
  
  if(proprob==0){ printf("TrdMTrack::GetProProb: No Probability calculated...\n"); return -1;}
  
  return proprob;
}

double TrdMTrack::GetProProb(float amp, float rig, float pl, int lay, float xe ){

  double proprob=Ppdf->GetLikelihood(amp, fabs(rig), pl, lay, xe/1000);

  if(proprob==0){ printf("TrdMTrack::GetProProb: No Probability calculated...\n"); return -1;}
  
  return proprob;
}
/////////////////////////////////////////////////////////////////////
