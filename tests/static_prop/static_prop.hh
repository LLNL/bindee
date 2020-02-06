// static_prop.hh --

class A {
public:
    static int get_readwrite() { return readwrite; }
    static void set_readwrite(int i) { readwrite = i; }

    static int getReadonly() { return readonly; }
    static void setWriteonly(int i) { writeonly = i; }

private:
    static int readwrite;
    static int readonly;
    static int writeonly;
};

int A::readwrite = 1;
int A::readonly  = 2;
int A::writeonly = 3;

namespace NS {
class B {
public:
    template <typename T>
    static T getVal();

    static A& getA() { return a; }

private:
    static int i;
    static double d;
    static A a;
};

template <>
int B::getVal() { return i; }

template <>
double B::getVal() { return d; }

int B::i = 4;
double B::d = 5.0;
A B::a = A();

} // namespace NS

class NoProp {
public:
    static int getI() { return i; }
    void setI(int x) { i = x; }
private:
    static int i;
};

int NoProp::i = 6;
