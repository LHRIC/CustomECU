#include "us_timer.h"
#include "stm32f407xx.h"
#include "tim.h"
#include "ulog.h"

void init_us_timer(void) {
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim2.Instance = TIM2;

  // Set to 1 us tick, see clock diagram in STM32CubeMX.
  htim2.Init.Prescaler = HAL_RCC_GetPCLK2Freq() * 2;

  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295; // ~70 Min overflow.
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
    ULOG_ERROR("Failed to intialize microsecond timer!");
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
    ULOG_ERROR("Failed to configure microsecond timer clock source!");
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK) {
    ULOG_ERROR("Failed to initialize master configuration!");
  }
}

inline uint32_t get_micros(void) { return TIM2->CNT; }
