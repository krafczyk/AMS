CC=gcc 
CXX=g++ 

CFLAGS          = -Wall -g -fPIC
ROOTCFLAGS      = $(shell $(ROOTSYS)/bin/root-config --cflags)
ROOTLIBS        = $(shell $(ROOTSYS)/bin/root-config --libs)
CFLAGS         += $(ROOTCFLAGS)

AMICLIB=/Offline/AMI_clib/
XMLRPCLIB=$(AMICLIB)/xmlrpc_lib
CFLAGS+=  -I$(XMLRPCLIB)/include -I../include
XRPCC=$(XMLRPCLIB)/bin/xmlrpc-c-config

all: ConvertAMI ScanDB MergeDB ami2root

ConvertAMI: src/ConvertAMI.cc $(AMICLIB)/libAMI.a  SCDBDict.o
	$(CXX) $(CFLAGS) -o ConvertAMI src/ConvertAMI.cc $(AMICLIB)/libAMI.a `$(XRPCC) client  --libs` -Wl,--rpath -Wl,`$(XRPCC) --libdir` -I$(AMICLIB)  `$(ROOTSYS)/bin/root-config --cflags --libs` -I$(PWD)/inc src/xmlrpclib.C SCDBDict.o

ConvertAMI2: src/ConvertAMI.cc $(AMICLIB)/libAMI.a  SCDBDict.o
	$(CXX) $(CFLAGS) -o ConvertAMI src/ConvertAMI.cc $(AMICLIB)/libAMI.a `$(XRPCC) client  --libs` -Wl,--rpath -Wl,`$(XRPCC) --libdir` -I$(AMICLIB)  -L$(ROOTSYS)/lib -lRoot -lpcre -ldl  -I$(PWD)/inc src/xmlrpclib.C SCDBDict.o

ami2root: src/ami2root.C $(AMICLIB)/libAMI.a  SCDBDict.o
	$(CXX) $(CFLAGS) -static -o ami2root.exe src/ami2root.C $(AMICLIB)/libAMI.a `$(XRPCC) client  --libs` -Wl,--rpath -Wl,`$(XRPCC) --libdir` -I$(AMICLIB) -lpthread -lkrb5support `/usr/kerberos/bin/krb5-config --libs` -lselinux -lsepol -lkeyutils -lresolv `/usr/bin/libwww-config --libs` -lxmltok -L$(ROOTSYS)/lib -lRoot -lpcre -ldl -I$(PWD)/inc src/xmlrpclib.C SCDBDict.o

ScanDB: src/ScanDB.cc  SCDBDict.o
	$(CXX) $(CFLAGS) -o ScanDB src/ScanDB.cc $(ROOTCFLAGS) $(ROOTLIBS) -I$(PWD)/inc SCDBDict.o

MergeDB: src/MergeDB.C SCDBDict.o
	$(CC) -fPIC $(CFLAGS) $(ROOTLIBS) -o MergeDB src/MergeDB.C -I$(PWD)/inc SCDBDict.o

SCDBDict.cc: ../CC/SlowControlDB.C
	$(ROOTSYS)/bin/rootcint -f SCDBDict.cc -c -I../include ../CC/SlowControlDB.C inc/linkdef.h

SCDBDict.o: SCDBDict.cc SCDBDict.h
	$(CXX) $(CFLAGS) -c $<

lib/libSCDB.so: SCDBDict.o ../CC/SlowControlDB.C
	g++ $(CFLAGS) -shared -Wl,-soname,libSCDB.so -o lib/libSCDB.so SCDBDict.o

lib/libSCDB.a: SCDBDict.o ../CC/SlowControlDB.C
	ar rcv lib/libSCDB.a SCDBDict.o

clean:
	rm -f SCDBDict.*;
