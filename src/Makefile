#
# OMNeT++/OMNEST Makefile for $(LIB_PREFIX)Simcan2Fog
#
# This file was generated with the command:
#  opp_makemake --make-lib -f --deep -O out -KINET4_PROJ=../../inet4 -DINET_IMPORT -I. -I$$\(INET4_PROJ\)/src -L$$\(INET4_PROJ\)/src -lmysqlcppconn -lINET$$\(D\)
#

# Name of target to be created (-o option)
TARGET = $(LIB_PREFIX)Simcan2Fog$(D)$(A_LIB_SUFFIX)
TARGET_DIR = .

# C++ include paths (with -I)
INCLUDE_PATH = -I. -I$(INET4_PROJ)/src

# Additional object and library files to link with
EXTRA_OBJS =

# Output directory
PROJECT_OUTPUT_DIR = ../out
PROJECTRELATIVE_PATH = src
O = $(PROJECT_OUTPUT_DIR)/$(CONFIGNAME)/$(PROJECTRELATIVE_PATH)

# Object files for local .cc, .msg and .sm files
OBJS = \
    $O/s2f/apps/ApplicationBuilder.o \
    $O/s2f/apps/models/AppBase.o \
    $O/s2f/apps/models/DummyApp/DummyApplication.o \
    $O/s2f/apps/models/IotApplication/IotApplication.o \
    $O/s2f/apps/models/LocalApplication/LocalApplication.o \
    $O/s2f/apps/models/LocalIotApplication/LocalIotApplication.o \
    $O/s2f/apps/models/simservice/SimService.o \
    $O/s2f/architecture/cpu/CpuCore.o \
    $O/s2f/architecture/disk/DiskManager.o \
    $O/s2f/architecture/dns/DnsServiceSimplified.o \
    $O/s2f/architecture/dns/cache/DnsCache.o \
    $O/s2f/architecture/dns/resolver/DnsQuerierService.o \
    $O/s2f/architecture/dns/resolver/DnsResolverService.o \
    $O/s2f/architecture/dns/server/DnsService.o \
    $O/s2f/architecture/messagequeue/MessageQueueManager.o \
    $O/s2f/architecture/messagequeue/internals/SinkDelegate.o \
    $O/s2f/architecture/messagequeue/internals/SourceDelegate.o \
    $O/s2f/architecture/messagequeue/services/PullService.o \
    $O/s2f/architecture/messagequeue/services/PushService.o \
    $O/s2f/architecture/net/hub/CheckingHubModule.o \
    $O/s2f/architecture/net/stack/listener/UdpListener.o \
    $O/s2f/architecture/net/stack/queue/PullClient.o \
    $O/s2f/architecture/net/stack/queue/PushClient.o \
    $O/s2f/architecture/net/stack/resolver/DnsResolver.o \
    $O/s2f/architecture/net/switch/Switch.o \
    $O/s2f/architecture/nodes/hardwaremanagers/HardwareManager.o \
    $O/s2f/architecture/nodes/iot/sensors/IotSensorModel.o \
    $O/s2f/core/cSIMCAN_Core.o \
    $O/s2f/core/include/internal_types.o \
    $O/s2f/core/simdata/DataManager.o \
    $O/s2f/core/simdata/mysql/SimSchemaMySQL.o \
    $O/s2f/management/cloudprovider/CloudProvider.o \
    $O/s2f/management/dataClasses/Applications/AppInstance.o \
    $O/s2f/management/dataClasses/Applications/AppInstanceCollection.o \
    $O/s2f/management/dataClasses/Applications/Application.o \
    $O/s2f/management/dataClasses/Applications/AppParameter.o \
    $O/s2f/management/dataClasses/SLAs/Sla.o \
    $O/s2f/management/dataClasses/Users/CloudUser.o \
    $O/s2f/management/dataClasses/Users/CloudUserInstance.o \
    $O/s2f/management/dataClasses/Users/CloudUserInstancePriority.o \
    $O/s2f/management/dataClasses/Users/CloudUserInstanceTrace.o \
    $O/s2f/management/dataClasses/Users/CloudUserPriority.o \
    $O/s2f/management/dataClasses/Users/User.o \
    $O/s2f/management/dataClasses/Users/UserAppReference.o \
    $O/s2f/management/dataClasses/Users/UserInstance.o \
    $O/s2f/management/dataClasses/Users/UserVmReference.o \
    $O/s2f/management/dataClasses/VirtualMachines/VirtualMachine.o \
    $O/s2f/management/dataClasses/VirtualMachines/VmInstance.o \
    $O/s2f/management/managers/DcManager.o \
    $O/s2f/management/managers/ManagerBase.o \
    $O/s2f/management/managers/ResourceManager.o \
    $O/s2f/management/managers/VmDeployment.o \
    $O/s2f/management/managers/selection/BestFit.o \
    $O/s2f/management/managers/selection/CostFit.o \
    $O/s2f/management/managers/selection/FirstFit.o \
    $O/s2f/management/managers/selection/SelectionStrategy.o \
    $O/s2f/management/utils/LogUtils.o \
    $O/s2f/messages/AppRequest.o \
    $O/s2f/messages/INET_AppMessage.o \
    $O/s2f/messages/SIMCAN_Message.o \
    $O/s2f/messages/SM_CPU_Message.o \
    $O/s2f/messages/SM_FogIO.o \
    $O/s2f/messages/SM_UserAPP.o \
    $O/s2f/messages/SM_UserVM.o \
    $O/s2f/messages/VMRequest.o \
    $O/s2f/os/hypervisors/DcHypervisor.o \
    $O/s2f/os/hypervisors/EdgeHypervisor.o \
    $O/s2f/os/hypervisors/Hypervisor.o \
    $O/s2f/os/hypervisors/OsCore.o \
    $O/s2f/os/schedulers/CpuSchedulerRR.o \
    $O/s2f/users/base/BaseUserModel.o \
    $O/s2f/users/cloud/CloudUserModel.o \
    $O/s2f/users/edge/EdgeUserModelDriver.o \
    $O/s2f/users/generators/UserGenerator_simple.o \
    $O/s2f/users/generators/UserGeneratorBase.o \
    $O/s2f/users/generators/UserGeneratorCost.o \
    $O/s2f/apps/models/simservice/SimServiceReq_m.o \
    $O/s2f/architecture/dns/DnsRequest_m.o \
    $O/s2f/architecture/net/routing/RoutingInfo_m.o \
    $O/s2f/architecture/net/stack/resolver/RequestTimeout_m.o \
    $O/s2f/architecture/nodes/iot/IotPayload_m.o \
    $O/s2f/core/PayloadMessage_m.o \
    $O/s2f/management/cloudprovider/NodeEvent_m.o \
    $O/s2f/messages/INET_AppMessage_m.o \
    $O/s2f/messages/SIMCAN_Message_m.o \
    $O/s2f/messages/SM_CloudProvider_Control_m.o \
    $O/s2f/messages/SM_CPU_Message_m.o \
    $O/s2f/messages/SM_FogIO_m.o \
    $O/s2f/messages/SM_ResolverRequest_m.o \
    $O/s2f/messages/SM_REST_API_m.o \
    $O/s2f/messages/SM_UserAPP_m.o \
    $O/s2f/messages/SM_UserVM_m.o \
    $O/s2f/messages/SM_UserVM_Cost_m.o \
    $O/s2f/messages/SM_VmExtend_m.o \
    $O/s2f/messages/Syscall_m.o

# Message files
MSGFILES = \
    s2f/apps/models/simservice/SimServiceReq.msg \
    s2f/architecture/dns/DnsRequest.msg \
    s2f/architecture/net/routing/RoutingInfo.msg \
    s2f/architecture/net/stack/resolver/RequestTimeout.msg \
    s2f/architecture/nodes/iot/IotPayload.msg \
    s2f/core/PayloadMessage.msg \
    s2f/management/cloudprovider/NodeEvent.msg \
    s2f/messages/INET_AppMessage.msg \
    s2f/messages/SIMCAN_Message.msg \
    s2f/messages/SM_CloudProvider_Control.msg \
    s2f/messages/SM_CPU_Message.msg \
    s2f/messages/SM_FogIO.msg \
    s2f/messages/SM_ResolverRequest.msg \
    s2f/messages/SM_REST_API.msg \
    s2f/messages/SM_UserAPP.msg \
    s2f/messages/SM_UserVM.msg \
    s2f/messages/SM_UserVM_Cost.msg \
    s2f/messages/SM_VmExtend.msg \
    s2f/messages/Syscall.msg

# SM files
SMFILES =

# Other makefile variables (-K)
INET4_PROJ=../../inet4

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

COPTS = $(CFLAGS) $(IMPORT_DEFINES) -DINET_IMPORT $(INCLUDE_PATH) -I$(OMNETPP_INCL_DIR)
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
# inserted from file 'makefrag':
MSGC:=$(MSGC) --msg6

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
	@echo Creating static library: $@
	$(Q)$(AR) $O/$(TARGET) $(OBJS) $(EXTRA_OBJS)

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
