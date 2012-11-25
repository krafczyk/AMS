# $Id: tkdisplay.pro,v 1.10 2012/11/25 15:10:06 shaino Exp $
#
# Project file for TkDisplay on Qt4
# Run qmake first to generate Makefile
#
#                     10/06/2009 by SH
#
# For Mac OSX users:
#  You should set: DYLD_LIBRARY_PATH=$ROOTSYS/lib to avoid "bellenot" error
# 


CONFIG       += uitools warn_off

HEADERS       = inc/glutils.h    inc/glcamera.h   inc/gllight.h   \
                inc/glviewer.h   inc/glwidget.h   inc/glrender.h  \
                inc/gvconst.h    inc/gvgeom.h     inc/gvdraw.h    \
                inc/gltdisp.h    \
                inc/tewidget.h   inc/infotext.h   inc/subwindow.h \
                inc/swevent.h    inc/swtrack.h    inc/swmctrack.h \
                inc/swladder.h   \
                inc/trcls.h      inc/qwchist.h    inc/clwidget.h  \
                inc/wdcuts.h     inc/dlevsel.h    \
                inc/qnetio.h     inc/evthread.h   inc/tkdisplay.h

SOURCES       = src/glutils.cpp  src/glcamera.cpp src/gllight.cpp   \
                src/glviewer.cpp src/glwidget.cpp src/glrender.cpp  \
                src/gvconst.cpp  src/gvgeom.cpp   src/gvdraw.cpp    \
                src/gltdisp.cpp  \
                src/tewidget.cpp src/infotext.cpp src/subwindow.cpp \
                src/swevent.cpp  src/swtrack.cpp  src/swmctrack.cpp \
                src/swladder.cpp \
                src/trcls.cpp    src/qwchist.cpp  src/clwidget.cpp  \
                src/wdcuts.cpp   src/dlevsel.cpp  \
                src/qnetio.cpp   src/evthread.cpp src/tkdisplay.cpp \
                src/main.cpp


FORMS         = ui/tkdisplay.ui ui/clwidget.ui ui/dlevsel.ui
RESOURCES     = images/tkdisplay.qrc
ICON          = images/ams02.icns
QT           += opengl

INCLUDEPATH  += inc $(ROOTSYS)/include ../include /usr/X11/include
DEPENDPATH   += dep
DEFINES      += _PGTRACK_ _ROOTSHAREDLIBRARY_

macx {
LIBS         += ../lib/macosx64/libntuple_slc4_PG.a \
                -L$(ROOTSYS)/lib -lCore -lCint -lRIO -lTMVA \
                -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint \
                -lPostscript -lMatrix -lPhysics -lMathCore -lThread \
                -pthread -Wl -lm -ldl -lMinuit -lTreePlayer -lNetx -lXrdClient
}
else {
LIBS         += $(AMSWD)/lib/linux/libntuple_slc4_PG.a \
                -L$(ROOTSYS)/lib -lCore -lCint -lRIO -lNet \
                -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript \
                -lMatrix -lPhysics -lMathCore -lThread -pthread \
                -lNetx -lMinuit -lTreePlayer -lm -ldl -rdynamic
}

OBJECTS_DIR   = obj
RCC_DIR       = src
MOC_DIR       = moc
UI_DIR        = inc

target.path   = .
sources.path  = src inc .
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS $$ICON *.pro
INSTALLS     += target sources
