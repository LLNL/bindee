/*
 * Copyright (c) 2020, Lawrence Livermore National Security, LLC and other
 * bindee developers. See the top-level LICENSE file for details.
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef BINDEE_HH
#define BINDEE_HH
#include <string>
#include <vector>
#include <unordered_map>

#include "param.hh"

namespace Bindee {

struct Info;

extern const std::string BINDEE;
extern const std::string TARGET;

std::string usingClass(const std::string &type, const Info &info);

struct Bindee {
    Bindee(std::string qualName, 
           std::string qualCls);

    virtual ~Bindee() {}
    virtual std::string bind(const Info &info) const = 0;

    const std::string qualName;
    const std::string qualCls;
    const std::string name;
};

struct RecordBindee : public Bindee {
    RecordBindee(std::string qualName,
                 std::string qualCls,
                 std::vector<std::string> templateParams,
                 std::vector<std::string> qualBases,
                 std::vector<std::string>::size_type numBases);

    virtual std::string bind(const Info &info) const override;

    const std::vector<std::string> templateParams;
    const std::vector<std::string> qualBases;
    const std::vector<std::string>::size_type numBases;
};

struct FunctionBindee : public Bindee {
    FunctionBindee(std::string qualName,
                   std::string qualCls,
                   int numTemplateParams,
                   std::string ret,
                   std::vector<Param> params,
                   bool isConst,
                   bool isStatic);

    static const std::unordered_map<std::string, std::string> validOps;
    static const std::string OPERATOR;

    static void registerMethod(const FunctionBindee& bindee);
    static void resetRegistry();

    virtual std::string bind(const Info &info) const override;
    std::string cast(const Info &info, bool _const=false) const;
    std::string signature() const;

    bool isConstructor() const;
    bool isOperator() const;
    bool isGetter() const;
    bool isSetter() const;

    const int numTemplateParams;
    const std::string ret;
    const std::vector<Param> params;
    const bool isConst;
    const bool isStatic;

private:
    static std::unordered_map<std::string, int> _consts;
    std::string _bindConstructor(const Info &info) const;
};

struct VariableBindee : public Bindee {
    VariableBindee(std::string qualName, 
                   std::string qualCls,
                   bool isConst,
                   bool isStatic);
    virtual std::string bind(const Info &info) const override;
    std::string bindGlobal() const;

    const bool isConst;
    const bool isStatic;
};

struct EnumBindee : public Bindee {
    EnumBindee(std::string qualName,
               std::string qualCls,
               std::vector<std::string> templateParams,
               std::vector<std::string> values,
               bool isScoped);

    virtual std::string bind(const Info &info) const override;

    const std::vector<std::string> templateParams;
    const std::vector<std::string> values;
    const bool isScoped;

    void setIndentLength(int i) { _indentLength = i; }

private:
    int _indentLength;
};

} // namespace Bindee

#endif // BINDEE_HH
