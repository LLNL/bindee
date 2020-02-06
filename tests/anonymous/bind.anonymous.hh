// Generated by bindee 2.0 from anonymous.hh.
#ifndef BIND_ANONYMOUS_HH
#define BIND_ANONYMOUS_HH

#include <pybind11/pybind11.h>
namespace py = pybind11;

@INCLUDES@

// Records.

template <typename Target>
void bind_Node(Target &target);
template <typename Target>
void bind_Record(Target &target);

// Bind functions.

template <typename Target>
void bind_Node(Target &target) {
    using Class = Node;

    std::string className = "Node";
    std::string docString = @DOC_STRING@;

    py::class_<Class> bindee(target, className.c_str(), docString.c_str());
    bindee.def("f", (void (Class::*)()) &Class::f);
    bindee.def_readwrite("next", &Class::next);
}

template <typename Target>
void bind_Record(Target &target) {
    using Class = Record;

    std::string className = "Record";
    std::string docString = @DOC_STRING@;

    py::class_<Class> bindee(target, className.c_str(), docString.c_str());
    bindee.def("f", (void (Class::*)(int, const double *)) &Class::f);
}

#endif // BIND_ANONYMOUS_HH
