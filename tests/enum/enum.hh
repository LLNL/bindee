// enum.hh --

enum A { AA };
enum class B { BA, BB };
enum class Forward;

struct _nontemplated {
    enum class Forward;

    enum C : unsigned int { _ntCA };
    enum class D : long { _ntDA, _ntDB };

    C bar(D);
};

template <typename T> 
struct templated {
    enum class Forward;

    enum C : T { tCA };
    enum class D : T { tDA, tDB };

    D bar(C c);
};

enum class Forward { FA };
enum class _nontemplated::Forward { NTFA };
template <typename T> enum class templated<T>::Forward { TFA };

struct x {
    enum lower { xa };
};

struct _ {
    enum class lower { _a };
};

class Private {
    enum a { pa };
    enum class b { pb };
};

_nontemplated::D foo(_nontemplated::C);

template <typename T>
typename templated<T>::C foo(typename templated<T>::D d);
