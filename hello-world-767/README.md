# Hello World 767

Teste basico de GPIO e comunicacao serial na NUCLEO-F767ZI.

## Funcionamento

O firmware monitora continuamente o botao de usuario (B1) e transmite seu estado pela USART3. Enquanto o botao permanece pressionado, os tres LEDs onboard sao mantidos acesos; ao soltar, os LEDs sao apagados.

## Pinagem

| Recurso | Pino | Modo | Descricao |
|---|---|---|---|
| Botao B1 | `PC13` | Input | Nivel 1 quando pressionado |
| LD1 (Verde) | `PB0` | Output | LED onboard |
| LD2 (Azul) | `PB7` | Output | LED onboard |
| LD3 (Vermelho) | `PB14` | Output | LED onboard |
| USART3 TX/RX | `PD8` / `PD9` | AF7 | VCP via ST-LINK (115200 8N1) |

## Teste

1. Conecte a placa via USB e abra o terminal serial em 115200 8N1 (`/dev/ttyACM0` ou `COMx`).
2. O terminal recebera continuamente `estado: 0\r\n`.
3. Pressione o botao azul `PC13`: os tres LEDs acendem e o terminal passa a receber `estado: 1\r\n`.
