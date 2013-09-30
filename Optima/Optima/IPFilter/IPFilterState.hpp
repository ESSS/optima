/*
 * IPFilterState.hpp
 *
 *  Created on: 11 Apr 2013
 *      Author: allan
 */

#pragma once

// Eigen includes
#include <Eigen/Core>
using namespace Eigen;

// Optima includes
#include <Optima/Utils/Constraint.hpp>
#include <Optima/Utils/Objective.hpp>

namespace Optima {

/**
 * The algorithmic state at the point (@b x, @b y, @b z)
 */
struct IPFilterState
{
    /**
     * The primal iterate @b x of the algorithm
     */
    VectorXd x;

    /**
     * The dual iterate @b y of the algorithm
     */
    VectorXd y;

    /**
     * The dual iterate @b z of the algorithm
     */
    VectorXd z;

    /**
     * The state of the objective function at @b x
     */
    ObjectiveResult f;

    /**
     * The state of the constraint function at @b x
     */
    ConstraintResult h;

    /**
     * The perturbation parameter @f$\mu@f$ at (@b x, @b z)
     */
    double mu;

    /**
     * The optimality measure @f$\theta_{h}@f$
     */
    double thh;

    /**
     * The optimality measure @f$\theta_{c}@f$
     */
    double thc;

    /**
     * The optimality measure @f$\theta_{l}@f$
     */
    double thl;

    /**
     * The optimality measure @f$\theta@f$
     */
    double theta;

    /**
     * The optimality measure @f$\psi@f$
     */
    double psi;

    /**
     * The feasibility error given by @f$\left\Vert \mathbf{h}(\mathbf{x}_{k})\right\Vert _{\infty}@f$
     */
    double errorh;

    /**
     * The centrality error given by @f$\mathbf{x}_{k}^{T}\mathbf{z}_{k}@f$
     */
    double errorc;

    /**
     * The optimality error given by @f$\left\Vert \nabla_{x}\mathcal{L}(\mathbf{w}_{k})\right\Vert _{\infty}@f$
     */
    double errorl;

    /**
     * The maximum error among the feasibility, centrality and optimality errors
     */
    double error;

    /**
     * The residual error defined as the norm of the KKT equations
     *
     * The residual error @f$ r_{k}@f$ at iteration @e k is defined as the Euclidean norm of the KKT equations,
     * defined by:
     *
     * \f[
     *     \mathbf{F}_{k}=\begin{bmatrix}
     *                      \nabla_{x}\mathcal{L}(\mathbf{x}_{k},\mathbf{y}_{k},\mathbf{z}_{k})\\
     *                      \mathbf{h}(\mathbf{x}_{k})\\
     *                      \mathbf{X}_{k}\mathbf{z}_{k}
     *                    \end{bmatrix}.
     * \f]
     */
    double residual;
};

} /* namespace Optima */