#ifndef PI_MC_H
#define PI_MC_H

#include <stdint.h>

/* Pontos sorteados por lote (uma impressão por lote). */
#define PI_MC_BATCH 1000000u

/* Fonte de números aleatórios de 32 bits uniformes. */
typedef uint32_t (*pi_rng_fn)(void);

typedef struct {
    uint64_t inside; /* pontos dentro do quarto de círculo */
    uint64_t total;  /* pontos sorteados */
} pi_mc_t;

void     pi_mc_init(pi_mc_t *s);
/* 1 se x^2 + y^2 < 2^64 (coordenadas em escala 2^32 = 1.0), sem overflow. */
int      pi_mc_is_inside(uint32_t x, uint32_t y);
/* Sorteia n pontos e acumula em s. */
void     pi_mc_run(pi_mc_t *s, pi_rng_fn rng, uint32_t n);
/* Estimativa de pi multiplicada por 1e6 (0 se ainda não há pontos). */
uint64_t pi_mc_micro(const pi_mc_t *s);

#endif /* PI_MC_H */
