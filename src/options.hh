/*
 * Copyright (c) 2020, Lawrence Livermore National Security, LLC and other
 * bindee developers. See the top-level LICENSE file for details.
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef OPTIONS_HH
#define OPTIONS_HH

#include <string>

namespace Bindee {

class Options {
public:
    std::string headerPath;
    std::string outputPath;
    std::string inputFile;
    bool dumpMode;
    bool inputMode;
    bool pipelineMode;
    bool useProp;
    bool useChain;
    bool bindOverrides;
    bool bindImmutables;
    bool declare;

    std::string headerFile() { return getFile(headerPath); }
    std::string outputFile() { return getFile(outputPath); }

private:
    static std::string getFile(const std::string &path) {
        std::string ret(path);

        auto pos = ret.rfind("/");
        if (pos != std::string::npos) {
            ret = ret.substr(pos+1);
        }

        return ret;
    }
};

extern Options opts;

} // namespace Bindee

#endif // OPTIONS_HH
