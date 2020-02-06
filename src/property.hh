/*
 * Copyright (c) 2020, Lawrence Livermore National Security, LLC and other
 * bindee developers. See the top-level LICENSE file for details.
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef PROPERTY_HH
#define PROPERTY_HH

#include <string>

namespace Bindee {
    
struct FunctionBindee;
struct Info;

struct Property {
    std::string bind(const Info &info) const;
    std::string propertyName() const;
    bool isValidIfStatic() const;

    const FunctionBindee *getter = nullptr;
    const FunctionBindee *setter = nullptr;
    bool fill = false;
};

} // namespace Bindee

#endif // PROPERTY_HH
