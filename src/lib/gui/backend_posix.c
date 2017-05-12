#include <pocas/gui/private/backend.h>

static Backend posixBackend = {
    .backendApi = {
        .name = 0,
        .control = {
            .setContainer = 0,
            .setBounds = 0,
            .setShown = 0,
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

SOLOCAL Backend *defaultBackend = &posixBackend;

