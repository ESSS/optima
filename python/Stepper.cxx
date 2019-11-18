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

// pybind11 includes
#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
namespace py = pybind11;

// Optima includes
#include <Optima/IpSaddlePointMatrix.hpp>
#include <Optima/Options.hpp>
#include <Optima/Params.hpp>
#include <Optima/State.hpp>
#include <Optima/Stepper.hpp>
#include <Optima/Structure.hpp>
#include <Optima/Result.hpp>
using namespace Optima;

void exportStepper(py::module& m)
{
    py::class_<Stepper>(m, "Stepper")
        .def(py::init<const Structure&>())
        .def("setOptions", &Stepper::setOptions)
        .def("decompose", &Stepper::decompose)
        .def("solve", &Stepper::solve)
        .def("step", &Stepper::step)
        .def("residual", &Stepper::residual)
        .def("matrix", &Stepper::matrix)
        ;
}