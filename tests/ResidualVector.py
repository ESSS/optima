# Optima is a C++ library for numerical solution of linear and nonlinear programing problems.
#
# Copyright (C) 2014-2018 Allan Leal
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

from optima import *
from numpy import *
from numpy.testing import assert_almost_equal
from pytest import approx, mark

tested_nx  = [5, 10, 20, 50]  # The tested number of x variables
tested_np  = [0, 5, 10]       # The tested number of p variables
tested_ny  = [2, 5, 10]       # The tested number of y variables
tested_nz  = [0, 2, 5]        # The tested number of z variables
tested_nl  = [0, 1, 2]        # The tested number of linearly dependent rows in Ax

# The tested indices of unstable basic variables in x
tested_junit = [
    [],
    [1]
]

# The tested indices of unstable variables in x
tested_ju = [
    [],
    [1],
    [1, 2, 3]
]

@mark.parametrize("nx",  tested_nx)
@mark.parametrize("np",  tested_np)
@mark.parametrize("ny",  tested_ny)
@mark.parametrize("nz",  tested_nz)
@mark.parametrize("nl",  tested_nl)
@mark.parametrize("junit", tested_junit)
@mark.parametrize("ju",  tested_ju)
def test_residual_vector(nx, np, ny, nz, nl, junit, ju, createMasterMatrix):

    # Ensure nx is larger than np and (ny + nz)
    if nx < np or nx < ny + nz: return

    # Ensure nl < ny
    if ny <= nl: return

    M = createMasterMatrix(nx, np, ny, nz, nl, junit, ju)

    F = ResidualVector(nx, np, ny, nz)

    x = random.rand(nx)
    p = random.rand(np)
    y = random.rand(ny)
    z = random.rand(nz)
    g = random.rand(nx)
    v = random.rand(np)
    b = random.rand(ny)
    h = random.rand(nz)

    F.update(M, x, p, y, z, g, v, b, h)

    Mbar = M.canonicalForm()

    dims = Mbar.dims
    nbs = dims.nbs

    js, ju  = Mbar.js, Mbar.ju

    R = Mbar.R

    As, Au, Ap, Js = Mbar.As, Mbar.Au, Mbar.Ap, Mbar.Js

    ax = zeros(nx)
    ax[js] = -(g[js] + As.T @ y + Js.T @ z)

    ap = -v
    ay = -(As @ x[js] + Au @ x[ju] + Ap @ p - b)
    az = -h
    aw = concatenate([ay, az])
    awbar = R @ aw

    a = F.canonicalVector()

    assert_almost_equal(a.xs, ax[js])
    assert_almost_equal(a.p, ap)
    assert_almost_equal(a.wbs, awbar[:nbs])