SHELL=/bin/sh
file= AMS
GC=g++ -Wno-deprecated
LIB_DIR := $(shell ruby -r rbconfig -e 'puts Config::CONFIG["sitedir"]')
INC_DIR := $(shell ruby -r rbconfig -e 'puts Config::CONFIG["rubylibdir"] + "/" + Config::CONFIG["arch"]')

ifndef AMSWD
AMSWD := ..
endif

GCINCS=-I$(INC_DIR) -I$(AMSWD)/include -I$(ROOTSYS)/include
LIBS=-L$(ROOTSYS)/lib -lCore -lCint -lHist -lTree -lMatrix -lMinuit \
-lGraf -lGraf3d -lGpad -lGX11 -lGX11TTF -lGui -lHistPainter -lPostscript \
-lRint -lHtml $(AMSDir)/bin/linux/icc/root_rs.o \
$(AMSDir)/bin/linux/icc/rootdict_s.o
RUBYLIB=$(LIB_DIR)/$(file).so

all : $(file).so
      
install : $(RUBYLIB)

$(RUBYLIB) : $(file).so
	cp -f $(file).so $(RUBYLIB)

$(file).so : $(file)_wrapnew.o
	$(GC) -shared $(file)_wrapnew.o -o $(file).so $(LIBS)

$(file)_wrapnew.o : $(file)_wrapnew.cxx
	$(GC) $(GCINCS) -c $(file)_wrapnew.cxx

$(file)_wrapnew.cxx : 
	./configure

clean :
	rm -f $(file)_wrap* $(file).so $(file).h
