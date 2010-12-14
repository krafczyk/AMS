#include <tracking.h>
#include <TDecompChol.h>
#include <TMatrixDSym.h>

//#############################################################################
TrHitA::TrHitA(float* coo, float* ecoo, float* bfield) {
      for (int i=0; i<3; i++) {
            Coo[i] = coo[i];
            ECoo[i] = ecoo[i];
            Bfield[i] = bfield[i];
      }
      _Link = NULL;
      _Type = "";
}

//#############################################################################
TrHitA::TrHitA(double* coo, double* ecoo, double* bfield) {
      for (int i=0; i<3; i++) {
            Coo[i] = coo[i];
            ECoo[i] = ecoo[i];
            Bfield[i] = bfield[i];
      }
      _Link = NULL;
      _Type = "";
}

//#############################################################################
TrHitA::TrHitA(TrRecHitR* phit) {
      for (int i=0; i<3; i++) {
            Coo[i] = phit->Hit[i];
            ECoo[i] = phit->EHit[i];
            Bfield[i] = phit->Bfield[i];
      }
      _Link = (void*)phit;
      _Type = "TrRecHitR";
}

//#############################################################################
TrTrackA::TrTrackA(TrTrackR* ptr) : _PathIntExist(false) {
      if (!ptr) return;
      for (int i=0; i<ptr->NTrRecHit(); i++) {
            _Hit.push_back( TrHitA(ptr->pTrRecHit(i)));
      }
}

//#############################################################################
void TrTrackA::use_hits_from(TrTrackR* ptr) {
      if (!ptr) return;
      _Hit.clear();
      _PathIntExist = false;
      for (int i=0; i<ptr->NTrRecHit(); i++) {
            _Hit.push_back( TrHitA(ptr->pTrRecHit(i)));
      }
}

//#############################################################################
TrHitA* TrTrackA::add_hit(float* coo, float* ecoo, float* bfield) {
      int index = 0;
      for (int i=0; i<NHits(); i++) {
            if (coo[2]<_Hit[i].Coo[2]) index++;
            else break;
      }
      _Hit.insert(_Hit.begin()+index, TrHitA(coo, ecoo, bfield));
      _PathIntExist = false;
      return &_Hit[index];
}

//#############################################################################
TrHitA* TrTrackA::add_hit(double* coo, double* ecoo, double* bfield) {
      int index = 0;
      for (int i=0; i<NHits(); i++) {
            if (coo[2]<_Hit[i].Coo[2]) index++;
            else break;
      }
      _Hit.insert(_Hit.begin()+index, TrHitA(coo, ecoo, bfield));
      _PathIntExist = false;
      return &_Hit[index];
}

//#############################################################################
TrHitA* TrTrackA::add_hit(TrRecHitR* phit) {
      int index = 0;
      for (int i=0; i<NHits(); i++) {
            if (phit->Hit[2]<_Hit[i].Coo[2]) index++;
            else break;
      }
      _Hit.insert(_Hit.begin()+index, TrHitA(phit));
      _PathIntExist = false;
      return &_Hit[index];
}

//#############################################################################
void TrTrackA::del_hit(TrHitA* phit) {
      for(int i=0;i<NHits();i++) {
            if (phit==&_Hit[i]) {
                 _Hit.erase(_Hit.begin()+i);
                 _PathIntExist = false;
                 break;
            }
      }
}

//#############################################################################
void TrTrackA::del_hit(int index) {
      if (index>=0 && index<NHits()) {
            _Hit.erase(_Hit.begin()+index);
            _PathIntExist = false;
      }
}

//#############################################################################
void TrTrackA::Clear() {
      _Hit.clear();
      _PathIntExist = false;
}

//#############################################################################
void TrTrackA::SetPathInt(){

  // Lengths
  int nhits = _Hit.size();
  _PathLength.clear();
  for (int i=0;i<nhits;i++) {
      if (i==0){
            _PathLength.push_back(0.);
      } else {
            double len = sqrt(  pow(_Hit[i].Coo[0]-_Hit[i-1].Coo[0],2)
                              + pow(_Hit[i].Coo[1]-_Hit[i-1].Coo[1],2)
                              + pow(_Hit[i].Coo[2]-_Hit[i-1].Coo[2],2) );
            _PathLength.push_back(len);
      }
      /*
      printf ("Hit %d, path_length %.5f\n", i, _PathLength[i]);
      */
  }

  // Calculate path integrals
  for (int k=0;k<3;k++){
      _PathIntegral_x[k].clear();
      _PathIntegral_u[k].clear();
      for (int i=0;i<nhits;i++){
            _PathIntegral_x[k].push_back(0.);
            _PathIntegral_u[k].push_back(0.);
      }
  }

  for (int i=1;i<nhits;i++){

            double ba[3], bave[3], u[3];
            for (int k=0; k<3; k++) {
                  ba[k] = _Hit[i-1].Bfield[k];
                  bave[k] = 0.5*(_Hit[i-1].Bfield[k]+_Hit[i].Bfield[k]);
                  u[k] = (_Hit[i].Coo[k]-_Hit[i-1].Coo[k])/_PathLength[i];
            }

            _PathIntegral_x[0][i] = (u[1]*ba[2]-u[2]*ba[1])/2;
            _PathIntegral_x[1][i] = (u[2]*ba[0]-u[0]*ba[2])/2;
            _PathIntegral_x[2][i] = (u[0]*ba[1]-u[1]*ba[0])/2;
      
            _PathIntegral_u[0][i] = u[1]*bave[2]-u[2]*bave[1];
            _PathIntegral_u[1][i] = u[2]*bave[0]-u[0]*bave[2];
            _PathIntegral_u[2][i] = u[0]*bave[1]-u[1]*bave[0];
           
  }

  _PathIntExist = true;
          
}

//#############################################################################
int TrTrackA::SimpleFit(){

    int _Nhit = _Hit.size();

  // Reset fit values
    Chi2 = FLT_MAX;
    Ndof = 2*_Nhit-5;
    Theta = 0.0;
    Phi = 0.0;
    U[0] = 0.0;
    U[1] = 0.0;
    U[2] = 1.0;
    P0[0] = 0.0;
    P0[1] = 0.0;
    P0[2] = 0.0;
    Rigidity = FLT_MAX;

  // Consistency check on the number of hits
    if (_Nhit<3) return -2;

  // Scale errors (we will use sigmas in microns)
    double hits[_Nhit][3];
    double sigma[_Nhit][3];
    for (int i=0;i<_Nhit;i++){
      for (int j=0;j<3;j++){
          hits[i][j] = _Hit[i].Coo[j];
          sigma[i][j] = 1.e4*_Hit[i].ECoo[j];
      }
    }

    if (!_PathIntExist) SetPathInt();


  // F and G matrices
    const int idim = 5;
    double d[2*_Nhit][idim];
    for (int i=0;i<_Nhit;i++) {
      int ix = i;
      int iy = i+_Nhit;
      for (int j=0;j<idim;j++) { d[ix][j] = 0; d[iy][j] = 0;}
      d[ix][0] = 1.;
      d[iy][1] = 1.;
      for (int k=0;k<=i;k++) {
        d[ix][2] += _PathLength[k];
        d[iy][3] += _PathLength[k];
        d[ix][4] += _PathLength[k]*_PathLength[k]*_PathIntegral_x[0][k];
        d[iy][4] += _PathLength[k]*_PathLength[k]*_PathIntegral_x[1][k];
        for (int l=k+1;l<=i;l++) {
            d[ix][4] += _PathLength[k]*_PathLength[l]*_PathIntegral_u[0][k];
            d[iy][4] += _PathLength[k]*_PathLength[l]*_PathIntegral_u[1][k];
        }
      }
    }

  // F*S_x*x + G*S_y*y
    double dx[idim];
    for (int j=0;j<idim;j++) {
      dx[j] = 0.;
      for (int l=0;l<_Nhit;l++) {
        dx[j] += d[l][j]/sigma[l][0]/sigma[l][0]*hits[l][0];
        dx[j] += d[l+_Nhit][j]/sigma[l][1]/sigma[l][1]*hits[l][1];
      }
    }

  // (F*S_x*F + G*S_y*G)

    double Param[idim];
    TMatrixDSym ParaCovariance(idim);
    //printf(">>>>>>>>>>>>> Cov BEFORE:\n");
    for (int j=0;j<idim;j++) {
      for (int k=0;k<idim;k++) {
        ParaCovariance(j,k) = 0.;
        for (int l=0;l<_Nhit;l++) {
          ParaCovariance(j,k) += d[l][j]/sigma[l][0]/sigma[l][0]*d[l][k];
          ParaCovariance(j,k) += d[l+_Nhit][j]/sigma[l][1]/sigma[l][1]*d[l+_Nhit][k];
        } 
        //printf(" %f", ParaCovariance[j][k]);
      }
      //printf("\n");
    }
      
  // (F*S_x*F + G*S_y*G)**{-1}
    TDecompChol decomp(ParaCovariance);
    ParaCovariance = decomp.Invert();

  // Solution
    //printf(">>>>>>>>>>>>> Cov AFTER:\n");
    for (int k=0;k<idim;k++) {
      Param[k] = 0.;
      for (int i=0;i<idim;i++) {
        Param[k] += ParaCovariance[k][i]*dx[i];
        //printf(" %f", ParaCovariance[k][i]);
      }
      //printf("\n");
    }

  // Chi2 (xl and yl in microns, since sigmas are in microns too)
    Chi2 = 0.;
    for (int l=0;l<_Nhit;l++) {
      double xl = hits[l][0]*1.e4;
      double yl = hits[l][1]*1.e4;
      for (int k=0;k<idim;k++) {
        xl -= d[l][k]*Param[k]*1.e4;
        yl -= d[l+_Nhit][k]*Param[k]*1.e4;
      }
      Chi2 += xl/sigma[l][0]/sigma[l][0]*xl + yl/sigma[l][1]/sigma[l][1]*yl;
    }

  // Final results
    P0[0] = Param[0];
    P0[1] = Param[1];
    P0[2] = hits[0][2];
    Phi = atan2(Param[3],Param[2]);
    Theta = acos(-sqrt(1-Param[2]*Param[2]-Param[3]*Param[3]));
    U[0] = sin(Theta)*cos(Phi);
    U[1] = sin(Theta)*sin(Phi);
    U[2] = cos(Theta);

    if (Param[4]!=0.0 && ParaCovariance[4][4]>0.0) {
        Rigidity = 2.997E-4/Param[4];
    } else {
        Rigidity = FLT_MAX;
    }

    return 0;

}

//#############################################################################
int TrTrackA::StraightLineFit(){

    int _Nhit = _Hit.size();

  // Reset fit values
    Chi2 = FLT_MAX;
    Ndof = 2*_Nhit-4;
    Theta = 0.0;
    Phi = 0.0;
    U[0] = 0.0;
    U[1] = 0.0;
    U[2] = 1.0;
    P0[0] = 0.0;
    P0[1] = 0.0;
    P0[2] = 0.0;
    Rigidity = FLT_MAX;

  // Consistency check on the number of hits
    if (_Nhit<3) return -2;
    P0[2] = _Hit[0].Coo[2];

  // Get hit positions and uncertainties
  // Scale errors (we will use sigmas in microns)
    double hits[_Nhit][3];
    double sigma[_Nhit][3];
    for (int i=0;i<_Nhit;i++){
      for (int j=0;j<3;j++){
          hits[i][j] = _Hit[i].Coo[j];
          sigma[i][j] = 1.e4*_Hit[i].ECoo[j];
      }
    }

  // Lenghts
    double lenz[_Nhit];
    for (int i=0;i<_Nhit;i++) lenz[i] = hits[i][2] - P0[2];

  // F and G matrices
    const int idim = 4;
    double d[2*_Nhit][idim];
    for (int i=0;i<_Nhit;i++) {
      int ix = i;
      int iy = i+_Nhit;
      for (int j=0;j<idim;j++) { d[ix][j] = 0; d[iy][j] = 0;}
      d[ix][0] = 1.;
      d[iy][1] = 1.;
      d[ix][2] = lenz[i];
      d[iy][3] = lenz[i];
    }

  // F*S_x*x + G*S_y*y
    double dx[idim];
    for (int j=0;j<idim;j++) {
      dx[j] = 0.;
      for (int l=0;l<_Nhit;l++) {
        dx[j] += d[l][j]/sigma[l][0]/sigma[l][0]*hits[l][0];
        dx[j] += d[l+_Nhit][j]/sigma[l][1]/sigma[l][1]*hits[l][1];
      }
    }

  // (F*S_x*F + G*S_y*G)
    double Param[idim];
    double InvCov[idim][idim];
    for (int j=0;j<idim;j++) {
      for (int k=0;k<idim;k++) {
        InvCov[j][k] = 0.;
        for (int l=0;l<_Nhit;l++) {
          InvCov[j][k] += d[l][j]/sigma[l][0]/sigma[l][0]*d[l][k];
          InvCov[j][k] += d[l+_Nhit][j]/sigma[l][1]/sigma[l][1]*d[l+_Nhit][k];
        } 
      }
    }
      
  // (F*S_x*F + G*S_y*G)**{-1}
    double determ = 0.0;
    TMatrixD ParaCovariance = TMatrixD(idim,idim,(Double_t*)InvCov," ");
    ParaCovariance = ParaCovariance.Invert(&determ);
    if (determ<=0) return -1;

  // Solution
    for (int k=0;k<idim;k++) {
      Param[k] = 0.;
      for (int i=0;i<idim;i++) {
        Param[k] += ParaCovariance(k,i)*dx[i];
      }
    }

  // Chi2 (xl and yl in microns, since sigmas are in microns too)
    Chi2 = 0.;
    for (int l=0;l<_Nhit;l++) {
      double xl = hits[l][0]*1.e4;
      double yl = hits[l][1]*1.e4;
      for (int k=0;k<idim;k++) {
        xl -= d[l][k]*Param[k]*1.e4;
        yl -= d[l+_Nhit][k]*Param[k]*1.e4;
      }
      Chi2 += xl/sigma[l][0]/sigma[l][0]*xl + yl/sigma[l][1]/sigma[l][1]*yl;
    }

  // Final result
    P0[0] = Param[0];
    P0[1] = Param[1];
    Phi = atan2(Param[3],Param[2]);
    Theta = acos(-sqrt(1-Param[2]*Param[2]-Param[3]*Param[3]));
    U[0] = sin(Theta)*cos(Phi);
    U[1] = sin(Theta)*sin(Phi);
    U[2] = cos(Theta);

    return 0;

}

//#############################################################################
double* TrTrackA::Prediction(TrHitA* phit){
      for(int i=0;i<NHits();i++) {
            if (phit==&_Hit[i]) return Prediction(i);
      }
      return NULL;
}

//#############################################################################
double* TrTrackA::Prediction(double* coo, double* ecoo, double* bfield) {
      TrHitA* myhit = add_hit(coo, ecoo, bfield);
      double* result = Prediction(myhit);
      del_hit(myhit);
      return result;
}

//#############################################################################
double* TrTrackA::Prediction(TrRecHitR* phit){
      if (!phit) return NULL;
      TrHitA* myhit = add_hit(phit);
      double* result = Prediction(myhit);
      del_hit(myhit);
      return result;
}

//#############################################################################
double* TrTrackA::PredictionStraightLine(TrHitA* phit){
      for(int i=0;i<NHits();i++) {
            if (phit==&_Hit[i]) return PredictionStraightLine(i);
      }
      return NULL;
}

//#############################################################################
double* TrTrackA::PredictionStraightLine(double* coo, double* ecoo, double* bfield) {
      TrHitA* myhit = add_hit(coo, ecoo, bfield);
      double* result = PredictionStraightLine(myhit);
      del_hit(myhit);
      return result;
}

//#############################################################################
double* TrTrackA::PredictionStraightLine(TrRecHitR* phit){
      if (!phit) return NULL;
      TrHitA* myhit = add_hit(phit);
      double* result = PredictionStraightLine(myhit);
      del_hit(myhit);
      return result;
}

//#############################################################################
double* TrTrackA::Prediction(int index){

    static double pred[7];

    int _Nhit = _Hit.size();

  // Consistency check on the number of hits
    if (_Nhit<4) return NULL;

  // Scale errors (we will use sigmas in microns)
    double hits[_Nhit][3];
    double sigma[_Nhit][3];
    for (int i=0;i<_Nhit;i++){
      for (int j=0;j<3;j++){
          hits[i][j] = _Hit[i].Coo[j];
          sigma[i][j] = 1.e4*_Hit[i].ECoo[j];
          if (i==index) sigma[i][j] *= 1.e2;
      }
    }

    if (!_PathIntExist) SetPathInt();


  // F and G matrices
    const int idim = 5;
    double d[2*_Nhit][idim];
    for (int i=0;i<_Nhit;i++) {
      int ix = i;
      int iy = i+_Nhit;
      for (int j=0;j<idim;j++) { d[ix][j] = 0; d[iy][j] = 0;}
      d[ix][0] = 1.;
      d[iy][1] = 1.;
      for (int k=0;k<=i;k++) {
        d[ix][2] += _PathLength[k];
        d[iy][3] += _PathLength[k];
        d[ix][4] += _PathLength[k]*_PathLength[k]*_PathIntegral_x[0][k];
        d[iy][4] += _PathLength[k]*_PathLength[k]*_PathIntegral_x[1][k];
        for (int l=k+1;l<=i;l++) {
            d[ix][4] += _PathLength[k]*_PathLength[l]*_PathIntegral_u[0][k];
            d[iy][4] += _PathLength[k]*_PathLength[l]*_PathIntegral_u[1][k];
        }
      }
    }

  // F*S_x*x + G*S_y*y
    double dx[idim];
    for (int j=0;j<idim;j++) {
      dx[j] = 0.;
      for (int l=0;l<_Nhit;l++) {
        dx[j] += d[l][j]/sigma[l][0]/sigma[l][0]*hits[l][0];
        dx[j] += d[l+_Nhit][j]/sigma[l][1]/sigma[l][1]*hits[l][1];
      }
    }

  // (F*S_x*F + G*S_y*G)

    double Param[idim];
    double InvCov[idim][idim];
    for (int j=0;j<idim;j++) {
      for (int k=0;k<idim;k++) {
        InvCov[j][k] = 0.;
        for (int l=0;l<_Nhit;l++) {
          InvCov[j][k] += d[l][j]/sigma[l][0]/sigma[l][0]*d[l][k];
          InvCov[j][k] += d[l+_Nhit][j]/sigma[l][1]/sigma[l][1]*d[l+_Nhit][k];
        } 
      }
    }
      
  // (F*S_x*F + G*S_y*G)**{-1}
    double determ = 0.0;
    TMatrixD ParaCovariance = TMatrixD(idim,idim,(Double_t*)InvCov," ");
    ParaCovariance = ParaCovariance.Invert(&determ);
    if (determ<=0) return NULL;

  // Solution
    //printf(">>>>>>>>>>>>> Cov AFTER:\n");
    for (int k=0;k<idim;k++) {
      Param[k] = 0.;
      for (int i=0;i<idim;i++) {
        Param[k] += ParaCovariance[k][i]*dx[i];
        //printf(" %f", ParaCovariance[k][i]);
      }
      //printf("\n");
    }

  // Chi2 (xl and yl in microns, since sigmas are in microns too)
    pred[0] = 0;
    pred[1] = 0;
    pred[2] = hits[index][2];
    pred[3] = acos(-sqrt(1-Param[2]*Param[2]-Param[3]*Param[3]));
    pred[4] = atan2(Param[3],Param[2]);
    pred[5] = 0.;
    pred[6] = 0.;
    for (int k=0;k<idim;k++) {
      pred[0] += d[index][k]*Param[k];
      pred[1] += d[index+_Nhit][k]*Param[k];
      for (int l=0;l<idim;l++) {
        pred[5] += d[index][k]*d[index][l]*ParaCovariance(k,l);
        pred[6] += d[index+_Nhit][k]*d[index+_Nhit][l]*ParaCovariance(k,l);
      }
    }
    if (pred[5]>0.) pred[5] = 1.e-4*sqrt(pred[5]); else pred[5]=0.;
    if (pred[6]>0.) pred[6] = 1.e-4*sqrt(pred[6]); else pred[6]=0.;

    return pred;
    
}

//#############################################################################
double* TrTrackA::PredictionStraightLine(int index){

    static double pred[7];

    int _Nhit = _Hit.size();

  // Consistency check on the number of hits
    if (_Nhit<3) return NULL;

  // Get hit positions and uncertainties
  // Scale errors (we will use sigmas in microns)
    double hits[_Nhit][3];
    double sigma[_Nhit][3];
    for (int i=0;i<_Nhit;i++){
      for (int j=0;j<3;j++){
          hits[i][j] = _Hit[i].Coo[j];
          sigma[i][j] = 1.e4*_Hit[i].ECoo[j];
          if (i==index) sigma[i][j] *= 1.e2;
      }
    }

  // Lenghts
    double lenz[_Nhit];
    for (int i=0;i<_Nhit;i++) lenz[i] = hits[i][2] - _Hit[0].Coo[2];

  // F and G matrices
    const int idim = 4;
    double d[2*_Nhit][idim];
    for (int i=0;i<_Nhit;i++) {
      int ix = i;
      int iy = i+_Nhit;
      for (int j=0;j<idim;j++) { d[ix][j] = 0; d[iy][j] = 0;}
      d[ix][0] = 1.;
      d[iy][1] = 1.;
      d[ix][2] = lenz[i];
      d[iy][3] = lenz[i];
    }

  // F*S_x*x + G*S_y*y
    double dx[idim];
    for (int j=0;j<idim;j++) {
      dx[j] = 0.;
      for (int l=0;l<_Nhit;l++) {
        dx[j] += d[l][j]/sigma[l][0]/sigma[l][0]*hits[l][0];
        dx[j] += d[l+_Nhit][j]/sigma[l][1]/sigma[l][1]*hits[l][1];
      }
    }

  // (F*S_x*F + G*S_y*G)
    double Param[idim];
    double InvCov[idim][idim];
    for (int j=0;j<idim;j++) {
      for (int k=0;k<idim;k++) {
        InvCov[j][k] = 0.;
        for (int l=0;l<_Nhit;l++) {
          InvCov[j][k] += d[l][j]/sigma[l][0]/sigma[l][0]*d[l][k];
          InvCov[j][k] += d[l+_Nhit][j]/sigma[l][1]/sigma[l][1]*d[l+_Nhit][k];
        } 
      }
    }
      
  // (F*S_x*F + G*S_y*G)**{-1}
    double determ = 0.0;
    TMatrixD ParaCovariance = TMatrixD(idim,idim,(Double_t*)InvCov," ");
    ParaCovariance = ParaCovariance.Invert(&determ);
    if (determ<=0) return NULL;

  // Solution
    for (int k=0;k<idim;k++) {
      Param[k] = 0.;
      for (int i=0;i<idim;i++) {
        Param[k] += ParaCovariance(k,i)*dx[i];
      }
    }

  // Chi2 (xl and yl in microns, since sigmas are in microns too)
    pred[0] = 0;
    pred[1] = 0;
    pred[2] = _Hit[index].Coo[2];
    pred[3] = acos(-sqrt(1-Param[2]*Param[2]-Param[3]*Param[3]));
    pred[4] = atan2(Param[3],Param[2]);
    pred[5] = 0.;
    pred[6] = 0.;
    for (int k=0;k<idim;k++) {
      pred[0] += d[index][k]*Param[k];
      pred[1] += d[index+_Nhit][k]*Param[k];
      for (int l=0;l<idim;l++) {
        pred[5] += d[index][k]*d[index][l]*ParaCovariance(k,l);
        pred[6] += d[index+_Nhit][k]*d[index+_Nhit][l]*ParaCovariance(k,l);
      }
    }
    if (pred[5]>0.) pred[5] = 1.e-4*sqrt(pred[5]); else pred[5]=0.;
    if (pred[6]>0.) pred[6] = 1.e-4*sqrt(pred[6]); else pred[6]=0.;

    return pred;

}

