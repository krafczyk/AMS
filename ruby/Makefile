SHELL=/bin/sh
OS := $(shell uname)
MY_OS := $(shell uname | tr '[A-Z]' '[a-z]')
file= AMS
GC=g++
ifeq  "$(OS)"  "Linux" 
      RUBY := $(shell which ruby 2> /dev/null)
endif
ifndef RUBY
	RUBY=/afs/cern.ch/user/a/alcaraz/public/$(OS)/bin/ruby
endif

LIB_DIR := $(shell $(RUBY) -r rbconfig -e 'puts Config::CONFIG["sitedir"]')
INC_DIR := $(shell $(RUBY) -r rbconfig -e 'puts Config::CONFIG["rubylibdir"] + "/" + Config::CONFIG["arch"]')

AMSWD := ..
AMSOBJDIR := $(AMSWD)/bin/$(MY_OS)

ifdef  AMSICC
ifeq  "$(OS)"  "Linux"
      AMSOBJDIR := $(AMSWD)/bin/$(MY_OS)/icc
endif
endif

GCINCS=-I$(INC_DIR) -I$(AMSWD)/include -I$(ROOTSYS)/include
LIBS= $(AMSOBJDIR)/root_rs.o $(AMSOBJDIR)/rootdict_s.o \
-L$(ROOTSYS)/lib -lCore -lCint -lHist -lTree -lMatrix -lMinuit \
-lGraf -lGraf3d -lGpad -lGX11 -lGX11TTF -lGui -lHistPainter -lPostscript \
-lRint -lHtml
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
