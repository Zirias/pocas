pocasgui_winapi_MODULES:= backend
pocasgui_winapi_DEFINES:= -DBUILDING_POCAS_GUI_WINAPI
pocasgui_winapi_DEPS:= pocascore
pocasgui_winapi_CFLAGS_STATIC:= -DSTATIC_POCAS_CORE
pocasgui_winapi_posix_LDFLAGS:= -pthread
pocasgui_winapi_STATICLIBS:= pthread
pocasgui_winapi_LIBS:= gdi32 comctl32 pocascore
pocasgui_winapi_V_MAJ:= 0
pocasgui_winapi_V_MIN:= 0
pocasgui_winapi_V_REV:= 1
$(call librules, pocasgui_winapi)
