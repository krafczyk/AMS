/**
 * \file debugging.hh
 *
 * Useful macros for debugging and warning output. These macros behave
 * exactly like cout or cerr, except append useful text about where
 * the warning or debug message occurred.
 *
 * Use DEBUG_OUT to display a debugging message whenever DEBUG is
 * defines as a number greater than 0.
 *
 * Use DEBUG_OUT_L(level) to write debugging messages that should only
 * print out if DEBUG is set to  "level" or above
 *
 * use WARN_OUT to print red warning messages (regardless of the value
 * of DEBUG)
 *
 * example:
 *
 * \code
 *
 * #define DEBUG 2
 * #include "utilities/debugging.hh"
 *
 * void myfunc() {
 *   WARN_OUT << "This is a warning" << endl;
 *   DEBUG_OUT << "This only prints when DEBUG is greater than 0" << endl;
 *   DEBUG_OUT_L(1) << "Equivalent to DEBUG_OUT" << endl;
 *   DEBUG_OUT_L(2) << "Prints out, since DEBUG==2" << endl;
 *   DEBUG_OUT_L(3) << "doesn't print, since DEBUG==2" << endl;
 * }
 * \endcode
 *
 * <h1> WARNING </h1> 
 *
 * There is a possibilty of introducing buggy behavior into your code
 * if you are not careful with these macros!  Since DEBUG_OUT
 * ... expands to code like: \code if (DEBUG) cout ... \endcode you
 * will have a major problem if you include it after a bare "if"
 * statement that also uses an "else". Do not do the following:
 *
 * \code
 *    if (condition) 
 *       DEBUG_OUT << "Stuff" << endl;
 *    else
 *       doOtherStuff();
 * \endcode 
 *
 * since the macro expands this to:
 *
 * \code
 *    if (condition) 
 *       if (DEBUG) cout << "Stuff" << endl;
 *       else
 *       doOtherStuff(); // never happens if DEBUG is true!
 * \endcode 
 * 
 * which means the else goes with the if (DEBUG) and not the one you
 * intended! There's no easy way to fix this unfortunately.  Just be
 * careful and use {} in your if statements to avoid it.
 */

#ifndef DEBUGGING_HH
#define DEBUGGING_HH

#include <iostream>
#include <stdlib.h>

#ifndef DEBUG
#define DEBUG 0
#endif
// enables locale settings
static inline void enableLocale() {
  #ifndef Q_WS_MAC
    std::cout.imbue(std::locale(""));
  #else
    // if we're on a Mac, at least enable locale support for (s)printf
    setlocale(LC_ALL, "");
  #endif
}


// Returns true if the ACSOFT_BATCH_MODE environment variable is present and not set to zero.
static inline bool isBatchMode() {

  static const char* value = getenv("ACSOFT_BATCH_MODE");
  if(!value) return false;

  return std::string(value) != std::string("0");
}

// Returns true if the ACSOFT_COLOUR_OUTPUT environment variable is present and not set to zero.
static inline bool isColourOutputMode() {

  static const char* value = getenv("ACSOFT_COLOUR_OUTPUT");
  if(!value) return false;

  return std::string(value) != std::string("0");
}

static inline const char* debugWarningColor() { return isColourOutputMode() ? "\033[31;01m" : ""; }
static inline const char* debugInfoColor()    { return isColourOutputMode() ? "\033[32;01m" : ""; }
static inline const char* debugResetColor()   { return isColourOutputMode() ? "\033[0m" : ""; }

#define DEBUG_OUT if (DEBUG) std::cout<<"DEBUG> "<<__FILE__<<":"<<__LINE__ \
                      <<": " <<__FUNCTION__<< ": " 


#define DEBUG_OUT_L(level) if (DEBUG>=level) std::cout<<"DEBUG> "\
                      <<__FILE__<<":"<<__LINE__ \
                      <<": " <<__FUNCTION__<< ": " 


#define WARN_OUT std::cout << debugWarningColor() << "WARNING> " << debugResetColor() \
                      <<__FILE__<<":"<<__LINE__ \
                      <<": " <<__FUNCTION__<< ": " 


#ifndef INFO_OUT_TAG
#define INFO_OUT_TAG "* "
#endif

#define INFO_OUT std::cout << debugInfoColor() << INFO_OUT_TAG << debugResetColor()

#endif
