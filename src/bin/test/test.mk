pocastest_MODULES:= main test runner preproc testcase testclass testresult
pocastest_PLATFORMMODULES:= runner testcase
pocastest_DEFINES:= -DBUILDING_POCAS_TEST
ifeq ($(WITH_DEFAULT_GUI_BACKEND),winapi)
pocastest_MODULES+= gui
pocastest_DEFINES+= -DSTATIC_POCAS_CORE -DSTATIC_POCAS_GUI
pocastest_STATICDEPS:= pocasgui pocascore
pocastest_STATICLIBS:= pocasgui pocascore $(pocasgui_LIBS_STATIC)
else
ifneq ($(WITH_DEFAULT_GUI_BACKEND),)
pocastest_MODULES+= gui
pocastest_DEPS:= pocascore pocasgui
pocastest_LIBS:= pocascore pocasgui
else
pocastest_DEFINES+= -DSTATIC_POCAS_CORE -DNO_GUI
pocastest_STATICDEPS:= pocascore
pocastest_STATICLIBS:= pocascore
endif
endif
pocastest_posix_LDFLAGS:= -Wl,-E -pthread
pocastest_win32_STATICLIBS:= pthread
pocastest_posix_LIBS:= dl
$(call binrules, pocastest)
