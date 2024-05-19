#
# OMNeT++/OMNEST Makefile for $(LIB_PREFIX)Simcan2Cloud
#
# This file was generated with the command:
#  opp_makemake --make-lib -f --deep -O out -KINET4_PROJ=../../inet4 -DINET_IMPORT -I. -I$$\(INET4_PROJ\)/src -L$$\(INET4_PROJ\)/src -lmysqlcppconn -lINET$$\(D\)
#

# Name of target to be created (-o option)
TARGET = $(LIB_PREFIX)Simcan2Cloud$(D)$(A_LIB_SUFFIX)
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
    $O/Applications/Base/UserAppBase/UserAppBase.o \
    $O/Applications/Builder/ApplicationBuilder.o \
    $O/Applications/UserApps/DummyApp/DummyApplication.o \
    $O/Applications/UserApps/LocalApplication/LocalApplication.o \
    $O/Architecture/Cpu/CpuCores/CpuCore/CpuCore.o \
    $O/Architecture/Disk/DiskManager.o \
    $O/Architecture/Network/Adapter/Message/MessageAdapter.o \
    $O/Architecture/Network/Adapter/Message/Managers/BaseManager.o \
    $O/Architecture/Network/Adapter/Message/Managers/RequestManager.o \
    $O/Architecture/Network/Adapter/Message/Managers/ResponseManager.o \
    $O/Architecture/Network/Adapter/Multiplexer/RequestMultiplexer.o \
    $O/Architecture/Network/Adapter/Resolver/DNS_Resolver.o \
    $O/Architecture/Network/DNS/Service/DNS_Service.o \
    $O/Architecture/Network/DataCentreNetwork/CommNetwork/CommNetwork.o \
    $O/Architecture/Network/DataCentreNetwork/StorageNetwork/StorageNetwork.o \
    $O/Architecture/Network/MessageQueue/MessageQueueManager.o \
    $O/Architecture/Network/MessageQueue/Internals/SinkDelegate.o \
    $O/Architecture/Network/MessageQueue/Internals/SourceDelegate.o \
    $O/Architecture/Network/MessageQueue/Services/PullService.o \
    $O/Architecture/Network/MessageQueue/Services/PushService.o \
    $O/Architecture/Network/Stack/BaseProxy.o \
    $O/Architecture/Network/Stack/HttpProxyService.o \
    $O/Architecture/Network/Stack/StackMultiplexer.o \
    $O/Architecture/Network/Stack/TcpBaseClient.o \
    $O/Architecture/Network/Stack/TcpBaseProxy.o \
    $O/Architecture/Network/Stack/TcpBaseProxyThread.o \
    $O/Architecture/Network/Stack/Queue/QueuePullClient.o \
    $O/Architecture/Network/Stack/Queue/QueuePushClient.o \
    $O/Architecture/Network/Switch/Switch.o \
    $O/Architecture/Nodes/HardwareManagers/HardwareManager/HardwareManager.o \
    $O/Architecture/Nodes/Iot/IotController.o \
    $O/Architecture/Nodes/Iot/IotSensor.o \
    $O/Core/cSIMCAN_Core.o \
    $O/Core/CheckingHubModule/CheckingHubModule.o \
    $O/Core/DataManager/DataManager.o \
    $O/Core/SimSchema/MySQL/SimSchemaMySQL.o \
    $O/Core/include/internal_types.o \
    $O/Management/CloudManagerBase/CloudManagerBase.o \
    $O/Management/CloudProvider/CloudProvider.o \
    $O/Management/DataCentreManagers/DataCentreManagerBase/DataCentreManagerBase.o \
    $O/Management/DataCentreManagers/ResourceManager/ResourceManager.o \
    $O/Management/DataCentreManagers/ResourceManager/VmDeployment.o \
    $O/Management/DataCentreManagers/Selection/BestFit.o \
    $O/Management/DataCentreManagers/Selection/CostFit.o \
    $O/Management/DataCentreManagers/Selection/FirstFit.o \
    $O/Management/DataCentreManagers/Selection/SelectionStrategy.o \
    $O/Management/UserGenerators/UserGeneratorBase/UserGeneratorBase.o \
    $O/Management/UserGenerators/UserGeneratorCost/UserGeneratorCost.o \
    $O/Management/UserGenerators/UserGenerator_simple/UserGenerator_simple.o \
    $O/Management/dataClasses/Applications/AppInstance.o \
    $O/Management/dataClasses/Applications/AppInstanceCollection.o \
    $O/Management/dataClasses/Applications/Application.o \
    $O/Management/dataClasses/Applications/AppParameter.o \
    $O/Management/dataClasses/SLAs/Sla.o \
    $O/Management/dataClasses/Users/CloudUser.o \
    $O/Management/dataClasses/Users/CloudUserInstance.o \
    $O/Management/dataClasses/Users/CloudUserInstancePriority.o \
    $O/Management/dataClasses/Users/CloudUserInstanceTrace.o \
    $O/Management/dataClasses/Users/CloudUserPriority.o \
    $O/Management/dataClasses/Users/User.o \
    $O/Management/dataClasses/Users/UserAppReference.o \
    $O/Management/dataClasses/Users/UserInstance.o \
    $O/Management/dataClasses/Users/UserVmReference.o \
    $O/Management/dataClasses/VirtualMachines/VirtualMachine.o \
    $O/Management/dataClasses/VirtualMachines/VmInstance.o \
    $O/Management/dataClasses/VirtualMachines/VmInstanceCollection.o \
    $O/Management/utils/LogUtils.o \
    $O/Messages/AppRequest.o \
    $O/Messages/DNS_Request.o \
    $O/Messages/INET_AppMessage.o \
    $O/Messages/SIMCAN_Message.o \
    $O/Messages/SM_CPU_Message.o \
    $O/Messages/SM_FogIO.o \
    $O/Messages/SM_UserAPP.o \
    $O/Messages/SM_UserVM.o \
    $O/Messages/VMRequest.o \
    $O/OperatingSystem/CpuSchedulers/CpuSchedulerRR/CpuSchedulerRR.o \
    $O/OperatingSystem/Hypervisors/DcHypervisor/DcHypervisor.o \
    $O/OperatingSystem/Hypervisors/EdgeHypervisor/EdgeHypervisor.o \
    $O/OperatingSystem/Hypervisors/Hypervisor/Hypervisor.o \
    $O/OperatingSystem/Hypervisors/OsCore/OsCore.o \
    $O/UserModel/Base/BaseUserModel.o \
    $O/UserModel/Cloud/CloudUserModel.o \
    $O/UserModel/Edge/EdgeUserModelDriver.o \
    $O/Architecture/Network/RoutingInfo/RoutingInfo_m.o \
    $O/Architecture/Network/Stack/NetworkIOEvent_m.o \
    $O/Management/CloudProvider/NodeEvent_m.o \
    $O/Messages/DNS_Request_m.o \
    $O/Messages/INET_AppMessage_m.o \
    $O/Messages/SIMCAN_Message_m.o \
    $O/Messages/SM_CloudProvider_Control_m.o \
    $O/Messages/SM_CPU_Message_m.o \
    $O/Messages/SM_FogIO_m.o \
    $O/Messages/SM_ResolverRequest_m.o \
    $O/Messages/SM_REST_API_m.o \
    $O/Messages/SM_Syscall_m.o \
    $O/Messages/SM_UserAPP_m.o \
    $O/Messages/SM_UserVM_m.o \
    $O/Messages/SM_UserVM_Cost_m.o \
    $O/Messages/SM_VmExtend_m.o \
    $O/OperatingSystem/AppIdLabel/AppIdLabel_m.o

# Message files
MSGFILES = \
    Architecture/Network/RoutingInfo/RoutingInfo.msg \
    Architecture/Network/Stack/NetworkIOEvent.msg \
    Management/CloudProvider/NodeEvent.msg \
    Messages/DNS_Request.msg \
    Messages/INET_AppMessage.msg \
    Messages/SIMCAN_Message.msg \
    Messages/SM_CloudProvider_Control.msg \
    Messages/SM_CPU_Message.msg \
    Messages/SM_FogIO.msg \
    Messages/SM_ResolverRequest.msg \
    Messages/SM_REST_API.msg \
    Messages/SM_Syscall.msg \
    Messages/SM_UserAPP.msg \
    Messages/SM_UserVM.msg \
    Messages/SM_UserVM_Cost.msg \
    Messages/SM_VmExtend.msg \
    OperatingSystem/AppIdLabel/AppIdLabel.msg

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
