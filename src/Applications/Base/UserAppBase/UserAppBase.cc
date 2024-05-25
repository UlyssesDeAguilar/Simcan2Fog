#include "UserAppBase.h"
using namespace hypervisor;

UserAppBase::~UserAppBase()
{
    connections.clear();
}

void UserAppBase::initialize()
{
    // Init the super-class
    cSIMCAN_Core::initialize();

    // Init the state
    state = State::RUN;
    pc = 0;

    // Init module parameters
    startDelay = par("startDelay");
    connectionDelay = par("connectionDelay");
    isDistributedApp = par("isDistributedApp");
    myRank = (unsigned int)par("myRank");
    testID = par("testID").stdstringValue();
    appInstance = par("appInstance").stdstringValue();
    vmInstance = par("vmInstance").stdstringValue();
    userInstance = par("userInstance").stdstringValue();
    debugUserAppBase = par("debugUserAppBase");

    // Get the pid from "wrapper module"
    cModule *appModule = getParentModule();
    pid = appModule->par("pid");

    // Get the vmId from the AppArray
    cModule *appVector = appModule->getParentModule();
    vmId = appVector->par("vmId");

    // Init cGates
    inGate = gate("in");
    outGate = gate("out");

    // Schedule start
    scheduleExecStart();

    // Check connections
    if (!outGate->getNextGate()->isConnected())
        error("outGate is not connected");
}

void UserAppBase::scheduleExecStart()
{
    // Schedule waiting time to execute
    EV_DEBUG << "App module: " << getClassName() << " will start executing in " << startDelay << " seconds from now\n";
    event = new cMessage("EXECUTION START", Event::EXEC_START);
    scheduleAt(simTime() + startDelay, event);
}

void UserAppBase::finish()
{
    // Delete the event message
    cancelAndDelete(event);

    // Finish the super-class
    cSIMCAN_Core::finish();
}

cGate *UserAppBase::getOutGate(cMessage *msg)
{
    // If msg arrives from the Operating System
    if (msg->getArrivalGate() == inGate)
        return outGate;

    // If gate not found!
    return nullptr;
}

void UserAppBase::processSelfMessage(cMessage *msg)
{
    // Allow timers from children classes?
    // Start executing
    run();
}

void UserAppBase::sendRequestMessage(SIMCAN_Message *sm, cGate *outGate)
{
    // Record the starting time and change state
    state = State::WAIT;
    operationStart = simTime();

    // Send
    cSIMCAN_Core::sendRequestMessage(sm, outGate);
}

void UserAppBase::processResponseMessage(SIMCAN_Message *msg)
{
    auto syscall = check_and_cast<SM_Syscall *>(msg);

    // Sanity check
    if (state == State::RUN)
        error("Recieving response message while in RUN state");

    auto timeElapsed = simTime() - operationStart;

    // Call the corresponding callback
    switch (syscall->getContext().opCode)
    {
    case EXEC:
        callback->returnExec(timeElapsed, syscall->getContext().data.cpuRequest);
        break;
    case READ:
        callback->returnRead(timeElapsed);
        break;
    case WRITE:
        callback->returnWrite(timeElapsed);
        break;
    default:
        break;
    }

    // Delete the system call
    delete syscall;

    // Increment the program counter, set RUN state and call run() for next step
    pc++;
    state = State::RUN;
    run();
}

// ----------------- CPU calls ----------------- //

void UserAppBase::execute(double MIs)
{
    // Prepare the system call
    SM_Syscall *syscall = new SM_Syscall();
    SM_CPU_Message *sm_cpu = new SM_CPU_Message();

    // Set PID and Context
    syscall->setPid(pid);
    syscall->setVmId(vmId);
    syscall->setContext({.opCode = EXEC, .data.cpuRequest = sm_cpu});

    // Prepare the cpu request
    sm_cpu->setOperation(SM_ExecCpu);
    sm_cpu->setAppInstance(appInstance.c_str());
    sm_cpu->setUseMis(true);
    sm_cpu->setMisToExecute(MIs);
    sm_cpu->updateLength();

    // Debug (Trace)
    if (debugUserAppBase)
        EV_TRACE << "(SIMCAN_request_cpu): Message ready to perform a CPU request." << '\n'
                 << sm_cpu->contentsToString(showMessageContents, showMessageTrace) << '\n';

    // Send the request to the Operating System
    sendRequestMessage(syscall, outGate);
}

void UserAppBase::execute(simtime_t cpuTime)
{
    // Prepare the system call
    SM_Syscall *syscall = new SM_Syscall();
    SM_CPU_Message *sm_cpu = new SM_CPU_Message();

    // Set PID and Context
    syscall->setPid(pid);
    syscall->setVmId(vmId);
    syscall->setContext({.opCode = EXEC, .data.cpuRequest = sm_cpu});

    // Prepare the cpu request
    sm_cpu->setOperation(SM_ExecCpu);
    sm_cpu->setAppInstance(appInstance.c_str());
    sm_cpu->setUseTime(true);
    sm_cpu->setCpuTime(cpuTime);
    sm_cpu->updateLength();

    // Debug (Trace)
    if (debugUserAppBase)
        EV_TRACE << "(SIMCAN_request_cpuTime): Message ready to perform a CPU request." << '\n'
                 << sm_cpu->contentsToString(showMessageContents, showMessageTrace) << '\n';

    // Send the request to the Operating System
    sendRequestMessage(syscall, outGate);
}

void UserAppBase::read(double bytes) 
{
    // Prepare the system call
    SM_Syscall *syscall = new SM_Syscall();

    // Set PID and Context
    syscall->setPid(pid);
    syscall->setVmId(vmId);
    syscall->setContext({.opCode = READ, .data.bufferSize = bytes});

    EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]" << " sending read call: " << bytes << "B" << "\n";

    // Send the request to the Operating System
    sendRequestMessage(syscall, outGate);
}

void UserAppBase::write(double bytes) 
{
    // Prepare the system call
    SM_Syscall *syscall = new SM_Syscall();

    // Set PID and Context
    syscall->setPid(pid);
    syscall->setVmId(vmId);
    syscall->setContext({.opCode = WRITE, .data.bufferSize = bytes});

    EV_TRACE << "App " << "[" << vmId << "]" << "[" << pid << "]" << " sending write call: " << bytes << "B" << "\n";

    // Send the request to the Operating System
    sendRequestMessage(syscall, outGate);
}

void UserAppBase::abort()
{
    // Prepare the system call
    SM_Syscall *syscall = new SM_Syscall();
    SM_CPU_Message *sm_cpu = new SM_CPU_Message();

    // Set PID and Context
    syscall->setPid(pid);
    syscall->setVmId(vmId);
    syscall->setContext({.opCode = EXEC, .data.cpuRequest = sm_cpu});

    // Prepare the abort request
    sm_cpu->setOperation(SM_AbortCpu);
    sm_cpu->setAppInstance(appInstance.c_str());

    // sm_cpu->setNextModuleIndex(nextModuleIndex);

    // Debug (Trace)
    if (debugUserAppBase)
        EV_TRACE << "(SIMCAN_request_cpu): Message ready to abort a CPU request." << '\n'
                 << sm_cpu->contentsToString(showMessageContents, showMessageTrace) << '\n';

    // Send the request to the Operating System
    sendRequestMessage(syscall, outGate);
}
