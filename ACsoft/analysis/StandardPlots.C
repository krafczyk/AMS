#include "StandardPlots.hh"

#include <TList.h>
#include <TH1.h>
#include <TFile.h>
#include <TCanvas.h>

#include "Event.h"

#include <ConfigHandler.hh>
#include <FileManager.hh>

#define DEBUG 0
#define INFO_OUT_TAG "StandardPlots> "
#include <debugging.hh>

Analysis::StandardPlots::StandardPlots( Utilities::ConfigHandler* cfghandler,
                                        std::string resultDirectory,
                                        std::string resultFileSuffix ) :
  fResultDirectory(resultDirectory),
  fResultFileSuffix(resultFileSuffix)
{

  // read options from cfghandler here and save them in the class
  fOutputFilePrefix = "standardplots";
  if(cfghandler)
    cfghandler->GetValue( "StandardPlots", "OutputFilePrefix", fOutputFilePrefix,
                          "Prefix for output file. Directory and suffix are set via global option." );

  fHistogramList = new TList();
  CreateHistograms();
}

Analysis::StandardPlots::~StandardPlots() {

}

/** Create new histograms here.
  *
  */
void Analysis::StandardPlots::CreateHistograms() {


  // store pointers in the list of histograms, to simplify operations on all histograms of the class


  return;
}


/** Fill histogram from the current event.
  *
  * \param event pointer to the current event
  */

void Analysis::StandardPlots::FillHistosFrom( const Analysis::Particle& ) {

  return;
}



/** Create canvases and draw histograms.
  *
  */
void Analysis::StandardPlots::DrawHistos() {

  INFO_OUT << "Drawing histograms..." << std::endl;



}

/** Writes all histograms to the currently opened ROOT file.
  *
  */
void Analysis::StandardPlots::WriteHistos() {

  INFO_OUT << "Write Histograms to current root file..." << std::endl;

  for( int i=0 ; i<fHistogramList->GetSize() ; ++i ) {
    fHistogramList->At(i)->Write();
  }
}

/** Creates a new ROOT file and writes all histograms to the file.
  *
  * The filename is set via the configuration file.
  */
void Analysis::StandardPlots::WriteHistosToNewFile() {

  std::string outputFileName = Analysis::FileManager::MakeStandardRootFileName(fResultDirectory,fOutputFilePrefix,fResultFileSuffix);

  TFile* f = new TFile( outputFileName.c_str(), "RECREATE");
  INFO_OUT << "Set current root file to " << f->GetName() << std::endl;
  if( !f->IsOpen() ){
    WARN_OUT << "ERROR opening output file \"" << outputFileName << "\"!" << std::endl;
    throw std::runtime_error("ERROR opening output file");
  }

  f->cd();

  WriteHistos();

  f->Close();
}
