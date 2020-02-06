/*
 * Copyright (c) 2020, Lawrence Livermore National Security, LLC and other
 * bindee developers. See the top-level LICENSE file for details.
 *
 * SPDX-License-Identifier: MIT
 */
#include "llvm/Support/raw_ostream.h"

#include "diagnostics.hh"

namespace Bindee {
    
void
ErrorCountDiagnosticConsumer::
HandleDiagnostic(clang::DiagnosticsEngine::Level diagLevel,
                 const clang::Diagnostic &info)
{
    llvm::SmallVector<char, 128> msg;
    info.FormatDiagnostic(msg);

    const char* level = 0;
    using Level = decltype(diagLevel);
    switch (diagLevel) {
        case Level::Ignored:
            level = "ignored";
            break;
        case Level::Note:
            level = "note";
            break;
        case Level::Remark:
            level = "remark";
            break;
        case Level::Warning:
            level = "warning";
            break;
        case Level::Error:
            level = "error";
            break;
        case Level::Fatal:
            level = "fatal";
            break;
    }

    llvm::errs() << info.getLocation().printToString(info.getSourceManager()) 
                 << ": " << level 
                 << ": " << msg << "\n";

    if (diagLevel == Level::Fatal || diagLevel == Level::Error) {
        NumErrors++;
    }
}

} // namespace Bindee
