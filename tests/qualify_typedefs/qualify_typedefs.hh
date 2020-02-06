// -i qualify_typedefs.in qualify_typedefs.hh --
namespace NS {
template <typename T>
struct A {
    typedef int INT;
    typedef T TYP;
};

struct B {
    typedef long INT;
    typedef float FLT;

    INT foo(A<int>::INT);
    INT foo(A<INT>::TYP);
};
}
