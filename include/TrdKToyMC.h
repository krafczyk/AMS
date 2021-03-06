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

    void SetMDR(float _mdr){MDR=_mdr;return;}
    void PrepareToyMC(int _seed, float Rigidity_Min,float Rigidity_Max);
    void SetParticle(int iparticle){MCParticle=iparticle;return;} // 0:Electron, 1:Proton, 2:Helium
    void SetSpectrum(TF1* f_userspectrum[3]){for(int i=0;i<3;i++)f_rigidity[i]=f_userspectrum[i];return;}
    float GetXePressure(int Time);
    int GenerateToyMCEvent(int Time, int particle=-999); // 0:Electron, 1:Proton, 2:Helium
    void GeneratePattern(AMSPoint* P0, AMSDir *Dir);


    TrdKCluster GetKCluster(int TrackType=1);


    TRandom3* gRandom;
    TRandom3 *GetRandom(){return gRandom;}


    int MCParticle;
    float mc_Rigidity,mc_Pressure,mc_EcalEnergy;
    float MDR;

    AMSPoint MCTrack_P0;
    AMSDir MCTrack_Dir;
    float MCTrack_Rigidity;

    AMSPoint TrTrack_P0;
    AMSDir TrTrack_Dir;
    float TrTrack_Rigidity;

    static map<int, TRDOnline> map_TRDOnline;
    float Pressure_Xe;


    TrdKCluster _trdcluster;


private:

    int Seed;
    TF1 *f_rigidity[3];


    vector<TrdKHit> select_tube;
    vector<TrdKHit> Real_tube_collection;

    TrdKPDF *pdf;
    TrdKPDF *kpdf_p;
    TrdKPDF *kpdf_e;
    TrdKPDF *kpdf_h;



};

#endif // TRDKTOYMC_H
