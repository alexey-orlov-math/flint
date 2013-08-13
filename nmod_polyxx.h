/*=============================================================================

    This file is part of FLINT.

    FLINT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    FLINT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FLINT; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

=============================================================================*/
/******************************************************************************

    Copyright (C) 2013 Tom Bachmann

******************************************************************************/

#ifndef NMOD_POLYXX_H
#define NMOD_POLYXX_H

#include <cstdlib>

#include "nmod_poly.h"

#include "fmpzxx.h"
#include "nmod_vecxx.h"

#include "flintxx/expression.h"
#include "flintxx/ltuple.h"
#include "flintxx/flint_classes.h"
#include "flintxx/flint_exception.h"
#include "flintxx/frandxx.h"
#include "flintxx/stdmath.h"
#include "flintxx/traits.h"

// TODO exhibit this as a specialisation of a generic poly<nmodxx>
// TODO input
// TODO automatic mulmod, powmod etc?
// TODO mulmod_preinv
// TODO use underscore function versions?
// TODO powmod_mpz*, powmod*preinv, compose_mod_brent_kung_preinv
// TODO nmod_series class?
// TODO subproduct trees?
// TODO factorisation

// TODO 21.17, 21.20-

namespace flint {
FLINT_DEFINE_BINOP(div_newton)
FLINT_DEFINE_BINOP(divrem_newton)
FLINT_DEFINE_BINOP(evaluate_fast)
FLINT_DEFINE_BINOP(evaluate_iter)
FLINT_DEFINE_BINOP(exp_series_basecase)
FLINT_DEFINE_BINOP(gcd_euclidean)
FLINT_DEFINE_BINOP(gcd_hgcd)
FLINT_DEFINE_BINOP(inv_series_basecase)
FLINT_DEFINE_BINOP(nmod_polyxx_get_coeff)
FLINT_DEFINE_BINOP(poly_deflate)
FLINT_DEFINE_BINOP(poly_inflate)
FLINT_DEFINE_BINOP(resultant_euclidean)
FLINT_DEFINE_BINOP(taylor_shift_convolution)
FLINT_DEFINE_BINOP(xgcd_euclidean)
FLINT_DEFINE_BINOP(xgcd_hgcd)

FLINT_DEFINE_THREEARY(compose_mod)
FLINT_DEFINE_THREEARY(compose_mod_brent_kung)
FLINT_DEFINE_THREEARY(compose_mod_horner)
FLINT_DEFINE_THREEARY(div_newton21_preinv)
FLINT_DEFINE_THREEARY(divrem_newton21_preinv)
FLINT_DEFINE_THREEARY(exp_series_monomial)
FLINT_DEFINE_THREEARY(log_series_monomial)
FLINT_DEFINE_THREEARY(mulmod)
FLINT_DEFINE_THREEARY(powmod_binexp)

FLINT_DEFINE_BINOP(nmod_polyxx_interpolate)
FLINT_DEFINE_BINOP(nmod_polyxx_interpolate_barycentric)
FLINT_DEFINE_BINOP(nmod_polyxx_interpolate_fast)
FLINT_DEFINE_BINOP(nmod_polyxx_interpolate_newton)
FLINT_DEFINE_UNOP(nmod_polyxx_product_roots)

template<class Operation, class Data>
class nmod_polyxx_expression
    : public expression<derived_wrapper<nmod_polyxx_expression>,
                            Operation, Data>
{
public:
    typedef expression<derived_wrapper< ::flint::nmod_polyxx_expression>,
              Operation, Data> base_t;

    FLINTXX_DEFINE_BASICS(nmod_polyxx_expression)
    FLINTXX_DEFINE_CTORS(nmod_polyxx_expression)
    FLINTXX_DEFINE_C_REF(nmod_polyxx_expression, nmod_poly_struct, _poly)

    // static functions for nmod_polyxx
    template<class Nmod_vec1, class Nmod_vec2>
    static FLINT_BINOP_ENABLE_RETTYPE(nmod_polyxx_interpolate,
        Nmod_vec1, Nmod_vec2)
    interpolate(const Nmod_vec1& xs, const Nmod_vec2& ys)
    {
        return nmod_polyxx_interpolate(xs, ys);
    }
    template<class Nmod_vec1, class Nmod_vec2>
    static FLINT_BINOP_ENABLE_RETTYPE(nmod_polyxx_interpolate_fast,
        Nmod_vec1, Nmod_vec2)
    interpolate_fast(const Nmod_vec1& xs, const Nmod_vec2& ys)
    {
        return nmod_polyxx_interpolate_fast(xs, ys);
    }
    template<class Nmod_vec1, class Nmod_vec2>
    static FLINT_BINOP_ENABLE_RETTYPE(nmod_polyxx_interpolate_newton,
        Nmod_vec1, Nmod_vec2)
    interpolate_newton(const Nmod_vec1& xs, const Nmod_vec2& ys)
    {
        return nmod_polyxx_interpolate_newton(xs, ys);
    }
    template<class Nmod_vec1, class Nmod_vec2>
    static FLINT_BINOP_ENABLE_RETTYPE(nmod_polyxx_interpolate_barycentric,
        Nmod_vec1, Nmod_vec2)
    interpolate_barycentric(const Nmod_vec1& xs, const Nmod_vec2& ys)
    {
        return nmod_polyxx_interpolate_barycentric(xs, ys);
    }

    template<class Nmod_vec>
    static FLINT_UNOP_ENABLE_RETTYPE(nmod_polyxx_product_roots, Nmod_vec)
    product_roots(const Nmod_vec& xs)
    {
        return nmod_polyxx_product_roots(xs);
    }

    // XXX this is difficult to make lazy
    template<class Fmpz>
    static typename mp::enable_if<traits::is_fmpzxx<Fmpz>,
        nmod_polyxx_expression>::type
    bit_unpack(const Fmpz& a, mp_bitcnt_t bits, nmodxx_ctx_srcref modulus)
    {
        nmod_polyxx_expression res(modulus);
        nmod_poly_bit_unpack(res._poly(), a.evaluate()._fmpz(), bits);
        return res;
    }

    // these only make sense with immediates
    void realloc(slong alloc) {nmod_poly_realloc(_poly(), alloc);}
    void fit_length(slong len) {nmod_poly_fit_length(_poly(), len);}
    void _normalise() {_nmod_poly_normalise(_poly());}
    nmodxx_ctx_srcref _ctx() const
        {return nmodxx_ctx_srcref::make(_poly()->mod);}

    // These only make sense with target immediates
    void set_coeff(slong n, ulong c) {nmod_poly_set_coeff_ui(_poly(), n, c);}
    template<class Nmod>
    typename mp::enable_if<traits::is_nmodxx<Nmod> >::type
    set_coeff(slong j, const Nmod& c)
    {
        // TODO this does not need reduction
        nmod_poly_set_coeff_ui(_poly(), j, c.template to<mp_limb_t>());
    }
    void truncate(slong n) {nmod_poly_truncate(_poly(), n);}

    void set_randtest(frandxx& state, slong len)
        {nmod_poly_randtest(_poly(), state._data(), len);}
    void set_randtest_irreducible(frandxx& state, slong len)
        {nmod_poly_randtest_irreducible(_poly(), state._data(), len);}

    // These work on any expression without evaluation
    nmodxx_ctx_srcref estimate_ctx() const;
    mp_limb_t modulus() const {return estimate_ctx().n();}

    evaluated_t create_temporary() const
    {
        return evaluated_t(estimate_ctx());
    }

    // These cause evaluation
    slong length() const {return nmod_poly_length(this->evaluate()._poly());}
    slong degree() const {return nmod_poly_degree(this->evaluate()._poly());}
    bool is_one() const {return nmod_poly_is_one(this->evaluate()._poly());}
    bool is_zero() const {return nmod_poly_is_zero(this->evaluate()._poly());}
    bool is_squarefree() const
        {return nmod_poly_is_squarefree(this->evaluate()._poly());}
    bool is_irreducible() const
        {return nmod_poly_is_irreducible(this->evaluate()._poly());}
    slong max_bits() const {return nmod_poly_max_bits(this->evaluate()._poly());}
    ulong deflation() const
        {return nmod_poly_deflation(this->evaluate()._poly());}

    // Lazy members
    FLINTXX_DEFINE_MEMBER_BINOP_(get_coeff, nmod_polyxx_get_coeff)
    FLINTXX_DEFINE_MEMBER_BINOP_(operator(), compeval)
    FLINTXX_DEFINE_MEMBER_BINOP_(inflate, poly_inflate)
    FLINTXX_DEFINE_MEMBER_BINOP_(deflate, poly_deflate)

    FLINTXX_DEFINE_MEMBER_BINOP(compose_divconquer)
    FLINTXX_DEFINE_MEMBER_BINOP(compose_horner)
    FLINTXX_DEFINE_MEMBER_BINOP(div_basecase)
    FLINTXX_DEFINE_MEMBER_BINOP(div_divconquer)
    FLINTXX_DEFINE_MEMBER_BINOP(div_newton)
    FLINTXX_DEFINE_MEMBER_BINOP(divrem)
    FLINTXX_DEFINE_MEMBER_BINOP(divrem_basecase)
    FLINTXX_DEFINE_MEMBER_BINOP(divrem_divconquer)
    FLINTXX_DEFINE_MEMBER_BINOP(divrem_newton)
    FLINTXX_DEFINE_MEMBER_BINOP(div_root)
    FLINTXX_DEFINE_MEMBER_BINOP(evaluate_fast)
    FLINTXX_DEFINE_MEMBER_BINOP(evaluate_iter)
    FLINTXX_DEFINE_MEMBER_BINOP(gcd)
    FLINTXX_DEFINE_MEMBER_BINOP(gcd_euclidean)
    FLINTXX_DEFINE_MEMBER_BINOP(gcd_hgcd)
    FLINTXX_DEFINE_MEMBER_BINOP(inv_series)
    FLINTXX_DEFINE_MEMBER_BINOP(inv_series_basecase)
    FLINTXX_DEFINE_MEMBER_BINOP(inv_series_newton)
    FLINTXX_DEFINE_MEMBER_BINOP(invsqrt_series)
    FLINTXX_DEFINE_MEMBER_BINOP(mul_classical)
    FLINTXX_DEFINE_MEMBER_BINOP(mul_KS)
    FLINTXX_DEFINE_MEMBER_BINOP(poly_shift_left)
    FLINTXX_DEFINE_MEMBER_BINOP(poly_shift_right)
    FLINTXX_DEFINE_MEMBER_BINOP(pow)
    FLINTXX_DEFINE_MEMBER_BINOP(pow_binexp)
    FLINTXX_DEFINE_MEMBER_BINOP(rem_basecase)
    FLINTXX_DEFINE_MEMBER_BINOP(resultant)
    FLINTXX_DEFINE_MEMBER_BINOP(resultant_euclidean)
    FLINTXX_DEFINE_MEMBER_BINOP(reverse)
    FLINTXX_DEFINE_MEMBER_BINOP(revert_series)
    FLINTXX_DEFINE_MEMBER_BINOP(revert_series_lagrange)
    FLINTXX_DEFINE_MEMBER_BINOP(revert_series_lagrange_fast)
    FLINTXX_DEFINE_MEMBER_BINOP(revert_series_newton)
    FLINTXX_DEFINE_MEMBER_BINOP(sqrt_series)
    FLINTXX_DEFINE_MEMBER_BINOP(taylor_shift)
    FLINTXX_DEFINE_MEMBER_BINOP(taylor_shift_convolution)
    FLINTXX_DEFINE_MEMBER_BINOP(taylor_shift_horner)
    FLINTXX_DEFINE_MEMBER_BINOP(xgcd)
    FLINTXX_DEFINE_MEMBER_BINOP(xgcd_euclidean)
    FLINTXX_DEFINE_MEMBER_BINOP(xgcd_hgcd)
    FLINTXX_DEFINE_MEMBER_BINOP(log_series)
    FLINTXX_DEFINE_MEMBER_BINOP(exp_series)
    FLINTXX_DEFINE_MEMBER_BINOP(exp_series_basecase)
    FLINTXX_DEFINE_MEMBER_BINOP(atan_series)
    FLINTXX_DEFINE_MEMBER_BINOP(atanh_series)
    FLINTXX_DEFINE_MEMBER_BINOP(asin_series)
    FLINTXX_DEFINE_MEMBER_BINOP(asinh_series)
    FLINTXX_DEFINE_MEMBER_BINOP(sin_series)
    FLINTXX_DEFINE_MEMBER_BINOP(cos_series)
    FLINTXX_DEFINE_MEMBER_BINOP(tan_series)
    FLINTXX_DEFINE_MEMBER_BINOP(sinh_series)
    FLINTXX_DEFINE_MEMBER_BINOP(cosh_series)
    FLINTXX_DEFINE_MEMBER_BINOP(tanh_series)

    FLINTXX_DEFINE_MEMBER_BINOP_(bit_pack, poly_bit_pack)

    FLINTXX_DEFINE_MEMBER_UNOP(derivative)
    FLINTXX_DEFINE_MEMBER_UNOP(integral)
    FLINTXX_DEFINE_MEMBER_UNOP(make_monic)
    FLINTXX_DEFINE_MEMBER_UNOP(sqrt)

    FLINTXX_DEFINE_MEMBER_3OP(compose_mod)
    FLINTXX_DEFINE_MEMBER_3OP(compose_mod_horner)
    FLINTXX_DEFINE_MEMBER_3OP(compose_mod_brent_kung)
    FLINTXX_DEFINE_MEMBER_3OP(compose_series)
    FLINTXX_DEFINE_MEMBER_3OP(compose_series_brent_kung)
    FLINTXX_DEFINE_MEMBER_3OP(compose_series_divconquer)
    FLINTXX_DEFINE_MEMBER_3OP(compose_series_horner)
    FLINTXX_DEFINE_MEMBER_3OP(div_newton21_preinv)
    FLINTXX_DEFINE_MEMBER_3OP(divrem_newton21_preinv)
    FLINTXX_DEFINE_MEMBER_3OP(div_series)
    FLINTXX_DEFINE_MEMBER_3OP(mulhigh)
    FLINTXX_DEFINE_MEMBER_3OP(mulhigh_classical)
    FLINTXX_DEFINE_MEMBER_3OP(mullow)
    FLINTXX_DEFINE_MEMBER_3OP(mullow_classical)
    FLINTXX_DEFINE_MEMBER_3OP(mullow_KS)
    FLINTXX_DEFINE_MEMBER_3OP(mulmod)
    FLINTXX_DEFINE_MEMBER_3OP(powmod_binexp)
    FLINTXX_DEFINE_MEMBER_3OP(pow_trunc)
    FLINTXX_DEFINE_MEMBER_3OP(pow_trunc_binexp)
};

namespace detail {
struct nmod_poly_data;
}

typedef nmod_polyxx_expression<operations::immediate, detail::nmod_poly_data>
           nmod_polyxx;
typedef nmod_polyxx_expression<operations::immediate,
            flint_classes::ref_data<nmod_polyxx, nmod_poly_struct> >
           nmod_polyxx_ref;
typedef nmod_polyxx_expression<operations::immediate,
            flint_classes::srcref_data<
                nmod_polyxx, nmod_polyxx_ref, nmod_poly_struct> >
           nmod_polyxx_srcref;

#define NMOD_POLYXX_COND_S FLINTXX_COND_S(nmod_polyxx)
#define NMOD_POLYXX_COND_T FLINTXX_COND_T(nmod_polyxx)

namespace detail {
struct nmod_poly_data
{
    nmod_poly_t inner;
    typedef nmod_poly_t& data_ref_t;
    typedef const nmod_poly_t& data_srcref_t;

    nmod_poly_data(mp_limb_t n) {nmod_poly_init(inner, n);}
    nmod_poly_data(nmodxx_ctx_srcref c)
    {
        nmod_poly_init_preinv(inner, c.n(), c._nmod().ninv);
    }
    nmod_poly_data(mp_limb_t n, slong alloc) {nmod_poly_init2(inner, n, alloc);}
    nmod_poly_data(nmodxx_ctx_srcref c, slong alloc)
    {
        nmod_poly_init2_preinv(inner, c.n(), c._nmod().ninv, alloc);
    }
    ~nmod_poly_data() {nmod_poly_clear(inner);}

    nmod_poly_data(const nmod_poly_data& o)
    {
        nmod_poly_init2_preinv(inner, o.inner->mod.n,
                o.inner->mod.ninv, o.inner->length);
        nmod_poly_set(inner, o.inner);
    }

    nmod_poly_data(nmod_polyxx_srcref r)
    {
        nmod_poly_init2_preinv(inner, r.modulus(),
                r._poly()->mod.ninv, r.length());
        nmod_poly_set(inner, r._poly());
    }
};
} // detail
namespace traits {
template<> struct has_nmodxx_ctx<nmod_polyxx> : mp::true_ { };
template<> struct has_nmodxx_ctx<nmod_polyxx_ref> : mp::true_ { };
template<> struct has_nmodxx_ctx<nmod_polyxx_srcref> : mp::true_ { };
} // traits
template<class Operation, class Data>
inline nmodxx_ctx_srcref
nmod_polyxx_expression<Operation, Data>::estimate_ctx() const
{
    return tools::find_nmodxx_ctx(*this);
}

namespace rules {
#define NMOD_POLYXX_COND_S FLINTXX_COND_S(nmod_polyxx)
#define NMOD_POLYXX_COND_T FLINTXX_COND_T(nmod_polyxx)

FLINT_DEFINE_DOIT_COND2(assignment, NMOD_POLYXX_COND_T, NMOD_POLYXX_COND_S,
        nmod_poly_set(to._poly(), from._poly()))

FLINTXX_DEFINE_ASSIGN_STR(nmod_polyxx, execution_check(
            nmod_poly_set_str(to._poly(), from), "assign string", "nmod_polyxx"))

FLINTXX_DEFINE_EQUALS(nmod_polyxx, nmod_poly_equal(e1._poly(), e2._poly()))
FLINTXX_DEFINE_TO_STR(nmod_polyxx, nmod_poly_get_str(from._poly()))
FLINTXX_DEFINE_SWAP(nmod_polyxx, nmod_poly_swap(e1._poly(), e2._poly()))

FLINT_DEFINE_BINARY_EXPR_COND2(nmod_polyxx_get_coeff_op, nmodxx,
        NMOD_POLYXX_COND_S, traits::fits_into_slong,
        to.set_nored(nmod_poly_get_coeff_ui(e1._poly(), e2)))

FLINT_DEFINE_BINARY_EXPR_COND2(plus, nmod_polyxx,
        NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S,
        nmod_poly_add(to._poly(), e1._poly(), e2._poly()))
FLINT_DEFINE_BINARY_EXPR_COND2(minus, nmod_polyxx,
        NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S,
        nmod_poly_sub(to._poly(), e1._poly(), e2._poly()))

FLINT_DEFINE_UNARY_EXPR_COND(negate, nmod_polyxx, NMOD_POLYXX_COND_S,
        nmod_poly_neg(to._poly(), from._poly()))

FLINT_DEFINE_BINARY_EXPR_COND2(reverse_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, traits::fits_into_slong,
        nmod_poly_reverse(to._poly(), e1._poly(), e2))

FLINT_DEFINE_BINARY_EXPR_COND2(poly_shift_left_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, traits::fits_into_slong,
        nmod_poly_shift_left(to._poly(), e1._poly(), e2))
FLINT_DEFINE_BINARY_EXPR_COND2(poly_shift_right_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, traits::fits_into_slong,
        nmod_poly_shift_right(to._poly(), e1._poly(), e2))

FLINT_DEFINE_CBINARY_EXPR_COND2(times, nmod_polyxx,
        NMOD_POLYXX_COND_S, NMODXX_COND_S,
        nmod_poly_scalar_mul_nmod(to._poly(), e1._poly(), e2._limb()))
FLINT_DEFINE_UNARY_EXPR_COND(make_monic_op, nmod_polyxx, NMOD_POLYXX_COND_S,
        nmod_poly_make_monic(to._poly(), from._poly()))

FLINT_DEFINE_BINARY_EXPR_COND2(poly_bit_pack_op, fmpzxx,
        NMOD_POLYXX_COND_S, traits::fits_into_mp_bitcnt_t,
        nmod_poly_bit_pack(to._fmpz(), e1._poly(), e2))

FLINT_DEFINE_BINARY_EXPR_COND2(times, nmod_polyxx,
        NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S,
        nmod_poly_mul(to._poly(), e1._poly(), e2._poly()))
FLINT_DEFINE_BINARY_EXPR_COND2(mul_classical_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S,
        nmod_poly_mul_classical(to._poly(), e1._poly(), e2._poly()))
FLINT_DEFINE_BINARY_EXPR_COND2(mul_KS_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S,
        nmod_poly_mul_KS(to._poly(), e1._poly(), e2._poly(), 0 /* TODO */))

#define NMOD_POLYXX_DEFINE_MULFUNC(name) \
FLINT_DEFINE_THREEARY_EXPR_COND3(name##_op, nmod_polyxx, \
    NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S, traits::fits_into_slong, \
    nmod_poly_##name(to._poly(), e1._poly(), e2._poly(), e3))
NMOD_POLYXX_DEFINE_MULFUNC(mullow_classical)
NMOD_POLYXX_DEFINE_MULFUNC(mullow)
NMOD_POLYXX_DEFINE_MULFUNC(mulhigh_classical)
NMOD_POLYXX_DEFINE_MULFUNC(mulhigh)

FLINT_DEFINE_THREEARY_EXPR_COND3(mullow_KS_op, nmod_polyxx,
    NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S, traits::fits_into_slong,
    nmod_poly_mullow_KS(to._poly(), e1._poly(), e2._poly(), 0 /* TODO */, e3))

FLINT_DEFINE_THREEARY_EXPR_COND3(mulmod_op, nmod_polyxx,
    NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S,
    nmod_poly_mulmod(to._poly(), e1._poly(), e2._poly(), e3._poly()))

FLINT_DEFINE_BINARY_EXPR_COND2(pow_binexp_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, traits::is_unsigned_integer,
        nmod_poly_pow_binexp(to._poly(), e1._poly(), e2))
FLINT_DEFINE_BINARY_EXPR_COND2(pow_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, traits::is_unsigned_integer,
        nmod_poly_pow(to._poly(), e1._poly(), e2))

FLINT_DEFINE_THREEARY_EXPR_COND3(powmod_binexp_op, nmod_polyxx,
    NMOD_POLYXX_COND_S, traits::is_unsigned_integer, NMOD_POLYXX_COND_S,
    nmod_poly_powmod_ui_binexp(to._poly(), e1._poly(), e2, e3._poly()))

FLINT_DEFINE_THREEARY_EXPR_COND3(pow_trunc_op, nmod_polyxx,
    NMOD_POLYXX_COND_S, traits::is_unsigned_integer, traits::fits_into_slong,
    nmod_poly_pow_trunc(to._poly(), e1._poly(), e2, e3))
FLINT_DEFINE_THREEARY_EXPR_COND3(pow_trunc_binexp_op, nmod_polyxx,
    NMOD_POLYXX_COND_S, traits::is_unsigned_integer, traits::fits_into_slong,
    nmod_poly_pow_trunc_binexp(to._poly(), e1._poly(), e2, e3))

FLINT_DEFINE_UNARY_EXPR_COND(derivative_op, nmod_polyxx, NMOD_POLYXX_COND_S,
        nmod_poly_derivative(to._poly(), from._poly()))
FLINT_DEFINE_UNARY_EXPR_COND(integral_op, nmod_polyxx, NMOD_POLYXX_COND_S,
        nmod_poly_integral(to._poly(), from._poly()))

FLINT_DEFINE_BINARY_EXPR_COND2(evaluate_op, nmodxx,
        NMOD_POLYXX_COND_S, NMODXX_COND_S,
        to.set_nored(nmod_poly_evaluate_nmod(e1._poly(), e2._limb())))

FLINT_DEFINE_BINARY_EXPR_COND2(evaluate_op, nmod_vecxx,
        NMOD_POLYXX_COND_S, NMOD_VECXX_COND_S,
        nmod_poly_evaluate_nmod_vec(to._array(), e1._poly(), e2._array(),
            to.size()))
FLINT_DEFINE_BINARY_EXPR_COND2(evaluate_fast_op, nmod_vecxx,
        NMOD_POLYXX_COND_S, NMOD_VECXX_COND_S,
        nmod_poly_evaluate_nmod_vec_fast(to._array(), e1._poly(), e2._array(),
            to.size()))
FLINT_DEFINE_BINARY_EXPR_COND2(evaluate_iter_op, nmod_vecxx,
        NMOD_POLYXX_COND_S, NMOD_VECXX_COND_S,
        nmod_poly_evaluate_nmod_vec_iter(to._array(), e1._poly(), e2._array(),
            to.size()))

namespace rdetail {
typedef make_ltuple<mp::make_tuple<nmod_polyxx, nmod_polyxx>::type>::type
    nmod_polyxx_pair;
} // rdetail
#define NMOD_POLYXX_DEFINE_DIVREM(name) \
FLINT_DEFINE_BINARY_EXPR_COND2(name##_op, rdetail::nmod_polyxx_pair, \
    NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S, \
    nmod_poly_##name(to.template get<0>()._poly(), to.template get<1>()._poly(), \
        e1._poly(), e2._poly()))
NMOD_POLYXX_DEFINE_DIVREM(divrem_basecase)
NMOD_POLYXX_DEFINE_DIVREM(divrem_divconquer)
NMOD_POLYXX_DEFINE_DIVREM(divrem_newton)
NMOD_POLYXX_DEFINE_DIVREM(divrem)

FLINT_DEFINE_BINARY_EXPR_COND2(div_basecase_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S,
        nmod_poly_div_basecase(to._poly(), e1._poly(), e2._poly()))
FLINT_DEFINE_BINARY_EXPR_COND2(div_divconquer_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S,
        nmod_poly_div_divconquer(to._poly(), e1._poly(), e2._poly()))
FLINT_DEFINE_BINARY_EXPR_COND2(div_newton_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S,
        nmod_poly_div_newton(to._poly(), e1._poly(), e2._poly()))
FLINT_DEFINE_BINARY_EXPR_COND2(divided_by, nmod_polyxx,
        NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S,
        nmod_poly_div_divconquer(to._poly(), e1._poly(), e2._poly()))

FLINT_DEFINE_THREEARY_EXPR_COND3(div_newton21_preinv_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S,
        nmod_poly_div_newton21_preinv(to._poly(),
            e1._poly(), e2._poly(), e3._poly()))
FLINT_DEFINE_THREEARY_EXPR_COND3(divrem_newton21_preinv_op,
        rdetail::nmod_polyxx_pair,
        NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S,
        nmod_poly_divrem_newton21_preinv(to.template get<0>()._poly(),
            to.template get<1>()._poly(), e1._poly(), e2._poly(), e3._poly()))

FLINT_DEFINE_BINARY_EXPR_COND2(rem_basecase_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S,
        nmod_poly_rem_basecase(to._poly(), e1._poly(), e2._poly()))
FLINT_DEFINE_BINARY_EXPR_COND2(modulo, nmod_polyxx,
        NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S,
        nmod_poly_rem(to._poly(), e1._poly(), e2._poly()))

FLINT_DEFINE_BINARY_EXPR_COND2(inv_series_newton_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, traits::fits_into_slong,
        nmod_poly_inv_series_newton(to._poly(), e1._poly(), e2))
FLINT_DEFINE_BINARY_EXPR_COND2(inv_series_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, traits::fits_into_slong,
        nmod_poly_inv_series(to._poly(), e1._poly(), e2))
FLINT_DEFINE_BINARY_EXPR_COND2(inv_series_basecase_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, traits::fits_into_slong,
        nmod_poly_inv_series_basecase(to._poly(), e1._poly(), e2))

#define NMOD_POLYXX_DEFINE_SERIES(name) \
FLINT_DEFINE_THREEARY_EXPR_COND3(name##_op, nmod_polyxx, \
    NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S, traits::fits_into_slong, \
    nmod_poly_##name(to._poly(), e1._poly(), e2._poly(), e3))
NMOD_POLYXX_DEFINE_SERIES(div_series)

FLINT_DEFINE_BINARY_EXPR_COND2(compose_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S,
        nmod_poly_compose(to._poly(), e1._poly(), e2._poly()))
FLINT_DEFINE_BINARY_EXPR_COND2(compose_divconquer_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S,
        nmod_poly_compose_divconquer(to._poly(), e1._poly(), e2._poly()))
FLINT_DEFINE_BINARY_EXPR_COND2(compose_horner_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S,
        nmod_poly_compose_horner(to._poly(), e1._poly(), e2._poly()))

FLINT_DEFINE_BINARY_EXPR_COND2(div_root_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, NMODXX_COND_S,
        nmod_poly_div_root(to._poly(), e1._poly(), e2._limb()))

FLINT_DEFINE_BINARY_EXPR_COND2(nmod_polyxx_interpolate_op, nmod_polyxx,
        NMOD_VECXX_COND_S, NMOD_VECXX_COND_S,
        nmod_poly_interpolate_nmod_vec(to._poly(), e1._data().array,
            e2._data().array, e2.size()))
FLINT_DEFINE_BINARY_EXPR_COND2(nmod_polyxx_interpolate_fast_op, nmod_polyxx,
        NMOD_VECXX_COND_S, NMOD_VECXX_COND_S,
        nmod_poly_interpolate_nmod_vec_fast(to._poly(), e1._data().array,
            e2._data().array, e2.size()))
FLINT_DEFINE_BINARY_EXPR_COND2(nmod_polyxx_interpolate_newton_op, nmod_polyxx,
        NMOD_VECXX_COND_S, NMOD_VECXX_COND_S,
        nmod_poly_interpolate_nmod_vec_newton(to._poly(), e1._data().array,
            e2._data().array, e2.size()))
FLINT_DEFINE_BINARY_EXPR_COND2(nmod_polyxx_interpolate_barycentric_op, nmod_polyxx,
        NMOD_VECXX_COND_S, NMOD_VECXX_COND_S,
        nmod_poly_interpolate_nmod_vec_barycentric(to._poly(), e1._data().array,
            e2._data().array, e2.size()))

FLINT_DEFINE_BINARY_EXPR_COND2(taylor_shift_horner_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, NMODXX_COND_S,
        nmod_poly_taylor_shift_horner(to._poly(), e1._poly(), e2._limb()))
FLINT_DEFINE_BINARY_EXPR_COND2(taylor_shift_convolution_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, NMODXX_COND_S,
        nmod_poly_taylor_shift_convolution(to._poly(), e1._poly(), e2._limb()))
FLINT_DEFINE_BINARY_EXPR_COND2(taylor_shift_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, NMODXX_COND_S,
        nmod_poly_taylor_shift(to._poly(), e1._poly(), e2._limb()))

FLINT_DEFINE_THREEARY_EXPR_COND3(compose_mod_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S,
        nmod_poly_compose_mod(to._poly(), e1._poly(), e2._poly(), e3._poly()))
FLINT_DEFINE_THREEARY_EXPR_COND3(compose_mod_horner_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S,
        nmod_poly_compose_mod_horner(
            to._poly(), e1._poly(), e2._poly(), e3._poly()))
FLINT_DEFINE_THREEARY_EXPR_COND3(compose_mod_brent_kung_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S,
        nmod_poly_compose_mod_brent_kung(
            to._poly(), e1._poly(), e2._poly(), e3._poly()))

FLINT_DEFINE_BINARY_EXPR_COND2(gcd_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S,
        nmod_poly_gcd(to._poly(), e1._poly(), e2._poly()))
FLINT_DEFINE_BINARY_EXPR_COND2(gcd_hgcd_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S,
        nmod_poly_gcd_hgcd(to._poly(), e1._poly(), e2._poly()))
FLINT_DEFINE_BINARY_EXPR_COND2(gcd_euclidean_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S,
        nmod_poly_gcd_euclidean(to._poly(), e1._poly(), e2._poly()))

namespace rdetail {
typedef make_ltuple<mp::make_tuple<nmod_polyxx, nmod_polyxx, nmod_polyxx>::type>::type
    nmod_polyxx_triple;
} // rdetail
FLINT_DEFINE_BINARY_EXPR_COND2(xgcd_op, rdetail::nmod_polyxx_triple,
    NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S,
    nmod_poly_xgcd(to.template get<0>()._poly(), to.template get<1>()._poly(),
        to.template get<2>()._poly(), e1._poly(), e2._poly()))
FLINT_DEFINE_BINARY_EXPR_COND2(xgcd_hgcd_op, rdetail::nmod_polyxx_triple,
    NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S,
    nmod_poly_xgcd_hgcd(to.template get<0>()._poly(),
        to.template get<1>()._poly(),
        to.template get<2>()._poly(), e1._poly(), e2._poly()))
FLINT_DEFINE_BINARY_EXPR_COND2(xgcd_euclidean_op, rdetail::nmod_polyxx_triple,
    NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S,
    nmod_poly_xgcd_euclidean(to.template get<0>()._poly(),
        to.template get<1>()._poly(),
        to.template get<2>()._poly(), e1._poly(), e2._poly()))

FLINT_DEFINE_BINARY_EXPR_COND2(resultant_op, nmodxx,
        NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S,
        to.set_nored(nmod_poly_resultant(e1._poly(), e2._poly())))
FLINT_DEFINE_BINARY_EXPR_COND2(resultant_euclidean_op, nmodxx,
        NMOD_POLYXX_COND_S, NMOD_POLYXX_COND_S,
        to.set_nored(nmod_poly_resultant_euclidean(e1._poly(), e2._poly())))

NMOD_POLYXX_DEFINE_SERIES(compose_series)
NMOD_POLYXX_DEFINE_SERIES(compose_series_horner)
NMOD_POLYXX_DEFINE_SERIES(compose_series_brent_kung)
NMOD_POLYXX_DEFINE_SERIES(compose_series_divconquer)

#define NMOD_POLYXX_DEFINE_SERIESUN(name) \
FLINT_DEFINE_BINARY_EXPR_COND2(name##_op, nmod_polyxx, \
        NMOD_POLYXX_COND_S, traits::fits_into_slong, \
        nmod_poly_##name(to._poly(), e1._poly(), e2))
NMOD_POLYXX_DEFINE_SERIESUN(revert_series)
NMOD_POLYXX_DEFINE_SERIESUN(revert_series_newton)
NMOD_POLYXX_DEFINE_SERIESUN(revert_series_lagrange_fast)
NMOD_POLYXX_DEFINE_SERIESUN(revert_series_lagrange)

#define NMOD_POLYXX_DEFINE_SERIES_F(name) \
FLINT_DEFINE_BINARY_EXPR_COND2(name##_series_op, nmod_polyxx, \
        NMOD_POLYXX_COND_S, traits::fits_into_slong, \
        nmod_poly_##name##_series(to._poly(), e1._poly(), e2))
NMOD_POLYXX_DEFINE_SERIES_F(sqrt)
NMOD_POLYXX_DEFINE_SERIES_F(invsqrt)
NMOD_POLYXX_DEFINE_SERIES_F(log)
NMOD_POLYXX_DEFINE_SERIES_F(exp)
NMOD_POLYXX_DEFINE_SERIES_F(atan)
NMOD_POLYXX_DEFINE_SERIES_F(atanh)
NMOD_POLYXX_DEFINE_SERIES_F(asin)
NMOD_POLYXX_DEFINE_SERIES_F(asinh)
NMOD_POLYXX_DEFINE_SERIES_F(sin)
NMOD_POLYXX_DEFINE_SERIES_F(cos)
NMOD_POLYXX_DEFINE_SERIES_F(tan)
NMOD_POLYXX_DEFINE_SERIES_F(sinh)
NMOD_POLYXX_DEFINE_SERIES_F(cosh)
NMOD_POLYXX_DEFINE_SERIES_F(tanh)

FLINT_DEFINE_BINARY_EXPR_COND2(exp_series_basecase_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, traits::fits_into_slong,
        nmod_poly_exp_series_basecase(to._poly(), e1._poly(), e2))

FLINT_DEFINE_THREEARY_EXPR_COND3(log_series_monomial_op, nmod_polyxx,
        NMODXX_COND_S, traits::is_unsigned_integer, traits::fits_into_slong,
        nmod_poly_log_series_monomial_ui(to._poly(), e1._limb(), e2, e3))
FLINT_DEFINE_THREEARY_EXPR_COND3(exp_series_monomial_op, nmod_polyxx,
        NMODXX_COND_S, traits::is_unsigned_integer, traits::fits_into_slong,
        nmod_poly_exp_series_monomial_ui(to._poly(), e1._limb(), e2, e3))

FLINT_DEFINE_UNARY_EXPR_COND(sqrt_op, nmod_polyxx, NMOD_POLYXX_COND_S,
        execution_check(nmod_poly_sqrt(to._poly(), from._poly()),
            "sqrt", "nmod_polyxx"))

FLINT_DEFINE_UNARY_EXPR_COND(nmod_polyxx_product_roots_op, nmod_polyxx,
        NMOD_VECXX_COND_S,
        nmod_poly_product_roots_nmod_vec(to._poly(),
            from._data().array, from.size()))

FLINT_DEFINE_BINARY_EXPR_COND2(poly_deflate_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, traits::is_unsigned_integer,
        nmod_poly_deflate(to._poly(), e1._poly(), e2))
FLINT_DEFINE_BINARY_EXPR_COND2(poly_inflate_op, nmod_polyxx,
        NMOD_POLYXX_COND_S, traits::is_unsigned_integer,
        nmod_poly_inflate(to._poly(), e1._poly(), e2))
} // rules
} // flint

#endif
