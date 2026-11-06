# Display OLED SSD1306 via I2C (display-tft-f767)

Driver grafico para display OLED monocromatico SSD1306 (128x64 pixels) conectado via I2C1 na STM32F767ZI, com animacoes procedurais e script para conversao de video.

## Funcionamento

- Periferico I2C1 configurado a 400 kHz (Fast Mode) no endereco `0x3C` (`0x78`).
- Gerencia um buffer de 1024 bytes na memoria RAM para desenho de primitivas e fontes tipograficas.
- Ao inicializar, exibe uma tela de apresentacao por 2 segundos e em seguida roda um loop de animacao procedural (`SSD1306_DemoProceduralAnimation` com radar, esfera quicando e barra de progresso).
- Inclui o script `video_to_ssd1306.py` para converter videos MP4/GIF em quadros C para o display.

## Conexoes (Pinagem I2C1)

| Display SSD1306 | Sinal | Pino NUCLEO-F767ZI |
|---|---|---|
| VCC | 3.3V | `3.3V` |
| GND | Terra | `GND` |
| SCL | Clock I2C | `PB6` (I2C1_SCL) |
| SDA | Dados I2C | `PB9` (I2C1_SDA) |

## Teste

1. Conecte o display OLED nos pinos `PB6` (SCL) e `PB9` (SDA) e alimente com 3.3V/GND.
2. Grave o firmware: o display exibira "Hello, World!" seguido das animacoes procedurais em tempo real.

## Script Auxiliar (`video_to_ssd1306.py`)

Localizado na raiz do repositorio:
```bash
# Converter video MP4 em array C para o SSD1306
python3 video_to_ssd1306.py -i video.mp4 -o Core/Inc/animation_frames.h --fps 15
```
