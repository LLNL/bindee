/*
 * Copyright (c) 2020, Lawrence Livermore National Security, LLC and other
 * bindee developers. See the top-level LICENSE file for details.
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef CALLBACKS_HH
#define CALLBACKS_HH
#include <sstream>
#include <unordered_map>

#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

namespace Bindee {

class Database;

class BindeeCallback : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
    BindeeCallback(Database &db, const clang::PrintingPolicy pp, const clang::LangOptions lo);
    virtual void run(const clang::ast_matchers::MatchFinder::MatchResult &result) = 0;

    std::string methodSignature(const clang::CXXMethodDecl *method);
protected:
    Database &_db;
    const clang::PrintingPolicy _pp;
    const clang::LangOptions _lo;
};


struct RecordCallback : public BindeeCallback {
    using BindeeCallback::BindeeCallback;
    virtual void run(const clang::ast_matchers::MatchFinder::MatchResult &result);
};

class FunctionCallback : public BindeeCallback {
public:
    using BindeeCallback::BindeeCallback;
    virtual void run(const clang::ast_matchers::MatchFinder::MatchResult &result);

    using QualAliasMap = std::unordered_map<std::string, std::string>;
    static std::unordered_map<std::string, QualAliasMap> qualAliasMaps;
};

struct VariableCallback : public BindeeCallback {
    using BindeeCallback::BindeeCallback;
    virtual void run(const clang::ast_matchers::MatchFinder::MatchResult &result);
};

struct EnumCallback : public BindeeCallback {
    using BindeeCallback::BindeeCallback;
    virtual void run(const clang::ast_matchers::MatchFinder::MatchResult &result);
};

} // namespace Bindee

#endif // CALLBACKS_HH
