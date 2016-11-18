// Optima is a C++ library for numerical solution of linear and nonlinear programing problems.
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

#pragma once

// C++ includes
#include <tuple>

// Optima includes
#include <Optima/Common/Index.hpp>
#include <Optima/Math/Matrix.hpp>

namespace Optima {

/// Determine the set of linearly independent columns in a matrix using a column pivoting QR algorithm.
/// @param A The matrix whose linearly independent columns must be found
/// @return The indices of the linearly independent columns
auto linearlyIndependentCols(const Matrix& A) -> Indices;

/// Determine the set of linearly independent rows in a matrix.
/// @param A The matrix whose linearly independent rows must be found
/// @return The indices of the linearly independent rows
auto linearlyIndependentRows(const Matrix& A) -> Indices;

/// Determine the set of linearly independent columns in a matrix.
/// @param[in] A The matrix whose linearly independent columns must be found
/// @param[out] B The matrix composed by linearly independent columns only
/// @return The indices of the linearly independent columns
auto linearlyIndependentCols(const Matrix& A, Matrix& B) -> Indices;

/// Determine the set of linearly independent rows in a matrix.
/// @param[in] A The matrix whose linearly independent rows must be found
/// @param[out] B The matrix composed by linearly independent rows only
/// @return The indices of the linearly independent rows
auto linearlyIndependentRows(const Matrix& A, Matrix& B) -> Indices;

/// Calculate the inverse of `A + D` where `inv(A)` is already known and `D` is a diagonal matrix.
/// @param invA[in,out] The inverse of the matrix `A` and the final inverse of `A + D`
/// @param D The diagonal matrix `D`
auto inverseShermanMorrison(const Matrix& invA, const Vector& D) -> Matrix;

/// Calculate the rational number given a floating-point number
/// @param x The floating point number
/// @param maxden The maximum number the denominator can assume
/// @param[out] num The calculated numerator
/// @param[out] den The calculated denominator
auto fraction(double x, long maxden, long& num, long& den) -> void;

/// Clean an array that is known to have rational numbers from round-off errors.
/// @param vals[in,out] The values to be cleaned
/// @param maxden The maximum known denominator in the array with rational numbers
auto cleanRationalNumbers(double* vals, long size, long maxden = 6) -> void;

/// Clean a matrix that is known to have rational numbers from round-off errors.
/// @param A[in,out] The matrix to be cleaned
/// @param maxden The maximum known denominator in the matrix with rational numbers
auto cleanRationalNumbers(Matrix& A, long maxden = 6) -> void;

/// Clean a vector that is known to have rational numbers from round-off errors.
/// @param x[in,out] The vector to be cleaned
/// @param maxden The maximum known denominator in the matrix with rational numbers
auto cleanRationalNumbers(Vector& x, long maxden = 6) -> void;

/// Return the dot product `s + dot(x, y)` of two vectors with triple-precision.
auto dot3p(const Vector& x, const Vector& y, double s) -> double;

/// Return the residual of the equation `A*x - b` with triple-precision.
auto residual3p(const Matrix& A, const Vector& x, const Vector& b) -> Vector;

/// Calculates the rational number that approximates a given real number.
/// The algorithm is based on Farey sequence as shown
/// [here](http://www.johndcook.com/blog/2010/10/20/best-rational-approximation/).
/// @param x The real number.
/// @param n The maximum denominator.
auto rationalize(double x, unsigned n) -> std::tuple<long, long>;

} // namespace Optima
