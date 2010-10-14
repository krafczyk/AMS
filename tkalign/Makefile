# $Id: Makefile,v 1.1 2010/10/14 09:28:03 shaino Exp $
#
# Makefile to build executables for Tracker alignment with TB/CR
#
# bin/tbalign: Generate a TTree for TB alignment from AMSRoot files
# bin/pmalign: Generate a TTree for CR alignment from AMSRoot files
#
# bin/tbafit : Tracker alignment with tbalign.root files
# bin/pmafit : Tracker alignment with pmalign.root files
#
# 2010/10/14 SH


ARCH = linux
DEFS = -D_PGTRACK_ -D__ROOTSHAREDLIBRARY__
FLGS = -I$(ROOTSYS)/include -I../include
OPTS = -O3
LIBS = $(AMSWD)/lib/$(ARCH)/libntuple_slc4_PG.a \
       -L$(ROOTSYS)/lib -lRoot -lXrdPosix -lpcre
MODS = tbalign tbafit tbaf tbafm tbafcp pmalign pmafit pmaf pmafcp

BINS = $(addprefix bin/, $(MODS));

all: $(BINS)

bin/%: CC/m_%.C
	@if ! [ -d bin ] ; then mkdir -p bin; fi
	$(CXX) -o $@ $(DEFS) $(FLGS) $(OPTS) $< $(LIBS)

clean:
	rm -f bin/*

bin/tbalign: CC/tbalign.C CC/tbpos.C
bin/tbafit : CC/tbafit.C  CC/tkalign.C
bin/tbaf   : CC/tbafit.C  CC/tkalign.C
bin/tbafm  : CC/tbafit.C  CC/tkalign.C
bin/tbafcp : CC/tbafit.C  CC/tkalign.C

bin/pmalign: CC/pmalign.C
bin/pmafit : CC/pmafit.C CC/tkalign.C
bin/pmaf   : CC/pmafit.C CC/tkalign.C
bin/pmafcp : CC/pmafit.C CC/tkalign.C
