# Pratica Serial Teclado

Controle interativo de LEDs onboard a partir de comandos de teclado recebidos pela USART3.

## Funcionamento

O loop monitora a flag de hardware `USART_ISR_RXNE` no registrador `USART3->ISR` de forma nao-bloqueante. Ao receber um caractere, comuta o respectivo LED de acordo com o comando:

- `'R'`: Alterna o LED vermelho (LD3).
- `'G'`: Alterna o LED verde (LD1).
- `'B'`: Alterna o LED azul (LD2).

## Pinagem

| Recurso | Pino | Modo | Descricao |
|---|---|---|---|
| USART3 TX/RX | `PD8` / `PD9` | AF7 | VCP via ST-LINK (115200 8N1) |
| LD1 (Verde) | `PB0` | Output | Controlado pelo comando 'G' |
| LD2 (Azul) | `PB7` | Output | Controlado pelo comando 'B' |
| LD3 (Vermelho) | `PB14` | Output | Controlado pelo comando 'R' |

## Teste

1. Abra o terminal serial em 115200 8N1.
2. Pressione as teclas `R`, `G` ou `B` no terminal.
3. Observe os respectivos LEDs onboard alternando de estado instantaneamente.
