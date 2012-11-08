# When we try to build against AMS software using gcc we'll encounter tons of warnings that we have to supress.
*-g++* {
  # We have to put this here instead of QMAKE_CXXFLAGS, to work-around a qmake spec problem on Mac and older linux versions of Qt, that we need to support (Qt 4.6).
  QMAKE_CXXFLAGS_THREAD += -Wno-extra -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-reorder -Wno-write-strings -Wno-sign-compare -Wno-unknown-pragmas
  CXXVERSION = $$system(g++ -dumpversion)
  CXXMAJOR = $$system("echo $$CXXVERSION | sed 's/^\\([0-9]\\)\\.\\([0-9]\\).*/\\1/g'")
  CXXMINOR = $$system("echo $$CXXVERSION | sed 's/^\\([0-9]\\)\\.\\([0-9]\\).*/\\2/g'")
  isEqual(CXXMAJOR, 4) {
    greaterThan(CXXMINOR, 6) {
      QMAKE_CXXFLAGS_THREAD += -Wno-delete-non-virtual-dtor
    }
  }
}
