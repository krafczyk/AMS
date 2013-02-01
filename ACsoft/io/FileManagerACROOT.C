#include "FileManager.hh"

#include "ReducedEvent.hh"
#include "ReducedFileHeader.hh"
#include "TimeHistogramManager.hh"

#include <TChain.h>
#include <TROOT.h>
#include <TStopwatch.h>

#include <cmath>

#define DEBUG 0
#define INFO_OUT_TAG "FileManagerACROOT> "
#include <debugging.hh>

namespace ACsoft {

namespace IO {

void FileManager::DestructACROOTFiles() {

  delete fReducedEvent;
  // Don't delete fReducedFileHeader - it's stored in the TTrees userinfo.
  delete fChain;
  // Don't delete fCustomChain - it's a friend of the former tree and deletion is handled for those
}

void FileManager::RetrieveFirstAndLastEventTimeForACROOTFiles() {

  Q_ASSERT(fChain);
  Q_ASSERT(fEntries > 0);
  Q_ASSERT(fReducedEvent);

  fChain->GetEntry(0);
  TTimeStamp timeOfFirstEvent = fReducedEvent->EventTime;
  INFO_OUT << "First event time: " << timeOfFirstEvent.AsString() << std::endl;

  fChain->GetEntry(fChain->GetEntries() - 1);
  TTimeStamp timeOfLastEvent = fReducedEvent->EventTime;
  INFO_OUT << "Last event time: " << timeOfLastEvent.AsString() << std::endl;

  fCurrentEventNumber = -1;

  Utilities::TimeHistogramManager::instance()->SetFirstAndLastEventTimes( timeOfFirstEvent, timeOfLastEvent );
  int nbins;
  double xmin, xmax;
  Utilities::TimeHistogramManager::instance()->GetTimeBinning(nbins,xmin,xmax);
}

bool FileManager::InitializeACROOTFiles(const std::vector<std::string>& files, const std::vector<std::string>& customFiles) {

  Q_ASSERT(!fReducedEvent);
  Q_ASSERT(!fChain);
  Q_ASSERT(!fCustomChain);
  TChain* chain = new TChain("ACtree");
  chain->SetCacheSize(150000000);
  chain->AddBranchToCache("*");

  unsigned int nFiles = 0;
  for (unsigned int i = 0; i < files.size(); ++i) {
    std::string file = files[i];
    ExpandEnvironmentVariables(file);
    if (!DoesFileExist(file))
      continue;

    INFO_OUT << "Adding file " << file << " to list of ACROOT files." << std::endl;
    chain->Add(file.c_str());
    ++nFiles;
  }

  if (!nFiles) {
    WARN_OUT << "No files added to list of ACROOT files!" << std::endl;
    delete chain;
    return false;
  }

  Q_ASSERT(!fEntries);
  if (chain->GetEntries() <= 0) {
    WARN_OUT << "No events found in ACROOT files!" << std::endl;
    delete chain;
    return false;
  }

  chain->LoadTree(0);
  fReducedFileHeader = (::IO::ReducedFileHeader*) chain->GetTree()->GetUserInfo()->At(0)->Clone();
  Q_ASSERT(fReducedFileHeader);
  if (!fReducedFileHeader) {
    WARN_OUT << "ACROOT files are missing a file header! Are you trying to open an old tree? This no longer works, aborting." << std::endl;
    delete chain;
    return false;
  }

  TChain* customChain = 0;
  unsigned int nCustomFiles = 0;
  if (!customFiles.empty()) {
    customChain = new TChain("ACtree_custom");

    for (unsigned int i = 0; i < customFiles.size(); ++i) {
      std::string file = customFiles[i];
      ExpandEnvironmentVariables(file);
      if (!DoesFileExist(file))
        continue;

      INFO_OUT << "Adding file " << file << " to list of ACROOT files with custom variables." << std::endl;
      customChain->Add(file.c_str());
      ++nCustomFiles;
    }

    if (!nCustomFiles) {
      WARN_OUT << "No files added to list of ACROOT files with custom variables!" << std::endl;
      delete chain;
      delete customChain;
      return false;
    }

    if (customChain->GetEntries() <= 0) {
      WARN_OUT << "No events found in ACROOT files with custom variables!" << std::endl;
      delete chain;
      delete customChain;
      return false;
    }

    if (customChain->GetEntries() != chain->GetEntries()) {
      WARN_OUT << "ACROOT files contain " << chain->GetEntries() << " entries, and custom files " << customChain->GetEntries() << " entries! Aborting." << std::endl;
      delete chain;
      delete customChain;
      return false;
    }

    customChain->LoadTree(0);
    ::IO::ReducedFileHeader* customFileHeader = (::IO::ReducedFileHeader*) customChain->GetTree()->GetUserInfo()->At(0);
    if (customFileHeader->GitSHA() != fReducedFileHeader->GitSHA()) {
      WARN_OUT << "ACROOT files built from ACQt files with Git SHA " << fReducedFileHeader->GitSHA()
                << " and custom files from ACQt files with Git SHA " << customFileHeader->GitSHA()
                << ". Aborting." << std::endl;
      delete chain;
      delete customChain;
      return false;
    }

    // FIXME: Validate run & event number for every entry.
    fCustomChain = customChain;
    chain->AddFriend("ACtree_custom");
  }

  gROOT->ProcessLine("#include <vector>");
  fChain = chain;
  fEntries = fChain->GetEntries();
  SetACROOTBranchAddresses();
  fNumberOfFiles = nFiles;

  INFO_OUT << "Created ACROOT file list with " << GetEntries() << " events from " << nFiles << " files." << std::endl;
  if (fCustomChain)
    INFO_OUT << "Created ACROOT custom file list with " << fCustomChain->GetEntries() << " events from " << nCustomFiles << " files." << std::endl;
  RetrieveFirstAndLastEventTimeForACROOTFiles();
  return true;
}

bool FileManager::GetNextACROOTEvent() {

  Q_ASSERT(fChain);
  Q_ASSERT(fReducedEvent);
  if (!fRunTimer) {
    fRunTimer = new TStopwatch;
    fRunTimer->Start();
  }
  if (!fEventTimer) {
    fEventTimer = new TStopwatch;
    fEventTimer->Start();
  }

  if (fChain->GetFile() && fChain->GetFile() != fCurrentRootFile) {
    ++fCurrentFile;
    fCurrentRootFile = fChain->GetFile();
  }

  if (fReducedEvent->Run != -1 && fReducedEvent->Run != fRun) {
    fRunTimer->Stop();
    fAccumulatedRealTime += fRunTimer->RealTime();
    fAccumulatedCPUTime += fRunTimer->CpuTime();
    ++fNumberOfProcessedRuns;
    fRunTimer->Start();
    fRun = fReducedEvent->Run;
  }

  const unsigned int bunch = floor((double)fCurrentEventNumber / fEventBunchSize);
  if (fNumberOfProcessedBunches < bunch) {
    fEventTimer->Stop();
    fAccumulatedEventRealTime += fEventTimer->RealTime();
    fEventTimer->Start();
    ++fNumberOfProcessedBunches;
  }

  ++fCurrentEventNumber;
  if (fCurrentEventNumber >= GetEntries())
    return false;

  fChain->GetEntry(fCurrentEventNumber);
  return true;
}

bool FileManager::GetACROOTEvent(int index) {

  Q_ASSERT(fChain);
  return fChain->GetEntry(index);
}

void FileManager::ResetACROOTFiles() {

  if (fChain)
    fChain->Reset();
  
  delete fReducedEvent;
  fReducedEvent = 0;
}

void FileManager::SetACROOTBranchAddresses() {

  Q_ASSERT(!fReducedEvent);
  Q_ASSERT(fReducedFileHeader);
  fReducedEvent = new ReducedEvent(fReducedFileHeader);

  fChain->SetBranchAddress("Run",                  &fReducedEvent->Run);
  fChain->SetBranchAddress("Event",                &fReducedEvent->Event);
  fChain->SetBranchAddress("RunTag",               &fReducedEvent->RunTag);
  fChain->SetBranchAddress("BadRunTag",            &fReducedEvent->BadRunTag);
  fChain->SetBranchAddress("EventTime",            &fReducedEvent->EventTime);
  fChain->SetBranchAddress("TriggerRate",          &fReducedEvent->TriggerRate);
  fChain->SetBranchAddress("TriggerLifeTime",      &fReducedEvent->TriggerLifeTime);
  fChain->SetBranchAddress("LongitudeDeg",         &fReducedEvent->LongitudeDeg);
  fChain->SetBranchAddress("LatitudeDeg",          &fReducedEvent->LatitudeDeg);;
  fChain->SetBranchAddress("MagLatDeg",            &fReducedEvent->MagLatDeg);
  fChain->SetBranchAddress("MagLongDeg",           &fReducedEvent->MagLongDeg);
  fChain->SetBranchAddress("GeomagneticMaxCutOff", &fReducedEvent->GeomagneticMaxCutOff);
  fChain->SetBranchAddress("GeomagneticMaxCutOffCone",  &fReducedEvent->GeomagneticMaxCutOffCone);

  fChain->SetBranchAddress("nTrkTrks",             &fReducedEvent->nTrkTrks);
  fChain->SetBranchAddress("TrkHits",              &fReducedEvent->TrkHits);
  fChain->SetBranchAddress("TrackerLayerPatternClassification", &fReducedEvent->TrackerLayerPatternClassification);
  fChain->SetBranchAddress("Rig",                  &fReducedEvent->Rig);
  fChain->SetBranchAddress("eRinv",                &fReducedEvent->eRinv);
  fChain->SetBranchAddress("Chi2TrkX",             &fReducedEvent->Chi2TrkX);
  fChain->SetBranchAddress("Chi2TrkY",             &fReducedEvent->Chi2TrkY);
  fChain->SetBranchAddress("Rigs",                 fReducedEvent->Rigs);
  fChain->SetBranchAddress("TrkPhi",               fReducedEvent->TrkPhi);
  fChain->SetBranchAddress("TrkTht",               fReducedEvent->TrkTht);
  fChain->SetBranchAddress("TrkXcors",             fReducedEvent->TrkXcors);
  fChain->SetBranchAddress("TrkYcors",             fReducedEvent->TrkYcors);
  fChain->SetBranchAddress("TrkXhits",             fReducedEvent->TrkXhits);
  fChain->SetBranchAddress("TrkYhits",             fReducedEvent->TrkYhits);
  fChain->SetBranchAddress("TrkSigLay1",           &fReducedEvent->TrkSigLay1);
  fChain->SetBranchAddress("TrkC",                 &fReducedEvent->TrkC);
  fChain->SetBranchAddress("TrkCe",                &fReducedEvent->TrkCe);

  fChain->SetBranchAddress("RichC",                &fReducedEvent->RichC);
  fChain->SetBranchAddress("RichCe",               &fReducedEvent->RichCe);
  fChain->SetBranchAddress("RichBeta",             &fReducedEvent->RichBeta);

  fChain->SetBranchAddress("nEcalShr",             &fReducedEvent->nEcalShr);
  fChain->SetBranchAddress("EcalEnergy",           &fReducedEvent->EcalEnergy);
  fChain->SetBranchAddress("EcalBDT",              &fReducedEvent->EcalBDT);
  fChain->SetBranchAddress("EcalSum",              &fReducedEvent->EcalSum);

  fChain->SetBranchAddress("nTofHits",             &fReducedEvent->nTofHits);
  fChain->SetBranchAddress("TofBeta",              &fReducedEvent->TofBeta);
  fChain->SetBranchAddress("TofC",                 &fReducedEvent->TofC);
  fChain->SetBranchAddress("TofCe",                &fReducedEvent->TofCe);
  fChain->SetBranchAddress("TofMeanE",             &fReducedEvent->TofMeanE);
  fChain->SetBranchAddress("TofMaxE",              &fReducedEvent->TofMaxE);
  fChain->SetBranchAddress("nAccHits",             &fReducedEvent->nAccHits);

  fChain->SetBranchAddress("ParticleId",           &fReducedEvent->ParticleId);
  fChain->SetBranchAddress("LR_Elec_Prot",         &fReducedEvent->LR_Elec_Prot);
  fChain->SetBranchAddress("LR_Heli_Elec",         &fReducedEvent->LR_Heli_Elec);
  fChain->SetBranchAddress("LR_Heli_Prot",         &fReducedEvent->LR_Heli_Prot);
  fChain->SetBranchAddress("LR_Elec_Prot_MC",      &(fReducedEvent->LR_Elec_Prot_MC));
  fChain->SetBranchAddress("LR_Heli_Elec_MC",      &(fReducedEvent->LR_Heli_Elec_MC));
  fChain->SetBranchAddress("LR_Elec_Prot_Rndm",    &(fReducedEvent->LR_Elec_Prot_Rndm));

  fChain->SetBranchAddress("XePressure",           &fReducedEvent->XePressure);
  fChain->SetBranchAddress("TrdInAcceptance",      &fReducedEvent->TrdInAcceptance);
  fChain->SetBranchAddress("TrdActivePathLen",     &fReducedEvent->TrdActivePathLen);
  fChain->SetBranchAddress("TrdCandidatePathLen",  &fReducedEvent->TrdCandidatePathLen);
  fChain->SetBranchAddress("TrdActiveLayers",      &fReducedEvent->TrdActiveLayers);
  fChain->SetBranchAddress("TrdCandidateLayers",   &fReducedEvent->TrdCandidateLayers);
  fChain->SetBranchAddress("TrdActiveStraws",      &fReducedEvent->TrdActiveStraws);
  fChain->SetBranchAddress("TrdUnassignedHits",    &fReducedEvent->TrdUnassignedHits);
  fChain->SetBranchAddress("nTrdRawHits",          &fReducedEvent->nTrdRawHits);

  fChain->SetBranchAddress("TrdTrkDeltaAngleXZ", &fReducedEvent->TrdTrkDeltaAngleXZ );
  fChain->SetBranchAddress("TrdTrkDeltaAngleYZ", &fReducedEvent->TrdTrkDeltaAngleYZ );
  fChain->SetBranchAddress("TrdTrackCombinedFitXZChi2", &fReducedEvent->TrdTrackCombinedFitXZChi2 );
  fChain->SetBranchAddress("TrdTrackCombinedFitYZChi2", &fReducedEvent->TrdTrackStandAloneFitYZChi2 );
  fChain->SetBranchAddress("TrdTrackStandAloneFitXZChi2", &fReducedEvent->TrdTrackStandAloneFitXZChi2 );
  fChain->SetBranchAddress("TrdTrackStandAloneFitYZChi2", &fReducedEvent->TrdTrackStandAloneFitYZChi2 );

  fChain->SetBranchAddress("TofCLay1",           &fReducedEvent->TofCLay1);
  fChain->SetBranchAddress("TofCLay2",           &fReducedEvent->TofCLay2);
  fChain->SetBranchAddress("TofCLay3",           &fReducedEvent->TofCLay3);
  fChain->SetBranchAddress("TofCLay4",           &fReducedEvent->TofCLay4);
}

}

}
