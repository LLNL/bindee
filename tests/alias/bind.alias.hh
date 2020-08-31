// Generated by bindee 2.1.2 from alias.hh.
#ifndef BIND_ALIAS_HH
#define BIND_ALIAS_HH

#include <pybind11/pybind11.h>

@INCLUDES@

// Records.

template <typename T, typename U, typename Target>
void bind_A(Target &target, const std::string &classPrefix);
template <typename T, typename U, typename Target>
void bind_B(Target &target, const std::string &classPrefix);

// Bind functions.

template <typename T, typename U, typename Target>
void bind_A(Target &target, const std::string &classPrefix) {
    namespace py = pybind11;
    using Class = A<T, U>;

    std::string className = classPrefix + "A";
    std::string docString = @DOC_STRING@;

    py::class_<Class> bindee(target, className.c_str(), docString.c_str());
}

template <typename T, typename U, typename Target>
void bind_B(Target &target, const std::string &classPrefix) {
    namespace py = pybind11;
    using Class = B<T, U>;

    std::string className = classPrefix + "B";
    std::string docString = @DOC_STRING@;

    py::class_<Class> bindee(target, className.c_str(), docString.c_str());
    bindee.def("foo", (A<typename Class::X, typename A<T, U>::Y> (Class::*)()) &Class::foo);
}

#endif // BIND_ALIAS_HH
