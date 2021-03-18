/******************************************************************************
** File name: radar_counter_terminal_ui.h
**
** Description: This file contains the function prototypes and constants used
**   in radar_counter_terminal_ui.c.
**
** ===========================================================================
** Copyright (C) 2021 Infineon Technologies AG. All rights reserved.
** ===========================================================================
**
** ===========================================================================
** Infineon Technologies AG (INFINEON) is supplying this file for use
** exclusively with Infineon's sensor products. This file can be freely
** distributed within development tools and software supporting such
** products.
**
** THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
** OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
** MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
** INFINEON SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES, FOR ANY REASON
** WHATSOEVER.
** ===========================================================================
*/
#pragma once

/* Header file for local task */
#include "radar_counter_task.h"

/*******************************************************************************
 * Macros
 *******************************************************************************/
/* Name of the radar counter terminal ui task*/
#define RADAR_COUNTER_TERMINAL_UI_TASK_NAME "RADAR COUNTER TERMINAL UI"
/* Stack size for radar counter terminal ui */
#define RADAR_COUNTER_TERMINAL_UI_TASK_STACK_SIZE (2048)
/* Task priority for radar counter terminal ui */
#define RADAR_COUNTER_TERMINAL_UI_TASK_PRIORITY (CY_RTOS_PRIORITY_BELOWNORMAL)

/*******************************************************************************
 * Functions
 *******************************************************************************/
void radar_counter_terminal_ui(cy_thread_arg_t arg);
