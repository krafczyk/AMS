/* 
 ******************************************************************************

 *      Objectivity Inc.
 *      Copyright (c) 1995, Objectivity, Inc. USA   All rights reserved.
 *
 ****************************************************************************
 *
 *      File:   ooEnvironment.cc
 *
 *      Functions(s):
 *
 *      Description:
 *
 *      RCSid = "$Id: ooEnvironment.C,v 1.1 1996/10/18 20:58:22 choutko Exp $"
 *
 ****************************************************************************
 */


#include <oo.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "ooEnvironment.h"



ooEnv*	ooEnvironment::eptr = new ooEnv ( ) ;


void
ooEnvironment::setStr (
	const char** toPtr,
	const char* fromPtr ) 
{
	if ( toPtr ) 
		free ( (char*) *toPtr ) ;
	*toPtr = strdup ( (char*)fromPtr ) ;

	return ;
}

ostream&
operator<< (
        ostream& os,
        ooEnvironment& e_r )
{
        os << *(e_r.eptr) << endl ;

	return os ;
}

/*
 ******************************************************************************
 *
 *      Function Name:  ooEnv::ooEnv
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
ooEnv::ooEnv ( )
{
	lockWait	= oocTransNoWait ;
	indexMode	= oocInsensitive ;
	verbose		= oocFalse ;
	runStats	= oocFalse ;
	handleStack	= oocFalse ;
	dbg_txns	= oocFalse ;
	time_txns	= oocFalse ;
	oo_no_lock	= oocFalse ;
	oo_no_journal	= oocFalse ;
	oo_hot_mode	= oocTrue ;	// defaulted to true!
	txn_run_stats	= oocFalse ;
	use_session_db	= oocFalse ;
	cacheIPgs	= 100 ;
	randDist	= 1 ;
	cacheMPgs	= 250 ;
	CntIPgs		= 0 ;
	CntGPct		= 0 ;
	mrow		= oocNoMROW ;
	rndCntIPgs	= 0 ;
	rndCntGPct	= 0 ;

	fdBootName	= (char*) 0 ;
	dbFileName	= (char*) 0 ;
	dbHostName	= (char*) 0 ;
	dbPathName	= (char*) 0 ;
	contName	= "session-cont" ;

	readEnvVars ( ) ;
}

/*
 ******************************************************************************
 *
 *      Function Name:  ooEnv::~ooEnv
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
ooEnv::~ooEnv ( )
{
	if ( fdBootName )
		free ( (char*) fdBootName ) ;

	if ( dbHostName )
		free ( (char*) dbHostName ) ;

	if ( dbPathName )
		free ( (char*) dbPathName ) ;
}

/*
 ******************************************************************************
 *
 *      Function Name:  ooEnv::readEnvVars
 *
 *      Description: 
 *		reads the relevant process environment variables and sets
 *		class member values.
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
ooEnv::readEnvVars ( )
{
	int tempint ;
	ooBoolean tempBool ;
	char* cptr ;

	//	Get a name for the FD, if set in the environment.
	cptr = getenv("OO_FD_BOOT");
	if ( cptr ) 
		fdBootName = strdup(cptr);

	//	Get the hostname for the DataBase.
	cptr = getenv("OO_DB_HOST");
	if ( cptr )
		dbHostName = strdup(cptr);

	//	Get the path for the DataBase.
	cptr = getenv("OO_DB_PATH");
	if ( cptr )
		dbPathName = strdup(cptr);

	//	Get a name to start with for a DB.
	cptr = getenv("OO_DB_NAME");
	if ( cptr )
		dbFileName = strdup(cptr);

	//	Get a name for the container.
	cptr = getenv("OO_CONT_NAME");
	if (cptr)
		contName = strdup(cptr);

	//	Find out if MROW required.
	getEnv ( tempBool,"OO_MROW" ) ;
	if ( tempBool ) 
		mrow = oocMROW ;
	else
		mrow = oocNoMROW ;

	//	Find out if Lock Wait required.
	//	Set the wait timeout.
	if (getEnv(tempint,"OO_LOCK_WAIT") != 0 ) {
		if ( tempint ) 
			lockWait = tempint ;
		else if ( tempint == 0 )
			lockWait = oocNoWait ;
		else if ( tempint < 0 )
			lockWait = oocWait ;
	}
	
	//	Get transaction mode for indexes.
	if (getEnv(tempint,"OO_INDEX_MODE") != 0 ) {
		if ( tempint == 0 )
			indexMode = oocInsensitive ;
		else
			indexMode = oocSensitive ;
	}

	//	Get the Cache Initial size.
	if (getEnv(tempint,"OO_CACHE_INIT") != 0 )
		cacheIPgs = tempint ;

	//	Get the containr distribution default;
	if (getEnv(tempint,"OO_RAND_DIST") != 0 )
		randDist = tempint ;

	//	Get the Cache Max. size.
	if (getEnv(tempint,"OO_CACHE_MAX") != 0 )
		cacheMPgs = tempint ;

	//	Container Initial Size
	if (getEnv(tempint,"OO_CONT_INIT") != 0 ) 
		CntIPgs = tempint ;

	//	Container Growth Range in percentage of current size
	if (getEnv(tempint,"OO_CONT_GROW") != 0 ) 
		CntGPct = tempint ;

	//	Random Container Initial Size
	if (getEnv(tempint,"OO_RAND_CONT_INIT") != 0 ) 
		rndCntIPgs = tempint ;

	//	Random Container Growth Rage in percentage of current size
	if (getEnv(tempint,"OO_RAND_CONT_GROW") != 0 )
		rndCntGPct = tempint ;

	//	Print Transaction timing information.
	//      == 0, NO, == 3, just totals, else each txn.
	if (getEnv(tempint,"OO_TIME_TXNS") != 0 )
		time_txns = tempint ;


	/////////////////  BOOLEANS /////////////////////////

	//	Verbose mode.
	getEnv ( verbose, "OO_VERBOSE" ) ;

	//	Print Run status.
	getEnv ( runStats, "OO_RUNSTATS" ) ;

	//	Print Handle Stack.
	getEnv ( handleStack, "OO_PRINT_HANDLE_STACK" ) ;

	//	Print Transaction debug information.
	getEnv ( dbg_txns, "OO_DBG_TXNS" ) ;

	//	Disable locking and journaling et al...
	getEnv ( oo_no_lock, "OO_NO_LOCK" ) ;
	getEnv ( oo_no_journal, "OO_NO_JOURNAL" ) ;

	//	Hot mode ?
	getEnv ( oo_hot_mode, "OO_HOT_MODE" ) ;

	//	Use the session database ?
	getEnv ( use_session_db, "OO_SESSION_DB" ) ;

	//	Print ooRunStats at each txn commit ?
	getEnv ( txn_run_stats, "OO_TXN_RUN_STATS" ) ;
}

/*
 ******************************************************************************
 *
 *      Function Name:  ooEnv::getEnv
 *
 *      Description: 
 *		Gets a string value from the users envirnment based
 *		on a string.
 *
 *      Returns:
 *		1 if the environemnt variable was explicitly set.
 * 
 *      Side Effects:
 *		Defaults to "".
 *
 *
 ******************************************************************************
 */
/* STATIC */
int
ooEnv::getEnv (
	char* inputPtr,
	int length,
	const char* s )
{
	int ret_val = 0 ;

	char* val = getenv ( (char*)s ) ;

	if ( val != 0 ) {
		strncpy ( inputPtr, val, length ) ;
		ret_val = 1 ;
	}

	return ret_val ;
}


/*
 ******************************************************************************
 *
 *      Function Name:  ooEnv::getEnv
 *
 *      Description: 
 *		Gets an ooBoolean value from the users envirnment based
 *		on a string.
 *
 *      Returns:
 *		1 if the environemnt variable was explicitly set.
 * 
 *      Side Effects:
 *		Defaults to oocFalse.
 *
 *
 ******************************************************************************
 */
/* STATIC */
int
ooEnv::getEnv (
	ooBoolean& value,
	const char* s )
{
	int ret_val = 0 ;
	int actualV = 0 ;

	ret_val = getEnv ( actualV, s ) ;

	if ( actualV > 0 )
		value = oocTrue ;
	else
		value = oocFalse ;

	return ret_val ;
}

/*
 ******************************************************************************
 *
 *      Function Name:  ooEnv::getEnv
 *
 *      Description: 
 *		Gets an integer value from the users envirnment based
 *		on a string.
 *
 *      Returns:
 *		1 if the environemnt variable was explicitly set.
 * 
 *      Side Effects:
 *		Defaults to 0.
 *
 *
 ******************************************************************************
 */
/* STATIC */
int
ooEnv::getEnv (
	int& value,
	const char* s )
{
	int ret_val = 0 ;

	char* val = getenv ( (char*)s ) ;

	if ( val != 0 ) {
		value = atoi ( val ) ;
		ret_val = 1 ;
	}

	return ret_val ;
}


/*
 ******************************************************************************
 *
 *      Function Name:  operator << ostream& ooEnv&
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
	ooEnv& s_r ) 
{
	os << "Federation: " << s_r.fdBootName << endl ;
	os << "Database: " << s_r.dbFileName << endl ;
	if ( s_r.dbHostName != 0 )
		os << "database Host: " << s_r.dbHostName << endl ;
	if ( s_r.dbPathName != 0 )
		os << "database Path: " << s_r.dbPathName << endl ;
	os << "Container: " << s_r.contName << endl ;

	if ( s_r.mrow == oocMROW ) 
		os << "Transaction Mode is: MROW" << endl ;
	else
		os << "Transaction Mode is: noMROW" << endl ;

	if ( s_r.indexMode == oocInsensitive )
		os << "Index mode is \"Insensitive\"" << endl ;
	else
	if (  s_r.indexMode == oocSensitive )
		os << "Index mode is \"Sensitive\"" << endl ;
	else
		os << "Index mode is defaulted." << endl ;

	os << "LockWaiting is: " << s_r.lockWait << endl ;

	os << "Initial pages in SM cache: " << s_r.cacheIPgs << endl ;
	os << "Max SM cache size: " << s_r.cacheMPgs << endl ;

	os << "Number of pages init for new containers: " << s_r.CntIPgs << endl ;
	os << "Growth percent for containers: " << s_r.CntGPct << endl ;

	os << "Number of pages init for new random containers: " << s_r.rndCntIPgs << endl ;
	os << "Growth percent for random containers: " << s_r.rndCntGPct << endl ;
 
	return os ;
}

