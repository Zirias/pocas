#include <pocas/gui/private/backend.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
static Backend backend = {
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
            .create = 0,
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
#pragma GCC diagnostic pop

SOLOCAL Backend *qtBackend = &backend;

