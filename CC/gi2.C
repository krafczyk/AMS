//  $Id: gi2.C,v 1.5 2005/05/17 09:54:04 pzuccon Exp $
#include "geantnamespace.h"
int Error;
extern "C" void shl_getsymbols(){
}
extern "C" void shl_findsym(){
}
extern "C" void shl_load(){
}
extern "C" void shl_unload(){
}
extern "C" void shl_get(){
}
extern "C" void uginit_(){
int argc=0;
char **argv=0;
gams::UGINIT(argc,argv);
}
extern "C" void uglast_(){
gams::UGLAST();
}
