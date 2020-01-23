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

#include "BasicSolver.hpp"

// Optima includes
#include <Optima/ActiveStepper.hpp>
#include <Optima/Constraints.hpp>
#include <Optima/Exception.hpp>
#include <Optima/Options.hpp>
#include <Optima/Outputter.hpp>
#include <Optima/Result.hpp>
#include <Optima/SaddlePointMatrix.hpp>
#include <Optima/Timing.hpp>
#include <Optima/Utils.hpp>

namespace Optima {
namespace {

auto isfinite(const ObjectiveResult& res) -> bool
{
    return std::isfinite(res.f) && res.g.allFinite();
}

//auto xStepLength(VectorConstRef x, VectorConstRef dx, VectorConstRef l, VectorConstRef u, double tau) -> double
//{
//    double alpha = 1.0;
//    const Index size = x.size();
//    for(Index i = 0; i < size; ++i)
//        alpha = (dx[i] < 0.0) ? std::min(alpha, tau*(l[i] - x[i])/dx[i]) :
//                (dx[i] > 0.0) ? std::min(alpha, tau*(u[i] - x[i])/dx[i]) : alpha;
//    return alpha;
//}
//
//auto zStepLength(VectorConstRef z, VectorConstRef dz, double tau) -> double
//{
//    double alpha = 1.0;
//    const Index size = z.size();
//    for(Index i = 0; i < size; ++i)
//        if(dz[i] < 0.0) alpha = std::min(alpha, -tau*z[i]/dz[i]);
//    return alpha;
//}
//
//auto wStepLength(VectorConstRef w, VectorConstRef dw, double tau) -> double
//{
//    double alpha = 1.0;
//    const Index size = w.size();
//    for(Index i = 0; i < size; ++i)
//        if(dw[i] > 0.0) alpha = std::min(alpha, -tau*w[i]/dw[i]);
//    return alpha;
//}

} // namespace

/// The implementation of the solver for basic optimization problems.
struct BasicSolver::Impl
{
    /// The optimization problem
    BasicProblem problem;

    /// The calculator of the Newton step (dx, dy, dz, dw)
    ActiveStepper stepper;

    /// The options for the optimization calculation
    Options options;

    /// The result of the non-linear equality constraint function h(x)
    Vector h;

    /// The Jacobian matrix J of the non-linear equality constraint function h(x)
    Matrix J;

    /// The evaluated objective function f(x).
    double f;

    /// The evaluated gradient of the objective function f(x).
    Vector g;

    /// The evaluated Hessian of the objective function f(x).
    Matrix H;

    /// The result of the optimization problem
    Result result;

    /// The Newton step for the primal variables *x*.
    Vector dx;

    /// The Newton step for the Lagrange multipliers *y*.
    Vector dy;

    /// The residuals of the first-order optimality conditions.
    Vector rx;

    /// The residuals of the linear/nonlinear feasibility conditions.
    Vector ry;

    /// The *unstabilities* of the variables defined as `z = g + tr(W)*y`.
    Vector z;

    /// The trial iterate x(trial)
    Vector xtrial;

    /// The trial Newton step dx(trial)
    Vector dxtrial;

    /// The lower bounds for each variable x (-inf with no lower bound)
    Vector xlower;

    /// The upper bounds for each variable x (+inf with no lower bound)
    Vector xupper;

    /// The number of variables
    Index n;

    /// The number free and fixed variables.
    Index nx, nf;

    /// The number of linear equality constraints in Ax = b.
    Index mb;

    /// The number of non-linear equality constraints in h(x) = 0.
    Index mh;

    /// The number of constraints.
    Index m;

    /// The total number of variables (x, y, z, w).
    Index t;

    /// The outputter instance
    Outputter outputter;

    /// Initialize the optimization solver with the objective and constraints of the problem.
    Impl()
    {}

    /// Initialize the optimization solver with the objective and constraints of the problem.
    Impl(const BasicProblem& problem)
    : problem(problem)
    {
        // Initialize the members related to number of variables and constraints
        n  = problem.dims.x;
        mb = problem.dims.b;
        mh = problem.dims.h;
        m  = mb + mh;

        // Initialize the number of fixed and free variables in x
        nf = problem.constraints.ifixed.size();
        nx = n - nf;

        // Allocate memory
        h.resize(mh);
        J.resize(mh, n);
        g.resize(n);
        H.resize(n, n);
        xtrial.resize(n);

        // Initialize xlower and xupper with -inf and +inf
        xlower = constants(n, -infinity());
        xupper = constants(n,  infinity());

        // Ensure the objective function has been set in the problem.
        Assert(problem.objective != nullptr,
            "Could not initialize BasicSolver.",
                "No objective function given (BasicProblem::objective is empty).");

        // Ensure the objective function has been set in the problem.
        Assert(mh == 0 or problem.constraints.h != nullptr,
            "Could not initialize BasicSolver.",
                "No constraint function given (BasicProblem::BasicConstraints::h is empty).");
    }

    /// Set the options for the optimization calculation.
    auto setOptions(const Options& _options) -> void
    {
    	// Set member options
    	options = _options;

        // Set the options of the optimization stepper
        stepper.setOptions(options);

        // Set the options of the outputter
        outputter.setOptions(options.output);
    }

    // Output the header and initial state of the solution
    auto outputInitialState(const BasicState& state) -> void
    {
        if(!options.output.active) return;

        // Aliases to canonical variables
        const auto& x = state.x;
        const auto& y = state.y;
        const auto& z = state.z;
        const auto& w = state.w;

        outputter.addEntry("Iteration");
        outputter.addEntry("f(x)");
        outputter.addEntry("Error");
        outputter.addEntries(options.output.xprefix, n, options.output.xnames);
        outputter.addEntries(options.output.yprefix, m, options.output.ynames);
        outputter.addEntries(options.output.zprefix, n, options.output.xnames);
        outputter.addEntries("r", n, options.output.xnames);
        outputter.addEntry("Optimality");
        outputter.addEntry("Feasibility");

        outputter.outputHeader();
        outputter.addValue(result.iterations);
        outputter.addValue(f);
        outputter.addValue(result.error);
        outputter.addValues(x);
        outputter.addValues(y);
        outputter.addValues(z);
        outputter.addValues(abs(rx));
        outputter.addValue(result.error_optimality);
        outputter.addValue(result.error_feasibility);
        outputter.outputState();
    };

    // The function that outputs the current state of the solution
    auto outputCurrentState(const BasicState& state) -> void
    {
        if(!options.output.active) return;

        // Aliases to canonical variables
        const auto& x = state.x;
        const auto& y = state.y;
        const auto& z = state.z;

        outputter.addValue(result.iterations);
        outputter.addValue(f);
        outputter.addValue(result.error);
        outputter.addValues(x);
        outputter.addValues(y);
        outputter.addValues(z);
        outputter.addValues(abs(rx));
        outputter.addValue(result.error_optimality);
        outputter.addValue(result.error_feasibility);
        outputter.outputState();
    };

    // Initialize internal state before calculation begins
    auto initialize(const BasicParams& params, BasicState& state) -> void
	{
        // Clear previous state of the Outputter instance
        outputter.clear();

        // Aliases to canonical variables
        auto& x = state.x;
        auto& y = state.y;

        // The indices of variables with lower/upper bounds and fixed values
        const auto ilower = problem.constraints.ilower;
        const auto iupper = problem.constraints.iupper;
        const auto ifixed = problem.constraints.ifixed;

        // Initialize xlower and xupper with the given lower and upper bounds
        xlower(ilower) = params.xlower;
        xupper(iupper) = params.xupper;

        // Ensure the initial guesses for x, y, z, w have proper dimensions
        Assert(x.size() == n, "Cannot solve the optimization problem.", "The size of vector x has not been properly initialized.");
        Assert(y.size() == m, "Cannot solve the optimization problem.", "The size of vector y has not been properly initialized.");

        // Ensure the initial guess for `x` does not violate lower/upper bounds
        for(Index i : ilower) x[i] = std::max(x[i], xlower[i]);
        for(Index i : iupper) x[i] = std::min(x[i], xupper[i]);

        // Set the values of x, z, w corresponding to fixed variables
        x(ifixed) = params.xfixed;

        // Evaluate the objective function
        evaluateObjectiveFunction(params, state);

        // Evaluate the constraint function
        evaluateConstraintFunction(params, state);

        // Assert the objective function produces finite numbers at this point
        Assert(std::isfinite(f) && g.allFinite() && H.allFinite(),
			"Failure evaluating the objective function.", "The evaluation of "
			"the objective function at the entry point of the optimization "
			"calculation produced non-finite numbers, "
			"such as `nan` and/or `inf`.");

        // Compute the Newton step for the current state
        computeNewtonStep(params, state);

        // Update the optimality, feasibility and complementarity errors
        updateResultErrors();
	}

    // Evaluate the objective function
    auto evaluateObjectiveFunction(const BasicParams& params, BasicState& state) -> void
	{
        // Create an ObjectiveResult with f, g, H to be filled
        ObjectiveResult res(f, g, H);

        // Establish the current needs for the objective function evaluation
        res.requires.f = true;
        res.requires.g = true;
        res.requires.H = true;

        // Evaluate the objective function f(x)
        problem.objective(state.x, res);
	}

    // Evaluate the equality constraint function
    auto evaluateConstraintFunction(const BasicParams& params, BasicState& state) -> void
	{
        // Skip if there are no non-linear equality constraints
        if(mh == 0)
            return;

        // Create a ConstraintResult with h and J to be filled
        ConstraintResult res{h, J};

        // Evaluate the constraint function h(x)
        problem.constraints.h(state.x, res);
	}

    // The function that computes the Newton step
    auto computeNewtonStep(const BasicParams& params, const BasicState& state) -> void
    {
    	Timer timer;

        const auto& x = state.x;
        const auto& y = state.y;
        const auto& b = params.b;

    	// Decompose the Jacobian matrix and calculate a Newton step
        stepper.decompose({x, y, J, g, H});

        // Calculate the Newton step
        stepper.solve({x, y, b, h, g}, {dx, dy, rx, ry, z});

        // Update the time spent in linear systems
		result.time_linear_systems += timer.elapsed();
    };

	// Update the optimality, feasibility and complementarity errors
	auto updateResultErrors() -> void
	{
		// Update the current optimality, feasibility and complementarity errors
		result.error_optimality  = norminf(rx);
		result.error_feasibility = norminf(ry);

		// Update the current maximum error of the optimization calculation
		result.error = std::max({
			result.error_optimality,
			result.error_feasibility
		});
	}

    // Update the variables (x, y, z, w) with a Newton stepping scheme
    auto applyNewtonStepping(const BasicParams& params, BasicState& state) -> void
    {
        switch(options.step) {
        case Aggressive: return applyNewtonSteppingAggressive(params, state);
        default: return applyNewtonSteppingConservative(params, state);
        }
    };

    /// Perform a backtracking line search algorithm to check if a shorter
    /// Newton step results in better residuals.
    auto applyBacktrackingLineSeach() -> void
	{
    	// Skip if first iteration

    	// TODO Use dxtrial to calculate alpha step length to decrease the stepping

//		// Establish the current needs for the objective function evaluation at the trial iterate
//		f.requires.f = true;
//		f.requires.g = false;
//		f.requires.H = false;
//
//		// Evaluate the objective function at the trial iterate


//		f = objective(xtrial);
//
//		state.f = f.f;
//		state.g = f.g;
//		state.H = f.H;
//
//		// Initialize the step length factor
//		double alpha = fractionToTheBoundary(x, dx, options.tau);
//
//		// The number of tentatives to find a trial iterate that results in finite objective result
//		unsigned tentatives = 0;
//
//		// Repeat until f(xtrial) is finite
//		while(!isfinite(f) && ++tentatives < 10)
//		{
//			// Calculate a new trial iterate using a smaller step length
//			xtrial = x + alpha * dx;
//
//			// Evaluate the objective function at the new trial iterate
//			f.requires.f = true;
//			f.requires.g = false;
//			f.requires.H = false;


//			f = objective(xtrial);
//
//			state.f = f.f;
//			state.g = f.g;
//			state.H = f.H;
//
//			// Decrease the current step length
//			alpha *= 0.5;
//		}
//
//		// Return false if xtrial could not be found s.t. f(xtrial) is finite
//		if(tentatives == 10)
//			return false;
//
//		// Update the iterate x from xtrial
//		x = xtrial;
//
//		// Update the gradient and Hessian at x
//		f.requires.f = false;
//		f.requires.g = true;
//		f.requires.H = true;


//		f = objective(x);
//
//		state.f = f.f;
//		state.g = f.g;
//		state.H = f.H;

	}

	// Update the variables (x, y, z, w) with an aggressive Newton stepping scheme
	auto applyNewtonSteppingAggressive(const BasicParams& params, BasicState& state) -> void
	{
        // Aliases to canonical variables
        auto& x = state.x;
        auto& y = state.y;

        // The indices of variables with lower/upper bounds and fixed values
        auto ilower = problem.constraints.ilower;
        auto iupper = problem.constraints.iupper;
        auto ifixed = problem.constraints.ifixed;

		// Update xtrial with the calculated Newton step
		xtrial = x + dx;

        // Ensure the initial guess for `x` does not violate lower/upper bounds
        for(Index i : ilower) xtrial[i] = std::max(xtrial[i], xlower[i]);
        for(Index i : iupper) xtrial[i] = std::min(xtrial[i], xupper[i]);

		// Calculate the trial Newton step for the aggressive mode
		dxtrial = xtrial - x;

		// Update the x variables
		x = xtrial;

		// Update the y-Lagrange multipliers
		y += dy;

		// Set the values of x, z, w corresponding to fixed variables
		x(ifixed) = params.xfixed;
	};

	// Update the variables (x, y, z, w) with a conservative Newton stepping scheme
	auto applyNewtonSteppingConservative(const BasicParams& params, BasicState& state) -> void
	{
//		// Aliases to variables x, y, z, w
//		VectorRef x = state.x;
//		VectorRef y = state.y;
//		VectorRef z = state.z;
//		VectorRef w = state.w;
//
//		// Aliases to Newton steps calculated before
//		VectorConstRef dx = stepper.step().x;
//		VectorConstRef dy = stepper.step().y;
//		VectorConstRef dz = stepper.step().z;
//		VectorConstRef dw = stepper.step().w;
//
//		// The indices of variables with lower/upper bounds and fixed values
//		IndicesConstRef ilower = constraints.variablesWithLowerBounds();
//		IndicesConstRef iupper = constraints.variablesWithUpperBounds();
//		IndicesConstRef ifixed = constraints.variablesWithFixedValues();
//
//		// Initialize the step length factor
//		double alphax = xStepLength(x, dx, xlower, xupper, tau);
//		double alphaz = zStepLength(z, dz, tau);
//		double alphaw = wStepLength(w, dw, tau);
//		double alpha = alphax;
//
//		// The number of tentatives to find a trial iterate that results in finite objective result
//		unsigned tentatives = 0;
//
//		// Repeat until a suitable xtrial iterate if found such that f(xtrial) is finite
//		for(; tentatives < 10; ++tentatives)
//		{
//			// Calculate the current trial iterate for x
//			xtrial = x + alpha * dx;
//
//			// Evaluate the objective function at the trial iterate
//			f.requires.f = true;
//			f.requires.g = false;
//			f.requires.H = false;


//			f = objective(xtrial);
//
//			state.f = f.f;
//			state.g = f.g;
//			state.H = f.H;
//
//			// Leave the loop if f(xtrial) is finite
//			if(isfinite(f))
//				break;
//
//			// Decrease alpha in a hope that a shorter step results f(xtrial) finite
//			alpha *= 0.01;
//		}
//
//		// Return false if xtrial could not be found s.t. f(xtrial) is finite
//		if(tentatives == 10)
//			return false;
//
//		// Update the iterate x from xtrial
//		x = xtrial;
//
//		// Update the z-Lagrange multipliers
//		z += alphaz * dz;
//
//		// Update the w-Lagrange multipliers
//		w += alphaw * dw;
//
//		// Update the y-Lagrange multipliers
//		y += dy;
//
//		// Update the gradient and Hessian at x
//		f.requires.f = false;
//		f.requires.g = true;
//		f.requires.H = true;


//		f = objective(x);
//
//		state.f = f.f;
//		state.g = f.g;
//		state.H = f.H;
//
//		// Return true as found xtrial results in finite f(xtrial)
//		return true;
	};

	/// Return true if the calculation converged.
    auto converged() const -> bool
    {
        // Check if the calculation should stop based on optimality/feasibility errors
        return result.error < options.tolerance;
    };

    auto solve(const BasicParams& params, BasicState& state) -> Result
    {
        // Start timing the calculation
        Timer timer;

        // Finish the calculation if the problem has no variable
        if(n == 0)
        {
            result.succeeded = true;
            result.time = timer.elapsed();
            return result;
        }

        const auto maxiters = options.max_iterations;

        // Auxiliary references to some result variables
        auto& iterations = result.iterations;
        auto& succeeded = result.succeeded = false;

        initialize(params, state);
        outputInitialState(state);

        for(iterations = 1; iterations <= maxiters && !succeeded; ++iterations)
        {
            applyNewtonStepping(params, state);
            outputCurrentState(state);

            if((succeeded = converged()))
                break;

            evaluateObjectiveFunction(params, state);
			computeNewtonStep(params, state);
			updateResultErrors();
        }

        // Output a final header
        outputter.outputHeader();

        // Finish timing the calculation
        result.time = timer.elapsed();

        return result;
    }

//     /// Calculate the sensitivity of the optimal solution with respect to parameters.
//     auto dxdp(VectorConstRef dgdp, VectorConstRef dbdp) -> Matrix
//     {
//     	// TODO Implement the calculation of sensitivity derivatives
// //        // Initialize the right-hand side of the KKT equations
// //        rhs.rx.noalias() = -dgdp;
// //        rhs.ry.noalias() =  dbdp;
// //        rhs.rz.fill(0.0);
// //
// //        // Solve the KKT equations to get the derivatives
// //        kkt.solve(rhs, sol);

//         assert(false);

//         // Return the calculated sensitivity vector
//         return {};
//     }
};

BasicSolver::BasicSolver()
: pimpl(new Impl())
{}

BasicSolver::BasicSolver(const BasicProblem& problem)
: pimpl(new Impl(problem))
{}

BasicSolver::BasicSolver(const BasicSolver& other)
: pimpl(new Impl(*other.pimpl))
{}

BasicSolver::~BasicSolver()
{}

auto BasicSolver::operator=(BasicSolver other) -> BasicSolver&
{
    pimpl = std::move(other.pimpl);
    return *this;
}

auto BasicSolver::setOptions(const Options& options) -> void
{
	pimpl->setOptions(options);
}

auto BasicSolver::solve(const BasicParams& params, BasicState& state) -> Result
{
    return pimpl->solve(params, state);
}

} // namespace Optima
