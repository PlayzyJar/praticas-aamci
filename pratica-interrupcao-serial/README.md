# Prática: Interrupção & Protocolo Serial com Handshake (`pratica-interrupcao-serial`)

Protocolo de comunicação serial robusto bidirecional sobre a interface **USART2** (`115200 bps`), incluindo pacote de comando e resposta de confirmação (*ACK*), máquina de estados no processamento de bytes, recuperação automática de erro de *Overrun* (`ORE`) e debouncing não-bloqueante por amostragem temporal.

---

## 📋 Arquitetura do Protocolo

O módulo `serial_protocol.c` implementa um parser determinístico:

* **Tabela de Comandos:**
  * `CMD_USER_BTN = 0xAA`: Enviado ao pressionar o botão de usuário `B1` (`PC13`). Aciona a ação `ACTION_TOGGLE_LD1` e devolve `ACK_BYTE`.
  * `CMD_EXT_BTN = 0xBB`: Enviado ao pressionar o botão externo `PE0`. Aciona a ação `ACTION_TOGGLE_LD2` e devolve `ACK_BYTE`.
  * `ACK_BYTE = 0xCC`: Byte de reconhecimento enviado imediatamente pelo receptor para confirmar a execução.
* **Timeout de Confirmação:** O transmissor aguarda resposta em até `50 ms` usando `HAL_GetTick()`. Se o ACK não for recebido dentro da janela temporal, a função indica falha de handshake e comuta o LED local.
* **Resiliência a Erros de UART:** O loop verifica e limpa proativamente a flag de sobrecarga (`__HAL_UART_CLEAR_OREFLAG(&huart2)`), impedindo que recepções desalinhadas travem o hardware receptor.

---

## 🔌 Hardware e Pinagem

| Sinal / Recurso | Pino STM32 | Modo / Função | Descrição |
|---|---|---|---|
| **USART2 TX** | `PD5` | AF7 (USART2_TX) | Transmissão de comandos e ACK para outro nó / serial. |
| **USART2 RX** | `PA3` | AF7 (USART2_RX) | Recepção de comandos e ACK. |
| **Botão de Usuário B1** | `PC13` | Entrada Digital (Push-Button) | Dispara o envio de `0xAA` via USART2. |
| **Botão Externo (BTN_2)** | `PE0` | Entrada Digital com Pull-Up | Dispara o envio de `0xBB` via USART2. |
| **LED 1 (Verde)** | `PB0` | Saída Digital (`LD1_Pin`) | Alternado ao receber `0xAA` (ou na falha de ACK local). |
| **LED 2 (Azul)** | `PB7` | Saída Digital (`LD2_Pin`) | Alternado ao receber `0xBB` (ou na falha de ACK local). |
| **USART3 (VCP)** | `PD8`/`PD9` | AF7 (USART3) | Porta auxiliar ST-LINK ativa. |

---

## ⚙️ Compilação e Gravação

1. No **STM32CubeIDE**, selecione `pratica-interrupcao-serial`.
2. Compile o código pressionando `Ctrl + B`.
3. Conecte a placa via cabo USB `CN1` e execute **Run As** -> **STM32 C/C++ Application**.

---

## 🧪 Como Testar e Validar

### Teste em Loopback Local (Eco Direto)
1. Conecte um jumper entre **`PD5` (USART2_TX)** e **`PA3` (USART2_RX)** no conector Morpho da placa.
2. Pressione o botão azul `PC13`:
   * O STM32 transmitirá `0xAA` por `PD5` e receberá imediatamente por `PA3`.
   * O parser responderá enviando `0xCC` (ACK) e alternará o **LED Verde (`LD1`)**.
   * O transmissor receberá seu próprio ACK dentro de 50 ms com sucesso.
3. Repita pressionando o botão externo em `PE0` (ou aterrando `PE0` brevemente): o **LED Azul (`LD2`)** alternará.

### Teste entre Duas Placas NUCLEO
1. Ligue `PD5` (TX) da Placa 1 em `PA3` (RX) da Placa 2.
2. Ligue `PA3` (RX) da Placa 1 em `PD5` (TX) da Placa 2.
3. Conecte os pinos `GND` de ambas as placas.
4. Pressionar `PC13` na Placa 1 comutará o `LD1` da Placa 2 remotamente e devolverá o ACK para a Placa 1.

---

## 🤖 Prompt Pronto para Agente IA

```text
Estou analisando o projeto "pratica-interrupcao-serial" na NUCLEO-F767ZI.
Ele utiliza a USART2 (TX em PD5, RX em PA3 a 115200 bps) para trocar pacotes de comando 0xAA/0xBB e ACK 0xCC com tratamento de ORE.
Me guie no teste de loopback físico entre PD5 e PA3 e na validação do parser de comandos.
```
