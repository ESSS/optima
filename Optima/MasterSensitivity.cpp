// Optima is a C++ library for solving linear and non-linear constrained optimization problems
//
// Copyright © 2020-2023 Allan Leal
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

#include "MasterSensitivity.hpp"

namespace Optima {

MasterSensitivity::MasterSensitivity()
{}

MasterSensitivity::MasterSensitivity(const MasterDims& dims, Index nc)
: xc(zeros(dims.nx, nc)),
  pc(zeros(dims.np, nc)),
  wc(zeros(dims.nw, nc)),
  sc(zeros(dims.nx, nc))
{}

auto MasterSensitivity::resize(const MasterDims& dims, Index nc) -> void
{
    xc.resize(dims.nx, nc);
    pc.resize(dims.np, nc);
    wc.resize(dims.nw, nc);
    sc.resize(dims.nx, nc);
}

} // namespace Optima
