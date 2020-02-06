// -i dec_and_def.in dec_and_def.hh --

template <typename T>
struct Templated {
    typedef int INT;
    INT foo();
};

template <typename T>
inline typename Templated<T>::INT
Templated<T>::
foo()
{ return 1; }

struct Nontemplated {
    template <typename T, typename U>
    typename Templated<T>::INT foo(typename Templated<U>::INT);
};

