# Prática: Hello World 767 (GPIO & USART3)

Projeto inicial para validação de periféricos básicos da placa **NUCLEO-F767ZI**: leitura de entrada digital (Botão de Usuário), escrita em saídas digitais (LEDs coloridos) e transmissão serial formatada via USART3.

---

## 📋 Objetivo do Firmware

1. Ler o estado lógico do botão azul `B1` (`PC13`).
2. Transmitir periodicamente via porta serial USART3 a string `estado: <0 ou 1>\r\n`.
3. Ao pressionar o botão, acender simultaneamente os 3 LEDs on-board (`LD1`, `LD2` e `LD3`).
4. Enquanto o botão for mantido pressionado, os LEDs permanecem acesos; ao soltar, todos os LEDs são apagados após 200 ms.

---

## 🔌 Hardware e Pinagem

| Sinal / Recurso | Pino STM32 | Modo GPIO | Descrição |
|---|---|---|---|
| **Botão de Usuário B1** | `PC13` | Entrada Digital (Input) | Botão azul on-board. Nível ALTO (`1`) quando pressionado. |
| **LED 1 (Verde)** | `PB0` | Saída Digital (Push-Pull) | LD1 da placa NUCLEO. |
| **LED 2 (Azul)** | `PB7` | Saída Digital (Push-Pull) | LD2 da placa NUCLEO. |
| **LED 3 (Vermelho)** | `PB14` | Saída Digital (Push-Pull) | LD3 da placa NUCLEO. |
| **USART3 TX** | `PD8` | Função Alternativa (AF7) | Transmissão serial conectada ao ST-LINK USB. |
| **USART3 RX** | `PD9` | Função Alternativa (AF7) | Recepção serial conectada ao ST-LINK USB. |

---

## ⚙️ Compilação e Gravação

1. No **STM32CubeIDE**, selecione a pasta `hello-world-767` no *Project Explorer*.
2. Pressione `Ctrl + B` para compilar o projeto.
3. Conecte a placa NUCLEO-F767ZI via cabo USB no conector `CN1`.
4. Clique com o botão direito no projeto -> **Run As** -> **STM32 C/C++ Application**.

---

## 🧪 Como Testar e Validar

1. Abra um terminal serial (ex: `picocom`, `minicom`, PuTTY ou monitor serial do VS Code):
   * **Porta:** `/dev/ttyACM0` (Linux) ou `COMx` (Windows)
   * **Baud Rate:** `115200`
   * **Configuração:** 8N1 (8 bits, Sem paridade, 1 stop bit)
2. **Sem pressionar o botão:** Você verá mensagens contínuas:
   ```text
   estado: 0
   estado: 0
   ```
3. **Pressione e segure o botão azul B1 (`PC13`):**
   * Os 3 LEDs (`LD1`, `LD2`, `LD3`) acenderão juntos imediatamente.
   * O terminal exibirá:
     ```text
     estado: 1
     ```
4. **Solte o botão:** Os LEDs se apagam e as mensagens voltam a indicar `estado: 0`.

---

## 🤖 Prompt Pronto para Agente IA

Copie e envie o bloco abaixo caso queira que uma IA te guie neste projeto:

```text
Estou rodando o projeto "hello-world-767" na placa NUCLEO-F767ZI.
O firmware lê o botão PC13 e controla os LEDs PB0, PB7 e PB14 enquanto envia "estado: X" na USART3 (115200 bps).
Me guie passo a passo na validação no STM32CubeIDE e no monitoramento via terminal serial.
```
