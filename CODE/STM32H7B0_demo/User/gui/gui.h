/*
 * @Author: SUN  BI4NEG@gmail.com
 * @Date: 2023-11-19 14:05:47
 * @LastEditTime: 2023-11-21 23:53:56
 * @Description: 请填写简介
 */
#ifndef __GUI_H
#define __GUI_H
#include "gui_dev.h"

//窗口源节点
#define ORIGINAL_PARENT  (0)

//创建待执行事件
#define CREATE_GUI_EVENT(a)  (a++)

//设置gui专用标志位
#define GUI_HIDE         (0x0001)
#define GUI_DRAWED       (0x0002)
#define GUI_REHIDE       (0x0004)
#define GUI_OVERM        (0x0008)

//ui绘制标志位
#define GUI_BTN_BOLD     (0x0010)

#define GUI_CH_STA     (0x0100)
#define GUI_CO_STA     (0x0200)
#define GUI_RA_STA     (0x0400)

//位运算定义下
#define CHECK_HIDE(a)    ((a&GUI_HIDE)?1:0)
#define SET_HIDE(a)      ((a |= GUI_HIDE))
#define CLEAR_HIDE(a)    ((a &=~ (GUI_HIDE|GUI_DRAWED)))

#define CHECK_DRAWED(a)  ((a&GUI_DRAWED)?1:0)
#define SET_DRAWED(a)    ((a |= GUI_DRAWED))
#define CLEAR_DRAWED(a)  ((a &=~ GUI_DRAWED))

#define CHECK_REHIDE(a)  ((a&GUI_REHIDE)?1:0)
#define SET_REHIDE(a)    ((a |= GUI_REHIDE))
#define CLEAR_REHIDE(a)  ((a &=~ GUI_REHIDE))

#define CHECK_OVERM(a)   ((a&GUI_OVERM)?1:0)
#define SET_OVERM(a)     ((a |= GUI_OVERM))
#define CLEAR_OVERM(a)   ((a &=~ GUI_OVERM))

typedef struct win window_t;

//ui信息结构体定义
typedef struct gui_info
{
    void * pri_data;

    uint16_t x;
    uint16_t y;
    uint16_t x_size;
    uint16_t y_size;
    uint16_t wflags;

    uint16_t color;

    uint16_t mark_flag;
    uint16_t mx;
    uint16_t my;
    uint16_t mxstop;
    uint16_t mystop;

    uint16_t abx;
    uint16_t aby;

    uint32_t layer;
    
}gui_info_t;

//组件结构体定义
typedef struct widget
{
    struct widget * peer_linker;
    gui_info_t msg;
    window_t * parent;
	gui_dev_t * dev;
    void (*draw)(struct widget *);
    void *msg_response;
}widget_t;

//窗口结构体定义
typedef struct win
{
    struct win * win_child;
    widget_t * wid_child;
    gui_info_t msg;
    gui_dev_t * dev;
    void (*draw)(window_t *);
    void * msg_response;
}window_t;


int gui_show_win_noload(window_t *win);
int gui_win_creater(window_t *win);
int gui_wid_creater(widget_t *wid);
int gui_clear_all(void);
int gui_find_connect(window_t *win, uint16_t x, uint16_t y);
int gui_clear_widget(window_t *win);
int gui_hide_win(window_t *win);
int gui_show_win(window_t *win);
int gui_hide_widget(widget_t *wid);
int gui_show_widget(widget_t *wid);
int gui_set_wid_text(widget_t * wid,char * data);
int gui_move_widget(widget_t * wid,uint16_t px,uint16_t py);
void gui_create_event(void);




#endif

