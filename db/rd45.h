//  $Id: rd45.h,v 1.4 2001/01/22 17:32:30 choutko Exp $
#ifndef __RD45SUBS__
#define __RD45SUBS__

//
// some basic types used in rd45 programms
// D.Duellmann
//
// $Id: rd45.h,v 1.4 2001/01/22 17:32:30 choutko Exp $
//
// add AMS related functions .  A.Klimentov May, 1997.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>
#include <strstream.h>

#include <sys/times.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>



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

inline char *StrCat3(const char *str1, const char* str2, const char* str3)
{
  char *nstr;
  nstr = new char[strlen(str1) + strlen(str2)+ strlen(str3) + 1];
  strcpy(nstr,str1);
  strcat(nstr,str2);
  strcat(nstr,str3);
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

inline int UnixFilestat(const char *path, long *id, long *size,
                              long *flags, time_t *modtime)
{
   // Get info about a file: id, size, flags, modification time.
   // Id      is (statbuf.st_dev << 24) + statbuf.st_ino
   // Size    is the file size
   // Flags   is file type: bit 0 set executable, bit 1 set directory,
   //                       bit 2 set regular file
   // Modtime is modification time
   // The function returns 0 in case of success and 1 if the file could
   // not be stat'ed.
   //
   struct stat statbuf;

   if (path != 0 && stat(path, &statbuf) >= 0) {
      if (id)
         *id = (statbuf.st_dev << 24) + statbuf.st_ino;
      if (size)
         *size = statbuf.st_size;
      if (modtime)
         *modtime = statbuf.st_mtime;
      if (flags) {
         *flags = 0;
         if (statbuf.st_mode & ((S_IEXEC)|(S_IEXEC>>3)|(S_IEXEC>>6)))
            *flags |= 1;
         if ((statbuf.st_mode & S_IFMT) == S_IFDIR)
            *flags |= 2;
         if ((statbuf.st_mode & S_IFMT) != S_IFREG &&
             (statbuf.st_mode & S_IFMT) != S_IFDIR)
            *flags |= 4;
      }
      return 0;
   }
   return 1;
}



#endif
