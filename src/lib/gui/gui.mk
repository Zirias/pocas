pocasgui_MODULES:= backend bounds button command control container lbox messagebox window menu label
pocasgui_PLATFORMMODULES := backend
pocasgui_DEFINES:= -DBUILDING_POCAS_GUI
pocasgui_DEPS:= pocascore
pocasgui_LIBS:= pocascore
pocasgui_CFLAGS_STATIC:= -DSTATIC_POCAS_CORE
pocasgui_posix_LDFLAGS:= -pthread
pocasgui_win32_LIBS:= gdi32 comctl32
pocasgui_win32_STATICLIBS:= pthread
pocasgui_V_MAJ:= 0
pocasgui_V_MIN:= 0
pocasgui_V_REV:= 1
$(call librules, pocasgui)
