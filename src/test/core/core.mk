pocascore-listtests_MODULES:= list
pocascore-listtests_LIBTYPE:= test
pocascore-listtests_DEPS:= pocastest pocascore
pocascore-listtests_LIBS:= pocascore
pocascore-listtests_win32_LIBS:= pocastest
pocascore-listtests_PREPROC:= POCASTEST
pocascore-listtests_PREPROC_ADD:= 1
$(call librules, pocascore-listtests)

pocascore-hashtabletests_MODULES:= hashtable
pocascore-hashtabletests_LIBTYPE:= test
pocascore-hashtabletests_DEPS:= pocastest pocascore
pocascore-hashtabletests_LIBS:= pocascore
pocascore-hashtabletests_win32_LIBS:= pocastest
pocascore-hashtabletests_PREPROC:= POCASTEST
pocascore-hashtabletests_PREPROC_ADD:= 1
$(call librules, pocascore-hashtabletests)
