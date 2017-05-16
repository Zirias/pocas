pocastesttests_MODULES:= test
pocastesttests_PREPROCMODULES:= test
pocastesttests_LIBTYPE:= test
pocastesttests_DEPS:= pocastest
pocastesttests_win32_LIBS:= pocastest
pocastesttests_PREPROC:= POCASTEST
$(call librules, pocastesttests)
