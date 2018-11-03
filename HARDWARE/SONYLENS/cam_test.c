
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


static unsigned char camera_id = 1;
static uint16_t cam_unit = CAM_INIT;


void cam_unit_test(void)
{
    VISCA_result_e visca_result;
    uint8 u8value;
    
    //sonylens task main loop
    switch(cam_unit)
    {
        case CAM_INIT:
            visca_init();
            cam_unit = CAM_SET_ADDRESS;
            break;
        case CAM_SET_ADDRESS:
            visca_result = visca_set_address(camera_id);
            if(VISCA_result_ok == visca_result) {
                cam_unit = CAM_TITLE_SET1;
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
                cam_unit++;
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
                cam_unit++;
            } else {
                printf("\r\n visca_result:%d", visca_result);
                Wait10Ms(100);
            }
            break;
        case CAM_TEST_DONE:
            break;
    }
}


