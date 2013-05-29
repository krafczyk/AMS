# -- V.Z. 31/10/2005 -------------

TEST     = ${AMSana}
BIN      = ${AMSana}
OBJ      = ${AMSana}
SRC      = ${AMSana}
INC      = ${AMSana} 

###includes

INCLPATH =          -I$(INC)    \
		    -I$(AMSWD)/include \
                    -I$(AC)/acroot \
                    -I$(AC)/interface \
                    -I$(ROOTSYS)/include \
                    -I$(ROOTSYS)/include/TMVA \
                    -I$(TEST) 


###libs

LIBS = 	        ${AMSWD}/lib/linuxx8664icc/ntuple_slc4_PG.so \
                    -L/afs/cern.ch/ams/local2/opt/intel/Compiler/11.1/073/lib/intel64/  \
		`root-config --libs` `root-config --glibs`    -lASImage -lRIO -lRFIO -lNet -lNetx -lMinuit -lTMVA -lMLP -lXMLIO -lTreePlayer \
		 -lm -lblas -llapack -lgfortran


###methodes
CXXFLAGS =      -fPIC  -c -pipe -O2  -lgfortran  -D_PGTRACK_ -D__ROOTSHAREDLIBRARY__  -DAMS_ACQT_INTERFACE
CC =            g++
LFLAGS  =       -O4
AR      =       ar
AFLAGS  =       -rv
LD  =           g++   
LDFLAGS =        -shared  -O  



###definitions

TARGET  =    mainmvatest  mainintermva 
### implicit rules

all: $(TARGET) 
.PHONY:  clean 
clean : 
		-rm  $(TEST)*.o  



$(TEST)%.o : $(TEST)%.cpp 
		$(CC) $(CXXFLAGS) $< -o $@   $(INCLPATH)


$(TEST)%.o : $(TEST)%.C
		$(CC) $(CXXFLAGS) $< -o $@   $(INCLPATH)




mainmvatest :  $(TEST)mainmvatest.o $(TEST)MVAtest.o		
	$(LD) -O  -o $(BIN)$@  $^  $(LIBS) 


mainintermva :  $(TEST)mainintermva.o $(TEST)InterMVA.o		
	$(LD) -O  -o $(BIN)$@  $^  $(LIBS) 


$(TEST)MVAtest.o :    $(TEST)MVAtest.C 
$(TEST)mainmvatest.o:   $(TEST)mainmvatest.cpp 
$(TEST)mainmvatest :    $(TEST)mainmvatest.o 

$(TEST)InterMVA.o :      $(TEST)InterMVA.C 
$(TEST)mainintermva.o:   $(TEST)mainintermva.cpp 
$(TEST)mainintermva :    $(TEST)mainintermva.o 