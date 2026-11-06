#include "ssd1306_anim.h"

void SSD1306_DrawFrame(const uint8_t *frame_buffer) {
    if (frame_buffer == NULL) return;
    ssd1306_FillBuffer((uint8_t*)frame_buffer, SSD1306_BUFFER_SIZE);
    ssd1306_UpdateScreen();
}

void SSD1306_PlayAnimation(const uint8_t frames[][1024], uint16_t frame_count, uint8_t fps, uint16_t loops) {
    if (frames == NULL || frame_count == 0 || fps == 0) return;

    uint32_t frame_target_delay = 1000 / fps;
    uint16_t current_loop = 0;

    while (loops == 0 || current_loop < loops) {
        for (uint16_t i = 0; i < frame_count; i++) {
            uint32_t start_tick = HAL_GetTick();

            // Atualiza display com o frame da flash
            SSD1306_DrawFrame(frames[i]);

            // Compensacao automatica do tempo de transmissao I2C
            uint32_t elapsed = HAL_GetTick() - start_tick;
            if (elapsed < frame_target_delay) {
                HAL_Delay(frame_target_delay - elapsed);
            }
        }
        current_loop++;
    }
}

void SSD1306_DemoProceduralAnimation(uint32_t duration_ms) {
    static const int8_t cos_table[16] = { 16, 15, 11,  6,   0,  -6, -11, -15, -16, -15, -11,  -6,   0,   6,  11,  15 };
    static const int8_t sin_table[16] = {  0,  6, 11, 15,  16,  15,  11,   6,   0,  -6, -11, -15, -16, -15, -11,  -6 };

    uint32_t start = HAL_GetTick();
    uint8_t angle_idx = 0;
    int ball_x = 20, ball_y = 25;
    int dx = 2, dy = 1;

    while (HAL_GetTick() - start < duration_ms) {
        uint32_t frame_start = HAL_GetTick();

        ssd1306_Fill(Black);

        // Moldura externa elegante com cantos chanfrados
        ssd1306_DrawRectangle(0, 0, 127, 63, White);

        // Animacao de esfera quicando dentro dos limites
        ball_x += dx;
        ball_y += dy;
        if (ball_x <= 6 || ball_x >= 121) dx = -dx;
        if (ball_y <= 16 || ball_y >= 52) dy = -dy;

        ssd1306_FillCircle((uint8_t)ball_x, (uint8_t)ball_y, 3, White);

        // Radar rotativo central
        ssd1306_DrawCircle(64, 35, 16, White);
        ssd1306_DrawCircle(64, 35, 8, White);
        int rx = 64 + cos_table[angle_idx];
        int ry = 35 + sin_table[angle_idx];
        ssd1306_Line(64, 35, (uint8_t)rx, (uint8_t)ry, White);

        // Cabecalho informativo
        ssd1306_SetCursor(4, 3);
        ssd1306_WriteString("OLED RADAR DEMO", Font_6x8, White);

        // Barra inferior animada de progresso
        uint32_t progress = (HAL_GetTick() - start) * 120 / duration_ms;
        if (progress > 120) progress = 120;
        ssd1306_DrawRectangle(3, 58, 124, 61, White);
        ssd1306_Line(4, 59, (uint8_t)(4 + progress), 59, White);

        // Envia o buffer via I2C para o OLED
        ssd1306_UpdateScreen();

        angle_idx = (angle_idx + 1) % 16;

        // Mantem cadencia estavel (~20 FPS)
        uint32_t elapsed = HAL_GetTick() - frame_start;
        if (elapsed < 50) {
            HAL_Delay(50 - elapsed);
        }
    }
}
