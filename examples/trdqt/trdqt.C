#include "root_RVSP.h"
#include "amschain.h"
#include "HistoMan.h"
#include "TrdQt.hh"

// Preseletion by V.Vagelli (status: 06.11.2012)
#include "preselect_ele_pos.h"

#include <algorithm>
#include <signal.h>
#include <TStopwatch.h>

bool ProcessEvent(AMSEventR*);
void BookHistos();

bool stopProcessing = false;
void SignalHandler(int) {

  stopProcessing = true;
}

TrdQt trdQt;
float accumulatedRealTime = 0;
float accumulatedCPUTime = 0;

int main(int argc, char** argv) {

  long long maxEvents;
  char datafiles[255];
  if (argc == 2){
    sprintf(datafiles,"%s",argv[1]);
    maxEvents = 9999999;
  } else if(argc == 3) {
    sprintf(datafiles,"%s", argv[1]);
    maxEvents = atoll(argv[2]);
  } else {
    printf("Usage: %s  <fileset_to_analyze> [max_number_of_events]\n", argv[0]);
    exit(-1);
  }

  // Register signal handler.
  signal(SIGTERM, SignalHandler);
  signal(SIGINT, SignalHandler);

  // Enable AMS histogram manager 
  hman.Enable();
  hman.Setname("TrdQtExample.root");
  BookHistos();

  AMSChain amsChain;
  amsChain.Add(datafiles);

  long long trdProcessedEvents = 0;
  maxEvents = std::min(amsChain.GetEntries(), maxEvents);
  TStopwatch watch;
  for (long long event = 0; event < maxEvents; ++event) {
    if(event % 10000 == 0)
      printf("Processed %7lli out of %7lli\n", event, maxEvents);

    AMSEventR* pEvent = amsChain.GetEvent(event);

    // Preseletion by V.Vagelli (status: 06.11.2012)
    if (!Clean_Event(pEvent))
      continue;

    // Process TrdQt and fill histograms
    watch.Start();
    if (ProcessEvent(pEvent)) {
      watch.Stop();

      accumulatedRealTime += watch.RealTime();
      accumulatedCPUTime += watch.CpuTime();
      ++trdProcessedEvents;

      if (trdProcessedEvents % 1000 == 0) {
        std::printf(" TrdQt Mean real time per event: %5.1f [ms]", accumulatedRealTime / trdProcessedEvents * 1000.0);
        std::printf(" Mean CPU time per event: %5.1f [ms]", accumulatedCPUTime / trdProcessedEvents * 1000.0);
        std::cout << std::endl << std::flush;

        accumulatedRealTime = 0;
        accumulatedCPUTime = 0;
        trdProcessedEvents = 0;
      } else
        watch.Stop();
    }

    if (stopProcessing)
      break;
  }

  hman.Save();
  return 1;
}

void BookHistos() {

  hman.Add(new TH1F("nTrdActiveLayers_all", "Number of active TRD layers; Number of layers; #", 21, -0.5, 20.5));
  hman.Add(new TH1F("nTrdActiveLayers_in_acceptance", "Number of active TRD layers - in TRD geometrical acceptance; Number of layers; #", 21, -0.5, 20.5));
  hman.Add(new TH1F("nTrdActiveLayers_passes_preselection_cuts", "Number of active TRD layers - passes TRD preselection cuts; Number of layers; #", 21, -0.5, 20.5));
  hman.Add(new TH1F("nTrdActiveLayers_passes_all_cuts", "Number of active TRD layers - passes TRD preselection cuts & candidate matching; Number of layers; #", 21, -0.5, 20.5));
  hman.Add(new TH1F("logLikelihoodRatioElectronProton", "Log likelihood Ratio e/p; Likelihood ratio; #", 1000, 0, 5));
}

bool ProcessEvent(AMSEventR* pEvent) {

  int numberOfTracks = pEvent->nTrTrack();
  if (numberOfTracks <= 0)
    return false;

  // Query TRD information by passing the trackIndex 0 to operate on the first tracker track, and the 1,3,3 tracker track fit.
  if (!trdQt.ProcessEvent(pEvent, 0, 1, 3, 3))
    return false;

  hman.Fill("nTrdActiveLayers_all", trdQt.GetNumberOfActiveLayers());

  if (trdQt.IsInsideTrdGeometricalAcceptance()) {
    hman.Fill("nTrdActiveLayers_in_acceptance", trdQt.GetNumberOfActiveLayers());

    if (trdQt.PassesTrdPreselectionCuts()) {
      hman.Fill("nTrdActiveLayers_passes_preselection_cuts", trdQt.GetNumberOfActiveLayers());
      
      if (trdQt.UsefulForTrdParticleId())
        hman.Fill("nTrdActiveLayers_passes_all_cuts", trdQt.GetNumberOfActiveLayers());

      hman.Fill("logLikelihoodRatioElectronProton", trdQt.LogLikelihoodRatioElectronProton());
    }
  }

  return true;
}
