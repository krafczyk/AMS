#  $Id: Makefile,v 1.12 2012/08/17 13:56:04 mkrafczy Exp $
#
# Makefile to build libamsut.a 
#
# This Makefile is confirmed to work on 
#  SLC 4.5 with root-5.10.00
#  Mac OS 10.5.2 (Darwin 9.2.1) with root-5.19.02
#

#-------------------------------------
# Settings
#-------------------------------------

### Architecture ###
ARCH   := $(shell $(ROOTSYS)/bin/root-config --arch)

GCC_VER_MAJOR=$(shell gcc --version | grep gcc | cut -b11 )
GCC_VER_MIDDLE=$(shell gcc --version | grep gcc |cut -b13 )
GCC_VER_MINOR=$(shell gcc --version | grep gcc |cut -b15 )

ifeq ($(GCC_VER_MAJOR),4)
VERGCC:=1

ifeq ($(GCC_VER_MIDDLE),4)
VERGCCH      := 1
endif
endif

ifndef AMSWD
AMSWD := ..
endif


LIB= $(AMSWD)/lib/$(ARCH)/


ifeq "$(ARCH)" "linux"
DEFINES=
ifdef VERGCC
FC       = gfortran -m32
else
### Compilers ###
FC       = g77 -m32
endif
endif
ifeq "$(ARCH)" "linuxicc"
DEFINES=
### Compilers ###
  FC       = ifort  -m32
  CXX      = icc -m32 
  LD       = icc   -m32
endif
ifeq "$(ARCH)" "linuxx8664gcc"
DEFINES=
### Compilers ###
  FC       = gfortran
  CXX      = g++ 
  LD       = g++
endif
ifeq "$(ARCH)" "linuxx8664icc"
DEFINES=
### Compilers ###
  FC       = ifort
  CXX      = icpc 
  LD       = icpc
endif
ifeq ($(ARCH), Darwin)
DEFINES  = -D__DARWIN__
FC=gfortran -m64
CXX      = g++  
LD       = g++ 



endif

  
SRC= .
OBJ= ./tmp/$(ARCH)/

### Documentation ###
ifeq ($(DOXYGEN),)
DOXYGEN  = doxygen
endif

### ROOT settings ###
CINT     = $(ROOTSYS)/bin/rootcint
ROOTCFG  = $(ROOTSYS)/bin/root-config
ROOTFLGS = $(shell $(ROOTCFG) --cflags)
ROOTLIBS = $(shell $(ROOTCFG) --libs)

ifndef CERNDIR
CERNDIR=/cern/pro/
endif
CXXFLAGS = -g -O3   -fPIC -march=i686  -I. $(INC) $(DEFINES) $(ROOTFLGS)
FFLAGS= -I.  -I$(CERNDIR)/include 

### LD flags ###
ifeq ($(ARCH), Darwin)
SOFLAGS  = -dynamiclib -undefined dynamic_lookup
else
SOFLAGS  = -shared
endif
######### FILES #######################


SRCS = call_rqmd.F rqmd.F call_fluka.F 
OBJS = $(patsubst %.F, $(OBJ)%.o, $(SRCS))


##########  RULES #################

$(OBJ)%.o : %.F
	@echo Compiling  $< ...
	@if ! [ -d $(OBJ) ] ; then mkdir -p $(OBJ); fi
	$(FC) $(FFLAGS) -c $< -o $@  



#########   TARGETS #######################
all: lib 

test: 
	echo $(SRCS) $(OBJS)



lib: $(LIB)/libamsut.a 


$(LIB)/libamsut.a : $(OBJS)
	@echo Bulding $@
	@if ! [ -d $(LIB) ] ; then mkdir -p $(LIB); fi
	@$(AR) r $@ $^ 



.PHONY: clean 

### Cleanup ###
clean:
	@echo Cleaning up ...
	@rm -fvr $(LIB)/libamsut.a 
	@rm -fvr $(OBJ) 
