#ifndef OOSESSION_H
#define OOSESSION_H

//session class made Singleton

#include <oo.h>
#include <time.h>
#include "ooStats.h"

class ooSession {
private:
        static ooSession*       _instance;     //instance counter

	char*			_fdName;	// FDSysName (aka bootfile path/file)
	ooHandle(ooFDObj)	_fdHandle;	// FD Handle
	ooMode			_fdMROWMode;	// FD MROW override

	uint32			_nFiles;	// Maximum # of file descriptors
	uint32			_nPages;	// Initial cache size
	uint32			_nMaxPages;	// Maximum cache size

	ooTrans			_transaction;	// Current Transaction
	int			_transLevel;	// Transaction nesting level
	ooBoolean		_abortPending;	// Transaction abort flag
	ooBoolean		_verbose;	// Debugging echo enable
	int32			_lockWait;	// Lock Wait value/flag

	time_t			_startTime;	// Starting time
	time_t			_endTime;	// Ending time
	ooStats			_monitor;	// ooRunStatus encapsulation
protected:
	ooSession();				// Constructor

	char*			_databaseName;	// Default Database name
	ooHandle(ooDBObj)	_databaseH;	// Default Database handle
public:
        static ooSession* Instance(const char* fdName = 0,
				   const uint32 nFiles = 12,
				   const uint32 nPages = 200,
				   const uint32 nMaxPages = 500);
	~ooSession();				// Destructor

	ooStatus	Initialize(const char* fdName = 0,
				   const uint32 nFiles = 12,
				   const uint32 nPages = 200,
				   const uint32 nMaxPages = 500);
	void		ReadEnvironmentVars();

	// Transaction Methods

	ooStatus	StartTransaction(ooMode fdMode = oocRead,
					 ooMode mrowMode = oocNoMROW,
					 ooIndexMode indexMode = oocInsensitive);
	ooStatus	HardCommitTransaction();
	ooStatus	CommitTransaction();
	ooStatus	CommitandHoldTransaction();
	ooStatus	AbortTransaction();

	// Locking and Concurrency Method

	void		SetLockWait(int32 waitOption);

	// Database Methods

	ooStatus		OpenDatabase(const char* DBName, ooMode mode,
				     ooHandle(ooDBObj)& databaseH);
	ooStatus		CreateDatabase(const char* DBName,
				     ooHandle(ooDBObj)& databaseH,
				     ooBoolean replaceDatabase = oocFalse);
	ooStatus		OorCDatabase(const char* DBName, ooMode mode,
				     ooHandle(ooDBObj)& databaseH);
	ooStatus		DeleteDatabase(const char* DBName);
	ooHandle(ooDBObj)&	DefaultDatabase() { return _databaseH; }


	// Monitor Methods

	void		StartTimer();
	void		EndTimer();
	void		PrintTimer(char* label);
	void		StartStatistics();
	void		PrintDeltaStatistics(char* label);
	void		PrintCumulativeStatistics(char* label);
        int             getTransLevel() { return _transLevel;}
};

#endif
