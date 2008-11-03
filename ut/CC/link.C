//  $Id: link.C,v 1.9 2008/11/03 14:10:40 pzuccon Exp $
// Author V. Choutko 24-may-1996
 
#include "link.h"

integer AMSlink::testlast( ){
 if ( _next==0 || (*this < *_next) || (*_next < *this))return 1;
 else return 0;
}

integer AMSlink::operator < ( AMSlink & o) const{
  return 1;     // Dummy operator
}
