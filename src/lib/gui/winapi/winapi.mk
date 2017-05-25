pocasgui_winapi_MODULES:= backend
pocasgui_winapi_DEFINES:= -DBUILDING_POCAS_GUI_WINAPI
pocasgui_winapi_DEPS:= pocascore
pocasgui_winapi_CFLAGS_STATIC:= -DSTATIC_POCAS_CORE
pocasgui_winapi_STATICLIBS:= pthread
pocasgui_winapi_LIBS:= gdi32 comctl32 pocascore
pocasgui_winapi_LIBS_STATIC:= $(pocasgui_winapi_STATICLIBS) $(pocasgui_winapi_LIBS)
pocasgui_winapi_V_MAJ:= 0
pocasgui_winapi_V_MIN:= 0
pocasgui_winapi_V_REV:= 1
pocasgui_winapi_HEADERS_INSTALL:= winapi
pocasgui_winapi_HEADERDIR:= include/pocas/gui/backend
pocasgui_winapi_HEADERTGTDIR:= $(includedir)/pocas/gui/backend
$(call librules, pocasgui_winapi)
