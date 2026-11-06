#ifndef INC_RTC_UTILITIES_H_
#define INC_RTC_UTILITIES_H_

#include "main.h"

/**
 * @brief Status retornado pela validação e parser da string do calendário.
 */
typedef enum {
    RTC_STATUS_OK = 0,
    RTC_STATUS_ERR_FORMAT,
    RTC_STATUS_ERR_LIMITS
} RTC_StatusTypeDef;

/**
 * @brief Estrutura para armazenamento dos campos de data e hora do RTC.
 */
typedef struct {
    uint8_t formato;  // 0 = 24h, 1 = AM/PM
    uint8_t dia;      // 1 a 7 (dia da semana: 1 = Mon, 2 = Tue, ..., 7 = Sun)
    uint8_t data;     // 1 a 31 (dia do mês)
    uint8_t mes;      // 1 a 12 (mês)
    uint8_t ano;      // 0 a 99 (ano, dois dígitos)
    uint8_t horas;    // 0 a 23
    uint8_t minutos;  // 0 a 59
    uint8_t segundos; // 0 a 59
} RTC_CalendarTypeDef;

/**
 * @brief Inicializa o RTC com valores padrão:
 *        Dia 1 da semana, Data 01/01/00, 00:00:00 e modo PM ativado.
 */
void RTC_InitDefault(void);

/**
 * @brief Atualiza os registradores do RTC (CR, TR e DR) a partir da estrutura informada.
 * @param cal Ponteiro para a estrutura com os novos dados de data e hora.
 */
void RTC_SetCalendar(const RTC_CalendarTypeDef *cal);

/**
 * @brief Analisa e valida uma string no formato DSDMMMAAAAHHMNSS.
 * @param str String com 16 dígitos numéricos.
 * @param cal Ponteiro para a estrutura que receberá os valores convertidos.
 * @return RTC_STATUS_OK se válida,
 *         RTC_STATUS_ERR_FORMAT se formato/tamanho incorreto,
 *         RTC_STATUS_ERR_LIMITS se valores fora dos limites.
 */
RTC_StatusTypeDef RTC_ParseCalendarString(const char *str, RTC_CalendarTypeDef *cal);

#endif /* INC_RTC_UTILITIES_H_ */
