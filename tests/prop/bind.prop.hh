// Generated by bindee 2.0 from prop.hh.
#ifndef BIND_PROP_HH
#define BIND_PROP_HH

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
    bindee.def_property(@PROP_NAME@, (int (Class::*)()) &Class::get_, (void (Class::*)(int)) &Class::set_);
    bindee.def_property(@PROP_NAME@, (int (Class::*)()) &Class::get, (void (Class::*)(int)) &Class::set);
    bindee.def_property("read_write", (int (Class::*)()) &Class::get_read_write, (void (Class::*)(int)) &Class::set_read_write);
    bindee.def_property_readonly("read_only", (int (Class::*)()) &Class::get_read_only);
    bindee.def_property_readonly("readOnly", (int (Class::*)()) &Class::getReadOnly);
    bindee.def_property("readWrite", (int (Class::*)()) &Class::getReadWrite, (void (Class::*)(int)) &Class::setReadWrite);
    bindee.def_property("write_only", nullptr, (void (Class::*)(int)) &Class::set_write_only);
    bindee.def_property("writeOnly", nullptr, (void (Class::*)(int)) &Class::setWriteOnly);
}

#endif // BIND_PROP_HH
