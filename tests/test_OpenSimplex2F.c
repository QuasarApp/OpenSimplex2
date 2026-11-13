/*
 * Copyright (C) 2026 QuasarApp Group DOO
 * Licensed under the GNU Lesser General Public License v3.0 (LGPL-3.0),
 * see the accompanying LICENSE file.
 *
 * Main test executable of the OpenSimplex2 project.
 *
 * Every test is implemented in its own function:
 *   - test_noise2()  2D noise:  OpenSimplex2F_noise2, noise2_XBeforeY
 *   - test_noise3()  3D noise:  noise3_Classic, noise3_XYBeforeZ, noise3_XZBeforeY
 *   - test_noise4()  4D noise:  noise4_Classic (3-octave blend),
 *                               noise4_XYBeforeZW, noise4_XZBeforeYW,
 *                               noise4_XYZBeforeW
 *
 * main() only selects and runs the tests: with no argument it runs all of
 * them, with one argument it runs the test with that name only. The tests
 * are registered in the CTest framework (see tests/CMakeLists.txt), so each
 * of them can also be run separately, e.g.:  ctest -R test_noise2
 */

#include <stdio.h>
#if ((__STDC_VERSION__ >= 199901L) || (_MSC_VER))
	#include <stdint.h>
#endif
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#include <png.h>

#include "OpenSimplex2F.h"

#define WIDTH 512
#define HEIGHT 512
#define FEATURE_SIZE 24

/* All OpenSimplex2F variants are defined to return values in [-1, 1].
 * The 3-octave blend of the 4D image is a convex combination of the same
 * functions, so it stays in the same range. A small tolerance covers
 * floating point rounding. */
#define NOISE_LIMIT 1.0
#define NOISE_EPSILON 1e-6

/* Seed shared by every test, keeps the results reproducible. */
#define TEST_SEED 77374

/* z/w offsets used when range-checking the 3D/4D variants off the image
 * plane, so the extra dimensions are actually exercised. */
#define Z_OFFSET 0.37
#define W_OFFSET 0.71

static int write_png_image(const char *filename, unsigned char *pixels, int w, int h, int has_alpha)
{
	png_structp png_ptr;
	png_infop info_ptr;
	png_byte **row;
	int x, y, rc, colordepth = 8;
	int bytes_per_pixel = has_alpha ? 4 : 3;
	FILE *f;

	f = fopen(filename, "w");
	if (!f) {
		fprintf(stderr, "fopen: %s:%s\n", filename, strerror(errno));
		return -1;
	}
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
		goto cleanup1;
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
		goto cleanup2;
	if (setjmp(png_jmpbuf(png_ptr))) /* oh libpng, you're old as dirt, aren't you. */
		goto cleanup2;

	png_set_IHDR(png_ptr, info_ptr, (size_t) w, (size_t) h, colordepth,
			has_alpha ? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB,
			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
			PNG_FILTER_TYPE_DEFAULT);

	row = png_malloc(png_ptr, h * sizeof(*row));
	for (y = 0; y < h; y++) {
		row[y] = png_malloc(png_ptr, w * bytes_per_pixel);
		for (x = 0; x < w; x++) {
			unsigned char *r = (unsigned char *) row[y];
			unsigned char *src = (unsigned char *)
				&pixels[y * w * bytes_per_pixel + x * bytes_per_pixel];
			unsigned char *dest = &r[x * bytes_per_pixel];
			memcpy(dest, src, bytes_per_pixel);
		}
	}

	png_init_io(png_ptr, f);
	png_set_rows(png_ptr, info_ptr, row);
	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_PACKING, NULL);

	for (y = 0; y < h; y++)
		png_free(png_ptr, row[y]);
	png_free(png_ptr, row);
	rc = 0;
cleanup2:
	png_destroy_write_struct(&png_ptr, &info_ptr);
cleanup1:
	fclose(f);
	return rc;
}

/* ---------------------------------------------------------------------------
 * Readable test output: every generated image is checked (noise value range
 * and PNG file) and reported line by line, so the CI log shows what the
 * test did and why it passed or failed.
 * ------------------------------------------------------------------------- */

struct noise_stats {
	double min;
	double max;
	double sum;
};

static void stats_init(struct noise_stats *s)
{
	s->min = 1e300;
	s->max = -1e300;
	s->sum = 0.0;
}

static void stats_add(struct noise_stats *s, double value)
{
	if (value < s->min)
		s->min = value;
	if (value > s->max)
		s->max = value;
	s->sum += value;
}

static int stats_in_range(const struct noise_stats *s)
{
	return (s->min >= -NOISE_LIMIT - NOISE_EPSILON) &&
	       (s->max <= NOISE_LIMIT + NOISE_EPSILON);
}

static int report_test(const char *name, const char *png,
		       int write_rc, const struct noise_stats *s)
{
	struct stat file;
	long long size = 0;
	double mean = s->sum / (double) (WIDTH * HEIGHT);
	int in_range = stats_in_range(s);
	int wrote = (write_rc == 0) && (stat(png, &file) == 0);
	int ok = in_range && wrote;

	if (wrote)
		size = (long long) file.st_size;
	printf("  %-40s min=% .6f max=% .6f mean=% .6f -> %s (%lld bytes) ... %s\n",
	       name, s->min, s->max, mean, png, size,
	       ok ? "PASS" : "FAIL");
	if (!in_range)
		printf("       noise value outside the expected range [-1, 1]\n");
	if (write_rc != 0)
		printf("       write_png_image(%s) failed\n", png);
	else if (!wrote)
		printf("       %s is missing or unreadable\n", png);
	return ok;
}

/* Range-check one 2D noise variant over a half-resolution grid.
 * Returns 1 on pass, 0 on fail. */
static int check_noise2_variant(struct OpenSimplex2F_context *ctx, const char *name,
				double (*f)(struct OpenSimplex2F_context *, double, double))
{
	struct noise_stats s;
	int x, y, ok;

	stats_init(&s);
	for (y = 0; y < HEIGHT; y += 2)
		for (x = 0; x < WIDTH; x += 2)
			stats_add(&s, f(ctx, (double) x / FEATURE_SIZE, (double) y / FEATURE_SIZE));
	ok = stats_in_range(&s);
	printf("  %-40s min=% .6f max=% .6f ... %s\n",
	       name, s.min, s.max, ok ? "PASS" : "FAIL");
	if (!ok)
		printf("       noise value outside the expected range [-1, 1]\n");
	return ok;
}

/* Range-check one 3D noise variant over a half-resolution grid at
 * z = Z_OFFSET. Returns 1 on pass, 0 on fail. */
static int check_noise3_variant(struct OpenSimplex2F_context *ctx, const char *name,
				double (*f)(struct OpenSimplex2F_context *, double, double, double))
{
	struct noise_stats s;
	int x, y, ok;

	stats_init(&s);
	for (y = 0; y < HEIGHT; y += 2)
		for (x = 0; x < WIDTH; x += 2)
			stats_add(&s, f(ctx, (double) x / FEATURE_SIZE,
					(double) y / FEATURE_SIZE, Z_OFFSET));
	ok = stats_in_range(&s);
	printf("  %-40s min=% .6f max=% .6f ... %s\n",
	       name, s.min, s.max, ok ? "PASS" : "FAIL");
	if (!ok)
		printf("       noise value outside the expected range [-1, 1]\n");
	return ok;
}

/* Range-check one 4D noise variant over a half-resolution grid at
 * z = Z_OFFSET, w = W_OFFSET. Returns 1 on pass, 0 on fail. */
static int check_noise4_variant(struct OpenSimplex2F_context *ctx, const char *name,
				double (*f)(struct OpenSimplex2F_context *, double, double, double, double))
{
	struct noise_stats s;
	int x, y, ok;

	stats_init(&s);
	for (y = 0; y < HEIGHT; y += 2)
		for (x = 0; x < WIDTH; x += 2)
			stats_add(&s, f(ctx, (double) x / FEATURE_SIZE,
					(double) y / FEATURE_SIZE, Z_OFFSET, W_OFFSET));
	ok = stats_in_range(&s);
	printf("  %-40s min=% .6f max=% .6f ... %s\n",
	       name, s.min, s.max, ok ? "PASS" : "FAIL");
	if (!ok)
		printf("       noise value outside the expected range [-1, 1]\n");
	return ok;
}

/* ---------------------------------------------------------------------------
 * Test functions. Each one is a separate test case of the main test
 * executable and is registered as its own CTest test. Every function
 * creates its own noise context, renders/checks the requested noise
 * functions and returns 0 on success, 1 on failure.
 * ------------------------------------------------------------------------- */

/* 2D test: renders test2d.png from OpenSimplex2F_noise2 and range-checks
 * the alternative 2D orientation. */
static int test_noise2(void)
{
	/* Static storage: a 1MB array would not fit the default Windows
	 * thread stack (1MB) and crash with STATUS_STACK_BUFFER_OVERRUN
	 * (0xC000013D); Linux survives it only because its default stack
	 * is 8MB.
	 */
	static uint32_t image[HEIGHT][WIDTH];
	struct OpenSimplex2F_context *ctx = NULL;
	struct noise_stats s;
	uint32_t rgb;
	int x, y, ok = 1;

	if (OpenSimplex2F(TEST_SEED, &ctx) != 0) {
		printf("  OpenSimplex2F() failed\n");
		return 1;
	}

	stats_init(&s);
	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH; x++) {
			double value = OpenSimplex2F_noise2(ctx, (double) x / FEATURE_SIZE, (double) y / FEATURE_SIZE);
			stats_add(&s, value);
			rgb = 0x010101 * (uint32_t) ((value + 1) * 127.5);
			image[y][x] = ((uint32_t) 0x0ff << 24) | (rgb);
		}
	}

	if (!report_test("OpenSimplex2F_noise2", "test2d.png",
			 write_png_image("test2d.png", (unsigned char *) image, WIDTH, HEIGHT, 1),
			 &s))
		ok = 0;
	if (!check_noise2_variant(ctx, "OpenSimplex2F_noise2_XBeforeY", OpenSimplex2F_noise2_XBeforeY))
		ok = 0;

	OpenSimplex2F_free(ctx);
	return ok ? 0 : 1;
}

/* 3D test: renders test3d.png from OpenSimplex2F_noise3_Classic and
 * range-checks the alternative 3D orientations. */
static int test_noise3(void)
{
	static uint32_t image[HEIGHT][WIDTH];
	struct OpenSimplex2F_context *ctx = NULL;
	struct noise_stats s;
	uint32_t rgb;
	int x, y, ok = 1;

	if (OpenSimplex2F(TEST_SEED, &ctx) != 0) {
		printf("  OpenSimplex2F() failed\n");
		return 1;
	}

	stats_init(&s);
	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH; x++) {
			double value = OpenSimplex2F_noise3_Classic(ctx, (double) x / FEATURE_SIZE,
								    (double) y / FEATURE_SIZE, 0.0);
			stats_add(&s, value);
			rgb = 0x010101 * (uint32_t) ((value + 1) * 127.5);
			image[y][x] = ((uint32_t) 0x0ff << 24) | (rgb);
		}
	}

	if (!report_test("OpenSimplex2F_noise3_Classic", "test3d.png",
			 write_png_image("test3d.png", (unsigned char *) image, WIDTH, HEIGHT, 1),
			 &s))
		ok = 0;
	if (!check_noise3_variant(ctx, "OpenSimplex2F_noise3_XYBeforeZ", OpenSimplex2F_noise3_XYBeforeZ))
		ok = 0;
	if (!check_noise3_variant(ctx, "OpenSimplex2F_noise3_XZBeforeY", OpenSimplex2F_noise3_XZBeforeY))
		ok = 0;

	OpenSimplex2F_free(ctx);
	return ok ? 0 : 1;
}

/* 4D test: renders test4d.png from a 3-octave blend of
 * OpenSimplex2F_noise4_Classic and range-checks the alternative
 * 4D orientations. */
static int test_noise4(void)
{
	static uint32_t image[HEIGHT][WIDTH];
	struct OpenSimplex2F_context *ctx = NULL;
	struct noise_stats s;
	uint32_t rgb;
	int x, y, ok = 1;

	if (OpenSimplex2F(TEST_SEED, &ctx) != 0) {
		printf("  OpenSimplex2F() failed\n");
		return 1;
	}

	stats_init(&s);
	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH; x++) {
			/* Use three octaves: frequency N, N/2 and N/4 with relative amplitudes 4:2:1. */
			double v0 = OpenSimplex2F_noise4_Classic(ctx, (double) x / FEATURE_SIZE / 4,
								 (double) y / FEATURE_SIZE / 4, 0.0, 0.0);
			double v1 = OpenSimplex2F_noise4_Classic(ctx, (double) x / FEATURE_SIZE / 2,
								 (double) y / FEATURE_SIZE / 2, 0.0, 0.0);
			double v2 = OpenSimplex2F_noise4_Classic(ctx, (double) x / FEATURE_SIZE / 1,
								 (double) y / FEATURE_SIZE / 1, 0.0, 0.0);
			double value = v0 * 4 / 7.0 + v1 * 2 / 7.0 + v2 * 1 / 7.0;

			stats_add(&s, value);
			rgb = 0x010101 * (uint32_t) ((value + 1) * 127.5);
			image[y][x] = ((uint32_t) 0x0ff << 24) | (rgb);
		}
	}

	if (!report_test("OpenSimplex2F_noise4 (3 octaves)", "test4d.png",
			 write_png_image("test4d.png", (unsigned char *) image, WIDTH, HEIGHT, 1),
			 &s))
		ok = 0;
	if (!check_noise4_variant(ctx, "OpenSimplex2F_noise4_XYBeforeZW", OpenSimplex2F_noise4_XYBeforeZW))
		ok = 0;
	if (!check_noise4_variant(ctx, "OpenSimplex2F_noise4_XZBeforeYW", OpenSimplex2F_noise4_XZBeforeYW))
		ok = 0;
	if (!check_noise4_variant(ctx, "OpenSimplex2F_noise4_XYZBeforeW", OpenSimplex2F_noise4_XYZBeforeW))
		ok = 0;

	OpenSimplex2F_free(ctx);
	return ok ? 0 : 1;
}

/* ---------------------------------------------------------------------------
 * Test registry and the main test executable entry point.
 * ------------------------------------------------------------------------- */

struct test_case {
	const char *name;
	int (*run)(void);
};

static const struct test_case all_tests[] = {
	{ "test_noise2", test_noise2 },
	{ "test_noise3", test_noise3 },
	{ "test_noise4", test_noise4 },
};

#define TEST_COUNT (sizeof(all_tests) / sizeof(all_tests[0]))

static void print_usage(const char *prog)
{
	size_t i;
	printf("usage: %s [test-name]\n", prog);
	printf("available tests:\n");
	for (i = 0; i < TEST_COUNT; i++)
		printf("  %s\n", all_tests[i].name);
}

int main(int argc, char *argv[])
{
	size_t i;
	int selected = -1;
	int ran = 0, failed = 0;

	if (argc > 2) {
		print_usage(argv[0]);
		return 2;
	}

	if (argc == 2) {
		for (i = 0; i < TEST_COUNT; i++) {
			if (strcmp(argv[1], all_tests[i].name) == 0) {
				selected = (int) i;
				break;
			}
		}
		if (selected < 0) {
			printf("unknown test: %s\n", argv[1]);
			print_usage(argv[0]);
			return 2;
		}
	}

	printf("OpenSimplex2F tests (seed %d, image %dx%d)\n", TEST_SEED, WIDTH, HEIGHT);

	if (selected >= 0) {
		printf("== %s ==\n", all_tests[selected].name);
		if (all_tests[selected].run() != 0)
			failed++;
		ran = 1;
	} else {
		for (i = 0; i < TEST_COUNT; i++) {
			printf("== %s ==\n", all_tests[i].name);
			if (all_tests[i].run() != 0)
				failed++;
			ran++;
		}
	}

	OpenSimplex2F_shutdown();
	printf("Result: %d/%d tests passed\n", ran - failed, ran);
	return failed ? 1 : 0;
}
