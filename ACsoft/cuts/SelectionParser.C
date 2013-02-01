
#include "SelectionParser.hh"

#include "StringTools.hh"
#include "Selector.hh"
#include "CutFactory.hh"
#include "ConfigHandler.hh"

#include <algorithm>
#include <assert.h>

#define DEBUG 0
#include <debugging.hh>

ACsoft::Cuts::SelectionParser::SelectionParser( Utilities::ConfigHandler& handler )
{

  Parse(handler);

}

void ACsoft::Cuts::SelectionParser::Parse( Utilities::ConfigHandler& handler ) {

  handler.GetValue( "EventSelectors", "List", fListOfSelectorNames, "Names of event selectors to create (comma-separated list).");

  for (unsigned int i = 0; i < fListOfSelectorNames.size(); ++i) {

    std::string selTag = fListOfSelectorNames.at(i);

    std::vector<std::string> titleTokens;
    handler.GetValue( selTag, "Title", titleTokens, "Title of selector." );
    std::string title = join(titleTokens,std::string(" ") );
    DEBUG_OUT << "Title: " << title << std::endl;
    std::vector<std::string> listOfCutDescriptions;
    handler.GetValue( selTag, "Cuts", listOfCutDescriptions, "Comma-separated list of cuts in selector.");

    ::Cuts::Selector* selector = new ::Cuts::Selector(title);

    for( unsigned int cutCtr=0 ; cutCtr < listOfCutDescriptions.size() ; ++cutCtr ) {

      std::string cutDescription = listOfCutDescriptions.at(cutCtr);

      DEBUG_OUT << cutCtr << ": " << cutDescription << std::endl;

      ::Cuts::Cut* cut = ::Cuts::CutFactory::Self()->CreateCut(cutDescription);
      selector->RegisterCut(cut);
    }

    fSelectors.push_back(selector);
  }
}



/** Find an event selector created via the config file.
  *
  * \param[in] name Name of the selector, must be one of the values specified in the EventSelectors/List option.
  *
  */
Cuts::Selector* ACsoft::Cuts::SelectionParser::GetSelector( std::string name ) const {

  // fListOfSelectorNames and fSelectors will have a 1:1 relation after the call to Parse()

  std::vector<std::string>::const_iterator it = std::find(fListOfSelectorNames.begin(),fListOfSelectorNames.end(),name);
  if( it == fListOfSelectorNames.end() ){
    WARN_OUT << "Selector " << name << " does not exist! Please check your config file (EventSelectors/List option)!" << std::endl;
    throw std::runtime_error("Non-existing selector.");
  }

  unsigned int index = it-fListOfSelectorNames.begin();

  assert(index<fSelectors.size());

  return fSelectors.at(index);
}

