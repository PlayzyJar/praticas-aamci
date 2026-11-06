# Prática: Estimativa de Pi por Monte Carlo com HW RNG (`pratica-rng-estimativa-pi`)

Implementação de computação numérica de alto desempenho no microcontrolador STM32F767ZI: estimativa contínua do número $\pi$ através do **Método de Monte Carlo**, alimentado pelo periférico de hardware **True RNG** e otimizado com aritmética de 64 bits para evitar overflow.

---

## 📋 Fundamentação Matemática e Computacional

O método de Monte Carlo para cálculo de $\pi$ fundamenta-se na razão de áreas entre um quadrante de círculo de raio $R$ e o quadrado circunscrito de lado $R$:

$$\frac{\text{Área do Quadrante}}{\text{Área do Quadrado}} = \frac{\frac{\pi R^2}{4}}{R^2} = \frac{\pi}{4} \implies \pi = 4 \times \frac{N_{\text{dentro}}}{N_{\text{total}}}$$

### Otimizações Implementadas em C (`pi_mc.c`)
1. **Espaço Amostral:** Coordenadas $x, y \in [0, \, 2^{32}-1]$ sorteadas diretamente do hardware RNG do STM32F767.
2. **Prevenção de Overflow de 64 bits:** A condição de pertinência ao círculo é $x^2 + y^2 \le (2^{32}-1)^2$. Em C com inteiros sem sinal de 64 bits (`uint64_t`), a soma direta $x^2 + y^2$ poderia estourar `UINT64_MAX`. O código utiliza a relação equivalente:
   ```c
   /* xx <= UINT64_MAX - yy (garante teste sem risco de overflow) */
   return xx <= (UINT64_MAX - yy);
   ```
3. **Execução em Lotes (Batching):** O algoritmo processa amostras em blocos (`PI_MC_BATCH`) a cada iteração, maximizando a taxa de amostragem na CPU Cortex-M7 @ 216 MHz.
4. **Ponto Fixo de Alta Precisão:** A função `pi_mc_micro()` calcula o valor em micro-unidades inteiras (`(4_000_000 * inside) / total`), permitindo imprimir 6 casas decimais exatas (`%lu.%06lu`) sem custos de ponto flutuante na formatação.

---

## 🔌 Hardware e Pinagem

| Sinal / Recurso | Pino STM32 | Modo / Função | Descrição |
|---|---|---|---|
| **HW True RNG** | Interno | Fonte de Entropia | Fornece pares $(x, y)$ aleatórios verdadeiros de 32 bits por hardware. |
| **USART3 TX** | `PD8` | AF7 (USART3_TX) | Transmissão de telemetria contínua com progresso e valor de $\pi$. |
| **USART3 RX** | `PD9` | AF7 (USART3_RX) | Linha de recepção serial (ST-LINK). |
| **CPU Cortex-M7** | Interno | 216 MHz | Execução paralela de instruções aritméticas de 64 bits. |

---

## ⚙️ Compilação e Gravação

1. No **STM32CubeIDE**, selecione `pratica-rng-estimativa-pi`.
2. Compile o projeto via `Ctrl + B`.
3. Conecte a placa NUCLEO-F767ZI via cabo USB.
4. Clique em **Run As** -> **STM32 C/C++ Application**.

---

## 🧪 Como Testar e Validar

1. Abra o monitor serial:
   * **Porta:** `/dev/ttyACM0` (Linux) ou `COMx` (Windows)
   * **Baud Rate:** `115200`
   * **Configuração:** 8N1
2. Logo após reiniciar a placa, você verá o banner inicial e as estimativas convergindo conforme os milhões de pontos são processados:
   ```text
   Estimativa de pi por Monte Carlo (RNG STM32F767)
   Mpontos=0 pi=3.141520
   Mpontos=1 pi=3.141634
   Mpontos=2 pi=3.141588
   Mpontos=3 pi=3.141595
   ...
   ```
3. Observe que:
   * `Mpontos` indica a quantidade de **milhões de pontos aleatórios** já gerados e avaliados.
   * Conforme o número de amostras cresce, o valor de `pi` se estabiliza em torno de `3.14159...` comprovando a lei dos grandes números e a distribuição uniforme do gerador RNG da ST.

---

## 🤖 Prompt Pronto para Agente IA

```text
Estou executando o projeto "pratica-rng-estimativa-pi" na placa STM32F767ZI.
Ele calcula o valor de Pi usando Monte Carlo através do HW RNG e aritmética de 64 bits com saída serial a 115200 bps.
Me ajude a monitorar o cálculo, analisar a taxa de convergência e verificar o desempenho da CPU Cortex-M7.
```
