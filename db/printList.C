// Last Edit 
//           
// Oct 22, 1996 ak. delete run and all associated events
//
// Last Edit : Oct 31, 1996. use listname as a parameter
//
#include <iostream.h>
#include <strstream.h>


#include <typedefs.h>
#include <ooSession.h>
#include <A_LMS.h>



ooMode               _openMode = oocRead;

implement (ooVArray, geant)   //;
implement (ooVArray, number)  //;
implement (ooVArray, integer) //;
implement (ooVArray, AMSTOFMCClusterD) //;
implement (ooVArray, AMSTrMCClusterD) //;

LMS                  dbout(oocRead);

int main(int argc, char** argv)
{
 ooStatus               rstatus;
 char*                  listName = NULL;
 char*                  printMode = "S";

 cout<<" "<<endl;
 if(argc > 1) {
  listName = argv[2];
  printMode = argv[1];
  if (argc == 3) dbout.PrintList(listName, _openMode, printMode);
  if (argc == 2) dbout.PrintList(_openMode, printMode);
 } else {
  dbout.PrintList(_openMode, printMode);
 } 
}
