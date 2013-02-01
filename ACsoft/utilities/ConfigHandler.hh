#ifndef CONFIGHANDLER_HH
#define CONFIGHANDLER_HH

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <stdexcept>
#include <sstream>
#include <string>
#include <algorithm>

#include <exception>
#include <stdexcept>

#include "StringTools.hh"

/// Utility classes.
namespace ACsoft {

namespace Utilities {
  typedef std::map < std::string, std::string > Dict_t;
  typedef std::map < std::string, Dict_t > SectionedDict_t;

  /**
   * Class for reading, writing and storing of configuration data. The
   * data is arranged as a set of key/value pairs which are contained in
   * sections.  The data can be read in from a text file, from a stream,
   * or from a command-line argument.
   *
   * \author Karl Kosack (CEA Saclay)
   *
   * Please address questions related to this class to Henning. Code
   * used here with permission from Karl.
   *
   * Format is as follows:
   *  \code
   *     [section name]
   *         key1 = value      #  comment 
   *         key2 = other value 
   *  \endcode
   *
   * Whitespace is ignored (quotation marks can be used to override
   * this), comments are stripped off, the "=" sign is optional.  Long
   * lines may be continued on the next line using the "\" character,
   * as in bash.
   *
   * You may use environment variable or refer to other keys within
   * the same section by using the $(VARIABLE) syntax. For example,
   *
   * \code
   *    [OPTIONS]
   *        filename = "$(SOMEVARIABLE)/analysis/lists/blah.txt"
   *        variable = 12.6
   *        anotherone = $(variable)  # now is also 12.6
   * \endcode
   *
   * For command-line args, it reads arguments in following formats
   * (negative number arguments are handled correctly):
   *
   * \code 
   *   > mycommand --key1=value --key2 --key3 value3 -key4=value --key5 -2.4
   *   > mycommand --help --section1/key3 12.4
   * \endcode
   *
   * note that keywords start with a "-" or "--", and may be separated
   * from the value with an "=" or whitespace.  If no argument is
   * specified, the value is set to 1. To specify a particular section
   * on the command line, use keywords like: 
   *
   * \code 
   *   --section/keyword 12.4 
   * \endcode
   *
   * Global options specified on the command-line (without a specific
   * section name) are stored in a section called "OPTIONS" (all
   * caps). For example the "--help" option in the above example would
   * be accessed by:
   * 
   * \code 
   *    conf.GetFlag( "OPTIONS", "help", "show help mesage" ) 
   * \endcode
   *
   *
   * Note that you can include other config files with the --include
   * or the --config option, so you cannot use these as regular options
   * for your programs.
   * --include <filename> searches first in the current working directory 
   * and then any include directories (set by the AddIncludeDir() function).
   * 
   *
   * You can turn on some extra debugging output by specifying the
   * --debug-config option to your program
   */
  class ConfigHandler {
  
  public:
    /// \cond INTERNAL
    class SyntaxError : public std::runtime_error {
    public: 
      SyntaxError( std::string what ) : std::runtime_error( what ) {;}
    };
    /// \endcond

    ConfigHandler(  );

    enum WriteMode { WRITE_ALL_SECTIONS, WRITE_MARKED_SECTIONS };

    void Read( std::string filename, int level = 0 );
    void Read( std::istream & s, char linesep = '\n', int level = 0 );
    void ReadCommandLine( int argc, char* argv[] );
    void ReadEnvironment();

    void Write( std::ostream & stream );
    void Write( std::string filename );
    void SetWriteMode( WriteMode mode ) {fWriteMode=mode;}
    void MarkSection( std::string section );
    void IgnoreKeywordCheckForSection( std::string section ) {
      fNoCheckSection[section] = true;
    }

    void SetSectionHelpText( std::string section, std::string text );
    void SetHelpText( std::string section, std::string key,
		      std::string text );
    void SetProgramHelpText( std::string program_name, std::string text );

    void AddHelpExample( std::string description, std::string example );

    std::string GetHelpText( std::string section, std::string key );
    std::string GetSectionHelpText( std::string section );

    void PrintUsageMessage(); 
    bool PrintUnusedKeywordWarning(bool pause_on_error=false);
    bool PrintDeprecatedKeywordWarning();
    void HandleHelpAndInvalidOptions(int argc, std::string extra_message = "");

    template <class T> void GetValue( std::string section, std::string key,
				      T& value, std::string helptext = "");
    
    bool GetFlag( std::string section, std::string key, 
		  std::string helptext = "");
    
    
    inline std::string GetStringValue( std::string section, 
				       std::string key );    
    template <class T> void SetValueByRef( std::string section, 
					   std::string key, T &value );
    template <class T> void SetValue( std::string section, 
				      std::string key, T value );
    

    void EnableExceptions(bool val){fEnableExceptions=val;}
    inline std::vector<std::string> GetListOfSections();
    inline std::vector<std::string> GetListOfKeysForSection(std::string sec);

    void AddIncludeDir( std::string dir );
    void SetIncludePath( std::string path );

    bool SectionExists( std::string section ) {
      return ( fDict.count( section ) != 0 );
    }

    bool KeyExists( std::string section, std::string key ) {
      if ( SectionExists( section ) ) {
	return ( fDict[section].count( key ) != 0 );
      }
      else
	return false;
    }

    void Deprecate(std::string section,std::string key, std::string reason);

    std::string GetName() {return fName;};

  private:
    static const char SPACE_PLACEHOLDER = ( char ) 254;

    void AddToDictionary( std::string section, std::string key,
			  std::string value ) {
      fDict[section][key] = value;
    }

    bool IsRequired( std::string section, std::string key );
    bool IsDeprecated( std::string section,std::string key );
    bool WarnIfDeprecated( std::string section,std::string key );

    void PrepareQuotedStrings( std::string & line );
    void FinishQuotedStrings( std::string & line );
    std::string ExpandVariables( std::string &line, std::string cursection );

    bool fEnableExceptions;
    SectionedDict_t fDict;
    SectionedDict_t fHelp;
    SectionedDict_t fDefaultValue;
    SectionedDict_t fDeprecated;
    Dict_t fSectionHelp;
    Dict_t fSectionRequired;	// just for use with PrintUsageMessage()
    std::map<std::string,bool> fNoCheckSection;  
    std::string fProgramHelp;
    std::string fProgramName;
    std::vector < std::string > fExamples;
    std::vector < std::string > fExampleDescriptions;
    std::vector < std::string > fIncludeDirs;
    
    std::vector < std::string > fMarkedSections;
    WriteMode fWriteMode;

    std::string fName;
    
    //    ClassDef(Utilities::ConfigHandler, 0);
  };

  /**
   * Returns a vector of section names as strings.
   */
  std::vector < std::string > ConfigHandler::GetListOfSections(  ) {
    std::vector < std::string > seclist;
    SectionedDict_t::iterator it;
    for ( it = fDict.begin(  ); it != fDict.end(  ); it++ ) {
      if (fWriteMode == WRITE_MARKED_SECTIONS) {
	// only marked sections
	if (std::find( fMarkedSections.begin(),
		       fMarkedSections.end(), it->first) 
	    != fMarkedSections.end()) {
	  seclist.push_back( it->first );
	}
      }
      else {
	// all sections
	seclist.push_back( it->first );
      }
    }
    return seclist;
  }

  /**
   * Returns a vector of keywords in a particular section.
   */
  std::vector < std::string >
  ConfigHandler::GetListOfKeysForSection( std::string section ) {
    std::vector < std::string > keylist;
    Dict_t::iterator it;
    if ( SectionExists( section ) ) {
      for ( it = fDict[section].begin(  ); it != fDict[section].end(  );
	    it++ ) {
	keylist.push_back( it->first );
      }
    }
    return keylist;
  }

  /**
   * Helper function to get the string representation of a key. See
   * GetValue() for a more general function.
   */
  std::string ConfigHandler::GetStringValue( std::string section,
					     std::string key ) {
    if ( KeyExists( section, key ) )
      return fDict[section][key];
    else {
      if ( fEnableExceptions )
	throw std::runtime_error( "key doesn't exist: " + key );
      else
	return "";
    }
  }

  /**
   * Sets the value of the 'val' argument to the value from the
   * config file, if it exists. The value is converted from an
   * internal string representation using the object's streamer,
   * which must be defined. If the values doesn't exist in the config
   * file, it is added given value is stored as a default.
   *
   * Example:
   *
   * \code
   *    ConfigHandler config;
   *    ...
   *
   *    double gamma=1.234; // need to give gamma a default value!
   *
   *    config.GetValue( "cuts","gamma", gamma,"Important paramter for cuts!");
   *
   *    cout << gamma << endl;
   * \endcode
   *
   * This will set gamma to the value in the config, or if it isn't
   * already there, gamma will remain 1.234 and that will be set as
   * it's default.  You should also specify help text for all values
   * you get here or using Utilites::ConfigHandler::SetHelpText().
   *
   * \param[in] section  the section in which to look for the key
   * \param[in] key      configuration keyword
   * \param[in,out] val variable to store value in, should be set to 
   * default value before being passed. Remains as default if the key is 
   * not found.
   */
  template < class T > void ConfigHandler::GetValue( std::string section,
						     std::string key, T & val,
						     std::string helptext ) {
    // stream the object into a string:
    std::ostringstream os;
    os << val;

    WarnIfDeprecated(section,key);

    // ===========================================================
    // Either extract the value if it's already in the dictionary,
    // or put it there if it isn't
    // ===========================================================

    if ( KeyExists( section, key ) ) {
      // Key exists, so get it's value from the dictionary and
      // overwrite val

      std::string valstring = GetStringValue( section, key );
      RemoveWhiteSpace( valstring );
      
      // if the value is not blank,
      if ( valstring != "" ) {
	std::istringstream stream( valstring );

	// extract the key into the value:
	if ( !( stream >> val ) ) {
	  if ( fEnableExceptions )
	    throw SyntaxError( "SyntaxError while streaming: '"
			       + section + "/" + key + "=" +
			       GetStringValue( section,
					       key ) + "'" );
	  else
	    std::cout << "** ERROR: couldn't stream " << section << "/"
		      << key << std::endl;
	}
      }
    }
    else  {
      // key doesn't exist, so add it to the dictionary
      AddToDictionary( section, key, os.str(  ) );
    }
    
    // ===========================================================
    // mark this key as required and set its default value
    // ===========================================================

    if ( IsRequired( section, key ) == false ) {
      fDefaultValue[section][key] = os.str(  );
      fSectionRequired[section] = "true";
    }
    
    if (helptext != "") {
      if (GetHelpText( section, key ) == "") {
	SetHelpText(section,key,helptext);
      }
      else {
	if (KeyExists("OPTIONS","debug-config") ) {
	  std::cout<< "Help text already set for config key: "
		   << section<<"/"<<key<< std::endl;
	}
      }
    }
  }
  
  /**
   * Set value of a key (or add a new key) from an object with a streamer.
   */
  template <class T> void ConfigHandler::SetValueByRef( std::string section, 
							std::string key, T &value){
    // stream the object into a string:
    std::ostringstream os;
    os << std::setprecision(30) << value;
    
    AddToDictionary ( section, key, os.str() );
  }
  
  /**
   * Set value of a key (or add a new key) from an object or basic data
   * type with a streamer (called by value instead of reference).
   */
  template < class T > void ConfigHandler::SetValue( std::string section,
						     std::string key,
						     T value ) {
    SetValueByRef( section, key, value );
  }
}

/**
 * Streamer for a standard pair: e.g. (2,3)
 */
template < class T > std::istream & operator >>( std::istream & is,
						 std::pair < T, T > &p )
{
  T lo, hi;
  char ch;

  is >> ch;
  if ( ch == '(' ) {
    is >> lo >> ch;
    if ( ch == ',' ) {
      is >> hi >> ch;
      if ( ch == ')' ) {
	p.first = lo;
	p.second = hi;
      }
    }
    else if ( ch == ')' )
      p.second = lo;
  }
  else {
    is.putback( ch );
    is >> hi;
    p.second = hi;
  }
  return is;
}


/**
 * Streamer for a standard pair: e.g. (2,3)
 */
template < class T > std::ostream & operator <<( std::ostream & os,
						 std::pair < T, T > &p )
{
  os << "(" << p.first << "," << p.second << ")";

  return os;
}


/**
 * Streamer to read and write a comma-separated list of values into a vector
 * Template Class which takes (in principle) any comma separated list
 */
template<class T> std::ostream& operator<<(std::ostream& os,const std::vector<T>& vec)
{
  int nvec = vec.size();
  for (int ivec = 0;ivec < nvec;++ivec){
    os << vec[ivec];
    if(ivec < nvec-1) os << ", ";
  }
  return os;
}

/**
 * Streamer to read and write a comma-separated list of values into a vector
 * Template Class which takes (in principle) any comma separated list
 */
template<class T> std::istream& operator>>(std::istream& is,std::vector<T>& vec)
{

  // clear the vector first:
  vec.clear();
  
  std::string line;
  if (getline(is,line)){
    
    std::vector<std::string> newvec = split(line,", \t");
    for (std::vector<std::string>::iterator inew = newvec.begin();
         inew != newvec.end();++inew){
      std::stringstream vstream(*inew);
      T val;
      vstream >> val;
       vec.push_back(val);
    }
  }
  return is;
}

/**
 * Streamer for a comma-separated list into a set
 */
template < class T > std::istringstream & operator >>( std::istringstream & is,
						       std::set < T > &s )
{
  T locv;

  s.clear(  );
  char c;

  while ( !is.eof(  ) ) {
    is >> c;
    if ( c != ',' ) {
      is.putback( c );
      is >> locv;
      s.insert( locv );
    }
  }

  return is;
}

/**
 * Streamer for a comma-separated list into a set
 */
template < class T > std::ostream & operator <<( std::ostream & os,
						 const std::set < T > &s )
{
  typename std::set < T >::iterator sit;

  sit = s.begin(  );
  os << *sit;
  sit++;
  while ( sit != s.end(  ) ) {
    os << "," << *sit;
    sit++;
  }

  return os;
}

#ifndef __CINT__

/**
 * Streamer for a comma-separated list into a map of value -> boolean
 */
template < class T > std::istringstream & operator>>( std::istringstream & is,
						      std::map < T, bool > &m )
{
  T locv;

  m.clear(  );
  char c;

  while ( !is.eof(  ) ) {
    is >> c;
    if ( c != ',' ) {
      is.putback( c );
      is >> locv;
      m[locv] = true;
    }
  }

  return is;
}

/**
 * output Streamer for a comma-separated list into a map of value -> boolean
 */
template < class T > std::ostream & operator <<( std::ostream & os,
						 const std::map < T, bool > &m )
{
  typename std::map < T, bool >::const_iterator sit;

  sit = m.begin(  );
  if ( sit->second )
    os << sit->first;
  sit++;
  while ( sit != m.end(  ) ) {
    if ( sit->second )
      os << "," << sit->first;
    sit++;
  }

  return os;
}

#endif

/**
 * Supercharged enumerator class, can be streamed in and out from a string
 * example:
 *
 * \code
 * ENUM{Name,firstvalue,secondvalue,thirdvalue}
 *
 * Name myvalue = firstvalue;
 * std::cout << myvalue << std::endl;
 *
 * \endcode
 *
 * A global vector<string> called [Name]Vector is created also, which
 * contains the mapping of value to text string
 */
#if defined(__CINT__) || defined(CINTOBJECT) 
#define ENUM(name, ...) enum name { __VA_ARGS__ };
#else
#define ENUM(name, ...) enum name { __VA_ARGS__ };\
 static std::vector<std::string> name##Vector = split(#__VA_ARGS__,std::string(", "));\
std::istream& operator>>(std::istream& is,name& val)\
{ std::string valstring;\
if (is >> valstring){\
std::vector<std::string>::iterator it = find(name##Vector.begin(),\
name##Vector.end(),valstring);\
if (it != name##Vector.end()){ val = (name)distance(name##Vector.begin(),it);\
}else{throw std::runtime_error("SyntaxError in enumerator " + std::string(#name)\
                         + ": " + valstring + " is not valid. Possible values: "\
                          + # __VA_ARGS__);\
}}return is;}\
std::ostream& operator<<(std::ostream& os,name& val)\
{os << name##Vector[val];\
return os;}
#endif // __CINT__

}

#endif				// CONFIGHANDLER_HH
