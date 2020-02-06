// -i default_arg.in default_arg.hh --
#include <string>
struct A {
    void foo(int x = 2, std::string z = "hi");
    static int y;
};

#define ARG 42

void bar(int y = A::y, float f = ARG);
