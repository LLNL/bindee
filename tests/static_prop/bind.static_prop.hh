// Generated by bindee 2.0 from static_prop.hh.
#ifndef BIND_STATIC_PROP_HH
#define BIND_STATIC_PROP_HH

#include <pybind11/pybind11.h>
namespace py = pybind11;

@INCLUDES@

// Records.

template <typename Target>
void bind_A(Target &target);
template <typename Target>
void bind_B(Target &target);
template <typename Target>
void bind_NoProp(Target &target);

// Bind functions.

template <typename Target>
void bind_A(Target &target) {
    using Class = A;

    std::string className = "A";
    std::string docString = @DOC_STRING@;

    py::class_<Class> bindee(target, className.c_str(), docString.c_str());
    bindee.def_property_readonly_static("readonly", [](py::object) { return Class::getReadonly(); });
    bindee.def_property_static("readwrite", [](py::object) { return Class::get_readwrite(); }, [](py::object, int i) { Class::set_readwrite(i); });
    bindee.def_property_static("writeonly", nullptr, [](py::object, int i) { Class::setWriteonly(i); });
}

template <typename Target>
void bind_B(Target &target) {
    using Class = NS::B;

    std::string className = "B";
    std::string docString = @DOC_STRING@;

    py::class_<Class> bindee(target, className.c_str(), docString.c_str());
    bindee.def_property_readonly_static("a", [](py::object) { return Class::getA(); }, @RETURN_VALUE_POLICY@);
    bindee.def_property_readonly_static("val", [](py::object) { return Class::getVal<@T0@>(); });
}

template <typename Target>
void bind_NoProp(Target &target) {
    using Class = NoProp;

    std::string className = "NoProp";
    std::string docString = @DOC_STRING@;

    py::class_<Class> bindee(target, className.c_str(), docString.c_str());
    bindee.def_static("getI", (int (*)()) &Class::getI);
    bindee.def("setI", (void (Class::*)(int)) &Class::setI, py::arg("x"));
}

#endif // BIND_STATIC_PROP_HH