#include "LocalIotApplication.h"

#include "inet/common/TimeTag_m.h"

Define_Module(LocalIotApplication);

using namespace hypervisor;
using namespace iot;

void LocalIotApplication::initialize()
{
    // Init the super-class
    AppBase::initialize();

    processingMIs = par("processingMIs");
    listeningPort = par("listeningPort");
    serviceUp = false;
    setReturnCallback(this);

    cModule *edgeTile = getModuleByPath(par("parentPath"));
    // endpointName = edgeTile->par("serviceName");
    cPatternMatcher matcher(par("actuatorPattern"), true, true, true);
    EV_INFO << edgeTile->getFullName() << "\n";

    for (cModule::SubmoduleIterator it(edgeTile); !it.end(); ++it)
    {
        cModule *submodule = *it;
        if (matcher.matches(submodule->getFullName()))
        {
            actuators.push_back(L3AddressResolver().addressOf(submodule));
            EV_INFO << "Detected actuator " << submodule->getFullName() << " with address: " << actuators.back();
        }
    }

    // Record times
    simStartTime = simTime();
    runStartTime = time(nullptr);
}

void LocalIotApplication::processSelfMessage(cMessage *msg)
{
    // Open connection on listening port for starting the simulation
    udpSocket = open(listeningPort, SOCK_DGRAM);
}

void LocalIotApplication::handleDataArrived(int sockFd, Packet *p)
{
    if (sockFd == udpSocket)
    {
        EV << "Recieved packet: " << p->getName() << "\n";
        numPings++;
        if (numPings > (int)par("pingsThreshold"))
        {
            execute(processingMIs);
            numPings = 0;
        }
        delete p;
    }
}

void LocalIotApplication::returnExec(simtime_t timeElapsed, SM_CPU_Message *sm)
{
    if (serviceUp)
    {
        // Send to endpoint
    }
    else
    {
        int randAcutator = intuniform(0, actuators.size() - 1);
        int on = intuniform(0, 1);

        EV << "Sending command to actuator\n";
        // Send command to actuator
        Packet *packet = new Packet("Controller command");
        const auto &payload = makeShared<IotPayload>();
        payload->setChunkLength(B(20));
        payload->addTag<CreationTimeTag>()->setCreationTime(simTime());
        payload->setTag("Controller command");
        payload->setValue(on);
        payload->setUnit("");
        packet->insertAtBack(payload);

        _send(udpSocket, packet, actuators[randAcutator].toIpv4().getInt(), listeningPort);
    }
}

void LocalIotApplication::returnRead(simtime_t timeElapsed) {}
void LocalIotApplication::returnWrite(simtime_t timeElapsed) {}

void LocalIotApplication::handleConnectReturn(int sockFd, bool connected) {}
bool LocalIotApplication::handlePeerClosed(int sockFd) { return true; }

void LocalIotApplication::finish()
{
    // Calculate the total runtime
    double runtime = difftime(time(nullptr), runStartTime);

    // Log results
    EV_INFO << "Execution results:" << '\n';
    EV_INFO << " + Total simulation time (simulated):" << (simTime().dbl() - simStartTime.dbl()) << " seconds " << '\n';
    EV_INFO << " + Total execution time (real):" << runtime << " seconds" << '\n';
    // EV_INFO << " + Time for CPU:" << total_service_CPU.dbl() << '\n';

    // Finish the super-class
    actuators.clear();
    AppBase::finish();
}
