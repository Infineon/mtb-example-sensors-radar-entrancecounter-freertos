/******************************************************************************
** File Name:   radar_counter_task.h
**
** Description: This file contains the function prototypes and constants used
**   in radar_counter_task.c.
**
** Related Document: See README.md
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

/* Header file for library */
#include "mtb_radar_sensing.h"

/*******************************************************************************
 * Macros
 *******************************************************************************/
/* Radar counter task name */
#define RADAR_COUNTER_TASK_NAME "RADAR COUNTER TASK"
/* Radar counter task stack */
#define RADAR_COUNTER_TASK_STACK_SIZE (1024 * 4)
/* Radar counter task priority */
#define RADAR_COUNTER_TASK_PRIORITY (CY_RTOS_PRIORITY_NORMAL)

/*******************************************************************************
 * Global Variables
 *******************************************************************************/
extern mtb_radar_sensing_context_t sensing_context;

/*******************************************************************************
 * Functions
 *******************************************************************************/
void radar_counter_task(cy_thread_arg_t arg);
void radar_counter_task_set_mute(bool mute);
