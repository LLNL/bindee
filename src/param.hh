/*
 * Copyright (c) 2020, Lawrence Livermore National Security, LLC and other
 * bindee developers. See the top-level LICENSE file for details.
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef PARAM_HH
#define PARAM_HH
#include <utility>

namespace Bindee {

struct Param {
    Param(std::string t, std::string n, std::string d) :
        type(std::move(t)), name(std::move(n)), defaultArg(std::move(d))
    {}

    const std::string type;
    const std::string name;
    const std::string defaultArg;
};

} // namespace Bindee
 
#endif // PARAM_HH
