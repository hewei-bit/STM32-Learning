#ifndef GUICONF_H
#define GUICONF_H

#define GUI_NUM_LAYERS            10   // ��ʾ��������

#define GUI_OS                    (1)  // ʹ�ò���ϵͳ
#define GUI_MAXTASK				  (5)  // ���ɵ���EMWIN������
#define GUI_SUPPORT_TOUCH         (1)  // ֧�ִ���

/*********************************************************************
*
*       Default font
*/
#define GUI_DEFAULT_FONT          &GUI_Font6x8

/*********************************************************************
*
*         Configuration of available packages
*/
#define GUI_SUPPORT_MOUSE             (1)    /* Support a mouse */
#define GUI_WINSUPPORT                (1)    /* Use window manager */
#define GUI_SUPPORT_MEMDEV            (1)    /* Memory device package available */
#define GUI_SUPPORT_DEVICES           (1)    /* Enable use of device pointers */

#endif  /* Avoid multiple inclusion */
