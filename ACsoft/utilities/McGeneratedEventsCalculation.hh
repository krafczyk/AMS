#ifndef McGeneratedEventsCalculation_hh
#define McGeneratedEventsCalculation_hh

#include <QVector>
#include <QStringList>

#include <vector>
#include "McParticleId.hh"

class TH1D;

namespace ACsoft {

namespace AC {
  class Event;
}

namespace Analysis {
  class Particle;
}

namespace Utilities {

  class ObjectManager;

  class McGeneratedEventsCalculation {
  public:
    McGeneratedEventsCalculation();
    ~McGeneratedEventsCalculation();

    void Fill(const Analysis::Particle* particle);
    void FillLogHistos(Utilities::ObjectManager*);
    TH1D* FillGeneratedHistos(TH1D* histogram, McBinningMode binningMode, double mass) const;

  private:
    QVector<unsigned long long> m_numberOfGeneratedEvents;
    QVector<double> m_minimumGeneratedMomentums;
    QVector<double> m_maximumGeneratedMomentums;
    QVector<TH1D* > m_logHistograms;
    QStringList m_datasetIdentifiers;
    int m_currentRun;

    TH1D* WriteToHistogram(int identifierIndex) const;

  };
}

}

#endif
