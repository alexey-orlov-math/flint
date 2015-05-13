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

    Copyright (C) 2015 Kushagra Singh

******************************************************************************/

#include <gmp.h>
#include "flint.h"
#include "fmpz.h"

/* Sets x to (x^2 + a) % n */

void
sqr_and_add_a(fmpz_t x, fmpz_t a, const fmpz_t n)
{
    fmpz_mul(x, x, x);
    fmpz_mod(x, x, n);    
    fmpz_add(x, x, a);
    fmpz_mod(x, x, n);
}

int
fmpz_factor_pollard_brent_single(fmpz_t p_factor, const fmpz_t n, const fmpz_t ai, 
                           const fmpz_t xi, mp_limb_t max_iters)
{
    fmpz_t x, y, a, q, gcdval, ys, subval;
    mp_limb_t iter, i, k, j, minval, m;
    int ret;

    fmpz_init(x);       /* Initial values, set as random */
    fmpz_init(y);       
    fmpz_init(a);       
    fmpz_init(ys);      /* Used for backtracking */
    fmpz_init(q);       /* stores product of gcd values */
    fmpz_init(subval);
    fmpz_init(gcdval);
        
    fmpz_set(y, xi);
    fmpz_set(a, ai);
    fmpz_set_ui(q, 1);
    fmpz_set_ui(gcdval, 1);

    m = 100;
    iter = 1;

    do {
        fmpz_set(x, y);
        k = 0;

        for (i = 0; i < iter; i++)
            sqr_and_add_a(y, a, n);

        do {
            minval = iter - k;  /* minval = min(m, iter - k) */
            if (m < minval)
                minval = m;

            fmpz_set(ys, y);

            for (i = 0; i < minval; i++)
            {
                sqr_and_add_a(y, a, n);
                fmpz_sub(subval, x, y);
                fmpz_mul(q, q, subval);
                fmpz_mod(q, q, n);
            }
            if (!fmpz_cmp_ui(q, 0))
            {
                ret = 0;
                goto cleanup;
            }
            fmpz_gcd(gcdval, q, n);
            k += m;
            j = fmpz_is_one(gcdval);
        } while ((k < iter) && j);

        if (iter > max_iters)
            break;

        iter *= 2;
    } while (j);                /* while gcdval == 1 */

    if (fmpz_equal(gcdval, n))
    {
        do {
            sqr_and_add_a(ys, a, n);
            fmpz_sub(subval, x, ys);
            fmpz_gcd(gcdval, subval, n);
        } while (fmpz_is_one(gcdval));
    }

    if (!fmpz_cmp(gcdval, n) || fmpz_is_one(gcdval))
        ret = 0;
    else
        ret = 1;
    
    fmpz_set(p_factor, gcdval);

    cleanup:

    fmpz_clear(x);
    fmpz_clear(y);
    fmpz_clear(ys);
    fmpz_clear(a);
    fmpz_clear(subval);
    fmpz_clear(q);
    fmpz_clear(gcdval);

    return ret;
}

int
fmpz_factor_pollard_brent(fmpz_t p_factor, flint_rand_t state, const fmpz_t n, 
                        mp_limb_t max_tries, mp_limb_t max_iters)
{

    fmpz_t a, x, maxa, maxx;
    int ret;

    fmpz_init(a);
    fmpz_init(x);
    fmpz_init(maxa);
    fmpz_init(maxx);
    ret = 0;

    fmpz_sub_ui(maxa, n, 3);
    fmpz_sub_ui(maxx, n, 1);

    while (max_tries--)
    {
        fmpz_randm(a, state, maxa);  
        fmpz_add_ui(a, a, 1);
        fmpz_randm(x, state, maxx);
        fmpz_add_ui(x, x, 1);

        ret = fmpz_factor_pollard_brent_single(p_factor, n, a, x, max_iters);

        if (ret == 1)
            break;
    }

    fmpz_clear(a);
    fmpz_clear(x);
    fmpz_clear(maxa);
    fmpz_clear(maxx);

    return ret;
}