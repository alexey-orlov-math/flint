#include "qqbar.h"
#include "flint/fmpq_poly.h"
#include "flint/fmpq_mat.h"
#include "gr.h"

int
_gr_fmpq_ctx_write(gr_stream_t out, gr_ctx_t ctx)
{
    gr_stream_write(out, "Rational field (fmpq)");
    return GR_SUCCESS;
}

void
_gr_fmpq_init(fmpq_t x, const gr_ctx_t ctx)
{
    fmpq_init(x);
}

void
_gr_fmpq_clear(fmpq_t x, const gr_ctx_t ctx)
{
    fmpq_clear(x);
}

void
_gr_fmpq_swap(fmpq_t x, fmpq_t y, const gr_ctx_t ctx)
{
    fmpq_t t;
    *t = *x;
    *x = *y;
    *y = *t;
}

/* todo: limits */
int
_gr_fmpq_randtest(fmpq_t res, flint_rand_t state, const gr_ctx_t ctx)
{
    switch (n_randint(state, 4))
    {
        case 0:
            fmpq_randtest(res, state, 100);
            break;
        default:
            fmpq_randtest(res, state, 10);
    }

    return GR_SUCCESS;
}

int
_gr_fmpq_write(gr_stream_t out, const fmpq_t x, const gr_ctx_t ctx)
{
    gr_stream_write_fmpz(out, fmpq_numref(x));

    if (!fmpz_is_one(fmpq_denref(x)))
    {
        gr_stream_write(out, "/");
        gr_stream_write_fmpz(out, fmpq_denref(x));
    }

    return GR_SUCCESS;
}

int
_gr_fmpq_zero(fmpq_t x, const gr_ctx_t ctx)
{
    fmpq_zero(x);
    return GR_SUCCESS;
}

int
_gr_fmpq_one(fmpq_t x, const gr_ctx_t ctx)
{
    fmpq_one(x);
    return GR_SUCCESS;
}

int
_gr_fmpq_set_si(fmpq_t res, slong v, const gr_ctx_t ctx)
{
    fmpq_set_si(res, v, 1);
    return GR_SUCCESS;
}

int
_gr_fmpq_set_ui(fmpq_t res, ulong v, const gr_ctx_t ctx)
{
    fmpq_set_ui(res, v, 1);
    return GR_SUCCESS;
}

int
_gr_fmpq_set_fmpz(fmpq_t res, const fmpz_t v, const gr_ctx_t ctx)
{
    fmpz_set(fmpq_numref(res), v);
    fmpz_one(fmpq_denref(res));
    return GR_SUCCESS;
}

int
_gr_fmpq_set_other(fmpq_t res, gr_srcptr x, gr_ctx_t x_ctx, const gr_ctx_t ctx)
{
    switch (x_ctx->which_ring)
    {
        case GR_CTX_FMPZ:
            fmpq_set_fmpz(res, x);
            return GR_SUCCESS;

        case GR_CTX_FMPQ:
            fmpq_set(res, x);
            return GR_SUCCESS;
    }

    return gr_get_fmpq(res, x, x_ctx);
}

int
_gr_fmpq_get_ui(ulong * res, const fmpq_t x, const gr_ctx_t ctx)
{
    if (!fmpz_is_one(fmpq_denref(x)))
        return GR_DOMAIN;

    if (fmpz_sgn(fmpq_numref(x)) < 0 || fmpz_cmp_ui(fmpq_numref(x), UWORD_MAX) > 0)
        return GR_DOMAIN;

    *res = fmpz_get_ui(fmpq_numref(x));
    return GR_SUCCESS;
}

int
_gr_fmpq_get_si(slong * res, const fmpq_t x, const gr_ctx_t ctx)
{
    if (!fmpz_is_one(fmpq_denref(x)))
        return GR_DOMAIN;

    if (!fmpz_fits_si(fmpq_numref(x)))
        return GR_DOMAIN;

    *res = fmpz_get_si(fmpq_numref(x));
    return GR_SUCCESS;
}

int
_gr_fmpq_get_fmpz(fmpz_t res, const fmpq_t x, const gr_ctx_t ctx)
{
    if (!fmpz_is_one(fmpq_denref(x)))
        return GR_DOMAIN;

    fmpz_set(res, fmpq_numref(x));
    return GR_SUCCESS;
}

int
_gr_fmpq_get_d(double * res, const fmpq_t x, const gr_ctx_t ctx)
{
    *res = fmpq_get_d(x);
    return GR_SUCCESS;
}

truth_t
_gr_fmpq_is_zero(const fmpq_t x, const gr_ctx_t ctx)
{
    return fmpq_is_zero(x) ? T_TRUE : T_FALSE;
}

truth_t
_gr_fmpq_is_one(const fmpq_t x, const gr_ctx_t ctx)
{
    return fmpq_is_one(x) ? T_TRUE : T_FALSE;
}

truth_t
_gr_fmpq_is_neg_one(const fmpq_t x, const gr_ctx_t ctx)
{
    return ((*fmpq_numref(x) == -1) && fmpz_is_one(fmpq_denref(x))) ? T_TRUE : T_FALSE;
}

truth_t
_gr_fmpq_equal(const fmpq_t x, const fmpq_t y, const gr_ctx_t ctx)
{
    return fmpq_equal(x, y) ? T_TRUE : T_FALSE;
}

int
_gr_fmpq_set(fmpq_t res, const fmpq_t x, const gr_ctx_t ctx)
{
    fmpq_set(res, x);
    return GR_SUCCESS;
}

int
_gr_fmpq_neg(fmpq_t res, const fmpq_t x, const gr_ctx_t ctx)
{
    fmpq_neg(res, x);
    return GR_SUCCESS;
}

int
_gr_fmpq_add(fmpq_t res, const fmpq_t x, const fmpq_t y, const gr_ctx_t ctx)
{
    fmpq_add(res, x, y);
    return GR_SUCCESS;
}

int
_gr_fmpq_add_si(fmpq_t res, const fmpq_t x, slong y, const gr_ctx_t ctx)
{
    fmpq_add_si(res, x, y);
    return GR_SUCCESS;
}

int
_gr_fmpq_add_ui(fmpq_t res, const fmpq_t x, ulong y, const gr_ctx_t ctx)
{
    fmpq_add_ui(res, x, y);
    return GR_SUCCESS;
}

int
_gr_fmpq_sub(fmpq_t res, const fmpq_t x, const fmpq_t y, const gr_ctx_t ctx)
{
    fmpq_sub(res, x, y);
    return GR_SUCCESS;
}

int
_gr_fmpq_mul(fmpq_t res, const fmpq_t x, const fmpq_t y, const gr_ctx_t ctx)
{
    fmpq_mul(res, x, y);
    return GR_SUCCESS;
}

int
_gr_fmpq_mul_si(fmpq_t res, const fmpq_t x, slong y, const gr_ctx_t ctx)
{
    fmpq_mul_si(res, x, y);
    return GR_SUCCESS;
}

int
_gr_fmpq_inv(fmpq_t res, const fmpq_t x, const gr_ctx_t ctx)
{
    if (fmpq_is_zero(x))
    {
        return GR_DOMAIN;
    }
    else
    {
        fmpq_inv(res, x);
        return GR_SUCCESS;
    }
}

int
_gr_fmpq_div(fmpq_t res, const fmpq_t x, const fmpq_t y, const gr_ctx_t ctx)
{
    if (fmpq_is_zero(y))
    {
        return GR_DOMAIN;
    }
    else
    {
        fmpq_div(res, x, y);
        return GR_SUCCESS;
    }
}

truth_t
_gr_fmpq_is_invertible(const fmpq_t x, const gr_ctx_t ctx)
{
    return (!fmpq_is_zero(x)) ? T_TRUE : T_FALSE;
}

int
_gr_fmpq_pow_ui(fmpq_t res, const fmpq_t x, ulong exp, const gr_ctx_t ctx)
{
    if (exp > (ulong) WORD_MAX || exp >= ctx->size_limit)  /* todo: systematic size solution for test code */
    {
        return GR_UNABLE;
    }
    else
    {
        fmpq_pow_si(res, x, exp);
        return GR_SUCCESS;
    }
}

int
_gr_fmpq_pow_si(fmpq_t res, const fmpq_t x, slong exp, const gr_ctx_t ctx)
{
    if (fmpq_is_one(x))
    {
        fmpq_one(res);
        return GR_SUCCESS;
    }
    else if (fmpz_equal_si(fmpq_numref(x), -1) && fmpz_is_one(fmpq_denref(x)))
    {
        if (exp % 2)
            fmpq_set_si(res, -1, 1);
        else
            fmpq_one(res);
        return GR_SUCCESS;
    }
    else if (fmpq_is_zero(x))
    {
        if (exp > 0)
        {
            fmpq_zero(res);
            return GR_SUCCESS;
        }
        else
        {
            return GR_DOMAIN;
        }
    }
    else
    {
        fmpq_pow_si(res, x, exp);
        return GR_SUCCESS;
    }
}

int
_gr_fmpq_pow_fmpz(fmpq_t res, const fmpq_t x, const fmpz_t exp, const gr_ctx_t ctx)
{
    if (!COEFF_IS_MPZ(*exp))
    {
        return _gr_fmpq_pow_si(res, x, *exp, ctx);
    }
    else if (fmpq_is_one(x))
    {
        fmpq_one(res);
        return GR_SUCCESS;
    }
    else if (fmpz_equal_si(fmpq_numref(x), -1) && fmpz_is_one(fmpq_denref(x)))
    {
        if (fmpz_is_odd(exp))
            fmpq_set_si(res, -1, 1);
        else
            fmpq_one(res);
        return GR_SUCCESS;
    }
    else if (fmpq_is_zero(x))
    {
        if (fmpz_sgn(exp) > 0)
        {
            fmpq_zero(res);
            return GR_SUCCESS;
        }
        else
        {
            return GR_DOMAIN;
        }
    }
    else
    {
        return GR_UNABLE;
    }
}

/* TODO: exploit fmpz_root return value in qqbar nth root as well */
int
_gr_fmpq_pow_fmpq(fmpq_t res, const fmpq_t x, const fmpq_t exp, const gr_ctx_t ctx)
{
    if (fmpz_is_one(fmpq_denref(exp)))
    {
        return _gr_fmpq_pow_fmpz(res, x, fmpq_numref(exp), ctx);
    }
    else if (fmpq_is_one(x) || fmpq_is_zero(exp))
    {
        return _gr_fmpq_one(res, ctx);
    }
    else if (fmpq_is_zero(x))
    {
        if (fmpq_sgn(exp) > 0)
            return _gr_fmpq_zero(res, ctx);
        else
            return GR_DOMAIN;
    }
    else if (COEFF_IS_MPZ(*fmpq_denref(exp)))
    {
        return GR_UNABLE;
    }
    else if (fmpq_sgn(x) < 0 && fmpz_is_even(fmpq_denref(exp)))
    {
        return GR_DOMAIN;
    }
    else
    {
        int status;
        ulong n;

        fmpq_t t;
        fmpq_init(t);

        n = *fmpq_denref(exp);

        if (fmpz_root(fmpq_numref(t), fmpq_numref(x), n) && fmpz_root(fmpq_denref(t), fmpq_denref(x), n))
            status = _gr_fmpq_pow_fmpz(res, t, fmpq_numref(exp), ctx);
        else
            status = GR_DOMAIN;

        fmpq_clear(t);
        return status;
    }
}


truth_t
_gr_fmpq_is_square(const fmpq_t x, const gr_ctx_t ctx)
{
    return (fmpz_is_square(fmpq_numref(x)) && fmpz_is_square(fmpq_denref(x))) ? T_TRUE : T_FALSE;
}

int
_gr_fmpq_sqrt(fmpq_t res, const fmpq_t x, const gr_ctx_t ctx)
{
    if (fmpq_sgn(x) < 0)
        return GR_DOMAIN;


    if (fmpz_root(fmpq_numref(res), fmpq_numref(x), 2) &&
        fmpz_root(fmpq_denref(res), fmpq_denref(x), 2))
    {
        return GR_SUCCESS;
    }
    else
    {
        return GR_DOMAIN;
    }
}

int
_gr_fmpq_rsqrt(fmpq_t res, const fmpq_t x, const gr_ctx_t ctx)
{
    if (fmpq_sgn(x) <= 0)
        return GR_DOMAIN;

    if (fmpz_root(fmpq_numref(res), fmpq_numref(x), 2) &&
        fmpz_root(fmpq_denref(res), fmpq_denref(x), 2))
    {
        fmpq_inv(res, res);
        return GR_SUCCESS;
    }
    else
    {
        return GR_DOMAIN;
    }
}

int
_gr_fmpq_floor(fmpq_t res, const fmpq_t x, const gr_ctx_t ctx)
{
    fmpz_fdiv_q(fmpq_numref(res), fmpq_numref(x), fmpq_denref(x));
    fmpz_one(fmpq_denref(res));
    return GR_SUCCESS;
}

int
_gr_fmpq_ceil(fmpq_t res, const fmpq_t x, const gr_ctx_t ctx)
{
    fmpz_cdiv_q(fmpq_numref(res), fmpq_numref(x), fmpq_denref(x));
    fmpz_one(fmpq_denref(res));
    return GR_SUCCESS;
}

int
_gr_fmpq_trunc(fmpq_t res, const fmpq_t x, const gr_ctx_t ctx)
{
    fmpz_tdiv_q(fmpq_numref(res), fmpq_numref(x), fmpq_denref(x));
    fmpz_one(fmpq_denref(res));
    return GR_SUCCESS;
}

int
_gr_fmpq_nint(fmpq_t res, const fmpq_t x, const gr_ctx_t ctx)
{
    if (fmpz_is_one(fmpq_denref(x)))
    {
        fmpq_set(res, x);
    }
    else
    {
        /* nint(x) = floor(x+0.5) - isint((2*x-1)/4) */
        fmpq_t t;
        fmpq_init(t);
        fmpq_set_si(t, 1, 2);
        fmpq_add(t, x, t);
        fmpz_fdiv_q(fmpq_numref(res), fmpq_numref(t), fmpq_denref(t));

        if (fmpz_is_one(fmpq_denref(t)) && fmpz_is_odd(fmpq_numref(t)))
            fmpq_sub_ui(res, res, 1);

        fmpq_clear(t);
    }

    return GR_SUCCESS;
}

int
_gr_fmpq_abs(fmpq_t res, const fmpq_t x, const gr_ctx_t ctx)
{
    fmpq_abs(res, x);
    return GR_SUCCESS;
}

int
_gr_fmpq_im(fmpq_t res, const fmpq_t x, const gr_ctx_t ctx)
{
    fmpq_zero(res);
    return GR_SUCCESS;
}

int
_gr_fmpq_sgn(fmpq_t res, const fmpq_t x, const gr_ctx_t ctx)
{
    fmpq_set_si(res, fmpq_sgn(x), 1);
    return GR_SUCCESS;
}

int
_gr_fmpq_cmp(int * res, const fmpq_t x, const fmpq_t y, const gr_ctx_t ctx)
{
    int cmp = fmpq_cmp(x, y);

    if (cmp < 0) cmp = -1;
    if (cmp > 0) cmp = 1;
    *res = cmp;
    return GR_SUCCESS;
}

/* todo: fast flint code */
int
_gr_fmpq_cmpabs(int * res, const fmpq_t x, const fmpq_t y, const gr_ctx_t ctx)
{
    fmpq_t t, u;
    int cmp;

    fmpq_init(t);
    fmpq_init(u);

    fmpq_abs(t, x);
    fmpq_abs(u, y);

    cmp = fmpq_cmp(t, u);

    fmpq_clear(t);
    fmpq_clear(u);

    if (cmp < 0) cmp = -1;
    if (cmp > 0) cmp = 1;
    *res = cmp;
    return GR_SUCCESS;
}

static int
_fmpq_vec_is_fmpz_vec(const fmpq * vec, slong len)
{
    slong i;
    for (i = 0; i < len; i++)
        if (!fmpz_is_one(fmpq_denref(vec + i)))
                return 0;
    return 1;
}

static void
_fmpq_vec_get_fmpz_vec_den(fmpz * c, fmpz_t den, const fmpq * vec, slong len)
{
    slong i;

    fmpz_one(den);

    if (_fmpq_vec_is_fmpz_vec(vec, len))
    {
        for (i = 0; i < len; i++)
            fmpz_set(c + i, fmpq_numref(vec + i));
    }
    else
    {
        for (i = 0; i < len; i++)
            fmpz_lcm(den, den, fmpq_denref(vec + i));

        for (i = 0; i < len; i++)
        {
            fmpz_divexact(c + i, den, fmpq_denref(vec + i));
            fmpz_mul(c + i, c + i, fmpq_numref(vec + i));
        }
    }
}

static void
_fmpq_vec_set_fmpz_vec_div_fmpz(fmpq * res, const fmpz * v, const fmpz_t den, slong len)
{
    slong i;

    if (fmpz_is_one(den))
    {
        for (i = 0; i < len; i++)
        {
            fmpz_set(fmpq_numref(res + i), v + i);
            fmpz_one(fmpq_denref(res + i));
        }
    }
    else
    {
        for (i = 0; i < len; i++)
            fmpq_set_fmpz_frac(res + i, v + i, den);
    }
}

void
_gr_fmpq_poly_mullow(fmpq * res,
    const fmpq * poly1, slong len1,
    const fmpq * poly2, slong len2, slong n, gr_ctx_t ctx)
{
    fmpz *z1, *z2, *z3;
    fmpz_t den1, den2;

    /* Todo: handle mixed cases */
    if (_fmpq_vec_is_fmpz_vec(poly1, len1) &&
        _fmpq_vec_is_fmpz_vec(poly2, len2))
    {
        slong i;

        z1 = _fmpz_vec_init(len1 + len2 + n);
        z2 = z1 + len1;
        z3 = z2 + len2;

        for (i = 0; i < len1; i++)
            z1[i] = *fmpq_numref(poly1 + i);
        for (i = 0; i < len2; i++)
            z2[i] = *fmpq_numref(poly2 + i);

        if (len1 >= len2)
            _fmpz_poly_mullow(z3, z1, len1, z2, len2, n);
        else
            _fmpz_poly_mullow(z3, z2, len2, z1, len1, n);

        for (i = 0; i < n; i++)
        {
            fmpz_one(fmpq_denref(res + i));
            fmpz_clear(fmpq_numref(res + i));
            *fmpq_numref(res + i) = z3[i];
        }

        flint_free(z1);
    }
    else
    {
        fmpz_init(den1);
        fmpz_init(den2);
        z1 = _fmpz_vec_init(len1 + len2 + n);
        z2 = z1 + len1;
        z3 = z2 + len2;

        _fmpq_vec_get_fmpz_vec_den(z1, den1, poly1, len1);
        _fmpq_vec_get_fmpz_vec_den(z2, den2, poly2, len2);

        fmpz_mul(den1, den1, den2);
        if (len1 >= len2)
            _fmpz_poly_mullow(z3, z1, len1, z2, len2, n);
        else
            _fmpz_poly_mullow(z3, z2, len2, z1, len1, n);

        _fmpq_vec_set_fmpz_vec_div_fmpz(res, z3, den1, n);

        _fmpz_vec_clear(z1, len1 + len2 + n);
        fmpz_clear(den1);
        fmpz_clear(den2);
    }
}

int
_gr_fmpq_mat_mul(fmpq_mat_t res, const fmpq_mat_t x, const fmpq_mat_t y, gr_ctx_t ctx)
{
    fmpq_mat_mul(res, x, y);
    return GR_SUCCESS;
}

int
_gr_fmpq_mat_det(fmpq_t res, const fmpq_mat_t x, const gr_ctx_t ctx)
{
    fmpq_mat_det(res, x);
    return GR_SUCCESS;
}


int _fmpq_methods_initialized = 0;

gr_static_method_table _fmpq_methods;

gr_method_tab_input _fmpq_methods_input[] =
{
    {GR_METHOD_CTX_WRITE,       (gr_funcptr) _gr_fmpq_ctx_write},
    {GR_METHOD_CTX_IS_RING,     (gr_funcptr) gr_generic_ctx_predicate_true},
    {GR_METHOD_CTX_IS_COMMUTATIVE_RING, (gr_funcptr) gr_generic_ctx_predicate_true},
    {GR_METHOD_CTX_IS_INTEGRAL_DOMAIN,  (gr_funcptr) gr_generic_ctx_predicate_true},
    {GR_METHOD_CTX_IS_FIELD,            (gr_funcptr) gr_generic_ctx_predicate_true},
    {GR_METHOD_CTX_IS_UNIQUE_FACTORIZATION_DOMAIN,
                                (gr_funcptr) gr_generic_ctx_predicate_true},
    {GR_METHOD_CTX_IS_FINITE,
                                (gr_funcptr) gr_generic_ctx_predicate_false},
    {GR_METHOD_CTX_IS_FINITE_CHARACTERISTIC,
                                (gr_funcptr) gr_generic_ctx_predicate_false},
    {GR_METHOD_CTX_IS_ALGEBRAICALLY_CLOSED,
                                (gr_funcptr) gr_generic_ctx_predicate_false},
    {GR_METHOD_CTX_IS_ORDERED_RING,
                                (gr_funcptr) gr_generic_ctx_predicate_true},
    {GR_METHOD_CTX_IS_EXACT,    (gr_funcptr) gr_generic_ctx_predicate_true},
    {GR_METHOD_CTX_IS_CANONICAL,
                                (gr_funcptr) gr_generic_ctx_predicate_true},
    {GR_METHOD_INIT,            (gr_funcptr) _gr_fmpq_init},
    {GR_METHOD_CLEAR,           (gr_funcptr) _gr_fmpq_clear},
    {GR_METHOD_SWAP,            (gr_funcptr) _gr_fmpq_swap},
    {GR_METHOD_RANDTEST,        (gr_funcptr) _gr_fmpq_randtest},
    {GR_METHOD_WRITE,           (gr_funcptr) _gr_fmpq_write},
    {GR_METHOD_ZERO,            (gr_funcptr) _gr_fmpq_zero},
    {GR_METHOD_ONE,             (gr_funcptr) _gr_fmpq_one},
    {GR_METHOD_IS_ZERO,         (gr_funcptr) _gr_fmpq_is_zero},
    {GR_METHOD_IS_ONE,          (gr_funcptr) _gr_fmpq_is_one},
    {GR_METHOD_IS_NEG_ONE,      (gr_funcptr) _gr_fmpq_is_neg_one},
    {GR_METHOD_EQUAL,           (gr_funcptr) _gr_fmpq_equal},
    {GR_METHOD_SET,             (gr_funcptr) _gr_fmpq_set},
    {GR_METHOD_SET_SI,          (gr_funcptr) _gr_fmpq_set_si},
    {GR_METHOD_SET_UI,          (gr_funcptr) _gr_fmpq_set_ui},
    {GR_METHOD_SET_FMPZ,        (gr_funcptr) _gr_fmpq_set_fmpz},
    {GR_METHOD_SET_FMPQ,        (gr_funcptr) _gr_fmpq_set},
    {GR_METHOD_SET_OTHER,       (gr_funcptr) _gr_fmpq_set_other},
    {GR_METHOD_GET_SI,          (gr_funcptr) _gr_fmpq_get_si},
    {GR_METHOD_GET_UI,          (gr_funcptr) _gr_fmpq_get_ui},
    {GR_METHOD_GET_FMPZ,        (gr_funcptr) _gr_fmpq_get_fmpz},
    {GR_METHOD_GET_FMPQ,        (gr_funcptr) _gr_fmpq_set},
    {GR_METHOD_GET_D,           (gr_funcptr) _gr_fmpq_get_d},
    {GR_METHOD_NEG,             (gr_funcptr) _gr_fmpq_neg},
    {GR_METHOD_ADD,             (gr_funcptr) _gr_fmpq_add},
    {GR_METHOD_ADD_UI,          (gr_funcptr) _gr_fmpq_add_ui},
    {GR_METHOD_ADD_SI,          (gr_funcptr) _gr_fmpq_add_si},
    {GR_METHOD_SUB,             (gr_funcptr) _gr_fmpq_sub},
    {GR_METHOD_MUL,             (gr_funcptr) _gr_fmpq_mul},
    {GR_METHOD_MUL_SI,          (gr_funcptr) _gr_fmpq_mul_si},
    {GR_METHOD_DIV,             (gr_funcptr) _gr_fmpq_div},
    {GR_METHOD_IS_INVERTIBLE,   (gr_funcptr) _gr_fmpq_is_invertible},
    {GR_METHOD_INV,             (gr_funcptr) _gr_fmpq_inv},
    {GR_METHOD_POW_UI,          (gr_funcptr) _gr_fmpq_pow_ui},
    {GR_METHOD_POW_SI,          (gr_funcptr) _gr_fmpq_pow_ui},
    {GR_METHOD_POW_FMPZ,        (gr_funcptr) _gr_fmpq_pow_fmpz},
    {GR_METHOD_POW_FMPQ,        (gr_funcptr) _gr_fmpq_pow_fmpq},
    {GR_METHOD_POW,             (gr_funcptr) _gr_fmpq_pow_fmpq},
    {GR_METHOD_IS_SQUARE,       (gr_funcptr) _gr_fmpq_is_square},
    {GR_METHOD_SQRT,            (gr_funcptr) _gr_fmpq_sqrt},
    {GR_METHOD_RSQRT,           (gr_funcptr) _gr_fmpq_rsqrt},
    {GR_METHOD_FLOOR,           (gr_funcptr) _gr_fmpq_floor},
    {GR_METHOD_CEIL,            (gr_funcptr) _gr_fmpq_ceil},
    {GR_METHOD_TRUNC,           (gr_funcptr) _gr_fmpq_trunc},
    {GR_METHOD_NINT,            (gr_funcptr) _gr_fmpq_nint},
    {GR_METHOD_I,               (gr_funcptr) gr_not_in_domain},
    {GR_METHOD_PI,              (gr_funcptr) gr_not_in_domain},
    {GR_METHOD_ABS,             (gr_funcptr) _gr_fmpq_abs},
    {GR_METHOD_CONJ,            (gr_funcptr) _gr_fmpq_set},
    {GR_METHOD_RE,              (gr_funcptr) _gr_fmpq_set},
    {GR_METHOD_IM,              (gr_funcptr) _gr_fmpq_im},
    {GR_METHOD_SGN,             (gr_funcptr) _gr_fmpq_sgn},
    {GR_METHOD_CSGN,            (gr_funcptr) _gr_fmpq_sgn},
    {GR_METHOD_CMP,             (gr_funcptr) _gr_fmpq_cmp},
    {GR_METHOD_CMPABS,          (gr_funcptr) _gr_fmpq_cmpabs},
    {GR_METHOD_POLY_MULLOW,     (gr_funcptr) _gr_fmpq_poly_mullow},
    {GR_METHOD_MAT_MUL,         (gr_funcptr) _gr_fmpq_mat_mul},
    {GR_METHOD_MAT_DET,         (gr_funcptr) _gr_fmpq_mat_det},
    {0,                         (gr_funcptr) NULL},
};

void
gr_ctx_init_fmpq(gr_ctx_t ctx)
{
    ctx->which_ring = GR_CTX_FMPQ;
    ctx->sizeof_elem = sizeof(fmpq);
    ctx->size_limit = WORD_MAX;

    ctx->methods = _fmpq_methods;

    if (!_fmpq_methods_initialized)
    {
        gr_method_tab_init(_fmpq_methods, _fmpq_methods_input);
        _fmpq_methods_initialized = 1;
    }
}
