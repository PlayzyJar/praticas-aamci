# Especificação de Design: Monorepo Práticas AAMCI (STM32F767ZI)

**Data:** 2026-11-06  
**Repositório Remoto:** `https://github.com/PlayzyJar/praticas-aamci.git`  
**Placa-Alvo:** STMicroelectronics NUCLEO-F767ZI (MCU STM32F767ZIT6, ARM Cortex-M7 @ 216MHz)  
**Disciplina:** AAMCI (Microcontroladores e Microprocessadores II)  

---

## 1. Visão Geral e Objetivos

O objetivo deste projeto é organizar, documentar e versionar um monorepo contendo todos os projetos práticos desenvolvidos na disciplina AAMCI para a placa **NUCLEO-F767ZI**. O repositório servirá como fonte central de código e documentação padronizada para permitir que, após a formatação da máquina de desenvolvimento ou quando consultado por um agente autônomo de inteligência artificial (LLM), o agente seja capaz de:

1. Explicar o funcionamento e conceitos de sistemas embarcados de cada prática.
2. Identificar a pinagem e conexões elétricas necessárias no hardware.
3. Fornecer instruções precisas de build e gravação através do STM32CubeIDE.
4. Instruir sobre testes, parâmetros seriais (115200 8N1) e comportamento esperado dos atuadores/sensores.

---

## 2. Escopo e Seleção de Projetos

Com base na avaliação dos diretórios de trabalho, foram selecionados **10 projetos ativos** construídos para o microcontrolador STM32F767ZITx:

| # | Diretório | Periféricos / Recursos | Descrição Resumida |
|---|-----------|-------------------------|---------------------|
| 1 | `hello-world-767/` | GPIO, USART3 | Teste básico de I/O com leitura do botão B1 (`PC13`) e controle dos LEDs LD1, LD2 e LD3 (`PB0`, `PB7`, `PB14`). |
| 2 | `pratica-io-putchar/` | USART3, TIM2 | Redirecionamento da libc (`printf` via `__io_putchar`) e temporização precisa em microssegundos com `TIM2`. |
| 3 | `pratica-serial-teclado/` | USART3, GPIO | Máquina de comando serial não-bloqueante lendo teclas (`R`, `G`, `B`) para alternar LEDs correspondentes. |
| 4 | `comunicacao-serial-767/` | GPIO (Bit-Banging), TIM2 | Protocolo serial próprio a 1 fio (half-duplex no pino `PE2`) com timing de 50 µs/bit, handshake e debounce. |
| 5 | `pratica-interrupcao-serial/` | USART2, GPIO | Protocolo serial com pacotes de comando/ACK e debounce não-bloqueante para envio/recepção de estados. |
| 6 | `pratica-rng/` | HW True RNG, USART3 | Periférico de hardware RNG gerando dados em um jogo de terminal "Melhor de 3" formatado com códigos ANSI. |
| 7 | `pratica-rng-estimativa-pi/` | HW True RNG, USART3 | Estimativa numérica de $\pi$ via método de Monte Carlo de alto desempenho em lotes com o HW RNG. |
| 8 | `pratica-rtc-767/` | RTC, USART3, PWR | Relógio e calendário de tempo real integrado com parser serial para ajuste de data e hora (`DSDMMMAAAAHHMNSS`). |
| 9 | `pratica-spi-bmp/` | SPI1, GPIO, USART3 | Leitura de pressão barométrica, temperatura e altitude do sensor Bosch BMP280 via barramento SPI. |
| 10 | `display-tft-f767/` | I2C1, GPIO | Driver para display OLED SSD1306 (128x64) via I2C com animações procedurais e script `video_to_ssd1306.py`. |

### Itens Excluídos do Versionamento
* `aula-usart/`: Template padrão gerado pelo CubeMX sem lógica de usuário implementada.
* `teste-display-tft/`: Projeto de teste prévio direcionado à placa STM32F411 (BlackPill).
* `.metadata/`: Diretório de cache de workspace do Eclipse / STM32CubeIDE.

---

## 3. Arquitetura do Repositório e `.gitignore`

### Estrutura de Arquivos
```text
praticas-aamci/
├── .gitignore
├── README.md
├── video_to_ssd1306.py
├── docs/
│   └── superpowers/specs/
│       └── 2026-11-06-praticas-aamci-monorepo-design.md
├── hello-world-767/
│   ├── README.md
│   ├── hello-world-767.ioc
│   ├── Core/
│   └── Drivers/
├── pratica-io-putchar/
│   ├── README.md
│   ├── pratica-io-putchar.ioc
│   ├── Core/
│   └── Drivers/
... (demais 8 pastas com estrutura similar)
```

### Regras de `.gitignore`
Para evitar o upload de binários compilados (que somam ~400 MB) e manter o repositório leve (< 5 MB):
```gitignore
# Artefatos de compilação
Debug/
Release/
build/
*.o
*.d
*.su
*.elf
*.bin
*.hex
*.map
*.a
*.so

# Eclipse / STM32CubeIDE caches e workspace
.metadata/
.settings/language.settings.xml

# Projetos descartados
aula-usart/
teste-display-tft/

# Arquivos de sistema operacional
.DS_Store
Thumbs.db
```

---

## 4. Padrão de Documentação

### 4.1. `README.md` Raiz
O README principal atuará como o portal de entrada do monorepo, contendo:
1. **Cabeçalho:** Identificação da disciplina, instituição/contexto e objetivos.
2. **Especificação de Hardware:**
   * Placa NUCLEO-144 F767ZI.
   * Pinout de periféricos comuns on-board:
     * USART3 (ST-LINK Virtual COM Port): TX=`PD8`, RX=`PD9`.
     * Botão de Usuário B1 (Azul): `PC13`.
     * LED Verde LD1: `PB0`.
     * LED Azul LD2: `PB7`.
     * LED Vermelho LD3: `PB14`.
3. **Instruções de Importação e Execução:**
   * Como abrir o STM32CubeIDE e importar múltiplos projetos (`File -> Import -> Existing Projects into Workspace`).
   * Como compilar (`Project -> Build Project` ou `Ctrl+B`).
   * Como gravar e depurar via ST-LINK GDB Server.
   * Conexão e monitoramento serial no Linux/Windows.
4. **Instruções para Agentes Autônomos de IA:**
   * Orientações para a LLM sobre como diagnosticar problemas de conexão, validação de pinos, taxas de transmissão e checagem de jumpers da placa.
5. **Matriz de Práticas:** Tabela de navegação rápida com links para cada pasta de projeto.

### 4.2. `README.md` Individual (Por Projeto)
Cada subdiretório de projeto terá seu próprio `README.md` estruturado com:
1. **Objetivo & Conceito:** Descrição teórica e prática do que o código executa.
2. **Tabela de Pinagem e Conexões:** Lista detalhada dos pinos e portas GPIO/periféricos utilizados.
3. **Hardware Adicional Necessário:** Sensores, displays ou jumpers externos (se aplicável).
4. **Como Compilar e Executar no STM32CubeIDE:** Passo a passo focado no projeto específico.
5. **Guia de Teste e Validação Serial / Física:**
   * Configuração de terminal (Baud: 115200, 8N1).
   * Parâmetros de entrada e comandos aceitos.
   * Saída de texto esperada e comportamento dos LEDs.
6. **Card de Prompt para Agentes IA:** Um bloco pronto para o usuário copiar e colar em qualquer chat de IA para receber tutoria imediata e especializada naquela prática.

---

## 5. Estratégia de Deploy e Versionamento Git

1. Inicialização do repositório Git local com branch padrão `main`.
2. Criação do `.gitignore` e verificação via `git status` para confirmar que nenhum arquivo de `Debug/` ou `.metadata/` está staged.
3. Criação de todos os `README.md` (raiz e 10 subpastas).
4. Criação do commit inicial: `feat: monorepo inicial de praticas AAMCI para STM32F767ZI`.
5. Configuração do remote `origin` apontando para `https://github.com/PlayzyJar/praticas-aamci.git`.
6. Validação final de integridade e instrução de push via HTTPS.
