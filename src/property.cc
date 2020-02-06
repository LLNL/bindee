/*
 * Copyright (c) 2020, Lawrence Livermore National Security, LLC and other
 * bindee developers. See the top-level LICENSE file for details.
 *
 * SPDX-License-Identifier: MIT
 */
#include <sstream>

#include "property.hh"
#include "bindee.hh"

namespace Bindee {
    
std::string
Property::
bind(const Info &info) const
{
    static constexpr auto GENERIC_NAME = "x";

    std::string ret;
    std::stringstream ss;

    const bool readonly = getter && !setter;
    const bool readwrite = getter && setter;
    const bool writeonly = !getter && setter;

    // passed isValidIfStatic at this point
    bool isStatic = false;
    if (getter) { isStatic = getter->isStatic; }
    if (setter) { isStatic = setter->isStatic; }

    auto bind = [&info, isStatic](const FunctionBindee *p) {
        std::stringstream ss;

        if (isStatic) {
            ss << "[](py::object";
            if (p->isSetter()) {
                const auto &param = p->params.at(0);
                ss << ", " << param.type << " "
                   << ((param.name.empty()) ? GENERIC_NAME : param.name);
            }
            ss << ") { ";
            if (p->isGetter()) {
                ss << "return ";
            }
        } else {
            ss << p->cast(info)
               << "&";
        }

        // Qualified name.
        ss << usingClass(p->qualCls, info) << "::" << p->name;

        if (p->numTemplateParams > 0) {
            ss << "<@T0@";
            for (int i=1; i<p->numTemplateParams; i++) {
                ss << ", @T" << std::to_string(i) << "@";
            }
            ss << ">";
        }

        if (isStatic) {
            ss << "(";
            if (p->isSetter()) {
                const auto &param = p->params.at(0);
                ss << ((param.name.empty()) ? GENERIC_NAME : param.name);
            }
            ss << "); }";
        }

        return ss.str();
    };

    // camel vs snake case
    std::string propName = propertyName();
    if (fill && propName.at(0) != '@') {
        std::stringstream ss;
        ss << "@";
        for (const auto &c : propName) {
            ss << std::string(1, std::toupper(c));
        }
        ss << "_EXISTS@";
        propName = ss.str();
    }
    if (propName.at(0) != '@') {
        propName = "\"" + propName + "\"";
    }

    ss << ".def_property";
    if (readonly) {
        ss << "_readonly";
    }
    if (isStatic) {
        ss << "_static";
    }
    ss << "(" << propName << ", ";

    if (readwrite) {
        ss << bind(getter) << ", "
           << bind(setter);
        if (getter->ret.back() == '*' || getter->ret.back() == '&') {
            ss << ", @RETURN_VALUE_POLICY@";
        }
        ss << ")";
    } else if (writeonly) {
        ss << "nullptr, "
           << bind(setter)
           << ")";
    } else if (readonly) {
        ss << bind(getter);
        if (getter->ret.back() == '*' || getter->ret.back() == '&') {
            ss << ", @RETURN_VALUE_POLICY@";
        }
        ss << ")";
    }
    ret = ss.str();

    return ret;
}

std::string 
Property::
propertyName() const
{
    const FunctionBindee *p = getter ? getter : setter;
    const std::string &name = p->name;

    std::string ret = "@PROP_NAME@";

    std::string::size_type start{3};
    if (name.size() > start) {
        if (name.at(start) == '_') {
            if (name.size() > ++start) {
                ret = std::string(1, std::tolower(name.at(start))) 
                    + name.substr(start+1);
            }
        } else {
            ret = std::string(1, std::tolower(name.at(start))) 
                + name.substr(start+1);
        }
    }

    return ret;
}

bool
Property::
isValidIfStatic() const
{
    if (getter && setter) {
        // neither or both are static
        return !(getter->isStatic ^ setter->isStatic);
    } 

    // single function is always valid
    return true;
}

} // namespace Bindee
