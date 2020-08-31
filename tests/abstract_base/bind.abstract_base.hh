// Generated by bindee 2.1.2 from abstract_base.hh.
#ifndef BIND_ABSTRACT_BASE_HH
#define BIND_ABSTRACT_BASE_HH

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
    bindee.def("bar", (void (Class::*)(int) const) &Class::bar);
}

#endif // BIND_ABSTRACT_BASE_HH
