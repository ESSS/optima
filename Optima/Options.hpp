// Optima is a C++ library for solving linear and non-linear constrained optimization problems
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
#include <string>
#include <vector>

// Optima includes
#include <Optima/Outputter.hpp>
#include <Optima/SaddlePointOptions.hpp>

namespace Optima {

/// The available stepping modes for some optimization algorithms.
enum StepMode
{
    /// This mode ensures that Newton steps have direction preserved.
    /// This is a more conservative approach, more often used in the optimization literature.
    Conservative,

    /// This mode permits that components of the Newton steps that would not violate bounds, are not affected.
    /// Although not conventional, this more aggressive stepping approach results in faster convergence in many cases.
    Aggressive
};

/// A type that describes the options for the output of a optimization calculation
struct OutputOptions : OutputterOptions
{
    /// The prefix for the primal variables `x`.
    std::string xprefix = "x";

    /// The prefix for the dual variables `y`.
    std::string yprefix = "y";

    /// The prefix for the dual variables `z`.
    std::string zprefix = "z";

    /// The prefix for the dual variables `w`.
    std::string wprefix = "w";

    /// The names of the primal variables `x`.
    /// Numbers will be used if not properly set (e.g., `x[0]`, `x[1]`)
    std::vector<std::string> xnames;

    /// The names of the dual variables `y`.
    /// Numbers will be used if not properly set (e.g., `y[0]`, `y[1]`)
    std::vector<std::string> ynames;

    /// Assign a boolean value to `active` member.
    auto operator=(bool active) -> OutputOptions&;
};

/// The options for the linear search minimization operation.
struct LineSearchOptions
{
    /// The tolerance in the minimization calculation during the line search operation.
    double tolerance = 1.0e-5;

    /// The maximum number of iterations during the minimization calculation in the line search operation.
    double maxiters = 5;

    /// The parameter that triggers line-search when current error is greater than initial error by a given factor (`Enew > factor*E0`).
    double trigger_when_current_error_is_greater_than_initial_error_by_factor = 1.0;

    /// The parameter that triggers line-search when current error is greater than previous error by a given factor (`Enew > factor*Eold`).
    double trigger_when_current_error_is_greater_than_previous_error_by_factor = 10.0;
};

/// The options for the backtrack linear search operation.
struct BacktrackSearchOptions
{
    /// The factor between 0 and 1 used to decrease the Newton length in each backtrack step.
    double factor = 0.1;

    /// The maximum number of iterations during the backtrack search operations.
    double maxiters = 10;
};

/// The options for the steepest descent step operation when needed.
struct SteepestDescentOptions
{
    /// The tolerance in the minimization calculation during the steepest descent operation.
    double tolerance = 1.0e-6;

    /// The maximum number of iterations during the minimization calculation during the steepest descent operation.
    double maxiters = 10;
};

/// A type that describes the options of a optimization calculation
class Options
{
public:
    /// The options for the output of the optimization calculations
    OutputOptions output;

    /// The tolerance for the residual of the optimality conditions.
    double tolerance = 1.0e-6;

    /// The tolerance for the variation in primal variables x.
    /// Set this to a value greater than zero to stop the calculation
    /// whenever `max(abs(dx)) < tolerancex`, where `dx` is the current step
    /// of the primal variables.
    double tolerancex = 0.0;

    /// The tolerance for the variation in objective value.
    /// Set this to a value greater than zero to stop the calculation
    /// whenever `abs(f - fprev) < tolerancef`, where `f` and `fprev` are the
    /// current and previous value of the objective function.
    double tolerancef = 0.0;

    /// The relative tolerance for the linear equality contraints.
    double tolerance_linear_equality_constraints = 1.0e-14;

    /// The maximum number of iterations in the optimization calculations.
    unsigned max_iterations = 200;

    /// The perturbation/barrier parameter for the interior-point method.
    double mu = 1.0e-20;

    /// The fraction-to-the boundary parameter to relax the line-search backtracking step.
    /// This parameter should be carefully selected as it can mistakenly drive some
    /// primal variables prematurely to the bounds, keeping them trapped there until convergence.
    /// The closest this parameter is to one, the more this effect is probable.
    double tau = 0.99;

    /// The step mode for the Newton updates.
    StepMode step = Aggressive;

    /// The options for the solution of the KKT equations.
    SaddlePointOptions kkt;

    /// The options for the linear search minimization operation.
    LineSearchOptions linesearch;

    /// The options for the steepest descent step operation when needed.
    SteepestDescentOptions steepestdescent;

    /// The options for the backtrack linear search operation.
    BacktrackSearchOptions backtrack;
};

} // namespace Optima
