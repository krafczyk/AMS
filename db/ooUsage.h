//  $Id: ooUsage.h,v 1.2 2001/01/22 17:32:30 choutko Exp $
#ifndef _ooUsage_h_
#define _ooUsage_h_

#include <sys/time.h>
#include <sys/resource.h>
#include <stream.h>
          

extern "C" int getrusage ( int, struct rusage* ) ;

class ooUsage {

	private:

#ifdef ALPHAOSF1
	public:
#endif

		struct rusage _usage ;

#ifndef ALPHAOSF1
		friend  ostream& operator<< ( ostream& os, ooUsage& ) ;
#endif

	protected:

	public:

		ooUsage ( ) { getrusage ( RUSAGE_SELF, &_usage ) ; }

} ;


inline 
ostream&
operator<< (
	ostream& os,
	ooUsage& ru ) 
{
	long ru_utime_t = (long)(ru._usage.ru_utime.tv_sec*1000) + (long)(ru._usage.ru_utime.tv_usec/1000) ;
	long ru_stime_t = (long)(ru._usage.ru_stime.tv_sec*1000) + (long)(ru._usage.ru_stime.tv_usec/1000) ;

	os << "user time used: " << ru_utime_t << endl ;
	os << "system time used: " << ru_stime_t << endl ;
	os << "total cpu time used: " << ru_utime_t + ru_stime_t << endl ;

	os << "maximum resident set size: " << ru._usage.ru_maxrss << endl ;
//	os << "currently 0 " << ru._usage.ru_ixrss << endl ;
	os << "integral resident set size: " << ru._usage.ru_idrss  << endl ;

//	os << "currently 0 " << ru._usage.ru_isrss << endl ;
	os << "page faults not requiring physical I/O: " << ru._usage.ru_minflt << endl ;
	os << "page faults requiring physical I/O: " << ru._usage.ru_majflt << endl ;

	os << "swaps: " << ru._usage.ru_nswap << endl ;
	os << "block input operations: " << ru._usage.ru_inblock << endl ;
	os << "block output operations: " << ru._usage.ru_oublock << endl ;

	os << "messages sent: " << ru._usage.ru_msgsnd << endl ;
	os << "messages received: " << ru._usage.ru_msgrcv << endl ;
	os << "signals received: " << ru._usage.ru_nsignals << endl ;

	os << "voluntary context switches: " << ru._usage.ru_nvcsw << endl ;
	os << "involuntary context switches: " << ru._usage.ru_nivcsw << endl ;

	return os ;
}
 

#endif /* _ooUsage_h_ */



