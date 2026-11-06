# Prática: Sensor Barométrico BMP280 via SPI1 (`pratica-spi-bmp`)

Interface de comunicação síncrona de alta velocidade utilizando o periférico **SPI1** da STM32F767ZI para leitura de pressão atmosférica, temperatura e cálculo de altitude em tempo real a partir do sensor digital **Bosch BMP280**.

---

## 📋 Objetivo do Firmware

1. Configurar o periférico **SPI1** no modo Master (Full-Duplex, 8 bits, MSB First, CPOL=0 / CPHA=0).
2. Gerenciar a linha de seleção de chip (*Chip Select* - `CSB`) via controle GPIO em `PA4`.
3. Comunicar com o sensor **BMP280**:
   * Ler os coeficientes de calibração de fábrica gravados na memória ROM do sensor (`dig_T1..T3`, `dig_P1..P9`).
   * Configurar modo de operação contínuo (*Normal Mode*) e filtros de sobreamostragem (*Oversampling* de temperatura e pressão).
   * Realizar leitura contínua dos registradores de dados brutos e aplicar as fórmulas de compensação de ponto flutuante da Bosch.
   * Calcular a altitude barométrica em metros em relação ao nível do mar (fórmula hipsométrica padrão $1013.25 \text{ hPa}$).
4. Transmitir as medições formatadas via **USART3** para o monitor serial. Em caso de falha de inicialização (ex: fio desconectado ou pino incorreto), pisca o **LED Vermelho (LD3)** como alerta visual de erro.

---

## 🔌 Hardware e Conexões (Pinagem SPI1)

Conecte o módulo sensor BMP280 à placa NUCLEO-F767ZI conforme a tabela:

| Pino do BMP280 | Sinal SPI | Pino STM32F767ZI | Descrição |
|---|---|---|---|
| **VCC** | Alimentação | `3.3V` | Tensão de operação recomendada do sensor (3.3V). |
| **GND** | Terra | `GND` | Referência de 0V compartilhada. |
| **SCL / SCK** | Clock SPI | `PA5` | SPI1_SCK (Sinal de clock gerado pelo microcontrolador). |
| **SDO / MISO** | Master In Slave Out | `PA6` | SPI1_MISO (Dados enviados do sensor para a placa). |
| **SDA / SDI / MOSI** | Master Out Slave In | `PD7` | SPI1_MOSI (Comandos/dados enviados da placa para o sensor). |
| **CSB / CS** | Chip Select | `PA4` | Controle de seleção de escravo (ativo em nível BAIXO). |

> ⚠️ **Importante:** Verifique se o módulo BMP280 possui o pino `SDO` em 3.3V ou GND se houver jumper de seleção de endereço, e garanta que o conector `CSB` esteja firmemente conectado a `PA4` para que o módulo opere em modo SPI (e não I2C).

---

## ⚙️ Compilação e Gravação

1. No **STM32CubeIDE**, selecione `pratica-spi-bmp`.
2. Compile o código com `Ctrl + B`.
3. Conecte a placa NUCLEO-F767ZI via cabo USB.
4. Execute via **Run As** -> **STM32 C/C++ Application**.

---

## 🧪 Como Testar e Validar

1. Abra o terminal serial:
   * **Porta:** `/dev/ttyACM0` (Linux) ou `COMx` (Windows)
   * **Baud Rate:** `115200`
   * **Configuração:** 8N1
2. **Caso 1: Conexão Bem-Sucedida**
   O terminal iniciará exibindo a telemetria em tempo real:
   ```text
   Temp: 24.85 C | Pressao: 1012.30 hPa | Altitude: 8.42 m
   Temp: 24.86 C | Pressao: 1012.28 hPa | Altitude: 8.58 m
   ```
   *(Aproxime o dedo do sensor para ver a temperatura subir imediatamente).*

3. **Caso 2: Falha de Conexão ou Cabo Solto**
   Se o sensor não for detectado ou responder com Chip ID inválido:
   * O terminal exibirá:
     ```text
     [ERRO] Falha ao inicializar o BMP280 via SPI1!
     Verifique as ligacoes: PA5(SCK), PA6(SDO/MISO), PD7(SDI/MOSI), PA4(CSB)
     ```
   * O **LED Vermelho (LD3 - `PB14`)** começará a piscar continuamente na placa NUCLEO.

---

## 🤖 Prompt Pronto para Agente IA

```text
Estou rodando o projeto "pratica-spi-bmp" na NUCLEO-F767ZI com o sensor BMP280 conectado via SPI1.
Os pinos configurados são PA5 (SCK), PA6 (MISO), PD7 (MOSI) e PA4 (CSB), com telemetria na USART3 a 115200 bps.
Me ajude a verificar a pinagem na placa, diagnosticar eventuais erros de leitura de Chip ID e interpretar os dados de pressão e altitude.
```
