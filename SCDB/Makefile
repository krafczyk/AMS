CC=gcc 
CXX=g++ 

CFLAGS          = -Wall -O2 -fPIC
ROOTCFLAGS      = $(shell $(ROOTSYS)/bin/root-config --cflags)
ROOTLIBS        = $(shell $(ROOTSYS)/bin/root-config --libs)
CFLAGS         += $(ROOTCFLAGS)

AMICLIB=/f7dah1/aachen/users/mmilling/analysis/AMI/AMI_clib/
XMLRPCLIB=$(AMICLIB)/xmlrpc_lib
CFLAGS+=  -I$(XMLRPCLIB)/include
XRPCC=$(XMLRPCLIB)/bin/xmlrpc-c-config

ifeq ($(MODE), test)
CFLAGS += -DTEST -Wall -Wno-write-strings
endif

all: ConvertAMI ScanDB MergeDB ami2root

ConvertAMI: src/ConvertAMI.cc $(AMICLIB)/libAMI.a  SCDBDict.o
	$(CXX) $(CFLAGS) -o ConvertAMI src/ConvertAMI.cc $(AMICLIB)/libAMI.a `$(XRPCC) client  --libs` -Wl,--rpath -Wl,`$(XRPCC) --libdir` -I$(AMICLIB)  `$(ROOTSYS)/bin/root-config --cflags --libs` -I$(PWD)/inc src/xmlrpclib.C SCDBDict.o

ConvertAMI2: src/ConvertAMI.cc $(AMICLIB)/libAMI.a  SCDBDict.o
	$(CXX) $(CFLAGS) -o ConvertAMI src/ConvertAMI.cc $(AMICLIB)/libAMI.a `$(XRPCC) client  --libs` -Wl,--rpath -Wl,`$(XRPCC) --libdir` -I$(AMICLIB)  -L$(ROOTSYS)/lib -lRoot -lpcre -ldl  -I$(PWD)/inc src/xmlrpclib.C SCDBDict.o

ami2root: src/ami2root.C $(AMICLIB)/libAMI.a  SCDBDict.o
	$(CXX) $(CFLAGS) -o ami2root.exe src/ami2root.C $(AMICLIB)/libAMI.a `$(XRPCC) client  --libs` -Wl,--rpath -Wl,`$(XRPCC) --libdir` -I$(AMICLIB)  -L$(ROOTSYS)/lib -lRoot -lpcre -ldl -I$(PWD)/inc src/xmlrpclib.C SCDBDict.o

ScanDB: src/ScanDB.cc  SCDBDict.o
	$(CXX) $(CFLAGS) -o ScanDB src/ScanDB.cc -L$(ROOTSYS)/lib -lRoot -lpcre -ldl -I$(PWD)/inc SCDBDict.o

MergeDB: src/MergeDB.C SCDBDict.o
	$(CC) -fPIC $(CFLAGS) $(ROOTLIBS) -o MergeDB src/MergeDB.C -I$(PWD)/inc SCDBDict.o

SCDBDict.cc: inc/SlowControlDB.h
	$(ROOTSYS)/bin/rootcint -f SCDBDict.cc -c inc/SlowControlDB.h inc/linkdef.h

SCDBDict.o: SCDBDict.cc SCDBDict.h
	$(CXX) $(CFLAGS) -c $<

lib/libSCDB.so: SCDBDict.o src/SlowControlDB.cc
	g++ $(CFLAGS) -shared -Wl,-soname,libSCDB.so -o lib/libSCDB.so SCDBDict.o

lib/libSCDB.a: SCDBDict.o src/SlowControlDB.cc
	ar rcv lib/libSCDB.a SCDBDict.o

clean:
	rm -f SCDBDict.*;