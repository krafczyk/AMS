// Author J. Alcaraz June-July 2003
//
//

#include <vtx.h>
#include <point.h>
#include <event.h>
#include <trrec.h>
#include <ntuple.h>
#include <tkdbc.h>

#include <extC.h>
#include <mceventg.h>
//

///////////////////////////////////////////////////////////
integer AMSVtx::min_layers_with_different_hits=3;
float AMSVtx::_TimeInit=0;
integer AMSVtx::minimum_number_of_layers=5;
number AMSVtx::chi2cut=100.;
bool AMSVtx::InTofPath=true;

///////////////////////////////////////////////////////////
integer AMSVtx::build(integer refit){

   if (refit) {
// Clean up containers
     for(int i=0;;i++){
       AMSContainer *pctr=AMSEvent::gethead()->getC("AMSVtx",i); 
       if (pctr) pctr->eraseC(); 
       else break; 
     } 
   }

// Do nothing if too many hits or too few hits in the event...
   int nlay=0;
   int nrhits=0;
   for(int i=0;i<TKDBc::nlay();i++){ 
     int nh = (AMSEvent::gethead()->getC("AMSTrRecHit",i))->getnelem(); 
     if (nh>0) {
	  nlay++;
	  nrhits += nh;
     }
   }
   if (nlay<AMSVtx::minimum_number_of_layers) return 0;
   if (nrhits>=min(TRFITFFKEY.MaxTrRecHitsPerLayer*nlay,root::MAXTRRH02)) return 0;

// Start timing (we should not go beyond AMSFFKEY::CpuLimit)
   _StartTiming();

// Go ahead...
   int nfound = 0;
   AMSTrTrack* ptrack[Vtxconst::maxtr];

// Add up tracks
   // cout << "Trying to build a vertex... " << endl;
   while (nfound<Vtxconst::maxtr) {
     if (nfound==0) {
       AMSVtx::minimum_number_of_layers=5;
       AMSVtx::chi2cut=100.;
       AMSVtx::InTofPath=true;
     } else {
       AMSVtx::minimum_number_of_layers=4;
       AMSVtx::chi2cut=500.;
       AMSVtx::InTofPath=false;
     }
     ptrack[nfound] = AMSVtx::next_track();
     if (ptrack[nfound]==NULL) break;
     // Fill non-Juan's track information
     ptrack[nfound]->Fit(0,2);
     ptrack[nfound]->setstatus(AMSDBc::TOFFORGAMMA);
     nfound++;
   }

// Create a vertex
   if (nfound>1) {
     AMSVtx *p= new AMSVtx(nfound, ptrack);
     if (p->set_all()) { 
         AMSEvent::gethead()->addnext(AMSID("AMSVtx",0),p); 
#ifdef __AMSDEBUG__
	   p->print();
#endif
     }

   }

   return nfound;

}

///////////////////////////////////////////////////////////
integer AMSVtx::set_all(){


     // Initial values
     _Momentum = 0.;
     _Charge = 0;
     _ErrMomentum = 0.;
     _Chi2 = 0.;
     _Ndof = 0;
     number mom[3] = {0.0, 0.0, 0.0};
     _Vertex = AMSPoint(0.0, 0.0, 0.0);
     
     if (_Ntracks <= 0) return 0;

     // Most of the information
     number u[3];
     for (int i=0; i<_Ntracks; i++) { 
	 number rig =  _Ptrack[i]->getpirid();
	 number en =  fabs(rig);
	 number erig =  _Ptrack[i]->getepirid();
	 _Momentum += en;
	 _Charge += (rig>=0.0)? 1:-1;
	 _ErrMomentum += erig*erig;
	 _Chi2 += _Ptrack[i]->getpichi2(); 
	 _Ndof += 2*_Ptrack[i]->getnhits() - 5; 
       AMSDir dir = AMSDir(_Ptrack[i]->gettheta(2),_Ptrack[i]->getphi(2));
       dir.getp(u);
	 mom[0] += en*u[0]; 
       mom[1] += en*u[1]; 
       mom[2] += en*u[2];
     }
     _ErrMomentum = sqrt(_ErrMomentum);
     _Mass = _Momentum*_Momentum - mom[0]*mom[0] - mom[1]*mom[1] - mom[2]*mom[2];
     if (_Mass>0.0) _Mass = sqrt(_Mass); else _Mass = 0.0; 
     AMSDir U = AMSDir(mom);
     _Theta = U.gettheta();
     _Phi = U.getphi();

     set_vertex();

     return 1;

}

///////////////////////////////////////////////////////////
void AMSVtx::set_vertex(){

   _Vertex = AMSPoint(0.0, 0.0, 0.0);
   if (_Ntracks<=0) return;

// Set reference point at first hitted layer as initial value
   _Vertex = _Ptrack[0]->getpiP0();

// Find minimum distance in the case of 2 tracks
  if (_Ntracks==2) {

    AMSDir dir[2];
    for (int i=0; i<2; i++) {
      dir[i] = AMSDir(_Ptrack[i]->gettheta(2),_Ptrack[i]->getphi(2));
    }

    number dirprod = dir[0].prod(dir[1]);
    AMSPoint deltax =  _Ptrack[0]->getpiP0() - _Ptrack[1]->getpiP0();

    number lambda[2];
    if (fabs(dirprod)<1.) {
       AMSPoint aux = dir[0] - dir[1]*dirprod;
       lambda[0] = - deltax.prod(aux) / (1.-dirprod*dirprod);
       aux = dir[1] - dir[0]*dirprod;
       lambda[1] = deltax.prod(aux) / (1.-dirprod*dirprod);
       AMSPoint poi[2];
       for (int j=0; j<3; j++) {
         _Vertex[j] = 0.0;
         for (int i=0; i<2; i++) {
           poi[i] = AMSPoint(_Ptrack[i]->getpiP0()+dir[i]*lambda[i]);
           _Vertex[j] += poi[i][j] * fabs(_Ptrack[i]->getpirid())/_Momentum;
         }
       }
    }
  }

}

///////////////////////////////////////////////////////////
AMSTrRecHit* AMSVtx::firstgood(integer pattern, integer index){

	 integer layer = TKDBc::patconf(pattern,index)-1;
       AMSTrRecHit* phit = AMSTrRecHit::gethead(layer);

       while (phit) {
         if (Use_for_vtx(phit, pattern)) break;
         phit=phit->next();
       }

       return phit;

}

///////////////////////////////////////////////////////////
AMSTrRecHit* AMSVtx::nextgood(integer pattern, AMSTrRecHit* phit){

       if (phit) {phit = phit->next();}

       while (phit) {
         if (Use_for_vtx(phit, pattern)) break;
         phit=phit->next();
       }

       return phit;

}

///////////////////////////////////////////////////////////
AMSTrRecHit* AMSVtx::firstgood_path(integer pattern, integer index,number par[2][3]){

	 integer layer = TKDBc::patconf(pattern,index)-1;
       AMSTrRecHit* phit = AMSTrRecHit::gethead(layer);

       while (phit) {
         if (Use_for_vtx(phit, pattern) && Inside_Road(par,phit)) break;
         phit=phit->next();
       }

       return phit;

}

///////////////////////////////////////////////////////////
AMSTrRecHit* AMSVtx::nextgood_path(integer pattern, AMSTrRecHit* phit, number par[2][3]){

       if (phit) {phit = phit->next();}

       while (phit) {
         if (Use_for_vtx(phit, pattern) && Inside_Road(par,phit)) break;
         phit=phit->next();
       }

       return phit;

}

///////////////////////////////////////////////////////////
integer AMSVtx::Use_for_vtx(AMSTrRecHit* phit, integer pattern) {
  int max_layer_with_same_hits;
  int index =   TKDBc::patpoints(pattern) 
	        - AMSVtx::min_layers_with_different_hits - 1;
  if (index>=0) {
    max_layer_with_same_hits = TKDBc::patconf(pattern,index)-1;
  }
  else {
    max_layer_with_same_hits = 0; 
  }

  return (    (phit->checkstatus(AMSDBc::GOOD))
           && (phit->getLayer() <= max_layer_with_same_hits
		    || !phit->checkstatus(AMSDBc::RECOVERED))
	     && (!AMSVtx::InTofPath || !phit->checkstatus(AMSDBc::AwayTOF)) 
	     && (!phit->checkstatus(AMSDBc::FalseTOFX)) 
         );

}

///////////////////////////////////////////////////////////
integer AMSVtx::Inside_Road(number par[2][3], AMSTrRecHit *ptr){

  return (fabs(par[0][1]+par[0][0]*ptr->getHit()[2]-ptr->getHit()[0]) 
	          < 3.*TRFITFFKEY.SearchRegStrLine*par[0][2]) &&
         (fabs(par[1][1]+par[1][0]*ptr->getHit()[2]-ptr->getHit()[1])
                < TRFITFFKEY.SearchRegCircle*par[1][2]);
}

///////////////////////////////////////////////////////////
void AMSVtx::_copyEl(){
#ifdef __WRITEROOT__
   if (PointerNotSet()) return;
   VertexR & ptr = AMSJob::gethead()->getntuple()->Get_evroot02()->Vertex(_vpos);
   for (int i=0;i<_Ntracks;i++) {
      if (_Ptrack[i]) {
              ptr.fTrTrack.push_back(_Ptrack[i]->GetClonePointer());
      } else {
              ptr.fTrTrack.push_back(-1);
      }
   }
#endif
}

///////////////////////////////////////////////////////////
void AMSVtx::_writeEl(){

   if(!AMSVtx::Out(1)) return;

#ifdef __WRITEROOT__
   AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif

   VtxNtuple* ntp = AMSJob::gethead()->getntuple()->Get_vtx02();

   int nvtx = ntp->Nvtx;
   if (nvtx>=MAXVTX02) return; 

// Fill the ntuple in
   //
   ntp->Momvtx[nvtx]= (float)_Momentum;
   ntp->ErrMomvtx[nvtx]= (float)_ErrMomentum;
   ntp->Massvtx[nvtx]= (float)_Mass;
   ntp->Thetavtx[nvtx]= (float)_Theta;
   ntp->Phivtx[nvtx]= (float)_Phi;
   ntp->Chargevtx[nvtx]= _Charge;
   ntp->Statusvtx[nvtx]= getstatus();
   for (int i=0;i<3;i++) {ntp->Vertex[nvtx][i]= (float)_Vertex[i];}
   ntp->Chi2vtx[nvtx]= (float)_Chi2;
   ntp->Ndofvtx[nvtx]= _Ndof;
   ntp->Ntrkvtx[nvtx]= _Ntracks;
   for (int i=0; i<_Ntracks; i++) { 
           ntp->PtrTrack[nvtx][i] = _Ptrack[i]->getpos();
   }
   
   ntp->Nvtx++;
     
}
 
///////////////////////////////////////////////////////////
integer AMSVtx::Out(integer status){
  static integer init=0;
  static integer WriteAll=0;
  if(init == 0){
    init=1;
    integer ntrig=AMSJob::gethead()->gettriggerN();
    for(int n=0;n<ntrig;n++){
      if(strcmp("AMSVtx",AMSJob::gethead()->gettriggerC(n))==0){
        WriteAll=1;
        break;
      }
    }
  }
  return (WriteAll || status);
}
 


///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
AMSTrTrack* AMSVtx::next_track(){

  static AMSTrRecHit * phit[trconst::maxlay];
  AMSTrTrack* ptrack = NULL;
  number minchi2 = AMSVtx::chi2cut;

  for (int pat=0;pat<TKDBc::npat();pat++){
    if(!TKDBc::patallow(pat)) continue;
    if(TKDBc::patpoints(pat)<AMSVtx::minimum_number_of_layers) continue;

    // First and last layers
    int fp=TKDBc::patpoints(pat)-1;    
    for (phit[0]=firstgood(pat,0);
         phit[0]; phit[0]=nextgood(pat,phit[0])){
     for (phit[fp]=firstgood(pat,fp);
          phit[fp]; phit[fp]=nextgood(pat,phit[fp])){

       if (_NoMoreTime()) { 
         cout << " AMSVtx: Cpulimit Exceeded " << endl;
         return remove_track(ptrack); 
	 }

       // Parameters to check distance to a straight line
	 number par[2][3];
       par[0][0]=(phit[fp]->getHit()[0] - phit[0]->getHit()[0])/
              (phit[fp]->getHit()[2] - phit[0]->getHit()[2]);
       par[0][1]=phit[0]->getHit()[0] - par[0][0]*phit[0]->getHit()[2];
       par[0][2]=sqrt(1+par[0][0]*par[0][0]);
       par[1][0]=(phit[fp]->getHit()[1] - phit[0]->getHit()[1])/
              (phit[fp]->getHit()[2] - phit[0]->getHit()[2]);
       par[1][1]=phit[0]->getHit()[1] - par[1][0]*phit[0]->getHit()[2];
       par[1][2]=sqrt(1+par[1][0]*par[1][0]);
       
       // Initial hit choice on intermediate layers
	 int nhits_in_path = 0;
       for (int ilay=1;ilay<fp;ilay++){
         phit[ilay]=firstgood_path(pat,ilay,par);
         if (phit[ilay]==NULL) break;
	   nhits_in_path++;
       }
	 if (nhits_in_path != fp-1) continue;

       // Get the combination with the best chi2_per_dof
	 number chi2_per_dof;

	 AMSTrTrack ptest = AMSTrTrack(pat, fp+1, phit);
       while(1){

         chi2_per_dof = AMSVtx::Test(&ptest);
	   if (chi2_per_dof<minchi2 && chi2_per_dof>0){
           minchi2 = chi2_per_dof;
           ptrack = copy_track(&ptest);
           /*
	     cout << " OK, #hits: " << fp+1;
	     cout << " chi2/ndof: " << chi2_per_dof;
	     cout << endl;
	     for (int i=0;i<ptrack->getnhits();i++){
	       cout << "        " << ptrack->getphit(i)->getHit()[0];
	       cout << ", " << ptrack->getphit(i)->getHit()[1];
	       cout << ", " << ptrack->getphit(i)->getHit()[2];
	       cout << endl;
	     }
           */
	   }

         // Try next combination of hits
         // Stop when done
         if (_NoMoreTime()) { 
           cout << " AMSVtx: Cpulimit Exceeded " << endl;
           return remove_track(ptrack); 
	   }

         // Stop when no more intermediate hit combinations are left...
         if ( !AMSVtx::next_combination(ptest,1,fp-1,par) ) break;

       }

     }
    }

    // Wait for next pattern only if the number of hits is the same...
    if (ptrack && pat+1<TKDBc::npat()) {
      if (TKDBc::patpoints(pat+1)<TKDBc::patpoints(pat)) break;
    }

  }

  // End of analysis; return ptrack if succesful
  if (ptrack){ 
    // cout << "Marking hits !!" << endl;
    int pat = ptrack->getpattern();
    int nhits = ptrack->getnhits();
    for (int i=0;i<nhits;i++){
      phit[i] = ptrack->getphit(i);
    }
    ptrack->Fit(5,2);
    AMSTrTrack::_addnextR(ptrack, pat, nhits, phit);
    for(int i=0;i<nhits;i++){
      AMSTrCluster* py = phit[i]->getClusterP(1);
      if (py){
        AMSTrRecHit* paux = AMSTrRecHit::gethead(phit[i]->getLayer()-1);
        while (paux) {
          if (py==paux->getClusterP(1)) paux->setstatus(AMSDBc::RECOVERED);
          paux = paux->next();
        }
      }
    }
    return ptrack;

  } else {

    return remove_track(ptrack); 

  }

}

///////////////////////////////////////////////////////////
AMSTrTrack* AMSVtx::copy_track(AMSTrTrack* ptrin){

        
  static AMSTrTrack* ptrout = NULL;
  AMSTrRecHit* phit[trconst::maxlay];
  
  if (ptrin) {
    int nhits = ptrin->getnhits();
    for (int i=0; i<nhits; i++) { 
       phit[i] = ptrin->getphit(i);
    }
    int patt = ptrin->getpattern();
    ptrout = new AMSTrTrack(patt, nhits, phit);

  }

  return ptrout;

}

///////////////////////////////////////////////////////////
AMSTrTrack* AMSVtx::remove_track(AMSTrTrack* ptrack){
  if (ptrack) {
    for(int i=0;;i++){
      AMSContainer *pctr=AMSEvent::gethead()->getC("AMSTrTrack",i);
      if(pctr==NULL) break;
      for (AMSlink* ptr=pctr->gethead(); ptr!=NULL; ptr=ptr->next()){
        if (ptr->next()==ptrack) {
          pctr->removeEl(ptr,1);
          break; 
        }
      }
    }
  }
  delete ptrack;
  ptrack = NULL;
  return ptrack;

}

///////////////////////////////////////////////////////////
integer AMSVtx::next_combination(AMSTrTrack& ptrack, int index_min, int index_max, number par[2][3]){
         if (index_min<0) return 0;
         int pattern = ptrack.getpattern();
         if (index_max>=TKDBc::patpoints(pattern)) return 0;
	   int index_run = index_min;
next_index:
         ptrack._Pthit[index_run] = AMSVtx::nextgood_path(pattern,ptrack._Pthit[index_run],par);
         if (!ptrack._Pthit[index_run]){
	       if (index_run>=index_max) return 0;
             ptrack._Pthit[index_run] = AMSVtx::firstgood_path(pattern,index_run,par);
             index_run++;
	       goto next_index;
         }

	   return 1;
}

///////////////////////////////////////////////////////////
number AMSVtx::Test(AMSTrTrack* ptrack){
//*******************************************************************
//
//    Filling VtxTrack information in the fastest way
//
//    Fast determination of relevant parameters for a track initialized
//    with:
//       ptrack = AMSTrTrack(int pattern, int nhits, AMSTrRecHit* phit[])
//
//    Fitted parameters: position, direction and rigidity (cm, GeV)
//
//    NO MULTIPLE SCATTERING TREATMENT in this case
//
//    Information already available:
	  //    integer nhits = ptrack->getnhits();
	  //    Layer i: AMSTrRecHit * phit[i] = ptrack->getphit(i);
//
//    Information internally filled:
//    
	  //    number Chi2;
	  //    integer Ndof;
	  //    number PathIntegral_x[trconst::maxlay][3];
	  //    number PathIntegral_u[trconst::maxlay][3];
	  //    number Param[5];
	  //    number Covariance[5][5];
//
//    Output is Chi2/Ndof if everything is OK; -999.d0 otherwise
//
//    Written for AMS-02, J. Alcaraz (May 2003)
//
//*******************************************************************

// Consistency check on the number of hits
  int nhits = ptrack->getnhits();
  if (nhits<3 || nhits>trconst::maxlay) {
    return -999.;
  }

// Get hit positions and uncertainties
// Scale errors (we will use sigmas in microns)
  geant hits[trconst::maxlay][3];
  geant sigma[trconst::maxlay][3];
  for (int i=0;i<nhits;i++){
    AMSTrRecHit* phit = ptrack->getphit(i);
    for (int j=0;j<3;j++){
      hits[i][j]=phit->getHit()[j];
      sigma[i][j]=1.e4*phit->getEHit()[j];
    }
  }

// Calculate path integrals
  number PathIntegral_x[trconst::maxlay][3];
  number PathIntegral_u[trconst::maxlay][3];
  for (int i=0;i<nhits;i++) {
    for (int j=0;j<3;j++) {
      PathIntegral_x[i][j] = 0.;
      PathIntegral_u[i][j] = 0.;
    }
  }

  for (int i=1;i<nhits;i++){
    number unorm = sqrt( (hits[i][0]-hits[i-1][0])*(hits[i][0]-hits[i-1][0])
                       +(hits[i][1]-hits[i-1][1])*(hits[i][1]-hits[i-1][1])
                       +(hits[i][2]-hits[i-1][2])*(hits[i][2]-hits[i-1][2]) );

    integer ntot=20;
    for (int l=0;l<ntot;l++){
      number alpha = float(l)/ntot;

	number fact;
      if (l==0 || l==ntot) {
        fact = 1.;
	}
      else if ((l%2)==1) {
        fact = 4.;
	}
      else {
        fact = 2.;
	}

      geant x[3], u[3], b[3];
      for (int j=0;j<3;j++){
        x[j] = hits[i-1][j] + alpha*(hits[i][j]-hits[i-1][j]);
        u[j] = (hits[i][j]-hits[i-1][j])/unorm;
      }
      GUFLD(x,b);

      PathIntegral_x[i][0] += fact*(1.-alpha)*(u[1]*b[2]-u[2]*b[1]);
      PathIntegral_x[i][1] += fact*(1.-alpha)*(u[2]*b[0]-u[0]*b[2]);
      PathIntegral_x[i][2] += fact*(1.-alpha)*(u[0]*b[1]-u[1]*b[0]);

      PathIntegral_u[i][0] += fact*(u[1]*b[2]-u[2]*b[1]);
      PathIntegral_u[i][1] += fact*(u[2]*b[0]-u[0]*b[2]);
      PathIntegral_u[i][2] += fact*(u[0]*b[1]-u[1]*b[0]);
    }

    PathIntegral_x[i][0] /= 3.*ntot;
    PathIntegral_x[i][1] /= 3.*ntot;
    PathIntegral_x[i][2] /= 3.*ntot;
    PathIntegral_u[i][0] /= 3.*ntot;
    PathIntegral_u[i][1] /= 3.*ntot;
    PathIntegral_u[i][2] /= 3.*ntot;

  }

// Lenghts
  number len[trconst::maxlay];
  for (int i=0;i<nhits;i++){
    if (i==0){
      len[i] = 0.;
    }
    else {
      len[i] = sqrt( (hits[i][0]-hits[i-1][0])*(hits[i][0]-hits[i-1][0])
                    +(hits[i][1]-hits[i-1][1])*(hits[i][1]-hits[i-1][1])
                    +(hits[i][2]-hits[i-1][2])*(hits[i][2]-hits[i-1][2]) );
    }
  }

// F and G matrices
  number d[2*trconst::maxlay][5];
  for (int i=0;i<nhits;i++) {
    int ix = i;
    int iy = i+nhits;
    for (int j=0;j<5;j++) { d[ix][j] = 0; d[iy][j] = 0;}
    d[ix][0] = 1.;
    d[iy][1] = 1.;
    for (int k=0;k<=i;k++) {
	d[ix][2] += len[k];
	d[iy][3] += len[k];
      for (int l=0;l<=k;l++) {
        if (l==k) {
          d[ix][4] += len[k]*len[k]*PathIntegral_x[k][0];
          d[iy][4] += len[k]*len[k]*PathIntegral_x[k][1];
        }
        else {
          d[ix][4] += len[k]*len[l]*PathIntegral_u[l][0];
          d[iy][4] += len[k]*len[l]*PathIntegral_u[l][1];
        }
	}
    }
  }

// F*S_x*x + G*S_y*y
  number dx[5];
  for (int j=0;j<5;j++) {
    dx[j] = 0.;
    for (int l=0;l<nhits;l++) {
      dx[j] += d[l][j]/sigma[l][0]/sigma[l][0]*hits[l][0];
      dx[j] += d[l+nhits][j]/sigma[l][1]/sigma[l][1]*hits[l][1];
    }
  }

// (F*S_x*F + G*S_y*G)
  
  number Param[5];
  number Covariance[5][5];
  for (int j=0;j<5;j++) {
    for (int k=0;k<5;k++) {
      Covariance[j][k] = 0.;
      for (int l=0;l<nhits;l++) {
        Covariance[j][k] += d[l][j]/sigma[l][0]/sigma[l][0]*d[l][k];
        Covariance[j][k] += d[l+nhits][j]/sigma[l][1]/sigma[l][1]*d[l+nhits][k];
      } 
    }
  }
        
// (F*S_x*F + G*S_y*G)**{-1}
  int ifail;
  int idim = 5;
  INVERTMATRIX((double*)Covariance, idim, idim, ifail);
  if (ifail) {
    cout << " At AMSVtx::Test... Covariance Matrix is Singular " << endl;
    Param[4]=0;
    return -999.;
  }

// Solution
  for (int k=0;k<5;k++) {
    Param[k] = 0.;
    for (int i=0;i<5;i++) {
      Param[k] += Covariance[k][i]*dx[i];
    }
  }
  
// Chi2 (xl and yl in microns, since sigmas are in microns too)
  number Chi2 = 0.;
  for (int l=0;l<nhits;l++) {
    number xl = hits[l][0]*1.e4;
    number yl = hits[l][1]*1.e4;
    for (int k=0;k<5;k++) {
      xl -= d[l][k]*Param[k]*1.e4;
      yl -= d[l+nhits][k]*Param[k]*1.e4;
    }
    Chi2 += xl/sigma[l][0]/sigma[l][0]*xl + yl/sigma[l][1]/sigma[l][1]*yl;
  }

// Return Chi2/Ndof
  return Chi2/(2.*nhits-5.);

}
