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

LMS                  dbout(oocRead);

int main(int argc, char** argv)
{
 ooStatus               rstatus;
 char*                  listName = NULL;

 if(argc == 2) {
  listName = argv[1];
  dbout.PrintList(listName, _openMode);
 } else {
  dbout.PrintList(_openMode);
 } 
}
