# Relatório de Execução - Task 2: Integração no Firmware (`main.c`)

## 1. Visão Geral
Integração do módulo Monte Carlo `pi_mc` ao firmware STM32F767ZI (`Core/Src/main.c`), retarget da saída de `printf` para USART3 (VCP ST-LINK), injeção do periférico de hardware RNG e execução contínua com impressão a cada 1.000.000 de pontos amostrados (versão `Mpontos` em milhões).

## 2. Modificações em `Core/Src/main.c`
Todas as alterações foram realizadas estritamente dentro dos blocos delimitados por comentários `USER CODE`, preservando integralmente o padrão de quebras de linha CRLF (`\r\n`):

### 2.1. Inclusão de Cabeçalhos (`USER CODE BEGIN Includes`)
```c
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdint.h>
#include "pi_mc.h"
/* USER CODE END Includes */
```

### 2.2. Retarget de `printf` e Gerador de RNG por Hardware (`USER CODE BEGIN 0`)
```c
/* USER CODE BEGIN 0 */
/* printf -> USART3 (VCP do ST-LINK). Chamado por _write() em syscalls.c */
int __io_putchar(int ch)
{
  uint8_t c = (uint8_t)ch;
  HAL_UART_Transmit(&huart3, &c, 1, HAL_MAX_DELAY);
  return ch;
}

/* Fonte de aleatoriedade: periférico RNG (32 bits por chamada). */
static uint32_t hw_rng(void)
{
  uint32_t v;
  if (HAL_RNG_GenerateRandomNumber(&hrng, &v) != HAL_OK)
  {
    Error_Handler();
  }
  return v;
}
/* USER CODE END 0 */
```

### 2.3. Inicialização do Monte Carlo e Banner (`USER CODE BEGIN 2`)
```c
  /* USER CODE BEGIN 2 */
  pi_mc_t pi;
  pi_mc_init(&pi);
  printf("\r\nEstimativa de pi por Monte Carlo (RNG STM32F767)\r\n");
  /* USER CODE END 2 */
```

### 2.4. Laço de Execução e Impressão em Milhões (`USER CODE BEGIN 3`)
```c
    /* USER CODE BEGIN 3 */
    pi_mc_run(&pi, hw_rng, PI_MC_BATCH);
    uint64_t micro = pi_mc_micro(&pi);
    printf("Mpontos=%lu pi=%lu.%06lu\r\n",
           (unsigned long)(pi.total / 1000000u),
           (unsigned long)(micro / 1000000u),
           (unsigned long)(micro % 1000000u));
  }
  /* USER CODE END 3 */
```

## 3. Verificações Executadas (Step 4)

### 3.1. Localização do Toolchain ARM
O compilador `arm-none-eabi-gcc` foi localizado na instalação local do STM32CubeIDE:
- Caminho: `/var/home/carloseliasf/st/stm32cubeide_2.2.0/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.14.3.rel1.linux64_1.0.100.202602081740/tools/bin/arm-none-eabi-gcc`
- Versão: GNU Tools for STM32 14.3.rel1 (14.3.1 20250623)

### 3.2. Compilação Cruzada dos Fontes (Cortex-M7)
Comando executado para compilação do `main.c`:
```bash
/var/home/carloseliasf/st/stm32cubeide_2.2.0/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.14.3.rel1.linux64_1.0.100.202602081740/tools/bin/arm-none-eabi-gcc \
  -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard -std=gnu11 \
  -DUSE_HAL_DRIVER -DSTM32F767xx \
  -ICore/Inc \
  -IDrivers/STM32F7xx_HAL_Driver/Inc \
  -IDrivers/STM32F7xx_HAL_Driver/Inc/Legacy \
  -IDrivers/CMSIS/Device/ST/STM32F7xx/Include \
  -IDrivers/CMSIS/Include \
  -Wall -c Core/Src/main.c -o /tmp/main.o
```
Resultado: **Código de retorno 0**, sem erros e sem avisos (`0 errors, 0 warnings`).

Compilação do `pi_mc.c` com as mesmas diretivas:
Resultado: **Código de retorno 0**, sem erros e sem avisos.

### 3.3. Conferência de Blocos e Quebras de Linha CRLF
1. Conferência manual com `grep -n "pi_mc\|hw_rng\|__io_putchar" Core/Src/main.c`:
   - Linha 27: em `USER CODE BEGIN Includes`
   - Linhas 92 e 100: em `USER CODE BEGIN 0`
   - Linhas 148-149: em `USER CODE BEGIN 2`
   - Linhas 160-161: em `USER CODE BEGIN 3`
2. Validação do formato CRLF:
   - Total `\r\n`: 491; `\n` isolados: 0. Fim de linha CRLF perfeitamente preservado.

### 3.4. Regressão dos Testes Unitários da Task 1
Comando executado:
```bash
gcc -std=gnu11 -Wall -Wextra -ICore/Inc tests/test_pi_mc.c Core/Src/pi_mc.c -o /tmp/test_pi_mc && /tmp/test_pi_mc
```
Saída:
```text
pi ~ 3.142736
OK
```

## 4. Instruções para o Usuário
1. Abrir o projeto no **STM32CubeIDE**.
2. Executar **Project > Build Project** para gerar o binário `.elf`.
3. Conectar a placa **Nucleo-F767ZI** via cabo USB (conector ST-LINK/VCP).
4. Gravar/Depurar (**Run** ou **Debug**).
5. Abrir um terminal serial apontando para a porta serial virtual do ST-LINK (ex.: `/dev/ttyACM0`):
   - **Baud Rate:** 115200 (configurado em `huart3.Init.BaudRate` no `main.c`)
   - **Data bits:** 8
   - **Parity:** None
   - **Stop bits:** 1
   - **Flow control:** None

## 5. Status e Conclusão
- **Status:** CONCLUÍDO COM SUCESSO (PASS)
- **Preocupações:** Nenhuma. A formatação com `Mpontos=%lu` garante que o contador não sofra overflow na impressão de 32 bits, a verificação com o compilador cruzado ARM real da ST confirmou a compatibilidade de tipos e inclusões, e todas as modificações respeitam o isolamento de regeneração do CubeMX.
