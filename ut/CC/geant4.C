#include <geant4.h>
using namespace g4ams;
void g4ams::G4INIT(){
 
new G4RunManager();

// Here user class/action initialization needed
// ...
//

 G4RunManager::GetRunManager()->Initialize();

}
void g4ams::G4RUN(){
 G4RunManager::GetRunManager()->BeamOn(GCFLAG.NEVENT);
}


void g4ams::G4LAST(){
delete  G4RunManager::GetRunManager();
}


