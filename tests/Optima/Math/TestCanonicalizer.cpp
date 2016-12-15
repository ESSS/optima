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

#include <doctest/doctest.hpp>

// Optima includes
#include <Optima/Optima.hpp>
using namespace Optima;

#define CHECK_CANONICAL_FORM                         \
{                                                    \
    const auto& R    = canonicalizer.R();            \
    const auto& Rinv = canonicalizer.Rinv();         \
    const auto& Q    = canonicalizer.Q();            \
    const auto& C    = canonicalizer.C();            \
    CHECK((R * Rinv).isApprox(identity(r, r)));      \
    CHECK((R * A * Q - C).norm() == approx(0.0));    \
}                                                    \

TEST_CASE("Testing Canonicalizer")
{
	const Index m = 4;
	const Index n = 10;

	Matrix A = random(m, n);

	Canonicalizer canonicalizer(A);
	const Index r = canonicalizer.rows();

	CHECK_CANONICAL_FORM

	for(Index i = 0; i < r; ++i)
	{
		for(Index j = 0; j < n - r; ++j)
		{
			canonicalizer.swap(i, j);
			CHECK_CANONICAL_FORM
		}
	}
}

TEST_CASE("Testing Canonicalizer with two linearly dependent rows")
{
	const Index m = 4;
	const Index n = 10;

	Matrix A = random(m, n);
	A.row(2) = A.row(0) + 2*A.row(1);
	A.row(3) = A.row(1) - 2*A.row(2);

	Canonicalizer canonicalizer(A);
    const Index r = canonicalizer.rows();

	CHECK_CANONICAL_FORM

	for(Index i = 0; i < r; ++i)
	{
		for(Index j = 0; j < n - r; ++j)
		{
			canonicalizer.swap(i, j);
			CHECK_CANONICAL_FORM
		}
	}
}

TEST_CASE("Testing the update method of the Canonicalizer class")
{
	const Matrix A = {
		{2,  1,  1,  1,  0,  0},
		{1,  0,  1,  3,  2,  3},
		{0,  0,  0,  1,  1,  1},
		{0,  1, -1, -1,  0, -2}
	};

	Canonicalizer canonicalizer(A);
    const Index r = canonicalizer.rows();

	CHECK(r == 3);
	CHECK_CANONICAL_FORM

	Vector w = {55.1, 1.e-4, 1.e-10, 0.1, 0.5, 1e-2};

	canonicalizer.update(w);

	CHECK_CANONICAL_FORM

	Eigen::VectorXi expectedQ = {0, 4, 3, 5, 1, 2};
	Eigen::VectorXi actualQ = canonicalizer.Q().indices();

	CHECK(expectedQ.isApprox(actualQ));

	w = {55.1, 1.e-4, 1.e-10, 0.3, 0.1, 0.8};

	canonicalizer.update(w);

	CHECK_CANONICAL_FORM

	expectedQ = {0, 5, 3, 4, 1, 2};
	actualQ = canonicalizer.Q().indices();

	CHECK(expectedQ.isApprox(actualQ));

//	std::cout << "C = \n" << C << std::endl;
//	std::cout << "Q = \n" << Q.indices() << std::endl;
//
//	std::vector<std::string> species = {
//		"H2O", "H+", "OH-", "HCO3-", "CO2", "CO3--"
//	};
//
//	const auto& S = canonicalizer.S();
//	std::cout << "C = \n" << C << std::endl;
//	std::cout << "R * A * Q = \n" << R * A * Q << std::endl;
//	std::cout << "Q = \n" << Q.indices() << std::endl;
//
//	for(Index i = 0; i < S.cols(); ++i)
//	{
//		std::cout << species[Q.indices()[i + S.rows()]] << " = ";
//		for(Index j = 0; j < S.rows(); ++j)
//			if(S(j, i))
//				std::cout  << " " << S(j, i) << "*" << species[Q.indices()[j]];
//		std::cout << std::endl;
//	}
//
//	for(Index i = 0; i < Q.cols(); ++i)
//		std::cout << species[Q.indices()[i]] << std::endl;
}
