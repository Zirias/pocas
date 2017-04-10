pocastesttests_MODULES:= test
pocastesttests_LIBTYPE:= test
pocastesttests_DEPS:= pocastest
pocastesttests_win32_LIBS:= pocastest
pocastesttests_PREPROC:= POCASTEST
pocastesttests_PREPROC_ADD:= 1
$(call librules, pocastesttests)
