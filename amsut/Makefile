#  $Id: Makefile,v 1.1 2008/08/28 13:43:20 pzuccon Exp $
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
ARCH     = $(shell uname)

### Compilers ###
FC       = g77
CXX      = g++
LD       = g++


ifeq "$(ARCH)" "Linux"
LIB= ../lib/linux/
else
LIB= ../lib/$(ARCH)/
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

### C++ compiler flags ###
ifeq ($(ARCH), Darwin)
DEFINES  = -D__DARWIN__
FC=gfortran
else
DEFINES  =
endif
CXXFLAGS = -g -O3   -fPIC -march=i686  -I. $(INC) $(DEFINES) $(ROOTFLGS)
FFLAGS= -I. -I/cern/pro/include

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
	@$(FC) $(FFLAGS) -c $< -o $@  



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
