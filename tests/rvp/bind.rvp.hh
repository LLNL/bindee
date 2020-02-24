// Generated by bindee 2.1.1 from rvp.hh.
#ifndef BIND_RVP_HH
#define BIND_RVP_HH

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
    bindee.def(py::init<>());
    bindee.def("foo", (int * (Class::*)()) &Class::foo, @RETURN_VALUE_POLICY@);
    bindee.def("bar", (int & (Class::*)()) &Class::bar, @RETURN_VALUE_POLICY@);
}

#endif // BIND_RVP_HH
