#include "McGeneratedEventsCalculation.hh"

#include <TH1D.h>

#include "AnalysisParticle.hh"
#include "Binning.hh"
#include "Event.h"
#include "ObjectManager.hh"
#include "RunHeader.h"

#include <limits.h>
#include <assert.h>
#include <cmath>

#define DEBUG 0
#define INFO_OUT_TAG "McGeneratedEventsCalculation> "
#include <debugging.hh>

ACsoft::Utilities::McGeneratedEventsCalculation::McGeneratedEventsCalculation()
: m_currentRun(0)
{

}

ACsoft::Utilities::McGeneratedEventsCalculation::~McGeneratedEventsCalculation() {
  qDeleteAll(m_logHistograms);
}

static inline QString GenerateIdentifier(const ACsoft::Analysis::Particle* particle, double min, double max) {

  assert(particle);
  return QString("McParticle_%1_%2_%3").arg(particle->McParticleId()).arg(min).arg(max);
}

void ACsoft::Utilities::McGeneratedEventsCalculation::Fill(const ACsoft::Analysis::Particle* particle) {

  assert(particle);
  assert(particle->IsMC());
  double min = particle->McMinimumMomentum();
  double max = particle->McMaximumMomentum();
  const QString identifier = GenerateIdentifier(particle, min, max);
  if (!m_datasetIdentifiers.contains(identifier)) {
    m_datasetIdentifiers << identifier;
    m_numberOfGeneratedEvents << 0;
    m_minimumGeneratedMomentums << min;
    m_maximumGeneratedMomentums << max;

    if (DEBUG) std::cout << std::endl;
    DEBUG_OUT << "Data set: '" << identifier.toStdString() << "' minimum momentum: " << min << " maximum momentum: " << max << std::endl;
  }

  if (m_currentRun != particle->Run()){
    const int idIndex = m_datasetIdentifiers.indexOf(identifier);
    m_numberOfGeneratedEvents[idIndex] += particle->McNumberOfTriggeredEvents();
    if (m_currentRun > 0) {
      if (DEBUG) std::cout << std::endl;
      DEBUG_OUT << "Run " << particle->Run() << " from data set: '" << identifier.toStdString()
                << "' has " << particle->McNumberOfTriggeredEvents() << " generated events" << std::endl;
    }
    m_currentRun = particle->Run();
  }
}

void ACsoft::Utilities::McGeneratedEventsCalculation::FillLogHistos(ACsoft::Utilities::ObjectManager* objectManager)
{
  for(int i = 0; i< m_datasetIdentifiers.size(); ++i)
    m_logHistograms << ((objectManager) ? objectManager->Add(WriteToHistogram(i)) : WriteToHistogram(i));
}

TH1D* ACsoft::Utilities::McGeneratedEventsCalculation::WriteToHistogram(int identifierIndex) const {

  const QString& identifier = m_datasetIdentifiers[identifierIndex];
  const double min = m_minimumGeneratedMomentums.at(identifierIndex);
  const double max = m_maximumGeneratedMomentums.at(identifierIndex);
  const double norm = m_numberOfGeneratedEvents.at(identifierIndex);
  const int nBins = 1000;
  const std::vector<double> binning = ACsoft::Utilities::Binning::GenerateLogBinning(nBins, min, max);

  TH1D* logHisto = new TH1D(qPrintable(identifier), qPrintable(identifier), binning.size()-1, binning.data());

  const double value = norm / nBins;
  for( int i = 1; i <= logHisto->GetNbinsX(); i++)
    logHisto->AddBinContent(i, value);

  return logHisto;
}

static inline double ConvertToMomentum(double momentumOrEnergy, ACsoft::Utilities::McBinningMode binningMode, double mass) {

  switch (binningMode) {
  case ACsoft::Utilities::KineticEnergyBinning:
    // E_kin = sqrt(momentum * momentum + mass * mass) - mass
    // --> momentum = sqrt((E_kin + mass) * (E_kin + mass) - mass * mass)
    return sqrt((momentumOrEnergy + mass) * (momentumOrEnergy + mass) - mass * mass);
  case ACsoft::Utilities::MomentumBinning:
    return momentumOrEnergy;
  }

  assert(false);
  return 0;
}

TH1D* ACsoft::Utilities::McGeneratedEventsCalculation::FillGeneratedHistos(TH1D* histogram, McBinningMode binningMode, double mass) const
{
  assert(mass >= 0);
  assert(histogram);
  histogram->Reset();
  for(int i = 0; i < m_datasetIdentifiers.size(); ++i)
  {
    const TH1D* logHisto = m_logHistograms[i];
    assert(logHisto);
    for(int j = 1; j <= histogram->GetNbinsX(); ++j)
    {
      const double firstMomentum = ConvertToMomentum(histogram->GetBinLowEdge(j), binningMode, mass);
      const double lastMomentum = ConvertToMomentum(histogram->GetBinLowEdge(j + 1), binningMode, mass);
      const int firstBin = logHisto->FindFixBin(firstMomentum);
      const int lastBin = logHisto->FindFixBin(lastMomentum);
      double width = 0;
      double events = 0;
      for (int k = firstBin; k <= lastBin; ++k) {
        if (k == 0 || k > logHisto->GetNbinsX())
          continue;
        width += logHisto->GetBinWidth(k);
        events += logHisto->GetBinContent(k);
      }
      const double eventsPerGeV = (width > 0) ? events / width : 0;
      double binWidth = lastMomentum - firstMomentum;
      if( ConvertToMomentum(histogram->GetBinLowEdge(j), binningMode, mass) < m_minimumGeneratedMomentums[i] && ConvertToMomentum(histogram->GetBinLowEdge(j+1), binningMode, mass) > m_minimumGeneratedMomentums[i] )
        binWidth -=  m_minimumGeneratedMomentums[i] - ConvertToMomentum(histogram->GetBinLowEdge(j), binningMode, mass);
      if( ConvertToMomentum(histogram->GetBinLowEdge(j), binningMode, mass) < m_maximumGeneratedMomentums[i] && ConvertToMomentum(histogram->GetBinLowEdge(j+1), binningMode, mass) > m_maximumGeneratedMomentums[i] )
        binWidth -= ConvertToMomentum(histogram->GetBinLowEdge(j+1), binningMode, mass) - m_maximumGeneratedMomentums[i];

      histogram->AddBinContent(j, binWidth * eventsPerGeV);
    }
  }
  return histogram;
}
