// catalog of databases
// June,    1997. ak. First try with Objectivity  
//
// last edit June 16, 1997, ak.
//

#include <typedefs.h>
enum { dbany = 0, dbtag = 1, dbraw = 2, dbsim = 3, dbrec = 4, dbsetup = 5, 
       maxTagDBSize = 1000, maxRawDBSize = 1000, maxMCDBSize = 1000,
       maxRecDBSize = 1000, maxSetupDBSize = 1000}; // db size in MBytes

static char anydb_name[4]    = {"Any"};
static char tagdb_name[9]    = {"EventTag"};
static char rawdb_name[10]   = {"RawEvents"};
static char simdb_name[9]    = {"MCEvents"};
static char recdb_name[11]   = {"RecoEvents"};
static char setupdb_name[6]  = {"Setup"};

declare(ooVArray, ooRef(ooDBObj) )

class dbcatalog : public ooObj {

 private:

  ooVArray(ooRef(ooDBObj))       dbCat;

 public:

// Constructor
  dbcatalog();
  dbcatalog(const ooHandle(ooDBObj) &dbH);
 ~dbcatalog() {};

// Set/Get Methods
   ooStatus  add(const ooHandle(ooDBObj) &dbH) {dbCat.extend(dbH);
                                                return oocSuccess;}
   ooStatus  extend();
   ooStatus  getDB(integer n, ooHandle(ooDBObj) &dbH);
   ooStatus  getLastDB(ooHandle(ooDBObj) &dbH);
   //integer   size() {return dbCat[size()-1];}
   integer   size() {return dbCat.size();}
};

class AMSdbs : public ooObj {

private:
  


 ooRef(dbcatalog)         tag;
 ooRef(dbcatalog)         mc;
 ooRef(dbcatalog)         raw;
 ooRef(dbcatalog)         reco;
 ooRef(dbcatalog)         setup;
 ooRef(dbcatalog)         hk;
 ooRef(dbcatalog)         any;

public:

  static char   *pathNameTab[];
  static int    filesPerDir;


 AMSdbs();
 ooHandle(ooDBObj) currentDB(integer whichdb); 
 long              dbsize(integer whichdb);
 ooStatus          extend(integer whichdb);
 ooHandle(ooDBObj) getDB(integer whichdb, integer n); 
 ooStatus          newDB(const int whichdb, const ooHandle(ooDBObj) &dbH);
 integer           size(integer whichdb);
 
};
