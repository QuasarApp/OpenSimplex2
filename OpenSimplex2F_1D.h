/*
 * Copyright (C) 2019-2020 KdotJPG (original work, previous license:
 * CC0 1.0 Universal, public domain dedication).
 *
 * Copyright (C) 2026 QuasarApp Group DOO
 * Licensed under the GNU Lesser General Public License v3.0 (LGPL-3.0),
 * see the accompanying LICENSE file.
 */

#ifndef OpenSimplex2F_1D_h__
#define OpenSimplex2F_1D_h__

/**
 * K.jpg's OpenSimplex 2, 1D noise variant
 *
 * This provides 1D noise generation using the OpenSimplex2 algorithm.
 * Optimized for performance and quality.
 *
 * Multiple versions of each function are provided. See the
 * documentation above each, for more info.
 *
 * Ported from Java to C by Stephen M. Cameron
 *
 */

#if ((__GNUC_STDC_INLINE__) || (__STDC_VERSION__ >= 199901L))
	#include <stdint.h>
	#define INLINE inline
#elif (defined (_MSC_VER) || defined (__GNUC_GNU_INLINE__))
	#include <stdint.h>
	#define INLINE __inline
#else
	/* ANSI C doesn't have inline or stdint.h. */
	#define INLINE
#endif

#include "OpenSimplex2F.h"

/* Gradient structure for 1D */
struct Grad1D {
    double dx;
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 1D Simplex noise function.
 *
 * @param seed Seed for pseudo-random number generator
 * @param ctx Pointer to context structure (will be allocated)
 * @return 0 on success, non-zero on error
 */
int OpenSimplex2F_1D(int64_t seed, struct OpenSimplex2F_context **ctx);

/**
 * Free 1D noise context.
 *
 * @param ctx OpenSimplex2F context to free
 */
void OpenSimplex2F_1D_free(struct OpenSimplex2F_context *ctx);

/**
 * 1D Simplex noise function.
 *
 * @param ctx OpenSimplex2F context (obtained from OpenSimplex2F_1D)
 * @param x Input coordinate
 * @return Noise value in range [-1, 1]
 *
 * @note This function provides smooth, continuous noise values suitable
 *       for terrain generation, procedural content, and other applications.
 */
double OpenSimplex2F_1D_noise(struct OpenSimplex2F_context *ctx, double x);

#ifdef __cplusplus
}
#endif

#endif
