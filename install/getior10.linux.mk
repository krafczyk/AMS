include $(ORACLE_HOME)/precomp/lib/env_precomp.mk
#
PCINCLUDE=/usr/include/c++/4.1.1/backward,/usr/include/c++/4.1.1
OBJS=getior.o
EXE=../exe/linux/getior.exe
BIN=../bin/linux/getior.c
PCCSRC=../CC/getior.pc

cppbuild: 
	$(PROC) $(PROCPPFLAGS) INCLUDE=$(PCINCLUDE) iname=$(PCCSRC) oname=$(BIN)
	$(CPLUSPLUS) -m32 -w -g -c -fpermissive -I/usr/include  -I../include  -I/usr/local/include/g++  $(INCLUDE) $(BIN)
	$(CPLUSPLUS) -m32 -static -w -o $(EXE) $(OBJS) -L$(LIBHOME) -L/afs/cern.ch/ams/oracle/afs/cern.ch/project/oracle/linux/9206/lib/ -lclntst10g $(STATICCPPLDLIBS) -pthread /Offline/vdev/bin/linux/ctype.o

$(CPPSAMPLES): cppdemo2
	$(MAKE) -f $(MAKEFILE) OBJS=$@.o EXE=$@ cppbuild
#
# The macro definition fill in some details or override some defaults from 
# other files.
#
OTTFLAGS=$(PCCFLAGS)
CLIBS= $(TTLIBS_QA) $(STATICCPPLDLIBS)
PRODUCT_LIBHOME=
MAKEFILE=./getior.linux.mk
PROCPLSFLAGS= sqlcheck=full userid=$(USERID) 
PROCPPFLAGS= code=cpp $(CPLUS_SYS_INCLUDE)
USERID=scott/tiger
NETWORKHOME=$(ORACLE_HOME)/network/
PLSQLHOME=$(ORACLE_HOME)/plsql/
INCLUDE_PC= $(I_SYM)$(PRECOMPHOME)syshdr $(I_SYM)$(PRECOMPHOME)public $(I_SYM)$(RDBMSHOME)public $(I_SYM)$(RDBMSHOME)demo $(I_SYM)$(PLSQLHOME)public $(I_SYM)$(NETWORKHOME)public
INCLUDE= $(I_SYM)$(PRECOMPHOME)public $(I_SYM)$(RDBMSHOME)public $(I_SYM)$(RDBMSHOME)demo $(I_SYM)$(PLSQLHOME)public $(I_SYM)$(NETWORKHOME)public
