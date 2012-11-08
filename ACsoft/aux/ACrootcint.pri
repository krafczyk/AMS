## ROOT's rootcint.pri file adapted by Henning to simplify and remove dependence on ROOTSYS
#-------------------------------------------------------------------------
#
# Usage:
#  Provide the list of the class header files followed by the appropriated LinkDef.f file
#    within your prpoject with CREATE_ROOT_DICT_FOR_CLASSES  QMAKE variable
# -----------------------------------------------
#
#   For example
#
#    . . . 
#    !exists ($$(ROOTSYS)/include/rootcint.pri){
#        message "The Rootcint.pri was not found"
#    }
#    exists ($$(ROOTSYS)/include/rootcint.pri){
#       include ($$(ROOTSYS)/include/rootcint.pri)
#       CREATE_ROOT_DICT_FOR_CLASSES  = ${HEADERS} MyParticle.h MyDetector.h MyEvent.h ShowerMain.h 
#       CREATE_ROOT_DICT_FOR_CLASSES *= ${HEADERS} RSLinkDef.h
#    }
#

ROOTINCDIR = $$system(root-config --incdir)
ROOTLIBS = $$system(root-config --libs)

exists ($$ROOTINCDIR){

  DEPENDPATH *= $$ROOTINCDIR
  unix {
     INCLUDEPATH *= $$ROOTINCDIR
  }
}

!isEmpty( CREATE_ROOT_DICT_FOR_CLASSES ) {
  ROOT_CINT_TARGET = $${TARGET}
  GENERATED_SOURCES *= $${OBJECTS_DIR}/$${ROOT_CINT_TARGET}Dict.cxx

# FIXME the following has a bug: (INCLUDEPATH) should be {INCLUDEPATH}, but we need '-I' in front of each directory,
# FIXME plus, some files like Event.h are nor parseable by CINT!

  rootcint.target  = $${OBJECTS_DIR}/$${ROOT_CINT_TARGET}Dict.cxx
  rootcint.commands += rootcint
  rootcint.commands += -f $$rootcint.target -c -p $$ROOTCINTINCLUDEPATH $$CREATE_ROOT_DICT_FOR_CLASSES
  rootcint.depends  = $$CREATE_ROOT_DICT_FOR_CLASSES
  
  QMAKE_EXTRA_TARGETS += rootcint 
  PRE_TARGETDEPS += $${OBJECTS_DIR}/$${ROOT_CINT_TARGET}Dict.cxx
  QMAKE_CLEAN += $${OBJECTS_DIR}/$${ROOT_CINT_TARGET}Dict.cxx $${OBJECTS_DIR}/$${ROOT_CINT_TARGET}Dict.h
}
