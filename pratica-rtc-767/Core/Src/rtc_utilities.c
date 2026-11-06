#include "rtc_utilities.h"
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

void RTC_InitDefault(void)
{
    RTC_CalendarTypeDef def_cal;
    def_cal.formato = 1; // Modo PM ativado (AM/PM no CR com bit PM setado)
    def_cal.dia = 1;     // Dia 1 da semana (1 = Mon)
    def_cal.data = 1;    // Dia 01 do mês
    def_cal.mes = 1;     // Mês 01
    def_cal.ano = 0;     // Ano 00
    def_cal.horas = 0;   // 00 horas
    def_cal.minutos = 0; // 00 minutos
    def_cal.segundos = 0;// 00 segundos

    RTC_SetCalendar(&def_cal);
}

void RTC_SetCalendar(const RTC_CalendarTypeDef *cal)
{
    if (cal == NULL)
    {
        return;
    }

    // Garante acesso de escrita ao domínio de backup
    HAL_PWR_EnableBkUpAccess();

    // Desabilita a proteção de escrita nos registradores do RTC
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    // Entra no modo de inicialização do calendário
    RTC->ISR |= RTC_ISR_INIT;
    while (!(RTC->ISR & RTC_ISR_INITF));

    // Configuração do formato da hora: 0 = 24h, 1 = AM/PM
    if (cal->formato)
    {
        RTC->CR |= RTC_CR_FMT;
    }
    else
    {
        RTC->CR &= ~RTC_CR_FMT;
    }

    uint8_t dezena_data = cal->data / 10;
    uint8_t unidade_data = cal->data % 10;
    uint8_t dezena_mes = cal->mes / 10;
    uint8_t unidade_mes = cal->mes % 10;
    uint8_t dezena_ano = cal->ano / 10;
    uint8_t unidade_ano = cal->ano % 10;

    uint8_t dezena_hora = cal->horas / 10;
    uint8_t unidade_hora = cal->horas % 10;
    uint8_t dezena_minuto = cal->minutos / 10;
    uint8_t unidade_minuto = cal->minutos % 10;
    uint8_t dezena_segundo = cal->segundos / 10;
    uint8_t unidade_segundo = cal->segundos % 10;

    // Bit PM (bit 22 do RTC_TR): se formato for AM/PM (formato=1)
    uint32_t pm_bit = 0;
    if (cal->formato)
    {
        pm_bit = (1UL << 22);
    }

    RTC->TR = pm_bit |
              (dezena_hora << 20) | (unidade_hora << 16) |
              (dezena_minuto << 12) | (unidade_minuto << 8) |
              (dezena_segundo << 4) | (unidade_segundo);

    RTC->DR = (dezena_ano << 20) | (unidade_ano << 16) |
              (((uint32_t)cal->dia & 0x07) << 13) |
              (dezena_mes << 12) | (unidade_mes << 8) |
              (dezena_data << 4) | (unidade_data);

    // Sai do modo de inicialização
    RTC->ISR &= ~RTC_ISR_INIT;

    // Aguarda sincronização dos registradores sombra com os novos valores
    RTC->ISR = (uint32_t)(RTC_RSF_MASK & RTC_ISR_RESERVED_MASK);
    while (!(RTC->ISR & RTC_ISR_RSF));

    // Reabilita a proteção de escrita nos registradores do RTC
    RTC->WPR = 0xFF;
}

RTC_StatusTypeDef RTC_ParseCalendarString(const char *str, RTC_CalendarTypeDef *cal)
{
    if (str == NULL || cal == NULL)
    {
        return RTC_STATUS_ERR_FORMAT;
    }

    // Validação de comprimento: exatamente 16 caracteres
    // Formato: DSDMMMAAAAHHMNSS
    if (strlen(str) != 16)
    {
        return RTC_STATUS_ERR_FORMAT;
    }

    // Validação de formato: todos os caracteres devem ser dígitos numéricos
    for (int i = 0; i < 16; i++)
    {
        if (!isdigit((unsigned char)str[i]))
        {
            return RTC_STATUS_ERR_FORMAT;
        }
    }

    // Extração dos campos numéricos
    uint32_t ds   = (str[0] - '0') * 10 + (str[1] - '0');
    uint32_t dm   = (str[2] - '0') * 10 + (str[3] - '0');
    uint32_t mm   = (str[4] - '0') * 10 + (str[5] - '0');
    uint32_t aaaa = (str[6] - '0') * 1000 + (str[7] - '0') * 100 +
                    (str[8] - '0') * 10 + (str[9] - '0');
    uint32_t hh   = (str[10] - '0') * 10 + (str[11] - '0');
    uint32_t mn   = (str[12] - '0') * 10 + (str[13] - '0');
    uint32_t ss   = (str[14] - '0') * 10 + (str[15] - '0');

    // Validação dos limites de valores
    if (ds < 1 || ds > 7)
    {
        return RTC_STATUS_ERR_LIMITS;
    }
    if (mm < 1 || mm > 12)
    {
        return RTC_STATUS_ERR_LIMITS;
    }
    // Cálculo do limite máximo de dias no mês (com regra de ano bissexto)
    uint32_t max_days = 31;
    if (mm == 4 || mm == 6 || mm == 9 || mm == 11)
    {
        max_days = 30;
    }
    else if (mm == 2)
    {
        bool is_leap = ((aaaa % 4 == 0 && aaaa % 100 != 0) || (aaaa % 400 == 0));
        max_days = is_leap ? 29 : 28;
    }

    if (dm < 1 || dm > max_days)
    {
        return RTC_STATUS_ERR_LIMITS;
    }
    if (hh > 23)
    {
        return RTC_STATUS_ERR_LIMITS;
    }
    if (mn > 59)
    {
        return RTC_STATUS_ERR_LIMITS;
    }
    if (ss > 59)
    {
        return RTC_STATUS_ERR_LIMITS;
    }

    // Preenchimento da estrutura (formato 24h para operação normal de calendário)
    cal->formato  = 0;
    cal->dia      = (uint8_t)ds;
    cal->data     = (uint8_t)dm;
    cal->mes      = (uint8_t)mm;
    cal->ano      = (uint8_t)(aaaa % 100);
    cal->horas    = (uint8_t)hh;
    cal->minutos  = (uint8_t)mn;
    cal->segundos = (uint8_t)ss;

    return RTC_STATUS_OK;
}
