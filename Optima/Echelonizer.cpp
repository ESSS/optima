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

#include "Echelonizer.hpp"

// Eigen includes
#include <Optima/deps/eigen3/Eigen/Dense>

// Optima includes
#include <Optima/Exception.hpp>
#include <Optima/IndexUtils.hpp>
#include <Optima/Utils.hpp>

namespace Optima {

struct Echelonizer::Impl
{
    /// The full-pivoting LU decomposition of A so that P*A*Q = L*U;
    Eigen::FullPivLU<Matrix> lu;

    /// The current echelonizer matrix R such that RAQ = C = [I S].
    Matrix R;

    /// The current matrix S in the canonical form C = [I S].
    Matrix S;

    /// The permutation matrix P.
    Indices P;

    /// The transpose of the permutation matrix P.
    Indices Ptr;

    /// The permutation matrix Q.
    Indices Q;

    /// The auxiliary permutation matrix Q.
    Indices Qaux;

    /// The inverse permutation of the new ordering of the variables
    Indices inv_ordering;

    /// The matrix M used in the swap operation.
    Vector M;

    /// The permutation matrix Kb used in the weighted update method.
    PermutationMatrix Kb;

    /// The permutation matrix Kn used in the weighted update method.
    PermutationMatrix Kn;

    /// The backup matrix R used to reset this object to a state with non-accumulated round-off errors.
    Matrix R0;

    /// The backup matrix S used to reset this object to a state with non-accumulated round-off errors.
    Matrix S0;

    /// The backup permutation matrix Q used to reset this object to a state with non-accumulated round-off errors.
    Indices Q0;

    /// The threshold used to compare numbers.
    double threshold;

    /// The number used for eliminating round-off errors during cleanup procedure.
    /// This is computed as 10**[1 + ceil(log10(maxAij))], where maxAij is the
    /// inf norm of matrix A. For each entry in R and S, we add sigma and
    /// subtract sigma, so that residual round-off errors are eliminated.
    double sigma;

    /// Return the number of basic variables, which is also the the rank of matrix A.
    auto numBasicVariables() -> Index
    {
        // Check if max pivot is very small
        if(lu.maxPivot() < 10*std::numeric_limits<double>::epsilon())
        {
            const auto previous_threshold = lu.threshold();
            lu.setThreshold(1.0); // In this case, set threshold to 1, to effectively obtain an absolute comparion instead of relative
            const auto r = lu.rank();
            lu.setThreshold(previous_threshold);
            return r;
        }
        else return lu.rank();
    }

    /// Compute the canonical matrix of the given matrix.
    auto compute(MatrixConstRef A) -> void
    {
        // The number of rows and columns of A
        const auto m = A.rows();
        const auto n = A.cols();

        // Check if number of columns is greater/equal than number of rows
        assert(n >= m && "Could not canonicalize the given matrix. "
            "The given matrix has more rows than columns.");

        /// Initialize the current ordering of the variables
        inv_ordering = indices(n);

        // Compute the full-pivoting LU of A so that P*A*Q = L*U
        lu.compute(A);

        // Get the rank of matrix A
        const auto r = numBasicVariables();

        // Get the LU factors of matrix A
        const auto L   = lu.matrixLU().leftCols(m).triangularView<Eigen::UnitLower>();
        const auto U   = lu.matrixLU().rightCols(n).triangularView<Eigen::Upper>();
        const auto Ubb = lu.matrixLU().topLeftCorner(r, r).triangularView<Eigen::Upper>();
        const auto Ubn = lu.matrixLU().topRightCorner(r, n - r);

        // Set the permutation matrices P and Q
        P = lu.permutationP().indices().cast<Index>();
        Q = lu.permutationQ().indices().cast<Index>();

        // Initialize the permutation matrix Q(aux)
        Qaux = Q;

        Ptr.resize(m);
        Ptr(P) = indices(m);

        // Calculate the regularizer matrix R
        R = P.asPermutation();
        // R.bottomRows(m - r).fill(0.0); // TODO: Decide on these temporary commented lines to ensure rows/cols in R corresponding to linearly dependent rows in A are zero.
        R = L.solve(R);
        R.topRows(r) = Ubb.solve(R.topRows(r));
        // R.bottomRows(m - r).fill(0.0);

        // Calculate matrix S
        S = Ubn;
        S = Ubb.solve(S);

        // Initialize the permutation matrices Kb and Kn
        Kb.setIdentity(r);
        Kn.setIdentity(n - r);

        // Initialize the threshold value
        threshold = std::abs(lu.maxPivot()) * lu.threshold() * std::max(A.rows(), A.cols());

        // Compute sigma for given matrix A
        sigma = A.size() ? A.cwiseAbs().maxCoeff() : 0.0;
        sigma = A.size() ? std::pow(10, 1 + std::ceil(std::log10(sigma))) : 0.0;

        // Set the backup matrices R0, S0, Q0 for resetting purposes
        R0 = R;
        S0 = S;
        Q0 = Q;
    }

    /// Swap a basic variable by a non-basic variable.
    auto updateWithSwapBasicVariable(Index ib, Index in) -> void
    {
        // Get the rank of matrix A
        const Index r = numBasicVariables();

        // Check if ib < rank(A)
        assert(ib < r &&
            "Could not swap basic and non-basic variables. "
                "Expecting an index of basic variable below `r`, where `r = rank(A)`.");

        // Check if in < n - rank(A)
        assert(in < lu.cols() - r &&
            "Could not swap basic and non-basic variables. "
                "Expecting an index of non-basic variable below `n - r`, where `r = rank(A)`.");

        // Check if S(ib, in) is different than zero
        assert(std::abs(S(ib, in)) > threshold &&
            "Could not swap basic and non-basic variables. "
                "Expecting a non-basic variable with non-zero pivot.");

        // Initialize the matrix M
        M = S.col(in);

        // Auxiliary variables
        const Index m = S.rows();
        const double aux = 1.0/S(ib, in);

        // Update the echelonizer matrix R (only its `r` upper rows, where `r = rank(A)`)
        R.row(ib) *= aux;
        for(Index i = 0; i < m; ++i)
            if(i != ib) R.row(i) -= S(i, in) * R.row(ib);

        // Update matrix S
        S.row(ib) *= aux;
        for(Index i = 0; i < m; ++i)
            if(i != ib) S.row(i) -= S(i, in) * S.row(ib);
        S.col(in) = -M*aux;
        S(ib, in) = aux;

        // Update the permutation matrix Q
        std::swap(Q[ib], Q[m + in]);
    }

    /// Update the existing canonical form with given priority weights for the columns.
    auto updateWithPriorityWeights(VectorConstRef w) -> void
    {
        // Assert there are as many weights as there are variables
        assert(w.rows() == lu.cols() &&
            "Could not update the canonical form."
                "Mismatch number of variables and given priority weights.");

        // The rank and number of columns of matrix A
        const Index r = numBasicVariables();
        const Index n = lu.cols();

        // The number of basic and non-basic variables
        const Index nb = r;
        const Index nn = n - r;

        // The upper part of R corresponding to linearly independent rows of A
        auto Rb = R.topRows(nb);

        // The indices of the basic and non-basic variables
        auto ibasic = Q.head(nb);
        auto inonbasic = Q.tail(nn);

        // Find the non-basic variable with maximum proportional weight with respect to a basic variable
        auto find_nonbasic_candidate = [&](Index i, Index& j)
        {
            j = 0; double max = -infinity();
            double tmp = 0.0;
            for(Index k = 0; k < nn; ++k) {
                if(std::abs(S(i, k)) <= threshold) continue;
                tmp = w[inonbasic[k]] * std::abs(S(i, k));
                if(tmp > max) {
                    max = tmp;
                    j = k;
                }
            }
            return max;
        };

        // Check if there are basic variables to be swapped with non-basic variables with higher priority
        if(nn > 0) for(Index i = 0; i < nb; ++i)
        {
            Index j;
            const double wi = w[ibasic[i]];
            const double wj = find_nonbasic_candidate(i, j);
            if(wi < wj)
                updateWithSwapBasicVariable(i, j);
        }

        // Sort the basic variables in descend order of weights
        std::sort(Kb.indices().data(), Kb.indices().data() + nb,
            [&](Index l, Index r) { return w[ibasic[l]] > w[ibasic[r]]; });

        // Sort the non-basic variables in descend order of weights
        std::sort(Kn.indices().data(), Kn.indices().data() + nn,
            [&](Index l, Index r) { return w[inonbasic[l]] > w[inonbasic[r]]; });

        // Rearrange the rows of S based on the new order of basic variables
        Kb.transpose().applyThisOnTheLeft(S);

        // Rearrange the columns of S based on the new order of non-basic variables
        Kn.applyThisOnTheRight(S);

        // Rearrange the top `nb` rows of R based on the new order of basic variables
        Kb.transpose().applyThisOnTheLeft(Rb);

        // Rearrange the permutation matrix Q based on the new order of basic variables
        Kb.transpose().applyThisOnTheLeft(ibasic);

        // Rearrange the permutation matrix Q based on the new order of non-basic variables
        Kn.transpose().applyThisOnTheLeft(inonbasic);
    }

    /// Reset to the canonical matrix form computed initially.
    auto reset() -> void
    {
        R = R0;
        S = S0;
        Q = Q0;
    }

    /// Update the ordering of the basic and non-basic variables,
    auto updateOrdering(IndicesConstRef Kb, IndicesConstRef Kn) -> void
    {
        const auto n  = Q.rows();
        const auto nb = S.rows();
        const auto nn = n - nb;

        assert(nb == Kb.size());
        assert(nn == Kn.size());

        // The top nb rows of R, since its remaining rows correspond to linearly dependent rows in A
        auto Rt = R.topRows(nb);

        // The indices of the basic and non-basic variables
        auto ibasic = Q.head(nb);
        auto inonbasic = Q.tail(nn);

        // Rearrange the rows of S based on the new order of basic variables
        Kb.asPermutation().transpose().applyThisOnTheLeft(S);

        // Rearrange the columns of S based on the new order of non-basic variables
        Kn.asPermutation().applyThisOnTheRight(S);

        // Rearrange the top `nb` rows of R based on the new order of basic variables
        Kb.asPermutation().transpose().applyThisOnTheLeft(Rt);

        // Rearrange the permutation matrix Q based on the new order of basic variables
        Kb.asPermutation().transpose().applyThisOnTheLeft(ibasic);

        // Rearrange the permutation matrix Q based on the new order of non-basic variables
        Kn.asPermutation().transpose().applyThisOnTheLeft(inonbasic);
    }

    /// Perform a cleanup procedure to remove residual round-off errors from the canonical form.
    auto cleanResidualRoundoffErrors() -> void
    {
        S.array() += sigma;
        S.array() -= sigma;

        R.array() += sigma;
        R.array() -= sigma;
    }
};

Echelonizer::Echelonizer()
: pimpl(new Impl())
{}

Echelonizer::Echelonizer(MatrixConstRef A)
: pimpl(new Impl())
{
    compute(A);
}

Echelonizer::Echelonizer(const Echelonizer& other)
: pimpl(new Impl(*other.pimpl))
{}

Echelonizer::~Echelonizer()
{}

auto Echelonizer::operator=(Echelonizer other) -> Echelonizer&
{
    pimpl = std::move(other.pimpl);
    return *this;
}

auto Echelonizer::numVariables() const -> Index
{
    return pimpl->lu.cols();
}

auto Echelonizer::numEquations() const -> Index
{
    return pimpl->lu.rows();
}

auto Echelonizer::numBasicVariables() const -> Index
{
    return pimpl->numBasicVariables();
}

auto Echelonizer::numNonBasicVariables() const -> Index
{
    return numVariables() - numBasicVariables();
}

auto Echelonizer::S() const -> MatrixConstRef
{
    return pimpl->S;
}

auto Echelonizer::R() const -> MatrixConstRef
{
    return pimpl->R;
}

auto Echelonizer::Q() const -> IndicesConstRef
{
    return pimpl->Q;
}

auto Echelonizer::C() const -> Matrix
{
    const Index m  = numEquations();
    const Index n  = numVariables();
    const Index nb = numBasicVariables();
    Matrix res = zeros(m, n);
    res.topRows(nb) << identity(nb, nb), S();
    return res;
}

auto Echelonizer::indicesEquations() const -> IndicesConstRef
{
    return pimpl->Ptr;
}

auto Echelonizer::indicesBasicVariables() const -> IndicesConstRef
{
    return Q().head(numBasicVariables());
}

auto Echelonizer::indicesNonBasicVariables() const -> IndicesConstRef
{
    return Q().tail(numNonBasicVariables());
}

auto Echelonizer::compute(MatrixConstRef A) -> void
{
    pimpl->compute(A);
}

auto Echelonizer::updateWithSwapBasicVariable(Index ibasic, Index inonbasic) -> void
{
    pimpl->updateWithSwapBasicVariable(ibasic, inonbasic);
}

auto Echelonizer::updateWithPriorityWeights(VectorConstRef weights) -> void
{
    pimpl->updateWithPriorityWeights(weights);
}

auto Echelonizer::updateOrdering(IndicesConstRef Kb, IndicesConstRef Kn) -> void
{
    pimpl->updateOrdering(Kb, Kn);
}

auto Echelonizer::reset() -> void
{
    pimpl->reset();
}

auto Echelonizer::cleanResidualRoundoffErrors() -> void
{
    pimpl->cleanResidualRoundoffErrors();
}

} // namespace Optima