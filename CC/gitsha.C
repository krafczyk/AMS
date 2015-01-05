#include "gitsha.h"

#define stringifyi(argument) #argument
#define stringify(argument) stringifyi(argument)

const char* GBatch::gGitSHA = stringify(GBATCHGITSHA);
