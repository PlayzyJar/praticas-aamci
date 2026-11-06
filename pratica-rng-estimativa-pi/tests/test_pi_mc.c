#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "pi_mc.h"

static uint32_t rng_zero(void) { return 0u; }
static uint32_t rng_max(void)  { return 0xFFFFFFFFu; }

/* xorshift32 determinístico para o teste estatístico */
static uint32_t xs_state = 2463534242u;
static uint32_t rng_xorshift(void)
{
    xs_state ^= xs_state << 13;
    xs_state ^= xs_state >> 17;
    xs_state ^= xs_state << 5;
    return xs_state;
}

static void test_is_inside(void)
{
    assert(pi_mc_is_inside(0u, 0u) == 1);
    assert(pi_mc_is_inside(0xFFFFFFFFu, 0u) == 1);          /* (1-eps, 0) */
    assert(pi_mc_is_inside(0xFFFFFFFFu, 0xFFFFFFFFu) == 0); /* canto: fora, sem overflow */
    assert(pi_mc_is_inside(0xB504F334u, 0xB504F334u) == 0); /* logo acima do limite */
    assert(pi_mc_is_inside(0xB504F333u, 0xB504F333u) == 1); /* floor(2^32/sqrt(2)), último dentro */
}

static void test_all_inside_gives_4(void)
{
    pi_mc_t s; pi_mc_init(&s);
    pi_mc_run(&s, rng_zero, 1000u);
    assert(s.total == 1000u && s.inside == 1000u);
    assert(pi_mc_micro(&s) == 4000000u);
}

static void test_all_outside_gives_0(void)
{
    pi_mc_t s; pi_mc_init(&s);
    pi_mc_run(&s, rng_max, 1000u);
    assert(s.total == 1000u && s.inside == 0u);
    assert(pi_mc_micro(&s) == 0u);
}

static void test_empty_is_zero(void)
{
    pi_mc_t s; pi_mc_init(&s);
    assert(pi_mc_micro(&s) == 0u);
}

static void test_accumulates(void)
{
    pi_mc_t s; pi_mc_init(&s);
    pi_mc_run(&s, rng_zero, 10u);
    pi_mc_run(&s, rng_zero, 15u);
    assert(s.total == 25u);
}

static void test_statistical_pi(void)
{
    pi_mc_t s; pi_mc_init(&s);
    pi_mc_run(&s, rng_xorshift, 4u * PI_MC_BATCH);
    uint64_t micro = pi_mc_micro(&s);
    printf("pi ~ %llu.%06llu\n",
           (unsigned long long)(micro / 1000000u),
           (unsigned long long)(micro % 1000000u));
    assert(micro > 3130000u && micro < 3150000u);
}

int main(void)
{
    test_is_inside();
    test_all_inside_gives_4();
    test_all_outside_gives_0();
    test_empty_is_zero();
    test_accumulates();
    test_statistical_pi();
    puts("OK");
    return 0;
}
