/////////////////////////////////////////////////////////////////////// 
// ooEnvironment.h
//
// Encapsulates application timing functions, for operations, and
// accumulations.
//
// Copyright (c) 1995 Objectivity, Inc.  All Rights Reserved  	
//
// UNPUBLISHED PROPRIETARY SOURCE CODE OF OBJECTIVITY, INC.
//
// The copyright notice above does not evidence any actual
// or intended publication of such source code.
//
/////////////////////////////////////////////////////////////////////// 


#ifndef _OOENVIRONMENT_H_
#define _OOENVIRONMENT_H_

#include <stream.h>


	class ooEnv {

		private:

			void		readEnvVars ( ) ;

			ooBoolean	verbose ;	// OO_VERBOSE
			ooBoolean	runStats ;	// OO_RUNSTATS
			ooBoolean	handleStack ;	// OO_PRINT_HANDLE_STACK
			ooBoolean	dbg_txns ;	// OO_DBG_TXNS
			ooBoolean	time_txns ;	// OO_TIME_TXNS
			ooBoolean	oo_no_lock ;	// OO_NO_LOCK
			ooBoolean	oo_no_journal; 	// OO_NO_JOURNAL
			ooBoolean	oo_hot_mode ;	// OO_HOT_MODE
			ooBoolean	txn_run_stats ;	// OO_TXN_RUN_STATS
			ooBoolean	use_session_db;	// OO_SESSION_DB

			int		randDist ;	// OO_RAND_DIST
			int32		lockWait ;	// OO_LOCK_WAIT == 0, noWait, < 0 Wait, or = n
			ooIndexMode	indexMode ;	// OO_INDEX_MODE == 0 oocInsensitive, == 1 oocSensitive
			uint32		cacheIPgs ;	// OO_CACHE_INIT
			uint32		cacheMPgs ;	// OO_CACHE_MAX
			uint32		CntIPgs ;	// OO_CONT_INIT
			uint32		CntGPct ;	// OO_CONT_GROW
			uint32		rndCntIPgs ;	// OO_RAND_CONT_INIT
			uint32		rndCntGPct ;	// OO_RAND_CONT_GROW

			ooMode		mrow ;		// OO_MROW

			const char*	fdBootName ;	// OO_FD_BOOT
			const char*	dbFileName ;	// OO_DB_NAME
			const char*	dbHostName ;	// OO_DB_HOST
			const char*	dbPathName ;	// OO_DB_PATH
			const char*	contName ;	// OO_CONT_NAME

		friend	ostream& operator<< ( ostream& os, ooEnv& ) ;
		friend  class ooEnvironment ;

		protected:

		public:

		ooEnv ( ) ;
		~ooEnv ( ) ;

		static	int		getEnv ( char* inputPtr, int length, const char* ) ;
		static	int		getEnv ( ooBoolean&, const char* ) ;
		static	int		getEnv ( int&, const char* ) ;

	} ;


	class ooEnvironment {

	private:	// PRIVATE

		void		setStr ( const char**, const char* ) ;
		static		ooEnv*		eptr ;	// environment accessor.

		friend	ostream& operator<< ( ostream& os, ooEnvironment& ) ;

	protected:	// PROTECTED

		void		txn_run_stats ( ooBoolean v ) { eptr->txn_run_stats = v ; }
		ooBoolean	txn_run_stats ( ) { return eptr->txn_run_stats ; }

		void		time_txns ( ooBoolean v ) { eptr->time_txns = v ; }
		ooBoolean	time_txns ( ) { return eptr->time_txns ; }

		void		dbg_txns ( ooBoolean v ) { eptr->dbg_txns = v ; }
		ooBoolean	dbg_txns ( ) { return eptr->dbg_txns ; }

		void		verbose ( ooBoolean v ) { eptr->verbose = v ; }
		ooBoolean	verbose ( ) { return eptr->verbose ; }

		void		runStats ( ooBoolean v ) { eptr->runStats = v ; }
		ooBoolean	runStats ( ) { return eptr->runStats ; }

		void		handleStack ( ooBoolean v ) { eptr->handleStack = v ; }
		ooBoolean	handleStack ( ) { return eptr->handleStack ; }

	public:		// PUBLIC 

		void		randDist ( int v ) { eptr->randDist = v ; }
		int		randDist ( ) { return eptr->randDist ; }

		void		lockWait (int32 v ) { eptr->lockWait = v ; }
		int32		lockWait ( ) { return eptr->lockWait ; }

		void		indexMode (ooIndexMode v ) { eptr->indexMode = v ; }
		ooIndexMode	indexMode ( ) { return eptr->indexMode ; }

		void		oo_no_lock ( ooBoolean v ) { eptr->oo_no_lock = v ; }
		ooBoolean	oo_no_lock ( ) { return eptr->oo_no_lock ; }

		void		oo_no_journal ( ooBoolean v ) { eptr->oo_no_journal = v ; }
		ooBoolean	oo_no_journal ( ) { return eptr->oo_no_journal ; }

		void		oo_hot_mode ( ooBoolean v ) { eptr->oo_hot_mode = v ; }
		ooBoolean	oo_hot_mode ( ) { return eptr->oo_hot_mode ; }

		void		use_session_db ( ooBoolean v ) { eptr->use_session_db = v ; }
		ooBoolean	use_session_db ( ) { return eptr->use_session_db ; }

		void		cacheIPgs ( uint32 v ) { eptr->cacheIPgs = v ; }
		uint32		cacheIPgs ( ) { return eptr->cacheIPgs ; }

		void		cacheMPgs  ( uint32 v ) { eptr->cacheMPgs = v ; }
		uint32		cacheMPgs  ( ) { return eptr->cacheMPgs ; }

		void		CntIPgs  ( uint32 v ) { eptr->CntIPgs = v ; }
		uint32		CntIPgs  ( ) { return eptr->CntIPgs ; }

		void		CntGPct  ( uint32 v ) { eptr->CntGPct = v ; }
		uint32		CntGPct  ( ) { return eptr->CntGPct ; }

		void		rndCntIPgs  ( uint32 v ) { eptr->rndCntIPgs = v ; }
		uint32		rndCntIPgs  ( ) { return eptr->rndCntIPgs ; }

		void		rndCntGPct  ( uint32 v ) { eptr->rndCntGPct = v ; }
		uint32		rndCntGPct  ( ) { return eptr->rndCntGPct ; }

		void		mrow ( ooMode m ) { eptr->mrow = m ; }
		ooMode		mrow ( ) { return eptr->mrow ; }

		void		fdBootName ( const char* s ) { setStr(&(eptr->fdBootName),s); }
		const char*	fdBootName ( ) { return eptr->fdBootName ; }

		void		dbFileName ( const char* s ) { setStr(&(eptr->dbFileName),s); }
		const char*	dbFileName ( ) { return eptr->dbFileName ; }

		void		dbHostName ( const char* s ) { setStr(&(eptr->dbHostName),s); }
		const char*	dbHostName ( ) { return eptr->dbHostName ; }

		void		dbPathName ( const char* s ) { setStr(&(eptr->dbPathName),s); }
		const char*	dbPathName ( ) { return eptr->dbPathName ; }

		void		contName ( const char* s ) { setStr(&(eptr->contName),s); }
		const char*	contName ( ) { return eptr->contName ; }
	} ;


#endif	/* _OOENVIRONMENT_H_ */
