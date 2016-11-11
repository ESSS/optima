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

// Optima includes
#include <Optima/Common/Index.hpp>
#include <Optima/Math/Matrix.hpp>

namespace Optima {

/// The class that computes the full pivoting Auxiliary struct for storing the LU decomposition of a matrix `A`.
struct LU
{
    /// Construct a default LU instance.
    LU();

    /// Construct a LU instance with given matrix.
    explicit LU(const Matrix& A);

    /// Construct a LU instance with given matrix and scaling column-weights.
    LU(const Matrix& A, const Vector& W);

    /// Return true if empty.
    auto empty() const -> bool;

    /// Compute the LU decomposition of the given matrix.
    auto compute(const Matrix& A) -> void;

    /// Compute the LU decomposition of the given matrix with scaling column-weights.
    auto compute(const Matrix& A, const Vector& W) -> void;

    /// Solve the linear system `AX = B` using the calculated LU decomposition.
    auto solve(const Matrix& b) -> Matrix;

    /// Solve the linear system `tr(A)X = B` using the calculated LU decomposition.
    auto trsolve(const Matrix& B) -> Matrix;

    /// The last decomposed matrix A
    Matrix A_last;

    /// The last weights used for column scaling
    Vector W_last;

    /// The lower triangular matrix `L` in the LU decomposition of the matrix `PAQ = LU`.
    Matrix L;

    /// The upper triangular matrix `U` in the LU decomposition of the matrix `PAQ = LU`.
    Matrix U;

    /// The permutation matrix `P` in the LU decomposition of the matrix `PAQ = LU`.
    PermutationMatrix P;

    /// The permutation matrix `Q` in the LU decomposition of the matrix `PAQ = LU`.
    PermutationMatrix Q;

    /// The rank of the matrix `A`
    Index rank;
};

} // namespace Optima
