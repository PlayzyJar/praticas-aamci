# Pratica Interrupcao Serial (Protocolo com Handshake na USART2)

Protocolo de comunicacao serial estruturado sobre a USART2 (115200 8N1) com confirmacao de entrega (ACK), recuperacao automatica de erro de overrun (`ORE`) e debounce nao-bloqueante por amostragem de tempo (`HAL_GetTick`).

## Protocolo

- **Comandos:**
  - `CMD_USER_BTN (0xAA)`: Enviado ao pressionar o botao `PC13`. Comuta LD1 (Verde) e responde com `ACK_BYTE`.
  - `CMD_EXT_BTN (0xBB)`: Enviado ao pressionar o botao externo `PE0`. Comuta LD2 (Azul) e responde com `ACK_BYTE`.
  - `ACK_BYTE (0xCC)`: Confirmacao de recebimento enviada pelo receptor.
- **Timeout:** Transmissor aguarda o byte `0xCC` por ate 50 ms. Se expirar, comuta o LED local indicando falha.
- **Tratamento ORE:** A cada ciclo do loop, limpa a flag `UART_FLAG_ORE` com `__HAL_UART_CLEAR_OREFLAG(&huart2)` para evitar travamento da recepcao.

## Pinagem

| Recurso | Pino | Modo | Descricao |
|---|---|---|---|
| USART2 TX | `PD5` | AF7 | Transmissao de comandos/ACK |
| USART2 RX | `PA3` | AF7 | Recepcao de comandos/ACK |
| Botao B1 | `PC13` | Input | Envia `0xAA` |
| Botao Ext | `PE0` | Input | Envia `0xBB` |
| LD1 (Verde) | `PB0` | Output | Alternado por `0xAA` |
| LD2 (Azul) | `PB7` | Output | Alternado por `0xBB` |

## Teste

1. **Loopback local (1 placa):** Conecte um jumper entre `PD5` (TX) e `PA3` (RX). Pressione `PC13`: o microcontrolador transmite `0xAA`, recebe seu proprio pacote, responde `0xCC` e alterna o LED verde (`LD1`).
2. **Entre duas placas:** Conecte `PD5` (TX) da placa A em `PA3` (RX) da placa B, `PA3` (RX) da placa A em `PD5` (TX) da placa B, e compartilhe o GND.
