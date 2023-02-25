#ifndef __XORSHIRO256_H__
#define __XORSHIRO256_H__

// http://prng.di.unimi.it
struct xoshiro256_s {
	uint64_t s[4];
};

void xoshiro256_init(struct xoshiro256_s *state, uint64_t seed);
uint64_t xoshiro256_next_long(struct xoshiro256_s *state);
double xoshiro256_next_real(struct xoshiro256_s *state);
bool xoshiro256_next_bool(struct xoshiro256_s *state);

void xoshiro256_jump(struct xoshiro256_s *state);
void xoshiro256_long_jump(struct xoshiro256_s *state);

#define XOSHIRO256_RANGED(value, min, max)	((value) * ((max) - (min)) + (min));

#endif /* __XORSHIRO256_H__ */
