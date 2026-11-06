# Pratica RNG Estimativa Pi (Monte Carlo)

Estimativa numerica de Pi por simulacao de Monte Carlo utilizando o periferico de hardware True RNG do STM32F767ZI, processado em lotes com aritmetica de 64 bits.

## Funcionamento

- A razao de pontos aleatorios uniformemente distribuidos que caem dentro de um quadrante circular em relacao ao quadrado circunscrito converge para `pi / 4`.
- A condicao $x^2 + y^2 \le (2^{32}-1)^2$ e calculada em `uint64_t` sem risco de overflow da soma utilizando `xx <= UINT64_MAX - yy`.
- Amostragem executada em lotes (`PI_MC_BATCH`) a cada ciclo do loop principal no Cortex-M7 @ 216MHz.
- Formata a aproximacao com 6 casas decimais inteiras via ponto fixo (`pi_mc_micro`).

## Pinagem

| Recurso | Pino | Modo | Descricao |
|---|---|---|---|
| HW True RNG | Interno | - | Pares de coordenadas (x, y) de 32 bits |
| USART3 TX/RX | `PD8` / `PD9` | AF7 | VCP via ST-LINK (115200 8N1) |

## Teste

1. Abra o monitor serial em 115200 8N1.
2. Acompanhe as mensagens continuas:
   ```text
   Estimativa de pi por Monte Carlo (RNG STM32F767)
   Mpontos=0 pi=3.141520
   Mpontos=1 pi=3.141634
   Mpontos=2 pi=3.141588
   ```
   Onde `Mpontos` indica milhoes de amostras processadas.
