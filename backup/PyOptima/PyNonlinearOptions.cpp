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

#include <boost/python.hpp>

#include <../PyOptima/PyOptima/Optimization/PyOptimumOptions.hpp"
namespace py = boost::python;

// Optima includes
#include <Optima/Core/NonlinearSolver.hpp>

namespace Optima {

auto export_NonlinearOptions() -> void
{
    py::class_<NonlinearOutput, py::bases<OutputterOptions>>("NonlinearOutput")
        .def_readwrite("xprefix", &NonlinearOutput::xprefix)
        .def_readwrite("fprefix", &NonlinearOutput::fprefix)
        .def_readwrite("xnames", &NonlinearOutput::xnames)
        .def_readwrite("fnames", &NonlinearOutput::fnames)
        ;

    py::class_<NonlinearOptions>("NonlinearOptions")
        .def_readwrite("tolerance", &NonlinearOptions::tolerance)
        .def_readwrite("tolerancex", &NonlinearOptions::tolerancex)
        .def_readwrite("max_iterations", &NonlinearOptions::max_iterations)
        .def_readwrite("tau", &NonlinearOptions::tau)
        .def_readwrite("armijo", &NonlinearOptions::armijo)
        .def_readwrite("output", &NonlinearOptions::output)
        ;
}

} // namespace Optima
