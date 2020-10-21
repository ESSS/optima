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

#pragma once

// C++ includes
#include <memory>

// Optima includes
#include <Optima/Index.hpp>
#include <Optima/Matrix.hpp>

namespace Optima {

// Forward declarations
class JacobianMatrix;

/// The arguments in method @ref ResidualVector::update.
struct ResidualVectorUpdateArgs
{
    JacobianMatrix const& M;
    VectorConstRef x;
    VectorConstRef p;
    VectorConstRef y;
    VectorConstRef z;
    VectorConstRef g;
    VectorConstRef v;
    VectorConstRef b;
    VectorConstRef h;
};

/// Used to represent the residual vector in the optimization problem.
class ResidualVector
{
private:
    struct Impl;

    std::unique_ptr<Impl> pimpl;

public:
    /// Construct a ResidualVector instance.
    ResidualVector(Index nx, Index np, Index ny, Index nz);

    /// Construct a copy of a ResidualVector instance.
    ResidualVector(const ResidualVector& other);

    /// Destroy this ResidualVector instance.
    virtual ~ResidualVector();

    /// Assign a ResidualVector instance to this.
    auto operator=(ResidualVector other) -> ResidualVector&;

    /// Update the residual vector.
    auto update(ResidualVectorUpdateArgs args) -> void;

    /// The vector components in the canonical form of the residual vector.
    struct CanonicalForm
    {
        VectorConstRef as;   ///< The vector as in the canonical residual vector.
        VectorConstRef ap;   ///< The vector ap in the canonical residual vector.
        VectorConstRef awbs; ///< The vector awbs in the canonical residual vector.
    };

    /// Return a view to the components of the canonical form of the residual vector.
    auto canonicalForm() const -> CanonicalForm;
};

} // namespace Optima