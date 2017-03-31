pocascore_MODULES:= string stringbuilder list event eventloop
pocascore_PLATFORMMODULES := event eventloop file processhost
pocascore_DEFINES:= -DBUILDING_POCAS_CORE
pocascore_V_MAJ:= 0
pocascore_V_MIN:= 0
pocascore_V_REV:= 1
$(call librules, pocascore)
