/*
 * Copyright (c) 2022-2023, The Puppy RTOS Authors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "main.h"
#include <puppy.h>

int main(void)
{
    int board_init(void);
    board_init();

    puppy_init();
    puppy_start();
}

