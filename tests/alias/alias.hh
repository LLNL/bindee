// alias.hh --

template <typename T, typename U>
struct A {
    typedef T X;
    typedef U Y;
};

template <typename T, typename U>
struct B {
    typedef typename A<T,U>::X X;
    typedef typename A<T,U>::Y Y;

    A<X,typename A<T,U>::Y> foo();
};
