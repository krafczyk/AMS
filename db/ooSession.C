/* 
 ******************************************************************************
 *
 *      Objectivity Inc.
 *      Copyright (c) 1995, Objectivity, Inc. USA   All rights reserved.
 *
 ****************************************************************************
 *
 *      File:   ooSession.C
 *
 *      Functions(s):
 *
 *      Description:
 *
 *      RCSid = "$Id: ooSession.C,v 1.2 1997/05/14 05:41:45 alexei Exp $"
 *
 ****************************************************************************
 */
#ifndef _OOSESSION_C_
#define _OOSESSION_C_


#include <oo.h>
#include <ooMap.h>

#ifndef _WIN32
#  include <unistd.h>
#endif

#include <stdlib.h>

#include "ooSession.h"






implement(ooVArray,ooRef(ooContObj))

ooHandle(ooFDObj) ooSession::fdbH ; 
ooHandle(ooContObj) ooSession::contH ;   
ooHandle(ooDBObj) ooSession::dbH ; 
ooTrans ooSession::transaction ; 


ooStats* ooSession::stats_p = new ooStats ( ) ; 

// these two are used only by the session class to support persistent
// session configuration and object lookup.
//
ooHandle(ooContObj) ooSession::sessionContH ;   
ooHandle(ooDBObj) ooSession::sessiondbH ; 
ooHandle(ooMap) ooSession::contMapH ; 
static const char* sessionCont_Name = "sessionCont-0" ;
static const char* sessiondb_Name = "ooSession" ;
static const char* contMap_Name = "contMap_Name" ;




#ifdef OO_VERSION3X
ooAMSUsage ooSession::amsUsage ;
#endif

unsigned ooSession::refCount = 0 ;

unsigned short ooSession::transactionCount = 0 ;
unsigned short ooSession::init_called = 0 ;


// ooTimer Stuff...
#ifdef __ALPHAOSF1
struct timespec ooTimer::txn_startT ;
struct timespec ooTimer::txn_endT ;
#else
struct timeval ooTimer::txn_startT ;
struct timeval ooTimer::txn_endT ;
struct timezone ooTimer::tz ;
#endif
unsigned ooTimer::update_msecs = 0 ;
unsigned ooTimer::no_updates = 0 ;
unsigned ooTimer::read_msecs = 0 ;
unsigned ooTimer::no_reads = 0 ;
unsigned ooTimer::tot_update_msecs = 0 ;
unsigned ooTimer::tot_no_updates = 0 ;
unsigned ooTimer::tot_read_msecs = 0 ;
unsigned ooTimer::tot_no_reads = 0 ;




#ifdef ROGUEWAVE
#include <rw/tvhdict.h>
#include <rw/cstring.h>
#include <rw/rstream.h>
static unsigned hashString(const RWCString& str) {return str.hash();}
RWTValHashDictionary<RWCString, ooHandle(ooObj)> ooSession::containerDict (hashString);
#endif


// used to tag/name transactions for timing.
static char* txn_tag = (char*) 0 ;





/*
 ******************************************************************************
 *
 *      Function Name:  ooSession::ooSession
 *
 *      Description: 
 *		constructor
 *
 *      Returns:
 *		nothing
 * 
 *      Side Effects:
 *		none.
 *
 *
 ******************************************************************************
 */
ooSession::ooSession (
	ooMode read_write )
{
	++refCount ;
	if ( refCount == 1 )
		stats_p->Start();

	lastErrCode		= oocNoError ;
	lastErrString		= (char*) 0 ;
	mode			= read_write ;
	persistent		= 1 ;

	if ( setMode () != oocSuccess )
		ooSignal ( oocWarning, ooS_failed_fd_reopen, 0, "ooSession()", mode ) ;

	if (contR.size() < 20)
		contR.resize ( 20 ) ;

	if ( dbg_txns() )
		cout << "ooSession::transactionCount= " <<
		transactionCount << " :<ooSession(" <<
		(read_write == oocRead ? "oocRead" : "oocUpdate") << ")" << endl;

}

/*
 ******************************************************************************
 *
 *      Function Name:  ooSession::~ooSession
 *
 *      Description: 
 *		destructor
 *
 *      Returns:
 *		nothing
 * 
 *      Side Effects:
 *		frees error string malloc'd by strdup at transaction commit.
 *
 *
 ******************************************************************************
 */
ooSession::~ooSession ( )
{
	--refCount ;

	if ( lastErrString != 0 ) 
		free ( lastErrString ) ;

	if ( dbg_txns() ) {
		cout << "ooSession::transactionCount= " <<
		transactionCount << " :>~ooSession(" <<
		(mode == oocRead ? "oocRead" : "oocUpdate") << ")" << endl;
	}

	if ( runStats() && refCount == 0 ) {
		if ( tot_no_updates != 0 && tot_no_reads != 0 ) {
			 cout << endl << 
			"cumulative average msecs, update txns: " <<
			tot_update_msecs/tot_no_updates << endl <<
			"cumulative average msecs, read txns: " <<
			tot_read_msecs/tot_no_reads << endl ;
		}
		ooRunStatus ( ) ;
	}
}


// Optionally print run status ;
void
ooSession::stats ( )
{
	if ( runStats() ) 
		ooRunStatus ( ) ;

	return ;
}

//	extern void opiPrintHList(uint32 level = 0, uint32 number = 0);
//	opiPrintHList(0);  // How many on the stack
//	opiPrintHList(1);  // List the pointer address of all handles
//	opiPrintHList(2);  // Above and oids
//
// print out the list of active handles.
void
ooSession::printHandleStack (
	int level )
{
	opiPrintHList ( level ) ;

	return ;
}



/*
 ******************************************************************************
 *
 *      Function Name:  ooSession::Init
 *
 *      Description: 
 *		initializes the connection to the federation, and
 *		opens it in the mode specified in the ctor.
 *
 *		always called, a 'final constructor', allows static
 *		instances of this class, since Init must be called 
 *		after main().
 *
 *      Returns:
 *		nothing
 * 
 *      Side Effects:
 *		Opens fddb and db, initializes handles.
 *
 *
 ******************************************************************************
 */
void
ooSession::Init ( )
{
	if ( verbose() ) 
		cout << "initializing Objectivity/DB ..." << endl;

	//	Initialize Objectivity/DB
	//
	if ( ooInit(12,cacheIPgs(),cacheMPgs()) != oocSuccess ) 
		ooSignal ( oocUserError, ooS_failed_init, 0, "Init()" )  ;

	init_called = 1 ;

#ifdef OO_VERSION3X
	ooSetAMSUsage ( amsUsage ) ; 
#endif

	ooSetLockWait ( lockWait() ) ;

	if ( oo_hot_mode() )
		ooSetHotMode ( ) ;

	if ( oo_no_lock() )
		ooNoLock ( ) ;

	if ( oo_no_journal() )
		ooNoJournal ( ) ;

	// Start the First Transaction here.
	//
	transaction.start ( mrow() ) ;

	// Open the Federated DataBase.
	//
//	if ( fdbH.open ( fdBootName(), mode ) != oocSuccess ) {
	if ( fdbH.exist ( fdBootName(), mode ) == oocFalse )
                ooSignal ( oocUserError, ooS_failed_fd_open, 0, "Init()", fdBootName() )  ;

	// initialize the ooSession's database (large scale use) if indicated.
	//
	if ( use_session_db() )
		setup_session_db ( sessiondbH ) ;

	if ( verbose() ) {
		cout << "ooSession::Init - Opening Context, FD: " <<
		fdBootName() <<
		" DB: " << 
		dbFileName() << endl <<
		*this ;

		if ( oo_hot_mode() )
			cout << "ooSession - Warning, ooSetHotMode() selected." << endl ;
		if (  oo_no_lock() )
			cout << "ooSession - Warning, oo_no_lock() selected." << endl ;
		if (  oo_no_journal() )
			cout << "ooSession - Warning, oo_no_journal() selected." << endl ;
	}

	transaction.commit ( ) ;
}


/*
 ******************************************************************************
 *
 *      Function Name:  ooSession::setup_session_db
 *
 *      Description: 
 *		Find/Create the session database
 *
 *      Returns:
 *		nothing
 * 
 *      Side Effects:
 *		finds and initializes the session database.
 *
 *
 ******************************************************************************
 */
void
ooSession::setup_session_db (
	ooHandle(ooDBObj)& sessiondb_H ) 
{
	if ( db ( sessiondb_H, sessiondb_Name, dbPathName(), dbHostName() ) != oocSuccess )
		ooSignal ( oocUserError, ooS_failed_db_open, 0, "setup_session_db()", (char*) sessiondb_Name ) ;

	return ;
}


/*
 ******************************************************************************
 *
 *      Function Name:  ooSession::setup_session_map
 *
 *      Description: 
 *		Find/Create the session container and container map.
 *
 *      Returns:
 *		nothing
 * 
 *      Side Effects:
 *		finds and initializes the session container map.
 *
 *
 ******************************************************************************
 */
void
ooSession::setup_session_map (
	ooHandle(ooDBObj)& _db_H )
{
	if ( _db_H == 0 )
		ooSignal ( oocUserError, ooS_failed_session_map, &fdbH, "setup_session_map()" ) ;

	if ( sessionContH.lookupObj ( _db_H, sessionCont_Name ) != oocSuccess ) {
		sessionContH = new ( _db_H ) ooContObj ( ) ;
		sessionContH.nameObj ( _db_H, sessionCont_Name ) ;
	}
	if( contMapH.lookupObj ( sessionContH, contMap_Name ) != oocSuccess ) {
		contMapH = new ( sessionContH ) ooMap ( ) ;
		contMapH.nameObj ( sessionContH, contMap_Name ) ;
	}

	return ;
}

ooHandle(ooMap)&
ooSession::contMap()
{
	if ( contMapH == 0 )
		setup_session_map ( dbH ) ;

	return contMapH ;
}



/*
 ******************************************************************************
 *
 *      Function Name:  ooSession::TransStart
 *
 *      Description: 
 *		Start a transaction, if you are not already within one.
 *
 *      Returns:
 *		nothing
 * 
 *      Side Effects:
 *		none.
 *
 *
 ******************************************************************************
 */
ooStatus
ooSession::TransStart (
	ooMode md,
	const char* tag )
{
	ooStatus ret_val = oocError ;

	if ( init_called == 1 ) {
		mrow ( md ) ;

		if ( transaction.isActive() == oocFalse &&
			transactionCount == 0 )
		{
			if ( time_txns() ) {
				if ( tag != 0 ) 
					txn_tag = strdup(tag);
				set_txn_startT ( ) ;
			}

			// *** REAL TXN START ***
		//	transaction.start ( mrow(), oocTransNoWait, indexMode() ) ;
			ret_val = transaction.start ( mrow(), lockWait(), indexMode() ) ;

			if ( mode == oocUpdate )
				tot_no_updates += ++no_updates ;
			else 
				tot_no_reads += ++no_reads ;
			if ( dbg_txns() )
				cout << "ooSession::transactionCount= " <<
				transactionCount << " :<TransStart(" <<
				(mode == oocRead ? "oocRead" : "oocUpdate") << ")" << endl;
		}

		++transactionCount ;

		// if ( fd().isOpen == oocFalse )
		fd().open ( fdBootName(), mode ) ;   
	}

	return ret_val ;
}


/*
 ******************************************************************************
 *
 *      Function Name:  ooSession::TransCommit
 *
 *      Description: 
 *		Commits the current transaction if one is active.
 *		Saves the last error number and string, if set.
 *
 *      Returns:
 *		nothing
 * 
 *      Side Effects:
 *		DELETES all temporary containers, invalidating their handles.
 *		Cross transaction cacheing of temnporary data is not supported
 *		by this class.
 *
 *
 ******************************************************************************
 */
ooStatus
ooSession::TransCommit ( )
{
	ooStatus ret_val = oocError ;

	if ( init_called == 1 ) {
		if ( transaction.isActive() == oocTrue &&
			transactionCount == 1 ) {
			internalCommit ( ) ;

			// if print handle stack ?
			if ( handleStack() ) 
				printHandleStack ( ) ;

			// *** REAL COMMIT ***
			ret_val = transaction.commit ( ) ;

			if ( time_txns() > 0 ) {
				set_txn_endT ( ) ;
				if ( mode == oocUpdate ) { 
					update_msecs += cmpTime ( txn_startT, txn_endT ) ;
					tot_update_msecs += update_msecs ;
				}
				else {
					read_msecs += cmpTime ( txn_startT, txn_endT ) ;
					tot_read_msecs += read_msecs ;
				}

				if ( time_txns() == 3 )
					print_txn_time ( mode, txn_tag ) ;
				if ( txn_tag != 0 ) 
					free ( txn_tag ) ;
				txn_tag = (char*) 0 ;
			}

			// print transaction count(s) ?
			if ( dbg_txns() )
				cout << "ooSession::transactionCount= " <<
				transactionCount << " :>TransCommit(" <<
				(mode == oocRead ? "oocRead" : "oocUpdate") << ")" << endl;

			// print run status ?
			if ( txn_run_stats() )
				stats_p->PrintDelta ( cout ) ;
		}

		if ( transactionCount >= 1 )
			--transactionCount ;
	}

	return ret_val ;
}


/*
 ******************************************************************************
 *
 *      !!!!!   CHANGED TransCommit FUNCTION   !!!!!
 *
 *      Function Name:  ooSession::TransAbort
 *
 *      Description: 
 *		Aborts the current transaction if one is active.
 *		Saves the last error number and string, if set.
 *
 *      Returns:
 *		nothing
 * 
 *      Side Effects:
 *		DELETES all temporary containers, invalidating their handles.
 *		Cross transaction cacheing of temnporary data is not supported
 *		by this class.
 *
 *
 ******************************************************************************
 */
ooStatus
ooSession::TransAbort ( )
{
	ooStatus ret_val = oocError ;

	if ( init_called == 1 ) {
		if ( transaction.isActive() == oocTrue &&
			transactionCount == 1 ) {
			internalCommit ( ) ;

			// if print handle stack ?
			if ( handleStack() ) 
				printHandleStack ( ) ;

			// *** REAL ABORT ***
			ret_val = transaction.abort ( ) ;

			if ( time_txns() > 0 ) {
				set_txn_endT ( ) ;
				if ( mode == oocUpdate ) { 
					update_msecs += cmpTime ( txn_startT, txn_endT ) ;
					tot_update_msecs += update_msecs ;
				}
				else {
					read_msecs += cmpTime ( txn_startT, txn_endT ) ;
					tot_read_msecs += read_msecs ;
				}

				if ( time_txns() == 3 )
					print_txn_time ( mode, txn_tag ) ;
				if ( txn_tag != 0 ) 
					free ( txn_tag ) ;
				txn_tag = (char*) 0 ;
			}

			// print transaction count(s) ?
			if ( dbg_txns() )
				cout << "ooSession::transactionCount= " <<
				transactionCount << " :>TransAbort(" <<
				(mode == oocRead ? "oocRead" : "oocUpdate") << ")" << endl;

			// print run status ?
			if ( txn_run_stats() )
				stats_p->PrintDelta ( cout ) ;
		}

		if ( transactionCount >= 1 )
			--transactionCount ;
	}

	return ret_val ;
}


/*
 ******************************************************************************
 *
 *      Function Name:  ooSession::internalCommit
 *
 *      Description: 
 *		Does the internal transaction commit cleanup work
 *
 *      Returns:
 *		nothing
 * 
 *      Side Effects:
 *		many...
 *
 *
 ******************************************************************************
 */
void
ooSession::internalCommit ( ) 
{
	int size = contR.size ( ) ;
    ooRef(ooContObj) nulRef(0);

	ooHandle(ooContObj) tmp_h = 0 ;
	for ( int ix = 0; ix < size; ix++ ) {
		tmp_h = contR.elem(ix) ;
		if ( tmp_h.isValid() == oocTrue  ) {
			ooDelete ( tmp_h ) ;
			contR.set(ix,nulRef) ;		// 9.1.97  M.N + D.D
		}
	}

	if ( oovLastError != 0 ) {
		lastErrCode = oovLastError->errorN ; 
		if ( oovLastError->message != 0 ) {
			if ( lastErrString != 0 )
				free ( lastErrString ) ;
			lastErrString = strdup(oovLastError->message) ;
		}
	}

	return ;
}


/*
 ******************************************************************************
 *
 *      Function Name:  ooSession::setMode
 *
 *      Description: 
 *		Actively sets the openMode of the Federation within the
 *		context of a transaction.
 *
 *      Returns:
 *		oocSuccess or oocError
 * 
 *      Side Effects:
 *		none.
 *
 *
 ******************************************************************************
 */
int
ooSession::setMode ( ) 
{
	int ret_val = oocSuccess ;

	if ( transaction.isActive() == oocTrue ) {
		ooMode c_mode = fd().openMode() ;
		if ( c_mode != oocUpdate && c_mode != mode && c_mode != oocNoOpen ) {
			fd().close ( ) ;
			ret_val = fd().open ( fdBootName(), mode ) ; 
			if ( ret_val != oocSuccess ) 
				ooSignal ( oocWarning, ooS_failed_fd_reopen, 0, "setMode()", mode ) ;
		}
	}

	return ret_val ;
}


/*
 ******************************************************************************
 *
 *      Function Name:  ooSession::setRead
 *
 *      Description: 
 *
 *      Returns:
 *		nothing
 * 
 *      Side Effects:
 *		none.
 *
 *
 ******************************************************************************
 */
int
ooSession::setRead ( ) 
{
	int ret_val = oocSuccess ;

	mode = oocRead ;

	ret_val = setMode ( ) ;

	return ret_val ;
}



/*
 ******************************************************************************
 *
 *      Function Name:  ooSession::setUpdate
 *
 *      Description: 
 *
 *      Returns:
 *		nothing
 * 
 *      Side Effects:
 *		none.
 *
 *
 ******************************************************************************
 */
int
ooSession::setUpdate ( )
{
	int ret_val = oocSuccess ;

	mode = oocUpdate ;

	ret_val = setMode ( ) ;

	return ret_val ;
}



/*
 ******************************************************************************
 *
 *      Function Name:  ooSession::addTempCont
 *
 *      Description: 
 *
 *      Returns:
 *		nothing
 * 
 *      Side Effects:
 *		none.
 *
 *
 ******************************************************************************
 */
void
ooSession::addTempCont (
	ooHandle(ooContObj)& cont_h )
{
	int size = contR.size ( ) ;

	ooHandle(ooContObj) tmp_h = 0 ;
	for ( int ix = 0; ix < size; ix++ ) {
		tmp_h = contR.elem(ix) ;
		if ( tmp_h == 0 ) {
			contR.set (ix,cont_h) ;
			break ;
		}
	}
 
	if ( ix == size ) {
		contR.resize ( contR.size() + 10 ) ;
		contR.set(ix,cont_h) ;
	}

	return ;
}



/*
 ******************************************************************************
 *
 *      Function Name:  ooSession::tempCont
 *
 *      Description: 
 *
 *      Returns:
 *		nothing
 * 
 *      Side Effects:
 *		none.
 *
 *
 ******************************************************************************
 */
ooHandle(ooContObj)
ooSession::tempCont (
	const char* ContName,
	uint32 hash,
	uint32 initPages ) 
{
	ooHandle(ooContObj)	ret_h ;
	static int num = 0 ;
	uint32 initial_pages = ( CntIPgs() >= initPages ? CntIPgs() : initPages ) ;

	if ( ContName != 0 ) 
		ret_h = container ( ContName, hash ) ;
	else {
		setUpdate ( ) ;
		char buffer[64] ;

		sprintf ( buffer, "%X-%d", 
#ifdef _WIN32
			(int)GetCurrentProcessId(),
#else
			getpid(),
#endif
			num++ ) ;

		if ( ret_h.lookupObj ( dbH, buffer ) != oocSuccess ) {
			ret_h = new ( buffer, hash, initial_pages, CntGPct(), dbH ) ooContObj ( ) ;
			if (! ret_h.isValid() )
				ooSignal(oocUserError,ooS_failed_container,0,"tempCont()",(char*)buffer);
			else
				ret_h.nameObj ( dbH, buffer ) ;
		}

	}

	addTempCont ( ret_h ) ;

	return ret_h ;
}


/*
 ******************************************************************************
 *
 *      Function Name:  ooSession::random
 *
 *      Description: 
 *
 *      Returns:
 *		integer, == 1, ooContObj was created, == 0, ooContObj was found.
 * 
 *      Side Effects:
 *		none.
 *
 *
 ******************************************************************************
 */
int
ooSession::random (
	ooHandle(ooContObj)& cont_h,
	int MaxDistribution,
	const char* Prefix,
	uint32 hash,
	uint32 initPages ) 
{
	ooStatus ret =  0 ;
	uint32 initial_pages = ( rndCntIPgs() >= initPages ? rndCntIPgs() : initPages ) ;

	int maxdistribution = randDist() ;
	if ( MaxDistribution != 0 )
		maxdistribution = MaxDistribution ;

	int seed = rand() % maxdistribution ;
	char nameBuff[128] ;
	char buff[128] ;
	if ( Prefix != 0 ) 
		strcpy ( buff, Prefix ) ;
	else
		strcpy ( buff, "SessionCont" ) ;
	sprintf ( nameBuff, "%s-%X", buff, seed ) ;

	ret = contMap()->lookup ( nameBuff, cont_h, oocNoOpen ) ;
	if ( cont_h.isValid() == oocFalse || ret != oocSuccess ) {
		setUpdate ( ) ;
		ret = 1 ;
		ooHandle(ooContObj)* Handles = new ooHandle(ooContObj) [maxdistribution] ;
		ooNewConts (ooContObj,maxdistribution,dbH,hash,initial_pages,rndCntGPct(),oocFalse,Handles) ; 
		for ( int cn = 0; cn < maxdistribution; cn++ ) {
			sprintf ( nameBuff, "%s-%X", buff, cn ) ;
			contMap()->remove ( nameBuff ) ;
			contMap()->add ( nameBuff, Handles[cn] ) ;
		}
		delete [] Handles ;
	}

	// now look it up, it should be there.
	sprintf ( nameBuff, "%s-%X", buff, seed ) ;
	if ( contMap()->lookup ( nameBuff, cont_h, oocNoOpen ) != oocSuccess )
                ooSignal(oocUserError,ooS_failed_container,0,"random()",(char*)nameBuff);

	if ( persistent == 0 )
		addTempCont ( cont_h ) ;

	return ret ;
}



/*
 ******************************************************************************
 *
 *      Function Name:  ooSession::container
 *
 *      Description: 
 *
 *      Returns:
 *		integer, == 1, ooContObj was created, == 0, ooContObj was found.
 * 
 *      Side Effects:
 *		none.
 *
 *
 ******************************************************************************
 */
int
ooSession::container (
	ooHandle(ooContObj)& cont_h,
	int ContainerNumber,
	const char* Prefix,
	uint32 hash,
	uint32 initPages ) 
{
	int ret = 0 ;
	char buff[64] ;

	if ( Prefix != 0 ) 
		sprintf ( buff, "%.50s-%X", Prefix, ContainerNumber ) ;
	else
		sprintf ( buff, "SessionCont-%X", ContainerNumber ) ;

	ret = container ( cont_h, buff, hash, initPages ) ;

	return ret ;
}


/*
 ******************************************************************************
 *
 *      Function Name:  ooSession::container
 *
 *      Description: 
 *
 *      Returns:
 *		integer, == 1, ooContObj was created, == 0, ooContObj was found.
 * 
 *      Side Effects:
 *		none.
 *
 *
 ******************************************************************************
 */
int
ooSession::container (
	ooHandle(ooContObj)& cont_h,
	const char* ContName,
	uint32 hash,
	uint32 initPages )
{
	ooStatus ret = oocError ;

	uint32 initial_pages = ( CntIPgs() >= initPages ? CntIPgs() : initPages ) ;

	const char* container_name = contName ( ) ;

	if ( ContName != 0 )
		container_name = ContName ;

	if ( !dbH.isValid() )
		db();

#ifdef ROGUEWAVE
	if ( containerDict.findValue(container_name,cont_h) ) { 
		if ( cont_h.isValid() ) 
			ret = oocSuccess ;
		else	// cont_h is invalidated (by abort)
			containerDict.remove(container_name);
	}
	else {
#endif

	ret = contMap()->lookup ( container_name, cont_h, oocNoOpen ) ;
	if ( cont_h.isValid() == oocFalse || ret != oocSuccess ) {
		setUpdate ( ) ;
		cont_h = new ( container_name, hash, initial_pages, CntGPct(), dbH )
					ooContObj ( ) ;
		ret = oocSuccess ;
		if (! cont_h.isValid())
			ooSignal(oocUserError,ooS_failed_container,0,"container()",(char*)container_name);
		else {
			contMap()->remove ( container_name ) ;
			contMap()->add ( container_name, cont_h ) ;
		}
	}

#ifdef ROGUEWAVE
		containerDict.insertKeyAndValue(container_name,cont_h) ;
	}
#endif

	if ( cont_h != 0 )
		ret = oocSuccess ;

	if ( persistent == 0 )
		addTempCont ( cont_h ) ;

	return ret ;
}

/*
 ******************************************************************************
 *
 *      Function Name:  ooSession::findContainer
 *
 *      Description: 
 *
 *      Returns:
 *		integer, == oocSuccess, ooContObj was found, == oocError, ooContObj was not found.
 * 
 *      Side Effects:
 *		none.
 *
 *
 ******************************************************************************
 */
int
ooSession::findContainer (
	ooHandle(ooContObj)& cont_h,
	const char* ContName )
{
	ooStatus ret = oocError ;

	if (dbH == 0) db();
	if ( ContName != 0 )
		ret = contMap()->lookup ( ContName, cont_h, oocNoOpen ) ;

	return ret ;
}

/*
 ******************************************************************************
 *
 *      Function Name:  ooSession::db
 *
 *      Description: 
 *
 *      Returns:
 *		integer, == 1, db was created, == 0, db was found.
 * 
 *      Side Effects:
 *		none.
 *
 *
 ******************************************************************************
 */
ooStatus
ooSession::db (
	ooHandle(ooDBObj)& db_h,
	const char* DBName,
	const char* DBPathName,
	const char* DBHostName )
{
	ooStatus ret = oocError ;
	const char* db_name = (const char*) 0 ;
	const char* db_hostname = (const char*) 0 ;
	const char* db_pathname = (const char*) 0 ;

	if ( DBName != 0 ) 
		db_name = DBName ;
	else
		db_name = dbFileName() ;

	if ( DBHostName != 0 ) 
		db_hostname = DBHostName ;
	else
		db_hostname = dbHostName() ;

	if ( DBPathName != 0 ) 
		db_pathname = DBPathName ;
	else
		db_pathname = dbPathName() ;

	if ( db_name != 0 || (db_h.isValid() != oocTrue) ) {
		db_h = 0 ;
//		if ( db_h.open ( fdbH, db_name, mode ) != oocSuccess ) {
		if ( db_h.exist ( fdbH, db_name, mode ) != oocTrue ) {
			db_h = new ( fdbH ) ooDBObj (
				db_name, 0, 0, db_hostname, db_pathname ) ;
			ret = oocSuccess ;
			if (db_h.isValid() != oocTrue)
				ooSignal(oocUserError,ooS_failed_db_open,0,"db()",(char*)dbFileName());
		}
	}

	if ( db_h != 0 ) 
		ret = oocSuccess ;

	return ret ;
}


/*
 ******************************************************************************
 *
 *      Function Name:  ooSession::replace_db
 *
 *      Description: 
 *
 *      Returns:
 *		integer, == 1, db was created, == 0, db was found.
 * 
 *      Side Effects:
 *		none.
 *
 *
 ******************************************************************************
 */
int
ooSession::replace_db (
	ooHandle(ooDBObj)& db_h,
	const char* DBName )
{
	int ret = 0 ;
	const char* db_name = (const char*) 0 ;

	if ( DBName == 0 ) 
		db_name = dbFileName() ;
	else
		db_name = DBName ;

	if ( db_h.exist ( fdbH, db_name, mode ) == oocTrue ) {
/*
		db_h.close ( ) ;
		ooDelete ( db_h ) ;
		db ( db_h, db_name, dbPathName(), dbHostName() ) ;
*/
		db_h = ooReplace ( ooDBObj, ( db_name, 0, 0, dbHostName(), dbPathName() ), fdbH ) ;
	}
	else {
               	db_h = new ( fdbH ) ooDBObj ( db_name, 0, 0, dbHostName(), dbPathName() ) ;
		ret = 1 ;
		if (! db_h.isValid() )
                        ooSignal(oocUserError,ooS_failed_db_open,0,"replace_db()",(char*)DBName);
	}

	return ret ;
}

/*
 ******************************************************************************
 *
 *      Function Name:  operator << ostream& ooSession&
 *
 *      Description: 
 *
 *      Returns:
 *		nothing
 * 
 *      Side Effects:
 *		none.
 *
 *
 ******************************************************************************
 */
ostream&
operator<< (
	ostream& os,
	ooSession& s_r ) 
{
	os << *(ooEnvironment*)(&s_r) << endl ;

	ooRef(ooFDObj) fd_r = s_r.fdbH ;
	os << "Federation Handle: " <<
	"[" << fd_r.get_DB() << "," << fd_r.get_OC() << ","
	<< fd_r.get_page() << "," << fd_r.get_slot() << "]" << endl ;

	ooRef(ooDBObj) db_r = s_r.dbH ;
	os << "Database Handle: " <<
	"[" << db_r.get_DB() << "," << db_r.get_OC() << ","
	<< db_r.get_page() << "," << db_r.get_slot() << "]" << endl ;

	ooRef(ooContObj) cn_r = s_r.contH ;
	os << "Container Handle: " <<
	"[" << cn_r.get_DB() << "," << cn_r.get_OC() << ","
	<< cn_r.get_page() << "," << cn_r.get_slot() << "]" << endl ;

	if ( s_r.mode == oocNoOpen ) 
		os << "Access mode is: noOpen." << endl ;
	else if ( s_r.mode == oocRead ) 
		os << "Access mode is: Read." << endl ;
	else
		os << "Access mode is: Update" << endl ;

	return os ;
}


void
ooSession::printContainerMap ( ) 
{
	// ooMapItr mapItr = contMapH ; 
	ooMapItr mapItr ( contMapH ) ; 	// needed by cxx, alphaOSF1

	while ( mapItr.next() ) {
		printf ( "%s --- ", mapItr->name() );
		mapItr->oid().print();
	}

	return ;
}



#endif /* _OOSESSION_C_ */
