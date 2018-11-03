
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

#endif
