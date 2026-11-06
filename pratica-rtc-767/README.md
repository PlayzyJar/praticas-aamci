# Pratica RTC 767 (Real-Time Clock)

Operacao do periferico de hardware RTC interno da STM32F767ZI, com desbloqueio do dominio de backup e parser serial para sincronizacao do calendario.

## Funcionamento

- Desbloqueia escrita no RTC via `HAL_PWR_EnableBkUpAccess()` e chaves de protecao `0xCA` e `0x53` em `RTC->WPR`.
- Imprime a data e hora a cada 1 segundo na USART3.
- Recebe uma string numerica de 16 digitos para reconfigurar o relogio em tempo de execucao.

## Formato do Comando Serial (16 digitos)

Formato: `DSDMMMAAAAHHMNSS`
- `DS` (01 a 07): Dia da semana (01 = Segunda ... 07 = Domingo)
- `DM` (01 a 31): Dia do mes
- `MM` (01 a 12): Mes
- `AAAA` (4 digitos): Ano (ex: 2026)
- `HH` (00 a 23): Hora
- `MN` (00 a 59): Minuto
- `SS` (00 a 59): Segundo

Exemplo para configurar Sexta-feira, 06/11/2026 as 14:00:00:
```text
0506112026140000
```

## Pinagem

| Recurso | Pino | Modo | Descricao |
|---|---|---|---|
| RTC | Interno | - | Contador de calendario de hardware |
| USART3 TX/RX | `PD8` / `PD9` | AF7 | VCP via ST-LINK (115200 8N1) |
| LD1 (Verde) | `PB0` | Output | Alterna a cada caractere recebido |

## Teste

1. Abra o terminal serial em 115200 8N1.
2. O relogio imprimira os segundos continuamente (`[Mon] 01/01/00 - 00:00:01`).
3. Digite a string de 16 digitos e envie com Enter: o relogio sera atualizado de imediato.
