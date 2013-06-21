
#include "Binning.hh"

#include "ResolutionModels.hh"

#include <assert.h>
#include <cmath>
#include <vector>

#include <TH1D.h>
#include <TAxis.h>

#include <QVector>
#include <QMap>
#include <QString>

std::vector<double> ACsoft::Utilities::Binning::PamelaBinning() {
  QVector<double> binning;
  binning << 0.5 << 1.0 << 1.5 << 1.8 << 2.2 << 2.7 << 3.3 << 4.1 << 5.0 << 6.1 << 7.4 << 9.1 << 11.2 << 15.0 << 20.0 << 28.0 << 42.0 << 65.0 << 100.0;
  return binning.toStdVector();
}

int ACsoft::Utilities::Binning::nBinsPamela() {
  return PamelaBinning().size() - 1;
};

std::vector<double> ACsoft::Utilities::Binning::EcalBinning() {
  QVector<double> binning;
  binning << 0.86 << 1.35 << 1.95 << 2.69 << 3.57 << 4.60 << 5.78 << 7.14 << 8.68 << 10.41 << 12.36 << 14.54 << 16.97 << 19.67 << 22.67 << 25.99 << 29.66 << 33.73 << 38.22 << 43.17 << 48.64 << 54.66 << 61.30 << 68.62 << 76.68 << 85.55 << 96.59 << 110.83 << 130.07 << 157.84 << 202 << 270 << 370 << 530 << 750 << 1100;
  return binning.toStdVector();
}

int ACsoft::Utilities::Binning::nBinsEcal() {
  return EcalBinning().size() - 1;
}

std::vector<double> ACsoft::Utilities::Binning::AlphaBinning() {
  QVector<double> binning;
  binning << 0.75 << 1.00 << 1.39 << 1.86 << 2.41 << 3.04 << 3.75 << 4.55 << 5.43 << 6.42 << 7.50 << 8.69 << 10.00 << 12.01 << 14.26 << 16.78 << 19.59 << 22.71 << 26.18 << 31.00 << 38.36 << 47.03 << 57.22 << 69.18 << 83.20 << 100.00 << 127.90 << 162.60 << 206.00 << 260.00 << 350.0 << 500 << 750 << 1000;
  return binning.toStdVector();
}

std::vector<double> ACsoft::Utilities::Binning::AmsPositronFractionPaper2013Binning() {
  QVector<double> binning;
  binning << 0.50 << 0.65 << 0.81 << 1.00 << 1.21 << 1.45 << 1.70 << 1.97 << 2.28 << 2.60 <<
             2.94 << 3.30 << 3.70 << 4.11 << 4.54 << 5.00 << 5.50 << 6.00 << 6.56 << 7.16 <<
             7.80 << 8.50 << 9.21 << 9.95 << 10.73 << 11.54 << 12.39 << 13.27 << 14.19 << 15.15 <<
            16.15 << 17.18 << 18.25 << 19.37 << 20.54 << 21.76 << 23.07 << 24.45 << 25.87 <<
            27.34 << 28.87 << 30.45 << 32.10 << 33.80 << 35.57 << 37.40 << 40.00 << 43.39 << 47.01 <<
            50.87 << 54.98 << 59.36 << 64.03 << 69.00 << 74.30 << 80.00 << 86.00 << 92.50 << 100.00 <<
           115.10 << 132.10 << 151.50 << 173.50 << 206.00 << 260.00<< 350.00 <<
           500.0  << 750.0  << 1000.0 << 1500.0;
  return binning.toStdVector();
}

int ACsoft::Utilities::Binning::nBinsAmsPositronFractionPaper2013Binning() {
  return AmsPositronFractionPaper2013Binning().size() - 1;
}

int ACsoft::Utilities::Binning::nBinsAlpha() {
  return AlphaBinning().size() - 1;
}

std::vector<double> ACsoft::Utilities::Binning::GenerateLogBinning(int nBinLog, double Tmin, double Tmax) {

  std::vector<double> xMin;
  xMin.push_back(Tmin);
  for (int i=0; i<nBinLog; i++) {
    double xMax = exp(log(xMin[i])+(log(Tmax)-log(Tmin))/nBinLog);
    xMin.push_back(xMax);
  }
  return xMin;
}

void ACsoft::Utilities::Binning::CollectEquidistantBinning(std::vector<double>& vec, int nbins, double fromValue, double toValue) {

  assert(nbins>0);
  assert(toValue>fromValue);

  vec.clear();
  vec.assign(nbins+1, 0.);

  const double binwidth = (toValue-fromValue)/nbins;

  for( int i=0 ; i<=nbins ; ++i )
    vec[i] = fromValue + i*binwidth;
}

std::vector<double> ACsoft::Utilities::Binning::EnergyResolutionBinning(const double minEnergy, const double maxEnergy, const double factor) {

  std::vector<double> binning;
  binning.push_back(minEnergy);

  double energy = minEnergy;
  while (true) {
    double energyDiff = energy + factor * ResolutionModels::EcalSigmaEperE(energy) * energy;
    double energyProduct = sqrt(energy * energyDiff);
    energy = energyProduct + factor * ResolutionModels::EcalSigmaEperE(energyProduct) * energyProduct;
    if (energy > maxEnergy)
      break;
    binning.push_back(energy);
  }

  return binning;
}

std::vector<double> ACsoft::Utilities::Binning::SagittaResolutionBinning(const double minRigidity, const double maxRigidity, const double mdr, const double factor) {

  const double mass = 1; //set proton mass
  QVector<double> binning;
  double xi = 1. / minRigidity;
  binning.prepend(xi);
  while (xi > 1. / maxRigidity) {
    double xiDiff = xi - factor * ResolutionModels::TrackerSigmaSperS(xi, mdr, mass) * xi;
    double xiProduct = sqrt(xi * xiDiff);
    double xiFinal = xiProduct - factor * ResolutionModels::TrackerSigmaSperS(xiProduct, mdr, mass) * xiProduct;
    xi = qMin(xiDiff, xiFinal);
    if (xi > 0)
      binning.prepend(xi);
  }
  return binning.toStdVector();
}

std::vector<double> ACsoft::Utilities::Binning::SagittaResolutionBinningFullspan(const double minRigidity, const double maxRigidity, const double factor) {

  const double mdr = 2000; // fullspann tracker
  return SagittaResolutionBinning(minRigidity, maxRigidity, mdr, factor);
}

TH1D* ACsoft::Utilities::Binning::ConvertToRigidity(const TH1D* sagittaHistogram) {

  const QVector<double>& sagittaAxis = QVector<double>::fromStdVector(GetAxis(sagittaHistogram->GetXaxis()));
  QVector<double> inverseAxisNegative;
  QVector<double> inverseAxisPositive;
  QVector<int> oldBinsNegative;
  QVector<int> oldBinsPositive;
  // it is assumed that the axis is sorted in increasing order
  for (int i = 0; i < sagittaAxis.size(); ++i) {
    const double sagittaValue = sagittaAxis.at(i);
    const bool isNegative = (sagittaValue < 0) ? true : false;
    const bool isZero = qFuzzyCompare(sagittaValue + 1, 1);
    const double rigidityValue = (isZero) ? NAN: 1. / sagittaValue;
    if (isZero) {
      inverseAxisNegative.append(0.);
      oldBinsNegative.prepend(i);
    } else if (isNegative) {
      inverseAxisNegative.prepend(rigidityValue);
      oldBinsNegative.prepend(i);
    } else {
      inverseAxisPositive.prepend(rigidityValue);
      oldBinsPositive.prepend(i);
    }
  }
  oldBinsPositive.prepend(sagittaAxis.size());
  const QVector<double> inverseAxis = inverseAxisNegative + inverseAxisPositive;
  const QVector<int> oldBins = oldBinsNegative + oldBinsPositive;

  const QString name = QString(sagittaHistogram->GetName()) + "_inverseAxis";
  const QString title = sagittaHistogram->GetTitle();
  TH1D* newHistogram = new TH1D(qPrintable(name), qPrintable(title), inverseAxis.size() - 1, inverseAxis.constData());
  newHistogram->SetMarkerStyle(sagittaHistogram->GetMarkerStyle());
  newHistogram->SetMarkerColor(sagittaHistogram->GetMarkerColor());
  newHistogram->SetMarkerSize(sagittaHistogram->GetMarkerSize());
  newHistogram->SetLineColor(sagittaHistogram->GetLineColor());
  newHistogram->SetLineStyle(sagittaHistogram->GetLineStyle());
  newHistogram->SetLineWidth(sagittaHistogram->GetLineWidth());

  for (int i = 0; i <= newHistogram->GetNbinsX() + 1; ++i) {
    const int oldBin = oldBins.at(i);
    const double content = sagittaHistogram->GetBinContent(oldBin);
    const double error = sagittaHistogram->GetBinError(oldBin);
    newHistogram->SetBinContent(i, content);
    newHistogram->SetBinError(i, error);
  }
  return newHistogram;
}

std::vector<double>  ACsoft::Utilities::Binning::GetAxis(const TAxis* axis) {

  std::vector<double> newAxis;
  const int nBins = axis->GetNbins();
  for (int i = 0; i < nBins + 1; ++i) {
    newAxis.push_back(axis->GetBinLowEdge(i+1));
  }
  return newAxis;
}

std::vector<double>  ACsoft::Utilities::Binning::GetPositiveBinning(const TAxis* axis) {

  std::vector<double> newAxis;
  const int nBins = axis->GetNbins();
  for (int i = 0; i < nBins + 1; ++i) {
    const double lowEdge = axis->GetBinLowEdge(i+1);
    if (lowEdge >= 0)
      newAxis.push_back(lowEdge);
  }
  return newAxis;
}

std::vector<double> ACsoft::Utilities::Binning::MirrorSign(const std::vector<double>& binning) {

  QVector<double> newBinning;
  for (unsigned int i = 0; i < binning.size(); ++i) {
    newBinning.prepend(binning.at(i) * -1.);
  }
  return newBinning.toStdVector();
}

std::vector<double> ACsoft::Utilities::Binning::Inverse(const std::vector<double>& binning) {

  QVector<double> newBinning;
  for (unsigned int i = 0; i < binning.size(); ++i) {
    const double value = binning.at(i);
    newBinning.prepend((value != 0) ? 1./value : 0);
  }
  qSort(newBinning);
  return newBinning.toStdVector();
}

std::vector<double> ACsoft::Utilities::Binning::CompleteWithNegative(const std::vector<double>& positiveBinning) {
  QVector<double> binning = QVector<double>::fromStdVector(MirrorSign(positiveBinning));
  if (!binning.contains(0.))
    binning += QVector<double>() << 0.;
  else
    binning.remove(binning.size()-1);
  binning += QVector<double>::fromStdVector(positiveBinning);
  return binning.toStdVector();
}

