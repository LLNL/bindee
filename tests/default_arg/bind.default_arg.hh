// Generated by bindee 2.1.2 from default_arg.hh.
#ifndef BIND_DEFAULT_ARG_HH
#define BIND_DEFAULT_ARG_HH

#include <pybind11/pybind11.h>

@INCLUDES@

// Records.

template <typename Target>
void bind_A(Target &target);

// Bind functions.

template <typename Target>
void bind_A(Target &target) {
    namespace py = pybind11;
    using Class = A;

    std::string className = "A";
    std::string docString = @DOC_STRING@;

    py::class_<Class> bindee(target, className.c_str(), docString.c_str());
    bindee.def("foo", (void (Class::*)(int, std::string)) &Class::foo, py::arg("x") = 2, py::arg("z") = "hi");
    bindee.def_readwrite_static("y", &Class::y);
}

template <typename Target>
void bind_default_arg_globals(Target &target) {
    namespace py = pybind11;
    target.def("bar", (void (*)(int, float)) &bar, py::arg("y") = A::y, py::arg("f") = ARG);
}

#endif // BIND_DEFAULT_ARG_HH
