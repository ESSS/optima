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
#include <memory>

// Optima includes
#include <Optima/Math/Matrix.hpp>

namespace Optima {

// Forward declarations
struct OptimumOptions;
struct OptimumProblem;
struct OptimumResult;
struct OptimumSensitivity;
struct OptimumState;
enum class OptimumMethod;

/// The friendly interface to all optimization algorithms.
class OptimumSolver
{
public:
    /// Construct a default OptimumSolver instance.
    OptimumSolver();

    /// Construct an OptimumSolver instance with given method.
    OptimumSolver(OptimumMethod method);

    /// Construct a copy of an OptimumSolver instance.
    OptimumSolver(const OptimumSolver& other);

    /// Destroy this OptimumSolver instance.
    virtual ~OptimumSolver();

    /// Assign a copy of an OptimumSolver instance.
    auto operator=(OptimumSolver other) -> OptimumSolver&;

    /// Set the optimization method.
    auto setMethod(OptimumMethod method) -> void;

    /// Find an initial guess for an optimization problem.
    /// @param problem The definition of the optimization problem
    /// @param state[in,out] The initial guess and the final state of the optimization approximation
    auto approximate(const OptimumProblem& problem, OptimumState& state) -> OptimumResult;

    /// Find an initial guess for an optimization problem with given options.
    /// @param problem The definition of the optimization problem
    /// @param state[in,out] The initial guess and the final state of the optimization approximation
    /// @param options The options for the optimization calculation
    auto approximate(const OptimumProblem& problem, OptimumState& state, const OptimumOptions& options) -> OptimumResult;

    /// Solve an optimization problem.
    /// @param problem The definition of the optimization problem
    /// @param state[in,out] The initial guess and the final state of the optimization calculation
    auto solve(const OptimumProblem& problem, OptimumState& state) -> OptimumResult;

    /// Solve an optimization problem with given options.
    /// @param problem The definition of the optimization problem
    /// @param state[in,out] The initial guess and the final state of the optimization calculation
    /// @param options The options for the optimization calculation
    auto solve(const OptimumProblem& problem, OptimumState& state, const OptimumOptions& options) -> OptimumResult;

    /// Return the sensitivity `dx/dp` of the solution `x` with respect to a vector of parameters `p`.
    /// @param dgdp The derivatives `dg/dp` of the objective gradient `grad(f)` with respect to the parameters `p`
    /// @param dbdp The derivatives `db/dp` of the vector `b` with respect to the parameters `p`
    auto dxdp(const Vector& dgdp, const Vector& dbdp) -> Vector;

private:
    struct Impl;

    std::unique_ptr<Impl> pimpl;
};

} // namespace Optima
