#ifndef PALETTES_HH
#define PALETTES_HH

#include <TObject.h>

namespace Utilities {

class Palette : public TObject {

public:

  Palette() { }

  static void UseNicePaletteWithLessGreen();

  ClassDef(Utilities::Palette,0)
};

}


#endif // PALETTES_HH
