#include "ACQtProducer-include.C"
#include "GM_SubLibrary.h"

bool ACsoft::ACQtProducer::DoCustomBacktracing(ParticleR* pParticle, UChar_t& statusOfAllBacktracings,
                                       UChar_t& statusOfRigidityBacktracing, double& latitudeFromRigidity, double& longitudeFromRigidity,
                                       UChar_t& statusOfEnergyBacktracing, double& latitudeFromEnergy, double& longitudeFromEnergy) {

  // BackTracing to x-check StoermerCutOff
  // - for all rigidities between 0.9*StoermerMin and 1.5*StoermerMax
  // - in addition also for all electron-candidates above 1.5*StoermerMax 
  //   to get better galactic coordinates for anisotropy studies
  // 
  // DoCustomBacktracing performs up to 4 BackTracings in this order:
  // 1 (momt,  beta, -ichg) if(momt<1.5*StoermerMax) // for opposite charge sign 
  // 2 (momt,  beta,  ichg)                          // always with particle charge
  // 3 (Eecal, beta, -ichg) if(Eecal/momt>0.5)       // for electron-candidate with opposite charge-sign
  // 4 (Eecal, beta,  ichg) if(Eecal/momt>0.5)       // for electron-candidate withparticle charge
  //
  // BT_Status flags (2 bits) from all BackTracings are saved in BT_status - handed over to DoCustomBacktracing()
  //
  // Galactic-latitide/longitude are taken from pParticle as set in last BackTracing
  //                             so electrons have coordinates from ECAL energy
  //
  // >>>  StoermerMin/StoermerMax replaced by Particle->GetGeoCutoff
  //

  assert(pParticle);

  AMSDir dir(pParticle->Theta, pParticle->Phi);
  int    ichg = pParticle->Charge;
  double beta = pParticle->Beta;
  double momentum = pParticle->Momentum;
  if (beta < 0) { dir  = dir*(-1); beta = -beta; }
  if (momentum < 0) { momentum = -momentum; ichg = -ichg; }

  float ecalEnergy = pParticle->pEcalShower() ? pParticle->pEcalShower()->EnergyE : 0.0;
  if (ecalEnergy / momentum < 0.5)
    ecalEnergy = 0.0; // not e.m. shower
  assert(ecalEnergy >= 0.0);

  double cutOff = fabs(pParticle->GetGeoCutoff(fAMSEvent));
  float rigidity = ichg != 0 ? momentum/ichg : 0.0;
  if (rigidity <= 0.9 * cutOff || !(rigidity < 1.5 * cutOff || ecalEnergy / rigidity > 0.5))
    return false;

  int pos = (int)(ichg>0); // 0:Neg  1:Pos
  int neg = (int)(ichg<0); // 1:Neg  0:Pos
  double glon = 0, glat = 0, RPTO[3] = { 0, 0, 0 }, time;
  int resultBacktracing = -1;
  int statusBacktracing = -1;
  statusOfAllBacktracings = 0;
  statusOfRigidityBacktracing = 0;
  statusOfEnergyBacktracing = 0;

  if (fabs(rigidity) < 1.5 * fabs(pParticle->GetGeoCutoff(AMSEventR::Head()))) { // below 1.5*StoermerMaxCutoff
    if (AMSEventR::Head()->DoBacktracing(resultBacktracing, statusBacktracing, glon, glat, RPTO, time, dir.gettheta(), dir.getphi(), momentum, beta, -ichg) >= 0)
      statusOfAllBacktracings += statusBacktracing << (2 * neg); // set bits for opposite charge in lower nibble  (from Tracker rigidity)
  }

  // do BackTracing for (momentum, beta, ichg) in any case:
  if (AMSEventR::Head()->DoBacktracing(resultBacktracing, statusBacktracing, glon, glat, RPTO, time, dir.gettheta(), dir.getphi(), momentum, beta, ichg) >= 0) {
    statusOfAllBacktracings += statusBacktracing << (2 * pos);   // set bits for particle charge in lower nibble  (from Tracker rigidity)

    // save BackTracing result with Rigidity and particle charge    - BT_glon/glat will be saved in ACQtProducer-Particle
    statusOfRigidityBacktracing = statusBacktracing;
    latitudeFromRigidity = glat;
    longitudeFromRigidity = glon;
  } 

  if (fabs(ecalEnergy / momentum) > 0.5) { // electron/positron candidate - backtrace also for -ecalEnergy and +ecalEnergy:
    if (AMSEventR::Head()->DoBacktracing(resultBacktracing, statusBacktracing, glon, glat, RPTO, time, dir.gettheta(), dir.getphi(), fabs(ecalEnergy), beta, -ichg) >= 0)
      statusOfAllBacktracings += statusBacktracing << (2 * (2 + neg)); // set bits for opposite charge in upper nibble (from ECAL energy) 

    if (AMSEventR::Head()->DoBacktracing(resultBacktracing, statusBacktracing, glon, glat, RPTO, time, dir.gettheta(), dir.getphi(), fabs(ecalEnergy), beta, ichg) >= 0) {
      statusOfAllBacktracings += statusBacktracing << (2 * (2 + pos)); // set bits for particle charge in upper nibble (from ECAL energy)

      // overwrite BackTracing result with ECAL-energy and particle charge for electrons
      statusOfEnergyBacktracing = statusBacktracing;
      latitudeFromEnergy = glat;
      longitudeFromEnergy = glon;
    }
  }

  return true;
}
