#include "bsp.h"

// 根窗口
static window_t *origin_win = 0;
// UI事件计数器
static uint32_t GUI_EVENT = 0;

static void gui_thread(void);
static int gui_draw_task(void);

OS_INIT_REGISTER("gui",gui_dev_init,gui_draw_task,5);

OS_TSK_REGISTER(gui_thread,PRIORITY_4,10);

static void gui_thread(void)
{
	int ret = 0;
	if( GUI_EVENT != 0 )
	{
		ret = gui_draw_task();
		if( ret >= 0 )
		{
			GUI_EVENT -= ret;
		}
		if(ret < 0)
		{
			ret = 0;
		}
	}
}

/**
 * @description: ui绘制任务，以窗口和组件构成，各层依次叠加绘制
 * @return {*}
 */
static int gui_draw_task(void)
{
    int ret = 0;

    // 对所有窗口遍历并绘制
    for (window_t *win_base = origin_win; win_base != 0; win_base = win_base->win_child)
    {
        // 检查此窗口 是否已绘制 && 是否隐藏 && 是否有对应绘制函数
        if ((!CHECK_DRAWED(win_base->msg.wflags)) && (!CHECK_HIDE(win_base->msg.wflags)) && (win_base->draw != 0))
        {
            win_base->draw(win_base);
            SET_DRAWED(win_base->msg.wflags);
            win_base->msg.mark_flag = 0;
            ret++;
        }
        else
        {
            // 如果隐藏则直接跳过此窗口绘制
            if (CHECK_HIDE(win_base->msg.wflags))
            {
                continue;
            }
        }
        // 对此窗口下所有部件遍历并绘制
        for (widget_t *wid_base = win_base->wid_child; wid_base != 0; wid_base = wid_base->peer_linker)
        {
            // 检查此部件 是否已绘制 && 是否隐藏 || 是否更新 && 是否有对应绘制函数
            if ((!CHECK_DRAWED(wid_base->msg.wflags)) && (!CHECK_HIDE(wid_base->msg.wflags)) && (wid_base->draw != 0) || (CHECK_REHIDE(wid_base->msg.wflags)))
            {
                wid_base->draw(wid_base);
                SET_DRAWED(wid_base->msg.wflags);
                if (CHECK_REHIDE(wid_base->msg.wflags))
                {
                    CLEAR_REHIDE(wid_base->msg.wflags);
                }
                ret++;
            }
        }
    }
    return ret;
}

/**
 * @description: 立即绘制窗口及包含组件函数
 * @param {window_t *} win 要绘制的窗口
 * @return {*}
 */
int gui_show_win_noload(window_t *win)
{
    window_t *win_base = win;
    // 判断窗口是否有对应绘制函数
    if (win_base->draw != 0)
    {
        win_base->draw(win_base);
        SET_DRAWED(win_base->msg.wflags);
    }
    else
    {
        // return OS_ERR;
    }
    // 遍历此窗口下的组件并绘制
    for (widget_t *wid_base = win_base->wid_child; wid_base != 0; wid_base = wid_base->peer_linker)
    {
        if (wid_base->draw != 0)
        {
            wid_base->draw(wid_base);
            // SET_DRAWED(wid_base->msg.wflags);
        }
        else
        {
            return OS_ERR;
        }
    }
    return OS_OK;
}

/**
 * @description: 创建窗口
 * @param {window_t *} win 要创建的窗口
 * @return {*}
 */
int gui_win_creater(window_t *win)
{
    // 第一个窗口
    if (origin_win == ORIGINAL_PARENT)
    {
        origin_win = win;
        return OS_OK;
    }
    // 后续窗口
    window_t *win_base = 0;
    for (win_base = origin_win; win_base->win_child != 0; win_base = win_base->win_child)
    {
        // 对所有窗户进行遍历，直到 win_base 为最后一个窗口时停止
    }
    win_base->win_child = win;

    return OS_OK;
}

/**
 * @description: 创建组件，将第一个widget设为parent的wchild，后续的widget都通过peer串起来
 * @param {widget_t *} wid
 * @return {*}
 */
int gui_wid_creater(widget_t *wid)
{
    // 无窗口时 错误
    if (wid->parent == ORIGINAL_PARENT)
    {
        return OS_ERR;
    }
    // 此组件为窗口的第一个时，注意此时组件指向窗口，但窗口未指向组件
    if (wid->parent->wid_child == ORIGINAL_PARENT)
    {
        // 定义结构体指针的指针，指向窗口的第一个组件，此处可简化？
        widget_t **widg_base = &wid->parent->wid_child;
        // 将此组件赋值给指针指向内容
        *widg_base = wid;
        return OS_OK;
    }

    widget_t *wid_base = 0;

    for (wid_base = wid->parent->wid_child; wid_base->peer_linker != 0; wid_base = wid_base->peer_linker)
    {
        // 对此窗口的所有组件进行遍历，直到 wid_base 为最后一个组件时停止
    }
    wid_base->peer_linker = wid;

    return OS_OK;
}

/**
 * @description: 清除所有绘制，并重新绘制
 * @return {*}
 */
int gui_clear_all(void)
{
    for (window_t *win_base = origin_win; win_base->win_child != 0; win_base = win_base->win_child)
    {
        CLEAR_DRAWED(win_base->msg.wflags);
        for (widget_t *wid_base = win_base->wid_child; wid_base != 0; wid_base = wid_base->peer_linker)
        {
            CLEAR_DRAWED(wid_base->msg.wflags);
        }
    }
    return OS_OK;
}

/**
 * @description: 匹配某点是否位于此窗口或者其子窗口
 * @param {window_t *} win
 * @param {uint16_t} x
 * @param {uint16_t} y
 * @return {*}
 */
int gui_find_connect(window_t *win, uint16_t x, uint16_t y)
{
    for (window_t *win_base = win->win_child; win_base != 0; win_base = win_base->win_child)
    {
        if (CHECK_HIDE(win_base->msg.wflags))
        {
            continue;
        }

        if ((x >= win_base->msg.x) && (x <= (win_base->msg.x + win_base->msg.x_size)))
        {
            if ((y >= win_base->msg.y) && (y <= (win_base->msg.y + win_base->msg.y_size)))
            {
                return 1;
            }
        }
    }
    return 0;
}

/**
 * @description: 清除窗口的所有部件，并重新绘制
 * @param {window_t *} win 要清除的窗口
 * @return {*}
 */
int gui_clear_widget(window_t *win)
{
    for (widget_t *wid_base = win->wid_child; wid_base != 0; wid_base = wid_base->peer_linker)
    {
        CLEAR_DRAWED(wid_base->msg.wflags);
    }
    return OS_OK;
}

/**
 * @description: 隐藏窗口
 * @param {window_t *} win
 * @return {*}
 */
int gui_hide_win(window_t *win)
{
    if (CHECK_HIDE(win->msg.wflags))
    {
        return OS_OK;
    }
    uint8_t gotxy = 0;
    for (window_t *win_base = win->win_child; win_base != 0; win_base = win_base->win_child)
    {
        if (win == win_base)
        {
            // 已经隐藏
            continue;
        }
        gotxy = 0;
        if (((win->msg.x <= win_base->msg.x) && (win->msg.x + win->msg.x_size >= win_base->msg.x)) || ((win->msg.x >= win_base->msg.x) && (win->msg.x <= win_base->msg.x + win->msg.x_size)))
        {
            gotxy = 1;
        }
        if (((win->msg.y <= win_base->msg.y) && (win->msg.y + win->msg.y_size >= win_base->msg.y)) || ((win->msg.y >= win_base->msg.y) && (win->msg.y <= win_base->msg.y + win->msg.y_size)))
        {
            if (gotxy == 1)
            {
                win_base->msg.mark_flag = 1;
                win_base->msg.mx = win->msg.x;
                win_base->msg.my = win->msg.y;
                win_base->msg.mxstop = win->msg.x_size;
                win_base->msg.mystop = win->msg.y_size;
                CLEAR_DRAWED(win_base->msg.wflags);
                SET_HIDE(win->msg.wflags);
                CREATE_GUI_EVENT(GUI_EVENT);
            }
        }
    }
    return OS_OK;
}

/**
 * @description: 重新显示窗口及其组件
 * @param {window_t *} win
 * @return {*}
 */
int gui_show_win(window_t *win)
{
    if (!CHECK_HIDE(win->msg.wflags))
    {
        // 已经显示
        return OS_OK;
    }
    CLEAR_HIDE(win->msg.wflags);
    CLEAR_DRAWED(win->msg.wflags);
    gui_clear_widget(win);
    CREATE_GUI_EVENT(GUI_EVENT);
    return OS_OK;
}

/**
 * @description: 隐藏部件
 * @param {widget_t *} wid
 * @return {*}
 */
int gui_hide_widget(widget_t *wid)
{
    if (!CHECK_HIDE(wid->parent->msg.wflags))
    {
        if (!CHECK_HIDE(wid->msg.wflags))
        {
            SET_REHIDE(wid->msg.wflags);
            SET_HIDE(wid->msg.wflags);
            CLEAR_DRAWED(wid->msg.wflags);
            CREATE_GUI_EVENT(GUI_EVENT);
            return OS_OK;
        }
    }
    else
    {
        if (!CHECK_HIDE(wid->msg.wflags))
        {
            SET_HIDE(wid->msg.wflags);
        }
    }
    return OS_ERR;
}

/**
 * @description: 显示部件
 * @param {widget_t} *wid
 * @return {*}
 */
int gui_show_widget(widget_t *wid)
{
    if (!CHECK_HIDE(wid->parent->msg.wflags) && CHECK_HIDE(wid->msg.wflags))
    {
        CLEAR_HIDE(wid->msg.wflags);
        CREATE_GUI_EVENT(GUI_EVENT);
        return OS_OK;
    }
    return OS_ERR;
}

/**
 * @description: 设置窗口标题
 * @param {widget_t *} wid
 * @param {char *} data
 * @return {*}
 */
int gui_set_wid_text(widget_t * wid,char * data)
{
	if( !CHECK_OVERM(wid->msg.wflags))
	{
		wid->msg.pri_data = data;
		if( !CHECK_HIDE(wid->msg.wflags) && !CHECK_HIDE(wid->parent->msg.wflags) )
		{
			SET_OVERM(wid->msg.wflags);
			CLEAR_DRAWED(wid->msg.wflags);
			CREATE_GUI_EVENT(GUI_EVENT);
			return OS_OK;
		}
	}
	return OS_ERR;
}

/**
 * @description: 移动组件
 * @param {widget_t *} wid
 * @param {uint16_t} px
 * @param {uint16_t} py
 * @return {*}
 */
int gui_move_widget(widget_t * wid,uint16_t px,uint16_t py)
{
    wid->msg.mx = px;
    wid->msg.my = py;
    if(!CHECK_HIDE(wid->parent->msg.wflags) && !CHECK_HIDE(wid->msg.wflags))
    {
        wid->msg.mark_flag = 2;
        CLEAR_DRAWED(wid->msg.wflags);
        CREATE_GUI_EVENT(GUI_EVENT);
        return OS_OK;
    }
    return OS_OK;
}

/**
 * @description: 产生ui更新事件
 * @return {*}
 */
void gui_create_event(void)
{
    CREATE_GUI_EVENT(GUI_EVENT);
}
