SHELL=/bin/sh
MY_OS := $(shell uname | tr '[A-Z]' '[a-z]')
file= AMS
GC=g++ -Wno-deprecated
PREF := $(shell python2.2 -c "import sys; print sys.prefix")

ifndef AMSWD
AMSWD := ..
endif

GCINCS=-I$(PREF)/include/python2.2 -I$(AMSWD)/include -I$(ROOTSYS)/include
LIBS=-L$(ROOTSYS)/lib -lCore -lCint -lHist -lTree -lMatrix -lMinuit \
-lGraf -lGraf3d -lGpad -lGX11 -lGX11TTF -lGui -lHistPainter -lPostscript \
-lRint -lHtml $(AMSWD)/bin/linux/icc/root_rs.o \
$(AMSWD)/bin/linux/icc/rootdict_s.o
MYLIB=$(MY_OS)/_$(file).so
MYPY=$(MY_OS)/$(file).py
MYPYC=$(MY_OS)/$(file).pyc
PYTHONLIB=$(PREF)/lib/python2.2/lib-dynload/_$(file).so
PYTHONPY=$(PREF)/lib/python2.2/$(file).py
PYTHONPYC=$(PREF)/lib/python2.2/$(file).pyc

all : $(MYPY) $(MYLIB)
	cd $(MY_OS)
	python2.2 -c "import py_compile; py_compile.compile('$(MYPY)')"
      
install : $(PYTHONLIB) $(PYTHONPY)
	cp -f $(MYPYC) $(PYTHONPYC)

$(PYTHONLIB) : $(MYLIB)
	cp -f $(MYLIB) $(PYTHONLIB)

$(PYTHONPY) : $(MYPY)
	cp -f $(MYPY) $(PYTHONPY)

$(MYPY) : 
	./configure
	mv -f $(file).py $(MYPY)

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
