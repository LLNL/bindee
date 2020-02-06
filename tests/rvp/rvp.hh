// -i rvp.in rvp.hh --
#include <cstdio>
struct Example {
    Example();
    int *foo();
    int &bar();
};
