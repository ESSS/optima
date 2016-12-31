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

#include "HessianMatrix.hpp"

namespace Optima {

HessianMatrix::HessianMatrix()
: m_dim(0), m_mode(Zero)
{}

HessianMatrix::~HessianMatrix()
{}

auto HessianMatrix::zero(Index dim) -> void
{
    m_dim = dim;
    m_mode = Zero;
}

auto HessianMatrix::diagonal(Index dim) -> Vector&
{
    m_dim = dim;
    m_mode = Diagonal;
    m_diagonal.resize(dim);
    return m_diagonal;
}

auto HessianMatrix::diagonal() const -> const Vector&
{
    return m_diagonal;
}

auto HessianMatrix::dense(Index dim) -> Matrix&
{
    m_dim = dim;
    m_mode = Dense;
    m_dense.resize(dim, dim);
    return m_dense;
}

auto HessianMatrix::dense() const -> const Matrix&
{
    return m_dense;
}

auto HessianMatrix::mode() const -> Mode
{
    return m_mode;
}

auto HessianMatrix::dim() const -> Index
{
    return m_dim;
}

auto HessianMatrix::convert() const -> Matrix
{
    switch(m_mode)
    {
    case Diagonal: return diag(m_diagonal);
    case Dense: return m_dense;
    default: return zeros(m_dim, m_dim);
    }
}

} // namespace Optima