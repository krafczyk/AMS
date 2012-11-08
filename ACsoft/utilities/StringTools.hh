#ifndef UTILITIES_STRINGTOOLS
#define UTILITIES_STRINGTOOLS

#include <vector>
#include <string>
#include <set>
#include <sstream>
#include <bitset>
#include <algorithm>

std::string str2regstr(const std::string& s);
std::string regstr2str(const std::string& s);

std::vector<std::string> split(const std::string &s, 
			       const std::string& delimiters = "\t ");

std::vector<float> split_to_float(const std::string &s, 
				  const std::string delimiters="\t ");
std::set<std::string> unique_split(const std::string &s, char c);

std::string strip(const std::string &);
std::string stoupper(const std::string &);
std::string stolower(const std::string &);

std::string wordWrap(const std::string &text, const std::string &prefix="",int width=80 );

// The following could be replaced by calls to the newer versions fo
// split, split_to_floatbut are left in for backward compatibility:

std::vector<std::string> split(const std::string &s, char c);
std::vector<float> split_to_float(const std::string &s, char c) ;

std::string StripCharacters(const std::string &instring,const std::string &chars_to_remove );

void RemoveWhiteSpace( std::string & str );


float ValueFromString(const std::string &str, const std::string &valueName, bool& success );
std::string StringFromString(const std::string &str,const std::string &valueName, bool& success );



/**
 * Join a vector back into a string, separated by specified delimiter
 */
template <class T> 
std::string join(const std::vector<T> &vec,const std::string &delimiter ) {
  
  std::ostringstream stream;
  typename std::vector<T>::const_iterator it;
  it = vec.begin();

  stream << *it;
  it++;
  for (; it!=vec.end(); it++) {
    stream << delimiter << *it;
  }
  
  return stream.str();

}

#endif
