// http://prng.di.unimi.it
/*  Written in 2018 by David Blackman and Sebastiano Vigna (vigna@acm.org)
    Written in 2015 by Sebastiano Vigna (vigna@acm.org)

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

See <http://creativecommons.org/publicdomain/zero/1.0/>. */

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "xoshiro256.h"

struct splitmix64_s {
	uint64_t seed;
};

/* This is a fixed-increment version of Java 8's SplittableRandom generator
   See http://dx.doi.org/10.1145/2714064.2660195 and 
   http://docs.oracle.com/javase/8/docs/api/java/util/SplittableRandom.html

   It is a very fast generator passing BigCrush, and it can be useful if
   for some reason you absolutely want 64 bits of state. */
static uint64_t splitmix64(struct splitmix64_s *state) {
	uint64_t z = (state->seed += 0x9e3779b97f4a7c15);
	z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
	z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
	return z ^ (z >> 31);
}

/* This is xoshiro256** 1.0, one of our all-purpose, rock-solid
   generators. It has excellent (sub-ns) speed, a state (256 bits) that is
   large enough for any parallel application, and it passes all tests we
   are aware of.

   For generating just floating-point numbers, xoshiro256+ is even faster.

   The state must be seeded so that it is not everywhere zero. If you have
   a 64-bit seed, we suggest to seed a splitmix64 generator and use its
   output to fill s. */
static inline uint64_t rotl(const uint64_t x, int32_t k) {
	return (x << k) | (x >> (64 - k));
}

void xoshiro256_init(struct xoshiro256_s *state, uint64_t seed) {
	struct splitmix64_s splitmax = { seed };

	uint64_t tmp = splitmix64(&splitmax);
	state->s[0] = (uint32_t) tmp;
	state->s[1] = (uint32_t) (tmp >> 32);

	tmp = splitmix64(&splitmax);
	state->s[2] = (uint32_t) tmp;
	state->s[3] = (uint32_t) (tmp >> 32);
}

uint64_t xoshiro256_next_long(struct xoshiro256_s *state) {
	const uint64_t result = rotl(state->s[1] * 5, 7) * 9;

	const uint64_t t = state->s[1] << 17;

	state->s[2] ^= state->s[0];
	state->s[3] ^= state->s[1];
	state->s[1] ^= state->s[2];
	state->s[0] ^= state->s[3];

	state->s[2] ^= t;

	state->s[3] = rotl(state->s[3], 45);

	return result;
}

double xoshiro256_next_real(struct xoshiro256_s *state) {
	uint64_t value = xoshiro256_next_long(state);
	// This might be faster but produces 1/2 the range of the multiplication version below as the least significant bit will always be set to zero
    // const union { uint64_t i; double d; } u = { .i = UINT64_C(0x3FF) << 52 | value >> 12 };
    // return u.d - 1.0;
    return (value >> 11) * 0x1.0p-53;
}

// int32_t xoshiro256_next_int(struct xoshiro256_s *state) {
// 	uint64_t value = xoshiro256_next_long(state);
// 	return (value >> 32);
// }

bool xoshiro256_next_bool(struct xoshiro256_s *state) {
	return xoshiro256_next_real(state) < 0.5;
	//uint64_t value = xoshiro256_next_long(state);
	// Return true if the sign bit is positive; false otherwise
	//return signbit(value) == 0;
}

/*
 * This is the jump function for the generator. It is equivalent
 * to 2^128 calls to next(); it can be used to generate 2^128
 * non-overlapping subsequences for parallel computations.
 */
void xoshiro256_jump(struct xoshiro256_s *state) {
	static const uint64_t JUMP[] = { 0x180ec6d33cfd0aba, 0xd5a61266f0c9392c, 0xa9582618e03fc9aa, 0x39abdc4529b1661c };

	uint64_t s0 = 0;
	uint64_t s1 = 0;
	uint64_t s2 = 0;
	uint64_t s3 = 0;
	for(size_t i = 0; i < sizeof JUMP / sizeof *JUMP; i++)
		for(int b = 0; b < 64; b++) {
			if (JUMP[i] & UINT64_C(1) << b) {
				s0 ^= state->s[0];
				s1 ^= state->s[1];
				s2 ^= state->s[2];
				s3 ^= state->s[3];
			}
			xoshiro256_next_long(state);
		}
		
	state->s[0] = s0;
	state->s[1] = s1;
	state->s[2] = s2;
	state->s[3] = s3;
}



/*
 * This is the long-jump function for the generator. It is equivalent to
 * 2^192 calls to next(); it can be used to generate 2^64 starting points,
 * from each of which jump() will generate 2^64 non-overlapping
 * subsequences for parallel distributed computations.
 */
void xoshiro256_long_jump(struct xoshiro256_s *state) {
	static const uint64_t LONG_JUMP[] = { 0x76e15d3efefdcbbf, 0xc5004e441c522fb3, 0x77710069854ee241, 0x39109bb02acbe635 };

	uint64_t s0 = 0;
	uint64_t s1 = 0;
	uint64_t s2 = 0;
	uint64_t s3 = 0;
	for(size_t i = 0; i < sizeof LONG_JUMP / sizeof *LONG_JUMP; i++) {
		for(int b = 0; b < 64; b++) {
			if (LONG_JUMP[i] & UINT64_C(1) << b) {
				s0 ^= state->s[0];
				s1 ^= state->s[1];
				s2 ^= state->s[2];
				s3 ^= state->s[3];
			}
			xoshiro256_next_long(state);
		}
	}
		
	state->s[0] = s0;
	state->s[1] = s1;
	state->s[2] = s2;
	state->s[3] = s3;
}
