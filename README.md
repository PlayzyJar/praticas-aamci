# Monorepo de Práticas — AAMCI (STM32F767ZI)

Repositório central de firmwares práticos desenvolvidos para a disciplina **AAMCI (Microcontroladores e Microprocessadores II)**, utilizando a plataforma de desenvolvimento **STMicroelectronics NUCLEO-F767ZI**.

Este monorepo foi projetado tanto para estudantes e desenvolvedores quanto para **Agentes Autônomos de Inteligência Artificial (LLMs/Coding Agents)**, contendo instruções explícitas de pinagem, compilação, gravação e validação em hardware para cada projeto.

---

## 🎯 Especificações da Placa-Alvo

* **Placa:** STMicroelectronics NUCLEO-144 F767ZI
* **Microcontrolador:** STM32F767ZIT6
  * Núcleo: ARM® 32-bit Cortex®-M7 com FPU de precisão dupla (DP-FPU) e DSP
  * Frequência Máxima de Clock: 216 MHz
  * Memória Flash: 2 MB
  * SRAM: 512 KB (incluindo 128 KB de DTCM RAM)
* **Gravador/Depurador Embutido:** ST-LINK/V2-1 integrado (conector Micro-USB CN1)
* **Porta Serial Virtual (VCP):** Roteada internamente para a **USART3**

### Pinout Padrão de Recursos On-Board (NUCLEO-F767ZI)

| Recurso | Identificação | Pino STM32 | Características / Nível Lógico |
|---|---|---|---|
| **Virtual COM Port (VCP)** | USART3 TX | `PD8` | Transmissão serial para o PC via ST-LINK |
| **Virtual COM Port (VCP)** | USART3 RX | `PD9` | Recepção serial vinda do PC via ST-LINK |
| **LED 1 (Verde)** | LD1 | `PB0` | Nível ALTO (`GPIO_PIN_SET`) acende |
| **LED 2 (Azul)** | LD2 | `PB7` | Nível ALTO (`GPIO_PIN_SET`) acende |
| **LED 3 (Vermelho)** | LD3 | `PB14` | Nível ALTO (`GPIO_PIN_SET`) acende |
| **Botão de Usuário (B1)** | USER_Btn (Azul) | `PC13` | Nível ALTO (`GPIO_PIN_SET`) quando pressionado |
| **Botão de Reset (B2)** | RESET (Preto) | `NRST` | Reinicializa o microcontrolador |

---

## 📂 Matriz de Práticas e Projetos

| Projeto / Diretório | Periféricos & Interfaces | Descrição Sucinta | Documentação |
|---|---|---|---|
| [`hello-world-767`](./hello-world-767/) | GPIO, USART3 | Teste básico de I/O: leitura do botão `PC13`, transmissão serial do estado e acionamento síncrono dos LEDs LD1, LD2 e LD3. | [Ver README](./hello-world-767/README.md) |
| [`pratica-io-putchar`](./pratica-io-putchar/) | USART3, TIM2 | Redirecionamento da libc (`printf` via `__io_putchar`) pela USART3 e implementação de delay em microssegundos com `TIM2`. | [Ver README](./pratica-io-putchar/README.md) |
| [`pratica-serial-teclado`](./pratica-serial-teclado/) | USART3, GPIO | Leitura serial não-bloqueante na USART3; teclas `'R'`, `'G'`, `'B'` alternam os respectivos LEDs coloridos na placa. | [Ver README](./pratica-serial-teclado/README.md) |
| [`comunicacao-serial-767`](./comunicacao-serial-767/) | GPIO, TIM2 | Protocolo serial proprietário 1-wire half-duplex por bit-banging (`PE2`) com timing de 50 µs/bit, handshake e verificação de erro. | [Ver README](./comunicacao-serial-767/README.md) |
| [`pratica-interrupcao-serial`](./pratica-interrupcao-serial/) | USART2, GPIO | Protocolo serial estruturado com pacote de comando/ACK, debounce temporal não-bloqueante e tratamento de overrun UART. | [Ver README](./pratica-interrupcao-serial/README.md) |
| [`pratica-rng`](./pratica-rng/) | HW True RNG, USART3 | Periférico de hardware True RNG gerando números aleatórios para um jogo de dados "Melhor de 3" formatado em terminal ANSI. | [Ver README](./pratica-rng/README.md) |
| [`pratica-rng-estimativa-pi`](./pratica-rng-estimativa-pi/) | HW True RNG, USART3 | Estimativa numérica contínua de $\pi$ via método de Monte Carlo acelerado por hardware RNG em lotes com inteiros de 64 bits. | [Ver README](./pratica-rng-estimativa-pi/README.md) |
| [`pratica-rtc-767`](./pratica-rtc-767/) | RTC, USART3, PWR | Configuração e leitura do Real-Time Clock de hardware, com parser serial para sincronização de data e hora (`DSDMMMAAAAHHMNSS`). | [Ver README](./pratica-rtc-767/README.md) |
| [`pratica-spi-bmp`](./pratica-spi-bmp/) | SPI1, GPIO, USART3 | Leitura de pressão atmosférica, temperatura e cálculo de altitude barométrica do sensor Bosch BMP280 via barramento SPI. | [Ver README](./pratica-spi-bmp/README.md) |
| [`display-tft-f767`](./display-tft-f767/) | I2C1, GPIO | Driver gráfico para display OLED SSD1306 (128x64) via I2C com demo procedural e script conversor de vídeo/imagem Python. | [Ver README](./display-tft-f767/README.md) |

---

## 🛠️ Como Importar, Compilar e Gravar no STM32CubeIDE

### 1. Clonar o Repositório
```bash
git clone https://github.com/PlayzyJar/praticas-aamci.git
cd praticas-aamci
```

### 2. Importar os Projetos no STM32CubeIDE
1. Abra o **STM32CubeIDE**.
2. Vá em **File** -> **Import...**
3. Selecione **General** -> **Existing Projects into Workspace** e clique em **Next**.
4. Em **Select root directory**, clique em **Browse...** e aponte para a pasta raiz `praticas-aamci`.
5. Marque os projetos desejados na lista (certifique-se de que a opção *"Copy projects into workspace"* esteja **desmarcada** para manter os arquivos vinculados ao Git).
6. Clique em **Finish**.

### 3. Compilar um Projeto
1. Na aba *Project Explorer*, clique com o botão direito sobre o projeto desejado (ex: `pratica-rng`).
2. Clique em **Build Project** (ou use o atalho `Ctrl + B`).
3. O compilador `arm-none-eabi-gcc` gerará o binário sem erros ou alertas críticos.

### 4. Gravar na Placa NUCLEO-F767ZI
1. Conecte a placa ao computador pelo conector Micro-USB **CN1** (ST-LINK).
2. Clique com o botão direito no projeto -> **Run As** -> **STM32 C/C++ Application**.
3. O STM32CubeIDE detectará automaticamente o ST-LINK/V2-1 integrado e fará o flash na memória do STM32F767.

### 5. Monitoramento Serial
A maioria das práticas envia telemetria e interações pela USART3 (Virtual COM Port).
* **Parâmetros de Conexão:**
  * **Baud Rate:** `115200`
  * **Data Bits:** `8`
  * **Parity:** `None`
  * **Stop Bits:** `1`
  * **Flow Control:** `None`
* **Linux:** Dispositivo `/dev/ttyACM0` (ou `/dev/ttyACM1`):
  ```bash
  # Exemplo usando picocom, minicom ou screen:
  picocom -b 115200 /dev/ttyACM0
  ```
* **Windows:** Dispositivo `COMx` (verificar no Gerenciador de Dispositivos) via PuTTY, TeraTerm ou monitor serial integrado do VS Code / CubeIDE.

---

## 🤖 Guia de Atuação para Agentes Autônomos de IA (LLMs)

Se você é um agente de inteligência artificial encarregado de guiar o usuário na execução ou depuração destas práticas, siga rigorosamente estas diretrizes:

1. **Identificação da Prática:** Pergunte ou identifique qual das 10 pastas o usuário deseja rodar e consulte o `README.md` específico dessa pasta.
2. **Verificação de Hardware Prévia:**
   * Certifique-se de que a placa está conectada via porta USB ST-LINK (`CN1`).
   * Para projetos com sensores ou periféricos externos (ex: `pratica-spi-bmp`, `display-tft-f767`, `comunicacao-serial-767`), verifique se o usuário realizou as conexões elétricas nos pinos exatos listados no README do projeto antes de orientar a gravação.
3. **Parâmetros de Comunicação:** Lembre sempre o usuário de abrir o terminal serial a **115200 bps** antes ou logo após dar reset na placa para capturar mensagens de inicialização.
4. **Resolução de Problemas Comuns:**
   * **Erro ST-LINK "No target connected":** Oriente o usuário a segurar o botão preto `RESET`, clicar em gravar no IDE e soltar o botão assim que o OpenOCD/GDB iniciar a conexão.
   * **Serial sem dados:** Verifique se os jumpers `CN4` (TX/RX do ST-LINK) estão inseridos na placa conectando `PD8`/`PD9` à interface USB.
   * **Erro no BMP280 / I2C OLED:** Verifique alimentação de 3.3V, linhas pull-up (I2C) e correspondência de SCK/MOSI/MISO/CS nos pinos do conector Morpho ou Zio.
