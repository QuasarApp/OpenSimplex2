/*
 * Test file for OpenSimplex2F_noise1 and OpenSimplex2F_noise1_fast functions.
 * This file tests that both functions produce identical results and that
 * the output values are within the expected range.
 */

#include <stdio.h>
#include <math.h>
#include "OpenSimplex2F.h"

#define NOISE_EPSILON 1e-6
#define RANGE_MIN -1.5
#define RANGE_MAX 1.5

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
	double test_points[] = {0.0, 0.5, 1.0, -0.5, 5.5, -5.5, 0.25, 0.75, 1.25, -1.25};
	size_t num_tests = sizeof(test_points) / sizeof(test_points[0]);

	printf("Testing OpenSimplex2F_noise1...\n");
	for (size_t i = 0; i < num_tests; i++) {
		double x = test_points[i];
		double result = OpenSimplex2F_noise1(ctx, x);
		printf("noise1(%.2f) = %.6f\n", x, result);
	}

	printf("\nTesting OpenSimplex2F_noise1_fast...\n");
	for (size_t i = 0; i < num_tests; i++) {
		double x = test_points[i];
		double result = OpenSimplex2F_noise1_fast(ctx, x);
		printf("noise1_fast(%.2f) = %.6f\n", x, result);
	}

	// Verify that both functions produce identical results
	printf("\nVerifying identical results...\n");
	for (size_t i = 0; i < num_tests; i++) {
		double x = test_points[i];
		double result1 = OpenSimplex2F_noise1(ctx, x);
		double result_fast = OpenSimplex2F_noise1_fast(ctx, x);

		if (fabs(result1 - result_fast) > NOISE_EPSILON) {
			printf("MISMATCH at x = %.2f: noise1=%.6f, noise1_fast=%.6f\n", x, result1, result_fast);
			tests_passed = 0;
		}
	}

	// Verify that noise values are within expected range
	printf("\nVerifying output range...\n");
	for (size_t i = 0; i < num_tests; i++) {
		double x = test_points[i];
		double result1 = OpenSimplex2F_noise1(ctx, x);
		double result_fast = OpenSimplex2F_noise1_fast(ctx, x);

		// Check both functions produce values within expected range
		if (result1 < RANGE_MIN || result1 > RANGE_MAX) {
			printf("OUT OF RANGE at x = %.2f: noise1=%.6f (expected [%.2f, %.2f])\n", x, result1, RANGE_MIN, RANGE_MAX);
			tests_passed = 0;
		}
		if (result_fast < RANGE_MIN || result_fast > RANGE_MAX) {
			printf("OUT OF RANGE at x = %.2f: noise1_fast=%.6f (expected [%.2f, %.2f])\n", x, result_fast, RANGE_MIN, RANGE_MAX);
			tests_passed = 0;
		}
	}

	if (tests_passed) {
		printf("\nAll tests passed!\n");
	} else {
		printf("\nSome tests failed!\n");
	}

	// Cleanup
	OpenSimplex2F_free(ctx);
	return tests_passed ? 0 : 1;
}
