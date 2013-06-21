#include "ObjectManager.hh"

#include "ConfigHandler.hh"

#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TFile.h>
#include <TF1.h>
#include <TObject.h>
#include <TEfficiency.h>
#include <TCanvas.h>
#include <TROOT.h>

#include <QVector>
#include <QStringList>
#include <QDate>

#include <cmath>
#include <assert.h>

#define DEBUG 0
#define INFO_OUT_TAG "ObjectManager> "
#include <debugging.hh>

#include <QDebug>

namespace ACsoft {

Utilities::ObjectManager::ObjectManager(ConfigHandler* config, const std::string& resultdir, const std::string& suffix)
: m_file(0)
, m_objects()
, m_resultdir(resultdir)
, m_prefix("")
, m_suffix(suffix)
, m_ownedObjects()
{
  Initialize("ObjectManager", config);
}

Utilities::ObjectManager::ObjectManager(const std::string& optionName, ConfigHandler* config, const std::string& resultdir, const std::string& suffix)
: m_file(0)
, m_objects()
, m_resultdir(resultdir)
, m_prefix("")
, m_suffix(suffix)
, m_ownedObjects()
{
  Initialize(optionName, config);
}

Utilities::ObjectManager::ObjectManager(const std::string& filename, const std::string& resultdir, const std::string& suffix)
: m_file(0)
, m_objects()
, m_resultdir(resultdir)
, m_prefix("")
, m_suffix(suffix)
, m_ownedObjects()
{
  if (!filename.empty())
    Open(filename);
}

Utilities::ObjectManager::~ObjectManager() {
  Close();
  qDeleteAll(m_ownedObjects);
}

void Utilities::ObjectManager::Open(const std::string& filename, const std::string& option) {
  if (m_file) {
    INFO_OUT << "One file was already loaded. Closing it." << std::endl;
    Close();
  }
  INFO_OUT << "Opening file: " << filename << std::endl;
  m_file = new TFile(filename.c_str(), option.c_str());
  gROOT->cd();
}

void Utilities::ObjectManager::Close() {
  if (m_file) {
    INFO_OUT << "Closing file: " << m_file->GetName() << std::endl;
    m_file->Close();
    delete m_file;
    m_file = 0;
  }
}

TObject* Utilities::ObjectManager::Add(TObject* object) {
  return AddObject(object);
}

TH3* Utilities::ObjectManager::Add(TH3* object) {
  return static_cast<TH3*>(AddObject(object));
}

TH2* Utilities::ObjectManager::Add(TH2* object) {
  return static_cast<TH2*>(AddObject(object));
}

TH1* Utilities::ObjectManager::Add(TH1* object) {
  return static_cast<TH1*>(AddObject(object));
}

TH3D* Utilities::ObjectManager::Add(TH3D* object) {
  return static_cast<TH3D*>(AddObject(object));
}

TH2D* Utilities::ObjectManager::Add(TH2D* object) {
  return static_cast<TH2D*>(AddObject(object));
}

TH1D* Utilities::ObjectManager::Add(TH1D* object) {
  return static_cast<TH1D*>(AddObject(object));
}

TH3F* Utilities::ObjectManager::Add(TH3F* object) {
  return static_cast<TH3F*>(AddObject(object));
}

TH2F* Utilities::ObjectManager::Add(TH2F* object) {
  return static_cast<TH2F*>(AddObject(object));
}

TH1F* Utilities::ObjectManager::Add(TH1F* object) {
  return static_cast<TH1F*>(AddObject(object));
}

TEfficiency* Utilities::ObjectManager::Add(TEfficiency* object) {
  return static_cast<TEfficiency*>(AddObject(object));
}

TCanvas* Utilities::ObjectManager::Add(TCanvas* object) {
  return static_cast<TCanvas*>(AddObject(object));
}

void Utilities::ObjectManager::HandOverLastAdded() {
  m_ownedObjects.push_back(m_objects.last());
}

void Utilities::ObjectManager::WriteToFile() {
  WriteToFile(m_resultdir, m_prefix, m_suffix);
}

void Utilities::ObjectManager::WriteToFile(const std::string& resultdir, const std::string& prefix, const std::string& suffix) {
  QString outputFileName = QString::fromStdString(MakeStandardRootFileName(resultdir, prefix, suffix));
  Open(outputFileName.toStdString(), "RECREATE");
  INFO_OUT <<  "Set current root file to " << m_file->GetName() << std::endl;
  if( !m_file->IsOpen() ){
    WARN_OUT << "ERROR opening output file \"" << qPrintable(outputFileName) << "\"!" << std::endl;
    throw std::runtime_error("ERROR opening output file");
  }
  m_file->cd();
  for (int i = 0; i < m_objects.size(); ++i) {
    assert(m_objects[i]);
    std::cout << "Writing " << m_objects[i]->GetName() << " to file ..." << std::endl;
    m_objects[i]->Write();
  }
  Close();
}

bool Utilities::ObjectManager::HasFile() const {
  return m_file;
}

std::string Utilities::ObjectManager::GetFilename() const {
  if (m_file)
    return m_file->GetName();
  return "";
}

void Utilities::ObjectManager::SetPrefix(const std::string& prefix) {
  m_prefix = prefix;
}

int Utilities::ObjectManager::GetNumberOfAddedObjects() const {
  return m_objects.size();
}

TObject* Utilities::ObjectManager::AddObject(TObject* object) {
  if (m_file) {
    const QString name = object->GetName();
    TObject* readObject = m_file->Get(qPrintable(name));
    if (!readObject) {
      WARN_OUT << "Object " << qPrintable(name) << " not found in file! The new created object will be used." << std::endl;
      return object;
    }
    delete object;
    object = 0;
    return readObject->Clone(qPrintable(name));
  } else {
    m_objects.push_back(object);
    return object;
  }
}

void Utilities::ObjectManager::Initialize(const std::string& optionName, ConfigHandler* config) {
  m_prefix = (QString("output_")+(QDate::currentDate()).toString("yyyy-MM-dd")).toStdString();
  std::string inputfile = "";
  config->GetValue(optionName, "inputfile", inputfile, "Input root file.");
  config->GetValue(optionName, "prefix", m_prefix, "Prefix for output file. Directory and suffix are set via global option.");
  if (inputfile.size() != 0)
    Open(inputfile);
}

std::string Utilities::ObjectManager::MakeStandardRootFileName( std::string resultdir, std::string prefix, std::string suffix) {
  
  if( prefix == "" )
    WARN_OUT << "Empty prefix!" << std::endl;
  std::string outputFileName = "";
  if( resultdir != "" )
    outputFileName += ( resultdir + std::string("/") );
  outputFileName += prefix;
  if( suffix != "" )
    outputFileName += ( std::string("_") + suffix );
  outputFileName += std::string(".root");
  return outputFileName;
}

std::string Utilities::ObjectManager::MakeStandardPdfFileName( std::string resultdir, std::string prefix, std::string suffix) {
  
  if( prefix == "" )
    WARN_OUT << "Empty prefix!" << std::endl;
  std::string outputFileName = "";
  if( resultdir != "" )
    outputFileName += ( resultdir + std::string("/") );
  outputFileName += prefix;
  if( suffix != "" )
    outputFileName += ( std::string("_") + suffix );
  outputFileName += std::string(".pdf");
  return outputFileName;
}

}
