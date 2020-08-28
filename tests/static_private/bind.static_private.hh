// Generated by bindee 2.1.2 from static_private.hh.
#ifndef BIND_STATIC_PRIVATE_HH
#define BIND_STATIC_PRIVATE_HH

#include <pybind11/pybind11.h>

@INCLUDES@

// Records.

template <typename Target>
void bind_Example(Target &target);

// Bind functions.

template <typename Target>
void bind_Example(Target &target) {
    namespace py = pybind11;
    using Class = NS::Example;

    std::string className = "Example";
    std::string docString = @DOC_STRING@;

    py::class_<Class> bindee(target, className.c_str(), docString.c_str());
    bindee.def_static("type", (void (*)()) &Class::type);
    bindee.def_readwrite_static("a", &Class::a);
    bindee.def_readonly_static("ca", &Class::ca);
}

void bind_static_private_globals(py::module &bindee) {
    bindee.attr("b") = NS::b;
    bindee.attr("c") = NS::c;
}

#endif // BIND_STATIC_PRIVATE_HH
