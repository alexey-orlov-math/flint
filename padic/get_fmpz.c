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

    Copyright (C) 2011 Sebastian Pancratz
 
******************************************************************************/

#include "padic.h"

void _padic_get_fmpz(fmpz_t rop, const padic_t op, const padic_ctx_t ctx)
{
    if (fmpz_is_zero(padic_unit(op)) || (padic_val(op) < 0))
    {
        fmpz_zero(rop);
    }
    else
    {
        if (padic_val(op) == 0)
        {
            fmpz_set(rop, padic_unit(op));
        }
        else  /* (padic_val(op) > 0) */
        {
            fmpz_pow_ui(rop, ctx->p, padic_val(op));
            fmpz_mul(rop, rop, padic_unit(op));
        }
    }
}

void padic_get_fmpz(fmpz_t rop, const padic_t op, const padic_ctx_t ctx)
{
    if (padic_is_zero(op, ctx) || (padic_val(op) < 0))
    {
        fmpz_zero(rop);
    }
    else
    {
        fmpz_t pow;
        int alloc = 0;

        _padic_ctx_pow_ui(pow, &alloc, ctx->N - padic_val(op), ctx);
        fmpz_mod(rop, padic_unit(op), pow);

        if (padic_val(op) > 0)
        {
            if (alloc)
                fmpz_clear(pow);
            _padic_ctx_pow_ui(pow, &alloc, padic_val(op), ctx);
            fmpz_mul(rop, rop, pow);
        }

        if (alloc)
            fmpz_clear(pow);
    }
}

