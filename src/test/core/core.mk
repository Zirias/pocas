pocascore-listtests_MODULES:= list
pocascore-listtests_LIBTYPE:= test
pocascore-listtests_DEPS:= pocastest pocascore
pocascore-listtests_win32_LIBS:= pocastest
$(call librules, pocascore-listtests)
