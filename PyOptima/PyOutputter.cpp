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

#include <../PyOptima/PyOptima/Common/PyOutputter.hpp"

#include <boost/python.hpp>
namespace py = boost::python;

// Optima includes
#include <Optima/Common/Outputter.hpp>

namespace Optima {

auto export_Outputter() -> void
{
    py::class_<OutputterOptions>("OutputterOptions")
        .def_readwrite("active", &OutputterOptions::active)
        .def_readwrite("fixed", &OutputterOptions::fixed)
        .def_readwrite("scientific", &OutputterOptions::scientific)
        .def_readwrite("precision", &OutputterOptions::precision)
        .def_readwrite("width", &OutputterOptions::width)
        .def_readwrite("separator", &OutputterOptions::separator)
        ;
}

} // namespace Optima

