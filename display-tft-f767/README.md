# Prática: Display Gráfico OLED SSD1306 via I2C (`display-tft-f767`)

Driver gráfico e conjunto de demonstrações visuais para display OLED monocromático **SSD1306** (128x64 pixels) conectado via barramento **I2C1** na STM32F767ZI, incluindo fontes tipográficas de múltiplos tamanhos, animações procedurais em tempo real e script em Python (`video_to_ssd1306.py`) para conversão de vídeos/GIFs em quadros C compactados.

---

## 📋 Objetivo do Firmware

1. Inicializar o controlador SSD1306 através do barramento **I2C1** a 400 kHz (Fast Mode) no endereço I2C `0x3C` (`0x78` com bit R/W).
2. Gerenciar um buffer gráfico em memória RAM (GDDRAM de $128 \times 64 / 8 = 1024 \text{ bytes}$) com funções de desenho primitivas (linhas, círculos, retângulos preenchidos, texto e bitmaps).
3. Exibir uma tela de apresentação (*Hello World*) utilizando fontes proporcionais (`Font_11x18` e `Font_7x10`).
4. Executar continuamente uma suíte de **animações procedurais** em tempo real (`SSD1306_DemoProceduralAnimation`):
   * Varredura contínua de feixe tipo Radar militar circular.
   * Física de esfera quicando (*bouncing ball*) com colisão em bordas.
   * Barra de progresso gráfica com preenchimento gradual.
5. Disponibilizar o script complementar `video_to_ssd1306.py` para converter animações personalizadas (MP4, GIF, PNG) diretamente em headers C.

---

## 🔌 Hardware e Conexões (Pinagem I2C1)

Conecte os 4 pinos do módulo OLED SSD1306 à placa NUCLEO-F767ZI:

| Pino do Display OLED | Sinal I2C | Pino STM32F767ZI | Descrição |
|---|---|---|---|
| **VCC** | Alimentação | `3.3V` | Tensão de alimentação (3.3V da placa). |
| **GND** | Terra | `GND` | Referência de 0V comum. |
| **SCL** | Clock I2C | `PB6` | I2C1_SCL (Linha de clock síncrono com pull-up). |
| **SDA** | Dados I2C | `PB9` | I2C1_SDA (Linha de dados bidirecional com pull-up). |

---

## ⚙️ Compilação e Gravação

1. No **STM32CubeIDE**, selecione o projeto `display-tft-f767`.
2. Pressione `Ctrl + B` para compilar o projeto.
3. Conecte a placa NUCLEO-F767ZI via cabo USB.
4. Execute via **Run As** -> **STM32 C/C++ Application**.

---

## 🧪 Como Testar e Validar

1. Conecte o display nos pinos `PB6` (SCL) e `PB9` (SDA) e alimente com `3.3V` e `GND`.
2. Após o upload do firmware:
   * **Primeiros 2 segundos:** O display acende com a mensagem centralizada:
     ```text
        Hello, World!
       STM32 + SSD1306
     ```
   * **Em seguida:** Entra no ciclo de animação procedural contínua exibindo o radar em rotação fluida, a bola quicando com física vetorial e a barra de carregamento.

---

## 🎞️ Conversor de Vídeo para OLED (`video_to_ssd1306.py`)

Na raiz do repositório encontra-se o script utilitário `video_to_ssd1306.py`. Ele permite converter qualquer vídeo (MP4/AVI) ou animação GIF em arrays C para rodar no display:

```bash
# Instalar dependências (Pillow)
pip install pillow

# Gerar arquivo C com frames de demonstração procedural
python3 video_to_ssd1306.py --demo -o Core/Inc/animation_frames.h

# Converter um vídeo MP4 a 15 FPS com dithering para o SSD1306
python3 video_to_ssd1306.py -i video.mp4 -o Core/Inc/animation_frames.h --fps 15
```

---

## 🤖 Prompt Pronto para Agente IA

```text
Estou rodando o projeto "display-tft-f767" na placa NUCLEO-F767ZI.
Ele controla um display OLED SSD1306 (128x64) via I2C1 (SCL em PB6, SDA em PB9 no endereço 0x3C).
Me auxilie na verificação das ligações do barramento I2C e na inclusão de novas animações ou telas gráficas.
```
