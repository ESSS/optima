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

#include "Hessian.hpp"

// Optima includes
#include <Optima/Common/Exception.hpp>

namespace Optima {

auto operator*(const Hessian& H, const Vector& x) -> Vector
{
    if(H.mode == Hessian::Dense)
        return H.dense * x;
    if(H.mode == Hessian::Diagonal)
        return H.diagonal % x;
    RuntimeError("Could not multiply a Hessian matrix with a vector.",
        "The Hessian matrix must be in either Dense or Diagonal mode.");
}

} // namespace Optima
