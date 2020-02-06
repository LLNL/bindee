/*
 * Copyright (c) 2020, Lawrence Livermore National Security, LLC and other
 * bindee developers. See the top-level LICENSE file for details.
 *
 * SPDX-License-Identifier: MIT
 */
#include <sstream>
#include <cctype>
#include <utility>

#include "bindee.hh"
#include "options.hh"
#include "info.hh"

namespace Bindee {
    
const std::string BINDEE = "bindee";
const std::string TARGET = "target";

std::unordered_map<std::string, int> FunctionBindee::_consts{};
const std::unordered_map<std::string, std::string> FunctionBindee::validOps{
    { "+", "__add__" },
    { "-", "__sub__" },
    { "*", "__mul__" },
    { "/", "__div__" },
    { "+=", "__iadd__" },
    { "-=", "__isub__" },
    { "*=", "__imul__" },
    { "/=", "__idiv__" },
    { "<", "__lt__" },
    { ">", "__gt__" },
    { "<=", "__le__" },
    { ">=", "__ge__" },
    { "==", "__eq__" },
    { "!=", "__ne__" },

    // unary keys
    { "pos", "__pos__" },
    { "neg", "__neg__" }
};
const std::string FunctionBindee::OPERATOR = "operator";

std::string
usingClass(const std::string &type, const Info &info)
{
    if (info.className.empty()) { return type; }

    std::stringstream ss;
    std::string::size_type start{0};
    std::string::size_type pos{0};

    while ((pos = type.find(info.className, start)) != std::string::npos) {
        ss << type.substr(start, pos-start);
        start = pos + info.className.size();

        if (pos > 0) {
            auto c = type.at(pos-1);
            if (std::isalnum(c) || c == '_') {
                ss << type.substr(pos, info.className.size());
                continue;
            }
        }

        if (start < type.size()) {
            auto c = type.at(start);
            if (std::isalnum(c) || c == '_') {
                ss << type.substr(pos, info.className.size());
                continue;
            }

            if (info.isClassTemplate && c == ':') {
                ss << "typename ";
            }
        }

        ss << "Class";

        if (start < type.size() && info.isClassTemplate) {
            auto c = type.at(start);
            if (c == '<') {
                int count{0};
                while (true) {
                    auto o = type.find('<', start+1);
                    auto c = type.find('>', start+1);
                    if (o < c) {
                        count++;
                    } else if (count-- == 0) {
                        start = c+1;
                        break;
                    }
                }
            }
        }
    }

    if (start < type.size()) {
       ss << type.substr(start);
    }
    return ss.str();
}

Bindee::
Bindee(std::string qname,
       std::string cls)
    : qualName(std::move(qname)),
      qualCls(std::move(cls)),
      name(qualName.substr(qualName.rfind(":")+1))
{
}

RecordBindee::
RecordBindee(std::string name,
             std::string cls,
             std::vector<std::string> tp,
             std::vector<std::string> qb,
             std::vector<std::string>::size_type nb)
    : Bindee(std::move(name), std::move(cls)),
      templateParams(std::move(tp)),
      qualBases(std::move(qb)),
      numBases(std::move(nb))
{
}

FunctionBindee::
FunctionBindee(std::string name,
               std::string cls,
               int ntp,
               std::string r,
               std::vector<Param> p,
               bool c,
               bool s)
    : Bindee(std::move(name), std::move(cls)),
      numTemplateParams(std::move(ntp)),
      ret(std::move(r)),
      params(std::move(p)),
      isConst(std::move(c)),
      isStatic(std::move(s))
{
}

VariableBindee::
VariableBindee(std::string name, 
               std::string cls,
               bool c,
               bool s)
    : Bindee(std::move(name), std::move(cls)),
      isConst(std::move(c)),
      isStatic(std::move(s))
{
} 

EnumBindee::
EnumBindee(std::string name, 
           std::string cls,
           std::vector<std::string> tp,
           std::vector<std::string> vals,
           bool s)
    : Bindee(std::move(name), std::move(cls)),
      templateParams(std::move(tp)),
      values(std::move(vals)),
      isScoped(std::move(s))
{
} 

std::string
RecordBindee::
bind(const Info &info) const
{
    std::stringstream ss;
    ss << "py::class_<" << usingClass(qualCls, info);

    if (qualBases.size() != numBases) {
        for (decltype(qualBases.size()) i=0; i<numBases; i++) {
            ss << ", @BASE_" << std::to_string(i) << "@";
        }
    } else {
        for (const auto &base : qualBases) {
            ss << ", " << base;
        }
    }
    ss << ">";

    if (!opts.useChain) {
        ss << " " << BINDEE;
    }
    ss << "(" << TARGET << ", className.c_str(), docString.c_str())";

    if (!opts.useChain) {
        ss << ";";
    }

    return ss.str();
}

std::string
FunctionBindee::
bind(const Info &info) const
{
    if (isConstructor()) {
        return _bindConstructor(info);
    } 

    std::string def = "def";
    if (isStatic) {
        def += "_static";
    }

    std::stringstream ss;
    ss << "." << def << "(\"";

    if (isOperator()) {
        std::string op = name.substr(OPERATOR.size());
        bool isUnary = (qualCls.empty() && params.size() == 1) 
                   || (!qualCls.empty() && params.size() == 0);
        if (isUnary) {
            if (op == "+") {
                op = "pos";
            } else if (op == "-") {
                op = "neg";
            }
        }
        ss << validOps.at(op);
    } else {
        ss << name;
    }
    ss << "\", ";

    // Cast.
    if (_consts[signature()] == 2) {
        // const and non-const
        ss << cast(info, true);
        _consts.at(signature())++;
    } else if (_consts[signature()] == 3) {
        // const and non-const, but already bound
        return "";
    } else {
        ss << cast(info);
    }

    // Function address.
    ss << "&";
    if (qualCls.empty()) {
        ss << usingClass(qualName, info);
    } else {
        ss << usingClass(qualCls, info) << "::" << name;
    }

    if (numTemplateParams > 0) {
        ss << "<@T0@";
        for (int i=1; i<numTemplateParams; i++) {
            ss << ", @T" << std::to_string(i) << "@";
        }
        ss << ">";
    }

    // Return value policy or operator.
    if (isOperator()) {
        ss << ", py::is_operator()";
    } else {
        if (ret.back() == '*' || ret.back() == '&') {
            ss << ", @RETURN_VALUE_POLICY@";
        }
    }

    // Kwargs.
    if (!isOperator()) {
        for (const auto &param : params) {
            if (param.name.empty()) { continue; }

            ss << ", py::arg(\"" << param.name << "\")";
            if (!param.defaultArg.empty()) {
                ss << " = " << param.defaultArg;
            }
        }
    }

    ss << ")";
    return ss.str();
}

std::string
FunctionBindee::
_bindConstructor(const Info &info) const
{
    std::stringstream ss;
    ss << ".def(py::init<";
    for (const auto &param : params) {
        ss << usingClass(param.type, info);
        if (&param.type != &params.back().type) {
            ss << ", ";
        }
    }
    ss << ">())";
    return ss.str();
}

void 
FunctionBindee::
registerMethod(const FunctionBindee& bindee)
{
    _consts[bindee.signature()]++;
}

void 
FunctionBindee::
resetRegistry()
{
    _consts.clear();
}

std::string
FunctionBindee::
cast(const Info &info, bool _const) const
{
    std::stringstream ss;

    // Return type.
    ss << "(" << usingClass(ret, info) << " ";

    // Explicit cast.
    if (qualCls.empty() || isStatic) {
       ss << "(*)";
    } else {
       ss << "(" << usingClass(qualCls, info) << "::*)";
    }

    // Arguments.
    ss << "(";
    for (const auto &param : params) {
        ss << usingClass(param.type, info);
        if (&param.type != &params.back().type) {
            ss << ", ";
        }
    }
    ss << ")";

    // Const-ness.
    if (_const) {
        ss << " @CONST?@";
    } else if (isConst) {
        ss << " const";
    }
    ss << ") ";

    return ss.str();
} // FunctionBindee::cast

std::string 
FunctionBindee::
signature() const
{
    std::stringstream ss;
    ss << ret << qualName << numTemplateParams;
    for (const auto &param : params) {
        ss << param.type;
    }
    return ss.str();
}

bool
FunctionBindee::
isConstructor() const
{
    return name == qualCls.substr(qualCls.rfind(":")+1);
}

bool
FunctionBindee::
isOperator() const
{
    return name.compare(0, OPERATOR.size(), OPERATOR) == 0;
}

namespace {
    bool checkPropertyName(const std::string &name, const std::string &prop) {
        bool properPrefix = name.compare(0, prop.size(), prop) == 0;
        bool properCase = true;
        if (properPrefix && (name.size() > prop.size())) {
            properCase = std::isupper(name.at(prop.size())) 
                         || (name.at(prop.size()) == '_');
        }
        return properPrefix && properCase;
    }
}

bool
FunctionBindee::
isGetter() const
{
    std::string get = "get";
    if (name.size() < get.size() || params.size() != 0) {
        return false;
    }
    return checkPropertyName(name, get);
}

bool
FunctionBindee::
isSetter() const
{
    std::string set = "set";
    if (name.size() < set.size() || params.size() != 1) {
        return false;
    }
    return checkPropertyName(name, set);
}

std::string
VariableBindee::
bind(const Info &info) const
{
    std::string def = "def";
    if (isConst) {
        def += "_readonly";
    } else {
        def += "_readwrite";
    }

    if (isStatic) {
        def += "_static";
    }

    std::stringstream ss;
    ss << "." << def << "("
       << "\"" << name << "\", "
       << "&";
    if (qualCls.empty()) {
        ss << qualName;
    } else {
        ss << usingClass(qualCls, info) << "::" << name;
    }
    ss << ")";

    return ss.str();
}

std::string
VariableBindee::
bindGlobal() const
{
    std::string attr = "attr";
    std::stringstream ss;
    ss << "." << attr << "("
       << "\"" << name << "\") = "
       << qualName;
    return ss.str();
}

std::string
EnumBindee::
bind(const Info &info) const
{
    std::stringstream ss;
    std::string indents(_indentLength, ' ');

    ss << indents << "py::enum_<Enum>";
    if (!opts.useChain) { ss << " " << BINDEE; }
    ss << "(" << TARGET << ", enumName.c_str(), docString.c_str())";
    if (!opts.useChain) { ss << ";"; }
    ss << "\n";

    for (const auto &value : values) {
        ss << indents;
        if (!opts.useChain) { ss << BINDEE; }
        ss << ".value(\"" << value << "\", "
           << "Enum::" << value << ")";
        if (!opts.useChain) { ss << ";"; }
        ss << "\n";
    }

    if (!isScoped) {
        ss << indents;
        if (!opts.useChain) { ss << BINDEE; }
        ss << ".export_values()";
        if (!opts.useChain) { ss << ";"; }
        ss << "\n";
    }

    return ss.str();
}

} // namespace Bindee
