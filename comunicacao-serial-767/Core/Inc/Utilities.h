#ifndef UTILITIES_H_
#define UTILITIES_H_

void TIM2_Micros_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    __DSB();
    TIM2->CR1 &= ~TIM_CR1_CEN;

    /* Configura o Timer para 2 MHz (1 tick = 0.5 us)
       Considerando o Clock do APB1 em 96 MHz: PSC = (96 / 2) - 1 */
    TIM2->PSC = 47U;
    TIM2->ARR = 0xFFFFFFFFU;
    TIM2->CNT = 0U;
    TIM2->EGR = TIM_EGR_UG;
    TIM2->SR = 0U;
    TIM2->CR1 |= TIM_CR1_CEN;
}

/* Aguarda múltiplos de 0.5 us */
void Delay_Half_us(uint32_t half_us)
{
    TIM2->CNT = 0;
    while (TIM2->CNT < half_us);
}

/* Aguarda múltiplos de 1 us (multiplica por 2 ticks) */
void Delay_us(uint32_t tempo_us)
{
    TIM2->CNT = 0;
    while (TIM2->CNT < (tempo_us * 2));
}

void Delay_ms(uint32_t tempo_ms)
{
    TIM2->CNT = 0;
    while (TIM2->CNT < (tempo_ms * 2000));
}

#endif /* UTILITIES_H_ */
