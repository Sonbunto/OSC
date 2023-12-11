#include "bsp.h"

// ������
static window_t *origin_win = 0;
// UI�¼�������
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
 * @description: ui���������Դ��ں�������ɣ��������ε��ӻ���
 * @return {*}
 */
static int gui_draw_task(void)
{
    int ret = 0;

    // �����д��ڱ���������
    for (window_t *win_base = origin_win; win_base != 0; win_base = win_base->win_child)
    {
        // ���˴��� �Ƿ��ѻ��� && �Ƿ����� && �Ƿ��ж�Ӧ���ƺ���
        if ((!CHECK_DRAWED(win_base->msg.wflags)) && (!CHECK_HIDE(win_base->msg.wflags)) && (win_base->draw != 0))
        {
            win_base->draw(win_base);
            SET_DRAWED(win_base->msg.wflags);
            win_base->msg.mark_flag = 0;
            ret++;
        }
        else
        {
            // ���������ֱ�������˴��ڻ���
            if (CHECK_HIDE(win_base->msg.wflags))
            {
                continue;
            }
        }
        // �Դ˴��������в�������������
        for (widget_t *wid_base = win_base->wid_child; wid_base != 0; wid_base = wid_base->peer_linker)
        {
            // ���˲��� �Ƿ��ѻ��� && �Ƿ����� || �Ƿ���� && �Ƿ��ж�Ӧ���ƺ���
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
 * @description: �������ƴ��ڼ������������
 * @param {window_t *} win Ҫ���ƵĴ���
 * @return {*}
 */
int gui_show_win_noload(window_t *win)
{
    window_t *win_base = win;
    // �жϴ����Ƿ��ж�Ӧ���ƺ���
    if (win_base->draw != 0)
    {
        win_base->draw(win_base);
        SET_DRAWED(win_base->msg.wflags);
    }
    else
    {
        // return OS_ERR;
    }
    // �����˴����µ����������
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
 * @description: ��������
 * @param {window_t *} win Ҫ�����Ĵ���
 * @return {*}
 */
int gui_win_creater(window_t *win)
{
    // ��һ������
    if (origin_win == ORIGINAL_PARENT)
    {
        origin_win = win;
        return OS_OK;
    }
    // ��������
    window_t *win_base = 0;
    for (win_base = origin_win; win_base->win_child != 0; win_base = win_base->win_child)
    {
        // �����д������б�����ֱ�� win_base Ϊ���һ������ʱֹͣ
    }
    win_base->win_child = win;

    return OS_OK;
}

/**
 * @description: �������������һ��widget��Ϊparent��wchild��������widget��ͨ��peer������
 * @param {widget_t *} wid
 * @return {*}
 */
int gui_wid_creater(widget_t *wid)
{
    // �޴���ʱ ����
    if (wid->parent == ORIGINAL_PARENT)
    {
        return OS_ERR;
    }
    // �����Ϊ���ڵĵ�һ��ʱ��ע���ʱ���ָ�򴰿ڣ�������δָ�����
    if (wid->parent->wid_child == ORIGINAL_PARENT)
    {
        // ����ṹ��ָ���ָ�룬ָ�򴰿ڵĵ�һ��������˴��ɼ򻯣�
        widget_t **widg_base = &wid->parent->wid_child;
        // ���������ֵ��ָ��ָ������
        *widg_base = wid;
        return OS_OK;
    }

    widget_t *wid_base = 0;

    for (wid_base = wid->parent->wid_child; wid_base->peer_linker != 0; wid_base = wid_base->peer_linker)
    {
        // �Դ˴��ڵ�����������б�����ֱ�� wid_base Ϊ���һ�����ʱֹͣ
    }
    wid_base->peer_linker = wid;

    return OS_OK;
}

/**
 * @description: ������л��ƣ������»���
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
 * @description: ƥ��ĳ���Ƿ�λ�ڴ˴��ڻ������Ӵ���
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
 * @description: ������ڵ����в����������»���
 * @param {window_t *} win Ҫ����Ĵ���
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
 * @description: ���ش���
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
            // �Ѿ�����
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
 * @description: ������ʾ���ڼ������
 * @param {window_t *} win
 * @return {*}
 */
int gui_show_win(window_t *win)
{
    if (!CHECK_HIDE(win->msg.wflags))
    {
        // �Ѿ���ʾ
        return OS_OK;
    }
    CLEAR_HIDE(win->msg.wflags);
    CLEAR_DRAWED(win->msg.wflags);
    gui_clear_widget(win);
    CREATE_GUI_EVENT(GUI_EVENT);
    return OS_OK;
}

/**
 * @description: ���ز���
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
 * @description: ��ʾ����
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
 * @description: ���ô��ڱ���
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
 * @description: �ƶ����
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
 * @description: ����ui�����¼�
 * @return {*}
 */
void gui_create_event(void)
{
    CREATE_GUI_EVENT(GUI_EVENT);
}
