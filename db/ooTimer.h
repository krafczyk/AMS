//  $Id: ooTimer.h,v 1.2 2001/01/22 17:32:30 choutko Exp $
/////////////////////////////////////////////////////////////////////// 
// ooTimer.h
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


#ifndef _OOTIMER_H_
#define _OOTIMER_H_

#include <oo.h>
#ifdef __ALPHAOSF1
#include <sys/timers.h>
#else
#include <sys/time.h>
#endif


extern "C" int getclock(int clock_type, struct timespec *tp);

	class ooTimer {

		private:	// PRIVATE

			// general operation timers.
			//
#ifdef __ALPHAOSF1
		struct	timespec	startTime ;
		struct	timespec	endTime ;
#else
		struct	timeval		startTime ;
		struct	timeval		endTime ;
	static	struct	timezone	tz ;    // just used for timer.
#endif

		protected:	// PROTECTED

			ooBoolean	streamIO ;

			// Transaction operation timers.
			//
#ifdef __ALPHAOSF1
			long		cmpTime ( const timespec&, const timespec& ) ;
	static 	struct	timespec	txn_startT ;
	static 	struct	timespec	txn_endT ;
#else
			long		cmpTime ( const timeval&, const timeval& ) ;
	static	struct	timeval		txn_startT ;
	static	struct	timeval		txn_endT ;
#endif

			// Transaction timing counters.
			//
	static		unsigned	update_msecs ;
	static		unsigned 	no_updates ;
	static		unsigned	read_msecs ;
	static		unsigned	no_reads ;

			// Transaction total timing counters.
			//
	static		unsigned	tot_update_msecs ;
	static		unsigned 	tot_no_updates ;
	static		unsigned	tot_read_msecs ;
	static		unsigned	tot_no_reads ;


			// Transaction timing, internal start/stop.
			//
#ifdef __ALPHAOSF1
			void		set_txn_endT ( ) {
						getclock(TIMEOFDAY,&txn_endT); }
			void		set_txn_startT ( ) {
						getclock(TIMEOFDAY,&txn_startT); }
#else
			void		set_txn_endT ( ) {
						gettimeofday(&txn_endT,&tz); }
			void		set_txn_startT ( ) {
						gettimeofday(&txn_startT,&tz); }
#endif

		public:		// PUBLIC 

			ooTimer ( ) {
				streamIO		= oocTrue ;
				startTime.tv_sec	= 0 ;
				endTime.tv_sec		= 0 ;
#ifdef __ALPHAOSF1
				startTime.tv_nsec	= 0 ;
				endTime.tv_nsec		= 0 ;
#else
				startTime.tv_usec	= 0 ;
				endTime.tv_usec		= 0 ;
#endif
			}

			// General timer control and access functions
			//	
#ifdef __ALPHAOSF1
			void		set_endTime ( ) {
						getclock(TIMEOFDAY,&endTime); }
			void		set_startTime ( ) {
						getclock(TIMEOFDAY,&startTime); }
#else
			void		set_endTime ( ) {
						gettimeofday(&endTime,&tz); }
			void		set_startTime ( ) {
						gettimeofday(&startTime,&tz); }
#endif

			// Transaction timing, reset group.
			//
			void		reset_txnTimer ( ) {
						update_msecs = 0 ;
						no_updates = 0 ;
						read_msecs = 0 ;
						no_reads = 0 ;
					}

			// General operation printing function.
			//
		virtual void		print_time ( const char* text ) ;

			// Transaction operation printing function.
			//
		virtual void		print_txn_time ( ooMode, const char* text ) ;

	} ;


////////////////////////////  INLINE FUNCTIONS  //////////////////////////////////////

#define MILLISEC 1000
#define MICROSEC 1000000

#ifdef __ALPHAOSF1
inline
long
ooTimer::cmpTime (
	const timespec& start_ts,
	const timespec& end_ts )
#else
inline
long
ooTimer::cmpTime (
	const timeval& start_ts,
	const timeval& end_ts )
#endif
{
	long ret_val = 0 ;

#ifdef __ALPHAOSF1
	long mills = (end_ts.tv_sec - start_ts.tv_sec) * MILLISEC ;
	long mics = (end_ts.tv_nsec - start_ts.tv_nsec ) / MILLISEC ;
#else
	long mills = (end_ts.tv_sec - start_ts.tv_sec) * MILLISEC ;
	long mics = (end_ts.tv_usec - start_ts.tv_usec ) / MILLISEC ;
#endif
	ret_val = mills + mics ;

	return ret_val ;
}

inline
void
ooTimer::print_time (
	const char* text )
{
	if ( streamIO )
		cout << text << " Elapsed Time: " <<
		cmpTime(startTime,endTime) << " msec." << endl ;
	else 
		printf ( "%s Elapsed Time: %d msec.\n", text, cmpTime(startTime,endTime) ) ;
}

inline
void
ooTimer::print_txn_time ( 
	ooMode mode,
	const char* text )
{
	if ( streamIO )
		cout << text <<
			(mode==oocUpdate ? " update" : " read") <<
			" msec: " <<
			cmpTime(txn_startT,txn_endT) <<
			" cum average: " <<
			(mode==oocUpdate ? (update_msecs/no_updates) : (read_msecs/no_reads)) <<
			endl ;
	else
		printf("%s%s msec: %d  cum average: %d\n",
			(text==0 ? "" : text),
			(mode==oocUpdate ? " update" : " read"),
			cmpTime(txn_startT,txn_endT),
			(mode==oocUpdate ? (update_msecs/no_updates) : (read_msecs/no_reads)) ) ;
}


#endif	/* _OOTIMER_H_ */
