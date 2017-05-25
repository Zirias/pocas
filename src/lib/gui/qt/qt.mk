AVAILABLE_QTVERSIONS:= 4 5
ifeq ($(QTVERSION),)
QTVERSION:=5
else
ifeq ($(filter $(QTVERSION),$(AVAILABLE_QTVERSIONS)),)
$(error QTVERSION=$(QTVERSION) unsupported, supported versions are: \
    $(AVAILABLE_QTVERSIONS))
endif
endif

ifeq ($(QTVERSION),4)
pocasgui_QTMODULES:= Core Gui
pocasgui_QTLIBPREFIX:= Qt
pocasgui_QTINCLUDEPREFIX:= Qt
pocasgui_QTDEFINES:= -DWITH_QT4
endif

ifeq ($(QTVERSION),5)
pocasgui_QTMODULES:= Core Gui Widgets
pocasgui_QTLIBPREFIX:= Qt5
pocasgui_QTINCLUDEPREFIX:= Qt
pocasgui_QTDEFINES:= -DWITH_QT5
endif

pocasgui_qt_INCLUDES= -I$(QTINCLUDES) \
	$(addprefix -I$(QTINCLUDES)/$(pocasgui_QTINCLUDEPREFIX), \
	$(pocasgui_QTMODULES)) -I$(pocasgui_qt_PPSRCDIR)
pocasgui_qt_PREPROCMODULES:= bqt_eventloop bqt_eventfilter bqt_control bqt_window bqt_label bqt_button bqt_menuitem bqt_textbox
pocasgui_qt_CXXMODULES:= $(pocasgui_qt_PREPROCMODULES) bqt_messagebox bqt_menu
pocasgui_qt_MODULES:= bqt_backend
pocasgui_qt_DEFINES:= -DBUILDING_POCAS_GUI_QT $(pocasgui_QTDEFINES)
pocasgui_qt_DEPS:= pocascore
pocasgui_qt_LIBS:= pocascore \
	$(addprefix $(pocasgui_QTLIBPREFIX),$(pocasgui_QTMODULES))
pocasgui_qt_CFLAGS_STATIC:= -DSTATIC_POCAS_CORE
pocasgui_qt_CXXFLAGS_STATIC:= -DSTATIC_POCAS_CORE
pocasgui_qt_LDFLAGS:= -L$(QTLIBS)
pocasgui_qt_posix_LDFLAGS:= -pthread
pocasgui_qt_win32_STATICLIBS:= pthread
pocasgui_qt_PREPROC:= MOC
pocasgui_qt_PREPROCFLAGS= -p.
pocasgui_qt_V_MAJ:= 0
pocasgui_qt_V_MIN:= 0
pocasgui_qt_V_REV:= 1
pocasgui_qt_HEADERS_INSTALL:= qt
pocasgui_qt_HEADERDIR:= include/pocas/gui/backend
pocasgui_qt_HEADERTGTDIR:= $(includedir)/pocas/gui/backend
$(call librules, pocasgui_qt)
