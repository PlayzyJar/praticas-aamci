# Práticas AAMCI Monorepo Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Transform the STM32 projects folder into a clean, lightweight monorepo with standardized documentation for 10 STM32F767ZI practices, configured for GitHub (`https://github.com/PlayzyJar/praticas-aamci.git`), with detailed guides allowing an AI agent or developer to build, flash, and test each firmware on hardware.

**Architecture:** A flat monorepo structure preserving STM32CubeIDE project root folders (`.ioc`, `.project`, `.cproject`, `Core/`, `Drivers/`). A robust `.gitignore` removes compilation binaries and caches (~400MB reduction). A root `README.md` acts as master portal with AI agent instructions, and 10 individual `README.md` files describe pinouts, peripherals, test commands, and agent prompts.

**Tech Stack:** STM32CubeIDE, ARM Cortex-M7 (STM32F767ZIT6), STM32 HAL & CMSIS, C, Python 3, Git, GitHub.

**Spec:** `docs/superpowers/specs/2026-11-06-praticas-aamci-monorepo-design.md`

## Global Constraints

- Target board: STMicroelectronics NUCLEO-F767ZI.
- Default VCP Serial: USART3 (TX `PD8`, RX `PD9`) @ 115200 bps, 8N1.
- On-board I/O: User Button B1 (`PC13`), LD1 Green (`PB0`), LD2 Blue (`PB7`), LD3 Red (`PB14`).
- No build artifacts (`Debug/`, `Release/`, `*.o`, `*.elf`, `*.su`, etc.) committed to Git.
- Excluded folders: `aula-usart/`, `teste-display-tft/`, `.metadata/`.
- Remote repository URL: `https://github.com/PlayzyJar/praticas-aamci.git`.

---

### Task 1: Git Repository Setup and `.gitignore` Configuration

**Files:**
- Create: `.gitignore`

**Interfaces:**
- Produces: Git ignore rules filtering out binaries, `.metadata/`, `aula-usart/`, `teste-display-tft/`.

- [ ] **Step 1: Create `.gitignore`**

Create `.gitignore` at the project root with the following content:
```gitignore
# Compilation artifacts
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

# Eclipse / STM32CubeIDE caches
.metadata/
.settings/language.settings.xml

# Unused / excluded projects
aula-usart/
teste-display-tft/

# OS files
.DS_Store
Thumbs.db
```

- [ ] **Step 2: Initialize Git and verify ignore rules**

Run:
```bash
git init -b main
git status --ignored
```
Expected: `Debug/`, `.metadata/`, `aula-usart/`, `teste-display-tft/` appear under "Ignored files", while project source directories appear as untracked.

- [ ] **Step 3: Commit `.gitignore`**

Run:
```bash
git add .gitignore docs/
git commit -m "chore: setup .gitignore and design spec for AAMCI monorepo"
```

---

### Task 2: Root `README.md` Master Documentation

**Files:**
- Create: `README.md`

**Interfaces:**
- Consumes: Spec overview, hardware mapping, list of 10 practices.
- Produces: Master documentation guiding developers and AI agents.

- [ ] **Step 1: Write root `README.md`**

Create `README.md` at the project root with:
1. Title: "Monorepo de Práticas - AAMCI (STM32F767ZI)"
2. Overview of the AAMCI course (Microcontroladores e Microprocessadores II).
3. Board specifications (NUCLEO-144 F767ZI, Cortex-M7 @ 216MHz, ST-LINK V2-1).
4. Standard on-board pin mapping:
   - USART3 (VCP): TX=`PD8`, RX=`PD9`, 115200 8N1.
   - User Button B1 (Blue): `PC13` (active HIGH).
   - LD1 (Green): `PB0`.
   - LD2 (Blue): `PB7`.
   - LD3 (Red): `PB14`.
5. STM32CubeIDE build and flash guide:
   - How to clone and open the IDE.
   - How to import existing projects (`File -> Import -> Existing Projects into Workspace`).
   - How to build (`Project -> Build Project` or `Ctrl+B`).
   - How to debug/flash (`Run As -> STM32 C/C++ Application`).
   - How to open serial monitor on Linux (`/dev/ttyACM0`) and Windows (`COMx`).
6. AI Agent Guidelines section: Explicit instructions for an AI agent on how to assist the user, diagnose hardware issues, verify pinouts, and test firmware.
7. Full practice navigation matrix linking to all 10 project directories.

- [ ] **Step 2: Verify `README.md` links and formatting**

Check that all folder links resolve to existing project directories.

- [ ] **Step 3: Commit root `README.md`**

Run:
```bash
git add README.md
git commit -m "docs: add root README with master guide and AI agent instructions"
```

---

### Task 3: Documentation for Core GPIO & USART Practices

**Files:**
- Create: `hello-world-767/README.md`
- Create: `pratica-io-putchar/README.md`
- Create: `pratica-serial-teclado/README.md`

**Interfaces:**
- Consumes: Code implementation of `hello-world-767`, `pratica-io-putchar`, `pratica-serial-teclado`.
- Produces: Individual practice READMEs with pin tables, test steps, and AI prompts.

- [ ] **Step 1: Create `hello-world-767/README.md`**
  - Document GPIO read on Button B1 (`PC13`), GPIO write on LD1/LD2/LD3 (`PB0`, `PB7`, `PB14`), and serial state transmission on USART3.
  - Explain button press behavior (turning on all 3 LEDs and sending `estado: 1\r\n`).
  - Include AI prompt card.

- [ ] **Step 2: Create `pratica-io-putchar/README.md`**
  - Document libc `printf` retargeting via `__io_putchar` mapped to `huart3`.
  - Document microsecond hardware timer delay using `TIM2` (`TIM2_Micros_Init()`, `Delay_ms()`, `Delay_us()`).
  - Document periodic message output and blinking LD1.
  - Include AI prompt card.

- [ ] **Step 3: Create `pratica-serial-teclado/README.md`**
  - Document non-blocking UART polling via `USART3->ISR & USART_ISR_RXNE`.
  - Document keyboard command mappings:
    - 'R' -> Toggle LD3 (Red, `PB14`)
    - 'G' -> Toggle LD1 (Green, `PB0`)
    - 'B' -> Toggle LD2 (Blue, `PB7`)
  - Include terminal interaction guide and AI prompt card.

- [ ] **Step 4: Commit core GPIO & USART documentation**

Run:
```bash
git add hello-world-767/README.md pratica-io-putchar/README.md pratica-serial-teclado/README.md
git commit -m "docs: add documentation for hello-world, io-putchar, and serial-teclado practices"
```

---

### Task 4: Documentation for Custom Serial Communication Practices

**Files:**
- Create: `comunicacao-serial-767/README.md`
- Create: `pratica-interrupcao-serial/README.md`

**Interfaces:**
- Consumes: Code in `comunicacao-serial-767/Core/Src/main.c` and `pratica-interrupcao-serial/Core/Src/serial_protocol.c`.
- Produces: Practice READMEs covering 1-wire software protocol and structured packet protocol.

- [ ] **Step 1: Create `comunicacao-serial-767/README.md`**
  - Document 1-wire half-duplex bit-banging protocol on pin `PE2`.
  - Document bit timing (50 µs/bit using TIM2), frame format (1 Start bit, 8 data bits LSB first, 1 Stop bit), and active-low handshake ACK.
  - Document command codes: `0xAA` (User Btn PC13 -> toggles LD1), `0xBB` (External Btn PE0 -> toggles LD2).
  - Document error handling: if handshake fails, the local LED toggles as a warning.
  - Include connection diagram / instructions and AI prompt card.

- [ ] **Step 2: Create `pratica-interrupcao-serial/README.md`**
  - Document structured packet communication over USART2 (`serial_protocol.c`).
  - Document commands `CMD_USER_BTN` (toggles LD1), `CMD_EXT_BTN` (toggles LD2), and `ACK_BYTE`.
  - Document non-blocking button debouncing with `HAL_GetTick()`.
  - Document UART overrun error recovery via `__HAL_UART_CLEAR_OREFLAG`.
  - Include testing instructions and AI prompt card.

- [ ] **Step 3: Commit custom serial practices documentation**

Run:
```bash
git add comunicacao-serial-767/README.md pratica-interrupcao-serial/README.md
git commit -m "docs: add documentation for comunicacao-serial-767 and pratica-interrupcao-serial"
```

---

### Task 5: Documentation for Hardware RNG Practices

**Files:**
- Create: `pratica-rng/README.md`
- Create: `pratica-rng-estimativa-pi/README.md`

**Interfaces:**
- Consumes: Code in `pratica-rng/Core/Src/main.c` and `pratica-rng-estimativa-pi/Core/Src/pi_mc.c`.
- Produces: Practice READMEs for Hardware True RNG and Monte Carlo Pi estimation.

- [ ] **Step 1: Create `pratica-rng/README.md`**
  - Document STM32 True Random Number Generator (RNG) peripheral configuration.
  - Document ANSI terminal formatting (clear screen, cursor positioning, scorecard).
  - Document the "Melhor de 3" dice game logic (Machine vs Player).
  - Include terminal instructions and AI prompt card.

- [ ] **Step 2: Create `pratica-rng-estimativa-pi/README.md`**
  - Document Monte Carlo algorithm for estimating $\pi$: generating random $(x, y)$ coordinates in $[0, 2^{32}-1]$ and testing $x^2 + y^2 \le (2^{32}-1)^2$ with 64-bit overflow prevention.
  - Document batch processing (`PI_MC_BATCH`) and fixed-point micro-precision reporting (`pi_mc_micro()`).
  - Document serial output format (`Mpontos=... pi=3.141592...`).
  - Include AI prompt card.

- [ ] **Step 3: Commit RNG documentation**

Run:
```bash
git add pratica-rng/README.md pratica-rng-estimativa-pi/README.md
git commit -m "docs: add documentation for pratica-rng and pratica-rng-estimativa-pi"
```

---

### Task 6: Documentation for Peripherals: RTC, SPI BMP280, and I2C OLED Display

**Files:**
- Create: `pratica-rtc-767/README.md`
- Create: `pratica-spi-bmp/README.md`
- Create: `display-tft-f767/README.md`

**Interfaces:**
- Consumes: Code in `rtc_utilities.c`, `bmp280.c`, `ssd1306.c`, `video_to_ssd1306.py`.
- Produces: Practice READMEs for RTC, SPI BMP280 sensor, and I2C SSD1306 OLED display.

- [ ] **Step 1: Create `pratica-rtc-767/README.md`**
  - Document internal RTC peripheral, Backup domain write protection unlocking (`RTC->WPR = 0xCA, 0x53`), and initialization mode.
  - Document serial calendar update format: 16 digits `DSDMMMAAAAHHMNSS` (Day of week, Day of month, Month, Year, Hour, Minute, Second).
  - Document live clock display and status messages.
  - Include test commands and AI prompt card.

- [ ] **Step 2: Create `pratica-spi-bmp/README.md`**
  - Document SPI1 interface and connections: `PA5` (SCK), `PA6` (MISO/SDO), `PD7` (MOSI/SDI), `PA4` (CSB).
  - Document Bosch BMP280 sensor calibration readout, temperature compensation, barometric pressure in hPa, and barometric altitude in meters.
  - Include LED alert behavior (blinking LD3 if sensor init fails) and AI prompt card.

- [ ] **Step 3: Create `display-tft-f767/README.md`**
  - Document I2C1 SSD1306 OLED configuration (`PB8` SCL, `PB9` SDA, address `0x3C << 1`).
  - Document graphics library, procedural radar/animation demo (`SSD1306_DemoProceduralAnimation`), and text fonts.
  - Document `video_to_ssd1306.py` script for converting MP4/GIF/images into C byte arrays.
  - Include connection guide and AI prompt card.

- [ ] **Step 4: Commit peripheral practices documentation**

Run:
```bash
git add pratica-rtc-767/README.md pratica-spi-bmp/README.md display-tft-f767/README.md
git commit -m "docs: add documentation for rtc, spi-bmp, and display-tft practices"
```

---

### Task 7: Repository Verification, Initial Commit & GitHub Remote Setup

**Files:**
- Remote: `https://github.com/PlayzyJar/praticas-aamci.git`

**Interfaces:**
- Consumes: All 10 practice READMEs, root README, `.gitignore`.
- Produces: Fully tracked monorepo on `main` branch, verified clean build status, remote origin configured.

- [ ] **Step 1: Check all READMEs and tracked files**

Run:
```bash
git status
git ls-files | grep -E '\.(o|su|elf|bin|d|map)$' || echo "All binaries successfully ignored!"
```
Verify that no compilation artifacts are tracked.

- [ ] **Step 2: Stage and commit all project files**

Run:
```bash
git add hello-world-767/ pratica-io-putchar/ pratica-serial-teclado/ comunicacao-serial-767/ pratica-interrupcao-serial/ pratica-rng/ pratica-rng-estimativa-pi/ pratica-rtc-767/ pratica-spi-bmp/ display-tft-f767/ video_to_ssd1306.py
git commit -m "feat: complete monorepo for AAMCI STM32F767ZI practices"
```

- [ ] **Step 3: Configure GitHub remote origin**

Run:
```bash
git remote add origin https://github.com/PlayzyJar/praticas-aamci.git || git remote set-url origin https://github.com/PlayzyJar/praticas-aamci.git
git remote -v
```

- [ ] **Step 4: Verify repository size and prepare push command**

Run:
```bash
du -sh .git
```
Verify that `.git` size is under 15MB (compared to ~480MB of raw files).

- [ ] **Step 5: Provide user instructions for final HTTPS push**

Document the exact 1-liner command for the user:
```bash
git push -u origin main
```
