pocastest_MODULES:= main test runner preproc testcase testclass testresult
pocastest_PLATFORMMODULES:= runner testcase
pocastest_DEFINES:= -DBUILDING_POCAS_TEST
ifeq ($(WITH_DEFAULT_GUI_BACKEND),winapi)
pocastest_MODULES+= gui
pocastest_DEFINES+= -DSTATIC_POCAS_CORE -DSTATIC_POCAS_GUI
pocastest_STATICDEPS:= pocascore pocasgui $(pocasgui_DEPS)
pocastest_STATICLIBS:= pocascore pocasgui $(pocascore_LIBS_STATIC)\
	$(pocasgui_LIBS_STATIC)
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
pocastest_posix_LDFLAGS:= -Wl,-E
$(call binrules, pocastest)
