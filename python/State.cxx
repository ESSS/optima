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
#include <Optima/State.hpp>
using namespace Optima;

void exportState(py::module& m)
{
    py::class_<State>(m, "State")
        .def(py::init<const Dims&>())
        .def_readonly("dims", &State::dims)
        .def_readwrite("x", &State::x)
        .def_readwrite("p", &State::p)
        .def_readwrite("y", &State::y)
        .def_readwrite("ybe", &State::ybe)
        .def_readwrite("ybg", &State::ybg)
        .def_readwrite("yhe", &State::yhe)
        .def_readwrite("yhg", &State::yhg)
        .def_readwrite("z", &State::z)
        .def_readwrite("dxdp", &State::dxdp)
        .def_readwrite("dydp", &State::dydp)
        .def_readwrite("dzdp", &State::dzdp)
        .def_readwrite("xbar", &State::xbar)
        .def_readwrite("zbar", &State::zbar)
        .def_readwrite("xbg", &State::xbg)
        .def_readwrite("xhg", &State::xhg)
        .def_readwrite("stability", &State::stability)
        ;
}
