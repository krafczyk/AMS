#include <iostream.h>
#include <LMS.h>

int main(int argc, char** argv)
{
	//////////////////////////////////////////////////////////////////
	//
	// Process the command line:
	//	- No arguments -> Interactive Menu
	//	- One argument -> Batch Operation
	//
	//////////////////////////////////////////////////////////////////

	int	batchFlag;
	char*	batchFile;

	if (argc == 2) {
	   batchFlag = 1;
	   batchFile = argv[1];
	} else if (argc == 1) {
	   batchFlag = 0;
	} else {
	   cerr << "Usage: " << argv[0] << " [batchFile]" << endl;
	   return 0;
	}

	//////////////////////////////////////////////////////////////////
	//
	// Create a LMS Session, Initialize Objectivity, Display the Top Menu
	//
	//////////////////////////////////////////////////////////////////

	LMS	session;

	ooInit();

	if (batchFlag)
	   session.Batch(batchFile);
	else
	   session.Interactive();

	//////////////////////////////////////////////////////////////////
	//
	// Return
	//
	//////////////////////////////////////////////////////////////////

	return 0;
}
