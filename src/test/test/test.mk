pocastesttests_MODULES:= test
pocastesttests_LIBTYPE:= test
pocastesttests_DEPS:= pocastest
pocastesttests_win32_LIBS:= pocastest
$(call librules, pocastesttests)
