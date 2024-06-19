/*
    Copyright (C) 2009 William Hart

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "test_helpers.h"
#include "ulong_extras.h"
#include "fmpz.h"

TEST_FUNCTION_START(fmpz_set, state)
{
    int i, result;

    if (!FLINT_RAND_GMP_STATE_IS_INITIALISED(state))
        _flint_rand_init_gmp_state(state);

    for (i = 0; i < 10000 * flint_test_multiplier(); i++)
    {
        fmpz_t a, b;
        mpz_t c, d;
        flint_bitcnt_t bits;

        mpz_init(c);
        mpz_init(d);

        bits = n_randint(state, 200) + 1;

        mpz_rrandomb(c, state->__gmp_state, bits);

        if (n_randint(state, 2))
            mpz_neg(c, c);

        fmpz_init(a);
        fmpz_init(b);

        fmpz_set_mpz(a, c);
        fmpz_set(b, a);
        fmpz_get_mpz(d, b);

        result = (mpz_cmp(c, d) == 0) && _fmpz_is_canonical(b);
        if (!result)
        {
            flint_printf("FAIL:\n");
            gmp_printf("c = %Zd, d = %Zd\n", c, d);
            fflush(stdout);
            flint_abort();
        }

        fmpz_clear(a);
        fmpz_clear(b);

        mpz_clear(c);
        mpz_clear(d);
    }

    TEST_FUNCTION_END(state);
}
