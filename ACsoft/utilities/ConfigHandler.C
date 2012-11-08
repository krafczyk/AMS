#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <map>
#include <list>
#include <ctime>
#include <cstring>
#include <unistd.h>
#include <algorithm>    
#include <stdexcept>
#include <sstream>
#include "StringTools.hh"
#include "ConfigHandler.hh"

#define DEBUG 0
#include "debugging.hh"

using namespace std;


#ifdef __APPLE__
#include <crt_externs.h>
#define environ (*_NSGetEnviron())
#else
extern char **environ; // environment variables
#endif

Utilities::ConfigHandler::ConfigHandler() :
  fEnableExceptions( true ) ,
  fWriteMode( WRITE_ALL_SECTIONS )
{
  // add user include path from the CONFIGPATH variable

  char* path = getenv("CONFIGPATH");
  
  if (path != NULL) {
    SetIncludePath( path );
  }

}


/**
 * Set help string for a particular section, which is used when
 * writing out a configuration database to a text file.
 */
void Utilities::ConfigHandler::SetSectionHelpText( std::string section, std::string text) {
  fSectionHelp[section] = text;
}

/**
 * Sets help text that displays at the top of the usage message, which
 * is general for the specific program containing the
 * ConfigHandler. See hddst/src/hap.C for an example.
 */
void Utilities::ConfigHandler::SetProgramHelpText( std::string program_name, std::string text ) {
  fProgramName = program_name;
  fProgramHelp = text;
}

/**
 * Set help string for a particular keyword in a section, which is
 * used when writing out a configuration database to a text file. The
 * text is always appended to any previously set help text for the
 * given key.
 */
void Utilities::ConfigHandler::SetHelpText( std::string section,std::string key,std::string text ) {
  if (fHelp[section].count( key ) > 0)
    fHelp[section][key] += text;
  else 
    fHelp[section][key] = text;
}



/**
 * Add an example usage string which will appear at the end of the
 * usage message.
 *
 * \param description - what the example does
 *
 * \param example - an example usage (omit the name of the program, it
 * is added automatically)
 *
 * For example:
 * \code
 *   conf.SetProgramHelpText("myprog", "my program");
 *   conf.AddHelpExample( "Analyze some data",
 *                        "--file data.dat --enable-analysis" );
 * \endcode
 *
 * will add something like the following to the usage message:
 *
 * Analyze some data:
 *    myprog --file data.dat --enable-analysis
 */

void Utilities::ConfigHandler::AddHelpExample( std::string description, std::string example ) {

  fExampleDescriptions.push_back( description );
  fExamples.push_back( example );

}


/**
 * Get the help text for a particular keyword in a section
 */
string Utilities::ConfigHandler::GetHelpText( string section, string key ){
  if ( fHelp.count( section ) != 0 && 
       fHelp[section].count( key ) != 0)  {
    return fHelp[section][key];
  }
  else
    return "";
}

/**
 * Returns help string for specified section
 */
string Utilities::ConfigHandler::GetSectionHelpText( std::string section ) {

  if (fSectionHelp.count( section )) {
    return fSectionHelp[section];

  }
  return "";

}


/**
 * Read a configuration database from a stream. This may be an already
 * opened file, or for instance a command-line argument string.
 *
 * \param stream - stream to read from
 * \param linesep - character to use as line separator (default '\n')
 * \param level - used to determine recursion depth, leave unspecified
 *
 */
void Utilities::ConfigHandler::Read( std::istream &stream, char linesep, int level ) {
   
  string line;
  vector<string> tokens;
  unsigned long k;
  int num=0;
  string cursection="OPTIONS";
  string key;
  string spacer = "CONF: ";


  // check for include-loop
  if (level > 20 ) {
    WARN_OUT << "*** TOO MANY LEVELS OF RECURSION! "<< endl;
    WARN_OUT << "*** Perhaps you have an 'include' loop? "<< endl;
    throw std::runtime_error("ConfigHandler: recursion too deep");
  }

  for (int i=0; i<level; i++)
    spacer += "| ";

  while (stream && !stream.eof() && getline( stream, line, linesep )) {

    try {

      // ------------------------------------------------------------
      // check for continuation lines (marked by "\"):
      while ((k = line.find("\\")) != string::npos) {
	string extraline;
	getline( stream, extraline, linesep );
	line = line.substr( 0,k ); // remove the "\"
	line += extraline;	   // accumulate the next line
	num++;			   // count continued lines too
      }

      num++;
	 
      // ------------------------------------------------------------
      // remove white space and comments, and expand variables
	 
      k = line.find("#");
      line = line.substr(0,k);
      line = ExpandVariables(line,cursection);
      RemoveWhiteSpace(line);

      // ------------------------------------------------------------
      // check if this is a section heading (which look like [NAME]):
	 
      if (line[0] == '[') {
	int br = line.find("]");
	cursection = line.substr(1,br-1);
	cout << spacer << "  "<<cursection << endl;
	continue;
      }
	 
      // ------------------------------------------------------------
      // ensure that quoted strings take up exactly one token (convert
      // whitespace into special characters). Later,
      // finishQuotedStrings() turns the special characters back into
      // whitespace.
	 
      PrepareQuotedStrings( line );

      // ------------------------------------------------------------
      // tokenize, and skip if line is empty or malformed
	 
      tokens = split( line, "= \t" );
      if (tokens.size() == 0) {
	continue;
      }
      else if (tokens.size() == 1 ) {
 	tokens.push_back(" ");	// set value to "1" if no value is
	// specified
      }
	 
      string key2 = tokens[0];
	 
      // remove any extra "-" at the beginning of key (in case people
      // used a "--" commandline arg)
	 
      string::size_type where = key2.find_first_not_of('-');
      if (where != string::npos && where != 0)            
	key2 = key2.substr(where);
	 
      // concatenate the rest of the tokens back into the value
	 
      string value = "";
      for (unsigned int i=1; i<tokens.size(); i++) {
	value.append( tokens[i] );
	if (i!=tokens.size()-1) value.append(" ");
      }
	 
      FinishQuotedStrings( value ); // remove the quoted string delim
      FinishQuotedStrings( key2 ); // remove the quoted string delim
	 
      // ------------------------------------------------------------
      // check if this is an include or config directive. If so,
      // recurse appropriately"
	 
      if (key2 == "include" ) {
	cout << spacer << "Including: "<< value << endl;
	Read(value, level+1);
	cout << spacer << ". end: " << value << endl;
	continue;
      }
//       else if ( key2 == "config") {
// 	// "config" means, look into the $HESSCONFIG/<value>
// 	// directory and open a special file called
// 	// "analysis.conf". Also, add a key to the dictionary for the
// 	// name of the conf

// 	std::string configfile;

// 	try{
// 	  fName = value;
// 	  string HESSCONFIG = getenv("HESSCONFIG");
// 	  string configfile = HESSCONFIG + "/" + value+"/analysis.conf";

// 	  cout << spacer << "Using config: "<< value << endl;
// 	  Read( configfile, level+1 );
// 	  cout << spacer << ". end config: "<< value << endl;

// 	}
// 	catch(std::logic_error& e){
// 	  throw runtime_error("HESSCONFIG environment variable not set");
// 	}	    
//       }
	 
	 
      // ------------------------------------------------------------
      // If we got here, then the only possibility is a key/value
      // pair. Put it into the dictionary:
	 
      // first, check if this key has a section-delimeter in it (usually
      // '/'). If so, ignore "cursection" and use the specified
      // section.  This is so you can specify a single
      // "section/key=value" command on the command-line.
	 
      size_t pos = key2.find("/");
      if ( pos != std::string::npos ) {
	    
	// section specified with "section/key=value";
	    
	string sec = key2.substr(0,pos);
	key2 = key2.substr(pos+1, key2.length()-1);
	    
	WarnIfDeprecated( sec, key2 );
	AddToDictionary( sec, key2, value );
	    
      }
      else {
	    
	// normal key=value, just add it
	    
	WarnIfDeprecated( cursection, key2 );
	AddToDictionary( cursection, key2, value );
	    
      }

    }
    catch (SyntaxError e) {
      cout << "Syntax error at line: "<<num
	   <<": "<<e.what()
	   << endl;
      if (fEnableExceptions) throw e;
	    
    }
	 
  }
   
}


/**
 * Read a configuration database from a text file.
 *
 * \param filename - name of file to read from
 */
void Utilities::ConfigHandler::Read( std::string filename, int level ){

  ifstream infile(filename.c_str());
  bool isopen = false;

  if (infile.fail()) {

    // try the include path:
    vector<string>::iterator dir;
    for (dir = fIncludeDirs.begin(); dir != fIncludeDirs.end(); dir++) {

      infile.clear();
          
      if (KeyExists("OPTIONS","debug-config")) {
	cout << "Trying include dir: "<< *dir << endl;
      } 
      string filename2 = *dir + "/" + filename ;
      infile.open( filename2.c_str() );
    
      if (infile && !infile.fail()) {
        isopen = true;
        break;
      }
      
    }
    
    if (isopen == false) {
      WARN_OUT << __func__ << ": couldn't open '"<< filename<<"'"<<endl;
      if (fEnableExceptions) 
	throw runtime_error("Couldn't open: '"+filename+"'");
      else
	return;
    }
    
  }
  
  // read from the strea
  Read( infile, '\n', level );
  infile.close();

}



/**
 * \returns true if specified key has been registered as required.
 */
bool Utilities::ConfigHandler::IsRequired(string section, string key ) {
  if ( fDefaultValue.count( section ) != 0 && 
       fDefaultValue[section].count( key ) != 0)  {
    return true;
  }
  else
    return false;
}


/**
 * Reads the current environment variables into a section called "ENV".
 *
 * Example:
 *
 * \code
 *  Utilities::ConfigHandler conf;
 *
 *  conf.ReadEnvironment();
 *  string homedir = conf.GetStringValue("ENV","HOME");
 * \endcode
 *
 * A good way to use this is to let environment variable be default
 * values for explicitly set config variables like so:
 *
 * \code
 *  string analysispath = conf.GetStringValue("ENV","MYROOT");
 *  conf.GetValue( "OPTIONS","AnalysisPath", analysispath, 
 *                 "Path for finding analysis code" );
 *
 * // now analysispath will default to the value of the MYROOT
 * // variable if it's not specified
 * \endcode
 */
void Utilities::ConfigHandler::ReadEnvironment() {
  
  std::stringstream envstream;

  envstream << "[ENV]" << endl;

  for ( ; *environ; ++environ) {
    envstream << *environ << endl;
  }
  
  Read( envstream );

}


/**
 * Calls Read() using the appropriate options for parsing the standard
 * command-line variables argc, argv.
 */
void Utilities::ConfigHandler::ReadCommandLine( int argc, char* argv[] ) {

  string commandline;

  // first make a string out of argv (note argv[0] is the program
  // name, so skip it)

  for (int i=1; i<argc; i++) {

    string arg = argv[i];

    //check if argument starts with a "-", and is not followed by
    //number with a negative value (which might be interpreted as
    //a keyword!)

    if ( (arg[0] == '-') && ! (isdigit(arg[1]) || arg[1] == '.') ) {
      arg[0] = '\n'; // change separator to newline (otherwise
      // leave it as a minus
    }  
	    
    commandline.append(arg);
    if (i!=argc-1) commandline.append(" ");
  }

  // now, make it a stream and call read

  istringstream iss(commandline);

  Read( iss );

}


/**
 * Prints out a usage message with all required keywords, their
 * values, and help text. Note, this only prints required keywords
 * (ones that have been accessed already with GetValue()), not the
 * whole dictionary and is just for use for "usage" messages for
 * programs. To write the database in a readable format, use the
 * ConfigHandler::Write() method.
 *
 * \code
 *
 *    Utilities::ConfigHandler ch;
 *
 *    ch.ReadCommandLine( argc, argv );
 *
 *    // Get values of all required keywords *before* printing usage
 *    // message so that ConfigHandler can tell which ones are required.
 *
 *    ch.GetValue( "OPTIONS","somekey", value, "example key/value");
 *
 *    ...
 *
 *    // Now, print the usage message if requested
 *   
 *    bool help = ch.GetFlag("OPTIONS", "help", "display usage message");
 *     
 *    if (help) {
 *	ch.PrintUsageMessage();
 *	return 0;
 *    }
 *
 * \endcode
 */
void Utilities::ConfigHandler::PrintUsageMessage() {
   
  const string BOLD="\033[01m";
  const string RESET="\033[0m";

  if (fProgramName.length()) {
    cout << "NAME" << endl;
    
    std::string progtext =  fProgramName + " - " + fProgramHelp;
    cout <<  wordWrap( progtext, "    ", 70 ) << endl << endl;
  }

  cout << "CONFIGURATION OPTIONS: "<< endl;


  vector<string>::iterator sect, key;
  vector<string> sections= GetListOfSections();

  // move OPTIONS section to the top:

  sect = std::find( sections.begin(), sections.end(), "OPTIONS" );

  if (sect != sections.end()) {
      sections.erase( sect );
      sections.insert( sections.begin(), 1, "OPTIONS" );
  }
   
   
  // loop through all keys in the dictionary and check if required:


  for (sect=sections.begin(); sect != sections.end(); sect++) {

    // check if this section has any required values in it:
    if (fDefaultValue.count(*sect)!=0) {

      if (*sect == "OPTIONS")
	cout << endl << "    Global options: " << endl;
      else 
	cout << endl 
	     << wordWrap("Options for "+*sect+": "+GetSectionHelpText(*sect),
			 "    ",70)
	     << endl;

      vector<string> keys= GetListOfKeysForSection( *sect );
      for (key=keys.begin(); key != keys.end(); key++) {
	    
	if ( IsRequired( *sect, *key ) ){

	  std::string help = GetHelpText( *sect, *key );
	  if (help == "" ) 
	    help = "PLEASE WRITE HELP TEXT FOR "+ *sect+":"+ *key;
	       
	  if (*sect == "OPTIONS") 
	    cout << BOLD << "\t--"<<*key  << RESET <<endl;	    
	  else
	    cout << BOLD << "\t--"<< *sect<<"/" <<*key << RESET <<endl;

	  cout << wordWrap( help, "\t\t", 60 ) << endl;
	  cout << "\t\tDefault is: "<< fDefaultValue[*sect][*key] << endl;
	  cout << endl;

	}
	    
      }
    }
  }

  cout << "CONFIGURATION FILES: "<< endl;
  cout << BOLD <<"\t--include "<<RESET<<"<configfile>"<< endl;
  
  string message = "All of these options can also be specified in a "
    "configuration file with the proper format (see below). "
    "Configuration files can be included on the command line "
    "by specifying the  --include option (which searches the local "
    "directory first, and then all directories in the CONFIGPATH environment "
    "variable). You can also include other "
    "config files from within a config file itself using 'include=filename'";
  cout <<wordWrap(message, "\t\t",60)<< endl<< endl;

  cout << "CONFIGURATION FILE FORMAT" << endl;
  message ="Configuration files can be written as a set of "
    "key/value pairs in the following format. White space "
    "is generally ignored (unless inside quotation marks), "
    "the '#' character is used for comments, and '\\' for values that span "
    "multiple lines.";
  cout << wordWrap(message,"\t",70)
       << endl<< endl;  

  cout << "\t\t [SectionOne] "<< endl;
  cout << "\t\t      key1 = value1 "<< endl;
  cout << "\t\t      key2 = value2 # comments..."<< endl << endl;
  cout << "\t\t [AnotherSection] "<< endl;
  cout << "\t\t      sample = 1.234 "<< endl;
  cout << "\t\t      thing  = \"this is a test\" "<< endl;
  cout << "\t\t      list   = 12, 24, 17, \\ " << endl
       << "\t\t               18, 19, 20        # Multiple lines use '\\'" << endl
       << "\t\t      key4  = $(sample)          # Substitutions" << endl
       << "\t\t      key4a = $(SectionOne/key1) # Substitutions from other section"<<endl
       << "\t\t      key5  = \"$(HOME)/bin\"    # Environment variables" 
       << endl<<endl;

  cout << "\t\t include \"myconfigfile.cfg\" # include another file "
       << endl << endl;

  if ( fExamples.size() > 0 ) {    
    
    cout << "EXAMPLE USAGE: " << endl;
    size_t nex = fExamples.size();
    for (size_t iex=0; iex<nex; iex++) {
      cout << wordWrap(fExampleDescriptions[iex],"    ",70) << endl;
      cout << wordWrap("% "+fProgramName+" "+fExamples[iex],
		       "         ",70) 
	   << endl<<endl;
    }

  }



}


/**
 * Add a section to the list of "marked" sections. if the WriteMode is
 * WRITE_MARKED_SECTIONS then only marked sections will be written (or
 * returned by GetListOfSections())
 */
void Utilities::ConfigHandler::MarkSection( std::string section ) {

  fMarkedSections.push_back( section );

}


/**
 * Write configuration to the specified file (appends to end of file).
 * If a comma or space-separated list of section names is given in the
 * sections variable, only those sections will be written (otherwise all are).
 */
void Utilities::ConfigHandler::Write(std::string filename ) {

  ofstream outfile( filename.c_str(), std::ios::app );

  if(!outfile) {
    WARN_OUT << "Couldn't open: '" <<filename<<"'"<< endl;
    return;
  }

  Write( outfile );
   
  outfile.close();

}


void Utilities::ConfigHandler::Write( std::ostream &stream ) {

  const int HELPCOL=40;
  const int COLWIDTH=80;
  const string indent="     ";

  vector<string> sections = GetListOfSections();
  vector<string>::iterator sect, key;

  char *thetime;
  time_t lt;

  sect = std::find( sections.begin(), sections.end(), "OPTIONS" );
  if (sect != sections.end()) {   // move OPTIONS section to the top:
      sections.erase( sect );
      sections.insert( sections.begin(), 1, "OPTIONS" );
  }

  stream.fill(' '); // ensure strings area filled with space
  
  lt = time( NULL );
  thetime = ctime( &lt );
  thetime[strlen(thetime)-1] = '\0';

  stream << endl
	 << "# *******************************************"  << endl
	 << "# CONFIG WRITTEN: " << thetime << endl
	 << "# *******************************************"  << endl
	 << endl;

  for (sect=sections.begin(); sect != sections.end(); sect++) {

    stream <<  endl;

    if (GetSectionHelpText( *sect ) != "") {
      stream << "#=================================================="
	     << "==========================" <<endl;
      stream << wordWrap( GetSectionHelpText( *sect ), "# " ) << endl;
      stream << "#=================================================="
	     << "==========================" <<endl;
    }
    stream << "[" << *sect << "]" << endl;
      
    vector<string> keys = GetListOfKeysForSection( *sect );
    for (key=keys.begin(); key != keys.end(); key++) {

      string helptext = GetHelpText( *sect, *key );
      string text = indent + *key + " = ";
      if (fDict[*sect][*key].find_first_not_of(" ") == string::npos) 
	text += "\"\"";
      else 
	text += fDict[*sect][*key];

      if (IsRequired(*sect, *key)) {
	if (fDefaultValue[*sect][*key] != fDict[*sect][*key]) {
	  helptext += " ["+fDefaultValue[*sect][*key]+"]";
	}
      }

      // print out the key/value pair with help text. The position of
      // the help text is either to the right or above the key/value
      // depending on how long it is.

      if ( (int)helptext.length() < COLWIDTH-HELPCOL-2 
	   && (int)text.length() < HELPCOL) {

	// print help after the command
	stream << text; 
	stream << setw(HELPCOL-text.length()) 
	       <<"# "<< helptext
	       << endl;

      }
      else {
	// print help before the command as a block (word wrapped)
	stream <<endl<<wordWrap( helptext, indent+"# ",60) <<endl;
	stream << text <<endl;
      }
      

    }

  }
}


/**
 * Add a directory to the list of  directories searched when finding
 * included files
 */
void Utilities::ConfigHandler::AddIncludeDir( string dir ) {
  
  fIncludeDirs.push_back( dir );

}


/**
 * Set the path for includes (in standard path format, separated by
 * colons). This just calls AddIncludeDir() for each specified path.
 */
void Utilities::ConfigHandler::SetIncludePath( std::string path ) {
  
  vector<string> paths = split( path, ":" );
  vector<string>::iterator dir;
  
  fIncludeDirs.clear();

  for (dir=paths.begin(); dir !=paths.end(); dir++) {
    
    AddIncludeDir( *dir );

  }
  

}

void Utilities::ConfigHandler::PrepareQuotedStrings( string &line ) {

  vector<int> quotes;
   
  // find quotes
   
  for (int i=0; i<(int)line.length(); i++) {
    if (line[i] == '"') quotes.push_back(i);
  }
   
  if (quotes.size() % 2 != 0) {
    throw SyntaxError("Missing quotation mark");
  }
   
  for (int i=0; i<(int)quotes.size(); i+=2) {
      
    for (int j=quotes[i]; j<=quotes[i+1]; j++) {
      if (line[j] == ' ') {
	line[j]=SPACE_PLACEHOLDER;
      }
    }
      
  }
   
  for (int i=0; i<(int)quotes.size(); i++) {
    line[quotes[i]] = ' ';
  }
   
   
}

void Utilities::ConfigHandler::FinishQuotedStrings( string &line ) {
   
  for (unsigned int i=0; i<line.length(); i++) {
    if (line[i]==SPACE_PLACEHOLDER) line[i]=' ';
  }

}


/**
 * Attempt to expand variables written in the form: $(VARIABLE)
 * Variables will be read from the user's environment first, and then
 * from the current section otherwise.
 */
string Utilities::ConfigHandler::ExpandVariables( std::string &line, std::string cursection ) {

  size_t pos;
  size_t endpos;
  string newline = line;
  
  while ( (pos = newline.find("$(")) != string::npos) {
    
    endpos = newline.find(")",pos);

    if (endpos == string::npos) {
      throw SyntaxError("Missing final ')' for variable");
    }

    std::string varname = newline.substr( pos+2, endpos-pos-2 );
    std::string value = "";
    std::string section = cursection;

    // first try to expand an environment variable
    if (getenv(varname.c_str()) != NULL) {
      value = getenv(varname.c_str());
    }
    else {
      // otherwise, try a config variable

      size_t pos2 = varname.find("/"); // deal with $(section/variable)
				      // style keys
      if ( pos2 != std::string::npos ) {
	section = varname.substr(0,pos2);
	varname = varname.substr(pos2+1, varname.length()-1);
      }
      
      if (KeyExists( section, varname ) ){
	value = GetStringValue( section, varname );
      }
      else {
	throw SyntaxError("Couldn't expand variable: '"+varname+"'");
      }
    }
       
    newline.replace( pos,endpos-pos+1, value );
      
  }

  return newline;

}


/**
 * Similar to GetValue, but specifically for handling booleans. 
 *
 * \returns true or false
 *
 * \code
 *
 *  bool helpflag = config.GetFlag( "OPTIONS","help", "display help message" );
 *  if (helpflag==true) 
 *     DoSomething();
 *
 * \endcode
 */
bool Utilities::ConfigHandler::GetFlag( std::string section, std::string key, std::string helptext) {

  string def;

  if (KeyExists( section,key ) ) {

    string val = GetStringValue( section,key );
    if (val == "" || val =="false") {
      def = "false";
      GetValue( section,key,def,helptext );
      SetValue( section,key,def );
      return false;
    }
    else {
      def = "true";
      GetValue(section,key,def,helptext);
      SetValue( section,key,def );
      return true;
    }

  }
  else {
      def = "false";
      GetValue(section,key,def,helptext);
      return false;
   
  }


}


/**
 * Prints a warning if any keywords are specified that were never
 * registered. This is useful for catching typos!
 *
 * \returns true if any keywords were unused (so you can for example
 * stop the program)
 *
 * \note: if you have a "special" section, where the keywords are not
 * known in advance (e.g. a section used by EventSelector2), you can
 * call IgnoreKeywordCheckForSection("sectionname") to prevent false
 * errors from being reported.
 *
 */
bool Utilities::ConfigHandler::PrintUnusedKeywordWarning(bool pause_on_error) {

  vector<string>::iterator sect, key;
  vector<string> sections= GetListOfSections();  
  vector<string> unused;

  for (sect=sections.begin(); sect != sections.end(); sect++) {

    if (fNoCheckSection.count(*sect) && fNoCheckSection[*sect]) 
      continue;
    
    vector<string> keys= GetListOfKeysForSection( *sect );
    for (key=keys.begin(); key != keys.end(); key++) {

      if (IsRequired( *sect, *key ) == false) {
	unused.push_back( *sect+"/"+*key );
      }

    }
  }

  if (unused.size() > 0) {
    cout << "============================================================="<<endl;
    cout << " \033[01;31mWARNING [from ConfigHandler]: \033[0m" << endl;
      
    cout << "  Your config file or command line contained the following " << endl
	 << "  " << unused.size() << " keywords that were unknown: " << endl;

    for (size_t i=0; i<unused.size(); i++) {
      cout << "\t -> "<<unused[i]<< endl;
    }
    cout << "============================================================="<<endl; 

    if (pause_on_error) {
      for (int i=0; i<5; i++) {
	cerr << "continuing in "<< 5-i<<" seconds...                   \r";
	sleep(1);
      }
      cerr << "                                            \n";
    }

    return true;
  }
  else {
    return false;
  }

}


/**
 *  Print out a warning if there are deprecated keywords
 *
 * \returns true if any keywords were deprecated
 */
bool Utilities::ConfigHandler::PrintDeprecatedKeywordWarning() {

  vector<string>::iterator sect, key;
  vector<string> sections= GetListOfSections();  
  bool has_deprecated = false;

  for (sect=sections.begin(); sect != sections.end(); sect++) {
    
    vector<string> keys= GetListOfKeysForSection( *sect );
    for (key=keys.begin(); key != keys.end(); key++) {
      
      if (WarnIfDeprecated( *sect, *key ))
	has_deprecated = true;
     
    }
  }
  
  return has_deprecated;

}

/**
 * Handle the cases where programs should exit
 * with a help or error message.
 *
 * - Print help message and exit if user said --help
 *   or if argc <= 1 (this is useful so that users
 *   that don't know that they can print the help
 *   message with
 *   $ program --help
 *   get the help message when simply typing
 *   $ program
 * - Print warning messages and exit if the user
 *   chose unused or deprecated keywords
 *
 * This method is typically called by every program after setting
 * the config options, before proceeding to do its thing ...
 */
void Utilities::ConfigHandler::HandleHelpAndInvalidOptions
(int argc, std::string extra_message) {
  bool help = GetFlag("OPTIONS", "help", "Print help message");
  if( argc <= 1 || help) {
    PrintUsageMessage();
    std::cout << extra_message << std::endl;
    _exit(1);
  }

  bool badkeywords = false;
  badkeywords  = PrintUnusedKeywordWarning();
  badkeywords |= PrintDeprecatedKeywordWarning();
  if (badkeywords) {
    std::cout << "You gave invalid or deprecated options (see warnings above). "
      "Exiting now." << std::endl;
    _exit(1);
  }
}


/**
 * Mark a keyword as deprecated (meaning that the keyword is likely to
 * be removed in future versions). This should be done when you rename
 * or remove a key. A warning message will then appear telling the
 * user to update his or her config file.
 */
void Utilities::ConfigHandler::Deprecate(std::string section,std::string key, std::string reason) {
  fDeprecated[section][key] = reason;
}

/**
 * \returns true if specified key has been registered as deprecated.
 */
bool Utilities::ConfigHandler::IsDeprecated(string section, string key ) {
  if ( fDeprecated.count( section ) != 0 && 
       fDeprecated[section].count( key ) != 0)  {
    return true;
  }
  else
    return false;
}



/**
 * Print deprecated keyword warning
 */
bool Utilities::ConfigHandler::WarnIfDeprecated( std::string section,std::string key ) {

  bool ret = false;

  if ( (ret = IsDeprecated(section,key)) ) {

    std::ostringstream ost;

    ost << " \033[01;31mWARNING : \033[0m the config key '"
	<< section<<"/"<<key<<"' is DEPRECATED. "
	<< fDeprecated[section][key] << ". "
	<< "Please update your config file.";
    
    cout << "***************************************"
	 << "*******************************"<<endl;
    cout << wordWrap( ost.str(), "*   ",70 )  << endl;
    cout << "***************************************"
	 << "*******************************"<<endl;

  }


  return ret;

}
