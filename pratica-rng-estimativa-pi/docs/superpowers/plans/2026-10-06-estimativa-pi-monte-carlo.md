# Estimativa de π por Monte Carlo (RNG STM32F767) Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Firmware para a Nucleo-F767ZI que usa o periférico RNG para estimar π por Monte Carlo e imprime a estimativa acumulada via `printf` (USART3 / VCP do ST-LINK).

**Architecture:** Um módulo C puro (`pi_mc.c/.h`), sem dependência de HAL, recebe a fonte de números aleatórios por ponteiro de função; isso permite testá-lo no PC com `gcc`. O `main.c` apenas injeta o RNG de hardware, redireciona `printf` para a UART e roda um laço infinito: sorteia 1.000.000 de pontos por lote e imprime a estimativa acumulada. Toda a aritmética do laço é inteira (`uint64_t`); π é impresso em ponto fixo (6 casas), então não é preciso `-u _printf_float`.

**Tech Stack:** C (gnu11), STM32 HAL (F7), STM32CubeIDE project (.cproject), `gcc` do host para testes unitários.

**Spec:** Sem arquivo de spec; o design aprovado no chat é o spec: abordagem A (laço contínuo, impressão a cada 1.000.000 de pontos, contadores `uint64_t`, aritmética inteira) + enunciado do usuário (usar RNG, Monte Carlo, `printf` do π estimado, maior precisão possível).

## Global Constraints

- Projeto em `/var/home/carloseliasf/Documentos/stm-projects/hello-worlrd-767/pratica-rng-estimativa-pi` (não é repositório git; **não há commits**: o implementador só confirma com os testes/arquivos).
- `x` e `y` são `uint32_t` vindos do RNG; um ponto está dentro se `x² + y² < 2^64`, calculado **sem overflow** como `x*x <= UINT64_MAX - y*y` (com `x*x`, `y*y` em `uint64_t`).
- Contadores `inside` e `total` são `uint64_t`.
- Lote = `PI_MC_BATCH` = `1000000u` pontos por impressão.
- π impresso em micro-unidades inteiras: `pi_micro = 4 * 1000000 * inside / total` (`uint64_t`), formato `"%lu.%06lu"`.
- Só editar código de usuário dentro dos blocos `/* USER CODE BEGIN ... */` de `main.c` (o CubeMX regenera o resto). Preservar comentários existentes.
- `HAL_RNG_GenerateRandomNumber` com retorno diferente de `HAL_OK` → `Error_Handler()`.
- Não alterar o arquivo `.ioc`.
- Novos arquivos ficam em `Core/Src` e `Core/Inc` (já são source folders do CubeIDE).

## File Structure

- Create `Core/Inc/pi_mc.h`: API pública do módulo.
- Create `Core/Src/pi_mc.c`: lógica do Monte Carlo (sem HAL).
- Create `tests/test_pi_mc.c`: testes no PC (gcc), com `assert`.
- Modify `Core/Src/main.c`: `__io_putchar`, `hw_rng`, laço principal (apenas em blocos USER CODE).

---

### Task 1: Módulo `pi_mc` testado no PC

**Files:**
- Create: `Core/Inc/pi_mc.h`
- Create: `Core/Src/pi_mc.c`
- Test: `tests/test_pi_mc.c`

**Interfaces:**
- Consumes: nada.
- Produces (usados pela Task 2):
  ```c
  #define PI_MC_BATCH 1000000u
  typedef uint32_t (*pi_rng_fn)(void);
  typedef struct { uint64_t inside; uint64_t total; } pi_mc_t;
  void     pi_mc_init(pi_mc_t *s);
  int      pi_mc_is_inside(uint32_t x, uint32_t y);
  void     pi_mc_run(pi_mc_t *s, pi_rng_fn rng, uint32_t n);
  uint64_t pi_mc_micro(const pi_mc_t *s);   /* π * 1e6, 0 se total == 0 */
  ```

- [ ] **Step 1: Escrever o teste que falha**

Criar `tests/test_pi_mc.c`:

```c
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
    assert(pi_mc_is_inside(0xB504F333u, 0xB504F333u) == 0); /* ~ sqrt(2)/2 * 2^32, ligeiramente fora */
    assert(pi_mc_is_inside(0xB504F332u, 0xB504F332u) == 1); /* logo abaixo do limite */
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
```

- [ ] **Step 2: Rodar e ver falhar**

Run (na raiz do projeto): `gcc -std=gnu11 -Wall -Wextra -ICore/Inc tests/test_pi_mc.c Core/Src/pi_mc.c -o /tmp/test_pi_mc && /tmp/test_pi_mc`
Expected: FAIL de compilação (`pi_mc.h: No such file or directory`).

- [ ] **Step 3: Implementação mínima**

Criar `Core/Inc/pi_mc.h`:

```c
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
```

Criar `Core/Src/pi_mc.c`:

```c
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
```

- [ ] **Step 4: Rodar e ver passar**

Run: `gcc -std=gnu11 -Wall -Wextra -ICore/Inc tests/test_pi_mc.c Core/Src/pi_mc.c -o /tmp/test_pi_mc && /tmp/test_pi_mc`
Expected: imprime `pi ~ 3.14xxxx` e `OK`, sem warnings. Se o valor de limite em `test_is_inside` (0xB504F333/0xB504F332) falhar, recalcule: o limite é `floor(2^32 / sqrt(2)) = 3037000499 = 0xB504F333`; ajuste para que `0xB504F333` seja o último valor *dentro* se a conta indicar isso e registre no relatório.

- [ ] **Step 5: Registrar**

Não há git neste projeto; listar no relatório os 3 arquivos criados e a saída do teste.

---

### Task 2: Integração no firmware (`main.c`)

**Files:**
- Modify: `Core/Src/main.c` (blocos `USER CODE BEGIN Includes`, `USER CODE BEGIN 0`, `USER CODE BEGIN 2`, `USER CODE BEGIN WHILE`/`3`)

**Interfaces:**
- Consumes (da Task 1): `pi_mc_t`, `pi_mc_init`, `pi_mc_run`, `pi_mc_micro`, `PI_MC_BATCH` de `pi_mc.h`.
- Produces: firmware final. Usa símbolos existentes: `hrng` (`RNG_HandleTypeDef`), `huart3` (`UART_HandleTypeDef`), `Error_Handler()`; `syscalls.c` já chama o `__io_putchar` fraco, então basta definir `int __io_putchar(int ch)`.

- [ ] **Step 1: Includes**

Dentro de `/* USER CODE BEGIN Includes */ ... /* USER CODE END Includes */`:

```c
#include <stdio.h>
#include <stdint.h>
#include "pi_mc.h"
```

- [ ] **Step 2: Retarget do printf e RNG de hardware**

Dentro de `/* USER CODE BEGIN 0 */ ... /* USER CODE END 0 */`:

```c
/* printf -> USART3 (VCP do ST-LINK). Chamado por _write() em syscalls.c */
int __io_putchar(int ch)
{
  uint8_t c = (uint8_t)ch;
  HAL_UART_Transmit(&huart3, &c, 1, HAL_MAX_DELAY);
  return ch;
}

/* Fonte de aleatoriedade: periférico RNG (32 bits por chamada). */
static uint32_t hw_rng(void)
{
  uint32_t v;
  if (HAL_RNG_GenerateRandomNumber(&hrng, &v) != HAL_OK)
  {
    Error_Handler();
  }
  return v;
}
```

- [ ] **Step 3: Laço principal**

Em `/* USER CODE BEGIN 2 */ ... /* USER CODE END 2 */`:

```c
  pi_mc_t pi;
  pi_mc_init(&pi);
  printf("\r\nEstimativa de pi por Monte Carlo (RNG STM32F767)\r\n");
```

Em `/* USER CODE BEGIN 3 */` (dentro do `while (1)`, antes do `}`):

```c
    pi_mc_run(&pi, hw_rng, PI_MC_BATCH);
    uint64_t micro = pi_mc_micro(&pi);
    printf("pontos=%lu pi=%lu.%06lu\r\n",
           (unsigned long)pi.total,
           (unsigned long)(micro / 1000000u),
           (unsigned long)(micro % 1000000u));
```

Nota: `pi.total` é impresso como `unsigned long` (32 bits no ARM) → estoura após ~4,29×10⁹ pontos (~2 h). Para evitar, imprimir em milhões: trocar `pontos=%lu` por `Mpontos=%lu` com argumento `(unsigned long)(pi.total / 1000000u)`. **Usar esta versão em milhões.**

- [ ] **Step 4: Verificar**

Se `arm-none-eabi-gcc` existir no PATH: compilar só as fontes novas/alteradas com `arm-none-eabi-gcc -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F767xx -ICore/Inc -IDrivers/STM32F7xx_HAL_Driver/Inc -IDrivers/STM32F7xx_HAL_Driver/Inc/Legacy -IDrivers/CMSIS/Device/ST/STM32F7xx/Include -IDrivers/CMSIS/Include -Wall -c Core/Src/main.c -o /tmp/main.o`. Expected: sem erros. Se o toolchain não existir (cenário atual da máquina), reportar "não compilado: sem toolchain ARM" e conferir manualmente: apenas blocos USER CODE editados, `grep -n "pi_mc\|hw_rng\|__io_putchar" Core/Src/main.c` mostra as inserções, e rodar de novo o teste da Task 1.

- [ ] **Step 5: Registrar**

Sem git: listar o diff manual (trechos inseridos) no relatório e instruir o usuário: abrir no STM32CubeIDE, Build, Run/Debug, abrir terminal serial 115200 8N1 (confirmar baud em `huart3.Init.BaudRate` no `main.c`).
