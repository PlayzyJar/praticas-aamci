# Comunicacao Serial 767 (1-Wire Bit-Banging com Handshake)

Protocolo serial proprietario half-duplex em linha unica por software (bit-banging em `PE2`) com temporizacao via TIM2, debounce temporal de botoes e confirmacao ativa (ACK).

## Protocolo

- **Taxa de bits:** 50 us por bit (`TEMPO_BIT = 50`).
- **Estrutura do frame:**
  - Start bit: nivel baixo (50 us).
  - 8 bits de dados: transmitidos do LSB para o MSB (50 us cada).
  - Stop bit: nivel alto (50 us).
- **Handshake (ACK):**
  - Apos o Stop bit, o transmissor aguarda ate 200 us (4x bit time) pelo ACK.
  - O receptor responde puxando a linha para nivel baixo por 150 us (3x bit time).
  - Se nao houver ACK no tempo limite, `envia_cmd` retorna falha (`0`).
- **Comandos:**
  - `0xAA`: Enviado pelo botao de usuario (`PC13`). Alterna LD1 (Verde) no receptor.
  - `0xBB`: Enviado pelo botao externo (`PE0`). Alterna LD2 (Azul) no receptor.
  - Na falha de handshake, o no transmissor comuta o seu proprio LED como alerta.

## Pinagem

| Recurso | Pino | Modo | Descricao |
|---|---|---|---|
| Linha Serial | `PE2` | Open-Drain / Bi-direcional | Barramento compartilhado 1-wire |
| Botao B1 | `PC13` | Input | Dispara envio de `0xAA` |
| Botao Ext | `PE0` | Input com Pull-Up | Dispara envio de `0xBB` |
| LD1 (Verde) | `PB0` | Output | Alvo de `0xAA` |
| LD2 (Azul) | `PB7` | Output | Alvo de `0xBB` |
| TIM2 | Interno | Timer 32 bits | Temporizacao em microssegundos |

## Teste

- **Placa isolada (teste de falha de ACK):** Aperte o botao azul `PC13`. Como nao ha receptor em `PE2` para responder com ACK, o transmissor detecta a falha e comuta o LED verde (LD1) local.
- **Entre duas placas STM32:** Conecte `PE2` da placa A em `PE2` da placa B, e una os GNDs. Pressionar `PC13` na placa A alterna o LD1 da placa B.
