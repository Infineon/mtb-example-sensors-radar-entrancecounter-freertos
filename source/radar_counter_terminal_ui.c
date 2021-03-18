/*****************************************************************************
** File name: radar_counter_terminal_ui.c
**
** Description: This file implements a terminal UI to configure parameters
** for entrance counter application.
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
#include <ctype.h>

/* Header file includes */
#include "cy_retarget_io.h"
#include "cybsp.h"
#include "cyhal.h"

/* Header file for local task */
#include "radar_counter_task.h"
#include "radar_counter_terminal_ui.h"

/*******************************************************************************
 * Constants
 *******************************************************************************/
#define IFX_RADAR_SENSING_VALUE_MAXLENGTH 256

/*******************************************************************************
 * Function Name: terminal_ui_menu
 ********************************************************************************
 * Summary:
 *   This function prints the available parameters configurable for entrance
 *   counter application. The existing values of the parameters are also
 *   displayed with their units (if any).
 *
 * Parameters:
 *   none
 *
 * Return:
 *   none
 *******************************************************************************/
static void terminal_ui_menu(void)
{
    char value[IFX_RADAR_SENSING_VALUE_MAXLENGTH];
    radar_counter_task_set_mute(true);
    /* Print main menu */
    printf("Select a setting to configure\r\n");
    mtb_radar_sensing_get_parameter(&sensing_context,
                                    "radar_counter_installation",
                                    value,
                                    IFX_RADAR_SENSING_VALUE_MAXLENGTH);
    printf("'i': installation (%s)\r\n", value);
    mtb_radar_sensing_get_parameter(&sensing_context,
                                    "radar_counter_orientation",
                                    value,
                                    IFX_RADAR_SENSING_VALUE_MAXLENGTH);
    printf("'o': orientation (%s)\r\n", value);
    mtb_radar_sensing_get_parameter(&sensing_context,
                                    "radar_counter_ceiling_height",
                                    value,
                                    IFX_RADAR_SENSING_VALUE_MAXLENGTH);
    printf("'h': ceiling height (%s)\r\n", value);
    mtb_radar_sensing_get_parameter(&sensing_context,
                                    "radar_counter_entrance_width",
                                    value,
                                    IFX_RADAR_SENSING_VALUE_MAXLENGTH);
    printf("'w': entrance width (%s)\r\n", value);
    mtb_radar_sensing_get_parameter(&sensing_context,
                                    "radar_counter_sensitivity",
                                    value,
                                    IFX_RADAR_SENSING_VALUE_MAXLENGTH);
    printf("'s': sensitivity (%s)\r\n", value);
    mtb_radar_sensing_get_parameter(&sensing_context,
                                    "radar_counter_traffic_light_zone",
                                    value,
                                    IFX_RADAR_SENSING_VALUE_MAXLENGTH);
    printf("'t': traffic light zone (%s)\r\n", value);
    mtb_radar_sensing_get_parameter(&sensing_context,
                                    "radar_counter_reverse",
                                    value,
                                    IFX_RADAR_SENSING_VALUE_MAXLENGTH);
    printf("'r': reverse (%s)\n", value);
    mtb_radar_sensing_get_parameter(&sensing_context,
                                    "radar_counter_min_person_height",
                                    value,
                                    IFX_RADAR_SENSING_VALUE_MAXLENGTH);
    printf("'m': min person height (%s)\n", value);
    printf("\n");
    radar_counter_task_set_mute(false);
}

/*******************************************************************************
 * Function Name: terminal_ui_info
 ********************************************************************************
 * Summary:
 *   This function prints character using which a user can see all available
 *   parameters configurable for entrance counter.
 *
 * Parameters:
 *   none
 *
 * Return:
 *   none
 *******************************************************************************/
static void terminal_ui_info(void)
{
    printf("Press '?' to list all radar counter settings\r\n");
}

/*******************************************************************************
 * Function Name: terminal_ui_readline
 ********************************************************************************
 * Summary:
 *   This function reads values entered by a user and prints it.
 *
 * Parameters:
 *   uart_ptr: UART object
 *   line: value to be read
 *   maxlength: maximum number of characters to be read
 *
 * Return:
 *   none
 *******************************************************************************/
static void terminal_ui_readline(void *obj_ptr, char *line, int maxlength)
{
    radar_counter_task_set_mute(true);

    if (maxlength <= 0)
    {
        return;
    }

    int i = 0;
    uint8_t rx_value = 0;
    while ((rx_value != '\r') && (--maxlength > 0))
    {
        cyhal_uart_getc(&cy_retarget_io_uart_obj, &rx_value, 0);
        cyhal_uart_putc(&cy_retarget_io_uart_obj, rx_value);
        if (isspace(rx_value))
        {
            continue;
        }
        line[i++] = rx_value;
    }
    cyhal_uart_putc(&cy_retarget_io_uart_obj, '\n');
    line[i] = '\0';

    radar_counter_task_set_mute(false);
}

/*******************************************************************************
 * Function Name: terminal_ui_getselection
 ********************************************************************************
 * Summary:
 *   This function prints choices for certain entrance counter parameters
 *   and obtains user selection for those choices.
 *
 * Parameters:
 *   obj_ptr: UART object
 *   choices: available choices for parameters that is to be printed
 *   num_choices: maximum number of choices
 *
 * Return:
 *   Character associated with user selection
 *******************************************************************************/
static char *terminal_ui_getselection(void *obj_ptr, char *choices[], int num_choices)
{
    radar_counter_task_set_mute(true);

    int i;
    for (i = 0; i < num_choices; i++)
    {
        printf("\t'%c': %s\r\n", '1' + i, choices[i]);
    }

    uint8_t rx_value;
    do
    {
        if (cyhal_uart_getc(&cy_retarget_io_uart_obj, &rx_value, 0) != CY_RSLT_SUCCESS)
        {
            radar_counter_task_set_mute(false);
            return NULL;
        }
    } 
    while (isspace(rx_value));

    i = rx_value - '1';
    if ((i < 0) || (i >= num_choices))
    {
        printf("not updated\r\n");
        radar_counter_task_set_mute(false);
        return NULL;
    }
    printf("selected '%c': %s\r\n", rx_value, choices[i]);

    radar_counter_task_set_mute(false);
    
    return choices[i];
}

/*******************************************************************************
 * Function Name: terminal_ui_print_result
 ********************************************************************************
 * Summary:
 *   This function displays a success/error message once a parameter has been
 *   configured.
 *
 * Parameters:
 *   result: success/error status message
 *
 * Return:
 *   none
 *******************************************************************************/
static void terminal_ui_print_result(cy_rslt_t result)
{
    switch (result)
    {
        case MTB_RADAR_SENSING_SUCCESS:
            printf("OK\r\n");
            break;
        default:
            printf("ERROR\r\n");
    }
}

/*******************************************************************************
 * Function Name: radar_counter_terminal_ui
 ********************************************************************************
 * Summary:
 *   Continuously checks if a key has been pressed to configure entrance
 *   counter parameters. Displays a status message according to the user
 *   input/selection.
 *
 * Parameters:
 *   arg: thread
 *
 * Return:
 *   none
 *******************************************************************************/
void radar_counter_terminal_ui(cy_thread_arg_t arg)
{
    char *selected_option = "";
    char value[IFX_RADAR_SENSING_VALUE_MAXLENGTH];
    uint8_t rx_value = 0;

    terminal_ui_menu();

    /* Check if a key was pressed */
    while (cyhal_uart_getc(&cy_retarget_io_uart_obj, &rx_value, 0) == CY_RSLT_SUCCESS)
    {
        switch ((char)rx_value)
        {
            // menu
            case '?':
                terminal_ui_menu();
                break;
            case 'i':
            {
                printf("Select counter installation:\r\n");
                char *counter_choices[] = {"ceiling", "side"};
                selected_option = terminal_ui_getselection(&cy_retarget_io_uart_obj,
                                                        counter_choices,
                                                        sizeof(counter_choices) / sizeof(char *));
                if (selected_option != NULL)
                {
                    terminal_ui_print_result(mtb_radar_sensing_set_parameter(&sensing_context,
                                                                            "radar_counter_installation",
                                                                            selected_option));
                }
                break;
            }
            case 'o':
            {
                printf("Select counter orientation:\r\n");
                char *orientation_choices[] = {"landscape", "portrait"};
                selected_option = terminal_ui_getselection(&cy_retarget_io_uart_obj,
                                                        orientation_choices,
                                                        sizeof(orientation_choices) / sizeof(char *));
                if (selected_option != NULL)
                {
                    terminal_ui_print_result(mtb_radar_sensing_set_parameter(&sensing_context,
                                                                            "radar_counter_orientation",
                                                                            selected_option));
                }
                break;
            }
            case 'h':
                printf("Enter counter ceiling height [0.0-3.0]m, press enter\r\n");
                terminal_ui_readline(&cy_retarget_io_uart_obj, value, IFX_RADAR_SENSING_VALUE_MAXLENGTH);
                terminal_ui_print_result(
                    mtb_radar_sensing_set_parameter(&sensing_context, "radar_counter_ceiling_height", value));
                break;
            case 'w':
                printf("Enter counter entrance width [0.0-3.0]m, press enter\r\n");
                terminal_ui_readline(&cy_retarget_io_uart_obj, value, IFX_RADAR_SENSING_VALUE_MAXLENGTH);
                terminal_ui_print_result(
                    mtb_radar_sensing_set_parameter(&sensing_context, "radar_counter_entrance_width", value));
                break;
            case 's':
                printf("Set sensitivity: [0.0 - 1.0]\r\n");
                terminal_ui_readline(&cy_retarget_io_uart_obj, value, IFX_RADAR_SENSING_VALUE_MAXLENGTH);
                terminal_ui_print_result(
                    mtb_radar_sensing_set_parameter(&sensing_context, "radar_counter_sensitivity", value));
                break;
            case 't':
                printf("Enter counter traffic light zone [0.0-1.0]m, press enter\r\n");
                terminal_ui_readline(&cy_retarget_io_uart_obj, value, IFX_RADAR_SENSING_VALUE_MAXLENGTH);
                terminal_ui_print_result(
                    mtb_radar_sensing_set_parameter(&sensing_context, "radar_counter_traffic_light_zone", value));
                break;
            case 'r':
            {
                printf("Select counter reverse:\r\n");
                char *reverse_choices[] = {"true", "false"};
                char *selected = terminal_ui_getselection(&cy_retarget_io_uart_obj,
                                                        reverse_choices,
                                                        sizeof(reverse_choices) / sizeof(char *));
                if (selected != NULL)
                {
                    terminal_ui_print_result(
                        mtb_radar_sensing_set_parameter(&sensing_context, "radar_counter_reverse", selected));
                }
                break;
            }
            case 'm':
                printf("Enter counter min person height [0.0-2.0]m, press enter\r\n");
                terminal_ui_readline(&cy_retarget_io_uart_obj, value, IFX_RADAR_SENSING_VALUE_MAXLENGTH);
                terminal_ui_print_result(
                    mtb_radar_sensing_set_parameter(&sensing_context, "radar_counter_min_person_height", value));
                break;
            default:
                terminal_ui_info();
        }
        rx_value = 0;
    }
    printf("Exiting terminal ui\r\n");
    /* Exit current thread (suspend) */
    (void)cy_rtos_exit_thread();
}
