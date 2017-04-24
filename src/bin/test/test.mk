pocastest_MODULES:= main test runner preproc
pocastest_PLATFORMMODULES:= runner
pocastest_DEFINES:= -DBUILDING_POCAS_TEST -DSTATIC_POCAS_CORE
pocastest_STATICDEPS:= pocascore
pocastest_STATICLIBS:= pocascore
pocastest_posix_LDFLAGS:= -pthread
pocastest_win32_STATICLIBS:= pthread
$(call binrules, pocastest)
