//  $Id: insure_ibm.csh,v 1.3 2001/01/22 17:32:51 choutko Exp $
setenv INUSE on
setenv INUSE_PORT 79267
set ARCH=aix4
setenv INSIGHT /afs/cern.ch/pttools/Insure/insure
setenv PATH $INSIGHT/bin.${ARCH}:$PATH
if ( $?MANPATH ) then
	setenv MANPATH $INSIGHT/man:$MANPATH
else
	setenv MANPATH $INSIGHT/man
endif
if ( $?LD_LIBRARY_PATH ) then
	setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:$INSIGHT/lib.${ARCH}
else
	setenv LD_LIBRARY_PATH $INSIGHT/lib.${ARCH}
endif
rehash
