/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author         Notes
 * 2021-01-28     flybreak       first version
 */

#include <puppy.h>

#define LED_PIN 25
void rt_hw_board_init();

int main(void)
{
    rt_hw_board_init();
    puppy_init();
    puppy_start();
}
