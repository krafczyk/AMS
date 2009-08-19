/////////////////////////////////////////////
/////////   PGTRACK VERSION     /////////////
/////////////////////////////////////////////

// Author J. Alcaraz June-July 2003
// Modified P. Zuccon January 2009 for new tk sw
//

#include "Vertex.h"
#include "tkdcards.h"
#include "amsdbc.h"
#include "TString.h"

#ifdef __ROOTSHAREDLIBRARY__
#ifdef _STANDALONE_
#include "VCon_root2.h"
#else
#include "VCon_root.h"
#endif
VCon* VertexR::vcon = new VCon_root();
#else  
#include "VCon_gbatch.h"
VCon* VertexR::vcon = new VCon_gb();
#endif





void VertexR::Clear(){
  _Momentum=0;
  _ErrMomentum=0;
  _Theta=0;
  _Phi=0;
  _Mass=0;
  _Charge=0;
  _Chi2=0;
  _Ndof=0;
  _fTrTrack.clear();
  _Ptrack.clear();
  _Vertex.setp(0,0,0);
  _filled=0;
}

//
///////////////////////////////////////////////////////////

VertexR::VertexR(int ntracks, TrTrackR *ptrack[]) {
  
  //Reset to initial values
  Clear();
  //Set the track number

  for (int i=0; i<ntracks; i++) { _Ptrack.push_back(ptrack[i]); }
  BuildTracksIndex();
  number mom[3] = {0.0, 0.0, 0.0};
     
  if (NTrTrack() <= 0) return ;
  
  // Most of the information
  number u[3];
  for (int i=0; i<NTrTrack(); i++) { 
    _Ptrack[i]->setstatus(AMSDBc::USED);
    number rig =  _Ptrack[i]->GetRigidity();
    number en =  fabs(rig);
    number erig =  _Ptrack[i]->GetErrRinv()*en*en;
    _Momentum += en;
    _Charge += (rig>=0.0)? 1:-1;
    _ErrMomentum += erig*erig;
    _Chi2 += _Ptrack[i]->GetChi2(); 
    _Ndof += 2*_Ptrack[i]->GetNhits() - 5; 
    AMSDir dir = AMSDir(_Ptrack[i]->GetTheta(),_Ptrack[i]->GetPhi());
    dir.getp(u);
    mom[0] += en*u[0]; 
    mom[1] += en*u[1]; 
    mom[2] += en*u[2];
  }

  _ErrMomentum = sqrt(_ErrMomentum)/_Momentum/_Momentum;
  _Mass = _Momentum*_Momentum - mom[0]*mom[0] - mom[1]*mom[1] - mom[2]*mom[2];
  if (_Mass>0.0) _Mass = sqrt(_Mass); else _Mass = 0.0; 
  AMSDir U = AMSDir(mom);
  _Theta = U.gettheta();
  _Phi = U.getphi();

   set_vertex();

  _filled=1;

}




void VertexR::_PrepareOutput(int full){ 

                     
  sout.append(Form("Vtx: #tracks %d  Mom %f ErrMom  %f Chrg %d Coo: (%f, %f, %f)",
		   NTrTrack(), _Momentum, _ErrMomentum, _Charge,_Vertex[0],_Vertex[1],_Vertex[2]));
  
  if(!full)
    sout.append("\n");
  else
    sout.append(Form("Theta %f  Phi %f Chi2/Ndf %f/%d \n",
		   _Theta,_Phi, _Chi2, _Ndof));
}
void VertexR::Print(int opt) { 
  _PrepareOutput(opt);
  cout << sout;
}


char* VertexR::Info(int iRef){
  string aa;
  aa.append(Form("Vtx #%d ",iRef));
  _PrepareOutput(0);
  aa.append(sout);
  int len=MAXINFOSIZE;
  if(aa.size()<len) len=aa.size();
  strncpy(_Info,aa.c_str(),len+1);
  return _Info;
}

std::ostream &VertexR::putout(std::ostream &ostr) {
  _PrepareOutput(1);
  return ostr << sout  << std::endl;
}









void VertexR::BuildTracksIndex()
{
  VCon *cont2 = vcon->GetCont("AMSTrTrack");
  if (!cont2) return;
  _fTrTrack.clear();
  for (int i = 0; i < NTrTrack(); i++) {
    _fTrTrack.push_back(cont2->getindex(_Ptrack[i]));
  }
  delete cont2;
}



TrTrackR *VertexR::pTrTrack(unsigned int i) 
{

  if(i>=_fTrTrack.size()) return 0;
  if (_Ptrack[i] == 0 && _fTrTrack[i] >= 0) {
    VCon* cont2 = vcon->GetCont("AMSTrTrack");
    _Ptrack[i] = (TrTrackR*)cont2->getelem(_fTrTrack[i]);
    delete cont2;
  }

  return _Ptrack[i];
}


void VertexR::set_vertex(){
  
  _Vertex = AMSPoint(0.0, 0.0, 0.0);
  if (NTrTrack()<2) return;

  // Find minimum distance in the case of 2 tracks
  //  if (NTrTrack()==2) {

  //  VC 12-18-2003
  //  Correct vertex coordinates
  //

  {

    int number_of_pairs = 0;
    float maxz=2000;
    for (int i0=0; i0<NTrTrack()-1; i0++){ 
      for (int i1=i0+1; i1<NTrTrack(); i1++){ 

	AMSDir dir[2];

	dir[0] = AMSDir(_Ptrack[i0]->GetPdir().gettheta(),_Ptrack[i0]->GetPdir().getphi());
	dir[1] = AMSDir(_Ptrack[i1]->GetPdir().gettheta(),_Ptrack[i1]->GetPdir().getphi());

	number dirprod = dir[0].prod(dir[1]);
	AMSPoint deltax =  _Ptrack[i0]->GetPentry() - _Ptrack[i1]->GetPentry();

	number lambda[2];
	if (fabs(dirprod)<1.) {
	  AMSPoint aux = dir[0] - dir[1]*dirprod;
	  lambda[0] = - deltax.prod(aux) / (1.-dirprod*dirprod);
	  aux = dir[1] - dir[0]*dirprod;
	  lambda[1] = deltax.prod(aux) / (1.-dirprod*dirprod);
	  AMSPoint poi[2];
	  poi[0] = AMSPoint(_Ptrack[i0]->GetPentry()+dir[0]*lambda[0]);
	  poi[1] = AMSPoint(_Ptrack[i1]->GetPentry()+dir[1]*lambda[1]);
	  number mom=fabs(_Ptrack[i0]->GetRigidity())+fabs(_Ptrack[i1]->GetRigidity());
	  //       cout <<" poi0 "<<poi[0]<<endl;
	  //       cout <<" poi1 "<<poi[1]<<endl;
	  if(fabs(poi[0][2])<maxz && fabs(poi[1][2])<maxz){
	    number_of_pairs++;
	    for (int j=0; j<3; j++) {
	      _Vertex[j] += poi[0][j] * fabs(_Ptrack[i0]->GetRigidity())/mom;
	      _Vertex[j] += poi[1][j] * fabs(_Ptrack[i1]->GetRigidity())/mom;
	    }
	  }
	  else{
	    static int nerr=0;
	    if(nerr++<100)cerr<<"AMSVTx::set_vertex-W-VertexZTooBig "<<poi[0]<<" "<<poi[1]<<endl; 
	    setstatus(AMSDBc::BAD);
	  }
	}

   
      }
    }
    for (int j=0; j<3; j++) {
      if(number_of_pairs)_Vertex[j] = _Vertex[j]/(number_of_pairs);
    }
#ifdef __AMSDEBUG__
    if(checkstatus(AMSDBc::BAD))cout <<" vertex "<<_Vertex<<endl;
#endif  
  }
  if(checkstatus(AMSDBc::BAD)){


    int number_of_pairs = 0;
    for (int i0=0; i0<NTrTrack()-1; i0++){ 
      for (int i1=i0+1; i1<NTrTrack(); i1++){ 

	AMSDir dir[2];

	dir[0] = AMSDir(_Ptrack[i0]->GetPdir().gettheta(),_Ptrack[i0]->GetPdir().getphi());
	dir[1] = AMSDir(_Ptrack[i1]->GetPdir().gettheta(),_Ptrack[i1]->GetPdir().getphi());

	number dirprod = dir[0].prod(dir[1]);
	AMSPoint deltax =  _Ptrack[i0]->GetPentry() - _Ptrack[i1]->GetPentry();

	number lambda[2];
	if (fabs(dirprod)<1.) {
	  AMSPoint aux = dir[0] - dir[1]*dirprod;
	  lambda[0] = - deltax.prod(aux) / (1.-dirprod*dirprod);
	  aux = dir[1] - dir[0]*dirprod;
	  lambda[1] = deltax.prod(aux) / (1.-dirprod*dirprod);
	  AMSPoint poi[2];
	  poi[0] = AMSPoint(_Ptrack[i0]->GetPentry()+dir[0]*lambda[0]);
	  poi[1] = AMSPoint(_Ptrack[i1]->GetPentry()+dir[1]*lambda[1]);
	  number mom=fabs(_Ptrack[i0]->GetRigidity())+fabs(_Ptrack[i1]->GetRigidity());
	  number_of_pairs++;
	  for (int j=0; j<3; j++) {
	    _Vertex[j] += poi[0][j] * fabs(_Ptrack[i0]->GetRigidity())/mom;
	    _Vertex[j] += poi[1][j] * fabs(_Ptrack[i1]->GetRigidity())/mom;
	  }
	}

   
      }
    }
    for (int j=0; j<3; j++) {
      if(number_of_pairs)_Vertex[j] = _Vertex[j]/(number_of_pairs);
    }
#ifdef __AMSDEBUG__
    if(checkstatus(AMSDBc::BAD))cout <<" vertex "<<_Vertex<<endl;
#endif  
  }


}


