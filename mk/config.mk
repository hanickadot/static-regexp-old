.PHONY: visible all
	
all:

VIS := @
ECHO := 

libcxx := no
LIBCXX_INCLUDE := 
LIBCXX_LIBPATH := 

OPTIMIZE := -O3 
LDOPTIMIZE := 

override CXX_TYPE := unknown
override CXX_VERSION := $(shell $(CXX) -v 2>&1 | grep " version " )
override UNAME_SYSTEM := $(shell uname)

# detekce gcc-clang (na osx)
ifneq (,$(findstring Apple LLVM,$(CXX_VERSION)))
 override CXX_TYPE_CLANG := clang
 override CXX_TYPE := clang
 override CXX_VERSION := ""
endif

# detekce clangu
ifneq (,$(findstring clang,$(CXX_VERSION)))
 override CXX_TYPE_CLANG := clang
 override CXX_TYPE := clang
 override CXX_CLANG_MAJOR := $(shell $(CXX) -v 2>&1 | grep " version " | cut -d' ' -f3  | cut -d'.' -f1)
 override CXX_CLANG_MINOR := $(shell $(CXX) -v 2>&1 | grep " version " | cut -d' ' -f3  | cut -d'.' -f2)
 ifeq ($(CXX_CLANG_MAJOR),3)
  ifeq ($(CXX_CLANG_MINOR),3)
   # supported version (partially)
  else 
	 ifeq ($(CXX_CLANG_MINOR),4)
    # supported version (partially)
   else 
	  $(error "You don't have at least CLANG 3.3 (with C++11)")
	 endif
	endif
 else
  $(error "You don't have CLANG 3 (you have $(CXX_CLANG_MAJOR).$(CXX_CLANG_MINOR))")
 endif
endif

# detekce gcc
ifneq (,$(findstring gcc,$(CXX_VERSION)))
 override CXX_TYPE_GCC := gcc
 override CXX_TYPE := gcc
 override CXX_GCC_MAJOR := $(shell $(CXX) -v 2>&1 | grep " version " | cut -d' ' -f3  | cut -d'.' -f1)
 override CXX_GCC_MINOR := $(shell $(CXX) -v 2>&1 | grep " version " | cut -d' ' -f3  | cut -d'.' -f2)
 #ifeq ($(CXX_GCC_MAJOR),4)
 # ifeq ($(CXX_GCC_MINOR),7)
 #  # supported version (partially)
 # else 
 #  $(error "You don't have at least GCC 4.7 (with C++11)")
 # endif
 #else
 # $(error "You don't have GCC 4")
 #endif
endif

ifneq (,$(findstring icc,$(CXX_VERSION)))
 override CXX_TYPE_ICC := icc
 override CXX_TYPE := icc
endif

# detekce OSX
ifeq ($(UNAME_SYSTEM),Darwin)
 override SYSTEM := darwin
 override BUILD := DARWIN
endif

# detekce Linuxu
ifeq ($(UNAME_SYSTEM),Linux)
 override SYSTEM := linux
 override BUILD := LINUX
endif

##### KOMBINACE SYSTEMU / KOMPILATORU

override SLIBFLAGS += -shared

lto := yes

ifeq ($(lto), yes)
override FLTO := -flto
override LDOPTIMIZE := $(OPTIMIZE)
else
override FLTO := 
endif

ifneq ($(libcxx), no)
 LIBCXX_INCLUDE := $(libcxx)/include/c++/v1
 LIBCXX_LIBPATH := $(libcxx)/lib
endif

ifeq ($(SYSTEM), linux)
 override SONAME := -Wl,-soname,
else ifeq ($(SYSTEM), darwin)
 override SONAME := -Wl,-install_name,
endif

#ifeq ($(CXX_TYPE), gcc)
# override DEPFLAGS += -std=c++11
# override CXXFLAGS += -std=c++11 -fPIC
# override LDFLAGS += -ldl -lpthread 
#endif

#ifeq ($(CXX_TYPE), icc)
# override DEPFLAGS += -std=c++11 -stdlib=libc++ 
# override CXXFLAGS += -std=c++11 -fPIC -stdlib=libc++ -axSSE4.1
# override LDFLAGS += -ldl -lpthread -lc++
#endif

ifeq ($(CXX_TYPE), clang)
 override CFLAGS += -fPIC 
 override CXXFLAGS +=  -fPIC -std=c++11 -stdlib=libc++
 override DEPFLAGS += -std=c++11 -stdlib=libc++ 
 ifeq ($(SYSTEM), linux)
  override OPTIMIZE := -O3 $(FLTO)
  override LDOPTIMIZE := -O3 $(FLTO)
  override LDFLAGS += -lc++ -lc++abi -ldl -lpthread 
 else ifeq ($(SYSTEM), darwin)
  override OPTIMIZE := -O3 $(FLTO)
  override LDOPTIMIZE := -O3 $(FLTO)
  ifneq ($(LIBCXX_INCLUDE),)
    override DEPFLAGS += -I$(LIBCXX_INCLUDE)
    override CXXFLAGS += -I$(LIBCXX_INCLUDE)
  endif
  ifneq ($(LIBCXX_LIBPATH),)
    override LDFLAGS += -L$(LIBCXX_LIBPATH) -lc++
  endif
  override LDFLAGS +=  -lc++
  override SLIBFLAGS += -Wl,-undefined -Wl,dynamic_lookup
 endif
endif

ifeq ($(debug), yes)
 TYPE := debug
 override CFLAGS += -g -DNDEBUG
 override CXXFLAGS += -g -DNDEBUG
 OPTIMIZE := -O0
 LDOPTIMIZE := 
endif

override CFLAGS += $(OPTIMIZE) $(INCLUDE_HEADS) -DCXX_TYPE_$(CXX_TYPE)
override CXXFLAGS += $(OPTIMIZE) $(INCLUDE_HEADS) -DCXX_TYPE_$(CXX_TYPE)
override DEPFLAGS += $(INCLUDE_HEADS) -DCXX_TYPE_$(CXX_TYPE)
override LDFLAGS += $(LDOPTIMIZE)

verbose := no

ifeq ($(verbose), yes)
override VIS := 
override ECHO := > /dev/null
else
override VIS := @
override ECHO := 
endif
#verbose := no
#override VIS_no := @
#override VIS_yes :=
#override VIS := $(VIS_$(verbose))
#override ECHO_no := 
#override ECHO_yes := > /dev/null
#override ECHO := $(ECHO_$(verbose))

config:
	$(VIS)echo "$(CXXFLAGS)"
	$(VIS)echo "$(TYPE)"
	$(VIS)echo "version: $(CXX_VERSION)"
	$(VIS)echo "$(CXX_TYPE_CLANG)"
	$(VIS)echo "$(CXX_CLANG_MAJOR)"
	$(VIS)echo "$(CXX_CLANG_MINOR)"
	$(VIS)echo "$(CXX_TYPE_LLVM_GCC)"
	$(VIS)echo "type: $(CXX_TYPE)"
	$(VIS)echo "$(CXX_GCC_MAJOR)"
	$(VIS)echo "$(CXX_GCC_MINOR)"
	$(VIS)echo "$(findstring icc,$(CXX_VERSION))"
	$(VIS)echo $(findstring v,$(MAKEFLAGS))