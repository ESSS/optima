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
# but WITHOUT ANY WARRANTY without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

from optima import *
from numpy import *
from numpy.linalg import norm
from pytest import approx, mark
from itertools import product

from utils.matrices import testing_matrices_A, matrix_non_singular


def print_state(M, r, s, m, n):
    slu = eigen.solve(M, r)
    print( 'M        = \n', M )
    print( 'r        = ', r )
    print( 'x        = ', s[:n] )
    print( 'x(lu)    = ', slu[:n] )
    print( 'x(diff)  = ', abs(s[:n] - slu[:n]) )
    print( 'y        = ', s[n:n + m] )
    print( 'y(lu)    = ', slu[n:n + m] )
    print( 'y(diff)  = ', abs(s[n:n + m] - slu[n:n + m]) )
    print( 'res      = ', M.dot(s) - r )
    print( 'res(lu)  = ', M.dot(slu) - r )


# The number of variables
n = 20

# Tested cases for the matrix A
tested_matrices_A = testing_matrices_A

# Tested cases for the structure of matrix H
tested_structures_H = [
    'denseH',
    'diagonalH'
]

# Tested cases for the structure of matrix D
tested_structures_D = [
    'diagonalD',
    'zeroD'
]

# Tested cases for the structure of matrix G
tested_structures_G = [
    # 'denseG', # TODO: currently, dense G tests produces more residual error than the other cases (I think it is because of R*G*tr(R) terms, Allan, 21.01.20).
    'zeroG'
]

# Tested cases for the indices of fixed variables
tested_jf = [
    arange(0),
    arange(1),
    array([1, 3, 7, 9])
]

# Tested number of rows in matrix Au (upper block of A)
tested_mu = [6, 4]
tested_ml = [3, 1, 0]

# Tested cases for the conditions of the variables in terms of pivot variables
tested_variable_conditions = [
    'all-variables-pivot',
    'all-variables-nonpivot',
    'some-variables-pivot'
]

# Tested cases for the saddle point methods
tested_methods = [
    SaddlePointMethod.Fullspace,
    SaddlePointMethod.Nullspace,
    SaddlePointMethod.Rangespace
]

# Combination of all tested cases
testdata = product(tested_matrices_A,
                   tested_structures_H,
                   tested_structures_D,
                   tested_structures_G,
                   tested_jf,
                   tested_mu,
                   tested_ml,
                   tested_variable_conditions,
                   tested_methods)

@mark.parametrize("args", testdata)
def test_saddle_point_solver(args):

    assemble_A, structure_H, structure_D, structure_G, jf, mu, ml, variable_condition, method = args

    m = mu + ml

    t = m + n

    nf = len(jf)

    expected = linspace(1, t, t)

    A = assemble_A(m, n, jf)

    Au = A[:mu, :]  # extract the upper block of A
    Al = A[mu:, :]  # extract the lower block of A

    H = matrix_non_singular(n)
    G = matrix_non_singular(m) if structure_G == 'denseG' else eigen.matrix()
    D = eigen.ones(n) if structure_D == 'diagonalD' else eigen.vector()

    if method == SaddlePointMethod.Rangespace:
        H = abs(eigen.diag(linspace(1, n, num=n)))

    # The diagonal entries of the Hessian matrix
    Hdiag = H[diag_indices(n)]

    # The sequence along the diagonal that is affected to control the number of pivot variables
    seq = slice(m) if variable_condition == 'some-variables-pivot' else slice(n)

    # The factor multiplied by the entries in the diagonal of the Hessian matrix
    factor = 1e-6 if variable_condition == 'all-variables-nonpivot' else 1e6

    # Adjust the diagonal entries to control number of pivot variables
    Hdiag[seq] = factor * Hdiag[seq]

    # Create the SaddlePointMatrix object
    lhs = SaddlePointMatrix(H, D, Au, Al, G, jf)

    # Use the SaddlePointMatrix object to create an array M
    M = lhs.array()

    # Compute the right-hand side vector r = M * expected
    r = M @ expected

    # The solution vector
    s = zeros(t)

    # The right-hand side and solution saddle point vectors
    rhs = SaddlePointVector(r, n, m)
    sol = SaddlePointSolution(s, n, m)

    # Specify the saddle point method for the current test
    options = SaddlePointOptions()
    options.method = method

    # Create a SaddlePointSolver to solve the saddle point problem
    solver = SaddlePointSolver()
    solver.setOptions(options)
    solver.decompose(lhs)
    solver.solve(rhs, sol)

    # Check the residual of the equation M * s = r
    succeeded = norm(M @ s - r) / norm(r) == approx(0.0)

    if not succeeded:
        set_printoptions(linewidth=1000)
        print()
        print(f"assemble_A = {assemble_A}")
        print(f"structure_H = {structure_H}")
        print(f"structure_D = {structure_D}")
        print(f"structure_G = {structure_G}")
        print(f"jf = {jf}")
        print(f"mu = {mu}")
        print(f"ml = {ml}")
        print(f"variable_condition = {variable_condition}")
        print(f"method = {method}")
        print()
        print_state(M, r, s, m, n)

    assert norm(M @ s - r) / norm(r) == approx(0.0)
