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
from pytest import approx, mark


def test_optimum_structure():
    n = 10
    m = 5

    structure = OptimumStructure(n, m)
    structure.A = eigen.random(m, n)
     
    structure.setVariablesWithLowerBounds([0, 2])
    assert set(structure.variablesWithLowerBounds()) == set([0, 2])
    assert set(structure.variablesWithoutLowerBounds()) == set([1, 3, 4, 5, 6, 7, 8, 9])

    structure.setVariablesWithUpperBounds([2, 4])
    assert set(structure.variablesWithUpperBounds()) == set([2, 4])
    assert set(structure.variablesWithoutUpperBounds()) == set([0, 1, 3, 5, 6, 7, 8, 9])

    structure.setVariablesWithFixedValues([6, 8, 9])
    assert set(structure.variablesWithFixedValues()) == set([6, 8, 9])
    assert set(structure.variablesWithoutFixedValues()) == set([0, 1, 2, 3, 4, 5, 7])

    structure.allVariablesHaveLowerBounds()
    assert structure.variablesWithLowerBounds() == approx(arange(n))

    structure.allVariablesHaveUpperBounds()
    assert structure.variablesWithUpperBounds() == approx(arange(n))

    assert structure.numVariables() == n
    assert structure.numEqualityConstraints() == m
