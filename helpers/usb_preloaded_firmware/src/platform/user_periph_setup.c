/**
 ****************************************************************************************
 *
 * @file user_periph_setup.c
 *
 * @brief Peripherals setup and initialization.
 *
 * Copyright (C) 2015-2023 Renesas Electronics Corporation and/or its affiliates.
 * All rights reserved. Confidential Information.
 *
 * This software ("Software") is supplied by Renesas Electronics Corporation and/or its
 * affiliates ("Renesas"). Renesas grants you a personal, non-exclusive, non-transferable,
 * revocable, non-sub-licensable right and license to use the Software, solely if used in
 * or together with Renesas products. You may make copies of this Software, provided this
 * copyright notice and disclaimer ("Notice") is included in all such copies. Renesas
 * reserves the right to change or discontinue the Software at any time without notice.
 *
 * THE SOFTWARE IS PROVIDED "AS IS". RENESAS DISCLAIMS ALL WARRANTIES OF ANY KIND,
 * WHETHER EXPRESS, IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. TO THE
 * MAXIMUM EXTENT PERMITTED UNDER LAW, IN NO EVENT SHALL RENESAS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE, EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGES. USE OF THIS SOFTWARE MAY BE SUBJECT TO TERMS AND CONDITIONS CONTAINED IN
 * AN ADDITIONAL AGREEMENT BETWEEN YOU AND RENESAS. IN CASE OF CONFLICT BETWEEN THE TERMS
 * OF THIS NOTICE AND ANY SUCH ADDITIONAL LICENSE AGREEMENT, THE TERMS OF THE AGREEMENT
 * SHALL TAKE PRECEDENCE. BY CONTINUING TO USE THIS SOFTWARE, YOU AGREE TO THE TERMS OF
 * THIS NOTICE.IF YOU DO NOT AGREE TO THESE TERMS, YOU ARE NOT PERMITTED TO USE THIS
 * SOFTWARE.
 *
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "user_periph_setup.h"
#include "datasheet.h"
#include "system_library.h"
#include "rwip_config.h"
#include "gpio.h"
#include "uart.h"
#include "syscntl.h"
#include "fpga_helper.h"
#include "systick.h"
#include "uart_utils.h"
#include "user_config.h"
#include "arch_ram.h"
#include "app.h"                // Application Definition
/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

extern struct bd_addr app_random_addr                                              __SECTION_ZERO("retention_mem_area0"); //@ RETENTION MEMORY
static void print_db_adress(void);
int systick_var = 0;
/**
 ****************************************************************************************
 * @brief Each application reserves its own GPIOs here.
 ****************************************************************************************
 */

#if DEVELOPMENT_DEBUG

void GPIO_reservations(void)
{
/*
    i.e. to reserve P0_1 as Generic Purpose I/O:
    RESERVE_GPIO(DESCRIPTIVE_NAME, GPIO_PORT_0, GPIO_PIN_1, PID_GPIO);
*/

#if defined (CFG_PRINTF_UART2)
    RESERVE_GPIO(UART2_TX, UART2_TX_PORT, UART2_TX_PIN, PID_UART2_TX);
#endif
			#if ! defined (__DA14533__)
    RESERVE_GPIO(LED, GPIO_LED_PORT, GPIO_LED_PIN, PID_GPIO);
	#endif

#if !defined (__DA14586__)
    RESERVE_GPIO(SPI_EN, SPI_EN_PORT, SPI_EN_PIN, PID_SPI_EN);
#endif
}

#endif

void set_pad_functions(void)
{
/*
    i.e. to set P0_1 as Generic purpose Output:
    GPIO_ConfigurePin(GPIO_PORT_0, GPIO_PIN_1, OUTPUT, PID_GPIO, false);
*/

#if defined (__DA14586__)
    // Disallow spontaneous DA14586 SPI Flash wake-up
    GPIO_ConfigurePin(GPIO_PORT_2, GPIO_PIN_3, OUTPUT, PID_GPIO, true);
#else
    // Disallow spontaneous SPI Flash wake-up
    GPIO_ConfigurePin(SPI_EN_PORT, SPI_EN_PIN, OUTPUT, PID_SPI_EN, true);
#endif

#if defined (CFG_PRINTF_UART2)
    // Configure UART2 TX Pad
    GPIO_ConfigurePin(UART2_TX_PORT, UART2_TX_PIN, OUTPUT, PID_UART2_TX, false);
#endif
#if ! defined (__DA14533__)
    GPIO_ConfigurePin(GPIO_LED_PORT, GPIO_LED_PIN, OUTPUT, PID_GPIO, false);
#endif
}

// Configuration struct for UART2
static const uart_cfg_t uart_cfg = {
    .baud_rate = UART2_BAUDRATE,
    .data_bits = UART2_DATABITS,
    .parity = UART2_PARITY,
    .stop_bits = UART2_STOPBITS,
    .auto_flow_control = UART2_AFCE,
    .use_fifo = UART2_FIFO,
    .tx_fifo_tr_lvl = UART2_TX_FIFO_LEVEL,
    .rx_fifo_tr_lvl = UART2_RX_FIFO_LEVEL,
    .intr_priority = 2,
};

static void print_db_adress(void)
{
	    static uint8_t n = 0;
    // when pass  10 * 100ms
    if ( 10 == n )
    {
        n = 0;
//        timeout_expiration--;
					
	printf_string(UART2, "#################################################################################################");
					printf_string(UART2, "\n\r");
					
					#if defined (__DA14531__) && (__DA14535__) && (__DA14533__)
					printf_string(UART2, " This is your Unique Static Random Address For your Device embed in the DA14533 Development KIT : \n\r");
					#elif defined (__DA14531__) && (__DA14535__)
					printf_string(UART2, " This is your Unique Static Random Address For your Device embed in the DA14535 USB KIT : \n\r");
					#else
					printf_string(UART2, " This is your Unique Static Random Address For your Device embed in the DA14531 USB KIT : \n\r");
					#endif
					printf_string(UART2, "#################################################################################################");
					   
					printf_string(UART2, "\n\r");
					printf_string(UART2, "\n\r");
					          
				  printf_byte(UART2, app_random_addr.addr[5]);
					printf_string(UART2, ":");
					printf_byte(UART2, app_random_addr.addr[4]);
					printf_string(UART2, ":");
					printf_byte(UART2, app_random_addr.addr[3]);
					printf_string(UART2, ":");
					printf_byte(UART2, app_random_addr.addr[2]);
					printf_string(UART2, ":");
					printf_byte(UART2, app_random_addr.addr[1]);
					printf_string(UART2, ":");
					printf_byte(UART2, app_random_addr.addr[0]);
					printf_string(UART2, "\n\r");
					printf_string(UART2, "\n\r");
					printf_string(UART2, "\n\r");
     }
		
     n++;
}
static void systick_isr(void)
{	 
    if (systick_var == 0)
    {
			#if ! defined (__DA14533__)
        GPIO_SetActive(GPIO_LED_PORT, GPIO_LED_PIN);
			#endif
        systick_var = 1;
		  print_db_adress();
    }
    else
    {
			#if ! defined (__DA14533__)
        GPIO_SetInactive(GPIO_LED_PORT, GPIO_LED_PIN);
			#endif
        systick_var = 0;
		  print_db_adress();
    }
}

void demo_start(void)
{
    systick_register_callback(systick_isr);
    // Systick will be initialized to use a reference clock frequency of 1 MHz
    systick_start(SYSTICK_PERIOD_US, SYSTICK_EXCEPTION);
}

void periph_init(void)
{
#if defined (__DA14531__)
    // Select FPGA GPIO_MAP 1
    // set debugger SWD to SW_CLK = P0[2], SW_DIO=P0[5]
    //FPGA_HELPER(FPGA_GPIO_MAP_1, SWD_DATA_AT_P0_5);

    // In Boost mode enable the DCDC converter to supply VBAT_HIGH for the used GPIOs
    // Assumption: The connected external peripheral is powered by 3V
    syscntl_dcdc_turn_on_in_boost(SYSCNTL_DCDC_LEVEL_3V0);
#else
    // Power up peripherals' power domain
    SetBits16(PMU_CTRL_REG, PERIPH_SLEEP, 0);
    while (!(GetWord16(SYS_STAT_REG) & PER_IS_UP));
    SetBits16(CLK_16M_REG, XTAL16_BIAS_SH_ENABLE, 1);
#endif

    // ROM patch
    patch_func();

    // Set pad functionality
    set_pad_functions();
	
		    // Enable the pads
    GPIO_set_pad_latch_en(true);
		
		uart_initialize(UART2, &uart_cfg);
				
		GPIO_ConfigurePin(UART2_TX_PORT, UART2_TX_PIN, OUTPUT, PID_UART2_TX, false);
		
		demo_start();
}
