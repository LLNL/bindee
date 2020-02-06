// -i qual_params.in qual_params.hh --

namespace NS {

template <typename T>
struct A{
    void foo();
};

}

namespace NS {

template <typename T>
struct B {
    typedef A<T> A1;
    using A2 = A<T>;
    A1& foo(A<T>, A<T>*&, const A1&, A2);
};

}
