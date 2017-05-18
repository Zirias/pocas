#include "../c11threads.h"

#include <pocas/gui/private/backend.h>

#include "bqt_backend.h"

struct bdata
{
    PC_EventProcessor originalProcessor;
};

static thread_local struct bdata bdata = {
    .originalProcessor = 0,
};

SOLOCAL_CDECL void Bqt_Backend_init(void)
{
    if (bdata.originalProcessor) return;

    bdata.originalProcessor = Bqt_EventLoop_install();
}

static PG_Backend backend_qt = {
    .backendApi = {
        .name = 0,
        .control = {
            .setContainer = Bqt_Control_setContainer,
            .setBounds = Bqt_Control_setBounds,
            .setShown = Bqt_Control_setShown,
            .setEnabled = Bqt_Control_setEnabled,
            .focus = Bqt_Control_focus,
        },
        .window = {
            .create = Bqt_Window_create,
            .setMenu = Bqt_Window_setMenu,
            .close = Bqt_Window_close,
            .showModal = Bqt_Window_showModal,
            .hideModal = Bqt_Window_hideModal,
            .destroy = Bqt_Window_destroy,
        },
        .menu = {
            .create = Bqt_Menu_create,
            .addItem = Bqt_Menu_addItem,
            .removeItem = Bqt_Menu_removeItem,
            .destroy = Bqt_Menu_destroy,
        },
        .menuItem = {
            .create = Bqt_MenuItem_create,
            .destroy = Bqt_MenuItem_destroy,
        },
        .messageBox = {
            .show = Bqt_MessageBox_show,
        },
        .label = {
            .create = Bqt_Label_create,
            .setText = Bqt_Label_setText,
            .destroy = Bqt_Label_destroy
        },
        .button = {
            .create = Bqt_Button_create,
            .setText = Bqt_Button_setText,
            .destroy = Bqt_Button_destroy,
        },
        .textBox = {
            .create = Bqt_TextBox_create,
            .setText = Bqt_TextBox_setText,
            .destroy = Bqt_TextBox_destroy,
        },
    },
    .privateApi = 0,
};

SOEXPORT PG_Backend *PG_qtBackend = &backend_qt;
