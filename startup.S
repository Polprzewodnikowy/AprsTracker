/*
 * startup.S
 *
 *  Created on: 7.1.2017
 *  Based on work of: freddiechopin.info
 *      Author: korgeaux
 */

.text
.balign 2
.syntax unified
.thumb
.thumb_func
.global Reset_Handler
Reset_Handler:
    ldr     r0, =__stack_end
    msr     MSP, r0

    ldr     r1, =__data_init_start
    ldr     r2, =__data_start
    ldr     r3, =__data_end

    b       2f
1:  ldmia   r1!, {r0}
    stmia   r2!, {r0}
2:  cmp     r2, r3
    bne     1b

    movs    r0, #0
    ldr     r1, =__bss_start
    ldr     r2, =__bss_end

    b       2f
1:  stmia   r1!, {r0}
2:  cmp     r1, r2
    bne     1b

    ldr     r0, =SystemInit
    blx     r0

    ldr     r0, =main
    blx     r0

    b       .

.text
.balign 2
.syntax unified
.thumb
.thumb_func
.global __SystemInit
__SystemInit:
    bx      lr

.weak   SystemInit
.global SystemInit
.set    SystemInit, __SystemInit

.text
.balign 2
.syntax unified
.thumb
.thumb_func
.global __Default_Handler
__Default_Handler:
    b       .

.weak   Default_Handler
.global Default_Handler
.set    Default_Handler, __Default_Handler

.section .vectors, "a", %progbits
.type Vector_Table, %object
.global Vector_Table
Vector_Table:
    .word __stack_end
    .word Reset_Handler
    .word NMI_Handler
    .word HardFault_Handler
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word SVC_Handler
    .word 0
    .word 0
    .word PendSV_Handler
    .word SysTick_Handler
    .word WWDG_IRQHandler                   /* Window WatchDog              */
    .word PVD_VDDIO2_IRQHandler             /* PVD and VDDIO2 through EXTI Line detect */
    .word RTC_IRQHandler                    /* RTC through the EXTI line    */
    .word FLASH_IRQHandler                  /* FLASH                        */
    .word RCC_CRS_IRQHandler                /* RCC and CRS                  */
    .word EXTI0_1_IRQHandler                /* EXTI Line 0 and 1            */
    .word EXTI2_3_IRQHandler                /* EXTI Line 2 and 3            */
    .word EXTI4_15_IRQHandler               /* EXTI Line 4 to 15            */
    .word TSC_IRQHandler                    /* TSC                          */
    .word DMA1_Channel1_IRQHandler          /* DMA1 Channel 1               */
    .word DMA1_Channel2_3_IRQHandler        /* DMA1 Channel 2 and Channel 3 */
    .word DMA1_Channel4_5_6_7_IRQHandler    /* DMA1 Channel 4, Channel 5, Channel 6 and Channel 7*/
    .word ADC1_COMP_IRQHandler              /* ADC1, COMP1 and COMP2         */
    .word TIM1_BRK_UP_TRG_COM_IRQHandler    /* TIM1 Break, Update, Trigger and Commutation */
    .word TIM1_CC_IRQHandler                /* TIM1 Capture Compare         */
    .word TIM2_IRQHandler                   /* TIM2                         */
    .word TIM3_IRQHandler                   /* TIM3                         */
    .word TIM6_DAC_IRQHandler               /* TIM6 and DAC                 */
    .word TIM7_IRQHandler                   /* TIM7                         */
    .word TIM14_IRQHandler                  /* TIM14                        */
    .word TIM15_IRQHandler                  /* TIM15                        */
    .word TIM16_IRQHandler                  /* TIM16                        */
    .word TIM17_IRQHandler                  /* TIM17                        */
    .word I2C1_IRQHandler                   /* I2C1                         */
    .word I2C2_IRQHandler                   /* I2C2                         */
    .word SPI1_IRQHandler                   /* SPI1                         */
    .word SPI2_IRQHandler                   /* SPI2                         */
    .word USART1_IRQHandler                 /* USART1                       */
    .word USART2_IRQHandler                 /* USART2                       */
    .word USART3_4_IRQHandler               /* USART3 and USART4            */
    .word CEC_CAN_IRQHandler                /* CEC and CAN                  */
    .word USB_IRQHandler                    /* USB                          */

    .weak      NMI_Handler
    .thumb_set NMI_Handler,Default_Handler
    .weak      HardFault_Handler
    .thumb_set HardFault_Handler,Default_Handler
    .weak      SVC_Handler
    .thumb_set SVC_Handler,Default_Handler
    .weak      PendSV_Handler
    .thumb_set PendSV_Handler,Default_Handler
    .weak      SysTick_Handler
    .thumb_set SysTick_Handler,Default_Handler
    .weak      WWDG_IRQHandler
    .thumb_set WWDG_IRQHandler,Default_Handler
    .weak      PVD_VDDIO2_IRQHandler
    .thumb_set PVD_VDDIO2_IRQHandler,Default_Handler
    .weak      RTC_IRQHandler
    .thumb_set RTC_IRQHandler,Default_Handler
    .weak      FLASH_IRQHandler
    .thumb_set FLASH_IRQHandler,Default_Handler
    .weak      RCC_CRS_IRQHandler
    .thumb_set RCC_CRS_IRQHandler,Default_Handler
    .weak      EXTI0_1_IRQHandler
    .thumb_set EXTI0_1_IRQHandler,Default_Handler
    .weak      EXTI2_3_IRQHandler
    .thumb_set EXTI2_3_IRQHandler,Default_Handler
    .weak      EXTI4_15_IRQHandler
    .thumb_set EXTI4_15_IRQHandler,Default_Handler
    .weak      TSC_IRQHandler
    .thumb_set TSC_IRQHandler,Default_Handler
    .weak      DMA1_Channel1_IRQHandler
    .thumb_set DMA1_Channel1_IRQHandler,Default_Handler
    .weak      DMA1_Channel2_3_IRQHandler
    .thumb_set DMA1_Channel2_3_IRQHandler,Default_Handler
    .weak      DMA1_Channel4_5_6_7_IRQHandler
    .thumb_set DMA1_Channel4_5_6_7_IRQHandler,Default_Handler
    .weak      ADC1_COMP_IRQHandler
    .thumb_set ADC1_COMP_IRQHandler,Default_Handler
    .weak      TIM1_BRK_UP_TRG_COM_IRQHandler
    .thumb_set TIM1_BRK_UP_TRG_COM_IRQHandler,Default_Handler
    .weak      TIM1_CC_IRQHandler
    .thumb_set TIM1_CC_IRQHandler,Default_Handler
    .weak      TIM2_IRQHandler
    .thumb_set TIM2_IRQHandler,Default_Handler
    .weak      TIM3_IRQHandler
    .thumb_set TIM3_IRQHandler,Default_Handler
    .weak      TIM6_DAC_IRQHandler
    .thumb_set TIM6_DAC_IRQHandler,Default_Handler
    .weak      TIM7_IRQHandler
    .thumb_set TIM7_IRQHandler,Default_Handler
    .weak      TIM14_IRQHandler
    .thumb_set TIM14_IRQHandler,Default_Handler
    .weak      TIM15_IRQHandler
    .thumb_set TIM15_IRQHandler,Default_Handler
    .weak      TIM16_IRQHandler
    .thumb_set TIM16_IRQHandler,Default_Handler
    .weak      TIM17_IRQHandler
    .thumb_set TIM17_IRQHandler,Default_Handler
    .weak      I2C1_IRQHandler
    .thumb_set I2C1_IRQHandler,Default_Handler
    .weak      I2C2_IRQHandler
    .thumb_set I2C2_IRQHandler,Default_Handler
    .weak      SPI1_IRQHandler
    .thumb_set SPI1_IRQHandler,Default_Handler
    .weak      SPI2_IRQHandler
    .thumb_set SPI2_IRQHandler,Default_Handler
    .weak      USART1_IRQHandler
    .thumb_set USART1_IRQHandler,Default_Handler
    .weak      USART2_IRQHandler
    .thumb_set USART2_IRQHandler,Default_Handler
    .weak      USART3_4_IRQHandler
    .thumb_set USART3_4_IRQHandler,Default_Handler
    .weak      CEC_CAN_IRQHandler
    .thumb_set CEC_CAN_IRQHandler,Default_Handler
    .weak      USB_IRQHandler
    .thumb_set USB_IRQHandler,Default_Handler
