#ifndef __RD45SUBS__
#define __RD45SUBS__

//
// some basic types used in rd45 programms
// D.Duellmann
//
// $Id: rd45.h,v 1.1 1997/05/21 17:33:54 alexei Exp $
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>
#include <strstream.h>

#include <sys/times.h>
#include <unistd.h>

inline char *StrDup(const char *str)
{
  char *nstr;
  strcpy(nstr = new char [strlen(str)+1],str);
  return nstr;
}

inline char *StrCat(const char *str, const char* prefix)
{
  char *nstr;
  nstr = new char[strlen(str) + strlen(prefix)+1];
  strcpy(nstr,str);
  strcat(nstr,prefix);
  return nstr;
}

inline int BitSet(int source, int check)
{
  return (source/check)%2;
}

inline void Message(const char *mesg)
{            
    cerr <<"Info: " << mesg << endl;
}  
inline void Warning(const char *mesg)
{            
    cerr <<"WARNING: " << mesg << endl;
}  

inline void Error(const char *mesg)
{            
    cerr <<"Error: " << mesg << endl;
}  


inline void Fatal(const char *mesg)
  {
    cerr <<"FATAL: " << mesg << endl;
    exit(-1);
  }    

inline char   *PromptForValue(char* promptString)
{
	char    buffer[256];
	char*   returnString;   // Caller responsible for ::delete

	cout << promptString;
	cin.getline(buffer, 256);
        
        for (int i=255; i>0; i--) {
         if (buffer[i] == ' ') buffer[i] = '\0';
         else break;
        }

	returnString = new char[strlen(buffer) + 1];
	if (returnString) strcpy(returnString, buffer);
	return returnString;
}



#endif
