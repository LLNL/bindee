/*
 * Copyright (c) 2020, Lawrence Livermore National Security, LLC and other
 * bindee developers. See the top-level LICENSE file for details.
 *
 * SPDX-License-Identifier: MIT
 */
#include <iostream>
#include <cstdio>

#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

#include "options.hh"
#include "callbacks.hh"
#include "database.hh"
#include "diagnostics.hh"

using namespace clang;
using namespace clang::tooling;
using namespace clang::ast_matchers;

namespace Bindee {
    Options opts;
}

int run(ClangTool &clangTool, std::string inputFile = "");

int main(int argc, const char **argv) {

    llvm::cl::OptionCategory optionCategory("bindee options");
    llvm::cl::opt<bool> moduleOpt("m",
            llvm::cl::desc("Dump a generic module file."),
            llvm::cl::init(false),
            llvm::cl::cat(optionCategory));
    llvm::cl::opt<std::string> inputFile("i", 
            llvm::cl::value_desc("file"), 
            llvm::cl::init(""),
            llvm::cl::desc("Generate a bind file using an input file."),
            llvm::cl::cat(optionCategory));
    llvm::cl::opt<bool> dumpOpt("d",
            llvm::cl::desc("Generate an input file containing bindable names."),
            llvm::cl::init(false),
            llvm::cl::cat(optionCategory));
    llvm::cl::opt<std::string> outputOpt("o",
            llvm::cl::value_desc("file"), 
            llvm::cl::desc("Place output in a user-specified file."),
            llvm::cl::init(""),
            llvm::cl::ZeroOrMore,
            llvm::cl::cat(optionCategory));
    llvm::cl::opt<bool> propertyOpt("prop",
            llvm::cl::desc("Toggle property detection for getters and setters."),
            llvm::cl::init(true),
            llvm::cl::ZeroOrMore,
            llvm::cl::cat(optionCategory));
    llvm::cl::opt<bool> chainOpt("chain",
            llvm::cl::desc("Toggle chained function calls."),
            llvm::cl::init(false),
            llvm::cl::ZeroOrMore,
            llvm::cl::cat(optionCategory));
    llvm::cl::opt<bool> overrideOpt("override",
            llvm::cl::desc("Toggle overridden method bindings."),
            llvm::cl::init(false),
            llvm::cl::ZeroOrMore,
            llvm::cl::cat(optionCategory));
    llvm::cl::opt<bool> declareOpt("declare",
            llvm::cl::desc("Toggle declarations for bind functions."),
            llvm::cl::init(true),
            llvm::cl::ZeroOrMore,
            llvm::cl::cat(optionCategory));
    llvm::cl::opt<bool> immutableOpt("immutable",
            llvm::cl::desc("Toggle bindings for python-immutable parameters."),
            llvm::cl::init(false),
            llvm::cl::ZeroOrMore,
            llvm::cl::cat(optionCategory));

    CommonOptionsParser optsParser(argc, argv, optionCategory, llvm::cl::ZeroOrMore);
    ClangTool clangTool(optsParser.getCompilations(), 
                        optsParser.getSourcePathList());

    std::string welcome = 
        "welcome! bindee is a best-effort, binding tool for pybind11.\n\n"
        "~~~~~~~~~~~~~~~~~~~~ USAGE ~~~~~~~~~~~~~~~~~~~~\n"
        "1. bindee header -- compile_flags\n"
        "   a. bindee -d header -- compile_flags\n"
        "   b. bindee -i input header -- compile_flags\n"
        "2. bindee -m\n"
        "3. bindee -help\n"
        "Note: usage (1) pipelines (a) and (b).\n"
        "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n"
        "~~~~~~~~~~~~~~~~~~~~~~~~~ INSTRUCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
        " 0. Determine zero-error compile_flags for best results.\n"
        " 1. Generate an input file with the '-d' flag.\n"
        " 2. Remove any unnecessary names from the input file.\n"
        " 3. Generate a bind file with the '-i input_file' flag.\n"
        " 4. Replace any '@TEXT@'-s in the bind file.\n"
        " 5. Remove any unnecessary overloads.\n"
        " 6. Manually add any skipped bindings, if shown.\n"
        " 7. Manually add any non-trivial bindings.\n"
        " 8. Repeat steps 1-7, if applicable.\n"
        " 9. Generate a module file with the '-m' flag.\n"
        "10. Replace any '@TEXT@'-s in the module file.\n"
        "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";

    if (argc == 1) { 
        // Instructions.
        std::cout << welcome << "\n";
        return 0;
    } 

    if (moduleOpt) {
        std::string filename;
        if (outputOpt.empty()) {
            filename = "module.cc";
        } else {
            filename = outputOpt;
        }

        std::ofstream ofs(filename);
        if (!ofs.is_open()) {
            std::cerr << "error: cannot open '" << filename << "'\n";
            std::cerr << "error: failed to write module file.\n";
            return 1;
        }

        std::cout << "Writing: " << filename << "\n";
        ofs << "#include <pybind11/pybind11.h>\n\n"
            << "@BIND_INCLUDES@\n\n"
            << "PYBIND11_MODULE(@MODULE_NAME@, m) {\n"
            << "    m.doc() = @DOC_STRING@;\n"
            << "    @BIND_CALLS@\n"
            << "}\n";
        ofs.close();
        return 0;
    }

    bool dumpMode = dumpOpt;
    bool inputMode = !inputFile.empty();
    bool pipelineMode = !(inputMode || dumpMode);

    if (optsParser.getSourcePathList().size() == 0) {
        if (dumpMode || inputMode) {
            std::cerr << "error: missing header file.\n";
            return 1;
        } else {
            std::cout << welcome << "\n";
            return 0;
        }
    }

    Bindee::Options &opts = Bindee::opts;
    opts.headerPath     = optsParser.getSourcePathList().front();
    opts.outputPath     = outputOpt;
    opts.dumpMode       = dumpMode;
    opts.useProp        = propertyOpt;
    opts.useChain       = chainOpt;
    opts.bindOverrides  = overrideOpt;
    opts.bindImmutables = immutableOpt;
    opts.declare        = declareOpt;

    if (inputMode && dumpMode) {
        std::cerr << "error: cannot use -i and -d together.\n";
        return 1;
    }

    // Add default flags.
    ArgumentsAdjuster cppLang = getInsertArgumentAdjuster("-xc++", ArgumentInsertPosition::BEGIN);
    clangTool.appendArgumentsAdjuster(cppLang);

#ifdef DEFAULT_FLAGS
#define STR_(x) #x
#define STR(x) STR_(x)
    std::stringstream ss(STR(DEFAULT_FLAGS));
#undef STR
#undef STR_

    std::string s;
    while (ss >> s) {
        ArgumentsAdjuster flag = getInsertArgumentAdjuster(s.c_str());
        clangTool.appendArgumentsAdjuster(flag);
    }
#endif

    // Run.
    int rc = 0;
    if (pipelineMode) {
        if (opts.outputPath.empty()) {
            opts.outputPath = "bind." + opts.headerFile();
        }
        std::string origPath(opts.outputPath);

        inputFile = opts.headerFile();
        inputFile = inputFile.substr(0, inputFile.rfind(".")) + ".in";

        // Dump.
        opts.dumpMode = true;
        opts.outputPath = inputFile;
        rc = run(clangTool);
        if (rc) { return rc; }
        std::cout << std::endl;

        // Bind.
        opts.dumpMode = false;
        opts.outputPath = origPath;
        rc = run(clangTool, inputFile);
        if (rc) { return rc; }

        std::cout << "\nDeleting: " << inputFile << "\n";
        rc = std::remove(inputFile.c_str());
        if (rc) { std::cout << "error: " << std::strerror(errno) << "\n"; }
    } else if (dumpMode) {
        if (opts.outputPath.empty()) {
            std::string outFile(opts.headerFile());
            outFile = outFile.substr(0, outFile.rfind("."));
            opts.outputPath = outFile + ".in";
        }
        rc = run(clangTool);
    } else if (inputMode) {
        if (opts.outputPath.empty()) {
            opts.outputPath = "bind." + opts.headerFile();
        }
        rc = run(clangTool, inputFile);
    }

    return rc;
}

int run(ClangTool &clangTool, std::string inputFile) {
    int rc = 0;

    // '_Bool' -> 'bool'
    LangOptions lo;
    PrintingPolicy pp(lo);
    pp.Bool = 1;

    // Database.
    Bindee::Database db;
    if (!inputFile.empty()) {
        rc = db.addTargets(inputFile);
        if (rc != 0) { return rc; }
    }

    // Matchers.
    DeclarationMatcher recordMatcher = 
        cxxRecordDecl(
            isExpansionInMainFile(),
            isDefinition(),
            unless(isImplicit()),
            unless(anyOf(isPrivate(), isProtected())),
            unless(classTemplateSpecializationDecl()))
        .bind("record");
    DeclarationMatcher functionMatcher = 
        functionDecl(
            isExpansionInMainFile(),
            unless(cxxDestructorDecl()),
            unless(anyOf(isPrivate(), isProtected())),
            unless(hasAncestor(classTemplateSpecializationDecl())))
        .bind("function");
    DeclarationMatcher variableMatcher = 
        declaratorDecl(
            isExpansionInMainFile(),
            unless(anyOf(isPrivate(), isProtected())),
            anyOf(
                varDecl(
                    hasGlobalStorage(),
                    unless(hasAncestor(functionDecl()))), 
                fieldDecl(
                    isPublic(),
                    unless(hasAncestor(classTemplateSpecializationDecl())))))
        .bind("variable");
    DeclarationMatcher enumMatcher = 
        enumDecl(
            isExpansionInMainFile(),
            isDefinition())
        .bind("enum");

    // Callbacks.
    Bindee::RecordCallback recordCallback(db, pp, lo);
    Bindee::FunctionCallback functionCallback(db, pp, lo);
    Bindee::VariableCallback variableCallback(db, pp, lo);
    Bindee::EnumCallback enumCallback(db, pp, lo);

    // Finder.
    MatchFinder finder;
    finder.addMatcher(recordMatcher, &recordCallback);
    finder.addMatcher(functionMatcher, &functionCallback);
    finder.addMatcher(variableMatcher, &variableCallback);
    finder.addMatcher(enumMatcher, &enumCallback);

    // Run.
    std::cout << "!! processing; bad statements will not generate ";
    if (inputFile.empty()) { std::cout << "names.\n"; }
    else                   { std::cout << "bindings.\n"; }
    Bindee::ErrorCountDiagnosticConsumer dc;
    clangTool.setDiagnosticConsumer(&dc);
    clangTool.run(newFrontendActionFactory(&finder).get());

    if (dc.getNumErrors()) {
        std::cerr << ">> bindee: detected error; exiting.\n";
        return 1;
    }
    std::cout << "!! processing; done." << "\n\n";
    
    if (inputFile.empty()) {
        rc = db.dump();
        if (rc) {
            std::cerr << "error: failed to write input file.\n";
        }
    } else {
        rc = db.codegen();
        if (rc) {
            std::cerr << "error: failed to write bind files.\n";
        }
    }

    return 0;
}
