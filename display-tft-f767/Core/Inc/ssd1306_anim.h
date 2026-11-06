#ifndef __SSD1306_ANIM_H__
#define __SSD1306_ANIM_H__

#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Desenha um quadro diretamente da Flash/RAM no display SSD1306
 * @param frame_buffer Ponteiro para o buffer do quadro (1024 bytes para 128x64)
 */
void SSD1306_DrawFrame(const uint8_t *frame_buffer);

/**
 * @brief Reproduz uma animacao gravada em memoria Flash
 * @param frames Matriz de frames no formato SSD1306 (cada quadro com 1024 bytes para 128x64)
 * @param frame_count Quantidade total de quadros
 * @param fps Taxa de quadros por segundo desejada (recomendado: 12 a 20 FPS para I2C)
 * @param loops Quantidade de vezes a repetir (0 para loop infinito)
 */
void SSD1306_PlayAnimation(const uint8_t frames[][1024], uint16_t frame_count, uint8_t fps, uint16_t loops);

/**
 * @brief Roda uma animacao procedural de demonstracao em tempo real (Radar, bola quicando e barra)
 * @param duration_ms Duracao em milissegundos da animacao (ex: 5000 para 5 segundos)
 */
void SSD1306_DemoProceduralAnimation(uint32_t duration_ms);

#ifdef __cplusplus
}
#endif

#endif /* __SSD1306_ANIM_H__ */
