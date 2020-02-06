// Generated by bindee 2.0 from prop_overload.hh.
#ifndef BIND_PROP_OVERLOAD_HH
#define BIND_PROP_OVERLOAD_HH

#include <pybind11/pybind11.h>
namespace py = pybind11;

@INCLUDES@

// Records.

template <typename Target>
void bind_A(Target &target);

// Bind functions.

template <typename Target>
void bind_A(Target &target) {
    using Class = A;

    std::string className = "A";
    std::string docString = @DOC_STRING@;

    py::class_<Class> bindee(target, className.c_str(), docString.c_str());
    bindee.def(py::init<>());
    bindee.def_property("double", (double & (Class::*)()) &Class::getDouble, (void (Class::*)(double)) &Class::setDouble, @RETURN_VALUE_POLICY@);
    bindee.def_property("int", (int (Class::*)()) &Class::getInt, (void (Class::*)(int)) &Class::setInt);
    bindee.def("getT", (int (Class::*)()) &Class::getT);
    bindee.def("getT", (@T0@ (Class::*)()) &Class::getT<@T0@>);
    bindee.def("setT", (void (Class::*)(int)) &Class::setT);
    bindee.def("setT", (void (Class::*)(@T0@)) &Class::setT<@T0@>);
}

#endif // BIND_PROP_OVERLOAD_HH
