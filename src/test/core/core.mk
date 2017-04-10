pocascore-listtests_MODULES:= list
pocascore-listtests_LIBTYPE:= test
pocascore-listtests_DEPS:= pocastest pocascore
pocascore-listtests_win32_LIBS:= pocastest
pocascore-listtests_PREPROC:= POCASTEST
$(call librules, pocascore-listtests)
