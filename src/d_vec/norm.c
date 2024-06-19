/*
    Copyright (C) 2010 William Hart
    Copyright (C) 2010 Fredrik Johansson
    Copyright (C) 2014 Abhinav Baid

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "d_vec.h"

double
_d_vec_norm(const double *vec, slong len)
{
    double sum = 0;
    slong i;

    for (i = 0; i < len; i++)
        sum += vec[i] * vec[i];

    return sum;
}
