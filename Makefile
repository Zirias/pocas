SINGLECONFVARS := POCASTEST WITH_QT
DEFAULT_POCASTEST = $(BINDIR)$(PSEP)pocastest$(EXE) -p
LD_LIBRARY_PATH = $(LIBDIR)
export LD_LIBRARY_PATH

include zimk/zimk.mk

INCLUDES += -I.$(PSEP)include

$(call zinc, src/lib/core/core.mk)
$(call zinc, src/lib/gui/gui.mk)
$(call zinc, src/bin/test/test.mk)

$(call zinc, src/test/test.mk)

check:: tests
	$(VR)$(pocastest_EXE) $(TESTDIR)

checkdebug:: tests
	$(VR)$(pocastest_EXE) -ggdb $(TESTDIR)
