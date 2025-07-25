#
# OMNeT++/OMNEST Makefile for $(LIB_PREFIX)Simcan2Fog
#
# This file was generated with the command:
#  opp_makemake --make-so -f --deep -O out -KINET4_5_PROJ=../../inet4.5 -DINET_IMPORT -I. -I$$\(INET4_5_PROJ\)/src -L$$\(INET4_5_PROJ\)/src -lINET$$\(D\)
#

# Name of target to be created (-o option)
TARGET_DIR = .
TARGET_NAME = $(LIB_PREFIX)Simcan2Fog$(D)
TARGET = $(TARGET_NAME)$(SHARED_LIB_SUFFIX)
TARGET_IMPLIB = $(TARGET_NAME)$(IMPLIB_SUFFIX)
TARGET_IMPDEF = $(TARGET_NAME)$(IMPDEF_SUFFIX)
TARGET_FILES = $(TARGET_DIR)/$(TARGET)

# C++ include paths (with -I)
INCLUDE_PATH = -I. -I$(INET4_5_PROJ)/src

# Additional object and library files to link with
EXTRA_OBJS =

# Additional libraries (-L, -l options)
LIBS = $(LDFLAG_LIBPATH)$(INET4_5_PROJ)/src  -lINET$(D)

# Output directory
PROJECT_OUTPUT_DIR = ../out
PROJECTRELATIVE_PATH = src
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc, .msg and .sm files
OBJS = \
    $O/s2f/apps/AppBase.o \
    $O/s2f/apps/ApplicationBuilder.o \
    $O/s2f/apps/cloud/LocalApplication.o \
    $O/s2f/apps/dummy/DummyApplication.o \
    $O/s2f/apps/endpoints/ClientEndpoint.o \
    $O/s2f/apps/endpoints/ProbingClient.o \
    $O/s2f/apps/endpoints/ServerEndpoint.o \
    $O/s2f/apps/iot/IotApplication.o \
    $O/s2f/apps/iot/LocalIotApplication.o \
    $O/s2f/apps/services/IotService.o \
    $O/s2f/apps/services/SimService.o \
    $O/s2f/architecture/cpu/CpuCore.o \
    $O/s2f/architecture/disk/DiskManager.o \
    $O/s2f/architecture/dns/DnsCommon.o \
    $O/s2f/architecture/dns/client/DnsClientService.o \
    $O/s2f/architecture/dns/db/DnsDb.o \
    $O/s2f/architecture/dns/db/DnsTree.o \
    $O/s2f/architecture/dns/registry/DnsRegistryService.o \
    $O/s2f/architecture/dns/resolver/DnsResolverService.o \
    $O/s2f/architecture/dns/server/DnsServiceSimplified.o \
    $O/s2f/architecture/messagequeue/MessageQueueIn.o \
    $O/s2f/architecture/messagequeue/MessageQueueOut.o \
    $O/s2f/architecture/net/card/NetworkCard.o \
    $O/s2f/architecture/net/db/SimpleNetDb.o \
    $O/s2f/architecture/net/hub/CheckingHubModule.o \
    $O/s2f/architecture/net/stack/proxy/AppProxy.o \
    $O/s2f/architecture/net/stack/proxy/ServiceTable.o \
    $O/s2f/architecture/net/stack/proxy/Session.o \
    $O/s2f/architecture/net/stack/queue/PullClient.o \
    $O/s2f/architecture/net/stack/queue/PushClient.o \
    $O/s2f/architecture/net/stack/registry/ServiceRegistrar.o \
    $O/s2f/architecture/net/stack/resolver/StubDnsResolver.o \
    $O/s2f/architecture/net/switch/Switch.o \
    $O/s2f/architecture/nodes/iot/sensors/IotSensorModel.o \
    $O/s2f/core/cSIMCAN_Core.o \
    $O/s2f/core/include/internal_types.o \
    $O/s2f/core/include/signals.o \
    $O/s2f/core/logic/LogicDemuxHub.o \
    $O/s2f/core/simdata/DataManager.o \
    $O/s2f/core/simdata/DataRepository.o \
    $O/s2f/core/simdata/engine/DataEngine.o \
    $O/s2f/core/simdata/engine/JsonDataEngine.o \
    $O/s2f/core/simdata/engine/sql/MySqlDataEngine.o \
    $O/s2f/management/cloudprovider/CloudProvider.o \
    $O/s2f/management/cloudprovider/NodeDb.o \
    $O/s2f/management/cloudprovider/NodeTable.o \
    $O/s2f/management/dataClasses/Applications/AppInstance.o \
    $O/s2f/management/dataClasses/Applications/AppInstanceCollection.o \
    $O/s2f/management/dataClasses/Applications/Application.o \
    $O/s2f/management/dataClasses/SLAs/Sla.o \
    $O/s2f/management/dataClasses/Users/CloudUser.o \
    $O/s2f/management/dataClasses/Users/CloudUserInstance.o \
    $O/s2f/management/dataClasses/Users/CloudUserInstancePriority.o \
    $O/s2f/management/dataClasses/Users/CloudUserInstanceTrace.o \
    $O/s2f/management/dataClasses/Users/User.o \
    $O/s2f/management/dataClasses/Users/UserAppReference.o \
    $O/s2f/management/dataClasses/Users/UserInstance.o \
    $O/s2f/management/dataClasses/Users/UserVmReference.o \
    $O/s2f/management/dataClasses/VirtualMachines/VirtualMachine.o \
    $O/s2f/management/dataClasses/VirtualMachines/VmInstance.o \
    $O/s2f/management/managers/DcManager.o \
    $O/s2f/management/managers/ManagerBase.o \
    $O/s2f/management/managers/Node.o \
    $O/s2f/management/managers/NodePool.o \
    $O/s2f/management/managers/ResourceManager.o \
    $O/s2f/management/managers/selection/Strategies.o \
    $O/s2f/management/utils/LogUtils.o \
    $O/s2f/messages/SIMCAN_Message.o \
    $O/s2f/messages/SM_CPU_Message.o \
    $O/s2f/messages/SM_UserAPP.o \
    $O/s2f/messages/SM_UserVM.o \
    $O/s2f/messages/VMRequest.o \
    $O/s2f/os/control/ControlEntries.o \
    $O/s2f/os/control/ProcessDemux.o \
    $O/s2f/os/control/VmControlTable.o \
    $O/s2f/os/hardwaremanagers/DcHardwareManager.o \
    $O/s2f/os/hardwaremanagers/EdgeHardwareManager.o \
    $O/s2f/os/hardwaremanagers/HardwareManager.o \
    $O/s2f/os/hardwaremanagers/NodeResources.o \
    $O/s2f/os/hypervisors/DcHypervisor.o \
    $O/s2f/os/hypervisors/EdgeHypervisor.o \
    $O/s2f/os/hypervisors/Hypervisor.o \
    $O/s2f/os/schedulers/CpuSchedulerRR.o \
    $O/s2f/users/base/BaseUserModel.o \
    $O/s2f/users/cloud/CloudUserModel.o \
    $O/s2f/users/edge/EdgeUser.o \
    $O/s2f/users/generators/UserGenerator_simple.o \
    $O/s2f/users/generators/UserGeneratorBase.o \
    $O/s2f/users/generators/UserGeneratorCost.o \
    $O/s2f/apps/services/SimServiceReq_m.o \
    $O/s2f/architecture/dns/DnsRequest_m.o \
    $O/s2f/architecture/dns/ResourceRecord_m.o \
    $O/s2f/architecture/dns/client/DnsClientCommand_m.o \
    $O/s2f/architecture/dns/registry/DnsRegistrationRequest_m.o \
    $O/s2f/architecture/net/protocol/L2Protocol_m.o \
    $O/s2f/architecture/net/protocol/RestfulRequest_m.o \
    $O/s2f/architecture/net/protocol/RestfulResponse_m.o \
    $O/s2f/architecture/net/protocol/RestMsg_m.o \
    $O/s2f/architecture/net/stack/proxy/ProxyServiceRequest_m.o \
    $O/s2f/architecture/net/stack/resolver/StubDnsRequest_m.o \
    $O/s2f/architecture/nodes/iot/IotPayload_m.o \
    $O/s2f/management/cloudprovider/NodeUpdate_m.o \
    $O/s2f/messages/INET_AppMessage_m.o \
    $O/s2f/messages/SIMCAN_Message_m.o \
    $O/s2f/messages/SM_AppSettings_m.o \
    $O/s2f/messages/SM_CloudProvider_Control_m.o \
    $O/s2f/messages/SM_CPU_Message_m.o \
    $O/s2f/messages/SM_UserAPP_m.o \
    $O/s2f/messages/SM_UserVM_m.o \
    $O/s2f/messages/SM_UserVM_Cost_m.o \
    $O/s2f/messages/SM_VmExtend_m.o \
    $O/s2f/messages/Syscall_m.o \
    $O/s2f/users/edge/EdgeUsersControl_m.o

# Message files
MSGFILES = \
    s2f/apps/services/SimServiceReq.msg \
    s2f/architecture/dns/DnsRequest.msg \
    s2f/architecture/dns/ResourceRecord.msg \
    s2f/architecture/dns/client/DnsClientCommand.msg \
    s2f/architecture/dns/registry/DnsRegistrationRequest.msg \
    s2f/architecture/net/protocol/L2Protocol.msg \
    s2f/architecture/net/protocol/RestfulRequest.msg \
    s2f/architecture/net/protocol/RestfulResponse.msg \
    s2f/architecture/net/protocol/RestMsg.msg \
    s2f/architecture/net/stack/proxy/ProxyServiceRequest.msg \
    s2f/architecture/net/stack/resolver/StubDnsRequest.msg \
    s2f/architecture/nodes/iot/IotPayload.msg \
    s2f/management/cloudprovider/NodeUpdate.msg \
    s2f/messages/INET_AppMessage.msg \
    s2f/messages/SIMCAN_Message.msg \
    s2f/messages/SM_AppSettings.msg \
    s2f/messages/SM_CloudProvider_Control.msg \
    s2f/messages/SM_CPU_Message.msg \
    s2f/messages/SM_UserAPP.msg \
    s2f/messages/SM_UserVM.msg \
    s2f/messages/SM_UserVM_Cost.msg \
    s2f/messages/SM_VmExtend.msg \
    s2f/messages/Syscall.msg \
    s2f/users/edge/EdgeUsersControl.msg

# SM files
SMFILES =

# Other makefile variables (-K)
INET4_5_PROJ=../../inet4.5

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
OMNETPP_LIBS = -loppenvir$D $(KERNEL_LIBS) $(SYS_LIBS)
ifneq ($(PLATFORM),win32)
LIBS += -Wl,-rpath,$(abspath $(INET4_5_PROJ)/src)
endif

COPTS = $(CFLAGS) $(IMPORT_DEFINES) -DINET_IMPORT $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
MSGCOPTS = $(INCLUDE_PATH)
SMCOPTS =

# we want to recompile everything if COPTS changes,
# so we store COPTS into $COPTS_FILE (if COPTS has changed since last build)
# and make the object files depend on it
COPTS_FILE = $O/.last-copts
ifneq ("$(COPTS)","$(shell cat $(COPTS_FILE) 2>/dev/null || echo '')")
  $(shell $(MKPATH) "$O")
  $(file >$(COPTS_FILE),$(COPTS))
endif

# On Windows, the target has additional file(s). An import lib and an optional debug symbol file is created too.
ifeq ($(PLATFORM),win32)
  TARGET_FILES+= $(TARGET_DIR)/$(TARGET_IMPLIB)
  LDFLAGS+= $(LDFLAG_IMPLIB)$O/$(TARGET_IMPLIB)
  ifeq ($(TOOLCHAIN_NAME),clang-msabi)
    ifeq ($(MODE),debug)
      TARGET_FILES+=$(TARGET_DIR)/$(TARGET_NAME).pdb
    endif
  else
    TARGET_FILES+= $(TARGET_DIR)/$(TARGET_IMPDEF)
    LDFLAGS+= $(LDFLAG_IMPDEF)$O/$(TARGET_IMPDEF)
  endif
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

$O/$(TARGET) $O/$(TARGET_IMPDEF) $O/$(TARGET_IMPLIB) &: $(OBJS)  $(wildcard $(EXTRA_OBJS)) Makefile $(CONFIGFILE)
	@$(MKPATH) $O
	@echo Creating shared library: $@
	$(Q)$(SHLIB_LD) -o $O/$(TARGET) $(OBJS) $(EXTRA_OBJS) $(AS_NEEDED_OFF) $(WHOLE_ARCHIVE_ON) $(LIBS) $(WHOLE_ARCHIVE_OFF) $(OMNETPP_LIBS) $(LDFLAGS)
	$(Q)$(SHLIB_POSTPROCESS) $O/$(TARGET)
ifeq ($(PLATFORM),win32)
	$(Q)llvm-ar d $O/$(TARGET_IMPLIB) $(TARGET) # WORKAROUND: throw away the first file from the archive to make the LLD generated import lib valid
endif

.PHONY: all clean cleanall depend msgheaders smheaders

# disabling all implicit rules
.SUFFIXES :
.PRECIOUS : %_m.h %_m.cc

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
	$(Q)-rm -f $(TARGET_FILES)
	$(Q)-rm -f $(call opp_rwildcard, . , *_m.cc *_m.h *_sm.cc *_sm.h)

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
