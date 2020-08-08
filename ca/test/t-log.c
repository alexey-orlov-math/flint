/*
    Copyright (C) 2020 Fredrik Johansson

    This file is part of Calcium.

    Calcium is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#include "ca.h"

int main()
{
    slong iter;
    flint_rand_t state;

    flint_printf("log....");
    fflush(stdout);

    flint_randinit(state);

    for (iter = 0; iter < 1000 * calcium_test_multiplier(); iter++)
    {
        ca_ctx_t ctx;
        ca_t x, y, z, a, b, c, d, e, f;
        truth_t equal, zero;

        ca_ctx_init(ctx);
        ca_init(x, ctx);
        ca_init(y, ctx);
        ca_init(z, ctx);
        ca_init(a, ctx);
        ca_init(b, ctx);
        ca_init(c, ctx);
        ca_init(d, ctx);
        ca_init(e, ctx);
        ca_init(f, ctx);

        /* log(x * y * z) - log(x) - log(y) - log(z) */
        /* no test implemented yet; just verifying that the code runs */

        ca_randtest(x, state, 5, 5, ctx);
        ca_randtest(y, state, 5, 5, ctx);
        ca_randtest(z, state, 5, 5, ctx);
        ca_abs(x, x, ctx);
        ca_abs(y, y, ctx);
        ca_abs(y, y, ctx);

        ca_mul(a, x, y, ctx);
        ca_mul(a, a, z, ctx);
        ca_log(a, a, ctx);

        ca_log(b, x, ctx);
        ca_log(c, y, ctx);
        ca_log(d, z, ctx);

        ca_add(e, b, c, ctx);
        ca_add(e, e, d, ctx);

        ca_sub(f, a, e, ctx);

        equal = ca_check_equal(a, e, ctx);

        if (ca_check_is_infinity(a, ctx) == T_FALSE)
        {
            zero = ca_check_is_zero(f, ctx);
        }
        else
        {
            zero = T_UNKNOWN;
        }

        if (equal == T_FALSE || zero == T_FALSE)
        {
            flint_printf("FAIL (og(x * y * z) - log(x) - log(y) - log(z) != 0)\n\n");
            flint_printf("x = "); ca_print(x, ctx); flint_printf(" ~= "); ca_printn(x, 10, ARB_STR_NO_RADIUS, ctx); flint_printf("\n\n");
            flint_printf("y = "); ca_print(y, ctx); flint_printf(" ~= "); ca_printn(y, 10, ARB_STR_NO_RADIUS, ctx); flint_printf("\n\n");
            flint_printf("z = "); ca_print(z, ctx); flint_printf(" ~= "); ca_printn(z, 10, ARB_STR_NO_RADIUS, ctx); flint_printf("\n\n");
            flint_printf("a = "); ca_print(a, ctx); flint_printf(" ~= "); ca_printn(a, 10, ARB_STR_NO_RADIUS, ctx); flint_printf("\n\n");
            flint_printf("b = "); ca_print(b, ctx); flint_printf(" ~= "); ca_printn(b, 10, ARB_STR_NO_RADIUS, ctx); flint_printf("\n\n");
            flint_printf("c = "); ca_print(c, ctx); flint_printf(" ~= "); ca_printn(c, 10, ARB_STR_NO_RADIUS, ctx); flint_printf("\n\n");
            flint_printf("d = "); ca_print(d, ctx); flint_printf(" ~= "); ca_printn(d, 10, ARB_STR_NO_RADIUS, ctx); flint_printf("\n\n");
            flint_printf("e = "); ca_print(e, ctx); flint_printf(" ~= "); ca_printn(e, 10, ARB_STR_NO_RADIUS, ctx); flint_printf("\n\n");
            flint_printf("f = "); ca_print(f, ctx); flint_printf(" ~= "); ca_printn(f, 10, ARB_STR_NO_RADIUS, ctx); flint_printf("\n\n");
            flint_abort();
        }

        ca_clear(x, ctx);
        ca_clear(y, ctx);
        ca_clear(z, ctx);
        ca_clear(a, ctx);
        ca_clear(b, ctx);
        ca_clear(c, ctx);
        ca_clear(d, ctx);
        ca_clear(e, ctx);
        ca_clear(f, ctx);
        ca_ctx_clear(ctx);
    }

    flint_randclear(state);
    flint_cleanup();
    flint_printf("PASS\n");
    return EXIT_SUCCESS;
}

