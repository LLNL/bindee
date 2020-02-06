// -i static_private.in static_private.hh -- 
namespace NS {
class Example {
public:
    /* Static. */
    static void type() {
        int type_a;
        static int type_b;
    }
    static int a;
    static const int ca;

private:
    /* Static. */
    static void bad_type();
    static int bad_a;
    static const int bad_ca;

};

int b = 3;
const int c = 4;

}
