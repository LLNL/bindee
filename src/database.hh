/*
 * Copyright (c) 2020, Lawrence Livermore National Security, LLC and other
 * bindee developers. See the top-level LICENSE file for details.
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef DATABASE_HH
#define DATABASE_HH
#include <memory>
#include <unordered_set>
#include <map>
#include <fstream>

#include "bindee.hh"

namespace Bindee {
    
struct Info;

class Database {
public:
    static const std::string INDENT;

    Database();

    int addTargets(const std::string &filename);

    void add(const RecordBindee &bindee);
    void add(const FunctionBindee &bindee);
    void add(const VariableBindee &bindee);
    void add(const EnumBindee &bindee);

    int dump() const;
    int codegen() const;

private:
    using Bptr = std::unique_ptr<Bindee>;
    static const std::string DELIM;

    void _header(std::ofstream &ofs, 
                 const std::string &headerGuard) const;
    void _footer(std::ofstream &ofs,
                 const std::string &headerGuard) const;
    void _bind(std::ofstream &ss, 
               const std::vector<Bptr> &bindees,
               const Info &info) const;
    void _bindGlobals(std::ofstream &ss, 
                      const std::vector<Bptr> &bindees) const;
    void _bindProperties(std::ofstream &ofs,
                         const Info &info,
                         const std::unordered_set<std::string> &funcNames) const;

    // bindees.
    std::map<std::string, Bptr> _records;
    std::map<std::string, std::vector<Bptr> > _constructors;
    std::map<std::string, std::vector<Bptr> > _getters;
    std::map<std::string, std::vector<Bptr> > _setters;
    std::map<std::string, std::vector<Bptr> > _functions;
    std::map<std::string, std::vector<Bptr> > _variables;
    std::vector<Bptr> _enums;
    std::vector<Bptr> _freeFunctions;
    std::vector<Bptr> _freeVariables;

    // registry.
    std::unordered_set<std::string> _added;
    std::unordered_set<std::string> _targets;
};

} // namespace Bindee

#endif // DATABASE_HH
