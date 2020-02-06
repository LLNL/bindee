// -i prop_overload.in prop_overload.hh --
#include <cstdio>
#include <typeinfo>

class A {
public:
    A() : i{4}, d{4.0} {}

    int getInt() { return i; }
    void setInt(int i) { this->i = i; }

    double& getDouble() { return d; }
    void setDouble(double d) { this->d = d; }

    template <typename T>
    T getT() { printf("get: %s\n", typeid(T).name()); return T(); }
    template <typename T>
    void setT(T) { printf("set: %s\n", typeid(T).name()); }

    int getT() { return 1; }
    void setT(int) { }

private:
    int i;
    double d;
};
