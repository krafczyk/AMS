setenv INUSE on
setenv INUSE_PORT 79267
set ARCH=rs6000
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
