#include "ConfigParser.h"

using namespace std;
ConfigParser::ConfigParser(std::string const& configFile) {
  std::ifstream file(configFile.c_str());
  if(!file)  {std::cout<<" ***config file not found "<<configFile<<std::endl;}
    std::cout<<" Parser read "<<configFile<<std::endl;

  std::string line;
  std::string name;
  std::string value;
  std::string inSection;
  int posEqual;
  while (std::getline(file,line)) {

    if (! line.length()) continue;

    if (line[0] == '#') continue;
    if (line[0] == ';') continue;

    if (line[0] == '[') {
      inSection=trim(line.substr(1,line.find(']')-1));
      continue;
    }

    posEqual=line.find('=');
    name  = trim(line.substr(0,posEqual));
    value = trim(line.substr(posEqual+1));

    content_[inSection+'/'+name]=Chameleon(value);
  }

  Debug=this->Value("general","Debug");
  cout<<"Config: Debug "<<Debug<<endl;

}
//
Chameleon const& ConfigParser::Value(std::string const& section, std::string const& entry) const{
  if(Debug>1)  std::cout<<" config "<<section<<" "<<entry<<std::endl;
  std::map<std::string,Chameleon>::const_iterator ci = content_.find(section + '/' + entry);
  if(Debug>1) cout<<" "<<ci->first<<" "<<ci->second<<endl;
  if (ci == content_.end()) {  std::cout<<"******no parameter: "<<entry<<" in config section: "<<section<<std::endl; 
    throw "no";}
  if(Debug>1)  std::cout<<" found "<<entry<<endl;
  return ci->second;
}


Chameleon const& ConfigParser::Value(std::string const& section, std::string const& entry, std::string const& value) {
  try {
    return Value(section, entry);
  } catch(const char *) {
    return content_.insert(std::make_pair(section+'/'+entry, Chameleon(value))).first->second;
  }
}

