#ifndef ObjectManager_hh
#define ObjectManager_hh
  
#include <QVector>

#include <string>

class TFile;
class TObject;
class TH3;
class TH2;
class TH1;
class TH3D;
class TH2D;
class TH1D;
class TH3F;
class TH2F;
class TH1F;
class TEfficiency;
class TCanvas;

namespace ACsoft {

namespace Utilities {

  class ConfigHandler;

  /** A simple class for managing the write and read procedure of TObjects for you.
   *
   * Create a confighandler for managing your programs commandline arguments.
   * \code
   * Utilities::ConfigHandler config;
   * \endcode
   *
   * Besides your other arguments you need variables for the result directory and a suffix to uniquely identify result files. For example get them from the config manager.
   * \code
   * std::string resultdir = "";
   * config.GetValue( "OPTIONS", "resultdir", resultdir,"General directory where result files should be stored. Current directory is used if option is not specified.");
   * std::string suffix = "";
   * config.GetValue( "OPTIONS", "suffix", suffix, "A string identifier to be used in parallel computing, to uniquely identify result files.");
   * \endcode
   *
   * Create an ObjectManager. You can set the commandline argument --ObjectManager/prefix <prefix> to change the default("output_YYYY-MM-dd") root output file name . If you want to read from the output file again you can set the command line arguemnt --ObjectManager/inputfile <inputfile>.
   * \code
   * Utilities::ObjectManager objectManager(&config, resultdir, suffix);
   * \endcode
   *
   * Create your histograms or other objects.
   * \code
   * TH1* rigidityHistogram = objectManager.Add(new TH1D("rigidity", "", 100, 1, 100));
   * TH2* chargeVsEnergyHistogram = objectManager.Add(new TH2D("charge vs energy", "", 100, 1, 100, 100, 0, 10));
   * \endcode
   * Beware that the original histogram that you feed to ObjectManager gets destructed! Never use:
   * \code
   * TH1* someHistogram = new TH1D(...);
   * objectManager.Add(someHistogram);
   * someHistogram->SetLineColor(...);
   * \endcode
   * This will \b CRASH when the histogram is read from disk, as the original histogram is replaced. So stick to the usage paradigm described above.
   *
   * After you finished your event loop you can write the added object to file.
   * \code
   * objectManager.WriteToFile();
   * \endcode
   *
   * If you want to use the ObjectManager also to read from the files you can then ask it if the containing histograms have been read from a file. For example you could only execute your event loop if the ObjectManager has no file.
   * \code
   * objectManager.HasFile()
   * \endcode
   */

  class ObjectManager {
  public:
    /** Constructor
     * @param cfghandler Config handler
     * @param resultdir Result directory where the output file is stored
     * @param suffix Suffix for the output file. Used in parallel computing, to uniquely identify result files
     */
    ObjectManager(ConfigHandler* cfghandler, const std::string& resultdir, const std::string& suffix);

    /** Overloaded constructor if you want to specify the confighander OPTION*/
    ObjectManager(const std::string& optionName, ConfigHandler* cfghandler, const std::string& resultdir, const std::string& suffix);

    /** Overloaded constructor without ConfigHandler
     * @param filename Filename of the file
     * @param resultdir Result directory where the output file is stored
     * @param suffix Suffix for the output file. Used in parallel computing, to uniquely identify result files
     */
    ObjectManager(const std::string& filename, const std::string& resultdir = "", const std::string& suffix = "");

    ~ObjectManager();

    /** Add a TObject element to the manager*/
    TObject* Add(TObject*);
    /** Overloaded function for TH3*/
    TH3* Add(TH3*);
    /** Overloaded function for TH2*/
    TH2* Add(TH2*);
    /** Overloaded function for TH1*/
    TH1* Add(TH1*);
    /** Overloaded function for TH3D*/
    TH3D* Add(TH3D*);
    /** Overloaded function for TH2D*/
    TH2D* Add(TH2D*);
    /** Overloaded function for TH1D*/
    TH1D* Add(TH1D*);
    /** Overloaded function for TH3F*/
    TH3F* Add(TH3F*);
    /** Overloaded function for TH2F*/
    TH2F* Add(TH2F*);
    /** Overloaded function for TH1F*/
    TH1F* Add(TH1F*);
    /** Overloaded function for TEfficiency*/
    TEfficiency* Add(TEfficiency*);
    /** Overloaded function for TCanvas*/
    TCanvas* Add(TCanvas*);

    /** Transmit ownership of last added object to the ObjectManager so he is responsible for deletion.
     *
     * Keep in mind that ObjectManager::~ObjectManager will delete all those objects!
     *
     */
    void HandOverLastAdded();

    /** Write the added objects to file*/
    void WriteToFile();

    void Close();

    /** Tells you if the manager has a file set via --ObjectManager/inputfile and can load objects from this file*/
    bool HasFile() const;

    /** Get the input filename*/
    std::string GetFilename() const;

    /** Overwrite the prefix*/
    void SetPrefix(const std::string&);

    /** Get the number of added objects*/
    int GetNumberOfAddedObjects() const;

    /** Helperfunctions stolen from IO::FileManager to generate the filename. This heist prevented the utilities library to be dependent from the io library.*/
    /** Utility function to assemble an output file name according to standard scheme.
     *
     * Typically returns something like \c resultdir/prefix_suffix.root, but also handles the case where resultdir and/or suffix are empty.
     */
    static std::string MakeStandardRootFileName( std::string resultdir, std::string prefix, std::string suffix);

    /** Utility function to assemble an output file name according to standard scheme for pdf files.
     *
     * Typically returns something like \c resultdir/prefix_suffix.pdf, but also handles the case where resultdir and/or suffix are empty.
     */
    static std::string MakeStandardPdfFileName( std::string resultdir, std::string prefix, std::string suffix);

  private:
    TFile* m_file;
    QVector<TObject*> m_objects;
    std::string m_resultdir;
    std::string m_prefix;
    std::string m_suffix;
    QVector<TObject*> m_ownedObjects;

    void Initialize(const std::string& optionName, ConfigHandler* cfghandler);
    void WriteToFile(const std::string& resultdir, const std::string& prefix, const std::string& suffix);
    TObject* AddObject(TObject*);
    void Open(const std::string& filename, const std::string& option = "READ");
  };
}

}

#endif
