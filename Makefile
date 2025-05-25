#
# OMNeT++/OMNEST Makefile for Simcan2Fog
#
# This file was generated with the command:
#  opp_makemake -f -O out -KINET4_5_PROJ=../inet4.5 -DINET_IMPORT -I$$\(INET4_5_PROJ\)/src -L$$\(INET4_5_PROJ\)/src -Lout/$$\(CONFIGNAME\)/src -lSimcan2Fog$$\(D\) -lINET$$\(D\) -d src -X.
#

# Name of target to be created (-o option)
TARGET_DIR = bin
TARGET_NAME = Simcan2Fog$(D)
TARGET = $(TARGET_NAME)$(EXE_SUFFIX)
TARGET_IMPLIB = $(TARGET_NAME)$(IMPLIB_SUFFIX)
TARGET_IMPDEF = $(TARGET_NAME)$(IMPDEF_SUFFIX)
TARGET_FILES = $(TARGET_DIR)/$(TARGET)

# User interface (uncomment one) (-u option)
USERIF_LIBS = $(ALL_ENV_LIBS) # that is, $(QTENV_LIBS) $(CMDENV_LIBS)
#USERIF_LIBS = $(CMDENV_LIBS)
#USERIF_LIBS = $(QTENV_LIBS)

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS = $(LDFLAG_LIBPATH)$(INET4_5_PROJ)/src $(LDFLAG_LIBPATH)out/$(CONFIGNAME)/src  -lSimcan2Fog$(D) -lINET$(D)

# Output directory
PROJECT_OUTPUT_DIR = out
PROJECTRELATIVE_PATH =
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Other makefile variables (-K)
INET4_5_PROJ=../inet4.5

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
CONFIGFILE = $(shell opp_configfilepath)
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIBS = $(OPPMAIN_LIB) $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)
ifneq ($(PLATFORM),win32)
LIBS += -Wl,-rpath,$(abspath $(INET4_5_PROJ)/src) -Wl,-rpath,$(abspath out/$(CONFIGNAME)/src)
endif

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE (if COPTS has changed since last build)
# and make the object files depend on it
COPTS_FILE = $O/.last-copts
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
  $(shell $(MKPATH) "$O")
  $(file >$(COPTS_FILE),$(COPTS))
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
-include makefrag

#------------------------------------------------------------------------------

# Main target
all: $(TARGET_FILES)

$(TARGET_DIR)/% :: $O/%
	@mkdir -p $(TARGET_DIR)
	$(Q)$(LN) $< $@
ifeq ($(TOOLCHAIN_NAME),clang-msabi)
	-$(Q)-$(LN) $(<:%.dll=%.lib) $(@:%.dll=%.lib) 2>/dev/null

$O/$(TARGET_NAME).pdb: $O/$(TARGET)
endif

$O/$(TARGET):  submakedirs $(wildcard $(EXTRA_OBJS)) Makefile $(CONFIGFILE)
	@$(MKPATH) $O
	@echo Creating executable: $@
	$(Q)$(CXX) $(LDFLAGS) -o $O/$(TARGET)  $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS)

submakedirs:  src_dir

.PHONY: all clean cleanall depend msgheaders smheaders  src
src: src_dir

src_dir:
	cd src && $(MAKE) all

msgheaders:
	$(Q)cd src && $(MAKE) msgheaders

smheaders:
	$(Q)cd src && $(MAKE) smheaders

clean:
	$(qecho) Cleaning $(TARGET)
	$(Q)-rm -rf $O
	$(Q)-rm -f $(TARGET_FILES)
	$(Q)-rm -f $(call opp_rwildcard, . , *_m.cc *_m.h *_sm.cc *_sm.h)
	-$(Q)cd src && $(MAKE) clean

cleanall:
	$(Q)$(CLEANALL_COMMAND)
	$(Q)-rm -rf $(PROJECT_OUTPUT_DIR)

help:
	@echo "$$HELP_SYNOPSYS"
	@echo "$$HELP_TARGETS"
	@echo "$$HELP_VARIABLES"
	@echo "$$HELP_EXAMPLES"

# include all dependencies
-include $(OBJS:%=%.d) $(MSGFILES:%.msg=$O/%_m.h.d)
