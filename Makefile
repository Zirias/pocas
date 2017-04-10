include zimk/zimk.mk

INCLUDES += -I.$(PSEP)include

$(call zinc, src/lib/core/core.mk)
$(call zinc, src/bin/test/test.mk)

$(call zinc, src/test/test.mk)

check:: tests
	$(VR)$(pocastest_EXE) $(TESTDIR)

checkdebug:: tests
	$(VR)$(pocastest_EXE) -ggdb $(TESTDIR)

