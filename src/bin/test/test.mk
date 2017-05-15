pocastest_MODULES:= main gui test runner preproc testcase testclass testresult
pocastest_PLATFORMMODULES:= runner testcase
pocastest_DEFINES:= -DBUILDING_POCAS_TEST -DSTATIC_POCAS_CORE -DSTATIC_POCAS_GUI
pocastest_STATICDEPS:= pocasgui pocascore $(pocasgui_STATICDEPS)
pocastest_STATICLIBS:= pocasgui pocascore $(pocasgui_STATICLIBS)
pocastest_posix_LDFLAGS:= -Wl,-E -pthread
pocastest_win32_LIBS:= gdi32 comctl32
pocastest_win32_STATICLIBS:= pthread
pocastest_posix_LIBS:= dl
$(call binrules, pocastest)
