// Author V. Choutko 24-may-1996
// 
// Feb 13, 1997. ak. First try with Objectivity
//                   Add _id and _name. 
// May 12, 1997. ak. CRC and data are uinteger instead on integer;
//
// Last Edit : Sep 17, 1997. ak.
// 

#include <typedefs.h>
#include <time.h>
#include <timeid.h>

declare (ooVArray, uinteger);

class AMSTimeIDD: public ooObj {
protected:

integer  _id;        // get from AMSID
char     _name[80];  // get from AMSID

time_t   _Insert;    // insert time
time_t   _Begin;     // validity starts
time_t   _End;       // validity ends
uinteger _CRC;       // Control Sum
integer  _UpdateMe;
 
ooVArray(uinteger) _pData; // data

public:
AMSTimeIDD() {};
AMSTimeIDD::AMSTimeIDD(char* name, integer id, AMSTimeID* p);
AMSTimeIDD(char* name, integer id, time_t begin, time_t insert, time_t end);

inline integer getid()          {return _id;}
       char*   getname()        {return _name;}
uinteger       getCRC() const   {return _CRC;}
integer getsize()               { return _pData.size();}

void SetTime(time_t insert, time_t begin, time_t end);
void GetTime(time_t & insert, time_t & begin, time_t & end);
void PrintTime();
void update(AMSTimeID* p);
void copy(uinteger* tmp);
};

