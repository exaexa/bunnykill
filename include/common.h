
#ifndef _BK_COMMON_H
#define _BK_COMMON_H

#include <vector>
#define storage std::vector
#include <string>
using std::string;

#define FRAND (rand()/(float)RAND_MAX)
#define DFRAND (1-2*(rand()/(float)RAND_MAX))

string itos (int i);

#endif

