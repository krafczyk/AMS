//  $Id: deleteSetup.C,v 1.4 2001/01/22 17:32:29 choutko Exp $
// Last Edit 
//           
// Oct 22, 1996 ak. delete run and all associated events
//

#include <iostream.h>
#include <strstream.h>


#include <typedefs.h>
#include <ooSession.h>
#include <dbA.h>



ooMode               _openMode = oocUpdate;

implement (ooVArray, geant)   //;
implement (ooVArray, number)  //;
implement (ooVArray, integer) //;
implement (ooVArray, AMSTOFMCClusterD) //;
implement (ooVArray, AMSTrMCClusterD) //;

LMS                  dbout(oocUpdate);

int main()
{
 char*                  setupName = NULL;
 ooStatus               rstatus;


    setupName = dbout.PromptForValue("Enter Setup Name: ");
 
    rstatus   = dbout.DeleteSetup(setupName);
}
