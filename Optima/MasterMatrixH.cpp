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

#include "MasterMatrixH.hpp"

// Optima includes
#include <Optima/Exception.hpp>

namespace Optima {

MasterMatrixH::MasterMatrixH(Index nx, Index np)
: MasterMatrixH(zeros(nx, nx), zeros(nx, np))
{}

MasterMatrixH::MasterMatrixH(MatrixConstRef Hxx, MatrixConstRef Hxp)
: _Hxx(Hxx), _Hxp(Hxp), isdiag(false), Hxx(_Hxx), Hxp(_Hxp)
{}

MasterMatrixH::MasterMatrixH(const MasterMatrixH& other)
: _Hxx(other._Hxx), _Hxp(other._Hxp), isdiag(other.isdiag), Hxx(_Hxx), Hxp(_Hxp)
{}

auto MasterMatrixH::isHxxDiagonal() const -> bool
{
    return isdiag;
}

auto MasterMatrixH::isHxxDiagonal(bool enable) -> bool
{
    isdiag = enable;
}

} // namespace Optima