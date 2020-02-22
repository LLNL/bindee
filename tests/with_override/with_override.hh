// -override=1 with_override.hh -- 
#include <cstdio>

struct A {
    virtual void a() = 0;
    virtual void b() = 0;

    virtual void c() { printf("A::c()\n"); }
    virtual void d() { printf("A::d()\n"); }

    virtual void e() { printf("A::e()\n"); }

    void f() { printf("A::f()\n"); }

    virtual void g(int) { printf("A::g()\n"); }
    virtual void g(int) const { printf("A::g() const\n"); }
};

struct B : public A {
    virtual void a() { printf("B::a()\n"); }
    virtual void b() override { printf("B::b()\n"); }

    virtual void c() { printf("B::c()\n"); }
    virtual void d() override { printf("B::d()\n"); }

    void f() { printf("B::f()\n"); }

    void g(int) { printf("B::g()\n"); }
};

struct C : public B {
    void a() { printf("C::a()\n"); }
};

template <typename T>
struct D {
    virtual void foo() = 0;
};

template <typename T>
struct E : public D<T> {
    virtual void foo() override { printf("E::foo()\n"); }
};
