// -i const.in const.hh --

struct A {
    void foo();
    void foo() const;
    template <typename T> void foo();
    template <typename T> void foo() const;

    void bar() const;
    template <typename T> void bar();
};
