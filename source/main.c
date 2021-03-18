/* ***************************************************************************
** File name: main.c
**
** Description: This is the main file for entrance counter example project.
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

/* Header file includes. */
#include "cy_retarget_io.h"
#include "cybsp.h"
#include "cyhal.h"

/* Header file for RTOS abstraction */
#include "cyabs_rtos.h"

/* Header file for local task */
#include "radar_counter_task.h"
#include "radar_counter_terminal_ui.h"
#include "radar_led_task.h"

/*******************************************************************************
 * Function Name: main
 ********************************************************************************
 * Summary:
 * This is the main function for example project that demonstrates entrance
 * counter use case of radar. It sets up board support package, global
 * interrupts, UART and LED ports. Two tasks are then created: one for entrance
 * counter application, another for terminal UI to configure entrance counter
 * parameters. Scheduler is then started.
 *
 * Parameters:
 *  none
 *
 * Return:
 *  int
 *
 *******************************************************************************/
int main(void)
{
    /* Initialize the board support package. */
    cy_rslt_t result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Enable global interrupts. */
    __enable_irq();

    /* Initialize retarget-io to use the debug UART port. */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* \x1b[2J\x1b[;H - ANSI ESC sequence to clear screen. */
    printf("\x1b[2J\x1b[;H");
    printf("====================================================================\r\n");
    printf("Connected Sensor Kit: Radar Entrance Counter Application on FreeRTOS\r\n");
    printf("====================================================================\r\n\r\n");

    /* Create task that initializes context object of RadarSensing for     */
    /* entrance counter, initializes radar device configuration, sets      */
    /* parameters for entrance counter, registers callback to handle       */
    /* counter events and continuously processes data acquired from radar. */
    cy_thread_t ifxradar_counter_task;
    result = cy_rtos_create_thread(&ifxradar_counter_task,
                                   radar_counter_task,
                                   RADAR_COUNTER_TASK_NAME,
                                   NULL,
                                   RADAR_COUNTER_TASK_STACK_SIZE,
                                   RADAR_COUNTER_TASK_PRIORITY,
                                   (cy_thread_arg_t)NULL);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Create task for a terminal UI that configures parameters for entrance */
    /* counter application.                                                  */
    cy_thread_t ifxradar_counter_terminal_ui;
    result = cy_rtos_create_thread(&ifxradar_counter_terminal_ui,
                                   radar_counter_terminal_ui,
                                   RADAR_COUNTER_TERMINAL_UI_TASK_NAME,
                                   NULL,
                                   RADAR_COUNTER_TERMINAL_UI_TASK_STACK_SIZE,
                                   RADAR_COUNTER_TERMINAL_UI_TASK_PRIORITY,
                                   (cy_thread_arg_t)NULL);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Create task to configure LED blinking pattern for entrance counter  */
    /* events.                                                             */
    cy_thread_t ifxradar_led_task;
    result = cy_rtos_create_thread(&ifxradar_led_task,
                                   radar_led_task,
                                   RADAR_LED_TASK_NAME,
                                   NULL,
                                   RADAR_LED_TASK_STACK_SIZE,
                                   RADAR_LED_TASK_PRIORITY,
                                   (cy_thread_arg_t)NULL);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Start the FreeRTOS scheduler. */
    vTaskStartScheduler();

    /* Should never get here. */
    CY_ASSERT(0);
}

/* [] END OF FILE */
