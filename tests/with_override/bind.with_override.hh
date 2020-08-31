// Generated by bindee 2.1.2 from with_override.hh.
#ifndef BIND_WITH_OVERRIDE_HH
#define BIND_WITH_OVERRIDE_HH

#include <pybind11/pybind11.h>

@INCLUDES@

// Records.

template <typename Target>
void bind_A(Target &target);
template <typename Target>
void bind_B(Target &target);
template <typename Target>
void bind_C(Target &target);
template <typename T, typename Target>
void bind_D(Target &target, const std::string &classPrefix);
template <typename T, typename Target>
void bind_E(Target &target, const std::string &classPrefix);

// Bind functions.

template <typename Target>
void bind_A(Target &target) {
    namespace py = pybind11;
    using Class = A;

    std::string className = "A";
    std::string docString = @DOC_STRING@;

    py::class_<Class> bindee(target, className.c_str(), docString.c_str());
    bindee.def("a", (void (Class::*)()) &Class::a);
    bindee.def("b", (void (Class::*)()) &Class::b);
    bindee.def("c", (void (Class::*)()) &Class::c);
    bindee.def("d", (void (Class::*)()) &Class::d);
    bindee.def("e", (void (Class::*)()) &Class::e);
    bindee.def("f", (void (Class::*)()) &Class::f);
    bindee.def("g", (void (Class::*)(int) @CONST?@) &Class::g);
}

template <typename Target>
void bind_B(Target &target) {
    namespace py = pybind11;
    using Class = B;

    std::string className = "B";
    std::string docString = @DOC_STRING@;

    py::class_<Class, A> bindee(target, className.c_str(), docString.c_str());
    bindee.def("a", (void (Class::*)()) &Class::a);
    bindee.def("b", (void (Class::*)()) &Class::b);
    bindee.def("c", (void (Class::*)()) &Class::c);
    bindee.def("d", (void (Class::*)()) &Class::d);
    bindee.def("f", (void (Class::*)()) &Class::f);
    bindee.def("g", (void (Class::*)(int)) &Class::g);
}

template <typename Target>
void bind_C(Target &target) {
    namespace py = pybind11;
    using Class = C;

    std::string className = "C";
    std::string docString = @DOC_STRING@;

    py::class_<Class, B> bindee(target, className.c_str(), docString.c_str());
    bindee.def("a", (void (Class::*)()) &Class::a);
}

template <typename T, typename Target>
void bind_D(Target &target, const std::string &classPrefix) {
    namespace py = pybind11;
    using Class = D<T>;

    std::string className = classPrefix + "D";
    std::string docString = @DOC_STRING@;

    py::class_<Class> bindee(target, className.c_str(), docString.c_str());
    bindee.def("foo", (void (Class::*)()) &Class::foo);
}

template <typename T, typename Target>
void bind_E(Target &target, const std::string &classPrefix) {
    namespace py = pybind11;
    using Class = E<T>;

    std::string className = classPrefix + "E";
    std::string docString = @DOC_STRING@;

    py::class_<Class, D<T>> bindee(target, className.c_str(), docString.c_str());
    bindee.def("foo", (void (Class::*)()) &Class::foo);
}

#endif // BIND_WITH_OVERRIDE_HH
