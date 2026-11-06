# Prática: Serial Teclado (Controle de LEDs via Terminal USART3)

Recepção de caracteres pela porta serial USART3 sem bloqueio da CPU (verificação da flag `RXNE` nos registradores de hardware), permitindo controlar interativamente os três LEDs coloridos on-board através de comandos de teclado digitados no terminal.

---

## 📋 Objetivo do Firmware

1. Monitorar o registrador de status da USART3 (`USART3->ISR`) verificando se um novo caractere foi recebido (`USART_ISR_RXNE`).
2. Fazer a leitura imediata do dado no registrador de dados (`USART3->RDR`) para esvaziar o buffer de hardware.
3. Interpretar os comandos de tecla simples:
   * **`R` ou `r`:** Alterna o estado do **LED 3 (Vermelho - `PB14`)**.
   * **`G` ou `g`:** Alterna o estado do **LED 1 (Verde - `PB0`)**.
   * **`B` ou `b`:** Alterna o estado do **LED 2 (Azul - `PB7`)**.
4. Manter o loop principal responsivo sem uso de delays bloqueantes para recepção.

---

## 🔌 Hardware e Pinagem

| Sinal / Recurso | Pino STM32 | Modo / Função | Descrição |
|---|---|---|---|
| **USART3 TX** | `PD8` | AF7 (USART3_TX) | Linha de transmissão serial (ST-LINK VCP). |
| **USART3 RX** | `PD9` | AF7 (USART3_RX) | Linha de recepção de caracteres vindos do teclado do PC. |
| **LED 1 (Verde - G)** | `PB0` | Saída Digital (`LD1_Pin`) | Alternado ao enviar o caractere `'G'`. |
| **LED 2 (Azul - B)** | `PB7` | Saída Digital (`LD2_Pin`) | Alternado ao enviar o caractere `'B'`. |
| **LED 3 (Vermelho - R)** | `PB14` | Saída Digital (`LD3_Pin`) | Alternado ao enviar o caractere `'R'`. |

---

## ⚙️ Compilação e Gravação

1. No **STM32CubeIDE**, selecione `pratica-serial-teclado`.
2. Compile o código com `Ctrl + B`.
3. Grave na placa via **Run As** -> **STM32 C/C++ Application**.

---

## 🧪 Como Testar e Validar

1. Abra seu terminal serial preferido:
   * **Porta:** `/dev/ttyACM0` (Linux) ou `COMx` (Windows)
   * **Baud Rate:** `115200`
   * **Configuração:** 8N1
2. Com a janela do terminal em foco, digite as teclas (não é necessário pressionar Enter em terminais raw como `picocom`):
   * Digite **`R`**: O LED vermelho (`LD3`) liga ou desliga.
   * Digite **`G`**: O LED verde (`LD1`) liga ou desliga.
   * Digite **`B`**: O LED azul (`LD2`) liga ou desliga.
3. Você pode combinar os estados para acender 1, 2 ou todos os 3 LEDs ao mesmo tempo.

---

## 🤖 Prompt Pronto para Agente IA

```text
Estou rodando a "pratica-serial-teclado" na STM32F767ZI.
O firmware lê caracteres da USART3 e comuta os LEDs LD1 (Verde - PB0), LD2 (Azul - PB7) e LD3 (Vermelho - PB14) ao receber 'G', 'B' e 'R'.
Me ajude a configurar o terminal serial e validar a recepção de comandos sem bloqueio.
```
