# Pratica RNG (True Hardware RNG & Terminal ANSI)

Uso do periferico de hardware True RNG (*Random Number Generator*) do STM32F767ZI para geracao de numeros aleatorios verdadeiros e controle de tela fixa via sequencias de escape ANSI pela USART3.

## Funcionamento

- Periferico RNG de hardware gera entropia de 32 bits a partir de ruido termico analogico interno.
- Interface serial via USART3 (115200 8N1) utilizando sequencias ANSI para redesenho in-place (`\033[2J\033[H`).
- Implementa o jogo "Melhor de 3" (Machine vs Jogador):
  - Ao receber qualquer tecla via serial, sorteia dados de 1 a 6 para a maquina e para o jogador.
  - Atualiza o placar e declara o vencedor ao atingir 2 vitorias.

## Pinagem

| Recurso | Pino | Modo | Descricao |
|---|---|---|---|
| HW True RNG | Interno | - | Gerador de numeros aleatorios de 32 bits |
| USART3 TX/RX | `PD8` / `PD9` | AF7 | VCP via ST-LINK (115200 8N1) |
| LD1, LD2, LD3 | `PB0`, `PB7`, `PB14` | Output | Sinalizacao |

## Teste

1. Abra um terminal com suporte ANSI em 115200 8N1.
2. Observe o quadro do jogo:
   ```text
   ========================================
           MELHOR DE 3 - RODADA 1
    PLACAR: Machine [ 0 ] x [ 0 ] Jogador
   ========================================
   Pressione qualquer tecla para jogar os dados...
   ```
3. Pressione espaco ou qualquer tecla para rolar os dados e avancar as rodadas.
