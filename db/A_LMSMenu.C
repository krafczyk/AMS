//
// Menu program is stolen from Objectivity labs
//
// Oct 02, 1996. ak. remove obsolete calls
//
// Last edit Mar 27, 1996. ak.
//

#include <stdio.h>
#include <string.h>
#include <iostream.h>
#include <strstream.h>

#include <typedefs.h>
#include <A_LMS.h>


char*   LMS::PromptForValue(char* promptString)
{
	char    buffer[256];
	char*   returnString;   // Caller responsible for ::delete

	cout << promptString;
	cin.getline(buffer, 256);
        
        for (integer i=255; i>0; i--) {
         if (buffer[i] == ' ') buffer[i] = '\0';
         else break;
        }

	returnString = new char[strlen(buffer) + 1];
	if (returnString) strcpy(returnString, buffer);
	return returnString;
}

