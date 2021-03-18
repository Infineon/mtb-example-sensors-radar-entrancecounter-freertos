/*****************************************************************************
** File name: radar_counter_task.c
**
** Description: This file uses RadarSensing library APIs to demonstrate
** entrance counter use case of radar.
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

/* Header file from system */
#include <inttypes.h>

/* Header file includes */
#include "cyhal.h"
#include "cy_retarget_io.h"

/* Header file for local task */
#include "radar_counter_task.h"
#include "radar_led_task.h"

/*******************************************************************************
 * Macros
 ******************************************************************************/
/* RADAR sensor SPI frequency */
#define SPI_FREQUENCY (25000000UL)

/*******************************************************************************
 * Global Variables
 ******************************************************************************/
mtb_radar_sensing_context_t sensing_context;
static cy_mutex_t terminal_print_mutex;

/*******************************************************************************
 * Function Name: radar_counter_terminal_mutex_get
 ********************************************************************************
 * Summary:
 *   Get mutex
 *
 * Parameters:
 *   timeout_ms: Maximum number of milliseconds to wait while attempting to
 *   get mutex
 *
 * Return:
 *   Status of mutex request
 *******************************************************************************/
static cy_rslt_t radar_counter_terminal_mutex_get(cy_time_t timeout_ms)
{
    return (cy_rtos_get_mutex(&terminal_print_mutex, timeout_ms));
}

/*******************************************************************************
 * Function Name: radar_counter_terminal_mutex_release
 ********************************************************************************
 * Summary:
 *  Set mutex
 *
 * Parameters:
 *   none
 *
 * Return:
 *   Status of mutex request
 *******************************************************************************/
static cy_rslt_t radar_counter_terminal_mutex_release(void)
{
    return (cy_rtos_set_mutex(&terminal_print_mutex));
}

/*******************************************************************************
 * Function Name: radar_counter_callback
 ********************************************************************************
 * Summary:
 *   Callback function that handles entrance counter events
 *
 * Parameters:
 *   context: context object of RadarSensing
 *   event: types of events that are detected
 *   event_info: description of the event
 *   data:
 *
 * Return:
 *   none
 *******************************************************************************/
static void radar_counter_callback(mtb_radar_sensing_context_t* context,
                                   mtb_radar_sensing_event_t event,
                                   mtb_radar_sensing_event_info_t *event_info,
                                   void *data)
{
    /* Update LED pattern */
    radar_led_set_pattern(event);

    if (radar_counter_terminal_mutex_get(0) == CY_RSLT_SUCCESS)
    {
        /* Print out entrance counter event messages */
        switch (event)
        {
            // people walking in detected
            case MTB_RADAR_SENSING_EVENT_COUNTER_IN:
                printf("%.2f: Counter IN detected, IN: %d, OUT: %d\r\n",
                       (float)event_info->timestamp / 1000,
                       ((mtb_radar_sensing_counter_event_info_t *)event_info)->in_count,
                       ((mtb_radar_sensing_counter_event_info_t *)event_info)->out_count);
                break;
            // people walking out detected
            case MTB_RADAR_SENSING_EVENT_COUNTER_OUT:
                printf("%.2f: Counter OUT detected, IN: %d, OUT: %d\r\n",
                       (float)event_info->timestamp / 1000,
                       ((mtb_radar_sensing_counter_event_info_t *)event_info)->in_count,
                       ((mtb_radar_sensing_counter_event_info_t *)event_info)->out_count);
                break;
            // object detected in traffic zone, reminder for social distancing
            case MTB_RADAR_SENSING_EVENT_COUNTER_OCCUPIED:
                printf("%.2f: Counter occupied detected, IN: %d, OUT: %d\r\n",
                       (float)event_info->timestamp / 1000,
                       ((mtb_radar_sensing_counter_event_info_t *)event_info)->in_count,
                       ((mtb_radar_sensing_counter_event_info_t *)event_info)->out_count);
                break;
            // no more object detected in traffic zone
            case MTB_RADAR_SENSING_EVENT_COUNTER_FREE:
                printf("%.2f: Counter free detected, IN: %d, OUT: %d\r\n",
                       (float)event_info->timestamp / 1000,
                       ((mtb_radar_sensing_counter_event_info_t *)event_info)->in_count,
                       ((mtb_radar_sensing_counter_event_info_t *)event_info)->out_count);
                break;
            default:
                break;
        }
        radar_counter_terminal_mutex_release();
    }
}

/*******************************************************************************
 * Function Name: ifx_currenttime
 ********************************************************************************
 * Summary:
 *   Obtains system time in ms
 *
 * Parameters:
 *   none
 *
 * Return:
 *   system time in ms
 *******************************************************************************/
uint64_t ifx_currenttime()
{
    return (uint64_t)xTaskGetTickCount() * portTICK_PERIOD_MS;
}

/*******************************************************************************
 * Function Name: radar_counter_task
 ********************************************************************************
 * Summary:
 *   Initializes context object of RadarSensing for entrance counter,
 *   initializes radar device configuration, sets parameters for entrance
 *   counter, registers callback to handle counterevents and
 *   continuously processes data acquired from radar.
 *
 * Parameters:
 *   arg: thread
 *
 * Return:
 *   none
 *******************************************************************************/
void radar_counter_task(cy_thread_arg_t arg)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;

    /* Initialize mutex for terminal print */
    result = cy_rtos_init_mutex(&terminal_print_mutex);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Declare SPI object */
    cyhal_spi_t mSPI;

    mtb_radar_sensing_hw_cfg_t hw_cfg = {.spi_cs = CYBSP_SPI_CS,
                                         .reset = CYBSP_GPIO11,
                                         .ldo_en = CYBSP_GPIO5,
                                         .irq = CYBSP_GPIO10,
                                         .spi = &mSPI};

    /* Activate radar reset pin */
    cyhal_gpio_init(hw_cfg.reset, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, true);

    /* Enable LDO */
    cyhal_gpio_init(hw_cfg.ldo_en, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, true);

    /* Enable IRQ pin */
    cyhal_gpio_init(hw_cfg.irq, CYHAL_GPIO_DIR_INPUT, CYHAL_GPIO_DRIVE_PULLDOWN, false);

    /* CS handled manually */
    cyhal_gpio_init(hw_cfg.spi_cs, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, true);

    /* Configure SPI interface */
    if (cyhal_spi_init(hw_cfg.spi,
                       CYBSP_SPI_MOSI,
                       CYBSP_SPI_MISO,
                       CYBSP_SPI_CLK,
                       NC,
                       NULL,
                       8,
                       CYHAL_SPI_MODE_00_MSB,
                       false) != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }
    /* Set the data rate to 25 Mbps */
    if (cyhal_spi_set_frequency(hw_cfg.spi, SPI_FREQUENCY) != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initialize RadarSensing context object for presence detection, */
    /* also initialize radar device configuration */
    if (mtb_radar_sensing_init(&sensing_context, &hw_cfg, MTB_RADAR_SENSING_MASK_COUNTER_EVENTS) !=
        MTB_RADAR_SENSING_SUCCESS)
    {
        printf("ifx_radar_sensing_init error - Radar Wingboard not connected?\n");
        CY_ASSERT(0);
    }

    /* Register callback to handle counter events */
    if (mtb_radar_sensing_register_callback(&sensing_context, radar_counter_callback, NULL) !=
        MTB_RADAR_SENSING_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Enable context object */
    if (mtb_radar_sensing_enable(&sensing_context) != MTB_RADAR_SENSING_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Set parameters for entrance counter */
    if (mtb_radar_sensing_set_parameter(&sensing_context, "radar_counter_installation", "side") !=
        MTB_RADAR_SENSING_SUCCESS)
    {
        CY_ASSERT(0);
    }
    if (mtb_radar_sensing_set_parameter(&sensing_context, "radar_counter_orientation", "landscape") !=
        MTB_RADAR_SENSING_SUCCESS)
    {
        CY_ASSERT(0);
    }
    if (mtb_radar_sensing_set_parameter(&sensing_context, "radar_counter_entrance_width", "1.0") !=
        MTB_RADAR_SENSING_SUCCESS)
    {
        CY_ASSERT(0);
    }
    if (mtb_radar_sensing_set_parameter(&sensing_context, "radar_counter_traffic_light_zone", "0.2") !=
        MTB_RADAR_SENSING_SUCCESS)
    {
        CY_ASSERT(0);
    }
    for (;;)
    {
        /* Process data acquired from radar every 2ms */
        if (mtb_radar_sensing_process(&sensing_context, ifx_currenttime()) != MTB_RADAR_SENSING_SUCCESS)
        {
            printf("mtb_radar_sensing_process error\r\n");
            CY_ASSERT(0);
        }
        vTaskDelay(MTB_RADAR_SENSING_PROCESS_DELAY);
    }
}

/*******************************************************************************
 * Function Name: radar_counter_task_set_mute
 ********************************************************************************
 * Summary:
 *   Temporarily disables console output.
 *
 * Parameters:
 *   mute: true if muted. Care should be taken to match every
 *   radar_counter_task_set_mute(true) call with a call to
 *   radar_counter_task_set_mute(false). Failure to do so is likely to result
 *   in unpredictable behavior.
 *
 * Return:
 *   none
 *******************************************************************************/
void radar_counter_task_set_mute(bool mute)
{
    if (mute)
    {
        radar_counter_terminal_mutex_get(CY_RTOS_NEVER_TIMEOUT);
        return;
    }
    radar_counter_terminal_mutex_release();
}
