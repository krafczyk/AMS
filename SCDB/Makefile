#CC=gcc 
#CXX=g++ 

SLC6system=
ifneq "$(wildcard /etc/redhat-release)" ""
 SLC6system := $(shell cat /etc/redhat-release | grep "Scientific Linux CERN SLC release 6")
endif

CFLAGS          = -Wall  -fPIC -O2
CFLAGSD         = -Wall -O2 -fPIC  
ROOTCFLAGS      = $(shell $(ROOTSYS)/bin/root-config --cflags)
ROOTLIBS        = $(shell $(ROOTSYS)/bin/root-config --libs)
CFLAGS          += $(ROOTCFLAGS)
CFLAGSD         += $(ROOTCFLAGS)

ifdef SLC6system
  KRB5LIB=/afs/cern.ch/ams/local2/opt/krb5-1.11.2_distro/_slc6/_static64
  WWWLIB=/afs/cern.ch/ams/local2/opt/w3c-libwww-5.4.0_distro/_slc6/_64
else
  SLC5system=1
endif

AMICLIB=/Offline/AMI_clib/
XMLRPCLIB=$(AMICLIB)/xmlrpc_lib
CFLAGS+=  -I$(XMLRPCLIB)/include -I../include
CFLAGSD+=  -I$(XMLRPCLIB)/include -I../include
XRPCC=$(XMLRPCLIB)/bin/xmlrpc-c-config

all: ConvertAMI ScanDB MergeDB ami2root CheckDB

ConvertAMI: src/ConvertAMI.cc $(AMICLIB)/libAMI.a  SCDBDict.o
ifndef SLC5system
	$(CXX) $(CFLAGS) -o ConvertAMI src/ConvertAMI.cc $(AMICLIB)/libAMI.a `$(XRPCC) client  --libs` -Wl,--rpath -Wl,`$(XRPCC) --libdir` -I$(AMICLIB)  `$(ROOTSYS)/bin/root-config --cflags --libs` -I$(PWD)/inc src/xmlrpclib.C SCDBDict.o -lresolv `$(WWWLIB)/bin/libwww-config --libs` -lxmltok 
else
	$(CXX) $(CFLAGS) -o ConvertAMI src/ConvertAMI.cc $(AMICLIB)/libAMI.a `$(XRPCC) client  --libs` -Wl,--rpath -Wl,`$(XRPCC) --libdir` -I$(AMICLIB)  `$(ROOTSYS)/bin/root-config --cflags --libs` -I$(PWD)/inc src/xmlrpclib.C SCDBDict.o
endif	

ConvertAMI2: src/ConvertAMI.cc $(AMICLIB)/libAMI.a  SCDBDict.o
	$(CXX) $(CFLAGS) -o ConvertAMI src/ConvertAMI.cc $(AMICLIB)/libAMI.a `$(XRPCC) client  --libs` -Wl,--rpath -Wl,`$(XRPCC) --libdir` -I$(AMICLIB)  -L$(ROOTSYS)/lib -lRoot -lpcre -ldl  -I$(PWD)/inc src/xmlrpclib.C SCDBDict.o

ami2root: src/ami2root.C $(AMICLIB)/libAMI.a  SCDBDict.o
ifndef SLC5system
	$(CXX) $(CFLAGS) -static -o ami2root.exe src/ami2root.C $(AMICLIB)/libAMI.a `$(XRPCC) client  --libs` -Wl,--rpath -Wl,`$(XRPCC) --libdir` -I$(AMICLIB) -lpthread -L$(KRB5LIB)/lib -lkrb5support `$(KRB5LIB)/bin/krb5-config --libs` -lselinux -lsepol -lkeyutils -lresolv `$(WWWLIB)/bin/libwww-config --libs` -lxmltok -L$(ROOTSYS)/lib -lRoot -lpcre -ldl -I$(PWD)/inc src/xmlrpclib.C SCDBDict.o
else
	$(CXX) $(CFLAGS) -static -o ami2root.exe src/ami2root.C $(AMICLIB)/libAMI.a `$(XRPCC) client  --libs` -Wl,--rpath -Wl,`$(XRPCC) --libdir` -I$(AMICLIB) -lpthread -lkrb5support `/usr/kerberos/bin/krb5-config --libs` -lselinux -lsepol -lkeyutils -lresolv `/usr/bin/libwww-config --libs` -lxmltok -L$(ROOTSYS)/lib -lRoot -lpcre -ldl -I$(PWD)/inc src/xmlrpclib.C SCDBDict.o
endif	

checka2r: src/checka2r.C $(AMICLIB)/libAMI.a  SCDBDictg.o
ifndef SLC5system
	$(CXX) $(CFLAGSD)  -static -o checka2r.exe src/checka2r.C src/xmlrpclib.C SCDBDictg.o $(AMICLIB)/libAMI.a `$(XRPCC) client  --libs` -Wl,--rpath -Wl,`$(XRPCC) --libdir` -I$(AMICLIB) -lpthread -L$(KRB5LIB)/lib -lkrb5support `$(KRB5LIB)/bin/krb5-config --libs` -lselinux -lsepol -lkeyutils -lresolv `$(WWWLIB)/bin/libwww-config --libs` -lxmltok -L$(ROOTSYS)/lib -lRoot -lpcre -ldl -I$(PWD)/inc 
else
	$(CXX) $(CFLAGSD)  -static -o checka2r.exe src/checka2r.C src/xmlrpclib.C SCDBDictg.o $(AMICLIB)/libAMI.a `$(XRPCC) client  --libs` -Wl,--rpath -Wl,`$(XRPCC) --libdir` -I$(AMICLIB) -lpthread -lkrb5support `/usr/kerberos/bin/krb5-config --libs` -lselinux -lsepol -lkeyutils -lresolv `/usr/bin/libwww-config --libs` -lxmltok -L$(ROOTSYS)/lib -lRoot -lpcre -ldl -I$(PWD)/inc 
endif	

ScanDB: src/ScanDB.cc  SCDBDict.o
	$(CXX) $(CFLAGS) -o ScanDB src/ScanDB.cc $(ROOTCFLAGS) $(ROOTLIBS) -I$(PWD)/inc SCDBDict.o

CheckDB: src/CheckDB.cc  SCDBDict.o
	$(CXX) $(CFLAGS) -o CheckDB src/CheckDB.cc $(ROOTCFLAGS) $(ROOTLIBS) -I$(PWD)/inc SCDBDict.o

MergeDB: src/MergeDB.C SCDBDict.o
	$(CC) -fPIC $(CFLAGS) $(ROOTLIBS) -o MergeDB src/MergeDB.C -I$(PWD)/inc SCDBDict.o

SCDBDict.cc: ../CC/SlowControlDB.C
	$(ROOTSYS)/bin/rootcint -f SCDBDict.cc -c -I../include ../CC/SlowControlDB.C inc/linkdef.h

SCDBDict.o: SCDBDict.cc SCDBDict.h
	$(CXX) $(CFLAGS) -c $<

SCDBDictg.o: SCDBDict.cc SCDBDict.h
	$(CXX) $(CFLAGSD)  -c $<   -o SCDBDictg.o

lib/libSCDB.so: SCDBDict.o ../CC/SlowControlDB.C
	g++ $(CFLAGS) -shared -Wl,-soname,libSCDB.so -o lib/libSCDB.so SCDBDict.o

lib/libSCDB.a: SCDBDict.o ../CC/SlowControlDB.C
	ar rcv lib/libSCDB.a SCDBDict.o

clean:
	rm -f SCDBDict*.*;
