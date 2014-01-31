CXX_SUFFIX := .cpp
LIB_SUFFIX := .so

override LOCAL_SECONDARY_O_FILES += $(addsuffix .obj,$(filter %$(CXX_SUFFIX),$(O_FILES_SECONDARY_TMP)))
override LOCAL_O_FILES += $(addsuffix .obj,$(filter %$(CXX_SUFFIX),$(O_FILES_TMP)))
override LOCAL_D_FILES += $(addsuffix .d,$(filter %$(CXX_SUFFIX),$(D_FILES_TMP)))
override LOCAL_DEP_FILES += $(addsuffix .dep,$(filter %$(CXX_SUFFIX),$(D_FILES_TMP)))
override LOCAL_APPS += $(addsuffix .app,$(filter %$(CXX_SUFFIX),$(CMP_SOURCE_FILES_APPLICATIONS_TMP)))
override LOCAL_LIBS += $(addsuffix .lib,$(filter %$(CXX_SUFFIX),$(CMP_SOURCE_FILES_LIBRARIES_TMP)))

override SECONDARY_O_FILES += $(LOCAL_SECONDARY_O_FILES)
override O_FILES += $(LOCAL_O_FILES)
override D_FILES += $(LOCAL_D_FILES)
override XAPPS += $(LOCAL_APPS)
override XLIBS += $(LOCAL_LIBS)

override CXX_O_FILES_PATTERN := $(addprefix $(OBJ_FOLDER)/,$(addsuffix .obj,%$(CXX_SUFFIX)))

.PRECIOUS: $(LOCAL_O_FILES) $(LOCAL_DEP_FILES) $(LOCAL_APPS) $(LOCAL_LIBS)

$(DEP_FOLDER)/%$(CXX_SUFFIX).dep: %$(CXX_SUFFIX) | $(DEP_FOLDER)/.stamp
	@echo "  DEP "$(patsubst $(DEP_FOLDER)/%.dep,%,$@) $(ECHO)
	$(VIS)$(DEPCXX) -pipe $(DEPFLAGS) $(XDEPFLAGS) $(patsubst $(DEP_FOLDER)/%.dep,$(OBJ_FOLDER)/%.obj,$@) -c $(patsubst $(DEP_FOLDER)/%.dep,%,$@) -o $@
	$(VIS)cp $@ $(patsubst %.dep,%.d,$@)
	@touch $@

$(OBJ_FOLDER)/%$(CXX_SUFFIX).obj: $(DEP_FOLDER)/%$(CXX_SUFFIX).dep | $(OBJ_FOLDER)/.stamp
	@echo "  CXX "$(patsubst $(OBJ_FOLDER)/%.obj,%,$@) $(ECHO)
	$(VIS)$(CXX)  -pipe $(CXXFLAGS) -c $(patsubst $(OBJ_FOLDER)/%.obj,%,$@) -o $@
	@touch $@

$(OUT_FOLDER)/%$(CXX_SUFFIX).app: $(OBJ_FOLDER)/%$(CXX_SUFFIX).obj | $(OUT_FOLDER)/.stamp
	@echo " LINK "$(notdir $(basename $(patsubst %.app,%,$@))) $(ECHO)
	$(VIS)$(CXX)  -pipe $(LDFLAGS) $(LD_$(patsubst $(OUT_FOLDER)/%.app,%,$@)) $(SECONDARY_O_FILES) $(patsubst $(OUT_FOLDER)/%.app,$(OBJ_FOLDER)/%.obj,$@) $(LIBS) -o $@ 
	$(VIS)cp $@ $(BLD_FOLDER)/$(notdir $(basename $(patsubst $(OUT_FOLDER)/%.app,%,$@)))
	
$(OUT_FOLDER)/%$(CXX_SUFFIX).lib: $(OBJ_FOLDER)/%$(CXX_SUFFIX).obj | $(OUT_FOLDER)/.stamp
	@echo " LINK "lib$(notdir $(basename $(patsubst %.lib,%,$@)))$(LIB_SUFFIX) $(ECHO)
	$(VIS)$(CXX)  -pipe $(LDFLAGS) $(LD_$(patsubst $(OUT_FOLDER)/%.lib,%,$@)) $(SECONDARY_O_FILES) $(SLIBFLAGS) $(SONAME)lib$(notdir $(basename $(patsubst %.lib,%,$@)))$(LIB_SUFFIX) $(patsubst $(OUT_FOLDER)/%.lib,$(OBJ_FOLDER)/%.obj,$@) $(LIBS) -o $@ 
	$(VIS)cp $@ $(BLD_FOLDER)/lib$(notdir $(basename $(patsubst $(OUT_FOLDER)/%.lib,%,$@)))$(LIB_SUFFIX)
	