pocascore_MODULES:= cmdline string stringbuilder list hashtable plugin mtwaitqueue event eventloop textcolor
pocascore_PLATFORMMODULES := textcolor dso plugin eventloop file processhost
pocascore_DEFINES:= -DBUILDING_POCAS_CORE
pocascore_posix_LDFLAGS:= -pthread
pocascore_win32_STATICLIBS:= pthread
pocascore_posix_LIBS:= dl
pocascore_V_MAJ:= 0
pocascore_V_MIN:= 0
pocascore_V_REV:= 1
$(call librules, pocascore)
