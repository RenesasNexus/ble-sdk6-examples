/**
 ****************************************************************************************
 *
 * @file user_empty_peripheral_template.c
 *
 * @brief Empty peripheral template project source code.
 *
 * Copyright (c) 2012-2019 Dialog Semiconductor. All rights reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor.
 *
 * By using this Software you agree that Dialog Semiconductor retains all
 * intellectual property and proprietary rights in and to this Software and any
 * use, reproduction, disclosure or distribution of the Software without express
 * written permission or a license agreement from Dialog Semiconductor is
 * strictly prohibited. This Software is solely for use on or in conjunction
 * with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, THE
 * SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE
 * PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, IN NO EVENT SHALL
 * DIALOG SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THE SOFTWARE.
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APP
 * @{
 ****************************************************************************************
 */
#include "rwip_config.h"             // SW configuration


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "custs1.h"
#include "custs1_task.h"
#include "gpio.h"
#include "user_custs1_def.h"
#include "user_periph_setup.h"
#include "ble_notify_button_wakeup.h"
#include "wkupct_quadec.h"

/*
 * DEFINITIONS
 ****************************************************************************************
 */

#define WAKEUP_PINS                 (WKUPCT_PIN_SELECT(GPIO_SW2_PORT, GPIO_SW2_PIN) | WKUPCT_PIN_SELECT(GPIO_SW3_PORT, GPIO_SW3_PIN))
#define WAKEUP_POLARITIES           (WKUPCT_PIN_POLARITY(GPIO_SW2_PORT, GPIO_SW2_PIN, WKUPCT_PIN_POLARITY_LOW) | WKUPCT_PIN_POLARITY(GPIO_SW3_PORT, GPIO_SW3_PIN, WKUPCT_PIN_POLARITY_LOW))
#define WAKEUP_EVENTS_TO_TRIGGER    1
#define WAKEUP_DEBOUNCE_MS          30
/*
 * FUNCTION DECLARATIONS
 ****************************************************************************************
*/ 

// Wake-up callback function declarations
void move_wkup_cb(void);
void stop_wkup_cb(void);
 
/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
*/

void app_wakeup_press_cb(void)
{
#if !defined (__DA14531__)
    if (GetBits16(SYS_STAT_REG, PER_IS_DOWN))
#endif
    {
        periph_init();
    }

    arch_ble_force_wakeup();	

    // If wakeup is generated by button SW3
    if(!GPIO_GetPinStatus(GPIO_SW3_PORT, GPIO_SW3_PIN))						
    {
        app_easy_wakeup_set(move_wkup_cb);
    }
    // If wakeup is generated by button SW2
    else if (!GPIO_GetPinStatus(GPIO_SW2_PORT, GPIO_SW2_PIN))
    {
        app_easy_wakeup_set(stop_wkup_cb);
    }

    wkupct_enable_irq(WAKEUP_PINS, WAKEUP_POLARITIES, WAKEUP_EVENTS_TO_TRIGGER,
                      WAKEUP_DEBOUNCE_MS);
        
    app_easy_wakeup();

}

void move_wkup_cb(void)
{
    const char up_cmd[] = {'M', 'o', 'v', 'e', '!'};
    const size_t up_cmd_len = sizeof(up_cmd)/sizeof(up_cmd[0]);
	
    struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
                                                          prf_get_task_from_id(TASK_ID_CUSTS1),
                                                          TASK_APP,
                                                          custs1_val_ntf_ind_req,
                                                          DEF_SVC1_CTRL_POINT_CHAR_LEN);
		
    req->handle = SVC1_IDX_CONTROL_POINT_VAL;
    req->length = up_cmd_len;
    req->notification = true;
    memcpy(req->value, &up_cmd, up_cmd_len);
		
    ke_msg_send(req);
}

void stop_wkup_cb(void)
{
    const char stop_cmd[] = {'S', 't', 'o', 'p', '!'};
    const size_t stop_cmd_len = sizeof(stop_cmd)/sizeof(stop_cmd[0]);
	
    struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
                                                          prf_get_task_from_id(TASK_ID_CUSTS1),
                                                          TASK_APP,
                                                          custs1_val_ntf_ind_req,
                                                          DEF_SVC1_CTRL_POINT_CHAR_LEN);
		
    req->handle = SVC1_IDX_CONTROL_POINT_VAL;
    req->length = stop_cmd_len;
    req->notification = true;
    memcpy(req->value, &stop_cmd, stop_cmd_len);

    ke_msg_send(req);
}

/* User defined behavior for handling client's requests */
void user_catch_rest_hndl(ke_msg_id_t const msgid,
                          void const *param,
                          ke_task_id_t const dest_id,
                          ke_task_id_t const src_id)
{
	switch(msgid)
	{
        case CUSTS1_VAL_WRITE_IND:
        {
            const char hidden_msg_req[] = {'M', 'a', 'r', 'c', 'o', '?'};
            const char hidden_msg_resp[] = {'P', 'o', 'l', 'o', '!'};
            
            struct custs1_val_write_ind const *msg_param = (struct custs1_val_write_ind const *)(param);

            switch (msg_param->handle)
            {
                case SVC1_IDX_CONTROL_POINT_VAL:
                    if(!memcmp(&hidden_msg_req[0], &msg_param->value[0], msg_param->length)) {
                        size_t hidden_msg_resp_len = sizeof(hidden_msg_resp)/sizeof(hidden_msg_resp[0]);

                        struct custs1_val_ntf_ind_req *req = KE_MSG_ALLOC_DYN(CUSTS1_VAL_NTF_REQ,
                                                                              prf_get_task_from_id(TASK_ID_CUSTS1),
                                                                              TASK_APP,
                                                                              custs1_val_ntf_ind_req,
                                                                              DEF_SVC1_CTRL_POINT_CHAR_LEN);
                        // Send response 
                        req->handle = SVC1_IDX_CONTROL_POINT_VAL;
                        req->length = hidden_msg_resp_len;
                        req->notification = true;
                        memcpy(req->value, &hidden_msg_resp[0], hidden_msg_resp_len);
                        ke_msg_send(req);
                    }
                    break;

                default:
                    break;
                }
			}
			break;
			
			default:
                break;
	}
}

void user_on_connection(uint8_t connection_idx, struct gapc_connection_req_ind const *param)
{
    wkupct_enable_irq(WAKEUP_PINS, WAKEUP_POLARITIES, WAKEUP_EVENTS_TO_TRIGGER,
                      WAKEUP_DEBOUNCE_MS);

    wkupct_register_callback(app_wakeup_press_cb);	// sets this function as wake-up interrupt callback	

    default_app_on_connection(connection_idx, param);
}

void user_on_disconnect( struct gapc_disconnect_ind const *param )
{
    default_app_on_disconnect(param);
}


/// @} APP
