SHELL=/bin/sh
MY_OS := $(shell uname | tr '[A-Z]' '[a-z]')
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
-lRint -lHtml $(AMSWD)/bin/linux/icc/root_rs.o \
$(AMSWD)/bin/linux/icc/rootdict_s.o
MYLIB=$(MY_OS)/$(file).so
RUBYLIB=$(LIB_DIR)/$(file).so

all : $(MYLIB)
      
install : $(RUBYLIB)

$(RUBYLIB) : $(MYLIB)
	cp -f $(MYLIB) $(RUBYLIB)

$(MYLIB) : $(file)_wrapnew.o
	$(GC) -shared $(file)_wrapnew.o -o $(MYLIB) $(LIBS)

$(file)_wrapnew.o : $(file)_wrapnew.cxx
	$(GC) $(GCINCS) -c $(file)_wrapnew.cxx

$(file)_wrapnew.cxx : 
	./configure

clean_wrap :
	rm -f $(file)_wrap* $(file).h

clean :
	rm -f $(file)_wrap* $(file).h
	cd $(MY_OS)
	rm -f $(MYLIB)
