/*
    Copyright (C) 2013, 2024 Fredrik Johansson

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "fmpz_poly.h"
#include "arb.h"
#include "hypgeom.h"

void
arb_const_log2_hypgeom_eval(arb_t s, slong prec)
{
    hypgeom_t series;
    arb_t t;

    arb_init(t);
    hypgeom_init(series);

    fmpz_poly_set_str(series->A, "2  1497 1794");
    fmpz_poly_set_str(series->B, "1  1");
    fmpz_poly_set_str(series->P, "3  0 -1 2");
    fmpz_poly_set_str(series->Q, "3  1080 7776 7776");

    prec += FLINT_CLOG2(prec);
    arb_hypgeom_infsum(s, t, series, prec, prec);
    arb_mul_ui(t, t, 2160, prec);
    arb_div(s, s, t, prec);

    hypgeom_clear(series);
    arb_clear(t);
}

/*
Note: we ought to share the log(2) cache with the log(p) cache.

Note: for log(3) the corresponding formula is

    fmpz_poly_set_str(series->A, "2  74 88");
    fmpz_poly_set_str(series->B, "1  1");
    fmpz_poly_set_str(series->P, "3  0 -2 4");
    fmpz_poly_set_str(series->Q, "3  135 972 972");
    ...
    arb_mul_2exp_si(s, s, 1);
    arb_mul_ui(t, t, 135, prec);

but currently we do not have a use for this as a standalone constant.
*/

ARB_DEF_CACHED_CONSTANT(arb_const_log2_hypgeom, arb_const_log2_hypgeom_eval)

void
arb_const_log2(arb_t res, slong prec)
{
    if (prec < ARB_LOG_TAB2_LIMBS * FLINT_BITS - 16)
    {
        slong exp;

        /* just reading the table is known to give the correct rounding */
        _arf_set_round_mpn(arb_midref(res), &exp, arb_log_log2_tab,
            ARB_LOG_TAB2_LIMBS, 0, prec, ARF_RND_NEAR);
        _fmpz_set_si_small(ARF_EXPREF(arb_midref(res)), exp);

        /* 1/2 ulp error */
        _fmpz_set_si_small(MAG_EXPREF(arb_radref(res)), exp - prec);
        MAG_MAN(arb_radref(res)) = MAG_ONE_HALF;
    }
    else
    {
        arb_const_log2_hypgeom(res, prec);
    }
}
