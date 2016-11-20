// Optima is a C++ library for numerical sol of linear and nonlinear programing problems.
//
// Copyright (C) 2014-2016 Allan Leal
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

#include <doctest/doctest.hpp>

// Optima includes
#include <Optima/Optima.hpp>
using namespace Optima;

namespace aux {

auto saddlePointProblemCanonical(Index nb, Index ns, Index nu, Index p) -> SaddlePointProblemCanonical
{
	Index n = nb + ns + nu;
	Index m = nb;

	SaddlePointProblemCanonical problem;
    problem.lhs.Gb = Vector::Random(nb);
    problem.lhs.Gs = Vector::Random(ns);
    problem.lhs.Gu = Vector::Random(nu);
    problem.lhs.Bb = Vector::Random(nb);
    problem.lhs.Bs = Matrix::Random(nb, ns);
    problem.lhs.Bu = Matrix::Random(nb, nu);
    problem.lhs.Eb = Vector::Random(p ? nb : 0);
    problem.lhs.Es = Vector::Random(p ? ns : 0);
    problem.lhs.Eu = Vector::Random(p ? nu : 0);

    Vector rhs = problem.lhs * ones(problem.lhs.rows());

    problem.rhs.ab = rhs.topRows(n).topRows(nb);
    problem.rhs.as = rhs.topRows(n).middleRows(nb, ns);
    problem.rhs.au = rhs.topRows(n).bottomRows(nu);
    problem.rhs.b  = rhs.middleRows(n, m);
    problem.rhs.cb = rhs.bottomRows(n).topRows(nb);
    problem.rhs.cs = rhs.bottomRows(n).middleRows(nb, ns);
    problem.rhs.cu = rhs.bottomRows(n).bottomRows(nu);

    REQUIRE(problem.lhs.valid());
	REQUIRE(problem.rhs.valid());

	return problem;
}

} // namespace aux

TEST_CASE("Testing SaddlePointSolver - Case 1")
{
    SaddlePointProblemCanonical problem;
    SaddlePointVectorCanonical sol;

    problem.lhs.Gb = {9, 8, 7};
    problem.lhs.Gs = {};
    problem.lhs.Gu = {};
    problem.lhs.Bb = {1, 1, 1};
    problem.lhs.Bs = {};
    problem.lhs.Bu = {};
    problem.lhs.Eb = {1, 1, 1};
    problem.lhs.Es = {};
    problem.lhs.Eu = {};

    problem.rhs.ab = {11, 10, 9};
    problem.rhs.as = {};
    problem.rhs.au = {};
    problem.rhs.b  = {1, 1, 1};
    problem.rhs.cb = {2, 2, 2};
    problem.rhs.cs = {};
    problem.rhs.cu = {};

    SaddlePointSolver solver;
    solver.solve(problem, sol);

    CHECK(sol.isApprox(ones(sol.rows())));
}

TEST_CASE("Testing SaddlePointSolver - Case 2")
{
    Index np = 10;
    Index ns = 35;
    Index nu = 5;
    Index p  = 1;

    SaddlePointVectorCanonical sol;
    SaddlePointProblemCanonical problem =
		aux::saddlePointProblemCanonical(np, ns, nu, p);

    SaddlePointSolver solver;
    solver.solve(problem, sol);

    CHECK(sol.isApprox(ones(sol.rows())));
}

TEST_CASE("Testing SaddlePointSolver - Case 3")
{
    SaddlePointVectorCanonical sol;
    SaddlePointProblemCanonical problem;

    Index np = 10;
    Index ns = 35;
    Index nu = 0;
    Index p  = 0;

    problem = aux::saddlePointProblemCanonical(np, ns, nu, p);

    SaddlePointSolver solver;
    solver.solve(problem, sol);

    CHECK(sol.isApprox(ones(sol.rows())));
}

TEST_CASE("Testing SaddlePointSolver - Case 4")
{
    SaddlePointVectorCanonical sol;
    SaddlePointProblemCanonical problem;

    Index np = 10;
    Index ns = 0;
    Index nu = 0;
    Index p  = 0;

    problem = aux::saddlePointProblemCanonical(np, ns, nu, p);

    SaddlePointSolver solver;
    solver.solve(problem, sol);

    CHECK(sol.isApprox(ones(sol.rows())));
}

TEST_CASE("Testing SaddlePointSolver - Case 4")
{
    SaddlePointVectorCanonical sol;
    SaddlePointProblemCanonical problem;

    Index np = 10;
    Index ns = 0;
    Index nu = 0;
    Index p  = 1;

    problem = aux::saddlePointProblemCanonical(np, ns, nu, p);

    SaddlePointSolver solver;
    solver.solve(problem, sol);

    CHECK(sol.isApprox(ones(sol.rows())));
}

TEST_CASE("Testing SaddlePointSolver - Case 5")
{
    SaddlePointProblemCanonical problem;
    SaddlePointVectorCanonical sol;

    problem.lhs.Gb = {5};
    problem.lhs.Gs = {5};
    problem.lhs.Gu = {};
    problem.lhs.Bb = {2};
    problem.lhs.Bs = {2};
    problem.lhs.Bu = {};
    problem.lhs.Eb = {1};
    problem.lhs.Es = {1};
    problem.lhs.Eu = {};

    problem.rhs.ab = {8};
    problem.rhs.as = {8};
    problem.rhs.au = {};
    problem.rhs.b  = {4};
    problem.rhs.cb = {2};
    problem.rhs.cs = {2};
    problem.rhs.cu = {};

    SaddlePointSolver solver;
    solver.solve(problem, sol);

    CHECK(sol.isApprox(ones(sol.rows())));
}

TEST_CASE("Testing SaddlePointSolver - Case 6")
{
    SaddlePointProblemCanonical problem;
    SaddlePointVectorCanonical sol;

    problem.lhs.Gb = {5};
    problem.lhs.Gs = {};
    problem.lhs.Gu = {1};
    problem.lhs.Bb = {2};
    problem.lhs.Bs = {};
    problem.lhs.Bu = {2};
    problem.lhs.Eb = {1};
    problem.lhs.Es = {};
    problem.lhs.Eu = {6};

    problem.rhs.ab = {8};
    problem.rhs.as = {};
    problem.rhs.au = {9};
    problem.rhs.b  = {4};
    problem.rhs.cb = {2};
    problem.rhs.cs = {};
    problem.rhs.cu = {12};

    SaddlePointSolver solver;
    solver.solve(problem, sol);

    CHECK(sol.isApprox(ones(sol.rows())));
}

TEST_CASE("Testing SaddlePointSolver - Case 7")
{
    SaddlePointProblemCanonical problem;
    SaddlePointVectorCanonical sol;

    problem.lhs.Gb = {1, 2, 3};
    problem.lhs.Gs = {4, 5};
    problem.lhs.Gu = {6};
    problem.lhs.Bb = {9, 8, 7};
    problem.lhs.Bs = {{1, 2}, {2, 3}, {3, 4}};
    problem.lhs.Bu = {5, 6, 7};
    problem.lhs.Eb = {1, 1, 1};
    problem.lhs.Es = {1, 1};
    problem.lhs.Eu = {1};

    problem.rhs.ab = {11, 11, 11};
    problem.rhs.as = {11, 15};
    problem.rhs.au = {25};
    problem.rhs.b  = {17, 19, 21};
    problem.rhs.cb = {2, 2, 2};
    problem.rhs.cs = {2, 2};
    problem.rhs.cu = {2};

    SaddlePointSolver solver;
    solver.solve(problem, sol);

    CHECK(sol.isApprox(ones(sol.rows())));
}