//
// MODULE NAME:		ooSession.C
// AUTHOR:		Objectivity Training
// DATE:		April 11, 1995
//
// COPYRIGHT:
//   Copyright 1995 Objectivity, Inc.  All rights reserved.
//
// SECURITY:
//   Objectivity, Inc. Object Code Only source materials -
//   Company Confidential.
//
// DESCRIPTION:
//   This header file contains the definitions for the
//   objectivity training database services class
//
// USAGE:
//
// DEPENDENCIES:
//
// UPDATES/MODIFICATIONS:
// Date     SPR   By  Description
//
// LIMITATIONS/KNOWN BUGS
//
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include "ooSession.h"

/////////////////////////////////////////////////////////////////////////////////
//
// Class Name:		ooSession
// Method Name:		Instance
//
// Description:		This method make a single instance of the ooSession class.
//
// Input Arguments:     look to Initialize
//
// Output Arguments:	None
//
// Return Value:	pointer to the unique instance
//
// Special Notes:	None
//
/////////////////////////////////////////////////////////////////////////////////

ooSession * ooSession::_instance = 0;    // initialize static _instance

ooSession * ooSession::Instance(const char* fdName, const uint32 nFiles,
			   const uint32 nPages, const uint32 nMaxPages)
{
  if ( _instance == 0 ) {
     _instance = new ooSession;
     _instance->Initialize(fdName, nFiles, nPages, nMaxPages);
   }
  return _instance;
}


/////////////////////////////////////////////////////////////////////////////////
//
// Class Name:		ooSession
// Method Name:		ooSession
//
// Description:		This method is the constructor for the ooSession class.
//			It initializes the private variables used by this class.
//
// Input Arguments:	None
//
// Output Arguments:	None
//
// Return Value:	None
//
// Special Notes:	None
//
/////////////////////////////////////////////////////////////////////////////////

ooSession::ooSession()
{
	_fdName = NULL;
	_fdMROWMode = oocMROWIfAllowed;	// oocMROWIfAllowed is used as "unitiailized"
	_nFiles = 0;
	_nPages = 0;
	_nMaxPages = 0;
	_databaseName = NULL;
	_transLevel = 0;
	_abortPending = oocFalse;
	_lockWait = oocTransNoWait;	// Uninitialized value
	_startTime = 0;
	_endTime = 0;
	_verbose = oocFalse;
}

/////////////////////////////////////////////////////////////////////////////////
//
// Class Name:		ooSession
// Method Name:		~ooSession
//
// Description:		This method is the destructor for the ooSession class.
//
// Input Arguments:	None
//
// Output Arguments:	None
//
// Return Value:	None
//
// Special Notes:	None
//
/////////////////////////////////////////////////////////////////////////////////

ooSession::~ooSession()
{
	if (_fdName) delete _fdName;
	if (_databaseName) delete _databaseName;
}

/////////////////////////////////////////////////////////////////////////////////
//
// Class Name:		ooSession
// Method Name:		Initialize
//
// Description:		This method initializes an Objectivity/DB session. It:
//
//			 - Reads in the appropriate environment variables (as overrides)
//			 - Calls ooInit() to initialize the cache (converting MB to
//				pages)
//			 - Initializes the federated database name (validating it)
//
// Input Arguments:	fdName (char*)	federated database path/file
//			nFiles (uint32)	max number of simultaneous file descriptors
//			nPages (uint32)	initial cache size in MB
//			nMaxPages (uint32)	maximum cache size in MB
//
// Output Arguments:	None
//
// Return Value:	ooStatus (oocSuccess/oocError)
//
// Special Notes:	None
//
/////////////////////////////////////////////////////////////////////////////////

ooStatus	ooSession::Initialize(const char* fdName, const uint32 nFiles,
			   const uint32 nPages, const uint32 nMaxPages)
{
	ooStatus	rstatus;

	// Read in environment variables
	ReadEnvironmentVars();

	// Initialize Objectivity/DB, using overriding values from the enviroment

	if (_nFiles == 0)
	   _nFiles = nFiles;

	if (_nPages == 0)
	   _nPages = nPages;

	if (_nMaxPages == 0)
	   _nMaxPages = nMaxPages;

	if (_verbose) {
	   cout << "VERBOSE: ooInit(" << _nFiles << "," << _nPages << ",";
	   cout << _nMaxPages << ")" << endl;
	}
	rstatus = ooInit(_nFiles, _nPages, _nMaxPages);
	if (rstatus != oocSuccess) {
	   cerr << "ERROR: ooInit() failed." << endl;
	   return rstatus;
	}

	// Initialize the federated database name, using overriding env value
	if (_fdName == NULL) {
	   _fdName = new char[strlen(fdName) + 1];
	   if (fdName == NULL) {
	      cerr << "ERROR: Memory allocation failure for '" << fdName << "'." << endl;
	      return oocError;
	   }
	   strcpy(_fdName, fdName);
	}

	// Start a transaction to create/open a default database
	rstatus = StartTransaction(oocUpdate);
	if (rstatus != oocSuccess) {
	   cerr << "ERROR: TransactionStart failed." << endl;
	   return rstatus;
	}

	// Commit the transaction and return
	rstatus = CommitTransaction();

	return rstatus;

}

/////////////////////////////////////////////////////////////////////////////////
//
// Class Name:		ooSession
// Method Name:		ReadEnvironmentVars
//
// Description:		This method reads in overriding environment settings for:
//
//			 - federated database name
//			 - cache parameters
//			 - MROW mode (on off)
//			 - Lock waiting
//
// Input Arguments:	None
//
// Output Arguments:	None
//
// Return Value:	None
//
// Special Notes:	None
//
/////////////////////////////////////////////////////////////////////////////////

void	ooSession::ReadEnvironmentVars()
{
	// Read in a value for verbose (echo) mode.

	char*	verbose;

	verbose = getenv("OO_VERBOSE");
	if (verbose) {
	   _verbose = oocTrue;
	}

	// Read in a value for the Federated Database Name

	char*	fdName;

	fdName = getenv("OO_FD_BOOT");
	if (fdName) {
	   _fdName = new char[strlen(fdName) + 1];
	   strcpy(_fdName, fdName);
	   if (_verbose) {
	      cout << "VERBOSE: setenv OO_FD_BOOT '" << _fdName << "'" << endl;
	   }
	}

	// Read in a value for the Default Database Name
	//      (default to "ooDefaultDB" if not specified)

	char*	dbName;

	dbName = getenv("OO_DB_NAME");
	if (dbName) {
	   _databaseName = new char[strlen(dbName) + 1];
	   strcpy(_databaseName, dbName);
	   if (_verbose) {
	      cout << "VERBOSE: setenv OO_DB_NAME '" << _databaseName << "'" << endl;
	   }
	} else {
	   _databaseName = new char[12];
	   strcpy(_databaseName, "ooDefaultDB");
	   if (_verbose) {
	      cout << "VERBOSE: Default database set to '" << _databaseName << "'." << endl;
	   }
	}

	// Read in a value for the Number of Concurrent File Descriptors

	char*	value;

	value = getenv("OO_N_FILES");
	if (value) {
	   _nFiles = atoi(value);
	   if (_verbose) {
	      cout << "VERBOSE: setenv OO_N_FILES '" << _nFiles << "'" << endl;
	   }
	}

	// Read in a value for the Initial Cache Size (in pages)

	value = getenv("OO_N_PAGES");
	if (value) {
	   _nPages = atoi(value);
	   if (_verbose) {
	      cout << "VERBOSE: setenv OO_N_PAGES '" << _nPages << "'" << endl;
	   }
	}

	// Read in a value for the Maximum Cache Size (in pages)

	value = getenv("OO_N_MAX_PAGES");
	if (value) {
	   _nMaxPages = atoi(value);
	   if (_verbose) {
	      cout << "VERBOSE: setenv OO_N_MAX_PAGES '" << _nMaxPages << "'" << endl;
	   }
	}

	// Read in a value for the MROW global override

	value = getenv("OO_MROW");
	if (value) {
	   if (strcmp(value, "oocMROW") == 0) {
	      _fdMROWMode = oocMROW;
	      if (_verbose) {
		 cout << "VERBOSE: setenv OO_MROW 'oocMROW'" << endl;
	      }
	   } else if (strcmp(value, "oocNoMROW") == 0) {
	      _fdMROWMode = oocNoMROW;
	      if (_verbose) {
		 cout << "VERBOSE: setenv OO_MROW 'oocNoMROW'" << endl;
	      }
	   } else {
	      cerr << "WARNING: Unknown value for OO_MROW (" << value;
	      cerr << ") -- ignored." << endl;
	   }
	}

	// Read in a value for the Lock waiting global override

	value = getenv("OO_LOCK_WAIT");
	if (value) {
	   if (strcmp(value, "oocNoWait") == 0) {
	      _lockWait = oocNoWait;
	      if (_verbose) {
		 cout << "VERBOSE: setenv OO_LOCK_WAIT 'oocNoWait'" << endl;
	      }
	   } else if (strcmp(value, "oocWait") == 0) {
	      _lockWait = oocWait;
	      if (_verbose) {
		 cout << "VERBOSE: setenv OO_LOCK_WAIT 'oocWait'" << endl;
	      }
	   } else {
	      _lockWait = atoi(value);
	      if (_verbose) {
		 cout << "VERBOSE: setenv OO_LOCK_WAIT '" << _lockWait << "'" << endl;
	      }
	   }
	}

}

/////////////////////////////////////////////////////////////////////////////////
//
// Class Name:		ooSession
// Method Name:		StartTransaction
//
// Description:		This method starts a Read/Update MROW/non-MROW transaction.
//			The federated database is also opened in the
//			specified mode.
//			A transaction level is checked/incremented to
//			allow an outer transaction OVERRIDE (i.e. this function
//			is a NOP if their is already an active transaction).
//			Note: nested transactions are NOT supported in the current
//			version of Objectivity.
//
// Input Arguments:	fdMode (ooMode)		fd open mode (oocRead/oocUpdate)
//			mrowMode (ooMode)	enable/disable MROW (oocNoMROW/
//						oocMROW)
//
// Output Arguments:	None
//
// Return Value:	ooStatus (oocSuccess/oocError)
//
// Special Notes:	None
//
/////////////////////////////////////////////////////////////////////////////////

ooStatus	ooSession::StartTransaction(ooMode fdmode, ooMode mrowMode,
				ooIndexMode indexMode)
{
	ooStatus	rstatus = oocSuccess;

	// Start a transaction only if no other transaction is active

	ooMode	localMROWMode;

	if (_transLevel == 0) {

	   // Determine if MROW environmental override exists
	   //    (i.e., the session variable is different than constructor
	   //     set value) and use them if they do exist.

	   if (_fdMROWMode != oocMROWIfAllowed)
	      localMROWMode = _fdMROWMode;
	   else
	      localMROWMode = mrowMode;

	   if (_verbose) {
	      cout << "VERBOSE: ooTrans::start(";

	      switch (localMROWMode) {
	      case oocMROW:		cout << "oocMROW,";
					break;
	      case oocNoMROW:		cout << "oocNoMROW,";
					break;
	      case oocMROWIfAllowed:	cout << "oocMROWIfAllowed,";
					break;
	      default:			cout << "unknown=" << localMROWMode << ",";
	      }

	      switch (_lockWait) {
	      case oocWait:		cout << "oocWait,";
					break;
	      case oocNoWait:		cout << "oocNoWait,";
					break;
	      case oocTransNoWait:	cout << "oocTransNoWait,";
					break;
	      default:			cout << _lockWait << ",";
	      }

	      switch (indexMode) {
	      case oocInsensitive:	cout << "Insensitive";
					break;
	      case oocSensitive:	cout << "oocSensitive";
					break;
	      default:			cout << "unknown=" << indexMode;
	      }

	      cout << ")." << endl;
	   }

	   rstatus = _transaction.start(localMROWMode, _lockWait, indexMode);
	   if (rstatus != oocSuccess) {
	      cerr << "ERROR: Cannot start a transaction." << endl;
	      return rstatus;
	   }

	   // Increment the transaction level

	   _transLevel++;

	   // Open the Federated database

	   if (_verbose) {
	      if (fdmode == oocUpdate) {
		 cout << "VERBOSE: ooHandle(ooFDObj)::open(\"" << _fdName << "\",oocUpdate)" << endl;
	      } else {
		 cout << "VERBOSE: ooHandle(ooFDObj)::open(\"" << _fdName << "\",oocRead)" << endl;
	      }
	   }

	   rstatus = _fdHandle.open(_fdName, fdmode);
	   if (rstatus != oocSuccess) {
	      cerr << "ERROR: Cannot open the federated database '" << _fdName;
	      cerr << "'." << endl;
	      AbortTransaction();
	      return rstatus;
	   }

	   // Create/Open a default database
	   if (_databaseName != NULL) {
	      if (!_databaseH.exist(_fdHandle, _databaseName)) {
		 // Create a default database
		 rstatus = CreateDatabase(_databaseName, _databaseH);
	      } else {
		 // Open a default database (in same mode as the federated database)
	         rstatus = OpenDatabase(_databaseName, fdmode, _databaseH);
	         if (rstatus != oocSuccess) {
		    cerr << "ERROR: Cannot open the default database '" << _databaseName;
		    cerr << "'." << endl;
		    AbortTransaction();
		    return rstatus;
	         }
	      }
	   }

	} else {

	   if (_verbose) {
	      cout << "VERBOSE: Ignoring inner ooTrans::start() (level=";
	      cout << _transLevel << ")." << endl;
	   }
	   // Increment the transaction level and do nothing
	   _transLevel++;

	}

	return rstatus;

}

/////////////////////////////////////////////////////////////////////////////////
//
// Class Name:		ooSession
// Method Name:		CommitTransaction
//
// Description:		This method commits an outstanding transaction.
//			A transaction level is checked/decremented to allow
//			an outer transaction OVERRIDE (i.e. this function is
//			a NOP if it is not the outer transaction).
//			The transaction level is also checked to see if
//			any "nested" transactions hav aborted (abort = 1).
//			If so, then do not commit.
//
// Input Arguments:	None
//
// Output Arguments:	None
//
// Return Value:	ooStatus (oocSuccess/oocError)
//
// Special Notes:	None
//
/////////////////////////////////////////////////////////////////////////////////

ooStatus	ooSession::CommitTransaction()
{
	ooStatus	rstatus = oocSuccess;

	// Commit a transaction only if no other transaction is active
	if (_transLevel == 1) {

	   // Check to see if an abort is pending (if so, abort instead of commiting)
	   if (_abortPending) {

	      if (_verbose) {
		 cout << "VERBOSE: ooTrans::abort() due to inner abort." << endl;
	      }

	      // Abort the transaction
	      rstatus = _transaction.abort();

	      // Rest the abort pending flag
	      _abortPending = oocFalse;

	   } else {

	      if (_verbose) {
		 cout << "VERBOSE: ooTrans::commit()" << endl;
	      }

	      // Commit the transaction
	      rstatus = _transaction.commit();

	   }

	} else {
	   if (_verbose) {
	      cout << "VERBOSE: Ignoring inner ooTrans::commit() (level=";
	      cout << _transLevel << ")." << endl;
	   }
	}

	// Decrement the transaction level and do nothing
	_transLevel--;

	return rstatus;
}/////////////////////////////////////////////////////////////////////////////////
//
// Class Name:		ooSession
// Method Name:		HardCommitTransaction
//
// Description:		This method commits an outstanding transaction.
//			A transaction level is checked/zerod
//			The transaction level is also checked to see if
//			any "nested" transactions hav aborted (abort = 1).
//			If so, then do not commit.
//
// Input Arguments:	None
//
// Output Arguments:	None
//
// Return Value:	ooStatus (oocSuccess/oocError)
//
// Special Notes:	None
//
/////////////////////////////////////////////////////////////////////////////////

ooStatus	ooSession::HardCommitTransaction()
{
	ooStatus	rstatus = oocSuccess;

	// Commit a transaction if a transaction is active
	if (_transLevel >= 1) {

	   // Check to see if an abort is pending (if so, abort instead of commiting)
	   if (_abortPending) {

	      if (_verbose) {
		 cout << "VERBOSE: ooTrans::abort() due to inner abort." << endl;
	      }

	      // Abort the transaction
	      rstatus = _transaction.abort();

	      // Rest the abort pending flag
	      _abortPending = oocFalse;

	   } else {

	      if (_verbose) {
		 cout << "VERBOSE: ooTrans::commit()" << endl;
	      }

	      // Commit the transaction
	      rstatus = _transaction.commit();

	   }

	} else {
	   if (_verbose) {
	      cout << "VERBOSE: Ignoring inner ooTrans::commit() (level=";
	      cout << _transLevel << ")." << endl;
	   }
	}

	// zero the transaction level and do nothing
	_transLevel = 0;

	return rstatus;
}

/////////////////////////////////////////////////////////////////////////////////
//
// Class Name:		ooSession
// Method Name:		CommitandHoldTransaction
//
// Description:		This method commits an outstanding transaction.
//			A transaction level is checked/decremented to allow
//			an outer transaction OVERRIDE (i.e. this function is
//			a NOP if it is not the outer transaction).
//			The transaction level is also checked to see if
//			any "nested" transactions hav aborted (abort = 1).
//			If so, then do not commit.
//
// Input Arguments:	None
//
// Output Arguments:	None
//
// Return Value:	ooStatus (oocSuccess/oocError)
//
// Special Notes:	None
//
/////////////////////////////////////////////////////////////////////////////////

ooStatus	ooSession::CommitandHoldTransaction()
{
	ooStatus	rstatus = oocSuccess;

	// Commit a transaction only if no other transaction is active
	if (_transLevel == 1) {

	   // Check to see if an abort is pending (if so, abort instead of commiting)
	   if (_abortPending) {

	      if (_verbose) {
		 cout << "VERBOSE: ooTrans::abort() due to inner abort." << endl;
	      }

	      // Abort the transaction
	      rstatus = _transaction.abort();

	      // Rest the abort pending flag
	      _abortPending = oocFalse;

	   } else {

	      if (_verbose) {
		 cout << "VERBOSE: ooTrans::commitAndHold()" << endl;
	      }

	      // Commit the transaction
	      rstatus = _transaction.commitAndHold();

	   }

	} else {
	   if (_verbose) {
	      cout << "VERBOSE: Ignoring inner ooTrans::commitAndHold() (level=";
	      cout << _transLevel << ")." << endl;
	   }
	}

	// do not Decrement the transaction level and do nothing
//	_transLevel--;

	return rstatus;
}

/////////////////////////////////////////////////////////////////////////////////
//
// Class Name:		ooSession
// Method Name:		AbortTransaction
//
// Description:		This method aborts an outstanding transaction.
//			A transaction level is checked/decremented to allow
//			an outer transaction OVERRIDE (i.e. this function is
//			a NOP if it is not the outer transaction).
//
// Input Arguments:	None
//
// Output Arguments:	None
//
// Return Value:	ooStatus (oocSuccess/oocError)
//
// Special Notes:	None
//
/////////////////////////////////////////////////////////////////////////////////

ooStatus	ooSession::AbortTransaction()
{
	ooStatus	rstatus = oocSuccess;

	// Abort a transaction only if no other transaction is active
	if (_transLevel == 1) {

	   if (_verbose) {
	      cout << "VERBOSE: ooTrans::abort()" << endl;
	   }

	   // Abort the transaction
	   rstatus = _transaction.abort();

	   // Always set the abort pending to false
	   _abortPending = oocFalse;

	} else {

	   if (_verbose) {
	      cout << "VERBOSE: Ignoring inner ooTrans::abort() (level=";
	      cout << _transLevel << "). -- future commits disabled" << endl;
	   }

	   // Set the abort pending flag
	   _abortPending = oocTrue;

	}

	// Decrement the transaction level
	_transLevel--;

	return rstatus;
}

/////////////////////////////////////////////////////////////////////////////////
//
// Class Name:		ooSession
// Method Name:		SetLockWait
//
// Description:		This method sets the lock waiting mode.
//			The StartTransaction method uses the results of this
//			method for subsequent transactions.
//
// Input Arguments:	waitOption (int32)	Wait in seconds or ooc[No]Wait
//
// Output Arguments:	None
//
// Return Value:	None
//
// Special Notes:	None
//
/////////////////////////////////////////////////////////////////////////////////

void	ooSession::SetLockWait(int32 waitOption)
{
	if (_verbose) {
	   cout << "VERBOSE: ooSetLockWait(" << waitOption << ")" << endl;
	}

	// Set the session data member (for subsequent transactions)
	_lockWait = waitOption;

	// Set the immediate lock waiting behavior
	ooSetLockWait(waitOption);
}

/////////////////////////////////////////////////////////////////////////////////
//
// Class Name:		ooSession
// Method Name:		OpenDatabase
//
// Description:		This method opens the database specified by DBName in
//			the mode specified.
//
// Input Arguments:	DBName (char*)	database system name
//			mode (ooMode)	oocRead/oocUpdate
//
// Output Arguments:	databaseH (ooHandle(ooDBObj))	handle to opened database
//
// Return Value:	ooStatus (oocSuccess/oocError)
//
// Special Notes:	None
//
/////////////////////////////////////////////////////////////////////////////////

ooStatus	ooSession::OpenDatabase(const char* DBName, ooMode mode,
			     ooHandle(ooDBObj)& databaseH)
{
	ooStatus	rstatus = oocSuccess;

	if (_verbose) {
	   cout << "VERBOSE: ooHandle(ooDBObj)::open(fdH,\"" << DBName << "\")" << endl;
	}

	// Check to see if the database exists
	if (databaseH.exist(_fdHandle, DBName)) {
	   // It exists -- Open it in the appropriate mode
	   rstatus = databaseH.open(_fdHandle, DBName, mode);
	} else {
	   cerr << "ERROR: Database '" << DBName << "' does not exist." << endl;
	   rstatus = oocError;
	}

	return rstatus;
}

/////////////////////////////////////////////////////////////////////////////////
//
// Class Name:		ooSession
// Method Name:		CreateDatabase
//
// Description:		This method creates a database of name DBName.
//
// Input Arguments:	DBName (char*)	database system name
//
// Output Arguments:	databaseH (ooHandle(ooDBObj))	handle to created database
//
// Return Value:	ooStatus (oocSuccess/oocError)
//
// Special Notes:	None
//
/////////////////////////////////////////////////////////////////////////////////

ooStatus	ooSession::CreateDatabase(const char* DBName,
			      ooHandle(ooDBObj)& databaseH, ooBoolean replaceDatabase)
{
	ooStatus	rstatus = oocSuccess;

	// Check to see if the database exists
	if (databaseH.exist(_fdHandle, DBName)) {
	   if (replaceDatabase) {
		// Delete the current database
		rstatus = DeleteDatabase(DBName);
		if (rstatus != oocSuccess) {
		   cerr << "ERROR: Cannot delete existing database '" << DBName;
		   cerr << "'." << endl;
		   return rstatus;
		}

		// Create the new database
	        if (_verbose) {
	           cout << "VERBOSE: new ooDBObj(\"" << DBName << "\")" << endl;
	        }

		databaseH = new ooDBObj(DBName);

	   } else {
	      cerr << "ERROR: A database exists with name '" << DBName << "'." << endl;
	      return oocError;
	   }
	} else {
	   if (_verbose) {
	     cout << "VERBOSE: new ooDBObj(\"" << DBName << "\")" << endl;
	   }
	   databaseH = new ooDBObj(DBName);
	}

	return rstatus;

}
/////////////////////////////////////////////////////////////////////////////////
//
// Class Name:		ooSession
// Method Name:		OorCDatabase
//
// Description:		This method open or creates a database of name DBName.
//
// Input Arguments:	DBName (char*)	database system name
//
// Output Arguments:	databaseH (ooHandle(ooDBObj))	handle to created database
//
// Return Value:	ooStatus (oocSuccess/oocError)
//
// Special Notes:	None
//
/////////////////////////////////////////////////////////////////////////////////

ooStatus	ooSession::OorCDatabase(const char* DBName, ooMode mode,
			      ooHandle(ooDBObj)& databaseH)
{
	ooStatus	rstatus = oocSuccess;

	   if (_verbose) {
	     cout << "VERBOSE: new/open ooDBObj(\"" << DBName << "\")" << endl;
	   }

	   // Create/Open a database
	   if (DBName != NULL) {
	      if (!databaseH.exist(_fdHandle, DBName)) {
		 // Create a database
		 rstatus = CreateDatabase(DBName, databaseH);
	      } else {
		 // Open a database 
	         rstatus = OpenDatabase(DBName, mode, databaseH);
	         if (rstatus != oocSuccess) {
		    cerr << "ERROR: Cannot open the database '" << DBName;
		    cerr << "'." << endl;
		  }
		 return rstatus;
	      }
	   }
	return rstatus;
}

/////////////////////////////////////////////////////////////////////////////////
//
// Class Name:		ooSession
// Method Name:		DeleteDatabase
//
// Description:		This method deletes the database specified by DBName.
//
// Input Arguments:	DBName (char*)	database system name
//
// Output Arguments:	None
//
// Return Value:	ooStatus (oocSuccess/oocError)
//
// Special Notes:	None
//
/////////////////////////////////////////////////////////////////////////////////

ooStatus	ooSession::DeleteDatabase(const char* DBName)
{
	ooStatus		rstatus = oocSuccess;
	ooHandle(ooDBObj)	databaseH;

	// Open the database for update
	rstatus = OpenDatabase(DBName, oocUpdate, databaseH);
	if (rstatus != oocSuccess) {
	   cerr << "ERROR: Cannot open database '" << DBName << "' for update." << endl;
	   return rstatus;
	}

	if (_verbose) {
	   cout << "VERBOSE: ooDelete(handle to '" << DBName << ")" << endl;
	}

	// Delete the database
	rstatus = ooDelete(databaseH);

	return rstatus;
}

/////////////////////////////////////////////////////////////////////////////////
//
// Class Name:		ooSession
// Method Name:		StartTimer
//
// Description:		This method starts a wall-clock timer.
//
// Input Arguments:	None
//
// Output Arguments:	None
//
// Return Value:	None
//
// Special Notes:	None
//
/////////////////////////////////////////////////////////////////////////////////

void		ooSession::StartTimer()
{
	// Grab the current time
	_startTime = time(0);
}

/////////////////////////////////////////////////////////////////////////////////
//
// Class Name:		ooSession
// Method Name:		EndTimer
//
// Description:		This method stops a wall-clock timer.
//
// Input Arguments:	None
//
// Output Arguments:	None
//
// Return Value:	None
//
// Special Notes:	None
//
/////////////////////////////////////////////////////////////////////////////////

void		ooSession::EndTimer()
{
	// Grab the current time
	_endTime = time(0);
}

/////////////////////////////////////////////////////////////////////////////////
//
// Class Name:		ooSession
// Method Name:		PrintTimer
//
// Description:		This method prints the difference between start/stop times.
//
// Input Arguments:	label (char*)	Informational label prepended to output.
//
// Output Arguments:	None
//
// Return Value:	None
//
// Special Notes:	None
//
/////////////////////////////////////////////////////////////////////////////////

void		ooSession::PrintTimer(char* label)
{
	// Print the difference of start and end times
	cout << label << " Elapsed Time: " << ((float) (_endTime - _startTime));
	cout << " seconds." << endl;
}

/////////////////////////////////////////////////////////////////////////////////
//
// Class Name:		ooSession
// Method Name:		StartStatistics
//
// Description:		This method sets the encapsulated ooRunStatus() delta
//			counters. This method is used in conjunction with
//			ooSession::Print*Statistics() to observe Objectivity/DB
//			internal behavior.
//
// Input Arguments:	None
//
// Output Arguments:	None
//
// Return Value:	None
//
// Special Notes:	None
//
/////////////////////////////////////////////////////////////////////////////////

void		ooSession::StartStatistics()
{
	_monitor.Start();
}

/////////////////////////////////////////////////////////////////////////////////
//
// Class Name:		ooSession
// Method Name:		PrintDeltaStatistics
//
// Description:		This method prints the ooRunStatus() statistics of
//			for the Objectivity/DB Object and Storage Managers.
//			Delta information is printed since the last call to
//			ooSession::{StartStatistics,Print*Statistics}.
//
// Input Arguments:	label (char*)	Informational label prepended to output.
//
// Output Arguments:	None
//
// Return Value:	None
//
// Special Notes:	None
//
/////////////////////////////////////////////////////////////////////////////////

void		ooSession::PrintDeltaStatistics(char* label)
{
	// Print label
	cout << label << endl;

	// Print delta information
	_monitor.PrintDelta(cout);
}

/////////////////////////////////////////////////////////////////////////////////
//
// Class Name:		ooSession
// Method Name:		PrintCumulativeStatistics
//
// Description:		This method prints the ooRunStatus() statistics of
//			for the Objectivity/DB Object and Storage Managers.
//			Cumulative information (since the beginning of the
//			process is printed.
//
// Input Arguments:	label (char*)	Informational label prepended to output.
//
// Output Arguments:	None
//
// Return Value:	None
//
// Special Notes:	None
//
/////////////////////////////////////////////////////////////////////////////////

void		ooSession::PrintCumulativeStatistics(char* label)
{
	// Print label
	cout << endl << label << endl;

	// Print cumulative information
	_monitor.PrintCumulative(cout);
}

