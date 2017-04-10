pocascore-listtests_MODULES:= list
pocascore-listtests_LIBTYPE:= test
pocascore-listtests_DEPS:= pocastest pocascore
pocascore-listtests_win32_LIBS:= pocastest
pocascore-listtests_PREPROC:= POCASTEST
pocascore-listtests_PREPROC_ADD:= 1
$(call librules, pocascore-listtests)
