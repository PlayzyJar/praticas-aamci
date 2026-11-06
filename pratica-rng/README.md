# Prática: True Random Number Generator & Terminal ANSI (`pratica-rng`)

Demonstração do periférico de hardware **True RNG** (*Random Number Generator*) do STM32F767ZI, associado ao envio de sequências de escape ANSI para criar uma interface dinâmica de jogo no terminal serial com atualização de placar em tela fixa sem rolagem (*in-place refresh*).

---

## 📋 Objetivo do Firmware

1. Inicializar o periférico de hardware **RNG** (`RNG_HandleTypeDef hrng`), que gera números aleatórios verdadeiros a partir de ruído térmico analógico interno do chip.
2. Implementar uma interface de terminal interativa via **USART3** utilizando códigos de escape ANSI:
   * Limpeza de tela completa: `\033[2J\033[H`
   * Posicionamento do cursor no início: `\033[H`
3. Executar a lógica de uma partida de dados em formato **"Melhor de 3"** entre o microcontrolador (*Machine*) e o usuário (*Jogador*):
   * Aguarda o pressionamento de qualquer tecla no terminal para iniciar o sorteio da rodada.
   * Gera valores aleatórios de 1 a 6 (`(rand % 6) + 1`) para a Máquina e para o Jogador.
   * Compara os dados, atribui a vitória da rodada e atualiza o placar.
   * Declara o vencedor da partida assim que um competidor atinge 2 vitórias, permitindo reiniciar a disputa.

---

## 🔌 Hardware e Pinagem

| Sinal / Recurso | Pino STM32 | Modo / Função | Descrição |
|---|---|---|---|
| **HW True RNG** | Interno | PLL48CLK Source | Periférico gerador de entropia analógica de 32 bits. |
| **USART3 TX** | `PD8` | AF7 (USART3_TX) | Transmissão de quadros ANSI para o terminal serial do PC. |
| **USART3 RX** | `PD9` | AF7 (USART3_RX) | Recepção de tecla do usuário para jogar os dados. |
| **LEDs On-Board** | `PB0`, `PB7`, `PB14` | Saída Digital | Sinalização de atividade e depuração de hardware. |

---

## ⚙️ Compilação e Gravação

1. No **STM32CubeIDE**, selecione `pratica-rng`.
2. Compile o código pressionando `Ctrl + B`.
3. Grave o binário na placa clicando em **Run As** -> **STM32 C/C++ Application**.

---

## 🧪 Como Testar e Validar

1. Abra um terminal serial que suporte emulação ANSI (ex: `picocom`, PuTTY, Minicom ou o terminal integrado do Linux/VS Code):
   * **Porta:** `/dev/ttyACM0` (Linux) ou `COMx` (Windows)
   * **Baud Rate:** `115200`
   * **Configuração:** 8N1
2. A tela será limpa automaticamente e exibirá a moldura do jogo:
   ```text
   ========================================
           MELHOR DE 3 - RODADA 1
    PLACAR: Machine [ 0 ] x [ 0 ] Jogador
   ========================================
   Pressione qualquer tecla para jogar os dados...
   ```
3. Pressione a barra de espaço ou qualquer tecla:
   * Os dados serão rolados com entropia do hardware RNG:
   ```text
   Dado Machine: 5  |  Dado Jogador: 2
   >>> Ponto para a Machine nesta rodada!
   ```
4. Continue pressionando teclas para as rodadas seguintes até que o placar final seja atingido (ex: 2 x 0 ou 2 x 1), com anúncio do campeão.

---

## 🤖 Prompt Pronto para Agente IA

```text
Estou rodando a "pratica-rng" na placa NUCLEO-F767ZI.
O firmware utiliza o periférico de hardware True RNG para simular um jogo de dados "Melhor de 3" com renderização ANSI na USART3 (115200 bps).
Me instrua a conectar o terminal serial com suporte ANSI e validar o gerador de entropia do microcontrolador.
```
