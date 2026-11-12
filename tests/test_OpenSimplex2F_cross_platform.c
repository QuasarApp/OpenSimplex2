/*
 * Copyright (C) 2026 QuasarApp Group DOO
 * Licensed under the GNU Lesser General Public License v3.0 (LGPL-3.0),
 * see the accompanying LICENSE file.
 */

/*
 * Cross-platform benchmark for all OpenSimplex2F noise functions.
 *
 * Evaluates every public noise function (2D, 3D, 4D, all variants) with the
 * same seed and the same fixed input parameters on every platform, and
 * prints the exact IEEE-754 binary64 bit pattern (16 hex digits) of every
 * result. The report is plain, platform-independent ASCII text, so the
 * result files produced by the CI jobs on Linux, Windows and macOS can be
 * compared byte-for-byte: given the same input parameters the noise results
 * must match perfectly on all platforms (see
 * .github/workflows/cross-platform-compare.yml).
 *
 * The report is written to noise_benchmark_results.txt in the current
 * working directory (or to the path given as the first command line
 * argument, whose parent directory is created when missing) and printed
 * to stdout as well.
 *
 * Exit code: 0 if the report was written and every noise value is inside
 * the defined range [-1, 1], otherwise 1.
 */

#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
#include <direct.h>
#define MAKE_DIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define MAKE_DIR(path) mkdir((path), 0755)
#endif

#include "OpenSimplex2F.h"

#define BENCHMARK_SEED 77374LL /* Same seed as tests/test_OpenSimplex2F.c. */
#define RESULT_FILE "noise_benchmark_results.txt"

#define NOISE_LIMIT 1.0
#define NOISE_EPSILON 1e-6

/* FNV-1a 64-bit: one deterministic checksum summarizing the whole report. */
static uint64_t fnv1a64(uint64_t hash, const char *data, size_t length)
{
	size_t i;
	for (i = 0; i < length; i++)
		hash = (hash ^ (uint8_t) data[i]) * 0x100000001B3ULL;
	return hash;
}

/* Exact IEEE-754 binary64 representation of a double. All CI platforms
 * (x86-64 Linux, x86-64 Windows, arm64 macOS) use binary64 doubles, so the
 * bit pattern identifies the result unambiguously. */
static uint64_t double_bits(double value)
{
	uint64_t bits;
	memcpy(&bits, &value, sizeof(bits));
	return bits;
}

/* The report is assembled in a heap buffer so it can be written to stdout
 * and to the result file with exactly the same bytes. */
struct report {
	char *data;
	size_t length;
	size_t capacity;
	int failed;
};

static void report_put(struct report *r, const char *text, size_t length)
{
	if (r->failed)
		return;
	if (r->length + length + 1 > r->capacity) {
		size_t new_capacity = r->capacity ? r->capacity * 2 : (1u << 20);
		char *data;
		while (r->length + length + 1 > new_capacity)
			new_capacity *= 2;
		data = realloc(r->data, new_capacity);
		if (!data) {
			fprintf(stderr, "FAIL: out of memory\n");
			r->failed = 1;
			return;
		}
		r->data = data;
		r->capacity = new_capacity;
	}
	memcpy(r->data + r->length, text, length);
	r->length += length;
	r->data[r->length] = '\0';
}

static void report_printf(struct report *r, const char *format, ...)
{
	char line[256];
	va_list args;
	int length;

	va_start(args, format);
	length = vsnprintf(line, sizeof(line), format, args);
	va_end(args);

	if (length < 0) {
		fprintf(stderr, "FAIL: vsnprintf failed\n");
		r->failed = 1;
		return;
	}
	if ((size_t) length >= sizeof(line))
		length = (int) sizeof(line) - 1;
	report_put(r, line, (size_t) length);
}

/* Every noise function from the public header, grouped by dimension. */
typedef double (*noise2_fn)(struct OpenSimplex2F_context *, double, double);
typedef double (*noise3_fn)(struct OpenSimplex2F_context *, double, double, double);
typedef double (*noise4_fn)(struct OpenSimplex2F_context *, double, double, double, double);

static const struct {
	const char *name;
	noise2_fn fn;
} noise2_functions[] = {
	{ "OpenSimplex2F_noise2", OpenSimplex2F_noise2 },
	{ "OpenSimplex2F_noise2_XBeforeY", OpenSimplex2F_noise2_XBeforeY },
};

static const struct {
	const char *name;
	noise3_fn fn;
} noise3_functions[] = {
	{ "OpenSimplex2F_noise3_Classic", OpenSimplex2F_noise3_Classic },
	{ "OpenSimplex2F_noise3_XYBeforeZ", OpenSimplex2F_noise3_XYBeforeZ },
	{ "OpenSimplex2F_noise3_XZBeforeY", OpenSimplex2F_noise3_XZBeforeY },
};

static const struct {
	const char *name;
	noise4_fn fn;
} noise4_functions[] = {
	{ "OpenSimplex2F_noise4_Classic", OpenSimplex2F_noise4_Classic },
	{ "OpenSimplex2F_noise4_XYBeforeZW", OpenSimplex2F_noise4_XYBeforeZW },
	{ "OpenSimplex2F_noise4_XZBeforeYW", OpenSimplex2F_noise4_XZBeforeYW },
	{ "OpenSimplex2F_noise4_XYZBeforeW", OpenSimplex2F_noise4_XYZBeforeW },
};

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

/* The frequency scales multiply the base coordinates: they place the sample
 * points in different lattice cells, so every branch of the noise functions
 * is exercised. */
static const double scales[] = { 1.0, 2.3, 5.7 };

static int value_in_range(double value)
{
	return (value >= -NOISE_LIMIT - NOISE_EPSILON) &&
	       (value <= NOISE_LIMIT + NOISE_EPSILON);
}

static void report_value(struct report *r, const char *name, double scale,
                         double value, const char *coords)
{
	if (!value_in_range(value)) {
		fprintf(stderr, "FAIL: %s (scale=%f, %s) returned %f outside the expected range [-1, 1]\n",
		        name, scale, coords, value);
		r->failed = 1;
	}
	report_printf(r, "%s scale=%f %s bits=%016" PRIx64 "\n",
	              name, scale, coords, double_bits(value));
}

static void benchmark2(struct report *r, struct OpenSimplex2F_context *ctx)
{
	size_t s, j, i, f;
	for (s = 0; s < ARRAY_SIZE(scales); s++) {
		for (j = 0; j <= 16; j++) {
			for (i = 0; i <= 16; i++) {
				/* i / 8.0 - 1.0 is exact in binary64, so the
				 * input parameters are bit-identical on every
				 * platform. */
				double y = (double) j / 8.0 - 1.0;
				double sy = y * scales[s];
				for (f = 0; f < ARRAY_SIZE(noise2_functions); f++) {
					double x = (double) i / 8.0 - 1.0;
					double sx = x * scales[s];
					char coords[96];
					snprintf(coords, sizeof(coords), "x=%f y=%f", sx, sy);
					report_value(r, noise2_functions[f].name, scales[s],
					             noise2_functions[f].fn(ctx, sx, sy), coords);
				}
			}
		}
	}
}

static void benchmark3(struct report *r, struct OpenSimplex2F_context *ctx)
{
	size_t s, k, j, i, f;
	for (s = 0; s < ARRAY_SIZE(scales); s++) {
		for (k = 0; k <= 8; k++) {
			for (j = 0; j <= 8; j++) {
				for (i = 0; i <= 8; i++) {
					double z = (double) k / 8.0 - 1.0;
					double y = (double) j / 8.0 - 1.0;
					double x = (double) i / 8.0 - 1.0;
					double sz = z * scales[s];
					double sy = y * scales[s];
					double sx = x * scales[s];
					char coords[128];
					snprintf(coords, sizeof(coords), "x=%f y=%f z=%f", sx, sy, sz);
					for (f = 0; f < ARRAY_SIZE(noise3_functions); f++)
						report_value(r, noise3_functions[f].name, scales[s],
						             noise3_functions[f].fn(ctx, sx, sy, sz), coords);
				}
			}
		}
	}
}

static void benchmark4(struct report *r, struct OpenSimplex2F_context *ctx)
{
	size_t s, w, z, y, x, f;
	for (s = 0; s < ARRAY_SIZE(scales); s++) {
		for (w = 0; w <= 6; w++) {
			for (z = 0; z <= 6; z++) {
				for (y = 0; y <= 6; y++) {
					for (x = 0; x <= 6; x++) {
						double dw = (double) w / 4.0 - 1.0;
						double dz = (double) z / 4.0 - 1.0;
						double dy = (double) y / 4.0 - 1.0;
						double dx = (double) x / 4.0 - 1.0;
						double sw = dw * scales[s];
						double sz = dz * scales[s];
						double sy = dy * scales[s];
						double sx = dx * scales[s];
						char coords[160];
						snprintf(coords, sizeof(coords),
						         "x=%f y=%f z=%f w=%f", sx, sy, sz, sw);
						for (f = 0; f < ARRAY_SIZE(noise4_functions); f++)
							report_value(r, noise4_functions[f].name, scales[s],
							             noise4_functions[f].fn(ctx, sx, sy, sz, sw), coords);
					}
				}
			}
		}
	}
}

/* Create the parent directory of path when it is missing. */
static int make_parent_dir(const char *path)
{
	char *dir = strdup(path);
	char *slash;

	if (!dir)
		return -1;
	slash = strrchr(dir, '/');
#ifdef _WIN32
	if (!slash)
		slash = strrchr(dir, '\\');
#endif
	if (slash && *slash != '\0') {
		*slash = '\0';
		if (dir[0] != '\0' && MAKE_DIR(dir) != 0 && errno != EEXIST) {
			int e = errno;
			free(dir);
			errno = e;
			return -1;
		}
	} else if (!slash) {
		free(dir);
		return 0; /* no directory part, nothing to create */
	}
	free(dir);
	return 0;
}

int main(int argc, char *argv[])
{
	struct OpenSimplex2F_context *ctx = NULL;
	struct report report;
	const char *result_file = RESULT_FILE;
	uint64_t values2, values3, values4, values;
	uint64_t checksum;
	FILE *file;
	int result = 0;

	if (argc > 1 && argv[1][0] != '\0')
		result_file = argv[1];

	values2 = ARRAY_SIZE(noise2_functions) * 17u * 17u * ARRAY_SIZE(scales);
	values3 = ARRAY_SIZE(noise3_functions) * 9u * 9u * 9u * ARRAY_SIZE(scales);
	values4 = ARRAY_SIZE(noise4_functions) * 7u * 7u * 7u * 7u * ARRAY_SIZE(scales);
	values = values2 + values3 + values4;

	memset(&report, 0, sizeof(report));

	if (OpenSimplex2F(BENCHMARK_SEED, &ctx) != 0) {
		fprintf(stderr, "FAIL: OpenSimplex2F(%" PRId64 ") failed to create the context\n",
		        (int64_t) BENCHMARK_SEED);
		return 1;
	}

	report_printf(&report, "OpenSimplex2F cross-platform benchmark\n");
	report_printf(&report, "seed=%" PRId64 "\n", (int64_t) BENCHMARK_SEED);

	benchmark2(&report, ctx);
	benchmark3(&report, ctx);
	benchmark4(&report, ctx);
	report_printf(&report,
	              "values2=%" PRIu64 " values3=%" PRIu64 " values4=%" PRIu64
	              " values=%" PRIu64 "\n",
	              (uint64_t) values2, (uint64_t) values3, (uint64_t) values4,
	              (uint64_t) values);

	if (report.failed)
		result = 1;

	checksum = fnv1a64(0xcbf29ce484222325ULL, report.data, report.length);
	if (!result)
		report_printf(&report, "checksum=%016" PRIx64 "\n", checksum);

	/* Always print the full report so the CI log shows the results. */
	fputs(report.data, stdout);
	fflush(stdout);

	if (make_parent_dir(result_file) != 0) {
		fprintf(stderr, "FAIL: mkdir(parent of %s): %s\n", result_file, strerror(errno));
		result = 1;
	}

	file = fopen(result_file, "w");
	if (!file) {
		fprintf(stderr, "FAIL: fopen(%s): %s\n", result_file, strerror(errno));
		result = 1;
	} else {
		if (fwrite(report.data, 1, report.length, file) != report.length) {
			fprintf(stderr, "FAIL: write(%s): %s\n", result_file, strerror(errno));
			result = 1;
		}
		if (fclose(file) != 0) {
			fprintf(stderr, "FAIL: fclose(%s): %s\n", result_file, strerror(errno));
			result = 1;
		}
	}

	OpenSimplex2F_free(ctx);
	OpenSimplex2F_shutdown();
	free(report.data);

	printf("Result: %s (%" PRIu64 " values, seed %" PRId64 ")\n",
	       result ? "FAIL" : "PASS", (uint64_t) values, (int64_t) BENCHMARK_SEED);
	return result;
}

