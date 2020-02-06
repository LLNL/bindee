// immutable.hh --
#include <string>

void a(int*);
void b(float*);
void c(double*);
void d(long&);
void e(bool&);
void f(std::string*);
void g(std::string&);

void aa(const int*);
void bb(const float*);
void cc(const double*);
void dd(const long&);
void ee(const bool&);
void ff(const std::string*);
void gg(const std::string&);

void aaa(int);
void bbb(float);
void ccc(double);
void ddd(long);
void eee(bool);
void fff(std::string);
