
/*----------------------------------------------------------------------------*/
/* sonylens.c                                                                   */
/* 描述:sony 变倍机型通讯程序                                                          */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* 2017/10/17 yihuagang建立 V0.1                                                  */
/* 2017/10/17   */
/* 2017/10/17 yihuagang  v0.3                                                      */
/*            */
/*  */
/*                     */
/*                               */
/*   */
/*                      */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/


/* include */
#include "cam_test.h"
#include "visca.h"
#include "includes.h"
#include "pelco_d.h"


void cam_unit_key_handle(void);
void cam_unit_menu_display_handle(void);
void cam_unit_sync_data(void);

static unsigned char camera_id = 1;
static uint16_t cam_unit = CAM_INIT;
long cam_test_task_t_sec = 0;  ////单位秒

static unsigned char cam_unit_id = 1;
static bool is_menu_on = false;
static long menu_active_time = 0;
static bool is_power_on = true;
static bool is_auto_mode = true;
static uint8 u8Format= 0;  // 0x06:30P   0x08:25P   0x14:50P   0x13:60P

void cam_unit_test(void)
{
    VISCA_result_e visca_result;
    uint8 u8value;

    if( GetSysTick_Sec() > (cam_test_task_t_sec + 1) )
    {
        cam_test_task_t_sec = GetSysTick_Sec();
        printf("\r\n\r\nTask_t_sec:%ld state:%d menu_active_time:%ld is_menu_on:%d\r\n", 
            cam_test_task_t_sec, cam_unit, menu_active_time, (int)is_menu_on);
        //key_print();
    }

    // key scanning
    key_scan_process();
    
    //sonylens task main loop
    switch(cam_unit)
    {
        case CAM_INIT:
            visca_init();
            pelco_d_init();
            cam_unit = CAM_SET_ADDRESS;
            break;
        case CAM_SET_ADDRESS:
            visca_result = visca_set_address(camera_id);
            if(VISCA_result_ok == visca_result) {
                cam_unit = CAM_GET_REGISTER;
            } else if(VISCA_result_network_change == visca_result) {
                cam_unit = CAM_SET_ADDRESS;
            } else {
                printf("\r\n visca_result:%d", visca_result);
                Wait10Ms(100);
            }
            break;
        case CAM_ZOOM_TELE:
            visca_result = visca_set_zoom_tele(camera_id);
            if(VISCA_result_ok == visca_result) {
                Wait10Ms(500);
                cam_unit++;
            } else {
                printf("\r\n visca_result:%d", visca_result);
                Wait10Ms(100);
            }
            break;
        case CAM_ZOOM_WIDE:
            visca_result = visca_set_zoom_wide(camera_id);
            if(VISCA_result_ok == visca_result) {
                Wait10Ms(500);
                cam_unit = CAM_TEST_DONE;
            } else {
                printf("\r\n visca_result:%d", visca_result);
                Wait10Ms(100);
            }
            break;
        case CAM_FOCUS_AUTO:
            visca_result = visca_set_focus_mode(camera_id,2);
            if(VISCA_result_ok == visca_result) {
                cam_unit++;
            } else {
                printf("\r\n visca_result:%d", visca_result);
                Wait10Ms(100);
            }
            break;
        case CAM_FOCUS_MANUAL:
            visca_result = visca_set_focus_mode(camera_id,3);
            if(VISCA_result_ok == visca_result) {
                cam_unit++;
            } else {
                printf("\r\n visca_result:%d", visca_result);
                Wait10Ms(100);
            }
            break;
        case CAM_FOCUS_FAR:
            visca_result = visca_set_focus_far(camera_id);
            if(VISCA_result_ok == visca_result) {
                Wait10Ms(500);
                cam_unit++;
            } else {
                printf("\r\n visca_result:%d", visca_result);
                Wait10Ms(100);
            }
            break;
        case CAM_FOCUS_NEAR:
            visca_result = visca_set_focus_near(camera_id);
            if(VISCA_result_ok == visca_result) {
                Wait10Ms(500);
                cam_unit++;
            } else {
                printf("\r\n visca_result:%d", visca_result);
                Wait10Ms(100);
            }
            break;
        case CAM_DISPLAY_ON:
            visca_result = visca_set_display(camera_id, 0x2);
            if(VISCA_result_ok == visca_result) {
                cam_unit = CAM_TITLE_ON;
            } else {
                printf("\r\n visca_result:%d", visca_result);
                Wait10Ms(100);
            }
            break;
        case CAM_TITLE_ON:
            visca_result = visca_set_title_display(camera_id, 1, 0x2);
            if(VISCA_result_ok == visca_result) {
                cam_unit = CAM_TEST_DONE;
            } else {
                printf("\r\n visca_result:%d", visca_result);
                Wait10Ms(100);
            }
            break;
        case CAM_TITLE_SET1:
            visca_result = visca_set_title_param(camera_id, 1, 0, 0, 0);
            if(VISCA_result_ok == visca_result) {
                cam_unit = CAM_TEST_DONE;
            } else {
                printf("\r\n visca_result:%d", visca_result);
                Wait10Ms(100);
            }
            break;
        case CAM_TITLE_SET2:
            break;
        case CAM_TITLE_SET3:
            break;
        case CAM_DISPLAY_OFF:
            visca_result = visca_set_display(camera_id, 0x3);
            if(VISCA_result_ok == visca_result) {
                Wait10Ms(500);
                cam_unit = CAM_TEST_DONE;
            } else {
                printf("\r\n visca_result:%d", visca_result);
                Wait10Ms(100);
            }
            break;
        case CAM_SET_REGISTER:
            visca_result = visca_set_register(camera_id, 0x72, 1);
            if(VISCA_result_ok == visca_result) {
                cam_unit++;
            } else {
                printf("\r\n visca_result:%d", visca_result);
                Wait10Ms(100);
            }
            break;
        case CAM_GET_REGISTER:
            visca_result = visca_get_register(camera_id, 0x72, &u8value);
            if(VISCA_result_ok == visca_result) {
                printf("\r\n register value:%x", u8value);
                u8Format = u8value;
                if((0x13 == u8Format) || (0x14 == u8Format)) {
                    gennum_3g_enable(true);
                } else {
                    gennum_3g_enable(false);
                }
                cam_unit = CAM_TEST_DONE;
            } else {
                printf("\r\n visca_result:%d", visca_result);
                Wait10Ms(100);
            }
            break;
        case CAM_TEST_DONE:
            cam_unit_key_handle();
            cam_unit_menu_display_handle();
            pelco_d_process();
            break;
    }
}

void cam_unit_zoom_wide()
{
    VISCA_result_e result;
    
    result = visca_set_zoom_wide_speed(cam_unit_id, 5);
    if(VISCA_result_ok != result) {
        printf("\r\n %s failed.", __FUNCTION__);
        return;
    }
}
void cam_unit_zoom_tele()
{
    VISCA_result_e result;
    
    result = visca_set_zoom_tele_speed(cam_unit_id, 5);
    if(VISCA_result_ok != result) {
        printf("\r\n %s failed.", __FUNCTION__);
        return;
    }
}
void cam_unit_zoom_stop()
{
    VISCA_result_e result;

    result = visca_set_zoom_stop(cam_unit_id);
    if(VISCA_result_ok != result) {
        printf("\r\n %s failed.", __FUNCTION__);
        return;
    }
}


void cam_unit_key_handle(void) {

    if(is_menu_on)
    {
        if(key_condition(KEY_ID_M, 150))
        {
            cam_unit_menu_off();
            return;
        }
        
        if(key_is_pressed(KEY_ID_U)) {
            cam_unit_menu_up();
        } else if(key_is_pressed(KEY_ID_D)) {
            cam_unit_menu_down();
        } else if(key_is_pressed(KEY_ID_L)) {
            cam_unit_menu_left();
        } else if(key_is_pressed(KEY_ID_R)) {
            cam_unit_menu_right();
        }
    }
    else
    {
        if(key_condition(KEY_ID_M, 150))
        {
            cam_unit_menu_on();
            return;
        }

        if(key_is_pressed(KEY_ID_U))
        {
            sonylens_set_focus_far();
            return;
        }
        else if(key_is_unpressed(KEY_ID_U))
        {
            sonylens_set_focus_stop();
            return;
        }

        if(key_is_pressed(KEY_ID_D))
        {
            // focus near limit +
            sonylens_set_focus_near();
            return;
        }
        else if(key_is_unpressed(KEY_ID_D))
        {
            sonylens_set_focus_stop();
            return;
        }

        if(key_is_pressed(KEY_ID_L))
        {
            // zoom-
            cam_unit_zoom_wide();
            return;
        }
        else if(key_is_unpressed(KEY_ID_L))
        {
            cam_unit_zoom_stop();
            return;
        }

        if(key_is_pressed(KEY_ID_R))
        {
            // zoom-
            cam_unit_zoom_tele();
            return;
        }
        else if(key_is_unpressed(KEY_ID_R))
        {
            cam_unit_zoom_stop();
            return;
        }
    }

    if(key_condition(KEY_ID_F1, 150))
    {
        cam_unit_power_switch();
        return;
    }

    if(key_is_pressed(KEY_ID_F7))
    {
        cam_unit_mode_switch();
        return;
    }

    if(key_is_pressed(KEY_ID_F6))
    {
        // bright -
        cam_unit_expcomp_down();
        return;
    }

    if(key_is_pressed(KEY_ID_F5))
    {
        // bright +
        cam_unit_expcomp_up();
        return;
    }
}

void cam_unit_menu_display_handle(void)
{    
    if(is_menu_on && ((GetSysTick_Sec() - menu_active_time) > 20))
    {
        cam_unit_menu_off();
    }
}

void cam_unit_menu_toggle(void) {
    if(is_menu_on) {
        cam_unit_menu_off();
    } else {
        cam_unit_menu_on();
    }
}

bool cam_unit_is_menu_on(void) {
    return is_menu_on;
}

void cam_unit_menu_on(void) {
    VISCA_result_e result;
    if(!is_menu_on) {
        result = visca_menu_center(cam_unit_id);
        if(VISCA_result_ok == result) {
            is_menu_on = !is_menu_on;
            menu_active_time = GetSysTick_Sec();
        }
    }    
}

void cam_unit_menu_off(void) {
    VISCA_result_e result;
    if(is_menu_on) {
        result = visca_menu_center(cam_unit_id);
        if(VISCA_result_ok == result) {
            is_menu_on = !is_menu_on;
        }
        cam_unit_sync_data();
    }
}

void cam_unit_menu_up(void) {
    VISCA_result_e result;
    if(is_menu_on) {
        result = visca_menu_up(cam_unit_id);
        menu_active_time = GetSysTick_Sec();
    }
}
void cam_unit_menu_down(void) {
    VISCA_result_e result;
    if(is_menu_on) {
        result = visca_menu_down(cam_unit_id);
        menu_active_time = GetSysTick_Sec();
    } else {
    
    }
}
void cam_unit_menu_left(void) {
    VISCA_result_e result;
    if(is_menu_on) {
        result = visca_menu_left(cam_unit_id);
        menu_active_time = GetSysTick_Sec();
    }
}
void cam_unit_menu_right(void) {
    VISCA_result_e result;
    if(is_menu_on) {
        result = visca_menu_right(cam_unit_id);
        menu_active_time = GetSysTick_Sec();
    }
}

void cam_unit_power_switch(void) {
    VISCA_result_e result;
    if(is_power_on) {
        // power off
        result = visca_set_power(cam_unit_id, 0x03);
        if(VISCA_result_ok != result) {
            printf("\r\n power off failed. result:%d", result);
            return;
        }
    } else {
        // power on
        result = visca_set_power(cam_unit_id, 0x02);
        if(VISCA_result_ok != result) {
            printf("\r\n power on failed. result:%d", result);
            return;
        }
    }
    is_power_on = !is_power_on;
}

void cam_unit_mode_switch(void) {
    VISCA_result_e result;
    if(is_auto_mode) {
        // to manual mode
        result = visca_set_exposure_ae_mode(cam_unit_id, 0x03);
        if(VISCA_result_ok != result) {
            printf("\r\n exposure mode failed. result:%d", result);
            return;
        }
    } else {
        // to auto mode
        result = visca_set_exposure_ae_mode(cam_unit_id, 0x00);
        if(VISCA_result_ok != result) {
            printf("\r\n exposure mode failed. result:%d", result);
            return;
        }
    }
    is_auto_mode = !is_auto_mode;
}

void cam_unit_expcomp_down(void) {
    VISCA_result_e result;

    if(is_auto_mode) {
        result = visca_set_advance_expcomp(cam_unit_id, 0x2);
        if(VISCA_result_ok != result) {
            printf("\r\n exposure down failed. result:%d", result);
            return;
        }
        
        // expcomp_down
        result = visca_set_expcomp_down(cam_unit_id);
        if(VISCA_result_ok != result) {
            printf("\r\n exposure down failed. result:%d", result);
            return;
        }
    }
}

void cam_unit_expcomp_up(void) {
    VISCA_result_e result;
    if(is_auto_mode) {
        result = visca_set_advance_expcomp(cam_unit_id, 0x2);
        if(VISCA_result_ok != result) {
            printf("\r\n exposure down failed. result:%d", result);
            return;
        }
        
        // expcomp_down
        result = visca_set_expcomp_up(cam_unit_id);
        if(VISCA_result_ok != result) {
            printf("\r\n exposure up failed. result:%d", result);
            return;
        }
    }
}

void cam_unit_sync_data(void) {
    VISCA_result_e result;
    uint8 value = 0;
    result = visca_get_exposure_ae_mode(cam_unit_id, &value);
    if(result != VISCA_result_ok) {
        return;
    }
    if(0 == value) {
        is_auto_mode = true;
    } else {
        is_auto_mode = false;
    }
    printf("\r\n ae mode:%d", value);
}


