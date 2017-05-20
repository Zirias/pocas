pocasgui_MODULES:= backend bounds button command control container label \
                   boxlayout menu messagebox textbox window
pocasgui_DEFINES:= -DBUILDING_POCAS_GUI
pocasgui_DEPS:= pocascore
pocasgui_LIBS:= pocascore
pocasgui_CFLAGS_STATIC:= -DSTATIC_POCAS_CORE
pocasgui_posix_LDFLAGS:= -pthread
pocasgui_win32_STATICLIBS:= pthread
pocasgui_V_MAJ:= 0
pocasgui_V_MIN:= 0
pocasgui_V_REV:= 1

ifeq ($(PLATFORM),win32)
pocasgui_LIBS_STATIC:= pthread
endif

AVAILABLE_GUI_BACKENDS:= qt
ifeq ($(PLATFORM),win32)
AVAILABLE_GUI_BACKENDS+= winapi
endif

ifeq ($(WITH_DEFAULT_GUI_BACKEND),)
undefine WITH_DEFAULT_GUI_BACKEND
else
ifeq ($(filter $(WITH_DEFAULT_GUI_BACKEND),$(AVAILABLE_GUI_BACKENDS)),)
$(error WITH_DEFAULT_GUI_BACKEND=$(WITH_DEFAULT_GUI_BACKEND) unknown, \
available backends are: $(AVAILABLE_GUI_BACKENDS).)
endif
WITH_GUI_$(call toupper,$(WITH_DEFAULT_GUI_BACKEND)):= 1
endif

ifeq ($(WITH_GUI_WINAPI),1)
ifneq ($(PLATFORM),win32)
$(error The winapi gui backend is only available for the win32 platform.)
endif
WITH_DEFAULT_GUI_BACKEND ?= winapi
$(call zinc, winapi/winapi.mk)
else
ifeq ($(WITH_DEFAULT_GUI_BACKEND),winapi)
$(warning Ignoring WITH_DEFAULT_GUI_BACKEND=winapi because WITH_GUI_WINAPI is not set to 1.)
undefine WITH_DEFAULT_GUI_BACKEND
endif
endif

ifeq ($(WITH_GUI_QT),1)
WITH_DEFAULT_GUI_BACKEND ?= qt
$(call zinc, qt/qt.mk)
else
ifeq ($(WITH_DEFAULT_GUI_BACKEND),qt)
$(warning Ignoring WITH_DEFAULT_GUI_BACKEND=qt because WITH_GUI_QT is not set to 1.)
undefine WITH_DEFAULT_GUI_BACKEND
endif
endif

ifdef WITH_DEFAULT_GUI_BACKEND
pocasgui_DEPS += pocasgui_$(WITH_DEFAULT_GUI_BACKEND)
pocasgui_LIBS += pocasgui_$(WITH_DEFAULT_GUI_BACKEND)
pocasgui_LIBS_STATIC += pocasgui_$(WITH_DEFAULT_GUI_BACKEND) \
	$(pocasgui_$(WITH_DEFAULT_GUI_BACKEND)_LIBS_STATIC)
pocasgui_STATICLIBS += $(pocasgui_$(WITH_DEFAULT_GUI_BACKEND)_STATICLIBS)
pocasgui_DEFINES += -DDEFAULT_GUI_BACKEND=$(WITH_DEFAULT_GUI_BACKEND)
pocasgui_CFLAGS_STATIC += -DSTATIC_POCAS_GUI_$(call toupper,$(WITH_DEFAULT_GUI_BACKEND))
pocasgui_LDFLAGS_STATIC += $(pocasgui_$(WITH_DEFAULT_GUI_BACKEND)_LDFLAGS)
endif

$(call librules, pocasgui)
