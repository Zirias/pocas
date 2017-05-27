pocascore_MODULES:= cmdline string stringbuilder list hashtable plugin mtwaitqueue event eventloop textcolor
pocascore_PLATFORMMODULES := textcolor dso plugin eventloop file processhost
pocascore_DEFINES:= -DBUILDING_POCAS_CORE
pocascore_posix_LDFLAGS:= -pthread
pocascore_win32_STATICLIBS:= pthread
pocascore_posix_LIBS:= dl
pocascore_V_MAJ:= 0
pocascore_V_MIN:= 0
pocascore_V_REV:= 1
pocascore_HEADERS_INSTALL:= core/basedecl core/decl core/cmdline core/dso \
        core/event core/event_posix core/event_win32 core/eventloop \
        core/eventloop_win32 core/file core/file_posix core/file_win32 \
        core/hashtable core/list core/mtwaitqueue core/plugin \
        core/processhost core/string core/stringbuilder core/textcolor
pocascore_HEADERDIR:= include/pocas
pocascore_HEADERTGTDIR:= $(includedir)/pocas
$(call librules, pocascore)

ifeq ($(PLATFORM),win32)
pocascore_LIBS_STATIC:= pthread
endif

