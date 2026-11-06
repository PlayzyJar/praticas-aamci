#include "pi_mc.h"

void pi_mc_init(pi_mc_t *s)
{
    s->inside = 0u;
    s->total  = 0u;
}

int pi_mc_is_inside(uint32_t x, uint32_t y)
{
    uint64_t xx = (uint64_t)x * x;
    uint64_t yy = (uint64_t)y * y;
    /* x^2 + y^2 < 2^64  <=>  x^2 <= UINT64_MAX - y^2 (evita overflow da soma) */
    return xx <= (UINT64_MAX - yy);
}

void pi_mc_run(pi_mc_t *s, pi_rng_fn rng, uint32_t n)
{
    for (uint32_t i = 0u; i < n; i++) {
        uint32_t x = rng();
        uint32_t y = rng();
        if (pi_mc_is_inside(x, y)) {
            s->inside++;
        }
    }
    s->total += n;
}

uint64_t pi_mc_micro(const pi_mc_t *s)
{
    if (s->total == 0u) {
        return 0u;
    }
    return (4000000u * s->inside) / s->total;
}
