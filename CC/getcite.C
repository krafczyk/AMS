#include <iostream>
#include <stdlib.h>

using namespace std;

const char* GetCite (unsigned int run);

int main(int argc, char * argv[]) {
    if (argc <= 1) {
        cerr << "Please specify the run number." << endl;
        exit(1);
    }
    unsigned int jid = atoi(argv[1]);
    cout << GetCite(jid) << endl;
}

const char* GetCite (unsigned int run) {
    static const char *const sitename[] = { "", 
    "CERN", 
    "CERN-TEST", 
    "BOLO",
    "ETHZ",
    "MILANO-CNAF",
    "KNU",
    "CIEMAT",
    "LPTA",
    "EKP",
    "LIP",
    "MIT",
    "PERUGIA",
    "SIENA",
    "TAIWAN",
    "LYON",
    "UMD",
    "YALE",
    "ITEP",
    "SEU",
    "IAC",
    "LPSC",
    "MSU",
    "NLAA",
    "IHEP",
    "IHEP",
    "INFN-PISA",
    "AACHEN",
    "RWTH_APC",
    "RWTH_JARA",
    "Unassigned",
    "Unassigned"};
    const int max_cid = 26;

    unsigned long runmin, runmax;
    int cid = 0;
    for (cid = 1; cid < 32; cid++) {
        runmin = ((cid-1) << max_cid) + 1;
        runmax = cid << max_cid;
        if (run >= runmin && run < runmax)
            return sitename[cid];
    }
    return sitename[cid];
}
