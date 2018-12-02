
#ifndef __CAM_TEST_H
#define __CAM_TEST_H

#include "includes.h"

typedef enum CAM_UNIT_TEST {
    CAM_INIT,
    CAM_SET_ADDRESS,
    CAM_ZOOM_TELE,
    CAM_ZOOM_WIDE,
    CAM_FOCUS_AUTO,
    CAM_FOCUS_MANUAL,
    CAM_FOCUS_FAR,
    CAM_FOCUS_NEAR,
    CAM_DISPLAY_ON,
    CAM_TITLE_ON,
    CAM_TITLE_SET1,
    CAM_TITLE_SET2,
    CAM_TITLE_SET3,
    CAM_DISPLAY_OFF,
    CAM_SET_REGISTER,
    CAM_GET_REGISTER,
    CAM_TEST_DONE,
};

void cam_unit_test(void);
bool cam_unit_is_menu_on(void);
void cam_unit_menu_toggle(void);
void cam_unit_menu_on(void);
void cam_unit_menu_off(void);
void cam_unit_menu_up(void);
void cam_unit_menu_down(void);
void cam_unit_menu_left(void);
void cam_unit_menu_right(void);
void cam_unit_power_switch(void);
void cam_unit_mode_switch(void);
void cam_unit_expcomp_down(void);
void cam_unit_expcomp_up(void);


#endif
