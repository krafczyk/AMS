// Last Edit 
//           May 24, 1996 ak. extracted from geant.C
//
#include <iostream.h>
#include <strstream.h>

class AMSEventList;

#include <typedefs.h>
#include <A_LMS.h>

implement (ooVArray, int);
implement (ooVArray, sci_data);

LMS	               dbout(oocUpdate);
ooHandle(AMSEventList) listH;

void lmsStart()
{
  char        eventID[256];
  ostrstream  oss;
  static  integer  iev = 3601;
  ooStatus rstatus = oocSuccess;

  //   rstatus = dbout.LMSInit(oocUpdate);

  // add event to the known list

    rstatus = dbout.AddList("rawdata",-1, listH);
    if (rstatus == oocSuccess) {
      rstatus = oocError;
      while (rstatus == oocError) {
       oss << ++iev << ends;
       strcpy(eventID,oss.str());
       cout << "lmsStart:: add "<<eventID<<" to the list"<<endl;
       rstatus = dbout.AddEvent("rawdata",iev,eventID);
      }
    }

  dbout.Interactive();
  cout <<"lmsStart : quit from Interactive"<<endl;
}
