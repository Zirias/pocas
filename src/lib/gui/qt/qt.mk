pocasgui_qt_INCLUDES= -I$(QTINCLUDES) $(addprefix -I$(QTINCLUDES)/,QtCore QtGui) -I$(pocasgui_qt_PPSRCDIR)
pocasgui_qt_MODULES:= bqt_backend
pocasgui_qt_CXXMODULES:= bqt_eventloop bqt_window
pocasgui_qt_PREPROCMODULES:= bqt_eventloop bqt_window
pocasgui_qt_DEFINES:= -DBUILDING_POCAS_GUI_QT
pocasgui_qt_DEPS:= pocascore
pocasgui_qt_LIBS:= pocascore Qt5Core Qt5Gui
pocasgui_qt_CFLAGS_STATIC:= -DSTATIC_POCAS_CORE -DSTATIC_POCAS_GUI
pocasgui_qt_LDFLAGS:= -L$(QTLIBS)
pocasgui_qt_posix_LDFLAGS:= -pthread
pocasgui_qt_win32_STATICLIBS:= pthread
pocasgui_qt_PREPROC:= MOC
pocasgui_qt_PREPROCFLAGS= -p.
pocasgui_qt_V_MAJ:= 0
pocasgui_qt_V_MIN:= 0
pocasgui_qt_V_REV:= 1
$(call librules, pocasgui_qt)
