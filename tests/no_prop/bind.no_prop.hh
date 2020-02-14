// Generated by bindee 2.1 from no_prop.hh.
#ifndef BIND_NO_PROP_HH
#define BIND_NO_PROP_HH

#include <pybind11/pybind11.h>
namespace py = pybind11;

@INCLUDES@

// Records.

template <typename Target>
void bind_Example(Target &target);

// Bind functions.

template <typename Target>
void bind_Example(Target &target) {
    using Class = Example;

    std::string className = "Example";
    std::string docString = @DOC_STRING@;

    py::class_<Class> bindee(target, className.c_str(), docString.c_str());
    bindee.def("getY", (int (Class::*)()) &Class::getY);
    bindee.def("getX", (int (Class::*)()) &Class::getX);
    bindee.def("setX", (void (Class::*)(int)) &Class::setX);
    bindee.def("setZ", (void (Class::*)(int)) &Class::setZ);
}

#endif // BIND_NO_PROP_HH
