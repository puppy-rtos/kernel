/*
 * Copyright (c) 2022, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#ifndef __ARMCC_VERSION

extern uint8_t _estack;
extern uint8_t _sdata;
extern uint8_t _edata;
extern uint8_t _sidata;
extern uint8_t _sbss;
extern uint8_t _ebss;

extern int entry(void);

void Reset_handler(void)
{
    __asm ("ldr  sp, =_estack");
    uint8_t *flash_src = &_sidata;
    uint8_t *ram_dst = &_sdata;
    while (ram_dst < &_edata)
    {
        *ram_dst = *flash_src;
        ram_dst ++;
        flash_src ++;
    }
    for (ram_dst = &_sbss; ram_dst < &_ebss; ram_dst ++)
    {
        *ram_dst = 0;
    }
    main();
}

void Default_Handler()    __attribute__((weak));
void NMI_Handler()        __attribute__((weak));
void HardFault_Handler()  __attribute__((weak));
void MemManage_Handler()  __attribute__((weak));
void BusFault_Handler()   __attribute__((weak));
void UsageFault_Handler() __attribute__((weak));
void SVC_Handler()        __attribute__((weak));
void DebugMon_Handler()   __attribute__((weak));
void PendSV_Handler()     __attribute__((weak));
void SysTick_Handler()    __attribute__((weak));

__attribute__((section(".isr_vector"))) 
uint32_t *isr_vectors[] = {
    (uint32_t *)(&_estack),
    (uint32_t *)Reset_handler,
    (uint32_t *)NMI_Handler,
    (uint32_t *)HardFault_Handler,
    (uint32_t *)MemManage_Handler,
    (uint32_t *)BusFault_Handler,
    (uint32_t *)UsageFault_Handler,
    (uint32_t *)0,
    (uint32_t *)0,
    (uint32_t *)0,
    (uint32_t *)0,
    (uint32_t *)SVC_Handler,
    (uint32_t *)DebugMon_Handler,
    (uint32_t *)0,
    (uint32_t *)PendSV_Handler,
    (uint32_t *)SysTick_Handler,
};

__attribute__((weak))
void Default_Handler(void)
{
    while(1);
}

__attribute__((weak))
void NMI_Handler(void)
{
    while(1);
}

__attribute__((weak))
void HardFault_Handler(void)
{
    while(1);
}

__attribute__((weak))
void MemManage_Handler(void)
{
    while(1);
}

__attribute__((weak))
void BusFault_Handler(void)
{
    while(1);
}

__attribute__((weak))
void UsageFault_Handler(void)
{
    while(1);
}

__attribute__((weak))
void SVC_Handler(void)
{
    while(1);
}

__attribute__((weak))
void DebugMon_Handler(void)
{
    while(1);
}

__attribute__((weak))
void PendSV_Handler(void)
{
    while(1);
}

__attribute__((weak))
void SysTick_Handler(void)
{
    while(1);
}

#endif
