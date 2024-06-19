/*
    Copyright (C) 2010, 2012 William Hart
    Copyright (C) 2011, 2012 Fredrik Johansson
    Copyright (C) 2013 Mike Hansen

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "fq_nmod.h"
#include "fq_nmod_vec.h"
#include "fq_nmod_poly.h"

#ifdef T
#undef T
#endif

#define T fq_nmod
#define CAP_T FQ_NMOD
#include "fq_poly_templates/test/t-evaluate_fq_vec_fast.c"
#undef CAP_T
#undef T
