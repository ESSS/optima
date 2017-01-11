// Optima is a C++ library for numerical solution of linear and nonlinear programing problems.
//
// Copyright (C) 2014-2017 Allan Leal
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
#include <memory>

// Optima includes
#include <Optima/Math/Matrix.hpp>

namespace Optima {

// Forward declarations
struct OptimumOptions;
struct OptimumProblem;
struct OptimumResult;
struct OptimumState;

/// The base class for all optimization algorithms.
class OptimumSolverBase
{
public:
    /// Pure virtual destructor
    virtual ~OptimumSolverBase() = 0;

    /// Solve an optimization problem.
    /// @param problem The definition of the optimization problem
    /// @param state[in,out] The initial guess and the final state of the optimization calculation
    virtual auto solve(const OptimumProblem& problem, OptimumState& state) -> OptimumResult = 0;

    /// Solve an optimization problem with given options.
    /// @param problem The definition of the optimization problem
    /// @param state[in,out] The initial guess and the final state of the optimization calculation
    /// @param options The options for the optimization calculation
    virtual auto solve(const OptimumProblem& problem, OptimumState& state, const OptimumOptions& options) -> OptimumResult = 0;

    /// Return the sensitivity `dx/dp` of the solution `x` with respect to a vector of parameters `p`.
    /// @param dgdp The derivatives `dg/dp` of the objective gradient `grad(f)` with respect to the parameters `p`
    /// @param dbdp The derivatives `db/dp` of the vector `b` with respect to the parameters `p`
    virtual auto dxdp(const Vector& dgdp, const Vector& dbdp) -> Vector = 0;

    /// Return a clone of this instance.
    virtual auto clone() const -> OptimumSolverBase* = 0;
};

} // namespace Optima
