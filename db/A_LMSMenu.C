//
// Menu program is stolen from Objectivity labs
//
// Oct 02, 1996. ak. remove obsolete calls
//
// Last edit Oct 02, 1996. ak.
//

#include <stdio.h>
#include <string.h>
#include <iostream.h>
#include <strstream.h>

#include <typedefs.h>
#include <A_LMS.h>


void	LMS::Interactive()
{
	// Initialize Current List

	fListName = PromptForValue("Enter List Name: ");

	// Display the Top Level Menu

	DisplayTopMenu();
}

void	LMS::SetCurrentListPrompts()
{
	char*	listName = NULL;
	
	listName = PromptForValue("(SET LIST) Enter List Name: ");

	// Set the current List

	SetCurrentList(listName);

	// Delete prompt strings
	
	if (listName) delete listName;
	
	// Return the the Top Menu
	
	DisplayTopMenu();
}

void	LMS::DisplayTopMenu()
{
	// Display Top Menu
	
	int	needAnswer = 1;
	char	buffer[256];
	
	while (needAnswer) {
	
		cout << endl;
		cout << "--------------------------------" << endl;

		if (fListName)
		   cout << "Context List: " << fListName<< endl;
		else
		   cout << "Context List: <not specified>" << endl;

		cout << "--------------------------------" << endl;
		cout << "Top  | [S]et Context List" << endl;
		cout << "Menu |--------------------------" << endl;
		cout << "     | [A]dd Objects" << endl;
		cout << "     | [D]elete Objects" << endl;
		cout << "     | [F]ill Objects" << endl;
		cout << "     |--------------------------" << endl;
		cout << "     | [Q]uit the LMS" << endl;
		cout << "--------------------------------" << endl;
		cout << endl;
		cout << "(TOP) Enter an action: ";
	
		// Prompt for action
		
		cin.getline(buffer, 256);
		needAnswer = 0;
		
		switch (buffer[0]) {
		
		case 's':
		case 'S':	// Set Context List
				SetCurrentListPrompts();
				break;

		case 'a':
		case 'A':	// Add Objects
				DisplayAddMenu();
				break;

		case 'f':
		case 'F':	// Fill AMS banks 
				DisplayFillMenu();
				break;
		case 'd':
		case 'D':	// Delete Objects
				DisplayDeleteMenu();
				break;
					
		case 'q':
		case 'Q':	// Quit LMS
				break;
					
		default:	// Invalid option
				cout << "Invalid Option -- Type in a valid response." << endl;
				needAnswer = 1;
		}

	}
}

void	LMS::DisplayAddMenu()
{
	// Display Add Menu
	
	int	needAnswer = 1;
	char	buffer[256];
	
	while (needAnswer) {
	
		cout << endl;
		cout << "--------------------------------" << endl;

		if (fListName)
		   cout << "Context List: " << fListName << endl;
		else
		   cout << "Context List: <not specified>" << endl;

		cout << "--------------------------------" << endl;
		cout << "Add  | [L]ist" << endl;
		cout << "Menu | [E]vent" << endl;
		cout << "     | M[C]event" << endl;
		cout << "     | MCeve[N]tg" << endl;
		cout << "     | [G]eometry" << endl;
		cout << "     | [M]aterials" << endl;
		cout << "     | TMe[d]ia" << endl;
		cout << "     |--------------------------" << endl;
		cout << "     | [T]op-level Menu" << endl;
		cout << "     | [Q]uit the LMS" << endl;
		cout << "--------------------------------" << endl;
		cout << endl;
		cout << "(ADD) Enter an object to add: ";
	
		// Prompt for action
		
		cin.getline(buffer, 256);
		needAnswer = 0;
		
		switch (buffer[0]) {
		
		case 'l':
		case 'L':	// Add List
				AddListPrompts();
				break;
					
		case 'e':
		case 'E':	// Add Event
				AddEventPrompts(1000);
				break;
		case 'c':
		case 'C':	// Add Event
				AddEventPrompts(100);
				break;

		case 'n':
		case 'N':	// Add Event
				AddEventPrompts(10);
				break;

		case 'g':
		case 'G':	// Add Geometry
				AddGeometryPrompts();
				break;

		case 'm':
		case 'M':	// Add Materials
				AddMaterialPrompts();
				break;
					
		case 'd':
		case 'D':	// Add TMedia
				AddTMediaPrompts();
				break;

		case 't':
		case 'T':	// Return to the top level menu
				DisplayTopMenu();
				break;
					
		case 'q':
		case 'Q':	// Quit LMS
				break;
					
		default:	// Invalid option
				cout << "Invalid Option -- Type in a valid response." << endl;
				needAnswer = 1;
		}

	}
}
	
void	LMS::DisplayDeleteMenu()
{
	// Display Delete Menu
	
	int	needAnswer = 1;
	char	buffer[256];
	
	while (needAnswer) {
	
		cout << endl;
		cout << "--------------------------------" << endl;

		if (fListName)
		   cout << "Context List: " << fListName << endl;
		else
		   cout << "Context List: <not specified>" << endl;

		cout << "--------------------------------" << endl;
		cout << "Delete | [L]ist" << endl;
		cout << "Menu   | [E]vent" << endl;
		cout << "       |------------------------" << endl;
		cout << "       | [T]op-level Menu" << endl;
		cout << "       | [Q]uit the LMS" << endl;
		cout << "--------------------------------" << endl;
		cout << endl;
		cout << "(DELETE) Enter an object type to delete: ";
	
		// Prompt for action
		
		cin.getline(buffer, 256);
		needAnswer = 0;
		
		switch (buffer[0]) {
		
		case 'l':
		case 'L':	// Delete List
				break;
					
		case 'e':
		case 'E':	// Delete Event
				DeleteEventPrompts();
				break;
					
		case 't':
		case 'T':	// Return to top-level menu
				DisplayTopMenu();
				break;
					
		case 'q':
		case 'Q':	// Quit LMS
				break;
					
		default:	// Invalid option
				cout << "Invalid Option -- Type in a valid response." << endl;
				needAnswer = 1;
		}

	}
}

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
	if (returnString)
		strcpy(returnString, buffer);

	return returnString;
}

void	LMS::AddListPrompts()
{
	char*	 listName = NULL;
	char*    setupName = NULL;

	listName  = PromptForValue("(ADD LIST) Enter List Name: ");
	setupName = PromptForValue("(ADD LIST) Enter Setup Name: ");

	// Create a List

        ooHandle(AMSEventList) listH;
	if (AddList(listName, setupName, 0, listH)) {
		cout << "List '" << listName << "' is created." << endl;
		SetCurrentList(listName);
	} else {
		cout << "List '" << listName << "' is NOT created." << endl;
	}
	
	// Delete prompt strings
	
	if (listName) delete listName;
	
	// Return the the Add Menu
	
	DisplayAddMenu();
}

void	LMS::AddEventPrompts(integer N)
{
        char*   eventName = NULL;
        char*   runName   = NULL;
        integer event, run;
	
	
	runName = PromptForValue("(ADD EVENT) Enter Run Number: ");
        istrstream stat(runName);
        stat >> run;
	eventName = PromptForValue("(ADD EVENT) Enter Event Number: ");
        istrstream stat1(eventName);
        stat1 >> event;

	if (AddEvent(fListName, run, event, eventName, -2, N)) {
          cout << "Event " << event << " of run "<<run<<" is created." << endl;
	} else {
		cout << "Event '" << event << "' is NOT created." << endl;
	}

	// Delete prompt strings
	
	if (eventName) delete eventName;
	
	// Return the the Add Menu
	
	DisplayAddMenu();
}

void	LMS::AddGeometryPrompts()
{
	
        cout <<"LMS::AddGeometryPrompts: Add Geometry will be called"<<endl;
	if (AddGeometry(fListName)) cout << "Geometry is created." << endl;

	// Return the the Add Menu
	DisplayAddMenu();
}

void	LMS::AddMaterialPrompts()
{
	
        cout <<"LMS::AddMaterialPrompts: Add Material will be called"<<endl;
	if (AddMaterial(fListName)) cout << "Materials are added." << endl;

	// Return the the Add Menu
	DisplayAddMenu();
}

void	LMS::AddTMediaPrompts()
{
	
        cout <<"LMS::AddTMediaPrompts: Add TMedia will be called"<<endl;
	if (AddTMedia(fListName)) cout << "TMedia  added." << endl;

	// Return the the Add Menu
	DisplayAddMenu();
}

	
void	LMS::DeleteEventPrompts()
{
        char* eventID = NULL;

        eventID = PromptForValue("(DELETE Event) Enter Event ID: ");

	// Delete the Event

	DeleteEvent(fListName, eventID);

	if (eventID) delete eventID;
	
	// Return the the Delete Menu
	
	DisplayDeleteMenu();
}

void	LMS::DisplayFillMenu()
{
	// Display Fill Menu
	
	int	needAnswer = 1;
	char	buffer[256];
	
	while (needAnswer) {
	
		cout << endl;
		cout << "--------------------------------" << endl;

		if (fListName)
		   cout << "Context List: " << fListName << endl;
		else
		   cout << "Context List: <not specified>" << endl;

		cout << "--------------------------------" << endl;
		cout << "Fill | AMS[G]eometry" << endl;
		cout << "Menu | AMS[M]aterial" << endl;
		cout << "     | AMSTMe[D]ia" << endl;
		cout << "     |            "<< endl;
		cout << "     | AMS[E]vent" << endl;
		cout << "     | AMSMC[B]anks" << endl;
		cout << "     | AMSMCeve[N]tg" << endl;
		cout << "     |--------------------------" << endl;
		cout << "     | [T]op-level Menu" << endl;
		cout << "     | [Q]uit the LMS" << endl;
		cout << "--------------------------------" << endl;
		cout << endl;
		cout << "(ADD) Enter an object to add: ";
	
		// Prompt for action
		
		cin.getline(buffer, 256);
		needAnswer = 0;
		
		switch (buffer[0]) {
		
					
		case 't':
		case 'T':	// Return to the top level menu
				DisplayTopMenu();
				break;
					
                case 'e':
                case 'E':       FillEventPrompts(1000);
                                break;

                case 'b':
                case 'B':       FillEventPrompts(100);
                                break;

                case 'n':
                case 'N':       FillEventPrompts(10);
                                break;

                case 'g':
                case 'G':       FillGeometryPrompts();
                                break;
                case 'm':
                case 'M':       FillMaterialPrompts();
                                break;
                case 'd':
                case 'D':       FillTMediaPrompts();
                                break;
		case 'q':
		case 'Q':	// Quit LMS
				break;
					
		default:	// Invalid option
				cout << "Invalid Option -- Type in a valid response." << endl;
				needAnswer = 1;
		}

	}
}

void	LMS::FillGeometryPrompts()
{
        
        ooStatus rstatus = FillGeometry(fListName);

	// Return the the Fill Menu
	
	DisplayFillMenu();
}

void	LMS::FillMaterialPrompts()
{
        
        ooStatus rstatus = FillMaterial(fListName);

	// Return the the Fill Menu
	
	DisplayFillMenu();
}

void	LMS::FillTMediaPrompts()
{
        
        ooStatus rstatus = FillTMedia(fListName);

	// Return the the Fill Menu
	
	DisplayFillMenu();
}


void	LMS::FillEventPrompts(integer N)
{
        char*   eventID = NULL;
	ooStatus rstatus;
	
	eventID = PromptForValue("(Fill AMSEvent) Enter Event ID: ");


        rstatus = GetEvent(fListName, eventID, 0, 0, oocRead, -2, N);

	// Delete prompt strings
	
	if (eventID) delete eventID;
	
	// Return the the Fill Menu
	
	DisplayFillMenu();
}

