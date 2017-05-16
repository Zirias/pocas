#include <pocas/gui/private/backend.h>

#include "bqt_backend.h"

static PG_Backend backend_qt = {
    .backendApi = {
        .name = 0,
        .control = {
            .setContainer = 0,
            .setBounds = 0,
            .setShown = 0,
	    .setEnabled = 0,
	    .focus = 0,
        },
        .window = {
            .create = Bqt_Window_create,
            .setMenu = 0,
            .close = 0,
            .destroy = 0,
        },
        .menu = {
            .create = 0,
            .addItem = 0,
	    .removeItem = 0,
            .destroy = 0,
        },
        .menuItem = {
            .create = 0,
            .destroy = 0,
        },
        .messageBox = {
            .show = 0,
        },
        .label = {
            .create = 0,
            .setText = 0,
            .destroy = 0,
        },
        .button = {
            .create = 0,
            .setText = 0,
            .destroy = 0,
        },
        .textBox = {
            .create = 0,
            .setText = 0,
            .destroy = 0,
        },
    },
    .privateApi = 0,
};

SOEXPORT PG_Backend *PG_qtBackend = &backend_qt;

