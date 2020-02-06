// -i specialization.in specialization.hh --
template <typename T, typename U>
struct Example {
    Example() = default;
    Example(Example&&) = default;

    typedef int INT;

    template <typename V>
    void foo(typename U::INT, V, INT, int);
    
    template <typename W, typename X>
    void bar(W,X,X);

    T t;
};

template <typename T>
struct Example<T,int> {
    Example() = default;
    Example(Example&&) = default;

    typedef int INT;

    template <typename V>
    void foo(int, V, INT, int);
    
    template <typename W, typename X>
    void bar(W,X,X);

    T baz();

    T t;
};
