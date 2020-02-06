// Generated by bindee 2.0 from using_class.hh.
#ifndef BIND_USING_CLASS_HH
#define BIND_USING_CLASS_HH

#include <pybind11/pybind11.h>
namespace py = pybind11;

@INCLUDES@

// Records.

template <typename Target>
void bind_LongName(Target &target);
template <typename Target>
void bind_Name(Target &target);

// Bind functions.

template <typename Target>
void bind_LongName(Target &target) {
    using Class = LongName;

    std::string className = "LongName";
    std::string docString = @DOC_STRING@;

    py::class_<Class> bindee(target, className.c_str(), docString.c_str());
}

template <typename Target>
void bind_Name(Target &target) {
    using Class = Name;

    std::string className = "Name";
    std::string docString = @DOC_STRING@;

    py::class_<Class> bindee(target, className.c_str(), docString.c_str());
    bindee.def("foo", (LongName & (Class::*)(const LongName *)) &Class::foo, @RETURN_VALUE_POLICY@);
    bindee.def("funcName", (const Class (Class::*)(Class &)) &Class::funcName);
    bindee.def("NameName", (Class * (Class::*)(Class)) &Class::NameName, @RETURN_VALUE_POLICY@);
}

#endif // BIND_USING_CLASS_HH
