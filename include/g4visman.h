#ifndef __AMSG4VISMAN__
#define __AMSG4VISMAN__
#ifdef G4VIS_USE

#include "G4VisManager.hh"
 #include "G4UImanager.hh"
 #include "G4UIterminal.hh"


class AMSG4VisManager: public G4VisManager {

public:

  AMSG4VisManager (){};
  static void create();
  static void kill();
 private:
  static G4UIsession* _pses;
  static G4VisManager* _pman;

  void RegisterGraphicsSystems ();

};

#endif

#endif
