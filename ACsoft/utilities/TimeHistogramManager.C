#include "TimeHistogramManager.hh"

#include <iomanip>
#include <assert.h>
#include <math.h>

#include <TList.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TMath.h>
#include <TClass.h>

#define DEBUG 0
#define INFO_OUT_TAG "TimeHistogramManager> "
#include <debugging.hh>

namespace ACsoft {

Utilities::TimeHistogramManager::TimeHistogramManager():
  fTimeOfFirstEvent(TTimeStamp(0,0)),
  fTimeOfLastEvent(TTimeStamp(0,0)),
  fIsValid(false),
  fBinWidthSeconds(1200)
{
}


/** Generate a suitable time binning.
  *
  * This only works after a call to SetFirstAndLastEventTimes().
  *
  * \param[out] nbins Number of bins to be used for time histogram.
  * \param[out] xmin Left edge of first bin to be used for time histogram.
  * \param[out] xmax Right edge of last bin to be used for time histogram.
  *
  * \returns \c False if an error occurred.
  */
bool Utilities::TimeHistogramManager::GetTimeBinning( int& nbins, double& xmin, double& xmax ) const {

  if( !fIsValid ){
    WARN_OUT << "ERROR: Times of first and last event have not been set!" << std::endl;
    nbins = 0;
    xmin = xmax = 0.;
    return false;
  }

  xmin = fTimeOfFirstEvent.GetSec() - ( fTimeOfFirstEvent.GetSec() % fBinWidthSeconds );
  xmax = fTimeOfLastEvent.GetSec() + ( fBinWidthSeconds - ( fTimeOfLastEvent.GetSec() % fBinWidthSeconds ) );

  nbins = time_t(xmax-xmin)/fBinWidthSeconds;

  DEBUG_OUT << " first: " << fTimeOfFirstEvent.AsString() << "   (" << fTimeOfFirstEvent.GetSec() << " sec)" << std::endl;
  DEBUG_OUT << " last:  " << fTimeOfLastEvent.AsString() << "   (" << fTimeOfLastEvent.GetSec() << " sec)" << std::endl;
  DEBUG_OUT << " ==>    " << nbins << " bins from " << std::setprecision(10) << xmin << " to " << std::setprecision(10) << xmax << std::endl;
  DEBUG_OUT << "        xmin " << TTimeStamp(time_t(xmin),0).AsString() << std::endl;
  DEBUG_OUT << "        xmax " << TTimeStamp(time_t(xmax),0).AsString() << std::endl;
  return true;
}



/** Create a new histogram with fixed time binning on the x-axis that combines the input 1D time histograms.
  *
  * This function is called by the \c ac_merge tool.
  */
TH1* Utilities::TimeHistogramManager::Merge1DTimeHistograms( std::vector<TH1*>& histos ) {

  if( histos.empty() ) return 0;

  TH1* firstHisto = histos.front();
  assert( firstHisto );

  INFO_OUT << "Now merging " << histos.size() << " 1D histograms \"" << firstHisto->GetName() << "\"..." << std::endl;

  double binwidth  = firstHisto->GetXaxis()->GetBinWidth(1);
  double firstTime = firstHisto->GetXaxis()->GetXmin();
  double lastTime  = firstHisto->GetXaxis()->GetXmax();

  for( unsigned int i=0 ; i<histos.size() ; ++i ){

    TH1* histo = histos[i];

    if (fabs(firstHisto->GetXaxis()->GetBinWidth(1) - histo->GetXaxis()->GetBinWidth(1)) >= 1e-4 ) return 0;
    assert( std::string(firstHisto->IsA()->GetName()) == std::string(histo->IsA()->GetName()) );

    DEBUG_OUT << "Found histo: " << histo->GetNbinsX() << " bins, width " << histo->GetXaxis()->GetBinWidth(1) << " from " << histo->GetXaxis()->GetXmin() << " to " << histo->GetXaxis()->GetXmax() << std::endl;

    if( histo->GetXaxis()->GetXmin() < firstTime )
      firstTime = histo->GetXaxis()->GetXmin();

    if( histo->GetXaxis()->GetXmax() > lastTime )
      lastTime = histo->GetXaxis()->GetXmax();
  }

  int nbinsX = TMath::Nint( ( lastTime - firstTime ) / binwidth );

  DEBUG_OUT << "Created new binning: " << nbinsX << " bins from " << std::setprecision(10) << firstTime << " to " << std::setprecision(10) << lastTime << std::endl;
  DEBUG_OUT << "                xmin " << TTimeStamp(time_t(firstTime),0).AsString() << std::endl;
  DEBUG_OUT << "                xmax " << TTimeStamp(time_t(lastTime),0).AsString() << std::endl;

  TH1* mergedHisto = 0;

  if( firstHisto->IsA()->GetName() == std::string("TH1I") ){
    mergedHisto = new TH1I( firstHisto->GetName(), firstHisto->GetTitle(), nbinsX, firstTime, lastTime);
  }
  else if( firstHisto->IsA()->GetName() == std::string("TH1F") ){
    mergedHisto = new TH1F( firstHisto->GetName(), firstHisto->GetTitle(), nbinsX, firstTime, lastTime);
  }
  else if( firstHisto->IsA()->GetName() == std::string("TH1D") ){
    mergedHisto = new TH1D( firstHisto->GetName(), firstHisto->GetTitle(), nbinsX, firstTime, lastTime);
  }
  else if( firstHisto->IsA()->GetName() == std::string("TH1C") ){
    mergedHisto = new TH1C( firstHisto->GetName(), firstHisto->GetTitle(), nbinsX, firstTime, lastTime);
  }
  else if( firstHisto->IsA()->GetName() == std::string("TH1S") ){
    mergedHisto = new TH1S( firstHisto->GetName(), firstHisto->GetTitle(), nbinsX, firstTime, lastTime);
  }
  else{
    WARN_OUT << "Function not implemented for histogram of type " << firstHisto->IsA()->GetName() << std::endl;
    return 0;
  }

  mergedHisto->GetXaxis()->SetTitle("time");
  mergedHisto->GetXaxis()->SetTimeDisplay(1);
  mergedHisto->GetXaxis()->SetTimeFormat("%y-%m-%d %H:%M:%S%F1970-01-01 00:00:00");

  TList* list = new TList;
 
  // copy contents
  for( unsigned int histoCtr=0 ; histoCtr<histos.size() ; ++histoCtr ){

    TH1* histo = histos[histoCtr];
    DEBUG_OUT << "Merging histo number " << histoCtr << " ..." << std::endl;
    list->Add(histo);
  }

  mergedHisto->Merge(list);
  return mergedHisto;
}

/** Create a new histogram with fixed time binning on the x-axis that combines the input 2D time histograms.
  *
  * This function is called by the \c ac_merge tool.
  */
TH2* Utilities::TimeHistogramManager::Merge2DTimeHistograms( std::vector<TH2*>& histos ) {

  if( histos.empty() ) return 0;

  TH2* firstHisto = histos.front();
  assert( firstHisto );

  INFO_OUT << "Now merging " << histos.size() << " 2D histograms \"" << firstHisto->GetName() << "\"..." << std::endl;

  double binwidth  = firstHisto->GetXaxis()->GetBinWidth(1);
  double firstTime = firstHisto->GetXaxis()->GetXmin();
  double lastTime  = firstHisto->GetXaxis()->GetXmax();

  for( unsigned int i=0 ; i<histos.size() ; ++i ){

    TH2* histo = histos[i];

    DEBUG_OUT << "Histo " << i << ", name \"" << histo->GetName() << "\": " << histo->GetNbinsX() << " bins, width " << histo->GetXaxis()->GetBinWidth(1)
          << " from " << histo->GetXaxis()->GetXmin() << " to " << histo->GetXaxis()->GetXmax() << std::endl;
 
    assert( fabs(firstHisto->GetXaxis()->GetBinWidth(1) - histo->GetXaxis()->GetBinWidth(1)) < 1e-4 );
    assert( firstHisto->GetNbinsY() == histo->GetNbinsY() );
    assert( fabs( firstHisto->GetYaxis()->GetXmin() - histo->GetYaxis()->GetXmin() ) < 1e-4 );
    assert( fabs( firstHisto->GetYaxis()->GetXmax() - histo->GetYaxis()->GetXmax() ) < 1e-4 );
    assert( std::string(firstHisto->IsA()->GetName()) == std::string(histo->IsA()->GetName()) );


    if( histo->GetXaxis()->GetXmin() < firstTime )
      firstTime = histo->GetXaxis()->GetXmin();

    if( histo->GetXaxis()->GetXmax() > lastTime )
      lastTime = histo->GetXaxis()->GetXmax();
  }

  int nbinsX = TMath::Nint( ( lastTime - firstTime ) / binwidth );

  // calculate bin array, we need this because of the limited choice in TH* constructors
  std::vector<double> xbins(nbinsX+1);
  for( int i=0 ; i<=nbinsX ; ++i )
      xbins[i] = firstTime + i*binwidth;

  DEBUG_OUT << "Created new binning: " << nbinsX << " bins from " << std::setprecision(10) << firstTime << " to " << std::setprecision(10) << lastTime << std::endl;
  DEBUG_OUT << "                xmin " << TTimeStamp(time_t(firstTime),0).AsString() << std::endl;
  DEBUG_OUT << "                xmax " << TTimeStamp(time_t(lastTime),0).AsString() << std::endl;

  TH2* mergedHisto = 0;

  DEBUG_OUT_L(2) << "XBINS " << firstHisto->GetYaxis()->GetXbins() << std::endl;
  DEBUG_OUT_L(2) << "SIZE  " << firstHisto->GetYaxis()->GetXbins()->GetSize() << std::endl;
  DEBUG_OUT_L(2) << "ARRAY " << firstHisto->GetYaxis()->GetXbins()->GetArray() << std::endl;

  // it turns out that TH2 is a nasty bitch: depending on which constructor had been used to build it,
  // the internal state of the axes and the way their binning information is stored differs!
  // for equidistant binning, the TArrayD of bin edges is not filled.

  if( firstHisto->IsA()->GetName() == std::string("TH2I") ){
    if( firstHisto->GetYaxis()->GetXbins()->GetArray() ){
      mergedHisto = new TH2I( firstHisto->GetName(), firstHisto->GetTitle(),
                              nbinsX, &(xbins[0]),
                              firstHisto->GetNbinsY(), firstHisto->GetYaxis()->GetXbins()->GetArray() );
    }
    else{
      mergedHisto = new TH2I( firstHisto->GetName(), firstHisto->GetTitle(),
                              nbinsX, firstTime, lastTime,
                              firstHisto->GetNbinsY(), firstHisto->GetYaxis()->GetXmin(), firstHisto->GetYaxis()->GetXmax() );
    }
  }
  else if( firstHisto->IsA()->GetName() == std::string("TH2F") ){
    if( firstHisto->GetYaxis()->GetXbins()->GetArray() ){
      mergedHisto = new TH2F( firstHisto->GetName(), firstHisto->GetTitle(),
                              nbinsX, &(xbins[0]),
                              firstHisto->GetNbinsY(), firstHisto->GetYaxis()->GetXbins()->GetArray() );
    }
    else{
      mergedHisto = new TH2F( firstHisto->GetName(), firstHisto->GetTitle(),
                              nbinsX, firstTime, lastTime,
                              firstHisto->GetNbinsY(), firstHisto->GetYaxis()->GetXmin(), firstHisto->GetYaxis()->GetXmax() );
    }
  }
  else if( firstHisto->IsA()->GetName() == std::string("TH2D") ){
    if( firstHisto->GetYaxis()->GetXbins()->GetArray() ){
      mergedHisto = new TH2D( firstHisto->GetName(), firstHisto->GetTitle(),
                              nbinsX, &(xbins[0]),
                              firstHisto->GetNbinsY(), firstHisto->GetYaxis()->GetXbins()->GetArray() );
    }
    else{
      mergedHisto = new TH2D( firstHisto->GetName(), firstHisto->GetTitle(),
                              nbinsX, firstTime, lastTime,
                              firstHisto->GetNbinsY(), firstHisto->GetYaxis()->GetXmin(), firstHisto->GetYaxis()->GetXmax() );
    }
  }
  else if( firstHisto->IsA()->GetName() == std::string("TH2C") ){
    if( firstHisto->GetYaxis()->GetXbins()->GetArray() ){
      mergedHisto = new TH2C( firstHisto->GetName(), firstHisto->GetTitle(),
                              nbinsX, &(xbins[0]),
                              firstHisto->GetNbinsY(), firstHisto->GetYaxis()->GetXbins()->GetArray() );
    }
    else{
      mergedHisto = new TH2C( firstHisto->GetName(), firstHisto->GetTitle(),
                              nbinsX, firstTime, lastTime,
                              firstHisto->GetNbinsY(), firstHisto->GetYaxis()->GetXmin(), firstHisto->GetYaxis()->GetXmax() );
    }
  }
  else if( firstHisto->IsA()->GetName() == std::string("TH2S") ){
    if( firstHisto->GetYaxis()->GetXbins()->GetArray() ){
      mergedHisto = new TH2S( firstHisto->GetName(), firstHisto->GetTitle(),
                              nbinsX, &(xbins[0]),
                              firstHisto->GetNbinsY(), firstHisto->GetYaxis()->GetXbins()->GetArray() );
    }
    else{
      mergedHisto = new TH2S( firstHisto->GetName(), firstHisto->GetTitle(),
                              nbinsX, firstTime, lastTime,
                              firstHisto->GetNbinsY(), firstHisto->GetYaxis()->GetXmin(), firstHisto->GetYaxis()->GetXmax() );
    }
  }
  else{
    WARN_OUT << "Function not implemented for histogram of type " << firstHisto->IsA()->GetName() << std::endl;
    return 0;
  }

  mergedHisto->GetXaxis()->SetTitle("time");
  mergedHisto->GetYaxis()->SetTitle(firstHisto->GetYaxis()->GetTitle());
  mergedHisto->GetXaxis()->SetTimeDisplay(1);
  mergedHisto->GetXaxis()->SetTimeFormat("%y-%m-%d %H:%M:%S%F1970-01-01 00:00:00");

  // copy contents
  for( unsigned int histoCtr=0 ; histoCtr<histos.size() ; ++histoCtr ){

    TH2* histo = histos[histoCtr];
    DEBUG_OUT << "Merging histo number " << histoCtr << " ..." << std::endl;

    for( int i=1 ; i<=histo->GetNbinsX() ; ++i ){
      for( int j=1 ; j<=histo->GetNbinsY() ; ++j ){

        mergedHisto->Fill(histo->GetXaxis()->GetBinCenter(i),histo->GetYaxis()->GetBinCenter(j),histo->GetBinContent(i,j));
      }
    }
  }

  return mergedHisto;
}


/** Create a new 3D-histogram with fixed time binning on the x-axis that combines input 3D time histograms.
  *
  * This function is called by the \c ac_merge tool.
  */
TH3* Utilities::TimeHistogramManager::Merge3DTimeHistograms( std::vector<TH3*>& histos ) {

  if( histos.empty() ) return 0;

  TH3* firstHisto = histos.front();
  assert( firstHisto );

  INFO_OUT << "Now merging " << histos.size() << " 3D histograms \"" << firstHisto->GetName() << "\"..." << std::endl;

  double binwidth  = firstHisto->GetXaxis()->GetBinWidth(1);
  double firstTime = firstHisto->GetXaxis()->GetXmin();
  double lastTime  = firstHisto->GetXaxis()->GetXmax();

  for( unsigned int i=0 ; i<histos.size() ; ++i ){

    TH3* histo = histos[i];

    assert( fabs(firstHisto->GetXaxis()->GetBinWidth(1) - histo->GetXaxis()->GetBinWidth(1)) < 1e-4 );
    assert( firstHisto->GetNbinsY() == histo->GetNbinsY() );
    assert( firstHisto->GetNbinsZ() == histo->GetNbinsZ() );
    assert( fabs( firstHisto->GetYaxis()->GetXmin() - histo->GetYaxis()->GetXmin() ) < 1e-4 );
    assert( fabs( firstHisto->GetZaxis()->GetXmin() - histo->GetZaxis()->GetXmin() ) < 1e-4 );
    assert( fabs( firstHisto->GetYaxis()->GetXmax() - histo->GetYaxis()->GetXmax() ) < 1e-4 );
    assert( fabs( firstHisto->GetZaxis()->GetXmax() - histo->GetZaxis()->GetXmax() ) < 1e-4 );
    assert( std::string(firstHisto->IsA()->GetName()) == std::string(histo->IsA()->GetName()) );

    DEBUG_OUT << "Found histo: " << histo->GetNbinsX() << " bins, width " << histo->GetXaxis()->GetBinWidth(1) << " from " << histo->GetXaxis()->GetXmin() << " to " << histo->GetXaxis()->GetXmax() << std::endl;

    if( histo->GetXaxis()->GetXmin() < firstTime )
      firstTime = histo->GetXaxis()->GetXmin();

    if( histo->GetXaxis()->GetXmax() > lastTime )
      lastTime = histo->GetXaxis()->GetXmax();
  }

  int nbinsX = TMath::Nint( ( lastTime - firstTime ) / binwidth );


  // calculate bin array, we need this because of the limited choice in TH* constructors
  std::vector<double> xbins(nbinsX+1);
  for( int i=0 ; i<=nbinsX ; ++i )
      xbins[i] = firstTime + i*binwidth;

  DEBUG_OUT << "Created new binning: " << nbinsX << " bins from " << std::setprecision(10) << firstTime << " to " << std::setprecision(10) << lastTime << std::endl;
  DEBUG_OUT << "                xmin " << TTimeStamp(time_t(firstTime),0).AsString() << std::endl;
  DEBUG_OUT << "                xmax " << TTimeStamp(time_t(lastTime),0).AsString() << std::endl;

  TH3* mergedHisto = 0;

  if( !(firstHisto->GetYaxis()->GetXbins()->GetArray()) )
    WARN_OUT << "Y-axis array for " << firstHisto->IsA()->GetName() << " \"" << firstHisto->GetName() << "\" not set!" << std::endl;
  if( !(firstHisto->GetZaxis()->GetXbins()->GetArray()) )
    WARN_OUT << "Z-axis array for " << firstHisto->IsA()->GetName() << " \"" << firstHisto->GetName() << "\" not set!" << std::endl;


  if( firstHisto->IsA()->GetName() == std::string("TH3I") ){
      mergedHisto = new TH3I( firstHisto->GetName(), firstHisto->GetTitle(), nbinsX, &(xbins[0]), firstHisto->GetNbinsY(), firstHisto->GetYaxis()->GetXbins()->GetArray(),
                              firstHisto->GetNbinsZ(), firstHisto->GetZaxis()->GetXbins()->GetArray() );
  }
  else if( firstHisto->IsA()->GetName() == std::string("TH3F") ){
      mergedHisto = new TH3F( firstHisto->GetName(), firstHisto->GetTitle(), nbinsX, &(xbins[0]), firstHisto->GetNbinsY(), firstHisto->GetYaxis()->GetXbins()->GetArray(),
                              firstHisto->GetNbinsZ(), firstHisto->GetZaxis()->GetXbins()->GetArray() );
  }
  else if( firstHisto->IsA()->GetName() == std::string("TH3D") ){
      mergedHisto = new TH3D( firstHisto->GetName(), firstHisto->GetTitle(), nbinsX, &(xbins[0]), firstHisto->GetNbinsY(), firstHisto->GetYaxis()->GetXbins()->GetArray(),
                              firstHisto->GetNbinsZ(), firstHisto->GetZaxis()->GetXbins()->GetArray() );
  }
  else if( firstHisto->IsA()->GetName() == std::string("TH3C") ){
      mergedHisto = new TH3C( firstHisto->GetName(), firstHisto->GetTitle(), nbinsX, &(xbins[0]), firstHisto->GetNbinsY(), firstHisto->GetYaxis()->GetXbins()->GetArray(),
                              firstHisto->GetNbinsZ(), firstHisto->GetZaxis()->GetXbins()->GetArray() );
  }
  else if( firstHisto->IsA()->GetName() == std::string("TH3S") ){
      mergedHisto = new TH3S( firstHisto->GetName(), firstHisto->GetTitle(), nbinsX, &(xbins[0]), firstHisto->GetNbinsY(), firstHisto->GetYaxis()->GetXbins()->GetArray(),
                              firstHisto->GetNbinsZ(), firstHisto->GetZaxis()->GetXbins()->GetArray() );
  }
  else{
    WARN_OUT << "Function not implemented for histogram of type " << firstHisto->IsA()->GetName() << std::endl;
    return 0;
  }

  mergedHisto->GetXaxis()->SetTitle("time");
  mergedHisto->GetYaxis()->SetTitle(firstHisto->GetYaxis()->GetTitle());
  mergedHisto->GetXaxis()->SetTimeDisplay(1);
  mergedHisto->GetXaxis()->SetTimeFormat("%y-%m-%d %H:%M:%S%F1970-01-01 00:00:00");

  // copy contents
  for( unsigned int histoCtr=0 ; histoCtr<histos.size() ; ++histoCtr ){

    TH3* histo = histos[histoCtr];
    DEBUG_OUT << "Merging histo number " << histoCtr << " ..." << std::endl;

    for( int i=1 ; i<=histo->GetNbinsX() ; ++i ){
      for( int j=1 ; j<=histo->GetNbinsY() ; ++j ){
        for( int k=1 ; k<=histo->GetNbinsZ() ; ++k ){

            double bincontent = histo->GetBinContent(i,j,k);
            if( bincontent != 0.0 )
                mergedHisto->Fill(histo->GetXaxis()->GetBinCenter(i),histo->GetYaxis()->GetBinCenter(j),histo->GetZaxis()->GetBinCenter(k),bincontent);
        }
      }
    }
  }

  return mergedHisto;
}






/** Set time range.
  *
  * This function is supposed to be called by the FileManager before any time histograms are created.
  * The time range for the current analysis will then be fixed and a time binning can be calculated.
  *
  */
void Utilities::TimeHistogramManager::SetFirstAndLastEventTimes( TTimeStamp first, TTimeStamp last ) {

  if(fIsValid)
    WARN_OUT << "Overriding existing timestamps!" << std::endl;

  fTimeOfFirstEvent = first;
  fTimeOfLastEvent = last;

  fIsValid = true;
}

/** Reset TimeHistogramManager to initial state.
  */
void Utilities::TimeHistogramManager::Reset() {

  fTimeOfFirstEvent = TTimeStamp(0, 0);
  fTimeOfLastEvent = TTimeStamp(0, 0);
  fIsValid = false;
}

const TTimeStamp& Utilities::TimeHistogramManager::GetTimeOfFirstEvent() const {

  if( !fIsValid )
    WARN_OUT << "ERROR: Time of first event has not been set!" << std::endl;

  return fTimeOfFirstEvent;
}

const TTimeStamp& Utilities::TimeHistogramManager::GetTimeOfLastEvent() const {

  if( !fIsValid )
    WARN_OUT << "ERROR: Time of last event has not been set!" << std::endl;

  return fTimeOfLastEvent;
}

}
