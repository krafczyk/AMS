!exists($(ACROOTSOFTWARE)) {
  error("Please set the ACROOTSOFTWARE environment variable!")
}

include($$(ACROOTSOFTWARE)/aux/version.pri)

# Load Mac global settings
macx {
  QMAKE_CFLAGS += -mmacosx-version-min=10.6
  QMAKE_CFLAGS_X86_64 -= -mmacosx-version-min=10.5
  QMAKE_LFLAGS += -mmacosx-version-min=10.6
  QMAKE_CFLAGS_X86_64 -= -mmacosx-version-min=10.5
  QMAKE_CXXFLAGS += -mmacosx-version-min=10.6
  QMAKE_CXXFLAGS_X86_64 -= -mmacosx-version-min=10.5
}

# Enable compiler warnings
ACPRODUCTIONTAG = $$(ACPRODUCTIONTAG)

# Eventually load ICC compiler specifics
linux-icc | linux-icc-64 {
  include($$(ACROOTSOFTWARE)/aux/ACicc-compiler.pri)
}

# AMS Software
linux-icc | linux-icc-64 {
  exists ($(AMSWD)/lib/linuxx8664icc/ntuple_slc4_PG.so) {
    AMSNTUPLE = $(AMSWD)/lib/linuxx8664icc/ntuple_slc4_PG.so
  }
}
linux-g++ | linux-g++-64 {
  exists ($(AMSWD)/lib/linuxx8664gcc/ntuple_slc4_PG.so) {
    AMSNTUPLE = $(AMSWD)/lib/linuxx8664gcc/ntuple_slc4_PG.so
  }
}
macx {
  exists ($(AMSWD)/lib/macosx64/ntuple_slc4_PG.so) {
    AMSNTUPLE = $(AMSWD)/lib/macosx64/ntuple_slc4_PG.so
  }
}

CONFIG += warn_on
QMAKE_CXXFLAGS += -Werror

# Disable debug builds by default, override by passing "CONFIG += DEBUG_BUILD" to qmake.
!DEBUG_BUILD {
  CONFIG += release
  CONFIG -= debug
} else {
  CONFIG -= release
  CONFIG += debug
}

# Seperate source & build dirs
OBJECTS_DIR = ./obj
MOC_DIR     = ./obj
UI_DIR      = ./obj
RCC_DIR     = ./obj

# Helper test function which allows to add directories without using the subdirs template
defineTest(addSubdirs) {
  for(subdirs, 1) {
    entries = $$files($$subdirs)
    for(entry, entries) {
      name = $$replace(entry, [/\\\\], _)
      SUBDIRS += $$name
      eval ($${name}.subdir = $$entry)
      for(dep, 2):eval ($${name}.depends += $$replace(dep, [/\\\\], _))
      export ($${name}.subdir)
      export ($${name}.depends)
    }
  }
  export (SUBDIRS)
}
