/*
 * Copyright (c) 2020, Lawrence Livermore National Security, LLC and other
 * bindee developers. See the top-level LICENSE file for details.
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef DIAGNOSTICS_HH
#define DIAGNOSTICS_HH
#include "clang/Basic/Diagnostic.h"

namespace Bindee {
    
class ErrorCountDiagnosticConsumer : public clang::DiagnosticConsumer {
public:
    virtual void HandleDiagnostic(clang::DiagnosticsEngine::Level DiagLevel,
                                  const clang::Diagnostic &Info) override;
};

} // namespace Bindee

#endif // DIAGNOSTICS_HH
