pocastest_MODULES:= main gui test runner preproc testcase testclass testresult
pocastest_PLATFORMMODULES:= runner testcase
pocastest_DEFINES:= -DBUILDING_POCAS_TEST
ifeq ($(WITH_DEFAULT_GUI_BACKEND),winapi)
pocastest_DEFINES+= -DSTATIC_POCAS_CORE -DSTATIC_POCAS_GUI
pocastest_STATICDEPS:= pocasgui pocascore $(pocasgui_DEPS) $(pocasgui_STATICDEPS)
pocastest_STATICLIBS:= pocasgui pocascore $(pocasgui_LIBS) $(pocasgui_STATICLIBS)
else
ifneq ($(WITH_DEFAULT_GUI_BACKEND),)
pocastest_DEPS:= pocascore pocasgui pocasgui_$(WITH_DEFAULT_GUI_BACKEND)
pocastest_LIBS:= pocascore pocasgui pocasgui_$(WITH_DEFAULT_GUI_BACKEND)
else
pocastest_DEFINES+= -DSTATIC_POCAS_CORE
pocastest_STATICDEPS:= pocascore
pocastest_STATICLIBS:= pocascore
endif
endif
pocastest_posix_LDFLAGS:= -Wl,-E -pthread
pocastest_win32_LIBS:= gdi32 comctl32
pocastest_win32_STATICLIBS:= pthread
pocastest_posix_LIBS:= dl
$(call binrules, pocastest)
