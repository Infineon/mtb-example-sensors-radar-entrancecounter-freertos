/******************************************************************************
** File name: radar_led_task.h
**
** Description: This file contains the function prototypes and constants used
**   in radar_led_task.c.
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

/* Header file includes */
#include "cyabs_rtos.h"
#include "cycfg.h"

/* Header file for local task */
#include "radar_counter_task.h"

/* Header file for library */
#include "mtb_radar_sensing.h"

/*******************************************************************************
 * Macros
 *******************************************************************************/
/* LED task name */
#define RADAR_LED_TASK_NAME "RADAR LED TASK"
/* LED task stack size */
#define RADAR_LED_TASK_STACK_SIZE (512)
/* LED task priority */
#define RADAR_LED_TASK_PRIORITY (CY_RTOS_PRIORITY_BELOWNORMAL)

/*******************************************************************************
 * Functions
 *******************************************************************************/
void radar_led_task(cy_thread_arg_t arg);
void radar_led_set_pattern(mtb_radar_sensing_event_t event);
