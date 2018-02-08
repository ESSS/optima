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

#pragma once

// C++ includes
#include <chrono>

namespace Optima {

using Time = std::chrono::time_point<std::chrono::high_resolution_clock>;

using Duration = std::chrono::duration<double>;

/// Return the time point now
/// @see elapsed
auto time() -> Time;

/// Return the elapsed time between two time points (in units of s)
/// @param end The end time point
/// @param end The begin time point
/// @return The elapsed time between *end* and *begin* in seconds
auto elapsed(const Time& end, const Time& begin) -> double;

/// Return the elapsed time between a time point and now (in units of s)
/// @param end The begin time point
/// @return The elapsed time between now and *begin* in seconds
auto elapsed(const Time& begin) -> double;

} // namespace Optima
