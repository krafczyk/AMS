SHELL=/bin/sh
OS := $(shell uname)
MY_OS := $(shell uname | tr '[A-Z]' '[a-z]')
file= AMS
GC=g++
PREF := $(shell python2.2 -c "import sys; print sys.prefix")

AMSWD := ..
AMSOBJDIR := $(AMSWD)/bin/$(MY_OS)

ifdef  AMSICC
ifeq  "$(OS)"  "Linux"
      AMSOBJDIR := $(AMSWD)/bin/$(MY_OS)/icc
endif
endif

GCINCS=-I$(PREF)/include/python2.2 -I$(AMSWD)/include -I$(ROOTSYS)/include
LIBS= $(AMSOBJDIR)/root_rs.o $(AMSOBJDIR)/rootdict_s.o \
-L$(ROOTSYS)/lib -lCore -lCint -lHist -lTree -lMatrix -lMinuit \
-lGraf -lGraf3d -lGpad -lGX11 -lGX11TTF -lGui -lHistPainter -lPostscript \
-lRint -lHtml
MYLIB=$(MY_OS)/_$(file).so
MYPY=$(MY_OS)/$(file).py
MYPYC=$(MY_OS)/$(file).pyc
PYTHONLIB=$(PREF)/lib/python2.2/lib-dynload/_$(file).so
PYTHONPY=$(PREF)/lib/python2.2/$(file).py
PYTHONPYC=$(PREF)/lib/python2.2/$(file).pyc

all : $(MYPY) $(MYLIB)
      
install : $(PYTHONLIB) $(PYTHONPY)
	cp -f $(MYPYC) $(PYTHONPYC)

$(PYTHONLIB) : $(MYLIB)
	cp -f $(MYLIB) $(PYTHONLIB)

$(PYTHONPY) : $(MYPY)
	cp -f $(MYPY) $(PYTHONPY)

$(MYPY) : 
	./configure
	mv -f $(file).py $(MYPY)
	cd $(MY_OS)
	python2.2 -c "import py_compile; py_compile.compile('$(MYPY)')"

$(MYLIB) : $(file)_wrapnew.o
	$(GC) -shared $(file)_wrapnew.o -o $(MYLIB) $(LIBS)

$(file)_wrapnew.o : $(file)_wrapnew.cxx
	$(GC) $(GCINCS) -c $(file)_wrapnew.cxx
      
$(file)_wrapnew.cxx :
	./configure
	mv -f $(file).py $(MYPY)

clean_wrap :
	rm -f $(file)_wrap* $(file).h

clean :
	rm -f $(file)_wrap* $(file).h
	cd $(MY_OS)
	rm -f $(MYLIB) $(MYPY)*
