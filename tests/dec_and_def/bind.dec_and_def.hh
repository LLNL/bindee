// Generated by bindee 2.1.1 from dec_and_def.hh.
#ifndef BIND_DEC_AND_DEF_HH
#define BIND_DEC_AND_DEF_HH

#include <pybind11/pybind11.h>

@INCLUDES@

// Records.

template <typename Target>
void bind_Nontemplated(Target &target);
template <typename T, typename Target>
void bind_Templated(Target &target, const std::string &classPrefix);

// Bind functions.

template <typename Target>
void bind_Nontemplated(Target &target) {
    namespace py = pybind11;
    using Class = Nontemplated;

    std::string className = "Nontemplated";
    std::string docString = @DOC_STRING@;

    py::class_<Class> bindee(target, className.c_str(), docString.c_str());
    bindee.def("foo", (typename Templated<@T0@>::INT (Class::*)(typename Templated<@T1@>::INT)) &Class::foo<@T0@, @T1@>);
}

template <typename T, typename Target>
void bind_Templated(Target &target, const std::string &classPrefix) {
    namespace py = pybind11;
    using Class = Templated<T>;

    std::string className = classPrefix + "Templated";
    std::string docString = @DOC_STRING@;

    py::class_<Class> bindee(target, className.c_str(), docString.c_str());
    bindee.def("foo", (typename Class::INT (Class::*)()) &Class::foo);
}

#endif // BIND_DEC_AND_DEF_HH
