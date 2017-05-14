pocasgui_qt_CXXMODULES:= backend
pocasgui_qt_DEFINES:= -DBUILDING_POCAS_GUI_QT
pocasgui_qt_DEPS:= pocasgui pocascore
pocasgui_qt_LIBS:= pocasgui pocascore
pocasgui_qt_CFLAGS_STATIC:= -DSTATIC_POCAS_CORE -DSTATIC_POCAS_GUI
pocasgui_qt_posix_LDFLAGS:= -pthread
pocasgui_qt_win32_STATICLIBS:= pthread
pocasgui_qt_V_MAJ:= 0
pocasgui_qt_V_MIN:= 0
pocasgui_qt_V_REV:= 1
$(call librules, pocasgui_qt)
