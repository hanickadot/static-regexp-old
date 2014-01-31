 .PHONY: list all clean cleanall clean-all link

all:

override BUILD_ID := $(SYSTEM)-$(CXX_TYPE)-$(TYPE)

SHARED_LIBRARY_SUFFIX := .so
STATIC_LIBRARY_SUFFIX := .a

XDEPFLAGS := -MM -MT
DEPC := $(CC)
DEPCXX := $(CXX)
AR := ar -rcs

BUILDROOT := build

# vypisovani

AR_ECHO := '  AR '

# build cesty

OBJ_FOLDER := $(BUILDROOT)/$(BUILD_ID)/objects
DEP_FOLDER := $(BUILDROOT)/$(BUILD_ID)/deps
OUT_FOLDER := $(BUILDROOT)/$(BUILD_ID)/exec
BLD_FOLDER := $(BUILDROOT)/$(BUILD_ID)

# priprava souboru

STATIC_LIB := $(BLD_FOLDER)/lib$(PROJECT_NAME)$(STATIC_LIBRARY_SUFFIX)

CMP_SOURCE_FILES := $(sort $(filter-out $(IGNORE),$(SOURCE_FILES) $(filter-out $(IGNORE),$(LIBRARY_TARGETS)) $(filter-out $(IGNORE),$(APPLICATION_TARGETS))))
CMP_SOURCE_FILES_SECONDARY := $(filter-out $(APPLICATION_TARGETS) $(LIBRARY_TARGETS),$(CMP_SOURCE_FILES))
CMP_SOURCE_FILES_APPLICATIONS := $(filter-out $(IGNORE),$(sort $(APPLICATION_TARGETS)))
CMP_SOURCE_FILES_LIBRARIES := $(filter-out $(IGNORE),$(sort $(LIBRARY_TARGETS)))

O_FILES_TMP := $(patsubst %,$(OBJ_FOLDER)/%,$(CMP_SOURCE_FILES))
D_FILES_TMP := $(patsubst %,$(DEP_FOLDER)/%,$(CMP_SOURCE_FILES))
CMP_SOURCE_FILES_APPLICATIONS_TMP := $(patsubst %,$(OUT_FOLDER)/%,$(CMP_SOURCE_FILES_APPLICATIONS))
CMP_SOURCE_FILES_LIBRARIES_TMP := $(patsubst %,$(OUT_FOLDER)/%,$(CMP_SOURCE_FILES_LIBRARIES))

O_FILES_SECONDARY_TMP := $(patsubst %,$(OBJ_FOLDER)/%,$(CMP_SOURCE_FILES_SECONDARY))

SECONDARY_O_FILES :=
D_FILES := 
O_FILES :=
XAPPS :=
XLIBS :=

include mk/lng.cxx.mk

.PRECIOUS: $(OBJ_FOLDER)/.stamp $(DEP_FOLDER)/.stamp $(OUT_FOLDER)/.stamp $(BLD_FOLDER)/.stamp

$(XAPPS): $(SECONDARY_O_FILES) $(XLIBS)
$(XLIBS): $(SECONDARY_O_FILES)

all: $(XAPPS) $(XLIBS) link
	
link: $(APPS) $(XLIBS)
	$(VIS)rm -f $(BUILDROOT)/native
	$(VIS)ln -s ./$(BUILD_ID) $(BUILDROOT)/native

list:
	@echo O_FILES: $(O_FILES)
	@echo D_FILES: $(D_FILES)
	@echo O_FILES_TMP: $(O_FILES_TMP)
	@echo SECONDARY_O_FILES: $(SECONDARY_O_FILES)
	
%/.stamp: $(CMP_SOURCE_FILES)
	$(VIS)mkdir -p $(shell echo $(CMP_SOURCE_FILES) | xargs -n1 dirname | uniq | grep -xv '\.' | sed -e 's:^:$(patsubst %/.stamp,%,$@)\/:')
	$(VIS)touch $@

clean-all: clean
	@rm -rf $(BUILDROOT)

clean:
	@rm -rf $(BLD_FOLDER)
	
-include $(D_FILES)
