/**
******************************************************************************
* @file    bluenrg_lp_evb_ll_button.c
* @author  RF Application Team
* @brief   This file provides set of firmware functions to manage push-button
*          available on BlueNRGLP-EVB Kit from STMicroelectronics.
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT(c) 2019 STMicroelectronics</center></h2>
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*   1. Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*   2. Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*   3. Neither the name of STMicroelectronics nor the names of its contributors
*      may be used to endorse or promote products derived from this software
*      without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "bluenrg_lp_evb_button.h"

/** @addtogroup BSP
* @{
*/

/** @defgroup BSP_BLUENRGLP_BUTTON BSP BlueNRG-LP button
* @{
*/


/** @defgroup BSP_BLUENRGLP_BUTTON_Exported_Functions Exported Functions
* @{
*/

/**
* @brief  Configures Button GPIO and EXTI Line.
* @param  Button: Specifies the Button to be configured.
*   This parameter can be one of following parameters:
*     @arg BSP_PUSH1
*     @arg BSP_PUSH2
* @param  ButtonMode: Specifies Button mode.
*   This parameter can be one of following parameters:   
*     @arg BUTTON_MODE_GPIO: Button will be used as simple IO 
*     @arg BUTTON_MODE_EXTI: Button will be connected to EXTI line with interrupt
*                            generation capability  
* @retval None
*/
void BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode)
{
  /* Configure the button */
  if(Button == BSP_SLIDEUP) {
    BSP_SLIDEUP_GPIO_CLK_ENABLE();
    LL_GPIO_SetPinMode(BSP_SLIDEUP_GPIO_PORT, BSP_SLIDEUP_PIN, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(BSP_SLIDEUP_GPIO_PORT, BSP_SLIDEUP_PIN, LL_GPIO_PULL_NO);

    LL_AHB_EnableClock(LL_AHB_PERIPH_GPIOB);
    // BAT_EN
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_5, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_5, LL_GPIO_PULL_NO);
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_5);

    // DETGND_EN
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_10, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_10, LL_GPIO_PULL_NO);
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_10);

    // RL_EN
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_4, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_4, LL_GPIO_PULL_NO);
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4);
    
//    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_9, LL_GPIO_MODE_INPUT);
//    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_9, LL_GPIO_PULL_UP);
//    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_10, LL_GPIO_MODE_INPUT);
//    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_10, LL_GPIO_PULL_UP);
//    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_11, LL_GPIO_MODE_INPUT);
//    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_11, LL_GPIO_PULL_UP);

    // I2C
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_0, LL_GPIO_PULL_NO);    
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_1, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_1, LL_GPIO_PULL_NO);

    // IO2 + IO2_EN
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_6, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_6, LL_GPIO_PULL_NO);    
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_6);
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_7, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_7, LL_GPIO_PULL_NO);
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_7);

    // USB IN + USB CHG
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_8, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_8, LL_GPIO_PULL_NO);
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_9, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_9, LL_GPIO_PULL_NO);

    // 3D_INT
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_11, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_11, LL_GPIO_PULL_NO);
    
    // VOUT_EN
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_15, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_15, LL_GPIO_PULL_NO);
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_15);

    // MA_EN
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_14, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_14, LL_GPIO_PULL_DOWN);
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_14);

		// FLASH SPI
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_4, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_4, LL_GPIO_PULL_NO);
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_4);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_5, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_5, LL_GPIO_PULL_NO);
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_5);
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_7, LL_GPIO_PULL_NO);
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_7);
    // FLASH_CS
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_6, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_6, LL_GPIO_PULL_NO);
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_6);
    // FLASH_VDD
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_8, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_8, LL_GPIO_PULL_NO);    
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_8);

    // BAT_ADC
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_1, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_1, LL_GPIO_PULL_NO);
    // RLH_ADC
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_2, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_2, LL_GPIO_PULL_NO);   
    // RLL_ADC
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_3, LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_3, LL_GPIO_PULL_NO);
    // TEMP_ADC
    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_0, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_0, LL_GPIO_PULL_NO);    
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_0);
  }
  
  if(ButtonMode == BUTTON_MODE_EXTI) {
    
    /* Enable System Config clock */
    LL_APB0_EnableClock(LL_APB0_PERIPH_SYSCFG);

    if(Button == BSP_SLIDEUP) {
      LL_EXTI_SetTrigger(BSP_SLIDEUP_EXTI_LINE_TRIGGER, BSP_SLIDEUP_EXTI_LINE);
      LL_EXTI_ClearInterrupt(BSP_SLIDEUP_EXTI_LINE);
      LL_EXTI_EnableIT(BSP_SLIDEUP_EXTI_LINE);

      /* Enable and set Button EXTI Interrupt to the lowest priority */
      NVIC_SetPriority(BSP_SLIDEUP_EXTI_IRQn, IRQ_LOW_PRIORITY);
      NVIC_EnableIRQ(BSP_SLIDEUP_EXTI_IRQn);
    }
  }
}


/**
* @brief  Push Button DeInit.
* @param  Button: Button to be configured
*   This parameter can be one of following parameters:
*     @arg BSP_PUSH1
*     @arg BSP_PUSH2
* @note PB DeInit does not disable the GPIO clock
* @retval None
*/
void BSP_PB_DeInit(Button_TypeDef Button)
{
  /* Deinitialization of the button */
  if(Button == BSP_SLIDEUP) {
    LL_EXTI_DisableIT(BSP_SLIDEUP_EXTI_LINE);
    LL_EXTI_ClearInterrupt(BSP_SLIDEUP_EXTI_LINE);
    LL_GPIO_SetPinMode(BSP_SLIDEUP_GPIO_PORT, BSP_SLIDEUP_PIN, LL_GPIO_MODE_ANALOG);
  }  
}


/**
* @brief  Returns the selected Button state.
* @param  Button: Specifies the Button to be checked.
*   This parameter can be one of following parameters:
*     @arg BSP_PUSH1
*     @arg BSP_PUSH2
* @retval The Button GPIO pin value.
*/
uint32_t BSP_PB_GetState(Button_TypeDef Button)
{
  uint32_t tmp = 0;
  
  if(Button == BSP_SLIDEUP) {
    tmp =  LL_GPIO_IsInputPinSet(BSP_SLIDEUP_GPIO_PORT, BSP_SLIDEUP_PIN);
  }
  return tmp;
}


/**
* @brief  Returns the pending bit associated to the button.
* @param  Button: Specifies the Button to be checked.
*   This parameter can be one of following parameters:
*     @arg BSP_PUSH1
*     @arg BSP_PUSH2
* @retval The pending bit associated to the button.
*/
uint32_t BSP_PB_GetITPendingBit(Button_TypeDef Button)
{
  uint32_t tmp = 0;
  
  if(Button == BSP_SLIDEUP) {
    tmp = LL_EXTI_IsInterruptPending(BSP_SLIDEUP_EXTI_LINE);
  }
  
  return tmp;
}


/**
* @brief  Clear the pending bit associated to the button.
* @param  Button: Specifies the Button to be checked.
*   This parameter can be one of following parameters:
*     @arg BSP_PUSH1
*     @arg BSP_PUSH2
* @retval The Button GPIO pin value.
*/
void BSP_PB_ClearITPendingBit(Button_TypeDef Button)
{
  if(Button == BSP_SLIDEUP) {
    LL_EXTI_ClearInterrupt(BSP_SLIDEUP_EXTI_LINE);
  }  
}


/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/    

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
