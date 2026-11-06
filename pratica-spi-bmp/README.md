# Pratica SPI BMP280

Leitura de pressao barometrica, temperatura e calculo de altitude barometrica a partir do sensor digital Bosch BMP280 conectado via SPI1.

## Funcionamento

- Periferico SPI1 em modo Master (Full-Duplex, 8 bits, CPOL=0 / CPHA=0).
- Leitura dos coeficientes de calibracao de fabrica do BMP280 (`dig_T1..T3`, `dig_P1..P9`).
- Leitura continua dos registradores de dados e aplicacao das formulas de compensacao Bosch.
- Calculo da altitude barometrica em metros em relacao a pressao padrao ao nivel do mar (1013.25 hPa).
- Telemetria transmitida pela USART3. Se a comunicacao SPI falhar na inicializacao, o LED vermelho (LD3) pisca como aviso.

## Conexoes (Pinagem SPI1)

| BMP280 | Sinal | Pino NUCLEO-F767ZI |
|---|---|---|
| VCC | 3.3V | `3.3V` |
| GND | Terra | `GND` |
| SCL / SCK | Clock | `PA5` (SPI1_SCK) |
| SDO / MISO | Master In | `PA6` (SPI1_MISO) |
| SDA / SDI / MOSI | Master Out | `PD7` (SPI1_MOSI) |
| CSB / CS | Chip Select | `PA4` (GPIO Output) |

## Teste

1. Conecte o sensor nos pinos acima.
2. Abra o terminal serial em 115200 8N1.
3. Observe as leituras:
   ```text
   Temp: 24.85 C | Pressao: 1012.30 hPa | Altitude: 8.42 m
   ```
4. Se os fios estiverem soltos ou incorretos, o terminal exibira `[ERRO] Falha ao inicializar o BMP280 via SPI1!` e o LED LD3 comecara a piscar.
