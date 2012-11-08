#ifndef DUMPSTREAMERS_HH
#define DUMPSTREAMERS_HH

#include <ostream>

class TVector3;
class TRotation;

std::ostream& operator<<( std::ostream& out, const TVector3& v );

std::ostream& operator<<( std::ostream& out, const TRotation& r );

#endif // DUMPSTREAMERS_HH
