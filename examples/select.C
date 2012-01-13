#include "TkSens.h"

static Double_t Corr1[40][40] = {
  1.072,1.067,1.065,1.060,1.061,1.068,1.085,1.099,1.116,1.124,1.131,1.132,1.131,1.132,1.137,1.130,1.122,1.105,1.084,1.068,
  1.063,1.067,1.075,1.081,1.086,1.085,1.080,1.076,1.072,1.069,1.070,1.073,1.080,1.087,1.092,1.101,1.109,1.100,1.088,1.077,
  1.072,1.067,1.065,1.060,1.061,1.068,1.085,1.099,1.116,1.124,1.131,1.132,1.131,1.132,1.137,1.130,1.122,1.105,1.084,1.068,
  1.063,1.067,1.075,1.081,1.086,1.085,1.080,1.076,1.072,1.069,1.070,1.073,1.080,1.087,1.092,1.101,1.109,1.100,1.088,1.077,
  1.068,1.067,1.068,1.066,1.064,1.066,1.074,1.090,1.110,1.119,1.124,1.128,1.130,1.140,1.138,1.125,1.115,1.095,1.071,1.060,
  1.062,1.070,1.077,1.082,1.084,1.082,1.077,1.072,1.067,1.064,1.064,1.068,1.072,1.077,1.090,1.112,1.115,1.103,1.083,1.073,
  1.062,1.066,1.073,1.074,1.071,1.068,1.060,1.050,1.064,1.080,1.096,1.110,1.107,1.106,1.101,1.091,1.061,1.054,1.060,1.067,
  1.070,1.075,1.079,1.081,1.080,1.076,1.070,1.063,1.057,1.054,1.054,1.058,1.061,1.059,1.023,1.015,1.078,1.088,1.080,1.063,
  1.049,1.059,1.076,1.082,1.081,1.076,1.071,1.067,1.065,1.063,1.062,1.061,1.062,1.063,1.064,1.066,1.068,1.069,1.071,1.073,
  1.075,1.077,1.077,1.076,1.072,1.066,1.059,1.052,1.045,1.042,1.042,1.045,1.055,1.000,0.960,0.882,0.989,1.024,1.048,1.040,
  1.024,1.044,1.074,1.084,1.086,1.084,1.079,1.075,1.071,1.071,1.073,1.076,1.078,1.076,1.074,1.072,1.072,1.073,1.075,1.077,
  1.076,1.074,1.071,1.067,1.061,1.054,1.046,1.039,1.034,1.032,1.033,1.038,1.046,0.983,0.000,0.000,0.000,0.970,0.950,1.010,
  0.950,0.975,1.061,1.085,1.086,1.087,1.085,1.082,1.079,1.079,1.081,1.083,1.083,1.080,1.079,1.077,1.077,1.077,1.076,1.073,
  1.068,1.064,1.059,1.054,1.048,1.040,1.033,1.028,1.025,1.023,1.025,1.035,1.053,1.100,0.000,0.000,0.000,0.000,0.840,0.877,
  0.800,0.900,1.040,1.076,1.078,1.081,1.082,1.081,1.081,1.081,1.084,1.085,1.085,1.083,1.081,1.078,1.076,1.074,1.069,1.064,
  1.057,1.051,1.046,1.041,1.035,1.028,1.022,1.017,1.017,1.018,1.022,1.035,1.070,0.000,0.000,0.000,0.000,0.000,0.788,0.744,
  0.726,0.816,1.030,1.066,1.068,1.071,1.072,1.072,1.075,1.076,1.079,1.078,1.079,1.076,1.072,1.070,1.068,1.064,1.059,1.051,
  1.042,1.036,1.031,1.030,1.024,1.018,1.012,1.008,1.009,1.013,1.028,1.046,1.080,0.000,0.000,0.000,0.000,0.000,0.000,0.685,
  0.762,0.880,1.049,1.054,1.055,1.057,1.058,1.059,1.062,1.063,1.065,1.064,1.064,1.063,1.059,1.057,1.053,1.049,1.043,1.035,
  1.027,1.023,1.019,1.018,1.014,1.009,1.005,1.003,1.004,1.011,1.020,1.055,1.090,0.000,0.000,0.000,0.000,0.000,0.740,0.740,
  0.941,1.024,1.041,1.041,1.041,1.041,1.041,1.043,1.045,1.047,1.047,1.047,1.047,1.046,1.043,1.041,1.037,1.031,1.024,1.017,
  1.011,1.008,1.007,1.007,1.004,1.000,0.999,0.999,1.002,1.009,1.015,1.090,1.079,0.000,0.000,0.000,0.000,0.818,0.803,0.864,
  1.019,1.026,1.033,1.029,1.025,1.023,1.023,1.026,1.026,1.028,1.027,1.027,1.027,1.026,1.025,1.023,1.020,1.015,1.009,1.003,
  0.999,0.997,0.997,0.997,0.997,0.996,0.998,1.000,1.003,1.008,1.010,1.120,1.100,0.000,0.000,0.000,0.000,0.887,0.929,1.010,
  1.018,1.021,1.022,1.017,1.012,1.008,1.007,1.009,1.008,1.010,1.008,1.008,1.008,1.008,1.008,1.006,1.005,1.002,0.997,0.992,
  0.988,0.986,0.988,0.991,0.992,0.995,0.999,1.003,1.007,1.011,1.018,1.159,1.040,0.000,0.000,0.000,0.995,0.970,1.012,1.018,
  1.014,1.013,1.010,1.005,1.000,0.994,0.994,0.994,0.995,0.995,0.993,0.993,0.993,0.994,0.994,0.993,0.993,0.994,0.989,0.984,
  0.980,0.978,0.983,0.986,0.991,0.997,1.001,1.006,1.009,1.024,1.038,1.238,1.247,0.000,0.000,0.000,1.000,1.020,1.020,1.013,
  1.005,1.002,0.999,0.995,0.991,0.985,0.985,0.984,0.984,0.983,0.982,0.981,0.981,0.983,0.984,0.985,0.987,0.986,0.983,0.977,
  0.974,0.974,0.980,0.985,0.990,0.997,1.002,1.007,1.011,1.016,1.045,1.250,1.193,0.000,0.000,1.020,1.009,1.006,1.008,1.008,
  0.997,0.994,0.992,0.989,0.985,0.981,0.979,0.978,0.977,0.975,0.974,0.973,0.973,0.974,0.976,0.978,0.980,0.980,0.978,0.974,
  0.974,0.976,0.982,0.987,0.991,0.999,1.006,1.011,1.015,1.019,1.025,1.248,1.201,1.229,1.079,1.028,1.012,1.004,1.002,0.999,
  0.992,0.989,0.987,0.985,0.981,0.978,0.976,0.975,0.973,0.971,0.970,0.970,0.969,0.970,0.971,0.975,0.978,0.977,0.977,0.975,
  0.977,0.981,0.986,0.992,0.997,1.005,1.010,1.014,1.017,1.020,1.024,1.080,1.235,1.231,1.193,1.050,0.990,1.002,0.997,0.994,
  0.987,0.986,0.984,0.983,0.979,0.978,0.977,0.976,0.973,0.970,0.969,0.969,0.969,0.970,0.971,0.975,0.978,0.979,0.981,0.982,
  0.985,0.989,0.993,1.000,1.004,1.010,1.014,1.015,1.016,1.015,1.014,0.996,1.050,1.219,1.180,1.060,1.013,1.001,0.993,0.990,
  0.984,0.984,0.983,0.983,0.981,0.981,0.980,0.979,0.976,0.971,0.970,0.970,0.972,0.973,0.974,0.978,0.981,0.982,0.986,0.988,
  0.992,0.998,1.001,1.007,1.011,1.015,1.016,1.014,1.013,1.009,1.005,1.004,1.000,1.015,1.065,1.045,1.010,0.992,0.985,0.980,
  0.985,0.986,0.986,0.987,0.985,0.984,0.983,0.981,0.977,0.974,0.974,0.974,0.976,0.978,0.978,0.982,0.986,0.988,0.993,0.997,
  1.000,1.004,1.006,1.011,1.014,1.014,1.013,1.011,1.008,1.004,0.996,0.993,0.996,1.000,1.010,1.014,1.007,0.998,0.991,0.988,
  0.988,0.989,0.991,0.992,0.990,0.990,0.987,0.985,0.982,0.981,0.980,0.981,0.981,0.985,0.986,0.990,0.992,0.994,0.998,1.003,
  1.007,1.010,1.011,1.014,1.015,1.013,1.010,1.007,1.003,0.999,0.990,0.985,0.985,0.988,0.997,1.003,1.002,0.999,0.993,0.991,
  0.993,0.994,0.995,0.994,0.993,0.992,0.990,0.988,0.986,0.987,0.986,0.986,0.987,0.991,0.993,0.996,0.997,1.000,1.003,1.008,
  1.011,1.013,1.012,1.014,1.013,1.009,1.006,1.003,1.000,0.995,0.985,0.980,0.977,0.977,0.984,0.994,0.998,1.000,0.997,0.995,
  0.998,0.999,1.000,0.997,0.995,0.993,0.992,0.991,0.991,0.992,0.990,0.990,0.990,0.994,0.997,0.999,1.000,1.003,1.005,1.009,
  1.010,1.011,1.010,1.011,1.009,1.006,1.002,1.000,0.997,0.993,0.984,0.978,0.973,0.973,0.977,0.988,0.994,0.998,0.998,0.998,
  0.999,1.000,0.999,0.995,0.991,0.988,0.988,0.988,0.989,0.992,0.991,0.989,0.990,0.992,0.996,1.000,1.004,1.006,1.006,1.006,
  1.005,1.005,1.005,1.005,1.003,1.002,0.999,0.999,0.998,0.996,0.988,0.982,0.974,0.970,0.972,0.980,0.988,0.994,0.997,0.998,
  0.995,0.995,0.993,0.989,0.985,0.983,0.982,0.983,0.985,0.988,0.989,0.987,0.987,0.987,0.993,0.999,1.004,1.005,1.003,1.002,
  1.001,1.001,1.002,1.002,1.001,1.001,1.001,1.001,1.003,1.003,0.997,0.987,0.977,0.970,0.970,0.974,0.983,0.989,0.994,0.995,
  0.990,0.990,0.985,0.981,0.977,0.977,0.978,0.978,0.979,0.981,0.983,0.983,0.983,0.984,0.989,0.997,1.002,1.002,0.998,0.996,
  0.995,0.997,0.999,1.000,1.000,1.002,1.003,1.006,1.012,1.013,1.009,0.998,0.985,0.974,0.969,0.969,0.973,0.980,0.986,0.989,
  0.981,0.980,0.973,0.970,0.967,0.969,0.973,0.975,0.975,0.977,0.979,0.981,0.980,0.982,0.985,0.993,0.996,0.996,0.995,0.994,
  0.994,0.996,0.998,1.001,1.001,1.005,1.010,1.014,1.021,1.024,1.021,1.009,0.995,0.982,0.972,0.968,0.967,0.972,0.978,0.981,
  0.973,0.972,0.966,0.963,0.962,0.966,0.970,0.972,0.972,0.974,0.976,0.978,0.979,0.981,0.983,0.989,0.993,0.995,0.996,0.994,
  0.994,0.996,0.998,1.002,1.004,1.009,1.017,1.024,1.032,1.035,1.032,1.022,1.006,0.991,0.978,0.969,0.964,0.965,0.970,0.974,
  0.970,0.968,0.963,0.961,0.960,0.964,0.969,0.972,0.973,0.974,0.976,0.978,0.978,0.980,0.981,0.986,0.990,0.993,0.996,0.996,
  0.996,0.998,1.002,1.007,1.011,1.018,1.026,1.034,1.041,1.046,1.043,1.034,1.019,1.003,0.987,0.975,0.967,0.965,0.968,0.972,
  0.973,0.970,0.967,0.965,0.964,0.967,0.971,0.974,0.976,0.976,0.977,0.978,0.979,0.981,0.982,0.985,0.990,0.994,0.998,1.001,
  1.003,1.006,1.010,1.017,1.022,1.031,1.040,1.049,1.056,1.059,1.055,1.048,1.033,1.016,1.000,0.985,0.976,0.971,0.972,0.973,
  0.980,0.977,0.976,0.973,0.972,0.974,0.978,0.980,0.982,0.982,0.982,0.982,0.982,0.985,0.987,0.990,0.994,0.999,1.004,1.009,
  1.012,1.015,1.021,1.030,1.038,1.046,1.055,1.063,1.068,1.070,1.066,1.059,1.047,1.031,1.015,0.999,0.988,0.981,0.980,0.980,
  0.989,0.988,0.987,0.985,0.984,0.985,0.988,0.990,0.991,0.991,0.990,0.990,0.991,0.992,0.994,0.995,1.000,1.006,1.013,1.019,
  1.023,1.028,1.035,1.044,1.052,1.060,1.068,1.075,1.078,1.079,1.074,1.068,1.058,1.044,1.030,1.015,1.004,0.996,0.992,0.990,
  1.000,0.999,1.000,1.000,1.000,1.001,1.004,1.005,1.005,1.005,1.005,1.004,1.005,1.004,1.005,1.006,1.011,1.016,1.022,1.029,
  1.036,1.041,1.050,1.058,1.066,1.072,1.078,1.083,1.084,1.082,1.078,1.074,1.066,1.056,1.044,1.032,1.021,1.012,1.007,1.003,
  1.014,1.013,1.014,1.016,1.018,1.021,1.023,1.025,1.025,1.023,1.022,1.021,1.020,1.020,1.019,1.020,1.024,1.029,1.035,1.042,
  1.050,1.056,1.064,1.071,1.078,1.082,1.084,1.085,1.083,1.080,1.076,1.073,1.069,1.064,1.055,1.046,1.037,1.029,1.023,1.018,
  1.031,1.029,1.030,1.033,1.036,1.040,1.042,1.044,1.043,1.042,1.041,1.039,1.039,1.038,1.038,1.038,1.042,1.046,1.051,1.057,
  1.064,1.070,1.076,1.082,1.086,1.088,1.087,1.083,1.079,1.074,1.071,1.068,1.066,1.064,1.060,1.056,1.050,1.044,1.039,1.034,
  1.045,1.044,1.046,1.049,1.052,1.056,1.059,1.060,1.061,1.060,1.058,1.057,1.057,1.057,1.057,1.057,1.060,1.063,1.067,1.072,
  1.076,1.081,1.085,1.088,1.089,1.087,1.083,1.080,1.077,1.072,1.060,1.054,1.058,1.062,1.065,1.063,1.061,1.058,1.052,1.047,
  1.057,1.057,1.059,1.062,1.065,1.069,1.071,1.073,1.074,1.076,1.076,1.074,1.073,1.073,1.073,1.073,1.075,1.077,1.080,1.083,
  1.086,1.088,1.089,1.089,1.086,1.082,1.076,1.062,1.062,1.080,1.080,1.060,1.045,1.050,1.058,1.065,1.070,1.062,1.060,1.058,
  1.065,1.070,1.070,1.071,1.073,1.077,1.080,1.082,1.086,1.090,1.083,1.082,1.082,1.083,1.083,1.083,1.084,1.086,1.089,1.091,
  1.092,1.092,1.090,1.086,1.080,1.070,1.075,1.097,1.125,1.155,1.145,1.115,1.074,1.055,1.050,1.056,1.065,1.065,1.065,1.065,
  1.069,1.071,1.072,1.075,1.078,1.082,1.086,1.087,1.089,1.088,1.088,1.086,1.086,1.086,1.087,1.087,1.089,1.091,1.093,1.095,
  1.095,1.093,1.089,1.084,1.065,1.065,1.080,1.124,1.160,1.180,1.210,1.120,1.085,1.066,1.040,1.048,1.055,1.062,1.066,1.067,
  1.069,1.071,1.072,1.075,1.078,1.082,1.086,1.087,1.089,1.088,1.088,1.086,1.086,1.086,1.087,1.087,1.089,1.091,1.093,1.095,
  1.095,1.093,1.089,1.084,1.065,1.065,1.080,1.124,1.160,1.180,1.210,1.120,1.085,1.066,1.040,1.048,1.055,1.062,1.066,1.067};

bool ssa(float phi,float theta) {
  double const Pi=4*atan(1);
  phi=(phi-2*Pi)*100;
  theta=theta*100;
  // phi, theta geographic
  bool ssa_good=true;
  if(phi>=-74 && phi<-72 && theta>=-23 && theta<-18) ssa_good=false;
  if(phi>=-72 && phi<-70 && theta>=-27 && theta<-15) ssa_good=false;
  if(phi>=-70 && phi<-68 && theta>=-31 && theta<-13) ssa_good=false;
  if(phi>=-68 && phi<-66 && theta>=-34 && theta<-12) ssa_good=false;
  if(phi>=-66 && phi<-64 && theta>=-36 && theta<-11) ssa_good=false;
  if(phi>=-64 && phi<-62 && theta>=-38 && theta<-10) ssa_good=false;
  if(phi>=-62 && phi<-60 && theta>=-40 && theta<-10) ssa_good=false;
  if(phi>=-60 && phi<-58 && theta>=-40 && theta<-9) ssa_good=false;
  if(phi>=-58 && phi<-56 && theta>=-42 && theta<-8) ssa_good=false;
  if(phi>=-56 && phi<-54 && theta>=-43 && theta<-8) ssa_good=false;
  if(phi>=-54 && phi<-52 && theta>=-43 && theta<-8) ssa_good=false;
  if(phi>=-52 && phi<-50 && theta>=-43 && theta<-8) ssa_good=false;
  if(phi>=-50 && phi<-48 && theta>=-43 && theta<-8) ssa_good=false;
  if(phi>=-48 && phi<-46 && theta>=-44 && theta<-8) ssa_good=false;
  if(phi>=-46 && phi<-44 && theta>=-44 && theta<-8) ssa_good=false;
  if(phi>=-44 && phi<-42 && theta>=-44 && theta<-9) ssa_good=false;
  if(phi>=-42 && phi<-40 && theta>=-43 && theta<-9) ssa_good=false;
  if(phi>=-40 && phi<-38 && theta>=-43 && theta<-11) ssa_good=false;
  if(phi>=-38 && phi<-36 && theta>=-42 && theta<-13) ssa_good=false;
  if(phi>=-36 && phi<-34 && theta>=-42 && theta<-12) ssa_good=false;
  if(phi>=-34 && phi<-32 && theta>=-42 && theta<-14) ssa_good=false;
  if(phi>=-32 && phi<-30 && theta>=-41 && theta<-16) ssa_good=false;
  if(phi>=-30 && phi<-28 && theta>=-40 && theta<-17) ssa_good=false;
  if(phi>=-28 && phi<-26 && theta>=-40 && theta<-18) ssa_good=false;
  if(phi>=-26 && phi<-24 && theta>=-39 && theta<-19) ssa_good=false;
  if(phi>=-24 && phi<-22 && theta>=-38 && theta<-20) ssa_good=false;
  if(phi>=-22 && phi<-20 && theta>=-37 && theta<-21) ssa_good=false;
  if(phi>=-20 && phi<-18 && theta>=-36 && theta<-22) ssa_good=false;
  if(phi>=-18 && phi<-16 && theta>=-35 && theta<-24) ssa_good=false;
  if(phi>=-16 && phi<-14 && theta>=-34 && theta<-25) ssa_good=false;
  if(phi>=-14 && phi<-12 && theta>=-32 && theta<-26) ssa_good=false;
  if(phi>=-12 && phi<-10 && theta>=-31 && theta<-27) ssa_good=false;
  if(phi>=-10 && phi<-8 && theta>=-28 && theta<-27) ssa_good=false;
  return ssa_good;
}
bool Clean_Event(AMSEventR *pev){

  //sciense runtag
  HeaderR* header = &(pev->fHeader);
  if((header->RunType>>12)!=0xf) return false; 
  // printf("%04x\n", header->RunType);
  

  //Live Time >0.5
  Level1R * trig=pev-> pLevel1(0);
  if( trig->LiveTime <=0.65) return false;

  //SAA exclusion
  //kounin
  int i = (int)((header->ThetaS + 1.0)/0.05);
  if(i<0)  i = 0;
  if(i>39) i = 39;
  int j = (int)(header->PhiS/0.1571);
  if(j<0)  j = 0;
  if(j>39) j = 39;

  float cr = Corr1[i][j];

  // if ( cr == 0.000 ) return false;
  
  //contin
  // if(!ssa(header->PhiS,header->ThetaS)) return false;
 

  // 1! Tracker track
  if(pev->nTrTrack()!=1) return false;
  TrTrackR *track=pev->pTrTrack(0);
  int id=track->iTrTrackPar(1,3,1);
  if(id<0)return false;


  // Rigidity > 3 GeV 
  if(fabs(track->GetRigidity(id))<=3.0) return false;


  //downgoing particle with good beta
   
  BetaR *beta;
  int track_beta=0;   
  float Beta=0;
  float betasum=0;
  if(pev->nBeta()>1){
    vector<float> betadiff;
    for( int i=0; i<pev->nBeta(); i++){
      beta=pev->pBeta(i);
      
      if(beta->iTrTrack()!=-1){
	betadiff.push_back(beta->Beta);
	betasum+=beta->Beta;
      }
    }
    for(int i=0; i<betadiff.size(); i++){
      if(betadiff[i]>0.6 && betasum<betadiff[i]*betadiff.size()){
	beta=pev->pBeta(i);
	Beta=beta->Beta;
      }
    }
  }
  else if(pev->nBeta()==1){
    beta=pev->pBeta(0);
    Beta=beta->Beta;
  }
  
  if(Beta<=0.6 || Beta >=1.2) return false;
  
  //input Marco on d Track/Hits in ToF && 4/4 ToF

  bool minimum_bias;
  
  TofClusterR* cluster;
  
  bool goodlayer[4]={false,false,false,false};
  
  minimum_bias=false;
  if(pev->nTofCluster()<=0) return false;
  for (int icl=0; icl<pev->nTofCluster(); icl++) { 
    int layer;
    cluster = pev->pTofCluster(icl);
    bool good_c=false;
    if (cluster>0) { // cluster exists
      good_c=true;
      layer=cluster->Layer-1;
      for (int i=7; i<13; i++) if ((cluster->Status>>i)&1==1) good_c=false;
      if ((cluster->Status>>2)&1==1) good_c=false;
      if ((cluster->Status>>4)&1==1) good_c=false;
    }
    if(good_c) goodlayer[layer]=true;
  }
  minimum_bias=(goodlayer[0] && goodlayer[1] && goodlayer[2] && goodlayer[3]);
  //if(!minimum_bias) return false;

  float LONGCUT[4][10]={
    9.,8.,8.,8.,8.,8.,8.,9.,0.,0.,
    12.,8.,8.,8.,8.,8.,8.,12.,0.,0.,
    12.,8.,8.,8.,8.,8.,8.,8.,8.,12.,
    10.,8.,8.,8.,8.,8.,8.,10.,0.,0.}; // cm (cut on tof-track transversal coord.)
  float TRANCUT[4][10]={
    13.,6.,6.,6.,6.,6.,6.,13.,0.,0.,
    14.,6.,6.,6.,6.,6.,6.,14.,0.,0.,
    10.,6.,6.,6.,6.,6.,6.,6.,6.,10.,
    14.,6.,6.,6.,6.,6.,6.,14.,0.,0.}; // cm (cut on tof-track longitudinal coord.)
  
  bool good_match;
  double tlen;
  AMSPoint tofpnt;
  AMSDir tofdir;
  int longit[4]={0,1,1,0};
  int tranit[4]={1,0,0,1};
  double dlong,dtran;
  bool goodlay[4]={false,false,false,false};
  
  // search for track-cluster matching
  good_match=false;
  // match with cluster
  for (int icl=0; icl<pev->nTofCluster(); icl++) { 
    cluster = pev->pTofCluster(icl);
    if (cluster>0) { // cluster exists
      int layer=cluster->Layer-1;
      int bar=cluster->Bar-1;
      tlen=track->Interpolate(cluster->Coo[2],tofpnt,tofdir, id);
      dlong=cluster->Coo[longit[layer]]-tofpnt[longit[layer]];
      dtran=cluster->Coo[tranit[layer]]-tofpnt[tranit[layer]];
      if(fabs(dlong)<LONGCUT[layer][bar] && fabs(dtran)<TRANCUT[layer][bar]) goodlay[layer]=true;
    }
  }
  good_match=(goodlay[0] && goodlay[1] && goodlay[2] && goodlay[3]);
  //if(!good_match) return false;
  int nlay=0;
  for(int i=0; i<4; i++) if(goodlayer[i] && goodlay[i]) nlay++;
  if(nlay<3) return false;

  int tr_lay[9][2];
  for(int i=0;i<9;i++){tr_lay[i][0]=0;tr_lay[i][1]=0;}

  TrRecHitR *hit;
  for(int i=2; i<9; i++){
    hit=track->GetHitLJ(i);
    if(hit){
      if(!hit->OnlyY()) tr_lay[i-1][0]++;
      if(!hit->OnlyX()) tr_lay[i-1][1]++;
    }
  }

  int n_layers[2]={0,0};
  
  for(int i=1; i<8; i++){
    if(tr_lay[i][0]>0) n_layers[0]++;
    if(tr_lay[i][1]>0) n_layers[1]++;
  }
  
  
  // One hit on every inner tracker plane in X,Y
  if(!((tr_lay[2][0]||tr_lay[3][0]) && (tr_lay[4][0]||tr_lay[5][0]) && (tr_lay[6][0]||tr_lay[7][0]))) return false;
  if(!(tr_lay[1][1]&&(tr_lay[2][1]||tr_lay[3][1]) && (tr_lay[4][1]||tr_lay[5][1]) && (tr_lay[6][1]||tr_lay[7][1]))) return false;
  
  
  // if(track->GetChisqX(id)/ n_layers[0] >5 ) return false;
  if(track->GetNormChisqY(id) >=10. ) return false;

  AMSPoint pnt[2];

 
  pnt[0]=track->InterpolateLayerJ(1, id);
  pnt[1]=track->InterpolateLayerJ(9, id);

  TkSens ss(0);
  float err=0.1;
  bool sen[4][2];
  AMSPoint a[4], b[4];
  
  a[0].setp(pnt[0].x()+err, pnt[0].y(),pnt[0].z());
  a[1].setp(pnt[0].x(), pnt[0].y()+err,pnt[0].z());
  a[2].setp(pnt[0].x()-err, pnt[0].y(),pnt[0].z());
  a[3].setp(pnt[0].x(), pnt[0].y()-err,pnt[0].z());
  
  for(int i=0; i<4; i++){
    sen[i][0]=false;
    ss.SetGlobalCoo(a[i]);    
    if(ss.LadFound ()) sen[i][0]=true;
  }
  if(!(sen[0][0] || sen[1][0] || sen[2][0] || sen[3][0])) return false;
 
  
  b[0].setp(pnt[1].x()+err, pnt[1].y(),pnt[1].z());
  b[1].setp(pnt[1].x(), pnt[1].y()+err,pnt[1].z());
  b[2].setp(pnt[1].x()-err, pnt[1].y(),pnt[1].z());
  b[3].setp(pnt[1].x(), pnt[1].y()-err,pnt[1].z());
  
  for(int i=0; i<4; i++){
    sen[i][1]=false;
    ss.SetGlobalCoo(b[i]);    
    if(ss.LadFound ()) sen[i][1]=true;
  }
  if(!((sen[0][1] || sen[1][1] || sen[2][1] || sen[3][1]) && fabs(pnt[1].x())<30.)) return false;
  

  AMSPoint ecaltop;
  AMSDir ecal;
  float z=-142.8;  //EcalTop?
  
  track->Interpolate(z, ecaltop, ecal, id);
  
  if(fabs(ecaltop.x())>=32.4 || fabs(ecaltop.y())>=32.4 ) return false;
  

  return true;

}



// x[0] = fabs(P)
double FunTrdSigmaDy(double *x, double *par) {
  return 0.1*(par[0] + par[1]*exp(-par[2]*x[0]));
}

bool good_trd_event(AMSEventR *pev){
  
  if( pev->nTrdHTrack()!=1) return false;
  
  if( pev->nTrdHSegment() !=2) return false;

  TrdHTrackR *trd_track=pev->pTrdHTrack(0);
  float TrdChi2=trd_track->Chi2;
  int nTrdHits=trd_track->Nhits;

  if (TrdChi2<0.0 || TrdChi2/nTrdHits>=3.0) return false;
  
  int hitsontrack=0;
  int pat[3]={0,0,0};
  int lay[20];
  for(int i=0; i<20;i++) lay[i]=0;

  for(int is=0; is<trd_track->nTrdHSegment(); is++){
    TrdHSegmentR *seg=trd_track->pTrdHSegment(is);
    for(int ih=0; ih<seg->nTrdRawHit(); ih++){
      TrdRawHitR *hit=seg->pTrdRawHit(ih);
      if(hit->Amp>10){
	hitsontrack++;
	lay[hit->Layer]++;
      }
    }
  }
    
  for(int i=0; i<4; i++){
    if(lay[i]>0) pat[0]++;
  }
  for(int i=4; i<16; i++){
    if(lay[i]>0) pat[1]++;
  }
  for(int i=16; i<20; i++){
    if(lay[i]>0) pat[2]++;
  }
     
  if(!(pat[0]>2 && pat[1]>8 && pat[2]>2)) return false;
  
  //still not clear if needed
  //if( (float)(hitsontrack)/ (float)(pev->nTrdRawHit()) <0.5) return false;

  TrTrackR *tr_track=pev->pTrTrack(0);
  int id=tr_track->iTrTrackPar(1,3,1);
  float P=tr_track->GetRigidity(id);

  // TRD point and direction at z=UToF;
  float zpl= 65.2 ;  // UToF;
  AMSPoint trd_pnt0(trd_track->Coo[0], trd_track->Coo[1], trd_track->Coo[2]);
  AMSDir   trd_dir(trd_track->Dir[0], trd_track->Dir[1], trd_track->Dir[2]);

  double X_TRD= (zpl-trd_pnt0[2])*trd_dir[0]/trd_dir[2]+trd_pnt0[0];
  double Y_TRD= (zpl-trd_pnt0[2])*trd_dir[1]/trd_dir[2]+trd_pnt0[1];
  AMSPoint trd_pnt(X_TRD,Y_TRD,zpl);

  // Tracker point and direction at z=UToF;
  AMSPoint tk_pnt;
  AMSDir tk_dir;

  tr_track->Interpolate(zpl, tk_pnt, tk_dir, id);

  //float TrdTrkDr=sqrt(pow(trd_pnt[0]-tk_pnt[0],2)+pow(trd_pnt[1]-tk_pnt[1],2));
  //float TrdTrkDa=tk_dir.prod(trd_dir);
  
  double const Pi=4*atan(1);
  float TrdTrkDx=trd_pnt[0]-tk_pnt[0];
  float TrdTrkDy=trd_pnt[1]-tk_pnt[1];
  float TrdTrktheta=tk_dir.gettheta()-Pi+trd_dir.gettheta();
  float TrdTrkphi=tk_dir.getphi()+Pi-trd_dir.getphi();
  if(TrdTrkphi>Pi) TrdTrkphi=TrdTrkphi-2*Pi;

  TF1 *fTrdSigmaDx = new TF1("fTrdSigmaDx",FunTrdSigmaDy,0.0,1000.0,3);
  fTrdSigmaDx->SetParameters(2.484,0.1183,0.3487);
  TF1 *fTrdSigmaDy = new TF1("fTrdSigmaDy",FunTrdSigmaDy,0.0,1000.0,3);
  fTrdSigmaDy->SetParameters(1.686,0.1505,0.2347);

  TF1 *fTrd95Da = new TF1("fTrd95Da",FunTrdSigmaDy,0.0,1000.0,3);
  fTrd95Da->SetParameters(0.7729,0.7324,0.2005);

  double TrdCutDa = 10.0*fTrd95Da->Eval(fabs(P));
  double TrdCutDx =  10.0*fTrdSigmaDx->Eval(fabs(P));
  double TrdCutDy =  10.0*fTrdSigmaDy->Eval(fabs(P));
     
      
  if (fabs(TrdTrkDx)>TrdCutDx || fabs(TrdTrkDy)>TrdCutDy) return false;
  if( fabs(TrdTrktheta)>TrdCutDa || fabs(TrdTrkphi)>TrdCutDa) return false;
 
  return true;
}


bool good_ecal_event(AMSEventR *pev){

  if(pev->nEcalShower()<1) return false;

  EcalShowerR *shr;
  int highE=0;
  float Ehigh=0;

  for(int i=0; i<pev->nEcalShower(); i++){
    shr=pev->pEcalShower(i);
    
    float Edep=shr->EnergyE; // or EnergyD?
    if(Edep>Ehigh) {
      highE=i;
      Ehigh=Edep;
    }
  }
  
  shr=pev->pEcalShower(highE);

  if(fabs(shr->Entry[0])>=30.6 || fabs(shr->Entry[1])>=30.6) return false;
  if(fabs(shr->Exit[0])>=30.6 || fabs(shr->Exit[1])>=30.6) return false;

  TrTrackR *tr_track=pev->pTrTrack(0);
  int id=tr_track->iTrTrackPar(1,3,1);
  
  AMSPoint tk_pnt;
  AMSDir tk_dir;

  tr_track->Interpolate(shr->CofG[2], tk_pnt, tk_dir, id);

  float d= sqrt(pow(tk_pnt[0]-shr->CofG[0], 2)+ pow(tk_pnt[1]-shr->CofG[1], 2));

  if(d>=3.6) return false;

  return true;

}
