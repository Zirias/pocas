pocasgui_MODULES:= backend bounds button command control container label \
                   lbox menu messagebox textbox window
pocasgui_DEFINES:= -DBUILDING_POCAS_GUI
pocasgui_DEPS:= pocascore
pocasgui_LIBS:= pocascore
pocasgui_CFLAGS_STATIC:= -DSTATIC_POCAS_CORE
pocasgui_posix_LDFLAGS:= -pthread
pocasgui_win32_STATICLIBS:= pthread
pocasgui_V_MAJ:= 0
pocasgui_V_MIN:= 0
pocasgui_V_REV:= 1

ifeq ($(WITH_DEFAULT_GUI_BACKEND),)
undefine WITH_DEFAULT_GUI_BACKEND
endif

ifeq ($(WITH_GUI_WINAPI),1)
WITH_DEFAULT_GUI_BACKEND ?= winapi
$(call zinc, winapi/winapi.mk)
endif

ifeq ($(WITH_GUI_QT),1)
WITH_DEFAULT_GUI_BACKEND ?= qt
$(call zinc, qt/qt.mk)
endif

ifdef WITH_DEFAULT_GUI_BACKEND
pocasgui_DEPS += $(pocasgui_$(WITH_DEFAULT_GUI_BACKEND)_DEPS)
pocasgui_LIBS += $(pocasgui_$(WITH_DEFAULT_GUI_BACKEND)_LIBS)
pocasgui_STATICDEPS += pocasgui_$(WITH_DEFAULT_GUI_BACKEND)
pocasgui_STATICLIBS += pocasgui_$(WITH_DEFAULT_GUI_BACKEND)
pocasgui_DEFINES += -DDEFAULT_GUI_BACKEND=$(WITH_DEFAULT_GUI_BACKEND)
endif

$(call librules, pocasgui)
