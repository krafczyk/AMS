/*! \example SimpleFit.h
 *  Usage:
 * 
 *        - Add this file to the list of "includes":  
 *              - #include <file:/afs/ams.cern.ch/Offline/vdev/examples/SimpleFit.h>
 * 
 *        - Constructors:
 *              - SimpleTrack new_track;
 *              - SimpleTrack new_track = SimpleTrack(int nhits);
 *              - SimpleTrack new_track = SimpleTrack(int nhits, TrRecHitR* phit[]);
 * 
 *        - Available methods:
 *              - new_track.addHit(phit);
 *              - new_track.delHit(phit);
 *              - new_track.addHit(i);
 *              - new_track.delHit(i);
 *              - istat = new_track.SimpleFit(); 
 *                    - istat=0 ==> OK
 *                    - istat<0 ==> not succesful
 *               
 *        - Publicly accessible data:
 *              - int NHits;             
 *                    - 0<= NHits <=MAXLAY
 *              - TrRecHitR* pHit[i];    
 *                    - i from 0 to NHits-1
 *              - float Chi2StrLine;     
 *                    - Chi2/ndof in the XZ projection
 *              - float Chi2;            
 *                    - Chi2/ndof of fit
 *              - float Rigidity;        
 *                    - In GV
 *              - float ErrRigidity;     
 *                    - Error on 1/Rigidity   (1/GV)
 *              - float Theta;           
 *                    - Theta at first layer
 *              - float Phi;             
 *                    - Phi at first layer
 *              - float P0[3];           
 *                    - Reference point (cm, first layer)
 */
#include </afs/ams.cern.ch/Offline/vdev/include/root.h>
#include "TMatrixD.h"
#include <values.h>

class SimpleTrack {
public:

  static const int MAXLAY=8;
  int NHits;
  TrRecHitR* pHit[MAXLAY];
  float Chi2StrLine;  
  float Chi2;  
  float Rigidity;  
  float ErrRigidity;
  float Theta;
  float Phi;
  float P0[3];

  SimpleTrack(int nhits=0, TrRecHitR* phit[]=NULL):NHits(nhits){
        for(int i=0;i<nhits;i++) pHit[i]=phit[i];
  };

  void addHit(TrRecHitR* phit);
  void delHit(TrRecHitR* phit);
  void addHit(unsigned int ihit);
  void delHit(unsigned int ihit);
  int SimpleFit();

};

void SimpleTrack::addHit(TrRecHitR* phit) {
        if (NHits<MAXLAY) { 
                pHit[NHits] = phit; 
                NHits++;
        } else { 
                cout << "SimpleTrack WARNING: Can not add more than ";
                cout << MAXLAY << " hits; new hit ignored"; 
                cout << endl;
        }
};

void SimpleTrack::delHit(TrRecHitR* phit) {
        for(int i=0;i<NHits;i++) {
            if (pHit[i]==phit) {
                 for(int j=i+1;j<NHits;j++) pHit[j-1] = pHit[j];
                 NHits--;
                 return;
            }
        }
}

void SimpleTrack::addHit(unsigned int ihit) {
        if (NHits<MAXLAY) { 
                TrRecHitR* phit = AMSEventR::Head()->pTrRecHit(ihit);
                pHit[NHits] = phit; 
                NHits++;
        } else { 
                cout << "SimpleTrack WARNING: Can not add more than ";
                cout << MAXLAY << " hits; new hit ignored"; 
                cout << endl;
        }
};

void SimpleTrack::delHit(unsigned int ihit) {
        for(int i=0;i<NHits;i++) {
            TrRecHitR* phit = AMSEventR::Head()->pTrRecHit(ihit);
            if (pHit[i]==phit) {
                 for(int j=i+1;j<NHits;j++) pHit[j-1] = pHit[j];
                 NHits--;
                 return;
            }
        }
}

int SimpleTrack::SimpleFit(){

  // Reset fit values
    Chi2StrLine = FLT_MAX;
    Chi2 = FLT_MAX;
    Rigidity = 0.0;
    ErrRigidity = FLT_MAX;
    Theta = 0.0;
    Phi = 0.0;
    P0[0] = 0.0;
    P0[1] = 0.0;
    P0[2] = 0.0;

  // Consistency check on the number of hits
    if (NHits<3 || NHits>MAXLAY) return -1;

  // Get hit positions and uncertainties
  // Scale errors (we will use sigmas in microns)
    float hits[MAXLAY][3];
    float b[MAXLAY][3];
    float sigma[MAXLAY][3];
    for (int i=0;i<NHits;i++){
      for (int j=0;j<3;j++){
          hits[i][j] = pHit[i]->Hit[j];
          b[i][j] = pHit[i]->Bfield[j];
          sigma[i][j] = pHit[i]->EHit[j];
      }
      // cout << " i, hits, sigma: " << i << ",";
      // for (int j=0;j<3;j++){ cout << " " << hits[i][j];}
      // for (int j=0;j<3;j++){ cout << " " << 1.e-4*sigma[i][j];}
      // cout << endl;
      // cout << " i, bfield: " << i << ",";
      // for (int j=0;j<3;j++){ cout << " " << b[i][j];}
      // cout << endl;
}

  // Lenghts
    float len[MAXLAY];
    for (int i=0;i<NHits;i++){
      if (i==0){
        len[i] = 0.;
      }
      else {
        len[i] = sqrt( (hits[i][0]-hits[i-1][0])*(hits[i][0]-hits[i-1][0])
                      +(hits[i][1]-hits[i-1][1])*(hits[i][1]-hits[i-1][1])
                      +(hits[i][2]-hits[i-1][2])*(hits[i][2]-hits[i-1][2]) );
      }
    }

  // Calculate path integrals
    double PathIntegral_x[MAXLAY][3];
    double PathIntegral_u[MAXLAY][3];
  
    for (int j=0;j<3;j++){
          PathIntegral_x[0][j] = 0.;
          PathIntegral_u[0][j] = 0.;
    }

    for (int i=1;i<NHits;i++){

      double u[3], bave[3];
      for (int j=0;j<3;j++){
          u[j] = (hits[i][j]-hits[i-1][j])/len[i];
          bave[j] = (b[i-1][j]+b[i][j])/2;
      }

      PathIntegral_x[i][0] = (u[1]*b[i-1][2]-u[2]*b[i-1][1])/2;
      PathIntegral_x[i][1] = (u[2]*b[i-1][0]-u[0]*b[i-1][2])/2;
      PathIntegral_x[i][2] = (u[0]*b[i-1][1]-u[1]*b[i-1][0])/2;
  
      PathIntegral_u[i][0] = u[1]*bave[2]-u[2]*bave[1];
      PathIntegral_u[i][1] = u[2]*bave[0]-u[0]*bave[2];
      PathIntegral_u[i][2] = u[0]*bave[1]-u[1]*bave[0];
  
    }

  // F and G matrices
    double d[2*MAXLAY][5];
    for (int i=0;i<NHits;i++) {
      int ix = i;
      int iy = i+NHits;
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
    double dx[5];
    for (int j=0;j<5;j++) {
      dx[j] = 0.;
      for (int l=0;l<NHits;l++) {
        dx[j] += d[l][j]/sigma[l][0]/sigma[l][0]*hits[l][0];
        dx[j] += d[l+NHits][j]/sigma[l][1]/sigma[l][1]*hits[l][1];
      }
    }

  // (F*S_x*F + G*S_y*G)
    double Param[5];
    double InvCov[5][5];
    for (int j=0;j<5;j++) {
      for (int k=0;k<5;k++) {
        InvCov[j][k] = 0.;
        for (int l=0;l<NHits;l++) {
          InvCov[j][k] += d[l][j]/sigma[l][0]/sigma[l][0]*d[l][k];
          InvCov[j][k] += d[l+NHits][j]/sigma[l][1]/sigma[l][1]*d[l+NHits][k];
        } 
      }
    }
      
  // (F*S_x*F + G*S_y*G)**{-1}
    int idim = 5;
    double determ = 0.0;
    TMatrixD RootCovariance = TMatrixD(idim,idim,(double*)InvCov," ");
    RootCovariance = RootCovariance.Invert(&determ);
    // int ifail;
    // INVERTMATRIX((double*)InvCov, idim, idim, ifail);
    if (determ<=0) return -2;
    double* aux = RootCovariance.GetElements();
    double Covariance[5][5];
    int icount = 0;
    for (int j=0;j<5;j++) {
      for (int k=0;k<5;k++) {
              Covariance[j][k] = *(aux+icount);
              icount++;
      }
    }


  // Solution
    for (int k=0;k<5;k++) {
      Param[k] = 0.;
      for (int i=0;i<5;i++) {
        Param[k] += Covariance[k][i]*dx[i];
      }
    }

  // Chi2 (xl and yl in microns, since sigmas are in microns too)
    for (int l=0;l<NHits;l++) {
      double xl = hits[l][0]*1.e4;
      double yl = hits[l][1]*1.e4;
      for (int k=0;k<5;k++) {
        xl -= d[l][k]*Param[k]*1.e4;
        yl -= d[l+NHits][k]*Param[k]*1.e4;
      }
      Chi2 += xl/sigma[l][0]/sigma[l][0]*xl + yl/sigma[l][1]/sigma[l][1]*yl;
      Chi2StrLine += xl/sigma[l][0]/sigma[l][0]*xl;
    }

  // Get Chi2/Ndof
    if (Param[4]!=0.0 && Covariance[4][4]>0.0) {
        Chi2 /= (2.*NHits-5.);
        Chi2StrLine /= (NHits-2.);
        Rigidity = 2.997E-4/Param[4];
        ErrRigidity = sqrt(Covariance[4][4])/2.997E-4;
        Theta = atan2(1.,-sqrt(1-Param[2]*Param[2]-Param[3]*Param[3]));
        Phi = atan2(Param[3],Param[2]);
        P0[0] = Param[0];
        P0[1] = Param[1];
        P0[3] = hits[0][2];
        return 0;
    } else {
        return -3;
    }

};
