// -i inherit_templated.in inherit_templated.hh -- 
namespace NS {
template <typename T>
struct A {
    T x;
};

struct C {
};

template <typename T>
struct B : public A<T>, public C {
    T y;
};
}
