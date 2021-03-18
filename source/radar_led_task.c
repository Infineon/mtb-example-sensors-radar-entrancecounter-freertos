/*****************************************************************************
** File name: radar_led_task.c
**
** Description: This file implements a LED blinking pattern for entrance
** counter events.
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

/* Header file includes */
#include "cy_retarget_io.h"
#include "cybsp.h"
#include "cyhal.h"

/* Header file for local task */
#include "radar_counter_task.h"
#include "radar_led_task.h"

/*******************************************************************************
 * Macros
 *******************************************************************************/
/* Pin number designated for LED RED */
#define LED_RGB_RED (CYBSP_GPIOA0)
/* Pin number designated for LED GREEN */
#define LED_RGB_GREEN (CYBSP_GPIOA1)
/* Pin number designated for LED BLUE */
#define LED_RGB_BLUE (CYBSP_GPIOA2)
/* LED off */
#define LED_STATE_OFF (0U)
/* LED on */
#define LED_STATE_ON (1U)

/*******************************************************************************
 * Constants
 *******************************************************************************/
/* LED blink timing */
#define COUNTER_IN_LED_ON_TIME     (3U)
#define COUNTER_IN_LED_OFF_TIME    (20U)
#define COUNTER_OUT_LED_ON_TIME    (3U)
#define COUNTER_OUT_LED_OFF_TIME   (50U)
#define COUNTER_IN_LED_BLINK_TIME  (5U)
#define COUNTER_OUT_LED_BLINK_TIME (2U)

/* List of LED status */
typedef enum
{
    LED_PRESENCE,
    LED_ABSENCE,
    LED_COUNTER_IN,
    LED_COUNTER_OUT,
    LED_COUNTER_OCCUPIED,
    LED_COUNTER_FREE,
    LED_ERROR,
    LED_OFF
} led_condition_t;

/* List of LED colors */
typedef enum
{
    LED_NULL = 0x00,
    LED_RED = 0x01,
    LED_GREEN = 0x02,
    LED_BLUE = 0x04
} LED_COLOR;

/*******************************************************************************
 * Global Variables
 *******************************************************************************/
static led_condition_t led_state = LED_OFF; // LED status of different event
static LED_COLOR led_color = LED_GREEN;     // current LED color
static uint8_t led_counter_in_num = 0;      // current unprocessed counter IN event number
static uint8_t led_counter_out_num = 0;     // current unprocessed counter OUT event number
static uint8_t led_onoff_time_in = 0;       // LED/GPIO on off time counter for IN event
static uint8_t led_onoff_time_out = 0;      // LED/GPIO on off time counter for OUT event
static uint8_t led_blink_count_in = 0;      // LED blink counter for IN event
static uint8_t led_blink_count_out = 0;     // LED blink counter for OUT event

/*******************************************************************************
 * Function Name: gpio_led_set
 ********************************************************************************
 * Summary:
 *   This function sets GPIOs to activate LED set by the user.
 *
 * Parameters:
 *   led_set: LED color
 *
 * Return
 *   none
 *******************************************************************************/
static void gpio_led_set(uint32_t led_set)
{
    /* Set red LED */
    if (led_set & LED_RED)
    {
        cyhal_gpio_write(LED_RGB_RED, LED_STATE_ON);
    }
    else
    {
        cyhal_gpio_write(LED_RGB_RED, LED_STATE_OFF);
    }
    /* Set green LED */
    if (led_set & LED_GREEN)
    {
        cyhal_gpio_write(LED_RGB_GREEN, LED_STATE_ON);
    }
    else
    {
        cyhal_gpio_write(LED_RGB_GREEN, LED_STATE_OFF);
    }
    /* Set blue LED */
    if (led_set & LED_BLUE)
    {
        cyhal_gpio_write(LED_RGB_BLUE, LED_STATE_ON);
    }
    else
    {
        cyhal_gpio_write(LED_RGB_BLUE, LED_STATE_OFF);
    }
}

/*******************************************************************************
 * Function Name: radar_led_set_pattern
 ********************************************************************************
 * Summary:
 *   This function sets LED blinking pattern for entrance counter events.
 *
 * Parameters:
 *   event: Counter event
 *
 * Return
 *   none
 *******************************************************************************/
void radar_led_set_pattern(mtb_radar_sensing_event_t event)
{
    if (event == MTB_RADAR_SENSING_EVENT_COUNTER_IN)
    {
        /* Override LED pattern for LED drive mode */
        led_counter_in_num = 1;
        led_counter_out_num = 0;
        led_onoff_time_out = 0;
        led_blink_count_out = 0;
    }
    else if (event == MTB_RADAR_SENSING_EVENT_COUNTER_OUT)
    {
        /* Override LED pattern for LED drive mode */
        led_counter_out_num = 1;
        led_counter_in_num = 0;
        led_onoff_time_in = 0;
        led_blink_count_in = 0;
    }
    else if (event == MTB_RADAR_SENSING_EVENT_COUNTER_OCCUPIED)
    {
        led_state = LED_COUNTER_OCCUPIED;
        led_color = LED_RED;
    }
    else if (event == MTB_RADAR_SENSING_EVENT_COUNTER_FREE)
    {
        led_state = LED_COUNTER_FREE;
        led_color = LED_GREEN;
    }
}

/*******************************************************************************
 * Function Name: radar_led_task
 ********************************************************************************
 * Summary:
 *   This function initializes parameters for LED blinking pattern.
 *
 * Parameters:
 *   arg: thread
 *
 * Return
 *   none
 *******************************************************************************/
void radar_led_task(cy_thread_arg_t arg)
{
    /* Initialize the three LED ports and set the LEDs' initial state to off*/
    cy_rslt_t result = cyhal_gpio_init(LED_RGB_RED, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, LED_STATE_OFF);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    result = cyhal_gpio_init(LED_RGB_GREEN, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, LED_STATE_OFF);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    result = cyhal_gpio_init(LED_RGB_BLUE, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_STRONG, LED_STATE_OFF);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    for (;;)
    {
        /* Process counter IN event */
        if (led_counter_in_num > 0)
        {
            /* Blink LED according to COUNTER_IN_ON_TIME */
            led_onoff_time_in++;
            if (led_onoff_time_in < COUNTER_IN_LED_ON_TIME)
            {
                gpio_led_set(led_color);
            }
            else if (led_onoff_time_in < COUNTER_IN_LED_OFF_TIME)
            {
                gpio_led_set(LED_NULL);
            }
            else
            {
                led_onoff_time_in = 0;
                if (led_blink_count_in > COUNTER_IN_LED_BLINK_TIME)
                {
                    /* Deduct count after a LED blink generated */
                    led_counter_in_num--;
                    led_blink_count_in = 0;
                    /* Resume LED */
                    gpio_led_set(led_color);
                }
                else
                {
                    /* Repeat blink */
                    led_blink_count_in++;
                }
            }
        }
        /* Process counter OUT event */
        else if (led_counter_out_num > 0)
        {
            /* Blink LED according to COUNTER_OUT_ON_TIME */
            led_onoff_time_out++;
            if (led_onoff_time_out < COUNTER_OUT_LED_ON_TIME)
            {
                gpio_led_set(led_color);
            }
            else if (led_onoff_time_out < COUNTER_OUT_LED_OFF_TIME)
            {
                gpio_led_set(LED_NULL);
            }
            else
            {
                led_onoff_time_out = 0;
                if (led_blink_count_out > COUNTER_OUT_LED_BLINK_TIME)
                {
                    /* Deduct count after a LED blink generated */
                    led_counter_out_num--;
                    led_blink_count_out = 0;
                    /* Resume LED */
                    gpio_led_set(led_color);
                }
                else
                {
                    /* Repeat blink */
                    led_blink_count_out++;
                }
            }
        }
        /* Process traffic light event */
        else if ((led_state == LED_COUNTER_OCCUPIED) || (led_state == LED_COUNTER_FREE))
        {
            gpio_led_set(led_color);
        }
        /* Include 2ms delay */
        vTaskDelay(2);
    }
}
