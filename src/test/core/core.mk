pocascore-listtests_MODULES:= list
pocascore-listtests_PREPROCMODULES:= list
pocascore-listtests_LIBTYPE:= test
pocascore-listtests_DEPS:= pocastest pocascore
pocascore-listtests_LIBS:= pocascore
pocascore-listtests_win32_LIBS:= pocastest
pocascore-listtests_PREPROC:= POCASTEST
$(call librules, pocascore-listtests)

pocascore-hashtabletests_MODULES:= hashtable
pocascore-hashtabletests_PREPROCMODULES:= hashtable
pocascore-hashtabletests_LIBTYPE:= test
pocascore-hashtabletests_DEPS:= pocastest pocascore
pocascore-hashtabletests_LIBS:= pocascore
pocascore-hashtabletests_win32_LIBS:= pocastest
pocascore-hashtabletests_PREPROC:= POCASTEST
$(call librules, pocascore-hashtabletests)

pocascore-cmdlinetests_MODULES:= cmdline
pocascore-cmdlinetests_PREPROCMODULES:= cmdline
pocascore-cmdlinetests_LIBTYPE:= test
pocascore-cmdlinetests_DEPS:= pocastest pocascore
pocascore-cmdlinetests_LIBS:= pocascore
pocascore-cmdlinetests_win32_LIBS:= pocastest
pocascore-cmdlinetests_PREPROC:= POCASTEST
$(call librules, pocascore-cmdlinetests)
