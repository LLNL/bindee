// -i inheritance.in inheritance.hh --
namespace NS {

class Base {
public:
    Base(int);
    void f(int, double);
private:
    void g();
};

class Derived : public Base {
};

}
