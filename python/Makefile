SHELL=/bin/sh
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
PYTHONLIB=$(PREF)/lib/python2.2/lib-dynload/_$(file).so
PYTHONPY=$(PREF)/lib/python2.2/$(file).py
PYTHONPYC=$(PREF)/lib/python2.2/$(file).pyc

all : $(file).py _$(file).so
	python2.2 -c "import py_compile; py_compile.compile('$(file).py')"
      
install : $(PYTHONLIB) $(PYTHONPY)
	cp -f $(file).pyc $(PYTHONPYC)

$(PYTHONLIB) : _$(file).so
	cp -f _$(file).so $(PYTHONLIB)

$(PYTHONPY) : $(file).py
	cp -f $(file).py $(PYTHONPY)

$(file).py : 
	./configure

_$(file).so : $(file)_wrapnew.o
	$(GC) -shared $(file)_wrapnew.o -o _$(file).so $(LIBS)

$(file)_wrapnew.o : $(file)_wrapnew.cxx
	$(GC) $(GCINCS) -c $(file)_wrapnew.cxx
      
$(file)_wrapnew.cxx :
	./configure

clean :
	rm -f $(file)_wrap* _$(file).so $(file).py* $(file).h
