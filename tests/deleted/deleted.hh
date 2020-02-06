// -i deleted.in deleted.hh --
struct A {
    A() = default;
    A(const A&) = delete;
    A& operator=(const A&) = delete;
};
