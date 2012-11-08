#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <set>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <unistd.h>
#include <stdexcept>
#include <bitset>

#include "StringTools.hh"


/** expand string to "^string$" for absolute regex matching
 *
 * \param s - the string to expand
 *
 * \returns expanded string
 */
std::string str2regstr (const std::string& s)
{
  std::ostringstream ost;
  if(s.find("^") == std::string::npos)
    ost << "^";
  ost << s;
  if(s.find("$") == std::string::npos)
    ost << "$";
  return ost.str();
}

/** convert "^string$" to "string"
 *
 * \param s - the string to convert
 *
 * \returns converted string
 */
std::string regstr2str (const std::string& s)
{
  size_t begin = s.find("^");
  size_t end   = s.find("$");

  return s.substr( begin+1, end-begin-1);
}


/** Split a string using the character \b c as separator
 * \return a vector of substrings 
 * \deprecated - see split( string, string)
 */
std::vector<std::string> split(const std::string &s, char c)
{
  std::string::size_type begin = 0;
  std::string::size_type pos = 0;
  std::vector<std::string> result;
  if(s.size() == 0) return result;
  do {
    pos = s.find(c,begin);
    if(pos != std::string::npos) {
      result.push_back(s.substr(begin,pos - begin));
      begin = pos+1;
    }
  } while(pos!= std::string::npos);
  result.push_back(s.substr(begin));      
  return result;
}

/*
 * Given a string like "value1=x,value2=y", ValueFromString(string,"value2") returns y
 */
float ValueFromString(const std::string &str, const std::string &valueName, bool& success ) {

  bool ok = false;
  std::string val = StringFromString( str, valueName, ok );

  if( ok ){
    float v = atof(val.c_str());
    success = true;
    return v;
  }

  success = false;
  return 0.0;
}


/*
 * Given a string like "value1=x,value2=y", StringFromString(string,"value2") returns y
 */
std::string StringFromString(const std::string &str,const std::string &valueName, bool& success ) {

  std::vector<std::string> items = split(str,',');
  for( unsigned int i = 0 ; i<items.size() ; ++i ){
    std::vector<std::string> pair = split(items.at(i),'=');

    if( pair.size() != 2 )
      continue;

    if( pair.front() == valueName ){
      std::string val = pair.back();
      success = true;
      return val;
    }
  }

  success = false;

  return std::string("");
}

/**
 * Splits a string into a vector of tokens, skipping over delimiter
 * characters (which are user definable). Whitespace is treated
 * correctly - if there are multiple whitespace characters, they are
 * treated as one.
 * 
 * \param str - the string to split
 *
 * \param delimiters - a string of characters to use as whitespace
 * (e.g. ", \t" for comma, space, and tab)
 * 
 * \returns a vector of strings
 *
 */ 
std::vector<std::string> split(const std::string& str, const std::string& delimiters) {

  std::vector<std::string> tokens; 

  std::string::size_type lastPos = str.find_first_not_of( delimiters, 0 );
  std::string::size_type pos     = str.find_first_of( delimiters, lastPos );
  
  while (std::string::npos != pos || std::string::npos != lastPos) {
    tokens.push_back( str.substr( lastPos, pos - lastPos ) );
    lastPos = str.find_first_not_of( delimiters, pos );
    pos     = str.find_first_of( delimiters, lastPos );
  }

  return tokens;

}


/**
 * Split a string into float values and return a vector of floats
 */
std::vector<float>
split_to_float( const std::string &str, const std::string &delimiters ) {
  
  std::vector<std::string> tokens = split( str, delimiters );
  std::vector<float> values;
  std::vector<std::string>::iterator it;
  float f;

//   values.resize( tokens.size() ); // BUGFIX

  for (it = tokens.begin(); it != tokens.end(); it++) {
    
    std::stringstream ss( *it );
    ss >> f;
    values.push_back(f);

  }

  return values;

}




/** Split a string using the character \b c as separator,
 * convert to floats and \return a vector of floats
 * \deprecated - see split_to_float( string, string)
 */
std::vector<float> split_to_float(const std::string &s, char c)
{
  std::string::size_type begin = 0;
  std::string::size_type pos = 0;
  std::vector<std::string> result;
  std::vector<float> ret;
  if(s.size() == 0) return ret;
  do {
    pos = s.find(c,begin);
    if(pos != std::string::npos) {
      result.push_back(s.substr(begin,pos - begin));
      begin = pos+1;
    }
  } while(pos!= std::string::npos);
  result.push_back(s.substr(begin));      
  for (std::vector<std::string>::iterator it = result.begin();
       it != result.end(); ++it) {
    std::stringstream stream;
    stream << *it;
    double value = 0;
    stream >> value;
    ret.push_back(value);
  }
  return ret;
}

/* holds only unique strings */
std::set<std::string> unique_split(const std::string &s, char c)
{
  std::string::size_type begin = 0;
  std::string::size_type pos = 0;
  std::set<std::string> result;
  if(s.size() == 0) return result;
  do {
    pos = s.find(c,begin);
    if(pos != std::string::npos) {
      result.insert(s.substr(begin,pos - begin));
      begin = pos+1;
    }
  } while(pos!= std::string::npos);
  result.insert(s.substr(begin));      
  return result;
}

/** Remove the trailing and tailing blanks
 * from the string \b s */
std::string strip(const std::string & s)
{
  std::string::size_type begin = s.find_first_not_of(" \t\r\n");
  std::string::size_type end = s.find_last_not_of(" \t\r\n");
  if(begin == std::string::npos) return std::string(); // empty
  return s.substr(begin,end-begin+1);
}

/** Remove the trailing and tailing blanks
 * from the string \b s */
std::string stoupper(const std::string & s)
{
  std::string r = s;
  std::transform(r.begin(),r.end(),r.begin(),::toupper);
  return r;
}

/** Remove the trailing and tailing blanks
 * from the string \b s */
std::string stolower(const std::string & s)
{
  std::string r = s;
  std::transform(r.begin(),r.end(),r.begin(),::tolower);
  return r;
}

/**
 * Word-wrap a long string of text for nicer screen formatting. This
 * doesn't actually "split" the string into lines - it justs puts
 * newlines in at the right places, but that can be done as well by
 * writing for example:
 * 
 * \code
 *   vector<string> lines = split( wordWrap( mystring, "\n" ) )
 * \endcode
 * 
 * \param text text string to wrap
 * \param width max number of columns you want in the text block (default 80)
 * \param prefix optional prefix string (for indentation) applied to each line
 * \returns word-wrapped string.
 */
std::string
wordWrap( const std::string &text,const std::string &prefix, int width ) {

  int numchars;
  std::string wrappedtext = prefix;
  
  std::vector<std::string> tokens = split(text, " \n");
  
  numchars = 0;
  for (std::vector<std::string>::iterator token=tokens.begin();
       token != tokens.end(); token++) {
    
    numchars += token->length() +1;
    if (numchars >= width) {
      wrappedtext += "\n" + prefix;
      numchars = token->length()+1;
    }
    
    wrappedtext += *token + " ";
    
  }
  
  return wrappedtext;
  
}



/**
 * Returns a copy of instring, but without the characters listed in chars_to_remove
 */
std::string StripCharacters(const std::string &instring,const std::string &chars_to_remove ) {

  std::string stripped;

  for (size_t ii=0; ii<instring.size(); ii++){

    if (chars_to_remove.find( instring[ii] ) == std::string::npos)
      stripped.push_back( instring[ii] );

  }

  return stripped;
}


void RemoveWhiteSpace( std::string &str ) {

  // first convert tabs to spaces:
   
  std::string::iterator i;
   
  for( i=str.begin(); i != str.end(); i++) {
    if (*i == '\t') *i = ' ';
  }
   
  // now chop off white space at beginning and end
   
  std::string::size_type where = str.find_first_not_of(' ');
   
  if (where != std::string::npos && where != 0)
    str = str.substr(where);
   
  where = str.find_last_not_of(' ');
   
  if (where != std::string::npos && where != (str.length() - 1))
    str = str.substr(0, where + 1);
   
  if (str==" ") str = "";

}


