#ifndef TRDKTOYMC_H
#define TRDKTOYMC_H

#include "TrdKCluster.h"
#include "TrdKPDF.h"
#include "TrdKHit.h"


class TrdKToyMC
{
public:
    TrdKToyMC();
    TrdKToyMC(int _seed, float Rigidity_Min,float Rigidity_Max);

    void InitXePressure();

    void PrepareToyMC(int _seed, float Rigidity_Min,float Rigidity_Max);
    void SetParticle(int iparticle){MCParticle=iparticle;return;} // 0:Electron, 1:Proton, 2:Helium
    void SetSpectrum(TF1* f_userspectrum[3]){for(int i=0;i<3;i++)f_rigidity[i]=f_userspectrum[i];return;}
    float GetXePressure(int Time);
    int GenerateToyMCEvent(int Time, int particle=-999);
    void GeneratePattern(AMSPoint* P0, AMSDir *Dir);


    TrdKCluster GetKCluster(int TrackType=1);


private:

    int Seed;
    TRandom3* gRandom;
    TF1 *f_rigidity[3];
    double norm_rigidity[3];

    float MDR;

    vector<TrdKHit> select_tube;
    vector<TrdKHit> Real_tube_collection;

    TrdKPDF *pdf;
    TrdKPDF *kpdf_p;
    TrdKPDF *kpdf_e;
    TrdKPDF *kpdf_h;

    int MCParticle;
    float mc_Rigidity,mc_Pressure,mc_EcalEnergy;


    AMSPoint MCTrack_P0;
    AMSDir MCTrack_Dir;
    float MCTrack_Rigidity;

    AMSPoint TrTrack_P0;
    AMSDir TrTrack_Dir;
    float TrTrack_Rigidity;

    static map<int, TRDOnline> map_TRDOnline;
    float Pressure_Xe;


    TrdKCluster _trdcluster;

};

#endif // TRDKTOYMC_H
