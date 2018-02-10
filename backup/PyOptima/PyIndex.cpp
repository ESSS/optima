// Optima is a C++ library for numerical solution of linear and nonlinear programing problems.
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

#include <boost/python.hpp>

#include <../PyOptima/PyOptima/Common/PyMatrix.hpp"
namespace py = boost::python;

// Optima includes
#include <Optima/Common/Index.hpp>

namespace Optima {

auto export_Index() -> void
{
    // Export the typedef Index = std::size_t
    py::scope().attr("Index") = py::scope().attr("unsigned long");

    // Export the typedef Indices = std::vector<std::size_t>
    py::scope().attr("Indices") = py::scope().attr("SizetVector");
}

} // namespace Optima
