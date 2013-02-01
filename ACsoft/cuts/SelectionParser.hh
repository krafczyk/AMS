#ifndef SelectionParser_hh
#define SelectionParser_hh

#include <string>
#include <vector>

namespace Cuts {
  class Selector;
}

namespace ACsoft {

namespace Utilities {
class ConfigHandler;
}

namespace Cuts {

/** Parses the sections of a config file
  * related to the creation of Cuts and EventSelectors.
  */
class SelectionParser {

public:

  SelectionParser( Utilities::ConfigHandler& handler );

  ::Cuts::Selector* GetSelector( std::string name ) const;

private:

  void Parse( Utilities::ConfigHandler& handler );

private:

  std::vector<std::string> fListOfSelectorNames;

  typedef ::Cuts::Selector SelectorType;
  std::vector<SelectorType*> fSelectors;

};

}

}

#endif