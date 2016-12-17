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

#include "SaddlePointSolver.hpp"

// Optima includes
#include <Optima/Common/Exception.hpp>
#include <Optima/Math/Canonicalizer.hpp>
#include <Optima/Math/Eigen/src/Cholesky/LDLT.h>

namespace Optima {

struct SaddlePointSolver::Impl
{
    /// The left-hand side coefficient matrix of the saddle point problem in canonical and scaled form.
    SaddlePointMatrixCanonical clhs;

    /// The right-hand side vector of the saddle point problem in canonical and scaled form.
    SaddlePointVector crhs;

    /// The canonicalizer of the Jacobian matrix `A`.
    Canonicalizer canonicalizer;

    /// The auxiliary data to calculate the scaling of the saddle point problem.
    Vector X, Z;

    /// The threshold parameter used to detect unstable variables.
    double epsilon = 1e-15;

    /// The coefficient matrix of the linear system used to compute `xb`
    Matrix lhs_xb;

    /// The right-hand side vector of the linear system used to compute `xb`
    Vector rhs_xb;

    /// The LDLT solver applied to `lhs_xb` to compute `xb`
    Eigen::LDLT<Matrix> ldlt;

    /// Canonicalize the coefficient matrix \eq{A} of the saddle point problem.
    auto canonicalize(const SaddlePointMatrix& lhs) -> void
    {
        // Compute the canonical form of matrix A
        canonicalizer.compute(lhs.A);
    }

    /// Update the scaled form of the left-hand side canonical saddle point matrix.
    auto scale(const SaddlePointMatrix& lhs) -> void
    {
        // Alias to members of the canonical saddle point matrix
        auto& G  = clhs.G;
        auto& E  = clhs.E;
        auto& Bb = clhs.Bb;
        auto& Bn = clhs.Bn;
        auto& nb = clhs.nb;
        auto& nn = clhs.nn;
        auto& ns = clhs.ns;
        auto& nu = clhs.nu;

        // Alias to permutation matrix Q of the canonical form `C = RAQ = [I S]`.
        const auto& Q = canonicalizer.Q();

        // Alias to matrix S of the canonical form `C = RAQ = [I S]`.
        const auto& S = canonicalizer.S();

        // Alias to members of the saddle point matrix
        const auto& H = lhs.H;

        // Update member vectors X and Z
        X.noalias() = lhs.X;
        Z.noalias() = lhs.Z;

        // Update the canonical form and the ordering of the variables with current X values
        canonicalizer.update(X);

        // The number of rows and columns of the canonical form of A
        const Index m = canonicalizer.rows();
        const Index n = canonicalizer.cols();

        // Set the number of basic and non-basic variables of the canonical saddle point problem
        nb = m;
        nn = n - nb;

        // Compute the scaled matrices G and E
        G.noalias() =  X % H % X;
        E.noalias() = -X % Z;

        // Permute the rows of X and Z according to the ordering of the permutation matrix Q
        Q.transpose().applyThisOnTheLeft(X);
        Q.transpose().applyThisOnTheLeft(Z);

        // Create views to the basic and non-basic parts of X, now ordered as X = [Xb Xn]
        auto Xb = X.head(nb);
        auto Xn = X.tail(nn);

        // Create a view to the non-basic part of Z, now ordered as Z = [Zb Zn]
        auto Zn = Z.tail(nn);

        // Permute the rows of G and E according to the ordering of the permutation matrix Q
        Q.transpose().applyThisOnTheLeft(G); // Q.transpose() * G * Q; // in the dense case!
        Q.transpose().applyThisOnTheLeft(E);

        // Assemble the B matrix of the canonical saddle point problem, where `B = CX = [Xb SXn]`
        Bb.noalias() = Xb;
        Bn.noalias() = S * Xn;

        // Iterate over all non-basic variables and stop when Xn[i] < Zn[i]
        for(ns = 0; ns < nn; ++ns)
            if(std::abs(Xn[ns]) < std::abs(Zn[ns]))
                break;

        // Set the number of non-basic unstable variables
        nu = nn - ns;

        // TODO Update basic, non-basic, stable and unstable matrix/vector views here
    }

    /// Update the scaled form of the right-hand side canonical saddle point vector.
    auto scale(const SaddlePointVector& rhs) -> void
    {
        // Alias to members of the saddle point vector.
        const auto& a = rhs.x;
        const auto& b = rhs.y;
        const auto& c = rhs.z;

        // Alias to permutation matrix Q of the canonical form `C = RAQ = [I S]`.
        const auto& Q = canonicalizer.Q();

        // Alias to canonicalizer matrix R of the canonical form `C = RAQ = [I S]`.
        const auto& R = canonicalizer.R();

        // Alias to members of the canonical saddle point vector.
        auto& r = crhs.x;
        auto& s = crhs.y;
        auto& t = crhs.z;

        // Calculate the right-hand side vector of the canonical saddle point problem
        r.noalias() =  a;
        s.noalias() =  R*b;
        t.noalias() = -c;

        // Permute the rows of r and t according to the ordering of the permutation matrix Q.
        Q.transpose().applyThisOnTheLeft(r);
        Q.transpose().applyThisOnTheLeft(t);

        // Finalize the computation of vector r as `r = Xa`, noting that X has ordering `X = [Xb Xn]`.
        r.noalias() = X % r;

        // TODO Update basic, non-basic, stable and unstable matrix/vector views here
    }

    /// Decompose the coefficient matrix of the saddle point problem.
    auto decompose(const SaddlePointMatrix& lhs) -> void
    {
        // Alias to members of the canonical saddle point matrix
        const auto& G  = clhs.G;
        const auto& E  = clhs.E;
        const auto& Bb = clhs.Bb;
        const auto& Bn = clhs.Bn;
        const auto& nb = clhs.nb;
        const auto& nn = clhs.nn;
        const auto& ns = clhs.ns;
        const auto& nu = clhs.nu;

        // Scale the given saddle point matrix to update its canonical form `clhs`
        scale(lhs);

        // Create views to the basic, non-basic, stable, and unstable blocks of matrices G, E, and B.
        auto Gb =  G.head(nb);
        auto Gn =  G.tail(nn);
        auto Gs = Gn.head(ns);
        auto Gu = Gn.tail(nu);
        auto Eb =  E.head(nb);
        auto En =  E.tail(nn);
        auto Es = En.head(ns);
        auto Eu = En.tail(nu);
        auto Bs = Bn.leftCols(ns);
        auto Bu = Bn.rightCols(nu);

        // Define auxiliary light-weight matrix expressions
        auto GbEb = Gb - Eb;
        auto GsEs = Gs - Es;
        auto GuEu = Gu - Eu;
        auto BbBs = diag(inv(Bb)) * Bs;
        auto BbBu = diag(inv(Bb)) * Bu;

        // Assemble the left-hand side matrix of the linear system to compute `xb`
        lhs_xb = diag(inv(GbEb));
        lhs_xb += BbBs*diag(inv(GsEs))*tr(BbBs);
        lhs_xb += BbBu*diag(inv(GuEu))*tr(BbBu);

        // Compute the LDLT decomposition of `lhs_xb`.
        ldlt.compute(lhs_xb);
    }

    auto solve(const SaddlePointVector& rhs, SaddlePointVector& sol) -> void
    {
        // Alias to members of the canonical saddle point matrix
        const auto& G  = clhs.G;
        const auto& E  = clhs.E;
        const auto& Bb = clhs.Bb;
        const auto& Bn = clhs.Bn;
        const auto& nb = clhs.nb;
        const auto& nn = clhs.nn;
        const auto& ns = clhs.ns;
        const auto& nu = clhs.nu;

        // Alias to members of the canonical saddle point vector.
        const auto& r = crhs.x;
        const auto& s = crhs.y;
        const auto& t = crhs.z;

        // Scale the given saddle point vector to update its canonical form `crhs`
        scale(rhs);

        // Create views to the basic, non-basic, stable, and unstable blocks of matrices G, E, X, and B.
        auto Gb =  G.head(nb);
        auto Gn =  G.tail(nn);
        auto Gs = Gn.head(ns);
        auto Gu = Gn.tail(nu);
        auto Eb =  E.head(nb);
        auto En =  E.tail(nn);
        auto Es = En.head(ns);
        auto Eu = En.tail(nu);
        auto Bs = Bn.leftCols(ns);
        auto Bu = Bn.rightCols(nu);

        // Create views to the basic, non-basic, stable, and unstable blocks of vectors r and t.
        auto rb =  r.head(nb);
        auto rn =  r.tail(nn);
        auto rs = rn.head(ns);
        auto ru = rn.tail(nu);
        auto tb =  t.head(nb);
        auto tn =  t.tail(nn);
        auto ts = tn.head(ns);
        auto tu = tn.tail(nu);

        // Alias to members of the saddle point solution vector
        auto& x = sol.x;
        auto& y = sol.y;
        auto& z = sol.z;

        // The number of rows and columns of the canonical form of A
        const Index m = canonicalizer.rows();
        const Index n = canonicalizer.cols();

        // Resize the saddle point solution vector
        x.resize(n);
        y.resize(m);
        z.resize(n);

        // Create views to the basic, non-basic, stable, and unstable blocks of vectors x, y, z.
        auto xb =  x.head(nb);
        auto xn =  x.tail(nn);
        auto xs = xn.head(ns);
        auto xu = xn.tail(nu);
        auto zb =  z.head(nb);
        auto zn =  z.tail(nn);
        auto zs = zn.head(ns);
        auto zu = zn.tail(nu);

        // Define auxiliary light-weight matrix expressions
        auto rbp  = rb - tb;
        auto rsp  = rs - ts;
        auto rup  = ru - Gu % (tu/Eu);
        auto tbp  = tb/Eb;
        auto tsp  = ts/Es;
        auto tup  = tu/Eu;
        auto sp   = s - Bu * tup;
        auto Bsp  = diag(inv(Bb)) * Bs;
        auto Bup  = diag(inv(Bb)) * Bu;
        auto spp  = sp/Bb;
        auto GbEb = Gb - Eb;
        auto GsEs = Gs - Es;
        auto GuEu = Gu - Eu;

        // Assemble the right-hand side vector of the linear system to compute `xb`
        rhs_xb = spp;
        rhs_xb.noalias() += Bsp*diag(inv(GsEs))*tr(Bsp)*rbp;
        rhs_xb.noalias() += Bup*diag(inv(GuEu))*tr(Bup)*rbp;
        rhs_xb.noalias() -= Bsp*(rsp/GsEs);
        rhs_xb.noalias() -= Bup*(rup/GuEu);

        // Compute the canonical variables x, y, z
        xb.noalias() = ldlt.solve(rhs_xb);
         y.noalias() = rbp - xb;
        xb.noalias() = xb/GbEb;
        xs.noalias() = (rsp - tr(Bsp)*y)/GsEs;
        zu.noalias() = (tr(Bup)*y - rup)/GuEu;
         y.noalias() = y/Bb;
        zb.noalias() = tbp - xb;
        zs.noalias() = tsp - xs;
        xu.noalias() = tup - zu;

        // Alias to canonicalizer matrix R of the canonical form `C = RAQ = [I S]`.
        const auto& R = canonicalizer.R();

        // Create views to the basic, non-basic, stable, and unstable blocks of matrices X and Z.
        auto Xb =  X.head(nb);
        auto Xn =  X.tail(nn);
        auto Xs = Xn.head(ns);
        auto Xu = Xn.tail(nu);
        auto Zb =  Z.head(nb);
        auto Zn =  Z.tail(nn);
        auto Zs = Zn.head(ns);
        auto Zu = Zn.tail(nu);

        // Finalize the computation of the original variables x, y, z.
        xb.noalias() =  Xb % xb;
        xs.noalias() =  Xs % xs;
        xu.noalias() =  Xu % xu;
         y = -tr(R) * y;
        zb.noalias() =  Zb % zb;
        zs.noalias() =  Zs % zs;
        zu.noalias() =  Zu % zu;

        // Alias to permutation matrix Q of the canonical form `C = RAQ = [I S]`.
        const auto& Q = canonicalizer.Q();

        // Permute back the variables x and z to their original ordering
        Q.applyThisOnTheLeft(x);
        Q.applyThisOnTheLeft(z);
    }
};

SaddlePointSolver::SaddlePointSolver()
: pimpl(new Impl())
{}

SaddlePointSolver::SaddlePointSolver(const SaddlePointSolver& other)
: pimpl(new Impl(*other.pimpl))
{}

SaddlePointSolver::~SaddlePointSolver()
{}

auto SaddlePointSolver::operator=(SaddlePointSolver other) -> SaddlePointSolver&
{
    pimpl = std::move(other.pimpl);
    return *this;
}

auto SaddlePointSolver::canonicalize(const SaddlePointMatrix& lhs) -> void
{
    pimpl->canonicalize(lhs);
}

auto SaddlePointSolver::decompose(const SaddlePointMatrix& lhs) -> void
{
    pimpl->decompose(lhs);
}

auto SaddlePointSolver::solve(const SaddlePointVector& rhs, SaddlePointVector& sol) -> void
{
    return pimpl->solve(rhs, sol);
}

} // namespace Optima
