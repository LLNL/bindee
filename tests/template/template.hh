// -i template.in template.hh --
template <typename T, typename U>
struct Example {
    Example() = default;
    Example(Example&&) = default;

    typedef int INT;

    template <typename V>
    void foo(typename U::INT, V, INT, int);
    
    template <typename W, typename X>
    void bar(X,X,W);

    T t;
};

template <typename Type>
const Type* foo(Type &t);
