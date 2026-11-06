# Pratica IO Putchar

Redirecionamento de `printf` da biblioteca padrao C para a USART3 atraves da implementacao de `__io_putchar`, acompanhado de temporizacao precisa com TIM2.

## Funcionamento

- `__io_putchar(int ch)` envia cada caractere atraves de `HAL_UART_Transmit(&huart3, ...)`.
- TIM2 configurado com prescaler para contar a cada 1 us (`TIM2_Micros_Init`), provendo `Delay_us` e `Delay_ms`.
- O loop principal transmite uma mensagem formatada com contador sequencial e `HAL_GetTick()`, alternando o LED verde (LD1) a cada 1 segundo.

## Pinagem

| Recurso | Pino | Modo | Descricao |
|---|---|---|---|
| USART3 TX/RX | `PD8` / `PD9` | AF7 | VCP via ST-LINK (115200 8N1) |
| LD1 (Verde) | `PB0` | Output | Heartbeat (comuta a cada 1 s) |
| TIM2 | Interno | Timer 32 bits | Base de tempo em microssegundos |

## Teste

1. Abra o terminal serial em 115200 8N1.
2. Observe a saida a cada 1 segundo:
   ```text
   ========================================
     STM32F767ZI - Teste USART3 __io_putchar  
   ========================================
   Mensagem #0 | Tick: 2 ms
   Mensagem #1 | Tick: 1002 ms
   ```
3. O LED LD1 comuta a cada transmissao.
