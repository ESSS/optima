// Optima is a C++ library for numerical solution of linear and nonlinear programing problems.
//
// Copyright (C) 2014-2018 Allan Leal
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

// C++ includes
#include <functional>

// Optima includes
#include <Optima/VariantMatrix.hpp>

namespace Optima {

// Forward declarations
class OptimumStructure;

/// The requirements in the evaluation of the objective function.
class ObjectiveRequirement
{
public:
    /// The boolean flag that indicates the need for the objective value.
    bool f = true;

    /// The boolean flag that indicates the need for the objective gradient.
    bool g = true;

    /// The boolean flag that indicates the need for the objective Hessian.
    bool H = true;
};

/// The result of the evaluation of an objective function.
/// @see ObjectiveFunction
class ObjectiveResult
{
public:
    /// The evaluated value of the objective function.
    double f;

    /// The evaluated gradient of the objective function.
    Vector g;

    /// The evaluated Hessian of the objective function.
    VariantMatrix H;

    /// The requirements in the evaluation of the objective function.
    ObjectiveRequirement requires;

    /// The boolean flag that indicates if the objective function evaluation failed.
    bool failed;

    /// Construct a default ObjectiveResult instance.
    ObjectiveResult();

    /// Construct a ObjectiveResult instance with given optimization structure.
    ObjectiveResult(const OptimumStructure& structure);
};

/// The functional signature of an objective function.
/// @param x The values of the variables \eq{x}.
/// @return An ObjectiveResult object with the evaluated result of the objective function.
using ObjectiveFunction = std::function<ObjectiveResult(VectorConstRef)>;

} // namespace Optima
