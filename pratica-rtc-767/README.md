# Prática: Real-Time Clock de Hardware (`pratica-rtc-767`)

Configuração e operação do periférico de hardware **RTC** (*Real-Time Clock*) interno da STM32F767ZI, incluindo desbloqueio do domínio de *Backup*, manipulação direta dos registradores protegidos por chave (`WPR`) e implementação de um parser serial interativo para ajuste do calendário via comando de 16 dígitos.

---

## 📋 Objetivo do Firmware

1. Habilitar o acesso de escrita aos registradores de backup via `HAL_PWR_EnableBkUpAccess()`.
2. Desbloquear a proteção de escrita do RTC utilizando a sequência de chaves de segurança `0xCA` e `0x53` no registrador `RTC->WPR`.
3. Inicializar o calendário com data/hora padrão e transmitir periodicamente via **USART3** a cada segundo o tempo decorrido no formato:
   ```text
   [Dia_Semana] DD/MM/AA - HH:MM:SS
   ```
4. Interpretar comandos recebidos pela serial para atualização dinâmica do calendário em tempo real, sem necessidade de reset ou recompilação.

---

## 📅 Protocolo do Comando de Ajuste de Data e Hora

Para atualizar o relógio, envie uma string numérica contendo exatamente **16 dígitos** seguida de `Enter` (`\r` ou `\n`):

$$\text{Formato: } \underbrace{\text{DS}}_{\text{Dia Semana}} \, \underbrace{\text{DM}}_{\text{Dia Mês}} \, \underbrace{\text{MM}}_{\text{Mês}} \, \underbrace{\text{AAAA}}_{\text{Ano}} \, \underbrace{\text{HH}}_{\text{Hora}} \, \underbrace{\text{MN}}_{\text{Minuto}} \, \underbrace{\text{SS}}_{\text{Segundo}}$$

### Significado dos Campos:
* **`DS` (2 dígitos):** Dia da semana (de `01` a `07`, onde `01` = Segunda-feira ... `07` = Domingo).
* **`DM` (2 dígitos):** Dia do mês (de `01` a `31`).
* **`MM` (2 dígitos):** Mês (de `01` a `12`).
* **`AAAA` (4 dígitos):** Ano com 4 dígitos (ex: `2026`).
* **`HH` (2 dígitos):** Horas em formato 24h (de `00` a `23`).
* **`MN` (2 dígitos):** Minutos (de `00` a `59`).
* **`SS` (2 dígitos):** Segundos (de `00` a `59`).

### Exemplo Prático de Envio:
Para configurar: **Sexta-feira (05), 06 de Novembro de 2026, às 13:30:00**:
```text
0506112026133000
```

---

## 🔌 Hardware e Pinagem

| Sinal / Recurso | Pino STM32 | Modo / Função | Descrição |
|---|---|---|---|
| **RTC de Hardware** | Interno | LSI / LSE Clock | Contador de data e hora com domínio de energia em backup. |
| **USART3 TX** | `PD8` | AF7 (USART3_TX) | Transmissão de hora a cada 1 segundo e respostas do parser. |
| **USART3 RX** | `PD9` | AF7 (USART3_RX) | Recepção dos 16 dígitos de configuração enviados pelo usuário. |
| **LED 1 (Verde)** | `PB0` | Saída Digital (`LD1_Pin`) | Comuta a cada caractere recebido na USART3 (feedback imediato). |

---

## ⚙️ Compilação e Gravação

1. No **STM32CubeIDE**, selecione `pratica-rtc-767`.
2. Compile o projeto via `Ctrl + B`.
3. Conecte a placa NUCLEO-F767ZI via cabo USB.
4. Execute via **Run As** -> **STM32 C/C++ Application**.

---

## 🧪 Como Testar e Validar

1. Conecte seu terminal serial:
   * **Porta:** `/dev/ttyACM0` (Linux) ou `COMx` (Windows)
   * **Baud Rate:** `115200`
   * **Configuração:** 8N1
2. O relógio começará a imprimir o tempo a cada segundo:
   ```text
   [Mon] 01/01/00 - 00:00:01
   [Mon] 01/01/00 - 00:00:02
   ```
3. Digite a string de 16 dígitos (ex: `0506112026133000`) e aperte `Enter`:
   * O terminal responderá:
     ```text
     >>> [SUCESSO] Horario atualizado com sucesso!
     [Fri] 06/11/26 - 13:30:01
     [Fri] 06/11/26 - 13:30:02
     ```
4. Se enviar uma quantidade incorreta de caracteres ou números fora do limite (ex: mês 13 ou hora 25), o firmware rejeitará e acusará o erro correspondente sem corromper o relógio atual.

---

## 🤖 Prompt Pronto para Agente IA

```text
Estou executando a "pratica-rtc-767" na NUCLEO-F767ZI.
O firmware gerencia o RTC interno com parser de string serial de 16 dígitos (DSDMMMAAAAHHMNSS) na USART3 a 115200 bps.
Me ajude a formatar uma string de data/hora atual e validar o teste no terminal serial.
```
