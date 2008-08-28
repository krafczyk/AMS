//  $Id: link.C,v 1.8 2008/08/28 20:33:37 choutko Exp $
// Author V. Choutko 24-may-1996
 
#include "link.h"

integer AMSlink::testlast( ){
 if ( _next==0 || (*this < *_next) || (*_next < *this))return 1;
 else return 0;
}

integer AMSlink::operator < ( AMSlink & o) const{
  return 1;     // Dummy operator
}
