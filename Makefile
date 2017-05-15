SINGLECONFVARS := POCASTEST WITH_GUI_WINAPI WITH_GUI_QT WITH_DEFAULT_GUI_BACKEND
DEFAULT_POCASTEST = $(BINDIR)$(PSEP)pocastest$(EXE) -p
LD_LIBRARY_PATH = $(LIBDIR)
export LD_LIBRARY_PATH

PLATFORM_win32_WITH_GUI_WINAPI := 1
PLATFORM_posix_WITH_GUI_QT := 1

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
