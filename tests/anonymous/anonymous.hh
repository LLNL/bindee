// -i anonymous.in anonymous.hh --
struct Record {
    struct {};
    struct {} field;
    void f(int,const double*);
};

typedef struct Node {                                                           
    void f();                                                                   
    Node *next;                                                                 
} Node;

struct {
    struct {
        struct Named {
            void f();
            int i;
        };
    } s;
} anon;

struct {} var;
