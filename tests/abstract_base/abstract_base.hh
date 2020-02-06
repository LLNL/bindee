// -i abstract_base.in abstract_base.hh --
struct A {
    A();
    A(const A&);
    A& operator=(const A&);

    virtual void foo(A*) = 0;
    void bar(int) const;
};
