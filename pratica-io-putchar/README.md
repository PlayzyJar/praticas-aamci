# Prática: I/O Putchar (Redirecionamento de Printf & Delays TIM2)

Demonstração de redirecionamento de saída padrão da biblioteca C (`printf` via implementação de `__io_putchar`) para a porta serial USART3 do ST-LINK, associada a uma base de tempo em microssegundos configurada sobre o temporizador de hardware **TIM2**.

---

## 📋 Objetivo do Firmware

1. Implementar a função de baixo nível `__io_putchar(int ch)` para que qualquer chamada a `printf(...)` envie dados formatados automaticamente pela `huart3`.
2. Configurar o temporizador de 32 bits **TIM2** com prescaler para tick de 1 microssegundo (`TIM2_Micros_Init`), permitindo funções de atraso precisas e não-bloqueantes (`Delay_us` e `Delay_ms`).
3. Transmitir periodicamente via serial um cabeçalho formatado, número de mensagem sequencial e o carimbo de tempo em milissegundos (`HAL_GetTick()`), alternando o estado do LED verde (`LD1`) a cada 1 segundo.

---

## 🔌 Hardware e Pinagem

| Sinal / Recurso | Pino STM32 | Modo / Função | Descrição |
|---|---|---|---|
| **USART3 TX** | `PD8` | AF7 (USART3_TX) | Transmissão de caracteres formatados para o PC. |
| **USART3 RX** | `PD9` | AF7 (USART3_RX) | Recepção serial (ST-LINK VCP). |
| **LED 1 (Verde)** | `PB0` | Saída Digital (`LD1_Pin`) | Pisca a cada 1000 ms indicando heartbeat da aplicação. |
| **Timer 2** | Interno | Timer Base (Contador 32 bits) | Base de tempo com resolução de 1 µs para medições e delays. |

---

## ⚙️ Compilação e Gravação

1. No **STM32CubeIDE**, selecione `pratica-io-putchar`.
2. Compile o projeto via atalho `Ctrl + B`.
3. Conecte a NUCLEO-F767ZI via cabo USB.
4. Execute via **Run As** -> **STM32 C/C++ Application**.

---

## 🧪 Como Testar e Validar

1. Abra um terminal serial:
   * **Porta:** `/dev/ttyACM0` (Linux) ou `COMx` (Windows)
   * **Baud Rate:** `115200`
   * **Configuração:** 8N1
2. Pressione o botão preto `RESET` na placa.
3. Você verá o banner inicial seguido pelo contador de mensagens a cada 1 segundo:
   ```text
   ========================================
     STM32F767ZI - Teste USART3 __io_putchar  
   ========================================
   Mensagem #0 | Tick: 2 ms
   Mensagem #1 | Tick: 1002 ms
   Mensagem #2 | Tick: 2002 ms
   Mensagem #3 | Tick: 3002 ms
   ```
4. Simultaneamente, o **LED Verde (LD1)** alternará de ligado para desligado a cada segundo de forma sincronizada com o envio das mensagens.

---

## 🤖 Prompt Pronto para Agente IA

```text
Estou testando o projeto "pratica-io-putchar" na NUCLEO-F767ZI.
Ele utiliza o redirecionamento de printf via __io_putchar na USART3 (115200 bps) e temporização em microsegundos com TIM2.
Me auxilie a compilar, monitorar a saída serial e entender o funcionamento do retargeting de I/O na arquitetura ARM Cortex-M7.
```
