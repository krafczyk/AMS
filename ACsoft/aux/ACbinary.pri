TEMPLATE = app

DESTDIR = $$(ACROOTSOFTWARE)/bin

INCLUDEPATH += $$(ACROOTSOFTWARE) $$(ACROOTSOFTWARE)/acroot $$(ACROOTSOFTWARE)/analysis $$(ACROOTSOFTWARE)/cuts $$(ACROOTSOFTWARE)/io $$(ACROOTSOFTWARE)/detector $$(ACROOTSOFTWARE)/interface $$(ACROOTSOFTWARE)/producer $$(ACROOTSOFTWARE)/trdcalibration $$(ACROOTSOFTWARE)/utilities

DEPENDPATH += $$INCLUDEPATH

LIBS += -L$$(ACROOTSOFTWARE)/lib -lplatform

macx {
  CONFIG -= app_bundle
}

include($$(ACROOTSOFTWARE)/aux/AC.pri)
include($$(ACROOTSOFTWARE)/aux/ACrootcint.pri)
