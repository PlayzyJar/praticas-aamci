# Relatório de Execução - Task 1: Módulo `pi_mc`

## 1. Visão Geral
Implementação do módulo puro C `pi_mc` (`Core/Inc/pi_mc.h` e `Core/Src/pi_mc.c`) e da suíte de testes unitários no host (`tests/test_pi_mc.c`), seguindo a metodologia TDD (Test-Driven Development).

## 2. Metodologia TDD

### Fase RED (Falha Inicial)
1. Criação do arquivo de teste `tests/test_pi_mc.c` com as asserções iniciais.
2. Execução da compilação e teste antes de implementar o módulo:
   ```bash
   gcc -std=gnu11 -Wall -Wextra -ICore/Inc tests/test_pi_mc.c Core/Src/pi_mc.c -o /tmp/test_pi_mc && /tmp/test_pi_mc
   ```
3. Resultado observado:
   ```text
   tests/test_pi_mc.c:4:10: fatal error: pi_mc.h: Arquivo ou diretório inexistente
   Core/Src/pi_mc.c: Arquivo ou diretório inexistente
   ```
   Confirmada a falha inicial esperada (ausência dos arquivos de produção).

### Fase GREEN (Implementação Mínima)
1. Criação de `Core/Inc/pi_mc.h` e `Core/Src/pi_mc.c` com base literal no plano.
2. Compilação executada com sucesso sem warnings (`-Wall -Wextra`).
3. Ao executar `/tmp/test_pi_mc`, ocorreu falha de asserção na verificação de fronteira:
   ```text
   test_pi_mc: tests/test_pi_mc.c:24: test_is_inside: Assertion `pi_mc_is_inside(0xB504F333u, 0xB504F333u) == 0' failed.
   ```

### Ajuste Matemático da Fronteira (Conforme Especificado no Plano)
- Cálculo analítico do limite diagonal:
  $$\text{limite} = \left\lfloor \frac{2^{32}}{\sqrt{2}} \right\rfloor = \lfloor 3037000499.976\dots \rfloor = 3037000499 = \text{0xB504F333}$$
- Verificação de overflow e pertinência ao quarto de círculo ($x^2 + y^2 < 2^{64}$):
  - Para $x = y = \text{0xB504F333}$ ($3037000499$):
    $$2 \times 3037000499^2 = 18446744061852498002 < 2^{64} = 18446744073709551616$$
    Portanto, $\text{0xB504F333}$ é o último ponto **dentro** da fronteira (`pi_mc_is_inside(...) == 1`).
  - Para $x = y = \text{0xB504F334}$ ($3037000500$):
    $$2 \times 3037000500^2 = 18446744074000500000 \ge 2^{64}$$
    Portanto, $\text{0xB504F334}$ é o primeiro ponto **fora** da fronteira (`pi_mc_is_inside(...) == 0`).
- Ajuste realizado exclusivamente em `tests/test_pi_mc.c`:
  - `assert(pi_mc_is_inside(0xB504F334u, 0xB504F334u) == 0);`
  - `assert(pi_mc_is_inside(0xB504F333u, 0xB504F333u) == 1);`

### Verificação Final (GREEN)
Execução do teste:
```bash
gcc -std=gnu11 -Wall -Wextra -ICore/Inc tests/test_pi_mc.c Core/Src/pi_mc.c -o /tmp/test_pi_mc && /tmp/test_pi_mc
```
Saída:
```text
pi ~ 3.142736
OK
```
Código de retorno: `0`. Compilação limpa sem avisos.

## 3. Arquivos Criados / Modificados
1. `/var/home/carloseliasf/Documentos/stm-projects/hello-worlrd-767/pratica-rng-estimativa-pi/Core/Inc/pi_mc.h` (Criado)
2. `/var/home/carloseliasf/Documentos/stm-projects/hello-worlrd-767/pratica-rng-estimativa-pi/Core/Src/pi_mc.c` (Criado)
3. `/var/home/carloseliasf/Documentos/stm-projects/hello-worlrd-767/pratica-rng-estimativa-pi/tests/test_pi_mc.c` (Criado)

Nenhuma modificação foi feita em `Core/Src/main.c`. Nenhum comando git foi executado.

## 4. Status e Conclusão
- **Status:** DONE
- **Preocupações:** Nenhuma. O módulo não possui dependências de HAL, utiliza aritmética inteira pura `uint64_t` sem risco de overflow no cálculo da norma, e os testes cobrem casos de canto (0, 0), (MAX, 0), (MAX, MAX), limites de fronteira exatos e teste estatístico com gerador xorshift determinístico.
