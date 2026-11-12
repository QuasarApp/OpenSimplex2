/*
 * Test file for OpenSimplex2F_noise1 and OpenSimplex2F_noise1_fast functions.
 * This file tests that both functions produce identical results.
 */

#include <stdio.h>
#include <math.h>
#include "OpenSimplex2F.h"

#define NOISE_EPSILON 1e-6

int main(void)
{
	struct OpenSimplex2F_context *ctx;
	int64_t seed = 123456789ULL;
	int tests_passed = 1;

	// Initialize context
	if (OpenSimplex2F(seed, &ctx) != 0) {
		printf("Failed to initialize OpenSimplex2F context\n");
		return 1;
	}

	// Test points
	double test_points[] = {0.0, 0.5, 1.0, -0.5, 5.5, -5.5};
	int num_tests = sizeof(test_points) / sizeof(test_points[0]);

	printf("Testing OpenSimplex2F_noise1...\n");
	for (int i = 0; i < num_tests; i++) {
		double x = test_points[i];
		double result = OpenSimplex2F_noise1(ctx, x);
		printf("noise1(%.1f) = %.6f\n", x, result);
	}

	printf("\nTesting OpenSimplex2F_noise1_fast...\n");
	for (int i = 0; i < num_tests; i++) {
		double x = test_points[i];
		double result = OpenSimplex2F_noise1_fast(ctx, x);
		printf("noise1_fast(%.1f) = %.6f\n", x, result);
	}

	// Verify that both functions produce identical results
	printf("\nVerifying identical results...\n");
	for (int i = 0; i < num_tests; i++) {
		double x = test_points[i];
		double result1 = OpenSimplex2F_noise1(ctx, x);
		double result_fast = OpenSimplex2F_noise1_fast(ctx, x);

		if (fabs(result1 - result_fast) > 1e-10) {
			printf("MISMATCH at x = %.1f: noise1=%.6f, noise1_fast=%.6f\n", x, result1, result_fast);
			tests_passed = 0;
		}
	}

	// Verify that noise values are within expected range [-1, 1]
	printf("\nVerifying noise value range [-1, 1]...\n");
	int range_check_passed = 1;
	for (int i = 0; i < num_tests; i++) {
		double x = test_points[i];
		double result1 = OpenSimplex2F_noise1(ctx, x);
		double result_fast = OpenSimplex2F_noise1_fast(ctx, x);

		if (result1 < -1.0 - NOISE_EPSILON || result1 > 1.0 + NOISE_EPSILON) {
			printf("noise1(%.1f) = %.6f is outside range [-1, 1]\n", x, result1);
			range_check_passed = 0;
		}
		if (result_fast < -1.0 - NOISE_EPSILON || result_fast > 1.0 + NOISE_EPSILON) {
			printf("noise1_fast(%.1f) = %.6f is outside range [-1, 1]\n", x, result_fast);
			range_check_passed = 0;
		}
	}

	if (tests_passed && range_check_passed) {
		printf("\nAll tests passed!\n");
	} else {
		printf("\nSome tests failed!\n");
	}

	// Cleanup
	OpenSimplex2F_free(ctx);
	return tests_passed ? 0 : 1;
}
