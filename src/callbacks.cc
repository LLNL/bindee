/*
 * Copyright (c) 2020, Lawrence Livermore National Security, LLC and other
 * bindee developers. See the top-level LICENSE file for details.
 *
 * SPDX-License-Identifier: MIT
 */
#include <unordered_set>
#include <iostream>
#include <sstream>

#include "callbacks.hh"
#include "database.hh"
#include "bindee.hh"
#include "options.hh"

#include "clang/Lex/Lexer.h"

namespace Bindee {
    
BindeeCallback::
BindeeCallback(Database &db, const clang::PrintingPolicy pp, const clang::LangOptions lo) 
    : clang::ast_matchers::MatchFinder::MatchCallback(), 
      _db(db), 
      _pp(pp),
      _lo(lo)
{
}

std::unordered_map<std::string, FunctionCallback::QualAliasMap> FunctionCallback::qualAliasMaps = 
    std::unordered_map<std::string, FunctionCallback::QualAliasMap>();

// qualify helpers
namespace {
bool
qualifyAlias(std::string &typeName, const FunctionCallback::QualAliasMap *map)
{
    if (!map) { return false; }

    bool ret = false;
    for (const auto &kv : *map) {
        const auto &alias = kv.first;
        std::stringstream ss;
        std::string::size_type start{0};
        std::string::size_type pos{0};

        while ((pos = typeName.find(alias, start)) != std::string::npos) {
            ss << typeName.substr(start, pos-start);
            start = pos + alias.size();

            if (pos > 0) {
                auto c = typeName.at(pos-1);
                if (std::isalnum(c) || c == '_' || c == ':') {
                    ss << typeName.substr(pos, alias.size());
                    continue;
                }
            }

            if (start < typeName.size()) {
                auto c = typeName.at(start);
                if (std::isalnum(c) || c == '_') {
                    ss << typeName.substr(pos, alias.size());
                    continue;
                }
            } else {
                continue;
            }

            ss << kv.second;
            ret = true;
        }

        ss << typeName.substr(start);
        typeName = ss.str();
    }
    return false;
} // qualifyAlias

bool 
qualifyTemplate(std::string &typeName, clang::QualType qualType)
{
    bool ret = false;

    const auto *p = qualType->getAs<clang::PointerType>();
    const auto *r = qualType->getAs<clang::ReferenceType>();

    // Strip pointers and references.
    while (p || r) {
        qualType = p ? p->getPointeeType() : r->getPointeeType();
        p = qualType->getAs<clang::PointerType>();
        r = qualType->getAs<clang::ReferenceType>();
    }

    // Find class template.
    const auto *inst = qualType->getAs<clang::TemplateSpecializationType>();
    if (inst) {
        const auto *templateDecl = inst->getTemplateName().getAsTemplateDecl();
        if (templateDecl) {
            const std::string instName = templateDecl->getNameAsString();
            const std::string qualInstName = templateDecl->getQualifiedNameAsString();

            std::stringstream ss;
            std::string::size_type start{0};
            std::string::size_type pos{0};

            while ((pos = typeName.find(instName, start)) != std::string::npos) {
                ss << typeName.substr(start, pos-start);
                start = pos + instName.size();

                if (pos > 0) {
                    auto c = typeName.at(pos-1);
                    if (std::isalnum(c) || c == '_' || c == ':') {
                        ss << typeName.substr(pos, instName.size());
                        continue;
                    }
                }

                if (start < typeName.size()) {
                    auto c = typeName.at(start);
                    if (c != '<') {
                        ss << typeName.substr(pos, instName.size());
                        continue;
                    }
                } else {
                    continue;
                }

                ss << qualInstName;
                ret = true;
            }

            ss << typeName.substr(start);
            typeName = ss.str();
        }
    }

    return ret;
} // qualifyTemplate

} // anonymous namespace

std::string 
BindeeCallback::
methodSignature(const clang::CXXMethodDecl *method)
{
    std::string ret;
    std::stringstream ss;

    // return type
    const auto retType = method->getReturnType();
    std::string retName = retType.getAsString(_pp);
    qualifyTemplate(retName, retType);
    ss << retName;

    // name
    ss << method->getNameAsString();

    // params
    std::vector<std::string> params;
    for (const auto *param : method->parameters()) {
        auto qualType = param->getType();
        std::string typeName = qualType.getAsString(_pp);
        qualifyTemplate(typeName, qualType);
        ss << typeName;
    }

    // const
    ss << method->isConst();

    ret = ss.str();
    return ret;
} //signature

void 
RecordCallback::
run(const clang::ast_matchers::MatchFinder::MatchResult &result) 
{
    const clang::CXXRecordDecl *record = result.Nodes.getNodeAs<clang::CXXRecordDecl>("record");
    if (!record) { return; }
    if (record->getNameAsString().empty()) { return; } 

    std::string qualName = record->getQualifiedNameAsString();
    std::string qualCls = qualName;
    std::vector<std::string> templateParams;
    std::vector<std::string> qualBases;
    int numBases = record->bases().end() - record->bases().begin();

    for (const auto &base : record->bases()) {
        const auto qualType = base.getType();
        std::string typeName = qualType.getAsString(_pp);
        if (const auto *baseRecord = qualType->getAsCXXRecordDecl()) {
            qualBases.push_back(baseRecord->getQualifiedNameAsString());
        } else {
            if (qualifyTemplate(typeName, qualType)) {
                qualBases.push_back(typeName);
            }
        }
    }

    // Skip anonymous ancestor.
    const clang::RecordDecl *recordDecl = record;
    std::string prev = qualName;
    while ((recordDecl = recordDecl->getOuterLexicalRecordContext())) {
        if (recordDecl->getNameAsString().empty()) { return; }
        if (recordDecl->getQualifiedNameAsString() == prev) { break; }
        else { prev = recordDecl->getQualifiedNameAsString(); }
    }

    // Get number of template parameters.
    const auto *classTemplateDecl = record->getDescribedClassTemplate();
    if (classTemplateDecl) {
        for (const auto *param : *(classTemplateDecl->getTemplateParameters())) {
            templateParams.push_back(param->getNameAsString());
        }
    }

    RecordBindee bindee(qualName, qualCls, templateParams, qualBases, numBases);
    _db.add(bindee);
}

void 
FunctionCallback::
run(const clang::ast_matchers::MatchFinder::MatchResult &result) 
{
    const clang::FunctionDecl *function = result.Nodes.getNodeAs<clang::FunctionDecl>("function");
    if (!function) { return; }
    if (function->isInvalidDecl()
            || function->isImplicit()
            || function->isDeleted()) 
    { return; }

    static const std::string CLASS_ = "class ";
    static const std::string STRUCT_ = "struct ";
    static const std::string ENUM_ = "enum ";

    std::string qualName = function->getQualifiedNameAsString();
    const auto retType = function->getReturnType();
    std::string ret = retType.getAsString(_pp);
    int numTemplateParams = 0;
    std::vector<Param> params;
    std::string qualCls;
    bool isConst = false;
    bool isStatic = false;

    const auto &OPERATOR = FunctionBindee::OPERATOR;
    const auto &validOps = FunctionBindee::validOps;
    std::string name(function->getNameAsString());

    bool isOperator = false;
    if (name.compare(0, OPERATOR.size(), OPERATOR) == 0) {
        const auto c{name.at(OPERATOR.size())};
        if (!(std::isalnum(c) || c == '_')) {
            isOperator = true;
        }
    }

    std::unordered_map<std::string, std::string> *qualAliasMap = nullptr;
    const auto *method = dynamic_cast<const clang::CXXMethodDecl *>(function);

    if (method) {
        qualCls = method->getParent()->getQualifiedNameAsString();
        isConst = method->isConst();
        isStatic = method->isStatic();

        // Skip out-of-class nodes.
        if (method->getPreviousDecl()) { return; }

        // Skip constructors abstract classes.
        const clang::CXXRecordDecl *parent = method->getParent();
        if (parent->isAbstract()) {
            if (dynamic_cast<const clang::CXXConstructorDecl*>(method)) { return; }
        }

        // Skip bad member operator overloads.
        if (isOperator) {
            if (validOps.find(name.substr(OPERATOR.size())) == validOps.end()) {
                std::cerr << "bindee: info: skipping unsupported operator overload: "
                          << qualName << "\n";
                return;
            }
        }

        // Skip if inside anonymous record.
        const clang::RecordDecl *recordDecl = method->getParent();
        std::string prev = qualCls;
        while ((recordDecl = recordDecl->getOuterLexicalRecordContext())) {
            if (recordDecl->getNameAsString().empty()) { return; }
            if (recordDecl->getQualifiedNameAsString() == prev) { break; }
            else { prev = recordDecl->getQualifiedNameAsString(); }
        }

        // Skip overrides if desired.
        if (!opts.bindOverrides && method->isVirtual()) {
            std::string thisSig = methodSignature(method);
            bool skipOverride = false;

            if (method->size_overridden_methods() == 0) {
                // Check for override in templated base.
                for (const auto &base : parent->bases()) {
                    // Ultimately want the CXXRecordDecl for the template instantiation.
                    const auto qualType = base.getType();
                    const auto *inst = qualType->getAs<clang::TemplateSpecializationType>();
                    if (!inst) { continue; }
                    const auto *a = inst->getTemplateName().getAsTemplateDecl();
                    if (!a) { continue; }
                    const auto *b = dynamic_cast<const clang::ClassTemplateDecl*>(a);
                    if (!b) { continue; }
                    const auto *cxxRecordDecl = b->getTemplatedDecl();
                    if (!cxxRecordDecl) { continue; }
                    for (const auto *cxxMethodDecl : cxxRecordDecl->methods()) {
                        if (!cxxMethodDecl->isVirtual()) { continue; }
                        if (thisSig == methodSignature(cxxMethodDecl)) {
                            skipOverride = true;
                        }
                    }
                }
            } else {
                // Need to bind override if there is an overload.
                skipOverride = true;
                for (const auto other : parent->methods()) {
                    if (name == other->getNameAsString() && thisSig != methodSignature(other)) {
                        skipOverride = false;
                    }
                }
            }

            if (skipOverride) { 
                std::cerr << "bindee: toggle: skipping overriding method: "
                          << qualName << "\n";
                return;
            }
        }

        // Map aliases to their qualified names.
        std::string className = parent->getQualifiedNameAsString();
        if (qualAliasMaps.count(className) == 0) {
            qualAliasMaps[className];
            for (const auto *decl : parent->decls()) {
                if (const auto *alias = dynamic_cast<const clang::TypedefNameDecl *>(decl)) {
                    qualAliasMaps[className][alias->getNameAsString()] = alias->getQualifiedNameAsString();
                }
            }
        }
        qualAliasMap = &(qualAliasMaps.at(className));
    } else if (isOperator) {
        std::cerr << "bindee: info: skipping non-member operator overload: "
                  << qualName << "\n";
        return;
    }

    auto replace = [](const std::string &s, const std::string &old, const std::string &rep) {
        std::stringstream ss;
        std::string::size_type start{0};
        std::string::size_type pos{0};

        while ((pos = s.find(old, start)) != std::string::npos) {
            ss << s.substr(start, pos-start);
            start = pos + old.size();

            if (pos > 0) {
                auto c = s.at(pos-1);
                if (std::isalnum(c) || c == '_') {
                    ss << s.substr(pos, old.size());
                    continue;
                }
            }

            if (start < s.size()) {
                auto c = s.at(start);
                if (std::isalnum(c) || c == '_') {
                    ss << s.substr(pos, old.size());
                    continue;
                }
            }

            ss << rep;
        }
        ss << s.substr(start);
        return ss.str();
    };

    // Skip rvalue bindees.
    if ((int) ret.find("&&") != -1) {
        return;
    }

    // Qualify return type.
    qualifyAlias(ret, qualAliasMap);
    qualifyTemplate(ret, retType);

    // Erase template parameters from constructors.
    if (!isOperator) {
        auto pos = qualName.find("<");
        if (pos != std::string::npos) {
            qualName.erase(pos, qualName.size() - pos);
        }
    }

    // Get number of template parameters.
    std::unordered_map<std::string, int> templateParams;
    const auto *functionTemplateDecl = function->getDescribedFunctionTemplate();
    if (functionTemplateDecl) {
        for (const auto *param : *(functionTemplateDecl->getTemplateParameters())) {
            templateParams[param->getNameAsString()] = numTemplateParams++;
        }
    }

    // Erase "class ", "struct ", and "enum " substring.
    auto erase = [](std::string &s) {
        auto pos = s.find(CLASS_);
        if (pos != std::string::npos) {
            s.erase(pos, CLASS_.size());
        }
        pos = s.find(STRUCT_);
        if (pos != std::string::npos) {
            s.erase(pos, STRUCT_.size());
        }
        pos = s.find(ENUM_);
        if (pos != std::string::npos) {
            s.erase(pos, ENUM_.size());
        }
    };
    erase(ret);

    // Convert ret type if template type;
    for (const auto &kv : templateParams) {
        std::string rep = "@T" + std::to_string(templateParams.at(kv.first)) + "@";
        ret = replace(ret, kv.first, rep);
    }

    // Collect parameters.
    for (const auto *param : function->parameters()) {
        auto qualType = param->getType();
        std::string typeName = qualType.getAsString(_pp);

        // Skip if pointer to builtin.
        if (!opts.bindImmutables) {
            const auto *p = qualType->getAs<clang::PointerType>();
            const auto *r = qualType->getAs<clang::ReferenceType>();

            if (p || r) {
                if (p) {
                    qualType = p->getPointeeType();
                } else if (r) {
                    qualType = r->getPointeeType();
                }
                if (!qualType.isConstQualified() 
                        && (qualType->isBuiltinType() 
                            || qualType.getAsString(_pp) == "std::string")) { 
                    std::cerr << "bindee: toggle: skipping method with python-immutable parameter: "
                              << qualName << "\n";
                    return; 
                }
            }
        }

        // Skip rvalue bindees.
        if ((int) typeName.find("&&") != -1) {
            return;
        }

        // Erase "class "  and "struct " substring.
        erase(typeName);

        // Qualify parameter.
        qualifyAlias(typeName, qualAliasMap);
        qualifyTemplate(typeName, qualType);

        // Fill in function template parameters.
        for (const auto &kv : templateParams) {
            std::string rep = "@T" + std::to_string(templateParams.at(kv.first)) + "@";
            typeName = replace(typeName, kv.first, rep);
        }

        std::string defaultArg;
        if (param->hasDefaultArg()) {
            defaultArg = clang::Lexer::getSourceText(
                    clang::CharSourceRange(param->getDefaultArg()->getSourceRange(), true),
                    param->getASTContext().getSourceManager(),
                    _lo)
                .str();
        }

        params.emplace_back(typeName, param->getNameAsString(), defaultArg);
    }

    // Skip unsupported unary operator overload.
    if (isOperator) {
        bool isUnary = (qualCls.empty() && params.size() == 1) 
                   || (!qualCls.empty() && params.size() == 0);
        if (isUnary) {
            std::string op = name.substr(OPERATOR.size());
            if (!(op == "+" || op == "-")) {
                std::cerr << "bindee: info: skipping unsupported unary operator overload: "
                          << qualName << "\n";
                return;
            }
        }
    }

    FunctionBindee bindee(qualName, qualCls, numTemplateParams, ret, params, 
                          isConst, isStatic);
    _db.add(bindee);
}

void
VariableCallback::
run(const clang::ast_matchers::MatchFinder::MatchResult &result) {
    const clang::DeclaratorDecl *decl = result.Nodes.getNodeAs<clang::DeclaratorDecl>("variable");
    if (!decl) { return; }

    std::string qualName = decl->getQualifiedNameAsString();
    std::string qualCls = "";
    bool isConst = decl->getType().isConstQualified();
    bool isStatic = false;

    const auto *type = decl->getType()->getAsTagDecl();
    if (type && type->getNameAsString().empty()) { return; }

    if (const auto *varDecl = dynamic_cast<const clang::VarDecl*>(decl)) {
        if (varDecl->isCXXClassMember()) {
            isStatic = varDecl->isStaticDataMember();
            int pos = qualName.rfind(":")-1;
            if (pos > 0) {
                qualCls = qualName.substr(0, qualName.rfind(":")-1);
            }
        }
    }

    if (const auto *fieldDecl = dynamic_cast<const clang::FieldDecl*>(decl)) {
        if (const auto *recordDecl = fieldDecl->getParent()) {
            qualCls = recordDecl->getQualifiedNameAsString();

            // Skip if inside inner record.
            if (recordDecl->getOuterLexicalRecordContext()->getQualifiedNameAsString()
                    != qualCls) { return; }

            // Skip if inside anonymous record.
            std::string prev = qualCls;
            while ((recordDecl = recordDecl->getOuterLexicalRecordContext())) {
                if (recordDecl->getNameAsString().empty()) { return; }
                if (recordDecl->getQualifiedNameAsString() == prev) { break; }
                else { prev = recordDecl->getQualifiedNameAsString(); }
            }
        }
    }

    VariableBindee bindee(qualName, qualCls, isConst, isStatic);
    _db.add(bindee);
}

void EnumCallback::
run(const clang::ast_matchers::MatchFinder::MatchResult &result) {
    const clang::EnumDecl *decl = result.Nodes.getNodeAs<clang::EnumDecl>("enum");
    if (!decl) { return; }

    std::string qualName = decl->getQualifiedNameAsString();
    std::string qualCls = "";
    std::vector<std::string> templateParams;
    std::vector<std::string> values;
    bool isScoped = decl->isScoped();

    // Get qualCls and templateParams, if applicable.
    const auto *parent = decl->getParent();
    if (parent) {
        const auto *record = parent->getOuterLexicalRecordContext();
        if (record) {
            const auto *cxxRecordDecl = dynamic_cast<const clang::CXXRecordDecl*>(record);
            qualCls = cxxRecordDecl->getQualifiedNameAsString();

            if (decl->getAccess() != clang::AS_public) {
                return;
            }

            const auto *classTemplateDecl = cxxRecordDecl->getDescribedClassTemplate();
            if (classTemplateDecl) {
                // in template
                for (const auto *param : *(classTemplateDecl->getTemplateParameters())) {
                    templateParams.push_back(param->getNameAsString());
                }
            }
        }
    }

    // Get values.
    for (const auto val : decl->enumerators()) {
        values.push_back(val->getNameAsString());
    }

    EnumBindee bindee(qualName, qualCls, templateParams, values, isScoped);
    _db.add(bindee);
}

} // namespace Bindee
