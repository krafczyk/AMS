//  $Id: ooSession.h,v 1.5 2001/01/22 17:32:30 choutko Exp $
/////////////////////////////////////////////////////////////////////// 
// ooSession.h
//
// Encapsulates application specific databases, containers, 
// and allows shell environment variables to control the run of an application
//
// Copyright (c) 1995 Objectivity, Inc.  All Rights Reserved  	
//
// UNPUBLISHED PROPRIETARY SOURCE CODE OF OBJECTIVITY, INC.
//
// The copyright notice above does not evidence any actual
// or intended publication of such source code.
//
/////////////////////////////////////////////////////////////////////// 


#ifndef _OOSESSION_H_
#define _OOSESSION_H_

#define OO_VERSION3X
/* #define ROGUEWAVE */


#include <oo.h>
#include <stdio.h>
#include <iostream.h>
#include <setjmp.h>
#include "string.h"

#include "ooStats.h"
#include "ooTimer.h"
#include "ooEnvironment.h"

#ifdef ROGUEWAVE
#include <rw/tvhdict.h>
#include <rw/cstring.h>
#endif


extern "C" int getclock(int clock_type, struct timespec *tp);

declare(ooVArray,ooRef(ooContObj))

	class ooSession : public ooTimer, public ooEnvironment {

		private:	// PRIVATE

	static		unsigned		refCount ;
	static		ooStats*		stats_p ;

	static 		ooHandle(ooFDObj)	fdbH ;	// Federated Database in use
	static		ooHandle(ooDBObj)	dbH ; 	
	static		ooHandle(ooContObj)	contH ;	

	// these next three are used only by the session class to support persistent
	// session configuration and object lookup.
	static		ooHandle(ooDBObj)	sessiondbH ;   // private session db.
	static		ooHandle(ooContObj)	sessionContH ; // private session container.
	static		ooHandle(ooMap)		contMapH ; // private session container.

	static		unsigned short		transactionCount ; // used for transaction stack.
	static		unsigned short		init_called ; // set to 1 when ooInit called.


#ifdef ROGUEWAVE
			// hash table for caching container handles.
	static		RWTValHashDictionary<RWCString, ooHandle(ooObj)> containerDict ;
#endif
#ifdef OO_VERSION3X
	static		ooAMSUsage		amsUsage ;
#endif
			int		persistent ;	// boolean for temp vs persistent.

			uint32		lastErrCode ;	// The last error code, before commit
			char*		lastErrString ;	// The last error string, before commit

	static		ooTrans		transaction ;	// Transaction Control parameters
			ooMode		mode ;		// defines mode.

			ooVArray(ooRef(ooContObj)) contR ;	// array of temp containers

			void		addTempCont ( ooHandle(ooContObj)& ) ;
			int		setMode ( ) ;

			void		internalCommit ( ) ;

			void		setup_session_db ( ooHandle(ooDBObj)& ) ;
			void		setup_session_map ( ooHandle(ooDBObj)& ) ;
			ooHandle(ooMap)&	contMap();

	friend		ostream&	operator<< ( ostream& os, ooSession& ) ;

		public:		// PUBLIC 

			ooMode		Mode() {return mode;}
			ooSession ( ooMode read_write = oocRead ) ;	// oocRead -or- oocUpdate	
		virtual	~ooSession ( ) ;

			ooTrans&	get_trans ( ) { return transaction ; }
		static	int		in_trans ( ) { return transaction.isActive(); }

		static	int initialized ( ) { return init_called ; }

#ifdef OO_VERSION3X
			// call before Init, if needed to change the default.
			void setAMSUsage ( ooAMSUsage ams_flag = oocAMSPreferred ) {
						amsUsage = ams_flag ;
					}
#endif

			void Init ( ) ;			// Must always called.
			int setRead ( ) ;		// Change mode to read.
			int setUpdate ( ) ;		// Change mode to update.

			void setTransient ( )	{ persistent = 0 ; }
			void setPersistent ( )	{ persistent = 1 ; }

			// start transaction, select MROW, add tag for timing as option.
			ooStatus TransStart ( ooMode md = oocNoMROW, const char* tag = 0 ) ;

			// commit transaction.
			ooStatus TransCommit ( ) ;

			// !!!!!   CHANGED TransCommit FUNCTION   !!!!!
			// abort transaction.
			ooStatus TransAbort ( ) ;

			uint32	lastErrorNo ( ) { return lastErrCode ; }
			const char*	lastErrorString ( ) { return lastErrString ; }

			ooHandle(ooFDObj)&	fd ( )	{ return fdbH ; }

			ooHandle(ooDBObj)&	db ( const char* DBName = 0,
						const char* DBPathName = 0,
						const char* DBHostName = 0 ) ;

			ooHandle(ooDBObj)&	replace_db ( const char* DBName = 0 ) ;

			ooHandle(ooContObj)&	container (
						int ContainerNumber,
						const char* Prefix = 0,
						uint32 hash = 0,
						uint32 initPages = 4 ) ;

			ooHandle(ooContObj)&	random ( int MaxDistribution,
						const char* Prefix = 0,
						uint32 hash = 0,
						uint32 initPages = 4 ) ;

			ooHandle(ooContObj)&	container (
						const char* ContName = 0,
						uint32 hash = 0,
						uint32 initialPages = 4 ) ;

			ooHandle(ooContObj)	tempCont ( const char* ContName = 0,
						uint32 hash = 0,
						uint32 initPages = 0 ) ;

			// returns 1 if object was created with this call, else 0.
			// check the first argument reference for nil to check for success/failure.
			ooStatus	db ( ooHandle(ooDBObj)&,
					const char* DBName = 0,
					const char* DBPathName = 0,
					const char* DBHostName = 0 ) ;

			int	replace_db ( ooHandle(ooDBObj)&, const char* DBName = 0 ) ;

			int	container ( ooHandle(ooContObj)&,
					int ContainerNumber,
					const char* Prefix = 0,
					uint32 hash = 0,
					uint32 initPages = 4 ) ;

			int	random    ( ooHandle(ooContObj)&,
					int MaxDistribution,
					const char* Prefix = 0,
					uint32 hash = 0,
					uint32 initPages = 4 ) ;

			int	container ( ooHandle(ooContObj)&,
					const char* ContName = 0,
					uint32 hash = 0,
					uint32 initPages = 4 ) ;

			int	tempCont  ( ooHandle(ooContObj)&,
					const char* ContName = 0,
					uint32 hash = 0,
					uint32 initPages = 4 ) ;

			// this next returns oocSuccess if found, else oocError
			int	findContainer ( ooHandle(ooContObj)&, const char* ContName ) ;

			// diagnostics
			//
			void stats ( ) ;		// Optionally print run status ;
			void printHandleStack ( int level = 2 ) ; // print handle stack.

			void noop ( ) { } 

			void printContainerMap ( ) ;

			uint32		error ( ) { return lastErrCode ; }

			void		setStreamIO ( ) { streamIO = oocTrue ; }
			void		setCIO ( ) { streamIO = oocFalse ; }
	} ;





////////////////////////////  INLINE FUNCTIONS  //////////////////////////////////////

inline
ooHandle(ooDBObj)&
ooSession::db (
	const char* DBName, 
	const char* DBPathName, 
	const char* DBHostName )
{
	db ( dbH, DBName, DBPathName, DBHostName ) ;

	return dbH ;
}

inline
ooHandle(ooDBObj)&
ooSession::replace_db (
	const char* DBName )
{
	replace_db ( dbH, DBName ) ;

	return dbH ;
}

inline
ooHandle(ooContObj)&
ooSession::container (
	int ContainerNumber,
	const char* Prefix,
	uint32 hash,
	uint32 initPages )
{
	container ( contH, ContainerNumber, Prefix, hash, initPages ) ;

	return contH ;
}

inline
ooHandle(ooContObj)&
ooSession::random (
	int MaxDistribution,
	const char* Prefix,
	uint32 hash,
	uint32 initPages )
{
	random ( contH, MaxDistribution, Prefix, hash, initPages ) ;

	return contH ;
}

inline
ooHandle(ooContObj)&
ooSession::container (
	const char* ContName,
	uint32 hash,
	uint32 initPages )
{
	container ( contH, ContName, hash, initPages ) ;

	return contH ;
}


#ifdef _OOSESSION_C_
 
ooError ooS_failed_fd_reopen = {
	190100, "ooSession::%s failed to re-open federation in new mode %d." } ;
 
ooError ooS_failed_init = {
	190101, "ooSession::%s failure in ooInit." } ;
 
ooError ooS_failed_fd_open = {
	190102, "ooSession::%s could not open: <%s> is OO_FD_BOOT set correctly ?" } ;
 
ooError ooS_failed_db_open = {
	190103, "ooSession::%s could not open database <%s>." } ;
 
ooError ooS_failed_Sdb_open = {
	190104, "ooSession::%s could not open ooSession db <%s> is OO_SESSION_DB set correctly ?" } ;
 
ooError ooS_failed_session_map = {
	190105, "ooSession::%s could not get ooSession container map because db <%s> is was not opened." } ;
 
ooError ooS_failed_container = {
	190106, "ooSession::%s handle is invalid for <%s>." } ;
 
#endif /* _OOSESSION_C_ */


#endif	/* _OOSESSION_H_ */



