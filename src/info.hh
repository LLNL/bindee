/*
 * Copyright (c) 2020, Lawrence Livermore National Security, LLC and other
 * bindee developers. See the top-level LICENSE file for details.
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef INFO_HH
#define INFO_HH

namespace Bindee {

struct Info {
    Info(std::string name, bool ct) :
        className(name),
        isClassTemplate(ct)
    {}

    const std::string className;
    const bool isClassTemplate;
};

} // namespace Bindee

#endif // INFO_HH
