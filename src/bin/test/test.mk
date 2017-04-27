pocastest_MODULES:= main gui test runner preproc testcase testclass testresult
pocastest_PLATFORMMODULES:= runner testcase
pocastest_DEFINES:= -DBUILDING_POCAS_TEST -DSTATIC_POCAS_CORE -DSTATIC_POCAS_GUI
pocastest_STATICDEPS:= pocasgui pocascore
pocastest_STATICLIBS:= pocasgui pocascore
pocastest_posix_LDFLAGS:= -pthread
pocastest_win32_STATICLIBS:= pthread
$(call binrules, pocastest)
