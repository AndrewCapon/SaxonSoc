#pragma once

#include "soc.h"
#include "uart.h"
#include "clint.h"
#include "machineTimer.h"

#define BSP_HART_COUNT SYSTEM_CPU_COUNT

#define BSP_PLIC SYSTEM_PLIC_APB
#define BSP_PLIC_CPU_0 SYSTEM_PLIC_SYSTEM_CPU_EXTERNAL_INTERRUPT
//#define BSP_CLINT SYSTEM_CLINT_CTRL
#define BSP_CLINT_HZ SYSTEM_MACHINE_TIMER_HZ

#define BSP_UART_TERMINAL SYSTEM_UART_A_APB
#define BSP_LED_GPIO SYSTEM_GPIO_A_APB
#define BSP_LED_MASK 0x0F

#define bsp_init() {}
#define bsp_putChar(c) uart_write(BSP_UART_TERMINAL, c);
#define bsp_uDelay(usec) machineTimer_uDelay(usec, SYSTEM_MACHINE_TIMER_HZ, SYSTEM_MACHINE_TIMER_APB);
#define bsp_putString(s) uart_writeStr(BSP_UART_TERMINAL, s);

#define bsp_getTime() machineTimer_getTime(SYSTEM_MACHINE_TIMER_APB);
#define bsp_setCmp(cmp) machineTimer_setCmp(SYSTEM_MACHINE_TIMER_APB, cmp)

//
//// Freertos specifics
//#define configMTIME_BASE_ADDRESS        (BSP_CLINT + 0xBFF8)
//#define configMTIMECMP_BASE_ADDRESS     (BSP_CLINT + 0x4000)
//#define configCPU_CLOCK_HZ              ( ( uint32_t ) ( BSP_CLINT_HZ ) )