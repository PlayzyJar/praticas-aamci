# Prática: Comunicação Serial Customizada 1-Wire com Handshake (`comunicacao-serial-767`)

Implementação de um protocolo serial proprietário a 1 fio (*single-wire half-duplex bit-banging*) em nível de registrador com temporização em microssegundos baseada no hardware **TIM2**, incluindo confirmação de entrega (*handshake ativo / ACK*) e debouncing não-bloqueante de botões.

---

## 📋 Arquitetura do Protocolo de Comunicação

O protocolo utiliza uma única linha digital bidirecional (`PE2`) compartilhada entre dois nós (ou em laço de teste com temporização precisa):

1. **Taxa de Bits:** `TEMPO_BIT = 50 µs` por bit (~20.000 baud efetivos).
2. **Estrutura do Frame Transmitido:**
   * **Start Bit:** Nível lógico BAIXO (`0`) por 50 µs.
   * **8 Bits de Dados:** Transmitidos do bit menos significativo (LSB) para o mais significativo (MSB), 50 µs por bit.
   * **Stop Bit:** Nível lógico ALTO (`1`) por 50 µs.
3. **Mecanismo de Handshake (ACK Ativo):**
   * Imediatamente após o envio do Stop bit, o transmissor coloca o pino em modo leitura e aguarda até `200 µs` (4x o tempo de bit).
   * O receptor, ao validar o Start bit, 8 bits e Stop bit válidos, puxa a linha para nível BAIXO por `150 µs` (3x o tempo de bit) como confirmação de sucesso (ACK) e depois libera a linha.
   * Se o transmissor não detectar a linha em nível baixo dentro do timeout, o envio é classificado como falha de handshake.

---

## 🔌 Hardware e Pinagem

| Sinal / Recurso | Pino STM32 | Modo GPIO | Descrição |
|---|---|---|---|
| **Barramento 1-Wire (SERIAL)** | `PE2` | Saída/Entrada Open-Drain c/ Pull-Up | Linha de comunicação serial compartilhada com handshake. |
| **Botão de Usuário B1** | `PC13` | Entrada Digital (Push-Button) | Dispara o comando `0xAA` (comando para alternar LD1). |
| **Botão Externo (BTN_2)** | `PE0` | Entrada Digital com Pull-Up | Dispara o comando `0xBB` (comando para alternar LD2). |
| **LED 1 (Verde)** | `PB0` | Saída Digital (`LD1_Pin`) | Alternado ao receber `0xAA` (ou na falha de ACK do B1). |
| **LED 2 (Azul)** | `PB7` | Saída Digital (`LD2_Pin`) | Alternado ao receber `0xBB` (ou na falha de ACK do PE0). |
| **LED 3 (Vermelho)** | `PB14` | Saída Digital (`LD3_Pin`) | Reservado para sinalização de status/alerta. |
| **Timer de Precisão** | `TIM2` | 32-bit Timer @ 1 MHz | Base para `Delay_us()` e medição de timeout de ACK. |

---

## ⚙️ Compilação e Gravação

1. No **STM32CubeIDE**, selecione `comunicacao-serial-767`.
2. Compile o firmware com `Ctrl + B`.
3. Grave na placa via **Run As** -> **STM32 C/C++ Application**.

---

## 🧪 Como Testar e Validar

### Cenário 1: Teste com Placa Única (Validação de Falha de Handshake)
Sem nenhum outro dispositivo conectado ao pino `PE2`:
1. Pressione o botão azul `PC13`: o microcontrolador transmitirá o byte `0xAA` por `PE2`. Como não há receptor para responder o ACK, o transmissor detecta a falha e **alterna o LED verde (LD1) local** como alerta de ausência de handshake.
2. Pressione o botão externo conectado em `PE0` (ou aterre o pino `PE0` brevemente com um jumper): o microcontrolador transmitirá `0xBB` e, ao não receber ACK, **alternará o LED azul (LD2) local**.

### Cenário 2: Comunicação entre Duas Placas STM32
1. Conecte o pino `PE2` da Placa A ao pino `PE2` da Placa B.
2. Conecte o pino `GND` da Placa A ao `GND` da Placa B (referência comum obrigatória).
3. Ao pressionar `PC13` na Placa A:
   * A Placa A transmite `0xAA`.
   * A Placa B recebe `0xAA`, puxa `PE2` para GND por 150 µs (ACK) e **alterna seu LED Verde (LD1)**.
   * A Placa A detecta o ACK com sucesso e seu LED local **não** pisca erro.

---

## 🤖 Prompt Pronto para Agente IA

```text
Estou analisando a prática "comunicacao-serial-767" na placa NUCLEO-F767ZI.
Ela implementa um protocolo 1-wire customizado por bit-banging em PE2 a 50 us por bit com TIM2 e handshake de 150 us.
Explique a temporização de envio/recebimento, o tratamento de debounce não-bloqueante dos botões PC13/PE0 e como simular o handshake em bancada.
```
