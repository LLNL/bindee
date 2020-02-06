// -i override_and_overload.in override_and_overload.hh --
struct A {
    virtual void foo();
};

struct B : public A {
    void foo() override;
    void foo(int);
};
