#ifndef  ConfigParser_H
#define  ConfigParser_H



#include <sstream>
#include <string>
#include <map>
#include <fstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/** text config parser
 *  example in text file: 
 *  [SECTION]
 *  paramatername=value
 *  access:
 *  ConfigParser* theconf;
 *  type value=theconf->Value( "SECTION", "parametername");
 *  type can be double, float, int, string
 *
 *  3.07.2012-  VZ 
*/

class Chameleon {

  public:
Chameleon() {}
Chameleon(std::string const& value) {
  value_=value;
}


Chameleon(const char* c) {
  value_=c;
}

Chameleon(double d) {
  std::stringstream s;
  s<<d;
  value_=s.str();
}

Chameleon(Chameleon const& other) {
  value_=other.value_;
}

Chameleon& operator=(Chameleon const& other) {
  value_=other.value_;
  return *this;
}

Chameleon& operator=(double i) {
  std::stringstream s;
  s << i;
  value_ = s.str();
  return *this;
}

Chameleon& operator=(std::string const& s) {
  value_=s;
  return *this;
}

operator std::string() const {
  return value_;
}

operator double() const {
  return atof(value_.c_str());
}

private:
  std::string value_;
};



//---trim function

//---class
class ConfigParser {
  std::map<std::string,Chameleon> content_;

public:
  ConfigParser(std::string const& configFile);

  Chameleon const& Value(std::string const& section, std::string const& entry) const;
  Chameleon const& Value(std::string const& section, std::string const& entry, double value);
  Chameleon const& Value(std::string const& section, std::string const& entry, std::string const& value);

  
  std::string trim(std::string const& source, char const* delims = " \t\r\n"){
  std::string result(source);
  std::string::size_type index = result.find_last_not_of(delims);
  if(index != std::string::npos)
    result.erase(++index);

  index = result.find_first_not_of(delims);
  if(index != std::string::npos)
    result.erase(0, index);
  else
    result.erase();
  return result;
}

  int Debug;

};


#endif
