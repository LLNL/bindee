// Generated by bindee 2.0 from no_override.hh.
#ifndef BIND_NO_OVERRIDE_HH
#define BIND_NO_OVERRIDE_HH

#include <pybind11/pybind11.h>
namespace py = pybind11;

@INCLUDES@

// Records.

template <typename T, typename Target>
void bind_D(Target &target, const std::string &classPrefix);
template <typename T, typename Target>
void bind_E(Target &target, const std::string &classPrefix);

// Bind functions.

template <typename T, typename Target>
void bind_D(Target &target, const std::string &classPrefix) {
    using Class = D<T>;

    std::string className = classPrefix + "D";
    std::string docString = @DOC_STRING@;

    py::class_<Class> bindee(target, className.c_str(), docString.c_str());
    bindee.def("foo", (void (Class::*)()) &Class::foo);
}

template <typename T, typename Target>
void bind_E(Target &target, const std::string &classPrefix) {
    using Class = E<T>;

    std::string className = classPrefix + "E";
    std::string docString = @DOC_STRING@;

    py::class_<Class, D<T>> bindee(target, className.c_str(), docString.c_str());
}

#endif // BIND_NO_OVERRIDE_HH
