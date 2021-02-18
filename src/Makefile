#
# OMNeT++/OMNEST Makefile for Simcan2Cloud
#
# This file was generated with the command:
#  opp_makemake -f --deep -O out -I.
#

# Name of target to be created (-o option)
TARGET = Simcan2Cloud$(D)$(EXE_SUFFIX)
TARGET_DIR = .

# User interface (uncomment one) (-u option)
USERIF_LIBS = $(ALL_ENV_LIBS) # that is, $(TKENV_LIBS) $(QTENV_LIBS) $(CMDENV_LIBS)
#USERIF_LIBS = $(CMDENV_LIBS)
#USERIF_LIBS = $(TKENV_LIBS)
#USERIF_LIBS = $(QTENV_LIBS)

# C++ include paths (with -I)
INCLUDE_PATH = -I.

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS =

# Output directory
PROJECT_OUTPUT_DIR = ../out
PROJECTRELATIVE_PATH = src
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc, .msg and .sm files
OBJS = \
    $O/Applications/Base/UserAppBase/UserAppBase.o \
    $O/Applications/UserApps/DummyApp/DummyApplication.o \
    $O/Applications/UserApps/LocalApplication/LocalApplication.o \
    $O/Architecture/Cpu/CpuCores/CpuCore/CpuCore.o \
    $O/Architecture/Network/DataCenterNetwork/CommNetwork/CommNetwork.o \
    $O/Architecture/Network/DataCenterNetwork/StorageNetwork/StorageNetwork.o \
    $O/Architecture/Network/Switch/Switch.o \
    $O/Architecture/Nodes/HardwareManagers/HardwareManager/HardwareManager.o \
    $O/Core/cSIMCAN_Core.o \
    $O/Core/CheckingHubModule/CheckingHubModule.o \
    $O/Management/CloudManagerBase/CloudManagerBase.o \
    $O/Management/CloudProviders/CloudProviderBase/CloudProviderBase.o \
    $O/Management/CloudProviders/CloudProviderBase_firstBestFit/CloudProviderBase_firstBestFit.o \
    $O/Management/CloudProviders/CloudProviderBase_firstBestFit/NodeResourceInfo.o \
    $O/Management/CloudProviders/CloudProvider_firstBestFit/CloudProvider_firstBestFit.o \
    $O/Management/DataCenterManagers/DataCenterManager/DataCenterManager.o \
    $O/Management/DataCenterManagers/DataCenterManagerCost/DataCenterManagerCost.o \
    $O/Management/UserGenerators/UserGeneratorBase/UserGeneratorBase.o \
    $O/Management/UserGenerators/UserGeneratorCost/UserGeneratorCost.o \
    $O/Management/UserGenerators/UserGenerator_simple/UserGenerator_simple.o \
    $O/Management/dataClasses/AppInstance.o \
    $O/Management/dataClasses/AppInstanceCollection.o \
    $O/Management/dataClasses/Application.o \
    $O/Management/dataClasses/AppParameter.o \
    $O/Management/dataClasses/CloudUser.o \
    $O/Management/dataClasses/CloudUserInstance.o \
    $O/Management/dataClasses/CloudUserInstancePriority.o \
    $O/Management/dataClasses/CloudUserPriority.o \
    $O/Management/dataClasses/DataCenter.o \
    $O/Management/dataClasses/DataCenter_CpuType.o \
    $O/Management/dataClasses/DataCenterInfoCollection.o \
    $O/Management/dataClasses/DataCenterInfoCollectionReservation.o \
    $O/Management/dataClasses/DataCenterReservation.o \
    $O/Management/dataClasses/Node.o \
    $O/Management/dataClasses/NodeInfo.o \
    $O/Management/dataClasses/NodeResourceRequest.o \
    $O/Management/dataClasses/NodeUserInfo_.o \
    $O/Management/dataClasses/Rack.o \
    $O/Management/dataClasses/RackInfo.o \
    $O/Management/dataClasses/RackInstances.o \
    $O/Management/dataClasses/Sla.o \
    $O/Management/dataClasses/User.o \
    $O/Management/dataClasses/UserAppReference.o \
    $O/Management/dataClasses/UserInstance.o \
    $O/Management/dataClasses/UserVmReference.o \
    $O/Management/dataClasses/VirtualMachine.o \
    $O/Management/dataClasses/VmInstance.o \
    $O/Management/dataClasses/VmInstanceCollection.o \
    $O/Management/parser/AppListParser.o \
    $O/Management/parser/DataCenterConfigParser.o \
    $O/Management/parser/DataCenterListParser.o \
    $O/Management/parser/DataCenterReservationConfigParser.o \
    $O/Management/parser/DataCenterReservationListParser.o \
    $O/Management/parser/SlaListParser.o \
    $O/Management/parser/UserListParser.o \
    $O/Management/parser/UserPriorityListParser.o \
    $O/Management/parser/VmListParser.o \
    $O/Management/utils/LogUtils.o \
    $O/Messages/SIMCAN_Message.o \
    $O/Messages/SM_CPU_Message.o \
    $O/Messages/SM_UserAPP.o \
    $O/Messages/SM_UserVM.o \
    $O/OperatingSystem/CpuSchedulers/CpuSchedulerRR/CpuSchedulerRR.o \
    $O/OperatingSystem/Hypervisors/Hypervisor/Hypervisor.o \
    $O/Messages/SIMCAN_Message_m.o \
    $O/Messages/SM_CloudProvider_Control_m.o \
    $O/Messages/SM_CPU_Message_m.o \
    $O/Messages/SM_UserAPP_m.o \
    $O/Messages/SM_UserAPP_Finish_m.o \
    $O/Messages/SM_UserVM_m.o \
    $O/Messages/SM_UserVM_Cost_m.o \
    $O/Messages/SM_UserVM_Finish_m.o

# Message files
MSGFILES = \
    Messages/SIMCAN_Message.msg \
    Messages/SM_CloudProvider_Control.msg \
    Messages/SM_CPU_Message.msg \
    Messages/SM_UserAPP.msg \
    Messages/SM_UserAPP_Finish.msg \
    Messages/SM_UserVM.msg \
    Messages/SM_UserVM_Cost.msg \
    Messages/SM_UserVM_Finish.msg

# SM files
SMFILES =

#------------------------------------------------------------------------------

# Pull in OMNeT++ configuration (Makefile.inc)

ifneq ("$(OMNETPP_CONFIGFILE)","")
CONFIGFILE = $(OMNETPP_CONFIGFILE)
else
ifneq ("$(OMNETPP_ROOT)","")
CONFIGFILE = $(OMNETPP_ROOT)/Makefile.inc
else
CONFIGFILE = $(shell opp_configfilepath)
endif
endif

ifeq ("$(wildcard $(CONFIGFILE))","")
$(error Config file '$(CONFIGFILE)' does not exist -- add the OMNeT++ bin directory to the path so that opp_configfilepath can be found, or set the OMNETPP_CONFIGFILE variable to point to Makefile.inc)
endif

include $(CONFIGFILE)

# Simulation kernel and user interface libraries
OMNETPP_LIBS = $(OPPMAIN_LIB) $(USERIF_LIBS) $(KERNEL_LIBS) $(SYS_LIBS)

COPTS = $(CFLAGS) $(IMPORT_DEFINES)  $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)
SMCOPTS =

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE and have object
# files depend on it (except when "make depend" was called)
COPTS_FILE = $O/.last-copts
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
$(shell $(MKPATH) "$O" && echo "$(COPTS)" >$(COPTS_FILE))
endif

#------------------------------------------------------------------------------
# User-supplied makefile fragment(s)
# >>>
# <<<
#------------------------------------------------------------------------------

# Main target
all: $(TARGET_DIR)/$(TARGET)

$(TARGET_DIR)/% :: $O/%
	@mkdir -p $(TARGET_DIR)
	$(Q)$(LN) $< $@
ifeq ($(TOOLCHAIN_NAME),clangc2)
	$(Q)-$(LN) $(<:%.dll=%.lib) $(@:%.dll=%.lib)
endif

$O/$(TARGET): $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile $(CONFIGFILE)
	@$(MKPATH) $O
	@echo Creating executable: $@
	$(Q)echo >.tmp$$$$ $(OBJS) $(EXTRA_OBJS) && $(AR) .tmplib$$$$ @.tmp$$$$ && $(CXX) -o $O/$(TARGET) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) .tmplib$$$$ $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS) $(LDFLAGS) && rm .tmp$$$$ && rm .tmplib$$$$

.PHONY: all clean cleanall depend msgheaders smheaders

.SUFFIXES: .cc

$O/%.o: %.cc $(COPTS_FILE) | msgheaders smheaders
	@$(MKPATH) $(dir $@)
	$(qecho) "$<"
	$(Q)$(CXX) -c $(CXXFLAGS) $(COPTS) -o $@ $<

%_m.cc %_m.h: %.msg
	$(qecho) MSGC: $<
	$(Q)$(MSGC) -s _m.cc -MD -MP -MF $O/$(basename $<)_m.h.d $(MSGCOPTS) $?

%_sm.cc %_sm.h: %.sm
	$(qecho) SMC: $<
	$(Q)$(SMC) -c++ -suffix cc $(SMCOPTS) $?

msgheaders: $(MSGFILES:.msg=_m.h)

smheaders: $(SMFILES:.sm=_sm.h)

clean:
	$(qecho) Cleaning $(TARGET)
	$(Q)-rm -rf $O
	$(Q)-rm -f $(TARGET_DIR)/$(TARGET)
	$(Q)-rm -f $(TARGET_DIR)/$(TARGET:%.dll=%.lib)
	$(Q)-rm -f $(call opp_rwildcard, . , *_m.cc *_m.h *_sm.cc *_sm.h)

cleanall:
	$(Q)$(MAKE) -s clean MODE=release
	$(Q)$(MAKE) -s clean MODE=debug
	$(Q)-rm -rf $(PROJECT_OUTPUT_DIR)

# include all dependencies
-include $(OBJS:%=%.d) $(MSGFILES:%.msg=$O/%_m.h.d)