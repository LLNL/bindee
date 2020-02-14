/*
 * Copyright (c) 2020, Lawrence Livermore National Security, LLC and other
 * bindee developers. See the top-level LICENSE file for details.
 *
 * SPDX-License-Identifier: MIT
 */
#include <iostream>
#include <sstream>
#include <set>
#include <cctype>
#include <unordered_map>

#include "database.hh"
#include "info.hh"

#include "property.hh"
#include "options.hh"
#include "version.hh"

namespace Bindee {
    
const std::string Database::INDENT = "    ";
const std::string Database::DELIM = "!@#$";

namespace {
void 
write(std::ofstream &ofs, const std::string &s) {
    if (!s.empty()) {
        if (opts.useChain) {
            ofs << Database::INDENT << Database::INDENT << s << "\n";
        } else {
            ofs << Database::INDENT << BINDEE << s << ";\n";
        }
    }
}

void 
toId(std::string &s, bool upper) {
    for (auto &c : s) {
        if (!(std::isalnum(c) || c == '_')) {
            c = '_';
        } else if (upper) {
            c = std::toupper(c);
        }
    }
}

std::string
createFunction(const std::string &name, 
               const int funcIdx,
               const int funcCount,
               const std::vector<std::string> &templateParams,
               const bool isEnum = false)
{
    const bool isClassTemplate = templateParams.size() > 0;
    decltype(templateParams.size()) i{0};
    std::stringstream ss;

    bool first = true;
    ss << "template <";
    if (isClassTemplate) {
        first = false;
        i = 0;
        ss << "typename " << templateParams.at(i);
        for (i=1; i<templateParams.size(); i++) {
            ss << ", typename " << templateParams.at(i);
        }
    }
    if (!first) {
        ss << ", ";
    }
    ss << "typename Target>\n";

    std::string uniq;
    if (funcCount > 1) {
        uniq = "_" + std::to_string(funcIdx);
    }

    ss << "void bind_" << name << uniq << "(Target &" << TARGET;
    if (!isEnum && isClassTemplate) {
        ss << ", const std::string &classPrefix";
    }
    ss << ")";

    return ss.str();
}

} // anonymous namespace

Database::
Database()
{
    FunctionBindee::resetRegistry();
}

int 
Database::
addTargets(const std::string &filename)
{
    std::ifstream ifs(filename);
    std::string line;

    if (ifs.is_open()) {
        while (std::getline(ifs, line)) {
            _targets.insert(line);
        }
    } else {
        std::cerr << "error: cannot open file: " << filename << std::endl;
        return 1;
    }

    return 0;
}

void 
Database::
add(const RecordBindee &bindee)
{
    // Only bind specified targets.
    if (opts.inputMode && _targets.find(bindee.qualName) == _targets.end()) {
        return;
    }

    // Check for duplicate.
    if (_added.find(bindee.qualName) != _added.end()) {
        return; 
    }
    _added.insert(bindee.qualName);

    // Save it.
    _records[bindee.qualName] = Bptr(new RecordBindee(bindee));
}

void 
Database::
add(const FunctionBindee &bindee)
{
    // Only bind specified targets.
    if (opts.inputMode && _targets.find(bindee.qualName) == _targets.end()) {
        return;
    }

    // Check for duplicate.
    std::string name(bindee.qualName);
    if (!opts.dumpMode) {
        std::stringstream ss;
        ss << bindee.qualName << DELIM << bindee.ret << bindee.numTemplateParams;
        for (const auto &param : bindee.params) {
            ss << param.type;
        }
        if (bindee.isConst) {
            ss << "const";
        }
        name = ss.str();
    }
    if (_added.find(name) != _added.end()) {
        return; 
    }
    _added.insert(name);

    // Save it.
    if (bindee.qualCls.empty()) {
        _freeFunctions.push_back(Bptr(new FunctionBindee(bindee)));
    } else {
        FunctionBindee::registerMethod(bindee);
        if (bindee.isConstructor()) {
            _constructors[bindee.qualCls].push_back(Bptr(new FunctionBindee(bindee)));
        } else if (bindee.isGetter()) {
            _getters[bindee.qualCls].push_back(Bptr(new FunctionBindee(bindee)));
        } else if (bindee.isSetter()) {
            _setters[bindee.qualCls].push_back(Bptr(new FunctionBindee(bindee)));
        } else {
            _functions[bindee.qualCls].push_back(Bptr(new FunctionBindee(bindee)));
        }
    }
}

void 
Database::
add(const VariableBindee &bindee)
{
    // Only bind specified targets.
    if (opts.inputMode && _targets.find(bindee.qualName) == _targets.end()) {
        return;
    }

    // Check for duplicate.
    if (_added.find(bindee.qualName) != _added.end()) {
        return;
    }
    _added.insert(bindee.qualName);

    // Save it.
    if (bindee.qualCls.empty()) {
        _freeVariables.push_back(Bptr(new VariableBindee(bindee)));
    } else {
        _variables[bindee.qualCls].push_back(Bptr(new VariableBindee(bindee)));
    }
}

void 
Database::
add(const EnumBindee &bindee)
{
    // Only bind specified targets.
    if (opts.inputMode && _targets.find(bindee.qualName) == _targets.end()) {
        return;
    }

    // Check for duplicate.
    if (_added.find(bindee.qualName) != _added.end()) {
        return; 
    }
    _added.insert(bindee.qualName);

    // Save it.
    _enums.push_back(Bptr(new EnumBindee(bindee)));
}

int 
Database::
dump() const
{
    std::string filename(opts.outputPath);

    std::ofstream ofs(filename);
    if (!ofs.is_open()) {
        std::cerr << "error: failed to open '" << filename << "'\n";
        return 1;
    }

    std::cout << "Writing: " << filename << "\n";
    
    auto dump = [&ofs](const std::vector<Bptr> &bindees) {
        for (const auto &bindee : bindees) {
            ofs << bindee->qualName << "\n";
        }
    };

    for (const auto &kv : _records) {
        const std::string &className = kv.first;
        ofs << className << "\n";
        if (_constructors.count(className) > 0) {
            dump(_constructors.at(className));
        }
        if (_getters.count(className) > 0) {
            dump(_getters.at(className));
        }
        if (_setters.count(className) > 0) {
            dump(_setters.at(className));
        }
        if (_functions.count(className) > 0) {
            dump(_functions.at(className));
        }
        if (_variables.count(className) > 0) {
            dump(_variables.at(className));
        }
    }

    dump(_enums);

    dump(_freeFunctions);
    dump(_freeVariables);

    return 0;
}

int 
Database::
codegen() const
{
    // Get class names.
    std::set<std::string> classNames;
    for (const auto &kv : _records) {
        classNames.insert(kv.first);
    }
    for (const auto &kv : _constructors) {
        classNames.insert(kv.first);
    }
    for (const auto &kv : _getters) {
        classNames.insert(kv.first);
    }
    for (const auto &kv : _setters) {
        classNames.insert(kv.first);
    }
    for (const auto &kv : _functions) {
        classNames.insert(kv.first);
    }
    for (const auto &kv : _variables) {
        classNames.insert(kv.first);
    }

    // Create file.
    std::ofstream ofs(opts.outputPath);
    if (!ofs.is_open()) {
        std::cerr << "error: failed to open '" << opts.outputPath << "'\n";
        return 1;
    }

    // Begin writing.
    std::cout << "Writing: " << opts.outputPath << "\n";

    std::string headerGuard(opts.outputFile());
    toId(headerGuard, true);

    _header(ofs, headerGuard);

    std::unordered_map<std::string, int> funcCount;
    std::unordered_map<std::string, int> funcIdx;
    for (const auto &className : classNames) {
        if (_records.count(className) == 0) { continue; }
        funcCount[_records.at(className)->name]++;
    }
    for (const auto &bptr : _enums) {
        funcCount[bptr->name]++;
    }

    if (opts.declare) {
        bool first = true;
        for (const auto &className : classNames) {
            if (_records.count(className) == 0) { continue; }
            if (first) {
                ofs << "// Records.\n\n";
                first = false;
            }
            const auto *record = dynamic_cast<RecordBindee*>(_records.at(className).get());
            const auto &name = record->name;
            const auto &templateParams = record->templateParams;
            ofs << createFunction(name, funcIdx[name]++, funcCount[name], templateParams)
                << ";\n";
        }
        if (_enums.size() > 0) {
            first = true;
            for (auto &bptr : _enums) {
                if (first) {
                    ofs << "\n// Enums.\n\n";
                    first = false;
                }
                const auto *enm = dynamic_cast<EnumBindee*>(bptr.get());
                const auto &name = enm->name;
                const auto &templateParams = enm->templateParams;
                ofs << createFunction(name, funcIdx[name]++, funcCount[name], templateParams, true)
                    << ";\n";
            }
        }
        if (!first) {
            ofs << "\n";
        }
        funcIdx.clear();
    }

    ofs << "// Bind functions.\n\n";

    for (const auto &className : classNames) {
        if (_records.count(className) == 0) {
            std::cerr << "bindee: warning: add class '" << className 
                      << "' to the input file to bind its members; skipping.\n";
            continue;
        }

        const auto *record = dynamic_cast<RecordBindee*>(_records.at(className).get());
        const std::vector<std::string> &templateParams = record->templateParams;
        const bool isClassTemplate = templateParams.size() > 0;
        Info info{className, isClassTemplate};
        decltype(templateParams.size()) i{0};

        // bind-function declaration.
        const std::string &name = record->name;
        ofs << createFunction(name, funcIdx[name]++, funcCount[name], templateParams)
            << " {\n";

        // 'using' for class.
        ofs << INDENT << "using Class = " << className;
        if (isClassTemplate) {
            i = 0;
            ofs << "<" << templateParams.at(i);
            for (i=1; i<templateParams.size(); i++) {
                ofs << ", " << templateParams.at(i);
            }
            ofs << ">";
        }
        ofs << ";\n\n";

        // className and docString
        ofs << INDENT << "std::string className = ";
        if (isClassTemplate) {
            ofs << "classPrefix + \"" << name << "\"";
        } else {
            ofs << "\"" << name << "\"";
        } 
        ofs << ";\n";
        ofs << INDENT << "std::string docString = @DOC_STRING@;\n\n";

        std::unordered_set<std::string> funcNames;
        if (_functions.count(className) > 0) {
            for (const auto &function : _functions.at(className)) {
                funcNames.insert(function->name);
            }
        }

        // write binding code.
        ofs << INDENT << _records.at(className)->bind(info) << "\n";

        if (_constructors.count(className) > 0) {
            _bind(ofs, _constructors.at(className), info);
        }
        if (_getters.count(className) > 0 || _setters.count(className) > 0) {
            _bindProperties(ofs, info, funcNames);
        }
        if (_functions.count(className) > 0) {
            _bind(ofs, _functions.at(className), info);
        }
        if (_variables.count(className) > 0) {
            _bind(ofs, _variables.at(className), info);
        }

        if (opts.useChain) {
            ofs << INDENT << ";\n";
        }
        ofs << "}\n\n";
    }

    if (_enums.size() > 0) {
        for (auto &bptr : _enums) {
            auto *enm = dynamic_cast<EnumBindee*>(bptr.get());
            const auto &name = enm->name;
            const auto &qualCls = enm->qualCls;
            const auto &templateParams = enm->templateParams;
            decltype(templateParams.size()) i{0};
            const bool isClassTemplate = templateParams.size() > 0;

            Info info{"", false};

            // bind-function declaration
            ofs << createFunction(name, funcIdx[name]++, funcCount[name], templateParams, true)
                << " {\n";

            ofs << INDENT << "using Enum = ";
            if (!qualCls.empty()) {
                if (isClassTemplate) {
                    ofs << "typename " << qualCls;
                    i = 0;
                    ofs << "<" << templateParams.at(i);
                    for (i=1; i<templateParams.size(); i++) {
                        ofs << ", " << templateParams.at(i);
                    }
                    ofs << ">";
                } else {
                    ofs << qualCls;
                }
                ofs << "::";
            }
            ofs << name << ";\n\n";

            ofs << INDENT << "std::string enumName = \"" << name << "\";\n"
                << INDENT << "std::string docString = @DOC_STRING@;\n\n";

            int indentLength = INDENT.size();
            if (opts.useChain) {
                indentLength *= 2;
            }
            enm->setIndentLength(indentLength);

            ofs << enm->bind(info) << "\n"
                << "}\n\n";
        }
    }

    if ((_freeFunctions.size() > 0) || (_freeVariables.size() > 0)) {
        std::string headerBase(opts.headerFile());
        headerBase = headerBase.substr(0, headerBase.rfind("."));
        toId(headerBase, false);

        ofs << "void bind_" << headerBase << "_globals(py::module &"
            << BINDEE << ") {\n";

        if (opts.useChain) {
            ofs << INDENT << BINDEE << "\n";
        }

        _bindGlobals(ofs, _freeFunctions);
        _bindGlobals(ofs, _freeVariables);

        if (opts.useChain) {
            ofs << INDENT << ";\n";
        }

        ofs << "}\n\n";
    }

    _footer(ofs, headerGuard);

    // display skipped bindings.
    std::unordered_set<std::string> addedNames;
    for (const auto &qualName : _added) {
        auto pos = qualName.find(DELIM);
        if (pos != std::string::npos) {
            addedNames.insert(qualName.substr(0, pos));
        } else {
            addedNames.insert(qualName);
        }
    }

    bool first = true;
    for (const auto &target : _targets) {
        if (addedNames.find(target) == addedNames.end()) {
            if (first) {
                std::cout << "\n";
                first = false;
            }
            std::cout << "bindee: warning: skipped '" << target << "'\n";
        }
    }

    return 0;
}

void
Database::
_header(std::ofstream &ofs, const std::string &headerGuard) const
{
    ofs << "// Generated by bindee " << version() << " from " << opts.headerFile() << ".\n"
        << "#ifndef " << headerGuard << "\n"
        << "#define " << headerGuard << "\n\n"
        << "#include <pybind11/pybind11.h>\n";

    ofs << "namespace py = pybind11;\n\n"
        << "@INCLUDES@\n\n";
}

void
Database::
_footer(std::ofstream &ofs, const std::string &headerGuard) const
{
    ofs << "#endif // " << headerGuard << "\n";
}

void 
Database::
_bind(std::ofstream &ofs, const std::vector<Bptr> &bindees, const Info &info) const
{
    for (const auto &bindee : bindees) {
        write(ofs, bindee->bind(info));
    }
}

void 
Database::
_bindGlobals(std::ofstream &ofs, const std::vector<Bptr> &bindees) const
{
    Info info{"", false};
    for (const auto &bindee : bindees) {
        if (const auto *varBindee = dynamic_cast<const VariableBindee*>(bindee.get())) {
            write(ofs, varBindee->bindGlobal());
        } else {
            write(ofs, bindee->bind(info));
        }
    }
}

void 
Database::
_bindProperties(std::ofstream &ofs, 
                const Info &info, 
                const std::unordered_set<std::string> &funcNames) const
{
    using const_iter = decltype(_getters)::mapped_type::const_iterator;
    std::unordered_map<std::string, const_iter> gettersMap;
    std::unordered_map<std::string, const_iter> settersMap;

    // overloads cannot be bound as properties.
    std::vector<Property> properties;
    std::vector<const_iter> overloads;

    const std::string &className = info.className;

    // filter out all overloads.
    auto categorize = [&overloads](const decltype(_getters)::mapped_type &v, 
                                   decltype(gettersMap) &map) {
        std::unordered_set<std::string> names;
        for (auto it = v.cbegin(); it != v.cend(); it++) {
            std::string name = (*it)->name;
            if (names.find(name) == names.end()) {
                map[name.substr(3)] = it;
                names.insert(name);
            } else {
                overloads.push_back(it);
                overloads.push_back(map.at(name.substr(3)));
                map.erase(name.substr(3));
            }
        }
    };

    if (_getters.count(className) > 0) {
        categorize(_getters.at(className), gettersMap);
    }
    if (_setters.count(className) > 0) {
        categorize(_setters.at(className), settersMap);
    }

    // Gather properties.
    for (const auto &kv : gettersMap) {
        Property prop;
        prop.getter = dynamic_cast<const FunctionBindee*>(kv.second->get());

        if (settersMap.count(kv.first)) {
            prop.setter = dynamic_cast<const FunctionBindee*>(settersMap.at(kv.first)->get());
        }

        properties.push_back(prop);
        settersMap.erase(kv.first);
    }

    for (const auto &kv : settersMap) {
        Property prop;
        prop.setter = dynamic_cast<const FunctionBindee*>(kv.second->get());
        properties.push_back(prop);
    }

    // Write properties and overloads.
    for (auto &prop : properties) {
        if (opts.useProp && prop.isValidIfStatic()) {
            std::string propName = prop.propertyName();
            if (funcNames.find(propName) != funcNames.end()) {
                prop.fill = true;
            }
            write(ofs, prop.bind(info));
        } else {
            if (prop.getter) { write(ofs, prop.getter->bind(info)); }
            if (prop.setter) { write(ofs, prop.setter->bind(info)); }
        }
    }

    for (const auto it : overloads) {
        write(ofs, (*it)->bind(info));
    }
}

} // namespace Bindee
