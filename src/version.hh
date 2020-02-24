/*
 * Copyright (c) 2020, Lawrence Livermore National Security, LLC and other
 * bindee developers. See the top-level LICENSE file for details.
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef VERSION_HH
#define VERSION_HH

#define VERSION_MAJOR 2
#define VERSION_MINOR 1
#define VERSION_PATCH 1

#include <string>

namespace Bindee {

std::string version() {
    return std::to_string(VERSION_MAJOR) + "." 
         + std::to_string(VERSION_MINOR) + "."
         + std::to_string(VERSION_PATCH);
};

} // namespace Bindee

#endif // VERSION_HH
