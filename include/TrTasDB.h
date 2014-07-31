// $Id$

#ifndef __TrTasDB__
#define __TrTasDB__

//////////////////////////////////////////////////////////////////////////
///
///\file  TrTasDB.h
///\brief Header file of TrTasDB class
///
///\class TrTasDB
///\brief The tracker parameters data base
///\ingroup tkdbc
///
///\date  2009/12/10 SH  First version
///\date  2009/12/17 SH  First Gbatch version
///\date  2014/07/29 SH  Update for ISS data
///$Date$
///
///$Revision$
///
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TString.h"
#include "TrTasPar.h"

#include <map>
#include <ctime>

class TrTasDB : public TObject {

////////////////// New version for ISS //////////////////
protected:
  enum { Ne = 108 };
  static TString _sttl[Ne];
  static int     _beam[Ne];
  static int     _tkid[Ne];
  static int     _tfit[Ne];
  static double  _mean[Ne];
  static double  _ycoo[Ne];
  static int     _nfil;

public:
  static int    Load(const char *fname);
  static int    Find(const char *title, int ibeam);
  static bool Filled(void)  { return (_nfil == Ne) ? 1 : 0; }
  static int    TkID(int i) { return (0 <= i && i < Ne) ? _tkid[i] : 0; }
  static int    Tfit(int i) { return (0 <= i && i < Ne) ? _tfit[i] : 0; }
  static double Mean(int i) { return (0 <= i && i < Ne) ? _mean[i] : 0; }
  static double Ycoo(int i) { return (0 <= i && i < Ne) ? _ycoo[i] : 0; }

////////////////// New version for ISS //////////////////

protected:
  /// Map of TrTasPar with a key of _Index(ival, lddr)
  std::map<int, TrTasPar*> _parmap;

  /// TrTasPar iterator
  typedef std::map<int, TrTasPar*>::const_iterator _parIT;

  /// Generate index of ival and lddr
  static int _Index(int ival, int lddr) { return ival*0x10000+lddr; }

public:
  TrTasDB() { Head = this; }
 ~TrTasDB() {}

  /// Self pointer
  static TrTasDB *Head;

  /// Load TrTasDB from a root file
  static TrTasDB *Load_old(const char *fname);

  /// Find TrTasPar with ival and lddr
  TrTasPar *FindPar(int ival, int lddr);

  /// Convert dec to hex (used for datacards)
  static int Dec2Hex(int lddr) {
    return ((lddr/1000)%10)*0x1000+((lddr/ 100)%10)*0x0100+
	   ((lddr/  10)%10)*0x0010+((lddr/   1)%10)*0x0001;
  }

  /// Initialize database from text files under directory, dname
  int  Init(const char *dname = "dat");

  /// Output contents into text files under directory, dname
  void Output(const char *dname = "./");

  /// Print information
  /*
   * option= "rpk": print signal ranges and saturaton flags
   *         "tkp": print fitting parameters
   *      default : print TkID list and laser positions
   */
  void Print (Option_t *option = "") const;


  enum { NPMAX = 8*3 };
  static float* linear;

  /// Returns linear buffer size
  static int GetLinearSize() {
    return (NPMAX*TrTasPar::GetLinearSize())*sizeof(float);
  }

  /// Setup the linear space for the DB
  static void CreateLinear() {
    linear = new float[GetLinearSize()/4];
  }

  /// Save contents to TDV DB
  static int Save2DB(time_t statime = 1212990000,
		     time_t endtime = 1261000000);

  /// Interface to linear storage
  void DB2Lin();

  /// Interface to linear storage
  void Lin2DB();

  ClassDef(TrTasDB, 1);
};

void SLin2TasDB();

#endif
