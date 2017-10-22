#pragma once

#include <util/common/math/dsolve.h>
#include <util/common/math/common.h>

namespace model
{

    /**
     *  Given:
     *
     *    [ u''(x) = - 2m/h^2 (E - U(x)) u(x),
     *    | u(x<0) = exp(ikx) + R exp(-ikx),
     *   <|   u(0) = 0,
     *    | u(L+x) = T exp(ikx), \ no reflected wave
     *    [                        after barrier /
     *
     *  where u(x) -- wave function,
     *        U(x) -- barrier
     *           E -- initial energy
     *           k == sqrt ( 2m/h^2 E )
     *
     *  Then:
     *
     *      (solve against u, u')
     *
     *                   v~~~~ unknown
     *    [ u'(0) = - i k u(0) + 2 i k
     *    |        ^~~~~~ m0    ^~~~~~ n0
     *   <|
     *    |            v~~~~ unknown
     *    [ u'(L) = i k u(L)
     *             ^~~~ m1
     *
     *  Let u''(x) = q(x) u(x) + r(x)
     *             = q(x) u(x)  ^~~~~ = 0
     *
     *  Let  u'(x) = a(x) u(x) + b(x)   | d/dx and apply
     *                                 | `let` again
     *
     *  Then:
     *
     *    [ a'(x) + a^2(x)    = -2m/h^2 (E - U(x)),
     *    [ b'(x) + a(x) b(x) = 0
     *
     *  Now we can solve it with initial condition
     *
     *      u'(0) = a(0) u(0) + b(0)
     *               ||          ||
     *            =  m0  u(0) +  n0
     *
     *  and get a(L), b(L). Then
     *
     *    [ u'(L) = a(L) u(L) + b(L),
     *    [ u'(L) =  m1  u(L) +  n1
     *
     *  is linear equation system, from which we can get
     *  u(L), u'(L). With these initial conditions we can
     *  now solve the original equation.
     */

    using cv3 = math::v3 < math::complex < > > ;

    const double sq2pi = 2.5066282746310005; // std::sqrt ( 2 * M_PI )

    /**
     *  U(x) = v0 sum_{i=0}^N \delta(x - a i)
     *
     *  \delta ~ 1 / \sqrt{ 2\pi\sigma^2 } exp^{ - x^2 / \sigma^2 }
     */
    math::continuous_t make_barrier_fn
    (
        size_t n,
        double v0,
        double a,
        double sigma,
        double origin = 0)
    {
        return [=] (double x)
        {
            double y = 0;
            for (size_t j = 0; j < n; ++j)
            {
                y += std::exp (- (x - j * a) / sigma * (x - j * a) / sigma);
            }
            return y * v0 / sq2pi / sigma;
        };
    }

    math::dfunc3_t < cv3 > make_sweep_method_dfunc
    (
        const math::continuous_t & barrier_fn,
        double E,
        double L
    )
    {
        return [=] (double x, const cv3 & ab_)
        {
            return cv3
            {
                - L * L * (E - barrier_fn(x)) - ab_.at<0>() * ab_.at<0>(),
                - ab_.at<0>() * ab_.at<1>()
            };
        };
    }

    math::dfunc3s_t < cv3 > make_schrodinger_dfunc
    (
        const math::continuous_t & barrier_fn,
        double E,
        double L
    )
    {
        return [=] (double x, const cv3 & u__, const cv3 &)
        {
            return cv3
            {
                - L * L * (E - barrier_fn(x)) * u__.at<0>()
            };
        };
    }
}
