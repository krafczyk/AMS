#include "McSpectrumScaler.hh"

#include <TH1D.h>
#include <TObject.h>
#include <TF1.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLegendEntry.h>

#include <assert.h>

#include "FileManager.hh"
#include "MC.h"
#include "MCEventGenerator.h"
#include "AnalysisParticle.hh"
#include "Event.h"
#include "ObjectManager.hh"
#include "McGeneratedEventsCalculation.hh"
#include "ParticleFactory.hh"
#include "Utilities.hh"

#define DEBUG 0
#define INFO_OUT_TAG "McSpectrumScaler> "
#include <debugging.hh>

ACsoft::Utilities::McSpectrumScaler::McSpectrumScaler()
: m_objectManager(0)
, m_saveHistograms(false)
{

}

ACsoft::Utilities::McSpectrumScaler::McSpectrumScaler(ConfigHandler* cfghandler, const std::string& resultdir, const std::string& suffix)
: m_objectManager(new ObjectManager("McSpectrumScaler", cfghandler, resultdir, suffix))
, m_saveHistograms(false)
{
  m_saveHistograms = cfghandler->GetFlag("McSpectrumScaler", "save", "Run event loop.");
  m_objectManager->SetPrefix("mcScalingHistograms");
}

ACsoft::Utilities::McSpectrumScaler::~McSpectrumScaler() {
  for (QMap<McParticleId::McSpecies, TH1D*>::iterator iter = m_triggeredHistograms.begin(); iter != m_triggeredHistograms.end(); ++iter) {
    assert(m_isExternalHistogram.contains(iter.key()));
    if (!m_isExternalHistogram[iter.key()]) {
      delete iter.value();
      iter.value() = 0;
    }
  }
  qDeleteAll(m_generatedHistograms);
  qDeleteAll(m_triggeredHistogramsKineticEnergy);
  qDeleteAll(m_generatedHistogramsKineticEnergy);
  delete m_objectManager;
}

void ACsoft::Utilities::McSpectrumScaler::Add(McParticleId::McSpecies mcSpecies, const std::vector<double>& binning) {
  const QString& name = CheckAddedParticAndGetName(mcSpecies);
  TH1D* histogram = new TH1D(qPrintable(name), qPrintable(name + ";Momentum (GeV)"), binning.size()-1, binning.data());
  m_triggeredHistograms.insert(mcSpecies, (m_objectManager) ? m_objectManager->Add(histogram): histogram);
  histogram = new TH1D(qPrintable(name + "KineticEnergy"), qPrintable(name + ";Kinetic energy (GeV)"), binning.size()-1, binning.data());
  m_triggeredHistogramsKineticEnergy.insert(mcSpecies, (m_objectManager) ? m_objectManager->Add(histogram): histogram);
  const QString generatedName = QString("McSpectrumScaler_generated_%1").arg(ACsoft::Utilities::McParticleId::Id(mcSpecies));
  histogram = new TH1D(qPrintable(generatedName), qPrintable(generatedName + ";Momentum (GeV)"), binning.size()-1, binning.data());
  m_generatedHistograms.insert(mcSpecies, (m_objectManager) ? m_objectManager->Add(histogram): histogram);
  histogram = new TH1D(qPrintable(generatedName + "KineticEnergy"), qPrintable(generatedName + ";Kinetic energy (GeV)"), binning.size()-1, binning.data());
  m_generatedHistogramsKineticEnergy.insert(mcSpecies, (m_objectManager) ? m_objectManager->Add(histogram): histogram);
  m_isExternalHistogram.insert(mcSpecies, false);
}

void ACsoft::Utilities::McSpectrumScaler::Add(McParticleId::McSpecies mcSpecies, const std::vector<double>& binning, TGraph* spectrum) {
  Add(mcSpecies, binning);
  AddSpectrum(mcSpecies, spectrum);
}

void ACsoft::Utilities::McSpectrumScaler::Add(McParticleId::McSpecies mcSpecies, const std::vector<double>& binning, TF1* spectrum) {
  Add(mcSpecies, binning);
  AddSpectrum(mcSpecies, spectrum);
}

void ACsoft::Utilities::McSpectrumScaler::Add(McParticleId::McSpecies mcSpecies, int nBinsX, double xMin, double xMax) {
  const QString& name = CheckAddedParticAndGetName(mcSpecies);
  TH1D* histogram = new TH1D(qPrintable(name), qPrintable(name + ";Momentum (GeV)"), nBinsX, xMin, xMax);
  m_triggeredHistograms.insert(mcSpecies, (m_objectManager) ? m_objectManager->Add(histogram): histogram);
  histogram = new TH1D(qPrintable(name + "KineticEnergy"), qPrintable(name + ";Kinetic energy (GeV)"), nBinsX, xMin, xMax);
  m_triggeredHistogramsKineticEnergy.insert(mcSpecies, (m_objectManager) ? m_objectManager->Add(histogram): histogram);
  const QString generatedName = QString("McSpectrumScaler_generated_%1").arg(ACsoft::Utilities::McParticleId::Id(mcSpecies));
  histogram = new TH1D(qPrintable(generatedName), qPrintable(generatedName + ";Momentum (GeV)"), nBinsX, xMin, xMax);
  m_generatedHistograms.insert(mcSpecies, (m_objectManager) ? m_objectManager->Add(histogram): histogram);
  histogram = new TH1D(qPrintable(generatedName + "KineticEnergy"), qPrintable(generatedName + ";Kinetic energy (GeV)"), nBinsX, xMin, xMax);
  m_generatedHistogramsKineticEnergy.insert(mcSpecies, (m_objectManager) ? m_objectManager->Add(histogram): histogram);
  m_isExternalHistogram.insert(mcSpecies, false);
}

void ACsoft::Utilities::McSpectrumScaler::Add(McParticleId::McSpecies mcSpecies, int nBinsX, double xMin, double xMax, TGraph* spectrum) {
  Add(mcSpecies, nBinsX, xMin, xMax);
  AddSpectrum(mcSpecies, spectrum);
}

void ACsoft::Utilities::McSpectrumScaler::Add(McParticleId::McSpecies mcSpecies, int nBinsX, double xMin, double xMax, TF1* spectrum) {
  Add(mcSpecies, nBinsX, xMin, xMax);
  AddSpectrum(mcSpecies, spectrum);
}

void ACsoft::Utilities::McSpectrumScaler::Add(McParticleId::McSpecies mcSpecies, TH1D* histogram) {
  assert(!m_triggeredHistograms.contains(mcSpecies));
  m_triggeredHistograms.insert(mcSpecies, histogram);
  m_isExternalHistogram.insert(mcSpecies, true);
}

int ACsoft::Utilities::McSpectrumScaler::GetNumberOfAddedParticles() const {
  return m_triggeredHistograms.size();
}

TH1D* ACsoft::Utilities::McSpectrumScaler::GetTriggeredHistogram(McParticleId::McSpecies mcSpecies, McBinningMode binningMode) {
  switch (binningMode) {
    case MomentumBinning:
      return m_triggeredHistograms[mcSpecies];
    case KineticEnergyBinning:
      return m_triggeredHistogramsKineticEnergy[mcSpecies];

    default:
      return 0;
  }
}

const TH1D* ACsoft::Utilities::McSpectrumScaler::GetTriggeredHistogram(McParticleId::McSpecies mcSpecies, McBinningMode binningMode) const {
  return GetTriggeredHistogram(mcSpecies, binningMode);
}

TH1D* ACsoft::Utilities::McSpectrumScaler::CorrectedTriggeredHistogram(McParticleId::McSpecies mcSpecies) const {
  return CreateCorrectedHistogram(m_triggeredHistograms[mcSpecies], m_scalingFactors[mcSpecies]);
}

TH1D* ACsoft::Utilities::McSpectrumScaler::GetGeneratedHistogram(McParticleId::McSpecies mcSpecies, McBinningMode binningMode) {
  switch (binningMode) {
    case MomentumBinning:
      return m_generatedHistograms[mcSpecies];
    case KineticEnergyBinning:
      return m_generatedHistogramsKineticEnergy[mcSpecies];

    default:
      return 0;
  }
}

const TH1D* ACsoft::Utilities::McSpectrumScaler::GetGeneratedHistogram(McParticleId::McSpecies mcSpecies, McBinningMode binningMode) const {
  return GetGeneratedHistogram(mcSpecies, binningMode);
}

TH1D* ACsoft::Utilities::McSpectrumScaler::CorrectedGeneratedHistogram(McParticleId::McSpecies mcSpecies) const {
  return CreateCorrectedHistogram(m_generatedHistograms[mcSpecies], m_scalingFactors[mcSpecies]);
}

void ACsoft::Utilities::McSpectrumScaler::FillHistograms(const std::string& input) {
  if (m_objectManager && m_objectManager->HasFile()) {
    INFO_OUT << "Histograms for scaling will be taken from file " << m_objectManager->GetFilename() << std::endl;
    QString output("Particles with IDs: ");
    for (QMap<McParticleId::McSpecies, TH1D*>::const_iterator iter = m_triggeredHistograms.constBegin(); iter != m_triggeredHistograms.constEnd(); ++iter)
      output += QString("%1, ").arg(iter.key());
    output += "are added for scaling.";
    INFO_OUT << output.toStdString() << std::endl;
  } else {
  IO::FileManager fileManager(IO::DontDumpSettings);
  if(!fileManager.ReadFileList(input))
    assert(fileManager.ReadFile(input));
  INFO_OUT << "Scaling run started. Looping over " << fileManager.GetEntries() << " events..." << std::endl;
  QMap<McParticleId::McSpecies, McGeneratedEventsCalculation> generatedEventCalculations;
  foreach (McParticleId::McSpecies mcSpecies, m_generatedHistograms.keys()) {
    generatedEventCalculations.insert(mcSpecies, McGeneratedEventsCalculation());
  }
  Analysis::ParticleFactory particleFactory;

  while (fileManager.GetNextEvent()) {
    fileManager.DumpEventLoopProgress(20000, isBatchMode() ? '\n' : '\r');
    Analysis::Particle particle;
    particleFactory.PrepareWithDefaultObjects(fileManager.Event(), fileManager.EventReduced(), particle);
    if (!particle.IsMC())
      break;
    if (!particle.HasPrimaryEventGenerator()) // FIXME: assert here when primary eventgenerator is available for all mc events
      continue;
    const McParticleId::McSpecies mcSpecies = McParticleId::Species(particle.McParticleId());
    if (mcSpecies == McParticleId::NoSpecies)
      continue;

    double min = particle.McMinimumMomentum();
    double max = particle.McMaximumMomentum();
    const double momentumToFill = BinnedValue(&particle, MomentumBinning);
    if (momentumToFill < min || momentumToFill > max)
      continue;

    if (!m_masses.contains(mcSpecies)) {
      if (particle.RawEvent()) {
        const double generatorMass = particle.RawEvent()->MC().PrimaryEventGenerator()->Mass();
        const double particleIdMass = McParticleId::Mass(mcSpecies);
        if (generatorMass - particleIdMass > 1e-3) {
          WARN_OUT << "McEventGenerator mass (" << generatorMass << " GeV) and mass from McParticleId (" << particleIdMass << "GeV) differ too much for particle " << McParticleId::Name(mcSpecies) << ". Please fix masses!" << std::endl;
          assert(generatorMass - particleIdMass > 1e-3);
        }
      }
      m_masses.insert(mcSpecies, McParticleId::Mass(mcSpecies));
    }
    if (m_triggeredHistograms.contains(mcSpecies)) {
      m_triggeredHistograms[mcSpecies]->Fill(momentumToFill);
      assert(m_triggeredHistogramsKineticEnergy[mcSpecies]);
      m_triggeredHistogramsKineticEnergy[mcSpecies]->Fill(BinnedValue(&particle, KineticEnergyBinning));
    }
    if (generatedEventCalculations.contains(mcSpecies))
      generatedEventCalculations[mcSpecies].Fill(&particle);
  }
  for (QMap<McParticleId::McSpecies, McGeneratedEventsCalculation>::iterator iter = generatedEventCalculations.begin(); iter != generatedEventCalculations.end(); ++iter) {
    if (!m_masses.contains(iter.key())) {
      INFO_OUT << "No " << McParticleId::Name(iter.key()) << " events processed but added to scaler. Skipping calculation of generated mc histograms for this particle species." << std::endl;
      continue;
    }
    assert(m_generatedHistograms.contains(iter.key()));
    iter.value().FillLogHistos(m_objectManager);
    iter.value().FillGeneratedHistos(m_generatedHistograms[iter.key()], MomentumBinning, m_masses[iter.key()]);
    iter.value().FillGeneratedHistos(m_generatedHistogramsKineticEnergy[iter.key()], KineticEnergyBinning, m_masses[iter.key()]);
  }
  if (m_objectManager && m_saveHistograms)
    SaveHistograms();
  INFO_OUT << "Scaling run finished." << std::endl;
  }
}

void ACsoft::Utilities::McSpectrumScaler::CalculateWeights(McParticleId::McSpecies mcSpecies, const TF1* spectrum, bool useCorrection) {
  AddSpectrum(mcSpecies, spectrum);
  CalculateWeightsInternal(mcSpecies, spectrum, useCorrection);
}

void ACsoft::Utilities::McSpectrumScaler::CalculateWeights(McParticleId::McSpecies mcSpecies, const TGraph* spectrum, bool useCorrection) {
  AddSpectrum(mcSpecies, spectrum);
  CalculateWeightsInternal(mcSpecies, spectrum, useCorrection);
}

void ACsoft::Utilities::McSpectrumScaler::CalculateWeights(McParticleId::McSpecies mcSpecies, McParticleId::McSpecies mcSpeciesToUse, const TF1* spectrum, bool useCorrection) {
  AddSpectrum(mcSpecies, spectrum);
  assert(m_triggeredHistograms.contains(mcSpeciesToUse));
  assert(m_generatedHistograms.contains(mcSpeciesToUse));
  ReplaceHistogramContent(m_triggeredHistograms[mcSpeciesToUse], m_triggeredHistograms[mcSpecies]);
  ReplaceHistogramContent(m_generatedHistograms[mcSpeciesToUse], m_generatedHistograms[mcSpecies]);
  CalculateWeightsInternal(mcSpecies, spectrum, useCorrection);
}

void ACsoft::Utilities::McSpectrumScaler::CalculateWeights(McParticleId::McSpecies mcSpecies, McParticleId::McSpecies mcSpeciesToUse, const TGraph* spectrum, bool useCorrection) {
  AddSpectrum(mcSpecies, spectrum);
  assert(m_triggeredHistograms.contains(mcSpeciesToUse));
  assert(m_generatedHistograms.contains(mcSpeciesToUse));
  ReplaceHistogramContent(m_triggeredHistograms[mcSpeciesToUse], m_triggeredHistograms[mcSpecies]);
  ReplaceHistogramContent(m_generatedHistograms[mcSpeciesToUse], m_generatedHistograms[mcSpecies]);
  CalculateWeightsInternal(mcSpecies, spectrum, useCorrection);
}

void ACsoft::Utilities::McSpectrumScaler::CalculateWeights(bool useCorrection) {
  assert(m_spectra.size());
  for (QMap<McParticleId::McSpecies, const TObject*>::const_iterator iter = m_spectra.constBegin(); iter != m_spectra.constEnd(); ++iter)
    CalculateWeightsInternal(iter.key(), iter.value(), useCorrection);
}

double ACsoft::Utilities::McSpectrumScaler::GetWeight(const Analysis::Particle* particle) const {
  if (!particle->IsMC())
    return 1;
  const McParticleId::McSpecies mcSpecies = McParticleId::Species(particle->McParticleId());
  return GetWeight(particle, mcSpecies);
}

double ACsoft::Utilities::McSpectrumScaler::GetWeight(const Analysis::Particle* particle, const McParticleId::McSpecies mcSpecies) const {
  if (!particle->IsMC())
    return 1;
  if (!particle->HasPrimaryEventGenerator())
    return 0;
  if (!m_triggeredHistograms.contains(mcSpecies))
    return 0;
  assert(!m_scalingFactors.empty());
  // Assuming same binning for triggered and gerneated histograms
  const TH1D* histogram = m_triggeredHistograms[mcSpecies];
  const double momentum = BinnedValue(particle, MomentumBinning);
  const int bin = histogram->FindFixBin(momentum);
  // Asking if value is outside the binning then rturning 0 as weight!
  if (bin < 1 || bin > histogram->GetNbinsX())
    return 0;
  double min = particle->McMinimumMomentum();
  double max = particle->McMaximumMomentum();
  if (momentum < min || momentum > max)
    return 0;
  return m_scalingFactors[mcSpecies].at(bin);
}

const TObject* ACsoft::Utilities::McSpectrumScaler::GetSpectrum(McParticleId::McSpecies mcSpecies) const {
  if (!m_spectra.contains(mcSpecies)) {
    WARN_OUT << "Spectrum for " << McParticleId::Name(mcSpecies) << " not added." << std::endl;
    return 0;
  }
  return m_spectra[mcSpecies];
}

QString ACsoft::Utilities::McSpectrumScaler::CheckAddedParticAndGetName(McParticleId::McSpecies mcSpecies) const {
  assert(!m_triggeredHistograms.contains(mcSpecies));
  return QString("McSpectrumScaler_triggered_%1").arg(ACsoft::Utilities::McParticleId::Id(mcSpecies));
}

void ACsoft::Utilities::McSpectrumScaler::AddSpectrum(McParticleId::McSpecies mcSpecies, const TObject* spectrum) {
  if (m_spectra.contains(mcSpecies))
    INFO_OUT << "Scaling factors for " << McParticleId::Name(mcSpecies) << " were already calculated. Replacing old ones!" << std::endl;;
  m_spectra.insert(mcSpecies, spectrum);
}

void ACsoft::Utilities::McSpectrumScaler::CalculateWeightsInternal(McParticleId::McSpecies mcSpecies, const TObject* spectrum, bool useCorrection) {
  assert(m_generatedHistograms[mcSpecies]);
  assert(m_triggeredHistograms[mcSpecies]);
  const TH1D* histogram = (useCorrection) ? m_generatedHistograms[mcSpecies]: m_triggeredHistograms[mcSpecies];
  TH1D scaledHistogram(*histogram);
  scaledHistogram.Scale(1, "width");
  QList<double> factors;
  for(int i = 0; i <= scaledHistogram.GetNbinsX() + 1; ++i) {
    const double nEntries = scaledHistogram.GetBinContent(i);
    const double binnedValue = scaledHistogram.GetBinCenter(i);
    const double factor = (nEntries > 0) ? SpectrumValue(binnedValue, spectrum) / nEntries : 0;
    factors.append(factor);
  }
  m_scalingFactors.insert(mcSpecies, factors);
}

double ACsoft::Utilities::McSpectrumScaler::SpectrumValue(double valueToEval, const TObject* spectrum) const {
  double evaluated = 0;
  if (dynamic_cast<const TF1*>(spectrum))
    evaluated = dynamic_cast<const TF1*>(spectrum)->Eval(valueToEval);
  else if (dynamic_cast<const TGraph*>(spectrum))
    evaluated = dynamic_cast<const TGraph*>(spectrum)->Eval(valueToEval);
  else {
    WARN_OUT << "Other passing other objects is not implemented now." << std::endl;;
    assert(0);
  }
  return evaluated;
}

double ACsoft::Utilities::McSpectrumScaler::BinnedValue(const Analysis::Particle* particle, ACsoft::Utilities::McBinningMode binningMode) const {
  const double momentum = particle->McMomentum();
  switch (binningMode) {
    case ACsoft::Utilities::KineticEnergyBinning:
    {
      // E_kin = sqrt(momentum * momentum + mass * mass) - mass
      const double mass = McParticleId::Mass(McParticleId::Species(particle->McParticleId()));
      assert(mass >= 0);
      return sqrt(momentum * momentum + mass * mass) - mass;
    }
    case ACsoft::Utilities::MomentumBinning:
      return momentum;
    default:
      assert(0);
  }
  assert(false);
  return 0;
}

double ACsoft::Utilities::McSpectrumScaler::PowerSpectrum(double* momentum, double* par) {
  const double N0 = par[0];
  const double alpha = par[1];
  const double phi = par[2];
  const double M = par[3];

  const double T = sqrt(momentum[0] * momentum[0] + M * M) - M;

  const double T0 = T + phi;

  const double flux = N0 * pow(T0, alpha) * T * (T + 2. * M) / (T0 * (T0 + 2. * M));

  return flux;
}

void ACsoft::Utilities::McSpectrumScaler::SetMaximumMinimum(const std::vector<TH1*>& histograms) {
  // The first element has to be the first drawn histogram
  // Get the maximum y value of the histograms
  // Add 10% to match behaviour of ROOT's automatic scaling
  double maximum = histograms[1] ? histograms[1]->GetMaximum() : -DBL_MAX;
  for (unsigned int i = 1; i < histograms.size(); ++i) {
    TH1* h = histograms[i];
    if (h && h->GetMaximum() > maximum)
      maximum = h->GetMaximum();
  }
  if (maximum > histograms[0]->GetMaximum())
    histograms[0]->SetMaximum(1.1 * maximum);

  double minimum = histograms[0] ? MinimumNonZeroBinContent(histograms[0]) : DBL_MAX;
    for (unsigned int i = 1; i < histograms.size(); ++i) {
      TH1* h = histograms[i];
      if (h) {
        const double localMinimum = MinimumNonZeroBinContent(h);
        if (minimum > localMinimum)
          minimum = localMinimum;
      }
    }
  histograms[0]->SetMinimum (0.9 * minimum);
}

double ACsoft::Utilities::McSpectrumScaler::MinimumNonZeroBinContent(const TH1* histogram) {
  double minimum = DBL_MAX;
  for (int i = 0; i < histogram->GetNbinsX(); ++i) {
    double content = histogram->GetBinContent(i+1);
    if (content > 0. && content < minimum)
      minimum = content;
  }
  return minimum;
}

TH1D* ACsoft::Utilities::McSpectrumScaler::ApplyScaling(TH1D* histogram, const QList<double>& scalingFactors) const {
  for(int i = 0; i <= histogram->GetNbinsX() + 1; ++i) {
    const double nEntries = histogram->GetBinContent(i);
    const double factor = scalingFactors.at(i);
    histogram->SetBinContent(i, nEntries * factor);
    histogram->SetBinError(i, histogram->GetBinError(i) * factor);
  }
  return histogram;
}

TH1D* ACsoft::Utilities::McSpectrumScaler::CreateCorrectedHistogram(const TH1D* histogram, const QList<double>& scalingFactors) const {
  assert(histogram);
  const QString name = QString(histogram->GetName()) + "_corrected";
  TH1D* correctedHistogram = static_cast<TH1D*>(histogram->Clone(qPrintable(name)));
  ApplyScaling(correctedHistogram, scalingFactors);
  return correctedHistogram;
}

TCanvas* ACsoft::Utilities::McSpectrumScaler::ComparissonCanvas(McSpectrumScaler* scaler, std::string name) {
  if (name.empty())
    name = "McSpectrumScalerComparissonCanvas";
  TCanvas* canvas = new TCanvas(name.c_str(), name.c_str(), 800, 600);
  canvas->SetGrid();
  canvas->SetLogx();
  canvas->SetLogy();
  bool drawn = false;
  int colorIndex = 0;
  std::vector<TH1*> drawnHistograms;
  TLegend legend(0.674623, 0.75, 0.894472, 0.891608);
  legend.SetFillColor(0);
  const QMap<McParticleId::McSpecies, TH1D*>& triggeredHistogramMap = scaler->GetTriggeredHistograms();

  const int spectrumStyle = 1;
  const int triggeredHistogramStyle = 2;
  const int generatedHistogramStyle = 3;
  const int scaledTriggeredHistogramStyle = 20;
  const int scaledGeneratedHistogramStyle = 24;

  for (QMap<McParticleId::McSpecies, TH1D*>::const_iterator iter = triggeredHistogramMap.constBegin(); iter != triggeredHistogramMap.constEnd(); ++iter) {
    TH1D* triggeredHistogram = iter.value();
    const McParticleId::McSpecies mcSpecies = iter.key();
    const int color = (scaler->GetIsExternalHistogram()[mcSpecies]) ? triggeredHistogram->GetLineColor() : RootColor(colorIndex++);
    QString drawOption = "hist";
    if (!drawn)
      drawn = true;
    else
      drawOption += "same";
    TH1D scaledTriggeredHistogram(*triggeredHistogram);
    scaledTriggeredHistogram.SetYTitle("(GeV^{-1})");
    scaledTriggeredHistogram.Scale(1, "width");
    scaledTriggeredHistogram.SetStats(0);
    scaledTriggeredHistogram.SetTitle("");
    scaledTriggeredHistogram.SetLineColor(color);
    scaledTriggeredHistogram.SetMarkerColor(color);
    scaledTriggeredHistogram.SetLineWidth(2);
    scaledTriggeredHistogram.SetLineStyle(triggeredHistogramStyle);
    drawnHistograms.push_back(dynamic_cast<TH1*>(scaledTriggeredHistogram.DrawClone(qPrintable(drawOption))));

    const QString legendEntry = QString::fromStdString(McParticleId::Name(mcSpecies));
    legend.AddEntry(drawnHistograms.back(), qPrintable(legendEntry));

    TH1D scaledGeneratedHistogram(*scaler->GetGeneratedHistograms()[mcSpecies]);
    scaledGeneratedHistogram.Scale(1, "width");
    scaledGeneratedHistogram.SetStats(0);
    scaledGeneratedHistogram.SetTitle("");
    scaledGeneratedHistogram.SetLineColor(color);
    scaledGeneratedHistogram.SetMarkerColor(color);
    scaledGeneratedHistogram.SetLineWidth(2);
    scaledGeneratedHistogram.SetLineStyle(generatedHistogramStyle);
    drawnHistograms.push_back(dynamic_cast<TH1*>(scaledGeneratedHistogram.DrawClone(qPrintable(drawOption+"same"))));

    if (scaler->GetSpectra().contains(mcSpecies)) {
      if (scaler->GetSpectra()[mcSpecies]) {
        TAttLine* spectrum = dynamic_cast<TAttLine*>(scaler->GetSpectra()[mcSpecies]->DrawClone("same"));
        if (spectrum) {
          spectrum->SetLineColor(color);
          spectrum->SetLineStyle(spectrumStyle);

          TH1D correctedTriggeredHistogram(scaledTriggeredHistogram);
          correctedTriggeredHistogram.SetMarkerStyle(scaledTriggeredHistogramStyle);
          scaler->ApplyScaling(&correctedTriggeredHistogram, scaler->GetScalings()[mcSpecies]);
          drawnHistograms.push_back(dynamic_cast<TH1*>(correctedTriggeredHistogram.DrawClone("psame")));

          TH1D correctedGeneratedHistogram(scaledGeneratedHistogram);
          correctedGeneratedHistogram.SetMarkerStyle(scaledGeneratedHistogramStyle);
          scaler->ApplyScaling(&correctedGeneratedHistogram, scaler->GetScalings()[mcSpecies]);
          drawnHistograms.push_back(dynamic_cast<TH1*>(correctedGeneratedHistogram.DrawClone("psame")));
        }
      }
    }
  }
  SetMaximumMinimum(drawnHistograms);
  legend.DrawClone();
  TLegend legend2(0.674623, 0.907343, 0.894472, 0.994755);
  legend2.SetFillColor(0);
  const int overviewColor = kBlack;
  TLegendEntry triggerEntry;
  triggerEntry.SetLineColor(overviewColor);
  triggerEntry.SetLineStyle(triggeredHistogramStyle);
  legend2.AddEntry(&triggerEntry, "Raw triggered MC", "l");
  TLegendEntry generatedEntry;
  generatedEntry.SetLineColor(overviewColor);
  generatedEntry.SetLineStyle(generatedHistogramStyle);
  legend2.AddEntry(&generatedEntry, "Raw generated MC", "l");
  TLegendEntry spectrumEntry;
  spectrumEntry.SetLineColor(overviewColor);
  spectrumEntry.SetLineStyle(spectrumStyle);
  legend2.AddEntry(&spectrumEntry, "Preset spectrum", "l");
  TLegendEntry scaledTriggerEntry;
  scaledTriggerEntry.SetLineColor(overviewColor);
  scaledTriggerEntry.SetMarkerStyle(scaledTriggeredHistogramStyle);
  legend2.AddEntry(&scaledTriggerEntry, "Scaled triggered MC", "p");
  TLegendEntry scaledGeneratedEntry;
  scaledGeneratedEntry.SetLineColor(overviewColor);
  scaledGeneratedEntry.SetMarkerStyle(scaledGeneratedHistogramStyle);
  legend2.AddEntry(&scaledGeneratedEntry, "Scaled generated MC", "p");
  legend2.DrawClone();
  canvas->Modified();
  canvas->Update();
  return canvas;
}

TH1D* ACsoft::Utilities::McSpectrumScaler::ReplaceHistogramContent(const TH1D* origin, TH1D* destination) {
  // hope fully is has the same binning.
  assert(origin->GetNbinsX() == destination->GetNbinsX());
  assert(origin->GetBinLowEdge(1) == destination->GetBinLowEdge(1));
  assert(origin->GetBinLowEdge(origin->GetNbinsX()) == destination->GetBinLowEdge(destination->GetNbinsX()));
  for (int i = 0; i <= origin->GetNbinsX() + 1; ++i) {
    destination->SetBinContent(i, origin->GetBinContent(i));
    destination->SetBinError(i, origin->GetBinError(i));
  }
  return destination;
}

void ACsoft::Utilities::McSpectrumScaler::SaveHistograms(){
  assert(m_objectManager);
  m_objectManager->WriteToFile();
}
