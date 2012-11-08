#include "VectorCapacityTracker.h"

namespace AC {

void VectorCapacityTracker::reportCapacityExceededWarnings() {

  std::map<std::string, std::pair<size_t*, size_t> >* map = capacityTypesMap();
  if (map->empty())
    return;

  std::cerr << std::endl;
  std::cerr << "******************************************************************************************************" << std::endl;
  std::cerr << "Several vectors exceeded their inline capacities and had to reallocate, which hurts performance:" << std::endl;
  std::cerr << "If you see this warning, please contact Niko, to let him tune the parameters!" << std::endl << std::endl;

  std::map<std::string, std::pair<size_t*, size_t> >::iterator it = map->begin();
  for (std::map<std::string, std::pair<size_t*, size_t> >::iterator end = map->end(); it != end; ++it)
    std::cerr << "  " << it->first << " => inline capacity " << std::setw(3) << it->second.second << " should be raised to at least " << std::setw(3) << *it->second.first << std::endl << std::endl;

  std::cerr << "******************************************************************************************************" << std::endl;
}

}
