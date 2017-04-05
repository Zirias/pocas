pocascore_MODULES:= string stringbuilder list mtwaitqueue event eventloop
pocascore_PLATFORMMODULES := event eventloop file processhost
pocascore_DEFINES:= -DBUILDING_POCAS_CORE
pocascore_posix_LDFLAGS:= -pthread
pocascore_win32_STATICLIBS:= pthread
pocascore_V_MAJ:= 0
pocascore_V_MIN:= 0
pocascore_V_REV:= 1
$(call librules, pocascore)
