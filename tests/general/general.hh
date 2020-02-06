// -i general.in general.hh --
namespace NS {
class Example {
public:
    Example(int x) : readwrite(x+1), readonly(x+2), _x(x), _y(true) {}
    Example(const Example &);
    Example(Example &&);

    /* Methods. */
    void print() const;

    const int getX() const;
    void setX(int x);

    bool getY() const;

    Example& foo();

    /* Static. */
    static void type();
    static int a;
    static const int ca;

    bool operator==(const Example &rhs) const;
    Example& operator=(const Example &rhs) const;

    Example operator+(const Example& rhs) const;
    Example& operator+=(const Example& rhs);

    /* Member variables. */
    int readwrite;
    int readonly;
private:
    void _foo() {}

    int _x;
    const bool _y;
};

int Example::a = 5;
const int Example::ca = 6;

void Example::print() const { }

void f(Example* e) {}
}
