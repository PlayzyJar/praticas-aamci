# Praticas AAMCI - STM32F767ZI

Projetos práticos desenvolvidos para a disciplina AAMCI (Microcontroladores e Microprocessadores II) utilizando a placa NUCLEO-F767ZI (STM32F767ZIT6, ARM Cortex-M7 @ 216MHz).

## Hardware Base (NUCLEO-F767ZI)

Recursos onboard padrao compartilhados entre os projetos:

- **USART3 (ST-LINK Virtual COM Port):** TX em `PD8`, RX em `PD9` (115200 bps, 8N1).
- **Botao de Usuario (B1):** `PC13` (nivel logico alto ao pressionar).
- **LEDs de Diagnostico:**
  - LD1 (Verde): `PB0`
  - LD2 (Azul): `PB7`
  - LD3 (Vermelho): `PB14`

## Compilacao e Gravacao

1. Abra o STM32CubeIDE.
2. `File` -> `Import...` -> `General` -> `Existing Projects into Workspace`.
3. Selecione este repositorio como raiz e marque o projeto desejado.
4. Compile com `Ctrl + B` e execute via `Run As` -> `STM32 C/C++ Application` (ST-LINK/V2-1).
5. Abra o terminal serial na porta da placa (`/dev/ttyACM0` no Linux ou `COMx` no Windows) a 115200 bps, 8N1.

## Projetos

- `hello-world-767`: Leitura de GPIO (botao PC13) e acionamento de LEDs com envio de status na USART3.
- `pratica-io-putchar`: Redirecionamento de `printf` (`__io_putchar`) pela USART3 e atrasos com TIM2.
- `pratica-serial-teclado`: Recepcao serial na USART3 controlando LEDs (teclas R, G, B).
- `comunicacao-serial-767`: Protocolo serial 1-wire por software (bit-banging em PE2) com handshake.
- `pratica-interrupcao-serial`: Protocolo serial via USART2 com pacotes de comando, ACK e tratamento de overrun.
- `pratica-rng`: Geracao de numeros aleatorios com True RNG de hardware e jogo Melhor de 3 via terminal ANSI.
- `pratica-rng-estimativa-pi`: Estimativa de Pi por Monte Carlo usando True RNG de hardware e aritmetica de 64 bits.
- `pratica-rtc-767`: Relogio de tempo real (RTC) com ajuste de calendario via string serial de 16 digitos.
- `pratica-spi-bmp`: Leitura de pressao, temperatura e altitude do sensor Bosch BMP280 via SPI1.
- `display-tft-f767`: Driver para display OLED SSD1306 (128x64) via I2C1 com animacoes procedurais e conversor de frames Python.
