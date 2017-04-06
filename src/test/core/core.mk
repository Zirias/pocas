pocascoretests_MODULES:= test
pocascoretests_LIBTYPE:= test
pocascoretests_DEPS:= pocastest pocascore
pocascoretests_win32_LIBS:= pocastest
$(call librules, pocascoretests)
