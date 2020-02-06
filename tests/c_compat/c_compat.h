// -i c_compat.in c_compat.h --
extern "C" {
typedef struct Node {
    void f();
    Node *next;
} Node;

typedef struct {
    struct Named { float f; };
    struct { float f; };
    int anon;
} Anon;

double x = 1.0;
void f(const int *, double);
double* g();
}
