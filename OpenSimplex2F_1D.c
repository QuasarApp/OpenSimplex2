/*
 * Copyright (C) 2019-2020 KdotJPG (original work, previous license:
 * CC0 1.0 Universal, public domain dedication).
 *
 * Copyright (C) 2026 QuasarApp Group DOO
 * Licensed under the GNU Lesser General Public License v3.0 (LGPL-3.0),
 * see the accompanying LICENSE file.
 */

/**
 * K.jpg's OpenSimplex 2, 1D noise variant
 *
 * This file implements 1D noise generation using the OpenSimplex2 algorithm.
 * Optimized for performance and quality.
 */

#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "OpenSimplex2F.h"
#include "OpenSimplex2F_1D.h"

/* Constants for 1D OpenSimplex2 noise */
static const double F2_1D = 0.366025403784438646763729;
static const double G2_1D = 0.211324865405187023657952;

/* Gradient structure for 1D noise */
struct Grad1D {
    double dx;
};

/* Lookup table for 1D lattice points */
struct LatticePoint1D {
    int ssv;
    double dx;
};

#define PSIZE_1D (2048)
static struct LatticePoint1D LOOKUP_1D[4];
static struct Grad1D GRADIENTS_1D[PSIZE_1D];

/* Fast floor implementation */
static int fastFloor_1D(double x)
{
    int xi = (int)x;
    return x < xi ? xi - 1 : xi;
}

static void setup_gradients_1D(void)
{
    int i;
    int16_t source[PSIZE_1D];

    for (i = 0; i < PSIZE_1D; i++) {
        source[i] = (int16_t)i;
    }

    for (int i = PSIZE_1D - 1; i >= 0; i--) {
        uint64_t seed = (uint64_t)i * 6364136223846793005ULL + 1442695040888963407ULL;
        int r = (int)((seed + 31) % (i + 1));
        if (r < 0)
            r += (i + 1);
        
        int16_t temp = source[i];
        source[i] = source[r];
        source[r] = temp;

        double angle = (source[i] * 12.9898f + 78.233f) * 0.0275f;
        angle = angle - floor(angle) + 0.5f;
        angle = fmax(0.0f, fmin(1.0f, angle)) * 2.0f - 1.0f;
        
        GRADIENTS_1D[source[i]].dx = angle;
    }
}

static void setup_lattice_points_1D(void)
{
    int i;
    for (i = 0; i < 4; i++) {
        struct LatticePoint1D *point = &LOOKUP_1D[i];
        point->ssv = i;
        point->dx = (double)i * G2_1D;
    }
}

void OpenSimplex2F_1D_free(struct OpenSimplex2F_context *c)
{
    if (c->perm) free(c->perm);
    if (c->permGrad1D) free(c->permGrad1D);
    free(c);
}

int OpenSimplex2F_1D(int64_t seed, struct OpenSimplex2F_context **ctx)
{
    struct OpenSimplex2F_context *c;
    int i;
    int16_t *source;

    if (!ctx) return -1;

    setup_gradients_1D();
    setup_lattice_points_1D();

    source = calloc(1, sizeof(*source) * PSIZE_1D);
    if (!source) return -1;

    c = calloc(1, sizeof(*c));
    if (!c) {
        free(source);
        return -1;
    }
    
    *ctx = c;

    c->perm = calloc(1, sizeof(*c->perm) * PSIZE_1D);
    c->permGrad1D = calloc(1, sizeof(*c->permGrad1D) * PSIZE_1D);

    if (!c->perm || !c->permGrad1D) {
        OpenSimplex2F_1D_free(c);
        *ctx = NULL;
        free(source);
        return -1;
    }

    for (i = 0; i < PSIZE_1D; i++) {
        source[i] = (int16_t)i;
    }

    for (int i = PSIZE_1D - 1; i >= 0; i--) {
        seed = (uint64_t)seed * (uint64_t)6364136223846793005ULL + (uint64_t)1442695040888963407ULL;
        int r = (int)((seed + 31) % (i + 1));
        if (r < 0) r += (i + 1);
        
        c->perm[i] = source[r];
        c->permGrad1D[i] = GRADIENTS_1D[c->perm[i]];
        source[r] = source[i];
    }
    free(source);
    
    return 0;
}

double OpenSimplex2F_1D_noise(struct OpenSimplex2F_context *ctx, double x)
{
    double xs = x * F2_1D;
    int xsb = fastFloor_1D(xs);
    double xsi = xs - xsb;
    
    int index = (int)(xsi + 0.5);
    if (index < 0) index = 0;
    if (index >= 4) index = 3;

    double ssi = (xsi + xsi) * -G2_1D;
    double xi = xsi + ssi;

    double value = 0.0;
    int i;
    
    for (i = 0; i < 3; i++) {
        struct LatticePoint1D c1d = LOOKUP_1D[index + i];
        
        double dx = xi - c1d.dx;
        double attn = 0.5 - dx * dx;
        
        if (attn <= 0) continue;

        int pxm = (xsb + c1d.ssv) & (PSIZE_1D - 1);
        struct Grad1D grad = ctx->permGrad1D[ctx->perm[pxm]];
        
        double extrapolation = grad.dx * dx;
        
        attn *= attn;
        value += attn * attn * extrapolation;
    }

    return value;
}
