#ifndef STYLE_H_INCLUDED
#define STYLE_H_INCLUDED

#include "lvgl.h"


#define VIEW_STYLE_COLOR_RED   ((lv_color_t)LV_COLOR_MAKE(0xFF, 0x0, 0x0))
#define VIEW_STYLE_COLOR_GREEN ((lv_color_t)LV_COLOR_MAKE(0x0, 0xFF, 0x0))


void style_init(void);

#endif
